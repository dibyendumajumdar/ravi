#include "ravi_api.h"

#define LUA_CORE

#include "ravi_mirjit.h"

#include "lua.h"
#include "lapi.h"
#include "lauxlib.h"
#include "lfunc.h"
#include "lmem.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"

#include <string.h>

struct CompilerContext {
  lua_State* L;
  ravi_State* jit;
  Table* h; /* to avoid collection/reuse strings */
};

static void debug_message(void* context, const char* filename, long long line, const char* message) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  ravi_writestring(ccontext->L, filename, strlen(filename));
  char temp[80];
  snprintf(temp, sizeof temp, "%lld: ", line);
  ravi_writestring(ccontext->L, temp, strlen(temp));
  ravi_writestring(ccontext->L, message, strlen(message));
  ravi_writeline(ccontext->L);
}

void error_message(void* context, const char* message) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  ravi_writestring(ccontext->L, message, strlen(message));
  ravi_writeline(ccontext->L);
}

/* Create a new proto and insert it into parent's list of protos */
static Proto* lua_newProto(void* context, Proto* parent) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  lua_State* L = ccontext->L;
  Proto* p = luaF_newproto(L);
  assert(parent);
  /* FIXME make this more efficient */
  int old_size = parent->sizep;
  int new_size = parent->sizep + 1;
  luaM_reallocvector(L, parent->p, old_size, new_size, Proto*);
  parent->p[old_size] = p;
  parent->sizep++;
  lua_assert(parent->sizep == new_size);
  luaC_objbarrier(L, parent, p);
  return p;
}

/*
 * Based off the Lua lexer code. Strings are anchored in a table initially - eventually
 * ending up either in a Proto constant table or some other structure related to
 * protos.
 */
TString* intern_string(lua_State* L, Table* h, const char* str, size_t l) {
  TValue* o;                             /* entry for 'str' */
  TString* ts = luaS_newlstr(L, str, l); /* create new string */
  setsvalue2s(L, L->top++, ts);          /* temporarily anchor it in stack */
  o = luaH_set(L, h, L->top - 1);
  if (ttisnil(o)) {  /* not in use yet? */
                     /* boolean value does not need GC barrier;
                            table has no metatable, so it does not need to invalidate cache */
    setbvalue(o, 1); /* t[string] = true */
    luaC_checkGC(L);
  }
  else {                         /* string already present */
    ts = tsvalue(keyfromval(o)); /* re-use value previously stored */
  }
  L->top--; /* remove string from stack */
  return ts;
}

/*
** Add constant 'v' to prototype's list of constants (field 'k').
** Use parser's table to cache position of constants in constant list
** and try to reuse constants.
*/
static int add_konstant(lua_State* L, Proto* f, Table* h, TValue* key, TValue* v) {
  TValue* idx = luaH_set(L, h, key); /* The k index is cached against the key */
  int k, oldsize, newsize;
  if (ttisinteger(idx)) { /* is there an integer value index there? */
    k = cast_int(ivalue(idx));
    /* correct value? (warning: must distinguish floats from integers!) */
    if (k >= 0 && k < f->sizek && ttype(&f->k[k]) == ttype(v) && luaV_rawequalobj(&f->k[k], v))
      return k; /* reuse index */
  }
  /* constant not found; create a new entry in Proto->k */
  oldsize = k = f->sizek;
  newsize = oldsize + 1;
  /* numerical value does not need GC barrier;
     table has no metatable, so it does not need to invalidate cache */
  setivalue(idx, k);
  // FIXME make the allocation more efficient
  luaM_reallocvector(L, f->k, oldsize, newsize, TValue);
  setobj(L, &f->k[k], v); /* record the position k in the table against key */
  f->sizek++;
  lua_assert(f->sizek == newsize);
  luaC_barrier(L, f, v);
  return k;
}

/*
** Add a string to list of constants and return its index.
*/
static int add_string_konstant(lua_State* L, Proto* p, Table* h, TString* s) {
  TValue o;
  setsvalue(L, &o, s);
  return add_konstant(L, p, h, &o, &o); /* use string itself as key */
}

/* Create a Lua TString object from a string. Save it so that we can avoid creating same
 * string again.
 */
static inline TString* create_luaString(lua_State* L, Table* h, struct string_object* s) {
  if (s->userdata == NULL) {
    /* Create and save it */
    s->userdata = intern_string(L, h, s->str, s->len);
  }
  return (TString*)s->userdata;
}

/* Add a string constant to Proto and return its index */
static int lua_newStringConstant(void* context, Proto* proto, struct string_object* s) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  lua_State* L = ccontext->L;
  Table* h = ccontext->h;
  TString* ts = create_luaString(L, h, s);
  return add_string_konstant(L, proto, h, ts);
}

/* Add an upvalue. If the upvalue refers to a local variable in parent proto then idx should contain
 * the register for the local variable and instack should be true, else idx should have the index of
 * upvalue in parent proto and instack should be false.
 */
static int lua_addUpValue(void* context, Proto* f, struct string_object* name, unsigned idx, int instack, unsigned tc,
                          struct string_object* usertype) {
  ravitype_t typecode = (ravitype_t)tc;
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  lua_State* L = ccontext->L;
  Table* h = ccontext->h;
  int oldsize = f->sizeupvalues;
  int newsize = oldsize + 1;
  int pos = oldsize;
  // checklimit(fs, fs->nups + 1, MAXUPVAL, "upvalues");
  // FIXME optimize the allocation
  luaM_reallocvector(L, f->upvalues, oldsize, newsize, Upvaldesc);
  f->sizeupvalues++;
  lua_assert(f->sizeupvalues == newsize);
  f->upvalues[pos].instack = cast_byte(instack); /* is the upvalue in parent function's local stack ? */
  f->upvalues[pos].idx = cast_byte(idx); /* If instack then parent's local register else parent's upvalue index */
  TString* tsname = create_luaString(L, h, name); /* name of the variable */
  f->upvalues[pos].name = tsname;
  f->upvalues[pos].ravi_type = typecode;
  if (usertype != NULL) {
    /* User type string goes into the proto's constant table */
    int kpos = lua_newStringConstant(context, f, usertype);
    f->upvalues[pos].usertype = tsvalue(&f->k[kpos]);
  }
  else {
    f->upvalues[pos].usertype = NULL;
  }
  luaC_objbarrier(L, f, tsname);
  return pos;
}

static void init_C_compiler(void* context) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
#ifdef USE_MIRJIT
  mir_prepare(ccontext->jit->jit, 2);
#endif
}
static void* compile_C(void* context, const char* C_src, unsigned len) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  fprintf(stdout, "%s\n", C_src);
#ifdef USE_MIRJIT
  return mir_compile_C_module(&ccontext->jit->options, ccontext->jit->jit, C_src, "input");
#else
  return NULL;
#endif
}
static void finish_C_compiler(void* context) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
#ifdef USE_MIRJIT
  mir_cleanup(ccontext->jit->jit);
#endif
}
static lua_CFunction get_compiled_function(void* context, void* module, const char* name) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
#if USE_MIRJIT
  MIR_module_t M = (MIR_module_t)module;
  return (lua_CFunction)mir_get_func(ccontext->jit->jit, M, name);
#else
  return NULL;
#endif  
}
static void lua_setProtoFunction(void* context, Proto* p, lua_CFunction func) {
  p->ravi_jit.jit_function = func;
  p->ravi_jit.jit_status = RAVI_JIT_COMPILED;
}
static void lua_setVarArg(void* context, Proto* p) { p->is_vararg = 1; }
static void lua_setNumParams(void* context, Proto* p, unsigned num_params) { p->numparams = cast_byte(num_params); }
static void lua_setMaxStackSize(void *context, Proto *p, unsigned max_stack_size) {
  p->maxstacksize = cast_byte(max_stack_size);
}

static int load_and_compile(lua_State* L) {
  const char* s = luaL_checkstring(L, 1);
  struct CompilerContext ccontext = {.L = L, .jit = G(L)->ravi_state};

  LClosure* cl = luaF_newLclosure(L, 1); /* create main closure with 1 up-value for _ENV */
  setclLvalue(L, L->top, cl);            /* anchor it (to avoid being collected) */
  luaD_inctop(L);
  ccontext.h = luaH_new(L);         /* create table for string constants */
  sethvalue(L, L->top, ccontext.h); /* anchor it */
  luaD_inctop(L);
  Proto* main_proto = cl->p = luaF_newproto(L);
  luaC_objbarrier(L, cl, cl->p);

  struct Ravi_CompilerInterface ravicomp_interface = {.source = s,
                                                      .source_len = strlen(s),
                                                      .source_name = "input",
                                                      .main_proto = main_proto,
                                                      .context = &ccontext,
                                                      .lua_newProto = lua_newProto,
                                                      .lua_newStringConstant = lua_newStringConstant,
                                                      .lua_addUpValue = lua_addUpValue,
                                                      .lua_setVarArg = lua_setVarArg,
                                                      .lua_setProtoFunction = lua_setProtoFunction,
                                                      .lua_setNumParams = lua_setNumParams,
                                                      .lua_setMaxStackSize = lua_setMaxStackSize,
                                                      .init_C_compiler = init_C_compiler,
                                                      .compile_C = compile_C,
                                                      .finish_C_compiler = finish_C_compiler,
                                                      .get_compiled_function = get_compiled_function,
                                                      .debug_message = debug_message,
                                                      .error_message = error_message};

  int rc = raviX_compile(&ravicomp_interface);
  L->top--; /* remove table for string constants */
  if (rc == 0) {
    lua_assert(cl->nupvalues == cl->p->sizeupvalues);
    luaF_initupvals(L, cl);
    return 1;
  }
  else {
    lua_error(L);
    return 0;
  }
}

static const luaL_Reg ravilib[] = {{"load", load_and_compile}, {NULL, NULL}};

int(raviopen_compiler)(lua_State* L) {
  luaL_newlib(L, ravilib);
  return 1;
}
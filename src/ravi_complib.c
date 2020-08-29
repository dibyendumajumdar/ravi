#include "ravi_api.h"

#define LUA_CORE

#include "lua.h"
#include "lapi.h"
#include "lauxlib.h"
#include "lfunc.h"
#include "lmem.h"
#include "lstring.h"
#include "ltable.h"

struct CompilerContext {
	lua_State* L;
	Table* h;  /* to avoid collection/reuse strings */
};

static Proto* lua_newProto(void* context, Proto* parent) {
	struct CompilerContext* ccontext = (struct CompilerContext*)context;
	lua_State* L = ccontext->L;
	Proto* p = luaF_newproto(L);
	if (parent) {
		int old_size = parent->sizep;
		int new_size = parent->sizep + 1;
		luaM_growvector(L, parent->p, old_size, new_size, Proto*, MAXARG_Bx, "functions");
		parent->p[parent->sizep++] = p;
		// luaC_objbarrier(L, f, clp);
	}
	return p;
}

/*
* Based off the Lua lexer code.
*/
TString* create_newstring(lua_State* L, Table *h, const char* str, size_t l) {
	TValue* o;  /* entry for 'str' */
	TString* ts = luaS_newlstr(L, str, l);  /* create new string */
	setsvalue2s(L, L->top++, ts);  /* temporarily anchor it in stack */
	o = luaH_set(L, h, L->top - 1);
	if (ttisnil(o)) {  /* not in use yet? */
	  /* boolean value does not need GC barrier;
		 table has no metatable, so it does not need to invalidate cache */
		setbvalue(o, 1);  /* t[string] = true */
		luaC_checkGC(L);
	}
	else {  /* string already present */
		ts = tsvalue(keyfromval(o));  /* re-use value previously stored */
	}
	L->top--;  /* remove string from stack */
	return ts;
}

/*
** Add constant 'v' to prototype's list of constants (field 'k').
** Use scanner's table to cache position of constants in constant list
** and try to reuse constants. Because some values should not be used
** as keys (nil cannot be a key, integer keys can collapse with float
** keys), the caller must provide a useful 'key' for indexing the cache.
*/
static int addk(lua_State *L, Proto* f, Table* h, TValue* key, TValue* v) {
	TValue* idx = luaH_set(L, h, key);  /* index scanner table */
	int k, oldsize;
	if (ttisinteger(idx)) {  /* is there an index there? */
		k = cast_int(ivalue(idx));
		/* correct value? (warning: must distinguish floats from integers!) */
		if (k < f->sizek && ttype(&f->k[k]) == ttype(v) &&
			luaV_rawequalobj(&f->k[k], v))
			return k;  /* reuse index */
	}
	/* constant not found; create a new entry */
	oldsize = f->sizek;
	k = f->sizek;
	/* numerical value does not need GC barrier;
	   table has no metatable, so it does not need to invalidate cache */
	setivalue(idx, k);
	luaM_growvector(L, f->k, k, f->sizek, TValue, MAXARG_Ax, "constants");
	while (oldsize < f->sizek) setnilvalue(&f->k[oldsize++]);
	setobj(L, &f->k[k], v);
	f->sizek++;
	luaC_barrier(L, f, v);
	return k;
}


/*
** Add a string to list of constants and return its index.
*/
static int lua_stringK(lua_State *L, Proto* p, Table *h, TString* s) {
	TValue o;
	setsvalue(L, &o, s);
	return addk(L, p, h, &o, &o);  /* use string itself as key */
}

/* Add a string constant to Proto and return its index */
static int lua_newStringConstant(void *context, Proto* proto, struct string_object *s) {
	struct CompilerContext* ccontext = (struct CompilerContext*)context;
	lua_State* L = ccontext->L;
	Table* h = ccontext->h;
	TString* ts = NULL;
	if (s->userdata == NULL) {
		ts = create_newstring(L, h, s->str, s->len);
		s->userdata = ts;
	}
	else {
		ts = (TString *) s->userdata;
	}
	return lua_stringK(L, proto, h, ts);
}

static void init_C_compiler(void* context) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  mir_prepare(ccontext->jit->jit, 2);
}
static void* compile_C(void* context, const char* C_src, unsigned len) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;

  return NULL;
}
static void finish_C_compiler(void* context) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  mir_cleanup(ccontext->jit->jit);
}
static lua_CFunction get_compiled_function(void* context, void* module, const char* name) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  MIR_module_t M = (MIR_module_t)module;
  return (lua_CFunction)mir_get_func(ccontext->jit->jit, M, name);
}
static void lua_setProtoFunction(void* context, Proto* p, lua_CFunction func) { p->ravi_jit.jit_function = func; }

static int load_and_compile(lua_State* L) {
	const char* s = luaL_checkstring(L, 1);
	struct CompilerContext ccontext = { .L = L };

	LClosure* cl = luaF_newLclosure(L, 1);  /* create main closure */
	setclLvalue(L, L->top, cl);  /* anchor it (to avoid being collected) */
	luaD_inctop(L);
	ccontext.h = luaH_new(L);  /* create table for string constants */
	sethvalue(L, L->top, ccontext.h);  /* anchor it */
	luaD_inctop(L);
	Proto *main_proto = cl->p = luaF_newproto(L);
	luaC_objbarrier(L, cl, cl->p);

	struct Ravi_CompilerInterface ravicomp_interface = {
		.source = s,
		.source_len = strlen(s),
		.source_name = "input",
		.lua_newProto = lua_newProto,
		.lua_newStringConstant = lua_newStringConstant,
		.main_proto = main_proto,
		.context = &ccontext
	};

	int rc = raviX_compile(&ravicomp_interface);
	L->top--;  /* remove table */

	lua_assert(cl->nupvalues == cl->p->sizeupvalues);
	luaF_initupvals(L, cl);


}

static const luaL_Reg ravilib[] = { 
                                   {NULL, NULL} };

int (raviopen_compiler)(lua_State *L) {
	luaL_newlib(L, ravilib);
	return 1;
}
/******************************************************************************
 * Copyright (C) 2019-2021 Dibyendu Majumdar
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#include "ravi_mirjit.h"
#include "ravi_jit.h"
#include <stddef.h>
#include <assert.h>

#define LUA_CORE

#include "lauxlib.h"
#include "lobject.h"
#include "lstate.h"
#include "lua.h"


#if !RAVI_64
#error MIRJIT is currently only supported on 64-bit architecture
#endif

typedef struct LuaFunc {
  const char *name;
  void *ptr;
} LuaFunc;

static LuaFunc Lua_functions[] = {
    { "luaF_close", luaF_close },
    { "raviV_raise_error", raviV_raise_error },
    { "raviV_raise_error_with_info", raviV_raise_error_with_info },
    { "luaV_tonumber_", luaV_tonumber_ },
    { "luaV_tointeger", luaV_tointeger },
    { "luaV_tointegerns", luaV_tointegerns },
    { "luaV_shiftl", luaV_shiftl },
    { "luaD_poscall", luaD_poscall },
    { "luaV_equalobj", luaV_equalobj },
    { "luaV_lessthan", luaV_lessthan },
    { "luaV_lessequal", luaV_lessequal },
    { "luaV_execute", luaV_execute },
    { "luaV_gettable", luaV_gettable },
    { "luaV_settable", luaV_settable },
    { "luaD_precall", luaD_precall },
    { "raviV_op_newtable", raviV_op_newtable },
    { "luaO_arith", luaO_arith },
    { "raviV_op_newarrayint", raviV_op_newarrayint },
    { "raviV_op_newarrayfloat", raviV_op_newarrayfloat },
    { "raviV_op_setlist", raviV_op_setlist },
    { "raviV_op_concat", raviV_op_concat },
    { "raviV_op_closure", raviV_op_closure },
    { "raviV_op_vararg", raviV_op_vararg },
    { "luaV_objlen", luaV_objlen },
    { "luaV_forlimit", luaV_forlimit },
    { "raviV_op_setupval", raviV_op_setupval },
    { "raviV_op_setupvali", raviV_op_setupvali },
    { "raviV_op_setupvalf", raviV_op_setupvalf },
    { "raviV_op_setupvalai", raviV_op_setupvalai },
    { "raviV_op_setupvalaf", raviV_op_setupvalaf },
    { "raviV_op_setupvalt", raviV_op_setupvalt },
    { "luaD_call", luaD_call },
    { "raviH_set_int", raviH_set_int },
    { "raviH_set_float", raviH_set_float },
    { "raviV_check_usertype", raviV_check_usertype },
    { "luaT_trybinTM", luaT_trybinTM },
    { "raviV_gettable_sskey", raviV_gettable_sskey },
    { "raviV_settable_sskey", raviV_settable_sskey },
    { "raviV_gettable_i", raviV_gettable_i },
    { "raviV_settable_i", raviV_settable_i },
#ifdef RAVI_DEFER_STATEMENT
    { "raviV_op_defer", raviV_op_defer },
#endif
    { "raviV_op_bnot", raviV_op_bnot},

    { "lua_absindex", lua_absindex },
    { "lua_gettop", lua_gettop },
    { "lua_pushvalue", lua_pushvalue },

    { "lua_isnumber", lua_isnumber },
    { "lua_isstring", lua_isstring },
    { "lua_iscfunction", lua_iscfunction },
    { "lua_isinteger", lua_isinteger },
    { "lua_isuserdata", lua_isuserdata },
    { "lua_type", lua_type },
    { "lua_typename", (void*)lua_typename },
    { "ravi_typename", (void*)ravi_typename },

    { "lua_tonumberx", lua_tonumberx },
    { "lua_tointegerx", lua_tointegerx },
    { "lua_toboolean", lua_toboolean },
    { "lua_tolstring", (void*)lua_tolstring },
    { "lua_rawlen", lua_rawlen },
    { "lua_tocfunction", lua_tocfunction },
    { "lua_touserdata", lua_touserdata },
    { "lua_tothread", lua_tothread },
    { "lua_topointer", (void *)lua_topointer },

    { "lua_arith", lua_arith },
    { "lua_rawequal", lua_rawequal },
    { "lua_compare", lua_compare },
    { "lua_pushnil", lua_pushnil },

    { "lua_pushnumber", lua_pushnumber },
    { "lua_pushinteger", lua_pushinteger },
    { "lua_pushlstring", (void*)lua_pushlstring },
    { "lua_pushstring", (void*)lua_pushstring },
    { "lua_pushcclosure", lua_pushcclosure },
    { "lua_pushboolean", lua_pushboolean },
    { "lua_pushlightuserdata", lua_pushlightuserdata },
    { "lua_pushthread", lua_pushthread },

    { "lua_getglobal", lua_getglobal },
    { "lua_gettable", lua_gettable },
    { "lua_getfield", lua_getfield },
    { "lua_geti", lua_geti },
    { "lua_rawget", lua_rawget },
    { "lua_rawgeti", lua_rawgeti },
    { "lua_rawgetp", lua_rawgetp },
    { "lua_createtable", lua_createtable },
    { "lua_newuserdata", lua_newuserdata },
    { "lua_getmetatable", lua_getmetatable },
    { "lua_getuservalue", lua_getuservalue },
    { "lua_setglobal", lua_setglobal },
    { "lua_settable", lua_settable },
    { "lua_setfield", lua_setfield },
    { "lua_seti", lua_seti },
    { "lua_rawset", lua_rawset },
    { "lua_rawseti", lua_rawseti },
    { "lua_rawsetp", lua_rawsetp },
    { "lua_setmetatable", lua_setmetatable },
    { "lua_setuservalue", lua_setuservalue },
    { "luaD_growstack", luaD_growstack },
    { "luaF_newLclosure", luaF_newLclosure },
    { "luaS_newlstr", luaS_newlstr },
    { "luaF_newproto", luaF_newproto },
    { "luaD_inctop", luaD_inctop },
    { "luaM_realloc_", luaM_realloc_ },
    { "luaV_concat", luaV_concat },
    { NULL, NULL }
};

// Initialize the JIT State and attach it to the
// Global Lua State
// If a JIT State already exists then this function
// will return -1
int raviV_initjit(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state != NULL) return -1;
  ravi_State *jit = (ravi_State *)calloc(1, sizeof(ravi_State));
  jit->auto_ = 0;
  jit->enabled_ = 1;
  jit->min_code_size_ = 150;
  jit->min_exec_count_ = 50;
  jit->opt_level_ = 2;
  // The parameter true means we will be dumping stuff as we compile
  jit->jit = MIR_init();
  G->ravi_state = jit;
  return 0;
}

// Free up the JIT State
void raviV_close(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL) return;
  // All compiled functions will be deleted at this stage
  if (G->ravi_state->jit) {
    MIR_finish(G->ravi_state->jit);
  }
  free(G->ravi_state);
}

// Dump the intermediate C code
void raviV_dumpIR(struct lua_State *L, struct Proto *p) {
	global_State *G = G(L);
	if (G->ravi_state == NULL) 
		return;

	membuff_t buf;
	membuff_init(&buf, 4096);

	char fname[30];
	snprintf(fname, sizeof fname, "%s", "jit_function");
	ravi_compile_options_t options;
        memset(&options, 0, sizeof options);
	options.codegen_type = RAVI_CODEGEN_ALL;
	if (raviJ_codegen(L, p, &options, fname, &buf)) {
		ravi_writestring(L, buf.buf, strlen(buf.buf));
		ravi_writeline(L);
	}
	membuff_free(&buf);
}

// Dump the LLVM ASM
void raviV_dumpASM(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}

void raviV_setminexeccount(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->min_exec_count_ = value;
}
int raviV_getminexeccount(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->min_exec_count_;
}

void raviV_setmincodesize(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->min_code_size_ = value;
}
int raviV_getmincodesize(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->min_code_size_;
}

void raviV_setauto(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->auto_ = value;
}
int raviV_getauto(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->auto_;
}

// Turn on/off the JIT compiler
void raviV_setjitenabled(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->enabled_ = value;
}
int raviV_getjitenabled(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->enabled_;
}

void raviV_setoptlevel(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->opt_level_ = value >= 0 && value <= 3 ? value : G->ravi_state->opt_level_;
}
int raviV_getoptlevel(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->opt_level_;
}

void raviV_setsizelevel(lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getsizelevel(lua_State *L) {
  (void)L;
  return 0;
}

void raviV_setvalidation(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->validation_ = value;
}
int raviV_getvalidation(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->validation_;
}

void raviV_setverbosity(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->verbosity_ = value;
}
int raviV_getverbosity(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->verbosity_;
}

// Turn on/off the JIT compiler
void raviV_settraceenabled(lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_gettraceenabled(lua_State *L) {
  (void)L;
  return 0;
}

int raviV_compile_n(struct lua_State *L, struct Proto *p[], int n,
                    ravi_compile_options_t *options) {
  int count = 0;
  for (int i = 0; i < n; i++) {
    if (raviV_compile(L, p[i], options)) count++;
  }
  return count > 0;
}

static void* import_resolver(const char *name) {
  for (int i = 0; Lua_functions[i].name != NULL; i++) {
    if (strcmp(name, Lua_functions[i].name) == 0)
      return Lua_functions[i].ptr;
  }
  return NULL;
}

struct ReadBuffer {
  size_t Current_char;     /* position of current character */
  const char *Source_code; /* points to buffer containing source Source_code */
};

static int t_getc (void *data) {
  struct ReadBuffer *buffer = data;
  int c = buffer->Source_code[buffer->Current_char];

  if (c == 0)
    c = EOF;
  else
    buffer->Current_char++;
  return c;
}

/* Searches within a Module for a function by name */ 
MIR_item_t mir_find_function(MIR_module_t module, const char *func_name) {
  MIR_item_t func, main_func = NULL;
  for (func = DLIST_HEAD (MIR_item_t, module->items); func != NULL;
       func = DLIST_NEXT (MIR_item_t, func)) {
    if (func->item_type == MIR_func_item && strcmp (func->u.func->name, func_name) == 0)
      main_func = func;
  }
  return main_func;
}

MIR_module_t mir_compile_C_module(
  struct c2mir_options *options, 
  MIR_context_t ctx, 
  const char *inputbuffer, /* Code to be compiled */
  const char *source_name /* Name of the function, must be unique */
  )
{
  int ret_code = 0;
  char module_name[30];
  struct ReadBuffer read_buffer = {.Current_char = 0, .Source_code = inputbuffer};
  MIR_module_t module = NULL;
  options->module_num++;
  snprintf(module_name, sizeof module_name, "__mod_%lld__", options->module_num);
  options->message_file = stderr;
  if (!c2mir_compile(ctx, options, t_getc, &read_buffer, module_name, NULL)) {
    ret_code = 1;
  }
  else {
    /* The module just compiled will be at the end of the list */
    module = DLIST_TAIL (MIR_module_t, *MIR_get_module_list (ctx));
  }
  if (ret_code == 0 && !module) { 
    ret_code = 1;
  }
  if (ret_code == 0 && module) {
    MIR_load_module (ctx, module);
    MIR_link (ctx, MIR_set_gen_interface, import_resolver);
  }
  return ret_code == 0 && module ? module : NULL;
}

void *mir_get_func(MIR_context_t ctx, MIR_module_t module, const char *func_name) {
  MIR_item_t main_func = mir_find_function(module, func_name);
  if (main_func == NULL) {
    fprintf(stderr, "Error: Compiled function %s not found\n", func_name);
    exit(1);
  }
  return MIR_gen (ctx, 0, main_func);
}

void mir_prepare(MIR_context_t ctx, int optlevel) {
  c2mir_init(ctx);
  MIR_gen_init (ctx, optlevel);
}

void mir_cleanup(MIR_context_t ctx) {
  MIR_gen_finish (ctx);
  c2mir_finish (ctx);
}

static void *compile_C_module(
    struct c2mir_options *options,
    MIR_context_t ctx,
    const char *inputbuffer, /* Code to be compiled */
    const char *func_name /* Name of the function, must be unique */
    )
{
  int (*fun_addr) (void *) = NULL;
  mir_prepare(ctx, 2);
  MIR_module_t module = mir_compile_C_module(options, ctx, inputbuffer, func_name);
  if (module) {
    fun_addr = mir_get_func(ctx, module, func_name);
  }
  mir_cleanup(ctx);
  return fun_addr;
}


// Compile a Lua function
// If JIT is turned off then compilation is skipped
// Compilation occurs if either auto compilation is ON (subject to some
// thresholds)
// or if a manual compilation request was made
// Returns true if compilation was successful
int raviV_compile(struct lua_State *L, struct Proto *p, ravi_compile_options_t *options) {
  if (p->ravi_jit.jit_status == RAVI_JIT_COMPILED)
    return true;
  else if (p->ravi_jit.jit_status == RAVI_JIT_CANT_COMPILE)
    return false;
  if (options == NULL) return false;

  global_State *G = G(L);
  if (G->ravi_state == NULL) return false;
  if (G->ravi_state->jit == NULL) return false;

  bool doCompile = (bool)(options && options->manual_request != 0);
  if (!doCompile && G->ravi_state->auto_) {
    if (p->ravi_jit.jit_flags == RAVI_JIT_FLAG_HASFORLOOP) /* function has fornum loop, so compile */
      doCompile = true;
    else if (p->sizecode > G->ravi_state->min_code_size_) /* function is long so compile */
      doCompile = true;
    else {
      if (p->ravi_jit.execution_count < G->ravi_state->min_exec_count_) /* function has been executed many
                                                                                      times so compile */
        p->ravi_jit.execution_count++;
      else
        doCompile = true;
    }
  }
  if (!doCompile) { return false; }
  if (!raviJ_cancompile(p)) {
    p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
    return false;
  }

  if (G->ravi_state->compiling_) return false;
  G->ravi_state->compiling_ = 1;

  membuff_t buf;
  membuff_init(&buf, 4096);

  int (*fp)(lua_State * L) = NULL;
  char fname[30];
  snprintf(fname, sizeof fname, "jit%lld", G->ravi_state->id++);
  if (!raviJ_codegen(L, p, options, fname, &buf)) {
    p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
    goto Lerror;
  }
  if (options->manual_request && G->ravi_state->verbosity_) {
    ravi_writestring(L, buf.buf, strlen(buf.buf));
    ravi_writeline(L);
  }
  fp = compile_C_module(&G->ravi_state->options, G->ravi_state->jit, buf.buf, fname);
  if (!fp) {
    p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
  }
  else {
    p->ravi_jit.jit_data = NULL;
    p->ravi_jit.jit_function = fp;
    p->ravi_jit.jit_status = RAVI_JIT_COMPILED;
  }
Lerror:
  membuff_free(&buf);
  G->ravi_state->compiling_ = 0;
  return fp != NULL;
}

// Free the JIT compiled function
// Note that this is called by the garbage collector
void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}

int ravi_compile_C(lua_State *L) {
  (void)L;
  return 0;
}

const char *raviV_jit_id(struct lua_State *L) {
  return "mir";
}

#define RAVI_OPTION_STRING3 "mirjit"
#define RAVI_OPTIONS RAVI_OPTION_STRING1 RAVI_OPTION_STRING2 RAVI_OPTION_STRING3

const char *raviV_options(struct lua_State *L) {
  return RAVI_OPTIONS;
}

/******************************************************************************
 * Copyright (C) 2015 Dibyendu Majumdar
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

#include <ravi_nanojit.h>
#include <ravijit.h>
#include <stddef.h>
#include <assert.h>

// FIXME should be in ravi_State
static int id = 0;

#define LUA_CORE

#include "lauxlib.h"
#include "lobject.h"
#include "lstate.h"
#include "lua.h"

static const char *errortext[] = {
    "integer expected",
    "number expected",
    "integer[] expected",
    "number[] expected",
    "table expected",
    "upvalue of integer type, cannot be set to non integer value",
    "upvalue of number type, cannot be set to non number value",
    "upvalue of integer[] type, cannot be set to non integer[] value",
    "upvalue of number[] type, cannot be set to non number[] value",
    "upvalue of table type, cannot be set to non table value",
    NULL};

static void raise_error(lua_State *L, int errorcode) {
  assert(errorcode >= 0 && errorcode <= Error_upval_needs_table);
  luaG_runerror(L, errortext[errorcode]);
}

static bool register_builtin_arg1(NJXContextRef module, const char *name,
                                  void *fp, enum NJXValueKind return_type,
                                  enum NJXValueKind arg1) {
  enum NJXValueKind args[1];
  args[0] = arg1;
  return NJX_register_C_function(module, name, fp, return_type, args, 1);
}
static bool register_builtin_arg2(NJXContextRef module, const char *name,
                                  void *fp, enum NJXValueKind return_type,
                                  enum NJXValueKind arg1,
                                  enum NJXValueKind arg2) {
  enum NJXValueKind args[2];
  args[0] = arg1;
  args[1] = arg2;
  return NJX_register_C_function(module, name, fp, return_type, args, 2);
}
static bool register_builtin_arg3(NJXContextRef module, const char *name,
                                  void *fp, enum NJXValueKind return_type,
                                  enum NJXValueKind arg1,
                                  enum NJXValueKind arg2,
                                  enum NJXValueKind arg3) {
  enum NJXValueKind args[3];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  return NJX_register_C_function(module, name, fp, return_type, args, 3);
}
static bool register_builtin_arg4(NJXContextRef module, const char *name,
                                  void *fp, enum NJXValueKind return_type,
                                  enum NJXValueKind arg1,
                                  enum NJXValueKind arg2,
                                  enum NJXValueKind arg3,
                                  enum NJXValueKind arg4) {
  enum NJXValueKind args[4];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  args[3] = arg4;
  return NJX_register_C_function(module, name, fp, return_type, args, 4);
}
static bool register_builtin_arg5(NJXContextRef module, const char *name,
                                  void *fp, enum NJXValueKind return_type,
                                  enum NJXValueKind arg1,
                                  enum NJXValueKind arg2,
                                  enum NJXValueKind arg3,
                                  enum NJXValueKind arg4,
                                  enum NJXValueKind arg5) {
  enum NJXValueKind args[5];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  args[3] = arg4;
  args[4] = arg5;
  return NJX_register_C_function(module, name, fp, return_type, args, 5);
}

// Initialize the JIT State and attach it to the
// Global Lua State
// If a JIT State already exists then this function
// will return -1
int raviV_initjit(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state != NULL) return -1;
  ravi_State *jit = (ravi_State *)calloc(1, sizeof(ravi_State));
  // The parameter true means we will be dumping stuff as we compile
  jit->jit = NJX_create_context(false);
  //extern void luaF_close (lua_State *L, StkId level);
  register_builtin_arg2(jit->jit, "luaF_close", luaF_close, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P);
  register_builtin_arg2(jit->jit, "raise_error", raise_error, NJXValueKind_V, NJXValueKind_P, NJXValueKind_I);
  //extern int luaV_tonumber_(const TValue *obj, lua_Number *n);
  register_builtin_arg2(jit->jit, "luaV_tonumber_", luaV_tonumber_, NJXValueKind_I, NJXValueKind_P, NJXValueKind_P);
  //extern int luaV_tointeger(const TValue *obj, lua_Integer *p, int mode);
  register_builtin_arg3(jit->jit, "luaV_tointeger", luaV_tointeger, NJXValueKind_I, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I);
  //extern int luaD_poscall (lua_State *L, CallInfo *ci, StkId firstResult, int nres);
  register_builtin_arg4(jit->jit, "luaD_poscall", luaD_poscall, NJXValueKind_I, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I);
  //extern int luaV_equalobj(lua_State *L, const TValue *t1, const TValue *t2);\n"
  register_builtin_arg3(jit->jit, "luaV_equalobj", luaV_equalobj, NJXValueKind_I, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P);
  //extern int luaV_lessthan(lua_State *L, const TValue *l, const TValue *r);\n"
  register_builtin_arg3(jit->jit, "luaV_lessthan", luaV_lessthan, NJXValueKind_I, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P);
  //extern int luaV_lessequal(lua_State *L, const TValue *l, const TValue *r);\n"
  register_builtin_arg3(jit->jit, "luaV_lessequal", luaV_lessequal, NJXValueKind_I, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P);
  //extern int luaV_execute(lua_State *L);
  register_builtin_arg1(jit->jit, "luaV_execute", luaV_execute, NJXValueKind_I, NJXValueKind_P);
  //extern void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val)
  register_builtin_arg4(jit->jit, "luaV_gettable", luaV_gettable, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P);
  //extern void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val);
  register_builtin_arg4(jit->jit, "luaV_settable", luaV_settable, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P);
  //int luaD_precall (lua_State *L, StkId func, int nresults, int op_call);
  register_builtin_arg4(jit->jit, "luaD_precall", luaD_precall, NJXValueKind_I, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I, NJXValueKind_I);
  //extern void raviV_op_newtable(lua_State *L, CallInfo *ci, TValue *ra, int b, int c)
  register_builtin_arg5(jit->jit, "raviV_op_newtable", raviV_op_newtable, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I, NJXValueKind_I);
  //extern void luaO_arith (lua_State *L, int op, const TValue *p1, const TValue *p2, TValue *res);
  register_builtin_arg5(jit->jit, "luaO_arith", luaO_arith, NJXValueKind_V, NJXValueKind_P, NJXValueKind_I, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P);
  //extern void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra);
  register_builtin_arg3(jit->jit, "raviV_op_newarrayint", raviV_op_newarrayint, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P);
  //extern void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra);
  register_builtin_arg3(jit->jit, "raviV_op_newarrayfloat", raviV_op_newarrayfloat, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P);
  //LUAI_FUNC void raviV_op_setlist(lua_State *L, CallInfo *ci, TValue *ra, int b, int c);
  register_builtin_arg5(jit->jit, "raviV_op_setlist", raviV_op_setlist, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I, NJXValueKind_I);
  //LUAI_FUNC void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c);
  register_builtin_arg5(jit->jit, "raviV_op_concat", raviV_op_concat, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I, NJXValueKind_I, NJXValueKind_I);
  //LUAI_FUNC void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int Bx);
  register_builtin_arg5(jit->jit, "raviV_op_closure", raviV_op_closure, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I, NJXValueKind_I);
  //LUAI_FUNC void raviV_op_vararg(lua_State *L, CallInfo *ci, LClosure *cl, int a, int b);
  register_builtin_arg5(jit->jit, "raviV_op_vararg", raviV_op_vararg, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I, NJXValueKind_I);
  // void luaV_objlen (lua_State *L, StkId ra, const TValue *rb)
  register_builtin_arg3(jit->jit, "luaV_objlen", luaV_objlen, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P);
  //int luaV_forlimit(const TValue *obj, lua_Integer *p, lua_Integer step, int *stopnow);
  register_builtin_arg4(jit->jit, "luaV_forlimit", luaV_forlimit, NJXValueKind_I, NJXValueKind_P, NJXValueKind_P, NJXValueKind_Q, NJXValueKind_P);
  // void raviV_op_setupval(lua_State *L, LClosure *cl, TValue *ra, int b);
  register_builtin_arg4(jit->jit, "raviV_op_setupval", raviV_op_setupval, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I);
  register_builtin_arg4(jit->jit, "raviV_op_setupvali", raviV_op_setupvali, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I);
  register_builtin_arg4(jit->jit, "raviV_op_setupvalf", raviV_op_setupvalf, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I);
  register_builtin_arg4(jit->jit, "raviV_op_setupvalai", raviV_op_setupvalai, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I);
  register_builtin_arg4(jit->jit, "raviV_op_setupvalaf", raviV_op_setupvalaf, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I);
  register_builtin_arg4(jit->jit, "raviV_op_setupvalt", raviV_op_setupvalt, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I);
  //extern void luaD_call (lua_State *L, StkId func, int nResults);
  register_builtin_arg3(jit->jit, "luaD_call", luaD_call, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I);
  //"extern void raviH_set_int(lua_State *L, Table *t, lua_Unsigned key, lua_Integer value);\n"
  register_builtin_arg4(jit->jit, "raviH_set_int", raviH_set_int, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_Q, NJXValueKind_Q);
  //"extern void raviH_set_float(lua_State *L, Table *t, lua_Unsigned key, lua_Number value);\n"
  register_builtin_arg4(jit->jit, "raviH_set_float", raviH_set_float, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P, NJXValueKind_Q, NJXValueKind_D);

  G->ravi_state = jit;
  return 0;
}

// Free up the JIT State
void raviV_close(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL) return;
  // All compiled functions will be deleted at this stage
  NJX_destroy_context(G->ravi_state->jit);
  free(G->ravi_state);
}

// Dump the IR
void raviV_dumpIR(struct lua_State *L, struct Proto *p) {
	global_State *G = G(L);
	if (G->ravi_state == NULL) 
		return;

	membuff_t buf;
	membuff_init(&buf, 4096);

	char fname[30];
	snprintf(fname, sizeof fname, "jit", G->ravi_state->id++);
	ravi_compile_options_t options = { 0 };
	options.codegen_type = RAVI_CODEGEN_ALL;
	if (raviJ_codegen(L, p, &options, fname, &buf)) {
		printf(buf.buf);
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
  // G->ravi_state->jit->min_exec_count_ = value;
}
int raviV_getminexeccount(lua_State *L) {
  global_State *G = G(L);
  // if (!G->ravi_state)
  return 0;
  // return G->ravi_state->jit->min_exec_count_;
}

void raviV_setmincodesize(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  // G->ravi_state->jit->min_code_size_ = value;
}
int raviV_getmincodesize(lua_State *L) {
  global_State *G = G(L);
  // if (!G->ravi_state)
  return 0;
  // return G->ravi_state->jit->min_code_size_;
}

void raviV_setauto(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  // G->ravi_state->jit->auto_ = value;
}
int raviV_getauto(lua_State *L) {
  global_State *G = G(L);
  // if (!G->ravi_state)
  return 0;
  // return G->ravi_state->jit->auto_;
}

// Turn on/off the JIT compiler
void raviV_setjitenabled(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  // G->ravi_state->jit->enabled_ = value;
}
int raviV_getjitenabled(lua_State *L) {
  global_State *G = G(L);
  return G->ravi_state != NULL;
  // return G->ravi_state->jit->enabled_;
}

void raviV_setoptlevel(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  // G->ravi_state->jit->opt_level_ = value;
}
int raviV_getoptlevel(lua_State *L) {
  global_State *G = G(L);
  // if (!G->ravi_state)
  return 0;
  // return G->ravi_state->jit->opt_level_;
}

void raviV_setsizelevel(lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getsizelevel(lua_State *L) {
  (void)L;
  return 0;
}

void raviV_setgcstep(lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getgcstep(lua_State *L) {
  (void)L;
  return 0;
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



// Compile a Lua function
// If JIT is turned off then compilation is skipped
// Compilation occurs if either auto compilation is ON (subject to some
// thresholds)
// or if a manual compilation request was made
// Returns true if compilation was successful
int raviV_compile(struct lua_State *L, struct Proto *p,
                  ravi_compile_options_t *options) {
  if (p->ravi_jit.jit_status == RAVI_JIT_COMPILED) return true;
  else if (p->ravi_jit.jit_status == RAVI_JIT_CANT_COMPILE) return false;
  if (options == NULL || !options->manual_request) return false;

  if (!raviJ_cancompile(p)) {
	  p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
	  return false;
  }

  global_State *G = G(L);
  if (G->ravi_state == NULL) return false;
  NJXContextRef context = G->ravi_state->jit;
  if (context == NULL) return false;

  membuff_t buf;
  membuff_init(&buf, 4096);

  char fname[30];
  snprintf(fname, sizeof fname, "jit%lld", G->ravi_state->id++);

  if (!raviJ_codegen(L, p, options, fname, &buf)) {
	  membuff_free(&buf);
	  p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
	  return false;
  }
  int (*fp)(lua_State * L) = NULL;
  //printf("%s\n", buf.buf);
#if 1
  char *argv[] = {NULL};
  if (!dmrC_nanocompile(0, argv, context, buf.buf)) {
    printf("%s\n", buf.buf);
    p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
  }
  else {
    fp = NJX_get_function_by_name(context, fname);
    if (fp != NULL) {
      p->ravi_jit.jit_data = NULL;
      p->ravi_jit.jit_function = fp;
      p->ravi_jit.jit_status = RAVI_JIT_COMPILED;
    }
  }
#else
  p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
#endif
  membuff_free(&buf);
  return fp != NULL;
}

// Free the JIT compiled function
// Note that this is called by the garbage collector
void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}


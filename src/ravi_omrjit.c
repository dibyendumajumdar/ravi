/******************************************************************************
 * Copyright (C) 2018 Dibyendu Majumdar
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

#include <ravi_omrjit.h>
#include <ravijit.h>
#include <stddef.h>
#include <assert.h>

#define LUA_CORE

#include "lauxlib.h"
#include "lobject.h"
#include "lstate.h"
#include "lua.h"

static const char *errortext[] = {"integer expected",
                                  "number expected",
                                  "integer[] expected",
                                  "number[] expected",
                                  "table expected",
                                  "upvalue of integer type, cannot be set to non integer value",
                                  "upvalue of number type, cannot be set to non number value",
                                  "upvalue of integer[] type, cannot be set to non integer[] value",
                                  "upvalue of number[] type, cannot be set to non number[] value",
                                  "upvalue of table type, cannot be set to non table value",
                                  "for llimit must be a number",
                                  "for step must be a number",
                                  "for initial value must be a number",
                                  "array index is out of bounds",
                                  "string expected",
                                  "closure expected",
                                  "type mismatch: wrong userdata type",
                                  NULL};

static void raise_error(lua_State *L, int errorcode) {
  assert(errorcode >= 0 && errorcode <= Error_type_mismatch);
  luaG_runerror(L, errortext[errorcode]);
}

static void register_builtin_arg1(JIT_ContextRef module, const char *name,
                                  void *fp, JIT_Type return_type,
                                  JIT_Type arg1) {
  JIT_Type args[1];
  args[0] = arg1;
  JIT_RegisterFunction(module, name, return_type, 1, args, fp);
}
static void register_builtin_arg2(JIT_ContextRef module, const char *name,
                                  void *fp, JIT_Type return_type,
                                  JIT_Type arg1,
                                  JIT_Type arg2) {
  JIT_Type args[2];
  args[0] = arg1;
  args[1] = arg2;
  JIT_RegisterFunction(module, name, return_type, 2, args, fp);
}
static void register_builtin_arg3(JIT_ContextRef module, const char *name,
                                  void *fp, JIT_Type return_type,
                                  JIT_Type arg1,
                                  JIT_Type arg2,
                                  JIT_Type arg3) {
  JIT_Type args[3];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  JIT_RegisterFunction(module, name, return_type, 3, args, fp);
}
static void register_builtin_arg4(JIT_ContextRef module, const char *name,
                                  void *fp, JIT_Type return_type,
                                  JIT_Type arg1,
                                  JIT_Type arg2,
                                  JIT_Type arg3,
                                  JIT_Type arg4) {
  JIT_Type args[4];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  args[3] = arg4;
  JIT_RegisterFunction(module, name, return_type, 4, args, fp);
}
static void register_builtin_arg5(JIT_ContextRef module, const char *name,
                                  void *fp, JIT_Type return_type,
                                  JIT_Type arg1,
                                  JIT_Type arg2,
                                  JIT_Type arg3,
                                  JIT_Type arg4,
                                  JIT_Type arg5) {
  JIT_Type args[5];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  args[3] = arg4;
  args[4] = arg5;
  JIT_RegisterFunction(module, name, return_type, 5, args, fp);
}

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
  jit->opt_level_ = 1;
  // The parameter true means we will be dumping stuff as we compile
  jit->jit = JIT_CreateContext();
  //extern void luaF_close (lua_State *L, StkId level);
  register_builtin_arg2(jit->jit, "luaF_close", luaF_close, JIT_NoType, JIT_Address, JIT_Address);
  register_builtin_arg2(jit->jit, "raise_error", raise_error, JIT_NoType, JIT_Address, JIT_Int32);
  //extern int luaV_tonumber_(const TValue *obj, lua_Number *n);
  register_builtin_arg2(jit->jit, "luaV_tonumber_", luaV_tonumber_, JIT_Int32, JIT_Address, JIT_Address);
  //extern int luaV_tointeger(const TValue *obj, lua_Integer *p, int mode);
  register_builtin_arg3(jit->jit, "luaV_tointeger", luaV_tointeger, JIT_Int32, JIT_Address, JIT_Address, JIT_Int32);
  //extern int luaD_poscall (lua_State *L, CallInfo *ci, StkId firstResult, int nres);
  register_builtin_arg4(jit->jit, "luaD_poscall", luaD_poscall, JIT_Int32, JIT_Address, JIT_Address, JIT_Address, JIT_Int32);
  //extern int luaV_equalobj(lua_State *L, const TValue *t1, const TValue *t2);\n"
  register_builtin_arg3(jit->jit, "luaV_equalobj", luaV_equalobj, JIT_Int32, JIT_Address, JIT_Address, JIT_Address);
  //extern int luaV_lessthan(lua_State *L, const TValue *l, const TValue *r);\n"
  register_builtin_arg3(jit->jit, "luaV_lessthan", luaV_lessthan, JIT_Int32, JIT_Address, JIT_Address, JIT_Address);
  //extern int luaV_lessequal(lua_State *L, const TValue *l, const TValue *r);\n"
  register_builtin_arg3(jit->jit, "luaV_lessequal", luaV_lessequal, JIT_Int32, JIT_Address, JIT_Address, JIT_Address);
  //extern int luaV_execute(lua_State *L);
  register_builtin_arg1(jit->jit, "luaV_execute", luaV_execute, JIT_Int32, JIT_Address);
  //extern void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val)
  register_builtin_arg4(jit->jit, "luaV_gettable", luaV_gettable, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Address);
  //extern void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val);
  register_builtin_arg4(jit->jit, "luaV_settable", luaV_settable, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Address);
  //int luaD_precall (lua_State *L, StkId func, int nresults, int op_call);
  register_builtin_arg4(jit->jit, "luaD_precall", luaD_precall, JIT_Int32, JIT_Address, JIT_Address, JIT_Int32, JIT_Int32);
  //extern void raviV_op_newtable(lua_State *L, CallInfo *ci, TValue *ra, int b, int c)
  register_builtin_arg5(jit->jit, "raviV_op_newtable", raviV_op_newtable, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Int32, JIT_Int32);
  //extern void luaO_arith (lua_State *L, int op, const TValue *p1, const TValue *p2, TValue *res);
  register_builtin_arg5(jit->jit, "luaO_arith", luaO_arith, JIT_NoType, JIT_Address, JIT_Int32, JIT_Address, JIT_Address, JIT_Address);
  //extern void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra);
  register_builtin_arg3(jit->jit, "raviV_op_newarrayint", raviV_op_newarrayint, JIT_NoType, JIT_Address, JIT_Address, JIT_Address);
  //extern void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra);
  register_builtin_arg3(jit->jit, "raviV_op_newarrayfloat", raviV_op_newarrayfloat, JIT_NoType, JIT_Address, JIT_Address, JIT_Address);
  //LUAI_FUNC void raviV_op_setlist(lua_State *L, CallInfo *ci, TValue *ra, int b, int c);
  register_builtin_arg5(jit->jit, "raviV_op_setlist", raviV_op_setlist, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Int32, JIT_Int32);
  //LUAI_FUNC void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c);
  register_builtin_arg5(jit->jit, "raviV_op_concat", raviV_op_concat, JIT_NoType, JIT_Address, JIT_Address, JIT_Int32, JIT_Int32, JIT_Int32);
  //LUAI_FUNC void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int Bx);
  register_builtin_arg5(jit->jit, "raviV_op_closure", raviV_op_closure, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Int32, JIT_Int32);
  //LUAI_FUNC void raviV_op_vararg(lua_State *L, CallInfo *ci, LClosure *cl, int a, int b);
  register_builtin_arg5(jit->jit, "raviV_op_vararg", raviV_op_vararg, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Int32, JIT_Int32);
  // void luaV_objlen (lua_State *L, StkId ra, const TValue *rb)
  register_builtin_arg3(jit->jit, "luaV_objlen", luaV_objlen, JIT_NoType, JIT_Address, JIT_Address, JIT_Address);
  //int luaV_forlimit(const TValue *obj, lua_Integer *p, lua_Integer step, int *stopnow);
  register_builtin_arg4(jit->jit, "luaV_forlimit", luaV_forlimit, JIT_Int32, JIT_Address, JIT_Address, JIT_Int64, JIT_Address);
  // void raviV_op_setupval(lua_State *L, LClosure *cl, TValue *ra, int b);
  register_builtin_arg4(jit->jit, "raviV_op_setupval", raviV_op_setupval, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Int32);
  register_builtin_arg4(jit->jit, "raviV_op_setupvali", raviV_op_setupvali, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Int32);
  register_builtin_arg4(jit->jit, "raviV_op_setupvalf", raviV_op_setupvalf, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Int32);
  register_builtin_arg4(jit->jit, "raviV_op_setupvalai", raviV_op_setupvalai, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Int32);
  register_builtin_arg4(jit->jit, "raviV_op_setupvalaf", raviV_op_setupvalaf, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Int32);
  register_builtin_arg4(jit->jit, "raviV_op_setupvalt", raviV_op_setupvalt, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Int32);
  //extern void luaD_call (lua_State *L, StkId func, int nResults);
  register_builtin_arg3(jit->jit, "luaD_call", luaD_call, JIT_NoType, JIT_Address, JIT_Address, JIT_Int32);
  //"extern void raviH_set_int(lua_State *L, Table *t, lua_Unsigned key, lua_Integer value);\n"
  register_builtin_arg4(jit->jit, "raviH_set_int", raviH_set_int, JIT_NoType, JIT_Address, JIT_Address, JIT_Int64, JIT_Int64);
  //"extern void raviH_set_float(lua_State *L, Table *t, lua_Unsigned key, lua_Number value);\n"
  register_builtin_arg4(jit->jit, "raviH_set_float", raviH_set_float, JIT_NoType, JIT_Address, JIT_Address, JIT_Int64, JIT_Double);
  //int raviV_check_usertype(lua_State *L, TString *name, const TValue *o);
  register_builtin_arg3(jit->jit, "raviV_check_usertype", raviV_check_usertype, JIT_Int32, JIT_Address, JIT_Address, JIT_Address);
  // void luaT_trybinTM (lua_State *L, const TValue *p1, const TValue *p2, TValue *res, TMS event);
  register_builtin_arg5(jit->jit, "luaT_trybinTM", luaT_trybinTM, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Address, JIT_Int32);
  // void raviV_gettable_sskey(lua_State *L, const TValue *t, TValue *key, StkId val);
  register_builtin_arg4(jit->jit, "raviV_gettable_sskey", raviV_gettable_sskey, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Address);
  // void raviV_settable_sskey(lua_State *L, const TValue *t, TValue *key, StkId val);
  register_builtin_arg4(jit->jit, "raviV_settable_sskey", raviV_settable_sskey, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Address);
  // void raviV_gettable_i(lua_State *L, const TValue *t, TValue *key, StkId val);
  register_builtin_arg4(jit->jit, "raviV_gettable_i", raviV_gettable_i, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Address);
  // void raviV_settable_i(lua_State *L, const TValue *t, TValue *key, StkId val);
  register_builtin_arg4(jit->jit, "raviV_settable_i", raviV_settable_i, JIT_NoType, JIT_Address, JIT_Address, JIT_Address, JIT_Address);

  G->ravi_state = jit;
  return 0;
}

// Free up the JIT State
void raviV_close(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL) return;
  // All compiled functions will be deleted at this stage
  JIT_DestroyContext(G->ravi_state->jit);
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
  G->ravi_state->opt_level_ = value;
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
int raviV_compile(struct lua_State *L, struct Proto *p, ravi_compile_options_t *options) {
  if (p->ravi_jit.jit_status == RAVI_JIT_COMPILED)
    return true;
  else if (p->ravi_jit.jit_status == RAVI_JIT_CANT_COMPILE)
    return false;
  if (options == NULL) return false;

  global_State *G = G(L);
  if (G->ravi_state == NULL) return false;
  JIT_ContextRef context = G->ravi_state->jit;
  if (context == NULL) return false;

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
  char *argv[] = {NULL};
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
  if (!dmrC_omrcompile(0, argv, context, buf.buf)) {
    p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
  }
  else {
    fp = JIT_GetFunction(context, fname);
    if (fp != NULL) {
      p->ravi_jit.jit_data = NULL;
      p->ravi_jit.jit_function = fp;
      p->ravi_jit.jit_status = RAVI_JIT_COMPILED;
    }
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


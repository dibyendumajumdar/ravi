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

#include <ravi_gccjit.h>
#include <assert.h>

// OP_JMP
void ravi_emit_JMP(ravi_function_def_t *def, int A, int j, int pc) {

  //#define dojump(ci,i,e)
  // { int a = GETARG_A(i);
  //   if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  //   ci->u.l.savedpc += GETARG_sBx(i) + e; }
  //
  // dojump(ci, i, 0);

  assert(def->jmp_targets[j]->jmp);
  if (def->current_block_terminated) {
    gcc_jit_block *jmp_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "OP_JMP", pc));
    ravi_set_current_block(def, jmp_block);
  }
  // ravi_debug_printf2(def, "OP_JMP(%d) jmp to %d\n", ravi_int_constant(def,
  // pc+1), ravi_int_constant(def, j+1));

  // if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  if (A > 0) {
    ravi_emit_load_base(def);
    // base + a - 1
    gcc_jit_lvalue *val = ravi_emit_get_register(def, A - 1);
    // Call luaF_close
    gcc_jit_block_add_eval(
        def->current_block, NULL,
        ravi_function_call2_rvalue(def, def->ravi->types->luaF_closeT,
                                   gcc_jit_param_as_rvalue(def->L),
                                   gcc_jit_lvalue_get_address(val, NULL)));
  }

  ravi_emit_branch(def, def->jmp_targets[j]->jmp);

  gcc_jit_block *block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_JMP_post", pc));
  ravi_set_current_block(def, block);
}

// Handle OP_CALL
void ravi_emit_CALL(ravi_function_def_t *def, int A, int B, int C, int pc) {

  // int nresults = c - 1;
  // if (b != 0)
  //   L->top = ra + b;  /* else previous instruction set top */
  // int c = luaD_precall(L, ra, nresults);  /* C or JITed function? */
  // if (c) {
  //   if (c == 1 && nresults >= 0)
  //     L->top = ci->top;  /* adjust results if C function */
  // }
  // else {  /* Lua function */
  //   luaV_execute(L);
  // }

  ravi_emit_load_base(def);

  // int nresults = c - 1;
  int nresults = C - 1;

  // if (b != 0)
  if (B != 0) {
    ravi_emit_set_L_top_toreg(def, A + B);
  }

  // luaD_precall() returns following
  // 1 - C function called, results to be adjusted
  // 2 - JITed Lua function called, no action
  // 0 - Run interpreter on Lua function

  // int c = luaD_precall(L, ra, nresults);  /* C or JITed function? */
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_rvalue *nresults_const = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, nresults);
  gcc_jit_rvalue *precall_result = ravi_function_call4_rvalue(
      def, def->ravi->types->luaD_precallT, gcc_jit_param_as_rvalue(def->L),
      gcc_jit_lvalue_get_address(ra, NULL), nresults_const,
      ravi_int_constant(def, 1));
  /* Need to save the result of the luaD_precall() so that we can do another
   * check later on
   */
  gcc_jit_lvalue *tmp_var = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->C_intT,
      unique_name(def, "OP_CALL_luaD_precall_result", pc));
  gcc_jit_block_add_assignment(def->current_block, NULL, tmp_var,
                               precall_result);
  gcc_jit_rvalue *zero_const = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, 0);
  gcc_jit_rvalue *precall_bool =
      ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ,
                           gcc_jit_lvalue_as_rvalue(tmp_var), zero_const);

  gcc_jit_block *then_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_CALL_if_lua_function", pc));
  gcc_jit_block *else_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_CALL_else_lua_function", pc));
  gcc_jit_block *end_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_CALL_done", pc));

  ravi_emit_conditional_branch(def, precall_bool, then_block, else_block);
  ravi_set_current_block(def, then_block);

  // Lua function, not compiled, so call luaV_execute
  gcc_jit_rvalue *b = ravi_function_call1_rvalue(
      def, def->ravi->types->luaV_executeT, gcc_jit_param_as_rvalue(def->L));

  // If the return value is non zero then we need to refresh L->top = ci->top
  gcc_jit_rvalue *b_not_zero = ravi_emit_comparison(
      def, GCC_JIT_COMPARISON_NE, b, ravi_int_constant(def, 0));

  gcc_jit_block *if_b_block = gcc_jit_function_new_block(
      def->jit_function,
      unique_name(def, "OP_CALL_if_luaV_execute_b_value", pc));
  ravi_emit_conditional_branch(def, b_not_zero, if_b_block, else_block);

  ravi_set_current_block(def, if_b_block);

  ravi_emit_refresh_L_top(def);
  ravi_emit_branch(def, end_block);

  ravi_set_current_block(def, else_block);

  if (nresults >= 0) {
    // In case the precall returned 1 then a C function was
    // called so we need to update L->top
    //   if (c == 1 && nresults >= 0)
    //     L->top = ci->top;  /* adjust results if C function */
    gcc_jit_rvalue *one_const = gcc_jit_context_new_rvalue_from_int(
        def->function_context, def->ravi->types->C_intT, 1);
    gcc_jit_rvalue *precall_C =
        ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ,
                             gcc_jit_lvalue_as_rvalue(tmp_var), one_const);

    gcc_jit_block *then1_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "OP_CALL_if_C_function", pc));
    ravi_emit_conditional_branch(def, precall_C, then1_block, end_block);
    ravi_set_current_block(def, then1_block);

    //     L->top = ci->top;  /* adjust results if C function */
    ravi_emit_refresh_L_top(def);
  }

  ravi_emit_branch(def, end_block);
  ravi_set_current_block(def, end_block);
}

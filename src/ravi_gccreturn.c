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

void ravi_emit_RETURN(ravi_function_def_t *def, int A, int B, int pc) {

  // Here is what OP_RETURN looks like. We only compile steps
  // marked with //*. This is because the rest is only relevant in the
  // interpreter

  // case OP_RETURN: {
  //  int b = GETARG_B(i);
  //*  if (b != 0) L->top = ra + b - 1;
  //*  if (cl->p->sizep > 0) luaF_close(L, base);
  //*  b = luaD_poscall(L, ra);
  //    if (!(ci->callstatus & CIST_REENTRY))  /* 'ci' still the called one */
  //      return;  /* external invocation: return */
  //    else {  /* invocation via reentry: continue execution */
  //      ci = L->ci;
  //      if (b) L->top = ci->top;
  //      goto newframe;  /* restart luaV_execute over new Lua function */
  //    }
  // }

  // As Lua inserts redundant OP_RETURN instructions it is
  // possible that this is one of them. If this is the case then the
  // current block may already be terminated - so we have to insert
  // a new block
  if (def->current_block_terminated) {
    def->current_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "OP_RETURN", pc));
    def->current_block_terminated = false;
  }

  // Load pointer to base
  ravi_emit_load_base(def);

  // Get pointer to register A
  gcc_jit_rvalue *ra_ptr = ravi_emit_get_register(def, A);

  //*  if (b != 0) L->top = ra + b - 1;
  if (B != 0) {
    ravi_emit_set_L_top_toreg(def, A + B - 1);
  }

  // if (cl->p->sizep > 0) luaF_close(L, base);
  // Get pointer to Proto->sizep
  gcc_jit_lvalue *psize = ravi_emit_get_Proto_sizep(def);

  // Test if psize > 0
  gcc_jit_rvalue *psize_gt_0 = ravi_emit_comparison(
      def, GCC_JIT_COMPARISON_GT, gcc_jit_lvalue_as_rvalue(psize),
      gcc_jit_context_new_rvalue_from_int(def->function_context,
                                          def->ravi->types->C_intT, 0));

  gcc_jit_block *then_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RETURN_if_sizep_gt_0", pc));
  gcc_jit_block *else_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RETURN_else_sizep_gt_0", pc));

  ravi_emit_conditional_branch(def, psize_gt_0, then_block, else_block);

  ravi_set_current_block(def, then_block);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call2_rvalue(def, def->ravi->types->luaF_closeT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_as_rvalue(def->base)));

  gcc_jit_block_end_with_jump(def->current_block, NULL, else_block);
  ravi_set_current_block(def, else_block);

  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call2_rvalue(def, def->ravi->types->luaD_poscallT,
                                 gcc_jit_param_as_rvalue(def->L), ra_ptr));

  gcc_jit_block_end_with_return(
      def->current_block, NULL,
      gcc_jit_context_new_rvalue_from_int(def->function_context,
                                          def->ravi->types->C_intT, 1));
  def->current_block_terminated = true;
}

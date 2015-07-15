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

void ravi_emit_iFORLOOP(ravi_function_def_t *def, int A, int pc,
                        ravi_branch_def_t *b, int step_one) {

  //  lua_Integer step = ivalue(ra + 2);
  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  //  lua_Integer limit = ivalue(ra + 1);
  //  if (idx <= limit) {
  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    setivalue(ra, idx);  /* update internal index... */
  //    setivalue(ra + 3, idx);  /* ...and external index */
  //  }

  // We are in b->jmp as this is already the current block
  lua_assert(def->current_block == b->jmp);

  // Create the done block
  gcc_jit_block *exit_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "FORLOOP_I1_exit", 0));

  gcc_jit_rvalue *new_idx;

  if (!step_one) {
    //  lua_Integer step = ivalue(ra + 2);
    new_idx = gcc_jit_context_new_binary_op(
        def->function_context, NULL, GCC_JIT_BINARY_OP_PLUS,
        def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(b->iidx),
        gcc_jit_lvalue_as_rvalue(b->istep));
  } else
    new_idx = gcc_jit_context_new_binary_op(
        def->function_context, NULL, GCC_JIT_BINARY_OP_PLUS,
        def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(b->iidx),
        gcc_jit_context_new_rvalue_from_int(def->function_context,
                                            def->ravi->types->lua_IntegerT, 1));

  // save new index
  gcc_jit_block_add_assignment(def->current_block, NULL, b->iidx, new_idx);

  // lua_Integer limit = ivalue(ra + 1);

  // idx > limit?
  gcc_jit_rvalue *new_idx_gt_limit = ravi_emit_comparison(
      def, GCC_JIT_COMPARISON_GT, gcc_jit_lvalue_as_rvalue(b->iidx),
      gcc_jit_lvalue_as_rvalue(b->ilimit));

  // If idx > limit we are done
  gcc_jit_block *update_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "FORLOOP_I1_updatei", 0));
  ravi_emit_conditional_branch(def, new_idx_gt_limit, exit_block, update_block);

  ravi_set_current_block(def, update_block);

  // Load pointer to base
  ravi_emit_load_base(def);

  // setivalue(ra + 3, idx);  /* ...and external index */
  gcc_jit_lvalue *rvar = ravi_emit_get_register(def, A + 3);
  ravi_emit_store_reg_i_withtype(def, gcc_jit_lvalue_as_rvalue(b->iidx), rvar);

  // ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  ravi_emit_branch(def, def->jmp_targets[pc]->jmp);
  ravi_set_current_block(def, exit_block);
}

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

void ravi_emit_iFORPREP(ravi_function_def_t *def, int A, int pc, int step_one) {
  ravi_branch_def_t *forloop_target = def->jmp_targets[pc];
  assert(forloop_target);

  forloop_target->ilimit = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->lua_IntegerT,
      unique_name(def, "ilimit", 0));
  if (!step_one) {
    forloop_target->istep = gcc_jit_function_new_local(
        def->jit_function, NULL, def->ravi->types->lua_IntegerT,
        unique_name(def, "istep", 0));
  }
  forloop_target->iidx = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->lua_IntegerT,
      unique_name(def, "iidx", 0));

  //    lua_Integer initv = ivalue(init);
  //    setivalue(init, initv - ivalue(pstep));

  // Load pointer to base
  ravi_emit_load_base(def);

  //  TValue *init = ra;
  //  TValue *pstep = ra + 2;
  gcc_jit_lvalue *init = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *plimit = ravi_emit_get_register(def, A + 1);
  gcc_jit_lvalue *pstep = NULL;
  if (!step_one)
    pstep = ravi_emit_get_register(def, A + 2);
  // Get ivalue(pstep)

  gcc_jit_lvalue *limit_ivalue = ravi_emit_load_reg_i(def, plimit);
  gcc_jit_lvalue *init_ivalue = ravi_emit_load_reg_i(def, init);

  if (!step_one) {
    //    setivalue(init, initv - ivalue(pstep));
    gcc_jit_lvalue *step_ivalue = ravi_emit_load_reg_i(def, pstep);
    gcc_jit_rvalue *idx = gcc_jit_context_new_binary_op(
        def->function_context, NULL, GCC_JIT_BINARY_OP_MINUS,
        def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(init_ivalue),
        gcc_jit_lvalue_as_rvalue(step_ivalue));
    // Save idx
    gcc_jit_block_add_assignment(def->current_block, NULL, forloop_target->iidx,
                                 idx);

    // Save step
    gcc_jit_block_add_assignment(def->current_block, NULL,
                                 forloop_target->istep,
                                 gcc_jit_lvalue_as_rvalue(step_ivalue));
  } else {
    //    setivalue(init, initv - ivalue(pstep));
    gcc_jit_rvalue *idx = gcc_jit_context_new_binary_op(
        def->function_context, NULL, GCC_JIT_BINARY_OP_MINUS,
        def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(init_ivalue),
        gcc_jit_context_new_rvalue_from_int(def->function_context,
                                            def->ravi->types->lua_IntegerT, 1));

    // Save idx
    gcc_jit_block_add_assignment(def->current_block, NULL, forloop_target->iidx,
                                 idx);
  }

  // Save limit
  gcc_jit_block_add_assignment(def->current_block, NULL, forloop_target->ilimit,
                               gcc_jit_lvalue_as_rvalue(limit_ivalue));

  // We are done so jump to forloop
  lua_assert(def->jmp_targets[pc]->jmp);
  ravi_emit_branch(def, def->jmp_targets[pc]->jmp);
}

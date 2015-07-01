#include <ravi_gccjit.h>

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

void ravi_emit_CONCAT(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call5_rvalue(
          def, def->ravi->types->raviV_op_concatT,
          gcc_jit_param_as_rvalue(def->L),
          gcc_jit_lvalue_as_rvalue(def->ci_val),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, A),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, B),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, C)));
}

void ravi_emit_CLOSURE(ravi_function_def_t *def, int A, int Bx, int pc) {
  (void)pc;
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call5_rvalue(
          def, def->ravi->types->raviV_op_closureT,
          gcc_jit_param_as_rvalue(def->L),
          gcc_jit_lvalue_as_rvalue(def->ci_val), def->lua_closure,
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, A),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, Bx)));
}

void ravi_emit_VARARG(ravi_function_def_t *def, int A, int B, int pc) {
  (void)pc;
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call5_rvalue(
          def, def->ravi->types->raviV_op_varargT,
          gcc_jit_param_as_rvalue(def->L),
          gcc_jit_lvalue_as_rvalue(def->ci_val), def->lua_closure,
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, A),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, B)));
}
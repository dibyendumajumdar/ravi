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

// R(A) := RK(B) + C, result is floating
void ravi_emit_ADDFN(ravi_function_def_t *def, int A, int B, int C) {
  // Load pointer to base
  ravi_emit_load_base(def);

  // ra
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);

  // rb
  gcc_jit_rvalue *rb = ravi_emit_get_register_or_constant(def, B);

  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_n(def, rb);

  // result = rb->value_.n + (double)C
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_PLUS,
      def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_context_new_rvalue_from_int(def->function_context,
                                          def->ravi->types->lua_NumberT, C));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

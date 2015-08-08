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

// R(A) := -R(B), floating point
void ravi_emit_UNMF(ravi_function_def_t *def, int A, int B, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_n(def, rb);
  // result = -rb->value_.n;
  gcc_jit_rvalue *result = gcc_jit_context_new_unary_op(
      def->function_context, NULL, GCC_JIT_UNARY_OP_MINUS,
      def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs));
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := -R(B), integer
void ravi_emit_UNMI(ravi_function_def_t *def, int A, int B, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_i(def, rb);
  gcc_jit_rvalue *result = gcc_jit_context_new_unary_op(
      def->function_context, NULL, GCC_JIT_UNARY_OP_MINUS,
      def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(lhs));
  ravi_emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) + RK(C), all floating
void ravi_emit_ADDFF(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_n(def, rb);
  // rc->value_.n
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_n(def, rc);
  // result = rb->value_.n + rc->value_.n
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_PLUS,
      def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_lvalue_as_rvalue(rhs));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) * RK(C), float*float
void ravi_emit_MULFF(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_n(def, rb);
  // rc->value_.n
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_n(def, rc);
  // result = rb->value_.n * rc->value_.n
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_MULT,
      def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_lvalue_as_rvalue(rhs));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), float-float
void ravi_emit_SUBFF(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_n(def, rb);
  // rc->value_.n
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_n(def, rc);
  // result = rb->value_.n - rc->value_.n
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_MINUS,
      def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_lvalue_as_rvalue(rhs));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), float/float
void ravi_emit_DIVFF(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_n(def, rb);
  // rc->value_.n
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_n(def, rc);
  // result = rb->value_.n / rc->value_.n
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_DIVIDE,
      def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_lvalue_as_rvalue(rhs));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) + RK(C), float+int
void ravi_emit_ADDFI(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_n(def, rb);
  // rc->value_.i
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_i(def, rc);
  // result = rb->value_.n + (lua_number)rc->value_.i
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_PLUS,
      def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_context_new_cast(def->function_context, NULL,
                               gcc_jit_lvalue_as_rvalue(rhs),
                               def->ravi->types->lua_NumberT));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) * RK(C), float*int
void ravi_emit_MULFI(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_n(def, rb);
  // rc->value_.i
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_i(def, rc);
  // result = rb->value_.n * (lua_number)rc->value_.i
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_MULT,
      def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_context_new_cast(def->function_context, NULL,
                               gcc_jit_lvalue_as_rvalue(rhs),
                               def->ravi->types->lua_NumberT));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), float-int
void ravi_emit_SUBFI(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_n(def, rb);
  // rc->value_.i
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_i(def, rc);
  // result = rb->value_.n - (lua_number)rc->value_.i
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_MINUS,
      def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_context_new_cast(def->function_context, NULL,
                               gcc_jit_lvalue_as_rvalue(rhs),
                               def->ravi->types->lua_NumberT));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), float/int
void ravi_emit_DIVFI(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.n
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_n(def, rb);
  // rc->value_.i
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_i(def, rc);
  // result = rb->value_.n / (lua_number)rc->value_.i
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_DIVIDE,
      def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_context_new_cast(def->function_context, NULL,
                               gcc_jit_lvalue_as_rvalue(rhs),
                               def->ravi->types->lua_NumberT));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), int-float
void ravi_emit_SUBIF(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.i
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_i(def, rb);
  // rc->value_.n
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_n(def, rc);
  // result = (lua_Number) rb->value_.i - rc->value_.n
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_MINUS,
      def->ravi->types->lua_NumberT,
      gcc_jit_context_new_cast(def->function_context, NULL,
                               gcc_jit_lvalue_as_rvalue(lhs),
                               def->ravi->types->lua_NumberT),
      gcc_jit_lvalue_as_rvalue(rhs));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), int/float
void ravi_emit_DIVIF(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.i
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_i(def, rb);
  // rc->value_.n
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_n(def, rc);
  // result = (lua_Number) rb->value_.i / rc->value_.n
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_DIVIDE,
      def->ravi->types->lua_NumberT,
      gcc_jit_context_new_cast(def->function_context, NULL,
                               gcc_jit_lvalue_as_rvalue(lhs),
                               def->ravi->types->lua_NumberT),
      gcc_jit_lvalue_as_rvalue(rhs));
  // ra->value_.n = result
  // ra->tt_ = LUA_TNUMFLT
  ravi_emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) + RK(C), int+int
void ravi_emit_ADDII(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.i
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_i(def, rb);
  // rc->value_.i
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_i(def, rc);
  // result = rb->value_.i + rc->value_.i
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_PLUS,
      def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_lvalue_as_rvalue(rhs));
  // ra->value_.i = result
  // ra->tt_ = LUA_TNUMINT
  ravi_emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) * RK(C), int*int
void ravi_emit_MULII(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.i
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_i(def, rb);
  // rc->value_.i
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_i(def, rc);
  // result = rb->value_.i * rc->value_.i
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_MULT,
      def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_lvalue_as_rvalue(rhs));
  // ra->value_.i = result
  // ra->tt_ = LUA_TNUMINT
  ravi_emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), int-int
void ravi_emit_SUBII(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.i
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_i(def, rb);
  // rc->value_.i
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_i(def, rc);
  // result = rb->value_.i - rc->value_.i
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_MINUS,
      def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(lhs),
      gcc_jit_lvalue_as_rvalue(rhs));
  // ra->value_.i = result
  // ra->tt_ = LUA_TNUMINT
  ravi_emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), int/int but result is float
void ravi_emit_DIVII(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  // rb->value_.i
  gcc_jit_lvalue *lhs = ravi_emit_load_reg_i(def, rb);
  // rc->value_.i
  gcc_jit_lvalue *rhs = ravi_emit_load_reg_i(def, rc);
  // result = (lua_Number)rb->value_.i / (lua_Number)rc->value_.i
  gcc_jit_rvalue *result = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_DIVIDE,
      def->ravi->types->lua_IntegerT,
      gcc_jit_context_new_cast(def->function_context, NULL,
                               gcc_jit_lvalue_as_rvalue(lhs),
                               def->ravi->types->lua_NumberT),
      gcc_jit_context_new_cast(def->function_context, NULL,
                               gcc_jit_lvalue_as_rvalue(rhs),
                               def->ravi->types->lua_NumberT));
  // ra->value_.i = result
  // ra->tt_ = LUA_TNUMINT
  ravi_emit_store_reg_i_withtype(def, result, ra);
}

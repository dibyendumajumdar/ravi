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

// OP_ADD, OP_SUB, OP_MUL and OP_DIV
void ravi_emit_ARITH(ravi_function_def_t *def, int A, int B, int C, OpCode op,
                     TMS tms, int pc) {

  // TValue *rb = RKB(i);
  // TValue *rc = RKC(i);
  // lua_Number nb; lua_Number nc;
  // if (ttisinteger(rb) && ttisinteger(rc)) {
  //  lua_Integer ib = ivalue(rb); lua_Integer ic = ivalue(rc);
  //  setivalue(ra, intop(+, ib, ic));
  //}
  // else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
  //  setfltvalue(ra, luai_numadd(L, nb, nc));
  //}
  // else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_ADD)); }

  gcc_jit_lvalue *nb = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->lua_NumberT,
      unique_name(def, "ARITH_nb", pc));
  gcc_jit_lvalue *nc = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->lua_NumberT,
      unique_name(def, "ARITH_nc", pc));

  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);

  gcc_jit_lvalue *rb_type = ravi_emit_load_type(def, rb);
  gcc_jit_lvalue *rc_type = ravi_emit_load_type(def, rc);

  gcc_jit_block *float_op = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "ARITH_float.op", pc));
  gcc_jit_block *try_meta = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "ARITH_try_meta", pc));
  gcc_jit_block *done_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "done", pc));

  if (op != OP_DIV) {
    gcc_jit_rvalue *cmp1 = ravi_emit_is_value_of_type(
        def, gcc_jit_lvalue_as_rvalue(rb_type), LUA__TNUMINT);
    gcc_jit_rvalue *cmp2 = ravi_emit_is_value_of_type(
        def, gcc_jit_lvalue_as_rvalue(rc_type), LUA__TNUMINT);

    gcc_jit_rvalue *andvalue = gcc_jit_context_new_binary_op(
        def->function_context, NULL, GCC_JIT_BINARY_OP_LOGICAL_AND,
        def->ravi->types->C_boolT, cmp1, cmp2);

    // Check if both RB and RC are integers
    gcc_jit_block *then_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "ARITH_if_integer", pc));
    gcc_jit_block *else_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "ARITH_if_not_integer", pc));
    ravi_emit_conditional_branch(def, andvalue, then_block, else_block);
    ravi_set_current_block(def, then_block);

    // Both are integers
    gcc_jit_lvalue *lhs = ravi_emit_load_reg_i(def, rb);
    gcc_jit_lvalue *rhs = ravi_emit_load_reg_i(def, rc);

    gcc_jit_rvalue *result = NULL;
    switch (op) {
    case OP_ADD:
      result = gcc_jit_context_new_binary_op(
          def->function_context, NULL, GCC_JIT_BINARY_OP_PLUS,
          def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(lhs),
          gcc_jit_lvalue_as_rvalue(rhs));
      break;
    case OP_SUB:
      result = gcc_jit_context_new_binary_op(
          def->function_context, NULL, GCC_JIT_BINARY_OP_MINUS,
          def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(lhs),
          gcc_jit_lvalue_as_rvalue(rhs));
      break;
    case OP_MUL:
      result = gcc_jit_context_new_binary_op(
          def->function_context, NULL, GCC_JIT_BINARY_OP_MULT,
          def->ravi->types->lua_IntegerT, gcc_jit_lvalue_as_rvalue(lhs),
          gcc_jit_lvalue_as_rvalue(rhs));
      break;
    default:
      lua_assert(0);
    }

    ravi_emit_store_reg_i_withtype(def, result, ra);

    ravi_emit_branch(def, done_block);

    // Not integer
    ravi_set_current_block(def, else_block);
  }

  // Is RB a float?
  gcc_jit_rvalue *cmp1 = ravi_emit_is_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(rb_type), LUA__TNUMFLT);

  gcc_jit_block *convert_rb = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "ARITH_convert_rb", pc));
  gcc_jit_block *test_rc = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "ARITH_test_rc", pc));
  gcc_jit_block *load_rb = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "ARITH_load_rb", pc));

  // If RB is floating then load RB, else convert RB
  ravi_emit_conditional_branch(def, cmp1, load_rb, convert_rb);

  // Convert RB
  ravi_set_current_block(def, convert_rb);

  // Call luaV_tonumber_()
  gcc_jit_rvalue *rb_isnum =
      ravi_function_call2_rvalue(def, def->ravi->types->luaV_tonumberT,
                                 gcc_jit_lvalue_get_address(rb, NULL),
                                 gcc_jit_lvalue_get_address(nb, NULL));
  cmp1 = ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ, rb_isnum,
                              ravi_int_constant(def, 1));

  // If not number then go to meta block
  // Else proceed to test RC
  ravi_emit_conditional_branch(def, cmp1, test_rc, try_meta);

  ravi_set_current_block(def, load_rb);

  // Copy RB to local nb
  gcc_jit_lvalue *src = ravi_emit_load_reg_n(def, rb);
  gcc_jit_block_add_assignment(def->current_block, NULL, nb,
                               gcc_jit_lvalue_as_rvalue(src));

  ravi_emit_branch(def, test_rc);

  ravi_set_current_block(def, test_rc);

  // Is RC a float?
  cmp1 = ravi_emit_is_value_of_type(def, gcc_jit_lvalue_as_rvalue(rc_type),
                                    LUA__TNUMFLT);

  gcc_jit_block *convert_rc = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "ARITH_convert_rc", pc));
  gcc_jit_block *load_rc = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "ARITH_load_rc", pc));

  // If RC is float load RC
  // else try to convert RC
  ravi_emit_conditional_branch(def, cmp1, load_rc, convert_rc);

  ravi_set_current_block(def, convert_rc);

  // Call luaV_tonumber_()
  gcc_jit_rvalue *rc_isnum =
      ravi_function_call2_rvalue(def, def->ravi->types->luaV_tonumberT,
                                 gcc_jit_lvalue_get_address(rc, NULL),
                                 gcc_jit_lvalue_get_address(nc, NULL));
  cmp1 = ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ, rc_isnum,
                              ravi_int_constant(def, 1));

  // If not number then go to meta block
  // else both RB and RC float so go to op
  ravi_emit_conditional_branch(def, cmp1, float_op, try_meta);

  ravi_set_current_block(def, load_rc);

  // Copy RC to local;
  src = ravi_emit_load_reg_n(def, rc);
  gcc_jit_block_add_assignment(def->current_block, NULL, nc,
                               gcc_jit_lvalue_as_rvalue(src));

  ravi_emit_branch(def, float_op);

  ravi_set_current_block(def, float_op);

  gcc_jit_lvalue *lhs = nb;
  gcc_jit_lvalue *rhs = nc;

  gcc_jit_rvalue *result = NULL;
  // Add and set RA
  switch (op) {
  case OP_ADD:
    result = gcc_jit_context_new_binary_op(
        def->function_context, NULL, GCC_JIT_BINARY_OP_PLUS,
        def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
        gcc_jit_lvalue_as_rvalue(rhs));
    break;
  case OP_SUB:
    result = gcc_jit_context_new_binary_op(
        def->function_context, NULL, GCC_JIT_BINARY_OP_MINUS,
        def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
        gcc_jit_lvalue_as_rvalue(rhs));
    break;
  case OP_MUL:
    result = gcc_jit_context_new_binary_op(
        def->function_context, NULL, GCC_JIT_BINARY_OP_MULT,
        def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
        gcc_jit_lvalue_as_rvalue(rhs));
    break;
  case OP_DIV:
    result = gcc_jit_context_new_binary_op(
        def->function_context, NULL, GCC_JIT_BINARY_OP_DIVIDE,
        def->ravi->types->lua_NumberT, gcc_jit_lvalue_as_rvalue(lhs),
        gcc_jit_lvalue_as_rvalue(rhs));
    break;
  default:
    lua_assert(0);
  }

  ravi_emit_store_reg_n_withtype(def, result, ra);

  ravi_emit_branch(def, done_block);

  // Neither integer nor float so try meta
  ravi_set_current_block(def, try_meta);

  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call5_rvalue(
          def, def->ravi->types->luaT_trybinTMT,
          gcc_jit_param_as_rvalue(def->L), gcc_jit_lvalue_get_address(rb, NULL),
          gcc_jit_lvalue_get_address(rc, NULL),
          gcc_jit_lvalue_get_address(ra, NULL), ravi_int_constant(def, tms)));
  ravi_emit_branch(def, done_block);

  ravi_set_current_block(def, done_block);
}

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

// implements EQ, LE and LT - by using the supplied lua function to call.
void ravi_emit_EQ_LE_LT(ravi_function_def_t *def, int A, int B, int C, int j,
                        int jA, gcc_jit_function *callee, const char *opname,
                        OpCode opCode, int pc) {
  //  case OP_EQ: {
  //    TValue *rb = RKB(i);
  //    TValue *rc = RKC(i);
  //    Protect(
  //      if (cast_int(luaV_equalobj(L, rb, rc)) != GETARG_A(i))
  //        ci->u.l.savedpc++;
  //      else
  //        donextjump(ci);
  //    )
  //  } break;

  // Load pointer to base
  ravi_emit_load_base(def);

  // Get pointer to register B
  gcc_jit_lvalue *regB = ravi_emit_get_register_or_constant(def, B);
  // Get pointer to register C
  gcc_jit_lvalue *regC = ravi_emit_get_register_or_constant(def, C);

  gcc_jit_rvalue *result = NULL;
  switch (opCode) {

  case OP_RAVI_LT_II:
  case OP_RAVI_LE_II:
  case OP_RAVI_EQ_II: {
    gcc_jit_lvalue *p1 = ravi_emit_load_reg_i(def, regB);
    gcc_jit_lvalue *p2 = ravi_emit_load_reg_i(def, regC);

    switch (opCode) {
    case OP_RAVI_EQ_II:
      result = ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ,
                                    gcc_jit_lvalue_as_rvalue(p1),
                                    gcc_jit_lvalue_as_rvalue(p2));
      break;
    case OP_RAVI_LT_II:
      result = ravi_emit_comparison(def, GCC_JIT_COMPARISON_LT,
                                    gcc_jit_lvalue_as_rvalue(p1),
                                    gcc_jit_lvalue_as_rvalue(p2));
      break;
    case OP_RAVI_LE_II:
      result = ravi_emit_comparison(def, GCC_JIT_COMPARISON_LE,
                                    gcc_jit_lvalue_as_rvalue(p1),
                                    gcc_jit_lvalue_as_rvalue(p2));
      break;
    default:
      assert(0);
    }
    result = gcc_jit_context_new_cast(def->function_context, NULL, result,
                                      def->ravi->types->C_intT);

  } break;

  case OP_RAVI_LT_FF:
  case OP_RAVI_LE_FF:
  case OP_RAVI_EQ_FF: {
    gcc_jit_lvalue *p1 = ravi_emit_load_reg_n(def, regB);
    gcc_jit_lvalue *p2 = ravi_emit_load_reg_n(def, regC);

    switch (opCode) {
    case OP_RAVI_EQ_FF:
      result = ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ,
                                    gcc_jit_lvalue_as_rvalue(p1),
                                    gcc_jit_lvalue_as_rvalue(p2));
      break;
    case OP_RAVI_LT_FF:
      result = ravi_emit_comparison(def, GCC_JIT_COMPARISON_LT,
                                    gcc_jit_lvalue_as_rvalue(p1),
                                    gcc_jit_lvalue_as_rvalue(p2));
      break;
    case OP_RAVI_LE_FF:
      result = ravi_emit_comparison(def, GCC_JIT_COMPARISON_LE,
                                    gcc_jit_lvalue_as_rvalue(p1),
                                    gcc_jit_lvalue_as_rvalue(p2));
      break;
    default:
      assert(0);
    }
    result = gcc_jit_context_new_cast(def->function_context, NULL, result,
                                      def->ravi->types->C_intT);

  } break;

  default:
    // Call luaV_equalobj with register B and C
    result =
        ravi_function_call3_rvalue(def, callee, gcc_jit_param_as_rvalue(def->L),
                                   gcc_jit_lvalue_get_address(regB, NULL),
                                   gcc_jit_lvalue_get_address(regC, NULL));
  }

  // Test if result is equal to operand A
  gcc_jit_rvalue *A_const = ravi_int_constant(def, A);
  gcc_jit_rvalue *result_eq_A =
      ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ, result, A_const);
  // If result == A then we need to execute the next statement which is a jump
  char temp[80];
  snprintf(temp, sizeof temp, "%s_then", opname);
  gcc_jit_block *then_block =
      gcc_jit_function_new_block(def->jit_function, unique_name(def, temp, pc));

  snprintf(temp, sizeof temp, "%s_else", opname);
  gcc_jit_block *else_block =
      gcc_jit_function_new_block(def->jit_function, unique_name(def, temp, pc));
  ravi_emit_conditional_branch(def, result_eq_A, then_block, else_block);
  ravi_set_current_block(def, then_block);

  // if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  if (jA > 0) {
    // jA is the A operand of the Jump instruction

    // Reload pointer to base as the call to luaV_equalobj() may
    // have invoked a Lua function and as a result the stack may have
    // been reallocated - so the previous base pointer could be stale
    ravi_emit_load_base(def);

    // base + a - 1
    gcc_jit_lvalue *val = ravi_emit_get_register(def, jA - 1);

    // Call luaF_close
    gcc_jit_block_add_eval(
        def->current_block, NULL,
        ravi_function_call2_rvalue(def, def->ravi->types->luaF_closeT,
                                   gcc_jit_param_as_rvalue(def->L),
                                   gcc_jit_lvalue_get_address(val, NULL)));
  }
  // Do the jump
  ravi_emit_branch(def, def->jmp_targets[j]->jmp);
  // Add the else block and make it current so that the next instruction flows
  // here
  ravi_set_current_block(def, else_block);
}

gcc_jit_rvalue *ravi_emit_boolean_testfalse(ravi_function_def_t *def,
                                            gcc_jit_lvalue *reg, bool negate) {
  // (isnil() || isbool() && b == 0)

  gcc_jit_lvalue *var = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->C_boolT,
      unique_name(def, "isfalse", 0));
  gcc_jit_lvalue *type = ravi_emit_load_type(def, reg);

  // Test if type == LUA_TNIL (0)
  gcc_jit_rvalue *isnil = ravi_emit_is_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(type), LUA__TNIL);

  // Test if type == LUA_TBOOLEAN
  gcc_jit_rvalue *isbool = ravi_emit_is_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(type), LUA__TBOOLEAN);

  // Test if bool value == 0
  gcc_jit_lvalue *bool_value = ravi_emit_load_reg_b(def, reg);
  gcc_jit_rvalue *zero = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, 0);
  gcc_jit_rvalue *boolzero = ravi_emit_comparison(
      def, GCC_JIT_COMPARISON_EQ, gcc_jit_lvalue_as_rvalue(bool_value), zero);

  // Test type == LUA_TBOOLEAN && bool value == 0
  gcc_jit_rvalue *andvalue = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_LOGICAL_AND,
      def->ravi->types->C_boolT, isbool, boolzero);

  gcc_jit_rvalue *orvalue = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_LOGICAL_OR,
      def->ravi->types->C_boolT, isnil, andvalue);

  gcc_jit_block_add_assignment(def->current_block, NULL, var, orvalue);

  gcc_jit_rvalue *result = NULL;
  if (negate) {
    result = gcc_jit_context_new_unary_op(
        def->function_context, NULL, GCC_JIT_UNARY_OP_LOGICAL_NEGATE,
        def->ravi->types->C_boolT, gcc_jit_lvalue_as_rvalue(var));
  } else {
    result = gcc_jit_lvalue_as_rvalue(var);
  }
  return result;
}

void ravi_emit_TEST(ravi_function_def_t *def, int A, int B, int C, int j,
                    int jA, int pc) {

  //    case OP_TEST: {
  //      if (GETARG_C(i) ? l_isfalse(ra) : !l_isfalse(ra))
  //        ci->u.l.savedpc++;
  //      else
  //        donextjump(ci);
  //    } break;

  (void)B;

  //  if (C) {
  //    ravi_debug_printf3(def, "OP_TEST(%d C=1)) if (!reg(A=%d)) then skip next
  //    else jmp to %d\n", ravi_int_constant(def, pc+1),
  //                       ravi_int_constant(def, A), ravi_int_constant(def,
  //                       j+1));
  //  }
  //  else {
  //    ravi_debug_printf3(def, "OP_TEST(%d C=0) if (reg(A=%d)) then skip next
  //    else jmp to %d\n", ravi_int_constant(def, pc+1),
  //                       ravi_int_constant(def, A), ravi_int_constant(def,
  //                       j+1));
  //  }

  // Load pointer to base
  ravi_emit_load_base(def);

  // Get pointer to register A
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  // v = C ? is_false(ra) : !is_false(ra)
  gcc_jit_rvalue *v = C ? ravi_emit_boolean_testfalse(def, ra, false)
                        : ravi_emit_boolean_testfalse(def, ra, true);

  // Test NOT v
  gcc_jit_rvalue *result = gcc_jit_context_new_unary_op(
      def->function_context, NULL, GCC_JIT_UNARY_OP_LOGICAL_NEGATE,
      def->ravi->types->C_boolT, v);
  // If !v then we need to execute the next statement which is a jump
  gcc_jit_block *then_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_TEST_do_jmp", pc));
  gcc_jit_block *else_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_TEST_do_skip", pc));
  ravi_emit_conditional_branch(def, result, then_block, else_block);
  ravi_set_current_block(def, then_block);

  // if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  if (jA > 0) {
    // jA is the A operand of the Jump instruction

    // base + a - 1
    gcc_jit_lvalue *val = ravi_emit_get_register(def, jA - 1);

    // Call luaF_close
    gcc_jit_block_add_eval(
        def->current_block, NULL,
        ravi_function_call2_rvalue(def, def->ravi->types->luaF_closeT,
                                   gcc_jit_param_as_rvalue(def->L),
                                   gcc_jit_lvalue_get_address(val, NULL)));
  }
  // Do the jump
  ravi_emit_branch(def, def->jmp_targets[j]->jmp);

  // Add the else block and make it current so that the next instruction flows
  // here
  ravi_set_current_block(def, else_block);
}

void ravi_emit_NOT(ravi_function_def_t *def, int A, int B, int pc) {
  //  case OP_NOT: {
  //    TValue *rb = RB(i);
  //    int res = l_isfalse(rb);  /* next assignment may change this value */
  //    setbvalue(ra, res);
  //  } break;
  (void)pc;

  ravi_emit_load_base(def);
  // Get pointer to register B
  gcc_jit_lvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_rvalue *v = ravi_emit_boolean_testfalse(def, rb, false);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  ravi_emit_store_reg_b_withtype(
      def, gcc_jit_context_new_cast(def->function_context, NULL, v,
                                    def->ravi->types->C_intT),
      ra);
}

void ravi_emit_TESTSET(ravi_function_def_t *def, int A, int B, int C, int j,
                       int jA, int pc) {

  //  case OP_TESTSET: {
  //    TValue *rb = RB(i);
  //    if (GETARG_C(i) ? l_isfalse(rb) : !l_isfalse(rb))
  //      ci->u.l.savedpc++;
  //    else {
  //      setobjs2s(L, ra, rb);
  //      donextjump(ci);
  //    }
  //  } break;

  (void)pc;
  // Load pointer to base
  ravi_emit_load_base(def);

  // Get pointer to register B
  gcc_jit_lvalue *rb = ravi_emit_get_register(def, B);
  // v = C ? is_false(ra) : !is_false(ra)
  gcc_jit_rvalue *v = C ? ravi_emit_boolean_testfalse(def, rb, false)
                        : ravi_emit_boolean_testfalse(def, rb, true);

  // Test NOT v
  gcc_jit_rvalue *result = gcc_jit_context_new_unary_op(
      def->function_context, NULL, GCC_JIT_UNARY_OP_LOGICAL_NEGATE,
      def->ravi->types->C_boolT, v);

  // If !v then we need to execute the next statement which is a jump
  gcc_jit_block *then_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_TESTSET_if_then", pc));
  gcc_jit_block *else_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_TESTSET_if_else", pc));
  ravi_emit_conditional_branch(def, result, then_block, else_block);
  ravi_set_current_block(def, then_block);

  // Get pointer to register A
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  ravi_emit_struct_assign(def, ra, rb);

  // if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  if (jA > 0) {
    // jA is the A operand of the Jump instruction

    // base + a - 1
    gcc_jit_lvalue *val = ravi_emit_get_register(def, jA - 1);

    // Call luaF_close
    gcc_jit_block_add_eval(
        def->current_block, NULL,
        ravi_function_call2_rvalue(def, def->ravi->types->luaF_closeT,
                                   gcc_jit_param_as_rvalue(def->L),
                                   gcc_jit_lvalue_get_address(val, NULL)));
  }
  // Do the jump
  ravi_emit_branch(def, def->jmp_targets[j]->jmp);

  // Add the else block and make it current so that the next instruction flows
  // here
  ravi_set_current_block(def, else_block);
}

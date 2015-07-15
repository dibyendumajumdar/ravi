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

// R(A+1), ..., R(A+B) := nil
void ravi_emit_LOADNIL(ravi_function_def_t *def, int A, int B, int pc) {
  (void)pc;

  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call3_rvalue(
          def, def->ravi->types->raviV_op_loadnilT,
          gcc_jit_lvalue_as_rvalue(def->ci_val),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, A),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, B)));
}

// R(A) := tonumber(0)
void ravi_emit_LOADFZ(ravi_function_def_t *def, int A, int pc) {
  (void)pc;

  // Load pointer to base
  ravi_emit_load_base(def);

  // ra
  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);

  // destvalue->value_.n = 0.0
  ravi_emit_store_reg_n_withtype(
      def, gcc_jit_context_new_rvalue_from_double(
               def->function_context, def->ravi->types->lua_NumberT, 0.0),
      dest);
}

// R(A) := tointeger(0)
void ravi_emit_LOADIZ(ravi_function_def_t *def, int A, int pc) {
  (void)pc;

  // Load pointer to base
  ravi_emit_load_base(def);

  // ra
  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);

  // destvalue->value_.i =
  ravi_emit_store_reg_i_withtype(
      def, gcc_jit_context_new_rvalue_from_int(
               def->function_context, def->ravi->types->lua_IntegerT, 0),
      dest);
}

void ravi_emit_LOADK(ravi_function_def_t *def, int A, int Bx, int pc) {

  (void)pc;

  // TValue *rb = k + GETARG_Bx(i);
  // setobj2s(L, ra, rb);

  // Load pointer to base
  ravi_emit_load_base(def);

  // ra
  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);

#if 1
  TValue *Konst = &def->p->k[Bx];
  switch (Konst->tt_) {
  case LUA_TNUMINT:
    ravi_emit_store_reg_i_withtype(
        def, gcc_jit_context_new_rvalue_from_int(def->function_context,
                                                 def->ravi->types->lua_IntegerT,
                                                 Konst->value_.i),
        dest);
    break;
  case LUA_TNUMFLT:
    ravi_emit_store_reg_n_withtype(def, gcc_jit_context_new_rvalue_from_double(
                                            def->function_context,
                                            def->ravi->types->lua_NumberT,
                                            Konst->value_.n),
                                   dest);
    break;
  case LUA_TBOOLEAN:
    ravi_emit_store_reg_b_withtype(
        def,
        gcc_jit_context_new_rvalue_from_int(
            def->function_context, def->ravi->types->C_intT, Konst->value_.b),
        dest);
    break;
  default: {
#endif
    // rb
    gcc_jit_lvalue *src = ravi_emit_get_constant(def, Bx);

    // *ra = *rb
    ravi_emit_struct_assign(def, dest, src);
#if 1
  }
  }
#endif
}

// R(A) := R(B)
void ravi_emit_MOVE(ravi_function_def_t *def, int A, int B) {
  // setobjs2s(L, ra, RB(i));

  lua_assert(A != B);

  // Load pointer to base
  ravi_emit_load_base(def);

  // rb
  gcc_jit_lvalue *src = ravi_emit_get_register(def, B);
  // ra
  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);

  // *ra = *rb
  ravi_emit_struct_assign(def, dest, src);
}

// R(A) := (Bool)B; if (C) pc++
void ravi_emit_LOADBOOL(ravi_function_def_t *def, int A, int B, int C, int j,
                        int pc) {

  // setbvalue(ra, GETARG_B(i));
  // if (GETARG_C(i)) ci->u.l.savedpc++;  /* skip next instruction (if C) */

  //  ravi_debug_printf4(def, "LOADBOOL(pc=%d) set reg(A=%d) to boolean(B=%d);
  //  if ((C=%d) != 0) skip next\n", ravi_int_constant(def, pc+1),
  //  ravi_int_constant(def, A), ravi_int_constant(def, B),
  //  ravi_int_constant(def, C));
  // Load pointer to base
  ravi_emit_load_base(def);
  // ra
  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);
  // dest->i = 0
  ravi_emit_store_reg_b_withtype(
      def, gcc_jit_context_new_rvalue_from_int(def->function_context,
                                               def->ravi->types->C_intT, B),
      dest);
  if (C) {
    // Skip next instruction if C
    ravi_emit_branch(def, def->jmp_targets[j]->jmp);

    gcc_jit_block *block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "OP_LOADBOOL_skip_next", pc));
    ravi_set_current_block(def, block);
  }
}

// R(A) := R(B), check R(B) is int
void ravi_emit_MOVEI(ravi_function_def_t *def, int A, int B, int pc) {

  // TValue *rb = RB(i);
  // lua_Integer j;
  // if (tointeger(rb, &j)) {
  //   setivalue(ra, j);
  // }
  // else
  //   luaG_runerror(L, "integer expected");

  gcc_jit_lvalue *var = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->lua_IntegerT,
      unique_name(def, "OP_RAVI_MOVEI_i", pc));

  // Load pointer to base
  ravi_emit_load_base(def);

  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *src = ravi_emit_get_register(def, B);

  gcc_jit_lvalue *src_type = ravi_emit_load_type(def, src);

  // Compare src->tt == LUA_TNUMINT
  gcc_jit_rvalue *cmp1 = ravi_emit_is_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(src_type), LUA__TNUMINT);

  gcc_jit_block *then1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RAVI_MOVEI_if_integer", pc));
  gcc_jit_block *else1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RAVI_MOVEI_if_not_integer", pc));
  gcc_jit_block *end1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RAVI_MOVEI_done", pc));

  ravi_emit_conditional_branch(def, cmp1, then1, else1);
  ravi_set_current_block(def, then1);

  // Already a int - move
  gcc_jit_lvalue *tmp = ravi_emit_load_reg_i(def, src);
  gcc_jit_block_add_assignment(def->current_block, NULL, var,
                               gcc_jit_lvalue_as_rvalue(tmp));
  ravi_emit_branch(def, end1);

  // we need to convert
  ravi_set_current_block(def, else1);

  // Call luaV_tointeger_()
  gcc_jit_rvalue *var_isint =
      ravi_function_call2_rvalue(def, def->ravi->types->luaV_tointegerT,
                                 gcc_jit_lvalue_get_address(src, NULL),
                                 gcc_jit_lvalue_get_address(var, NULL));
  gcc_jit_rvalue *zero = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, 0);
  gcc_jit_rvalue *tobool =
      ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ, var_isint, zero);

  // Did conversion fail?
  gcc_jit_block *else2 = gcc_jit_function_new_block(
      def->jit_function,
      unique_name(def, "OP_RAVI_MOVEI_if_conversion_failed", pc));
  ravi_emit_conditional_branch(def, tobool, else2, end1);

  // Conversion failed, so raise error
  ravi_set_current_block(def, else2);
  ravi_emit_raise_lua_error(def, "integer expected");

  ravi_emit_branch(def, end1);

  // Conversion OK
  ravi_set_current_block(def, end1);

  // Set R(A)
  ravi_emit_store_reg_i_withtype(def, gcc_jit_lvalue_as_rvalue(var), dest);
}

void ravi_emit_MOVEF(ravi_function_def_t *def, int A, int B, int pc) {

  //  case OP_RAVI_MOVEF: {
  //    TValue *rb = RB(i);
  //    lua_Number j;
  //    if (tonumber(rb, &j)) {
  //      setfltvalue(ra, j);
  //    }
  //    else
  //      luaG_runerror(L, "float expected");
  //  } break;

  gcc_jit_lvalue *var = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->lua_NumberT,
      unique_name(def, "OP_RAVI_MOVEF_n", pc));

  // Load pointer to base
  ravi_emit_load_base(def);

  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *src = ravi_emit_get_register(def, B);

  gcc_jit_lvalue *src_type = ravi_emit_load_type(def, src);

  // Compare src->tt == LUA_TNUMFLT
  gcc_jit_rvalue *cmp1 = ravi_emit_is_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(src_type), LUA__TNUMFLT);

  gcc_jit_block *then1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RAVI_MOVEF_if_float", pc));
  gcc_jit_block *else1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RAVI_MOVEF_if_not_float", pc));
  gcc_jit_block *end1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RAVI_MOVEF_done", pc));

  ravi_emit_conditional_branch(def, cmp1, then1, else1);
  ravi_set_current_block(def, then1);

  // Already a float - copy to var
  gcc_jit_lvalue *tmp = ravi_emit_load_reg_n(def, src);
  gcc_jit_block_add_assignment(def->current_block, NULL, var,
                               gcc_jit_lvalue_as_rvalue(tmp));
  ravi_emit_branch(def, end1);

  // we need to convert
  ravi_set_current_block(def, else1);

  // Call luaV_tonumber()
  gcc_jit_rvalue *var_isflt =
      ravi_function_call2_rvalue(def, def->ravi->types->luaV_tonumberT,
                                 gcc_jit_lvalue_get_address(src, NULL),
                                 gcc_jit_lvalue_get_address(var, NULL));
  gcc_jit_rvalue *zero = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, 0);
  gcc_jit_rvalue *tobool =
      ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ, var_isflt, zero);

  // Did conversion fail?
  gcc_jit_block *else2 = gcc_jit_function_new_block(
      def->jit_function,
      unique_name(def, "OP_RAVI_MOVEF_if_conversion_failed", pc));
  ravi_emit_conditional_branch(def, tobool, else2, end1);

  // Conversion failed, so raise error
  ravi_set_current_block(def, else2);
  ravi_emit_raise_lua_error(def, "number expected");

  ravi_emit_branch(def, end1);

  // Conversion OK
  ravi_set_current_block(def, end1);

  // Set R(A)
  ravi_emit_store_reg_n_withtype(def, gcc_jit_lvalue_as_rvalue(var), dest);
}

void ravi_emit_TOINT(ravi_function_def_t *def, int A, int pc) {

  //  case OP_RAVI_TOINT: {
  //    lua_Integer j;
  //    if (tointeger(ra, &j)) {
  //      setivalue(ra, j);
  //    }
  //    else
  //      luaG_runerror(L, "integer expected");
  //  } break;

  gcc_jit_lvalue *var = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->lua_IntegerT,
      unique_name(def, "OP_RAVI_TOINT_i", pc));

  // Load pointer to base
  ravi_emit_load_base(def);

  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *src = dest;

  gcc_jit_lvalue *src_type = ravi_emit_load_type(def, src);

  // Is src->tt != LUA_TNUMINT?
  gcc_jit_rvalue *cmp1 = ravi_emit_is_not_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(src_type), LUA__TNUMINT);

  gcc_jit_block *then1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RAVI_TOINT_if_not_integer", pc));
  gcc_jit_block *end1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RAVI_TOINT_done", pc));

  ravi_emit_conditional_branch(def, cmp1, then1, end1);
  ravi_set_current_block(def, then1);

  // Call luaV_tointeger_()
  gcc_jit_rvalue *var_isint =
      ravi_function_call2_rvalue(def, def->ravi->types->luaV_tointegerT,
                                 gcc_jit_lvalue_get_address(src, NULL),
                                 gcc_jit_lvalue_get_address(var, NULL));
  gcc_jit_rvalue *zero = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, 0);
  gcc_jit_rvalue *failed_conversion =
      ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ, var_isint, zero);

  // Did conversion fail?
  gcc_jit_block *then2 = gcc_jit_function_new_block(
      def->jit_function,
      unique_name(def, "OP_RAVI_TOINT_if_conversion_failed", pc));
  gcc_jit_block *else2 = gcc_jit_function_new_block(
      def->jit_function,
      unique_name(def, "OP_RAVI_TOINT_if_conversion_ok", pc));
  ravi_emit_conditional_branch(def, failed_conversion, then2, else2);

  ravi_set_current_block(def, then2);

  // Conversion failed, so raise error
  ravi_emit_raise_lua_error(def, "integer expected");

  ravi_emit_branch(def, else2);

  // Conversion OK
  ravi_set_current_block(def, else2);

  ravi_emit_store_reg_i_withtype(def, gcc_jit_lvalue_as_rvalue(var), dest);

  ravi_emit_branch(def, end1);

  ravi_set_current_block(def, end1);
}

void ravi_emit_TOFLT(ravi_function_def_t *def, int A, int pc) {

  //  case OP_RAVI_TOFLT: {
  //    lua_Number j;
  //    if (tonumber(ra, &j)) {
  //      setfltvalue(ra, j);
  //    }
  //    else
  //      luaG_runerror(L, "float expected");
  //  } break;

  gcc_jit_lvalue *var = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->lua_NumberT,
      unique_name(def, "OP_RAVI_TOFLT_n", pc));

  // Load pointer to base
  ravi_emit_load_base(def);

  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *src = dest;

  gcc_jit_lvalue *src_type = ravi_emit_load_type(def, src);

  // Is src->tt != LUA_TNUMFLT?
  gcc_jit_rvalue *cmp1 = ravi_emit_is_not_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(src_type), LUA__TNUMFLT);

  gcc_jit_block *then1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RAVI_TOFLT_if_not_float", pc));
  gcc_jit_block *end1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_RAVI_TOFLT_done", pc));

  ravi_emit_conditional_branch(def, cmp1, then1, end1);
  ravi_set_current_block(def, then1);

  // Call luaV_tonumber()
  gcc_jit_rvalue *var_ptr = gcc_jit_lvalue_get_address(var, NULL);
  // ravi_debug_printf3(def, "number %p = %f before call to luaV_number\n",
  // var_ptr, gcc_jit_lvalue_as_rvalue(var));
  gcc_jit_rvalue *var_isflt = ravi_function_call2_rvalue(
      def, def->ravi->types->luaV_tonumberT,
      gcc_jit_lvalue_get_address(src, NULL), var_ptr);
  gcc_jit_rvalue *zero = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, 0);
  gcc_jit_rvalue *failed_conversion =
      ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ, var_isflt, zero);

  // Did conversion fail?
  gcc_jit_block *then2 = gcc_jit_function_new_block(
      def->jit_function,
      unique_name(def, "OP_RAVI_TOFLT_if_conversion_failed", pc));
  gcc_jit_block *else2 = gcc_jit_function_new_block(
      def->jit_function,
      unique_name(def, "OP_RAVI_TOFLT_if_conversion_ok", pc));
  ravi_emit_conditional_branch(def, failed_conversion, then2, else2);

  ravi_set_current_block(def, then2);

  // Conversion failed, so raise error
  ravi_emit_raise_lua_error(def, "number expected");

  ravi_emit_branch(def, else2);

  // Conversion OK
  ravi_set_current_block(def, else2);

  // ravi_debug_printf2(def, "number ok = %f\n", gcc_jit_lvalue_as_rvalue(var));
  ravi_emit_store_reg_n_withtype(def, gcc_jit_lvalue_as_rvalue(var), dest);

  ravi_emit_branch(def, end1);

  ravi_set_current_block(def, end1);
}

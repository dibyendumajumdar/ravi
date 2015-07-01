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

// R(A) := UpValue[B][RK(C)]
void ravi_emit_GETTABUP(ravi_function_def_t *def, int A, int B, int C, int pc) {
  // int b = GETARG_B(i);
  // Protect(luaV_gettable(L, cl->upvals[b]->v, RKC(i), ra));

  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_rvalue *rc = ravi_emit_get_register_or_constant(def, C);

  gcc_jit_rvalue *upval = ravi_emit_get_upvals(def, B);
  gcc_jit_lvalue *v = ravi_emit_get_upval_v(def, upval);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, def->ravi->types->luaV_gettableT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_as_rvalue(v), rc, ra));
}

// R(A+1) := R(B); R(A) := R(B)[RK(C)]
void ravi_emit_SELF(ravi_function_def_t *def, int A, int B, int C, int pc) {
  // StkId rb = RB(i);
  // setobjs2s(L, ra + 1, rb);
  // Protect(luaV_gettable(L, rb, RKC(i), ra));
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_rvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_rvalue *ra1 = ravi_emit_get_register(def, A + 1);
  ravi_emit_struct_assign(def, ra1, rb);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_rvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, def->ravi->types->luaV_gettableT,
                                 gcc_jit_param_as_rvalue(def->L), rb, rc, ra));
}

// R(A) := length of R(B)
void ravi_emit_LEN(ravi_function_def_t *def, int A, int B, int pc) {
  // Protect(luaV_objlen(L, ra, RB(i)));
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_rvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call3_rvalue(def, def->ravi->types->luaV_objlenT,
                                 gcc_jit_param_as_rvalue(def->L), ra, rb));
}

// R(A)[RK(B)] := RK(C)
void ravi_emit_SETTABLE(ravi_function_def_t *def, int A, int B, int C, int pc) {
  // Protect(luaV_settable(L, ra, RKB(i), RKC(i)));
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_rvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_rvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, def->ravi->types->luaV_settableT,
                                 gcc_jit_param_as_rvalue(def->L), ra, rb, rc));
}

// R(A) := R(B)[RK(C)]
void ravi_emit_GETTABLE(ravi_function_def_t *def, int A, int B, int C, int pc) {
  // Protect(luaV_gettable(L, RB(i), RKC(i), ra));
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_rvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_rvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, def->ravi->types->luaV_gettableT,
                                 gcc_jit_param_as_rvalue(def->L), rb, rc, ra));
}

void ravi_emit_NEWTABLE(ravi_function_def_t *def, int A, int B, int C, int pc) {
  //  case OP_NEWTABLE: {
  //    int b = GETARG_B(i);
  //    int c = GETARG_C(i);
  //    Table *t = luaH_new(L);
  //    sethvalue(L, ra, t);
  //    if (b != 0 || c != 0)
  //      luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
  //    checkGC(L, ra + 1);
  //  } break;
  (void)pc;

  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call5_rvalue(
          def, def->ravi->types->raviV_op_newtableT,
          gcc_jit_param_as_rvalue(def->L),
          gcc_jit_lvalue_as_rvalue(def->ci_val), ra,
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, B),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, C)));
}

void ravi_emit_NEWARRAYINT(ravi_function_def_t *def, int A, int pc) {
  (void)pc;

  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call3_rvalue(def, def->ravi->types->raviV_op_newarrayintT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_as_rvalue(def->ci_val), ra));
}

void ravi_emit_NEWARRAYFLOAT(ravi_function_def_t *def, int A, int pc) {
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call3_rvalue(def, def->ravi->types->raviV_op_newarrayfloatT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_as_rvalue(def->ci_val), ra));
}

void ravi_emit_SETLIST(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;

  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call5_rvalue(
          def, def->ravi->types->raviV_op_setlistT,
          gcc_jit_param_as_rvalue(def->L),
          gcc_jit_lvalue_as_rvalue(def->ci_val), ra,
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, B),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, C)));
}

void ravi_emit_GETTABLE_AF(ravi_function_def_t *def, int A, int B, int C,
                           int pc) {
  //#define raviH_get_float_inline(L, t, key, v)
  //{ unsigned ukey = (unsigned)((key));
  //  lua_Number *data = (lua_Number *)t->ravi_array.data;
  //  if (ukey < t->ravi_array.len) {
  //    setfltvalue(v, data[ukey]);
  //      }else
  //    luaG_runerror(L, "array out of bounds");
  //}

  // TValue *rb = RB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rc);
  // Table *t = hvalue(rb);
  // raviH_get_float_inline(L, t, idx, ra);

  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_rvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_rvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_lvalue *key = ravi_emit_load_reg_i(def, rc);
  gcc_jit_rvalue *t = ravi_emit_load_reg_h(def, rb);
  gcc_jit_rvalue *data = ravi_emit_load_reg_h_floatarray(def, t);
  gcc_jit_lvalue *len = ravi_emit_load_ravi_arraylength(def, t);
  gcc_jit_rvalue *ukey = gcc_jit_context_new_cast(
      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(key),
      def->ravi->types->C_unsigned_intT);

  gcc_jit_rvalue *cmp = ravi_emit_comparison(def, GCC_JIT_COMPARISON_LT, ukey,
                                             gcc_jit_lvalue_as_rvalue(len));
  gcc_jit_block *then_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "GETTABLE_AF_if_in_range", pc));
  gcc_jit_block *else_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "GETTABLE_AF_if_not_in_range", pc));
  gcc_jit_block *end_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "GETTABLE_AF_if_end", pc));
  ravi_emit_conditional_branch(def, cmp, then_block, else_block);
  ravi_set_current_block(def, then_block);

  gcc_jit_rvalue *value = ravi_emit_array_get(def, data, ukey);

  ravi_emit_store_reg_n_withtype(def, value, ra);
  ravi_emit_branch(def, end_block);

  ravi_set_current_block(def, else_block);

  ravi_emit_raise_lua_error(def, "array out of bounds");
  ravi_emit_branch(def, end_block);

  ravi_set_current_block(def, end_block);
}

void ravi_emit_GETTABLE_AI(ravi_function_def_t *def, int A, int B, int C,
                           int pc) {
  //#define raviH_get_int_inline(L, t, key, v)
  //{ unsigned ukey = (unsigned)((key));
  //  lua_Integer *data = (lua_Integer *)t->ravi_array.data;
  //  if (ukey < t->ravi_array.len) {
  //    setivalue(v, data[ukey]);
  //      } else
  //    luaG_runerror(L, "array out of bounds");
  //}

  // TValue *rb = RB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rc);
  // Table *t = hvalue(rb);
  // raviH_get_int_inline(L, t, idx, ra);

  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_rvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_rvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_lvalue *key = ravi_emit_load_reg_i(def, rc);
  gcc_jit_rvalue *t = ravi_emit_load_reg_h(def, rb);
  gcc_jit_rvalue *data = ravi_emit_load_reg_h_intarray(def, t);
  gcc_jit_lvalue *len = ravi_emit_load_ravi_arraylength(def, t);
  gcc_jit_rvalue *ukey = gcc_jit_context_new_cast(
      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(key),
      def->ravi->types->C_unsigned_intT);

  gcc_jit_rvalue *cmp = ravi_emit_comparison(def, GCC_JIT_COMPARISON_LT, ukey,
                                             gcc_jit_lvalue_as_rvalue(len));
  gcc_jit_block *then_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "GETTABLE_AI_if_in_range", pc));
  gcc_jit_block *else_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "GETTABLE_AI_if_not_in_range", pc));
  gcc_jit_block *end_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "GETTABLE_AI_if_end", pc));
  ravi_emit_conditional_branch(def, cmp, then_block, else_block);
  ravi_set_current_block(def, then_block);

  gcc_jit_rvalue *value = ravi_emit_array_get(def, data, ukey);

  ravi_emit_store_reg_i_withtype(def, value, ra);
  ravi_emit_branch(def, end_block);

  ravi_set_current_block(def, else_block);

  ravi_emit_raise_lua_error(def, "array out of bounds");
  ravi_emit_branch(def, end_block);

  ravi_set_current_block(def, end_block);
}

void ravi_emit_TOARRAY(ravi_function_def_t *def, int A, int array_type_expected,
                       const char *errmsg, int pc) {

  // if (!ttistable(ra) || hvalue(ra)->ravi_array.type != RAVI_TARRAYINT)
  //  luaG_runerror(L, "integer[] expected");

  ravi_emit_load_base(def);
  gcc_jit_rvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *type = ravi_emit_load_type(def, ra);

  // type != LUA_TTABLE ?
  gcc_jit_rvalue *cmp1 = ravi_emit_is_not_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(type), LUA__TTABLE);

  gcc_jit_block *raise_error = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "TOARRAY_if_not_table", pc));
  gcc_jit_block *else1 = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "TOARRAY_test_if_array", pc));
  gcc_jit_block *done = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "TOARRAY_done", pc));
  ravi_emit_conditional_branch(def, cmp1, raise_error, else1);
  ravi_set_current_block(def, raise_error);

  // Conversion failed, so raise error
  ravi_emit_raise_lua_error(def, errmsg);
  ravi_emit_branch(def, done);

  ravi_set_current_block(def, else1);

  // Get table
  gcc_jit_rvalue *h = ravi_emit_load_reg_h(def, ra);
  // Get array type
  gcc_jit_rvalue *ravi_array_type =
      gcc_jit_lvalue_as_rvalue(ravi_emit_load_ravi_arraytype(def, h));

  // array_type == RAVI_TARRAYXXX?
  gcc_jit_rvalue *expected = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->lu_byteT, array_type_expected);
  gcc_jit_rvalue *cmp2 = ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ,
                                              ravi_array_type, expected);

  // If array then fine else raise error
  ravi_emit_conditional_branch(def, cmp2, done, raise_error);

  ravi_set_current_block(def, done);
}

void ravi_emit_MOVEAI(ravi_function_def_t *def, int A, int B, int pc) {
  ravi_emit_TOARRAY(def, B, RAVI_TARRAYINT, "integer[] expected", pc);
  gcc_jit_rvalue *src = ravi_emit_get_register(def, B);
  gcc_jit_rvalue *dest = ravi_emit_get_register(def, A);
  ravi_emit_struct_assign(def, dest, src);
}

void ravi_emit_MOVEAF(ravi_function_def_t *def, int A, int B, int pc) {
  ravi_emit_TOARRAY(def, B, RAVI_TARRAYFLT, "number[] expected", pc);
  gcc_jit_rvalue *src = ravi_emit_get_register(def, B);
  gcc_jit_rvalue *dest = ravi_emit_get_register(def, A);
  ravi_emit_struct_assign(def, dest, src);
}

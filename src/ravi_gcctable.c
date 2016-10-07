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

// R(A+1) := R(B); R(A) := R(B)[RK(C)]
void ravi_emit_SELF(ravi_function_def_t *def, int A, int B, int C, int pc) {
  // StkId rb = RB(i);
  // setobjs2s(L, ra + 1, rb);
  // Protect(luaV_gettable(L, rb, RKC(i), ra));
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_lvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_lvalue *ra1 = ravi_emit_get_register(def, A + 1);
  ravi_emit_struct_assign(def, ra1, rb);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, def->ravi->types->luaV_gettableT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_get_address(rb, NULL),
                                 gcc_jit_lvalue_get_address(rc, NULL),
                                 gcc_jit_lvalue_get_address(ra, NULL)));
}

// R(A) := length of R(B)
void ravi_emit_LEN(ravi_function_def_t *def, int A, int B, int pc) {
  // Protect(luaV_objlen(L, ra, RB(i)));
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call3_rvalue(def, def->ravi->types->luaV_objlenT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_get_address(ra, NULL),
                                 gcc_jit_lvalue_get_address(rb, NULL)));
}

// R(A)[RK(B)] := RK(C)
void ravi_emit_SETTABLE(ravi_function_def_t *def, int A, int B, int C, int pc) {
  // Protect(luaV_settable(L, ra, RKB(i), RKC(i)));
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, def->ravi->types->luaV_settableT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_get_address(ra, NULL),
                                 gcc_jit_lvalue_get_address(rb, NULL),
                                 gcc_jit_lvalue_get_address(rc, NULL)));
}

// R(A) := R(B)[RK(C)]
void ravi_emit_GETTABLE(ravi_function_def_t *def, int A, int B, int C, int pc) {
  // Protect(luaV_gettable(L, RB(i), RKC(i), ra));
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, def->ravi->types->luaV_gettableT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_get_address(rb, NULL),
                                 gcc_jit_lvalue_get_address(rc, NULL),
                                 gcc_jit_lvalue_get_address(ra, NULL)));
}

void ravi_emit_GETTABLE_AF(ravi_function_def_t *def, int A, int B, int C,
                           int pc, bool omitArrayGetRangeCheck) {
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
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_lvalue *key = ravi_emit_load_reg_i(def, rc);
  gcc_jit_rvalue *t = ravi_emit_load_reg_h(def, rb);
  gcc_jit_rvalue *data = ravi_emit_load_reg_h_floatarray(def, t);
  gcc_jit_lvalue *len = ravi_emit_load_ravi_arraylength(def, t);
  gcc_jit_rvalue *ukey = gcc_jit_context_new_cast(
      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(key),
      def->ravi->types->lua_UnsignedT);

  gcc_jit_block *then_block = NULL;
  gcc_jit_block *else_block = NULL;
  gcc_jit_block *end_block = NULL;

  if (omitArrayGetRangeCheck) {
    gcc_jit_rvalue *ulen = gcc_jit_context_new_cast(
        def->function_context, NULL, gcc_jit_lvalue_as_rvalue(len),
        def->ravi->types->lua_UnsignedT);

    gcc_jit_rvalue *cmp =
        ravi_emit_comparison(def, GCC_JIT_COMPARISON_LT, ukey, ulen);
    then_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "GETTABLE_AF_if_in_range", pc));
    else_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "GETTABLE_AF_if_not_in_range", pc));
    end_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "GETTABLE_AF_if_end", pc));
    ravi_emit_conditional_branch(def, cmp, then_block, else_block);
    ravi_set_current_block(def, then_block);
  }
  gcc_jit_rvalue *value = ravi_emit_array_get(def, data, ukey);
  ravi_emit_store_reg_n_withtype(def, value, ra);

  if (omitArrayGetRangeCheck) {
    ravi_emit_branch(def, end_block);

    ravi_set_current_block(def, else_block);

    ravi_emit_raise_lua_error(def, "array out of bounds");
    ravi_emit_branch(def, end_block);

    ravi_set_current_block(def, end_block);
  }
}

void ravi_emit_GETTABLE_AI(ravi_function_def_t *def, int A, int B, int C,
                           int pc, bool omitArrayGetRangeCheck) {
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
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_lvalue *key = ravi_emit_load_reg_i(def, rc);
  gcc_jit_rvalue *t = ravi_emit_load_reg_h(def, rb);
  gcc_jit_rvalue *data = ravi_emit_load_reg_h_intarray(def, t);
  gcc_jit_lvalue *len = ravi_emit_load_ravi_arraylength(def, t);
  gcc_jit_rvalue *ukey = gcc_jit_context_new_cast(
      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(key),
      def->ravi->types->lua_UnsignedT);

  gcc_jit_block *then_block = NULL;
  gcc_jit_block *else_block = NULL;
  gcc_jit_block *end_block = NULL;

  if (omitArrayGetRangeCheck) {
    gcc_jit_rvalue *ulen = gcc_jit_context_new_cast(
        def->function_context, NULL, gcc_jit_lvalue_as_rvalue(len),
        def->ravi->types->lua_UnsignedT);

    gcc_jit_rvalue *cmp =
        ravi_emit_comparison(def, GCC_JIT_COMPARISON_LT, ukey, ulen);
    then_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "GETTABLE_AI_if_in_range", pc));
    else_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "GETTABLE_AI_if_not_in_range", pc));
    end_block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "GETTABLE_AI_if_end", pc));
    ravi_emit_conditional_branch(def, cmp, then_block, else_block);
    ravi_set_current_block(def, then_block);
  }
  gcc_jit_rvalue *value = ravi_emit_array_get(def, data, ukey);

  ravi_emit_store_reg_i_withtype(def, value, ra);
  if (omitArrayGetRangeCheck) {
    ravi_emit_branch(def, end_block);

    ravi_set_current_block(def, else_block);

    ravi_emit_raise_lua_error(def, "array out of bounds");
    ravi_emit_branch(def, end_block);

    ravi_set_current_block(def, end_block);
  }
}

void ravi_emit_SETTABLE_AI_AF(ravi_function_def_t *def, int A, int B, int C,
                              bool known_tt, lua_typecode_t tt, int pc) {

  //#define raviH_set_int_inline(L, t, key, value)
  //{ unsigned ukey = (unsigned)((key));
  //  lua_Integer *data = (lua_Integer *)t->ravi_array.data;
  //  if (ukey < t->ravi_array.len) {
  //    data[ukey] = value;
  //      } else
  //    raviH_set_int(L, t, ukey, value);
  //}

  // Table *t = hvalue(ra);
  // TValue *rb = RKB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rb);
  // lua_Integer value = ivalue(rc);
  // raviH_set_int_inline(L, t, idx, value);

  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);
  gcc_jit_lvalue *key = ravi_emit_load_reg_i(def, rb);
  gcc_jit_lvalue *value = NULL;

  switch (tt) {
  case LUA__TNUMINT:
    value = known_tt ? ravi_emit_load_reg_i(def, rc)
                     : ravi_emit_tonumtype(def, rc, LUA__TNUMINT, pc);
    break;
  case LUA__TNUMFLT:
    value = known_tt ? ravi_emit_load_reg_n(def, rc)
                     : ravi_emit_tonumtype(def, rc, LUA__TNUMFLT, pc);
    break;
  default:
    assert(false);
    abort();
  }

  gcc_jit_rvalue *t = ravi_emit_load_reg_h(def, ra);
  gcc_jit_rvalue *data = NULL;
  switch (tt) {
  case LUA__TNUMINT:
    data = ravi_emit_load_reg_h_intarray(def, t);
    break;
  case LUA__TNUMFLT:
    data = ravi_emit_load_reg_h_floatarray(def, t);
    break;
  default:
    assert(false);
    abort();
  }

  gcc_jit_lvalue *len = ravi_emit_load_ravi_arraylength(def, t);
  gcc_jit_rvalue *ukey = gcc_jit_context_new_cast(
      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(key),
      def->ravi->types->lua_UnsignedT);
  gcc_jit_rvalue *ulen = gcc_jit_context_new_cast(
      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(len),
      def->ravi->types->lua_UnsignedT);

  gcc_jit_rvalue *cmp =
      ravi_emit_comparison(def, GCC_JIT_COMPARISON_LT, ukey, ulen);
  gcc_jit_block *then_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "SETTABLE_AX_if_in_range", pc));
  gcc_jit_block *else_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "SETTABLE_AX_if_not_in_range", pc));
  gcc_jit_block *end_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "SETTABLE_AX_if_end", pc));
  ravi_emit_conditional_branch(def, cmp, then_block, else_block);
  ravi_set_current_block(def, then_block);

  gcc_jit_lvalue *ptr = ravi_emit_array_get_ptr(def, data, ukey);

  gcc_jit_block_add_assignment(def->current_block, NULL, ptr,
                               gcc_jit_lvalue_as_rvalue(value));
  ravi_emit_branch(def, end_block);

  ravi_set_current_block(def, else_block);

  gcc_jit_function *f = NULL;
  switch (tt) {
  case LUA__TNUMINT:
    f = def->ravi->types->raviH_set_intT;
    break;
  case LUA__TNUMFLT:
    f = def->ravi->types->raviH_set_floatT;
    break;
  default:
    assert(false);
    abort();
  }
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, f, gcc_jit_param_as_rvalue(def->L), t,
                                 ukey, gcc_jit_lvalue_as_rvalue(value)));
  ravi_emit_branch(def, end_block);

  ravi_set_current_block(def, end_block);
}

// R(A) := UpValue[B]
void ravi_emit_GETUPVAL(ravi_function_def_t *def, int A, int B, int pc) {
  // int b = GETARG_B(i);
  // setobj2s(L, ra, cl->upvals[b]->v);
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_rvalue *upval = ravi_emit_get_upvals(def, B);
  gcc_jit_lvalue *v = ravi_emit_load_upval_v(def, upval);
  ravi_emit_struct_assign(
      def, ra, gcc_jit_rvalue_dereference(gcc_jit_lvalue_as_rvalue(v), NULL));
}

// UpValue[B] := R(A)
void ravi_emit_SETUPVAL(ravi_function_def_t *def, int A, int B, int pc) {
#if 1
  // Work around libgccjit compilation failure
  // The inline version causes segmentation fault during compilation
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, def->ravi->types->raviV_op_setupvalT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_as_rvalue(def->lua_closure_val),
                                 gcc_jit_lvalue_get_address(ra, NULL),
                                 ravi_int_constant(def, B)));
#else
  // UpVal *uv = cl->upvals[GETARG_B(i)];
  // setobj(L, uv->v, ra);
  // luaC_upvalbarrier(L, uv);

  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_rvalue *upval = ravi_emit_get_upvals(def, B);
  gcc_jit_lvalue *v = ravi_emit_load_upval_v(def, upval);
  ravi_emit_struct_assign(def, v, ra);

  gcc_jit_lvalue *type = ravi_emit_load_type(def, v);

  // (type & BIT_ISCOLLECTIBLE) != 0
  gcc_jit_rvalue *bit_iscollectible = ravi_int_constant(def, BIT_ISCOLLECTABLE);
  gcc_jit_rvalue *is_collectible_bit = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_BITWISE_AND,
      def->ravi->types->C_intT, gcc_jit_lvalue_as_rvalue(type),
      bit_iscollectible);
  gcc_jit_rvalue *zero = ravi_int_constant(def, 0);
  gcc_jit_rvalue *is_collectible = ravi_emit_comparison(
      def, GCC_JIT_COMPARISON_NE, is_collectible_bit, zero);

  // Is upvalue closed?
  // (up->v == &up->u.value)
  gcc_jit_lvalue *value = ravi_emit_load_upval_value(def, upval);
  gcc_jit_rvalue *upisclosed = ravi_emit_comparison(
      def, GCC_JIT_COMPARISON_EQ, gcc_jit_lvalue_as_rvalue(v),
      gcc_jit_lvalue_get_address(value, NULL));

  // Collectible type and upvalue is closed
  // ((type & BIT_ISCOLLECTIBLE) != 0) && ((up)->v == &(up)->u.value))
  gcc_jit_rvalue *andcond = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_LOGICAL_AND,
      def->ravi->types->C_boolT, is_collectible, upisclosed);

  gcc_jit_block *then = gcc_jit_function_new_block(
      def->jit_function,
      unique_name(def, "SETUPVAL_if_collectible_and_upval_is_closed", pc));
  gcc_jit_block *end = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "SETUPVAL_if_end", pc));

  ravi_emit_conditional_branch(def, andcond, then, end);
  ravi_set_current_block(def, then);

  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call2_rvalue(def, def->ravi->types->luaC_upvalbarrierT,
                                 gcc_jit_param_as_rvalue(def->L), upval));
  ravi_emit_branch(def, end);

  ravi_set_current_block(def, end);
#endif
}

// UpValue[A][RK(B)] := RK(C)
void ravi_emit_SETTABUP(ravi_function_def_t *def, int A, int B, int C, int pc) {

  // int a = GETARG_A(i);
  // Protect(luaV_settable(L, cl->upvals[a]->v, RKB(i), RKC(i)));

  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_lvalue *rb = ravi_emit_get_register_or_constant(def, B);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);

  gcc_jit_rvalue *upval = ravi_emit_get_upvals(def, A);
  gcc_jit_lvalue *v = ravi_emit_load_upval_v(def, upval);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, def->ravi->types->luaV_settableT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_as_rvalue(v),
                                 gcc_jit_lvalue_get_address(rb, NULL),
                                 gcc_jit_lvalue_get_address(rc, NULL)));
}

// R(A) := UpValue[B][RK(C)]
void ravi_emit_GETTABUP(ravi_function_def_t *def, int A, int B, int C, int pc) {
  // int b = GETARG_B(i);
  // Protect(luaV_gettable(L, cl->upvals[b]->v, RKC(i), ra));

  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *rc = ravi_emit_get_register_or_constant(def, C);

  gcc_jit_rvalue *upval = ravi_emit_get_upvals(def, B);
  gcc_jit_lvalue *v = ravi_emit_load_upval_v(def, upval);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(def, def->ravi->types->luaV_gettableT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_as_rvalue(v),
                                 gcc_jit_lvalue_get_address(rc, NULL),
                                 gcc_jit_lvalue_get_address(ra, NULL)));
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
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call5_rvalue(
          def, def->ravi->types->raviV_op_newtableT,
          gcc_jit_param_as_rvalue(def->L),
          gcc_jit_lvalue_as_rvalue(def->ci_val),
          gcc_jit_lvalue_get_address(ra, NULL),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, B),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, C)));
}

void ravi_emit_NEWARRAYINT(ravi_function_def_t *def, int A, int pc) {
  (void)pc;

  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call3_rvalue(def, def->ravi->types->raviV_op_newarrayintT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_as_rvalue(def->ci_val),
                                 gcc_jit_lvalue_get_address(ra, NULL)));
}

void ravi_emit_NEWARRAYFLOAT(ravi_function_def_t *def, int A, int pc) {
  (void)pc;
  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call3_rvalue(def, def->ravi->types->raviV_op_newarrayfloatT,
                                 gcc_jit_param_as_rvalue(def->L),
                                 gcc_jit_lvalue_as_rvalue(def->ci_val),
                                 gcc_jit_lvalue_get_address(ra, NULL)));
}

void ravi_emit_SETLIST(ravi_function_def_t *def, int A, int B, int C, int pc) {
  (void)pc;

  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call5_rvalue(
          def, def->ravi->types->raviV_op_setlistT,
          gcc_jit_param_as_rvalue(def->L),
          gcc_jit_lvalue_as_rvalue(def->ci_val),
          gcc_jit_lvalue_get_address(ra, NULL),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, B),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, C)));
}

void ravi_emit_TOARRAY(ravi_function_def_t *def, int A, int array_type_expected,
                       const char *errmsg, int pc) {

  // if (!ttistable(ra) || hvalue(ra)->ravi_array.type != RAVI_TARRAYINT)
  //  luaG_runerror(L, "integer[] expected");
  OpCode op = OP_RAVI_TOTAB;
  lua_typecode_t expected_type = RAVI__TLTABLE;
  switch (array_type_expected) {
    case RAVI_TARRAYINT:
      op = OP_RAVI_TOARRAYI;
      expected_type = RAVI__TIARRAY;
      break;
    case RAVI_TARRAYFLT:
      op = OP_RAVI_TOARRAYF;
      expected_type = RAVI__TFARRAY;
      break;
    case RAVI_TTABLE:
    default: break;
  }

  ravi_emit_load_base(def);
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *type = ravi_emit_load_type(def, ra);

  // type != expected_type ?
  gcc_jit_rvalue *cmp1 = ravi_emit_is_not_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(type), expected_type);

  gcc_jit_block *raise_error = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "TOARRAY_if_not_table", pc));
  gcc_jit_block *done = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "TOARRAY_done", pc));
  ravi_emit_conditional_branch(def, cmp1, raise_error, done);
  ravi_set_current_block(def, raise_error);

  // Conversion failed, so raise error
  ravi_emit_raise_lua_error(def, errmsg);
  ravi_emit_branch(def, done);

  ravi_set_current_block(def, done);
}

void ravi_emit_MOVEAI(ravi_function_def_t *def, int A, int B, int pc) {
  ravi_emit_TOARRAY(def, B, RAVI_TARRAYINT, "integer[] expected", pc);
  gcc_jit_lvalue *src = ravi_emit_get_register(def, B);
  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);
  ravi_emit_struct_assign(def, dest, src);
}

void ravi_emit_MOVEAF(ravi_function_def_t *def, int A, int B, int pc) {
  ravi_emit_TOARRAY(def, B, RAVI_TARRAYFLT, "number[] expected", pc);
  gcc_jit_lvalue *src = ravi_emit_get_register(def, B);
  gcc_jit_lvalue *dest = ravi_emit_get_register(def, A);
  ravi_emit_struct_assign(def, dest, src);
}

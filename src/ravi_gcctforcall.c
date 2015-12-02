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

void ravi_emit_TFORCALL(ravi_function_def_t *def, int A, int B, int C, int j,
                        int jA, int pc) {

  //  case OP_TFORCALL: {
  //    StkId cb = ra + 3;  /* call base */
  //    setobjs2s(L, cb + 2, ra + 2);
  //    setobjs2s(L, cb + 1, ra + 1);
  //    setobjs2s(L, cb, ra);
  //    L->top = cb + 3;  /* func. + 2 args (state and index) */
  //    Protect(luaD_call(L, cb, GETARG_C(i), 1));
  //    L->top = ci->top;
  //    i = *(ci->u.l.savedpc++);  /* go to next instruction */
  //    ra = RA(i);
  //    lua_assert(GET_OPCODE(i) == OP_TFORLOOP);
  //    goto l_tforloop;
  //  }
  //  case OP_TFORLOOP: {
  //  l_tforloop:
  //    if (!ttisnil(ra + 1)) {  /* continue loop? */
  //      setobjs2s(L, ra, ra + 1);  /* save control variable */
  //      ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    }
  //  } break;
  (void)B;

  // Load pointer to base
  ravi_emit_load_base(def);

  // Get pointer to register A
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *ra1 = ravi_emit_get_register(def, A + 1);
  gcc_jit_lvalue *ra2 = ravi_emit_get_register(def, A + 2);
  gcc_jit_lvalue *cb = ravi_emit_get_register(def, A + 3);
  gcc_jit_lvalue *cb1 = ravi_emit_get_register(def, A + 4);
  gcc_jit_lvalue *cb2 = ravi_emit_get_register(def, A + 5);

  ravi_emit_struct_assign(def, cb2, ra2);
  ravi_emit_struct_assign(def, cb1, ra1);
  ravi_emit_struct_assign(def, cb, ra);

  // L->top = cb + 3;  /* func. + 2 args (state and index) */
  ravi_emit_set_L_top_toreg(def, A + 6);

  // Protect(luaD_call(L, cb, GETARG_C(i)));
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call3_rvalue(
          def, def->ravi->types->luaD_callT, gcc_jit_param_as_rvalue(def->L),
          gcc_jit_lvalue_get_address(cb, NULL),
          gcc_jit_context_new_rvalue_from_int(def->function_context,
                                              def->ravi->types->C_intT, C)));
  // reload base
  ravi_emit_load_base(def);
  // L->top = ci->top;
  ravi_emit_refresh_L_top(def);
  ra = ravi_emit_get_register(def, jA);
  ra1 = ravi_emit_get_register(def, jA + 1);
  gcc_jit_lvalue *type = ravi_emit_load_type(def, ra1);

  // Test if type != LUA_TNIL (0)
  gcc_jit_rvalue *isnotnil = ravi_emit_is_not_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(type), LUA__TNIL);
  gcc_jit_block *then_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_TFORCALL_if_not_nil", pc));
  gcc_jit_block *else_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_TFORCALL_if_nil", pc));
  ravi_emit_conditional_branch(def, isnotnil, then_block, else_block);

  ravi_set_current_block(def, then_block);

  ravi_emit_struct_assign(def, ra, ra1);
  // Do the jump
  ravi_emit_branch(def, def->jmp_targets[j]->jmp);

  // Add the else block and make it current so that the next instruction flows
  // here
  ravi_set_current_block(def, else_block);
}

void ravi_emit_TFORLOOP(ravi_function_def_t *def, int A, int j, int pc) {
  //  case OP_TFORLOOP: {
  //  l_tforloop:
  //    if (!ttisnil(ra + 1)) {  /* continue loop? */
  //      setobjs2s(L, ra, ra + 1);  /* save control variable */
  //      ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    }
  //  } break;

  // Load pointer to base
  ravi_emit_load_base(def);

  // Get pointer to register A
  gcc_jit_lvalue *ra = ravi_emit_get_register(def, A);
  gcc_jit_lvalue *ra1 = ravi_emit_get_register(def, A + 1);
  gcc_jit_lvalue *type = ravi_emit_load_type(def, ra1);

  // Test if type != LUA_TNIL (0)
  gcc_jit_rvalue *isnotnil = ravi_emit_is_not_value_of_type(
      def, gcc_jit_lvalue_as_rvalue(type), LUA__TNIL);
  gcc_jit_block *then_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_TFORLOOP_if_not_nil", pc));
  gcc_jit_block *else_block = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "OP_TFORLOOP_if_nil", pc));
  ravi_emit_conditional_branch(def, isnotnil, then_block, else_block);

  ravi_set_current_block(def, then_block);

  ravi_emit_struct_assign(def, ra, ra1);
  // Do the jump
  ravi_emit_branch(def, def->jmp_targets[j]->jmp);

  // Add the else block and make it current so that the next instruction flows
  // here
  ravi_set_current_block(def, else_block);
}
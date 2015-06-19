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
void ravi_emit_LOADNIL(ravi_function_def_t *def, int A, int B) {
  ravi_function_call3_rvalue(def, def->ravi->types->raviV_op_loadnilT, gcc_jit_lvalue_as_rvalue(def->ci_val),
                             gcc_jit_context_new_rvalue_from_int(def->function_context, def->ravi->types->C_intT, A),
                             gcc_jit_context_new_rvalue_from_int(def->function_context, def->ravi->types->C_intT, B));
}

// R(A) := tonumber(0)
void ravi_emit_LOADFZ(ravi_function_def_t *def, int A) {

  // Load pointer to base
  ravi_emit_refresh_base(def);

  // ra
  gcc_jit_rvalue *dest = ravi_emit_get_register(def, A);

  // destvalue->n = 0.0
  ravi_emit_store_reg_n_withtype(def, dest,
                                 gcc_jit_context_new_rvalue_from_double(def->function_context,
                                                                        def->ravi->types->lua_NumberT, 0.0));
}


void ravi_emit_LOADK(ravi_function_def_t *def, int A, int Bx, int pc) {

  (void)pc;

  // TValue *rb = k + GETARG_Bx(i);
  // setobj2s(L, ra, rb);

  // Load pointer to base
  ravi_emit_refresh_base(def);

  // ra
  gcc_jit_rvalue *dest = ravi_emit_get_register(def, A);
  // rb
  gcc_jit_rvalue *src = ravi_emit_get_constant(def, Bx);

  // *ra = *rb
  ravi_emit_struct_assign(def, dest, src);
}

// R(A) := R(B)
void ravi_emit_MOVE(ravi_function_def_t *def, int A, int B) {
  // setobjs2s(L, ra, RB(i));

  lua_assert(A != B);

  // Load pointer to base
  ravi_emit_refresh_base(def);

  // rb
  gcc_jit_rvalue *src = ravi_emit_get_register(def, B);
  // ra
  gcc_jit_rvalue *dest = ravi_emit_get_register(def, A);

  // *ra = *rb
  ravi_emit_struct_assign(def, dest, src);
}

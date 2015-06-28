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
  gcc_jit_block_add_eval(def->current_block, NULL,
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
  gcc_jit_block_add_eval(def->current_block, NULL,
                         ravi_function_call3_rvalue(def, def->ravi->types->luaV_objlenT,
                                                    gcc_jit_param_as_rvalue(def->L), ra, rb));
}

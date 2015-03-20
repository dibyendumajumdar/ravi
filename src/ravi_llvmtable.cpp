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
#include "ravi_llvmcodegen.h"

namespace ravi {

// R(A)[RK(B)] := RK(C)
void RaviCodeGenerator::emit_SETTABLE(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  def->builder->CreateCall4(def->luaV_settableF, def->L, ra, rb, rc);
}

// R(A) := R(B)[RK(C)]
void RaviCodeGenerator::emit_GETTABLE(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_ra(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  def->builder->CreateCall4(def->luaV_gettableF, def->L, rb, rc, ra);
}

// R(A) := UpValue[B]
void RaviCodeGenerator::emit_GETUPVAL(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B) {
  //  case OP_GETUPVAL: {
  //    int b = GETARG_B(i);
  //    setobj2s(L, ra, cl->upvals[b]->v);
  //  } break;
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *upval_ptr = emit_gep_upvals(def, def->p_LClosure, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  emit_assign(def, ra, v);
}

// R(A) := UpValue[B][RK(C)]
void RaviCodeGenerator::emit_GETTABUP(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {
  //  case OP_GETTABUP: {
  //    int b = GETARG_B(i);
  //    Protect(luaV_gettable(L, cl->upvals[b]->v, RKC(i), ra));
  //  } break;
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);

  llvm::Value *upval_ptr = emit_gep_upvals(def, def->p_LClosure, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  def->builder->CreateCall4(def->luaV_gettableF, def->L, v, rc, ra);
}

void RaviCodeGenerator::emit_NEWARRAYINT(RaviFunctionDef *def,
                                         llvm::Value *L_ci, llvm::Value *proto,
                                         int A) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  def->builder->CreateCall3(def->luaV_newarrayintF, def->L, def->ci_val, ra);
}

void RaviCodeGenerator::emit_NEWARRAYFLOAT(RaviFunctionDef *def,
                                           llvm::Value *L_ci,
                                           llvm::Value *proto, int A) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  def->builder->CreateCall3(def->luaV_newarrayfloatF, def->L, def->ci_val, ra);
}

void RaviCodeGenerator::emit_NEWTABLE(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {
  //  case OP_NEWTABLE: {
  //    int b = GETARG_B(i);
  //    int c = GETARG_C(i);
  //    Table *t = luaH_new(L);
  //    sethvalue(L, ra, t);
  //    if (b != 0 || c != 0)
  //      luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
  //    checkGC(L, ra + 1);
  //  } break;

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  def->builder->CreateCall5(def->luaV_newtableF, def->L, def->ci_val, ra,
                            def->types->kInt[B], def->types->kInt[C]);
}

void RaviCodeGenerator::emit_SETLIST(RaviFunctionDef *def, llvm::Value *L_ci,
                                     llvm::Value *proto, int A, int B, int C) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  def->builder->CreateCall5(def->luaV_setlistF, def->L, def->ci_val, ra,
                            def->types->kInt[B], def->types->kInt[C]);
}
}
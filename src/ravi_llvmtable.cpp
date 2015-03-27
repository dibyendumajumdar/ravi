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

void RaviCodeGenerator::emit_SELF(RaviFunctionDef *def, llvm::Value *L_ci,
                                  llvm::Value *proto, int A, int B, int C) {
  // StkId rb = RB(i);
  // setobjs2s(L, ra + 1, rb);
  // Protect(luaV_gettable(L, rb, RKC(i), ra));
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *rb = emit_gep_ra(def, base_ptr, B);
  llvm::Value *ra1 = emit_gep_ra(def, base_ptr, A + 1);
  emit_assign(def, ra1, rb);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  def->builder->CreateCall4(def->luaV_gettableF, def->L, rb, rc, ra);
}

void RaviCodeGenerator::emit_LEN(RaviFunctionDef *def, llvm::Value *L_ci,
                                 llvm::Value *proto, int A, int B) {
  //Protect(luaV_objlen(L, ra, RB(i)));
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_ra(def, base_ptr, B);
  def->builder->CreateCall3(def->luaV_objlenF, def->L, ra, rb);
}

// R(A)[RK(B)] := RK(C)
void RaviCodeGenerator::emit_SETTABLE(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {
  //Protect(luaV_settable(L, ra, RKB(i), RKC(i)));
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  def->builder->CreateCall4(def->luaV_settableF, def->L, ra, rb, rc);
}

// R(A) := R(B)[RK(C)]
void RaviCodeGenerator::emit_GETTABLE(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {
  //Protect(luaV_gettable(L, RB(i), RKC(i), ra));
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_ra(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  def->builder->CreateCall4(def->luaV_gettableF, def->L, rb, rc, ra);
}

// R(A) := UpValue[B]
void RaviCodeGenerator::emit_GETUPVAL(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B) {
  //int b = GETARG_B(i);
  //setobj2s(L, ra, cl->upvals[b]->v);
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *upval_ptr = emit_gep_upvals(def, def->p_LClosure, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  emit_assign(def, ra, v);
}

// UpValue[B] := R(A)
void RaviCodeGenerator::emit_SETUPVAL(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B) {

  // UpVal *uv = cl->upvals[GETARG_B(i)];
  // setobj(L, uv->v, ra);
  // luaC_upvalbarrier(L, uv);

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *upval_ptr = emit_gep_upvals(def, def->p_LClosure, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  emit_assign(def, v, ra);

  llvm::Value *type = emit_load_type(def, v);
  llvm::Value *is_collectible =
      def->builder->CreateAnd(type, def->types->kInt[BIT_ISCOLLECTABLE]);

  llvm::Value *value = emit_gep_upval_value(def, upval);
  llvm::Value *cmp = def->builder->CreateICmpNE(v, value, "v.ne.value");
  llvm::Value *tobool = def->builder->CreateICmpEQ(
      is_collectible, def->types->kInt[0], "not.collectible");
  llvm::Value *orcond =
      def->builder->CreateOr(cmp, tobool, "v.ne.value.or.not.collectible");

  llvm::BasicBlock *then =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *end =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");

  def->builder->CreateCondBr(orcond, end, then);
  def->builder->SetInsertPoint(then);

  def->builder->CreateCall2(def->luaC_upvalbarrierF, def->L, upval);
  def->builder->CreateBr(end);

  def->f->getBasicBlockList().push_back(end);
  def->builder->SetInsertPoint(end);
}

// R(A) := UpValue[B][RK(C)]
void RaviCodeGenerator::emit_GETTABUP(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {
  //int b = GETARG_B(i);
  //Protect(luaV_gettable(L, cl->upvals[b]->v, RKC(i), ra));
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);

  llvm::Value *upval_ptr = emit_gep_upvals(def, def->p_LClosure, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  def->builder->CreateCall4(def->luaV_gettableF, def->L, v, rc, ra);
}

// UpValue[A][RK(B)] := RK(C)
void RaviCodeGenerator::emit_SETTABUP(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {

  // int a = GETARG_A(i);
  // Protect(luaV_settable(L, cl->upvals[a]->v, RKB(i), RKC(i)));

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);

  llvm::Value *upval_ptr = emit_gep_upvals(def, def->p_LClosure, A);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  def->builder->CreateCall4(def->luaV_settableF, def->L, v, rb, rc);
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
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

// R(A+1) := R(B); R(A) := R(B)[RK(C)]
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
  CreateCall4(def->builder, def->luaV_gettableF, def->L, rb, rc, ra);
}

// R(A) := length of R(B)
void RaviCodeGenerator::emit_LEN(RaviFunctionDef *def, llvm::Value *L_ci,
                                 llvm::Value *proto, int A, int B) {
  // Protect(luaV_objlen(L, ra, RB(i)));
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_ra(def, base_ptr, B);
  CreateCall3(def->builder, def->luaV_objlenF, def->L, ra, rb);
}

// R(A)[RK(B)] := RK(C)
void RaviCodeGenerator::emit_SETTABLE(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {
  // Protect(luaV_settable(L, ra, RKB(i), RKC(i)));
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  CreateCall4(def->builder, def->luaV_settableF, def->L, ra, rb, rc);
}

// R(A) := R(B)[RK(C)]
void RaviCodeGenerator::emit_GETTABLE(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {
  // Protect(luaV_gettable(L, RB(i), RKC(i), ra));
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_ra(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  CreateCall4(def->builder, def->luaV_gettableF, def->L, rb, rc, ra);
}

void RaviCodeGenerator::emit_GETTABLE_AF(RaviFunctionDef *def,
                                         llvm::Value *L_ci, llvm::Value *proto,
                                         int A, int B, int C) {
  //#define raviH_get_float_inline(L, t, key, v) \
  //{ unsigned ukey = (unsigned)((key)); \
  //  lua_Number *data = (lua_Number *)t->ravi_array.data; \
  //  if (ukey < t->ravi_array.len) {\
  //    setfltvalue(v, data[ukey]); \
  //      }else \
  //    luaG_runerror(L, "array out of bounds"); \
  //}

  // TValue *rb = RB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rc);
  // Table *t = hvalue(rb);
  // raviH_get_float_inline(L, t, idx, ra);

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_ra(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  llvm::Instruction *key = emit_load_reg_i(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, rb);
  llvm::Instruction *data = emit_load_reg_h_floatarray(def, t);
  llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
  llvm::Value *ukey = def->builder->CreateTrunc(key, def->types->C_intT);

  llvm::Value *cmp = def->builder->CreateICmpULT(ukey, len);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.in.range", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");
  def->builder->CreateCondBr(cmp, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  llvm::Value *ptr = def->builder->CreateGEP(data, ukey);
  llvm::Instruction *value = def->builder->CreateLoad(ptr);
  // TODO tbaa

  emit_store_reg_n(def, value, ra);
  emit_store_type(def, ra, LUA_TNUMFLT);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  emit_raise_lua_error(def, "array out of bounds");
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}

void RaviCodeGenerator::emit_GETTABLE_AI(RaviFunctionDef *def,
                                         llvm::Value *L_ci, llvm::Value *proto,
                                         int A, int B, int C) {

  //#define raviH_get_int_inline(L, t, key, v) \
  //{ unsigned ukey = (unsigned)((key)); \
  //  lua_Integer *data = (lua_Integer *)t->ravi_array.data; \
  //  if (ukey < t->ravi_array.len) {\
  //    setivalue(v, data[ukey]); \
  //      } else \
  //    luaG_runerror(L, "array out of bounds"); \
  //}

  // TValue *rb = RB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rc);
  // Table *t = hvalue(rb);
  // raviH_get_int_inline(L, t, idx, ra);

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_ra(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  llvm::Instruction *key = emit_load_reg_i(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, rb);
  llvm::Instruction *data = emit_load_reg_h_intarray(def, t);
  llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
  llvm::Value *ukey = def->builder->CreateTrunc(key, def->types->C_intT);

  llvm::Value *cmp = def->builder->CreateICmpULT(ukey, len);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.in.range", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");
  def->builder->CreateCondBr(cmp, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  llvm::Value *ptr = def->builder->CreateGEP(data, ukey);
  llvm::Instruction *value = def->builder->CreateLoad(ptr);
  // TODO tbaa

  emit_store_reg_i(def, value, ra);
  emit_store_type(def, ra, LUA_TNUMINT);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  emit_raise_lua_error(def, "array out of bounds");
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}

void RaviCodeGenerator::emit_SETTABLE_AI(RaviFunctionDef *def,
                                         llvm::Value *L_ci, llvm::Value *proto,
                                         int A, int B, int C) {

  //#define raviH_set_int_inline(L, t, key, value) \
  //{ unsigned ukey = (unsigned)((key)); \
  //  lua_Integer *data = (lua_Integer *)t->ravi_array.data; \
  //  if (ukey < t->ravi_array.len) { \
  //    data[ukey] = value; \
  //      } else \
  //    raviH_set_int(L, t, ukey, value); \
  //}

  // Table *t = hvalue(ra);
  // TValue *rb = RKB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rb);
  // lua_Integer value = ivalue(rc);
  // raviH_set_int_inline(L, t, idx, value);

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  llvm::Instruction *key = emit_load_reg_i(def, rb);
  llvm::Instruction *value = emit_load_reg_i(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, ra);
  llvm::Instruction *data = emit_load_reg_h_intarray(def, t);
  llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
  llvm::Value *ukey = def->builder->CreateTrunc(key, def->types->C_intT);

  llvm::Value *cmp = def->builder->CreateICmpULT(ukey, len);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.in.range", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");
  def->builder->CreateCondBr(cmp, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  llvm::Value *ptr = def->builder->CreateGEP(data, ukey);

  def->builder->CreateStore(value, ptr);
  // TODO tbaa
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  CreateCall4(def->builder, def->raviH_set_intF, def->L, t, ukey, value);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}

void RaviCodeGenerator::emit_SETTABLE_AF(RaviFunctionDef *def,
                                         llvm::Value *L_ci, llvm::Value *proto,
                                         int A, int B, int C) {

  //#define raviH_set_float_inline(L, t, key, value) \
  //{ unsigned ukey = (unsigned)((key)); \
  //  lua_Number *data = (lua_Number *)t->ravi_array.data; \
  //  if (ukey < t->ravi_array.len) { \
  //    data[ukey] = value; \
  //      } else \
  //    raviH_set_float(L, t, ukey, value); \
  //  }

  // Table *t = hvalue(ra);
  // TValue *rb = RKB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rb);
  // if (ttisfloat(rc)) {
  //  raviH_set_float_inline(L, t, idx, fltvalue(rc));
  //}
  // else {
  //  raviH_set_float_inline(L, t, idx, ((lua_Number)ivalue(rc)));
  //}

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *nc = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nc");

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  llvm::Instruction *key = emit_load_reg_i(def, rb);

  llvm::Instruction *rc_type = emit_load_type(def, rc);

  // Is RC a float?
  llvm::Value *cmp = def->builder->CreateICmpEQ(
      rc_type, def->types->kInt[LUA_TNUMFLT], "rc.is.float");

  llvm::BasicBlock *convert_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.rc");
  llvm::BasicBlock *load_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "load.rc");
  llvm::BasicBlock *set_af =
      llvm::BasicBlock::Create(def->jitState->context(), "set.af");

  // If RC is floating then load RC, else convert RC
  def->builder->CreateCondBr(cmp, load_rc, convert_rc);

  // Load RC
  def->f->getBasicBlockList().push_back(load_rc);
  def->builder->SetInsertPoint(load_rc);

  // Copy RC to local
  auto src = emit_load_reg_n(def, rc);
  emit_store_local_n(def, src, nc);
  def->builder->CreateBr(set_af);

  // Convert int to float
  def->f->getBasicBlockList().push_back(convert_rc);
  def->builder->SetInsertPoint(convert_rc);

  llvm::Instruction *ivalue = emit_load_reg_i(def, rc);
  llvm::Value *floatvalue =
      def->builder->CreateSIToFP(ivalue, def->types->lua_NumberT);
  emit_store_local_n(def, floatvalue, nc);
  def->builder->CreateBr(set_af);

  def->f->getBasicBlockList().push_back(set_af);
  def->builder->SetInsertPoint(set_af);

  llvm::Instruction *load_nc = emit_load_local_n(def, nc);

  llvm::Instruction *t = emit_load_reg_h(def, ra);
  llvm::Instruction *data = emit_load_reg_h_floatarray(def, t);
  llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
  llvm::Value *ukey = def->builder->CreateTrunc(key, def->types->C_intT);

  cmp = def->builder->CreateICmpULT(ukey, len);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.in.range", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");
  def->builder->CreateCondBr(cmp, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  llvm::Value *ptr = def->builder->CreateGEP(data, ukey);

  def->builder->CreateStore(load_nc, ptr);
  // TODO tbaa
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  CreateCall4(def->builder, def->raviH_set_floatF, def->L, t, ukey, load_nc);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}

// R(A) := UpValue[B]
void RaviCodeGenerator::emit_GETUPVAL(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B) {
  // int b = GETARG_B(i);
  // setobj2s(L, ra, cl->upvals[b]->v);
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

  CreateCall2(def->builder, def->luaC_upvalbarrierF, def->L, upval);
  def->builder->CreateBr(end);

  def->f->getBasicBlockList().push_back(end);
  def->builder->SetInsertPoint(end);
}

// R(A) := UpValue[B][RK(C)]
void RaviCodeGenerator::emit_GETTABUP(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int B, int C) {
  // int b = GETARG_B(i);
  // Protect(luaV_gettable(L, cl->upvals[b]->v, RKC(i), ra));
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);

  llvm::Value *upval_ptr = emit_gep_upvals(def, def->p_LClosure, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  CreateCall4(def->builder, def->luaV_gettableF, def->L, v, rc, ra);
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
  CreateCall4(def->builder, def->luaV_settableF, def->L, v, rb, rc);
}

void RaviCodeGenerator::emit_NEWARRAYINT(RaviFunctionDef *def,
                                         llvm::Value *L_ci, llvm::Value *proto,
                                         int A) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  CreateCall3(def->builder, def->raviV_op_newarrayintF, def->L, def->ci_val,
              ra);
}

void RaviCodeGenerator::emit_NEWARRAYFLOAT(RaviFunctionDef *def,
                                           llvm::Value *L_ci,
                                           llvm::Value *proto, int A) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  CreateCall3(def->builder, def->raviV_op_newarrayfloatF, def->L, def->ci_val,
              ra);
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
  CreateCall5(def->builder, def->raviV_op_newtableF, def->L, def->ci_val, ra,
              def->types->kInt[B], def->types->kInt[C]);
}

void RaviCodeGenerator::emit_SETLIST(RaviFunctionDef *def, llvm::Value *L_ci,
                                     llvm::Value *proto, int A, int B, int C) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  CreateCall5(def->builder, def->raviV_op_setlistF, def->L, def->ci_val, ra,
              def->types->kInt[B], def->types->kInt[C]);
}

llvm::Instruction *RaviCodeGenerator::emit_TOARRAY(RaviFunctionDef *def,
                                                   llvm::Value *L_ci,
                                                   llvm::Value *proto, int A,
                                                   int array_type_expected,
                                                   const char *errmsg) {

  // if (!ttistable(ra) || hvalue(ra)->ravi_array.type != RAVI_TARRAYINT)
  //  luaG_runerror(L, "integer[] expected");

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Instruction *type = emit_load_type(def, ra);

  // type != LUA_TTABLE ?
  llvm::Value *cmp1 = def->builder->CreateICmpNE(
      type, def->types->kInt[LUA_TTABLE | BIT_ISCOLLECTABLE], "is.not.table");

  llvm::BasicBlock *raise_error = llvm::BasicBlock::Create(
      def->jitState->context(), "if.not.table", def->f);
  llvm::BasicBlock *else1 =
      llvm::BasicBlock::Create(def->jitState->context(), "test.if.array");
  llvm::BasicBlock *done =
      llvm::BasicBlock::Create(def->jitState->context(), "done");
  def->builder->CreateCondBr(cmp1, raise_error, else1);
  def->builder->SetInsertPoint(raise_error);

  // Conversion failed, so raise error
  emit_raise_lua_error(def, errmsg);
  def->builder->CreateBr(done);

  def->f->getBasicBlockList().push_back(else1);
  def->builder->SetInsertPoint(else1);

  // Get table
  llvm::Instruction *h = emit_load_reg_h(def, ra);
  // Get array type
  llvm::Instruction *ravi_array_type = emit_load_ravi_arraytype(def, h);

  // array_type == RAVI_TARRAYXXX?
  llvm::Value *cmp2 = def->builder->CreateICmpEQ(
      ravi_array_type, def->types->kByte[array_type_expected], "is.array.type");

  // If array then fine else raise error
  def->builder->CreateCondBr(cmp2, done, raise_error);

  def->f->getBasicBlockList().push_back(done);
  def->builder->SetInsertPoint(done);

  return base_ptr;
}

void RaviCodeGenerator::emit_MOVEAI(RaviFunctionDef *def, llvm::Value *L_ci,
                                    llvm::Value *proto, int A, int B) {
  llvm::Instruction *base_ptr =
      emit_TOARRAY(def, L_ci, proto, B, RAVI_TARRAYINT, "integer[] expected");
  llvm::Value *src = emit_gep_ra(def, base_ptr, B);
  llvm::Value *dest = emit_gep_ra(def, base_ptr, A);
  emit_assign(def, dest, src);
}

void RaviCodeGenerator::emit_MOVEAF(RaviFunctionDef *def, llvm::Value *L_ci,
                                    llvm::Value *proto, int A, int B) {
  llvm::Instruction *base_ptr =
      emit_TOARRAY(def, L_ci, proto, B, RAVI_TARRAYFLT, "number[] expected");
  llvm::Value *src = emit_gep_ra(def, base_ptr, B);
  llvm::Value *dest = emit_gep_ra(def, base_ptr, A);
  emit_assign(def, dest, src);
}
}
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
void RaviCodeGenerator::emit_SELF(RaviFunctionDef *def, int A, int B, int C,
                                  int pc) {
  // StkId rb = RB(i);
  // setobjs2s(L, ra + 1, rb);
  // Protect(luaV_gettable(L, rb, RKC(i), ra));
  bool traced = emit_debug_trace(def, OP_SELF, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *ra1 = emit_gep_register(def, A + 1);
  emit_assign(def, ra1, rb);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->luaV_gettableF, def->L, rb, rc, ra);
}

// R(A) := length of R(B)
void RaviCodeGenerator::emit_LEN(RaviFunctionDef *def, int A, int B, int pc) {
  // Protect(luaV_objlen(L, ra, RB(i)));
  bool traced = emit_debug_trace(def, OP_LEN, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register(def, B);
  CreateCall3(def->builder, def->luaV_objlenF, def->L, ra, rb);
}

// R(A)[RK(B)] := RK(C)
void RaviCodeGenerator::emit_SETTABLE(RaviFunctionDef *def, int A, int B, int C,
                                      int pc) {
  // Protect(luaV_settable(L, ra, RKB(i), RKC(i)));
  bool traced = emit_debug_trace(def, OP_SETTABLE, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->luaV_settableF, def->L, ra, rb, rc);
}

// R(A) := R(B)[RK(C)]
void RaviCodeGenerator::emit_GETTABLE(RaviFunctionDef *def, int A, int B, int C,
                                      int pc) {
  // Protect(luaV_gettable(L, RB(i), RKC(i), ra));
  bool traced = emit_debug_trace(def, OP_GETTABLE, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->luaV_gettableF, def->L, rb, rc, ra);
}

void RaviCodeGenerator::emit_GETTABLE_AF(RaviFunctionDef *def, int A, int B,
                                         int C, bool omitArrayGetRangeCheck,
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

  emit_debug_trace(def, OP_RAVI_GETTABLE_AF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *key = emit_load_reg_i(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, rb);
  llvm::Instruction *data = emit_load_reg_h_floatarray(def, t);
  llvm::BasicBlock *then_block = nullptr;
  llvm::BasicBlock *else_block = nullptr;
  llvm::BasicBlock *end_block = nullptr;
  if (!omitArrayGetRangeCheck) {
    llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
    llvm::Value *ulen =
        def->builder->CreateZExt(len, def->types->lua_UnsignedT, "ulen");

    llvm::Value *cmp = def->builder->CreateICmpULT(key, ulen);
    then_block = llvm::BasicBlock::Create(def->jitState->context(),
                                          "if.in.range", def->f);
    else_block =
        llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
    end_block = llvm::BasicBlock::Create(def->jitState->context(), "if.end");
    def->builder->CreateCondBr(cmp, then_block, else_block);
    def->builder->SetInsertPoint(then_block);
  }

  llvm::Value *ptr = def->builder->CreateGEP(data, key);
  llvm::Instruction *value = def->builder->CreateLoad(ptr);
  value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_pdoubleT);
  emit_store_reg_n_withtype(def, value, ra);

  if (!omitArrayGetRangeCheck) {
    def->builder->CreateBr(end_block);

    def->f->getBasicBlockList().push_back(else_block);
    def->builder->SetInsertPoint(else_block);

    emit_raise_lua_error(def, "array out of bounds");
    def->builder->CreateBr(end_block);

    def->f->getBasicBlockList().push_back(end_block);
    def->builder->SetInsertPoint(end_block);
  }
}

void RaviCodeGenerator::emit_GETTABLE_AI(RaviFunctionDef *def, int A, int B,
                                         int C, bool omitArrayGetRangeCheck,
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

  emit_debug_trace(def, OP_RAVI_GETTABLE_AI, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *key = emit_load_reg_i(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, rb);
  llvm::Instruction *data = emit_load_reg_h_intarray(def, t);

  llvm::BasicBlock *then_block = nullptr;
  llvm::BasicBlock *else_block = nullptr;
  llvm::BasicBlock *end_block = nullptr;
  if (!omitArrayGetRangeCheck) {
    llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
    llvm::Value *ulen =
        def->builder->CreateZExt(len, def->types->lua_UnsignedT, "ulen");

    llvm::Value *cmp = def->builder->CreateICmpULT(key, ulen);
    then_block = llvm::BasicBlock::Create(def->jitState->context(),
                                          "if.in.range", def->f);
    else_block =
        llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
    end_block = llvm::BasicBlock::Create(def->jitState->context(), "if.end");
    def->builder->CreateCondBr(cmp, then_block, else_block);
    def->builder->SetInsertPoint(then_block);
  }

  llvm::Value *ptr = def->builder->CreateGEP(data, key);
  llvm::Instruction *value = def->builder->CreateLoad(ptr);
  value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_plonglongT);
  emit_store_reg_i_withtype(def, value, ra);

  if (!omitArrayGetRangeCheck) {
    def->builder->CreateBr(end_block);

    def->f->getBasicBlockList().push_back(else_block);
    def->builder->SetInsertPoint(else_block);

    emit_raise_lua_error(def, "array out of bounds");
    def->builder->CreateBr(end_block);

    def->f->getBasicBlockList().push_back(end_block);
    def->builder->SetInsertPoint(end_block);
  }
}

void RaviCodeGenerator::emit_SETTABLE_AI(RaviFunctionDef *def, int A, int B,
                                         int C, bool known_int, int pc) {

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

  emit_debug_trace(def, known_int ? OP_RAVI_SETTABLE_AII : OP_RAVI_SETTABLE_AI,
                   pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *key = emit_load_reg_i(def, rb);
  llvm::Instruction *value =
      known_int ? emit_load_reg_i(def, rc) : emit_tointeger(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, ra);
  llvm::Instruction *data = emit_load_reg_h_intarray(def, t);
  llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
  llvm::Value *ulen =
      def->builder->CreateZExt(len, def->types->lua_UnsignedT, "ulen");

  llvm::Value *cmp = def->builder->CreateICmpULT(key, ulen);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.in.range", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");
  def->builder->CreateCondBr(cmp, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  llvm::Value *ptr = def->builder->CreateGEP(data, key);

  llvm::Instruction *storeinst = def->builder->CreateStore(value, ptr);
  storeinst->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_plonglongT);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  CreateCall4(def->builder, def->raviH_set_intF, def->L, t, key, value);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}

void RaviCodeGenerator::emit_SETTABLE_AF(RaviFunctionDef *def, int A, int B,
                                         int C, bool known_float, int pc) {

  //#define raviH_set_float_inline(L, t, key, value)
  //{ unsigned ukey = (unsigned)((key));
  //  lua_Number *data = (lua_Number *)t->ravi_array.data;
  //  if (ukey < t->ravi_array.len) {
  //    data[ukey] = value;
  //      } else
  //    raviH_set_float(L, t, ukey, value);
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

  emit_debug_trace(
      def, known_float ? OP_RAVI_SETTABLE_AFF : OP_RAVI_SETTABLE_AF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *key = emit_load_reg_i(def, rb);
  llvm::Instruction *value =
      known_float ? emit_load_reg_n(def, rc) : emit_tofloat(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, ra);
  llvm::Instruction *data = emit_load_reg_h_floatarray(def, t);
  llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
  llvm::Value *ulen =
      def->builder->CreateZExt(len, def->types->lua_UnsignedT, "ulen");

  llvm::Value *cmp = def->builder->CreateICmpULT(key, ulen);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.in.range", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");
  def->builder->CreateCondBr(cmp, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  llvm::Value *ptr = def->builder->CreateGEP(data, key);

  llvm::Instruction *storeinst = def->builder->CreateStore(value, ptr);
  storeinst->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_pdoubleT);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  CreateCall4(def->builder, def->raviH_set_floatF, def->L, t, key, value);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}

// R(A) := UpValue[B]
void RaviCodeGenerator::emit_GETUPVAL(RaviFunctionDef *def, int A, int B,
                                      int pc) {
  // int b = GETARG_B(i);
  // setobj2s(L, ra, cl->upvals[b]->v);
  emit_debug_trace(def, OP_GETUPVAL, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *upval_ptr = emit_gep_upvals(def, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  emit_assign(def, ra, v);
}

// UpValue[B] := R(A)
void RaviCodeGenerator::emit_SETUPVAL(RaviFunctionDef *def, int A, int B,
                                      int pc) {

  // UpVal *uv = cl->upvals[GETARG_B(i)];
  // setobj(L, uv->v, ra);
  // luaC_upvalbarrier(L, uv);

  emit_debug_trace(def, OP_SETUPVAL, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *upval_ptr = emit_gep_upvals(def, B);
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
void RaviCodeGenerator::emit_GETTABUP(RaviFunctionDef *def, int A, int B, int C,
                                      int pc) {
  // int b = GETARG_B(i);
  // Protect(luaV_gettable(L, cl->upvals[b]->v, RKC(i), ra));
  bool traced = emit_debug_trace(def, OP_GETTABUP, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  llvm::Value *upval_ptr = emit_gep_upvals(def, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  CreateCall4(def->builder, def->luaV_gettableF, def->L, v, rc, ra);
}

// UpValue[A][RK(B)] := RK(C)
void RaviCodeGenerator::emit_SETTABUP(RaviFunctionDef *def, int A, int B, int C,
                                      int pc) {

  // int a = GETARG_A(i);
  // Protect(luaV_settable(L, cl->upvals[a]->v, RKB(i), RKC(i)));

  bool traced = emit_debug_trace(def, OP_SETTABUP, pc);
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  llvm::Value *upval_ptr = emit_gep_upvals(def, A);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  CreateCall4(def->builder, def->luaV_settableF, def->L, v, rb, rc);
}

void RaviCodeGenerator::emit_NEWARRAYINT(RaviFunctionDef *def, int A, int pc) {
  emit_debug_trace(def, OP_RAVI_NEWARRAYI, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  CreateCall3(def->builder, def->raviV_op_newarrayintF, def->L, def->ci_val,
              ra);
}

void RaviCodeGenerator::emit_NEWARRAYFLOAT(RaviFunctionDef *def, int A,
                                           int pc) {
  emit_debug_trace(def, OP_RAVI_NEWARRAYF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  CreateCall3(def->builder, def->raviV_op_newarrayfloatF, def->L, def->ci_val,
              ra);
}

void RaviCodeGenerator::emit_NEWTABLE(RaviFunctionDef *def, int A, int B, int C,
                                      int pc) {
  //  case OP_NEWTABLE: {
  //    int b = GETARG_B(i);
  //    int c = GETARG_C(i);
  //    Table *t = luaH_new(L);
  //    sethvalue(L, ra, t);
  //    if (b != 0 || c != 0)
  //      luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
  //    checkGC(L, ra + 1);
  //  } break;

  emit_debug_trace(def, OP_NEWTABLE, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  CreateCall5(def->builder, def->raviV_op_newtableF, def->L, def->ci_val, ra,
              def->types->kInt[B], def->types->kInt[C]);
}

void RaviCodeGenerator::emit_SETLIST(RaviFunctionDef *def, int A, int B, int C,
                                     int pc) {
  emit_debug_trace(def, OP_SETLIST, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  CreateCall5(def->builder, def->raviV_op_setlistF, def->L, def->ci_val, ra,
              def->types->kInt[B], def->types->kInt[C]);
}

void RaviCodeGenerator::emit_TOARRAY(RaviFunctionDef *def, int A,
                                     int array_type_expected,
                                     const char *errmsg, int pc) {

  // if (!ttistable(ra) || hvalue(ra)->ravi_array.type != RAVI_TARRAYINT)
  //  luaG_runerror(L, "integer[] expected");

  emit_debug_trace(def,
                   array_type_expected == RAVI_TARRAYINT ? OP_RAVI_TOARRAYI
                                                         : OP_RAVI_TOARRAYF,
                   pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Instruction *type = emit_load_type(def, ra);

  // type != LUA_TTABLE ?
  llvm::Value *cmp1 =
      emit_is_not_value_of_type(def, type, LUA__TTABLE, "is.not.table");

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
}

void RaviCodeGenerator::emit_MOVEAI(RaviFunctionDef *def, int A, int B,
                                    int pc) {
  emit_debug_trace(def, OP_RAVI_MOVEAI, pc);
  emit_TOARRAY(def, B, RAVI_TARRAYINT, "integer[] expected", pc);
  llvm::Value *src = emit_gep_register(def, B);
  llvm::Value *dest = emit_gep_register(def, A);
  emit_assign(def, dest, src);
}

void RaviCodeGenerator::emit_MOVEAF(RaviFunctionDef *def, int A, int B,
                                    int pc) {
  emit_debug_trace(def, OP_RAVI_MOVEAF, pc);
  emit_TOARRAY(def, B, RAVI_TARRAYFLT, "number[] expected", pc);
  llvm::Value *src = emit_gep_register(def, B);
  llvm::Value *dest = emit_gep_register(def, A);
  emit_assign(def, dest, src);
}
}
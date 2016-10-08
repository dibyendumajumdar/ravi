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

// This file contains LLVM IR generation for standard Lua arithmetic op codes

namespace ravi {

// OP_ADD, OP_SUB, OP_MUL and OP_DIV
void RaviCodeGenerator::emit_ARITH(RaviFunctionDef *def, int A, int B, int C,
                                   OpCode op, TMS tms, int pc) {

#if 0
  bool traced = emit_debug_trace(def, op, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  switch (op) {
  case OP_ADD:
    CreateCall4(def->builder, def->raviV_op_addF,
      def->L, ra, rb, rc);
    break;
  case OP_SUB:
    CreateCall4(def->builder, def->raviV_op_subF,
      def->L, ra, rb, rc);
    break;
  case OP_MUL:
    CreateCall4(def->builder, def->raviV_op_mulF,
      def->L, ra, rb, rc);
    break;
  case OP_DIV:
    CreateCall4(def->builder, def->raviV_op_divF,
      def->L, ra, rb, rc);
    break;
  default:
    lua_assert(0);
  }

#else
  // TValue *rb = RKB(i);
  // TValue *rc = RKC(i);
  // lua_Number nb; lua_Number nc;
  // if (ttisinteger(rb) && ttisinteger(rc)) {
  //  lua_Integer ib = ivalue(rb); lua_Integer ic = ivalue(rc);
  //  setivalue(ra, intop(+, ib, ic));
  //}
  // else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
  //  setfltvalue(ra, luai_numadd(L, nb, nc));
  //}
  // else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_ADD)); }

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *nb = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nb");
  llvm::Value *nc = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nc");

  bool traced = emit_debug_trace(def, op, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  llvm::Value *rb_type = emit_load_type(def, rb);
  llvm::Value *rc_type = emit_load_type(def, rc);

  llvm::BasicBlock *float_op =
      llvm::BasicBlock::Create(def->jitState->context(), "float.op");
  llvm::BasicBlock *try_meta =
      llvm::BasicBlock::Create(def->jitState->context(), "try_meta");
  llvm::BasicBlock *done_block =
      llvm::BasicBlock::Create(def->jitState->context(), "done");

  if (op != OP_DIV) {
    llvm::Value *cmp1 =
        emit_is_value_of_type(def, rb_type, LUA__TNUMINT, "rb.is.integer");
    llvm::Value *cmp2 =
        emit_is_value_of_type(def, rc_type, LUA__TNUMINT, "rc.is.integer");

    llvm::Value *andvalue = def->builder->CreateAnd(cmp1, cmp2);

    // Check if both RB and RC are integers
    llvm::BasicBlock *then_block = llvm::BasicBlock::Create(
        def->jitState->context(), "if.integer", def->f);
    llvm::BasicBlock *else_block =
        llvm::BasicBlock::Create(def->jitState->context(), "if.not.integer");
    def->builder->CreateCondBr(andvalue, then_block, else_block);
    def->builder->SetInsertPoint(then_block);

    // Both are integers
    llvm::Instruction *lhs = emit_load_reg_i(def, rb);
    llvm::Instruction *rhs = emit_load_reg_i(def, rc);

    llvm::Value *result = nullptr;
    switch (op) {
    case OP_ADD:
      result = def->builder->CreateAdd(lhs, rhs, "", false, true);
      break;
    case OP_SUB:
      result = def->builder->CreateSub(lhs, rhs, "", false, true);
      break;
    case OP_MUL:
      result = def->builder->CreateMul(lhs, rhs, "", false, true);
      break;
    default:
      lua_assert(0);
    }

    emit_store_reg_i_withtype(def, result, ra);

    def->builder->CreateBr(done_block);

    // Not integer
    def->f->getBasicBlockList().push_back(else_block);
    def->builder->SetInsertPoint(else_block);
  }

  // Is RB a float?
  llvm::Value *cmp1 =
      emit_is_value_of_type(def, rb_type, LUA__TNUMFLT, "rb.is.float");

  llvm::BasicBlock *convert_rb =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.rb");
  llvm::BasicBlock *test_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "test.rc");
  llvm::BasicBlock *load_rb =
      llvm::BasicBlock::Create(def->jitState->context(), "load.rb");

  // If RB is floating then load RB, else convert RB
  def->builder->CreateCondBr(cmp1, load_rb, convert_rb);

  // Convert RB
  def->f->getBasicBlockList().push_back(convert_rb);
  def->builder->SetInsertPoint(convert_rb);

  // Call luaV_tonumber_()
  llvm::Value *rb_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, rb, nb);
  cmp1 =
      def->builder->CreateICmpEQ(rb_isnum, def->types->kInt[1], "rb.float.ok");

  // If not number then go to meta block
  // Else proceed to test RC
  def->builder->CreateCondBr(cmp1, test_rc, try_meta);

  def->f->getBasicBlockList().push_back(load_rb);
  def->builder->SetInsertPoint(load_rb);

  // Copy RB to local nb
  auto src = emit_load_reg_n(def, rb);
  emit_store_local_n(def, src, nb);

  def->builder->CreateBr(test_rc);

  def->f->getBasicBlockList().push_back(test_rc);
  def->builder->SetInsertPoint(test_rc);

  // Is RC a float?
  cmp1 = emit_is_value_of_type(def, rc_type, LUA__TNUMFLT, "rc.is.float");

  llvm::BasicBlock *convert_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.rc");
  llvm::BasicBlock *load_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "load.rc");

  // If RC is float load RC
  // else try to convert RC
  def->builder->CreateCondBr(cmp1, load_rc, convert_rc);

  def->f->getBasicBlockList().push_back(convert_rc);
  def->builder->SetInsertPoint(convert_rc);

  // Call luaV_tonumber_()
  llvm::Value *rc_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, rc, nc);
  cmp1 =
      def->builder->CreateICmpEQ(rc_isnum, def->types->kInt[1], "rc.float.ok");

  // If not number then go to meta block
  // else both RB and RC float so go to op
  def->builder->CreateCondBr(cmp1, float_op, try_meta);

  def->f->getBasicBlockList().push_back(load_rc);
  def->builder->SetInsertPoint(load_rc);

  // Copy RC to local;
  src = emit_load_reg_n(def, rc);
  emit_store_local_n(def, src, nc);

  def->builder->CreateBr(float_op);

  def->f->getBasicBlockList().push_back(float_op);
  def->builder->SetInsertPoint(float_op);

  llvm::Instruction *lhs = emit_load_local_n(def, nb);
  llvm::Instruction *rhs = emit_load_local_n(def, nc);

  llvm::Value *result = nullptr;
  // Add and set RA
  switch (op) {
  case OP_ADD:
    result = def->builder->CreateFAdd(lhs, rhs);
    break;
  case OP_SUB:
    result = def->builder->CreateFSub(lhs, rhs);
    break;
  case OP_MUL:
    result = def->builder->CreateFMul(lhs, rhs);
    break;
  case OP_DIV:
    result = def->builder->CreateFDiv(lhs, rhs);
    break;
  default:
    lua_assert(0);
  }

  emit_store_reg_n_withtype(def, result, ra);

  def->builder->CreateBr(done_block);

  // Neither integer nor float so try meta
  def->f->getBasicBlockList().push_back(try_meta);
  def->builder->SetInsertPoint(try_meta);

  // Before invoking metamethod set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  CreateCall5(def->builder, def->luaT_trybinTMF, def->L, rb, rc, ra,
              def->types->kInt[tms]);
  def->builder->CreateBr(done_block);

  def->f->getBasicBlockList().push_back(done_block);
  def->builder->SetInsertPoint(done_block);
#endif
}

// OP_MOD
void RaviCodeGenerator::emit_MOD(RaviFunctionDef *def, int A, int B, int C,
                                 int pc) {

  // TValue *rb = RKB(i);
  // TValue *rc = RKC(i);
  // lua_Number nb; lua_Number nc;
  // if (ttisinteger(rb) && ttisinteger(rc)) {
  //  lua_Integer ib = ivalue(rb); lua_Integer ic = ivalue(rc);
  //  setivalue(ra, luaV_mod(L, ib, ic));
  //}
  // else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
  //  lua_Number m;
  //  luai_nummod(L, nb, nc, m);
  //  setfltvalue(ra, m);
  //}
  // else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_MOD)); }

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *nb = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nb");
  llvm::Value *nc = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nc");

  bool traced = emit_debug_trace(def, OP_MOD, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  llvm::Value *rb_type = emit_load_type(def, rb);
  llvm::Value *rc_type = emit_load_type(def, rc);

  llvm::BasicBlock *float_op =
      llvm::BasicBlock::Create(def->jitState->context(), "float.op");
  llvm::BasicBlock *try_meta =
      llvm::BasicBlock::Create(def->jitState->context(), "try_meta");
  llvm::BasicBlock *done_block =
      llvm::BasicBlock::Create(def->jitState->context(), "done");

  llvm::Value *cmp1 =
      emit_is_value_of_type(def, rb_type, LUA__TNUMINT, "rb.is.integer");
  llvm::Value *cmp2 =
      emit_is_value_of_type(def, rc_type, LUA__TNUMINT, "rc.is.integer");

  llvm::Value *andvalue = def->builder->CreateAnd(cmp1, cmp2);

  // Check if both RB and RC are integers
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.integer", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.integer");
  def->builder->CreateCondBr(andvalue, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  // Both are integers
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);

  llvm::Value *result =
      CreateCall3(def->builder, def->luaV_modF, def->L, lhs, rhs);
  emit_store_reg_i_withtype(def, result, ra);

  def->builder->CreateBr(done_block);

  // Not integer
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  // Is RB a float?
  cmp1 = emit_is_value_of_type(def, rb_type, LUA__TNUMFLT, "rb.is.float");

  llvm::BasicBlock *convert_rb =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.rb");
  llvm::BasicBlock *test_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "test.rc");
  llvm::BasicBlock *load_rb =
      llvm::BasicBlock::Create(def->jitState->context(), "load.rb");

  // If RB is floating then load RB, else convert RB
  def->builder->CreateCondBr(cmp1, load_rb, convert_rb);

  // Convert RB
  def->f->getBasicBlockList().push_back(convert_rb);
  def->builder->SetInsertPoint(convert_rb);

  // Call luaV_tonumber_()
  llvm::Value *rb_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, rb, nb);
  cmp1 =
      def->builder->CreateICmpEQ(rb_isnum, def->types->kInt[1], "rb.float.ok");

  // If not number then go to meta block
  // Else proceed to test RC
  def->builder->CreateCondBr(cmp1, test_rc, try_meta);

  def->f->getBasicBlockList().push_back(load_rb);
  def->builder->SetInsertPoint(load_rb);

  // Copy RB to local nb
  auto src = emit_load_reg_n(def, rb);
  emit_store_local_n(def, src, nb);

  def->builder->CreateBr(test_rc);

  def->f->getBasicBlockList().push_back(test_rc);
  def->builder->SetInsertPoint(test_rc);

  // Is RC a float?
  cmp1 = emit_is_value_of_type(def, rc_type, LUA__TNUMFLT, "rc.is.float");

  llvm::BasicBlock *convert_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.rc");
  llvm::BasicBlock *load_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "load.rc");

  // If RC is float load RC
  // else try to convert RC
  def->builder->CreateCondBr(cmp1, load_rc, convert_rc);

  def->f->getBasicBlockList().push_back(convert_rc);
  def->builder->SetInsertPoint(convert_rc);

  // Call luaV_tonumber_()
  llvm::Value *rc_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, rc, nc);
  cmp1 =
      def->builder->CreateICmpEQ(rc_isnum, def->types->kInt[1], "rc.float.ok");

  // If not number then go to meta block
  // else both RB and RC float so go to op
  def->builder->CreateCondBr(cmp1, float_op, try_meta);

  def->f->getBasicBlockList().push_back(load_rc);
  def->builder->SetInsertPoint(load_rc);

  // Copy RC to local;
  src = emit_load_reg_n(def, rc);
  emit_store_local_n(def, src, nc);

  def->builder->CreateBr(float_op);

  def->f->getBasicBlockList().push_back(float_op);
  def->builder->SetInsertPoint(float_op);

  lhs = emit_load_local_n(def, nb);
  rhs = emit_load_local_n(def, nc);

  llvm::Value *fmod_result = CreateCall2(def->builder, def->fmodFunc, lhs, rhs);

  // if ((m)*(b) < 0) (m) += (b);
  llvm::Value *mb = def->builder->CreateFMul(fmod_result, rhs);

  // If m*b < 0
  cmp1 = def->builder->CreateFCmpOLT(
      mb, llvm::ConstantFP::get(def->types->lua_NumberT, 0.0));
  llvm::BasicBlock *mb_lt0_then =
      llvm::BasicBlock::Create(def->jitState->context(), "mb.lt.zero", def->f);
  llvm::BasicBlock *mb_lt0_else =
      llvm::BasicBlock::Create(def->jitState->context(), "mb.not.lt.zero");
  llvm::BasicBlock *mb_lt0_done =
      llvm::BasicBlock::Create(def->jitState->context(), "mb.not.lt.end");

  def->builder->CreateCondBr(cmp1, mb_lt0_then, mb_lt0_else);
  def->builder->SetInsertPoint(mb_lt0_then);

  result = def->builder->CreateFAdd(fmod_result, rhs);
  emit_store_local_n(def, result, nb);
  def->builder->CreateBr(mb_lt0_done);

  def->f->getBasicBlockList().push_back(mb_lt0_else);
  def->builder->SetInsertPoint(mb_lt0_else);

  emit_store_local_n(def, fmod_result, nb);
  def->builder->CreateBr(mb_lt0_done);

  def->f->getBasicBlockList().push_back(mb_lt0_done);
  def->builder->SetInsertPoint(mb_lt0_done);

  lhs = emit_load_local_n(def, nb);

  emit_store_reg_n_withtype(def, lhs, ra);

  def->builder->CreateBr(done_block);

  // Neither integer nor float so try meta
  def->f->getBasicBlockList().push_back(try_meta);
  def->builder->SetInsertPoint(try_meta);

  // before invoking metamethod set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  CreateCall5(def->builder, def->luaT_trybinTMF, def->L, rb, rc, ra,
              def->types->kInt[TM_MOD]);
  def->builder->CreateBr(done_block);

  def->f->getBasicBlockList().push_back(done_block);
  def->builder->SetInsertPoint(done_block);
}

// OP_IDIV
void RaviCodeGenerator::emit_IDIV(RaviFunctionDef *def, int A, int B, int C,
                                  int pc) {

  // TValue *rb = RKB(i);
  // TValue *rc = RKC(i);
  // lua_Number nb; lua_Number nc;
  // if (ttisinteger(rb) && ttisinteger(rc)) {
  //  lua_Integer ib = ivalue(rb); lua_Integer ic = ivalue(rc);
  //  setivalue(ra, luaV_div(L, ib, ic));
  //}
  // else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
  //  setfltvalue(ra, luai_numidiv(L, nb, nc));
  //}
  // else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_IDIV)); }

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *nb = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nb");
  llvm::Value *nc = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nc");

  bool traced = emit_debug_trace(def, OP_IDIV, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  llvm::Value *rb_type = emit_load_type(def, rb);
  llvm::Value *rc_type = emit_load_type(def, rc);

  llvm::BasicBlock *float_op =
      llvm::BasicBlock::Create(def->jitState->context(), "float.op");
  llvm::BasicBlock *try_meta =
      llvm::BasicBlock::Create(def->jitState->context(), "try_meta");
  llvm::BasicBlock *done_block =
      llvm::BasicBlock::Create(def->jitState->context(), "done");

  llvm::Value *cmp1 =
      emit_is_value_of_type(def, rb_type, LUA__TNUMINT, "rb.is.integer");
  llvm::Value *cmp2 =
      emit_is_value_of_type(def, rc_type, LUA__TNUMINT, "rc.is.integer");

  llvm::Value *andvalue = def->builder->CreateAnd(cmp1, cmp2);

  // Check if both RB and RC are integers
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.integer", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.integer");
  def->builder->CreateCondBr(andvalue, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  // Both are integers
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);

  llvm::Value *result =
      CreateCall3(def->builder, def->luaV_divF, def->L, lhs, rhs);
  emit_store_reg_i_withtype(def, result, ra);

  def->builder->CreateBr(done_block);

  // Not integer
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  // Is RB a float?
  cmp1 = emit_is_value_of_type(def, rb_type, LUA__TNUMFLT, "rb.is.float");

  llvm::BasicBlock *convert_rb =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.rb");
  llvm::BasicBlock *test_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "test.rc");
  llvm::BasicBlock *load_rb =
      llvm::BasicBlock::Create(def->jitState->context(), "load.rb");

  // If RB is floating then load RB, else convert RB
  def->builder->CreateCondBr(cmp1, load_rb, convert_rb);

  // Convert RB
  def->f->getBasicBlockList().push_back(convert_rb);
  def->builder->SetInsertPoint(convert_rb);

  // Call luaV_tonumber_()
  llvm::Value *rb_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, rb, nb);
  cmp1 =
      def->builder->CreateICmpEQ(rb_isnum, def->types->kInt[1], "rb.float.ok");

  // If not number then go to meta block
  // Else proceed to test RC
  def->builder->CreateCondBr(cmp1, test_rc, try_meta);

  def->f->getBasicBlockList().push_back(load_rb);
  def->builder->SetInsertPoint(load_rb);

  // Copy RB to local nb
  auto src = emit_load_reg_n(def, rb);
  emit_store_local_n(def, src, nb);

  def->builder->CreateBr(test_rc);

  def->f->getBasicBlockList().push_back(test_rc);
  def->builder->SetInsertPoint(test_rc);

  // Is RC a float?
  cmp1 = emit_is_value_of_type(def, rc_type, LUA__TNUMFLT, "rc.is.float");

  llvm::BasicBlock *convert_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.rc");
  llvm::BasicBlock *load_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "load.rc");

  // If RC is float load RC
  // else try to convert RC
  def->builder->CreateCondBr(cmp1, load_rc, convert_rc);

  def->f->getBasicBlockList().push_back(convert_rc);
  def->builder->SetInsertPoint(convert_rc);

  // Call luaV_tonumber_()
  llvm::Value *rc_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, rc, nc);
  cmp1 =
      def->builder->CreateICmpEQ(rc_isnum, def->types->kInt[1], "rc.float.ok");

  // If not number then go to meta block
  // else both RB and RC float so go to op
  def->builder->CreateCondBr(cmp1, float_op, try_meta);

  def->f->getBasicBlockList().push_back(load_rc);
  def->builder->SetInsertPoint(load_rc);

  // Copy RC to local;
  src = emit_load_reg_n(def, rc);
  emit_store_local_n(def, src, nc);

  def->builder->CreateBr(float_op);

  def->f->getBasicBlockList().push_back(float_op);
  def->builder->SetInsertPoint(float_op);

  lhs = emit_load_local_n(def, nb);
  rhs = emit_load_local_n(def, nc);

  result = def->builder->CreateFDiv(lhs, rhs);
  llvm::Value *floor_result = def->builder->CreateCall(def->floorFunc, result);

  emit_store_reg_n_withtype(def, floor_result, ra);

  def->builder->CreateBr(done_block);

  // Neither integer nor float so try meta
  def->f->getBasicBlockList().push_back(try_meta);
  def->builder->SetInsertPoint(try_meta);

  // before invoking metamethod set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  CreateCall5(def->builder, def->luaT_trybinTMF, def->L, rb, rc, ra,
              def->types->kInt[TM_IDIV]);
  def->builder->CreateBr(done_block);

  def->f->getBasicBlockList().push_back(done_block);
  def->builder->SetInsertPoint(done_block);
}

// OP_POW
void RaviCodeGenerator::emit_POW(RaviFunctionDef *def, int A, int B, int C,
                                 int pc) {

  // TValue *rb = RKB(i);
  // TValue *rc = RKC(i);
  // lua_Number nb; lua_Number nc;
  // if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
  //  setfltvalue(ra, luai_numpow(L, nb, nc));
  //}
  // else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_POW)); }

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *nb = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nb");
  llvm::Value *nc = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nc");

  bool traced = emit_debug_trace(def, OP_POW, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  llvm::Value *rb_type = emit_load_type(def, rb);
  llvm::Value *rc_type = emit_load_type(def, rc);

  llvm::BasicBlock *float_op =
      llvm::BasicBlock::Create(def->jitState->context(), "float.op");
  llvm::BasicBlock *try_meta =
      llvm::BasicBlock::Create(def->jitState->context(), "try_meta");
  llvm::BasicBlock *done_block =
      llvm::BasicBlock::Create(def->jitState->context(), "done");

  // Is RB a float?
  llvm::Value *cmp1 =
      emit_is_value_of_type(def, rb_type, LUA__TNUMFLT, "rb.is.float");

  llvm::BasicBlock *convert_rb =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.rb");
  llvm::BasicBlock *test_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "test.rc");
  llvm::BasicBlock *load_rb =
      llvm::BasicBlock::Create(def->jitState->context(), "load.rb");

  // If RB is floating then load RB, else convert RB
  def->builder->CreateCondBr(cmp1, load_rb, convert_rb);

  // Convert RB
  def->f->getBasicBlockList().push_back(convert_rb);
  def->builder->SetInsertPoint(convert_rb);

  // Call luaV_tonumber_()
  llvm::Value *rb_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, rb, nb);
  cmp1 =
      def->builder->CreateICmpEQ(rb_isnum, def->types->kInt[1], "rb.float.ok");

  // If not number then go to meta block
  // Else proceed to test RC
  def->builder->CreateCondBr(cmp1, test_rc, try_meta);

  def->f->getBasicBlockList().push_back(load_rb);
  def->builder->SetInsertPoint(load_rb);

  // Copy RB to local nb
  auto src = emit_load_reg_n(def, rb);
  emit_store_local_n(def, src, nb);

  def->builder->CreateBr(test_rc);

  def->f->getBasicBlockList().push_back(test_rc);
  def->builder->SetInsertPoint(test_rc);

  // Is RC a float?
  cmp1 = emit_is_value_of_type(def, rc_type, LUA__TNUMFLT, "rc.is.float");

  llvm::BasicBlock *convert_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.rc");
  llvm::BasicBlock *load_rc =
      llvm::BasicBlock::Create(def->jitState->context(), "load.rc");

  // If RC is float load RC
  // else try to convert RC
  def->builder->CreateCondBr(cmp1, load_rc, convert_rc);

  def->f->getBasicBlockList().push_back(convert_rc);
  def->builder->SetInsertPoint(convert_rc);

  // Call luaV_tonumber_()
  llvm::Value *rc_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, rc, nc);
  cmp1 =
      def->builder->CreateICmpEQ(rc_isnum, def->types->kInt[1], "rc.float.ok");

  // If not number then go to meta block
  // else both RB and RC float so go to op
  def->builder->CreateCondBr(cmp1, float_op, try_meta);

  def->f->getBasicBlockList().push_back(load_rc);
  def->builder->SetInsertPoint(load_rc);

  // Copy RC to local;
  src = emit_load_reg_n(def, rc);
  emit_store_local_n(def, src, nc);

  def->builder->CreateBr(float_op);

  def->f->getBasicBlockList().push_back(float_op);
  def->builder->SetInsertPoint(float_op);

  llvm::Instruction *lhs = emit_load_local_n(def, nb);
  llvm::Instruction *rhs = emit_load_local_n(def, nc);

  llvm::Value *pow_result = CreateCall2(def->builder, def->powFunc, lhs, rhs);

  emit_store_reg_n_withtype(def, pow_result, ra);

  def->builder->CreateBr(done_block);

  // Neither integer nor float so try meta
  def->f->getBasicBlockList().push_back(try_meta);
  def->builder->SetInsertPoint(try_meta);

  // Before invoking metamethod set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  CreateCall5(def->builder, def->luaT_trybinTMF, def->L, rb, rc, ra,
              def->types->kInt[TM_POW]);
  def->builder->CreateBr(done_block);

  def->f->getBasicBlockList().push_back(done_block);
  def->builder->SetInsertPoint(done_block);
}

void RaviCodeGenerator::emit_UNM(RaviFunctionDef *def, int A, int B, int pc) {

  // TValue *rb = RB(i);
  // lua_Number nb;
  // if (ttisinteger(rb)) {
  //  lua_Integer ib = ivalue(rb);
  //  setivalue(ra, intop(-, 0, ib));
  //}
  // else if (tonumber(rb, &nb)) {
  //  setfltvalue(ra, luai_numunm(L, nb));
  //}
  // else {
  //  Protect(luaT_trybinTM(L, rb, rb, ra, TM_UNM));
  //}

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *nb = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nb");

  bool traced = emit_debug_trace(def, OP_UNM, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);

  llvm::Value *rb_type = emit_load_type(def, rb);

  llvm::BasicBlock *float_op =
      llvm::BasicBlock::Create(def->jitState->context(), "float.op");
  llvm::BasicBlock *try_meta =
      llvm::BasicBlock::Create(def->jitState->context(), "try_meta");
  llvm::BasicBlock *done_block =
      llvm::BasicBlock::Create(def->jitState->context(), "done");

  llvm::Value *cmp1 =
      emit_is_value_of_type(def, rb_type, LUA__TNUMINT, "rb.is.integer");

  // Check if both RB and RC are integers
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.integer", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.integer");
  def->builder->CreateCondBr(cmp1, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  // Both are integers
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Value *result = def->builder->CreateNeg(lhs, "", false, true);

  emit_store_reg_i_withtype(def, result, ra);

  def->builder->CreateBr(done_block);

  // Not integer
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  // Is RB a float?
  cmp1 = emit_is_value_of_type(def, rb_type, LUA__TNUMFLT, "rb.is.float");

  llvm::BasicBlock *convert_rb =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.rb");
  llvm::BasicBlock *load_rb =
      llvm::BasicBlock::Create(def->jitState->context(), "load.rb");

  // If RB is floating then load RB, else convert RB
  def->builder->CreateCondBr(cmp1, load_rb, convert_rb);

  // Convert RB
  def->f->getBasicBlockList().push_back(convert_rb);
  def->builder->SetInsertPoint(convert_rb);

  // Call luaV_tonumber_()
  llvm::Value *rb_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, rb, nb);
  cmp1 =
      def->builder->CreateICmpEQ(rb_isnum, def->types->kInt[1], "rb.float.ok");

  // If not number then go to meta block
  // Else proceed to test RC
  def->builder->CreateCondBr(cmp1, float_op, try_meta);

  def->f->getBasicBlockList().push_back(load_rb);
  def->builder->SetInsertPoint(load_rb);

  // Copy RB to local nb
  auto src = emit_load_reg_n(def, rb);
  emit_store_local_n(def, src, nb);

  def->builder->CreateBr(float_op);

  def->f->getBasicBlockList().push_back(float_op);
  def->builder->SetInsertPoint(float_op);

  lhs = emit_load_local_n(def, nb);

  result = def->builder->CreateFNeg(lhs);

  emit_store_reg_n_withtype(def, result, ra);

  def->builder->CreateBr(done_block);

  // Neither integer nor float so try meta
  def->f->getBasicBlockList().push_back(try_meta);
  def->builder->SetInsertPoint(try_meta);

  // before invoking metamethod set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  CreateCall5(def->builder, def->luaT_trybinTMF, def->L, rb, rb, ra,
              def->types->kInt[TM_UNM]);
  def->builder->CreateBr(done_block);

  def->f->getBasicBlockList().push_back(done_block);
  def->builder->SetInsertPoint(done_block);
}
}
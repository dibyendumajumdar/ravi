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

void RaviCodeGenerator::emit_FORLOOP2(RaviFunctionDef *def, int A, int pc,
                                      RaviBranchDef &b, int pc1) {

  // 7[1]     FORLOOP         1 - 2; to 6
  // if (ttisinteger(ra)) {  /* integer loop? */
  //  lua_Integer step = ivalue(ra + 2);
  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  //  lua_Integer limit = ivalue(ra + 1);
  //  if ((0 < step) ? (idx <= limit) : (limit <= idx)) {
  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    setivalue(ra, idx);  /* update internal index... */
  //    setivalue(ra + 3, idx);  /* ...and external index */
  //  }
  //}
  // else {  /* floating loop */
  //  lua_Number step = fltvalue(ra + 2);
  //  lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
  //  lua_Number limit = fltvalue(ra + 1);
  //  if (luai_numlt(0, step) ? luai_numle(idx, limit)
  //    : luai_numle(limit, idx)) {
  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    setfltvalue(ra, idx);  /* update internal index... */
  //    setfltvalue(ra + 3, idx);  /* ...and external index */
  //  }
  //}

  emit_debug_trace(def, OP_FORLOOP, pc1);

  // We are in b.jmp1 as this is already the current block
  lua_assert(def->builder->GetInsertBlock() == b.jmp1);

  // Obtain pointers to the value.i field
  llvm::Value *step_int_ptr = b.istep;
  llvm::Value *idx_int_ptr = b.iidx;
  llvm::Value *limit_int_ptr = b.ilimit;
  // Obtain pointers to the value.n field
  llvm::Value *step_double_ptr = b.fstep;
  llvm::Value *idx_double_ptr = b.fidx;
  llvm::Value *limit_double_ptr = b.flimit;

  // Create the done block
  llvm::BasicBlock *exit_block =
      llvm::BasicBlock::Create(def->jitState->context(), "exit_iforloop");

  // INTEGER CASE

  //  lua_Integer step = ivalue(ra + 2);
  llvm::Instruction *step_int_value = emit_load_local_n(def, step_int_ptr);

  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  llvm::Instruction *idx_int_value = emit_load_local_n(def, idx_int_ptr);
  llvm::Value *new_idx = def->builder->CreateAdd(step_int_value, idx_int_value,
                                                 "next.idx", false, true);
  emit_store_local_n(def, new_idx, idx_int_ptr);

  // lua_Integer limit = ivalue(ra + 1);
  llvm::Instruction *limit_int_value = emit_load_local_n(def, limit_int_ptr);

  // idx > limit?
  llvm::Value *new_idx_gt_limit =
      def->builder->CreateICmpSGT(new_idx, limit_int_value, "idx.gt.limit");

  // If idx > limit we are done
  llvm::BasicBlock *update_block =
      llvm::BasicBlock::Create(def->jitState->context(), "updatei");
  def->builder->CreateCondBr(new_idx_gt_limit, exit_block, update_block);

  // NOW INTEGER step < 0

  def->f->getBasicBlockList().push_back(b.jmp2);
  def->builder->SetInsertPoint(b.jmp2);

  step_int_value = emit_load_local_n(def, step_int_ptr);

  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  idx_int_value = emit_load_local_n(def, idx_int_ptr);
  new_idx = def->builder->CreateAdd(step_int_value, idx_int_value, "next.idx",
                                    false, true);
  emit_store_local_n(def, new_idx, idx_int_ptr);

  // lua_Integer limit = ivalue(ra + 1);
  limit_int_value = emit_load_local_n(def, limit_int_ptr);

  // limit > idx?
  llvm::Value *limit_gt_idx =
      def->builder->CreateICmpSGT(limit_int_value, new_idx, "limit.gt.idx");

  // If limit > idx we are done
  def->builder->CreateCondBr(limit_gt_idx, exit_block, update_block);

  // Merge into update block
  def->f->getBasicBlockList().push_back(update_block);
  def->builder->SetInsertPoint(update_block);

  emit_load_base(def);

  llvm::Value *rvar = emit_gep_register(def, A + 3);

  //    setivalue(ra + 3, idx);  /* ...and external index */
  idx_int_value = emit_load_local_n(def, idx_int_ptr);

  emit_store_reg_i_withtype(def, idx_int_value, rvar);

  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);

  // FLOATING CASE

  def->f->getBasicBlockList().push_back(b.jmp3);
  def->builder->SetInsertPoint(b.jmp3);

  //  lua_Number step = fltvalue(ra + 2);
  llvm::Instruction *step_double_value =
      emit_load_local_n(def, step_double_ptr);

  //  lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
  llvm::Instruction *idx_double_value = emit_load_local_n(def, idx_double_ptr);
  new_idx =
      def->builder->CreateFAdd(step_double_value, idx_double_value, "next.idx");
  emit_store_local_n(def, new_idx, idx_double_ptr);

  //  lua_Number limit = fltvalue(ra + 1);
  llvm::Instruction *limit_double_value =
      emit_load_local_n(def, limit_double_ptr);

  // step > 0?
  // idx > limit?
  new_idx_gt_limit =
      def->builder->CreateFCmpOGT(new_idx, limit_double_value, "idx.gt.limit");

  // If idx > limit we are done
  update_block = llvm::BasicBlock::Create(def->jitState->context(), "updatef");
  def->builder->CreateCondBr(new_idx_gt_limit, exit_block, update_block);

  def->f->getBasicBlockList().push_back(b.jmp4);
  def->builder->SetInsertPoint(b.jmp4);

  //  lua_Number step = fltvalue(ra + 2);
  step_double_value = emit_load_local_n(def, step_double_ptr);

  //  lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
  idx_double_value = emit_load_local_n(def, idx_double_ptr);
  new_idx =
      def->builder->CreateFAdd(step_double_value, idx_double_value, "next.idx");
  emit_store_local_n(def, new_idx, idx_double_ptr);

  //  lua_Number limit = fltvalue(ra + 1);
  limit_double_value = emit_load_local_n(def, limit_double_ptr);

  // limit > idx?
  limit_gt_idx =
      def->builder->CreateFCmpOGT(limit_double_value, new_idx, "limit.gt.idx");

  // If limit > idx we are done
  def->builder->CreateCondBr(limit_gt_idx, exit_block, update_block);

  // Merge into update block
  def->f->getBasicBlockList().push_back(update_block);
  def->builder->SetInsertPoint(update_block);

  emit_load_base(def);
  rvar = emit_gep_register(def, A + 3);

  //    setfltvalue(ra + 3, idx);  /* ...and external index */
  idx_double_value = emit_load_local_n(def, idx_double_ptr);

  emit_store_reg_n_withtype(def, idx_double_value, rvar);

  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);

  def->f->getBasicBlockList().push_back(exit_block);
  def->builder->SetInsertPoint(exit_block);
}

void RaviCodeGenerator::emit_FORLOOP(RaviFunctionDef *def, int A, int pc,
                                     int pc1) {

  // 7[1]     FORLOOP         1 - 2; to 6
  // if (ttisinteger(ra)) {  /* integer loop? */
  //  lua_Integer step = ivalue(ra + 2);
  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  //  lua_Integer limit = ivalue(ra + 1);
  //  if ((0 < step) ? (idx <= limit) : (limit <= idx)) {
  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    setivalue(ra, idx);  /* update internal index... */
  //    setivalue(ra + 3, idx);  /* ...and external index */
  //  }
  //}
  // else {  /* floating loop */
  //  lua_Number step = fltvalue(ra + 2);
  //  lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
  //  lua_Number limit = fltvalue(ra + 1);
  //  if (luai_numlt(0, step) ? luai_numle(idx, limit)
  //    : luai_numle(limit, idx)) {
  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    setfltvalue(ra, idx);  /* update internal index... */
  //    setfltvalue(ra + 3, idx);  /* ...and external index */
  //  }
  //}

  emit_debug_trace(def, OP_FORLOOP, pc1);

  // Load pointer to base
  emit_load_base(def);

  //  TValue *rinit = ra;
  //  TValue *rlimit = ra + 1;
  //  TValue *rstep = ra + 2;
  //  TValue *rvar = ra + 3
  llvm::Value *rinit = emit_gep_register(def, A);
  llvm::Value *rlimit = emit_gep_register(def, A + 1);
  llvm::Value *rstep = emit_gep_register(def, A + 2);
  llvm::Value *rvar = emit_gep_register(def, A + 3);

  // Create the done block
  llvm::BasicBlock *exit_block =
      llvm::BasicBlock::Create(def->jitState->context(), "exit_forloop");

  // Is index an integer?
  llvm::Instruction *rinit_tt = emit_load_type(def, rinit);
  llvm::Value *cmp1 =
      emit_is_value_of_type(def, rinit_tt, LUA__TNUMINT, "init.is.integer");

  // Setup if then else branch for integer
  llvm::BasicBlock *if_integer =
      llvm::BasicBlock::Create(def->jitState->context(), "if.integer", def->f);
  llvm::BasicBlock *else_integer =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.integer");
  def->builder->CreateCondBr(cmp1, if_integer, else_integer);
  def->builder->SetInsertPoint(if_integer);

  // INTEGER CASE

  //  lua_Integer step = ivalue(ra + 2);
  llvm::Instruction *step_int_value = emit_load_reg_i(def, rstep);

  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  llvm::Instruction *idx_int_value = emit_load_reg_i(def, rinit);
  llvm::Value *new_idx = def->builder->CreateAdd(step_int_value, idx_int_value,
                                                 "next.idx", false, true);

  // lua_Integer limit = ivalue(ra + 1);
  llvm::Instruction *limit_int_value = emit_load_reg_i(def, rlimit);

  // step > 0?
  llvm::Value *step_gt_zero = def->builder->CreateICmpSGT(
      step_int_value, def->types->kluaInteger[0], "step.gt.zero");

  llvm::BasicBlock *step_gt_zero_true = llvm::BasicBlock::Create(
      def->jitState->context(), "step.gt.zero.true", def->f);
  llvm::BasicBlock *step_gt_zero_false =
      llvm::BasicBlock::Create(def->jitState->context(), "step.gt.zero.false");
  def->builder->CreateCondBr(step_gt_zero, step_gt_zero_true,
                             step_gt_zero_false);
  def->builder->SetInsertPoint(step_gt_zero_true);

  // idx > limit?
  llvm::Value *new_idx_gt_limit =
      def->builder->CreateICmpSGT(new_idx, limit_int_value, "idx.gt.limit");

  // If idx > limit we are done
  llvm::BasicBlock *update_block =
      llvm::BasicBlock::Create(def->jitState->context(), "update");
  def->builder->CreateCondBr(new_idx_gt_limit, exit_block, update_block);

  def->f->getBasicBlockList().push_back(step_gt_zero_false);
  def->builder->SetInsertPoint(step_gt_zero_false);

  // limit > idx?
  llvm::Value *limit_gt_idx =
      def->builder->CreateICmpSGT(limit_int_value, new_idx, "limit.gt.idx");

  // If limit > idx we are done
  def->builder->CreateCondBr(limit_gt_idx, exit_block, update_block);

  // Merge into update block
  def->f->getBasicBlockList().push_back(update_block);
  def->builder->SetInsertPoint(update_block);

  //    setivalue(ra, idx);  /* update internal index... */
  emit_store_reg_i(def, new_idx, rinit);

  //    setivalue(ra + 3, idx);  /* ...and external index */
  emit_store_reg_i_withtype(def, new_idx, rvar);

  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);

  // FLOATING CASE

  def->f->getBasicBlockList().push_back(else_integer);
  def->builder->SetInsertPoint(else_integer);

  //  lua_Number step = fltvalue(ra + 2);
  llvm::Instruction *step_double_value = emit_load_reg_n(def, rstep);

  //  lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
  llvm::Instruction *idx_double_value = emit_load_reg_n(def, rinit);

  new_idx =
      def->builder->CreateFAdd(step_double_value, idx_double_value, "next.idx");

  //  lua_Number limit = fltvalue(ra + 1);
  llvm::Instruction *limit_double_value = emit_load_reg_n(def, rlimit);

  // step > 0?
  step_gt_zero = def->builder->CreateFCmpOGT(
      step_double_value, llvm::ConstantFP::get(def->types->lua_NumberT, 0.0),
      "step.gt.zero");

  step_gt_zero_true = llvm::BasicBlock::Create(def->jitState->context(),
                                               "step.gt.zero.true", def->f);
  step_gt_zero_false =
      llvm::BasicBlock::Create(def->jitState->context(), "step.gt.zero.false");
  def->builder->CreateCondBr(step_gt_zero, step_gt_zero_true,
                             step_gt_zero_false);
  def->builder->SetInsertPoint(step_gt_zero_true);

  // idx > limit?
  new_idx_gt_limit =
      def->builder->CreateFCmpOGT(new_idx, limit_double_value, "idx.gt.limit");

  // If idx > limit we are done
  update_block = llvm::BasicBlock::Create(def->jitState->context(), "update");
  def->builder->CreateCondBr(new_idx_gt_limit, exit_block, update_block);

  def->f->getBasicBlockList().push_back(step_gt_zero_false);
  def->builder->SetInsertPoint(step_gt_zero_false);

  // limit > idx?
  limit_gt_idx =
      def->builder->CreateFCmpOGT(limit_double_value, new_idx, "limit.gt.idx");

  // If limit > idx we are done
  def->builder->CreateCondBr(limit_gt_idx, exit_block, update_block);

  // Merge into update block
  def->f->getBasicBlockList().push_back(update_block);
  def->builder->SetInsertPoint(update_block);

  //    setfltvalue(ra, idx);  /* update internal index... */
  emit_store_reg_n(def, new_idx, rinit);

  //    setfltvalue(ra + 3, idx);  /* ...and external index */
  emit_store_reg_n_withtype(def, new_idx, rvar);

  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);

  def->f->getBasicBlockList().push_back(exit_block);
  def->builder->SetInsertPoint(exit_block);
}

void RaviCodeGenerator::emit_iFORLOOP(RaviFunctionDef *def, int A, int pc,
                                      RaviBranchDef &b, int step_one, int pc1) {

  //  lua_Integer step = ivalue(ra + 2);
  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  //  lua_Integer limit = ivalue(ra + 1);
  //  if (idx <= limit) {
  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    setivalue(ra, idx);  /* update internal index... */
  //    setivalue(ra + 3, idx);  /* ...and external index */
  //  }

  emit_debug_trace(def, step_one ? OP_RAVI_FORLOOP_I1 : OP_RAVI_FORLOOP_IP,
                   pc1);
  // We are in b.jmp1 as this is already the current block
  lua_assert(def->builder->GetInsertBlock() == b.jmp1);

  // Obtain pointers to the value.i field
  llvm::Value *idx_int_ptr = b.iidx;
  llvm::Value *limit_int_ptr = b.ilimit;

  // Create the done block
  llvm::BasicBlock *exit_block =
      llvm::BasicBlock::Create(def->jitState->context(), "exit_iforloop");

  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  llvm::Instruction *idx_int_value = emit_load_local_n(def, idx_int_ptr);
  llvm::Value *new_idx;

  if (!step_one) {
    //  lua_Integer step = ivalue(ra + 2);
    llvm::Value *step_int_ptr = b.istep;
    llvm::Instruction *step_int_value = emit_load_local_n(def, step_int_ptr);
    new_idx = def->builder->CreateAdd(step_int_value, idx_int_value, "next.idx",
                                      false, true);
  } else
    new_idx = def->builder->CreateAdd(def->types->kluaInteger[1], idx_int_value,
                                      "next.idx", false, true);

  // save new index
  emit_store_local_n(def, new_idx, idx_int_ptr);

  // lua_Integer limit = ivalue(ra + 1);
  llvm::Instruction *limit_int_value = emit_load_local_n(def, limit_int_ptr);

  // idx > limit?
  llvm::Value *new_idx_gt_limit =
      def->builder->CreateICmpSGT(new_idx, limit_int_value, "idx.gt.limit");

  // If idx > limit we are done
  llvm::BasicBlock *update_block =
      llvm::BasicBlock::Create(def->jitState->context(), "updatei");
  def->builder->CreateCondBr(new_idx_gt_limit, exit_block, update_block);

  // Merge into update block
  def->f->getBasicBlockList().push_back(update_block);
  def->builder->SetInsertPoint(update_block);

  // Load pointer to base
  emit_load_base(def);

  llvm::Value *rvar = emit_gep_register(def, A + 3);

  //    setivalue(ra + 3, idx);  /* ...and external index */
  idx_int_value = emit_load_local_n(def, idx_int_ptr);
  emit_store_reg_i_withtype(def, idx_int_value, rvar);

  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);

  def->f->getBasicBlockList().push_back(exit_block);
  def->builder->SetInsertPoint(exit_block);
}
}
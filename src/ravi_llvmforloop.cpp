#include "ravi_llvmcodegen.h"

namespace ravi {

  void RaviCodeGenerator::emit_FORLOOP2(RaviFunctionDef *def, llvm::Value *L_ci,
    llvm::Value *proto, int A, int pc, RaviBranchDef& b) {

    // 7[1]     FORLOOP         1 - 2; to 6
    //if (ttisinteger(ra)) {  /* integer loop? */
    //  lua_Integer step = ivalue(ra + 2);
    //  lua_Integer idx = ivalue(ra) + step; /* increment index */
    //  lua_Integer limit = ivalue(ra + 1);
    //  if ((0 < step) ? (idx <= limit) : (limit <= idx)) {
    //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
    //    setivalue(ra, idx);  /* update internal index... */
    //    setivalue(ra + 3, idx);  /* ...and external index */
    //  }
    //}
    //else {  /* floating loop */
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
    llvm::BasicBlock *exit_block = llvm::BasicBlock::Create(def->jitState->context(), "exit_iforloop");

    // INTEGER CASE

    //  lua_Integer step = ivalue(ra + 2);
    llvm::Instruction *step_int_value = def->builder->CreateLoad(step_int_ptr, "step.i");
    step_int_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    //  lua_Integer idx = ivalue(ra) + step; /* increment index */
    llvm::Instruction *idx_int_value = def->builder->CreateLoad(idx_int_ptr, "init.i");
    idx_int_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
    llvm::Value *new_idx = def->builder->CreateAdd(step_int_value, idx_int_value, "next.idx", false, true);
    llvm::Instruction *idx_store = def->builder->CreateStore(new_idx, idx_int_ptr);
    idx_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    // lua_Integer limit = ivalue(ra + 1);
    llvm::Instruction *limit_int_value = def->builder->CreateLoad(limit_int_ptr, "limit.i");
    limit_int_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    // idx > limit?
    llvm::Value *new_idx_gt_limit = def->builder->CreateICmpSGT(new_idx, limit_int_value, "idx.gt.limit");

    // If idx > limit we are done
    llvm::BasicBlock *update_block = llvm::BasicBlock::Create(
      def->jitState->context(), "updatei");
    def->builder->CreateCondBr(new_idx_gt_limit, exit_block, update_block);

    // NOW INTEGER step < 0 

    def->f->getBasicBlockList().push_back(b.jmp2);
    def->builder->SetInsertPoint(b.jmp2);

    step_int_value = def->builder->CreateLoad(step_int_ptr, "step.i");
    step_int_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    //  lua_Integer idx = ivalue(ra) + step; /* increment index */
    idx_int_value = def->builder->CreateLoad(idx_int_ptr, "init.i");
    idx_int_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
    new_idx = def->builder->CreateAdd(step_int_value, idx_int_value, "next.idx", false, true);
    idx_store = def->builder->CreateStore(new_idx, idx_int_ptr);
    idx_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    // lua_Integer limit = ivalue(ra + 1);
    limit_int_value = def->builder->CreateLoad(limit_int_ptr, "limit.i");
    limit_int_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    // limit > idx?
    llvm::Value *limit_gt_idx = def->builder->CreateICmpSGT(limit_int_value, new_idx, "limit.gt.idx");

    // If limit > idx we are done
    def->builder->CreateCondBr(limit_gt_idx, exit_block, update_block);

    // Merge into update block
    def->f->getBasicBlockList().push_back(update_block);
    def->builder->SetInsertPoint(update_block);

    llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
    base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
      def->types->tbaa_luaState_ci_baseT);
    llvm::Value *rvar = emit_array_get(def, base_ptr, A + 3);
    llvm::Value *rvar_tt_ptr = emit_gep(def, "var.tt.ptr", rvar, 0, 1);
    llvm::Value *var_int_ptr = def->builder->CreateBitCast(rvar, def->types->plua_IntegerT, "var.i");

    //    setivalue(ra + 3, idx);  /* ...and external index */
    idx_int_value = def->builder->CreateLoad(idx_int_ptr, "init.i");
    idx_int_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
    idx_store = def->builder->CreateStore(idx_int_value, var_int_ptr);
    idx_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
    llvm::Instruction *idx_tt_store = def->builder->CreateStore(def->types->kInt[LUA_TNUMINT], rvar_tt_ptr);
    idx_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

    //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
    def->builder->CreateBr(def->jmp_targets[pc].jmp1);

    // FLOATING CASE

    def->f->getBasicBlockList().push_back(b.jmp3);
    def->builder->SetInsertPoint(b.jmp3);

    //  lua_Number step = fltvalue(ra + 2);
    llvm::Instruction *step_double_value = def->builder->CreateLoad(step_double_ptr, "step.n");
    step_double_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    //  lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
    llvm::Instruction *idx_double_value = def->builder->CreateLoad(idx_double_ptr, "init.n");
    idx_double_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
    new_idx = def->builder->CreateFAdd(step_double_value, idx_double_value, "next.idx");
    idx_store = def->builder->CreateStore(new_idx, idx_double_ptr);
    idx_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    //  lua_Number limit = fltvalue(ra + 1);
    llvm::Instruction *limit_double_value = def->builder->CreateLoad(limit_double_ptr, "limit.n");
    limit_double_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    // step > 0?
    // idx > limit?
    new_idx_gt_limit = def->builder->CreateFCmpOGT(new_idx, limit_double_value, "idx.gt.limit");

    // If idx > limit we are done
    update_block = llvm::BasicBlock::Create(
      def->jitState->context(), "updatef");
    def->builder->CreateCondBr(new_idx_gt_limit, exit_block, update_block);

    def->f->getBasicBlockList().push_back(b.jmp4);
    def->builder->SetInsertPoint(b.jmp4);

    //  lua_Number step = fltvalue(ra + 2);
    step_double_value = def->builder->CreateLoad(step_double_ptr, "step.n");
    step_double_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    //  lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
    idx_double_value = def->builder->CreateLoad(idx_double_ptr, "init.n");
    idx_double_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
    new_idx = def->builder->CreateFAdd(step_double_value, idx_double_value, "next.idx");
    idx_store = def->builder->CreateStore(new_idx, idx_double_ptr);
    idx_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    //  lua_Number limit = fltvalue(ra + 1);
    limit_double_value = def->builder->CreateLoad(limit_double_ptr, "limit.n");
    limit_double_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

    // limit > idx?
    limit_gt_idx = def->builder->CreateFCmpOGT(limit_double_value, new_idx, "limit.gt.idx");

    // If limit > idx we are done
    def->builder->CreateCondBr(limit_gt_idx, exit_block, update_block);

    // Merge into update block
    def->f->getBasicBlockList().push_back(update_block);
    def->builder->SetInsertPoint(update_block);

    base_ptr = def->builder->CreateLoad(def->Ci_base);
    base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
      def->types->tbaa_luaState_ci_baseT);
    rvar = emit_array_get(def, base_ptr, A + 3);
    rvar_tt_ptr = emit_gep(def, "var.tt.ptr", rvar, 0, 1);
    llvm::Value *var_double_ptr = def->builder->CreateBitCast(rvar, def->types->plua_NumberT, "var.n");

    //    setfltvalue(ra + 3, idx);  /* ...and external index */
    idx_double_value = def->builder->CreateLoad(idx_double_ptr, "init.n");
    idx_double_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
    idx_store = def->builder->CreateStore(idx_double_value, var_double_ptr);
    idx_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
    idx_tt_store = def->builder->CreateStore(def->types->kInt[LUA_TNUMFLT], rvar_tt_ptr);
    idx_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

    //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
    def->builder->CreateBr(def->jmp_targets[pc].jmp1);

    def->f->getBasicBlockList().push_back(exit_block);
    def->builder->SetInsertPoint(exit_block);

    //def->f->dump();
    //assert(false);

  }

void RaviCodeGenerator::emit_FORLOOP(RaviFunctionDef *def, llvm::Value *L_ci,
                                     llvm::Value *proto, int A, int pc) {


  // 7[1]     FORLOOP         1 - 2; to 6
  //if (ttisinteger(ra)) {  /* integer loop? */
  //  lua_Integer step = ivalue(ra + 2);
  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  //  lua_Integer limit = ivalue(ra + 1);
  //  if ((0 < step) ? (idx <= limit) : (limit <= idx)) {
  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    setivalue(ra, idx);  /* update internal index... */
  //    setivalue(ra + 3, idx);  /* ...and external index */
  //  }
  //}
  //else {  /* floating loop */
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

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
    def->types->tbaa_luaState_ci_baseT);

  //  TValue *rinit = ra;
  //  TValue *rlimit = ra + 1;
  //  TValue *rstep = ra + 2;
  //  TValue *rvar = ra + 3
  llvm::Value *rinit = A == 0 ? base_ptr : emit_array_get(def, base_ptr, A);
  llvm::Value *rlimit = emit_array_get(def, base_ptr, A + 1);
  llvm::Value *rstep = emit_array_get(def, base_ptr, A + 2);
  llvm::Value *rvar = emit_array_get(def, base_ptr, A + 3);

  // Obtain pointers to the tt_ field
  llvm::Value *rinit_tt_ptr = emit_gep(def, "init.tt.ptr", rinit, 0, 1);
  llvm::Value *rlimit_tt_ptr = emit_gep(def, "limit.tt.ptr", rlimit, 0, 1);
  llvm::Value *rstep_tt_ptr = emit_gep(def, "step.tt.ptr", rstep, 0, 1);
  llvm::Value *rvar_tt_ptr = emit_gep(def, "var.tt.ptr", rvar, 0, 1);

  // Obtain pointers to the value.i field
  llvm::Value *step_int_ptr = def->builder->CreateBitCast(rstep, def->types->plua_IntegerT, "step.i");
  llvm::Value *idx_int_ptr = def->builder->CreateBitCast(rinit, def->types->plua_IntegerT, "init.i");
  llvm::Value *limit_int_ptr = def->builder->CreateBitCast(rlimit, def->types->plua_IntegerT, "limit.i");
  llvm::Value *var_int_ptr = def->builder->CreateBitCast(rvar, def->types->plua_IntegerT, "var.i");
  // Obtain pointers to the value.n field
  llvm::Value *step_double_ptr = def->builder->CreateBitCast(rstep, def->types->plua_NumberT, "step.n");
  llvm::Value *idx_double_ptr = def->builder->CreateBitCast(rinit, def->types->plua_NumberT, "init.n");
  llvm::Value *limit_double_ptr = def->builder->CreateBitCast(rlimit, def->types->plua_NumberT, "limit.n");
  llvm::Value *var_double_ptr = def->builder->CreateBitCast(rvar, def->types->plua_NumberT, "var.n");

  // Create the done block
  llvm::BasicBlock *exit_block = llvm::BasicBlock::Create(def->jitState->context(), "exit_forloop");

  // Is index an integer?
  llvm::Instruction *rinit_tt = def->builder->CreateLoad(rinit_tt_ptr);
  rinit_tt->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  llvm::Value *cmp1 =
    def->builder->CreateICmpEQ(rinit_tt, def->types->kInt[LUA_TNUMINT], "is.integer");

  // Setup if then else branch for integer
  llvm::BasicBlock *if_integer = llvm::BasicBlock::Create(
    def->jitState->context(), "if.integer", def->f);
  llvm::BasicBlock *else_integer =
    llvm::BasicBlock::Create(def->jitState->context(), "if.not.integer");
  def->builder->CreateCondBr(cmp1, if_integer, else_integer);
  def->builder->SetInsertPoint(if_integer);

  // INTEGER CASE

  //  lua_Integer step = ivalue(ra + 2);
  llvm::Instruction *step_int_value = def->builder->CreateLoad(step_int_ptr, "step.i");
  step_int_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);

  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  llvm::Instruction *idx_int_value = def->builder->CreateLoad(idx_int_ptr, "init.i");
  idx_int_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Value *new_idx = def->builder->CreateAdd(step_int_value, idx_int_value, "next.idx", false, true);

  // lua_Integer limit = ivalue(ra + 1);
  llvm::Instruction *limit_int_value = def->builder->CreateLoad(limit_int_ptr, "limit.i");
  limit_int_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);

  // step > 0?
  llvm::Value *step_gt_zero = def->builder->CreateICmpSGT(step_int_value, def->types->kluaInteger[0], "step.gt.zero");

  llvm::BasicBlock *step_gt_zero_true = llvm::BasicBlock::Create(
    def->jitState->context(), "step.gt.zero.true", def->f);
  llvm::BasicBlock *step_gt_zero_false =
    llvm::BasicBlock::Create(def->jitState->context(), "step.gt.zero.false");
  def->builder->CreateCondBr(step_gt_zero, step_gt_zero_true, step_gt_zero_false);
  def->builder->SetInsertPoint(step_gt_zero_true);

  // idx > limit?
  llvm::Value *new_idx_gt_limit = def->builder->CreateICmpSGT(new_idx, limit_int_value, "idx.gt.limit");

  // If idx > limit we are done
  llvm::BasicBlock *update_block = llvm::BasicBlock::Create(
    def->jitState->context(), "update");
  def->builder->CreateCondBr(new_idx_gt_limit, exit_block, update_block);
  
  def->f->getBasicBlockList().push_back(step_gt_zero_false);
  def->builder->SetInsertPoint(step_gt_zero_false);

  // limit > idx?
  llvm::Value *limit_gt_idx = def->builder->CreateICmpSGT(limit_int_value, new_idx, "limit.gt.idx");

  // If limit > idx we are done
  def->builder->CreateCondBr(limit_gt_idx, exit_block, update_block);

  // Merge into update block
  def->f->getBasicBlockList().push_back(update_block);
  def->builder->SetInsertPoint(update_block);

  //    setivalue(ra, idx);  /* update internal index... */
  llvm::Instruction *idx_store = def->builder->CreateStore(new_idx, idx_int_ptr);
  idx_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Instruction *idx_tt_store;
  //idx_tt_store = def->builder->CreateStore(def->types->kInt[LUA_TNUMINT], rinit_tt_ptr);
  //idx_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

  //    setivalue(ra + 3, idx);  /* ...and external index */
  idx_store = def->builder->CreateStore(new_idx, var_int_ptr);
  idx_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  idx_tt_store = def->builder->CreateStore(def->types->kInt[LUA_TNUMINT], rvar_tt_ptr);
  idx_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);

  // FLOATING CASE

  def->f->getBasicBlockList().push_back(else_integer);
  def->builder->SetInsertPoint(else_integer);

  //  lua_Number step = fltvalue(ra + 2);
  llvm::Instruction *step_double_value = def->builder->CreateLoad(step_double_ptr, "step.n");
  step_double_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);

  //  lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
  llvm::Instruction *idx_double_value = def->builder->CreateLoad(idx_double_ptr, "init.n");
  idx_double_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  new_idx = def->builder->CreateFAdd(step_double_value, idx_double_value, "next.idx");

  //  lua_Number limit = fltvalue(ra + 1);
  llvm::Instruction *limit_double_value = def->builder->CreateLoad(limit_double_ptr, "limit.n");
  limit_double_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);

  // step > 0?
  step_gt_zero = def->builder->CreateFCmpOGT(step_double_value, llvm::ConstantFP::get(def->types->lua_NumberT, 0.0), "step.gt.zero");

  step_gt_zero_true = llvm::BasicBlock::Create(
    def->jitState->context(), "step.gt.zero.true", def->f);
  step_gt_zero_false =
    llvm::BasicBlock::Create(def->jitState->context(), "step.gt.zero.false");
  def->builder->CreateCondBr(step_gt_zero, step_gt_zero_true, step_gt_zero_false);
  def->builder->SetInsertPoint(step_gt_zero_true);

  // idx > limit?
  new_idx_gt_limit = def->builder->CreateFCmpOGT(new_idx, limit_double_value, "idx.gt.limit");

  // If idx > limit we are done
  update_block = llvm::BasicBlock::Create(
    def->jitState->context(), "update");
  def->builder->CreateCondBr(new_idx_gt_limit, exit_block, update_block);

  def->f->getBasicBlockList().push_back(step_gt_zero_false);
  def->builder->SetInsertPoint(step_gt_zero_false);

  // limit > idx?
  limit_gt_idx = def->builder->CreateFCmpOGT(limit_double_value, new_idx, "limit.gt.idx");

  // If limit > idx we are done
  def->builder->CreateCondBr(limit_gt_idx, exit_block, update_block);

  // Merge into update block
  def->f->getBasicBlockList().push_back(update_block);
  def->builder->SetInsertPoint(update_block);

  //    setfltvalue(ra, idx);  /* update internal index... */
  idx_store = def->builder->CreateStore(new_idx, idx_double_ptr);
  idx_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  //idx_tt_store = def->builder->CreateStore(def->types->kInt[LUA_TNUMFLT], rinit_tt_ptr);
  //idx_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

  //    setfltvalue(ra + 3, idx);  /* ...and external index */
  idx_store = def->builder->CreateStore(new_idx, var_double_ptr);
  idx_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  idx_tt_store = def->builder->CreateStore(def->types->kInt[LUA_TNUMFLT], rvar_tt_ptr);
  idx_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);

  def->f->getBasicBlockList().push_back(exit_block);
  def->builder->SetInsertPoint(exit_block);

}

}
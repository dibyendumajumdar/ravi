#include "ravi_llvmcodegen.h"

namespace ravi {

void RaviCodeGenerator::emit_FORPREP2(RaviFunctionDef *def, llvm::Value *L_ci,
                                      llvm::Value *proto, int A, int pc) {

  // Create additional forloop targets
  // First target is for int < limit
  RaviBranchDef &forloop_target = def->jmp_targets[pc];

  // case OP_FORPREP: {
  //  if (ttisinteger(init) && ttisinteger(pstep) &&
  //    forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {
  //    /* all values are integer */
  //    lua_Integer initv = (stopnow ? 0 : ivalue(init));
  //    setivalue(plimit, ilimit);
  //    setivalue(init, initv - ivalue(pstep));
  //  }
  //  else {  /* try making all values floats */
  //    if (!tonumber(plimit, &nlimit))
  //      luaG_runerror(L, "'for' limit must be a number");
  //    setfltvalue(plimit, nlimit);
  //    if (!tonumber(pstep, &nstep))
  //      luaG_runerror(L, "'for' step must be a number");
  //    setfltvalue(pstep, nstep);
  //    if (!tonumber(init, &ninit))
  //      luaG_runerror(L, "'for' initial value must be a number");
  //    setfltvalue(init, luai_numsub(L, ninit, nstep));
  //  }
  //  ci->u.l.savedpc += GETARG_sBx(i);
  //} break;

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base, "base");
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_luaState_ci_baseT);

  //  lua_Integer ilimit;
  //  int stopnow;
  //  lua_Number ninit; lua_Number nlimit; lua_Number nstep;

  // Setup local vars on C stack and link them
  // to the forloop_target so that the forloop code can access these

  llvm::Value *stopnow =
      def->builder->CreateAlloca(def->types->C_intT, nullptr, "stopnow");

  forloop_target.ilimit =
      def->builder->CreateAlloca(def->types->lua_IntegerT, nullptr, "ilimit");
  forloop_target.istep =
      def->builder->CreateAlloca(def->types->lua_IntegerT, nullptr, "istep");
  forloop_target.iidx =
      def->builder->CreateAlloca(def->types->lua_IntegerT, nullptr, "iidx");
  forloop_target.flimit =
      def->builder->CreateAlloca(def->types->lua_NumberT, nullptr, "nlimit");
  forloop_target.fidx =
      def->builder->CreateAlloca(def->types->lua_NumberT, nullptr, "ninit");
  forloop_target.fstep =
      def->builder->CreateAlloca(def->types->lua_NumberT, nullptr, "nstep");
  forloop_target.forloop_branch =
      def->builder->CreateAlloca(def->types->C_pcharT, nullptr, "brnch");

  llvm::Value *isint = def->builder->CreateAlloca(
      llvm::Type::getInt1Ty(def->jitState->context()), nullptr,
      "loop.is.integer");
  llvm::Value *isinc = def->builder->CreateAlloca(
      llvm::Type::getInt1Ty(def->jitState->context()), nullptr,
      "loop.increasing");

  //  TValue *init = ra;
  //  TValue *plimit = ra + 1;
  //  TValue *pstep = ra + 2;
  llvm::Value *init = A == 0 ? base_ptr : emit_array_get(def, base_ptr, A);
  llvm::Value *plimit = emit_array_get(def, base_ptr, A + 1);
  llvm::Value *pstep = emit_array_get(def, base_ptr, A + 2);

  //  if (ttisinteger(init) && ttisinteger(pstep) &&
  //    forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {

  // Get init->tt
  llvm::Value *pinit_tt_ptr = emit_gep(def, "init.tt.ptr", init, 0, 1);
  llvm::Instruction *pinit_tt =
      def->builder->CreateLoad(pinit_tt_ptr, "init.tt");
  pinit_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_TValue_ttT);

  // Compare init->tt == LUA_TNUMINT
  llvm::Value *cmp1 = def->builder->CreateICmpEQ(
      pinit_tt, def->types->kInt[LUA_TNUMINT], "init.is.integer");

  // Get pstep->tt
  llvm::Value *pstep_tt_ptr = emit_gep(def, "step.tt.ptr", pstep, 0, 1);
  llvm::Instruction *pstep_tt =
      def->builder->CreateLoad(pstep_tt_ptr, "step.tt");
  pstep_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_TValue_ttT);

  // Compare pstep->tt == LUA_TNUMINT
  llvm::Value *icmp2 = def->builder->CreateICmpEQ(
      pstep_tt, def->types->kInt[LUA_TNUMINT], "step.is.integer");

  // Get ivalue(pstep)
  llvm::Value *pstep_ivalue_ptr = def->builder->CreateBitCast(
      pstep, def->types->plua_IntegerT, "step.i.ptr");
  llvm::Instruction *pstep_ivalue =
      def->builder->CreateLoad(pstep_ivalue_ptr, "step.i");
  pstep_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_TValue_nT);

  // Call forlimit()
  llvm::Value *forlimit_ret =
      def->builder->CreateCall4(def->luaV_forlimitF, plimit,
                                forloop_target.ilimit, pstep_ivalue, stopnow);

  // Is init->tt == LUA_TNUMINT && pstep->tt == LUA_TNUMINT
  llvm::Value *and1 =
      def->builder->CreateAnd(cmp1, icmp2, "init.and.step.are.integers");

  // Convert result from forlimit() to bool
  llvm::Value *tobool =
      def->builder->CreateICmpNE(forlimit_ret, def->types->kInt[0]);

  // Are all vars integers?
  // init->tt == LUA_TNUMINT && pstep->tt == LUA_TNUMINT && forlimit()
  llvm::Value *and2 = def->builder->CreateAnd(and1, tobool, "all.integers");

  // Save outcome in forloop target - NOT NEEDED?
  llvm::Instruction *bInt_store = def->builder->CreateStore(and2, isint);
  bInt_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_charT);

  // Create if then else branch
  llvm::BasicBlock *then1 = llvm::BasicBlock::Create(def->jitState->context(),
                                                     "if.all.integers", def->f);
  llvm::BasicBlock *else1 =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.all.integers");
  def->builder->CreateCondBr(and2, then1, else1);
  def->builder->SetInsertPoint(then1);

  //    all values are integers
  //    lua_Integer initv = (stopnow ? 0 : ivalue(init));
  // Save step
  llvm::Instruction *istep_store =
    def->builder->CreateStore(pstep_ivalue, forloop_target.istep);
  istep_store->setMetadata(llvm::LLVMContext::MD_tbaa,
    def->types->tbaa_longlongT);

  // Get stopnow
  llvm::Instruction *stopnow_val =
      def->builder->CreateLoad(stopnow, "stopnow.value");
  stopnow_val->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_intT);

  // Test if stopnow is 0
  llvm::Value *stopnow_is_zero = def->builder->CreateICmpEQ(
      stopnow_val, def->types->kInt[0], "stopnow.is.zero");

  // Get ptr to init->i
  llvm::Value *init_value_ptr = def->builder->CreateBitCast(
      init, def->types->plua_IntegerT, "init.i.ptr");

  // Setup if then else branch for stopnow
  llvm::BasicBlock *then1_iffalse = llvm::BasicBlock::Create(
      def->jitState->context(), "if.stopnow.iszero", def->f);
  llvm::BasicBlock *then1_iftrue =
      llvm::BasicBlock::Create(def->jitState->context(), "if.stopnow.notzero");
  def->builder->CreateCondBr(stopnow_is_zero, then1_iffalse, then1_iftrue);
  def->builder->SetInsertPoint(then1_iffalse);

  // stopnow is 0
  // Get init->i
  llvm::Instruction *init_ivalue =
      def->builder->CreateLoad(init_value_ptr, "init.i");
  init_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_TValue_nT);

  // Join after the branch
  def->builder->CreateBr(then1_iftrue);
  def->f->getBasicBlockList().push_back(then1_iftrue);
  def->builder->SetInsertPoint(then1_iftrue);

  // Set initv to 0 if !stopnow else init->i
  auto phi1 = def->builder->CreatePHI(def->types->lua_IntegerT, 2, "initv");
  phi1->addIncoming(init_ivalue, then1_iffalse);
  phi1->addIncoming(def->types->kluaInteger[0], then1);

  //    setivalue(init, initv - ivalue(pstep));
  // we aleady know init is LUA_TNUMINT

  llvm::Value *sub =
      def->builder->CreateSub(phi1, pstep_ivalue, "initv-pstep.i", false, true);
  llvm::Instruction *init_ivalue_store =
      def->builder->CreateStore(sub, forloop_target.iidx);
  init_ivalue_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                                 def->types->tbaa_longlongT);

  // Ok so now we need to decide which jump target
  isinc = def->builder->CreateICmpSGT(pstep_ivalue, def->types->kluaInteger[0],
                                      "step.gt.zero");

  // Create if then else branch
  llvm::BasicBlock *b1 =
      llvm::BasicBlock::Create(def->jitState->context(), "b1", def->f);
  llvm::BasicBlock *b2 =
      llvm::BasicBlock::Create(def->jitState->context(), "b2");
  llvm::BasicBlock *b3 =
      llvm::BasicBlock::Create(def->jitState->context(), "b3");

  def->builder->CreateCondBr(isinc, b1, b2);
  def->builder->SetInsertPoint(b1);

  def->builder->CreateStore(
      llvm::BlockAddress::get(def->f, forloop_target.jmp1),
      forloop_target.forloop_branch);
  def->builder->CreateBr(b3);

  def->f->getBasicBlockList().push_back(b2);
  def->builder->SetInsertPoint(b2);
  def->builder->CreateStore(
      llvm::BlockAddress::get(def->f, forloop_target.jmp2),
      forloop_target.forloop_branch);
  def->builder->CreateBr(b3);

  def->f->getBasicBlockList().push_back(b3);
  def->builder->SetInsertPoint(b3);

  // Create branch
  def->builder->CreateCondBr(isinc, forloop_target.jmp1, forloop_target.jmp2);

  // NOW the non-integer case

  def->f->getBasicBlockList().push_back(else1);
  def->builder->SetInsertPoint(else1);

  // ************ PLIMIT - Convert plimit to float

  llvm::Value *plimit_tt_ptr = emit_gep(def, "limit.tt.ptr", plimit, 0, 1);
  llvm::Instruction *plimit_tt =
      def->builder->CreateLoad(plimit_tt_ptr, "limit.tt");
  plimit_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_TValue_ttT);
  // Test if already a float
  cmp1 = def->builder->CreateICmpEQ(plimit_tt, def->types->kInt[LUA_TNUMFLT],
                                    "limit.is.float");
  llvm::BasicBlock *else1_plimit_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.limit.isfloat", def->f);
  llvm::BasicBlock *else1_plimit_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.limit.notfloat");
  def->builder->CreateCondBr(cmp1, else1_plimit_ifnum, else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_ifnum);

  // Already a float - copy to nlimit
  llvm::Value *plimit_nvalue_ptr = def->builder->CreateBitCast(
      plimit, def->types->plua_NumberT, "limit.n.ptr");
  llvm::Instruction *plimit_nvalue_load =
      def->builder->CreateLoad(plimit_nvalue_ptr, "limit.n");
  plimit_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                                  def->types->tbaa_TValue_nT);
  llvm::Instruction *nlimit_store = def->builder->CreateStore(
      plimit_nvalue_load, forloop_target.flimit, "nlimit");
  nlimit_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_longlongT);

  // Go to the PSTEP section
  llvm::BasicBlock *else1_pstep =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.step");
  def->builder->CreateBr(else1_pstep);

  // If plimit was not already a float we need to convert
  def->f->getBasicBlockList().push_back(else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_elsenum);
  // Call luaV_tonumber_()
  llvm::Value *plimit_isnum = def->builder->CreateCall2(
      def->luaV_tonumberF, plimit, forloop_target.flimit);
  llvm::Value *plimit_isnum_bool = def->builder->CreateICmpEQ(
      plimit_isnum, def->types->kInt[0], "limit.float.ok");

  // Did conversion fail?
  llvm::BasicBlock *else1_plimit_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.limit.float.failed", def->f);
  def->builder->CreateCondBr(plimit_isnum_bool, else1_plimit_tonum_elsenum,
                             else1_pstep);

  // Conversion failed, so raise error
  def->builder->SetInsertPoint(else1_plimit_tonum_elsenum);
  llvm::Value *errmsg1 =
      def->builder->CreateGlobalString("'for' limit must be a number");
  def->builder->CreateCall2(def->luaG_runerrorF, def->L,
                            emit_gep(def, "", errmsg1, 0, 0));
  def->builder->CreateBr(else1_pstep);

  // Conversion OK
  // Update plimit
  def->f->getBasicBlockList().push_back(else1_pstep);
  def->builder->SetInsertPoint(else1_pstep);

  // ***********  PSTEP - convert pstep to float
  // Test if already a float
  pstep_tt = def->builder->CreateLoad(pstep_tt_ptr, "step.tt");
  pstep_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_TValue_ttT);
  cmp1 = def->builder->CreateICmpEQ(pstep_tt, def->types->kInt[LUA_TNUMFLT],
                                    "step.is.float");
  llvm::BasicBlock *else1_pstep_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.step.isfloat", def->f);
  llvm::BasicBlock *else1_pstep_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.step.notfloat");
  def->builder->CreateCondBr(cmp1, else1_pstep_ifnum, else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_ifnum);

  // We float then copy to nstep
  llvm::Value *pstep_nvalue_ptr = def->builder->CreateBitCast(
      pstep, def->types->plua_NumberT, "step.n.ptr");
  llvm::Instruction *pstep_nvalue_load =
      def->builder->CreateLoad(pstep_nvalue_ptr, "step.n");
  pstep_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                                 def->types->tbaa_TValue_nT);
  llvm::Instruction *nstep_store = def->builder->CreateStore(
      pstep_nvalue_load, forloop_target.fstep, "nstep");
  nstep_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_longlongT);

  // Now go to handle initial value
  llvm::BasicBlock *else1_pinit =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.init");
  def->builder->CreateBr(else1_pinit);

  // If pstep was not already a float then we need to convert
  def->f->getBasicBlockList().push_back(else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_elsenum);

  // call luaV_tonumber_()
  llvm::Value *pstep_isnum = def->builder->CreateCall2(
      def->luaV_tonumberF, pstep, forloop_target.fstep);
  llvm::Value *pstep_isnum_bool = def->builder->CreateICmpEQ(
      pstep_isnum, def->types->kInt[0], "step.float.ok");
  llvm::BasicBlock *else1_pstep_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.step.float.failed", def->f);
  def->builder->CreateCondBr(pstep_isnum_bool, else1_pstep_tonum_elsenum,
                             else1_pinit);

  // If conversion failed raise error
  def->builder->SetInsertPoint(else1_pstep_tonum_elsenum);
  errmsg1 = def->builder->CreateGlobalString("'for' step must be a number");
  def->builder->CreateCall2(def->luaG_runerrorF, def->L,
                            emit_gep(def, "", errmsg1, 0, 0));
  def->builder->CreateBr(else1_pinit);

  // Conversion okay so update pstep
  def->f->getBasicBlockList().push_back(else1_pinit);
  def->builder->SetInsertPoint(else1_pinit);

  // *********** PINIT finally handle initial value

  // Check if it is already a float
  pinit_tt = def->builder->CreateLoad(pinit_tt_ptr, "init.tt");
  pinit_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_TValue_ttT);
  cmp1 = def->builder->CreateICmpEQ(pinit_tt, def->types->kInt[LUA_TNUMFLT],
                                    "init.is.float");
  llvm::BasicBlock *else1_pinit_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.init.is.float", def->f);
  llvm::BasicBlock *else1_pinit_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.init.not.float");
  def->builder->CreateCondBr(cmp1, else1_pinit_ifnum, else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_ifnum);

  // Already float so copy to ninit
  llvm::Value *pinit_nvalue_ptr =
      def->builder->CreateBitCast(init, def->types->plua_NumberT, "init.n.ptr");
  llvm::Instruction *pinit_nvalue_load =
      def->builder->CreateLoad(pinit_nvalue_ptr, "init.n");
  pinit_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                                 def->types->tbaa_TValue_nT);
  llvm::Instruction *fidx_store =
      def->builder->CreateStore(pinit_nvalue_load, forloop_target.fidx);
  fidx_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);

  // Go to final section
  llvm::BasicBlock *else1_pdone =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.done");
  def->builder->CreateBr(else1_pdone);

  // Not a float so we need to convert
  def->f->getBasicBlockList().push_back(else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_elsenum);

  // Call luaV_tonumber_()
  llvm::Value *pinit_isnum =
      def->builder->CreateCall2(def->luaV_tonumberF, init, forloop_target.fidx);
  llvm::Value *pinit_isnum_bool = def->builder->CreateICmpEQ(
      pinit_isnum, def->types->kInt[0], "init.float.ok");
  llvm::BasicBlock *else1_pinit_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.init.float.failed", def->f);
  def->builder->CreateCondBr(pinit_isnum_bool, else1_pinit_tonum_elsenum,
                             else1_pdone);

  // Conversion failed so raise error
  def->builder->SetInsertPoint(else1_pinit_tonum_elsenum);
  errmsg1 =
      def->builder->CreateGlobalString("'for' initial value must be a number");
  def->builder->CreateCall2(def->luaG_runerrorF, def->L,
                            emit_gep(def, "", errmsg1, 0, 0));
  def->builder->CreateBr(else1_pdone);

  // Conversion OK so we are nearly done
  def->f->getBasicBlockList().push_back(else1_pdone);
  def->builder->SetInsertPoint(else1_pdone);
  llvm::Instruction *ninit_load =
      def->builder->CreateLoad(forloop_target.fidx, "ninit");
  ninit_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);
  llvm::Instruction *nstep_load =
      def->builder->CreateLoad(forloop_target.fstep, "nstep");
  nstep_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);

  //    setfltvalue(init, luai_numsub(L, ninit, nstep));
  llvm::Value *init_n =
      def->builder->CreateFSub(ninit_load, nstep_load, "ninit-nstep");

  llvm::Instruction *ninit_store =
      def->builder->CreateStore(init_n, forloop_target.fidx, "ninit");
  ninit_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_longlongT);

  // Done so jump to forloop
  // def->builder->CreateBr(def->jmp_targets[pc]);
  llvm::Value *fstep_gt_zero = def->builder->CreateFCmpOGT(
      nstep_load, llvm::ConstantFP::get(def->types->lua_NumberT, 0.0),
      "step.gt.zero");

  // Create if then else branch
  b1 = llvm::BasicBlock::Create(def->jitState->context(), "b1", def->f);
  b2 = llvm::BasicBlock::Create(def->jitState->context(), "b2");
  b3 = llvm::BasicBlock::Create(def->jitState->context(), "b3");

  def->builder->CreateCondBr(fstep_gt_zero, b1, b2);
  def->builder->SetInsertPoint(b1);

  def->builder->CreateStore(
      llvm::BlockAddress::get(def->f, forloop_target.jmp3),
      forloop_target.forloop_branch);
  def->builder->CreateBr(b3);

  def->f->getBasicBlockList().push_back(b2);
  def->builder->SetInsertPoint(b2);
  def->builder->CreateStore(
      llvm::BlockAddress::get(def->f, forloop_target.jmp4),
      forloop_target.forloop_branch);
  def->builder->CreateBr(b3);

  def->f->getBasicBlockList().push_back(b3);
  def->builder->SetInsertPoint(b3);

  def->builder->CreateCondBr(fstep_gt_zero, forloop_target.jmp3,
                             forloop_target.jmp4);
  // def->f->dump();
  // assert(false);
}

void RaviCodeGenerator::emit_FORPREP(RaviFunctionDef *def, llvm::Value *L_ci,
                                     llvm::Value *proto, int A, int pc) {

  // case OP_FORPREP: {
  //  if (ttisinteger(init) && ttisinteger(pstep) &&
  //    forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {
  //    /* all values are integer */
  //    lua_Integer initv = (stopnow ? 0 : ivalue(init));
  //    setivalue(plimit, ilimit);
  //    setivalue(init, initv - ivalue(pstep));
  //  }
  //  else {  /* try making all values floats */
  //    if (!tonumber(plimit, &nlimit))
  //      luaG_runerror(L, "'for' limit must be a number");
  //    setfltvalue(plimit, nlimit);
  //    if (!tonumber(pstep, &nstep))
  //      luaG_runerror(L, "'for' step must be a number");
  //    setfltvalue(pstep, nstep);
  //    if (!tonumber(init, &ninit))
  //      luaG_runerror(L, "'for' initial value must be a number");
  //    setfltvalue(init, luai_numsub(L, ninit, nstep));
  //  }
  //  ci->u.l.savedpc += GETARG_sBx(i);
  //} break;

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base, "base");
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_luaState_ci_baseT);

  //  lua_Integer ilimit;
  //  int stopnow;
  //  lua_Number ninit; lua_Number nlimit; lua_Number nstep;
  llvm::Value *ilimit =
      def->builder->CreateAlloca(def->types->lua_IntegerT, nullptr, "ilimit");
  llvm::Value *stopnow =
      def->builder->CreateAlloca(def->types->C_intT, nullptr, "stopnow");
  llvm::Value *nlimit =
      def->builder->CreateAlloca(def->types->lua_NumberT, nullptr, "nlimit");
  llvm::Value *ninit =
      def->builder->CreateAlloca(def->types->lua_NumberT, nullptr, "ninit");
  llvm::Value *nstep =
      def->builder->CreateAlloca(def->types->lua_NumberT, nullptr, "nstep");

  //  TValue *init = ra;
  //  TValue *plimit = ra + 1;
  //  TValue *pstep = ra + 2;
  llvm::Value *init = A == 0 ? base_ptr : emit_array_get(def, base_ptr, A);
  llvm::Value *plimit = emit_array_get(def, base_ptr, A + 1);
  llvm::Value *pstep = emit_array_get(def, base_ptr, A + 2);

  //  if (ttisinteger(init) && ttisinteger(pstep) &&
  //    forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {

  // Get init->tt_
  llvm::Value *pinit_tt_ptr = emit_gep(def, "init.tt.ptr", init, 0, 1);
  llvm::Instruction *pinit_tt =
      def->builder->CreateLoad(pinit_tt_ptr, "init.tt");
  pinit_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_TValue_ttT);

  // Compare init->tt_ == LUA_TNUMINT
  llvm::Value *cmp1 = def->builder->CreateICmpEQ(
      pinit_tt, def->types->kInt[LUA_TNUMINT], "init.is.integer");

  // Get pstep->tt_
  llvm::Value *pstep_tt_ptr = emit_gep(def, "step.tt.ptr", pstep, 0, 1);
  llvm::Instruction *pstep_tt =
      def->builder->CreateLoad(pstep_tt_ptr, "step.tt");
  pstep_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_TValue_ttT);

  // Compare pstep->tt_ == LUA_TNUMINT
  llvm::Value *icmp2 = def->builder->CreateICmpEQ(
      pstep_tt, def->types->kInt[LUA_TNUMINT], "step.is.integer");

  // Get ivalue(pstep)
  llvm::Value *pstep_ivalue_ptr = def->builder->CreateBitCast(
      pstep, def->types->plua_IntegerT, "step.i.ptr");
  llvm::Instruction *pstep_ivalue =
      def->builder->CreateLoad(pstep_ivalue_ptr, "step.i");
  pstep_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_TValue_nT);

  // Call forlimit()
  llvm::Value *forlimit_ret = def->builder->CreateCall4(
      def->luaV_forlimitF, plimit, ilimit, pstep_ivalue, stopnow);

  // init->tt_ == LUA_TNUMINT && pstep->tt_ == LUA_TNUMINT
  llvm::Value *and1 =
      def->builder->CreateAnd(cmp1, icmp2, "init.and.step.are.integers");

  // Convert result from forlimit() to bool
  llvm::Value *tobool =
      def->builder->CreateICmpNE(forlimit_ret, def->types->kInt[0]);

  // init->tt_ == LUA_TNUMINT && pstep->tt_ == LUA_TNUMINT && forlimit()
  llvm::Value *and2 = def->builder->CreateAnd(and1, tobool, "all.integers");

  // Create if then else branch
  llvm::BasicBlock *then1 = llvm::BasicBlock::Create(def->jitState->context(),
                                                     "if.all.integers", def->f);
  llvm::BasicBlock *else1 =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.all.integers");
  def->builder->CreateCondBr(and2, then1, else1);
  def->builder->SetInsertPoint(then1);

  //    all values are integers
  //    lua_Integer initv = (stopnow ? 0 : ivalue(init));

  // Get stopnow
  llvm::Instruction *stopnow_val =
      def->builder->CreateLoad(stopnow, "stopnow.value");
  stopnow_val->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_intT);

  // Test if stopnow is 0
  llvm::Value *stopnow_is_zero = def->builder->CreateICmpEQ(
      stopnow_val, def->types->kInt[0], "stopnow.is.zero");

  // Get ptr to init->i
  llvm::Value *init_value_ptr = def->builder->CreateBitCast(
      init, def->types->plua_IntegerT, "init.i.ptr");

  // Setup if then else branch for stopnow
  llvm::BasicBlock *then1_iffalse = llvm::BasicBlock::Create(
      def->jitState->context(), "if.stopnow.iszero", def->f);
  llvm::BasicBlock *then1_iftrue =
      llvm::BasicBlock::Create(def->jitState->context(), "if.stopnow.notzero");
  def->builder->CreateCondBr(stopnow_is_zero, then1_iffalse, then1_iftrue);
  def->builder->SetInsertPoint(then1_iffalse);

  // stopnow is 0
  // Get init->i
  llvm::Instruction *init_ivalue =
      def->builder->CreateLoad(init_value_ptr, "init.i");
  init_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_TValue_nT);

  // Join after the branch
  def->builder->CreateBr(then1_iftrue);
  def->f->getBasicBlockList().push_back(then1_iftrue);
  def->builder->SetInsertPoint(then1_iftrue);

  // Set initv to 0 if !stopnow else init->i
  auto phi1 = def->builder->CreatePHI(def->types->lua_IntegerT, 2, "initv");
  phi1->addIncoming(init_ivalue, then1_iffalse);
  phi1->addIncoming(def->types->kluaInteger[0], then1);

  //    setivalue(plimit, ilimit);
  llvm::Instruction *ilimit_val = def->builder->CreateLoad(ilimit, "ilimit");
  ilimit_val->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);
  llvm::Value *plimit_ivalue_ptr = def->builder->CreateBitCast(
      plimit, def->types->plua_IntegerT, "limit.i.ptr");
  llvm::Instruction *plimit_value =
      def->builder->CreateStore(ilimit_val, plimit_ivalue_ptr);
  plimit_value->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_TValue_nT);
  llvm::Value *plimit_tt_ptr = emit_gep(def, "limit.tt.ptr", plimit, 0, 1);
  llvm::Instruction *plimit_tt = def->builder->CreateStore(
      def->types->kInt[LUA_TNUMINT], plimit_tt_ptr, "limit.tt");
  plimit_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_TValue_ttT);

  //    setivalue(init, initv - ivalue(pstep));
  // we aleady know init is LUA_TNUMINT
  pstep_ivalue = def->builder->CreateLoad(pstep_ivalue_ptr, "step.i.ptr");
  pstep_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_TValue_nT);
  llvm::Value *sub =
      def->builder->CreateSub(phi1, pstep_ivalue, "initv-pstep.i", false, true);
  llvm::Instruction *init_ivalue_store =
      def->builder->CreateStore(sub, init_value_ptr, "init.i");
  init_ivalue_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                                 def->types->tbaa_TValue_nT);

  // We are done so jump to forloop
  lua_assert(def->jmp_targets[pc].jmp1);
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);

  // NOW the non-integer case

  def->f->getBasicBlockList().push_back(else1);
  def->builder->SetInsertPoint(else1);

  // ************ PLIMIT - Convert plimit to float

  plimit_tt_ptr = emit_gep(def, "limit.tt.ptr", plimit, 0, 1);
  plimit_tt = def->builder->CreateLoad(plimit_tt_ptr, "limit.tt");
  plimit_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_TValue_ttT);
  // Test if already a float
  cmp1 = def->builder->CreateICmpEQ(plimit_tt, def->types->kInt[LUA_TNUMFLT],
                                    "limit.is.float");
  llvm::BasicBlock *else1_plimit_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.limit.isfloat", def->f);
  llvm::BasicBlock *else1_plimit_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.limit.notfloat");
  def->builder->CreateCondBr(cmp1, else1_plimit_ifnum, else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_ifnum);

  // Already a float - copy to nlimit
  llvm::Value *plimit_nvalue_ptr = def->builder->CreateBitCast(
      plimit, def->types->plua_NumberT, "limit.n.ptr");
  llvm::Instruction *plimit_nvalue_load =
      def->builder->CreateLoad(plimit_nvalue_ptr, "limit.n");
  plimit_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                                  def->types->tbaa_TValue_nT);
  llvm::Instruction *nlimit_store =
      def->builder->CreateStore(plimit_nvalue_load, nlimit, "nlimit");
  nlimit_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_longlongT);

  // Go to the PSTEP section
  llvm::BasicBlock *else1_pstep =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.step");
  def->builder->CreateBr(else1_pstep);

  // If plimit was not already a float we need to convert
  def->f->getBasicBlockList().push_back(else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_elsenum);
  // Call luaV_tonumber_()
  llvm::Value *plimit_isnum =
      def->builder->CreateCall2(def->luaV_tonumberF, plimit, nlimit);
  llvm::Value *plimit_isnum_bool = def->builder->CreateICmpEQ(
      plimit_isnum, def->types->kInt[0], "limit.float.ok");

  // Did conversion fail?
  llvm::BasicBlock *else1_plimit_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.limit.float.failed", def->f);
  def->builder->CreateCondBr(plimit_isnum_bool, else1_plimit_tonum_elsenum,
                             else1_pstep);

  // Conversion failed, so raise error
  def->builder->SetInsertPoint(else1_plimit_tonum_elsenum);
  llvm::Value *errmsg1 =
      def->builder->CreateGlobalString("'for' limit must be a number");
  def->builder->CreateCall2(def->luaG_runerrorF, def->L,
                            emit_gep(def, "", errmsg1, 0, 0));
  def->builder->CreateBr(else1_pstep);

  // Conversion OK
  // Update plimit
  def->f->getBasicBlockList().push_back(else1_pstep);
  def->builder->SetInsertPoint(else1_pstep);
  llvm::Instruction *nlimit_load = def->builder->CreateLoad(nlimit, "nlimit");
  nlimit_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_longlongT);
  llvm::Value *plimit_n = def->builder->CreateBitCast(
      plimit, def->types->plua_NumberT, "limit.n.ptr");
  llvm::Instruction *plimit_store =
      def->builder->CreateStore(nlimit_load, plimit_n, "limit.n");
  plimit_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_TValue_nT);
  llvm::Instruction *plimit_tt_store = def->builder->CreateStore(
      def->types->kInt[LUA_TNUMFLT], plimit_tt_ptr, "limit.tt");
  plimit_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                               def->types->tbaa_TValue_ttT);

  // ***********  PSTEP - convert pstep to float
  // Test if already a float
  pstep_tt = def->builder->CreateLoad(pstep_tt_ptr, "step.tt.ptr");
  pstep_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_TValue_ttT);
  cmp1 = def->builder->CreateICmpEQ(pstep_tt, def->types->kInt[LUA_TNUMFLT],
                                    "step.is.float");
  llvm::BasicBlock *else1_pstep_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.step.isfloat", def->f);
  llvm::BasicBlock *else1_pstep_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.step.notfloat");
  def->builder->CreateCondBr(cmp1, else1_pstep_ifnum, else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_ifnum);

  // We float then copy to nstep
  llvm::Value *pstep_nvalue_ptr = def->builder->CreateBitCast(
      pstep, def->types->plua_NumberT, "step.n.ptr");
  llvm::Instruction *pstep_nvalue_load =
      def->builder->CreateLoad(pstep_nvalue_ptr, "step.n");
  pstep_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                                 def->types->tbaa_TValue_nT);
  llvm::Instruction *nstep_store =
      def->builder->CreateStore(pstep_nvalue_load, nstep, "nstep");
  nstep_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_longlongT);

  // Now go to handle initial value
  llvm::BasicBlock *else1_pinit =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.init");
  def->builder->CreateBr(else1_pinit);

  // If pstep was not already a float then we need to convert
  def->f->getBasicBlockList().push_back(else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_elsenum);

  // call luaV_tonumber_()
  llvm::Value *pstep_isnum =
      def->builder->CreateCall2(def->luaV_tonumberF, pstep, nstep);
  llvm::Value *pstep_isnum_bool = def->builder->CreateICmpEQ(
      pstep_isnum, def->types->kInt[0], "step.float.ok");
  llvm::BasicBlock *else1_pstep_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.step.float.failed", def->f);
  def->builder->CreateCondBr(pstep_isnum_bool, else1_pstep_tonum_elsenum,
                             else1_pinit);

  // If conversion failed raise error
  def->builder->SetInsertPoint(else1_pstep_tonum_elsenum);
  errmsg1 = def->builder->CreateGlobalString("'for' step must be a number");
  def->builder->CreateCall2(def->luaG_runerrorF, def->L,
                            emit_gep(def, "", errmsg1, 0, 0));
  def->builder->CreateBr(else1_pinit);

  // Conversion okay so update pstep
  def->f->getBasicBlockList().push_back(else1_pinit);
  def->builder->SetInsertPoint(else1_pinit);
  llvm::Instruction *nstep_load = def->builder->CreateLoad(nstep, "nstep");
  nstep_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);
  llvm::Value *pstep_n = def->builder->CreateBitCast(
      pstep, def->types->plua_NumberT, "step.n.ptr");
  llvm::Instruction *pstep_store =
      def->builder->CreateStore(nstep_load, pstep_n, "step.n");
  pstep_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_TValue_nT);
  llvm::Instruction *pstep_tt_store = def->builder->CreateStore(
      def->types->kInt[LUA_TNUMFLT], pstep_tt_ptr, "step.tt");
  pstep_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                              def->types->tbaa_TValue_ttT);

  // *********** PINIT finally handle initial value

  // Check if it is already a float
  pinit_tt = def->builder->CreateLoad(pinit_tt_ptr, "init.tt");
  pinit_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_TValue_ttT);
  cmp1 = def->builder->CreateICmpEQ(pinit_tt, def->types->kInt[LUA_TNUMFLT],
                                    "init.is.float");
  llvm::BasicBlock *else1_pinit_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.init.is.float", def->f);
  llvm::BasicBlock *else1_pinit_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.init.not.float");
  def->builder->CreateCondBr(cmp1, else1_pinit_ifnum, else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_ifnum);

  // Already float so copy to ninit
  llvm::Value *pinit_nvalue_ptr =
      def->builder->CreateBitCast(init, def->types->plua_NumberT, "init.n.ptr");
  llvm::Instruction *pinit_nvalue_load =
      def->builder->CreateLoad(pinit_nvalue_ptr, "init.n");
  pinit_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                                 def->types->tbaa_TValue_nT);
  llvm::Instruction *ninit_store =
      def->builder->CreateStore(pinit_nvalue_load, ninit, "ninit");
  ninit_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_longlongT);

  // Go to final section
  llvm::BasicBlock *else1_pdone =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.done");
  def->builder->CreateBr(else1_pdone);

  // Not a float so we need to convert
  def->f->getBasicBlockList().push_back(else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_elsenum);

  // Call luaV_tonumber_()
  llvm::Value *pinit_isnum =
      def->builder->CreateCall2(def->luaV_tonumberF, init, ninit);
  llvm::Value *pinit_isnum_bool = def->builder->CreateICmpEQ(
      pinit_isnum, def->types->kInt[0], "init.float.ok");
  llvm::BasicBlock *else1_pinit_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.init.float.failed", def->f);
  def->builder->CreateCondBr(pinit_isnum_bool, else1_pinit_tonum_elsenum,
                             else1_pdone);

  // Conversion failed so raise error
  def->builder->SetInsertPoint(else1_pinit_tonum_elsenum);
  errmsg1 =
      def->builder->CreateGlobalString("'for' initial value must be a number");
  def->builder->CreateCall2(def->luaG_runerrorF, def->L,
                            emit_gep(def, "", errmsg1, 0, 0));
  def->builder->CreateBr(else1_pdone);

  // Conversion OK so we are nearly done
  def->f->getBasicBlockList().push_back(else1_pdone);
  def->builder->SetInsertPoint(else1_pdone);
  llvm::Instruction *ninit_load = def->builder->CreateLoad(ninit, "ninit");
  ninit_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);
  nstep_load = def->builder->CreateLoad(nstep, "nstep");
  nstep_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);

  //    setfltvalue(init, luai_numsub(L, ninit, nstep));
  llvm::Value *init_n =
      def->builder->CreateFSub(ninit_load, nstep_load, "ninit-nstep");
  llvm::Value *pinit_n =
      def->builder->CreateBitCast(init, def->types->plua_NumberT, "init.n.ptr");
  llvm::Instruction *pinit_store =
      def->builder->CreateStore(init_n, pinit_n, "init.n");
  pinit_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_TValue_nT);
  llvm::Instruction *pinit_tt_store = def->builder->CreateStore(
      def->types->kInt[LUA_TNUMFLT], pinit_tt_ptr, "init.tt");
  pinit_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                              def->types->tbaa_TValue_ttT);

  // Done so jump to forloop
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);

  // def->f->dump();
}
}
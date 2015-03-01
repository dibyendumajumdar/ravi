#include "ravi_llvmcodegen.h"

namespace ravi {

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
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_luaState_ci_baseT);

  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr;

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
  llvm::Value *pinit_tt_ptr = emit_gep(def, "init.tt_", init, 0, 1);
  llvm::Instruction *tt_i = def->builder->CreateLoad(pinit_tt_ptr);
  tt_i->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

  // Compare init->tt_ == LUA_TNUMINT
  llvm::Value *cmp1 =
      def->builder->CreateICmpEQ(tt_i, def->types->kInt[LUA_TNUMINT]);

  // Get pstep->tt_
  llvm::Value *pstep_tt_ptr = emit_gep(def, "tt_", pstep, 0, 1);
  llvm::Instruction *tt_j = def->builder->CreateLoad(pstep_tt_ptr);
  tt_j->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

  // Compare pstep->tt_ == LUA_TNUMINT
  llvm::Value *icmp2 =
      def->builder->CreateICmpEQ(tt_j, def->types->kInt[LUA_TNUMINT]);

  // Get ivalue(pstep)
  llvm::Value *pstep_ivalue_ptr =
      def->builder->CreateBitCast(pstep, def->types->plua_IntegerT);
  llvm::Instruction *pstep_ivalue =
      def->builder->CreateLoad(pstep_ivalue_ptr, "pstep_ivalue");
  pstep_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_TValue_nT);

  // Call forlimit()
  llvm::Value *forlimit_ret = def->builder->CreateCall4(
      def->luaV_forlimitF, plimit, ilimit, pstep_ivalue, stopnow);

  // init->tt_ == LUA_TNUMINT && pstep->tt_ == LUA_TNUMINT
  llvm::Value *and1 = def->builder->CreateAnd(cmp1, icmp2);

  // Convert result from forlimit() to bool
  llvm::Value *tobool =
      def->builder->CreateICmpNE(forlimit_ret, def->types->kInt[0]);

  // init->tt_ == LUA_TNUMINT && pstep->tt_ == LUA_TNUMINT && forlimit()
  llvm::Value *and2 = def->builder->CreateAnd(and1, tobool);

  // Create if then else branch
  llvm::BasicBlock *then1 = llvm::BasicBlock::Create(def->jitState->context(),
                                                     "if.all.integer", def->f);
  llvm::BasicBlock *else1 =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.all.integer");
  def->builder->CreateCondBr(and2, then1, else1);
  def->builder->SetInsertPoint(then1);

  //    /* all values are integer */
  //    lua_Integer initv = (stopnow ? 0 : ivalue(init));

  // Get stopnow
  llvm::Instruction *stopnow_val =
      def->builder->CreateLoad(stopnow, "stopnow_val");
  stopnow_val->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_intT);

  // Test if stopnow is 0
  llvm::Value *stopnow_is_zero =
      def->builder->CreateICmpEQ(stopnow_val, def->types->kInt[0]);

  // Get ptr to init->i
  llvm::Value *init_value_ptr = def->builder->CreateBitCast(
      init, def->types->plua_IntegerT, "init_ivalue_ptr");

  // Setup if then else branch for stopnow
  llvm::BasicBlock *then1_iffalse = llvm::BasicBlock::Create(
      def->jitState->context(), "if.stopnow.iszero", def->f);
  llvm::BasicBlock *then1_iftrue =
      llvm::BasicBlock::Create(def->jitState->context(), "if.stopnow.nonzero");
  def->builder->CreateCondBr(stopnow_is_zero, then1_iffalse, then1_iftrue);
  def->builder->SetInsertPoint(then1_iffalse);

  // stopnow is 0
  // Get init->i
  llvm::Instruction *init_ivalue =
      def->builder->CreateLoad(init_value_ptr, "init_ivalue");
  init_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_TValue_nT);

  // Join after the branch
  def->builder->CreateBr(then1_iftrue);
  def->f->getBasicBlockList().push_back(then1_iftrue);
  def->builder->SetInsertPoint(then1_iftrue);

  // Set initv to 0 if !stopnow else init->i
  auto phi1 = def->builder->CreatePHI(def->types->lua_IntegerT, 2);
  phi1->addIncoming(init_ivalue, then1_iffalse);
  phi1->addIncoming(def->types->kluaInteger[0], then1);

  //    setivalue(plimit, ilimit);
  llvm::Instruction *ilimit_val = def->builder->CreateLoad(ilimit, "ilimit");
  ilimit_val->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);
  llvm::Value *plimit_ivalue_ptr = def->builder->CreateBitCast(
      init, def->types->plua_IntegerT, "plimit_ivalue_ptr");
  llvm::Instruction *plimit_value =
      def->builder->CreateStore(ilimit_val, plimit_ivalue_ptr);
  plimit_value->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_TValue_nT);
  llvm::Value *plimit_tt_ptr = emit_gep(def, "plimit.tt_", plimit, 0, 1);
  llvm::Instruction *plimit_tt =
      def->builder->CreateStore(def->types->kInt[LUA_TNUMINT], plimit_tt_ptr);
  plimit_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_TValue_ttT);

  //    setivalue(init, initv - ivalue(pstep));
  // we aleady know init is LUA_TNUMINT
  pstep_ivalue = def->builder->CreateLoad(pstep_ivalue_ptr, "pstep_ivalue");
  pstep_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_TValue_nT);
  llvm::Value *sub =
      def->builder->CreateSub(phi1, pstep_ivalue, "sub", false, true);
  llvm::Instruction *init_ivalue_store =
      def->builder->CreateStore(sub, init_value_ptr);
  init_ivalue_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                                 def->types->tbaa_TValue_nT);

  // We are done so jump to forloop
  lua_assert(def->jmp_targets[pc]);
  def->builder->CreateBr(def->jmp_targets[pc]);

  // NOW the non-integer case

  def->f->getBasicBlockList().push_back(else1);
  def->builder->SetInsertPoint(else1);

  // ************ PLIMIT - Convert plimit to float

  plimit_tt_ptr = emit_gep(def, "plimit.tt_", plimit, 0, 1);
  plimit_tt = def->builder->CreateLoad(plimit_tt_ptr);
  plimit_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_TValue_ttT);
  // Test if already a float
  cmp1 = def->builder->CreateICmpEQ(plimit_tt, def->types->kInt[LUA_TNUMFLT]);
  llvm::BasicBlock *else1_plimit_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.else.plimit.ifnum", def->f);
  llvm::BasicBlock *else1_plimit_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.else.plimit.elsenum");
  def->builder->CreateCondBr(cmp1, else1_plimit_ifnum, else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_ifnum);

  // Already a float - copy to nlimit
  llvm::Value *plimit_nvalue_ptr = def->builder->CreateBitCast(
      init, def->types->plua_NumberT, "plimit.n.ptr");
  llvm::Instruction *plimit_nvalue_load =
      def->builder->CreateLoad(plimit_nvalue_ptr);
  plimit_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                                  def->types->tbaa_TValue_nT);
  llvm::Instruction *nlimit_store =
      def->builder->CreateStore(plimit_nvalue_load, nlimit);
  nlimit_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_longlongT);

  // Go to the PSTEP section
  llvm::BasicBlock *else1_pstep =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.pstep");
  def->builder->CreateBr(else1_pstep);

  // If plimit was not already a float we need to convert
  def->f->getBasicBlockList().push_back(else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_elsenum);
  // Call luaV_tonumber_()
  llvm::Value *plimit_isnum =
      def->builder->CreateCall2(def->luaV_tonumberF, plimit, nlimit);
  llvm::Value *plimit_isnum_bool = def->builder->CreateICmpEQ(
      plimit_isnum, def->types->kInt[0], "plimit.isnum");

  // Did conversion fail?
  llvm::BasicBlock *else1_plimit_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.else.plimit.tonum.ifnum", def->f);
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
  llvm::Instruction *nlimit_load = def->builder->CreateLoad(nlimit);
  nlimit_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_longlongT);
  llvm::Value *plimit_n = def->builder->CreateBitCast(
      plimit, def->types->plua_NumberT, "plimit.n.ptr");
  llvm::Instruction *plimit_store =
      def->builder->CreateStore(nlimit_load, plimit_n);
  plimit_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_TValue_nT);
  llvm::Instruction *plimit_tt_store =
      def->builder->CreateStore(def->types->kInt[LUA_TNUMFLT], plimit_tt_ptr);
  plimit_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                               def->types->tbaa_TValue_ttT);

  // ***********  PSTEP - convert pstep to float
  // Test if already a float
  llvm::Instruction *pstep_tt = def->builder->CreateLoad(pstep_tt_ptr);
  pstep_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_TValue_ttT);
  cmp1 = def->builder->CreateICmpEQ(pstep_tt, def->types->kInt[LUA_TNUMFLT]);
  llvm::BasicBlock *else1_pstep_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.else.pstep.ifnum", def->f);
  llvm::BasicBlock *else1_pstep_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.else.pstep.elsenum");
  def->builder->CreateCondBr(cmp1, else1_pstep_ifnum, else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_ifnum);

  // We float then copy to nstep
  llvm::Value *pstep_nvalue_ptr = def->builder->CreateBitCast(
      pstep, def->types->plua_NumberT, "pstep.n.ptr");
  llvm::Instruction *pstep_nvalue_load =
      def->builder->CreateLoad(pstep_nvalue_ptr);
  pstep_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                                 def->types->tbaa_TValue_nT);
  llvm::Instruction *nstep_store =
      def->builder->CreateStore(pstep_nvalue_load, nstep);
  nstep_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_longlongT);

  // Now go to handle initial value
  llvm::BasicBlock *else1_pinit =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.pinit");
  def->builder->CreateBr(else1_pinit);

  // If pstep was not already a float then we need to convert
  def->f->getBasicBlockList().push_back(else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_elsenum);

  // call luaV_tonumber_()
  llvm::Value *pstep_isnum =
      def->builder->CreateCall2(def->luaV_tonumberF, pstep, nstep);
  llvm::Value *pstep_isnum_bool = def->builder->CreateICmpEQ(
      pstep_isnum, def->types->kInt[0], "pstep.isnum");
  llvm::BasicBlock *else1_pstep_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.else.pstep.tonum.ifnum", def->f);
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
  llvm::Instruction *nstep_load = def->builder->CreateLoad(nstep);
  nstep_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);
  llvm::Value *pstep_n = def->builder->CreateBitCast(
      pstep, def->types->plua_NumberT, "pstep.n.ptr");
  llvm::Instruction *pstep_store =
      def->builder->CreateStore(nstep_load, pstep_n);
  pstep_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_TValue_nT);
  llvm::Instruction *pstep_tt_store =
      def->builder->CreateStore(def->types->kInt[LUA_TNUMFLT], pstep_tt_ptr);
  pstep_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                              def->types->tbaa_TValue_ttT);

  // *********** PINIT finally handle initial value

  // Check if it is already a float
  llvm::Instruction *pinit_tt = def->builder->CreateLoad(pinit_tt_ptr);
  pinit_tt->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_TValue_ttT);
  cmp1 = def->builder->CreateICmpEQ(pinit_tt, def->types->kInt[LUA_TNUMFLT]);
  llvm::BasicBlock *else1_pinit_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.else.pinit.ifnum", def->f);
  llvm::BasicBlock *else1_pinit_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.else.pinit.elsenum");
  def->builder->CreateCondBr(cmp1, else1_pinit_ifnum, else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_ifnum);

  // Already float so copy to ninit
  llvm::Value *pinit_nvalue_ptr = def->builder->CreateBitCast(
      init, def->types->plua_NumberT, "pinit.n.ptr");
  llvm::Instruction *pinit_nvalue_load =
      def->builder->CreateLoad(pinit_nvalue_ptr);
  pinit_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                                 def->types->tbaa_TValue_nT);
  llvm::Instruction *ninit_store =
      def->builder->CreateStore(pinit_nvalue_load, ninit);
  ninit_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_longlongT);

  // Go to final section
  llvm::BasicBlock *else1_pdone =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.pdone");
  def->builder->CreateBr(else1_pdone);

  // Not a float so we need to convert
  def->f->getBasicBlockList().push_back(else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_elsenum);

  // Call luaV_tonumber_()
  llvm::Value *pinit_isnum =
      def->builder->CreateCall2(def->luaV_tonumberF, init, ninit);
  llvm::Value *pinit_isnum_bool = def->builder->CreateICmpEQ(
      pinit_isnum, def->types->kInt[0], "pinit.isnum");
  llvm::BasicBlock *else1_pinit_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.else.pinit.tonum.ifnum", def->f);
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
  llvm::Instruction *ninit_load = def->builder->CreateLoad(ninit);
  ninit_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);
  nstep_load = def->builder->CreateLoad(nstep);
  nstep_load->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_longlongT);

  //    setfltvalue(init, luai_numsub(L, ninit, nstep));
  llvm::Value *init_n = def->builder->CreateFSub(ninit_load, nstep_load);
  llvm::Value *pinit_n = def->builder->CreateBitCast(
      init, def->types->plua_NumberT, "pinit.n.ptr");
  llvm::Instruction *pinit_store = def->builder->CreateStore(init_n, pinit_n);
  pinit_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_TValue_nT);
  llvm::Instruction *pinit_tt_store =
      def->builder->CreateStore(def->types->kInt[LUA_TNUMFLT], pinit_tt_ptr);
  pinit_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa,
                              def->types->tbaa_TValue_ttT);

  // Done so jump to forloop
  def->builder->CreateBr(def->jmp_targets[pc]);

}

}
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

void RaviCodeGenerator::emit_FORPREP2(RaviFunctionDef *def, int A, int pc,
                                      int pc1) {

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

  emit_debug_trace(def, OP_FORPREP, pc1);
  // Load pointer to base
  emit_load_base(def);

  //  lua_Integer ilimit;
  //  int stopnow;
  //  lua_Number ninit; lua_Number nlimit; lua_Number nstep;

  // Setup local vars on C stack and link them
  // to the forloop_target so that the forloop code can access these

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *stopnow =
      TmpB.CreateAlloca(def->types->C_intT, nullptr, "stopnow");

  forloop_target.ilimit =
      TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "ilimit");
  forloop_target.istep =
      TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "istep");
  forloop_target.iidx =
      TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "iidx");
  forloop_target.flimit =
      TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nlimit");
  forloop_target.fidx =
      TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "ninit");
  forloop_target.fstep =
      TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nstep");
  forloop_target.forloop_branch =
      TmpB.CreateAlloca(def->types->C_pcharT, nullptr, "brnch");

  llvm::Value *isinc = nullptr;

  //  TValue *init = ra;
  //  TValue *plimit = ra + 1;
  //  TValue *pstep = ra + 2;
  llvm::Value *init = emit_gep_register(def, A);
  llvm::Value *plimit = emit_gep_register(def, A + 1);
  llvm::Value *pstep = emit_gep_register(def, A + 2);

  //  if (ttisinteger(init) && ttisinteger(pstep) &&
  //    forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {

  // Get init->tt
  llvm::Instruction *pinit_tt = emit_load_type(def, init);

  // Compare init->tt == LUA_TNUMINT
  llvm::Value *cmp1 =
      emit_is_value_of_type(def, pinit_tt, LUA__TNUMINT, "init.is.integer");

  // Get pstep->tt
  llvm::Instruction *pstep_tt = emit_load_type(def, pstep);

  // Compare pstep->tt == LUA_TNUMINT
  llvm::Value *icmp2 =
      emit_is_value_of_type(def, pstep_tt, LUA__TNUMINT, "step.is.integer");

  // Get ivalue(pstep)
  llvm::Instruction *pstep_ivalue = emit_load_reg_i(def, pstep);

  // Call forlimit()
  llvm::Value *forlimit_ret =
      CreateCall4(def->builder, def->luaV_forlimitF, plimit,
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
  emit_store_local_n(def, pstep_ivalue, forloop_target.istep);

  // Get stopnow
  llvm::Instruction *stopnow_val = emit_load_local_int(def, stopnow);

  // Test if stopnow is 0
  llvm::Value *stopnow_is_zero = def->builder->CreateICmpEQ(
      stopnow_val, def->types->kInt[0], "stopnow.is.zero");

  // Get ptr to init->i

  // Setup if then else branch for stopnow
  llvm::BasicBlock *then1_iffalse = llvm::BasicBlock::Create(
      def->jitState->context(), "if.stopnow.iszero", def->f);
  llvm::BasicBlock *then1_iftrue =
      llvm::BasicBlock::Create(def->jitState->context(), "if.stopnow.notzero");
  def->builder->CreateCondBr(stopnow_is_zero, then1_iffalse, then1_iftrue);
  def->builder->SetInsertPoint(then1_iffalse);

  // stopnow is 0
  // Get init->i
  llvm::Instruction *init_ivalue = emit_load_reg_i(def, init);

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
  emit_store_local_n(def, sub, forloop_target.iidx);

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

  // TODO tbaa?
  def->builder->CreateStore(
      llvm::BlockAddress::get(def->f, forloop_target.jmp1),
      forloop_target.forloop_branch);

  def->builder->CreateBr(b3);

  def->f->getBasicBlockList().push_back(b2);
  def->builder->SetInsertPoint(b2);

  // TODO tbaa?
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

  llvm::Instruction *plimit_tt = emit_load_type(def, plimit);

  // Test if already a float
  cmp1 = emit_is_value_of_type(def, plimit_tt, LUA__TNUMFLT, "limit.is.float");
  llvm::BasicBlock *else1_plimit_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.limit.isfloat", def->f);
  llvm::BasicBlock *else1_plimit_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.limit.notfloat");
  def->builder->CreateCondBr(cmp1, else1_plimit_ifnum, else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_ifnum);

  // Already a float - copy to nlimit
  llvm::Instruction *plimit_nvalue_load = emit_load_reg_n(def, plimit);
  emit_store_local_n(def, plimit_nvalue_load, forloop_target.flimit);

  // Go to the PSTEP section
  llvm::BasicBlock *else1_pstep =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.step");
  def->builder->CreateBr(else1_pstep);

  // If plimit was not already a float we need to convert
  def->f->getBasicBlockList().push_back(else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_elsenum);
  // Call luaV_tonumber_()
  llvm::Value *plimit_isnum = CreateCall2(def->builder, def->luaV_tonumberF,
                                          plimit, forloop_target.flimit);
  llvm::Value *plimit_isnum_bool = def->builder->CreateICmpEQ(
      plimit_isnum, def->types->kInt[0], "limit.float.ok");

  // Did conversion fail?
  llvm::BasicBlock *else1_plimit_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.limit.float.failed", def->f);
  def->builder->CreateCondBr(plimit_isnum_bool, else1_plimit_tonum_elsenum,
                             else1_pstep);

  // Conversion failed, so raise error
  def->builder->SetInsertPoint(else1_plimit_tonum_elsenum);
  emit_raise_lua_error(def, "'for' limit must be a number");
  def->builder->CreateBr(else1_pstep);

  // Conversion OK
  // Update plimit
  def->f->getBasicBlockList().push_back(else1_pstep);
  def->builder->SetInsertPoint(else1_pstep);

  // ***********  PSTEP - convert pstep to float
  // Test if already a float
  pstep_tt = emit_load_type(def, pstep);
  cmp1 = emit_is_value_of_type(def, pstep_tt, LUA__TNUMFLT, "step.is.float");
  llvm::BasicBlock *else1_pstep_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.step.isfloat", def->f);
  llvm::BasicBlock *else1_pstep_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.step.notfloat");
  def->builder->CreateCondBr(cmp1, else1_pstep_ifnum, else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_ifnum);

  // We float then copy to nstep
  llvm::Instruction *pstep_nvalue_load = emit_load_reg_n(def, pstep);
  emit_store_local_n(def, pstep_nvalue_load, forloop_target.fstep);

  // Now go to handle initial value
  llvm::BasicBlock *else1_pinit =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.init");
  def->builder->CreateBr(else1_pinit);

  // If pstep was not already a float then we need to convert
  def->f->getBasicBlockList().push_back(else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_elsenum);

  // call luaV_tonumber_()
  llvm::Value *pstep_isnum = CreateCall2(def->builder, def->luaV_tonumberF,
                                         pstep, forloop_target.fstep);
  llvm::Value *pstep_isnum_bool = def->builder->CreateICmpEQ(
      pstep_isnum, def->types->kInt[0], "step.float.ok");
  llvm::BasicBlock *else1_pstep_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.step.float.failed", def->f);
  def->builder->CreateCondBr(pstep_isnum_bool, else1_pstep_tonum_elsenum,
                             else1_pinit);

  // If conversion failed raise error
  def->builder->SetInsertPoint(else1_pstep_tonum_elsenum);
  emit_raise_lua_error(def, "'for' step must be a number");
  def->builder->CreateBr(else1_pinit);

  // Conversion okay so update pstep
  def->f->getBasicBlockList().push_back(else1_pinit);
  def->builder->SetInsertPoint(else1_pinit);

  // *********** PINIT finally handle initial value

  // Check if it is already a float
  pinit_tt = emit_load_type(def, init);
  cmp1 = emit_is_value_of_type(def, pinit_tt, LUA__TNUMFLT, "init.is.float");
  llvm::BasicBlock *else1_pinit_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.init.is.float", def->f);
  llvm::BasicBlock *else1_pinit_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.init.not.float");
  def->builder->CreateCondBr(cmp1, else1_pinit_ifnum, else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_ifnum);

  // Already float so copy to ninit
  llvm::Instruction *pinit_nvalue_load = emit_load_reg_n(def, init);
  emit_store_local_n(def, pinit_nvalue_load, forloop_target.fidx);

  // Go to final section
  llvm::BasicBlock *else1_pdone =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.done");
  def->builder->CreateBr(else1_pdone);

  // Not a float so we need to convert
  def->f->getBasicBlockList().push_back(else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_elsenum);

  // Call luaV_tonumber_()
  llvm::Value *pinit_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, init, forloop_target.fidx);
  llvm::Value *pinit_isnum_bool = def->builder->CreateICmpEQ(
      pinit_isnum, def->types->kInt[0], "init.float.ok");
  llvm::BasicBlock *else1_pinit_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.init.float.failed", def->f);
  def->builder->CreateCondBr(pinit_isnum_bool, else1_pinit_tonum_elsenum,
                             else1_pdone);

  // Conversion failed so raise error
  def->builder->SetInsertPoint(else1_pinit_tonum_elsenum);
  emit_raise_lua_error(def, "'for' initial value must be a number");
  def->builder->CreateBr(else1_pdone);

  // Conversion OK so we are nearly done
  def->f->getBasicBlockList().push_back(else1_pdone);
  def->builder->SetInsertPoint(else1_pdone);
  llvm::Instruction *ninit_load = emit_load_local_n(def, forloop_target.fidx);
  llvm::Instruction *nstep_load = emit_load_local_n(def, forloop_target.fstep);

  //    setfltvalue(init, luai_numsub(L, ninit, nstep));
  llvm::Value *init_n =
      def->builder->CreateFSub(ninit_load, nstep_load, "ninit-nstep");

  emit_store_local_n(def, init_n, forloop_target.fidx);

  // Done so jump to forloop
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
}

void RaviCodeGenerator::emit_FORPREP(RaviFunctionDef *def, int A, int pc,
                                     int pc1) {

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

  emit_debug_trace(def, OP_FORPREP, pc1);
  // Load pointer to base
  emit_load_base(def);

  //  lua_Integer ilimit;
  //  int stopnow;
  //  lua_Number ninit; lua_Number nlimit; lua_Number nstep;
  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());

  llvm::Value *ilimit =
      TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "ilimit");
  llvm::Value *stopnow =
      TmpB.CreateAlloca(def->types->C_intT, nullptr, "stopnow");
  llvm::Value *nlimit =
      TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nlimit");
  llvm::Value *ninit =
      TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "ninit");
  llvm::Value *nstep =
      TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "nstep");

  //  TValue *init = ra;
  //  TValue *plimit = ra + 1;
  //  TValue *pstep = ra + 2;
  llvm::Value *init = emit_gep_register(def, A);
  llvm::Value *plimit = emit_gep_register(def, A + 1);
  llvm::Value *pstep = emit_gep_register(def, A + 2);

  //  if (ttisinteger(init) && ttisinteger(pstep) &&
  //    forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {

  // Get init->tt_
  llvm::Instruction *pinit_tt = emit_load_type(def, init);

  // Compare init->tt_ == LUA_TNUMINT
  llvm::Value *cmp1 =
      emit_is_value_of_type(def, pinit_tt, LUA__TNUMINT, "init.is.integer");

  // Get pstep->tt_
  llvm::Instruction *pstep_tt = emit_load_type(def, pstep);

  // Compare pstep->tt_ == LUA_TNUMINT
  llvm::Value *icmp2 =
      emit_is_value_of_type(def, pstep_tt, LUA__TNUMINT, "step.is.integer");

  // Get ivalue(pstep)
  llvm::Instruction *pstep_ivalue = emit_load_reg_i(def, pstep);

  // Call forlimit()
  llvm::Value *forlimit_ret = CreateCall4(
      def->builder, def->luaV_forlimitF, plimit, ilimit, pstep_ivalue, stopnow);

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
  llvm::Instruction *stopnow_val = emit_load_local_int(def, stopnow);

  // Test if stopnow is 0
  llvm::Value *stopnow_is_zero = def->builder->CreateICmpEQ(
      stopnow_val, def->types->kInt[0], "stopnow.is.zero");

  // Setup if then else branch for stopnow
  llvm::BasicBlock *then1_iffalse = llvm::BasicBlock::Create(
      def->jitState->context(), "if.stopnow.iszero", def->f);
  llvm::BasicBlock *then1_iftrue =
      llvm::BasicBlock::Create(def->jitState->context(), "if.stopnow.notzero");
  def->builder->CreateCondBr(stopnow_is_zero, then1_iffalse, then1_iftrue);
  def->builder->SetInsertPoint(then1_iffalse);

  // stopnow is 0
  // Get init->i
  llvm::Instruction *init_ivalue = emit_load_reg_i(def, init);

  // Join after the branch
  def->builder->CreateBr(then1_iftrue);
  def->f->getBasicBlockList().push_back(then1_iftrue);
  def->builder->SetInsertPoint(then1_iftrue);

  // Set initv to 0 if !stopnow else init->i
  auto phi1 = def->builder->CreatePHI(def->types->lua_IntegerT, 2, "initv");
  phi1->addIncoming(init_ivalue, then1_iffalse);
  phi1->addIncoming(def->types->kluaInteger[0], then1);

  //    setivalue(plimit, ilimit);
  llvm::Instruction *ilimit_val = emit_load_local_n(def, ilimit);

  emit_store_reg_i_withtype(def, ilimit_val, plimit);

  //    setivalue(init, initv - ivalue(pstep));
  // we aleady know init is LUA_TNUMINT
  pstep_ivalue = emit_load_reg_i(def, pstep);
  llvm::Value *sub =
      def->builder->CreateSub(phi1, pstep_ivalue, "initv-pstep.i", false, true);

  emit_store_reg_i(def, sub, init);

  // We are done so jump to forloop
  lua_assert(def->jmp_targets[pc].jmp1);
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);

  // NOW the non-integer case

  def->f->getBasicBlockList().push_back(else1);
  def->builder->SetInsertPoint(else1);

  // ************ PLIMIT - Convert plimit to float

  llvm::Instruction *plimit_tt = emit_load_type(def, plimit);
  // Test if already a float
  cmp1 = emit_is_value_of_type(def, plimit_tt, LUA__TNUMFLT, "limit.is.float");
  llvm::BasicBlock *else1_plimit_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.limit.isfloat", def->f);
  llvm::BasicBlock *else1_plimit_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.limit.notfloat");
  def->builder->CreateCondBr(cmp1, else1_plimit_ifnum, else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_ifnum);

  // Already a float - copy to nlimit
  llvm::Instruction *plimit_nvalue_load = emit_load_reg_n(def, plimit);
  emit_store_local_n(def, plimit_nvalue_load, nlimit);

  // Go to the PSTEP section
  llvm::BasicBlock *else1_pstep =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.step");
  def->builder->CreateBr(else1_pstep);

  // If plimit was not already a float we need to convert
  def->f->getBasicBlockList().push_back(else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_elsenum);
  // Call luaV_tonumber_()
  llvm::Value *plimit_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, plimit, nlimit);
  llvm::Value *plimit_isnum_bool = def->builder->CreateICmpEQ(
      plimit_isnum, def->types->kInt[0], "limit.float.ok");

  // Did conversion fail?
  llvm::BasicBlock *else1_plimit_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.limit.float.failed", def->f);
  def->builder->CreateCondBr(plimit_isnum_bool, else1_plimit_tonum_elsenum,
                             else1_pstep);

  // Conversion failed, so raise error
  def->builder->SetInsertPoint(else1_plimit_tonum_elsenum);
  emit_raise_lua_error(def, "'for' limit must be a number");
  def->builder->CreateBr(else1_pstep);

  // Conversion OK
  // Update plimit
  def->f->getBasicBlockList().push_back(else1_pstep);
  def->builder->SetInsertPoint(else1_pstep);
  llvm::Instruction *nlimit_load = emit_load_local_n(def, nlimit);

  emit_store_reg_n_withtype(def, nlimit_load, plimit);

  // ***********  PSTEP - convert pstep to float
  // Test if already a float
  pstep_tt = emit_load_type(def, pstep);
  cmp1 = emit_is_value_of_type(def, pstep_tt, LUA__TNUMFLT, "step.is.float");
  llvm::BasicBlock *else1_pstep_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.step.isfloat", def->f);
  llvm::BasicBlock *else1_pstep_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.step.notfloat");
  def->builder->CreateCondBr(cmp1, else1_pstep_ifnum, else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_ifnum);

  // We float then copy to nstep
  llvm::Instruction *pstep_nvalue_load = emit_load_reg_n(def, pstep);
  emit_store_local_n(def, pstep_nvalue_load, nstep);

  // Now go to handle initial value
  llvm::BasicBlock *else1_pinit =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.init");
  def->builder->CreateBr(else1_pinit);

  // If pstep was not already a float then we need to convert
  def->f->getBasicBlockList().push_back(else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_elsenum);

  // call luaV_tonumber_()
  llvm::Value *pstep_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, pstep, nstep);
  llvm::Value *pstep_isnum_bool = def->builder->CreateICmpEQ(
      pstep_isnum, def->types->kInt[0], "step.float.ok");
  llvm::BasicBlock *else1_pstep_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.step.float.failed", def->f);
  def->builder->CreateCondBr(pstep_isnum_bool, else1_pstep_tonum_elsenum,
                             else1_pinit);

  // If conversion failed raise error
  def->builder->SetInsertPoint(else1_pstep_tonum_elsenum);
  emit_raise_lua_error(def, "'for' step must be a number");
  def->builder->CreateBr(else1_pinit);

  // Conversion okay so update pstep
  def->f->getBasicBlockList().push_back(else1_pinit);
  def->builder->SetInsertPoint(else1_pinit);

  llvm::Instruction *nstep_load = emit_load_local_n(def, nstep);

  emit_store_reg_n_withtype(def, nstep_load, pstep);

  // *********** PINIT finally handle initial value

  // Check if it is already a float
  pinit_tt = emit_load_type(def, init);
  cmp1 = emit_is_value_of_type(def, pinit_tt, LUA__TNUMFLT, "init.is.float");
  llvm::BasicBlock *else1_pinit_ifnum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.init.is.float", def->f);
  llvm::BasicBlock *else1_pinit_elsenum =
      llvm::BasicBlock::Create(def->jitState->context(), "if.init.not.float");
  def->builder->CreateCondBr(cmp1, else1_pinit_ifnum, else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_ifnum);

  // Already float so copy to ninit
  llvm::Instruction *pinit_nvalue_load = emit_load_reg_n(def, init);
  emit_store_local_n(def, pinit_nvalue_load, ninit);

  // Go to final section
  llvm::BasicBlock *else1_pdone =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else.done");
  def->builder->CreateBr(else1_pdone);

  // Not a float so we need to convert
  def->f->getBasicBlockList().push_back(else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_elsenum);

  // Call luaV_tonumber_()
  llvm::Value *pinit_isnum =
      CreateCall2(def->builder, def->luaV_tonumberF, init, ninit);
  llvm::Value *pinit_isnum_bool = def->builder->CreateICmpEQ(
      pinit_isnum, def->types->kInt[0], "init.float.ok");
  llvm::BasicBlock *else1_pinit_tonum_elsenum = llvm::BasicBlock::Create(
      def->jitState->context(), "if.init.float.failed", def->f);
  def->builder->CreateCondBr(pinit_isnum_bool, else1_pinit_tonum_elsenum,
                             else1_pdone);

  // Conversion failed so raise error
  def->builder->SetInsertPoint(else1_pinit_tonum_elsenum);
  emit_raise_lua_error(def, "'for' initial value must be a number");
  def->builder->CreateBr(else1_pdone);

  // Conversion OK so we are nearly done
  def->f->getBasicBlockList().push_back(else1_pdone);
  def->builder->SetInsertPoint(else1_pdone);
  llvm::Instruction *ninit_load = emit_load_local_n(def, ninit);
  nstep_load = emit_load_local_n(def, nstep);

  //    setfltvalue(init, luai_numsub(L, ninit, nstep));
  llvm::Value *init_n =
      def->builder->CreateFSub(ninit_load, nstep_load, "ninit-nstep");

  emit_store_reg_n_withtype(def, init_n, init);

  // Done so jump to forloop
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);
}

void RaviCodeGenerator::emit_iFORPREP(RaviFunctionDef *def, int A, int pc,
                                      int step_one, int pc1) {

  RaviBranchDef &forloop_target = def->jmp_targets[pc];

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());

  forloop_target.ilimit =
      TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "ilimit");
  if (!step_one) {
    forloop_target.istep =
        TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "istep");
  }
  forloop_target.iidx =
      TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "iidx");

  //    lua_Integer initv = ivalue(init);
  //    setivalue(init, initv - ivalue(pstep));

  emit_debug_trace(def, step_one ? OP_RAVI_FORPREP_I1 : OP_RAVI_FORPREP_IP,
                   pc1);
  // Load pointer to base
  emit_load_base(def);

  //  TValue *init = ra;
  //  TValue *pstep = ra + 2;
  llvm::Value *init = emit_gep_register(def, A);
  llvm::Value *plimit = emit_gep_register(def, A + 1);
  llvm::Value *pstep = nullptr;
  if (!step_one)
    pstep = emit_gep_register(def, A + 2);

  // Get ivalue(pstep)
  llvm::Instruction *limit_ivalue = emit_load_reg_i(def, plimit);
  llvm::Instruction *init_ivalue = emit_load_reg_i(def, init);

  if (!step_one) {
    //    setivalue(init, initv - ivalue(pstep));
    llvm::Instruction *step_ivalue = emit_load_reg_i(def, pstep);
    llvm::Value *idx = def->builder->CreateSub(init_ivalue, step_ivalue,
                                               "initv-pstep.i", false, true);

    // Save idx
    emit_store_local_n(def, idx, forloop_target.iidx);

    // Save step
    emit_store_local_n(def, step_ivalue, forloop_target.istep);
  } else {
    //    setivalue(init, initv - ivalue(pstep));
    llvm::Value *idx = def->builder->CreateSub(
        init_ivalue, def->types->kluaInteger[1], "initv-pstep.i", false, true);

    // Save idx
    emit_store_local_n(def, idx, forloop_target.iidx);
  }

  // Save limit
  emit_store_local_n(def, limit_ivalue, forloop_target.ilimit);

  // We are done so jump to forloop
  lua_assert(def->jmp_targets[pc].jmp1);
  def->builder->CreateBr(def->jmp_targets[pc].jmp1);
}
}
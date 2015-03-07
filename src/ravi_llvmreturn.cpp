#include "ravi_llvmcodegen.h"

namespace ravi {

void RaviCodeGenerator::emit_RETURN(RaviFunctionDef *def, llvm::Value *L_ci,
                                    llvm::Value *proto, int A, int B) {

  // Here is what OP_RETURN looks like. We only compile steps
  // marked with //*.
  // TODO that means we cannot handle functions that have sub
  // functions (closures) as do not handle the luaF_close() call

  // case OP_RETURN: {
  //  int b = GETARG_B(i);
  //*  if (b != 0) L->top = ra + b - 1;
  //*  if (cl->p->sizep > 0) luaF_close(L, base);
  //*  b = luaD_poscall(L, ra);
  //    if (!(ci->callstatus & CIST_REENTRY))  /* 'ci' still the called one */
  //      return;  /* external invocation: return */
  //    else {  /* invocation via reentry: continue execution */
  //*      ci = L->ci;
  //*      if (b) L->top = ci->top;
  //      goto newframe;  /* restart luaV_execute over new Lua function */
  //    }
  // }

  // As Lua inserts redundant OP_RETURN instructions it is
  // possible that this is one of them. If this is the case then the
  // current block may already be terminated - so we have to insert
  // a new block
  if (def->builder->GetInsertBlock()->getTerminator()) {
    llvm::BasicBlock *return_block =
        llvm::BasicBlock::Create(def->jitState->context(), "return", def->f);
    def->builder->SetInsertPoint(return_block);
  }

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_luaState_ci_baseT);

  llvm::Value *top = nullptr;

  // Get pointer to register A
  llvm::Value *ra_ptr = A == 0 ? base_ptr : emit_array_get(def, base_ptr, A);

  //*  if (b != 0) L->top = ra + b - 1;
  if (B != 0) {
    // Get pointer to register at ra + b - 1
    llvm::Value *ptr = emit_array_get(def, base_ptr, A + B - 1);
    // Get pointer to L->top
    top = emit_gep(def, "L_top", def->L, 0, 4);
    // Assign to L->top
    llvm::Instruction *ins = def->builder->CreateStore(ptr, top);
    ins->setMetadata(llvm::LLVMContext::MD_tbaa,
                     def->types->tbaa_luaState_topT);
  }

  // if (cl->p->sizep > 0) luaF_close(L, base);
  // Get pointer to Proto->sizep
  llvm::Value *psize_ptr = emit_gep(def, "sizep", def->proto_ptr, 0, 10);
  // Load psize
  llvm::Instruction *psize = def->builder->CreateLoad(psize_ptr);
  // Test if psize > 0
  llvm::Value *psize_gt_0 =
      def->builder->CreateICmpSGT(psize, def->types->kInt[0]);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(psize_gt_0, then_block, else_block);
  def->builder->SetInsertPoint(then_block);
  // Call luaF_close
  def->builder->CreateCall2(def->luaF_closeF, def->L, base_ptr);
  def->builder->CreateBr(else_block);
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  //*  b = luaD_poscall(L, ra);
  llvm::Value *result =
      def->builder->CreateCall2(def->luaD_poscallF, def->L, ra_ptr);

  //*      if (b) L->top = ci->top;
  // Test if b is != 0
  llvm::Value *result_is_notzero =
      def->builder->CreateICmpNE(result, def->types->kInt[0]);
  llvm::BasicBlock *ThenBB =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *ElseBB =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(result_is_notzero, ThenBB, ElseBB);
  def->builder->SetInsertPoint(ThenBB);
  // Get pointer to ci->top
  llvm::Value *citop = emit_gep(def, "ci_top", def->ci_val, 0, 1);
  // Load ci->top
  llvm::Instruction *citop_val = def->builder->CreateLoad(citop);
  if (!top)
    // Get L->top
    top = emit_gep(def, "L_top", def->L, 0, 4);
  // Assign ci>top to L->top
  auto ins = def->builder->CreateStore(citop_val, top);
  ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_topT);
  def->builder->CreateBr(ElseBB);
  def->f->getBasicBlockList().push_back(ElseBB);
  def->builder->SetInsertPoint(ElseBB);

  // as our prototype is lua_Cfunction we need
  // to return a value
  def->builder->CreateRet(def->types->kInt[1]);
}
}
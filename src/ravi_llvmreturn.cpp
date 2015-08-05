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

void RaviCodeGenerator::emit_RETURN(RaviFunctionDef *def, int A, int B, int pc) {

  // Here is what OP_RETURN looks like. We only compile steps
  // marked with //*. This is because the rest is only relevant in the
  // interpreter

  // case OP_RETURN: {
  //  int b = GETARG_B(i);
  //*  if (cl->p->sizep > 0) luaF_close(L, base);
  //*  b = luaD_poscall(L, ra, (b != 0 ? b - 1 : L->top - ra));
  //    if (!(ci->callstatus & CIST_REENTRY))  /* 'ci' still the called one */
  //      return;  /* external invocation: return */
  //    else {  /* invocation via reentry: continue execution */
  //      ci = L->ci;
  //      if (b) L->top = ci->top;
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

  emit_debug_trace(def, OP_RETURN, pc);
  // Load pointer to base
  emit_load_base(def);

  // Get pointer to register A
  llvm::Value *ra_ptr = emit_gep_register(def, A);

  // if (cl->p->sizep > 0) luaF_close(L, base);
  // Get pointer to Proto->sizep
  llvm::Instruction *psize = emit_load_proto_sizep(def);

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
  CreateCall2(def->builder, def->luaF_closeF, def->L, def->base_ptr);
  def->builder->CreateBr(else_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  //*  b = luaD_poscall(L, ra, (b != 0 ? b - 1 : L->top - ra));
  llvm::Value *nresults = NULL;
  if (B != 0)
    nresults = llvm::ConstantInt::get(def->types->C_intT, B - 1);
  else
    nresults = emit_num_stack_elements(def, ra_ptr);

  llvm::Value *b =
      CreateCall3(def->builder, def->luaD_poscallF, def->L, ra_ptr, nresults);

  // Reload L->ci (as L->ci would have been updated by luaD_poscall()
  // Get pointer to L->ci
  llvm::Value *L_ci = emit_gep(def, "L_ci", def->L, 0, 6);

  llvm::Instruction *ci_val = def->builder->CreateLoad(L_ci);
  ci_val->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_CallInfoT);

#if 0
  llvm::Value *b_not_zero = def->builder->CreateICmpNE(b, def->types->kInt[0]);

  // Is (L->ci->callstatus & CIST_LUA) != 0
  llvm::Value *is_Lua = emit_ci_is_Lua(def, ci_val);
  llvm::Value *is_jit = emit_is_jit_call(def, ci_val);

  llvm::Value *isNot_jit = def->builder->CreateNot(is_jit);
  llvm::Value *is_Lua_and_not_jit = def->builder->CreateAnd(is_Lua, isNot_jit);
  // b != 0 && isLua(ci) && !ci->jitstatus
  llvm::Value *b_not_zero_and_Lua =
      def->builder->CreateAnd(b_not_zero, is_Lua_and_not_jit, "b_not_zero_and_Lua");

  llvm::BasicBlock *then_block1 = llvm::BasicBlock::Create(
      def->jitState->context(), "if.poscall.returned.nonzero");
  llvm::BasicBlock *else_block1 = llvm::BasicBlock::Create(
      def->jitState->context(), "if.poscall.returned.zero");
  def->builder->CreateCondBr(b_not_zero_and_Lua, then_block1, else_block1);

  def->f->getBasicBlockList().push_back(then_block1);
  def->builder->SetInsertPoint(then_block1);

  // L->top = L->ci->top
  ///////////////////////////////////// FIXME emit_refresh_L_top(def);
  // Get pointer to ci->top
  llvm::Value *citop = emit_gep(def, "ci_top", ci_val, 0, 1);

  // Load ci->top
  llvm::Instruction *citop_val = def->builder->CreateLoad(citop);
  citop_val->setMetadata(llvm::LLVMContext::MD_tbaa,
    def->types->tbaa_CallInfo_topT);

  // Get L->top
  llvm::Value *top = emit_gep(def, "L_top", def->L, 0, 4);

  // Assign ci>top to L->top
  auto ins = def->builder->CreateStore(citop_val, top);
  ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_topT);
  ///////////////////////////////////// END FIXME


  def->builder->CreateBr(else_block1);

  def->f->getBasicBlockList().push_back(else_block1);
  def->builder->SetInsertPoint(else_block1);

  // emit_dump_stack(def, "<-- Function exit");
#endif

  def->builder->CreateRet(def->types->kInt[1]);
  //def->builder->CreateRet(nresults);
}
}
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

void RaviCodeGenerator::emit_RETURN(RaviFunctionDef *def, llvm::Value *L_ci,
                                    llvm::Value *proto, int A, int B) {

  // Here is what OP_RETURN looks like. We only compile steps
  // marked with //*.

  // case OP_RETURN: {
  //  int b = GETARG_B(i);
  //*  if (b != 0) L->top = ra + b - 1;
  //*  if (cl->p->sizep > 0) luaF_close(L, base);
  //*  b = luaD_poscall(L, ra);
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

  // Load pointer to base
  llvm::Instruction *base_ptr = emit_load_base(def);

  llvm::Value *top = nullptr;

  // Get pointer to register A
  llvm::Value *ra_ptr = A == 0 ? base_ptr : emit_array_get(def, base_ptr, A);

  //*  if (b != 0) L->top = ra + b - 1;
  if (B != 0) {
    emit_set_L_top_toreg(def, base_ptr, A + B - 1);
  }

  // if (cl->p->sizep > 0) luaF_close(L, base);
  // Get pointer to Proto->sizep
  llvm::Value *psize_ptr = emit_gep(def, "sizep", def->proto_ptr, 0, 10);
  // Load psize
  llvm::Instruction *psize = def->builder->CreateLoad(psize_ptr);
  // TODO add tbaa
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
  def->builder->CreateCall2(def->luaD_poscallF, def->L, ra_ptr);
  def->builder->CreateRet(def->types->kInt[1]);
}
}
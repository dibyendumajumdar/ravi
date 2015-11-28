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

void RaviCodeGenerator::emit_RETURN(RaviFunctionDef *def, int A, int B,
                                    int pc) {

  // Here is what OP_RETURN looks like. We only compile steps
  // marked with //*. This is because the rest is only relevant in the
  // interpreter

  // case OP_RETURN: {
  //  int b = GETARG_B(i);
  //*  if (cl->p->sizep > 0) luaF_close(L, base);
  //*  b = luaD_poscall(L, ra, (b != 0 ? b - 1 : L->top - ra));
  //   if (!(ci->callstatus & CIST_REENTRY))  /* 'ci' still the called one */
  //*     return b;  /* external invocation: return */
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
    llvm::BasicBlock *return_block = llvm::BasicBlock::Create(
        def->jitState->context(), "OP_RETURN_bridge", def->f);
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
  llvm::BasicBlock *then_block = llvm::BasicBlock::Create(
      def->jitState->context(), "OP_RETURN_if_close", def->f);
  llvm::BasicBlock *else_block = llvm::BasicBlock::Create(
      def->jitState->context(), "OP_RETURN_else_close");
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
      CreateCall4(def->builder, def->luaD_poscallF, def->L, def->ci_val, ra_ptr, nresults);
  def->builder->CreateRet(b);
}
}
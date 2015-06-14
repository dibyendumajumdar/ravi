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

#include <ravi_gccjit.h>

void ravi_emit_return(ravi_function_def_t *def, int A, int B) {

  // Here is what OP_RETURN looks like. We only compile steps
  // marked with //*. This is because the rest is only relevant in the
  // interpreter

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
  if (def->current_block_terminated) {
    def->current_block = gcc_jit_function_new_block(def->jit_function, NULL);
    def->current_block_terminated = false;
  }

  // Load pointer to base
  ravi_emit_refresh_base(def);

  // Get pointer to register A
  gcc_jit_lvalue *ra_ptr = ravi_emit_get_register(def, A);

  //*  if (b != 0) L->top = ra + b - 1;
  if (B != 0) {
    ravi_emit_set_L_top_toreg(def, A + B - 1);
  }
  (void) ra_ptr;

#if 0


  // if (cl->p->sizep > 0) luaF_close(L, base);
  // Get pointer to Proto->sizep
  llvm::Instruction *psize = emit_load_proto_sizep(def, def->proto_ptr);

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
  CreateCall2(def->builder, def->luaF_closeF, def->L, base_ptr);
  def->builder->CreateBr(else_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  //*  b = luaD_poscall(L, ra);
  CreateCall2(def->builder, def->luaD_poscallF, def->L, ra_ptr);

#if 0
  llvm::Value *msg1 =
      def->builder->CreateGlobalString("Returning from compiled function\n");
  def->builder->CreateCall(def->printfFunc, emit_gep(def, "msg", msg1, 0, 0));
#endif

  def->builder->CreateRet(def->types->kInt[1]);
#endif
}


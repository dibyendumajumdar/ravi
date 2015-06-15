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

// OP_JMP
void RaviCodeGenerator::emit_JMP(RaviFunctionDef *def, int A, int j) {

  //#define dojump(ci,i,e)
  // { int a = GETARG_A(i);
  //   if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  //   ci->u.l.savedpc += GETARG_sBx(i) + e; }
  //
  // dojump(ci, i, 0);

  assert(def->jmp_targets[j].jmp1);
  if (def->builder->GetInsertBlock()->getTerminator()) {
    llvm::BasicBlock *jmp_block =
        llvm::BasicBlock::Create(def->jitState->context(), "jump", def->f);
    def->builder->SetInsertPoint(jmp_block);
  }

  // if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  if (A > 0) {
    llvm::Instruction *base = emit_load_base(def);
    // base + a - 1
    llvm::Value *val = emit_gep_ra(def, base, A - 1);
    // Call luaF_close
    CreateCall2(def->builder, def->luaF_closeF, def->L, val);
  }

  def->builder->CreateBr(def->jmp_targets[j].jmp1);
  llvm::BasicBlock *block =
      llvm::BasicBlock::Create(def->jitState->context(), "postjump", def->f);
  def->builder->SetInsertPoint(block);
}

// Handle OP_CALL
void RaviCodeGenerator::emit_CALL(RaviFunctionDef *def, llvm::Value *L_ci,
                                  llvm::Value *proto, int A, int B, int C) {

  // int nresults = c - 1;
  // if (b != 0)
  //   L->top = ra + b;  /* else previous instruction set top */
  // int c = luaD_precall(L, ra, nresults);  /* C or JITed function? */
  // if (c) {
  //   if (c == 1 && nresults >= 0)
  //     L->top = ci->top;  /* adjust results if C function */
  // }
  // else {  /* Lua function */
  //   luaV_execute(L);
  // }

  llvm::Instruction *base_ptr = emit_load_base(def);

  // int nresults = c - 1;
  int nresults = C - 1;

  // if (b != 0)
  if (B != 0) {
    emit_set_L_top_toreg(def, base_ptr, A + B);
  }

  // luaD_precall() returns following
  // 1 - C function called, results to be adjusted
  // 2 - JITed Lua function called, no action
  // 0 - Run interpreter on Lua function

  // int c = luaD_precall(L, ra, nresults);  /* C or JITed function? */
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *precall_result =
      CreateCall3(def->builder, def->luaD_precallF, def->L, ra,
                  llvm::ConstantInt::get(def->types->C_intT, nresults));
  llvm::Value *precall_bool =
      def->builder->CreateICmpEQ(precall_result, def->types->kInt[0]);

  llvm::BasicBlock *then_block = llvm::BasicBlock::Create(
      def->jitState->context(), "if.lua.function", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.lua.function");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "op_call.done");
  def->builder->CreateCondBr(precall_bool, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  // Lua function, not compiled, so call luaV_execute
  def->builder->CreateCall(def->luaV_executeF, def->L);
  def->builder->CreateBr(end_block);
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  if (nresults >= 0) {
    // In case the precall returned 1 then a C function was
    // called so we need to update L->top
    //   if (c == 1 && nresults >= 0)
    //     L->top = ci->top;  /* adjust results if C function */
    llvm::Value *precall_C =
        def->builder->CreateICmpEQ(precall_result, def->types->kInt[1]);

    llvm::BasicBlock *then1_block = llvm::BasicBlock::Create(
        def->jitState->context(), "if.C.function", def->f);
    def->builder->CreateCondBr(precall_C, then1_block, end_block);
    def->builder->SetInsertPoint(then1_block);

    emit_refresh_L_top(def);
  }
  def->builder->CreateBr(end_block);
  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}
}
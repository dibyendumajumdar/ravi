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
void RaviCodeGenerator::emit_JMP(RaviFunctionDef *def, int A, int sBx, int pc) {

  //#define dojump(ci,i,e)
  // { int a = GETARG_A(i);
  //   if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  //   ci->u.l.savedpc += GETARG_sBx(i) + e; }
  //
  // dojump(ci, i, 0);

  assert(def->jmp_targets[sBx].jmp1);

  // If the current block is already terminated we
  // need to create a new block
  if (def->builder->GetInsertBlock()->getTerminator()) {
    llvm::BasicBlock *jmp_block = llvm::BasicBlock::Create(
        def->jitState->context(), "OP_JMP_bridge", def->f);
    def->builder->SetInsertPoint(jmp_block);
  }

  emit_debug_trace(def, OP_JMP, pc);

  // if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  if (A > 0) {
    emit_load_base(def);
    // base + a - 1
    llvm::Value *val = emit_gep_register(def, A - 1);
    // Call luaF_close
    CreateCall2(def->builder, def->luaF_closeF, def->L, val);
  }

  // Do the actual jump
  def->builder->CreateBr(def->jmp_targets[sBx].jmp1);

  // Start new block
  llvm::BasicBlock *block = llvm::BasicBlock::Create(def->jitState->context(),
                                                     "OP_JMP_postjmp", def->f);
  def->builder->SetInsertPoint(block);
}

// Handle OP_CALL
// Note that Lua assumes that functions called via OP_CALL
// are Lua functions and secondly that once OP_CALL completes the
// current function will continue within the same luaV_execute()
// call. However in a JIT case each JIT function is a different call
// so we need to take care of the behaviour differences between
// OP_CALL and external calls
void RaviCodeGenerator::emit_CALL(RaviFunctionDef *def, int A, int B, int C,
                                  int pc) {

  // int nresults = c - 1;
  // if (b != 0)
  //   L->top = ra + b;  /* else previous instruction set top */
  // int c = luaD_precall(L, ra, nresults, 1);  /* C or JITed function? */
  // if (c) {
  //   if (c == 1 && nresults >= 0)
  //     L->top = ci->top;  /* adjust results if C function */
  // }
  // else {  /* Lua function */
  //   int b = luaV_execute(L);
  //   if (b) L->top = ci->top;
  // }
  bool traced = emit_debug_trace(def, OP_CALL, pc);
  // Set savedpc before the call
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);

  // int nresults = c - 1;
  int nresults = C - 1;

  // if (b != 0)
  if (B != 0) {
    //   L->top = ra + b;  /* else previous instruction set top */
    emit_set_L_top_toreg(def, A + B);
  }

  // luaD_precall() returns following
  // 1 - C function called, results to be adjusted
  // 2 - JITed Lua function called, no action
  // 0 - Run interpreter on Lua function, if returns != 0 then update L->top

  // int c = luaD_precall(L, ra, nresults, op_call);  /* C or JITed function? */
  // The last parameter to luaD_precall() tells it that
  // this is an OP_CALL
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *precall_result =
      CreateCall4(def->builder, def->luaD_precallF, def->L, ra,
                  llvm::ConstantInt::get(def->types->C_intT, nresults),
                  def->types->kInt[1]);

  // If luaD_precall() returns 0 then we need to interpret the
  // Lua function
  llvm::Value *do_Lua_interp =
      def->builder->CreateICmpEQ(precall_result, def->types->kInt[0]);

  llvm::BasicBlock *then_block = llvm::BasicBlock::Create(
      def->jitState->context(), "OP_CALL_if_Lua_interp_function", def->f);
  llvm::BasicBlock *else_block = llvm::BasicBlock::Create(
      def->jitState->context(), "OP_CALL_if_not_Lua_interp_function");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "OP_CALL_done");
  def->builder->CreateCondBr(do_Lua_interp, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  // Lua function, not compiled, so call luaV_execute
  llvm::Value *b = def->builder->CreateCall(def->luaV_executeF, def->L);

  // If the return value is non zero then we need to refresh L->top = ci->top
  llvm::Value *b_not_zero = def->builder->CreateICmpNE(b, def->types->kInt[0]);

  llvm::BasicBlock *if_b_block = llvm::BasicBlock::Create(
      def->jitState->context(), "OP_CALL_if_need_reset_L_top", def->f);
  def->builder->CreateCondBr(b_not_zero, if_b_block, end_block);
  def->builder->SetInsertPoint(if_b_block);

  // Set L->top = ci->top
  emit_refresh_L_top(def, def->ci_val);

  // We are done
  def->builder->CreateBr(end_block);

  // Handle the C function or JIT case
  // function already executed by luaD_precall()
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
        def->jitState->context(), "OP_CALL_if_C_function_returned_values",
        def->f);
    def->builder->CreateCondBr(precall_C, then1_block, end_block);
    def->builder->SetInsertPoint(then1_block);

    emit_refresh_L_top(def, def->ci_val);
  }
  def->builder->CreateBr(end_block);
  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}
}
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

// Although the name is EQ this actually
// implements EQ, LE and LT - by using the supplied lua function to call.
void RaviCodeGenerator::emit_EQ(RaviFunctionDef *def, llvm::Value *L_ci,
                                llvm::Value *proto, int A, int B, int C, int j,
                                int jA, llvm::Constant *callee) {
  //  case OP_EQ: {
  //    TValue *rb = RKB(i);
  //    TValue *rc = RKC(i);
  //    Protect(
  //      if (cast_int(luaV_equalobj(L, rb, rc)) != GETARG_A(i))
  //        ci->u.l.savedpc++;
  //      else
  //        donextjump(ci);
  //    )
  //  } break;

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_luaState_ci_baseT);

  // Get pointer to register B
  llvm::Value *lhs_ptr = emit_gep_rkb(def, base_ptr, B);
  // Get pointer to register C
  llvm::Value *rhs_ptr = emit_gep_rkb(def, base_ptr, C);

  // Call luaV_equalobj with register B and C
  llvm::Value *result =
      def->builder->CreateCall3(callee, def->L, lhs_ptr, rhs_ptr);
  // Test if result is equal to operand A
  llvm::Value *result_eq_A = def->builder->CreateICmpEQ(
      result, llvm::ConstantInt::get(def->types->C_intT, A));
  // If result == A then we need to execute the next statement which is a jump
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(result_eq_A, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  // if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  if (jA > 0) {
    // jA is the A operand of the Jump instruction

    // Reload pointer to base as the call to luaV_equalobj() may
    // have invoked a Lua function and as a result the stack may have
    // been reallocated - so the previous base pointer could be stale
    base_ptr = def->builder->CreateLoad(def->Ci_base);
    base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_luaState_ci_baseT);

    // base + a - 1
    llvm::Value *val =
        jA == 1 ? base_ptr : emit_array_get(def, base_ptr, jA - 1);

    // Call luaF_close
    def->builder->CreateCall2(def->luaF_closeF, def->L, val);
  }
  // Do the jump
  def->builder->CreateBr(def->jmp_targets[j].jmp1);
  // Add the else block and make it current so that the next instruction flows
  // here
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
}
}
#include "ravi_llvmcodegen.h"

namespace ravi {

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

  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr;

  llvm::Value *lhs_ptr;
  llvm::Value *rhs_ptr;

  // Get pointer to register B
  llvm::Value *base_or_k = ISK(B) ? k_ptr : base_ptr;
  int b = ISK(B) ? INDEXK(B) : B;
  if (b == 0) {
    lhs_ptr = base_or_k;
  } else {
    lhs_ptr = emit_array_get(def, base_or_k, b);
  }

  // Get pointer to register C
  base_or_k = ISK(C) ? k_ptr : base_ptr;
  int c = ISK(C) ? INDEXK(C) : C;
  if (c == 0) {
    rhs_ptr = base_or_k;
  } else {
    rhs_ptr = emit_array_get(def, base_or_k, c);
  }

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

    // Load pointer to base
    llvm::Instruction *base2_ptr = def->builder->CreateLoad(def->Ci_base);
    base2_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_luaState_ci_baseT);

    // base + a - 1
    llvm::Value *val =
        jA == 1 ? base2_ptr : emit_array_get(def, base2_ptr, jA - 1);

    // Call
    def->builder->CreateCall2(def->luaF_closeF, def->L, val);
  }
  // Do the jump
  def->builder->CreateBr(def->jmp_targets[j]);
  // Add the else block and make it current so that the next instruction flows
  // here
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
}
}
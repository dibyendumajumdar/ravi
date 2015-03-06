#include "ravi_llvmcodegen.h"

namespace ravi {

void RaviCodeGenerator::emit_UNMF(RaviFunctionDef *def, llvm::Value *L_ci,
                                  llvm::Value *proto, int A, int B) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Value *result = def->builder->CreateFNeg(lhs);
  emit_store_reg_n(def, result, ra);
}

void RaviCodeGenerator::emit_UNMI(RaviFunctionDef *def, llvm::Value *L_ci,
  llvm::Value *proto, int A, int B) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Value *result = def->builder->CreateNeg(lhs, "", false, true);
  emit_store_reg_i(def, result, ra);
}

void RaviCodeGenerator::emit_ADDFN(RaviFunctionDef *def, llvm::Value *L_ci,
                                   llvm::Value *proto, int A, int B, int C) {

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_luaState_ci_baseT);

  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr;

  llvm::Value *ra;

  if (A == 0) {
    // If A is 0 we can use the base pointer which is &base[0]
    ra = base_ptr;
  } else {
    // emit &base[A]
    ra = emit_array_get(def, base_ptr, A);
  }

  llvm::Value *rb;

  // Get pointer to register B
  llvm::Value *base_or_k = ISK(B) ? k_ptr : base_ptr;
  int b = ISK(B) ? INDEXK(B) : B;
  if (b == 0) {
    rb = base_or_k;
  } else {
    rb = emit_array_get(def, base_or_k, b);
  }

  llvm::Value *rb_n = def->builder->CreateBitCast(rb, def->types->plua_NumberT);
  llvm::Instruction *lhs = def->builder->CreateLoad(rb_n);
  lhs->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);

  // Call luaV_equalobj with register B and C
  llvm::Value *result = def->builder->CreateFAdd(
      lhs,
      def->builder->CreateSIToFP(llvm::ConstantInt::get(def->types->C_intT, C),
                                 def->types->lua_NumberT));
  llvm::Value *ra_n = def->builder->CreateBitCast(ra, def->types->plua_NumberT);
  llvm::Instruction *store = def->builder->CreateStore(result, ra_n);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
}

void RaviCodeGenerator::emit_ADDIN(RaviFunctionDef *def, llvm::Value *L_ci,
                                   llvm::Value *proto, int A, int B, int C) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Value *result = def->builder->CreateAdd(
      lhs, llvm::ConstantInt::get(def->types->lua_IntegerT, C), "", false, true);
  emit_store_reg_i(def, result, ra);
}

}
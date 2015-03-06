#include "ravi_llvmcodegen.h"

namespace ravi {

// -float
void RaviCodeGenerator::emit_UNMF(RaviFunctionDef *def, llvm::Value *L_ci,
                                  llvm::Value *proto, int A, int B) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Value *result = def->builder->CreateFNeg(lhs);
  emit_store_reg_n(def, result, ra);
}

// -int
void RaviCodeGenerator::emit_UNMI(RaviFunctionDef *def, llvm::Value *L_ci,
                                  llvm::Value *proto, int A, int B) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Value *result = def->builder->CreateNeg(lhs, "", false, true);
  emit_store_reg_i(def, result, ra);
}

// float+c
void RaviCodeGenerator::emit_ADDFN(RaviFunctionDef *def, llvm::Value *L_ci,
                                   llvm::Value *proto, int A, int B, int C) {

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Value *result = def->builder->CreateFAdd(
      lhs,
      def->builder->CreateSIToFP(llvm::ConstantInt::get(def->types->C_intT, C),
                                 def->types->lua_NumberT));
  emit_store_reg_n(def, result, ra);
}

// float+float
void RaviCodeGenerator::emit_ADDFF(RaviFunctionDef *def, llvm::Value *L_ci,
                                   llvm::Value *proto, int A, int B, int C) {

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Instruction *rhs = emit_load_reg_n(def, rc);
  llvm::Value *result = def->builder->CreateFAdd(lhs, rhs);
  emit_store_reg_n(def, result, ra);
}

// float+int
void RaviCodeGenerator::emit_ADDFI(RaviFunctionDef *def, llvm::Value *L_ci,
                                   llvm::Value *proto, int A, int B, int C) {

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateFAdd(
      lhs, def->builder->CreateSIToFP(rhs, def->types->lua_NumberT));
  emit_store_reg_n(def, result, ra);
}

// int+int
void RaviCodeGenerator::emit_ADDII(RaviFunctionDef *def, llvm::Value *L_ci,
                                   llvm::Value *proto, int A, int B, int C) {

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Value *rc = emit_gep_rkb(def, base_ptr, C);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateAdd(lhs, rhs, "", false, true);
  emit_store_reg_i(def, result, ra);
}

// int+c
void RaviCodeGenerator::emit_ADDIN(RaviFunctionDef *def, llvm::Value *L_ci,
                                   llvm::Value *proto, int A, int B, int C) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *rb = emit_gep_rkb(def, base_ptr, B);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Value *result = def->builder->CreateAdd(
      lhs, llvm::ConstantInt::get(def->types->lua_IntegerT, C), "", false,
      true);
  emit_store_reg_i(def, result, ra);
}
}
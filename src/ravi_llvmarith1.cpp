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

// This file contains LLVM IR generation for Ravi's arithmetic
// op codes - these are enhanced type specific op codes not part of
// standard Lua

namespace ravi {

// R(A) := -R(B), floating point
void RaviCodeGenerator::emit_UNMF(RaviFunctionDef *def, int A, int B) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Value *result = def->builder->CreateFNeg(lhs);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := -R(B), integer
void RaviCodeGenerator::emit_UNMI(RaviFunctionDef *def, int A, int B) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Value *result = def->builder->CreateNeg(lhs, "", false, true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) + C, result is floating
void RaviCodeGenerator::emit_ADDFN(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Value *result = def->builder->CreateFAdd(
      lhs,
      def->builder->CreateSIToFP(llvm::ConstantInt::get(def->types->C_intT, C),
                                 def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) + RK(C), all floating
void RaviCodeGenerator::emit_ADDFF(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Instruction *rhs = emit_load_reg_n(def, rc);
  llvm::Value *result = def->builder->CreateFAdd(lhs, rhs);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) + RK(C), float+int
void RaviCodeGenerator::emit_ADDFI(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateFAdd(
      lhs, def->builder->CreateSIToFP(rhs, def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) + RK(C), int+int
void RaviCodeGenerator::emit_ADDII(RaviFunctionDef *def, int A, int B, int C) {

  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateAdd(lhs, rhs, "", false, true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) + C, int+c
void RaviCodeGenerator::emit_ADDIN(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Value *result = def->builder->CreateAdd(
      lhs, llvm::ConstantInt::get(def->types->lua_IntegerT, C), "", false,
      true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), float-float
void RaviCodeGenerator::emit_SUBFF(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Instruction *rhs = emit_load_reg_n(def, rc);
  llvm::Value *result = def->builder->CreateFSub(lhs, rhs);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), float-int
void RaviCodeGenerator::emit_SUBFI(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateFSub(
      lhs, def->builder->CreateSIToFP(rhs, def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), int-float
void RaviCodeGenerator::emit_SUBIF(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Instruction *rhs = emit_load_reg_n(def, rc);
  llvm::Value *result = def->builder->CreateFSub(
      def->builder->CreateSIToFP(lhs, def->types->lua_NumberT), rhs);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), int-int
void RaviCodeGenerator::emit_SUBII(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateSub(lhs, rhs, "", false, true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) - C, float - c
void RaviCodeGenerator::emit_SUBFN(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Value *result = def->builder->CreateFSub(
      lhs,
      def->builder->CreateSIToFP(llvm::ConstantInt::get(def->types->C_intT, C),
                                 def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := B - RK(C), b - float
void RaviCodeGenerator::emit_SUBNF(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *rhs = emit_load_reg_n(def, rc);
  llvm::Value *result = def->builder->CreateFSub(
      def->builder->CreateSIToFP(llvm::ConstantInt::get(def->types->C_intT, B),
                                 def->types->lua_NumberT),
      rhs);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := B - RK(C), b - int
void RaviCodeGenerator::emit_SUBIN(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Value *result = def->builder->CreateSub(
      lhs, llvm::ConstantInt::get(def->types->lua_IntegerT, C), "", false,
      true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) - C, int - c
void RaviCodeGenerator::emit_SUBNI(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateSub(
      llvm::ConstantInt::get(def->types->lua_IntegerT, C), rhs, "", false,
      true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) * C, float*c
void RaviCodeGenerator::emit_MULFN(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Value *result = def->builder->CreateFMul(
      lhs,
      def->builder->CreateSIToFP(llvm::ConstantInt::get(def->types->C_intT, C),
                                 def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) * RK(C), float*float
void RaviCodeGenerator::emit_MULFF(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Instruction *rhs = emit_load_reg_n(def, rc);
  llvm::Value *result = def->builder->CreateFMul(lhs, rhs);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) * RK(C), float*int
void RaviCodeGenerator::emit_MULFI(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateFMul(
      lhs, def->builder->CreateSIToFP(rhs, def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) * RK(C), int*int
void RaviCodeGenerator::emit_MULII(RaviFunctionDef *def, int A, int B, int C) {

  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateMul(lhs, rhs, "", false, true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) * C, int*c
void RaviCodeGenerator::emit_MULIN(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Value *result = def->builder->CreateMul(
      lhs, llvm::ConstantInt::get(def->types->lua_IntegerT, C), "", false,
      true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), float/float
void RaviCodeGenerator::emit_DIVFF(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Instruction *rhs = emit_load_reg_n(def, rc);
  llvm::Value *result = def->builder->CreateFDiv(lhs, rhs);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), float/int
void RaviCodeGenerator::emit_DIVFI(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateFDiv(
      lhs, def->builder->CreateSIToFP(rhs, def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), int/float
void RaviCodeGenerator::emit_DIVIF(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Instruction *rhs = emit_load_reg_n(def, rc);
  llvm::Value *result = def->builder->CreateFDiv(
      def->builder->CreateSIToFP(lhs, def->types->lua_NumberT), rhs);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), int/int but result is float
void RaviCodeGenerator::emit_DIVII(RaviFunctionDef *def, int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Instruction *rhs = emit_load_reg_i(def, rc);
  llvm::Value *result = def->builder->CreateFDiv(
      def->builder->CreateSIToFP(lhs, def->types->lua_NumberT),
      def->builder->CreateSIToFP(rhs, def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) + RK(C), int+int
void RaviCodeGenerator::emit_BITWISE_BINARY_OP(RaviFunctionDef *def, OpCode op, int A, int B, int C) {

  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);

  //llvm::Value *rb = emit_gep_register_or_constant(def, B);
  //llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Value *lhs = emit_load_register_or_constant_i(def, B);
  llvm::Value *rhs = emit_load_register_or_constant_i(def, C);

  llvm::Value *result = NULL;

  switch (op) {
  case OP_RAVI_BAND_II:
    result = def->builder->CreateAnd(lhs, rhs, "");
    break;
  case OP_RAVI_BOR_II:
    result = def->builder->CreateOr(lhs, rhs, "");
    break;
  case OP_RAVI_BXOR_II:
    result = def->builder->CreateXor(lhs, rhs, "");
    break;
  default:
    fprintf(stderr, "unexpected value of opcode %d\n", (int)op);
    lua_assert(false);
    abort();
  }
  emit_store_reg_i_withtype(def, result, ra);
}

void RaviCodeGenerator::emit_BITWISE_SHIFT_OP(RaviFunctionDef *def, OpCode op,
                                              int A, int B, int C) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  switch (op) {
  // FIXME Lua has specific requirements for shift operators
  case OP_SHL:
  case OP_RAVI_SHL_II:
    CreateCall4(def->builder, def->raviV_op_shlF, def->L, ra, rb, rc);
    break;
  case OP_SHR:
  case OP_RAVI_SHR_II:
    CreateCall4(def->builder, def->raviV_op_shrF, def->L, ra, rb, rc);
    break;
  default:
    fprintf(stderr, "unexpected value of opcode %d\n", (int)op);
    lua_assert(false);
    abort();
  }
}

void RaviCodeGenerator::emit_BNOT_I(RaviFunctionDef *def, int A, int B) {
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  //llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *lhs = emit_load_register_or_constant_i(def, B);
  llvm::Value *rhs = llvm::ConstantInt::get(def->types->lua_IntegerT, -1);
  llvm::Value *result = def->builder->CreateXor(lhs, rhs, "");
  emit_store_reg_i_withtype(def, result, ra);
}

}
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
// NOT yet used
void RaviCodeGenerator::emit_UNMF(RaviFunctionDef *def, int A, int B, int pc) {
  (void)pc;
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Instruction *lhs = emit_load_reg_n(def, rb);
  llvm::Value *result = def->builder->CreateFNeg(lhs);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := -R(B), integer
// NOT yet used
void RaviCodeGenerator::emit_UNMI(RaviFunctionDef *def, int A, int B, int pc) {
  (void)pc;
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Instruction *lhs = emit_load_reg_i(def, rb);
  llvm::Value *result = def->builder->CreateNeg(lhs, "", false, true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) + RK(C), all floating
void RaviCodeGenerator::emit_ADDFF(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_ADDFF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_n(def, B);
  llvm::Value *rc = emit_load_register_or_constant_n(def, C);
  llvm::Value *result = def->builder->CreateFAdd(rb, rc);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) + RK(C), float+int
void RaviCodeGenerator::emit_ADDFI(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_ADDFI, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_n(def, B);
  llvm::Value *rc = emit_load_register_or_constant_i(def, C);
  llvm::Value *result = def->builder->CreateFAdd(
      rb, def->builder->CreateSIToFP(rc, def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) + RK(C), int+int
void RaviCodeGenerator::emit_ADDII(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_ADDII, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_i(def, B);
  llvm::Value *rc = emit_load_register_or_constant_i(def, C);
  llvm::Value *result =
      def->builder->CreateAdd(rb, rc, "OP_RAVI_ADDII_result", false, true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), float-float
void RaviCodeGenerator::emit_SUBFF(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_SUBFF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_n(def, B);
  llvm::Value *rc = emit_load_register_or_constant_n(def, C);
  llvm::Value *result = def->builder->CreateFSub(rb, rc);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), float-int
void RaviCodeGenerator::emit_SUBFI(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_SUBFI, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_n(def, B);
  llvm::Value *rc = emit_load_register_or_constant_i(def, C);
  llvm::Value *result = def->builder->CreateFSub(
      rb, def->builder->CreateSIToFP(rc, def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), int-float
void RaviCodeGenerator::emit_SUBIF(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_SUBIF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_i(def, B);
  llvm::Value *rc = emit_load_register_or_constant_n(def, C);
  llvm::Value *result = def->builder->CreateFSub(
      def->builder->CreateSIToFP(rb, def->types->lua_NumberT), rc);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) - RK(C), int-int
void RaviCodeGenerator::emit_SUBII(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_SUBII, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_i(def, B);
  llvm::Value *rc = emit_load_register_or_constant_i(def, C);
  llvm::Value *result =
      def->builder->CreateSub(rb, rc, "OP_RAVI_SUBII_result", false, true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) * RK(C), float*float
void RaviCodeGenerator::emit_MULFF(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_MULFF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_n(def, B);
  llvm::Value *rc = emit_load_register_or_constant_n(def, C);
  llvm::Value *result = def->builder->CreateFMul(rb, rc);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) * RK(C), float*int
void RaviCodeGenerator::emit_MULFI(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_MULFI, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_n(def, B);
  llvm::Value *rc = emit_load_register_or_constant_i(def, C);
  llvm::Value *result = def->builder->CreateFMul(
      rb, def->builder->CreateSIToFP(rc, def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) * RK(C), int*int
void RaviCodeGenerator::emit_MULII(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_MULII, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_i(def, B);
  llvm::Value *rc = emit_load_register_or_constant_i(def, C);
  llvm::Value *result =
      def->builder->CreateMul(rb, rc, "OP_RAVI_MULII_result", false, true);
  emit_store_reg_i_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), float/float
void RaviCodeGenerator::emit_DIVFF(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_DIVFF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_n(def, B);
  llvm::Value *rc = emit_load_register_or_constant_n(def, C);
  llvm::Value *result = def->builder->CreateFDiv(rb, rc);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), float/int
void RaviCodeGenerator::emit_DIVFI(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_DIVFI, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_n(def, B);
  llvm::Value *rc = emit_load_register_or_constant_i(def, C);
  llvm::Value *result = def->builder->CreateFDiv(
      rb, def->builder->CreateSIToFP(rc, def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), int/float
void RaviCodeGenerator::emit_DIVIF(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_DIVIF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_i(def, B);
  llvm::Value *rc = emit_load_register_or_constant_n(def, C);
  llvm::Value *result = def->builder->CreateFDiv(
      def->builder->CreateSIToFP(rb, def->types->lua_NumberT), rc);
  emit_store_reg_n_withtype(def, result, ra);
}

// R(A) := RK(B) / RK(C), int/int but result is float
void RaviCodeGenerator::emit_DIVII(RaviFunctionDef *def, int A, int B, int C,
                                   int pc) {
  emit_debug_trace(def, OP_RAVI_DIVII, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_load_register_or_constant_i(def, B);
  llvm::Value *rc = emit_load_register_or_constant_i(def, C);
  llvm::Value *result = def->builder->CreateFDiv(
      def->builder->CreateSIToFP(rb, def->types->lua_NumberT),
      def->builder->CreateSIToFP(rc, def->types->lua_NumberT));
  emit_store_reg_n_withtype(def, result, ra);
}

// Bitwise AND, OR and XOR when both operands are known to be
// integers
void RaviCodeGenerator::emit_BITWISE_BINARY_OP(RaviFunctionDef *def, OpCode op,
                                               int A, int B, int C, int pc) {
  emit_debug_trace(def, op, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);

  llvm::Value *lhs = emit_load_register_or_constant_i(def, B);
  llvm::Value *rhs = emit_load_register_or_constant_i(def, C);

  llvm::Value *result = NULL;

  switch (op) {
    case OP_RAVI_BAND_II:
      result = def->builder->CreateAnd(lhs, rhs, "OP_RAVI_BAND_II_result");
      break;
    case OP_RAVI_BOR_II:
      result = def->builder->CreateOr(lhs, rhs, "OP_RAVI_BOR_II_result");
      break;
    case OP_RAVI_BXOR_II:
      result = def->builder->CreateXor(lhs, rhs, "OP_RAVI_BXOR_II_result");
      break;
    default:
      fprintf(stderr, "unexpected value of opcode %d\n", (int)op);
      abort();
  }
  emit_store_reg_i_withtype(def, result, ra);
}

/* number of bits in an integer */
#define NBITS cast_int(sizeof(lua_Integer) * CHAR_BIT)

// Handle the case when we have a constant RHS and the LHS is an integer
void RaviCodeGenerator::emit_bitwise_shiftl(RaviFunctionDef *def,
                                            llvm::Value *ra, int B,
                                            lua_Integer y) {
  if (y < 0) { /* shift right? */
    if (y <= -NBITS)
      emit_store_reg_i_withtype(
          def, llvm::ConstantInt::get(def->types->lua_IntegerT, 0), ra);
    else {
      llvm::Value *rb = emit_load_register_or_constant_i(def, B);
      llvm::Value *result = def->builder->CreateLShr(
          rb, llvm::ConstantInt::get(def->types->lua_IntegerT, -y));
      emit_store_reg_i_withtype(def, result, ra);
    }
  }
  else {
    if (y >= NBITS)
      emit_store_reg_i_withtype(
          def, llvm::ConstantInt::get(def->types->lua_IntegerT, 0), ra);
    else {
      llvm::Value *rb = emit_load_register_or_constant_i(def, B);
      llvm::Value *result = def->builder->CreateShl(
          rb, llvm::ConstantInt::get(def->types->lua_IntegerT, y));
      emit_store_reg_i_withtype(def, result, ra);
    }
  }
}

void RaviCodeGenerator::emit_BITWISE_SHIFT_OP(RaviFunctionDef *def, OpCode op,
                                              int A, int B, int C, int pc) {
  bool traced = emit_debug_trace(def, op, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);

  // If the RHS is a constant and we know that LHS is
  // and integer then we can optimize the code generation
  if (op == OP_RAVI_SHL_II && ISK(C)) {
    lua_Integer y = def->p->k[INDEXK(C)].value_.i;
    emit_bitwise_shiftl(def, ra, B, y);
  }
  else if (op == OP_RAVI_SHR_II && ISK(C)) {
    lua_Integer y = def->p->k[INDEXK(C)].value_.i;
    emit_bitwise_shiftl(def, ra, B, -y);
  }
  else {
    // RHS is not a constant
    llvm::Value *rc = emit_gep_register_or_constant(def, C);
    llvm::Value *rb = emit_gep_register_or_constant(def, B);

    // Since the Lua OP_SHL and OP_SHR bytecodes
    // could invoke metamethods we need to set
    // 'savedpc'
    switch (op) {
      case OP_SHL:
        if (!traced) emit_update_savedpc(def, pc);
      case OP_RAVI_SHL_II:
        CreateCall4(def->builder, def->raviV_op_shlF, def->L, ra, rb, rc);
        break;
      case OP_SHR:
        if (!traced) emit_update_savedpc(def, pc);
      case OP_RAVI_SHR_II:
        CreateCall4(def->builder, def->raviV_op_shrF, def->L, ra, rb, rc);
        break;
      default:
        fprintf(stderr, "unexpected value of opcode %d\n", (int)op);
        abort();
    }
  }
}

//	R(A) := ~R(B); known integer operand
void RaviCodeGenerator::emit_BNOT_I(RaviFunctionDef *def, int A, int B,
                                    int pc) {
  emit_debug_trace(def, OP_RAVI_BNOT_I, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *lhs = emit_load_register_or_constant_i(def, B);
  llvm::Value *rhs = llvm::ConstantInt::get(def->types->lua_IntegerT, -1);
  llvm::Value *result = def->builder->CreateXor(lhs, rhs, "");
  emit_store_reg_i_withtype(def, result, ra);
}

void RaviCodeGenerator::emit_BOR_BXOR_BAND(RaviFunctionDef *def, OpCode op,
                                           int A, int B, int C, int pc) {
  bool traced = emit_debug_trace(def, op, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder,
              op == OP_BOR
                  ? def->raviV_op_borF
                  : (op == OP_BAND ? def->raviV_op_bandF : def->raviV_op_bxorF),
              def->L, ra, rb, rc);
}

void RaviCodeGenerator::emit_BNOT(RaviFunctionDef *def, int A, int B, int pc) {
  bool traced = emit_debug_trace(def, OP_BNOT, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register(def, B);
  CreateCall3(def->builder, def->raviV_op_bnotF, def->L, ra, rb);
}
}
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

// R(A+1), ..., R(A+B) := nil
void RaviCodeGenerator::emit_LOADNIL(RaviFunctionDef *def, int A, int B,
                                     int pc) {
  emit_debug_trace(def, OP_LOADNIL, pc);
#if 1
  // Inline version, we unroll the loop
  emit_load_base(def);
  do {
    llvm::Value *dest = emit_gep_register(def, A);
    emit_store_type_(def, dest, LUA_TNIL);
    A++;
  } while (B--);
#else
  CreateCall3(def->builder, def->raviV_op_loadnilF, def->ci_val,
              llvm::ConstantInt::get(def->types->C_intT, A),
              llvm::ConstantInt::get(def->types->C_intT, B));
#endif
}

// R(A) := tonumber(0)
void RaviCodeGenerator::emit_LOADFZ(RaviFunctionDef *def, int A, int pc) {
  emit_debug_trace(def, OP_RAVI_LOADFZ, pc);
  emit_load_base(def);
  llvm::Value *dest = emit_gep_register(def, A);
  // destvalue->n = 0.0
  emit_store_reg_n_withtype(
      def, llvm::ConstantFP::get(def->types->lua_NumberT, 0.0), dest);
}

// R(A) := tointeger(0)
void RaviCodeGenerator::emit_LOADIZ(RaviFunctionDef *def, int A, int pc) {
  emit_debug_trace(def, OP_RAVI_LOADIZ, pc);
  emit_load_base(def);
  llvm::Value *dest = emit_gep_register(def, A);
  // dest->i = 0
  emit_store_reg_i_withtype(def, def->types->kluaInteger[0], dest);
}

// R(A) := (Bool)B; if (C) pc++
void RaviCodeGenerator::emit_LOADBOOL(RaviFunctionDef *def, int A, int B, int C,
                                      int j, int pc) {
  // setbvalue(ra, GETARG_B(i));
  // if (GETARG_C(i)) ci->u.l.savedpc++;  /* skip next instruction (if C) */

  emit_debug_trace(def, OP_LOADBOOL, pc);
  emit_load_base(def);
  llvm::Value *dest = emit_gep_register(def, A);
  // dest->i = 0
  emit_store_reg_b_withtype(def, llvm::ConstantInt::get(def->types->C_intT, B),
                            dest);
  if (C) {
    // Skip next instruction if C
    def->builder->CreateBr(def->jmp_targets[j].jmp1);
    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(def->jitState->context(), "nextblock", def->f);
    def->builder->SetInsertPoint(block);
  }
}

// R(A) := R(B)
void RaviCodeGenerator::emit_MOVE(RaviFunctionDef *def, int A, int B, int pc) {
  // setobjs2s(L, ra, RB(i));

  emit_debug_trace(def, OP_MOVE, pc);
  // Load pointer to base
  emit_load_base(def);

  lua_assert(A != B);

  llvm::Value *src = emit_gep_register(def, B);
  llvm::Value *dest = emit_gep_register(def, A);
  emit_assign(def, dest, src);
}

// R(A) := R(B), check R(B) is int
void RaviCodeGenerator::emit_MOVEI(RaviFunctionDef *def, int A, int B, int pc) {
  // TValue *rb = RB(i);
  // lua_Integer j;
  // if (tointeger(rb, &j)) {
  //   setivalue(ra, j);
  // }
  // else
  //   luaG_runerror(L, "integer expected");

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *var = TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "i");

  emit_debug_trace(def, OP_RAVI_MOVEI, pc);
  // Load pointer to base
  emit_load_base(def);

  llvm::Value *dest = emit_gep_register(def, A);
  llvm::Value *src = emit_gep_register(def, B);

  llvm::Value *src_type = emit_load_type(def, src);

  // Compare src->tt == LUA_TNUMINT
  llvm::Value *cmp1 =
      emit_is_value_of_type(def, src_type, LUA__TNUMINT, "is.src.integer");

  llvm::BasicBlock *then1 =
      llvm::BasicBlock::Create(def->jitState->context(), "if.integer", def->f);
  llvm::BasicBlock *else1 =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.integer");
  llvm::BasicBlock *end1 =
      llvm::BasicBlock::Create(def->jitState->context(), "done");

  def->builder->CreateCondBr(cmp1, then1, else1);
  def->builder->SetInsertPoint(then1);

  // Already a int - move
  llvm::Instruction *tmp = emit_load_reg_i(def, src);
  emit_store_local_n(def, tmp, var);
  def->builder->CreateBr(end1);

  // we need to convert
  def->f->getBasicBlockList().push_back(else1);
  def->builder->SetInsertPoint(else1);
  // Call luaV_tointeger_()

  llvm::Value *var_isint =
      CreateCall2(def->builder, def->luaV_tointegerF, src, var);
  llvm::Value *tobool = def->builder->CreateICmpEQ(
      var_isint, def->types->kInt[0], "int.conversion.failed");

  // Did conversion fail?
  llvm::BasicBlock *else2 = llvm::BasicBlock::Create(
      def->jitState->context(), "if.conversion.failed", def->f);
  def->builder->CreateCondBr(tobool, else2, end1);

  // Conversion failed, so raise error
  def->builder->SetInsertPoint(else2);
  emit_raise_lua_error(def, "integer expected");
  def->builder->CreateBr(end1);

  // Conversion OK
  def->f->getBasicBlockList().push_back(end1);
  def->builder->SetInsertPoint(end1);

  auto load_var = emit_load_local_n(def, var);
  emit_store_reg_i_withtype(def, load_var, dest);
}

void RaviCodeGenerator::emit_MOVEF(RaviFunctionDef *def, int A, int B, int pc) {
  //  case OP_RAVI_MOVEF: {
  //    TValue *rb = RB(i);
  //    lua_Number j;
  //    if (tonumber(rb, &j)) {
  //      setfltvalue(ra, j);
  //    }
  //    else
  //      luaG_runerror(L, "float expected");
  //  } break;

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *var = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "n");

  emit_debug_trace(def, OP_RAVI_MOVEF, pc);
  // Load pointer to base
  emit_load_base(def);

  llvm::Value *dest = emit_gep_register(def, A);
  llvm::Value *src = emit_gep_register(def, B);

  llvm::Value *src_type = emit_load_type(def, src);

  // Compare src->tt == LUA_TNUMFLT
  llvm::Value *cmp1 =
      emit_is_value_of_type(def, src_type, LUA__TNUMFLT, "is.src.float");

  llvm::BasicBlock *then1 =
      llvm::BasicBlock::Create(def->jitState->context(), "if.float", def->f);
  llvm::BasicBlock *else1 =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.float");
  llvm::BasicBlock *end1 =
      llvm::BasicBlock::Create(def->jitState->context(), "done");

  def->builder->CreateCondBr(cmp1, then1, else1);
  def->builder->SetInsertPoint(then1);

  // Already a float - copy to var
  llvm::Instruction *tmp = emit_load_reg_n(def, src);
  emit_store_local_n(def, tmp, var);
  def->builder->CreateBr(end1);

  // we need to convert
  def->f->getBasicBlockList().push_back(else1);
  def->builder->SetInsertPoint(else1);
  // Call luaV_tonumber()

  llvm::Value *var_isflt =
      CreateCall2(def->builder, def->luaV_tonumberF, src, var);
  llvm::Value *tobool = def->builder->CreateICmpEQ(
      var_isflt, def->types->kInt[0], "float.conversion.failed");

  // Did conversion fail?
  llvm::BasicBlock *else2 = llvm::BasicBlock::Create(
      def->jitState->context(), "if.conversion.failed", def->f);
  def->builder->CreateCondBr(tobool, else2, end1);

  // Conversion failed, so raise error
  def->builder->SetInsertPoint(else2);
  emit_raise_lua_error(def, "number expected");
  def->builder->CreateBr(end1);

  // Conversion OK
  def->f->getBasicBlockList().push_back(end1);
  def->builder->SetInsertPoint(end1);

  // Set R(A)
  auto load_var = emit_load_local_n(def, var);
  emit_store_reg_n_withtype(def, load_var, dest);
}

void RaviCodeGenerator::emit_TOINT(RaviFunctionDef *def, int A, int pc) {
  //  case OP_RAVI_TOINT: {
  //    lua_Integer j;
  //    if (tointeger(ra, &j)) {
  //      setivalue(ra, j);
  //    }
  //    else
  //      luaG_runerror(L, "integer expected");
  //  } break;

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *var = TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "i");

  emit_debug_trace(def, OP_RAVI_TOINT, pc);
  // Load pointer to base
  emit_load_base(def);

  llvm::Value *dest = emit_gep_register(def, A);
  llvm::Value *src = dest;

  llvm::Value *src_type = emit_load_type(def, src);

  // Is src->tt != LUA_TNUMINT?
  llvm::Value *cmp1 =
      emit_is_not_value_of_type(def, src_type, LUA__TNUMINT, "is.not.integer");

  llvm::BasicBlock *then1 = llvm::BasicBlock::Create(def->jitState->context(),
                                                     "if.not.integer", def->f);
  llvm::BasicBlock *end1 =
      llvm::BasicBlock::Create(def->jitState->context(), "done");
  def->builder->CreateCondBr(cmp1, then1, end1);
  def->builder->SetInsertPoint(then1);

  // Call luaV_tointeger_()
  llvm::Value *var_isint =
      CreateCall2(def->builder, def->luaV_tointegerF, src, var);
  llvm::Value *tobool = def->builder->CreateICmpEQ(
      var_isint, def->types->kInt[0], "int.conversion.failed");

  // Did conversion fail?
  llvm::BasicBlock *then2 = llvm::BasicBlock::Create(
      def->jitState->context(), "if.conversion.failed", def->f);
  llvm::BasicBlock *else2 =
      llvm::BasicBlock::Create(def->jitState->context(), "conversion.ok");
  def->builder->CreateCondBr(tobool, then2, else2);
  def->builder->SetInsertPoint(then2);

  // Conversion failed, so raise error
  emit_raise_lua_error(def, "integer expected");
  def->builder->CreateBr(else2);

  // Conversion OK
  def->f->getBasicBlockList().push_back(else2);
  def->builder->SetInsertPoint(else2);

  auto load_var = emit_load_local_n(def, var);
  emit_store_reg_i_withtype(def, load_var, dest);
  def->builder->CreateBr(end1);

  def->f->getBasicBlockList().push_back(end1);
  def->builder->SetInsertPoint(end1);
}

void RaviCodeGenerator::emit_TOFLT(RaviFunctionDef *def, int A, int pc) {
  //  case OP_RAVI_TOFLT: {
  //    lua_Number j;
  //    if (tonumber(ra, &j)) {
  //      setfltvalue(ra, j);
  //    }
  //    else
  //      luaG_runerror(L, "float expected");
  //  } break;

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *var = TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "n");

  emit_debug_trace(def, OP_RAVI_TOFLT, pc);
  // Load pointer to base
  emit_load_base(def);

  llvm::Value *dest = emit_gep_register(def, A);
  llvm::Value *src = dest;

  llvm::Value *src_type = emit_load_type(def, src);

  // Is src->tt != LUA_TNUMFLT?
  llvm::Value *cmp1 =
      emit_is_not_value_of_type(def, src_type, LUA__TNUMFLT, "is.not.float");

  llvm::BasicBlock *then1 = llvm::BasicBlock::Create(def->jitState->context(),
                                                     "if.not.float", def->f);
  llvm::BasicBlock *end1 =
      llvm::BasicBlock::Create(def->jitState->context(), "done");
  def->builder->CreateCondBr(cmp1, then1, end1);
  def->builder->SetInsertPoint(then1);

  // Call luaV_tonumber()
  llvm::Value *var_isflt =
      CreateCall2(def->builder, def->luaV_tonumberF, src, var);
  llvm::Value *tobool = def->builder->CreateICmpEQ(
      var_isflt, def->types->kInt[0], "float.conversion.failed");

  // Did conversion fail?
  llvm::BasicBlock *then2 = llvm::BasicBlock::Create(
      def->jitState->context(), "if.conversion.failed", def->f);
  llvm::BasicBlock *else2 =
      llvm::BasicBlock::Create(def->jitState->context(), "conversion.ok");
  def->builder->CreateCondBr(tobool, then2, else2);
  def->builder->SetInsertPoint(then2);

  // Conversion failed, so raise error
  emit_raise_lua_error(def, "number expected");
  def->builder->CreateBr(else2);

  // Conversion OK
  def->f->getBasicBlockList().push_back(else2);
  def->builder->SetInsertPoint(else2);

  auto load_var = emit_load_local_n(def, var);
  emit_store_reg_n_withtype(def, load_var, dest);
  def->builder->CreateBr(end1);

  def->f->getBasicBlockList().push_back(end1);
  def->builder->SetInsertPoint(end1);
}

void RaviCodeGenerator::emit_LOADK(RaviFunctionDef *def, int A, int Bx,
                                   int pc) {
  // TValue *rb = k + GETARG_Bx(i);
  // setobj2s(L, ra, rb);

  emit_debug_trace(def, OP_LOADK, pc);
  // Load pointer to base
  emit_load_base(def);

  // LOADK requires a structure assignment
  // in LLVM as far as I can tell this requires a call to
  // an intrinsic memcpy
  llvm::Value *dest = emit_gep_register(def, A);

  TValue *Konst = &def->p->k[Bx];
  switch (Konst->tt_) {
    case LUA_TNUMINT:
      emit_store_reg_i_withtype(
          def,
          llvm::ConstantInt::get(def->types->lua_IntegerT, Konst->value_.i),
          dest);
      break;
    case LUA_TNUMFLT:
      emit_store_reg_n_withtype(
          def, llvm::ConstantFP::get(def->types->lua_NumberT, Konst->value_.n),
          dest);
      break;
    case LUA_TBOOLEAN:
      emit_store_reg_b_withtype(
          def, llvm::ConstantInt::get(def->types->C_intT, Konst->value_.b),
          dest);
      break;
    default: {
      // rb
      llvm::Value *src = emit_gep_constant(def, Bx);

      // *ra = *rb
      emit_assign(def, dest, src);
    }
  }
}

void RaviCodeGenerator::emit_assign(RaviFunctionDef *def, llvm::Value *dest,
                                    llvm::Value *src) {
  // Below is more efficient that memcpy()
  // destvalue->value->i = srcvalue->value->i;
  // destvalue->value->tt = srcvalue->value->tt;
  llvm::Value *srcvalue = emit_gep(def, "srcvalue", src, 0, 0, 0);
  llvm::Value *destvalue = emit_gep(def, "destvalue", dest, 0, 0, 0);
  llvm::Instruction *load = def->builder->CreateLoad(srcvalue);
  load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Instruction *store = def->builder->CreateStore(load, destvalue);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);

  // destvalue->type = srcvalue->type
  llvm::Value *srctype = emit_gep(def, "srctype", src, 0, 1);
  llvm::Value *desttype = emit_gep(def, "desttype", dest, 0, 1);
  load = def->builder->CreateLoad(srctype);
  load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  store = def->builder->CreateStore(load, desttype);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
}
}
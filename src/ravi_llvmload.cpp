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

void RaviCodeGenerator::emit_LOADNIL(RaviFunctionDef *def, llvm::Value *L_ci,
                                     llvm::Value *proto, int A, int B) {
  def->builder->CreateCall3(def->luaV_op_loadnilF, def->ci_val,
                            llvm::ConstantInt::get(def->types->C_intT, A),
                            llvm::ConstantInt::get(def->types->C_intT, B));
}

void RaviCodeGenerator::emit_LOADFZ(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
  int A) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *dest = emit_gep_ra(def, base_ptr, A);
  // destvalue->n = 0.0
  emit_store_reg_n(def, llvm::ConstantFP::get(def->types->lua_NumberT, 0.0), dest);
  // destvalue->type = LUA_TNUMFLT
  emit_store_type(def, dest, LUA_TNUMFLT);
}

void RaviCodeGenerator::emit_LOADIZ(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
  int A) {
  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *dest = emit_gep_ra(def, base_ptr, A);
  // dest->i = 0
  emit_store_reg_i(def, def->types->kluaInteger[0], dest);
  // dest->type = LUA_TNUMINT
  emit_store_type(def, dest, LUA_TNUMINT);
}

void RaviCodeGenerator::emit_MOVE(RaviFunctionDef *def, llvm::Value *L_ci,
                                  llvm::Value *proto, int A, int B) {

  // case OP_MOVE: {
  //  setobjs2s(L, ra, RB(i));
  //} break;

  // Load pointer to base
  llvm::Instruction *base_ptr = emit_load_base(def);

  llvm::Value *src;
  llvm::Value *dest;

  lua_assert(A != B);

  if (A == 0) {
    // If A is 0 we can use the base pointer which is &base[0]
    dest = base_ptr;
  } else {
    // emit &base[A]
    dest = emit_array_get(def, base_ptr, A);
  }
  if (B == 0) {
    // If Bx is 0 we can use the base pointer which is &k[0]
    src = base_ptr;
  } else {
    // emit &base[B]
    src = emit_array_get(def, base_ptr, B);
  }

#if 1
  // Below is more efficient that memcpy()
  // destvalue->value->i = srcvalue->value->i;
  llvm::Value *srcvalue = emit_gep(def, "src.value", src, 0, 0, 0);
  llvm::Value *destvalue = emit_gep(def, "dest.value", dest, 0, 0, 0);
  llvm::Instruction *load = def->builder->CreateLoad(srcvalue);
  load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Instruction *store = def->builder->CreateStore(load, destvalue);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);

  // destvalue->type = srcvalue->type
  llvm::Value *srctype = emit_gep(def, "src.tt", src, 0, 1);
  llvm::Value *desttype = emit_gep(def, "dest.tt", dest, 0, 1);
  load = def->builder->CreateLoad(srctype);
  load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  store = def->builder->CreateStore(load, desttype);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
#else
  // First get the declaration for the inttrinsic memcpy
  llvm::SmallVector<llvm::Type *, 3> vec;
  vec.push_back(def->types->C_pcharT); /* i8 */
  vec.push_back(def->types->C_pcharT); /* i8 */
  vec.push_back(def->types->C_intT);
  llvm::Function *f = llvm::Intrinsic::getDeclaration(
      def->raviF->module(), llvm::Intrinsic::memcpy, vec);
  lua_assert(f);

  // Cast src and dest to i8*
  llvm::Value *dest_ptr =
      def->builder->CreateBitCast(dest, def->types->C_pcharT);
  llvm::Value *src_ptr = def->builder->CreateBitCast(src, def->types->C_pcharT);

  // Create call to intrinsic memcpy
  llvm::SmallVector<llvm::Value *, 5> values;
  values.push_back(dest_ptr);
  values.push_back(src_ptr);
  values.push_back(llvm::ConstantInt::get(def->types->C_intT, sizeof(TValue)));
  values.push_back(
      llvm::ConstantInt::get(def->types->C_intT, sizeof(L_Umaxalign)));
  values.push_back(def->types->kFalse);
  def->builder->CreateCall(f, values);
#endif
}

void RaviCodeGenerator::emit_MOVEI(RaviFunctionDef *def, llvm::Value *L_ci,
  llvm::Value *proto, int A, int B) {

  //  case OP_RAVI_MOVEI: {
  //    TValue *rb = RB(i);
  //    lua_Integer j;
  //    if (tointeger(rb, &j)) {
  //      setivalue(ra, j);
  //    }
  //    else
  //      luaG_runerror(L, "integer expected");
  //  } break;

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *var =
    TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "i");

  // Load pointer to base
  llvm::Instruction *base_ptr = emit_load_base(def);

  llvm::Value *dest = emit_gep_ra(def, base_ptr, A);
  llvm::Value *src = emit_gep_ra(def, base_ptr, B);

  llvm::Value *src_type = emit_load_type(def, src);

  // Compare src->tt == LUA_TNUMINT
  llvm::Value *cmp1 = def->builder->CreateICmpEQ(
    src_type, def->types->kInt[LUA_TNUMINT], "is.integer");

  llvm::BasicBlock *then1 = llvm::BasicBlock::Create(
    def->jitState->context(), "if.integer", def->f);
  llvm::BasicBlock *else1 =
    llvm::BasicBlock::Create(def->jitState->context(), "if.not.integer");
  llvm::BasicBlock *end1 =
    llvm::BasicBlock::Create(def->jitState->context(), "done");

  def->builder->CreateCondBr(cmp1, then1, else1);
  def->builder->SetInsertPoint(then1);

  // Already a int - move
  llvm::Instruction *tmp = emit_load_reg_i(def, src);
  llvm::Instruction *store = def->builder->CreateStore(
    tmp, var, "i");
  store->setMetadata(llvm::LLVMContext::MD_tbaa,
    def->types->tbaa_longlongT);
  def->builder->CreateBr(end1);

  // we need to convert
  def->f->getBasicBlockList().push_back(else1);
  def->builder->SetInsertPoint(else1);
  // Call luaV_tointeger_()

  llvm::Value *var_isint = def->builder->CreateCall2(
    def->luaV_tointegerF, src, var);
  llvm::Value *tobool = def->builder->CreateICmpEQ(
    var_isint, def->types->kInt[0], "int.conversion.failed");

  // Did conversion fail?
  llvm::BasicBlock *else2 = llvm::BasicBlock::Create(
    def->jitState->context(), "if.conversion.failed", def->f);
  def->builder->CreateCondBr(tobool, else2,
    end1);

  // Conversion failed, so raise error
  def->builder->SetInsertPoint(else2);
  llvm::Value *errmsg1 =
    def->builder->CreateGlobalString("integer expected");
  def->builder->CreateCall2(def->luaG_runerrorF, def->L,
    emit_gep(def, "", errmsg1, 0, 0));
  def->builder->CreateBr(end1);

  // Conversion OK
  def->f->getBasicBlockList().push_back(end1);
  def->builder->SetInsertPoint(end1);

  auto load_var = def->builder->CreateLoad(var);
  load_var->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
  emit_store_reg_i(def, load_var, dest);
  emit_store_type(def, dest, LUA_TNUMINT);
}


void RaviCodeGenerator::emit_LOADK(RaviFunctionDef *def, llvm::Value *L_ci,
                                   llvm::Value *proto, int A, int Bx) {
  //    case OP_LOADK: {
  //      TValue *rb = k + GETARG_Bx(i);
  //      setobj2s(L, ra, rb);
  //    } break;

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_luaState_ci_baseT);

  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr;

  // LOADK requires a structure assignment
  // in LLVM as far as I can tell this requires a call to
  // an intrinsic memcpy
  llvm::Value *src;
  llvm::Value *dest;

  if (A == 0) {
    // If A is 0 we can use the base pointer which is &base[0]
    dest = base_ptr;
  } else {
    // emit &base[A]
    dest = emit_array_get(def, base_ptr, A);
  }
  if (Bx == 0) {
    // If Bx is 0 we can use the base pointer which is &k[0]
    src = k_ptr;
  } else {
    // emit &k[Bx]
    src = emit_array_get(def, k_ptr, Bx);
  }

#if 1
  // Below is more efficient that memcpy()
  // destvalue->value->i = srcvalue->value->i;
  // destvalue->value->i = srcvalue->value->i;
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

#else
  // First get the declaration for the inttrinsic memcpy
  llvm::SmallVector<llvm::Type *, 3> vec;
  vec.push_back(def->types->C_pcharT); /* i8 */
  vec.push_back(def->types->C_pcharT); /* i8 */
  vec.push_back(def->types->C_intT);
  llvm::Function *f = llvm::Intrinsic::getDeclaration(
      def->raviF->module(), llvm::Intrinsic::memcpy, vec);
  lua_assert(f);

  // Cast src and dest to i8*
  llvm::Value *dest_ptr =
      def->builder->CreateBitCast(dest, def->types->C_pcharT);
  llvm::Value *src_ptr = def->builder->CreateBitCast(src, def->types->C_pcharT);

  // Create call to intrinsic memcpy
  llvm::SmallVector<llvm::Value *, 5> values;
  values.push_back(dest_ptr);
  values.push_back(src_ptr);
  values.push_back(llvm::ConstantInt::get(def->types->C_intT, sizeof(TValue)));
  values.push_back(
      llvm::ConstantInt::get(def->types->C_intT, sizeof(L_Umaxalign)));
  values.push_back(def->types->kFalse);
  def->builder->CreateCall(f, values);
#endif
}
}
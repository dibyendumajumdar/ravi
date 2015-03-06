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
  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
    def->types->tbaa_luaState_ci_baseT);

  llvm::Value *dest;

  if (A == 0) {
    // If A is 0 we can use the base pointer which is &base[0]
    dest = base_ptr;
  }
  else {
    // emit &base[A]
    dest = emit_array_get(def, base_ptr, A);
  }

  // destvalue->n = 0.0
  llvm::Value *destvalue = emit_gep(def, "dest.value", dest, 0, 0, 0);
  llvm::Instruction *store = def->builder->CreateStore(llvm::ConstantFP::get(def->types->lua_NumberT, 0.0), destvalue);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);

  // destvalue->type = LUA_TNUMFLT
  llvm::Value *desttype = emit_gep(def, "dest.tt", dest, 0, 1);
  store = def->builder->CreateStore(def->types->kInt[LUA_TNUMFLT], desttype);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
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
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_luaState_ci_baseT);

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
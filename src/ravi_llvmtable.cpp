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

// R(A+1) := R(B); R(A) := R(B)[RK(C)]
void RaviCodeGenerator::emit_SELF(RaviFunctionDef *def, int A, int B, int C,
                                  int pc) {
  // StkId rb = RB(i);
  // setobjs2s(L, ra + 1, rb);
  // Protect(luaV_gettable(L, rb, RKC(i), ra));
  bool traced = emit_debug_trace(def, OP_SELF, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *ra1 = emit_gep_register(def, A + 1);
  emit_assign(def, ra1, rb);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->luaV_gettableF, def->L, rb, rc, ra);
}

// R(A+1) := R(B); R(A) := R(B)[RK(C)]
void RaviCodeGenerator::emit_SELF_SK(RaviFunctionDef *def, int A, int B, int C,
                                     int pc) {
  // StkId rb = RB(i);
  // setobjs2s(L, ra + 1, rb);
  // Protect(raviV_gettable_sskey(L, rb, RKC(i), ra));
  bool traced = emit_debug_trace(def, OP_RAVI_SELF_SK, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *ra1 = emit_gep_register(def, A + 1);
  emit_assign(def, ra1, rb);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->raviV_gettable_sskeyF, def->L, rb, rc, ra);
}

// R(A+1) := R(B); R(A) := R(B)[RK(C)]
void RaviCodeGenerator::emit_SELF_S(RaviFunctionDef *def, int A, int B, int C,
                                    int pc, TString *key) {
  // StkId rb = RB(i);
  // setobjs2s(L, ra + 1, rb);
  // TValue *kv = k + INDEXK(GETARG_C(i));
  // TString *key = tsvalue(kv);
  // const TValue *v = luaH_getstr(hvalue(rb), key);
  // setobj2s(L, ra, v);
  emit_debug_trace(def, OP_RAVI_SELF_S, pc);
  emit_load_base(def);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *ra1 = emit_gep_register(def, A + 1);
  emit_assign(def, ra1, rb);
  emit_common_GETTABLE_S(def, A, B, C, key);
}

// R(A) := length of R(B)
void RaviCodeGenerator::emit_LEN(RaviFunctionDef *def, int A, int B, int pc) {
  // Protect(luaV_objlen(L, ra, RB(i)));
  bool traced = emit_debug_trace(def, OP_LEN, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register(def, B);
  CreateCall3(def->builder, def->luaV_objlenF, def->L, ra, rb);
}

// R(A)[RK(B)] := RK(C)
// This is a more optimized version that calls
// luaH_setint() instead of luaV_settable().
// This relies on two things:
// a) we know we have a table
// b) we know the key is an integer
void RaviCodeGenerator::emit_SETTABLE_I(RaviFunctionDef *def, int A, int B,
                                        int C, int pc) {
  bool traced = emit_debug_trace(def, OP_RAVI_SETTABLE_I, pc);
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->raviV_settable_iF, def->L, ra, rb, rc);
}

// R(A)[RK(B)] := RK(C)
void RaviCodeGenerator::emit_SETTABLE(RaviFunctionDef *def, int A, int B, int C,
                                      int pc) {
  // Protect(luaV_settable(L, ra, RKB(i), RKC(i)));
  bool traced = emit_debug_trace(def, OP_SETTABLE, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->luaV_settableF, def->L, ra, rb, rc);
}

// R(A)[RK(B)] := RK(C)
void RaviCodeGenerator::emit_SETTABLE_SK(RaviFunctionDef *def, int A, int B,
                                         int C, int pc) {
  // Protect(raviV_settable_sskey(L, ra, RKB(i), RKC(i)));
  bool traced = emit_debug_trace(def, OP_RAVI_SETTABLE_SK, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->raviV_settable_sskeyF, def->L, ra, rb, rc);
}

// R(A) := R(B)[RK(C)]
void RaviCodeGenerator::emit_GETTABLE(RaviFunctionDef *def, int A, int B, int C,
                                      int pc) {
  // Protect(luaV_gettable(L, RB(i), RKC(i), ra));
  bool traced = emit_debug_trace(def, OP_GETTABLE, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->luaV_gettableF, def->L, rb, rc, ra);
}

// R(A) := R(B)[RK(C)]
void RaviCodeGenerator::emit_GETTABLE_SK(RaviFunctionDef *def, int A, int B,
                                         int C, int pc, TString *key) {
  bool traced = emit_debug_trace(def, OP_RAVI_GETTABLE_SK, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *rb = emit_gep_register(def, B);
#if 0
  // Protect(raviV_gettable_sskey(L, RB(i), RKC(i), ra));
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->raviV_gettable_sskeyF, def->L, rb, rc, ra);
#else
  llvm::Instruction *type = emit_load_type(def, rb);

  // if table type try fast path
  llvm::Value *cmp1 = emit_is_value_of_type(def, type, RAVI__TLTABLE,
                                            "GETTABLE_SK_is_table_type");
  llvm::BasicBlock *is_table = llvm::BasicBlock::Create(
      def->jitState->context(), "GETTABLE_SK_is_table", def->f);
  llvm::BasicBlock *not_table = llvm::BasicBlock::Create(
      def->jitState->context(), "GETTABLE_SK_is_not_table");
  llvm::BasicBlock *done =
      llvm::BasicBlock::Create(def->jitState->context(), "GETTABLE_SK_done");
  auto brinst1 = def->builder->CreateCondBr(cmp1, is_table, not_table);
  attach_branch_weights(def, brinst1, 100, 0);
  def->builder->SetInsertPoint(is_table);

  emit_common_GETTABLE_S_(def, A, rb, C, key);

  def->builder->CreateBr(done);

  def->f->getBasicBlockList().push_back(not_table);
  def->builder->SetInsertPoint(not_table);

  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->raviV_gettable_sskeyF, def->L, rb, rc, ra);

  def->builder->CreateBr(done);

  def->f->getBasicBlockList().push_back(done);
  def->builder->SetInsertPoint(done);
#endif
}

// R(A) := R(B)[RK(C)]
// This is a more optimized version that attempts to do an inline
// array get first and only if that fails it falls back on calling
// luaH_getint(). This relies on two things:
// a) we know we have a table
// b) we know the key is an integer
void RaviCodeGenerator::emit_GETTABLE_I(RaviFunctionDef *def, int A, int B,
                                        int C, int pc) {
  // changed to that target may not be a table
  bool traced = emit_debug_trace(def, OP_RAVI_GETTABLE_I, pc);
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  CreateCall4(def->builder, def->raviV_gettable_iF, def->L, rb, rc, ra);

  // FIXME following is no longer valid as RB may not be a table

  // TValue *rb = RB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rc);
  // Table *t = hvalue(rb);
  // const TValue *v;
  // if (l_castS2U(idx - 1) < t->sizearray)
  //  v = &t->array[idx - 1];
  // else
  //  v = luaH_getint(t, idx);
  // if (!ttisnil(v) || metamethod_absent(t->metatable, TM_INDEX)) {
  //  setobj2s(L, ra, v);
  //}
  // else {
  //  Protect(luaV_finishget(L, rb, rc, ra, v));
  //}

  //emit_debug_trace(def, OP_RAVI_GETTABLE_I, pc);
  //emit_load_base(def);
  //llvm::Value *ra = emit_gep_register(def, A);
  //llvm::Value *rb = emit_gep_register(def, B);
  //llvm::Value *rc = emit_gep_register_or_constant(def, C);
  //llvm::Instruction *key = emit_load_reg_i(def, rc);
  //llvm::Instruction *t = emit_load_reg_h(def, rb);
  //llvm::Value *data = emit_table_get_array(def, t);
  //llvm::Value *len = emit_table_get_arraysize(def, t);
  //// As Lua arrays are 1 based we need to subtract by 1
  //llvm::Value *key_minus_1 =
  //    def->builder->CreateSub(key, def->types->kluaInteger[1]);
  //// As len is unsigned int we need to truncate
  //llvm::Value *ukey =
  //    def->builder->CreateTrunc(key_minus_1, def->types->C_intT);
  //// Do an unsigned comparison with length
  //llvm::Value *cmp = def->builder->CreateICmpULT(ukey, len);
  //llvm::BasicBlock *then_block =
  //    llvm::BasicBlock::Create(def->jitState->context(), "if.in.range", def->f);
  //llvm::BasicBlock *else_block =
  //    llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
  //llvm::BasicBlock *end_block =
  //    llvm::BasicBlock::Create(def->jitState->context(), "if.end");
  //def->builder->CreateCondBr(cmp, then_block, else_block);
  //def->builder->SetInsertPoint(then_block);

  //// Key is in range so array access possible
  //llvm::Value *value1 = def->builder->CreateGEP(data, ukey);
  //def->builder->CreateBr(end_block);

  //// Out of range so fall back to luaH_getint()
  //def->f->getBasicBlockList().push_back(else_block);
  //def->builder->SetInsertPoint(else_block);
  //llvm::Value *value2 = CreateCall2(def->builder, def->luaH_getintF, t, key);
  //def->builder->CreateBr(end_block);

  //// Merge results from the two branches above
  //def->f->getBasicBlockList().push_back(end_block);
  //def->builder->SetInsertPoint(end_block);
  //llvm::PHINode *phi = def->builder->CreatePHI(def->types->pTValueT, 2);
  //phi->addIncoming(value1, then_block);
  //phi->addIncoming(value2, else_block);
  //emit_finish_GETTABLE(def, phi, t, ra, rb, rc);
}

void RaviCodeGenerator::emit_finish_GETTABLE(RaviFunctionDef *def,
                                             llvm::Value *phi, llvm::Value *t,
                                             llvm::Value *ra, llvm::Value *rb,
                                             llvm::Value *rc) {
  // We need to test if value is not nil
  // or table has no metatable
  // or if the metatable cached flags indicate metamethod absent
  llvm::Value *value_type = emit_load_type(def, phi);
  llvm::Value *isnotnil = emit_is_not_value_of_type(def, value_type, LUA__TNIL);
  // llvm::Value *metamethod_absent = emit_table_no_metamethod(def, t,
  // TM_INDEX);
  // llvm::Value *cond = def->builder->CreateOr(isnotnil, metamethod_absent);
  llvm::Value *cond = isnotnil;

  llvm::BasicBlock *if_true_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.nil", def->f);
  llvm::BasicBlock *if_false_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.try.metamethod");
  llvm::BasicBlock *if_end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");
  auto brinst1 =
      def->builder->CreateCondBr(cond, if_true_block, if_false_block);
  attach_branch_weights(def, brinst1, 100, 0);
  def->builder->SetInsertPoint(if_true_block);

  // Fast path
  emit_assign(def, ra, phi);
  def->builder->CreateBr(if_end_block);

  def->f->getBasicBlockList().push_back(if_false_block);
  def->builder->SetInsertPoint(if_false_block);

  // If value is nil Lua requires that an index event be
  // generated - so we fall back on slow path for that
  CreateCall5(def->builder, def->luaV_finishgetF, def->L, rb, rc, ra, phi);
  def->builder->CreateBr(if_end_block);

  // Merge results from the two branches above
  def->f->getBasicBlockList().push_back(if_end_block);
  def->builder->SetInsertPoint(if_end_block);
}

// R(A) := R(B)[RK(C)]
// Emit inline code for accessing a table element using a string key
// We try to access the element using the hash part but if the
// key is not in the main position then we fall back on luaH_getstr().
// IMPORTANT - this emitter should only be called when key is known to
// to be short string
// NOTE: To add support for GETTABUP_SK we now let caller supply the
// rb register as it may be a register or an upvalue reference
// See emit_GETTABUP_SK
void RaviCodeGenerator::emit_common_GETTABLE_S_(RaviFunctionDef *def, int A,
                                                llvm::Value *rb, int C,
                                                TString *key) {
  // The code we want to generate is this:
  //   struct Node *n = hashstr(t, key);
  //   const struct TValue *k = gkey(n);
  //   TValue *v;
  //   if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
  //     v = gval(n);
  //   else
  //     v = luaH_getstr(t, key);
  //   if (!ttisnil(v) || metamethod_absent(t->metatable, TM_INDEX)) {
  //     setobj2s(L, ra, v);
  //   }
  //   else {
  //     Protect(luaV_finishget(L, rb, rc, ra, v));
  //   }

  // A number of macros are involved above do the
  // the generated code is somewhat more complex

  llvm::Value *ra = emit_gep_register(def, A);

  // Get the hash table
  llvm::Instruction *t = emit_load_reg_h(def, rb);

  // Obtain the offset where the key should be
  llvm::Value *offset = emit_table_get_hashstr(def, t, key);

  // Get access to the node array
  llvm::Value *node = emit_table_get_nodearray(def, t);

  // Now we need to get to the right element in the node array
  // and retrieve the type information which is held there
  llvm::Value *ktype = emit_table_get_keytype(def, node, offset);

  // We need to check that the key type is also short string
  llvm::Value *is_shortstring =
      emit_is_value_of_type(def, ktype, LUA__TSHRSTR, "is_shortstring");
  llvm::BasicBlock *testkey =
      llvm::BasicBlock::Create(def->jitState->context(), "testkey");
  llvm::BasicBlock *testok =
      llvm::BasicBlock::Create(def->jitState->context(), "testok");
  llvm::BasicBlock *testfail =
      llvm::BasicBlock::Create(def->jitState->context(), "testfail");
  llvm::BasicBlock *testend =
      llvm::BasicBlock::Create(def->jitState->context(), "testend");
  auto brinst1 = def->builder->CreateCondBr(is_shortstring, testkey, testfail);
  attach_branch_weights(def, brinst1, 100, 0);

  // Now we need to compare the keys
  def->f->getBasicBlockList().push_back(testkey);
  def->builder->SetInsertPoint(testkey);

  // Get the key from the node
  llvm::Value *keyvalue = emit_table_get_strkey(def, node, offset);

  // Cast the pointer to a intptr so we can compare
  llvm::Value *intptr =
      def->builder->CreatePtrToInt(keyvalue, def->types->C_intptr_t);
  llvm::Value *ourptr =
      llvm::ConstantInt::get(def->types->C_intptr_t, (intptr_t)key);
  // Compare the two pointers
  // If they match then we found the element
  llvm::Value *same = def->builder->CreateICmpEQ(intptr, ourptr);
  auto brinst2 = def->builder->CreateCondBr(same, testok, testfail);
  attach_branch_weights(def, brinst2, 5, 2);

  // If key found return the value
  def->f->getBasicBlockList().push_back(testok);
  def->builder->SetInsertPoint(testok);
  // Get the value
  llvm::Value *value1 = emit_table_get_value(def, node, offset);
  llvm::Value *rc1 = emit_gep_register_or_constant(def, C);
  def->builder->CreateBr(testend);

  // Not found so call luaH_getstr
  def->f->getBasicBlockList().push_back(testfail);
  def->builder->SetInsertPoint(testfail);
  llvm::Value *rc2 = emit_gep_register_or_constant(def, C);
  llvm::Value *value2 = CreateCall2(def->builder, def->luaH_getstrF, t,
                                    emit_load_reg_s(def, rc2));
  def->builder->CreateBr(testend);

  // merge
  def->f->getBasicBlockList().push_back(testend);
  def->builder->SetInsertPoint(testend);
  llvm::PHINode *phi = def->builder->CreatePHI(def->types->pTValueT, 2);
  phi->addIncoming(value1, testok);
  phi->addIncoming(value2, testfail);
  llvm::PHINode *phi2 = def->builder->CreatePHI(def->types->pTValueT, 2);
  phi2->addIncoming(rc1, testok);
  phi2->addIncoming(rc2, testfail);
  emit_finish_GETTABLE(def, phi, t, ra, rb, phi2);
}

// R(A) := R(B)[RK(C)]
// Emit inline code for accessing a table element using a string key
// We try to access the element using the hash part but if the
// key is not in the main position then we fall back on luaH_getstr().
// IMPORTANT - this emitter should only be called when key is known to
// to be short string
void RaviCodeGenerator::emit_common_GETTABLE_S(RaviFunctionDef *def, int A,
                                               int B, int C, TString *key) {
  // The code we want to generate is this:
  //   struct Node *n = hashstr(t, key);
  //   const struct TValue *k = gkey(n);
  //   TValue *v;
  //   if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
  //     v = gval(n);
  //   else
  //     v = luaH_getstr(t, key);
  //   if (!ttisnil(v) || metamethod_absent(t->metatable, TM_INDEX)) {
  //     setobj2s(L, ra, v);
  //   }
  //   else {
  //     Protect(luaV_finishget(L, rb, rc, ra, v));
  //   }

  // A number of macros are involved above do the
  // the generated code is somewhat more complex

  llvm::Value *rb = emit_gep_register(def, B);
  emit_common_GETTABLE_S_(def, A, rb, C, key);
}

// R(A) := R(B)[RK(C)]
// Emit inline code for accessing a table element using a string key
// We try to access the element using the hash part but if the
// key is not in the main position then we fall back on luaH_getstr().
// IMPORTANT - this emitter should only be called when key is known to
// to be short string
void RaviCodeGenerator::emit_GETTABLE_S(RaviFunctionDef *def, int A, int B,
                                        int C, int pc, TString *key) {
  // The code we want to generate is this:
  //   struct Node *n = hashstr(t, key);
  //   const struct TValue *k = gkey(n);
  //   if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
  //     return gval(n);
  //   return luaH_getstr(t, key);

  emit_debug_trace(def, OP_RAVI_GETTABLE_S, pc);
  emit_load_base(def);
  emit_common_GETTABLE_S(def, A, B, C, key);
}

void RaviCodeGenerator::emit_GETTABLE_AF(RaviFunctionDef *def, int A, int B,
                                         int C, bool omitArrayGetRangeCheck,
                                         int pc) {
  //#define raviH_get_float_inline(L, t, key, v)
  //{ unsigned ukey = (unsigned)((key));
  //  lua_Number *data = (lua_Number *)t->ravi_array.data;
  //  if (ukey < t->ravi_array.len) {
  //    setfltvalue(v, data[ukey]);
  //      }else
  //    luaG_runerror(L, "array out of bounds");
  //}

  // TValue *rb = RB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rc);
  // Table *t = hvalue(rb);
  // raviH_get_float_inline(L, t, idx, ra);

  emit_debug_trace(def, OP_RAVI_GETTABLE_AF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *key = emit_load_reg_i(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, rb);
  llvm::Instruction *data = emit_load_reg_h_floatarray(def, t);
  llvm::BasicBlock *then_block = nullptr;
  llvm::BasicBlock *else_block = nullptr;
  llvm::BasicBlock *end_block = nullptr;
  if (!omitArrayGetRangeCheck) {
    llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
    llvm::Value *ulen =
        def->builder->CreateZExt(len, def->types->lua_UnsignedT, "ulen");

    llvm::Value *cmp = def->builder->CreateICmpULT(key, ulen);
    then_block = llvm::BasicBlock::Create(def->jitState->context(),
                                          "if.in.range", def->f);
    else_block =
        llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
    end_block = llvm::BasicBlock::Create(def->jitState->context(), "if.end");
    auto brinst1 = def->builder->CreateCondBr(cmp, then_block, else_block);
    attach_branch_weights(def, brinst1, 100, 0);
    def->builder->SetInsertPoint(then_block);
  }

  llvm::Value *ptr = def->builder->CreateGEP(data, key);
  llvm::Instruction *value = def->builder->CreateLoad(ptr);
  value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_pdoubleT);
  emit_store_reg_n_withtype(def, value, ra);

  if (!omitArrayGetRangeCheck) {
    def->builder->CreateBr(end_block);

    def->f->getBasicBlockList().push_back(else_block);
    def->builder->SetInsertPoint(else_block);

    emit_raise_lua_error(def, "array out of bounds");
    def->builder->CreateBr(end_block);

    def->f->getBasicBlockList().push_back(end_block);
    def->builder->SetInsertPoint(end_block);
  }
}

void RaviCodeGenerator::emit_GETTABLE_AI(RaviFunctionDef *def, int A, int B,
                                         int C, bool omitArrayGetRangeCheck,
                                         int pc) {
  //#define raviH_get_int_inline(L, t, key, v)
  //{ unsigned ukey = (unsigned)((key));
  //  lua_Integer *data = (lua_Integer *)t->ravi_array.data;
  //  if (ukey < t->ravi_array.len) {
  //    setivalue(v, data[ukey]);
  //      } else
  //    luaG_runerror(L, "array out of bounds");
  //}

  // TValue *rb = RB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rc);
  // Table *t = hvalue(rb);
  // raviH_get_int_inline(L, t, idx, ra);

  emit_debug_trace(def, OP_RAVI_GETTABLE_AI, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *key = emit_load_reg_i(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, rb);
  llvm::Instruction *data = emit_load_reg_h_intarray(def, t);

  llvm::BasicBlock *then_block = nullptr;
  llvm::BasicBlock *else_block = nullptr;
  llvm::BasicBlock *end_block = nullptr;
  if (!omitArrayGetRangeCheck) {
    llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
    llvm::Value *ulen =
        def->builder->CreateZExt(len, def->types->lua_UnsignedT, "ulen");

    llvm::Value *cmp = def->builder->CreateICmpULT(key, ulen);
    then_block = llvm::BasicBlock::Create(def->jitState->context(),
                                          "if.in.range", def->f);
    else_block =
        llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
    end_block = llvm::BasicBlock::Create(def->jitState->context(), "if.end");
    auto brinst1 = def->builder->CreateCondBr(cmp, then_block, else_block);
    attach_branch_weights(def, brinst1, 100, 0);
    def->builder->SetInsertPoint(then_block);
  }

  llvm::Value *ptr = def->builder->CreateGEP(data, key);
  llvm::Instruction *value = def->builder->CreateLoad(ptr);
  value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_plonglongT);
  emit_store_reg_i_withtype(def, value, ra);

  if (!omitArrayGetRangeCheck) {
    def->builder->CreateBr(end_block);

    def->f->getBasicBlockList().push_back(else_block);
    def->builder->SetInsertPoint(else_block);

    emit_raise_lua_error(def, "array out of bounds");
    def->builder->CreateBr(end_block);

    def->f->getBasicBlockList().push_back(end_block);
    def->builder->SetInsertPoint(end_block);
  }
}

void RaviCodeGenerator::emit_SETTABLE_AI(RaviFunctionDef *def, int A, int B,
                                         int C, bool known_int, int pc) {
  //#define raviH_set_int_inline(L, t, key, value)
  //{ unsigned ukey = (unsigned)((key));
  //  lua_Integer *data = (lua_Integer *)t->ravi_array.data;
  //  if (ukey < t->ravi_array.len) {
  //    data[ukey] = value;
  //      } else
  //    raviH_set_int(L, t, ukey, value);
  //}

  // Table *t = hvalue(ra);
  // TValue *rb = RKB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rb);
  // lua_Integer value = ivalue(rc);
  // raviH_set_int_inline(L, t, idx, value);

  emit_debug_trace(def, known_int ? OP_RAVI_SETTABLE_AII : OP_RAVI_SETTABLE_AI,
                   pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *key = emit_load_reg_i(def, rb);
  llvm::Instruction *value =
      known_int ? emit_load_reg_i(def, rc) : emit_tointeger(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, ra);
  llvm::Instruction *data = emit_load_reg_h_intarray(def, t);
  llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
  llvm::Value *ulen =
      def->builder->CreateZExt(len, def->types->lua_UnsignedT, "ulen");

  llvm::Value *cmp = def->builder->CreateICmpULT(key, ulen);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.in.range", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");
  def->builder->CreateCondBr(cmp, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  llvm::Value *ptr = def->builder->CreateGEP(data, key);

  llvm::Instruction *storeinst = def->builder->CreateStore(value, ptr);
  storeinst->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_plonglongT);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  CreateCall4(def->builder, def->raviH_set_intF, def->L, t, key, value);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}

void RaviCodeGenerator::emit_SETTABLE_AF(RaviFunctionDef *def, int A, int B,
                                         int C, bool known_float, int pc) {
  //#define raviH_set_float_inline(L, t, key, value)
  //{ unsigned ukey = (unsigned)((key));
  //  lua_Number *data = (lua_Number *)t->ravi_array.data;
  //  if (ukey < t->ravi_array.len) {
  //    data[ukey] = value;
  //      } else
  //    raviH_set_float(L, t, ukey, value);
  //  }

  // Table *t = hvalue(ra);
  // TValue *rb = RKB(i);
  // TValue *rc = RKC(i);
  // lua_Integer idx = ivalue(rb);
  // if (ttisfloat(rc)) {
  //  raviH_set_float_inline(L, t, idx, fltvalue(rc));
  //}
  // else {
  //  raviH_set_float_inline(L, t, idx, ((lua_Number)ivalue(rc)));
  //}

  emit_debug_trace(
      def, known_float ? OP_RAVI_SETTABLE_AFF : OP_RAVI_SETTABLE_AF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);
  llvm::Instruction *key = emit_load_reg_i(def, rb);
  llvm::Instruction *value =
      known_float ? emit_load_reg_n(def, rc) : emit_tofloat(def, rc);
  llvm::Instruction *t = emit_load_reg_h(def, ra);
  llvm::Instruction *data = emit_load_reg_h_floatarray(def, t);
  llvm::Instruction *len = emit_load_ravi_arraylength(def, t);
  llvm::Value *ulen =
      def->builder->CreateZExt(len, def->types->lua_UnsignedT, "ulen");

  llvm::Value *cmp = def->builder->CreateICmpULT(key, ulen);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.in.range", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.in.range");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");
  def->builder->CreateCondBr(cmp, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  llvm::Value *ptr = def->builder->CreateGEP(data, key);

  llvm::Instruction *storeinst = def->builder->CreateStore(value, ptr);
  storeinst->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_pdoubleT);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  CreateCall4(def->builder, def->raviH_set_floatF, def->L, t, key, value);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}

// R(A) := UpValue[B]
void RaviCodeGenerator::emit_GETUPVAL(RaviFunctionDef *def, int A, int B,
                                      int pc) {
  // int b = GETARG_B(i);
  // setobj2s(L, ra, cl->upvals[b]->v);
  emit_debug_trace(def, OP_GETUPVAL, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *upval_ptr = emit_gep_upvals(def, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  emit_assign(def, ra, v);
}

// UpValue[B] := R(A)
void RaviCodeGenerator::emit_SETUPVAL_Specific(RaviFunctionDef *def, int A,
                                               int B, int pc, OpCode op,
                                               llvm::Function *f) {
  emit_debug_trace(def, op, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  CreateCall4(def->builder, f, def->L, def->p_LClosure, ra,
              llvm::ConstantInt::get(def->types->C_intT, B));
}

// UpValue[B] := R(A)
void RaviCodeGenerator::emit_SETUPVAL(RaviFunctionDef *def, int A, int B,
                                      int pc) {
  // UpVal *uv = cl->upvals[GETARG_B(i)];
  // setobj(L, uv->v, ra);
  // luaC_upvalbarrier(L, uv);

  emit_debug_trace(def, OP_SETUPVAL, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *upval_ptr = emit_gep_upvals(def, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  emit_assign(def, v, ra);

  llvm::Value *type = emit_load_type(def, v);
  llvm::Value *is_collectible =
      def->builder->CreateAnd(type, def->types->kInt[BIT_ISCOLLECTABLE]);

  llvm::Value *value = emit_gep_upval_value(def, upval);
  llvm::Value *cmp = def->builder->CreateICmpNE(v, value, "v.ne.value");
  llvm::Value *tobool = def->builder->CreateICmpEQ(
      is_collectible, def->types->kInt[0], "not.collectible");
  llvm::Value *orcond =
      def->builder->CreateOr(cmp, tobool, "v.ne.value.or.not.collectible");

  llvm::BasicBlock *then =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *end =
      llvm::BasicBlock::Create(def->jitState->context(), "if.end");

  def->builder->CreateCondBr(orcond, end, then);
  def->builder->SetInsertPoint(then);

  CreateCall2(def->builder, def->luaC_upvalbarrierF, def->L, upval);
  def->builder->CreateBr(end);

  def->f->getBasicBlockList().push_back(end);
  def->builder->SetInsertPoint(end);
}

// R(A) := UpValue[B][RK(C)]
void RaviCodeGenerator::emit_GETTABUP(RaviFunctionDef *def, int A, int B, int C,
                                      int pc) {
  // int b = GETARG_B(i);
  // Protect(luaV_gettable(L, cl->upvals[b]->v, RKC(i), ra));
  bool traced = emit_debug_trace(def, OP_GETTABUP, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  llvm::Value *upval_ptr = emit_gep_upvals(def, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  CreateCall4(def->builder, def->luaV_gettableF, def->L, v, rc, ra);
}

// R(A) := UpValue[B][RK(C)]
void RaviCodeGenerator::emit_GETTABUP_SK(RaviFunctionDef *def, int A, int B,
                                         int C, int pc) {
  // int b = GETARG_B(i);
  // Protect(luaV_gettable(L, cl->upvals[b]->v, RKC(i), ra));
  bool traced = emit_debug_trace(def, OP_RAVI_GETTABUP_SK, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  llvm::Value *upval_ptr = emit_gep_upvals(def, B);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  CreateCall4(def->builder, def->raviV_gettable_sskeyF, def->L, v, rc, ra);
}

// UpValue[A][RK(B)] := RK(C)
void RaviCodeGenerator::emit_SETTABUP(RaviFunctionDef *def, int A, int B, int C,
                                      int pc) {
  // int a = GETARG_A(i);
  // Protect(luaV_settable(L, cl->upvals[a]->v, RKB(i), RKC(i)));

  bool traced = emit_debug_trace(def, OP_SETTABUP, pc);
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  llvm::Value *upval_ptr = emit_gep_upvals(def, A);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  CreateCall4(def->builder, def->luaV_settableF, def->L, v, rb, rc);
}

// UpValue[A][RK(B)] := RK(C)
void RaviCodeGenerator::emit_SETTABUP_SK(RaviFunctionDef *def, int A, int B,
                                         int C, int pc) {
  // int a = GETARG_A(i);
  // Protect(luaV_settable(L, cl->upvals[a]->v, RKB(i), RKC(i)));

  bool traced = emit_debug_trace(def, OP_RAVI_SETTABLE_SK, pc);
  if (!traced) emit_update_savedpc(def, pc);
  emit_load_base(def);
  llvm::Value *rb = emit_gep_register_or_constant(def, B);
  llvm::Value *rc = emit_gep_register_or_constant(def, C);

  llvm::Value *upval_ptr = emit_gep_upvals(def, A);
  llvm::Instruction *upval = emit_load_pupval(def, upval_ptr);
  llvm::Value *v = emit_load_upval_v(def, upval);
  CreateCall4(def->builder, def->raviV_settable_sskeyF, def->L, v, rb, rc);
}

void RaviCodeGenerator::emit_NEWARRAYINT(RaviFunctionDef *def, int A, int pc) {
  emit_debug_trace(def, OP_RAVI_NEWARRAYI, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  CreateCall3(def->builder, def->raviV_op_newarrayintF, def->L, def->ci_val,
              ra);
}

void RaviCodeGenerator::emit_NEWARRAYFLOAT(RaviFunctionDef *def, int A,
                                           int pc) {
  emit_debug_trace(def, OP_RAVI_NEWARRAYF, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  CreateCall3(def->builder, def->raviV_op_newarrayfloatF, def->L, def->ci_val,
              ra);
}

void RaviCodeGenerator::emit_NEWTABLE(RaviFunctionDef *def, int A, int B, int C,
                                      int pc) {
  //  case OP_NEWTABLE: {
  //    int b = GETARG_B(i);
  //    int c = GETARG_C(i);
  //    Table *t = luaH_new(L);
  //    sethvalue(L, ra, t);
  //    if (b != 0 || c != 0)
  //      luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
  //    checkGC(L, ra + 1);
  //  } break;

  emit_debug_trace(def, OP_NEWTABLE, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  CreateCall5(def->builder, def->raviV_op_newtableF, def->L, def->ci_val, ra,
              def->types->kInt[B], def->types->kInt[C]);
}

void RaviCodeGenerator::emit_SETLIST(RaviFunctionDef *def, int A, int B, int C,
                                     int pc) {
  emit_debug_trace(def, OP_SETLIST, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  CreateCall5(def->builder, def->raviV_op_setlistF, def->L, def->ci_val, ra,
              def->types->kInt[B], def->types->kInt[C]);
}

void RaviCodeGenerator::emit_TOARRAY(RaviFunctionDef *def, int A,
                                     int array_type_expected,
                                     const char *errmsg, int pc) {
  OpCode op = OP_RAVI_TOTAB;
  LuaTypeCode expectedType = RAVI__TLTABLE;
  switch (array_type_expected) {
    case RAVI_TARRAYINT:
      op = OP_RAVI_TOARRAYI;
      expectedType = RAVI__TIARRAY;
      break;
    case RAVI_TARRAYFLT:
      op = OP_RAVI_TOARRAYF;
      expectedType = RAVI__TFARRAY;
      break;
    case RAVI_TTABLE:
    default: break;
  }

  emit_debug_trace(def, op, pc);
  emit_load_base(def);
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Instruction *type = emit_load_type(def, ra);

  // type != expectedType
  llvm::Value *cmp1 = emit_is_not_value_of_type(def, type, expectedType,
                                                "is.not.expected.type");
  llvm::BasicBlock *raise_error = llvm::BasicBlock::Create(
      def->jitState->context(), "if.not.expected_type", def->f);
  llvm::BasicBlock *done =
      llvm::BasicBlock::Create(def->jitState->context(), "done");
  auto brinst1 = def->builder->CreateCondBr(cmp1, raise_error, done);
  attach_branch_weights(def, brinst1, 0, 100);
  def->builder->SetInsertPoint(raise_error);

  // Conversion failed, so raise error
  emit_raise_lua_error(def, errmsg);
  def->builder->CreateBr(done);

  def->f->getBasicBlockList().push_back(done);
  def->builder->SetInsertPoint(done);
}

void RaviCodeGenerator::emit_MOVEAI(RaviFunctionDef *def, int A, int B,
                                    int pc) {
  emit_debug_trace(def, OP_RAVI_MOVEAI, pc);
  emit_TOARRAY(def, B, RAVI_TARRAYINT, "integer[] expected", pc);
  llvm::Value *src = emit_gep_register(def, B);
  llvm::Value *dest = emit_gep_register(def, A);
  emit_assign(def, dest, src);
}

void RaviCodeGenerator::emit_MOVEAF(RaviFunctionDef *def, int A, int B,
                                    int pc) {
  emit_debug_trace(def, OP_RAVI_MOVEAF, pc);
  emit_TOARRAY(def, B, RAVI_TARRAYFLT, "number[] expected", pc);
  llvm::Value *src = emit_gep_register(def, B);
  llvm::Value *dest = emit_gep_register(def, A);
  emit_assign(def, dest, src);
}

void RaviCodeGenerator::emit_MOVETAB(RaviFunctionDef *def, int A, int B,
                                     int pc) {
  emit_debug_trace(def, OP_RAVI_MOVETAB, pc);
  emit_TOARRAY(def, B, RAVI_TTABLE, "table expected", pc);
  llvm::Value *src = emit_gep_register(def, B);
  llvm::Value *dest = emit_gep_register(def, A);
  emit_assign(def, dest, src);
}
}

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
void RaviCodeGenerator::emit_EQ(RaviFunctionDef *def, int A, int B, int C, int j,
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
  emit_load_base(def);

  // Get pointer to register B
  llvm::Value *lhs_ptr = emit_gep_register_or_constant(def, B);
  // Get pointer to register C
  llvm::Value *rhs_ptr = emit_gep_register_or_constant(def, C);

  // Call luaV_equalobj with register B and C
  llvm::Value *result =
      CreateCall3(def->builder, callee, def->L, lhs_ptr, rhs_ptr);
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
    emit_load_base(def);

    // base + a - 1
    llvm::Value *val = emit_gep_register(def, jA-1);

    // Call luaF_close
    CreateCall2(def->builder, def->luaF_closeF, def->L, val);
  }
  // Do the jump
  def->builder->CreateBr(def->jmp_targets[j].jmp1);
  // Add the else block and make it current so that the next instruction flows
  // here
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
}

llvm::Value *RaviCodeGenerator::emit_boolean_testfalse(RaviFunctionDef *def,
                                                       llvm::Value *reg,
                                                       bool donot) {
  // (isnil() || isbool() && b == 0)

  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *var = TmpB.CreateAlloca(
      llvm::Type::getInt1Ty(def->jitState->context()), nullptr, "b");

  llvm::Value *type = emit_load_type(def, reg);

  // Test if type == LUA_TNIL (0)
  llvm::Value *isnil = emit_is_value_of_type(def, type, LUA__TNIL, "is.nil");
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.nil", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "not.nil");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "end");
  def->builder->CreateCondBr(isnil, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  emit_store_local_int(def, isnil, var);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
  // value is not nil
  // so check if bool and b == 0

  // Test if type == LUA_TBOOLEAN
  llvm::Value *isbool =
      emit_is_value_of_type(def, type, LUA__TBOOLEAN, "is.boolean");
  // Test if bool value == 0
  llvm::Value *bool_value = emit_load_reg_b(def, reg);
  llvm::Value *boolzero =
      def->builder->CreateICmpEQ(bool_value, def->types->kInt[0]);
  // Test type == LUA_TBOOLEAN && bool value == 0
  llvm::Value *andvalue = def->builder->CreateAnd(isbool, boolzero);

  emit_store_local_int(def, andvalue, var);
  def->builder->CreateBr(end_block);

  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);

  llvm::Value *result = nullptr;
  if (donot) {
    auto ins = emit_load_local_int(def, var);
    result = def->builder->CreateNot(ins);
  } else {
    auto ins = emit_load_local_int(def, var);
    result = ins;
  }

  return result;
}

void RaviCodeGenerator::emit_TEST(RaviFunctionDef *def, int A, int B, int C,
                                  int j, int jA) {

  //    case OP_TEST: {
  //      if (GETARG_C(i) ? l_isfalse(ra) : !l_isfalse(ra))
  //        ci->u.l.savedpc++;
  //      else
  //        donextjump(ci);
  //    } break;

  // Load pointer to base
  emit_load_base(def);

  // Get pointer to register A
  llvm::Value *ra = emit_gep_register(def, A);
  // v = C ? is_false(ra) : !is_false(ra)
  llvm::Value *v = C ? emit_boolean_testfalse(def, ra, false)
                     : emit_boolean_testfalse(def, ra, true);

  // Test NOT v
  llvm::Value *result = def->builder->CreateNot(v);
  // If !v then we need to execute the next statement which is a jump
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(result, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  // if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  if (jA > 0) {
    // jA is the A operand of the Jump instruction

    // base + a - 1
    llvm::Value *val = emit_gep_register(def, jA - 1);

    // Call luaF_close
    CreateCall2(def->builder, def->luaF_closeF, def->L, val);
  }
  // Do the jump
  def->builder->CreateBr(def->jmp_targets[j].jmp1);

  // Add the else block and make it current so that the next instruction flows
  // here
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
}

void RaviCodeGenerator::emit_NOT(RaviFunctionDef *def, int A, int B) {
  //  case OP_NOT: {
  //    TValue *rb = RB(i);
  //    int res = l_isfalse(rb);  /* next assignment may change this value */
  //    setbvalue(ra, res);
  //  } break;
  emit_load_base(def);
  // Get pointer to register B
  llvm::Value *rb = emit_gep_register(def, B);
  llvm::Value *v = emit_boolean_testfalse(def, rb, false);
  llvm::Value *result = def->builder->CreateZExt(v, def->types->C_intT, "i");
  llvm::Value *ra = emit_gep_register(def, A);
  emit_store_reg_b(def, result, ra);
  emit_store_type(def, ra, LUA_TBOOLEAN);
}

void RaviCodeGenerator::emit_TESTSET(RaviFunctionDef *def, int A, int B, int C,
                                     int j, int jA) {

  //  case OP_TESTSET: {
  //    TValue *rb = RB(i);
  //    if (GETARG_C(i) ? l_isfalse(rb) : !l_isfalse(rb))
  //      ci->u.l.savedpc++;
  //    else {
  //      setobjs2s(L, ra, rb);
  //      donextjump(ci);
  //    }
  //  } break;

  // Load pointer to base
  emit_load_base(def);

  // Get pointer to register B
  llvm::Value *rb = emit_gep_register(def, B);
  // v = C ? is_false(ra) : !is_false(ra)
  llvm::Value *v = C ? emit_boolean_testfalse(def, rb, false)
                     : emit_boolean_testfalse(def, rb, true);

  // Test NOT v
  llvm::Value *result = def->builder->CreateNot(v);
  // If !v then we need to execute the next statement which is a jump
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(result, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  // Get pointer to register A
  llvm::Value *ra = emit_gep_register(def, A);
  emit_assign(def, ra, rb);

  // if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  if (jA > 0) {
    // jA is the A operand of the Jump instruction

    // base + a - 1
    llvm::Value *val = emit_gep_register(def, jA - 1);

    // Call luaF_close
    CreateCall2(def->builder, def->luaF_closeF, def->L, val);
  }
  // Do the jump
  def->builder->CreateBr(def->jmp_targets[j].jmp1);

  // Add the else block and make it current so that the next instruction flows
  // here
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
}
}

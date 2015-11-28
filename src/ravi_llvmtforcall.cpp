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

void RaviCodeGenerator::emit_TFORCALL(RaviFunctionDef *def, int A, int B, int C,
                                      int j, int jA, int pc) {

  //  case OP_TFORCALL: {
  //    StkId cb = ra + 3;  /* call base */
  //    setobjs2s(L, cb + 2, ra + 2);
  //    setobjs2s(L, cb + 1, ra + 1);
  //    setobjs2s(L, cb, ra);
  //    L->top = cb + 3;  /* func. + 2 args (state and index) */
  //    Protect(luaD_call(L, cb, GETARG_C(i), 1));
  //    L->top = ci->top;
  //    i = *(ci->u.l.savedpc++);  /* go to next instruction */
  //    ra = RA(i);
  //    lua_assert(GET_OPCODE(i) == OP_TFORLOOP);
  //    goto l_tforloop;
  //  }
  //  case OP_TFORLOOP: {
  //  l_tforloop:
  //    if (!ttisnil(ra + 1)) {  /* continue loop? */
  //      setobjs2s(L, ra, ra + 1);  /* save control variable */
  //      ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    }
  //  } break;

  bool traced = emit_debug_trace(def, OP_TFORCALL, pc);
  // Below may invoke metamethod so we set savedpc
  if (!traced)emit_update_savedpc(def, pc);
  // Load pointer to base
  emit_load_base(def);

  // Get pointer to register A
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *ra1 = emit_gep_register(def, A + 1);
  llvm::Value *ra2 = emit_gep_register(def, A + 2);
  llvm::Value *cb = emit_gep_register(def, A + 3);
  llvm::Value *cb1 = emit_gep_register(def, A + 4);
  llvm::Value *cb2 = emit_gep_register(def, A + 5);

  emit_assign(def, cb2, ra2);
  emit_assign(def, cb1, ra1);
  emit_assign(def, cb, ra);

  // L->top = cb + 3;  /* func. + 2 args (state and index) */
  emit_set_L_top_toreg(def, A + 6);

  // Protect(luaD_call(L, cb, GETARG_C(i)));
  CreateCall3(def->builder, def->luaD_callF, def->L, cb, def->types->kInt[C]);
  // reload base
  emit_load_base(def);
  // L->top = ci->top;
  emit_refresh_L_top(def, def->ci_val);
  ra = emit_gep_register(def, jA);
  ra1 = emit_gep_register(def, jA + 1);
  llvm::Value *type = emit_load_type(def, ra1);

  // Test if type != LUA_TNIL (0)
  llvm::Value *isnotnil =
      emit_is_not_value_of_type(def, type, LUA__TNIL, "is.not.nil");
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.nil", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.nil");
  def->builder->CreateCondBr(isnotnil, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  emit_assign(def, ra, ra1);
  // Do the jump
  def->builder->CreateBr(def->jmp_targets[j].jmp1);

  // Add the else block and make it current so that the next instruction flows
  // here
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
}

void RaviCodeGenerator::emit_TFORLOOP(RaviFunctionDef *def, int A, int j,
                                      int pc) {
  //  case OP_TFORLOOP: {
  //  l_tforloop:
  //    if (!ttisnil(ra + 1)) {  /* continue loop? */
  //      setobjs2s(L, ra, ra + 1);  /* save control variable */
  //      ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    }
  //  } break;

  emit_debug_trace(def, OP_TFORLOOP, pc);
  // Load pointer to base
  emit_load_base(def);

  // Get pointer to register A
  llvm::Value *ra = emit_gep_register(def, A);
  llvm::Value *ra1 = emit_gep_register(def, A + 1);
  llvm::Value *type = emit_load_type(def, ra1);

  // Test if type != LUA_TNIL (0)
  llvm::Value *isnotnil =
      emit_is_not_value_of_type(def, type, LUA__TNIL, "is.not.nil");
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.nil", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.nil");
  def->builder->CreateCondBr(isnotnil, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  emit_assign(def, ra, ra1);
  // Do the jump
  def->builder->CreateBr(def->jmp_targets[j].jmp1);

  // Add the else block and make it current so that the next instruction flows
  // here
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
}
}
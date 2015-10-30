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

void RaviCodeGenerator::emit_CONCAT(RaviFunctionDef *def, int A, int B, int C,
                                    int pc) {
  bool traced = emit_debug_trace(def, OP_CONCAT, pc);
  // below may invoke metamethod so we need to set savedpc
  if (!traced) emit_update_savedpc(def, pc);
  CreateCall5(def->builder, def->raviV_op_concatF, def->L, def->ci_val,
              llvm::ConstantInt::get(def->types->C_intT, A),
              llvm::ConstantInt::get(def->types->C_intT, B),
              llvm::ConstantInt::get(def->types->C_intT, C));
}

void RaviCodeGenerator::emit_CLOSURE(RaviFunctionDef *def, int A, int Bx,
                                     int pc) {
  emit_debug_trace(def, OP_CLOSURE, pc);
  CreateCall5(def->builder, def->raviV_op_closureF, def->L, def->ci_val,
              def->p_LClosure, llvm::ConstantInt::get(def->types->C_intT, A),
              llvm::ConstantInt::get(def->types->C_intT, Bx));
}

void RaviCodeGenerator::emit_VARARG(RaviFunctionDef *def, int A, int B,
                                    int pc) {
  emit_debug_trace(def, OP_VARARG, pc);
  CreateCall5(def->builder, def->raviV_op_varargF, def->L, def->ci_val,
              def->p_LClosure, llvm::ConstantInt::get(def->types->C_intT, A),
              llvm::ConstantInt::get(def->types->C_intT, B));
}
}
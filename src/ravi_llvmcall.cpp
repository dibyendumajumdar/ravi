#include "ravi_llvmcodegen.h"

namespace ravi {

// Handle OP_CALL
void RaviCodeGenerator::emit_CALL(RaviFunctionDef *def, llvm::Value *L_ci,
                                  llvm::Value *proto, int A, int B, int C) {

  // int nresults = c - 1;
  // if (b != 0)
  //   L->top = ra + b;  /* else previous instruction set top */
  // int c = luaD_precall(L, ra, nresults, 1);  /* C or JITed function? */
  // if (c) {
  //   if (c == 1 && nresults >= 0)
  //     L->top = ci->top;  /* adjust results if C function */
  // }
  // else {  /* Lua function */
  //   luaV_execute(L);
  // }

  llvm::Instruction *base_ptr = emit_load_base(def);
  llvm::Value *top = nullptr;

  // int nresults = c - 1;
  int nresults = C - 1;

  // if (b != 0)
  if (B != 0) {
    // L->top = ra + b
    // See similar construct in OP_RETURN
    // Get pointer to register at ra + b
    llvm::Value *ptr = emit_array_get(def, base_ptr, A + B);
    // Get pointer to L->top
    top = emit_gep(def, "L.top", def->L, 0, 4);
    // Assign to L->top
    llvm::Instruction *ins = def->builder->CreateStore(ptr, top);
    ins->setMetadata(llvm::LLVMContext::MD_tbaa,
                     def->types->tbaa_luaState_topT);
  }

  // luaD_precall() returns following
  // 1 - C function called, results to be adjusted
  // 2 - JITed Lua function called, no action
  // 0 - Run interpreter on Lua function

  // int c = luaD_precall(L, ra, nresults, 1);  /* C or JITed function? */
  llvm::Value *ra = emit_gep_ra(def, base_ptr, A);
  llvm::Value *precall_result = def->builder->CreateCall4(
      def->luaD_precallF, def->L, ra,
      llvm::ConstantInt::get(def->types->C_intT, nresults),
      def->types->kInt[2]);
  llvm::Value *precall_bool =
      def->builder->CreateICmpEQ(precall_result, def->types->kInt[0]);

  llvm::BasicBlock *then_block = llvm::BasicBlock::Create(
      def->jitState->context(), "if.lua.function", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.not.lua.function");
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(def->jitState->context(), "op_call.done");
  def->builder->CreateCondBr(precall_bool, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  // Lua function so call luaV_execute
  def->builder->CreateCall(def->luaV_executeF, def->L);
  def->builder->CreateBr(end_block);
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  if (nresults >= 0) {
    //   if (c == 1 && nresults >= 0)
    //     L->top = ci->top;  /* adjust results if C function */
    llvm::Value *precall_C =
        def->builder->CreateICmpEQ(precall_result, def->types->kInt[1]);

    llvm::BasicBlock *then1_block = llvm::BasicBlock::Create(
        def->jitState->context(), "if.C.function", def->f);
    def->builder->CreateCondBr(precall_C, then1_block, end_block);
    def->builder->SetInsertPoint(then1_block);

    // Get pointer to ci->top
    llvm::Value *citop = emit_gep(def, "ci_top", def->ci_val, 0, 1);
    // Load ci->top
    llvm::Instruction *citop_val = def->builder->CreateLoad(citop);
    // TODO set tbaa
    if (!top)
      // Get L->top
      top = emit_gep(def, "L_top", def->L, 0, 4);
    // Assign ci>top to L->top
    auto ins = def->builder->CreateStore(citop_val, top);
    ins->setMetadata(llvm::LLVMContext::MD_tbaa,
                     def->types->tbaa_luaState_topT);
  }
  def->builder->CreateBr(end_block);
  def->f->getBasicBlockList().push_back(end_block);
  def->builder->SetInsertPoint(end_block);
}
}
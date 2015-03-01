#include "ravi_llvmcodegen.h"

namespace ravi {

RaviCodeGenerator::RaviCodeGenerator(RaviJITStateImpl *jitState)
    : jitState_(jitState), id_(1) {
  temp_[0] = 0;
}

const char *RaviCodeGenerator::unique_function_name() {
  snprintf(temp_, sizeof temp_, "ravif%d", id_++);
  return temp_;
}

llvm::Value *RaviCodeGenerator::emit_gep(RaviFunctionDef *def, const char *name,
                                         llvm::Value *s, int arg1, int arg2) {
  llvm::SmallVector<llvm::Value *, 2> values;
  values.push_back(def->types->kInt[arg1]);
  values.push_back(def->types->kInt[arg2]);
  return def->builder->CreateInBoundsGEP(s, values, name);
}

llvm::Value *RaviCodeGenerator::emit_gep(RaviFunctionDef *def, const char *name,
                                         llvm::Value *s, int arg1, int arg2,
                                         int arg3) {
  llvm::SmallVector<llvm::Value *, 3> values;
  values.push_back(def->types->kInt[arg1]);
  values.push_back(def->types->kInt[arg2]);
  values.push_back(def->types->kInt[arg3]);
  return def->builder->CreateInBoundsGEP(s, values, name);
}

llvm::Value *
RaviCodeGenerator::emit_gep_ci_func_value_gc_asLClosure(RaviFunctionDef *def,
                                                        llvm::Value *ci) {
  // emit code for (LClosure *)ci->func->value_.gc
  // fortunately as func is at the beginning of the ci
  // structure we can just cast ci to LClosure*
  llvm::Value *pppLuaClosure =
      def->builder->CreateBitCast(ci, def->types->pppLClosureT);
  llvm::Instruction *ppLuaClosure = def->builder->CreateLoad(pppLuaClosure);
  ppLuaClosure->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_CallInfo_funcT);
  return ppLuaClosure;
}

llvm::Value *RaviCodeGenerator::emit_array_get(RaviFunctionDef *def,
                                               llvm::Value *ptr, int offset) {
  // emit code for &ptr[offset]
  return def->builder->CreateInBoundsGEP(
      ptr, llvm::ConstantInt::get(def->types->C_intT, offset));
}

void RaviCodeGenerator::emit_JMP(RaviFunctionDef *def, int j) {
  assert(def->jmp_targets[j]);
  if (def->builder->GetInsertBlock()->getTerminator()) {
    llvm::BasicBlock *jmp_block =
        llvm::BasicBlock::Create(def->jitState->context(), "jump", def->f);
    def->builder->SetInsertPoint(jmp_block);
  }
  def->builder->CreateBr(def->jmp_targets[j]);
}

void RaviCodeGenerator::emit_LOADK(RaviFunctionDef *def, llvm::Value *L_ci,
                                   llvm::Value *proto, int A, int Bx) {
  //    case OP_LOADK: {
  //      TValue *rb = k + GETARG_Bx(i);
  //      setobj2s(L, ra, rb);
  //    } break;

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_ci_baseT);

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
  // destvalue->value->i = srcvalue->value->i;
  llvm::Value *srcvalue = emit_gep(def, "srcvalue", src, 0, 0, 0);
  llvm::Value *destvalue = emit_gep(def, "destvalue", dest, 0, 0, 0);
  llvm::Instruction *store = def->builder->CreateStore(def->builder->CreateLoad(srcvalue), destvalue);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);

  // destvalue->type = srcvalue->type
  llvm::Value *srctype = emit_gep(def, "srctype", src, 0, 1);
  llvm::Value *desttype = emit_gep(def, "desttype", dest, 0, 1);
  store = def->builder->CreateStore(def->builder->CreateLoad(srctype), desttype);
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

void RaviCodeGenerator::emit_RETURN(RaviFunctionDef *def, llvm::Value *L_ci,
                                    llvm::Value *proto, int A, int B) {

  // Here is what OP_RETURN looks like. We only compile steps
  // marked with //*.
  // TODO that means we cannot handle functions that have sub
  // functions (closures) as do not handle the luaF_close() call

  // case OP_RETURN: {
  //  int b = GETARG_B(i);
  //*  if (b != 0) L->top = ra + b - 1;
  //*  if (cl->p->sizep > 0) luaF_close(L, base);
  //*  b = luaD_poscall(L, ra);
  //    if (!(ci->callstatus & CIST_REENTRY))  /* 'ci' still the called one */
  //      return;  /* external invocation: return */
  //    else {  /* invocation via reentry: continue execution */
  //*      ci = L->ci;
  //*      if (b) L->top = ci->top;
  //      goto newframe;  /* restart luaV_execute over new Lua function */
  //    }
  // }

  // As Lua inserts redundant OP_RETURN instructions it is
  // possible that this is one of them. If this is the case then the
  // current block may already be terminated - so we have to insert
  // a new block
  if (def->builder->GetInsertBlock()->getTerminator()) {
    llvm::BasicBlock *return_block =
        llvm::BasicBlock::Create(def->jitState->context(), "return", def->f);
    def->builder->SetInsertPoint(return_block);
  }

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_ci_baseT);

  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr; 

  llvm::Value *top = nullptr;

  // Get pointer to register A
  llvm::Value *ra_ptr = A == 0 ? base_ptr : emit_array_get(def, base_ptr, A);

  //*  if (b != 0) L->top = ra + b - 1;
  if (B != 0) {
    // Get pointer to register at ra + b - 1
    llvm::Value *ptr = emit_array_get(def, base_ptr, A + B - 1);
    // Get pointer to L->top
    top = emit_gep(def, "L_top", def->L, 0, 4);
    // Assign to L->top
    llvm::Instruction *ins = def->builder->CreateStore(ptr, top);
    ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_topT);
  }

  // if (cl->p->sizep > 0) luaF_close(L, base);
  // Get pointer to Proto->sizep
  llvm::Value *psize_ptr = emit_gep(def, "sizep", def->proto_ptr, 0, 10);
  // Load psize
  llvm::Instruction *psize = def->builder->CreateLoad(psize_ptr);
  // Test if psize > 0
  llvm::Value *psize_gt_0 =
      def->builder->CreateICmpSGT(psize, def->types->kInt[0]);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(psize_gt_0, then_block, else_block);
  def->builder->SetInsertPoint(then_block);
  // Call luaF_close
  def->builder->CreateCall2(def->luaF_closeF, def->L, base_ptr);
  def->builder->CreateBr(else_block);
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  //*  b = luaD_poscall(L, ra);
  llvm::Value *result =
      def->builder->CreateCall2(def->luaD_poscallF, def->L, ra_ptr);

  //*      if (b) L->top = ci->top;
  // Test if b is != 0
  llvm::Value *result_is_notzero =
      def->builder->CreateICmpNE(result, def->types->kInt[0]);
  llvm::BasicBlock *ThenBB =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *ElseBB =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(result_is_notzero, ThenBB, ElseBB);
  def->builder->SetInsertPoint(ThenBB);
  // Get pointer to ci->top
  llvm::Value *citop = emit_gep(def, "ci_top", def->ci_val, 0, 1);
  // Load ci->top
  llvm::Instruction *citop_val = def->builder->CreateLoad(citop);
  if (!top)
    // Get L->top
    top = emit_gep(def, "L_top", def->L, 0, 4);
  // Assign ci>top to L->top
  llvm::Instruction *ins2 = def->builder->CreateStore(citop_val, top);
  def->builder->CreateBr(ElseBB);
  def->f->getBasicBlockList().push_back(ElseBB);
  def->builder->SetInsertPoint(ElseBB);

  // as our prototype is lua_Cfunction we need
  // to return a value
  def->builder->CreateRet(def->types->kInt[1]);
}

void RaviCodeGenerator::emit_EQ(RaviFunctionDef *def, llvm::Value *L_ci,
                                llvm::Value *proto, int A, int B, int C, int j,
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
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_ci_baseT);

  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr;

  llvm::Value *lhs_ptr;
  llvm::Value *rhs_ptr;

  // Get pointer to register B
  llvm::Value *base_or_k = ISK(B) ? k_ptr : base_ptr;
  int b = ISK(B) ? INDEXK(B) : B;
  if (b == 0) {
    lhs_ptr = base_or_k;
  } else {
    lhs_ptr = emit_array_get(def, base_or_k, b);
  }

  // Get pointer to register C
  base_or_k = ISK(C) ? k_ptr : base_ptr;
  int c = ISK(C) ? INDEXK(C) : C;
  if (c == 0) {
    rhs_ptr = base_or_k;
  } else {
    rhs_ptr = emit_array_get(def, base_or_k, c);
  }

  // Call luaV_equalobj with register B and C
  llvm::Value *result =
      def->builder->CreateCall3(callee, def->L, lhs_ptr, rhs_ptr);
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

    // Load pointer to base
    llvm::Instruction *base2_ptr = def->builder->CreateLoad(def->Ci_base);
    base2_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_ci_baseT);

    // base + a - 1
    llvm::Value *val =
        jA == 1 ? base2_ptr : emit_array_get(def, base2_ptr, jA - 1);

    // Call
    def->builder->CreateCall2(def->luaF_closeF, def->L, val);
  }
  // Do the jump
  def->builder->CreateBr(def->jmp_targets[j]);
  // Add the else block and make it current so that the next instruction flows
  // here
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
}

// Check if we can compile
// The cases we can compile will increase over time
bool RaviCodeGenerator::canCompile(Proto *p) {
  if (p->ravi_jit.jit_status == 1)
    return false;
  if (jitState_ == nullptr) {
    p->ravi_jit.jit_status = 1;
    return false;
  }
  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  // TODO we cannot handle variable arguments or
  // if the function has sub functions (closures)
  if (p->sizep > 0 || p->is_vararg) {
    p->ravi_jit.jit_status = 1;
    return false;
  }
  // Loop over the byte codes; as Lua compiler inserts
  // an extra RETURN op we need to ignore the last op
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode o = GET_OPCODE(i);
    switch (o) {
    case OP_LOADK:
    case OP_RETURN:
    case OP_JMP:
    case OP_EQ:
    case OP_LT:
    case OP_LE:
#if 0
    case OP_FORPREP:
    case OP_FORLOOP:
    case OP_MOVE:
#endif
      break;
    default:
      return false;
    }
  }
  return true;
}

std::unique_ptr<RaviJITFunctionImpl>
RaviCodeGenerator::create_function(llvm::IRBuilder<> &builder,
                                   RaviFunctionDef *def) {
  LuaLLVMTypes *types = jitState_->types();

  std::unique_ptr<ravi::RaviJITFunctionImpl> func =
      std::unique_ptr<RaviJITFunctionImpl>(
          (RaviJITFunctionImpl *)jitState_->createFunction(
              types->jitFunctionT, llvm::Function::ExternalLinkage,
              unique_function_name()));
  if (!func)
    return func;

  llvm::Function *mainFunc = func->function();
  llvm::BasicBlock *entry =
      llvm::BasicBlock::Create(jitState_->context(), "entry", mainFunc);
  builder.SetInsertPoint(entry);

  auto argiter = mainFunc->arg_begin();
  llvm::Value *arg1 = argiter++;
  arg1->setName("L");

  def->jitState = jitState_;
  def->f = mainFunc;
  def->entry = entry;
  def->L = arg1;
  def->raviF = func.get();
  def->types = types;
  def->builder = &builder;

  return func;
}

void RaviCodeGenerator::emit_extern_declarations(RaviFunctionDef *def) {
  // Add extern declarations for Lua functions that we need to call
  def->luaD_poscallF = def->raviF->addExternFunction(
      def->types->luaD_poscallT, reinterpret_cast<void *>(&luaD_poscall), "luaD_poscall");
  def->luaF_closeF = def->raviF->addExternFunction(def->types->luaF_closeT,
                                                   reinterpret_cast<void *>(&luaF_close), "luaF_close");
  def->luaV_equalobjF = def->raviF->addExternFunction(
      def->types->luaV_equalobjT, reinterpret_cast<void *>(&luaV_equalobj), "luaV_equalobj");
  def->luaV_lessthanF = def->raviF->addExternFunction(
      def->types->luaV_lessthanT, reinterpret_cast<void *>(&luaV_lessthan), "luaV_lessthan");
  def->luaV_lessequalF = def->raviF->addExternFunction(
      def->types->luaV_lessequalT, reinterpret_cast<void *>(&luaV_lessequal), "luaV_lessequal");
  def->luaG_runerrorF = def->raviF->addExternFunction(
    def->types->luaG_runerrorT, reinterpret_cast<void *>(&luaG_runerror), "luaG_runerror");
  def->luaV_forlimitF = def->raviF->addExternFunction(
    def->types->luaV_forlimitT, reinterpret_cast<void *>(&luaV_forlimit), "luaV_forlimit");
  def->luaV_tonumberF = def->raviF->addExternFunction(
    def->types->luaV_tonumberT, reinterpret_cast<void *>(&luaV_tonumber_), "luaV_tonumber_");
}

#define RA(i) (base + GETARG_A(i))
/* to be used after possible stack reallocation */
#define RB(i) check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i))
#define RC(i) check_exp(getCMode(GET_OPCODE(i)) == OpArgR, base + GETARG_C(i))
#define RKB(i)                                                                 \
  check_exp(getBMode(GET_OPCODE(i)) == OpArgK,                                 \
            ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i))
#define RKC(i)                                                                 \
  check_exp(getCMode(GET_OPCODE(i)) == OpArgK,                                 \
            ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i))
#define KBx(i)                                                                 \
  (k + (GETARG_Bx(i) != 0 ? GETARG_Bx(i) - 1 : GETARG_Ax(*ci->u.l.savedpc++)))
/* RAVI */
#define KB(i)                                                                  \
  check_exp(getBMode(GET_OPCODE(i)) == OpArgK, k + INDEXK(GETARG_B(i)))
#define KC(i)                                                                  \
  check_exp(getCMode(GET_OPCODE(i)) == OpArgK, k + INDEXK(GETARG_C(i)))

void RaviCodeGenerator::link_block(RaviFunctionDef *def, int pc) {
  // If the current bytecode offset pc is on a jump target
  // then we need to insert the block we previously created in
  // scan_jump_targets()
  // and make it the current insert block; also if the previous block
  // is unterminated then we simply provide a branch from previous block to the
  // new block
  if (def->jmp_targets[pc]) {
    // We are on a jump target
    // Get the block we previously created scan_jump_targets
    llvm::BasicBlock *block = def->jmp_targets[pc];
    if (!def->builder->GetInsertBlock()->getTerminator()) {
      // Previous block not terminated so branch to the
      // new block
      def->builder->CreateBr(block);
    }
    // Now add the new block and make it current
    def->f->getBasicBlockList().push_back(block);
    def->builder->SetInsertPoint(block);
  }
}

void RaviCodeGenerator::compile(lua_State *L, Proto *p) {
  if (p->ravi_jit.jit_status != 0 || !canCompile(p))
    return;
#if 1
  RaviFunctionDef def = {0};

  llvm::LLVMContext &context = jitState_->context();
  llvm::IRBuilder<> builder(context);

  auto f = create_function(builder, &def);
  if (!f) {
    p->ravi_jit.jit_status = 1; // can't compile
    return;
  }
  // Add extern declarations for Lua functions we need to call
  emit_extern_declarations(&def);

  // Create BasicBlocks for all the jump targets in the Lua bytecode
  scan_jump_targets(&def, p);

  // Get pointer to L->ci
  llvm::Value *L_ci = emit_gep(&def, "L_ci", def.L, 0, 6);

  // Load L->ci
  def.ci_val = builder.CreateLoad(L_ci);
  def.ci_val->setMetadata(llvm::LLVMContext::MD_tbaa, def.types->tbaa_CallInfoT); 

  // Get pointer to base
  def.Ci_base = emit_gep(&def, "base", def.ci_val, 0, 4, 0);

  // We need to get hold of the constants table
  // which is located in ci->func->value_.gc
  // and is a value of type LClosure*
  // fortunately as func is at the beginning of the ci
  // structure we can just cast ci to LClosure*
  llvm::Value *L_cl = emit_gep_ci_func_value_gc_asLClosure(&def, def.ci_val);

  // Load pointer to LClosure
  llvm::Instruction *cl_ptr = builder.CreateLoad(L_cl);
  cl_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def.types->tbaa_CallInfo_func_LClosureT);

  // Get pointer to the Proto* which is cl->p
  llvm::Value *proto = emit_gep(&def, "Proto", cl_ptr, 0, 5);

  // Load pointer to proto
  def.proto_ptr = builder.CreateLoad(proto);
  def.proto_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def.types->tbaa_LClosure_pT);

  // Obtain pointer to Proto->k
  def.proto_k = emit_gep(&def, "k", def.proto_ptr, 0, 14);

  // Load pointer to k
  def.k_ptr = builder.CreateLoad(def.proto_k);
  def.k_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def.types->tbaa_Proto_kT);
  
  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  for (pc = 0; pc < n; pc++) {
    link_block(&def, pc);
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    int A = GETARG_A(i);
    switch (op) {
    case OP_LOADK: {
      int Bx = GETARG_Bx(i);
      emit_LOADK(&def, L_ci, proto, A, Bx);
    } break;
    case OP_RETURN: {
      int B = GETARG_B(i);
      emit_RETURN(&def, L_ci, proto, A, B);
    } break;
    case OP_LT:
    case OP_LE:
    case OP_EQ: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      llvm::Constant *comparison_function =
          (op == OP_EQ
               ? def.luaV_equalobjF
               : (op == OP_LT ? def.luaV_lessthanF : def.luaV_lessequalF));
      // OP_EQ is followed by OP_JMP - we process this
      // along with OP_EQ
      pc++;
      i = code[pc];
      op = GET_OPCODE(i);
      lua_assert(op == OP_JMP);
      int sbx = GETARG_sBx(i);
      // j below is the jump target
      int j = sbx + pc + 1;
      emit_EQ(&def, L_ci, proto, A, B, C, j, GETARG_A(i), comparison_function);
    } break;
    case OP_JMP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      emit_JMP(&def, j);
    } break;
    case OP_FORPREP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      emit_FORPREP(&def, L_ci, proto, A, j);
    } break;
    case OP_FORLOOP: {
      assert(false);
    } break;
    default:
      break;
    }
  }

  llvm::verifyFunction(*f->function());
  ravi::RaviJITFunctionImpl *llvm_func = f.release();
  p->ravi_jit.jit_data = reinterpret_cast<void *>(llvm_func);
  p->ravi_jit.jit_function = (lua_CFunction)llvm_func->compile();
  lua_assert(p->ravi_jit.jit_function);
  if (p->ravi_jit.jit_function == nullptr) {
    p->ravi_jit.jit_status = 1; // can't compile
    delete llvm_func;
    p->ravi_jit.jit_data = NULL;
  } else {
    p->ravi_jit.jit_status = 2;
  }
#else
  // For now
  p->ravi_jit.jit_status = 1; // can't compile
#endif
}

void RaviCodeGenerator::scan_jump_targets(RaviFunctionDef *def, Proto *p) {
  // We need to precreate blocks for jump targets so that we
  // can generate branch instructions in the code
  def->jmp_targets.clear();
  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  for (pc = 0; pc < n; pc++)
    def->jmp_targets.push_back(nullptr);
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    int A = GETARG_A(i);
    switch (op) {
    case OP_JMP:
    case OP_FORLOOP:
    case OP_FORPREP:
    case OP_TFORLOOP: {
      const char *targetname = nullptr;
      char temp[80];
      if (op == OP_JMP)
        targetname = "jmp";
      else if (op == OP_FORLOOP)
        targetname = "forbody";
      else if (op == OP_FORPREP)
        targetname = "forloop";
      else
        targetname = "tforbody";
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      snprintf(temp, sizeof temp, "%s%d_", targetname, j+1);
      def->jmp_targets[j] =
          llvm::BasicBlock::Create(def->jitState->context(), temp);
    } break;
    default:
      break;
    }
  }
}

}
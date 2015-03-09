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

RaviBranchDef::RaviBranchDef()
    : jmp1(nullptr), jmp2(nullptr), jmp3(nullptr), jmp4(nullptr),
      ilimit(nullptr), istep(nullptr), iidx(nullptr), flimit(nullptr),
      fstep(nullptr), fidx(nullptr) {}

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
  ppLuaClosure->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_CallInfo_funcT);
  return ppLuaClosure;
}

llvm::Value *RaviCodeGenerator::emit_array_get(RaviFunctionDef *def,
                                               llvm::Value *ptr, int offset) {
  // emit code for &ptr[offset]
  return def->builder->CreateInBoundsGEP(
      ptr, llvm::ConstantInt::get(def->types->C_intT, offset));
}

void RaviCodeGenerator::emit_JMP(RaviFunctionDef *def, int j) {
  assert(def->jmp_targets[j].jmp1);
  if (def->builder->GetInsertBlock()->getTerminator()) {
    llvm::BasicBlock *jmp_block =
        llvm::BasicBlock::Create(def->jitState->context(), "jump", def->f);
    def->builder->SetInsertPoint(jmp_block);
  }
  def->builder->CreateBr(def->jmp_targets[j].jmp1);
}

llvm::Instruction *RaviCodeGenerator::emit_load_base(RaviFunctionDef *def) {
  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_luaState_ci_baseT);
  return base_ptr;
}

llvm::Value *RaviCodeGenerator::emit_gep_ra(RaviFunctionDef *def,
                                            llvm::Instruction *base_ptr,
                                            int A) {
  llvm::Value *dest;
  if (A == 0) {
    // If A is 0 we can use the base pointer which is &base[0]
    dest = base_ptr;
  } else {
    // emit &base[A]
    dest = emit_array_get(def, base_ptr, A);
  }
  return dest;
}

llvm::Instruction *RaviCodeGenerator::emit_load_reg_n(RaviFunctionDef *def,
                                                      llvm::Value *rb) {
  llvm::Value *rb_n = def->builder->CreateBitCast(rb, def->types->plua_NumberT);
  llvm::Instruction *lhs = def->builder->CreateLoad(rb_n);
  lhs->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  return lhs;
}

llvm::Instruction *RaviCodeGenerator::emit_load_reg_i(RaviFunctionDef *def,
                                                      llvm::Value *rb) {
  llvm::Value *rb_n =
      def->builder->CreateBitCast(rb, def->types->plua_IntegerT);
  llvm::Instruction *lhs = def->builder->CreateLoad(rb_n);
  lhs->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  return lhs;
}

void RaviCodeGenerator::emit_store_reg_n(RaviFunctionDef *def,
                                         llvm::Value *result,
                                         llvm::Value *dest_ptr) {
  llvm::Value *ra_n =
      def->builder->CreateBitCast(dest_ptr, def->types->plua_NumberT);
  llvm::Instruction *store = def->builder->CreateStore(result, ra_n);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
}

void RaviCodeGenerator::emit_store_reg_i(RaviFunctionDef *def,
                                         llvm::Value *result,
                                         llvm::Value *dest_ptr) {
  llvm::Value *ra_n =
      def->builder->CreateBitCast(dest_ptr, def->types->plua_IntegerT);
  llvm::Instruction *store = def->builder->CreateStore(result, ra_n);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
}

void RaviCodeGenerator::emit_store_reg_b(RaviFunctionDef *def,
  llvm::Value *result,
  llvm::Value *dest_ptr) {
  llvm::Value *ra_n =
    def->builder->CreateBitCast(dest_ptr, def->types->C_pintT);
  llvm::Instruction *store = def->builder->CreateStore(result, ra_n);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
}

void RaviCodeGenerator::emit_store_type(RaviFunctionDef *def,
                                        llvm::Value *value, int type) {
  llvm::Value *desttype = emit_gep(def, "dest.tt", value, 0, 1);
  lua_assert(type == LUA_TNUMFLT || type == LUA_TNUMINT || type == LUA_TBOOLEAN);
  llvm::Instruction *store =
      def->builder->CreateStore(def->types->kInt[type], desttype);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
}

llvm::Instruction *RaviCodeGenerator::emit_load_type(RaviFunctionDef *def,
                                                     llvm::Value *value) {
  llvm::Value *tt_ptr = emit_gep(def, "value.tt.ptr", value, 0, 1);
  llvm::Instruction *tt = def->builder->CreateLoad(tt_ptr, "value.tt");
  tt->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  return tt;
}

llvm::Value *RaviCodeGenerator::emit_gep_rkb(RaviFunctionDef *def,
                                             llvm::Instruction *base_ptr,
                                             int B) {
  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr;
  llvm::Value *rb;
  // Get pointer to register B
  llvm::Value *base_or_k = ISK(B) ? k_ptr : base_ptr;
  int b = ISK(B) ? INDEXK(B) : B;
  if (b == 0) {
    rb = base_or_k;
  } else {
    rb = emit_array_get(def, base_or_k, b);
  }
  return rb;
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
  //if (p->sizep > 0 || p->is_vararg) {
  if (p->is_vararg) {
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
    case OP_LOADNIL:
    case OP_LOADBOOL:
    case OP_CALL:
    case OP_RETURN:
    case OP_JMP:
    case OP_EQ:
    case OP_LT:
    case OP_LE:
    case OP_FORPREP:
    case OP_FORLOOP:
    case OP_MOVE:
    case OP_RAVI_MOVEI:
    case OP_RAVI_MOVEF:
    case OP_RAVI_TOINT:
    case OP_RAVI_TOFLT:
    case OP_RAVI_LOADFZ:
    case OP_RAVI_LOADIZ:
    case OP_RAVI_ADDFN:
    case OP_RAVI_ADDIN:
    case OP_RAVI_ADDFF:
    case OP_RAVI_ADDFI:
    case OP_RAVI_ADDII:
    case OP_RAVI_SUBFF:
    case OP_RAVI_SUBFI:
    case OP_RAVI_SUBIF:
    case OP_RAVI_SUBII:
    case OP_RAVI_SUBFN:
    case OP_RAVI_SUBNF:
    case OP_RAVI_SUBIN:
    case OP_RAVI_SUBNI:
    case OP_RAVI_MULFN:
    case OP_RAVI_MULIN:
    case OP_RAVI_MULFF:
    case OP_RAVI_MULFI:
    case OP_RAVI_MULII:
    case OP_RAVI_DIVFF:
    case OP_RAVI_DIVFI:
    case OP_RAVI_DIVIF:
    case OP_RAVI_DIVII:
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
      def->types->luaD_poscallT, reinterpret_cast<void *>(&luaD_poscall),
      "luaD_poscall");
  def->luaD_precallF = def->raviF->addExternFunction(
      def->types->luaD_precallT, reinterpret_cast<void *>(&luaD_precall),
      "luaD_precall");
  def->luaF_closeF = def->raviF->addExternFunction(
      def->types->luaF_closeT, reinterpret_cast<void *>(&luaF_close),
      "luaF_close");
  def->luaG_runerrorF = def->raviF->addExternFunction(
      def->types->luaG_runerrorT, reinterpret_cast<void *>(&luaG_runerror),
      "luaG_runerror");
  def->luaV_equalobjF = def->raviF->addExternFunction(
      def->types->luaV_equalobjT, reinterpret_cast<void *>(&luaV_equalobj),
      "luaV_equalobj");
  def->luaV_lessthanF = def->raviF->addExternFunction(
      def->types->luaV_lessthanT, reinterpret_cast<void *>(&luaV_lessthan),
      "luaV_lessthan");
  def->luaV_lessequalF = def->raviF->addExternFunction(
      def->types->luaV_lessequalT, reinterpret_cast<void *>(&luaV_lessequal),
      "luaV_lessequal");
  def->luaV_forlimitF = def->raviF->addExternFunction(
      def->types->luaV_forlimitT, reinterpret_cast<void *>(&luaV_forlimit),
      "luaV_forlimit");
  def->luaV_tonumberF = def->raviF->addExternFunction(
      def->types->luaV_tonumberT, reinterpret_cast<void *>(&luaV_tonumber_),
      "luaV_tonumber_");
  def->luaV_tointegerF = def->raviF->addExternFunction(
      def->types->luaV_tointegerT, reinterpret_cast<void *>(&luaV_tointeger_),
      "luaV_tointeger_");
  def->luaV_executeF = def->raviF->addExternFunction(
      def->types->luaV_executeT, reinterpret_cast<void *>(&luaV_execute),
      "luaV_execute");
  def->luaV_op_loadnilF = def->raviF->addExternFunction(
      def->types->luaV_op_loadnilT, reinterpret_cast<void *>(&luaV_op_loadnil),
      "luaV_op_loadnil");

  // Create printf declaration
  std::vector<llvm::Type *> args;
  args.push_back(def->types->C_pcharT);
  // accepts a char*, is vararg, and returns int
  llvm::FunctionType *printfType =
      llvm::FunctionType::get(def->types->C_intT, args, true);
  def->printfFunc =
      def->raviF->module()->getOrInsertFunction("printf", printfType);
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
  // If we are on a jump target then check if this is a forloop
  // target. Forloop targets are special as they use computed goto
  // to branch to one of 4 possible labels. That is why we test for
  // jmp2 below.
  // We need to check whether current block is already terminated -
  // this can be because of a return or break or goto within the forloop
  // body
  if (def->jmp_targets[pc].jmp2 &&
      !def->builder->GetInsertBlock()->getTerminator()) {
    // Handle special case for body of FORLOOP
    auto b = def->builder->CreateLoad(def->jmp_targets[pc].forloop_branch);
    auto idb = def->builder->CreateIndirectBr(b, 4);
    idb->addDestination(def->jmp_targets[pc].jmp1);
    idb->addDestination(def->jmp_targets[pc].jmp2);
    idb->addDestination(def->jmp_targets[pc].jmp3);
    idb->addDestination(def->jmp_targets[pc].jmp4);
    // As we have terminated the block the code below will not
    // add the branch insruction, but we still need to create the new
    // block which is handled below.
  }
  // If the current bytecode offset pc is on a jump target
  // then we need to insert the block we previously created in
  // scan_jump_targets()
  // and make it the current insert block; also if the previous block
  // is unterminated then we simply provide a branch from previous block to the
  // new block
  if (def->jmp_targets[pc].jmp1) {
    // We are on a jump target
    // Get the block we previously created scan_jump_targets
    llvm::BasicBlock *block = def->jmp_targets[pc].jmp1;
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
  def.ci_val->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def.types->tbaa_CallInfoT);

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
  cl_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                      def.types->tbaa_CallInfo_func_LClosureT);

  // Get pointer to the Proto* which is cl->p
  llvm::Value *proto = emit_gep(&def, "Proto", cl_ptr, 0, 5);

  // Load pointer to proto
  def.proto_ptr = builder.CreateLoad(proto);
  def.proto_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                             def.types->tbaa_LClosure_pT);

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
    case OP_LOADBOOL: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_LOADBOOL(&def, L_ci, proto, A, B, C, pc+2);
    } break;
    case OP_MOVE: {
      int B = GETARG_B(i);
      emit_MOVE(&def, L_ci, proto, A, B);
    } break;
    case OP_RAVI_MOVEI: {
      int B = GETARG_B(i);
      emit_MOVEI(&def, L_ci, proto, A, B);
    } break;
    case OP_RAVI_MOVEF: {
      int B = GETARG_B(i);
      emit_MOVEF(&def, L_ci, proto, A, B);
    } break;
    case OP_RAVI_TOINT: {
      emit_TOINT(&def, L_ci, proto, A);
    } break;
    case OP_RAVI_TOFLT: {
      emit_TOFLT(&def, L_ci, proto, A);
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
#if RAVI_CODEGEN_FORPREP2
      emit_FORPREP2(&def, L_ci, proto, A, j);
#else
      emit_FORPREP(&def, L_ci, proto, A, j);
#endif
    } break;
    case OP_FORLOOP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
#if RAVI_CODEGEN_FORPREP2
      emit_FORLOOP2(&def, L_ci, proto, A, j, def.jmp_targets[pc]);
#else
      emit_FORLOOP(&def, L_ci, proto, A, j);
#endif
    } break;
    case OP_LOADNIL: {
      int B = GETARG_B(i);
      emit_LOADNIL(&def, L_ci, proto, A, B);
    } break;
    case OP_RAVI_LOADFZ: {
      emit_LOADFZ(&def, L_ci, proto, A);
    } break;
    case OP_RAVI_LOADIZ: {
      emit_LOADIZ(&def, L_ci, proto, A);
    } break;
    case OP_CALL: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_CALL(&def, L_ci, proto, A, B, C);
    } break;

    case OP_RAVI_ADDFN: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_ADDFN(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_ADDIN: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_ADDIN(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_ADDFF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_ADDFF(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_ADDFI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_ADDFI(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_ADDII: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_ADDII(&def, L_ci, proto, A, B, C);
    } break;

    case OP_RAVI_SUBFF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_SUBFF(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_SUBFI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_SUBFI(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_SUBIF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_SUBIF(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_SUBII: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_SUBII(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_SUBFN: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_SUBFN(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_SUBNF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_SUBNF(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_SUBIN: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_SUBIN(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_SUBNI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_SUBNI(&def, L_ci, proto, A, B, C);
    } break;

    case OP_RAVI_MULFN: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_MULFN(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_MULIN: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_MULIN(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_MULFF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_MULFF(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_MULFI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_MULFI(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_MULII: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_MULII(&def, L_ci, proto, A, B, C);
    } break;

    case OP_RAVI_DIVFF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_DIVFF(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_DIVFI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_DIVFI(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_DIVIF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_DIVIF(&def, L_ci, proto, A, B, C);
    } break;
    case OP_RAVI_DIVII: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      emit_DIVII(&def, L_ci, proto, A, B, C);
    } break;

    default:
      break;
    }
  }

  lua_assert(!llvm::verifyFunction(*f->function(), &llvm::errs()));
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
}

void RaviCodeGenerator::scan_jump_targets(RaviFunctionDef *def, Proto *p) {
  // We need to pre-create blocks for jump targets so that we
  // can generate branch instructions in the code
  def->jmp_targets.clear();
  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  def->jmp_targets.resize(n);
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    switch (op) {
    case OP_LOADBOOL: {
      int C = GETARG_C(i);
      int j = pc + 2; // jump target
      if (C && !def->jmp_targets[j].jmp1)
        def->jmp_targets[j].jmp1 = llvm::BasicBlock::Create(def->jitState->context(), "loadbool");
    } break;
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
#if RAVI_CODEGEN_FORPREP2
        targetname = "forloop_ilt";
#else
        targetname = "forloop";
#endif
      else
        targetname = "tforbody";
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      // We append the Lua bytecode location to help debug the IR
      snprintf(temp, sizeof temp, "%s%d_", targetname, j + 1);
      // 
      if (!def->jmp_targets[j].jmp1) {
        def->jmp_targets[j].jmp1 =
          llvm::BasicBlock::Create(def->jitState->context(), temp);
      }
#if RAVI_CODEGEN_FORPREP2
      if (op == OP_FORPREP) {
        lua_assert(def->jmp_targets[j].jmp2 == nullptr);
        // first target (created above) is for int < limit 
        // Second target is for int > limit
        snprintf(temp, sizeof temp, "%s%d_", "forloop_igt", j + 1);
        def->jmp_targets[j].jmp2 =
            llvm::BasicBlock::Create(def->jitState->context(), temp);
        // Third target is for float < limit
        snprintf(temp, sizeof temp, "%s%d_", "forloop_flt", j + 1);
        def->jmp_targets[j].jmp3 =
            llvm::BasicBlock::Create(def->jitState->context(), temp);
        // Fourth target is for float > limit
        snprintf(temp, sizeof temp, "%s%d_", "forloop_fgt", j + 1);
        def->jmp_targets[j].jmp4 =
            llvm::BasicBlock::Create(def->jitState->context(), temp);
      }
#endif
    } break;
    default:
      break;
    }
  }
}
}
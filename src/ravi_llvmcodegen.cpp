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
#include <ravijit.h>
#include <ravi_llvmcodegen.h>
#include <ravi_jitshared.h>

namespace ravi {

RaviBranchDef::RaviBranchDef()
    : jmp1(nullptr),
      jmp2(nullptr),
      jmp3(nullptr),
      jmp4(nullptr),
      ilimit(nullptr),
      istep(nullptr),
      iidx(nullptr),
      flimit(nullptr),
      fstep(nullptr),
      fidx(nullptr) {}

RaviCodeGenerator::RaviCodeGenerator(RaviJITState *jitState)
    : jitState_(jitState), id_(1) {
  temp_[0] = 0;
}

const char *RaviCodeGenerator::unique_function_name() {
  // Create unique name for the compiled function
  // Lua functions are nameless so a compiled function has no name
  // as far as Lua is concerned
  snprintf(temp_, sizeof temp_, "ravif%d", id_++);
  return temp_;
}

llvm::CallInst *RaviCodeGenerator::CreateCall1(llvm::IRBuilder<> *builder,
                                               llvm::Value *func,
                                               llvm::Value *arg1) {
  llvm::SmallVector<llvm::Value *, 1> values;
  values.push_back(arg1);
  return builder->CreateCall(func, values);
}

llvm::CallInst *RaviCodeGenerator::CreateCall2(llvm::IRBuilder<> *builder,
                                               llvm::Value *func,
                                               llvm::Value *arg1,
                                               llvm::Value *arg2) {
  llvm::SmallVector<llvm::Value *, 2> values;
  values.push_back(arg1);
  values.push_back(arg2);
  return builder->CreateCall(func, values);
}

llvm::CallInst *RaviCodeGenerator::CreateCall3(llvm::IRBuilder<> *builder,
                                               llvm::Value *func,
                                               llvm::Value *arg1,
                                               llvm::Value *arg2,
                                               llvm::Value *arg3) {
  llvm::SmallVector<llvm::Value *, 3> values;
  values.push_back(arg1);
  values.push_back(arg2);
  values.push_back(arg3);
  return builder->CreateCall(func, values);
}

llvm::CallInst *RaviCodeGenerator::CreateCall4(
    llvm::IRBuilder<> *builder, llvm::Value *func, llvm::Value *arg1,
    llvm::Value *arg2, llvm::Value *arg3, llvm::Value *arg4) {
  llvm::SmallVector<llvm::Value *, 4> values;
  values.push_back(arg1);
  values.push_back(arg2);
  values.push_back(arg3);
  values.push_back(arg4);
  return builder->CreateCall(func, values);
}

llvm::CallInst *RaviCodeGenerator::CreateCall5(
    llvm::IRBuilder<> *builder, llvm::Value *func, llvm::Value *arg1,
    llvm::Value *arg2, llvm::Value *arg3, llvm::Value *arg4,
    llvm::Value *arg5) {
  llvm::SmallVector<llvm::Value *, 5> values;
  values.push_back(arg1);
  values.push_back(arg2);
  values.push_back(arg3);
  values.push_back(arg4);
  values.push_back(arg5);
  return builder->CreateCall(func, values);
}

void RaviCodeGenerator::attach_branch_weights(RaviFunctionDef *def,
                                              llvm::Instruction *ins,
                                              uint32_t true_branch,
                                              uint32_t false_branch) {
#if RAVI_USE_LLVM_BRANCH_WEIGHTS
  ins->setMetadata(llvm::LLVMContext::MD_prof,
                   def->jitState->types()->mdbuilder.createBranchWeights(
                       true_branch, false_branch));
#else
    (void)def;
    (void)ins;
    (void)true_branch;
    (void)false_branch;
#endif
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

llvm::Value *RaviCodeGenerator::emit_gep(RaviFunctionDef *def, const char *name,
                                         llvm::Value *ptr, llvm::Value *offset,
                                         int arg1) {
  llvm::SmallVector<llvm::Value *, 2> values;
  values.push_back(offset);
  values.push_back(def->types->kInt[arg1]);
  return def->builder->CreateInBoundsGEP(ptr, values, name);
}

llvm::Value *RaviCodeGenerator::emit_gep(RaviFunctionDef *def, const char *name,
                                         llvm::Value *ptr, llvm::Value *offset,
                                         int arg1, int arg2) {
  llvm::SmallVector<llvm::Value *, 3> values;
  values.push_back(offset);
  values.push_back(def->types->kInt[arg1]);
  values.push_back(def->types->kInt[arg2]);
  return def->builder->CreateInBoundsGEP(ptr, values, name);
}

llvm::Value *RaviCodeGenerator::emit_array_get(RaviFunctionDef *def,
                                               llvm::Value *ptr, int offset) {
  // emit code for &ptr[offset]
  return def->builder->CreateInBoundsGEP(
      ptr, llvm::ConstantInt::get(def->types->C_intT, offset));
}

// emit code for LClosure *cl = clLvalue(ci->func);
llvm::Instruction *RaviCodeGenerator::emit_gep_ci_func_value_gc_asLClosure(
    RaviFunctionDef *def) {
  // clLvalue() is a macros that expands to (LClosure *)ci->func->value_.gc
  // via a complex series of macros and unions
  // fortunately as func is at the beginning of the CallInfo
  // structure we can just cast ci to LClosure***
  // This insight is based on code generated by Clang
  llvm::Value *pppLuaClosure =
      def->builder->CreateBitCast(def->ci_val, def->types->pppLClosureT);

  // Load pointer to LClosure**
  llvm::Instruction *ppLuaClosure = def->builder->CreateLoad(pppLuaClosure);
  ppLuaClosure->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_CallInfo_funcT);

  // Load pointer to LClosure*
  llvm::Instruction *cl_ptr = def->builder->CreateLoad(ppLuaClosure);
  cl_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                      def->types->tbaa_CallInfo_func_LClosureT);

  return cl_ptr;
}

// Retrieve the proto->sizep
// sizep is a count of the number of closures defined
// in the function
llvm::Instruction *RaviCodeGenerator::emit_load_proto_sizep(
    RaviFunctionDef *def) {
  llvm::Value *psize_ptr = emit_gep(def, "sizep", def->proto_ptr, 0, 10);
  llvm::Instruction *psize = def->builder->CreateLoad(psize_ptr);
  psize->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_Proto_sizepT);
  return psize;
}

// Updates the savedpc pointer in the call frame
// The savedpc is unimportant for the JIT but it is relied upon
// by the debug interface. So we need to set this in order for the
// debug api to work. Rather than setting it on every bytecode instruction
// we have a dual approach. By default the JIT code only sets this prior to
// function calls - this enables better stack traces for example, and ad-hoc
// calls to debug api. An optional setting in the JIT compiler also
// enables this to be updated per bytecode instruction - this is only
// required if someone wishes to set a line hook. The second option
// is very expensive and will inhibit optimizations, hence it is optional
// See issue #15
void RaviCodeGenerator::emit_update_savedpc(RaviFunctionDef *def, int pc) {
  // Get proto->code
  llvm::Value *proto_code_ptr = emit_gep(def, "code", def->proto_ptr, 0, 15);
  llvm::Instruction *code_ptr = def->builder->CreateLoad(proto_code_ptr);
  code_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_Proto_codeT);
  // Need to set savedpc to the next instruction (pc+1) rather than current as
  // that is
  // what the VM does in interpreted mode
  llvm::Value *code_offset_ptr = emit_array_get(def, code_ptr, pc + 1);
  llvm::Value *savedpc_ptr = emit_gep(def, "savedpc", def->ci_val, 0, 4, 1);
  llvm::Instruction *ins =
      def->builder->CreateStore(code_offset_ptr, savedpc_ptr);
  ins->setMetadata(llvm::LLVMContext::MD_tbaa,
                   def->types->tbaa_CallInfo_savedpcT);
}

// Generate code to reload the function's stack 'base'
// pointer - the VM needs to do this after any bytecode that
// may invoke a Lua function and thereby resize the stack; which
// would invalidate the current 'base' pointer
void RaviCodeGenerator::emit_load_base(RaviFunctionDef *def) {
  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                        def->types->tbaa_luaState_ci_baseT);
  def->base_ptr = base_ptr;
}

// emit code to obtain address of register at location A
llvm::Value *RaviCodeGenerator::emit_gep_register(RaviFunctionDef *def, int A) {
  llvm::Value *dest;
  if (A == 0) {
    // If A is 0 we can use the base pointer which is &base[0]
    dest = def->base_ptr;
  }
  else {
    // emit &base[A]
    dest = emit_array_get(def, def->base_ptr, A);
  }
  return dest;
}

// Emit code to obtain address of a constant
llvm::Value *RaviCodeGenerator::emit_gep_constant(RaviFunctionDef *def,
                                                  int Bx) {
  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr;
  llvm::Value *src;
  if (Bx == 0) {
    // If Bx is 0 we can use the base pointer which is &k[0]
    src = k_ptr;
  }
  else {
    // emit &k[Bx]
    src = emit_array_get(def, k_ptr, Bx);
  }
  return src;
}

// Loads the value at register or constant at location B -
// if the value is an integer constant
// then returns a constant literal
llvm::Value *RaviCodeGenerator::emit_load_register_or_constant_i(
    RaviFunctionDef *def, int B) {
  if (ISK(B) && def->p->k[INDEXK(B)].tt_ == LUA_TNUMINT) {
    TValue *Konst = &def->p->k[INDEXK(B)];
    return llvm::ConstantInt::get(def->types->lua_IntegerT, Konst->value_.i);
  }
  else {
    llvm::Value *rb = emit_gep_register_or_constant(def, B);
    return emit_load_reg_i(def, rb);
  }
}

// Loads the value at register B - if the value is an number constant
// then returns a constant literal
llvm::Value *RaviCodeGenerator::emit_load_register_or_constant_n(
    RaviFunctionDef *def, int B) {
  if (ISK(B) && def->p->k[INDEXK(B)].tt_ == LUA_TNUMFLT) {
    TValue *Konst = &def->p->k[INDEXK(B)];
    return llvm::ConstantFP::get(def->types->lua_NumberT, Konst->value_.n);
  }
  else {
    llvm::Value *rb = emit_gep_register_or_constant(def, B);
    return emit_load_reg_n(def, rb);
  }
}

// emit code to load the lua_Number value from register (TValue)
llvm::Instruction *RaviCodeGenerator::emit_load_reg_n(RaviFunctionDef *def,
                                                      llvm::Value *rb) {
  llvm::Value *rb_n = def->builder->CreateBitCast(rb, def->types->plua_NumberT);
  // llvm::Value *rb_n = emit_gep(def, "value.value_.n", rb, 0, 0, 0);
  llvm::Instruction *lhs = def->builder->CreateLoad(rb_n);
  lhs->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  return lhs;
}

// emit code to load the lua_Integer value from register (TValue)
llvm::Instruction *RaviCodeGenerator::emit_load_reg_i(RaviFunctionDef *def,
                                                      llvm::Value *rb) {
  llvm::Value *rb_n =
      def->builder->CreateBitCast(rb, def->types->plua_IntegerT);
  llvm::Instruction *lhs = def->builder->CreateLoad(rb_n);
  lhs->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  return lhs;
}

// emit code to load the boolean value from register (TValue)
llvm::Instruction *RaviCodeGenerator::emit_load_reg_b(RaviFunctionDef *def,
                                                      llvm::Value *rb) {
  llvm::Value *rb_n = def->builder->CreateBitCast(rb, def->types->C_pintT);
  llvm::Instruction *lhs = def->builder->CreateLoad(rb_n);
  lhs->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  return lhs;
}

// emit code to load the table value from register (TValue)
llvm::Instruction *RaviCodeGenerator::emit_load_reg_h(RaviFunctionDef *def,
                                                      llvm::Value *rb) {
  llvm::Value *rb_h = def->builder->CreateBitCast(rb, def->types->ppTableT);
  llvm::Instruction *h = def->builder->CreateLoad(rb_h);
  h->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_hT);
  return h;
}

// Gets the size of the hash table
// This is the sizenode() macro in lobject.h
llvm::Value *RaviCodeGenerator::emit_table_get_hashsize(RaviFunctionDef *def,
                                                        llvm::Value *t) {
  // Obtain the lsizenode  of the hash table
  llvm::Value *lsizenode_ptr = emit_gep(def, "lsizenode", t, 0, 4);
  llvm::Instruction *lsizenode = def->builder->CreateLoad(lsizenode_ptr);
  lsizenode->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_Table_lsizenode);
  // convert to integer (lsizenode is a byte)
  llvm::Value *intsize =
      def->builder->CreateZExt(lsizenode, def->types->C_intT);
  // #define twoto(x)        (1<<(x))
  // #define sizenode(t)     (twoto((t)->lsizenode))
  llvm::Value *size = def->builder->CreateShl(def->types->kInt[1], intsize);
  return size;
}

// Gets the location of the hash node for given string key
// return value is the offset into the node array
llvm::Value *RaviCodeGenerator::emit_table_get_hashstr(RaviFunctionDef *def,
                                                       llvm::Value *table,
                                                       TString *key) {
#if RAVI_USE_NEWHASH
  unsigned int hash = key->hash;
  llvm::Value *hmask_ptr = emit_gep(def, "hmask", table, 0, 12);
  llvm::Instruction *hmask = def->builder->CreateLoad(hmask_ptr);
  hmask->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_Table_hmask);
  llvm::Value *offset = def->builder->CreateAnd(
      llvm::ConstantInt::get(def->types->C_intT, hash), hmask);
#else
  llvm::Value *size = emit_table_get_hashsize(def, table);
  unsigned int hash = key->hash;
  // #define lmod(s,size) (cast(int, (s) & ((size)-1)))
  llvm::Value *sizeminusone =
      def->builder->CreateNSWSub(size, def->types->kInt[1]);
  llvm::Value *offset = def->builder->CreateAnd(
      llvm::ConstantInt::get(def->types->C_intT, hash), sizeminusone);
#endif
  return offset;
}

// Gets access to the Table's node array (t->node)
llvm::Value *RaviCodeGenerator::emit_table_get_nodearray(RaviFunctionDef *def,
                                                         llvm::Value *table) {
  // Get access to the node array
  llvm::Value *node_ptr = emit_gep(def, "node", table, 0, 7);
  llvm::Instruction *node = def->builder->CreateLoad(node_ptr);
  node->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_Table_node);
  return node;
}

// Given a pointer to table's node array (node = t->node) and
// the location of the hashed key (index), this method retrieves the
// type of the value stored at the node - return value is of type int
// and is the type information stored in TValue->tt field.
llvm::Value *RaviCodeGenerator::emit_table_get_keytype(RaviFunctionDef *def,
                                                       llvm::Value *node,
                                                       llvm::Value *index) {
  llvm::Value *ktype_ptr = emit_gep(def, "keytype", node, index, 1, 1);
  llvm::Instruction *ktype = def->builder->CreateLoad(ktype_ptr);
  ktype->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  return ktype;
}

// Given a pointer to table's node array (node = t->node) and
// the location of the hashed key (index), this method retrieves the
// the string value stored at the node - return value is of type TString*
llvm::Value *RaviCodeGenerator::emit_table_get_strkey(RaviFunctionDef *def,
                                                      llvm::Value *node,
                                                      llvm::Value *index) {
  llvm::Value *value_ptr = emit_gep(def, "keyvalue", node, index, 1, 0);
  llvm::Value *sptr =
      def->builder->CreateBitCast(value_ptr, def->types->ppTStringT);
  llvm::Instruction *keyvalue = def->builder->CreateLoad(sptr);
  keyvalue->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_ppointerT);
  return keyvalue;
}

// Given a pointer to table's node array (node = t->node) and
// the location of the hashed key (index), this method retrieves the
// the pointer to value stored at the node - return value is of type TValue*
llvm::Value *RaviCodeGenerator::emit_table_get_value(RaviFunctionDef *def,
                                                     llvm::Value *node,
                                                     llvm::Value *index) {
  return emit_gep(def, "nodeval", node, index, 0);
}

// Gets the size of the table's array part
llvm::Value *RaviCodeGenerator::emit_table_get_arraysize(RaviFunctionDef *def,
                                                         llvm::Value *table) {
  // Obtain the lsizenode  of the hash table
  llvm::Value *sizearray_ptr = emit_gep(def, "sizearray", table, 0, 5);
  llvm::Instruction *sizearray = def->builder->CreateLoad(sizearray_ptr);
  sizearray->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_Table_sizearray);
  return sizearray;
}

llvm::Value *RaviCodeGenerator::emit_table_get_array(RaviFunctionDef *def,
                                                     llvm::Value *table) {
  // Get access to the node array
  llvm::Value *array_ptr = emit_gep(def, "array", table, 0, 6);
  llvm::Instruction *arry = def->builder->CreateLoad(array_ptr);
  arry->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_Table_array);
  return arry;
}

llvm::Instruction *RaviCodeGenerator::emit_load_reg_s(RaviFunctionDef *def,
                                                      llvm::Value *rb) {
  llvm::Value *rb_s = def->builder->CreateBitCast(rb, def->types->ppTStringT);
  llvm::Instruction *s = def->builder->CreateLoad(rb_s);
  // Following TBAA is okay as the field type is a pointer
  s->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_hT);
  return s;
}

llvm::Instruction *RaviCodeGenerator::emit_load_reg_h_floatarray(
    RaviFunctionDef *def, llvm::Instruction *h) {
  llvm::Value *data_ptr = emit_gep(def, "data_ptr", h, 0, 11, 0);
  llvm::Value *darray_ptr =
      def->builder->CreateBitCast(data_ptr, def->types->pplua_NumberT);
  llvm::Instruction *darray = def->builder->CreateLoad(darray_ptr);
  darray->setMetadata(llvm::LLVMContext::MD_tbaa,
                      def->types->tbaa_RaviArray_dataT);
  return darray;
}

llvm::Instruction *RaviCodeGenerator::emit_load_reg_h_intarray(
    RaviFunctionDef *def, llvm::Instruction *h) {
  llvm::Value *data_ptr = emit_gep(def, "data_ptr", h, 0, 11, 0);
  llvm::Value *darray_ptr =
      def->builder->CreateBitCast(data_ptr, def->types->pplua_IntegerT);
  llvm::Instruction *darray = def->builder->CreateLoad(darray_ptr);
  darray->setMetadata(llvm::LLVMContext::MD_tbaa,
                      def->types->tbaa_RaviArray_dataT);
  return darray;
}

void RaviCodeGenerator::emit_store_reg_n(RaviFunctionDef *def,
                                         llvm::Value *result,
                                         llvm::Value *dest_ptr) {
  llvm::Value *ra_n =
      def->builder->CreateBitCast(dest_ptr, def->types->plua_NumberT);
  // llvm::Value *ra_n = emit_gep(def, "value.value_.n", dest_ptr, 0, 0, 0);
  llvm::Instruction *store = def->builder->CreateStore(result, ra_n);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
}

void RaviCodeGenerator::emit_store_reg_n_withtype(RaviFunctionDef *def,
                                                  llvm::Value *result,
                                                  llvm::Value *dest_ptr) {
  emit_store_reg_n(def, result, dest_ptr);
  emit_store_type_(def, dest_ptr, LUA_TNUMFLT);
}

void RaviCodeGenerator::emit_store_reg_i(RaviFunctionDef *def,
                                         llvm::Value *result,
                                         llvm::Value *dest_ptr) {
  llvm::Value *ra_n =
      def->builder->CreateBitCast(dest_ptr, def->types->plua_IntegerT);
  llvm::Instruction *store = def->builder->CreateStore(result, ra_n);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
}

void RaviCodeGenerator::emit_store_reg_i_withtype(RaviFunctionDef *def,
                                                  llvm::Value *result,
                                                  llvm::Value *dest_ptr) {
  emit_store_reg_i(def, result, dest_ptr);
  emit_store_type_(def, dest_ptr, LUA_TNUMINT);
}

void RaviCodeGenerator::emit_store_reg_b(RaviFunctionDef *def,
                                         llvm::Value *result,
                                         llvm::Value *dest_ptr) {
  llvm::Value *ra_n =
      def->builder->CreateBitCast(dest_ptr, def->types->C_pintT);
  llvm::Instruction *store = def->builder->CreateStore(result, ra_n);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
}

void RaviCodeGenerator::emit_store_reg_b_withtype(RaviFunctionDef *def,
                                                  llvm::Value *result,
                                                  llvm::Value *dest_ptr) {
  emit_store_reg_b(def, result, dest_ptr);
  emit_store_type_(def, dest_ptr, LUA_TBOOLEAN);
}

void RaviCodeGenerator::emit_store_type_(RaviFunctionDef *def,
                                         llvm::Value *value, int type) {
  lua_assert(type == LUA_TNUMFLT || type == LUA_TNUMINT ||
             type == LUA_TBOOLEAN || type == LUA_TNIL);
  llvm::Value *desttype = emit_gep(def, "dest.tt", value, 0, 1);
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

llvm::Value *RaviCodeGenerator::emit_is_value_of_type(RaviFunctionDef *def,
                                                      llvm::Value *value_type,
                                                      LuaTypeCode lua_type,
                                                      const char *varname) {
  return def->builder->CreateICmpEQ(value_type, def->types->kInt[int(lua_type)],
                                    varname);
}

llvm::Value *RaviCodeGenerator::emit_is_not_value_of_type(
    RaviFunctionDef *def, llvm::Value *value_type, LuaTypeCode lua_type,
    const char *varname) {
  return def->builder->CreateICmpNE(value_type, def->types->kInt[int(lua_type)],
                                    varname);
}

// Compare without variants i.e. ttnov(value_type) == lua_type
llvm::Value *RaviCodeGenerator::emit_is_not_value_of_type_class(
    RaviFunctionDef *def, llvm::Value *value_type, LuaTypeCode lua_type,
    const char *varname) {
  llvm::Value *novariant_type =
      def->builder->CreateAnd(value_type, def->types->kInt[0x0F]);
  return def->builder->CreateICmpNE(novariant_type,
                                    def->types->kInt[int(lua_type)], varname);
}

llvm::Instruction *RaviCodeGenerator::emit_load_ravi_arraytype(
    RaviFunctionDef *def, llvm::Value *value) {
  llvm::Value *tt_ptr = emit_gep(def, "raviarray.type_ptr", value, 0, 11, 3);
  llvm::Instruction *tt = def->builder->CreateLoad(tt_ptr, "raviarray.type");
  tt->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_RaviArray_typeT);
  return tt;
}

llvm::Instruction *RaviCodeGenerator::emit_load_ravi_arraylength(
    RaviFunctionDef *def, llvm::Value *value) {
  llvm::Value *tt_ptr = emit_gep(def, "raviarray.len_ptr", value, 0, 11, 1);
  llvm::Instruction *tt = def->builder->CreateLoad(tt_ptr, "raviarray.len");
  tt->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_RaviArray_lenT);
  return tt;
}

// Tests following:
// ((t) == NULL) || ((t)->flags & (1u<<e)) != 0)
llvm::Value *RaviCodeGenerator::emit_table_no_metamethod(RaviFunctionDef *def,
                                                         llvm::Value *table,
                                                         TMS event) {
  // Is metatable NULL?
  llvm::Value *metatable_ptr =
      emit_gep(def, "table.metatable_ptr", table, 0, 9);
  llvm::Instruction *metatable =
      def->builder->CreateLoad(metatable_ptr, "table.metatable");
  metatable->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_Table_metatable);
  llvm::Value *null_constant =
      llvm::ConstantPointerNull::get(def->types->pTableT);
  llvm::Value *is_null = def->builder->CreateICmpEQ(
      def->builder->CreatePtrToInt(null_constant, def->types->C_intptr_t),
      def->builder->CreatePtrToInt(metatable, def->types->C_intptr_t));

  // Is metatable->flags & (1<<event) set?
  llvm::Value *flags_ptr = emit_gep(def, "table.flags_ptr", metatable, 0, 3);
  llvm::Instruction *flags = def->builder->CreateLoad(flags_ptr, "table.flags");
  flags->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_Table_flags);
  llvm::Value *no_event = def->builder->CreateICmpNE(
      def->builder->CreateAnd(
          flags, llvm::ConstantInt::get(def->types->lu_byteT, 1u << event)),
      llvm::ConstantInt::get(def->types->lu_byteT, 0));

  llvm::Value *metaabsent =
      def->builder->CreateOr(is_null, no_event, "metatable.isnull.or.no.event");
  return metaabsent;
}

// Store lua_Number or lua_Integer
llvm::Instruction *RaviCodeGenerator::emit_store_local_n(RaviFunctionDef *def,
                                                         llvm::Value *src,
                                                         llvm::Value *dest) {
  llvm::Instruction *ins = def->builder->CreateStore(src, dest);
  ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_plonglongT);
  return ins;
}

// Load lua_Number or lua_Integer
llvm::Instruction *RaviCodeGenerator::emit_load_local_n(RaviFunctionDef *def,
                                                        llvm::Value *src) {
  llvm::Instruction *ins = def->builder->CreateLoad(src);
  ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_plonglongT);
  return ins;
}

// Store int
llvm::Instruction *RaviCodeGenerator::emit_store_local_int(RaviFunctionDef *def,
                                                           llvm::Value *src,
                                                           llvm::Value *dest) {
  llvm::Instruction *ins = def->builder->CreateStore(src, dest);
  ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_pintT);
  return ins;
}

// Load int
llvm::Instruction *RaviCodeGenerator::emit_load_local_int(RaviFunctionDef *def,
                                                          llvm::Value *src) {
  llvm::Instruction *ins = def->builder->CreateLoad(src);
  ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_pintT);
  return ins;
}

// emit code to obtain address of register or constant at location B
llvm::Value *RaviCodeGenerator::emit_gep_register_or_constant(
    RaviFunctionDef *def, int B) {
  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr;
  llvm::Value *rb;
  // Get pointer to register B
  llvm::Value *base_or_k = ISK(B) ? k_ptr : def->base_ptr;
  int b = ISK(B) ? INDEXK(B) : B;
  if (b == 0) { rb = base_or_k; }
  else {
    rb = emit_array_get(def, base_or_k, b);
  }
  return rb;
}

#if 0
// Test if ci->jistatus is true, ci is of type CallInfo
llvm::Value *RaviCodeGenerator::emit_is_jit_call(RaviFunctionDef *def,
                                                 llvm::Value *ci) {
  // Get pointer to ci->jitstatus
  llvm::Value *ci_jitstatus_ptr = emit_gep(def, "ci_jit_status_ptr", ci, 0, 9);

  // Load ci->jitstatus
  llvm::Instruction *ci_jitstatus = def->builder->CreateLoad(ci_jitstatus_ptr);
  ci_jitstatus->setMetadata(llvm::LLVMContext::MD_tbaa,
                            def->types->tbaa_CallInfo_jitstatusT);

  return def->builder->CreateICmpNE(
      ci_jitstatus, llvm::ConstantInt::get(def->types->lu_byteT, 0),
      "jit_call");
}

// Return (ci->callstatus & CIST_LUA) != 0
llvm::Value *RaviCodeGenerator::emit_ci_is_Lua(RaviFunctionDef *def,
                                               llvm::Value *ci) {
  // Get pointer to ci->callstatus
  llvm::Value *ci_callstatus_ptr =
      emit_gep(def, "ci_call_status_ptr", ci, 0, 7);

  // Load ci->callstatus
  llvm::Instruction *ci_callstatus =
      def->builder->CreateLoad(ci_callstatus_ptr, "ci_call_status");
  ci_callstatus->setMetadata(llvm::LLVMContext::MD_tbaa,
                             def->types->tbaa_CallInfo_callstatusT);

  llvm::Value *isLua = def->builder->CreateAnd(
      ci_callstatus, llvm::ConstantInt::get(def->types->C_shortT, CIST_LUA),
      "isLua");
  return def->builder->CreateICmpNE(
      isLua, llvm::ConstantInt::get(def->types->C_shortT, 0));
}
#endif

llvm::Value *RaviCodeGenerator::emit_load_ci(RaviFunctionDef *def) {
  llvm::Value *L_ci = emit_gep(def, "L_ci", def->L, 0, 6);

  llvm::Instruction *ci_val = def->builder->CreateLoad(L_ci);
  ci_val->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_CallInfoT);

  return ci_val;
}

// L->top = ci->top
void RaviCodeGenerator::emit_refresh_L_top(RaviFunctionDef *def,
                                           llvm::Value *ci_val) {
  // Get pointer to ci->top
  llvm::Value *citop = emit_gep(def, "ci_top", ci_val, 0, 1);

  // Load ci->top
  llvm::Instruction *citop_val = def->builder->CreateLoad(citop);
  citop_val->setMetadata(llvm::LLVMContext::MD_tbaa,
                         def->types->tbaa_CallInfo_topT);

  // Get L->top
  llvm::Value *top = emit_gep(def, "L_top", def->L, 0, 4);

  // Assign ci>top to L->top
  auto ins = def->builder->CreateStore(citop_val, top);
  ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_topT);
}

llvm::Value *RaviCodeGenerator::emit_gep_L_top(RaviFunctionDef *def) {
  // Get pointer to L->top
  return emit_gep(def, "L.top", def->L, 0, 4);
}

// L->top = R(B)
void RaviCodeGenerator::emit_set_L_top_toreg(RaviFunctionDef *def, int B) {
  // Get pointer to register at R(B)
  llvm::Value *ptr = emit_gep_register(def, B);
  // Get pointer to L->top
  llvm::Value *top = emit_gep_L_top(def);
  // Assign to L->top
  llvm::Instruction *ins = def->builder->CreateStore(ptr, top);
  ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_topT);
}

llvm::Instruction *RaviCodeGenerator::emit_tointeger(RaviFunctionDef *def,
                                                     llvm::Value *reg) {
  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *value =
      TmpB.CreateAlloca(def->types->lua_IntegerT, nullptr, "value");
  llvm::Value *reg_type = emit_load_type(def, reg);

  // Is reg an integer?
  llvm::Value *cmp1 =
      emit_is_value_of_type(def, reg_type, LUA__TNUMINT, "reg.is.integer");

  llvm::BasicBlock *convert_reg =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.reg");
  llvm::BasicBlock *copy_reg =
      llvm::BasicBlock::Create(def->jitState->context(), "copy.reg");
  llvm::BasicBlock *load_val =
      llvm::BasicBlock::Create(def->jitState->context(), "load.val");
  llvm::BasicBlock *failed_conversion = llvm::BasicBlock::Create(
      def->jitState->context(), "if.conversion.failed");

  // If reg is integer then copy reg, else convert reg
  auto brinst1 = def->builder->CreateCondBr(cmp1, copy_reg, convert_reg);
  attach_branch_weights(def, brinst1, 100, 0);

  // Convert RB
  def->f->getBasicBlockList().push_back(convert_reg);
  def->builder->SetInsertPoint(convert_reg);

  llvm::Value *var_isint =
      CreateCall2(def->builder, def->luaV_tointegerF, reg, value);
  llvm::Value *tobool = def->builder->CreateICmpEQ(
      var_isint, def->types->kInt[0], "conversion.failed");

  // Did conversion fail?
  def->builder->CreateCondBr(tobool, failed_conversion, load_val);

  // Conversion failed, so raise error
  def->f->getBasicBlockList().push_back(failed_conversion);
  def->builder->SetInsertPoint(failed_conversion);
  emit_raise_lua_error(def, "integer expected");
  def->builder->CreateBr(load_val);

  // Conversion OK
  def->f->getBasicBlockList().push_back(copy_reg);
  def->builder->SetInsertPoint(copy_reg);

  llvm::Value *i = emit_load_reg_i(def, reg);
  emit_store_local_int(def, i, value);
  def->builder->CreateBr(load_val);

  def->f->getBasicBlockList().push_back(load_val);
  def->builder->SetInsertPoint(load_val);

  return emit_load_local_int(def, value);
}

llvm::Instruction *RaviCodeGenerator::emit_tofloat(RaviFunctionDef *def,
                                                   llvm::Value *reg) {
  llvm::IRBuilder<> TmpB(def->entry, def->entry->begin());
  llvm::Value *value =
      TmpB.CreateAlloca(def->types->lua_NumberT, nullptr, "value");
  llvm::Value *reg_type = emit_load_type(def, reg);

  // Is reg an number?
  llvm::Value *cmp1 =
      emit_is_value_of_type(def, reg_type, LUA__TNUMFLT, "reg.is.float");

  llvm::BasicBlock *convert_reg =
      llvm::BasicBlock::Create(def->jitState->context(), "convert.reg");
  llvm::BasicBlock *copy_reg =
      llvm::BasicBlock::Create(def->jitState->context(), "copy.reg");
  llvm::BasicBlock *load_val =
      llvm::BasicBlock::Create(def->jitState->context(), "load.val");
  llvm::BasicBlock *failed_conversion = llvm::BasicBlock::Create(
      def->jitState->context(), "if.conversion.failed");

  // If reg is integer then copy reg, else convert reg
  auto brinst1 = def->builder->CreateCondBr(cmp1, copy_reg, convert_reg);
  attach_branch_weights(def, brinst1, 100, 0);

  // Convert RB
  def->f->getBasicBlockList().push_back(convert_reg);
  def->builder->SetInsertPoint(convert_reg);

  llvm::Value *var_isfloat =
      CreateCall2(def->builder, def->luaV_tonumberF, reg, value);
  llvm::Value *tobool = def->builder->CreateICmpEQ(
      var_isfloat, def->types->kInt[0], "conversion.failed");

  // Did conversion fail?
  def->builder->CreateCondBr(tobool, failed_conversion, load_val);

  // Conversion failed, so raise error
  def->f->getBasicBlockList().push_back(failed_conversion);
  def->builder->SetInsertPoint(failed_conversion);
  emit_raise_lua_error(def, "number expected");
  def->builder->CreateBr(load_val);

  // Conversion OK
  def->f->getBasicBlockList().push_back(copy_reg);
  def->builder->SetInsertPoint(copy_reg);

  llvm::Value *i = emit_load_reg_n(def, reg);
  emit_store_local_n(def, i, value);
  def->builder->CreateBr(load_val);

  def->f->getBasicBlockList().push_back(load_val);
  def->builder->SetInsertPoint(load_val);

  return emit_load_local_n(def, value);
}

// (int)(L->top - ra)
llvm::Value *RaviCodeGenerator::emit_num_stack_elements(RaviFunctionDef *def,
                                                        llvm::Value *ra) {
  llvm::Value *L_top = emit_gep_L_top(def);
  llvm::Instruction *top_ptr = def->builder->CreateLoad(L_top, "L_top_ptr");
  llvm::Value *top_asint = def->builder->CreatePtrToInt(
      top_ptr, def->types->C_intptr_t, "L_top_ptr_as_int");
  llvm::Value *ra_asint =
      def->builder->CreatePtrToInt(ra, def->types->C_intptr_t, "ra_ptr_as_int");
  llvm::Value *diff =
      def->builder->CreateSub(top_asint, ra_asint, "L_top_minus_ra");
  // Because each value object is 16 bytes we need to divide by 16
  llvm::Value *n_elements = def->builder->CreateSDiv(
      diff, llvm::ConstantInt::get(def->types->C_intptr_t, sizeof(TValue)),
      "num_tvalue_elements", true);
  return def->builder->CreateTrunc(n_elements, def->types->C_intT,
                                   "num_stack_elements");
}

// Check if we can compile
// The cases we can compile will increase over time
bool RaviCodeGenerator::canCompile(Proto *p) {
  if (p->ravi_jit.jit_status == RAVI_JIT_CANT_COMPILE) return false;
  if (jitState_ == nullptr) {
    p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
    return false;
  }
  // const Instruction *code = p->code;
  // int pc, n = p->sizecode;
  // Loop over the byte codes; as Lua compiler inserts
  // an extra RETURN op we need to ignore the last op
  // for (pc = 0; pc < n; pc++) {
  //  Instruction i = code[pc];
  //  OpCode o = GET_OPCODE(i);
  //  if (o == OP_EXTRAARG)
  //    return false;
  //  else if (o == OP_RAVI_UNMF || o == OP_RAVI_UNMI) {
  //    fprintf(stderr, "Unexpected bytecode %d\n", o);
  //    abort();
  //  }
  //}
  return true;
}

std::unique_ptr<RaviJITFunction> RaviCodeGenerator::create_function(
    Proto *p, std::shared_ptr<RaviJITModule> module, llvm::IRBuilder<> &builder,
    RaviFunctionDef *def) {
  LuaLLVMTypes *types = jitState_->types();

  std::unique_ptr<ravi::RaviJITFunction> func =
      std::unique_ptr<RaviJITFunction>(new RaviJITFunction(
          &p->ravi_jit.jit_function, module, types->jitFunctionT,
          llvm::Function::ExternalLinkage, unique_function_name()));
  if (!func) return func;

  llvm::Function *mainFunc = func->function();
  llvm::BasicBlock *entry =
      llvm::BasicBlock::Create(jitState_->context(), "entry", mainFunc);
  builder.SetInsertPoint(entry);

  auto argiter = mainFunc->arg_begin();
  llvm::Value *arg1 = &(*argiter);
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

void RaviCodeGenerator::emit_dump_stack(RaviFunctionDef *def, const char *str) {
  CreateCall2(def->builder, def->ravi_dump_stackF, def->L,
              def->builder->CreateGlobalStringPtr(str));
}

void RaviCodeGenerator::emit_dump_stacktop(RaviFunctionDef *def,
                                           const char *str) {
  CreateCall2(def->builder, def->ravi_dump_stacktopF, def->L,
              def->builder->CreateGlobalStringPtr(str));
}

// Emit a call to ravi_debug_trace() function.
// This function will set savedpc and also invoke
// "line" hook if defined
bool RaviCodeGenerator::emit_debug_trace(RaviFunctionDef *def, int opCode,
                                         int pc) {
  if (def->jitState->is_tracehook_enabled()) {
    CreateCall3(def->builder, def->ravi_debug_traceF, def->L,
                llvm::ConstantInt::get(def->types->C_intT, opCode),
                llvm::ConstantInt::get(def->types->C_intT, pc));
    return true;
  }
  return false;
}

void RaviCodeGenerator::emit_raise_lua_error(RaviFunctionDef *def,
                                             const char *str) {
  CreateCall2(def->builder, def->luaG_runerrorF, def->L,
              def->builder->CreateGlobalStringPtr(str));
}

void RaviCodeGenerator::debug_printf(RaviFunctionDef *def, const char *str) {
  CreateCall1(def->builder, def->printfFunc,
              def->builder->CreateGlobalStringPtr(str));
}

void RaviCodeGenerator::debug_printf1(RaviFunctionDef *def, const char *str,
                                      llvm::Value *arg1) {
  CreateCall2(def->builder, def->printfFunc,
              def->builder->CreateGlobalStringPtr(str), arg1);
}

void RaviCodeGenerator::debug_printf2(RaviFunctionDef *def, const char *str,
                                      llvm::Value *arg1, llvm::Value *arg2) {
  CreateCall3(def->builder, def->printfFunc,
              def->builder->CreateGlobalStringPtr(str), arg1, arg2);
}

void RaviCodeGenerator::debug_printf3(RaviFunctionDef *def, const char *str,
                                      llvm::Value *arg1, llvm::Value *arg2,
                                      llvm::Value *arg3) {
  CreateCall4(def->builder, def->printfFunc,
              def->builder->CreateGlobalStringPtr(str), arg1, arg2, arg3);
}

void RaviCodeGenerator::debug_printf4(RaviFunctionDef *def, const char *str,
                                      llvm::Value *arg1, llvm::Value *arg2,
                                      llvm::Value *arg3, llvm::Value *arg4) {
  CreateCall5(def->builder, def->printfFunc,
              def->builder->CreateGlobalStringPtr(str), arg1, arg2, arg3, arg4);
}

void RaviCodeGenerator::emit_extern_declarations(RaviFunctionDef *def) {
  // Add extern declarations for Lua functions that we need to call
  def->luaT_trybinTMF = def->raviF->addExternFunction(
      def->types->luaT_trybinTMT, reinterpret_cast<void *>(&luaT_trybinTM),
      "luaT_trybinTM");
  def->luaD_callF = def->raviF->addExternFunction(
      def->types->luaD_callT, reinterpret_cast<void *>(&luaD_call),
      "luaD_call");
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
  def->luaG_runerrorF->setDoesNotReturn();
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
  def->luaV_settableF = def->raviF->addExternFunction(
      def->types->luaV_settableT, reinterpret_cast<void *>(&luaV_settable),
      "luaV_settable");
  def->luaV_gettableF = def->raviF->addExternFunction(
      def->types->luaV_gettableT, reinterpret_cast<void *>(&luaV_gettable),
      "luaV_gettable");
  def->luaH_getintF = def->raviF->addExternFunction(
      def->types->luaH_getintT, reinterpret_cast<void *>(&luaH_getint),
      "luaH_getint");
  def->luaH_setintF = def->raviF->addExternFunction(
      def->types->luaH_setintT, reinterpret_cast<void *>(&luaH_setint),
      "luaH_setint");
  def->luaH_getstrF = def->raviF->addExternFunction(
      def->types->luaH_getstrT, reinterpret_cast<void *>(&luaH_getstr),
      "luaH_getstr");
  def->luaH_getstrF->addFnAttr(llvm::Attribute::AttrKind::ReadOnly);
  //def->luaH_getstrF->setDoesNotAlias(1);
  //def->luaH_getstrF->setDoesNotCapture(1);
  //def->luaH_getstrF->setDoesNotAlias(2);
  //def->luaH_getstrF->setDoesNotCapture(2);
  def->luaV_finishgetF = def->raviF->addExternFunction(
      def->types->luaV_finishgetT, reinterpret_cast<void *>(&luaV_finishget),
      "luaV_finishget");

  def->raviV_op_loadnilF = def->raviF->addExternFunction(
      def->types->raviV_op_loadnilT,
      reinterpret_cast<void *>(&raviV_op_loadnil), "raviV_op_loadnil");
  def->raviV_op_newarrayintF = def->raviF->addExternFunction(
      def->types->raviV_op_newarrayintT,
      reinterpret_cast<void *>(&raviV_op_newarrayint), "raviV_op_newarrayint");
  def->raviV_op_newarrayfloatF = def->raviF->addExternFunction(
      def->types->raviV_op_newarrayfloatT,
      reinterpret_cast<void *>(&raviV_op_newarrayfloat),
      "raviV_op_newarrayfloat");
  def->raviV_op_newtableF = def->raviF->addExternFunction(
      def->types->raviV_op_newtableT,
      reinterpret_cast<void *>(&raviV_op_newtable), "raviV_op_newtable");
  def->raviV_op_setlistF = def->raviF->addExternFunction(
      def->types->raviV_op_setlistT,
      reinterpret_cast<void *>(&raviV_op_setlist), "raviV_op_setlist");
  def->luaV_modF = def->raviF->addExternFunction(
      def->types->luaV_modT, reinterpret_cast<void *>(&luaV_mod), "luaV_mod");
  def->luaV_divF = def->raviF->addExternFunction(
      def->types->luaV_divT, reinterpret_cast<void *>(&luaV_div), "luaV_div");
  def->luaV_objlenF = def->raviF->addExternFunction(
      def->types->luaV_objlenT, reinterpret_cast<void *>(&luaV_objlen),
      "luaV_objlen");
  def->luaC_upvalbarrierF = def->raviF->addExternFunction(
      def->types->luaC_upvalbarrierT,
      reinterpret_cast<void *>(&luaC_upvalbarrier_), "luaC_upvalbarrier_");
  def->raviV_op_concatF = def->raviF->addExternFunction(
      def->types->raviV_op_concatT, reinterpret_cast<void *>(&raviV_op_concat),
      "raviV_op_concat");
  def->raviV_op_closureF = def->raviF->addExternFunction(
      def->types->raviV_op_closureT,
      reinterpret_cast<void *>(&raviV_op_closure), "raviV_op_closure");
  def->raviV_op_varargF = def->raviF->addExternFunction(
      def->types->raviV_op_varargT, reinterpret_cast<void *>(&raviV_op_vararg),
      "raviV_op_vararg");
  def->raviH_set_intF = def->raviF->addExternFunction(
      def->types->raviH_set_intT, reinterpret_cast<void *>(&raviH_set_int),
      "raviH_set_int");
  def->raviH_set_floatF = def->raviF->addExternFunction(
      def->types->raviH_set_floatT, reinterpret_cast<void *>(&raviH_set_float),
      "raviH_set_float");
  def->raviV_op_shlF = def->raviF->addExternFunction(
      def->types->raviV_op_shlT, reinterpret_cast<void *>(&raviV_op_shl),
      "raviV_op_shl");
  def->raviV_op_shrF = def->raviF->addExternFunction(
      def->types->raviV_op_shrT, reinterpret_cast<void *>(&raviV_op_shr),
      "raviV_op_shr");
  def->raviV_op_borF = def->raviF->addExternFunction(
      def->types->raviV_op_borT, reinterpret_cast<void *>(&raviV_op_bor),
      "raviV_op_bor");
  def->raviV_op_bxorF = def->raviF->addExternFunction(
      def->types->raviV_op_bxorT, reinterpret_cast<void *>(&raviV_op_bxor),
      "raviV_op_bxor");
  def->raviV_op_bandF = def->raviF->addExternFunction(
      def->types->raviV_op_bandT, reinterpret_cast<void *>(&raviV_op_band),
      "raviV_op_band");
  def->raviV_op_bnotF = def->raviF->addExternFunction(
      def->types->raviV_op_bnotT, reinterpret_cast<void *>(&raviV_op_bnot),
      "raviV_op_bnot");
  def->raviV_op_addF = def->raviF->addExternFunction(
      def->types->raviV_op_addT, reinterpret_cast<void *>(&raviV_op_add),
      "raviV_op_add");
  def->raviV_op_subF = def->raviF->addExternFunction(
      def->types->raviV_op_subT, reinterpret_cast<void *>(&raviV_op_sub),
      "raviV_op_sub");
  def->raviV_op_mulF = def->raviF->addExternFunction(
      def->types->raviV_op_mulT, reinterpret_cast<void *>(&raviV_op_mul),
      "raviV_op_mul");
  def->raviV_op_divF = def->raviF->addExternFunction(
      def->types->raviV_op_divT, reinterpret_cast<void *>(&raviV_op_div),
      "raviV_op_div");
  def->raviV_op_setupvaliF = def->raviF->addExternFunction(
      def->types->raviV_op_setupvaliT,
      reinterpret_cast<void *>(&raviV_op_setupvali), "raviV_op_setupvali");
  def->raviV_op_setupvalfF = def->raviF->addExternFunction(
      def->types->raviV_op_setupvalfT,
      reinterpret_cast<void *>(&raviV_op_setupvalf), "raviV_op_setupvalf");
  def->raviV_op_setupvalaiF = def->raviF->addExternFunction(
      def->types->raviV_op_setupvalaiT,
      reinterpret_cast<void *>(&raviV_op_setupvalai), "raviV_op_setupvalai");
  def->raviV_op_setupvalafF = def->raviF->addExternFunction(
      def->types->raviV_op_setupvalafT,
      reinterpret_cast<void *>(&raviV_op_setupvalaf), "raviV_op_setupvalaf");
  def->raviV_op_setupvaltF = def->raviF->addExternFunction(
      def->types->raviV_op_setupvaltT,
      reinterpret_cast<void *>(&raviV_op_setupvalt), "raviV_op_setupvalt");
  def->raviV_settable_sskeyF = def->raviF->addExternFunction(
      def->types->raviV_settable_sskeyT,
      reinterpret_cast<void *>(&raviV_settable_sskey), "raviV_settable_sskey");
  def->raviV_gettable_sskeyF = def->raviF->addExternFunction(
      def->types->raviV_gettable_sskeyT,
      reinterpret_cast<void *>(&raviV_gettable_sskey), "raviV_gettable_sskey");
  def->raviV_settable_iF = def->raviF->addExternFunction(
	  def->types->raviV_settable_iT,
	  reinterpret_cast<void *>(&raviV_settable_i), "raviV_settable_i");
  def->raviV_gettable_iF = def->raviF->addExternFunction(
	  def->types->raviV_gettable_iT,
	  reinterpret_cast<void *>(&raviV_gettable_i), "raviV_gettable_i");

  def->ravi_dump_valueF = def->raviF->addExternFunction(
      def->types->ravi_dump_valueT, reinterpret_cast<void *>(&ravi_dump_value),
      "ravi_dump_value");
  def->ravi_dump_stackF = def->raviF->addExternFunction(
      def->types->ravi_dump_stackT, reinterpret_cast<void *>(&ravi_dump_stack),
      "ravi_dump_stack");
  def->ravi_dump_stacktopF = def->raviF->addExternFunction(
      def->types->ravi_dump_stacktopT,
      reinterpret_cast<void *>(&ravi_dump_stacktop), "ravi_dump_stacktop");
  def->ravi_debug_traceF = def->raviF->addExternFunction(
      def->types->ravi_debug_traceT,
      reinterpret_cast<void *>(&ravi_debug_trace), "ravi_debug_trace");

  // Create printf declaration
  std::vector<llvm::Type *> args;
  args.push_back(def->types->C_pcharT);
  // accepts a char*, is vararg, and returns int
  llvm::FunctionType *printfType =
      llvm::FunctionType::get(def->types->C_intT, args, true);
  def->printfFunc =
      def->raviF->module()->getOrInsertFunction("printf", printfType);

  // stdc fmod declaration
  args.clear();
  args.push_back(def->types->lua_NumberT);
  args.push_back(def->types->lua_NumberT);
  llvm::FunctionType *fmodType =
      llvm::FunctionType::get(def->types->lua_NumberT, args, false);
#ifdef LUA_32BITS
  def->fmodFunc = def->raviF->module()->getOrInsertFunction("fmodf", fmodType);
#else
  def->fmodFunc = def->raviF->module()->getOrInsertFunction("fmod", fmodType);
#endif
  llvm::FunctionType *powType =
      llvm::FunctionType::get(def->types->lua_NumberT, args, false);
#ifdef LUA_32BITS
  def->powFunc = def->raviF->module()->getOrInsertFunction("powf", powType);
#else
  def->powFunc = def->raviF->module()->getOrInsertFunction("pow", powType);
#endif
  // stdc floor declaration
  args.clear();
  args.push_back(def->types->lua_NumberT);
  llvm::FunctionType *floorType =
      llvm::FunctionType::get(def->types->lua_NumberT, args, false);
#ifdef LUA_32BITS
  def->floorFunc =
      def->raviF->module()->getOrInsertFunction("floorf", floorType);
#else
  def->floorFunc =
      def->raviF->module()->getOrInsertFunction("floor", floorType);
#endif
}

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
    // add the branch instruction, but we still need to create the new
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

llvm::Value *RaviCodeGenerator::emit_gep_upvals(RaviFunctionDef *def,
                                                int offset) {
  return emit_gep(def, "upvals", def->p_LClosure, 0, 6, offset);
}

llvm::Instruction *RaviCodeGenerator::emit_load_pupval(RaviFunctionDef *def,
                                                       llvm::Value *ppupval) {
  llvm::Instruction *ins = def->builder->CreateLoad(ppupval);
  ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_ppointerT);
  return ins;
}

// Load upval->v
llvm::Instruction *RaviCodeGenerator::emit_load_upval_v(
    RaviFunctionDef *def, llvm::Instruction *pupval) {
  llvm::Value *p_v = emit_gep_upval_v(def, pupval);
  llvm::Instruction *v = def->builder->CreateLoad(p_v);
  v->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_UpVal_vT);
  return v;
}

// Get &upval->v
llvm::Value *RaviCodeGenerator::emit_gep_upval_v(RaviFunctionDef *def,
                                                 llvm::Instruction *pupval) {
  return emit_gep(def, "v", pupval, 0, 0);
}

// Get &upval->value -> result is TValue *
llvm::Value *RaviCodeGenerator::emit_gep_upval_value(
    RaviFunctionDef *def, llvm::Instruction *pupval) {
  return emit_gep(def, "value", pupval, 0, 2);
}

bool RaviCodeGenerator::compile(lua_State *L, Proto *p,
                                std::shared_ptr<RaviJITModule> module,
                                ravi_compile_options_t *options) {
  if (p->ravi_jit.jit_status == RAVI_JIT_COMPILED) return true;

  bool doVerify = options ? options->verification_level != 0 : 0;
  bool omitArrayGetRangeCheck =
      options ? options->omit_array_get_range_check != 0 : 0;

  if (p->ravi_jit.jit_status != RAVI_JIT_NOT_COMPILED || !canCompile(p)) {
    // fprintf(stderr, "failed to compile!\n");
    return false;
  }

  llvm::LLVMContext &context = jitState_->context();
  llvm::IRBuilder<> builder(context);

  RaviFunctionDef definition = {0};
  RaviFunctionDef *def = &definition;

  // The Lua GC doesn't know about memory allocated by the JIT
  // compiler; this means that if lots of functions are being compiled
  // such as in the test cases, then memory usage can grow very large
  // as the GC is blissfully unaware of the actual memory in use
  // To workaround this issue we provide an option that can be used to
  // force a GC after every n compilations where n is supplied
  // by the gcstep parameter; this is not ideal as it kills performance
  // but appears to work for the test cases
  // A better solution is needed
  int gcstep = this->jitState_->get_gcstep();
  if (gcstep > 0 && (id_ % gcstep) == 0) {
    lua_unlock(L);
    lua_gc(L, LUA_GCCOLLECT, 0);
    lua_lock(L);
  }

  auto f = create_function(p, module, builder, def);
  if (!f) {
    p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;  // can't compile
    return false;
  }

  // The functions constants
  TValue *k = p->k;

  // Add extern declarations for Lua functions we need to call
  emit_extern_declarations(def);
  def->p = p;

  // Create BasicBlocks for all the jump targets in the Lua bytecode
  scan_jump_targets(def, p);

  // Get pointer to L->ci
  def->L_ci = emit_gep(def, "L_ci", def->L, 0, 6);

  // Load L->ci
  // This is the function's activation record / stack frame
  def->ci_val = builder.CreateLoad(def->L_ci);
  def->ci_val->setMetadata(llvm::LLVMContext::MD_tbaa,
                           def->types->tbaa_luaState_ciT);

  // Get pointer to function's 'base' stack location
  // 'base' is the address relative to which all the
  // function registers are located
  def->Ci_base = emit_gep(def, "base", def->ci_val, 0, 4, 0);

  // We need to get hold of the constants table
  // which is located in the function definition
  // This is the line in lvm.c that says:
  // LClosure *cl = clLvalue(ci->func)
  def->p_LClosure = emit_gep_ci_func_value_gc_asLClosure(def);

  // Get pointer to the Proto* which is cl->p
  def->proto = emit_gep(def, "Proto", def->p_LClosure, 0, 5);

  // Load pointer to proto
  def->proto_ptr = builder.CreateLoad(def->proto);
  def->proto_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                              def->types->tbaa_LClosure_pT);

  // Obtain pointer to Proto->k
  def->proto_k = emit_gep(def, "k", def->proto_ptr, 0, 14);

  // Load pointer to k
  def->k_ptr = builder.CreateLoad(def->proto_k);
  def->k_ptr->setMetadata(llvm::LLVMContext::MD_tbaa,
                          def->types->tbaa_Proto_kT);

  // emit_dump_stack(def, "Function entry-->");

  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  for (pc = 0; pc < n; pc++) {
    link_block(def, pc);
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    int A = GETARG_A(i);
    switch (op) {
      case OP_LOADK: {
        int Bx = GETARG_Bx(i);
        emit_LOADK(def, A, Bx, pc);
      } break;
      case OP_LOADKX: {
        // OP_LOADKX is followed by OP_EXTRAARG
        Instruction inst = code[++pc];
        int Ax = GETARG_Ax(inst);
        lua_assert(GET_OPCODE(inst) == OP_EXTRAARG);
        emit_LOADK(def, A, Ax, pc - 1);
      } break;

      case OP_CONCAT: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_CONCAT(def, A, B, C, pc);
      } break;
      case OP_CLOSURE: {
        int Bx = GETARG_Bx(i);
        emit_CLOSURE(def, A, Bx, pc);
      } break;
      case OP_VARARG: {
        int B = GETARG_B(i);
        emit_VARARG(def, A, B, pc);
      } break;

      case OP_LOADBOOL: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_LOADBOOL(def, A, B, C, pc + 2, pc);
      } break;
      case OP_MOVE: {
        int B = GETARG_B(i);
        emit_MOVE(def, A, B, pc);
      } break;
      case OP_RAVI_MOVEI: {
        int B = GETARG_B(i);
        emit_MOVEI(def, A, B, pc);
      } break;
      case OP_RAVI_MOVEF: {
        int B = GETARG_B(i);
        emit_MOVEF(def, A, B, pc);
      } break;
      case OP_RAVI_TOINT: {
        emit_TOINT(def, A, pc);
      } break;
      case OP_RAVI_TOFLT: {
        emit_TOFLT(def, A, pc);
      } break;
      case OP_RAVI_NEWARRAYI: {
        emit_NEWARRAYINT(def, A, pc);
      } break;
      case OP_RAVI_NEWARRAYF: {
        emit_NEWARRAYFLOAT(def, A, pc);
      } break;
      case OP_NEWTABLE: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_NEWTABLE(def, A, B, C, pc);
      } break;
      case OP_SETLIST: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        if (C == 0) {
          // OP_SETLIST is followed by OP_EXTRAARG
          Instruction inst = code[++pc];
          C = GETARG_Ax(inst);
          lua_assert(GET_OPCODE(inst) == OP_EXTRAARG);
        }
        emit_SETLIST(def, A, B, C, pc);
      } break;
      case OP_SELF:
      case OP_RAVI_SELF_SK: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        if (op == OP_RAVI_SELF_SK) { emit_SELF_SK(def, A, B, C, pc); }
        else {
          emit_SELF(def, A, B, C, pc);
        }
      } break;
      case OP_LEN: {
        int B = GETARG_B(i);
        emit_LEN(def, A, B, pc);
      } break;

      case OP_RETURN: {
        int B = GETARG_B(i);
        emit_RETURN(def, A, B, pc);
      } break;
      case OP_RAVI_EQ_II:
      case OP_RAVI_EQ_FF:
      case OP_RAVI_LT_II:
      case OP_RAVI_LT_FF:
      case OP_RAVI_LE_II:
      case OP_RAVI_LE_FF:
      case OP_LT:
      case OP_LE:
      case OP_EQ: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        OpCode compOperator = op;
        llvm::Constant *comparison_function =
            ((op == OP_EQ || op == OP_RAVI_EQ_II || op == OP_RAVI_EQ_FF)
                 ? def->luaV_equalobjF
                 : ((op == OP_LT || op == OP_RAVI_LT_II || op == OP_RAVI_LT_FF)
                        ? def->luaV_lessthanF
                        : def->luaV_lessequalF));
        // OP_EQ is followed by OP_JMP - we process this
        // along with OP_EQ
        pc++;
        i = code[pc];
        op = GET_OPCODE(i);
        lua_assert(op == OP_JMP);
        int sbx = GETARG_sBx(i);
        // j below is the jump target
        int j = sbx + pc + 1;
        emit_EQ(def, A, B, C, j, GETARG_A(i), comparison_function, compOperator,
                pc - 1);
      } break;
      case OP_TFORCALL: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        // OP_TFORCALL is followed by OP_TFORLOOP - we process this
        // along with OP_TFORCALL
        pc++;
        i = code[pc];
        op = GET_OPCODE(i);
        lua_assert(op == OP_TFORLOOP);
        int sbx = GETARG_sBx(i);
        // j below is the jump target
        int j = sbx + pc + 1;
        emit_TFORCALL(def, A, B, C, j, GETARG_A(i), pc - 1);
      } break;
      case OP_TFORLOOP: {
        int sbx = GETARG_sBx(i);
        int j = sbx + pc + 1;
        emit_TFORLOOP(def, A, j, pc);
      } break;
      case OP_NOT: {
        int B = GETARG_B(i);
        emit_NOT(def, A, B, pc);
      } break;
      case OP_RAVI_BNOT_I: {
        int B = GETARG_B(i);
        emit_BNOT_I(def, A, B, pc);
      } break;
      case OP_BNOT: {
        int B = GETARG_B(i);
        emit_BNOT(def, A, B, pc);
      } break;
      case OP_TEST: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        // OP_TEST is followed by OP_JMP - we process this
        // along with OP_EQ
        pc++;
        i = code[pc];
        op = GET_OPCODE(i);
        lua_assert(op == OP_JMP);
        int sbx = GETARG_sBx(i);
        // j below is the jump target
        int j = sbx + pc + 1;
        emit_TEST(def, A, B, C, j, GETARG_A(i), pc - 1);
      } break;
      case OP_TESTSET: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        // OP_TESTSET is followed by OP_JMP - we process this
        // along with OP_EQ
        pc++;
        i = code[pc];
        op = GET_OPCODE(i);
        lua_assert(op == OP_JMP);
        int sbx = GETARG_sBx(i);
        // j below is the jump target
        int j = sbx + pc + 1;
        emit_TESTSET(def, A, B, C, j, GETARG_A(i), pc - 1);
      } break;

      case OP_JMP: {
        int sbx = GETARG_sBx(i);
        int j = sbx + pc + 1;
        emit_JMP(def, A, j, pc);
      } break;

      case OP_RAVI_FORPREP_I1:
      case OP_RAVI_FORPREP_IP: {
        int sbx = GETARG_sBx(i);
        int j = sbx + pc + 1;
        emit_iFORPREP(def, A, j, op == OP_RAVI_FORPREP_I1, pc);
      } break;
      case OP_FORPREP: {
        int sbx = GETARG_sBx(i);
        int j = sbx + pc + 1;
#if RAVI_CODEGEN_FORPREP2
        emit_FORPREP2(def, A, j, pc);
#else
        emit_FORPREP(def, A, j, pc);
#endif
      } break;
      case OP_RAVI_FORLOOP_I1:
      case OP_RAVI_FORLOOP_IP: {
        int sbx = GETARG_sBx(i);
        int j = sbx + pc + 1;
        emit_iFORLOOP(def, A, j, def->jmp_targets[pc], op == OP_RAVI_FORLOOP_I1,
                      pc);
      } break;
      case OP_FORLOOP: {
        int sbx = GETARG_sBx(i);
        int j = sbx + pc + 1;
#if RAVI_CODEGEN_FORPREP2
        emit_FORLOOP2(def, A, j, def->jmp_targets[pc], pc);
#else
        emit_FORLOOP(def, A, j, pc);
#endif
      } break;

      case OP_LOADNIL: {
        int B = GETARG_B(i);
        emit_LOADNIL(def, A, B, pc);
      } break;
      case OP_RAVI_LOADFZ: {
        emit_LOADFZ(def, A, pc);
      } break;
      case OP_RAVI_LOADIZ: {
        emit_LOADIZ(def, A, pc);
      } break;
      case OP_TAILCALL:
      case OP_CALL: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_CALL(def, A, B, C, pc);
      } break;

      case OP_RAVI_SETTABLE_SK:
      case OP_RAVI_SETTABLE_S: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_SETTABLE_SK(def, A, B, C, pc);
      } break;
      case OP_RAVI_SETTABLE_I: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_SETTABLE_I(def, A, B, C, pc);
      } break;
      case OP_SETTABLE: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_SETTABLE(def, A, B, C, pc);
      } break;
      case OP_RAVI_GETTABLE_S: {
        int C = GETARG_C(i);
        int B = GETARG_B(i);
        lua_assert(ISK(C));
        TValue *kv = k + INDEXK(C);
        TString *key = tsvalue(kv);
        lua_assert(key->tt == LUA_TSHRSTR);
        emit_GETTABLE_S(def, A, B, C, pc, key);
      } break;

      case OP_RAVI_SELF_S: {
        int C = GETARG_C(i);
        int B = GETARG_B(i);
        lua_assert(ISK(C));
        TValue *kv = k + INDEXK(C);
        TString *key = tsvalue(kv);
        lua_assert(key->tt == LUA_TSHRSTR);
        emit_SELF_S(def, A, B, C, pc, key);
      } break;
      case OP_RAVI_GETTABLE_I: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_GETTABLE_I(def, A, B, C, pc);
      } break;
      case OP_RAVI_GETTABLE_SK: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        lua_assert(ISK(C));
        TValue *kv = k + INDEXK(C);
        TString *key = tsvalue(kv);
        lua_assert(key->tt == LUA_TSHRSTR);
        emit_GETTABLE_SK(def, A, B, C, pc, key);
      } break;
      case OP_GETTABLE: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_GETTABLE(def, A, B, C, pc);
      } break;
      case OP_RAVI_GETTABLE_AI: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_GETTABLE_AI(def, A, B, C, omitArrayGetRangeCheck, pc);
      } break;
      case OP_RAVI_SETTABLE_AII:
      case OP_RAVI_SETTABLE_AI: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_SETTABLE_AI(def, A, B, C, op == OP_RAVI_SETTABLE_AII, pc);
      } break;
      case OP_RAVI_GETTABLE_AF: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_GETTABLE_AF(def, A, B, C, omitArrayGetRangeCheck, pc);
      } break;
      case OP_RAVI_SETTABLE_AFF:
      case OP_RAVI_SETTABLE_AF: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_SETTABLE_AF(def, A, B, C, op == OP_RAVI_SETTABLE_AFF, pc);
      } break;
      case OP_RAVI_TOTAB: {
        emit_TOARRAY(def, A, RAVI_TTABLE, "table expected", pc);
      } break;
      case OP_RAVI_TOARRAYI: {
        emit_TOARRAY(def, A, RAVI_TARRAYINT, "integer[] expected", pc);
      } break;
      case OP_RAVI_TOARRAYF: {
        emit_TOARRAY(def, A, RAVI_TARRAYFLT, "number[] expected", pc);
      } break;
      case OP_RAVI_MOVEAI: {
        int B = GETARG_B(i);
        emit_MOVEAI(def, A, B, pc);
      } break;
      case OP_RAVI_MOVEAF: {
        int B = GETARG_B(i);
        emit_MOVEAF(def, A, B, pc);
      } break;
      case OP_RAVI_MOVETAB: {
        int B = GETARG_B(i);
        emit_MOVETAB(def, A, B, pc);
      } break;

      case OP_RAVI_GETTABUP_SK: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_GETTABUP_SK(def, A, B, C, pc);
      } break;
      case OP_GETTABUP: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_GETTABUP(def, A, B, C, pc);
      } break;
      case OP_GETUPVAL: {
        int B = GETARG_B(i);
        emit_GETUPVAL(def, A, B, pc);
      } break;
      case OP_SETTABUP: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_SETTABUP(def, A, B, C, pc);
      } break;
      case OP_SETUPVAL: {
        int B = GETARG_B(i);
        emit_SETUPVAL(def, A, B, pc);
      } break;

      case OP_RAVI_SETUPVALI: {
        int B = GETARG_B(i);
        emit_SETUPVAL_Specific(def, A, B, pc, OP_RAVI_SETUPVALI,
                               def->raviV_op_setupvaliF);
      } break;
      case OP_RAVI_SETUPVALF: {
        int B = GETARG_B(i);
        emit_SETUPVAL_Specific(def, A, B, pc, OP_RAVI_SETUPVALF,
                               def->raviV_op_setupvalfF);
      } break;
      case OP_RAVI_SETUPVALAI: {
        int B = GETARG_B(i);
        emit_SETUPVAL_Specific(def, A, B, pc, OP_RAVI_SETUPVALAI,
                               def->raviV_op_setupvalaiF);
      } break;
      case OP_RAVI_SETUPVALAF: {
        int B = GETARG_B(i);
        emit_SETUPVAL_Specific(def, A, B, pc, OP_RAVI_SETUPVALAF,
                               def->raviV_op_setupvalafF);
      } break;
      case OP_RAVI_SETUPVALT: {
        int B = GETARG_B(i);
        emit_SETUPVAL_Specific(def, A, B, pc, OP_RAVI_SETUPVALT,
                               def->raviV_op_setupvaltF);
      } break;

      case OP_RAVI_BXOR_II:
      case OP_RAVI_BOR_II:
      case OP_RAVI_BAND_II: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_BITWISE_BINARY_OP(def, op, A, B, C, pc);
      } break;

      case OP_BAND:
      case OP_BOR:
      case OP_BXOR: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_BOR_BXOR_BAND(def, op, A, B, C, pc);
      } break;

      case OP_SHR:
      case OP_SHL:
      case OP_RAVI_SHL_II:
      case OP_RAVI_SHR_II: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_BITWISE_SHIFT_OP(def, op, A, B, C, pc);
      } break;

      case OP_ADD: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_ARITH(def, A, B, C, OP_ADD, TM_ADD, pc);
      } break;
      case OP_RAVI_ADDFF: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_ADDFF(def, A, B, C, pc);
      } break;
      case OP_RAVI_ADDFI: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_ADDFI(def, A, B, C, pc);
      } break;
      case OP_RAVI_ADDII: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_ADDII(def, A, B, C, pc);
      } break;

      case OP_SUB: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_ARITH(def, A, B, C, OP_SUB, TM_SUB, pc);
      } break;
      case OP_RAVI_SUBFF: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_SUBFF(def, A, B, C, pc);
      } break;
      case OP_RAVI_SUBFI: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_SUBFI(def, A, B, C, pc);
      } break;
      case OP_RAVI_SUBIF: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_SUBIF(def, A, B, C, pc);
      } break;
      case OP_RAVI_SUBII: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_SUBII(def, A, B, C, pc);
      } break;

      case OP_MUL: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_ARITH(def, A, B, C, OP_MUL, TM_MUL, pc);
      } break;
      case OP_RAVI_MULFF: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_MULFF(def, A, B, C, pc);
      } break;
      case OP_RAVI_MULFI: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_MULFI(def, A, B, C, pc);
      } break;
      case OP_RAVI_MULII: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_MULII(def, A, B, C, pc);
      } break;

      case OP_DIV: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_ARITH(def, A, B, C, OP_DIV, TM_DIV, pc);
      } break;
      case OP_RAVI_DIVFF: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_DIVFF(def, A, B, C, pc);
      } break;
      case OP_RAVI_DIVFI: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_DIVFI(def, A, B, C, pc);
      } break;
      case OP_RAVI_DIVIF: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_DIVIF(def, A, B, C, pc);
      } break;
      case OP_RAVI_DIVII: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_DIVII(def, A, B, C, pc);
      } break;

      case OP_MOD: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_MOD(def, A, B, C, pc);
      } break;
      case OP_IDIV: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_IDIV(def, A, B, C, pc);
      } break;
      case OP_POW: {
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        emit_POW(def, A, B, C, pc);
      } break;
      case OP_UNM: {
        int B = GETARG_B(i);
        emit_UNM(def, A, B, pc);
      } break;

      default: {
        fprintf(stderr, "Unexpected bytecode %d\n", op);
        abort();
      }
    }
  }
  if (doVerify && llvm::verifyFunction(*f->function(), &llvm::errs())) {
    f->dump();
    fprintf(stderr, "LLVM Code Verification failed\n");
	exit(1);
  }
  ravi::RaviJITFunction *llvm_func = f.release();
  p->ravi_jit.jit_data = reinterpret_cast<void *>(llvm_func);
  p->ravi_jit.jit_function = nullptr;
  p->ravi_jit.jit_status = RAVI_JIT_COMPILED;
  return llvm_func != nullptr;
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
        int j = pc + 2;  // jump target
        if (C && !def->jmp_targets[j].jmp1)
          def->jmp_targets[j].jmp1 =
              llvm::BasicBlock::Create(def->jitState->context(), "loadbool");
      } break;
      case OP_JMP:
      case OP_RAVI_FORPREP_IP:
      case OP_RAVI_FORPREP_I1:
      case OP_RAVI_FORLOOP_IP:
      case OP_RAVI_FORLOOP_I1:
      case OP_FORLOOP:
      case OP_FORPREP:
      case OP_TFORLOOP: {
        const char *targetname = nullptr;
        char temp[80];
        if (op == OP_JMP)
          targetname = "jmp";
        else if (op == OP_FORLOOP || op == OP_RAVI_FORLOOP_IP ||
                 op == OP_RAVI_FORLOOP_I1)
          targetname = "forbody";
        else if (op == OP_FORPREP || op == OP_RAVI_FORPREP_IP ||
                 op == OP_RAVI_FORPREP_I1)
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
      default: break;
    }
  }
}
}

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

#ifndef RAVI_LLVMCODEGEN_H
#define RAVI_LLVMCODEGEN_H

#include "ravijit.h"
#include "ravillvm.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO we probably do not need all the headers
// below

#define lvm_c
#define LUA_CORE

#include "lprefix.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
//#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
//#include "ltm.h"
#include "lvm.h"

#ifdef __cplusplus
}
#endif

#include <array>
#include <iterator>
#include <type_traits>
#include <atomic>

namespace ravi {

// All Lua types are gathered here
struct LuaLLVMTypes {

  LuaLLVMTypes(llvm::LLVMContext &context);
  void dump();

  llvm::Type *C_doubleT;
  llvm::Type *C_intptr_t;
  llvm::Type *C_size_t;
  llvm::Type *C_ptrdiff_t;
  llvm::Type *C_int64_t;
  llvm::Type *C_shortT;

  llvm::Type *lua_NumberT;
  llvm::PointerType *plua_NumberT;
  llvm::PointerType *pplua_NumberT;

  llvm::Type *lua_IntegerT;
  llvm::PointerType *plua_IntegerT;
  llvm::PointerType *pplua_IntegerT;

  llvm::Type *lua_UnsignedT;
  llvm::Type *lua_KContextT;

  llvm::FunctionType *lua_CFunctionT;
  llvm::PointerType *plua_CFunctionT;

  llvm::FunctionType *lua_KFunctionT;
  llvm::PointerType *plua_KFunctionT;

  llvm::FunctionType *lua_HookT;
  llvm::PointerType *plua_HookT;

  llvm::FunctionType *lua_AllocT;
  llvm::PointerType *plua_AllocT;

  llvm::Type *l_memT;
  llvm::Type *lu_memT;

  llvm::Type *tmsT;

  llvm::Type *lu_byteT;
  llvm::Type *L_UmaxalignT;
  llvm::Type *C_pcharT;

  llvm::Type *C_intT;
  llvm::Type *C_pintT;

  llvm::StructType *lua_StateT;
  llvm::PointerType *plua_StateT;

  llvm::StructType *global_StateT;
  llvm::PointerType *pglobal_StateT;

  llvm::StructType *ravi_StateT;
  llvm::PointerType *pravi_StateT;

  llvm::StructType *GCObjectT;
  llvm::PointerType *pGCObjectT;

  llvm::StructType *ValueT;
  llvm::StructType *TValueT;
  llvm::PointerType *pTValueT;

  llvm::StructType *TStringT;
  llvm::PointerType *pTStringT;
  llvm::PointerType *ppTStringT;

  llvm::StructType *UdataT;
  llvm::StructType *RaviArrayT;
  llvm::StructType *TableT;
  llvm::PointerType *pTableT;
  llvm::PointerType *ppTableT;

  llvm::StructType *UpvaldescT;
  llvm::PointerType *pUpvaldescT;

  llvm::Type *ravitype_tT;
  llvm::StructType *LocVarT;
  llvm::PointerType *pLocVarT;

  llvm::Type *InstructionT;
  llvm::PointerType *pInstructionT;
  llvm::StructType *LClosureT;
  llvm::PointerType *pLClosureT;
  llvm::PointerType *ppLClosureT;
  llvm::PointerType *pppLClosureT;

  llvm::StructType *RaviJITProtoT;

  llvm::StructType *ProtoT;
  llvm::PointerType *pProtoT;
  llvm::PointerType *ppProtoT;

  llvm::StructType *UpValT;
  llvm::PointerType *pUpValT;

  llvm::StructType *CClosureT;
  llvm::PointerType *pCClosureT;

  llvm::StructType *TKeyT;
  llvm::PointerType *pTKeyT;

  llvm::StructType *NodeT;
  llvm::PointerType *pNodeT;

  llvm::StructType *lua_DebugT;
  llvm::PointerType *plua_DebugT;

  llvm::StructType *lua_longjumpT;
  llvm::PointerType *plua_longjumpT;

  llvm::StructType *MbufferT;
  llvm::StructType *stringtableT;

  llvm::PointerType *StkIdT;

  llvm::StructType *CallInfoT;
  llvm::StructType *CallInfo_cT;
  llvm::StructType *CallInfo_lT;
  llvm::PointerType *pCallInfoT;

  llvm::FunctionType *jitFunctionT;

  llvm::FunctionType *luaD_poscallT;
  llvm::FunctionType *luaD_precallT;
  llvm::FunctionType *luaD_callT;
  llvm::FunctionType *luaF_closeT;
  llvm::FunctionType *luaT_trybinTMT;
  llvm::FunctionType *luaG_runerrorT;
  llvm::FunctionType *luaV_equalobjT;
  llvm::FunctionType *luaV_lessthanT;
  llvm::FunctionType *luaV_lessequalT;
  llvm::FunctionType *luaV_forlimitT;
  llvm::FunctionType *luaV_tonumberT;
  llvm::FunctionType *luaV_tointegerT;
  llvm::FunctionType *luaV_modT;
  llvm::FunctionType *luaV_objlenT;
  llvm::FunctionType *luaV_divT;
  llvm::FunctionType *luaC_upvalbarrierT;

  llvm::FunctionType *luaV_executeT;
  llvm::FunctionType *luaV_gettableT;
  llvm::FunctionType *luaV_settableT;

  // Following are functions that handle specific bytecodes
  // We cheat for these bytecodes by calling the function that
  // implements it
  llvm::FunctionType *raviV_op_newarrayintT;
  llvm::FunctionType *raviV_op_newarrayfloatT;
  llvm::FunctionType *raviV_op_setlistT;
  llvm::FunctionType *raviV_op_newtableT;
  llvm::FunctionType *raviV_op_loadnilT;
  llvm::FunctionType *raviV_op_concatT;
  llvm::FunctionType *raviV_op_closureT;
  llvm::FunctionType *raviV_op_varargT;

  llvm::FunctionType *raviH_set_intT;
  llvm::FunctionType *raviH_set_floatT;

  std::array<llvm::Constant *, 256> kInt;
  std::array<llvm::Constant *, 21> kluaInteger;

  llvm::Constant *kFalse;

  // To allow better optimization we need to decorate the
  // LLVM Load/Store instructions with type information.
  // For this we need to construct tbaa metadata
  llvm::MDBuilder mdbuilder;
  llvm::MDNode *tbaa_root;
  llvm::MDNode *tbaa_charT;
  llvm::MDNode *tbaa_pcharT;
  llvm::MDNode *tbaa_shortT;
  llvm::MDNode *tbaa_pshortT;
  llvm::MDNode *tbaa_intT;
  llvm::MDNode *tbaa_pintT;
  llvm::MDNode *tbaa_longlongT;
  llvm::MDNode *tbaa_plonglongT;
  llvm::MDNode *tbaa_pointerT;
  llvm::MDNode *tbaa_ppointerT;
  llvm::MDNode *tbaa_CallInfo_lT;
  llvm::MDNode *tbaa_CallInfoT;
  llvm::MDNode *tbaa_luaStateT;
  llvm::MDNode *tbaa_luaState_ciT;
  llvm::MDNode *tbaa_luaState_ci_baseT;
  llvm::MDNode *tbaa_CallInfo_funcT;
  llvm::MDNode *tbaa_CallInfo_func_LClosureT;
  llvm::MDNode *tbaa_CallInfo_topT;
  llvm::MDNode *tbaa_LClosureT;
  llvm::MDNode *tbaa_LClosure_pT;
  llvm::MDNode *tbaa_LClosure_upvalsT;
  llvm::MDNode *tbaa_ProtoT;
  llvm::MDNode *tbaa_Proto_kT;
  llvm::MDNode *tbaa_Proto_sizepT;
  llvm::MDNode *tbaa_TValueT;
  llvm::MDNode *tbaa_TValue_nT;
  llvm::MDNode *tbaa_TValue_hT;
  llvm::MDNode *tbaa_TValue_ttT;
  llvm::MDNode *tbaa_luaState_topT;
  llvm::MDNode *tbaa_UpValT;
  llvm::MDNode *tbaa_UpVal_vT;
  llvm::MDNode *tbaa_RaviArrayT;
  llvm::MDNode *tbaa_TableT;
  llvm::MDNode *tbaa_RaviArray_typeT;
  llvm::MDNode *tbaa_RaviArray_dataT;
  llvm::MDNode *tbaa_RaviArray_lenT;
};

class RAVI_API RaviJITStateImpl;

// Represents a JITed or JITable function
// Each function gets its own module and execution engine - this
// may change in future
// The advantage is that we can drop the function when the corresponding
// Lua object is garbage collected - with MCJIT this is not possible
// to do at function level
class RAVI_API RaviJITFunctionImpl : public RaviJITFunction {

  // The function is tracked by RaviJITState so we need to
  // tell RaviJITState when this function dies
  RaviJITStateImpl *owner_;

  // Unique name for the function
  std::string name_;

  // The execution engine responsible for compiling the
  // module
  llvm::ExecutionEngine *engine_;

  // Module within which the function will be defined
  llvm::Module *module_;

  // The llvm Function definition
  llvm::Function *function_;

  // Pointer to compiled function - this is only set when
  // the function
  void *ptr_;

public:
  RaviJITFunctionImpl(RaviJITStateImpl *owner, llvm::FunctionType *type,
                      llvm::GlobalValue::LinkageTypes linkage,
                      const std::string &name);
  virtual ~RaviJITFunctionImpl();

  // Compile the function if not already compiled and
  // return pointer to function
  virtual void *compile();

  // Add declaration for an extern function that is not
  // loaded dynamically - i.e., is part of the the executable
  // and therefore not visible at runtime by name
  virtual llvm::Constant *addExternFunction(llvm::FunctionType *type,
                                            void *address,
                                            const std::string &name);

  virtual const std::string &name() const { return name_; }
  virtual llvm::Function *function() const { return function_; }
  virtual llvm::Module *module() const { return module_; }
  virtual llvm::ExecutionEngine *engine() const { return engine_; }
  virtual RaviJITState *owner() const;
  virtual void dump();
};

// Ravi's JIT State
// All of the JIT information is held here
class RAVI_API RaviJITStateImpl : public RaviJITState {

  // map of names to functions
  std::map<std::string, RaviJITFunction *> functions_;

  llvm::LLVMContext &context_;

  // The triple represents the host target
  std::string triple_;

  // Lua type definitions
  LuaLLVMTypes *types_;

  // Should we auto compile what we can?
  bool auto_;

  // Is JIT enabled
  bool enabled_;

  // Optimizer level (LLVM PassManagerBuilder)
  int opt_level_;

  // Size level (LLVM PassManagerBuilder)
  int size_level_;

public:
  RaviJITStateImpl();
  virtual ~RaviJITStateImpl();

  // Create a function of specified type and linkage
  virtual RaviJITFunction *
  createFunction(llvm::FunctionType *type,
                 llvm::GlobalValue::LinkageTypes linkage,
                 const std::string &name);

  // Stop tracking the named function - note that
  // the function is assumed to be destroyed by the user
  void deleteFunction(const std::string &name);

  void addGlobalSymbol(const std::string &name, void *address);

  virtual void dump();
  virtual llvm::LLVMContext &context() { return context_; }
  LuaLLVMTypes *types() const { return types_; }
  const std::string &triple() const { return triple_; }
  bool is_auto() const { return auto_; }
  void set_auto(bool value) { auto_ = value; }
  bool is_enabled() const { return enabled_; }
  void set_enabled(bool value) { enabled_ = value; }
  int get_optlevel() const { return opt_level_; }
  void set_optlevel(int value) {
    if (value >= 0 && value <= 3)
      opt_level_ = value;
  }
  int get_sizelevel() const { return size_level_; }
  void set_sizelevel(int value) {
    if (value >= 0 && value <= 2)
      size_level_ = value;
  }
};

// To optimise fornum loops
// i.e. OP_FORPREP and OP_FORLOOP instructions
// we use computed gotos to specialised
// jmp labels. Hence the 4 jmp targets.
// For other instructions only the first jump
// target is used
struct RaviBranchDef {
  // this field is used for all branches
  // forloop int step > 0
  llvm::BasicBlock *jmp1;
  // forloop int step < 0
  llvm::BasicBlock *jmp2;
  // forloop float step > 0
  llvm::BasicBlock *jmp3;
  // forlook float step < 0
  llvm::BasicBlock *jmp4;

  // These are local variables for a fornum
  // loop
  llvm::Value *ilimit;
  llvm::Value *istep;
  llvm::Value *iidx;
  llvm::Value *flimit;
  llvm::Value *fstep;
  llvm::Value *fidx;

  // This holds the branch to which the
  // loop body will jump to using a
  // IndirectBr instruction
  llvm::Value *forloop_branch;

  RaviBranchDef();
};

// This structure holds stuff we need when compiling a single
// function
struct RaviFunctionDef {
  RaviJITStateImpl *jitState;
  RaviJITFunctionImpl *raviF;
  llvm::Function *f;
  llvm::BasicBlock *entry;
  llvm::Value *L;
  LuaLLVMTypes *types;
  llvm::IRBuilder<> *builder;

  // Lua function declarations
  llvm::Constant *luaD_poscallF;
  llvm::Constant *luaD_precallF;
  llvm::Constant *luaD_callF;
  llvm::Constant *luaF_closeF;
  llvm::Constant *luaG_runerrorF;
  llvm::Constant *luaT_trybinTMF;
  llvm::Constant *luaV_equalobjF;
  llvm::Constant *luaV_lessthanF;
  llvm::Constant *luaV_lessequalF;
  llvm::Constant *luaV_forlimitF;
  llvm::Constant *luaV_tonumberF;
  llvm::Constant *luaV_tointegerF;
  llvm::Constant *luaV_executeF;
  llvm::Constant *luaV_gettableF;
  llvm::Constant *luaV_settableF;
  llvm::Constant *luaV_modF;
  llvm::Constant *luaV_divF;
  llvm::Constant *luaV_objlenF;
  llvm::Constant *luaC_upvalbarrierF;

  // Some cheats - these correspond to OPCODEs that
  // are not inlined as of now
  llvm::Constant *raviV_op_newarrayintF;
  llvm::Constant *raviV_op_newarrayfloatF;
  llvm::Constant *raviV_op_setlistF;
  llvm::Constant *raviV_op_newtableF;
  llvm::Constant *raviV_op_loadnilF;
  llvm::Constant *raviV_op_concatF;
  llvm::Constant *raviV_op_closureF;
  llvm::Constant *raviV_op_varargF;

  llvm::Constant *raviH_set_intF;
  llvm::Constant *raviH_set_floatF;

  // standard C functions
  llvm::Constant *printfFunc;
  llvm::Constant *fmodFunc;
  llvm::Constant *floorFunc;
  llvm::Constant *powFunc;

  // Jump targets in the function
  std::vector<RaviBranchDef> jmp_targets;

  // Load pointer to proto
  llvm::Instruction *proto_ptr;

  // Obtain pointer to Proto->k
  llvm::Value *proto_k;

  // Load pointer to k
  llvm::Instruction *k_ptr;

  // Load L->ci
  llvm::Instruction *ci_val;

  // Get pointer to base
  llvm::Value *Ci_base;

  // Pointer to LClosure
  llvm::Value *p_LClosure;
};

// This class is responsible for compiling Lua byte code
// to LLVM IR
class RaviCodeGenerator {
public:
  RaviCodeGenerator(RaviJITStateImpl *jitState);

  // Compile given function if possible
  // The p->ravi_jit structure will be updated
  // Note that if a function fails to compile then
  // a flag is set so that it doesn't get compiled again
  void compile(lua_State *L, Proto *p);

  // We can only compile a subset of op codes
  // and not all features are supported
  bool canCompile(Proto *p);

  // Create a unique function name in the context
  // of this generator
  const char *unique_function_name();

  // Create the prologue of the JIT function
  // Argument will be named L
  // Initial BasicBlock will be created
  // int func(lua_State *L) {
  std::unique_ptr<RaviJITFunctionImpl>
  create_function(llvm::IRBuilder<> &builder, RaviFunctionDef *def);

  // Add extern declarations for Lua functions we need to call
  void emit_extern_declarations(RaviFunctionDef *def);

  // Retrieve the proto->sizep
  llvm::Instruction *emit_load_proto_sizep(RaviFunctionDef *def, llvm::Value *proto_ptr);

  // Store lua_Number or lua_Integer 
  llvm::Instruction *emit_store_local_n(RaviFunctionDef *def, llvm::Value *src, llvm::Value *dest);

  // Load lua_Number or lua_Integer
  llvm::Instruction *emit_load_local_n(RaviFunctionDef *def, llvm::Value *src);

  // Store int 
  llvm::Instruction *emit_store_local_int(RaviFunctionDef *def, llvm::Value *src, llvm::Value *dest);

  // Load int
  llvm::Instruction *emit_load_local_int(RaviFunctionDef *def, llvm::Value *src);

  // emit code for (LClosure *)ci->func->value_.gc
  llvm::Value *emit_gep_ci_func_value_gc_asLClosure(RaviFunctionDef *def,
                                                    llvm::Value *ci);

  llvm::Value *emit_gep(RaviFunctionDef *def, const char *name, llvm::Value *s,
                        int arg1, int arg2);

  llvm::Value *emit_gep(RaviFunctionDef *def, const char *name, llvm::Value *s,
                        int arg1, int arg2, int arg3);

  // emit code for &ptr[offset]
  llvm::Value *emit_array_get(RaviFunctionDef *def, llvm::Value *ptr,
                              int offset);

  // emit code to load pointer L->ci->u.l.base
  llvm::Instruction *emit_load_base(RaviFunctionDef *def);

  // emit code to obtain address of register at location A
  llvm::Value *emit_gep_ra(RaviFunctionDef *def, llvm::Instruction *base,
                           int A);

  // emit code to obtain address of register or constant at location B
  llvm::Value *emit_gep_rkb(RaviFunctionDef *def, llvm::Instruction *base,
                            int B);

  // emit code to load the lua_Number value from register
  llvm::Instruction *emit_load_reg_n(RaviFunctionDef *def, llvm::Value *ra);

  // emit code to load the lua_Integer value from register
  llvm::Instruction *emit_load_reg_i(RaviFunctionDef *def, llvm::Value *rb);

  // emit code to load the boolean value from register
  llvm::Instruction *emit_load_reg_b(RaviFunctionDef *def, llvm::Value *ra);

  // emit code to load the table value from register
  llvm::Instruction *emit_load_reg_h(RaviFunctionDef *def, llvm::Value *ra);

  // emit code to load pointer to int array
  llvm::Instruction *emit_load_reg_h_intarray(RaviFunctionDef *def,
                                              llvm::Instruction *ra);

  // emit code to load pointer to double array
  llvm::Instruction *emit_load_reg_h_floatarray(RaviFunctionDef *def,
                                                llvm::Instruction *ra);

  // emit code to store lua_Number value into register
  void emit_store_reg_n(RaviFunctionDef *def, llvm::Value *value,
                        llvm::Value *dest_ptr);

  // emit code to store lua_Integer value into register
  void emit_store_reg_i(RaviFunctionDef *def, llvm::Value *value,
                        llvm::Value *dest_ptr);

  // emit code to store bool value into register
  void emit_store_reg_b(RaviFunctionDef *def, llvm::Value *value,
                        llvm::Value *dest_ptr);

  // emit code to set the type in the register
  void emit_store_type(RaviFunctionDef *def, llvm::Value *value, int type);

  // emit code to load the type from a register
  llvm::Instruction *emit_load_type(RaviFunctionDef *def, llvm::Value *value);

  // emit code to load the array type
  llvm::Instruction *emit_load_ravi_arraytype(RaviFunctionDef *def,
                                              llvm::Value *value);

  // emit code to load the array length
  llvm::Instruction *emit_load_ravi_arraylength(RaviFunctionDef *def,
                                                llvm::Value *value);

  // TValue assign
  void emit_assign(RaviFunctionDef *def, llvm::Value *ra, llvm::Value *rb);

  // Get &upvals[offset] from LClosure
  llvm::Value *emit_gep_upvals(RaviFunctionDef *def, llvm::Value *cl_ptr,
                               int offset);

  // Load the &upvals[offset] -> result is UpVal*
  llvm::Instruction *emit_load_pupval(RaviFunctionDef *def,
                                      llvm::Value *ppupval);

  // Get &upval->v
  llvm::Value *emit_gep_upval_v(RaviFunctionDef *def,
                                llvm::Instruction *pupval);

  // Load upval->v
  llvm::Instruction *emit_load_upval_v(RaviFunctionDef *def,
                                       llvm::Instruction *pupval);

  // Get &upval->value -> result is TValue *
  llvm::Value *emit_gep_upval_value(RaviFunctionDef *def,
                                    llvm::Instruction *pupval);

  // isnil(reg) || isboolean(reg) && reg.value == 0
  // !(isnil(reg) || isboolean(reg) && reg.value == 0)
  llvm::Value *emit_boolean_testfalse(RaviFunctionDef *def, llvm::Value *reg,
                                      bool donot);

  // L->top = ci->top
  void emit_refresh_L_top(RaviFunctionDef *def);

  // L->top = R(B)
  void emit_set_L_top_toreg(RaviFunctionDef *def, llvm::Instruction *base_ptr,
                            int B);

  // Look for Lua bytecodes that are jump targets and allocate
  // a BasicBlock for each such target in def->jump_targets.
  // The BasicBlocks are not inserted into the function until later
  // but having them created allows rest of the code to insert
  // branch instructions
  void scan_jump_targets(RaviFunctionDef *def, Proto *p);

  // Should be called before processing a Lua OpCode
  // This function checks if a new block should be started
  // and links in the new block
  void link_block(RaviFunctionDef *def, int pc);

  void emit_CONCAT(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                   int A, int B, int C);

  void emit_CLOSURE(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                    int A, int Bx);

  void emit_VARARG(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                   int A, int B);

  void emit_LOADNIL(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                    int A, int B);

  void emit_LOADFZ(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                   int A);

  void emit_LOADIZ(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                   int A);

  void emit_LOADBOOL(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int B, int C, int j);

  void emit_ARITH(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C, OpCode op, TMS tms);

  void emit_MOD(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                int A, int B, int C);

  void emit_IDIV(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                 int A, int B, int C);

  void emit_POW(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                int A, int B, int C);

  void emit_UNM(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                int A, int B);

  void emit_UNMF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                 int A, int B);

  void emit_UNMI(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                 int A, int B);

  void emit_ADDFF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_ADDFI(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_ADDII(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_ADDFN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_ADDIN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_SUBFF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_SUBFI(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_SUBIF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_SUBII(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_SUBFN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_SUBNF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_SUBIN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_SUBNI(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_MULFF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_MULFI(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_MULII(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_MULFN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_MULIN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_DIVFF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_DIVFI(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_DIVIF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_DIVII(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B, int C);

  void emit_LOADK(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int Bx);

  void emit_RETURN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                   int A, int B);

  void emit_CALL(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                 int A, int B, int C);

  void emit_JMP(RaviFunctionDef *def, int A, int j);

  void emit_iFORPREP(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int sBx, int step_one);

  void emit_iFORLOOP(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int sBx, RaviBranchDef &b,
                     int step_one);

  void emit_FORPREP(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                    int A, int sBx);

  void emit_FORLOOP(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                    int A, int sBx);

  void emit_FORPREP2(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int sBx);

  void emit_FORLOOP2(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int sBx, RaviBranchDef &b);

  void emit_MOVE(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                 int A, int B);

  void emit_MOVEI(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B);

  void emit_MOVEF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int B);

  void emit_TOINT(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A);

  void emit_TOFLT(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A);

  void emit_LEN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                int A, int B);

  void emit_SETTABLE(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int B, int C);

  void emit_GETTABLE(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int B, int C);

  void emit_SELF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                 int A, int B, int C);

  void emit_GETUPVAL(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int B);

  void emit_SETUPVAL(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int B);

  void emit_GETTABUP(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int B, int C);

  void emit_SETTABUP(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int B, int C);

  void emit_NEWARRAYINT(RaviFunctionDef *def, llvm::Value *L_ci,
                        llvm::Value *proto, int A);

  void emit_NEWARRAYFLOAT(RaviFunctionDef *def, llvm::Value *L_ci,
                          llvm::Value *proto, int A);

  void emit_NEWTABLE(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int B, int C);

  void emit_SETLIST(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                    int A, int B, int C);

  void emit_NOT(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                int A, int B);

  // Emit code for OP_EQ, OP_LT and OP_LE
  // The callee parameter should be luaV_equalobj, luaV_lessthan and
  // luaV_lessequal
  // respectively
  // A, B, C must be operands of the OP_EQ/OP_LT/OP_LE instructions
  // j must be the jump target (offset of the code to which we need to jump to)
  // jA must be the A operand of the jump instruction
  void emit_EQ(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
               int A, int B, int C, int j, int jA, llvm::Constant *callee);

  // OP_TEST is followed by a OP_JMP instruction - both are handled
  // together
  // A, B, C must be operands of the OP_TEST instruction
  // j must be the jump target (offset of the code to which we need to jump to)
  // jA must be the A operand of the jump instruction
  void emit_TEST(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                 int A, int B, int C, int j, int jA);

  // OP_TESTSET is followed by a OP_JMP instruction - both are handled
  // together
  // A, B, C must be operands of the OP_TESTSET instruction
  // j must be the jump target (offset of the code to which we need to jump to)
  // jA must be the A operand of the jump instruction
  void emit_TESTSET(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                    int A, int B, int C, int j, int jA);

  void emit_TFORCALL(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int B, int C, int j, int jA);

  void emit_TFORLOOP(RaviFunctionDef *def, llvm::Value *L_ci,
                     llvm::Value *proto, int A, int j);

  void emit_GETTABLE_AF(RaviFunctionDef *def, llvm::Value *L_ci,
                        llvm::Value *proto, int A, int B, int C);

  void emit_GETTABLE_AI(RaviFunctionDef *def, llvm::Value *L_ci,
                        llvm::Value *proto, int A, int B, int C);

  void emit_SETTABLE_AF(RaviFunctionDef *def, llvm::Value *L_ci,
                        llvm::Value *proto, int A, int B, int C);

  void emit_SETTABLE_AI(RaviFunctionDef *def, llvm::Value *L_ci,
                        llvm::Value *proto, int A, int B, int C);

  void emit_MOVEAI(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                   int A, int B);

  void emit_MOVEAF(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                   int A, int B);

  // Return the base pointer
  llvm::Instruction *emit_TOARRAY(RaviFunctionDef *def, llvm::Value *L_ci,
                                  llvm::Value *proto, int A,
                                  int array_type_expected, const char *errmsg);

private:
  RaviJITStateImpl *jitState_;
  char temp_[31]; // for name
  int id_;        // for name
};
}

#define RAVI_CODEGEN_FORPREP2 1

#endif

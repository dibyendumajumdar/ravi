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

#ifdef USE_LLVM

#include "ravi_llvm.h"
#include "ravijit.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO we probably do not need all the headers
// below

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
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"

#ifdef __cplusplus
}
#endif

#include <array>
#include <atomic>
#include <iterator>
#include <type_traits>

namespace ravi {

/*
** Lua typecode have certain bits that are used to
** indicate variants or subtypes, or whether the type
** is collectible. The enumerated values below
** reflect the way these type codes get set within
** Lua values - these are the codes the JIT code must
** use.
*/
enum LuaTypeCode {
  LUA__TNIL = LUA_TNIL,
  LUA__TBOOLEAN = LUA_TBOOLEAN,
  LUA__TLIGHTUSERDATA = LUA_TLIGHTUSERDATA,
  LUA__TNUMBER = LUA_TNUMBER,
  LUA__TSTRING = ctb(LUA_TSTRING),
  LUA__TTABLE = LUA_TTABLE,
  RAVI__TLTABLE = ctb(LUA_TTABLE),
  RAVI__TIARRAY = ctb(RAVI_TIARRAY),
  RAVI__TFARRAY = ctb(RAVI_TFARRAY),
  LUA__TFUNCTION = ctb(LUA_TFUNCTION),
  LUA__TUSERDATA = ctb(LUA_TUSERDATA),
  LUA__TTHREAD = ctb(LUA_TTHREAD),
  LUA__TLCL = ctb(LUA_TLCL),
  LUA__TLCF = LUA_TLCF,
  LUA__TCCL = ctb(LUA_TCCL),
  LUA__TSHRSTR = ctb(LUA_TSHRSTR),
  LUA__TLNGSTR = ctb(LUA_TLNGSTR),
  LUA__TNUMFLT = LUA_TNUMFLT,
  LUA__TNUMINT = LUA_TNUMINT
};

// All LLVM definitions for
// Lua types are gathered here
struct LuaLLVMTypes {
  LuaLLVMTypes(llvm::LLVMContext &context);
  void dump();

  // Following are standard C types
  // Must ensure that these types match
  // between JIT and the C compiler
  llvm::Type *C_voidT;
  llvm::Type *C_floatT;
  llvm::Type *C_doubleT;
  llvm::Type *C_intptr_t;
  llvm::Type *C_size_t;
  llvm::PointerType *C_psize_t; /* pointer to size_t */
  llvm::Type *C_ptrdiff_t;
  llvm::Type *C_int64_t;
  llvm::Type *C_shortT;
  llvm::Type *C_intT;
  llvm::PointerType *C_pintT;  /* pointer to int */
  llvm::PointerType *C_pcharT; /* pointer to char */

  llvm::Type *lua_NumberT;
  llvm::PointerType *plua_NumberT;
  llvm::PointerType *pplua_NumberT;

  // WARNING: We currently assume that lua_Integer is
  // 64-bit
  llvm::Type *lua_IntegerT;
  llvm::PointerType *plua_IntegerT;
  llvm::PointerType *pplua_IntegerT;

  // WARNING: We currently assume that lua_Unsigned is
  // 64-bit
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

  llvm::FunctionType *luaC_upvalbarrierT;
  llvm::FunctionType *luaD_poscallT;
  llvm::FunctionType *luaD_precallT;
  llvm::FunctionType *luaD_callT;
  llvm::FunctionType *luaF_closeT;
  llvm::FunctionType *luaG_runerrorT;
  llvm::FunctionType *luaH_getintT;
  llvm::FunctionType *luaH_setintT;
  llvm::FunctionType *luaH_getstrT;
  llvm::FunctionType *luaT_trybinTMT;
  llvm::FunctionType *luaV_equalobjT;
  llvm::FunctionType *luaV_lessthanT;
  llvm::FunctionType *luaV_lessequalT;
  llvm::FunctionType *luaV_forlimitT;
  llvm::FunctionType *luaV_tonumberT;
  llvm::FunctionType *luaV_tointegerT;
  llvm::FunctionType *luaV_modT;
  llvm::FunctionType *luaV_objlenT;
  llvm::FunctionType *luaV_divT;
  llvm::FunctionType *luaV_executeT;
  llvm::FunctionType *luaV_gettableT;
  llvm::FunctionType *luaV_settableT;
  llvm::FunctionType *luaV_finishgetT;

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
  llvm::FunctionType *raviV_op_addT;
  llvm::FunctionType *raviV_op_subT;
  llvm::FunctionType *raviV_op_mulT;
  llvm::FunctionType *raviV_op_divT;
  llvm::FunctionType *raviV_op_shrT;
  llvm::FunctionType *raviV_op_shlT;
  llvm::FunctionType *raviV_op_borT;
  llvm::FunctionType *raviV_op_bxorT;
  llvm::FunctionType *raviV_op_bandT;
  llvm::FunctionType *raviV_op_bnotT;
  llvm::FunctionType *raviV_op_setupvaliT;
  llvm::FunctionType *raviV_op_setupvalfT;
  llvm::FunctionType *raviV_op_setupvalaiT;
  llvm::FunctionType *raviV_op_setupvalafT;
  llvm::FunctionType *raviV_op_setupvaltT;
  llvm::FunctionType *raviV_gettable_sskeyT;
  llvm::FunctionType *raviV_settable_sskeyT;
  llvm::FunctionType *raviV_gettable_iT;
  llvm::FunctionType *raviV_settable_iT;

  llvm::FunctionType *raviH_set_intT;
  llvm::FunctionType *raviH_set_floatT;

  llvm::FunctionType *ravi_dump_valueT;
  llvm::FunctionType *ravi_dump_stackT;
  llvm::FunctionType *ravi_dump_stacktopT;
  llvm::FunctionType *ravi_debug_traceT;

  std::array<llvm::Constant *, 256> kInt;
  std::array<llvm::Constant *, 21> kluaInteger;
  std::array<llvm::Constant *, 10> kByte;

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
  llvm::MDNode *tbaa_doubleT;
  llvm::MDNode *tbaa_pdoubleT;
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
  llvm::MDNode *tbaa_CallInfo_jitstatusT;
  llvm::MDNode *tbaa_CallInfo_callstatusT;
  llvm::MDNode *tbaa_CallInfo_savedpcT;
  llvm::MDNode *tbaa_LClosureT;
  llvm::MDNode *tbaa_LClosure_pT;
  llvm::MDNode *tbaa_LClosure_upvalsT;
  llvm::MDNode *tbaa_ProtoT;
  llvm::MDNode *tbaa_Proto_kT;
  llvm::MDNode *tbaa_Proto_sizepT;
  llvm::MDNode *tbaa_Proto_codeT;
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
  llvm::MDNode *tbaa_TString_hash;
  llvm::MDNode *tbaa_Table_lsizenode;
  llvm::MDNode *tbaa_Table_node;
  llvm::MDNode *tbaa_Table_sizearray;
  llvm::MDNode *tbaa_Table_array;
  llvm::MDNode *tbaa_Table_flags;
  llvm::MDNode *tbaa_Table_metatable;
#if RAVI_USE_NEWHASH
  llvm::MDNode *tbaa_Table_hmask;
#endif
};

// The hierarchy of objects
// used to represent LLVM artifacts is as
// follows

// RaviJITState          - Root, held in Lua state; wraps llvm::Context
// +- RaviJITModule      - wraps llvm::Module
//    +- RaviJITFunction - wraps llvm::Function

// The RaviJITFunction is held within the
// Lua Proto structure - and garbage collected along
// with the Lua function. Each RaviJITFunction
// holds a reference to the owning RaviJITModule
// via a shared_ptr. This ensures that RaviJITModule gets
// released when no longer referenced.

class RaviJITState;
class RaviJITModule;
class RaviJITFunction;

class RaviJITStateFactory {
 public:
  static std::unique_ptr<RaviJITState> newJITState();
};

// A wrapper for LLVM Module
// Maintains a dedicated ExecutionEngine for the module
class RaviJITModule {
  // The Context that owns this module
  RaviJITState *owner_;

  // The execution engine responsible for compiling the
  // module
  llvm::ExecutionEngine *engine_;

  // The LLVM Module within which the functions will be defined
  llvm::Module *module_;

  // List of JIT functions in this module
  // We need this so that we can update the functions
  // post compilation
  std::vector<RaviJITFunction *> functions_;

  // Keep track of external symbols added
  std::map<std::string, llvm::Function *> external_symbols_;

 public:
  RaviJITModule(RaviJITState *owner);
  ~RaviJITModule();

  llvm::Module *module() const { return module_; }
  llvm::ExecutionEngine *engine() const { return engine_; }
  RaviJITState *owner() const { return owner_; }
  void dump();
  void dumpAssembly();

  // Add the function to this module, the function will be
  // saved in the functions_ array. The location of the
  // function is returned which must be returned by
  // f->getId()
  int addFunction(RaviJITFunction *f);
  // Remove a function from the array
  // This calls f->getId() to get the
  // functions location in the array
  void removeFunction(RaviJITFunction *f);

  // Runs LLVM code generation and optimization passes
  // The reason for separting this from the
  // finalization is that this method is also
  // used to dump the generated assembly code
  void runpasses(bool dumpAsm = false);
  // finalize the module, and assign each function
  // its function pointer
  void finalize(bool doDump = false);

  // Add declaration for an extern function that is not
  // loaded dynamically - i.e., is part of the the executable
  // and therefore not visible at runtime by name
  llvm::Function *addExternFunction(llvm::FunctionType *type, void *address,
                                    const std::string &name);
};

// Represents a JITed or JITable function
// This object is stored in the Lua Proto structure
// and gets destroyed when the Lua function is
// garbage collected
class RaviJITFunction {
  // The Module in which this function lives
  // We hold a shared_ptr to the module so that
  // the module will be destroyed when the
  // last associated RaviJITFunction is collected
  std::shared_ptr<RaviJITModule> module_;

  // Unique name for the function
  std::string name_;

  // ID allocated by the module to this function
  // This must be returned via getId()
  int id_;

  // The llvm Function definition
  llvm::Function *function_;

  // Pointer to compiled function
  void *ptr_;

  // A location provided by the caller where the
  // compiled function will be saved
  lua_CFunction *func_ptrptr_;

 public:
  RaviJITFunction(lua_CFunction *p,
                  const std::shared_ptr<RaviJITModule> &module,
                  llvm::FunctionType *type,
                  llvm::GlobalValue::LinkageTypes linkage,
                  const std::string &name);
  RaviJITFunction(lua_CFunction *p,
                  const std::shared_ptr<RaviJITModule> &module,
                  const std::string &name);

  ~RaviJITFunction();

  const std::string &name() const { return name_; }
  llvm::Function *function() const { return function_; }
  llvm::Module *module() const { return module_->module(); }
  std::shared_ptr<RaviJITModule> raviModule() const { return module_; }

  llvm::ExecutionEngine *engine() const { return module_->engine(); }
  RaviJITState *owner() const { return module_->owner(); }
  // This method retrieves the JITed function from the
  // execution engine and sets ptr_ member
  // It must be called after the module has run the
  // code generation and optimization passes
  void setFunctionPtr();
  void dump() { module_->dump(); }
  void dumpAssembly() { module_->dumpAssembly(); }
  int getId() const { return id_; }
  void setId(int id) { id_ = id; }
  llvm::Function *addExternFunction(llvm::FunctionType *type, void *address,
                                    const std::string &name) {
    return module_->addExternFunction(type, address, name);
  }
};

// Ravi's LLVM JIT State
// All of the JIT information is held here
class RaviJITState {
  // The LLVM Context
  llvm::LLVMContext *context_;

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

  // min code size for compilation
  int min_code_size_;

  // min execution count for compilation
  int min_exec_count_;

  // gc step size; defaults to 200
  int gc_step_;

  // enable calls to luaG_traceexec() at every bytecode
  // instruction; this is expensive!
  bool tracehook_enabled_;

  // Count of modules allocated
  // Used to debug module deallocation
  size_t allocated_modules_;

 public:
  RaviJITState();
  ~RaviJITState();

  void addGlobalSymbol(const std::string &name, void *address);

  void dump();
  llvm::LLVMContext &context() { return *context_; }
  LuaLLVMTypes *types() const { return types_; }
  const std::string &triple() const { return triple_; }
  bool is_auto() const { return auto_; }
  void set_auto(bool value) { auto_ = value; }
  bool is_enabled() const { return enabled_; }
  void set_enabled(bool value) { enabled_ = value; }
  int get_optlevel() const { return opt_level_; }
  void set_optlevel(int value) {
    if (value >= 0 && value <= 3) opt_level_ = value;
  }
  int get_sizelevel() const { return size_level_; }
  void set_sizelevel(int value) {
    if (value >= 0 && value <= 2) size_level_ = value;
  }
  int get_mincodesize() const { return min_code_size_; }
  void set_mincodesize(int value) {
    min_code_size_ = value > 0 ? value : min_code_size_;
  }
  int get_minexeccount() const { return min_exec_count_; }
  void set_minexeccount(int value) {
    min_exec_count_ = value > 0 ? value : min_exec_count_;
  }
  int get_gcstep() const { return gc_step_; }
  void set_gcstep(int value) { gc_step_ = value > 0 ? value : gc_step_; }
  bool is_tracehook_enabled() const { return tracehook_enabled_; }
  void set_tracehook_enabled(bool value) { tracehook_enabled_ = value; }
  void incr_allocated_modules() { allocated_modules_++; }
  void decr_allocated_modules() { allocated_modules_--; }
  size_t allocated_modules() const { return allocated_modules_; }
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
  RaviJITState *jitState;
  RaviJITFunction *raviF;
  llvm::Function *f;
  llvm::BasicBlock *entry;
  llvm::Value *L;
  LuaLLVMTypes *types;
  llvm::IRBuilder<> *builder;
  Proto *p;

  // Lua function declarations
  llvm::Function *luaD_poscallF;
  llvm::Function *luaD_precallF;
  llvm::Function *luaD_callF;
  llvm::Function *luaF_closeF;
  llvm::Function *luaG_runerrorF;
  llvm::Function *luaT_trybinTMF;
  llvm::Function *luaV_equalobjF;
  llvm::Function *luaV_lessthanF;
  llvm::Function *luaV_lessequalF;
  llvm::Function *luaV_forlimitF;
  llvm::Function *luaV_tonumberF;
  llvm::Function *luaV_tointegerF;
  llvm::Function *luaV_executeF;
  llvm::Function *luaV_gettableF;
  llvm::Function *luaV_settableF;
  llvm::Function *luaV_modF;
  llvm::Function *luaV_divF;
  llvm::Function *luaV_objlenF;
  llvm::Function *luaC_upvalbarrierF;
  llvm::Function *luaH_getstrF;
  llvm::Function *luaH_getintF;
  llvm::Function *luaH_setintF;
  llvm::Function *luaV_finishgetF;

  // Some cheats - these correspond to OPCODEs that
  // are not inlined as of now
  llvm::Function *raviV_op_newarrayintF;
  llvm::Function *raviV_op_newarrayfloatF;
  llvm::Function *raviV_op_setlistF;
  llvm::Function *raviV_op_newtableF;
  llvm::Function *raviV_op_loadnilF;
  llvm::Function *raviV_op_concatF;
  llvm::Function *raviV_op_closureF;
  llvm::Function *raviV_op_varargF;
  llvm::Function *raviV_op_addF;
  llvm::Function *raviV_op_subF;
  llvm::Function *raviV_op_mulF;
  llvm::Function *raviV_op_divF;
  llvm::Function *raviV_op_shrF;
  llvm::Function *raviV_op_shlF;
  llvm::Function *raviV_op_borF;
  llvm::Function *raviV_op_bxorF;
  llvm::Function *raviV_op_bandF;
  llvm::Function *raviV_op_bnotF;
  llvm::Function *raviV_op_setupvaliF;
  llvm::Function *raviV_op_setupvalfF;
  llvm::Function *raviV_op_setupvalaiF;
  llvm::Function *raviV_op_setupvalafF;
  llvm::Function *raviV_op_setupvaltF;
  llvm::Function *raviV_gettable_sskeyF;
  llvm::Function *raviV_settable_sskeyF;
  llvm::Function *raviV_gettable_iF;
  llvm::Function *raviV_settable_iF;

  // array setters
  llvm::Function *raviH_set_intF;
  llvm::Function *raviH_set_floatF;

  llvm::Function *ravi_dump_valueF;
  llvm::Function *ravi_dump_stackF;
  llvm::Function *ravi_dump_stacktopF;
  llvm::Function *ravi_debug_traceF;

  // standard C functions
  llvm::Constant *printfFunc;
  llvm::Constant *fmodFunc;
  llvm::Constant *floorFunc;
  llvm::Constant *powFunc;

  // Jump targets in the function
  std::vector<RaviBranchDef> jmp_targets;

  // Load pointer to proto
  llvm::Value *proto;  // gep
  llvm::Instruction *proto_ptr;

  // Obtain pointer to Proto->k
  llvm::Value *proto_k;
  // Load pointer to Proto->k; this does not change
  llvm::Instruction *k_ptr;

  // Load L->ci
  llvm::Value *L_ci;  // This is the GEP for L->ci
  llvm::Instruction *ci_val;

  // Pointer to ci->u.l.base
  llvm::Value *Ci_base;  // This is the GEP for ci->u.l.base
  llvm::Instruction *base_ptr;

  // Pointer to LClosure
  llvm::Value *p_LClosure;
};

// This class is responsible for compiling Lua byte code
// to LLVM IR
class RaviCodeGenerator {
 public:
  RaviCodeGenerator(RaviJITState *jitState);

  // Compile given function if possible
  // The p->ravi_jit structure will be updated
  // Note that if a function fails to compile then
  // a flag is set so that it doesn't get compiled again
  bool compile(lua_State *L, Proto *p, std::shared_ptr<RaviJITModule> module,
               ravi_compile_options_t *options);

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
  std::unique_ptr<RaviJITFunction> create_function(
      Proto *p, std::shared_ptr<RaviJITModule> module,
      llvm::IRBuilder<> &builder, RaviFunctionDef *def);

  // Save proto->code[pc] into savedpc
  void emit_update_savedpc(RaviFunctionDef *def, int pc);

  llvm::CallInst *CreateCall1(llvm::IRBuilder<> *builder, llvm::Value *func,
                              llvm::Value *arg1);
  llvm::CallInst *CreateCall2(llvm::IRBuilder<> *builder, llvm::Value *func,
                              llvm::Value *arg1, llvm::Value *arg2);
  llvm::CallInst *CreateCall3(llvm::IRBuilder<> *builder, llvm::Value *func,
                              llvm::Value *arg1, llvm::Value *arg2,
                              llvm::Value *arg3);
  llvm::CallInst *CreateCall4(llvm::IRBuilder<> *builder, llvm::Value *func,
                              llvm::Value *arg1, llvm::Value *arg2,
                              llvm::Value *arg3, llvm::Value *arg4);
  llvm::CallInst *CreateCall5(llvm::IRBuilder<> *builder, llvm::Value *func,
                              llvm::Value *arg1, llvm::Value *arg2,
                              llvm::Value *arg3, llvm::Value *arg4,
                              llvm::Value *arg5);

  void attach_branch_weights(RaviFunctionDef *def, llvm::Instruction *ins,
                             uint32_t true_branch, uint32_t false_branch);

  void emit_raise_lua_error(RaviFunctionDef *def, const char *str);

  // Add extern declarations for Lua functions we need to call
  void emit_extern_declarations(RaviFunctionDef *def);

  // Retrieve the proto->sizep
  llvm::Instruction *emit_load_proto_sizep(RaviFunctionDef *def);

  // Store lua_Number or lua_Integer
  llvm::Instruction *emit_store_local_n(RaviFunctionDef *def, llvm::Value *src,
                                        llvm::Value *dest);

  // Load lua_Number or lua_Integer
  llvm::Instruction *emit_load_local_n(RaviFunctionDef *def, llvm::Value *src);

  // Store int
  llvm::Instruction *emit_store_local_int(RaviFunctionDef *def,
                                          llvm::Value *src, llvm::Value *dest);

  // Load int
  llvm::Instruction *emit_load_local_int(RaviFunctionDef *def,
                                         llvm::Value *src);

  // Test if value type is of specific Lua type
  // Value_type should have been obtained by emit_load_type()
  // The Lua typecode to check must be in lua_typecode
  // The return value is a boolean type as a result of
  // integer comparison result which is i1 in LLVM
  llvm::Value *emit_is_value_of_type(RaviFunctionDef *def,
                                     llvm::Value *value_type,
                                     LuaTypeCode lua_typecode,
                                     const char *varname = "value.typeof");

  // Test if value type is NOT of specific Lua type
  // Value_type should have been obtained by emit_load_type()
  // The Lua typecode to check must be in lua_typecode
  // The return value is a boolean type as a result of
  // integer comparison result which is i1 in LLVM
  llvm::Value *emit_is_not_value_of_type(
      RaviFunctionDef *def, llvm::Value *value_type, LuaTypeCode lua_typecode,
      const char *varname = "value.not.typeof");

  // Test if value type is NOT of specific Lua type class
  // i.e. variants are ignore
  // Value_type should have been obtained by emit_load_type()
  // The Lua typecode to check must be in lua_typecode
  // The return value is a boolean type as a result of
  // integer comparison result which is i1 in LLVM
  llvm::Value *emit_is_not_value_of_type_class(
      RaviFunctionDef *def, llvm::Value *value_type, LuaTypeCode lua_typecode,
      const char *varname = "value.not.typeof");

  // emit code for LClosure *cl = clLvalue(ci->func)
  // this is same as:
  // emit code for (LClosure *)ci->func->value_.gc
  llvm::Instruction *emit_gep_ci_func_value_gc_asLClosure(RaviFunctionDef *def);

  llvm::Value *emit_gep(RaviFunctionDef *def, const char *name, llvm::Value *s,
                        int arg1, int arg2);

  llvm::Value *emit_gep(RaviFunctionDef *def, const char *name, llvm::Value *s,
                        int arg1, int arg2, int arg3);
  llvm::Value *emit_gep(RaviFunctionDef *def, const char *name,
                        llvm::Value *ptr, llvm::Value *arg1, int arg2,
                        int arg3);
  llvm::Value *emit_gep(RaviFunctionDef *def, const char *name,
                        llvm::Value *ptr, llvm::Value *arg1, int arg2);

  // emit code for &ptr[offset]
  llvm::Value *emit_array_get(RaviFunctionDef *def, llvm::Value *ptr,
                              int offset);

  // emit code to load pointer L->ci->u.l.base
  void emit_load_base(RaviFunctionDef *def);

  llvm::Value *emit_load_ci(RaviFunctionDef *def);

  // emit code to obtain address of register at location A
  llvm::Value *emit_gep_register(RaviFunctionDef *def, int A);

  // emit code to obtain address of register or constant at location B
  llvm::Value *emit_gep_register_or_constant(RaviFunctionDef *def, int B);

  // emit code to obtain address of constant at locatiion B
  llvm::Value *emit_gep_constant(RaviFunctionDef *def, int B);

#if 0
  llvm::Value *emit_is_jit_call(RaviFunctionDef *def, llvm::Value *ci);
  llvm::Value *emit_ci_is_Lua(RaviFunctionDef *def, llvm::Value *ci);
#endif

  // obtain address of L->top
  llvm::Value *emit_gep_L_top(RaviFunctionDef *def);

  // (int)(L->top - ra)
  llvm::Value *emit_num_stack_elements(RaviFunctionDef *def, llvm::Value *ra);

  // Load a register or constant - if constant is int then return a direct LLVM
  // constant
  llvm::Value *emit_load_register_or_constant_i(RaviFunctionDef *def, int K);

  // Load a register or constant - if constant is int then return a direct LLVM
  // constant
  llvm::Value *emit_load_register_or_constant_n(RaviFunctionDef *def, int K);

  // emit code to load the lua_Number value from register
  llvm::Instruction *emit_load_reg_n(RaviFunctionDef *def, llvm::Value *ra);

  // emit code to load the lua_Integer value from register
  llvm::Instruction *emit_load_reg_i(RaviFunctionDef *def, llvm::Value *rb);

  // emit code to load the boolean value from register
  llvm::Instruction *emit_load_reg_b(RaviFunctionDef *def, llvm::Value *ra);

  // emit code to load the table value from register
  llvm::Instruction *emit_load_reg_h(RaviFunctionDef *def, llvm::Value *ra);

  // Gets the size of the hash table
  // This is the sizenode() macro in lobject.h
  llvm::Value *emit_table_get_hashsize(RaviFunctionDef *def,
                                       llvm::Value *table);

  // Gets the location of the hash node for given string key
  // return value is the offset into the node array
  llvm::Value *emit_table_get_hashstr(RaviFunctionDef *def, llvm::Value *table,
                                      TString *key);

  // Gets access to the Table's node array (t->node)
  llvm::Value *emit_table_get_nodearray(RaviFunctionDef *def,
                                        llvm::Value *table);

  // Given a pointer to table's node array (node = t->node) and
  // the location of the hashed key (index), this method retrieves the
  // type of the value stored at the node - return value is of type int
  // and is the type information stored in TValue->tt field.
  llvm::Value *emit_table_get_keytype(RaviFunctionDef *def, llvm::Value *node,
                                      llvm::Value *index);

  // Given a pointer to table's node array (node = t->node) and
  // the location of the hashed key (index), this method retrieves the
  // the string value stored at the node - return value is of type TString*
  llvm::Value *emit_table_get_strkey(RaviFunctionDef *def, llvm::Value *node,
                                     llvm::Value *index);

  // Given a pointer to table's node array (node = t->node) and
  // the location of the hashed key (index), this method retrieves the
  // the pointer to value stored at the node - return value is of type TValue*
  llvm::Value *emit_table_get_value(RaviFunctionDef *def, llvm::Value *node,
                                    llvm::Value *index);

  // Gets the size of the table's array part
  llvm::Value *emit_table_get_arraysize(RaviFunctionDef *def,
                                        llvm::Value *table);

  llvm::Value *emit_table_get_array(RaviFunctionDef *def, llvm::Value *table);

  llvm::Value *emit_table_no_metamethod(RaviFunctionDef *def,
                                        llvm::Value *table, TMS event);

  llvm::Instruction *emit_load_reg_s(RaviFunctionDef *def, llvm::Value *rb);

  // emit code to load pointer to int array
  llvm::Instruction *emit_load_reg_h_intarray(RaviFunctionDef *def,
                                              llvm::Instruction *ra);

  // emit code to load pointer to double array
  llvm::Instruction *emit_load_reg_h_floatarray(RaviFunctionDef *def,
                                                llvm::Instruction *ra);

  // emit code to store lua_Number value into register
  void emit_store_reg_n(RaviFunctionDef *def, llvm::Value *value,
                        llvm::Value *dest_ptr);
  void emit_store_reg_n_withtype(RaviFunctionDef *def, llvm::Value *value,
                                 llvm::Value *dest_ptr);

  // emit code to store lua_Integer value into register
  void emit_store_reg_i(RaviFunctionDef *def, llvm::Value *value,
                        llvm::Value *dest_ptr);
  void emit_store_reg_i_withtype(RaviFunctionDef *def, llvm::Value *value,
                                 llvm::Value *dest_ptr);

  // emit code to store bool value into register
  void emit_store_reg_b(RaviFunctionDef *def, llvm::Value *value,
                        llvm::Value *dest_ptr);
  void emit_store_reg_b_withtype(RaviFunctionDef *def, llvm::Value *value,
                                 llvm::Value *dest_ptr);

  // emit code to set the type in the register
  void emit_store_type_(RaviFunctionDef *def, llvm::Value *value, int type);

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
  llvm::Value *emit_gep_upvals(RaviFunctionDef *def, int offset);

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

  llvm::Instruction *emit_tointeger(RaviFunctionDef *def, llvm::Value *reg);

  llvm::Instruction *emit_tofloat(RaviFunctionDef *def, llvm::Value *reg);

  // L->top = ci->top
  // ci is passed as the def->ci_val may be stale
  void emit_refresh_L_top(RaviFunctionDef *def, llvm::Value *ci);

  // L->top = R(B)
  void emit_set_L_top_toreg(RaviFunctionDef *def, int B);

  void debug_printf(RaviFunctionDef *def, const char *str);

  void debug_printf1(RaviFunctionDef *def, const char *str, llvm::Value *arg1);

  void debug_printf2(RaviFunctionDef *def, const char *str, llvm::Value *arg1,
                     llvm::Value *arg2);

  void debug_printf3(RaviFunctionDef *def, const char *str, llvm::Value *arg1,
                     llvm::Value *arg2, llvm::Value *arg3);

  void debug_printf4(RaviFunctionDef *def, const char *str, llvm::Value *arg1,
                     llvm::Value *arg2, llvm::Value *arg3, llvm::Value *arg4);

  void emit_dump_stack(RaviFunctionDef *def, const char *str);
  void emit_dump_stacktop(RaviFunctionDef *def, const char *str);
  bool emit_debug_trace(RaviFunctionDef *def, int opCode, int pc);

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

  void emit_CONCAT(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_CLOSURE(RaviFunctionDef *def, int A, int Bx, int pc);

  void emit_VARARG(RaviFunctionDef *def, int A, int B, int pc);

  void emit_LOADNIL(RaviFunctionDef *def, int A, int B, int pc);

  void emit_LOADFZ(RaviFunctionDef *def, int A, int pc);

  void emit_LOADIZ(RaviFunctionDef *def, int A, int pc);

  void emit_LOADBOOL(RaviFunctionDef *def, int A, int B, int C, int j, int pc);

  // Code size priority so go via function calls
  void emit_ARITH_calls(RaviFunctionDef *def, int A, int B, int C, OpCode op,
                        TMS tms, int pc);

  // integer arith priority over floating
  void emit_ARITH_intpriority(RaviFunctionDef *def, int A, int B, int C,
                              OpCode op, TMS tms, int pc);

  // floating arith priority over integer
  void emit_ARITH_floatpriority(RaviFunctionDef *def, int A, int B, int C,
                                OpCode op, TMS tms, int pc);

  inline void emit_ARITH(RaviFunctionDef *def, int A, int B, int C, OpCode op,
                         TMS tms, int pc) {
#if RAVI_USE_LLVM_ARITH_FLOATPRIORITY
    emit_ARITH_floatpriority(def, A, B, C, op, tms, pc);
#else
    emit_ARITH_intpriority(def, A, B, C, op, tms, pc);
#endif
  }

  void emit_MOD(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_IDIV(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_POW(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_UNM(RaviFunctionDef *def, int A, int B, int pc);

  void emit_UNMF(RaviFunctionDef *def, int A, int B, int pc);

  void emit_UNMI(RaviFunctionDef *def, int A, int B, int pc);

  void emit_ADDFF(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_ADDFI(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_ADDII(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_SUBFF(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_SUBFI(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_SUBIF(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_SUBII(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_MULFF(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_MULFI(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_MULII(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_DIVFF(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_DIVFI(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_DIVIF(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_DIVII(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_LOADK(RaviFunctionDef *def, int A, int Bx, int pc);

  void emit_RETURN(RaviFunctionDef *def, int A, int B, int pc);

  void emit_CALL(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_JMP(RaviFunctionDef *def, int A, int j, int pc);

  void emit_iFORPREP(RaviFunctionDef *def, int A, int sBx, int step_one,
                     int pc);

  void emit_iFORLOOP(RaviFunctionDef *def, int A, int sBx, RaviBranchDef &b,
                     int step_one, int pc);

  void emit_FORPREP(RaviFunctionDef *def, int A, int sBx, int pc);

  void emit_FORLOOP(RaviFunctionDef *def, int A, int sBx, int pc);

  void emit_FORPREP2(RaviFunctionDef *def, int A, int sBx, int pc);

  void emit_FORLOOP2(RaviFunctionDef *def, int A, int sBx, RaviBranchDef &b,
                     int pc);

  void emit_MOVE(RaviFunctionDef *def, int A, int B, int pc);

  void emit_MOVEI(RaviFunctionDef *def, int A, int B, int pc);

  void emit_MOVEF(RaviFunctionDef *def, int A, int B, int pc);

  void emit_TOINT(RaviFunctionDef *def, int A, int pc);

  void emit_TOFLT(RaviFunctionDef *def, int A, int pc);

  void emit_LEN(RaviFunctionDef *def, int A, int B, int pc);

  void emit_SETTABLE(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_SETTABLE_I(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_SETTABLE_SK(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_GETTABLE(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_GETTABLE_S(RaviFunctionDef *def, int A, int B, int C, int pc,
                       TString *key);

  void emit_GETTABLE_SK(RaviFunctionDef *def, int A, int B, int C, int pc,
                        TString *key);

  void emit_GETTABLE_I(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_finish_GETTABLE(RaviFunctionDef *def, llvm::Value *phi,
                            llvm::Value *t, llvm::Value *ra, llvm::Value *rb,
                            llvm::Value *rc);

  void emit_SELF(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_SELF_S(RaviFunctionDef *def, int A, int B, int C, int pc,
                   TString *key);
  void emit_SELF_SK(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_common_GETTABLE_S(RaviFunctionDef *def, int A, int B, int C,
                              TString *key);

  void emit_common_GETTABLE_S_(RaviFunctionDef *def, int A, llvm::Value *rb,
                               int C, TString *key);

  void emit_GETUPVAL(RaviFunctionDef *def, int A, int B, int pc);

  void emit_SETUPVAL(RaviFunctionDef *def, int A, int B, int pc);

  void emit_SETUPVAL_Specific(RaviFunctionDef *def, int A, int B, int pc,
                              OpCode op, llvm::Function *f);

  void emit_GETTABUP(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_GETTABUP_SK(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_SETTABUP(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_SETTABUP_SK(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_NEWARRAYINT(RaviFunctionDef *def, int A, int pc);

  void emit_NEWARRAYFLOAT(RaviFunctionDef *def, int A, int pc);

  void emit_NEWTABLE(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_SETLIST(RaviFunctionDef *def, int A, int B, int C, int pc);

  void emit_NOT(RaviFunctionDef *def, int A, int B, int pc);

  // Emit code for OP_EQ, OP_LT and OP_LE
  // The callee parameter should be luaV_equalobj, luaV_lessthan and
  // luaV_lessequal
  // respectively
  // A, B, C must be operands of the OP_EQ/OP_LT/OP_LE instructions
  // j must be the jump target (offset of the code to which we need to jump to)
  // jA must be the A operand of the jump instruction
  void emit_EQ(RaviFunctionDef *def, int A, int B, int C, int j, int jA,
               llvm::Constant *callee, OpCode opCode, int pc);

  // OP_TEST is followed by a OP_JMP instruction - both are handled
  // together
  // A, B, C must be operands of the OP_TEST instruction
  // j must be the jump target (offset of the code to which we need to jump to)
  // jA must be the A operand of the jump instruction
  void emit_TEST(RaviFunctionDef *def, int A, int B, int C, int j, int jA,
                 int pc);

  // OP_TESTSET is followed by a OP_JMP instruction - both are handled
  // together
  // A, B, C must be operands of the OP_TESTSET instruction
  // j must be the jump target (offset of the code to which we need to jump to)
  // jA must be the A operand of the jump instruction
  void emit_TESTSET(RaviFunctionDef *def, int A, int B, int C, int j, int jA,
                    int pc);

  void emit_TFORCALL(RaviFunctionDef *def, int A, int B, int C, int j, int jA,
                     int pc);

  void emit_TFORLOOP(RaviFunctionDef *def, int A, int j, int pc);

  void emit_GETTABLE_AF(RaviFunctionDef *def, int A, int B, int C,
                        bool omitArrayGetRangeCheck, int pc);

  void emit_GETTABLE_AI(RaviFunctionDef *def, int A, int B, int C,
                        bool omitArrayGetRangeCheck, int pc);

  void emit_SETTABLE_AF(RaviFunctionDef *def, int A, int B, int C,
                        bool known_int, int pc);

  void emit_SETTABLE_AI(RaviFunctionDef *def, int A, int B, int C,
                        bool known_float, int pc);

  void emit_MOVEAI(RaviFunctionDef *def, int A, int B, int pc);

  void emit_MOVEAF(RaviFunctionDef *def, int A, int B, int pc);

  void emit_MOVETAB(RaviFunctionDef *def, int A, int B, int pc);

  void emit_TOARRAY(RaviFunctionDef *def, int A, int array_type_expected,
                    const char *errmsg, int pc);

  void emit_BITWISE_BINARY_OP(RaviFunctionDef *def, OpCode op, int A, int B,
                              int C, int pc);

  void emit_BITWISE_SHIFT_OP(RaviFunctionDef *def, OpCode op, int A, int B,
                             int C, int pc);

  void emit_BNOT_I(RaviFunctionDef *def, int A, int B, int pc);

  void emit_BOR_BXOR_BAND(RaviFunctionDef *def, OpCode op, int A, int B, int C,
                          int pc);

  void emit_BNOT(RaviFunctionDef *def, int A, int B, int pc);

  void emit_bitwise_shiftl(RaviFunctionDef *def, llvm::Value *ra, int B,
                           lua_Integer y);

 private:
  RaviJITState *jitState_;
  char temp_[31];  // for name
  int id_;         // for name
};
}

struct ravi_State {
  ravi::RaviJITState *jit;
  ravi::RaviCodeGenerator *code_generator;
};

#define RaviJIT(L) ((ravi::RaviJITState *)G(L)->ravi_state->jit)

#define RAVI_CODEGEN_FORPREP2 0

#endif

#endif

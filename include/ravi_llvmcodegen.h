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

  llvm::Type *C_intptr_t;
  llvm::Type *C_size_t;
  llvm::Type *C_ptrdiff_t;
  llvm::Type *C_int64_t;

  llvm::Type *lua_NumberT;
  llvm::Type *plua_NumberT;

  llvm::Type *lua_IntegerT;
  llvm::PointerType *plua_IntegerT;

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
  llvm::StructType *TableT;
  llvm::PointerType *pTableT;

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
  llvm::PointerType *pRaviJITProtoT;

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
  llvm::FunctionType *luaF_closeT;
  llvm::FunctionType *luaV_equalobjT;
  llvm::FunctionType *luaV_lessthanT;
  llvm::FunctionType *luaV_lessequalT;
  llvm::FunctionType *luaG_runerrorT;
  llvm::FunctionType *luaV_forlimitT;
  llvm::FunctionType *luaV_tonumberT;

  std::array<llvm::Constant *, 21> kInt;
  std::array<llvm::Constant *, 21> kluaInteger;

  llvm::Constant *kFalse;

  // To allow better optimization we need to decorate the
  // LLVM Load/Store instructions with type information.
  // For this we need to construct tbaa metadata
  llvm::MDBuilder mdbuilder;
  llvm::MDNode *tbaa_root;
  llvm::MDNode *tbaa_charT;
  llvm::MDNode *tbaa_shortT;
  llvm::MDNode *tbaa_intT;
  llvm::MDNode *tbaa_longlongT;
  llvm::MDNode *tbaa_pointerT;
  llvm::MDNode *tbaa_CallInfo_lT;
  llvm::MDNode *tbaa_CallInfoT;
  llvm::MDNode *tbaa_luaStateT;
  llvm::MDNode *tbaa_luaState_ciT;
  llvm::MDNode *tbaa_luaState_ci_baseT;
  llvm::MDNode *tbaa_CallInfo_funcT;
  llvm::MDNode *tbaa_CallInfo_func_LClosureT;
  llvm::MDNode *tbaa_LClosureT;
  llvm::MDNode *tbaa_LClosure_pT;
  llvm::MDNode *tbaa_ProtoT;
  llvm::MDNode *tbaa_Proto_kT;
  llvm::MDNode *tbaa_TValueT;
  llvm::MDNode *tbaa_TValue_nT;
  llvm::MDNode *tbaa_TValue_ttT;
  llvm::MDNode *tbaa_luaState_topT;
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

  // Module within which the function will be defined
  llvm::Module *module_;

  // Unique name for the function
  std::string name_;

  // The execution engine responsible for compiling the
  // module
  llvm::ExecutionEngine *engine_;

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
  llvm::Constant *luaF_closeF;
  llvm::Constant *luaV_equalobjF;
  llvm::Constant *luaV_lessthanF;
  llvm::Constant *luaV_lessequalF;
  llvm::Constant *luaG_runerrorF;
  llvm::Constant *luaV_forlimitF;
  llvm::Constant *luaV_tonumberF;

  // Jump targets in the function
  std::vector<llvm::BasicBlock *> jmp_targets;

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

  // Look for Lua bytecodes that are jump targets and allocate
  // a BasicBlock for each such target in def->jump_targets.
  // The BasicBlocks are not inserted into the function until later
  // but having them created allows rest of the code to insert
  // branch instructions
  void scan_jump_targets(RaviFunctionDef *def, Proto *p);

  void link_block(RaviFunctionDef *def, int pc);

  void emit_LOADK(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int Bx);

  void emit_RETURN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                   int A, int B);

  void emit_JMP(RaviFunctionDef *def, int j);

  void emit_FORPREP(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                    int A, int sBx);

  void emit_FORLOOP(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                    int A, int sBx);

  void emit_MOVE(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
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

private:
  RaviJITStateImpl *jitState_;
  char temp_[31]; // for name
  int id_;        // for name
};
}

#endif
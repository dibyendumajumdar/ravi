#include "ravijit.h"
#include "ravillvm.h"

#ifdef __cplusplus
extern "C" {
#endif

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
#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"

#ifdef __cplusplus
}
#endif

#include <iterator>
#include <type_traits>

namespace ravi {

static volatile int init = 0;

RaviJITState::RaviJITState() : context_(llvm::getGlobalContext()) {
  // Unless following three lines are not executed then
  // ExecutionEngine cannot be created
  if (init == 0) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    init++;
  }
  triple_ = llvm::sys::getProcessTriple();
#ifdef _WIN32
  // On Windows we get error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set -elf as the object
  // format
  triple_ += "-elf";
#endif
}

RaviJITState::~RaviJITState() {
  std::vector<RaviJITFunction *> todelete;
  for (auto &f = std::begin(functions_); f != std::end(functions_); f++) {
    todelete.push_back(f->second);
  }
  for (int i = 0; i < todelete.size(); i++) {
    delete todelete[i];
  }
}

void RaviJITState::addGlobalSymbol(const std::string &name, void *address) {
  llvm::sys::DynamicLibrary::AddSymbol(name, address);
}

void RaviJITState::dump() {
  for (auto f : functions_) {
    f.second->dump();
  }
}

RaviJITFunction *
RaviJITState::createFunction(llvm::FunctionType *type,
                             llvm::GlobalValue::LinkageTypes linkage,
                             const std::string &name) {
  // MCJIT treats each module as a compilation unit
  // To enabe function level life cycle we create a
  // module per function
  std::string moduleName = "ravi_module_" + name;
  llvm::Module *module = new llvm::Module(moduleName, context_);
#if defined(_WIN32)
  // On Windows we get error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set
  // -elf as the object format
  module->setTargetTriple(triple_);
#endif
  RaviJITFunction *f = new RaviJITFunction(this, module, type, linkage, name);
  functions_[name] = f;
  return f;
}

void RaviJITState::deleteFunction(const std::string &name) {
  functions_.erase(name);
  // This is called when RaviJITFunction is deleted
}

RaviJITFunction::RaviJITFunction(RaviJITState *owner, llvm::Module *module,
                                 llvm::FunctionType *type,
                                 llvm::GlobalValue::LinkageTypes linkage,
                                 const std::string &name)
    : owner_(owner), module_(module), name_(name), engine_(nullptr),
      function_(nullptr), ptr_(nullptr) {
  function_ = llvm::Function::Create(type, linkage, name, module);
  std::string errStr;
  engine_ = llvm::EngineBuilder(module)
                .setEngineKind(llvm::EngineKind::JIT)
                .setUseMCJIT(true)
                .setErrorStr(&errStr)
                .create();
  if (!engine_) {
    fprintf(stderr, "Could not create ExecutionEngine: %s\n", errStr.c_str());
    return;
  }
}

RaviJITFunction::~RaviJITFunction() {
  // Remove this function from parent
  owner_->deleteFunction(name_);
  if (engine_)
    delete engine_;
  else if (module_)
    delete module_;
  // Check - we assume here that deleting engine deletes the module
  // and function, and deleting module deletes function
}

void *RaviJITFunction::compile() {

#if 0
  // Create a function pass manager for this engine
  llvm::FunctionPassManager *FPM = new llvm::FunctionPassManager(OpenModule);

  // Set up the optimizer pipeline.  Start with registering info about how the
  // target lays out data structures.
  OpenModule->setDataLayout(NewEngine->getDataLayout());
  FPM->add(new llvm::DataLayoutPass());
  // Provide basic AliasAnalysis support for GVN.
  FPM->add(llvm::createBasicAliasAnalysisPass());
  // Promote allocas to registers.
  FPM->add(llvm::createPromoteMemoryToRegisterPass());
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  FPM->add(llvm::createInstructionCombiningPass());
  // Reassociate expressions.
  FPM->add(llvm::createReassociatePass());
  // Eliminate Common SubExpressions.
  FPM->add(llvm::createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  FPM->add(llvm::createCFGSimplificationPass());
  FPM->doInitialization();

  // For each function in the module
  llvm::Module::iterator it;
  llvm::Module::iterator end = OpenModule->end();
  for (it = OpenModule->begin(); it != end; ++it) {
    // Run the FPM on this function
    FPM->run(*it);
  }

  // We don't need this anymore
  delete FPM;
#endif

  if (ptr_)
    return ptr_;
  if (!function_ || !engine_)
    return NULL;

  // Upon creation, MCJIT holds a pointer to the Module object
  // that it received from EngineBuilder but it does not immediately
  // generate code for this module. Code generation is deferred
  // until either the MCJIT::finalizeObject method is called
  // explicitly or a function such as MCJIT::getPointerToFunction
  // is called which requires the code to have been generated.
  engine_->finalizeObject();
  ptr_ = engine_->getPointerToFunction(function_);
  return ptr_;
}

llvm::Constant *RaviJITFunction::addExternFunction(llvm::FunctionType *type,
                                                   void *address,
                                                   const std::string &name) {
  llvm::Function *f = llvm::Function::Create(
      type, llvm::Function::ExternalLinkage, name, module_);
  // We should have been able to call
  // engine_->addGlobalMapping() but this doesn't work
  // See http://lists.cs.uiuc.edu/pipermail/llvmdev/2014-April/071856.html
  // See bug report http://llvm.org/bugs/show_bug.cgi?id=20656
  // following will call DynamicLibrary::AddSymbol
  owner_->addGlobalSymbol(name, address);
  return f;
}

void RaviJITFunction::dump() { module_->dump(); }

struct LuaLLVMTypes {

  void init(llvm::LLVMContext &context);

  llvm::Type *C_intptr_t;
  llvm::Type *C_size_t;
  llvm::Type *C_ptrdiff_t;

  llvm::Type *lua_NumberT;
  llvm::Type *lua_IntegerT;
  llvm::Type *lua_UnsignedT;
  llvm::Type *lua_KContextT;

  llvm::Type *lua_CFunctionT;
  llvm::Type *lua_KFunctionT;

  llvm::Type *l_memT;
  llvm::Type *lu_memT;

  llvm::Type *lu_byteT;
  llvm::Type *L_UmaxalignT;

  llvm::Type *GCObjectT;
};

void LuaLLVMTypes::init(llvm::LLVMContext &context) {

  static_assert(std::is_floating_point<lua_Number>::value &&
                    sizeof(lua_Number) == sizeof(double),
                "lua_Number is not a double");
  lua_NumberT = llvm::Type::getDoubleTy(context);

  static_assert(std::is_integral<lua_Integer>::value, "lua_Integer is not an integer type");
  lua_IntegerT = llvm::Type::getIntNTy(context, sizeof(lua_Integer)*8);

  static_assert(sizeof(lua_Integer) == sizeof(lua_Unsigned), "lua_Integer and lua_Unsigned are of different size");
  lua_UnsignedT = lua_IntegerT;

  C_intptr_t = llvm::Type::getIntNTy(context, sizeof(intptr_t) * 8);
  C_size_t = llvm::Type::getIntNTy(context, sizeof(size_t) * 8);
  C_ptrdiff_t = llvm::Type::getIntNTy(context, sizeof(ptrdiff_t) * 8);

  static_assert(sizeof(size_t) == sizeof(lu_mem), "lu_mem size is not same as size_t");
  lu_memT = C_size_t;

  static_assert(sizeof(ptrdiff_t) == sizeof(l_mem), "l_mem size is not same as ptrdiff_t");
  l_memT = C_ptrdiff_t;

  static_assert(sizeof(L_Umaxalign) == sizeof(double), "L_Umaxalign is not same size as double");
  L_UmaxalignT = llvm::Type::getDoubleTy(context);
}

}

#ifdef __cplusplus
extern "C" {
#endif

struct ravi_State {
  ravi::RaviJITState *jit;
};

int raviV_initjit(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state != NULL)
    return -1;
  ravi_State *jit = (ravi_State *)calloc(1, sizeof(ravi_State));
  jit->jit = new ravi::RaviJITState();
  G->ravi_state = jit;
  return 0;
}

void raviV_close(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL)
    return;
  delete G->ravi_state->jit;
  free(G->ravi_state);
}

#ifdef __cplusplus
}
#endif

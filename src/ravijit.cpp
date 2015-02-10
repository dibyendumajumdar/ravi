#include "ravijit.h"
#include "ravillvm.h"

#include <iterator>

static volatile int init = 0;

RaviJITState::RaviJITState() : context_(llvm::getGlobalContext()) {
  // Looks like unless following three lines are not executed then
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

RaviJITFunction *
RaviJITState::createFunction(llvm::FunctionType *type,
                             llvm::GlobalValue::LinkageTypes linkage,
                             const std::string &name) {
  // Otherwise create a new Module.
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

class HelpingMemoryManager : public llvm::SectionMemoryManager {
  HelpingMemoryManager(const HelpingMemoryManager &) LLVM_DELETED_FUNCTION;
  void operator=(const HelpingMemoryManager &) LLVM_DELETED_FUNCTION;

public:
  HelpingMemoryManager(RaviJITState *Helper) : MasterHelper(Helper) {}
  virtual ~HelpingMemoryManager() {}

  /// This method returns the address of the specified function.
  /// Our implementation will attempt to find functions in other
  /// modules associated with the MCJITHelper to cross link functions
  /// from one generated module to another.
  ///
  /// If \p AbortOnFailure is false and no function with the given name is
  /// found, this function returns a null pointer. Otherwise, it prints a
  /// message to stderr and aborts.
  virtual void *getPointerToNamedFunction(const std::string &Name,
                                          bool AbortOnFailure = true);

private:
  RaviJITState *MasterHelper;
};

void *HelpingMemoryManager::getPointerToNamedFunction(const std::string &Name,
                                                      bool AbortOnFailure) {
  // Try the standard symbol resolution first, but ask it not to abort.
  void *pfn = RTDyldMemoryManager::getPointerToNamedFunction(Name, true);
  // TODO
  return pfn;
}

void RaviJITState::deleteFunction(const std::string &name) {
  functions_.erase(name);
}

RaviJITFunction::RaviJITFunction(RaviJITState *owner, llvm::Module *module,
                                 llvm::FunctionType *type,
                                 llvm::GlobalValue::LinkageTypes linkage,
                                 const std::string &name)
    : owner_(owner), module_(module), name_(name), engine_(nullptr), function_(nullptr),
      ptr_(nullptr) {
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
  // Don't know why but engine_->addGlobalMapping() doesn't work
  // following seems to work though
  llvm::sys::DynamicLibrary::AddSymbol(name, address);
  return f;
}

void RaviJITState::dump() {}

void RaviJITFunction::dump() { module_->dump(); }

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

struct ravi_State {
  RaviJITState *jit;
};

int raviV_initjit(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state != NULL)
    return -1;
  ravi_State *jit = (ravi_State *)calloc(1, sizeof(ravi_State));
  jit->jit = new RaviJITState();
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

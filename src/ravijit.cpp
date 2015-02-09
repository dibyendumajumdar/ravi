#include "ravijit.h"
#include "ravillvm.h"

// FIXME: Obviously we can do better than this
static std::string GenerateUniqueName(const char *root) {
  static int i = 0;
  char s[16];
  sprintf(s, "%s%d", root, i++);
  std::string S = s;
  return S;
}

static std::string MakeLegalFunctionName(std::string Name) {
  std::string NewName;
  if (!Name.length())
    return GenerateUniqueName("anon_func_");

  // Start with what we have
  NewName = Name;

  // Look for a numberic first character
  if (NewName.find_first_of("0123456789") == 0) {
    NewName.insert(0, 1, 'n');
  }

  // Replace illegal characters with their ASCII equivalent
  std::string legal_elements =
      "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  size_t pos;
  while ((pos = NewName.find_first_not_of(legal_elements)) !=
         std::string::npos) {
    char old_c = NewName.at(pos);
    char new_str[16];
    sprintf(new_str, "%d", (int)old_c);
    NewName = NewName.replace(pos, 1, new_str);
  }

  return NewName;
}

static volatile int init = 0;

class HelpingMemoryManager : public llvm::SectionMemoryManager {
  HelpingMemoryManager(const HelpingMemoryManager &) LLVM_DELETED_FUNCTION;
  void operator=(const HelpingMemoryManager &) LLVM_DELETED_FUNCTION;

public:
  HelpingMemoryManager(RaviJITState *Helper) : MasterHelper(Helper) {}
  virtual ~HelpingMemoryManager() {}

  /// This method returns the address of the specified symbol.
  /// Our implementation will attempt to find symbols in other
  /// modules associated with the MCJITHelper to cross link symbols
  /// from one generated module to another.
  virtual uint64_t getSymbolAddress(const std::string &Name) override;

private:
  RaviJITState *MasterHelper;
};

uint64_t HelpingMemoryManager::getSymbolAddress(const std::string &Name) {
  uint64_t FnAddr = SectionMemoryManager::getSymbolAddress(Name);
  if (FnAddr)
    return FnAddr;

  uint64_t HelperFun = (uint64_t)MasterHelper->getSymbolAddress(Name);
  if (!HelperFun)
    fprintf(stderr, "Program used extern function '%s' "
                    " which could not be resolved!",
            Name.c_str());
  return HelperFun;
}

RaviJITState::RaviJITState()
    : Context(llvm::getGlobalContext()), OpenModule(nullptr) {
  // Looks like unless following three lines are not executed then
  // ExecutionEngine cannot be created
  if (init == 0) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    init++;
  }

  triple = llvm::sys::getProcessTriple();
#ifdef _WIN32
  // On Windows we get error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set -elf as the object
  // format
  triple += "-elf";
#endif
}

RaviJITState::~RaviJITState() {
  if (OpenModule)
    delete OpenModule;
  EngineVector::iterator begin = Engines.begin();
  EngineVector::iterator end = Engines.end();
  EngineVector::iterator it;
  for (it = begin; it != end; ++it)
    delete *it;
}

llvm::Module *RaviJITState::getModuleForNewFunction() {
  // If we have a Module that hasn't been JITed, use that.
  if (OpenModule)
    return OpenModule;

  // Otherwise create a new Module.
  std::string ModName = GenerateUniqueName("ravi_module_");
  llvm::Module *M = new llvm::Module(ModName, Context);
#if defined(_WIN32)
  // On Windows we get error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set
  // -elf as the object format
  M->setTargetTriple(triple);
#endif
  Modules.push_back(M);
  OpenModule = M;
  return M;
}

void *RaviJITState::getPointerToFunction(llvm::Function *F) {
#if 0
  // See if an existing instance of MCJIT has this function.
  EngineVector::iterator begin = Engines.begin();
  EngineVector::iterator end = Engines.end();
  EngineVector::iterator it;
  for (it = begin; it != end; ++it) {
    void *P = (*it)->getPointerToFunction(F);
    if (P)
      return P;
  }
#endif

  // If we didn't find the function, see if we can generate it.
  if (OpenModule) {
    llvm::Module *m = F->getParent();
    if (m != OpenModule) {
      fprintf(stderr, "Module does not match!\n");
    }
    std::string ErrStr;
    llvm::ExecutionEngine *NewEngine =
        llvm::EngineBuilder(OpenModule)
            .setEngineKind(llvm::EngineKind::JIT)
            .setUseMCJIT(true)
            .setMCJITMemoryManager(new HelpingMemoryManager(this))
            .setErrorStr(&ErrStr)
            .create();
    if (!NewEngine) {
      fprintf(stderr, "Could not create ExecutionEngine: %s\n", ErrStr.c_str());
      return NULL;
    }

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

    OpenModule = NULL;
    Engines.push_back(NewEngine);
    // Upon creation, MCJIT holds a pointer to the Module object
    // that it received from EngineBuilder but it does not immediately
    // generate code for this module. Code generation is deferred
    // until either the MCJIT::finalizeObject method is called
    // explicitly or a function such as MCJIT::getPointerToFunction
    // is called which requires the code to have been generated.
    NewEngine->finalizeObject();
    return NewEngine->getPointerToFunction(F);
  }
  return NULL;
}

void *RaviJITState::getSymbolAddress(const std::string &Name) {
  // Look for the symbol in each of our execution engines.
  EngineVector::iterator begin = Engines.begin();
  EngineVector::iterator end = Engines.end();
  EngineVector::iterator it;
  for (it = begin; it != end; ++it) {
    uint64_t FAddr = (*it)->getFunctionAddress(Name);
    if (FAddr) {
      return (void *)FAddr;
    }
  }
  return NULL;
}

void RaviJITState::dump() {
  ModuleVector::iterator begin = Modules.begin();
  ModuleVector::iterator end = Modules.end();
  ModuleVector::iterator it;
  for (it = begin; it != end; ++it)
    (*it)->dump();
}

class RaviJITFunction {};

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

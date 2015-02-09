#ifndef RAVI_LLVM_H
#define RAVI_LLVM_H

#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Host.h"
#include "llvm/Transforms/Scalar.h"

#include <cstdio>
#include <string>
#include <vector>
#include <memory>

#if defined(_WIN32) && defined(LUA_BUILD_AS_DLL)
#define RAVI_API __declspec(dllexport)
#else                                          
#define RAVI_API __declspec(dllimport)
#endif                                                     

// Most of the code below is based on the very useful
// blog post:
// http://blog.llvm.org/2013/07/using-mcjit-with-kaleidoscope-tutorial.html
class RAVI_API RaviJITState {
  typedef std::vector<llvm::Module *> ModuleVector;
  typedef std::vector<llvm::ExecutionEngine *> EngineVector;

  llvm::LLVMContext &Context;
  llvm::Module *OpenModule;
  ModuleVector Modules;
  EngineVector Engines;
  std::string triple;

public:
  RaviJITState();
  ~RaviJITState();
  llvm::Module *getModuleForNewFunction();
  void *getPointerToFunction(llvm::Function *F);
  void dump();
  void releaseFunction(llvm::Function *F);
  llvm::LLVMContext& getContext() { return Context; }

  // For internal use only
  void *getSymbolAddress(const std::string &Name);
};

#endif
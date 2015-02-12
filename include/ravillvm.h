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
#include "llvm/Support/DynamicLibrary.h"
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

namespace ravi {

class RAVI_API RaviJITState;

// Represents a JITed or JITable function
// Each function gets its own module and execution engine - this
// may change in future
// The advantage is that we can drop the function when the corresponding
// Lua object is garbage collected - with MCJIT this is not possible
// to do at function level
class RAVI_API RaviJITFunction {

  // The function is tracked by RaviJITState so we need to
  // tell RaviJITState when this function dies
  RaviJITState *owner_;

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
  RaviJITFunction(RaviJITState *owner, llvm::Module *module,
                  llvm::FunctionType *type,
                  llvm::GlobalValue::LinkageTypes linkage,
                  const std::string &name);
  ~RaviJITFunction();

  // Compile the function if not already compiled and
  // return pointer to function
  void *compile();

  // Add declaration for an extern function that is not
  // loaded dynamically - i.e., is part of the the executable
  // and therefore not visible at runtime by name
  llvm::Constant *addExternFunction(llvm::FunctionType *type, void *address,
                                    const std::string &name);

  const std::string &name() const { return name_; }
  llvm::Function *function() const { return function_; }
  llvm::Module *module() const { return module_; }
  llvm::ExecutionEngine *engine() const { return engine_; }
  RaviJITState *owner() const { return owner_; }
  void dump();
};

// Ravi's JIT State
// All of the JIT information is held here
class RAVI_API RaviJITState {
  // map of names to functions
  std::map<std::string, RaviJITFunction *> functions_;
  llvm::LLVMContext &context_;
  // The triple represents the host target
  std::string triple_;

public:
  RaviJITState();
  ~RaviJITState();

  // Create a function of specified type and linkage
  RaviJITFunction *createFunction(llvm::FunctionType *type,
                                  llvm::GlobalValue::LinkageTypes linkage,
                                  const std::string &name);

  // Stop tracking the named function - note that
  // the function is assumed to be destroyed by the user
  void deleteFunction(const std::string &name);

  void addGlobalSymbol(const std::string &name, void *address);

  void dump();
  llvm::LLVMContext &context() { return context_; }
};
}

#endif
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
#ifndef RAVI_LLVM_H
#define RAVI_LLVM_H

#include "llvm/Config/llvm-config.h"

#if (LLVM_VERSION_MAJOR != 3 || LLVM_VERSION_MINOR < 5)
#error Unsupported LLVM version
#endif

#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Metadata.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Transforms/Scalar.h"

#include <cstdio>
#include <string>
#include <vector>
#include <memory>

#if defined(_WIN32) 
#if defined(LUA_BUILD_AS_DLL)
#define RAVI_API __declspec(dllexport)
#else
#define RAVI_API __declspec(dllimport)
#endif
#else
#define RAVI_API
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
public:
  virtual ~RaviJITFunction() {}

  // Compile the function if not already compiled and
  // return pointer to function
  virtual void *compile() = 0;

  // Add declaration for an extern function that is not
  // loaded dynamically - i.e., is part of the the executable
  // and therefore not visible at runtime by name
  virtual llvm::Constant *addExternFunction(llvm::FunctionType *type, void *address,
                                    const std::string &name) = 0;

  virtual const std::string &name() const = 0;
  virtual llvm::Function *function() const = 0;
  virtual llvm::Module *module() const = 0;
  virtual llvm::ExecutionEngine *engine() const = 0;
  virtual RaviJITState *owner() const = 0;
  virtual void dump() = 0;

protected:
  RaviJITFunction() {}
private:
  RaviJITFunction(const RaviJITFunction&) = delete;
  RaviJITFunction& operator=(const RaviJITFunction&) = delete;
};

// Ravi's JIT State
// All of the JIT information is held here
class RAVI_API RaviJITState {
public:
  virtual ~RaviJITState() {}

  // Create a function of specified type and linkage
  virtual RaviJITFunction *createFunction(llvm::FunctionType *type,
                                  llvm::GlobalValue::LinkageTypes linkage,
                                  const std::string &name) = 0;

  virtual void dump() = 0;
  virtual llvm::LLVMContext &context() = 0;
  virtual bool is_auto() const = 0;
  virtual void set_auto(bool value) = 0;
protected:
  RaviJITState() {}
private:
  RaviJITState(const RaviJITState&) = delete;
  RaviJITState& operator=(const RaviJITState&) = delete;
};

class RAVI_API RaviJITStateFactory {
public:
  static std::unique_ptr<RaviJITState> newJITState();
};

}

#endif

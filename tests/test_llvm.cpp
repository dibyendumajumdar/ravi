#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ADT/Triple.h"

#include <cstdio>
#include <string>
#include <vector>
#include <memory>

// This mirrors the Lua GCObject structure in lobject.h
typedef struct RaviGCObject_st {
  struct RaviGCObject_st *next;
  unsigned char b1;
  unsigned char b2;
} RaviGCObject_t;

// Our prototype for the JITted function
typedef int (*myfunc_t)(RaviGCObject_t *);

int main() {

  // Looks like unless following three lines are not executed then
  // ExecutionEngine cannot be created
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  // Get context - need not be global
  llvm::LLVMContext &context = llvm::getGlobalContext();

  // Module is the translation unit
  std::unique_ptr<llvm::Module> theModule =
      std::unique_ptr<llvm::Module>(new llvm::Module("ravi", context));
  llvm::Module *module = theModule.get();
  llvm::IRBuilder<> builder(context);

#ifdef _WIN32
  // On Windows we get error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format
  // is not supported and therefore we need to set -elf as the object format
  module->setTargetTriple("x86_64-pc-win32-elf");
#endif

  // We will create the following C code
  //
  // extern int printf(const char *, ...);
  //
  // struct RaviObject {
  //   struct RaviObject *next;
  //   unsigned char a;
  //   unsigned char b;
  // };
  //
  // void testfunc(struct RaviObject *obj) {
  //   printf("value = %d\n", obj->a);
  // }
  //

  /* create a GCObject structure as defined in lobject.h */
  llvm::StructType *myt = llvm::StructType::create(context, "RaviGCObject");
  llvm::PointerType *pmyt = llvm::PointerType::get(myt, 0);
  std::vector<llvm::Type *> elements;
  elements.push_back(pmyt);
  elements.push_back(llvm::Type::getInt8Ty(context));
  elements.push_back(llvm::Type::getInt8Ty(context));
  myt->setBody(elements);
  myt->dump();

  // Create printf declaration
  std::vector<llvm::Type *> args;
  args.push_back(llvm::Type::getInt8PtrTy(context));
  // accepts a char*, is vararg, and returns int
  llvm::FunctionType *printfType =
      llvm::FunctionType::get(builder.getInt32Ty(), args, true);
  llvm::Constant *printfFunc =
      module->getOrInsertFunction("printf", printfType);

  // Create the testfunc()
  args.clear();
  args.push_back(pmyt);
  llvm::FunctionType *funcType =
      llvm::FunctionType::get(builder.getInt32Ty(), args, false);
  llvm::Function *mainFunc = llvm::Function::Create(
      funcType, llvm::Function::ExternalLinkage, "testfunc", module);
  llvm::BasicBlock *entry =
      llvm::BasicBlock::Create(context, "entrypoint", mainFunc);
  builder.SetInsertPoint(entry);

  // printf format string
  llvm::Value *formatStr = builder.CreateGlobalStringPtr("value = %d\n");

  // Get the first argument which is RaviGCObject *
  auto argiter = mainFunc->arg_begin();
  llvm::Value *arg1 = argiter++;
  arg1->setName("obj");

  // Now we need a GEP for the second field in RaviGCObject
  std::vector<llvm::Value *> values;
  llvm::APInt zero(32, 0);
  llvm::APInt one(32, 1);
  // This is the array offset into RaviGCObject*
  values.push_back(
      llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(context), zero));
  // This is the field offset
  values.push_back(
      llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(context), one));

  // Create the GEP value
  llvm::Value *arg1_a = builder.CreateGEP(arg1, values, "ptr");

  // Now retrieve the data from the pointer address
  llvm::Value *tmp1 = builder.CreateLoad(arg1_a, "a");
  // As the retrieved value is a byte - convert to int i
  llvm::Value *tmp2 =
      builder.CreateZExt(tmp1, llvm::Type::getInt32Ty(context), "i");

  // Call the printf function
  values.clear();
  values.push_back(formatStr);
  values.push_back(tmp2);
  builder.CreateCall(printfFunc, values);

  // return i
  builder.CreateRet(tmp2);
  module->dump();

  std::string errStr;
  auto engine = llvm::EngineBuilder(module)
                    .setErrorStr(&errStr)
                    .setEngineKind(llvm::EngineKind::JIT)
                    .setUseMCJIT(true)
                    .create();
  if (!engine) {
    llvm::errs() << "Failed to construct ExecutionEngine: " << errStr << "\n";
    return 1;
  }
  RaviGCObject_t obj = {0, 42, 65};

  std::string funcname = "testfunc";
  myfunc_t funcptr = (myfunc_t)engine->getFunctionAddress(funcname);
  if (funcptr == nullptr) {
    llvm::errs() << "Failed to obtain compiled function\n";
    return 1;
  }

  int ans = funcptr(&obj);

  printf("Answer is %d\n", ans);
  return ans == 42 ? 0 : 1;
}

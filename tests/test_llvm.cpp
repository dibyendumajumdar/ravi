#include "ravillvm.h"

#include <iostream>

// This file contains a basic test case that covers following:
// Creating a function that takes pointer to struct as argument
// The function gets value from one of the fields in the struct
// And returns it
// The equivalent C program is:
//
// extern int printf(const char *, ...);
//
// struct GCObject {
//   struct GCObject *next;
//   unsigned char a;
//   unsigned char b;
// };
//
// int testfunc(struct GCObject *obj) {
//   printf("value = %d\n", obj->a);
//   return obj->a;
// }

// This mirrors the Lua GCObject structure in lobject.h
typedef struct RaviGCObject {
  struct RaviGCObject *next;
  unsigned char b1;
  unsigned char b2;
} RaviGCObject;

// Our prototype for the JITted function
typedef int (*myfunc_t)(RaviGCObject *);

int test1() {
  // Get global context - not sure what the impact is of sharing
  // the global context
  llvm::LLVMContext &context = llvm::getGlobalContext();

  // Module is the translation unit
  std::unique_ptr<llvm::Module> theModule =
      std::unique_ptr<llvm::Module>(new llvm::Module("ravi", context));
  llvm::Module *module = theModule.get();
  llvm::IRBuilder<> builder(context);

#ifdef _WIN32
  // On Windows we get error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set -elf as the object
  // format
  auto triple = llvm::sys::getProcessTriple();
  // module->setTargetTriple("x86_64-pc-win32-elf");
  module->setTargetTriple(triple + "-elf");
#endif

  // create a GCObject structure as defined in lobject.h
  llvm::StructType *structType =
      llvm::StructType::create(context, "RaviGCObject");
  llvm::PointerType *pstructType =
      llvm::PointerType::get(structType, 0); // pointer to RaviGCObject
  std::vector<llvm::Type *> elements;
  elements.push_back(pstructType);
  elements.push_back(llvm::Type::getInt8Ty(context));
  elements.push_back(llvm::Type::getInt8Ty(context));
  structType->setBody(elements);
  structType->dump();

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
  args.push_back(pstructType);
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

  // Lets create the MCJIT engine
  std::string errStr;
#if LLVM_VERSION_MINOR > 5
  std::unique_ptr<llvm::Module> module_(module);
  auto engine = llvm::EngineBuilder(std::move(module_))
                    .setErrorStr(&errStr)
                    .setEngineKind(llvm::EngineKind::JIT)
                    .create();
#else
  auto engine = llvm::EngineBuilder(module)
    .setErrorStr(&errStr)
    .setEngineKind(llvm::EngineKind::JIT)
    .setUseMCJIT(true)
    .create();
#endif
  if (!engine) {
    std::cerr << "Failed to construct MCJIT ExecutionEngine: " << errStr
                 << "\n";
    return 1;
  }

  // Now lets compile our function into machine code
  std::string funcname = "testfunc";
  myfunc_t funcptr = (myfunc_t)engine->getFunctionAddress(funcname);
  if (funcptr == nullptr) {
    std::cerr << "Failed to obtain compiled function\n";
    return 1;
  }

  // Run the function and test results.
  RaviGCObject obj = {NULL, 42, 65};
  int ans = funcptr(&obj);
  printf("The answer is %d\n", ans);
  return ans == 42 ? 0 : 1;
}

extern "C" int mytest(RaviGCObject *obj) {
  printf("value = %d\n", obj->b1);
  return obj->b1;
}

// This version of the test calls mytest() rather than
// printf() as in test1(). Also we use RaviJITState and related
// infrastructure
int test2() {

  auto jitStatePtr = ravi::RaviJITStateFactory::newJITState();
  ravi::RaviJITState &jitState = *jitStatePtr.get();

  jitState.dump();

  llvm::LLVMContext &context = jitState.context();
  llvm::IRBuilder<> builder(context);

  // create a GCObject structure as defined in lobject.h
  llvm::StructType *structType =
      llvm::StructType::create(context, "RaviGCObject");
  llvm::PointerType *pstructType =
      llvm::PointerType::get(structType, 0); // pointer to RaviGCObject
  std::vector<llvm::Type *> elements;
  elements.push_back(pstructType);
  elements.push_back(llvm::Type::getInt8Ty(context));
  elements.push_back(llvm::Type::getInt8Ty(context));
  structType->setBody(elements);
  structType->dump();

  // Create declaration for mytest
  // int mytest(RaviGCObject *obj)
  std::vector<llvm::Type *> args;
  args.push_back(pstructType);
  llvm::FunctionType *mytestFuncType =
      llvm::FunctionType::get(builder.getInt32Ty(), args, false);

  // Create the testfunc()
  args.clear();
  args.push_back(pstructType);
  llvm::FunctionType *funcType =
      llvm::FunctionType::get(builder.getInt32Ty(), args, false);
  ravi::RaviJITFunction *func = jitState.createFunction(
      funcType, llvm::Function::ExternalLinkage, "testfunc");

  llvm::Function *mainFunc = func->function();
  llvm::BasicBlock *entry =
      llvm::BasicBlock::Create(context, "entrypoint", mainFunc);
  builder.SetInsertPoint(entry);

  // Get the first argument which is RaviGCObject *
  auto argiter = mainFunc->arg_begin();
  llvm::Value *arg1 = argiter++;
  arg1->setName("obj");

  // Add an extern int mytest(RaviGCObject *obj) and link this
  // to mytest()
  llvm::Constant *mytestFunc =
      func->addExternFunction(mytestFuncType, reinterpret_cast<void *>(&mytest), "mytest");

  // Call the mytest() function
  std::vector<llvm::Value *> values;
  values.push_back(arg1);
  llvm::Value *tmp2 = builder.CreateCall(mytestFunc, values, "i");

  // return i
  builder.CreateRet(tmp2);
  func->dump();

  // Now lets compile our function into machine code
  myfunc_t funcptr = (myfunc_t)func->compile();
  if (funcptr == nullptr) {
    std::cerr << "Failed to obtain compiled function\n";
    return 1;
  }

  // Run the function and test results.
  RaviGCObject obj = {NULL, 42, 65};
  int ans = funcptr(&obj);
  printf("The answer is %d\n", ans);
  return ans == 42 ? 0 : 1;
}

int main() {

  // Looks like unless following three lines are not executed then
  // ExecutionEngine cannot be created
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  int failure_count = 0;
  failure_count += test1();
  failure_count += test2();

  return failure_count != 0 ? 1 : 0;
}

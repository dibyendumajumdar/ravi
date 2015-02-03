#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include <vector>
#include <string>

int main() {
  // Get context - need not be global
  llvm::LLVMContext &context = llvm::getGlobalContext();
  // Module is the translation unit
  llvm::Module *module = new llvm::Module("ravi", context);
  llvm::IRBuilder<> builder(context);

  /* create a GCObject structure as defined in lobject.h */
  llvm::StructType *myt = llvm::StructType::create(context, "RaviGCObject");
  llvm::PointerType *pmyt = llvm::PointerType::get(myt, 0);
  std::vector<llvm::Type *> elements;
  elements.push_back(pmyt);
  elements.push_back(llvm::Type::getInt8Ty(context));
  elements.push_back(llvm::Type::getInt8Ty(context));
  myt->setBody(elements);
  myt->dump();

  // get printf() function
  std::vector<llvm::Type *> args;
  args.push_back(llvm::Type::getInt8PtrTy(context));
  // accepts a char*, is vararg, and returns int 
  llvm::FunctionType *printfType = llvm::FunctionType::get(builder.getInt32Ty(), args, true);
  llvm::Constant *printfFunc = module->getOrInsertFunction("printf", printfType);

  // Create the testfunc()
  args.clear();
  args.push_back(pmyt);
  llvm::FunctionType *funcType =
      llvm::FunctionType::get(builder.getVoidTy(), args, false);
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
  values.push_back(llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(context), zero));
  // This is the field offset
  values.push_back(llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(context), one));

  // Create the GEP value
  llvm::Value *arg1_a = builder.CreateGEP(arg1, values, "a");

  // Call the printf function
  values.clear();
  values.push_back(formatStr);
  values.push_back(arg1_a);
  builder.CreateCall(printfFunc, values);
  builder.CreateRetVoid();
  module->dump();

  return 0;
}

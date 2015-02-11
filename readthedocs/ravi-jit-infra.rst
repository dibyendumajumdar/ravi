Ravi LLVM JIT Infrastructure
============================

Ravi is using the LLVM MCJIT infrastrucure for JIT compilation. This poses some challenges as we have to worry about how modules
are related to MCJIT engines. It seems from the official samples and from feedback on mailing lists that due to limitations in MCJIT
it is advisable to ensure that each module has its own MCJIT engine. Furthermore it appears that once a module is compiled to machine code
i.e. JITed then no further changes can be made to the module, so it is not possible to keep adding functions to the same 
module.

The Kaleidoscope MCJIT samples try to optimise the module to function mapping by ensuring that a module is reused for each new 
function until a function is compiled. Once any function in the module is compiled, then a new module is allocated. Each module of course
gets its own MCJIT instance.

For Ravi, I want to have an abstraction layer that hides all this detail and allows us to change the implementation strategy without
having to modify the rest of the system. In order to allow that I have created the following two interfaces.

RaviJITState interface
----------------------
This is where the LLVM state is held for any Lua instance. An instance of this will be stored in the Lua State.
The interface looks like this::

  class RAVI_API RaviJITState {
  public:
    RaviJITState();
    ~RaviJITState();

    // Create a function of specified type and linkage
    RaviJITFunction *createFunction(llvm::FunctionType *type,
                                  llvm::GlobalValue::LinkageTypes linkage,
                                  const std::string &name);


    // Get the LLVM Context
    llvm::LLVMContext &context();
  };

RaviJITFunction interface
-------------------------
This interface is for each JITed function. It looks like this::

  class RAVI_API RaviJITFunction {

  public:
    // Destroy the function releasing any resources used
    // by the function - this can be called from Lua's
    // garbage collector when a function is finalized
    ~RaviJITFunction();

    // Compile the function if not already compiled and
    // return pointer to function
    void *compile();

    // Add declaration for an extern function that is not
    // loaded dynamically - i.e., is part of the the executable
    // and therefore not visible at runtime by name
    llvm::Constant *addExternFunction(llvm::FunctionType *type, void *address,
                                    const std::string &name);

    // Get the function name
    const std::string &name() const;

    // Get the LLVM Function object
    llvm::Function *function() const;

    // Get the Module that owns this function
    llvm::Module *module() const;

    // Get the MCJIT instance that owns this function
    llvm::ExecutionEngine *engine() const;
  };

Example Usage
-------------
What above does is abstracts away all the implementation details. Here is a test program that uses the above two interfaces::

  extern "C" int mytest(RaviGCObject *obj) {
    printf("value = %d\n", obj->b1);
    return obj->b1;
  }

  // This version of the test calls mytest() rather than
  // printf() as in test1(). Also we use RaviJITState and related
  // infrastructure
  int test2() {

    RaviJITState jitState;

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
    RaviJITFunction *func = jitState.createFunction(
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
      func->addExternFunction(mytestFuncType, &mytest, "mytest");

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
      llvm::errs() << "Failed to obtain compiled function\n";
      return 1;
    }

    // Run the function and test results.
    RaviGCObject obj = {NULL, 42, 65};
    int ans = funcptr(&obj);
    printf("The answer is %d\n", ans);
    return ans == 42 ? 0 : 1;
  }



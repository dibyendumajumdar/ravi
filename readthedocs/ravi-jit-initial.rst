LLVM First Steps
================

Note that the discussion below is for LVM 3.5.1. 

Although there appears to be a lot of documentation in the LLVM site surprisingly some basic information is hard to find.
The main source of guidance for creating a JIT is in the example toy language 
`Kaleidoscope <https://github.com/llvm-mirror/llvm/blob/master/examples/Kaleidoscope/MCJIT/complete/toy.cpp>`_. But here too
there are several versions - so you have to pick the right version that is compatible with the LLVM version you
are using.

A Lua JITed function will execute in the context of Lua. So it needs to be able to access the ``lua_State`` and its various 
structures. So I wanted a sample that demonstrates passing a pointer to a structure and accessing it within the JITed 
function. 

The initial test program I created is meant to be a "hello world" type test but covering the functionility described above.
The test I want to run is::

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

You can view the test program at `test_llvm.cpp <https://github.com/dibyendumajumdar/ravi/blob/master/tests/test_llvm.cpp>`_. 
It is also reproduced below.

I used the new ``MCJIT`` engine in my test. It seems that this engine compiles modules rather than individual
functions - and once compiled a module cannot be modified. So in the Lua context we need to create a new module
everytime we JIT compile a function - or alternatively we JIT compile a whole Lua source file including all its 
functions into a single module. 

I found the blog post `Using MCJIT with Kaleidoscope 
<http://blog.llvm.org/2013/07/using-mcjit-with-kaleidoscope-tutorial.html>`_ useful in understanding some 
finer points about using ``MCJIT``. 

The Lua GCObject structure in lobject.h we need is::

  typedef struct RaviGCObject {
    struct RaviGCObject *next;
    unsigned char b1;
    unsigned char b2;
  } RaviGCObject;

Our prototype for the JITted function::

  typedef int (*myfunc_t)(RaviGCObject *);

Get global context - not sure what the impact is of sharing::

    llvm::LLVMContext &context = llvm::getGlobalContext();

Module is the translation unit::

    std::unique_ptr<llvm::Module> theModule =
      std::unique_ptr<llvm::Module>(new llvm::Module("ravi", context));
    llvm::Module *module = theModule.get();
    llvm::IRBuilder<> builder(context);

On Windows we get error saying incompatible object format
Reading posts on mailining lists I found that the issue is that COEFF
format is not supported and therefore we need to set -elf as the object
format::

  #ifdef _WIN32
    auto triple = llvm::sys::getProcessTriple();
    module->setTargetTriple(triple + "-elf");
  #endif

create a GCObject structure as defined in lobject.h::

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

Create printf declaration::

    std::vector<llvm::Type *> args;
    args.push_back(llvm::Type::getInt8PtrTy(context));
    // accepts a char*, is vararg, and returns int
    llvm::FunctionType *printfType =
        llvm::FunctionType::get(builder.getInt32Ty(), args, true);
    llvm::Constant *printfFunc =
        module->getOrInsertFunction("printf", printfType);

Create the testfunc()::

    args.clear();
    args.push_back(pstructType);
    llvm::FunctionType *funcType =
      llvm::FunctionType::get(builder.getInt32Ty(), args, false);
    llvm::Function *mainFunc = llvm::Function::Create(
      funcType, llvm::Function::ExternalLinkage, "testfunc", module);
    llvm::BasicBlock *entry =
      llvm::BasicBlock::Create(context, "entrypoint", mainFunc);
    builder.SetInsertPoint(entry);

The printf format string::

    llvm::Value *formatStr = builder.CreateGlobalStringPtr("value = %d\n");

Get the first argument which is RaviGCObject *::

    auto argiter = mainFunc->arg_begin();
    llvm::Value *arg1 = argiter++;
    arg1->setName("obj");

Now we need a GEP for the second field in RaviGCObject::

    std::vector<llvm::Value *> values;
    llvm::APInt zero(32, 0);
    llvm::APInt one(32, 1);
    // This is the array offset into RaviGCObject*
    values.push_back(
       llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(context), zero));
    // This is the field offset
    values.push_back(
      llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(context), one));

Create the GEP value::

    llvm::Value *arg1_a = builder.CreateGEP(arg1, values, "ptr");

Now retrieve the data from the pointer address::

    llvm::Value *tmp1 = builder.CreateLoad(arg1_a, "a");

As the retrieved value is a byte - convert to int i::

    llvm::Value *tmp2 =
      builder.CreateZExt(tmp1, llvm::Type::getInt32Ty(context), "i");

Call the printf function::

    values.clear();
    values.push_back(formatStr);
    values.push_back(tmp2);
    builder.CreateCall(printfFunc, values);

return i::

    builder.CreateRet(tmp2);
    module->dump();

Lets create the MCJIT engine::

    std::string errStr;
    auto engine = llvm::EngineBuilder(module)
                    .setErrorStr(&errStr)
                    .setEngineKind(llvm::EngineKind::JIT)
                    .setUseMCJIT(true)
                    .create();
    if (!engine) {
      llvm::errs() << "Failed to construct MCJIT ExecutionEngine: " << errStr
                 << "\n";
      return 1;
    }

Now lets compile our function into machine code::

    std::string funcname = "testfunc";
    myfunc_t funcptr = (myfunc_t)engine->getFunctionAddress(funcname);
    if (funcptr == nullptr) {
      llvm::errs() << "Failed to obtain compiled function\n";
      return 1;
    }

Run the function and test results::

    RaviGCObject obj = {NULL, 42, 65};
    int ans = funcptr(&obj);
    printf("The answer is %d\n", ans);
    return ans == 42 ? 0 : 1;


Accessing ``extern`` functions from JIT compiled code
-----------------------------------------------------

The JITed function needs to access ``extern`` Lua functions. We need a way to map these to make these visible to the JITed code. Simply declaring
the functions ``extern`` only appears to work if the functios are available as exported symbols in dynamic libraries, e.g. the call to
``printf`` above.

From reading posts on the subject it appears that the way to do this is to add a global mapping in the ``ExecutionEngine`` by calling the
``addGlobalMapping()`` method. However this doesn't work with MCJIT due to a bug! So we need to use a workaround. Apparently there are two
solutions:

* Create a custom memory manager that resolves the ``extern`` functions.
* Add the symbol to the global symbols by calling ``llvm::sys::DynamicLibrary::AddSymbol()``.

I am using the latter approach for now. 

Memory Management in LLVM
-------------------------
Curiously LLVM docs do not say much about how memory should be managed. I am still trying to figure this out, but in general it seems that there is 
hierarchy of ownership. Example: ``ExecutionEngine`` owns the ``Module``. By deleting the parent the 'owned' objects are automatically
deleted.


Links
-----
* `Object format issue on Windows <http://lists.cs.uiuc.edu/pipermail/llvmdev/2013-December/068407.html>`_
* `ExecutionEngine::addGlobalMapping() bug in MCJIT <http://llvm.org/bugs/show_bug.cgi?id=20656>`_
* `LLVM Notes <http://nondot.org/sabre/LLVMNotes/>`_

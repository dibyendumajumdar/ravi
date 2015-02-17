LLVM Notes
==========

The notes below apply to LLVM 3.5.1 unless noted otherwise. All reflect my understanding - so if anything here is incorrect please log an issue and I will correct.

Structs and Unions
------------------
LLVM does not support defining union types so we need to basically use a ``struct`` of appropriate size and cast it as we need. The main thing to be careful about is to ensure that the ``struct`` is of the same size as the ``union``.

An example is::

  ///*
  //** Information about a call.
  //** When a thread yields, 'func' is adjusted to pretend that the
  //** top function has only the yielded values in its stack; in that
  //** case, the actual 'func' value is saved in field 'extra'.
  //** When a function calls another with a continuation, 'extra' keeps
  //** the function index so that, in case of errors, the continuation
  //** function can be called with the correct top.
  //*/
  // typedef struct CallInfo {
  //  StkId func;  /* function index in the stack */
  //  StkId	top;  /* top for this function */
  //  struct CallInfo *previous, *next;  /* dynamic call link */
  //  union {
  //    struct {  /* only for Lua functions */
  //      StkId base;  /* base for this function */
  //      const Instruction *savedpc;
  //    } l;
  //    struct {  /* only for C functions */
  //      lua_KFunction k;  /* continuation in case of yields */
  //      ptrdiff_t old_errfunc;
  //      lua_KContext ctx;  /* context info. in case of yields */
  //    } c;
  //  } u;
  //  ptrdiff_t extra;
  //  short nresults;  /* expected number of results from this function */
  //  lu_byte callstatus;
  //} CallInfo;

Above the union ``u`` has two members of unequal size. To handle this I created the following two sub-types of equal size - note the extra dummy field in the first type::


  elements.clear();
  elements.push_back(StkIdT);        /* base */
  elements.push_back(pInstructionT); /* savedpc */
  elements.push_back(
      C_ptrdiff_t); /* dummy to make this same size as the other member */
  CallInfo_lT = llvm::StructType::create(elements);

  elements.clear();
  elements.push_back(plua_KFunctionT); /* k */
  elements.push_back(C_ptrdiff_t);     /* old_errfunc */
  elements.push_back(lua_KContextT);   /* ctx */
  CallInfo_cT = llvm::StructType::create(elements);

Then as I intend to use the ``u.l`` field more often, I used the following definition for ``CallInfo``::

  CallInfoT = llvm::StructType::create(context, "ravi.CallInfo");
  pCallInfoT = llvm::PointerType::get(CallInfoT, 0);
  elements.clear();
  elements.push_back(StkIdT);     /* func */
  elements.push_back(StkIdT);     /* top */
  elements.push_back(pCallInfoT); /* previous */
  elements.push_back(pCallInfoT); /* next */
  elements.push_back(
      CallInfo_lT); /* u.l  - as we will typically access the lua call details
                       */
  elements.push_back(C_ptrdiff_t);                     /* extra */
  elements.push_back(llvm::Type::getInt16Ty(context)); /* nresults */
  elements.push_back(lu_byteT);                        /* callstatus */
  CallInfoT->setBody(elements);

JIT Compilation Error on Windows
--------------------------------
On Windows when we attempt to JIT compile we get an error saying incompatible object format
Reading posts on mailining lists I found that the issue is that COEFF
format is not supported and therefore we need to set -elf as the object
format::

  #include "llvm/Support/Host.h"

  /* some code */

  #ifdef _WIN32
    auto triple = llvm::sys::getProcessTriple();
    module->setTargetTriple(triple + "-elf");
  #endif

Memory Management
-----------------
It appears that most things in LLVM are owned by the parent object and when the parent object is deleted the children go too. So in my code the main objects I delete are the ``ExecutionEngine`` and ``Module``. Once a module is associated with an engine then only the engine needs to be explicitly deleted is my understanding.

It doesn't help that the tutorial available does not attempt to delete objects / release memory!

MCJIT Engines, Modules and Functions
------------------------------------
Functions live inside Modules but once a Module is finalized (compiled) then no further functions can be added to it. Although an ``MCJIT`` instance (engine) can support multiples modules, the recommendation is to ensure each module is assigned its own engine. The rationale for this is not explained.

Struct Assign 
-------------
My understanding is that to perform assignment of a struct value, one must call the intrinsic ``memcpy`` function. Example of code that does this::

  llvm::Value *src;
  llvm::Value *dest;

  // First get the declaration for the inttrinsic memcpy
  llvm::SmallVector<llvm::Type *, 3> vec;
  vec.push_back(def->types->C_pcharT);  /* i8 */
  vec.push_back(def->types->C_pcharT);  /* i8 */
  vec.push_back(def->types->C_intT);    
  llvm::Function *f = llvm::Intrinsic::getDeclaration(
      def->raviF->module(), llvm::Intrinsic::memcpy, vec);
  lua_assert(f);

  // Cast src and dest to i8*
  llvm::Value *dest_ptr =
      def->builder->CreateBitCast(dest, def->types->C_pcharT);
  llvm::Value *src_ptr = def->builder->CreateBitCast(src, def->types->C_pcharT);

  // Create call to intrinsic memcpy
  values_.clear();
  values_.push_back(dest_ptr);
  values_.push_back(src_ptr);
  values_.push_back(llvm::ConstantInt::get(def->types->C_intT, sizeof(TValue)));
  values_.push_back(
      llvm::ConstantInt::get(def->types->C_intT, sizeof(L_Umaxalign)));
  values_.push_back(def->types->kFalse);
  def->builder->CreateCall(f, values_);

Note that the call to memcpy supply an alignment. 

Accessing ``extern`` functions from JIT compiled code
-----------------------------------------------------

If the JITed function needs to access ``extern`` functions that are statically linked and not exported as dynamic symbols (e.g. in Visual C++) then we need some extra steps.  
From reading posts on the subject it appears that the way to do this is to add a global mapping in the ``ExecutionEngine`` by calling the
``addGlobalMapping()`` method. However this doesn't work with MCJIT due to a bug! So we need to use a workaround. Apparently there are two
solutions:

* Create a custom memory manager that resolves the ``extern`` functions.
* Add the symbol to the global symbols by calling ``llvm::sys::DynamicLibrary::AddSymbol()``.

I am using the latter approach for now. 

GEP instruction
---------------
The GEP instruction cannot compute addresses of fields in a pointer member - as the pointer needs to be 'loaded' first. This is explained in the `GEP FAQ <http://llvm.org/docs/GetElementPtr.html#id6>`_.


Links
-----
* `Mapping High Level Constructs to LLVM IR <http://llvm.lyngvig.org/Articles/Mapping-High-Level-Constructs-to-LLVM-IR>`_
* `IRBuilder Sample <https://github.com/eliben/llvm-clang-samples/blob/master/src_llvm/experimental/build_llvm_ir.cpp>`_
* `Using MCJIT with Kaleidoscope <http://blog.llvm.org/2013/07/using-mcjit-with-kaleidoscope-tutorial.html>`_
* `Object format issue on Windows <http://lists.cs.uiuc.edu/pipermail/llvmdev/2013-December/068407.html>`_
* `ExecutionEngine::addGlobalMapping() bug in MCJIT <http://llvm.org/bugs/show_bug.cgi?id=20656>`_
* `LLVM Notes <http://nondot.org/sabre/LLVMNotes/>`_



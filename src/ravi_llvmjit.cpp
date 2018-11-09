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
#include <ravi_llvmcodegen.h>
#include <ravijit.h>
// Important to include the C++ header files (ravi_llvmcodegen.h) before following
// (ravi_jitshared.h) as the Lua headers define macros that mess with C++ headers
#include <ravi_jitshared.h>

/*
 * Implementation Notes:
 * Each Lua function is compiled into an LLVM Module/Function
 * This strategy allows functions to be garbage collected as normal by Lua
 * See issue #78 for changes to this (wip)
 */

namespace ravi {

// This is just to avoid initializing LLVM repeatedly -
// see below
static std::atomic_int init;

static struct {
  const char *name;
  void *address;
} global_syms[] = {
    {"lua_absindex", reinterpret_cast<void *>(lua_absindex)},
    {"lua_gettop", reinterpret_cast<void *>(lua_gettop)},
    {"lua_settop", reinterpret_cast<void *>(lua_settop)},
    {"lua_pushvalue", reinterpret_cast<void *>(lua_pushvalue)},
    {"lua_rotate", reinterpret_cast<void *>(lua_rotate)},
    {"lua_copy", reinterpret_cast<void *>(lua_copy)},
    {"lua_checkstack", reinterpret_cast<void *>(lua_checkstack)},
    {"lua_xmove", reinterpret_cast<void *>(lua_xmove)},
    {"lua_isnumber", reinterpret_cast<void *>(lua_isnumber)},
    {"lua_isstring", reinterpret_cast<void *>(lua_isstring)},
    {"lua_iscfunction", reinterpret_cast<void *>(lua_iscfunction)},
    {"lua_isinteger", reinterpret_cast<void *>(lua_isinteger)},
    {"lua_isuserdata", reinterpret_cast<void *>(lua_isuserdata)},
    {"lua_type", reinterpret_cast<void *>(lua_type)},
    {"lua_typename", reinterpret_cast<void *>(lua_typename)},
    {"lua_tonumberx", reinterpret_cast<void *>(lua_tonumberx)},
    {"lua_tointegerx", reinterpret_cast<void *>(lua_tointegerx)},
    {"lua_toboolean", reinterpret_cast<void *>(lua_toboolean)},
    {"lua_tolstring", reinterpret_cast<void *>(lua_tolstring)},
    {"lua_rawlen", reinterpret_cast<void *>(lua_rawlen)},
    {"lua_tocfunction", reinterpret_cast<void *>(lua_tocfunction)},
    {"lua_touserdata", reinterpret_cast<void *>(lua_touserdata)},
    {"lua_tothread", reinterpret_cast<void *>(lua_tothread)},
    {"lua_topointer", reinterpret_cast<void *>(lua_topointer)},
    {"lua_arith", reinterpret_cast<void *>(lua_arith)},
    {"lua_rawequal", reinterpret_cast<void *>(lua_rawequal)},
    {"lua_compare", reinterpret_cast<void *>(lua_compare)},
    {"lua_pushnil", reinterpret_cast<void *>(lua_pushnil)},
    {"lua_pushnumber", reinterpret_cast<void *>(lua_pushnumber)},
    {"lua_pushinteger", reinterpret_cast<void *>(lua_pushinteger)},
    {"lua_pushlstring", reinterpret_cast<void *>(lua_pushlstring)},
    {"lua_pushstring", reinterpret_cast<void *>(lua_pushstring)},
    {"lua_pushvfstring", reinterpret_cast<void *>(lua_pushvfstring)},
    {"lua_pushfstring", reinterpret_cast<void *>(lua_pushfstring)},
    {"lua_pushcclosure", reinterpret_cast<void *>(lua_pushcclosure)},
    {"lua_pushboolean", reinterpret_cast<void *>(lua_pushboolean)},
    {"lua_pushlightuserdata", reinterpret_cast<void *>(lua_pushlightuserdata)},
    {"lua_pushthread", reinterpret_cast<void *>(lua_pushthread)},
    {"lua_getglobal", reinterpret_cast<void *>(lua_getglobal)},
    {"lua_gettable", reinterpret_cast<void *>(lua_gettable)},
    {"lua_getfield", reinterpret_cast<void *>(lua_getfield)},
    {"lua_geti", reinterpret_cast<void *>(lua_geti)},
    {"lua_rawget", reinterpret_cast<void *>(lua_rawget)},
    {"lua_rawgeti", reinterpret_cast<void *>(lua_rawgeti)},
    {"lua_rawgetp", reinterpret_cast<void *>(lua_rawgetp)},
    {"lua_createtable", reinterpret_cast<void *>(lua_createtable)},
    {"lua_newuserdata", reinterpret_cast<void *>(lua_newuserdata)},
    {"lua_getmetatable", reinterpret_cast<void *>(lua_getmetatable)},
    {"lua_getuservalue", reinterpret_cast<void *>(lua_getuservalue)},
    {"lua_setglobal", reinterpret_cast<void *>(lua_setglobal)},
    {"lua_settable", reinterpret_cast<void *>(lua_settable)},
    {"lua_setfield", reinterpret_cast<void *>(lua_setfield)},
    {"lua_seti", reinterpret_cast<void *>(lua_seti)},
    {"lua_rawset", reinterpret_cast<void *>(lua_rawset)},
    {"lua_rawseti", reinterpret_cast<void *>(lua_rawseti)},
    {"lua_rawsetp", reinterpret_cast<void *>(lua_rawsetp)},
    {"lua_setmetatable", reinterpret_cast<void *>(lua_setmetatable)},
    {"lua_setuservalue", reinterpret_cast<void *>(lua_setuservalue)},
    {"lua_gc", reinterpret_cast<void *>(lua_gc)},
    {"lua_error", reinterpret_cast<void *>(lua_error)},
    {"lua_next", reinterpret_cast<void *>(lua_next)},
    {"lua_concat", reinterpret_cast<void *>(lua_concat)},
    {"lua_len", reinterpret_cast<void *>(lua_len)},
    {"lua_stringtonumber", reinterpret_cast<void *>(lua_stringtonumber)},

    {"printf", reinterpret_cast<void *>(printf)},
    {"puts", reinterpret_cast<void *>(puts)},
    {nullptr, nullptr}};

// Construct the JIT compiler state
// The JIT compiler state will be attached to the
// lua_State - all compilation activity happens
// in the context of the JIT State
RaviJITState::RaviJITState()
    : auto_(false),
      enabled_(true),
      opt_level_(2),
      size_level_(0),
      verbosity_(0),
      tracehook_enabled_(false),
      validation_(false),
      gcstep_(300),
      min_code_size_(150),
      min_exec_count_(50),
      allocated_modules_(0),
      compiling_(false) {
  // LLVM needs to be initialized else
  // ExecutionEngine cannot be created
  // This needs to be an atomic check although LLVM docs
  // say that it is okay to call these functions more than once
  if (init == 0) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    // TODO see email trail on resolving symbols in process
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
    init++;
  }
  triple_ = llvm::sys::getProcessTriple();
#if defined(_WIN32) && \
    (!defined(_WIN64) || LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR < 7)
  // On Windows we get compilation error saying incompatible object format
  // Reading posts on mailing lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set -elf as the object
  // format; LLVM 3.7 onwards COEFF is supported
  triple_ += "-elf";
#endif
  context_ = new llvm::LLVMContext();

#if USE_ORC_JIT
  llvm::EngineBuilder eengineBuilder;
  auto target = eengineBuilder.selectTarget();
  TM = std::unique_ptr<llvm::TargetMachine>(target);
  TM->setO0WantsFastISel(true); // enable FastISel when -O0 is used
  DL = std::unique_ptr<llvm::DataLayout>(new llvm::DataLayout(TM->createDataLayout()));

  ObjectLayer = std::unique_ptr<ObjectLayerT>(new ObjectLayerT(
      []() { return std::make_shared<llvm::SectionMemoryManager>(); }));
  CompileLayer = std::unique_ptr<CompileLayerT>(new CompileLayerT(
      *ObjectLayer, llvm::orc::SimpleCompiler(*TM)));
  OptimizeLayer = std::unique_ptr<OptimizerLayerT>(new OptimizerLayerT(
      *CompileLayer,
      [this](std::shared_ptr<llvm::Module> M) {
        return optimizeModule(std::move(M));
      })),
#endif

  types_ = new LuaLLVMTypes(*context_);

  for (int i = 0; global_syms[i].name != nullptr; i++) {
    llvm::sys::DynamicLibrary::AddSymbol(global_syms[i].name,
                                         global_syms[i].address);
  }
}

// Destroy the JIT state freeing up any
// functions that were compiled
RaviJITState::~RaviJITState() {
  assert(allocated_modules_ == 0);
  delete types_;
  delete context_;
}

#if USE_ORC_JIT
std::shared_ptr<llvm::Module> RaviJITState::optimizeModule(
    std::shared_ptr<llvm::Module> M) {
#if LLVM_VERSION_MAJOR > 3 || LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 7
  using llvm::legacy::FunctionPassManager;
  using llvm::legacy::PassManager;
#else
  using llvm::FunctionPassManager;
  using llvm::PassManager;
#endif
  
  if (get_verbosity() >= 1)
    M->print(llvm::errs(), NULL, false, true);
  if (get_verbosity() >= 3)
    TM->Options.PrintMachineCode = 1;
  else
    TM->Options.PrintMachineCode = 0;
  if (get_optlevel() == 0)
    TM->Options.EnableFastISel = 1;
  else
    TM->Options.EnableFastISel = 0;

  // We use the PassManagerBuilder to setup optimization
  // passes - the PassManagerBuilder allows easy configuration of
  // typical C/C++ passes corresponding to O0, O1, O2, and O3 compiler options
  // If dumpAsm is true then the generated assembly code will be
  // dumped to stderr

  llvm::PassManagerBuilder pmb;
  pmb.OptLevel = get_optlevel();
  pmb.SizeLevel = get_sizelevel();

  {
    // Create a function pass manager for this engine
    std::unique_ptr<FunctionPassManager> FPM(new FunctionPassManager(M.get()));

    // Set up the optimizer pipeline.  Start with registering info about how the
    // target lays out data structures.
#if LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR == 6
    // LLVM 3.6.0 change
    module_->setDataLayout(engine_->getDataLayout());
    FPM->add(new llvm::DataLayoutPass());
#elif LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR == 5
    // LLVM 3.5.0
    auto target_layout = engine_->getTargetMachine()->getDataLayout();
    module_->setDataLayout(target_layout);
    FPM->add(new llvm::DataLayoutPass(*engine_->getDataLayout()));
#elif LLVM_VERSION_MAJOR > 3 || \
    LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 7
    // Apparently no need to set DataLayout
#else
#error Unsupported LLVM version
#endif
    pmb.populateFunctionPassManager(*FPM);
    FPM->doInitialization();
    // Run the optimizations over all functions in the module being added to
    // the JIT.
    for (auto &F : *M) FPM->run(F);
  }

  std::string codestr;
  {
    // In LLVM 3.7 for some reason the string is not saved
    // until the stream is destroyed - even though there is a
    // flush; so we introduce a scope here to ensure destruction
    // of the stream
    llvm::raw_string_ostream ostream(codestr);
#if LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR < 7
    llvm::formatted_raw_ostream formatted_stream(ostream);
#else
    llvm::buffer_ostream formatted_stream(ostream);
#endif

    // Also in 3.7 the pass manager seems to hold on to the stream
    // so we need to ensure that the stream outlives the pass manager
    std::unique_ptr<PassManager> MPM(new PassManager());
#if LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR == 6
    MPM->add(new llvm::DataLayoutPass());
#elif LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR == 5
    MPM->add(new llvm::DataLayoutPass(*engine_->getDataLayout()));
#endif
    pmb.populateModulePassManager(*MPM);

    for (int i = 0; get_verbosity() == 2 && i < 1; i++) {
      if (!TM) {
        llvm::errs() << "unable to dump assembly\n";
        break;
      }
      if (TM->addPassesToEmitFile(*MPM, formatted_stream,
                                  llvm::TargetMachine::CGFT_AssemblyFile)) {
        llvm::errs() << "unable to add passes for generating assemblyfile\n";
        break;
      }
    }
    MPM->run(*M);

    // Note that in 3.7 this flus appears to have no effect
#if LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR <= 7
    formatted_stream.flush();
#endif
  }
  if (get_verbosity() == 2 && codestr.length() > 0) llvm::errs() << codestr << "\n";
  
  return M;
}

#endif

void RaviJITState::addGlobalSymbol(const std::string &name, void *address) {
  llvm::sys::DynamicLibrary::AddSymbol(name, address);
}

#if USE_ORC_JIT
RaviJITState::ModuleHandle RaviJITState::addModule(
    std::unique_ptr<llvm::Module> M) {
  // Build our symbol resolver:
  // Lambda 1: Look back into the JIT itself to find symbols that are part of
  //           the same "logical dylib".
  // Lambda 2: Search for external symbols in the host process.
  auto Resolver = llvm::orc::createLambdaResolver(
      [&](const std::string &Name) {
        if (auto Sym = OptimizeLayer->findSymbol(Name, false)) return Sym;
        return llvm::JITSymbol(nullptr);
      },
      [](const std::string &Name) {
        if (auto SymAddr =
                llvm::RTDyldMemoryManager::getSymbolAddressInProcess(Name))
          return llvm::JITSymbol(SymAddr, llvm::JITSymbolFlags::Exported);
        return llvm::JITSymbol(nullptr);
      });

  // Add the set to the JIT with the resolver we created above and a newly
  // created SectionMemoryManager.
  return llvm::cantFail(
      OptimizeLayer->addModule(std::move(M), std::move(Resolver)));
}

llvm::JITSymbol RaviJITState::findSymbol(const std::string Name) {
  std::string MangledName;
  llvm::raw_string_ostream MangledNameStream(MangledName);
  llvm::Mangler::getNameWithPrefix(MangledNameStream, Name, *DL);
  // Note that the LLVM tutorial uses true below but that appears to
  // cause a failure in lookup
  return OptimizeLayer->findSymbol(MangledNameStream.str(), false);
}

void RaviJITState::removeModule(ModuleHandle H) {
  llvm::cantFail(OptimizeLayer->removeModule(H));
}
#endif

void RaviJITState::dump() { types_->dump(); }

static std::atomic_int module_id;

RaviJITModule::RaviJITModule(RaviJITState *owner)
    : owner_(owner)
#if !USE_ORC_JIT
      ,module_(nullptr)
      ,engine_(nullptr)
#endif
{
  int myid = module_id++;
  char buf[40];
  snprintf(buf, sizeof buf, "ravi_module_%d", myid);
  std::string moduleName(buf);
#if USE_ORC_JIT
  module_ = std::unique_ptr<llvm::Module>(new llvm::Module(moduleName, owner->context()));
  module_->setDataLayout(owner_->getTargetMachine().createDataLayout());
#else
  module_ = new llvm::Module(moduleName, owner->context());
#endif
  if (myid == 0) {
    // Extra validation to check that the LLVM sizes match Lua sizes
    llvm::DataLayout *layout = new llvm::DataLayout(module());
    // auto valueSize = layout->getTypeAllocSize(owner->types()->ValueT);
    // auto valueSizeOf = sizeof(Value);
    // auto TvalueSize = layout->getTypeAllocSize(owner->types()->TValueT);
    // auto TvalueSizeOf = sizeof(TValue);
    // printf("Value %d %d Tvalue %d %d\n", (int)valueSize, (int)valueSizeOf,
    // (int)TvalueSize, (int)TvalueSizeOf);
    assert(sizeof(Value) == layout->getTypeAllocSize(owner->types()->ValueT));
    assert(sizeof(TValue) == layout->getTypeAllocSize(owner->types()->TValueT));
    assert(sizeof(UTString) ==
           layout->getTypeAllocSize(owner->types()->TStringT));
    assert(sizeof(Udata) == layout->getTypeAllocSize(owner->types()->UdataT));
    assert(sizeof(CallInfo) ==
           layout->getTypeAllocSize(owner->types()->CallInfoT));
    assert(sizeof(lua_State) ==
           layout->getTypeAllocSize(owner->types()->lua_StateT));
    delete layout;
  }
#if !USE_ORC_JIT
#if defined(_WIN32) && \
    (!defined(_WIN64) || LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR < 7)
  // On Windows we get error saying incompatible object format
  // Reading posts on mailing lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set
  // -elf as the object format; LLVM 3.7 onwards COEFF is supported
  module_->setTargetTriple(owner->triple());
#endif
#if LLVM_VERSION_MAJOR > 3 || LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR > 5
  // LLVM 3.6.0 change
  std::unique_ptr<llvm::Module> module(module_);
  llvm::EngineBuilder builder(std::move(module));
#else
  llvm::EngineBuilder builder(module_);
  builder.setUseMCJIT(true);
#endif
  builder.setEngineKind(llvm::EngineKind::JIT);
  std::string errStr;
  builder.setErrorStr(&errStr);
  engine_ = builder.create();
  if (!engine_) {
    fprintf(stderr, "FATAL ERROR: could not create ExecutionEngine: %s\n",
            errStr.c_str());
    abort();
    return;
  }
#endif
  owner->incr_allocated_modules();
}

RaviJITModule::~RaviJITModule() {
#if !USE_ORC_JIT
  if (engine_)
    delete engine_;
  else if (module_)
    // if engine was created then we don't need to delete the
    // module as it would have been deleted by the engine
    delete module_;
#else
  owner()->removeModule(module_handle_);
#endif
  owner_->decr_allocated_modules();
#if 1
  // fprintf(stderr, "module destroyed\n");
#endif
}

int RaviJITModule::addFunction(RaviJITFunction *f) {
  int id = functions_.size();
  functions_.push_back(f);
  return id;
}

void RaviJITModule::removeFunction(RaviJITFunction *f) {
  lua_assert(functions_[f->getId()] == f);
  functions_[f->getId()] = nullptr;
}

RaviJITFunction::RaviJITFunction(lua_CFunction *p,
                                 const std::shared_ptr<RaviJITModule> &module,
                                 llvm::FunctionType *type,
                                 llvm::GlobalValue::LinkageTypes linkage,
                                 const std::string &name)
    : module_(module),
      name_(name),
      function_(nullptr),
      ptr_(nullptr),
      func_ptrptr_(p) {
  auto M = module_->module();
  lua_assert(M);
  function_ = llvm::Function::Create(type, linkage, name, M);
  id_ = module_->addFunction(this);
}

RaviJITFunction::RaviJITFunction(lua_CFunction *p,
                                 const std::shared_ptr<RaviJITModule> &module,
                                 const std::string &name)
    : module_(module), name_(name), ptr_(nullptr), func_ptrptr_(p) {
  auto M = module_->module();
  lua_assert(M);
  function_ = M->getFunction(name);
  id_ = module_->addFunction(this);
}

RaviJITFunction::~RaviJITFunction() {
  // Remove this function from parent
  // fprintf(stderr, "function destroyed\n");
  module_->removeFunction(this);
}

void RaviJITModule::runpasses(bool dumpAsm) {
  if (!module_) return;
#if !USE_ORC_JIT
#if LLVM_VERSION_MAJOR > 3 || LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 7
  using llvm::legacy::FunctionPassManager;
  using llvm::legacy::PassManager;
#else
  using llvm::FunctionPassManager;
  using llvm::PassManager;
#endif

  // We use the PassManagerBuilder to setup optimization
  // passes - the PassManagerBuilder allows easy configuration of
  // typical C/C++ passes corresponding to O0, O1, O2, and O3 compiler options
  // If dumpAsm is true then the generated assembly code will be
  // dumped to stderr

  llvm::PassManagerBuilder pmb;
  pmb.OptLevel = owner_->get_optlevel();
  pmb.SizeLevel = owner_->get_sizelevel();

  {
    // Create a function pass manager for this engine
    std::unique_ptr<FunctionPassManager> FPM(new FunctionPassManager(module_));

// Set up the optimizer pipeline.  Start with registering info about how the
// target lays out data structures.
#if LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR == 6
    // LLVM 3.6.0 change
    module_->setDataLayout(engine_->getDataLayout());
    FPM->add(new llvm::DataLayoutPass());
#elif LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR == 5
    // LLVM 3.5.0
    auto target_layout = engine_->getTargetMachine()->getDataLayout();
    module_->setDataLayout(target_layout);
    FPM->add(new llvm::DataLayoutPass(*engine_->getDataLayout()));
#elif LLVM_VERSION_MAJOR > 3 || \
    LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 7
// Apparently no need to set DataLayout
#else
#error Unsupported LLVM version
#endif
    pmb.populateFunctionPassManager(*FPM);
    FPM->doInitialization();
    for (int i = 0; i < functions_.size(); i++) {
      if (functions_[i] == nullptr) continue;
      FPM->run(*functions_[i]->function());
    }
  }

  std::string codestr;
  {
    // In LLVM 3.7 for some reason the string is not saved
    // until the stream is destroyed - even though there is a
    // flush; so we introduce a scope here to ensure destruction
    // of the stream
    llvm::raw_string_ostream ostream(codestr);
#if LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR < 7
    llvm::formatted_raw_ostream formatted_stream(ostream);
#else
    llvm::buffer_ostream formatted_stream(ostream);
#endif

    // Also in 3.7 the pass manager seems to hold on to the stream
    // so we need to ensure that the stream outlives the pass manager
    std::unique_ptr<PassManager> MPM(new PassManager());
#if LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR == 6
    MPM->add(new llvm::DataLayoutPass());
#elif LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR == 5
    MPM->add(new llvm::DataLayoutPass(*engine_->getDataLayout()));
#endif
    pmb.populateModulePassManager(*MPM);

    for (int i = 0; dumpAsm && i < 1; i++) {
      llvm::TargetMachine *TM = engine_->getTargetMachine();
      if (!TM) {
        llvm::errs() << "unable to dump assembly\n";
        break;
      }
      if (TM->addPassesToEmitFile(*MPM, formatted_stream,
                                  llvm::TargetMachine::CGFT_AssemblyFile)) {
        llvm::errs() << "unable to add passes for generating assemblyfile\n";
        break;
      }
    }
    MPM->run(*module_);

// Note that in 3.7 this flus appears to have no effect
#if LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR <= 7
    formatted_stream.flush();
#endif
  }
  if (dumpAsm && codestr.length() > 0) llvm::errs() << codestr << "\n";
#endif
}

void RaviJITModule::finalize(bool doDump) {
  lua_assert(module_);
  if (!module_) return;
#if !USE_ORC_JIT
  // Following will generate very verbose dump when machine code is
  // produced below
  llvm::TargetMachine *TM = engine_->getTargetMachine();
  // TM->setOptLevel(llvm::CodeGenOpt::None);
  if (doDump) { TM->Options.PrintMachineCode = 1; }

  // Upon creation, MCJIT holds a pointer to the Module object
  // that it received from EngineBuilder but it does not immediately
  // generate code for this module. Code generation is deferred
  // until either the MCJIT::finalizeObject method is called
  // explicitly or a function such as MCJIT::getPointerToFunction
  // is called which requires the code to have been generated.
  engine_->finalizeObject();
#else
  // With ORC api, the module gets compiled when it is added
  // The optimzation passes are run via the callback
  module_handle_ = owner()->addModule(std::move(module_));
#endif
  for (int i = 0; i < functions_.size(); i++) {
    if (functions_[i] == nullptr) continue;
    functions_[i]->setFunctionPtr();
  }
}

// Retrieve the JITed function from the
// execution engine and store it in ptr_
// Also associate the JITed function with the
// Lua Proto structure
void RaviJITFunction::setFunctionPtr() {
  lua_assert(ptr_ == nullptr);
  // function_ may be nullptr if it was obtained by name from the module, so
  // we need to check that
#if !USE_ORC_JIT
  if (function_) {
    ptr_ = engine()->getPointerToFunction(function_);
    *func_ptrptr_ = (lua_CFunction)ptr_;
  }
#else
  if (function_) {
    auto symbol = owner()->findSymbol(name());
    if (symbol) {
      ptr_ = (void *)(intptr_t)llvm::cantFail(symbol.getAddress());
      *func_ptrptr_ = (lua_CFunction)ptr_;
    }
  }
#endif
}

llvm::Function *RaviJITModule::addExternFunction(llvm::FunctionType *type,
                                                 void *address,
                                                 const std::string &name) {
  auto fn = external_symbols_.find(name);
  if (fn != external_symbols_.end()) return fn->second;
  llvm::Function *f = llvm::Function::Create(
      type, llvm::Function::ExternalLinkage, name, module());
  f->setDoesNotThrow();
  // We should have been able to call
  // engine_->addGlobalMapping() but this doesn't work
  // See http://lists.cs.uiuc.edu/pipermail/llvmdev/2014-April/071856.html
  // See bug report http://llvm.org/bugs/show_bug.cgi?id=20656
  // following will call DynamicLibrary::AddSymbol
  owner_->addGlobalSymbol(name, address);
  external_symbols_[name] = f;
  return f;
}

void RaviJITModule::dump() {
#if LLVM_VERSION_MAJOR < 5
  if (module_) module_->dump();
#endif
}

// Dumps the machine code
// Will execute the passes as required by currently set
// optimization level; this may or may not match the actual
// JITed code which would have used the optimzation level set at the
// time of compilation
void RaviJITModule::dumpAssembly() { runpasses(true); }

std::unique_ptr<RaviJITState> RaviJITStateFactory::newJITState() {
  return std::unique_ptr<RaviJITState>(new RaviJITState());
}
}  // namespace ravi

// Initialize the JIT State and attach it to the
// Global Lua State
// If a JIT State already exists then this function
// will return -1
int raviV_initjit(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state != NULL) return -1;
  ravi_State *jit = (ravi_State *)calloc(1, sizeof(ravi_State));
  jit->jit = new ravi::RaviJITState();
  jit->code_generator =
      new ravi::RaviCodeGenerator((ravi::RaviJITState *)jit->jit);
  G->ravi_state = jit;
  return 0;
}

// Free up the JIT State
void raviV_close(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL) return;
  delete G->ravi_state->code_generator;
  delete G->ravi_state->jit;
  free(G->ravi_state);
}

// Compile a Lua function
// If JIT is turned off then compilation is skipped
// Compilation occurs if either auto compilation is ON (subject to some
// thresholds)
// or if a manual compilation request was made
// Returns true if compilation was successful
int raviV_compile(struct lua_State *L, struct Proto *p,
                  ravi_compile_options_t *options) {
  if (p->ravi_jit.jit_function) return 1;
  global_State *G = G(L);
  if (G->ravi_state == NULL) return 0;
  if (!G->ravi_state->jit->is_enabled()) { return 0; }
  if (G->ravi_state->jit->get_compiling_flag()) { return 0; }
  bool doCompile = (bool)(options && options->manual_request != 0);
  if (!doCompile && G->ravi_state->jit->is_auto()) {
    if (p->ravi_jit.jit_flags ==
        RAVI_JIT_FLAG_HASFORLOOP) /* function has fornum loop, so compile */
      doCompile = true;
    else if (p->sizecode >
             G->ravi_state->jit
                 ->get_mincodesize()) /* function is long so compile */
      doCompile = true;
    else {
      if (p->ravi_jit.execution_count <
          G->ravi_state->jit
              ->get_minexeccount()) /* function has been executed many
                                       times so compile */
        p->ravi_jit.execution_count++;
      else
        doCompile = true;
    }
  }
  if (doCompile) {
    auto module = std::make_shared<ravi::RaviJITModule>(G->ravi_state->jit);
    if (G->ravi_state->code_generator->compile(L, p, module, options)) {
      module->runpasses();
      module->finalize(G->ravi_state->jit->get_verbosity() == 3);
    }
  }
  return p->ravi_jit.jit_function != nullptr;
}

// Compile a bunch of Lua functions
// And put them all in one module
// Returns true if compilation was successful
int raviV_compile_n(struct lua_State *L, struct Proto *p[], int n,
                    ravi_compile_options_t *options) {
  global_State *G = G(L);
  int count = 0;
  if (G->ravi_state->jit->get_compiling_flag())
    return 0;
  auto module = std::make_shared<ravi::RaviJITModule>(G->ravi_state->jit);
  for (int i = 0; i < n; i++) {
    if (G->ravi_state->code_generator->compile(L, p[i], module, options))
      count++;
  }
  if (count) {
    module->runpasses();
    module->finalize(G->ravi_state->jit->get_verbosity() == 3);
  }
  return count > 0;
}

// Free the JIT compiled function
// Note that this is called by the garbage collector
void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  ravi::RaviJITFunction *f =
      reinterpret_cast<ravi::RaviJITFunction *>(p->ravi_jit.jit_data);
  if (f) delete f;
  p->ravi_jit.jit_status = RAVI_JIT_NOT_COMPILED;
  p->ravi_jit.jit_function = NULL;
  p->ravi_jit.jit_data = NULL;
  p->ravi_jit.execution_count = 0;
}

// Dump the LLVM IR
void raviV_dumpIR(struct lua_State *L, struct Proto *p) {
  if (p->ravi_jit.jit_status == RAVI_JIT_COMPILED) /* compiled */ {
    ravi::RaviJITFunction *f =
        reinterpret_cast<ravi::RaviJITFunction *>(p->ravi_jit.jit_data);
    if (f) f->dump();
  }
}

// Dump the LLVM ASM
void raviV_dumpASM(struct lua_State *L, struct Proto *p) {
  if (p->ravi_jit.jit_status == RAVI_JIT_COMPILED) /* compiled */ {
    ravi::RaviJITFunction *f =
        reinterpret_cast<ravi::RaviJITFunction *>(p->ravi_jit.jit_data);
    if (f) f->dumpAssembly();
  }
}

void raviV_setminexeccount(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->jit->set_minexeccount(value);
}
int raviV_getminexeccount(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->jit->get_minexeccount();
}

void raviV_setmincodesize(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->jit->set_mincodesize(value);
}
int raviV_getmincodesize(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->jit->get_mincodesize();
}

void raviV_setauto(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->jit->set_auto(value != 0);
}
int raviV_getauto(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->jit->is_auto();
}

// Turn on/off the JIT compiler
void raviV_setjitenabled(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->jit->set_enabled(value != 0);
}
int raviV_getjitenabled(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->jit->is_enabled();
}

void raviV_setoptlevel(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->jit->set_optlevel(value);
}
int raviV_getoptlevel(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->jit->get_optlevel();
}

void raviV_setsizelevel(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->jit->set_sizelevel(value);
}
int raviV_getsizelevel(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->jit->get_sizelevel();
}

void raviV_setverbosity(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->jit->set_verbosity(value);
}
int raviV_getverbosity(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->jit->get_verbosity();
}

void raviV_setvalidation(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->jit->set_validation(value != 0);
}
int raviV_getvalidation(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->jit->get_validation();
}

void raviV_setgcstep(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->jit->set_gcstep(value);
}
int raviV_getgcstep(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->jit->get_gcstep();
}

// Turn on/off the JIT compiler
void raviV_settraceenabled(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  G->ravi_state->jit->set_tracehook_enabled(value != 0);
}
int raviV_gettraceenabled(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state) return 0;
  return G->ravi_state->jit->is_tracehook_enabled();
}

extern "C" int ravi_compile_C(lua_State *L) {
  return 0;
}
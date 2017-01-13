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
#include <ravijit.h>
#include "ravi_llvmcodegen.h"

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

// Construct the JIT compiler state
// The JIT compiler state will be attached to the
// lua_State - all compilation activity happens
// in the context of the JIT State
RaviJITState::RaviJITState()
    : auto_(false),
      enabled_(true),
      opt_level_(2),
      size_level_(0),
      min_code_size_(150),
      min_exec_count_(50),
      gc_step_(300),
      tracehook_enabled_(false),
      allocated_modules_(0) {
  // LLVM needs to be initialized else
  // ExecutionEngine cannot be created
  // This needs to be an atomic check although LLVM docs
  // say that it is okay to call these functions more than once
  if (init == 0) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    init++;
  }
  triple_ = llvm::sys::getProcessTriple();
#if defined(_WIN32) && (!defined(_WIN64) || LLVM_VERSION_MINOR < 7)
  // On Windows we get compilation error saying incompatible object format
  // Reading posts on mailing lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set -elf as the object
  // format; LLVM 3.7 onwards COEFF is supported
  triple_ += "-elf";
#endif
  context_ = new llvm::LLVMContext();
  types_ = new LuaLLVMTypes(*context_);
}

// Destroy the JIT state freeing up any
// functions that were compiled
RaviJITState::~RaviJITState() {
  assert(allocated_modules_ == 0);
  delete types_;
  delete context_;
}

void RaviJITState::addGlobalSymbol(const std::string &name, void *address) {
  llvm::sys::DynamicLibrary::AddSymbol(name, address);
}

void RaviJITState::dump() { types_->dump(); }

static std::atomic_int module_id;

RaviJITModule::RaviJITModule(RaviJITState *owner)
    : owner_(owner), engine_(nullptr), module_(nullptr) {
  int myid = module_id++;
  char buf[40];
  snprintf(buf, sizeof buf, "ravi_module_%d", myid);
  std::string moduleName(buf);
  module_ = new llvm::Module(moduleName, owner->context());
  if (myid == 0) {
    // Extra validation to check that the LLVM sizes match Lua sizes
    llvm::DataLayout *layout = new llvm::DataLayout(module_);
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
#if defined(_WIN32) && (!defined(_WIN64) || LLVM_VERSION_MINOR < 7)
  // On Windows we get error saying incompatible object format
  // Reading posts on mailing lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set
  // -elf as the object format; LLVM 3.7 onwards COEFF is supported
  module_->setTargetTriple(owner->triple());
#endif
#if LLVM_VERSION_MINOR > 5
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
  owner->incr_allocated_modules();
  if (!engine_) {
    fprintf(stderr, "FATAL ERROR: could not create ExecutionEngine: %s\n",
            errStr.c_str());
    abort();
    return;
  }
}

RaviJITModule::~RaviJITModule() {
  if (engine_)
    delete engine_;
  else if (module_)
    // if engine was created then we don't need to delete the
    // module as it would have been deleted by the engine
    delete module_;
  owner_->decr_allocated_modules();
#if 0
  //fprintf(stderr, "module destroyed\n");
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
                                 std::shared_ptr<RaviJITModule> module,
                                 llvm::FunctionType *type,
                                 llvm::GlobalValue::LinkageTypes linkage,
                                 const std::string &name)
    : module_(module),
      name_(name),
      function_(nullptr),
      ptr_(nullptr),
      func_ptrptr_(p) {
  function_ = llvm::Function::Create(type, linkage, name, module_->module());
  id_ = module_->addFunction(this);
}

RaviJITFunction::~RaviJITFunction() {
  // Remove this function from parent
  // fprintf(stderr, "function destroyed\n");
  module_->removeFunction(this);
}

void RaviJITModule::runpasses(bool dumpAsm) {
#if LLVM_VERSION_MINOR >= 7
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
#if LLVM_VERSION_MINOR == 6
    // LLVM 3.6.0 change
    module_->setDataLayout(engine_->getDataLayout());
    FPM->add(new llvm::DataLayoutPass());
#elif LLVM_VERSION_MINOR == 5
    // LLVM 3.5.0
    auto target_layout = engine_->getTargetMachine()->getDataLayout();
    module_->setDataLayout(target_layout);
    FPM->add(new llvm::DataLayoutPass(*engine_->getDataLayout()));
#elif LLVM_VERSION_MINOR >= 7
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
#if LLVM_VERSION_MINOR < 7
    llvm::formatted_raw_ostream formatted_stream(ostream);
#else
    llvm::buffer_ostream formatted_stream(ostream);
#endif

    // Also in 3.7 the pass manager seems to hold on to the stream
    // so we need to ensure that the stream outlives the pass manager
    std::unique_ptr<PassManager> MPM(new PassManager());
#if LLVM_VERSION_MINOR == 6
    MPM->add(new llvm::DataLayoutPass());
#elif LLVM_VERSION_MINOR == 5
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
#if LLVM_VERSION_MINOR <= 7
    formatted_stream.flush();
#endif
  }
  if (dumpAsm && codestr.length() > 0) llvm::errs() << codestr << "\n";
}

void RaviJITModule::finalize(bool doDump) {
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
  ptr_ = engine()->getPointerToFunction(function_);
  *func_ptrptr_ = (lua_CFunction)ptr_;
}

llvm::Function *RaviJITModule::addExternFunction(llvm::FunctionType *type,
                                                 void *address,
                                                 const std::string &name) {
  auto fn = external_symbols_.find(name);
  if (fn != external_symbols_.end()) return fn->second;
  llvm::Function *f = llvm::Function::Create(
      type, llvm::Function::ExternalLinkage, name, module_);
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

void RaviJITModule::dump() { module_->dump(); }

// Dumps the machine code
// Will execute the passes as required by currently set
// optimzation level; this may or may not match the actual
// JITed code which would have used the optimzation level set at the
// time of compilation
void RaviJITModule::dumpAssembly() { runpasses(true); }

std::unique_ptr<RaviJITState> RaviJITStateFactory::newJITState() {
  return std::unique_ptr<RaviJITState>(new RaviJITState());
}
}

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
  if (p->ravi_jit.jit_function) return true;
  global_State *G = G(L);
  if (G->ravi_state == NULL) return 0;
  if (!G->ravi_state->jit->is_enabled()) { return 0; }
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
      module->finalize(options ? options->dump_level != 0 : 0);
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
  auto module = std::make_shared<ravi::RaviJITModule>(G->ravi_state->jit);
  for (int i = 0; i < n; i++) {
    if (G->ravi_state->code_generator->compile(L, p[i], module, options))
      count++;
  }
  if (count) {
    module->runpasses();
    module->finalize(options ? options->dump_level != 0 : 0);
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

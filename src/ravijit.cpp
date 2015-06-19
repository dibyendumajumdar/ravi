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
#ifdef USE_LLVM
#include "ravi_llvmcodegen.h"

/*
 * Implementation Notes:
 * Each Lua function is compiled into an LLVM Module/Function
 * This strategy allows functions to be garbage collected as normal by Lua
 */

namespace ravi {

// This is just to avoid initializing LLVM repeatedly -
// see below
static std::atomic_int init;

RaviJITState *RaviJITFunctionImpl::owner() const { return owner_; }

// Construct the JIT compiler state
// The JIT compiler state will be attached to the
// lua_State - all compilation activity happens
// in the context of the JIT State
RaviJITStateImpl::RaviJITStateImpl()
    : context_(llvm::getGlobalContext()), auto_(false), enabled_(true),
      opt_level_(2), size_level_(0), min_code_size_(150), min_exec_count_(50) {
  // LLVM needs to be initialized else
  // ExecutionEngine cannot be created
  // This should ideally be an atomic check but because LLVM docs
  // say that it is okay to call these functions more than once we
  // do not bother
  if (init == 0) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    init++;
  }
  triple_ = llvm::sys::getProcessTriple();
#if defined(_WIN32) && LLVM_VERSION_MINOR < 7
  // On Windows we get compilation error saying incompatible object format
  // Reading posts on mailing lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set -elf as the object
  // format; LLVM 3.7 onwards COEFF is supported
  triple_ += "-elf";
#endif
  types_ = new LuaLLVMTypes(context_);
}

// Destroy the JIT state freeing up any
// functions that were compiled
RaviJITStateImpl::~RaviJITStateImpl() {
  std::vector<RaviJITFunction *> todelete;
  for (auto f = std::begin(functions_); f != std::end(functions_); f++) {
    todelete.push_back(f->second);
  }
  // delete all the compiled objects
  for (int i = 0; i < todelete.size(); i++) {
    delete todelete[i];
  }
  delete types_;
}

void RaviJITStateImpl::addGlobalSymbol(const std::string &name, void *address) {
  llvm::sys::DynamicLibrary::AddSymbol(name, address);
}

void RaviJITStateImpl::dump() {
  types_->dump();
  for (auto f : functions_) {
    f.second->dump();
  }
}

// Allocate a JIT Function of specified type
// and linkage - note at this stage the function has no
// implementation
RaviJITFunction *
RaviJITStateImpl::createFunction(llvm::FunctionType *type,
                                 llvm::GlobalValue::LinkageTypes linkage,
                                 const std::string &name) {
  RaviJITFunction *f = new RaviJITFunctionImpl(this, type, linkage, name);
  functions_[name] = f;
  return f;
}

// Unregister a function - to be used when a function is
// destroyed by the Lua garbage collector
void RaviJITStateImpl::deleteFunction(const std::string &name) {
  functions_.erase(name);
  // This is called when RaviJITFunction is deleted
}

RaviJITFunctionImpl::RaviJITFunctionImpl(
    RaviJITStateImpl *owner, llvm::FunctionType *type,
    llvm::GlobalValue::LinkageTypes linkage, const std::string &name)
    : owner_(owner), name_(name), engine_(nullptr), module_(nullptr),
      function_(nullptr), ptr_(nullptr) {
  // MCJIT treats each module as a compilation unit
  // To enable function level life cycle we create a
  // module per function
  std::string moduleName = "ravi_module_" + name;
  module_ = new llvm::Module(moduleName, owner->context());
#if defined(_WIN32) && LLVM_VERSION_MINOR < 7
  // On Windows we get error saying incompatible object format
  // Reading posts on mailing lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set
  // -elf as the object format; LLVM 3.7 onwards COEFF is supported
  module_->setTargetTriple(owner->triple());
#endif

  function_ = llvm::Function::Create(type, linkage, name, module_);

// TODO add stack checks as debug more
// function_->addFnAttr(llvm::Attribute::StackProtectReq);

#if defined(_WIN32)
// TODO On 32-bit Windows we need to force
// 16-byte alignment
// llvm::AttrBuilder attr;
// attr.addStackAlignmentAttr(16);
// function_->addAttributes(
//   llvm::AttributeSet::FunctionIndex,
//   llvm::AttributeSet::get(owner_->context(),
//                         llvm::AttributeSet::FunctionIndex, attr));
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
  if (!engine_) {
    fprintf(stderr, "Could not create ExecutionEngine: %s\n", errStr.c_str());
    return;
  }
}

RaviJITFunctionImpl::~RaviJITFunctionImpl() {
  // Remove this function from parent
  owner_->deleteFunction(name_);
  if (engine_)
    delete engine_;
  else if (module_)
    // if engine was created then we don't need to delete the
    // module as it would have been deleted by the engine
    delete module_;
}

#if 0
// TODO
// Following two functions based upon similar in Clang
static void addAddressSanitizerPasses(const llvm::PassManagerBuilder &Builder,
                                      llvm::PassManagerBase &PM) {
  PM.add(llvm::createAddressSanitizerFunctionPass());
  PM.add(llvm::createAddressSanitizerModulePass());
}

static void addMemorySanitizerPass(const llvm::PassManagerBuilder &Builder,
                                   llvm::PassManagerBase &PM) {
  PM.add(llvm::createMemorySanitizerPass());

  // MemorySanitizer inserts complex instrumentation that mostly follows
  // the logic of the original code, but operates on "shadow" values.
  // It can benefit from re-running some general purpose optimization passes.
  if (Builder.OptLevel > 0) {
    PM.add(llvm::createEarlyCSEPass());
    PM.add(llvm::createReassociatePass());
    PM.add(llvm::createLICMPass());
    PM.add(llvm::createGVNPass());
    PM.add(llvm::createInstructionCombiningPass());
    PM.add(llvm::createDeadStoreEliminationPass());
  }
}
#endif

void RaviJITFunctionImpl::runpasses(bool dumpAsm) {

#if LLVM_VERSION_MINOR == 7
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

#if 0
    // TODO - we want to allow instrumentation of JITed code
    // TODO - it should be controlled via a flag
    // Note that following appears to require linking to some
    // additional LLVM libraries
    pmb.addExtension(llvm::PassManagerBuilder::EP_OptimizerLast,
                     addAddressSanitizerPasses);
    pmb.addExtension(llvm::PassManagerBuilder::EP_EnabledOnOptLevel0,
                     addAddressSanitizerPasses);
    pmb.addExtension(llvm::PassManagerBuilder::EP_OptimizerLast,
                     addMemorySanitizerPass);
    pmb.addExtension(llvm::PassManagerBuilder::EP_EnabledOnOptLevel0,
                     addMemorySanitizerPass);
#endif
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
#elif LLVM_VERSION_MINOR == 7
// Apparently no need to set DataLayout
#else
#error Unsupported LLVM version
#endif
    pmb.populateFunctionPassManager(*FPM);
    FPM->doInitialization();
    FPM->run(*function_);
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
    formatted_stream.flush();
  }
  if (dumpAsm && codestr.length() > 0)
    llvm::errs() << codestr << "\n";
}

void *RaviJITFunctionImpl::compile(bool doDump) {
  if (ptr_)
    // Already compiled
    return ptr_;

  if (!function_ || !engine_)
    // Invalid - something went wrong
    return NULL;

  runpasses();

  // Following will generate very verbose dump when machine code is
  // produced below
  if (doDump) {
    llvm::TargetMachine *TM = engine_->getTargetMachine();
    TM->Options.PrintMachineCode = 1;
  }

  // Upon creation, MCJIT holds a pointer to the Module object
  // that it received from EngineBuilder but it does not immediately
  // generate code for this module. Code generation is deferred
  // until either the MCJIT::finalizeObject method is called
  // explicitly or a function such as MCJIT::getPointerToFunction
  // is called which requires the code to have been generated.
  engine_->finalizeObject();
  ptr_ = engine_->getPointerToFunction(function_);
  return ptr_;
}

llvm::Function *
RaviJITFunctionImpl::addExternFunction(llvm::FunctionType *type, void *address,
                                       const std::string &name) {
  llvm::Function *f = llvm::Function::Create(
      type, llvm::Function::ExternalLinkage, name, module_);
  f->setDoesNotThrow();
  // We should have been able to call
  // engine_->addGlobalMapping() but this doesn't work
  // See http://lists.cs.uiuc.edu/pipermail/llvmdev/2014-April/071856.html
  // See bug report http://llvm.org/bugs/show_bug.cgi?id=20656
  // following will call DynamicLibrary::AddSymbol
  owner_->addGlobalSymbol(name, address);
  return f;
}

void RaviJITFunctionImpl::dump() { module_->dump(); }

// Dumps the machine code
// Will execute the passes as required by currently set
// optimzation level; this may or may not match the actual
// JITed code which would have used the optimzation level set at the
// time of compilation
void RaviJITFunctionImpl::dumpAssembly() { runpasses(true); }

std::unique_ptr<RaviJITState> RaviJITStateFactory::newJITState() {
  return std::unique_ptr<RaviJITState>(new RaviJITStateImpl());
}
}

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_LLVM

#include "lualib.h"
#include "lauxlib.h"

struct ravi_State {
  ravi::RaviJITState *jit;
  ravi::RaviCodeGenerator *code_generator;
};

// Initialize the JIT State and attach it to the
// Global Lua State
// If a JIT State already exists then this function
// will return -1
int raviV_initjit(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state != NULL)
    return -1;
  ravi_State *jit = (ravi_State *)calloc(1, sizeof(ravi_State));
  jit->jit = new ravi::RaviJITStateImpl();
  jit->code_generator =
      new ravi::RaviCodeGenerator((ravi::RaviJITStateImpl *)jit->jit);
  G->ravi_state = jit;
  return 0;
}

// Free up the JIT State
void raviV_close(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL)
    return;
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
int raviV_compile(struct lua_State *L, struct Proto *p, int manual_request,
                  int dump) {
  if (p->ravi_jit.jit_status == 2)
    return true;
  global_State *G = G(L);
  if (G->ravi_state == NULL)
    return 0;
  if (!G->ravi_state->jit->is_enabled()) {
    return 0;
  }
  bool doCompile = (bool)manual_request;
  if (!doCompile && G->ravi_state->jit->is_auto()) {
    if (p->ravi_jit.jit_flags != 0) /* function has fornum loop, so compile */
      doCompile = true;
    else if (p->sizecode >
             G->ravi_state->jit
                 ->get_mincodesize()) /* function is long so compile */
      doCompile = true;
    else {
      if (p->ravi_jit.execution_count <
          G->ravi_state
              ->jit->get_minexeccount()) /* function has been executed many
                                            times so compile */
        p->ravi_jit.execution_count++;
      else
        doCompile = true;
    }
  }
  if (doCompile)
    G->ravi_state->code_generator->compile(L, p, dump != 0);
  return p->ravi_jit.jit_status == 2;
}

// Free the JIT compiled function
// Note that this is called by the garbage collector
void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  if (p->ravi_jit.jit_status == 2) /* compiled */ {
    ravi::RaviJITFunction *f =
        reinterpret_cast<ravi::RaviJITFunction *>(p->ravi_jit.jit_data);
    if (f)
      delete f;
    p->ravi_jit.jit_status = 3;
    p->ravi_jit.jit_function = NULL;
    p->ravi_jit.jit_data = NULL;
    p->ravi_jit.execution_count = 0;
  }
}

// Dump the LLVM IR
void raviV_dumpllvmir(struct lua_State *L, struct Proto *p) {
  if (p->ravi_jit.jit_status == 2) /* compiled */ {
    ravi::RaviJITFunction *f =
        reinterpret_cast<ravi::RaviJITFunction *>(p->ravi_jit.jit_data);
    if (f)
      f->dump();
  }
}

// Dump the LLVM ASM
void raviV_dumpllvmasm(struct lua_State *L, struct Proto *p) {
  if (p->ravi_jit.jit_status == 2) /* compiled */ {
    ravi::RaviJITFunction *f =
        reinterpret_cast<ravi::RaviJITFunction *>(p->ravi_jit.jit_data);
    if (f)
      f->dumpAssembly();
  }
}

// Turn on/off auto JIT compilation
int raviV_auto(lua_State *L) {
  global_State *G = G(L);
  int n = lua_gettop(L);
  if (G->ravi_state == NULL) {
    lua_pushboolean(L, 0);
    lua_pushinteger(L, -1);
    lua_pushinteger(L, -1);
  } else {
    lua_pushboolean(L, G->ravi_state->jit->is_auto());
    lua_pushinteger(L, G->ravi_state->jit->get_mincodesize());
    lua_pushinteger(L, G->ravi_state->jit->get_minexeccount());
  }
  if (G->ravi_state) {
    bool value = false;
    if (n >= 1)
      value = lua_toboolean(L, 1);
    if (n >= 1)
      G->ravi_state->jit->set_auto(value);
    int min_code_size = (n >= 2) ? (int)(lua_tointeger(L, 2)) : -1;
    int min_exec_count = (n == 3) ? (int)(lua_tointeger(L, 3)) : -1;
    if (min_code_size >= 1)
      G->ravi_state->jit->set_mincodesize(min_code_size);
    if (min_exec_count >= 1)
      G->ravi_state->jit->set_minexeccount(min_exec_count);
  }
  return 3;
}

// Turn on/off the JIT compiler
int raviV_jitenable(lua_State *L) {
  global_State *G = G(L);
  int n = lua_gettop(L);
  bool value = false;
  if (n == 1)
    value = lua_toboolean(L, 1);
  if (G->ravi_state == NULL)
    lua_pushboolean(L, 0);
  else
    lua_pushboolean(L, G->ravi_state->jit->is_enabled());
  if (n == 1 && G->ravi_state)
    G->ravi_state->jit->set_enabled(value);
  return 1;
}

// Set LLVM optimization level
int raviV_optlevel(lua_State *L) {
  global_State *G = G(L);
  int n = lua_gettop(L);
  int value = 1;
  if (n == 1)
    value = lua_tointeger(L, 1);
  if (G->ravi_state == NULL)
    lua_pushinteger(L, 0);
  else
    lua_pushinteger(L, G->ravi_state->jit->get_optlevel());
  if (n == 1 && G->ravi_state)
    G->ravi_state->jit->set_optlevel(value);
  return 1;
}

// Set LLVM code size level
int raviV_sizelevel(lua_State *L) {
  global_State *G = G(L);
  int n = lua_gettop(L);
  int value = 0;
  if (n == 1)
    value = lua_tointeger(L, 1);
  if (G->ravi_state == NULL)
    lua_pushinteger(L, 0);
  else
    lua_pushinteger(L, G->ravi_state->jit->get_sizelevel());
  if (n == 1 && G->ravi_state)
    G->ravi_state->jit->set_sizelevel(value);
  return 1;
}

#else

// TODO we probably do not need all the headers
// below

#define lvm_c
#define LUA_CORE

#include "lprefix.h"
#include "lua.h"
#include "lobject.h"
#include "lstate.h"
#include "lualib.h"
#include "lauxlib.h"

#include "ravi_gccjit.h"

// Initialize the JIT State and attach it to the
// Global Lua State
// If a JIT State already exists then this function
// will return -1
int raviV_initjit(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state != NULL)
    return -1;
  ravi_State *jit = (ravi_State *)calloc(1, sizeof(ravi_State));
  jit->jit = ravi_jit_new_context();
  jit->code_generator = ravi_jit_new_codegen(jit->jit);
  G->ravi_state = jit;
  return 0;
}

// Free up the JIT State
void raviV_close(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL)
    return;
  ravi_jit_codegen_free(G->ravi_state->code_generator);
  ravi_jit_context_free(G->ravi_state->jit);
  free(G->ravi_state);
}

// Dump the LLVM IR
void raviV_dumpllvmir(struct lua_State *L, struct Proto *p) {}

// Dump the LLVM ASM
void raviV_dumpllvmasm(struct lua_State *L, struct Proto *p) {}

// Turn on/off auto JIT compilation
int raviV_auto(lua_State *L) { return 0; }

// Turn on/off the JIT compiler
int raviV_jitenable(lua_State *L) { return 0; }

// Set LLVM optimization level
int raviV_optlevel(lua_State *L) { return 0; }

// Set LLVM code size level
int raviV_sizelevel(lua_State *L) { return 0; }

#endif

// Test if the given function is compiled
static int ravi_is_compiled(lua_State *L) {
  int n = lua_gettop(L);
  luaL_argcheck(L, n == 1, 1, "1 argument expected");
  luaL_argcheck(L, lua_isfunction(L, 1) && !lua_iscfunction(L, 1), 1,
                "argument must be a Lua function");
  void *p = (void *)lua_topointer(L, 1);
  LClosure *l = reinterpret_cast<LClosure *>(p);
  lua_pushboolean(L, l->p->ravi_jit.jit_status == 2);
  return 1;
}

// Try to JIT compile the given function
// Optional boolean (second) parameter specifies whether
// to dump the code generation
static int ravi_compile(lua_State *L) {
  int n = lua_gettop(L);
  luaL_argcheck(L, n >= 1, 1, "1 or 2 arguments expected");
  luaL_argcheck(L, lua_isfunction(L, 1) && !lua_iscfunction(L, 1), 1,
                "argument must be a Lua function");
  void *p = (void *)lua_topointer(L, 1);
  LClosure *l = reinterpret_cast<LClosure *>(p);
  int manualRequest = 1;
  // Is there a second boolean parameter requesting
  // dump of code generation?
  int dumpAsm = (n == 2) ? lua_toboolean(L, 2) : 0;
  int result = raviV_compile(L, l->p, manualRequest, dumpAsm);
  lua_pushboolean(L, result);
  return 1;
}

// Dump Lua bytecode of the supplied function
static int ravi_dump_luacode(lua_State *L) {
  int n = lua_gettop(L);
  luaL_argcheck(L, n == 1, 1, "1 argument expected");
  luaL_argcheck(L, lua_isfunction(L, 1) && !lua_iscfunction(L, 1), 1,
                "argument must be a Lua function");
  ravi_dump_function(L);
  return 0;
}

// Dump LLVM IR of the supplied function
// if it has been compiled
static int ravi_dump_llvmir(lua_State *L) {
  int n = lua_gettop(L);
  luaL_argcheck(L, n == 1, 1, "1 argument expected");
  luaL_argcheck(L, lua_isfunction(L, 1) && !lua_iscfunction(L, 1), 1,
                "argument must be a Lua function");
  void *p = (void *)lua_topointer(L, 1);
  LClosure *l = reinterpret_cast<LClosure *>(p);
  raviV_dumpllvmir(L, l->p);
  return 0;
}

// Dump LLVM ASM of the supplied function
// if it has been compiled
static int ravi_dump_llvmasm(lua_State *L) {
  int n = lua_gettop(L);
  luaL_argcheck(L, n == 1, 1, "1 argument expected");
  luaL_argcheck(L, lua_isfunction(L, 1) && !lua_iscfunction(L, 1), 1,
                "argument must be a Lua function");
  void *p = (void *)lua_topointer(L, 1);
  LClosure *l = reinterpret_cast<LClosure *>(p);
  raviV_dumpllvmasm(L, l->p);
  return 0;
}

// Turn on/off auto JIT compilation
static int ravi_auto(lua_State *L) { return raviV_auto(L); }

// Turn on/off the JIT compiler
static int ravi_jitenable(lua_State *L) { return raviV_jitenable(L); }

// Set LLVM optimization level
static int ravi_optlevel(lua_State *L) { return raviV_optlevel(L); }

// Set LLVM code size level
static int ravi_sizelevel(lua_State *L) { return raviV_sizelevel(L); }

static const luaL_Reg ravilib[] = {{"iscompiled", ravi_is_compiled},
                                   {"compile", ravi_compile},
                                   {"dumplua", ravi_dump_luacode},
                                   {"dumpllvm", ravi_dump_llvmir},
                                   {"dumpllvmasm", ravi_dump_llvmasm},
                                   {"auto", ravi_auto},
                                   {"jit", ravi_jitenable},
                                   {"optlevel", ravi_optlevel},
                                   {"sizelevel", ravi_sizelevel},
                                   {NULL, NULL}};

LUAMOD_API int raviopen_llvmjit(lua_State *L) {
  luaL_newlib(L, ravilib);
  return 1;
}

#ifdef __cplusplus
}
#endif

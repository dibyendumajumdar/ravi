/**
* Copyright (c) Dibyendu Majumdar
* 2015
*/

#include "ravi_llvmcodegen.h"

namespace ravi {

// TODO - should probably be an atomic int
static std::atomic_int init;

RaviJITState *RaviJITFunctionImpl::owner() const { return owner_; }

RaviJITStateImpl::RaviJITStateImpl() : context_(llvm::getGlobalContext()) {
  // Unless following three lines are not executed then
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
#ifdef _WIN32
  // On Windows we get compilation error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set -elf as the object
  // format
  triple_ += "-elf";
#endif
  types_ = new LuaLLVMTypes(context_);
}

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

RaviJITFunction *
RaviJITStateImpl::createFunction(llvm::FunctionType *type,
                                 llvm::GlobalValue::LinkageTypes linkage,
                                 const std::string &name) {
  RaviJITFunction *f =
      new RaviJITFunctionImpl(this, type, linkage, name);
  functions_[name] = f;
  return f;
}

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
#if defined(_WIN32)
  // On Windows we get error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set
  // -elf as the object format
  module_->setTargetTriple(owner->triple());
#endif

  function_ = llvm::Function::Create(type, linkage, name, module_);
  std::string errStr;
#if LLVM_VERSION_MINOR > 5
  // LLVM 3.6.0 change
  std::unique_ptr<llvm::Module> module(module_);
  engine_ = llvm::EngineBuilder(std::move(module))
                .setEngineKind(llvm::EngineKind::JIT)
                .setErrorStr(&errStr)
                .create();
#else
  engine_ = llvm::EngineBuilder(module_)
    .setEngineKind(llvm::EngineKind::JIT)
    .setUseMCJIT(true)
    .setErrorStr(&errStr)
    .create();
#endif
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

void *RaviJITFunctionImpl::compile() {

  //module_->dump();

  // Create a function pass manager for this engine
  llvm::FunctionPassManager *FPM = new llvm::FunctionPassManager(module_);

  // Set up the optimizer pipeline.  Start with registering info about how the
  // target lays out data structures.
#if LLVM_VERSION_MINOR > 5
  // LLVM 3.6.0 change
  module_->setDataLayout(engine_->getDataLayout());
  FPM->add(new llvm::DataLayoutPass());
#else
  auto target_layout = engine_->getTargetMachine()->getDataLayout();
  module_->setDataLayout(target_layout);
  FPM->add(new llvm::DataLayoutPass(*engine_->getDataLayout()));
#endif
  // Provide basic AliasAnalysis support for GVN.
  FPM->add(llvm::createBasicAliasAnalysisPass());
  // Promote allocas to registers.
  FPM->add(llvm::createPromoteMemoryToRegisterPass());
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  FPM->add(llvm::createInstructionCombiningPass());
  // Reassociate expressions.
  FPM->add(llvm::createReassociatePass());
  FPM->add(llvm::createTypeBasedAliasAnalysisPass());
  // Eliminate Common SubExpressions.
  FPM->add(llvm::createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  FPM->add(llvm::createCFGSimplificationPass());
  FPM->doInitialization();

  // For each function in the module
  // Run the FPM on this function
  FPM->run(*function_);

  //module_->dump();

  // We don't need this anymore
  delete FPM;

  if (ptr_)
    return ptr_;
  if (!function_ || !engine_)
    return NULL;

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

llvm::Constant *
RaviJITFunctionImpl::addExternFunction(llvm::FunctionType *type, void *address,
                                       const std::string &name) {
  llvm::Function *f = llvm::Function::Create(
      type, llvm::Function::ExternalLinkage, name, module_);
  // We should have been able to call
  // engine_->addGlobalMapping() but this doesn't work
  // See http://lists.cs.uiuc.edu/pipermail/llvmdev/2014-April/071856.html
  // See bug report http://llvm.org/bugs/show_bug.cgi?id=20656
  // following will call DynamicLibrary::AddSymbol
  owner_->addGlobalSymbol(name, address);
  return f;
}

void RaviJITFunctionImpl::dump() { module_->dump(); }

std::unique_ptr<RaviJITState> RaviJITStateFactory::newJITState() {
  return std::unique_ptr<RaviJITState>(new RaviJITStateImpl());
}

}

#ifdef __cplusplus
extern "C" {
#endif

struct ravi_State {
  ravi::RaviJITState *jit;
  ravi::RaviCodeGenerator *code_generator;
};

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

void raviV_close(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL)
    return;
  delete G->ravi_state->code_generator;
  delete G->ravi_state->jit;
  free(G->ravi_state);
}

int raviV_compile(struct lua_State *L, struct Proto *p) {
  global_State *G = G(L);
  if (G->ravi_state == NULL)
    return 0;
  G->ravi_state->code_generator->compile(L, p);
  return 0;
}

void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  if (p->ravi_jit.jit_status == 2) /* compiled */ {
    ravi::RaviJITFunction *f =
        reinterpret_cast<ravi::RaviJITFunction *>(p->ravi_jit.jit_data);
    if (f)
      delete f;
    p->ravi_jit.jit_status = 3;
    p->ravi_jit.jit_function = NULL;
    p->ravi_jit.jit_data = NULL;
  }
}

#ifdef __cplusplus
}
#endif

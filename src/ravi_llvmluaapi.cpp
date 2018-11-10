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

/*
This will hopefully eventually contain a Lua binding for LLVM.
*/

#if USE_DMR_C
#include <dmr_c.h>
#endif
#include <ravijit.h>
#include "ravi_llvmcodegen.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lauxlib.h"

// Utility to extract a boolean field from a table
static bool l_table_get_bool(lua_State *L, int idx, const char *key,
                             bool *result, bool default_value) {
  bool rc = false;
  lua_pushstring(L, key);
  lua_gettable(L, idx); /* get table[key] */
  if (!lua_isboolean(L, -1))
    *result = default_value;
  else {
    *result = lua_toboolean(L, -1) != 0;
    rc = true;
  }
  lua_pop(L, 1); /* remove value */
  return rc;
}

// We need fixed pointer values for metatable keys
static const char *LLVM_context = "LLVMcontext";
static const char *LLVM_irbuilder = "LLVMirbuilder";
static const char *LLVM_module = "LLVMmodule";
static const char *LLVM_mainfunction = "LLVMmainfunction";
static const char *LLVM_function = "LLVMfunction";
static const char *LLVM_type = "LLVMtype";
static const char *LLVM_value = "LLVMvalue";
static const char *LLVM_structtype = "LLVMstructtype";
static const char *LLVM_pointertype = "LLVMpointertype";
static const char *LLVM_functiontype = "LLVMfunctiontype";
static const char *LLVM_basicblock = "LLVMbasicblock";
static const char *LLVM_constant = "LLVMconstant";
static const char *LLVM_instruction = "LLVMinstruction";
static const char *LLVM_phinode = "LLVMphinode";

#define test_LLVM_irbuilder(L, idx) \
  ((IRBuilderHolder *)luaL_testudata(L, idx, LLVM_irbuilder))
#define check_LLVM_irbuilder(L, idx) \
  ((IRBuilderHolder *)luaL_checkudata(L, idx, LLVM_irbuilder))

#define test_LLVM_type(L, idx) \
  ((TypeHolder *)luaL_testudata(L, idx, LLVM_type))
#define check_LLVM_type(L, idx) \
  ((TypeHolder *)luaL_checkudata(L, idx, LLVM_type))

#define test_LLVM_context(L, idx) \
  ((ContextHolder *)luaL_testudata(L, idx, LLVM_context))
#define check_LLVM_context(L, idx) \
  ((ContextHolder *)luaL_checkudata(L, idx, LLVM_context))

#define test_LLVM_structtype(L, idx) \
  ((StructTypeHolder *)luaL_testudata(L, idx, LLVM_structtype))
#define check_LLVM_structtype(L, idx) \
  ((StructTypeHolder *)luaL_checkudata(L, idx, LLVM_structtype))

#define test_LLVM_pointertype(L, idx) \
  ((PointerTypeHolder *)luaL_testudata(L, idx, LLVM_pointertype))
#define check_LLVM_pointertype(L, idx) \
  ((PointerTypeHolder *)luaL_checkudata(L, idx, LLVM_pointertype))

#define test_LLVM_functiontype(L, idx) \
  ((FunctionTypeHolder *)luaL_testudata(L, idx, LLVM_functiontype))
#define check_LLVM_functiontype(L, idx) \
  ((FunctionTypeHolder *)luaL_checkudata(L, idx, LLVM_functiontype))

#define test_LLVM_mainfunction(L, idx) \
  ((MainFunctionHolder *)luaL_testudata(L, idx, LLVM_mainfunction))
#define check_LLVM_mainfunction(L, idx) \
  ((MainFunctionHolder *)luaL_checkudata(L, idx, LLVM_mainfunction))

#define test_LLVM_function(L, idx) \
  ((FunctionHolder *)luaL_testudata(L, idx, LLVM_function))
#define check_LLVM_function(L, idx) \
  ((FunctionHolder *)luaL_checkudata(L, idx, LLVM_function))

#define test_LLVM_basicblock(L, idx) \
  ((BasicBlockHolder *)luaL_testudata(L, idx, LLVM_basicblock))
#define check_LLVM_basicblock(L, idx) \
  ((BasicBlockHolder *)luaL_checkudata(L, idx, LLVM_basicblock))

#define test_LLVM_value(L, idx) \
  ((ValueHolder *)luaL_testudata(L, idx, LLVM_value))
#define check_LLVM_value(L, idx) \
  ((ValueHolder *)luaL_checkudata(L, idx, LLVM_value))

#define test_LLVM_constant(L, idx) \
  ((ConstantHolder *)luaL_testudata(L, idx, LLVM_constant))
#define check_LLVM_constant(L, idx) \
  ((ConstantHolder *)luaL_checkudata(L, idx, LLVM_constant))

#define test_LLVM_instruction(L, idx) \
  ((InstructionHolder *)luaL_testudata(L, idx, LLVM_instruction))
#define check_LLVM_instruction(L, idx) \
  ((InstructionHolder *)luaL_checkudata(L, idx, LLVM_instruction))

#define test_LLVM_module(L, idx) \
  ((ModuleHolder *)luaL_testudata(L, idx, LLVM_module))
#define check_LLVM_module(L, idx) \
  ((ModuleHolder *)luaL_checkudata(L, idx, LLVM_module))

#define test_LLVM_phinode(L, idx) \
  ((PhiNodeHolder *)luaL_testudata(L, idx, LLVM_phinode))
#define check_LLVM_phinode(L, idx) \
  ((PhiNodeHolder *)luaL_checkudata(L, idx, LLVM_phinode))

struct ContextHolder {
  /* Each Ravi instance (Lua instance) has its own
   * LLVM context
   * Lifetime of this is tied to the Lua instance so
   * no need to garbage collect
   */
  ravi::RaviJITState *jitState;
};

/* garbage collected */
struct IRBuilderHolder {
  llvm::IRBuilder<> *builder;
};

struct ValueHolder {
  llvm::Value *value;
};

struct TypeHolder {
  llvm::Type *type;
};

struct StructTypeHolder {
  llvm::StructType *type;
};

struct PointerTypeHolder {
  llvm::PointerType *type;
};

struct FunctionTypeHolder {
  llvm::FunctionType *type;
};

/* garbage collected */
struct MainFunctionHolder {
  ravi::RaviJITFunction *func;
  lua_CFunction compiled_func;
  llvm::Value *arg1;
  bool is_valid;
};

struct FunctionHolder {
  llvm::Function *function;
};

struct BasicBlockHolder {
  llvm::BasicBlock *b;
};

struct ConstantHolder {
  llvm::Constant *constant;
};

struct InstructionHolder {
  llvm::Instruction *i;
};

struct ModuleHolder {
  std::shared_ptr<ravi::RaviJITModule> M;

  ModuleHolder(const std::shared_ptr<ravi::RaviJITModule> &module) {
    M = module;
    // printf("ModuleHolder created\n");
  }
  ~ModuleHolder() { 
      //printf("ModuleHolder destroyed\n");
  }
};

struct PhiNodeHolder {
  llvm::PHINode *phi;
};

static int context_new_LLVM_irbuilder(lua_State *L) {
  ContextHolder *context = check_LLVM_context(L, 1);
  IRBuilderHolder *builder =
      (IRBuilderHolder *)lua_newuserdata(L, sizeof(IRBuilderHolder));
  builder->builder = new llvm::IRBuilder<>(context->jitState->context());
  luaL_getmetatable(L, LLVM_irbuilder);
  lua_setmetatable(L, -2);
  return 1;
}

/* __gc function for IRBuilderHolder */
static int collect_LLVM_irbuilder(lua_State *L) {
  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  if (builder->builder) {
    delete builder->builder;
    builder->builder = nullptr;
  }
  return 0;
}

static void alloc_LLVM_module(lua_State *L,
                              const std::shared_ptr<ravi::RaviJITModule> &M) {
  ModuleHolder *mh = (ModuleHolder *)lua_newuserdata(L, sizeof(ModuleHolder));
  luaL_getmetatable(L, LLVM_module);
  lua_setmetatable(L, -2);
  new (mh) ModuleHolder(M);
}

/* __gc for ModuleHolder */
static int collect_LLVM_module(lua_State *L) {
  ModuleHolder *mh = check_LLVM_module(L, 1);
  // printf("Module released: usecount %d\n", (int)mh->M.use_count());
  mh->~ModuleHolder();
  return 0;
}

static void alloc_LLVM_type(lua_State *L, llvm::Type *t) {
  TypeHolder *tt = (TypeHolder *)lua_newuserdata(L, sizeof(TypeHolder));
  luaL_getmetatable(L, LLVM_type);
  lua_setmetatable(L, -2);
  tt->type = t;
}

static void alloc_LLVM_context(lua_State *L, ravi::RaviJITState *jit) {
  ContextHolder *h = (ContextHolder *)lua_newuserdata(L, sizeof(ContextHolder));
  luaL_getmetatable(L, LLVM_context);
  lua_setmetatable(L, -2);
  h->jitState = jit;
}

static void alloc_LLVM_value(lua_State *L, llvm::Value *v) {
  ValueHolder *h = (ValueHolder *)lua_newuserdata(L, sizeof(ValueHolder));
  luaL_getmetatable(L, LLVM_value);
  lua_setmetatable(L, -2);
  h->value = v;
}

static void alloc_LLVM_structtype(lua_State *L, llvm::StructType *type) {
  StructTypeHolder *h =
      (StructTypeHolder *)lua_newuserdata(L, sizeof(StructTypeHolder));
  luaL_getmetatable(L, LLVM_structtype);
  lua_setmetatable(L, -2);
  h->type = type;
}

static void alloc_LLVM_pointertype(lua_State *L, llvm::PointerType *type) {
  PointerTypeHolder *h =
      (PointerTypeHolder *)lua_newuserdata(L, sizeof(PointerTypeHolder));
  luaL_getmetatable(L, LLVM_pointertype);
  lua_setmetatable(L, -2);
  h->type = type;
}

static void alloc_LLVM_functiontype(lua_State *L, llvm::FunctionType *type) {
  FunctionTypeHolder *h =
      (FunctionTypeHolder *)lua_newuserdata(L, sizeof(FunctionTypeHolder));
  luaL_getmetatable(L, LLVM_functiontype);
  lua_setmetatable(L, -2);
  h->type = type;
}

static void alloc_LLVM_function(lua_State *L, llvm::Function *f) {
  FunctionHolder *h =
      (FunctionHolder *)lua_newuserdata(L, sizeof(FunctionHolder));
  luaL_getmetatable(L, LLVM_function);
  lua_setmetatable(L, -2);
  h->function = f;
}

static void alloc_LLVM_constant(lua_State *L, llvm::Constant *f) {
  ConstantHolder *h =
      (ConstantHolder *)lua_newuserdata(L, sizeof(ConstantHolder));
  luaL_getmetatable(L, LLVM_constant);
  lua_setmetatable(L, -2);
  h->constant = f;
}

static void alloc_LLVM_instruction(lua_State *L, llvm::Instruction *i) {
  InstructionHolder *h =
      (InstructionHolder *)lua_newuserdata(L, sizeof(InstructionHolder));
  luaL_getmetatable(L, LLVM_instruction);
  lua_setmetatable(L, -2);
  h->i = i;
}

static void alloc_LLVM_phinode(lua_State *L, llvm::PHINode *phi) {
  PhiNodeHolder *h = (PhiNodeHolder *)lua_newuserdata(L, sizeof(PhiNodeHolder));
  luaL_getmetatable(L, LLVM_phinode);
  lua_setmetatable(L, -2);
  h->phi = phi;
}

// Creates a new module to hold a Lua C Function
static MainFunctionHolder *alloc_LLVM_mainfunction(lua_State *L,
                                                   ravi::RaviJITState *jit,
                                                   llvm::FunctionType *type,
                                                   const char *name) {
  MainFunctionHolder *h =
      (MainFunctionHolder *)lua_newuserdata(L, sizeof(MainFunctionHolder));
  h->func = nullptr;
  h->compiled_func = nullptr;
  h->arg1 = nullptr;
  luaL_getmetatable(L, LLVM_mainfunction);
  lua_setmetatable(L, -2);
  auto module = std::make_shared<ravi::RaviJITModule>(G(L)->ravi_state->jit);
  h->func = new ravi::RaviJITFunction(&h->compiled_func, module, type,
                                      llvm::Function::ExternalLinkage, name);
  h->is_valid = true;
  return h;
}

static bool validate_has_luaCFunction_signature(MainFunctionHolder *h,
                                                char *errormessage, int len) {
  auto f = h->func->function();
  if (!f) {
    snprintf(errormessage, len, "Function was not found");
    return false;
  }
  auto ftype = f->getFunctionType();
  auto returnType = ftype->getReturnType();
  if (!returnType->isIntegerTy()) {
    snprintf(errormessage, len,
             "Invalid return type in function: expected integer");
    return false;
  }
  if (ftype->isVarArg()) {
    // invalid
    snprintf(errormessage, len,
             "Invalid function: cannot be variable argument type");
    return false;
  }
  if (ftype->getFunctionNumParams() != 1) {
    // invalid
    snprintf(errormessage, len,
             "Invalid function: function must accept one argument (struct lua_State *) only");
    return false;
  }
  auto paramType = ftype->getFunctionParamType(0);
  if (!paramType->isPointerTy()) {
    // invalid
    snprintf(errormessage, len,
        "Invalid function: argument is not a pointer to struct lua_State");
    return false;
  }
  auto underLying = paramType->getPointerElementType();
  if (!underLying->isStructTy()) {
    // invalid
    snprintf(errormessage, len,
        "Invalid function: argument is not a pointer to struct lua_State");
    return false;
  }
  if (!underLying->getStructName().startswith("struct.lua_State")) {
    // invalid
    snprintf(
        errormessage, len,
        "Invalid function: argument is not a pointer to struct lua_State");
    return false;
  }
  return true;
}

// References an existing function in a Module
static MainFunctionHolder *alloc_LLVM_luaCfunction(
    lua_State *L, const std::shared_ptr<ravi::RaviJITModule> &module,
    const char *name) {
    char error_message[128];
  MainFunctionHolder *h =
      (MainFunctionHolder *)lua_newuserdata(L, sizeof(MainFunctionHolder));
  h->func = nullptr;
  h->compiled_func = nullptr;
  h->arg1 = nullptr;
  luaL_getmetatable(L, LLVM_mainfunction);
  lua_setmetatable(L, -2);
  h->func = new ravi::RaviJITFunction(&h->compiled_func, module, name);
  h->is_valid = validate_has_luaCFunction_signature(h, error_message, sizeof error_message);
  if (!h->is_valid) {
      luaL_argerror(L, 2, error_message);
  }
  return h;
}

/* __gc for FunctionHolder */
static int collect_LLVM_mainfunction(lua_State *L) {
  MainFunctionHolder *builder = check_LLVM_mainfunction(L, 1);
  if (builder->func) {
    delete builder->func;
    builder->func = nullptr;
    //printf("Collected MainFunctionHolder\n");
  }
  return 0;
}

static void alloc_LLVM_basicblock(lua_State *L, llvm::BasicBlock *b) {
  BasicBlockHolder *h =
      (BasicBlockHolder *)lua_newuserdata(L, sizeof(BasicBlockHolder));
  luaL_getmetatable(L, LLVM_basicblock);
  lua_setmetatable(L, -2);
  h->b = b;
}

/*
 Create a new struct type
 structtype(name)
*/
static int context_new_struct_type(lua_State *L) {
  ContextHolder *context = check_LLVM_context(L, 1);
  const char *name = luaL_checkstring(L, 2);
  alloc_LLVM_structtype(
      L, llvm::StructType::create(context->jitState->context(), name));
  return 1;
}

/*
 Create a pointer type
 pointertype(type)
*/
static int context_new_pointer_type(lua_State *L) {
  StructTypeHolder *sth = nullptr;
  PointerTypeHolder *ph = nullptr;
  TypeHolder *th = nullptr;
  FunctionTypeHolder *fth = nullptr;

  sth = test_LLVM_structtype(L, 2);
  if (sth) {
    alloc_LLVM_pointertype(L, llvm::PointerType::get(sth->type, 0));
    goto done;
  }
  ph = test_LLVM_pointertype(L, 2);
  if (ph) {
    alloc_LLVM_pointertype(L, llvm::PointerType::get(ph->type, 0));
    goto done;
  }
  th = test_LLVM_type(L, 2);
  if (th) {
    alloc_LLVM_pointertype(L, llvm::PointerType::get(th->type, 0));
    goto done;
  }
  fth = test_LLVM_functiontype(L, 2);
  if (fth) {
    alloc_LLVM_pointertype(L, llvm::PointerType::get(fth->type, 0));
    goto done;
  }
  luaL_argerror(L, 2, "invalid argument");
  return 0;
done:
  return 1;
}

/*
 Get pre-defined types
*/
static int context_get_types(lua_State *L) {
  ContextHolder *context = check_LLVM_context(L, 1);
  auto jit = context->jitState;

  lua_newtable(L);
  alloc_LLVM_type(L, jit->types()->C_intT);
  lua_setfield(L, -2, "int");
  alloc_LLVM_type(L, jit->types()->lua_NumberT);
  lua_setfield(L, -2, "lua_Number");
  alloc_LLVM_type(L, jit->types()->lua_IntegerT);
  lua_setfield(L, -2, "lua_Integer");
  alloc_LLVM_type(L, jit->types()->lu_byteT);
  lua_setfield(L, -2, "lu_byte");
  alloc_LLVM_pointertype(L, jit->types()->C_pcharT);
  lua_setfield(L, -2, "pchar");
  alloc_LLVM_type(L, jit->types()->C_doubleT);
  lua_setfield(L, -2, "double");
  alloc_LLVM_type(L, jit->types()->C_int64_t);
  lua_setfield(L, -2, "int64_t");
  alloc_LLVM_type(L, jit->types()->C_intptr_t);
  lua_setfield(L, -2, "intptr_t");
  alloc_LLVM_pointertype(L, jit->types()->C_pintT);
  lua_setfield(L, -2, "pint");
  alloc_LLVM_type(L, jit->types()->C_ptrdiff_t);
  lua_setfield(L, -2, "ptrdiff_t");
  alloc_LLVM_type(L, jit->types()->C_shortT);
  lua_setfield(L, -2, "short");
  alloc_LLVM_type(L, jit->types()->C_size_t);
  lua_setfield(L, -2, "size_t");
  alloc_LLVM_pointertype(L, jit->types()->C_psize_t);
  lua_setfield(L, -2, "psize_t");
  alloc_LLVM_type(L, jit->types()->lua_UnsignedT);
  lua_setfield(L, -2, "lua_Unsigned");
  alloc_LLVM_type(L, jit->types()->C_voidT);
  lua_setfield(L, -2, "void");
  alloc_LLVM_functiontype(L, jit->types()->lua_CFunctionT);
  lua_setfield(L, -2, "lua_CFunction");

  return 1;
}

static int get_llvm_context(lua_State *L) {
  auto jit = RaviJIT(L);
  alloc_LLVM_context(L, jit);
  return 1;
}

static llvm::Value *get_value(lua_State *L, int idx) {
  InstructionHolder *ii = test_LLVM_instruction(L, idx);
  if (ii) { return ii->i; }
  ValueHolder *v = test_LLVM_value(L, idx);
  if (v) { return v->value; }
  ConstantHolder *c = test_LLVM_constant(L, idx);
  if (c) { return c->constant; }
  MainFunctionHolder *m = test_LLVM_mainfunction(L, idx);
  if (m) { return m->func->function(); }
  FunctionHolder *f = test_LLVM_function(L, idx);
  if (f) { return f->function; }
  PhiNodeHolder *phi = test_LLVM_phinode(L, idx);
  if (phi) { return phi->phi; }
  luaL_argerror(L, idx, "Value expected");
  return nullptr;
}

/*
 Dump an LLVM object
*/
static int dump_content(lua_State *L) {
#if LLVM_VERSION_MAJOR < 5
  TypeHolder *th = nullptr;
  StructTypeHolder *sth = nullptr;
  PointerTypeHolder *ph = nullptr;
  FunctionTypeHolder *fh = nullptr;
  MainFunctionHolder *f = nullptr;
  InstructionHolder *ii = nullptr;
  ModuleHolder *m = nullptr;
  PhiNodeHolder *phi = nullptr;

  th = test_LLVM_type(L, 1);
  if (th) {
    th->type->dump();
    return 0;
  }
  sth = test_LLVM_structtype(L, 1);
  if (sth) {
    sth->type->dump();
    return 0;
  }
  ph = test_LLVM_pointertype(L, 1);
  if (ph) {
    ph->type->dump();
    return 0;
  }
  fh = test_LLVM_functiontype(L, 1);
  if (fh) {
    fh->type->dump();
    return 0;
  }
  f = test_LLVM_mainfunction(L, 1);
  if (f) {
    f->func->dump();
    return 0;
  }
  ii = test_LLVM_instruction(L, 1);
  if (ii) {
    ii->i->dump();
    return 0;
  }
  m = test_LLVM_module(L, 1);
  if (m) {
    m->M->dump();
    return 0;
  }
  phi = test_LLVM_phinode(L, 1);
  if (phi) {
    phi->phi->dump();
    return 0;
  }
#endif
  return 0;
}

/*
 Add members to a struct
 addmembers(struct, { members })
*/
static int struct_add_members(lua_State *L) {
  StructTypeHolder *sh = check_LLVM_structtype(L, 1);
  luaL_argcheck(L, lua_istable(L, 2), 2, "table expected");
  int len = luaL_len(L, 2);
  std::vector<llvm::Type *> elements;
  for (int i = 1; i <= len; i++) {
    lua_rawgeti(L, 2, i);
    TypeHolder *th = nullptr;
    StructTypeHolder *sth = nullptr;
    PointerTypeHolder *ph = test_LLVM_pointertype(L, -1);
    if (ph) {
      elements.push_back(ph->type);
      goto done;
    }
    th = test_LLVM_type(L, -1);
    if (th) {
      elements.push_back(th->type);
      goto done;
    }
    sth = test_LLVM_structtype(L, -1);
    if (sth) {
      elements.push_back(sth->type);
      goto done;
    }
  done:
    lua_pop(L, 1);
  }
  sh->type->setBody(elements);
  lua_pushinteger(L, elements.size());
  return 1;
}

static llvm::Type *return_type(lua_State *L, int idx) {
  TypeHolder *th = test_LLVM_type(L, idx);
  if (th) { return th->type; }
  PointerTypeHolder *ph = test_LLVM_pointertype(L, idx);
  if (ph) { return ph->type; }
  luaL_argerror(L, idx, "unsupported function return type");
  return nullptr;
}

static llvm::Type *arg_type(lua_State *L, int idx) {
  TypeHolder *th = test_LLVM_type(L, idx);
  if (th) { return th->type; }
  PointerTypeHolder *ph = test_LLVM_pointertype(L, idx);
  if (ph) { return ph->type; }
  luaL_argerror(L, idx, "unsupported function argument type");
  return nullptr;
}

/*
  functiontype(context, rettype, { argtypes } [, { vararg=true })
*/
static int context_new_function_type(lua_State *L) {
  llvm::Type *typeret = return_type(L, 2);
  luaL_argcheck(L, lua_istable(L, 3), 3, "table expected");
  int len = luaL_len(L, 3);
  std::vector<llvm::Type *> elements;
  for (int i = 1; i <= len; i++) {
    lua_rawgeti(L, 3, i);
    llvm::Type *t = arg_type(L, -1);
    if (t) elements.push_back(t);
    lua_pop(L, 1);
  }
  bool vararg = false;
  if (lua_istable(L, 4)) l_table_get_bool(L, 4, "vararg", &vararg, false);
  alloc_LLVM_functiontype(L,
                          llvm::FunctionType::get(typeret, elements, vararg));
  return 1;
}

llvm::Function *get_function(lua_State *L, int idx) {
  llvm::Function *func = nullptr;
  MainFunctionHolder *f = test_LLVM_mainfunction(L, idx);
  if (f)
    func = f->func->function();
  else {
    FunctionHolder *f = check_LLVM_function(L, idx);
    func = f->function;
  }
  return func;
}

static int context_new_lua_CFunction(lua_State *L) {
  ContextHolder *context = check_LLVM_context(L, 1);
  const char *name = luaL_checkstring(L, 2);
  MainFunctionHolder *h = alloc_LLVM_mainfunction(
      L, context->jitState, context->jitState->types()->lua_CFunctionT, name);
  /* set L arg */
  h->arg1 = &(*h->func->function()->arg_begin());
  h->arg1->setName("L");
  return 1;
}

static int context_new_module(lua_State *L) {
  ContextHolder *context = check_LLVM_context(L, 1);
  alloc_LLVM_module(L,
                    std::make_shared<ravi::RaviJITModule>(context->jitState));
  return 1;
}

static int func_getmodule(lua_State *L) {
  MainFunctionHolder *f = check_LLVM_mainfunction(L, 1);
  alloc_LLVM_module(L, f->func->raviModule());
  return 1;
}

static int module_newfunction(lua_State *L) {
  ModuleHolder *mh = check_LLVM_module(L, 1);
  const char *name = luaL_checkstring(L, 2);
  FunctionTypeHolder *fth = check_LLVM_functiontype(L, 3);
  bool extern_linkage = false;
  if (lua_istable(L, 4))
    l_table_get_bool(L, 4, "extern", &extern_linkage, false);
  llvm::Function *f =
      llvm::Function::Create(fth->type,
                             extern_linkage ? llvm::Function::ExternalLinkage
                                            : llvm::Function::InternalLinkage,
                             name, mh->M->module());
  alloc_LLVM_function(L, f);
  return 1;
}

static int module_getfunction(lua_State *L) {
  ModuleHolder *mh = check_LLVM_module(L, 1);
  const char *name = luaL_checkstring(L, 2);
  alloc_LLVM_luaCfunction(L, mh->M, name);
  // TODO we should check that the function signature is correct
  return 1;
}

enum {
    MAX_ARGS = 50,
    MAX_BUFFER = 4096
};

static int collect_args(lua_State *L, int tabindex, char *argv[], int maxargs,
                        char *buf, int buflen) {
  char errormessage[128];
  int len = lua_rawlen(L, tabindex);
  if (len > maxargs) {
    snprintf(errormessage, sizeof errormessage,
             "Arguments exceed total count %d elements", maxargs);
    luaL_argerror(L, 2, errormessage);
    len = maxargs;
  }
  char *p = buf;
  char *endp = buf + buflen;
  int n = 0;
  for (int i = 0; i < len; i++) {
    lua_rawgeti(L, tabindex, i + 1);
    size_t size = 0;
    const char *argument = luaL_checklstring(L, -1, &size);
    if (argument && size > 0) {
      if (p + size + 1 >= endp) {
        snprintf(errormessage, sizeof errormessage,
                 "Arguments exceed combined size of %d bytes", buflen);
        luaL_argerror(L, 2, errormessage);
        break;
      }
      strncpy(p, argument, size + 1);
      argv[n] = p;
      p += (size + 1);
      n++;
    }
  }
  assert(p <= endp);
  return n;
}

static int module_compile_C(lua_State *L) {
  ModuleHolder *mh = check_LLVM_module(L, 1);
  const char *codebuffer = NULL;  //  luaL_checkstring(L, 2);
  char *argv[MAX_ARGS + 1] = {NULL};
  int argc = 0;
  char buf[MAX_BUFFER + 1] = {0};
  int guard = 0xfefefefe;

  if (lua_istable(L, 2)) {
    argc = collect_args(L, 2, argv, MAX_ARGS, buf, sizeof buf);
    assert(argc >= 0 && argc <= MAX_ARGS);
    assert(argv[MAX_ARGS] == NULL);
    assert(guard == 0xfefefefe);
    if (lua_isstring(L, 3)) { codebuffer = lua_tostring(L, 3); }
  }
  else if (lua_isstring(L, 2)) {
    codebuffer = lua_tostring(L, 2);
  }

#if USE_DMR_C
  if (dmrC_llvmcompile(argc, argv, llvm::wrap(mh->M->module()), codebuffer)) {
    lua_pushboolean(L, true);
  }
  else {
    lua_pushboolean(L, false);
  }
#else
  (void)mh;
  (void)codebuffer;
  lua_pushboolean(L, false);
#endif
  return 1;
}

static int module_generate_code(lua_State *L) {
  ModuleHolder *mh = check_LLVM_module(L, 1);
  mh->M->runpasses();
  mh->M->finalize();
  return 0;
}

static int context_new_basicblock(lua_State *L) {
  ContextHolder *context = check_LLVM_context(L, 1);
  const char *name = luaL_checkstring(L, 2);
  alloc_LLVM_basicblock(
      L, llvm::BasicBlock::Create(context->jitState->context(), name));
  return 1;
}

static int func_append_basicblock(lua_State *L) {
  llvm::Function *func = get_function(L, 1);
  BasicBlockHolder *b = check_LLVM_basicblock(L, 2);
  func->getBasicBlockList().push_back(b->b);
  return 0;
}

static int irbuilder_set_current_block(lua_State *L) {
  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  BasicBlockHolder *b = check_LLVM_basicblock(L, 2);
  builder->builder->SetInsertPoint(b->b);
  return 0;
}

static int context_intconstant(lua_State *L) {
  ContextHolder *context = check_LLVM_context(L, 1);
  auto jit = context->jitState;
  int i = (int)luaL_checkinteger(L, 2);
  alloc_LLVM_value(L, llvm::ConstantInt::get(jit->types()->C_intT, i));
  return 1;
}

static int irbuilder_retval(lua_State *L) {
  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  llvm::Value *v = get_value(L, 2);
  llvm::Instruction *i;
  if (v)
    i = builder->builder->CreateRet(v);
  else
    i = builder->builder->CreateRetVoid();
  alloc_LLVM_instruction(L, i);
  return 1;
}

static int irbuilder_branch(lua_State *L) {
  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  BasicBlockHolder *b = check_LLVM_basicblock(L, 2);
  llvm::Instruction *i = builder->builder->CreateBr(b->b);
  alloc_LLVM_instruction(L, i);
  return 1;
}

static int irbuilder_condbranch(lua_State *L) {
  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  llvm::Value *cond = get_value(L, 2);
  BasicBlockHolder *true_br = check_LLVM_basicblock(L, 3);
  BasicBlockHolder *false_br = check_LLVM_basicblock(L, 4);
  llvm::Instruction *i =
      builder->builder->CreateCondBr(cond, true_br->b, false_br->b);
  alloc_LLVM_instruction(L, i);
  return 1;
}

static int func_compile(lua_State *L) {
  MainFunctionHolder *f = check_LLVM_mainfunction(L, 1);
  if (!f->is_valid) return 0;
  if (!f->func->function()) return 0;
  if (!f->compiled_func) {
    f->func->raviModule()->runpasses();
    f->func->raviModule()->finalize();
  }
  if (!f->compiled_func) return 0;
  lua_pushvalue(L, 1);
  lua_pushcclosure(L, f->compiled_func, 1);
  return 1;
}

typedef int (*decl_func)(lua_State *L, ravi::RaviJITState *jit,
                         ravi::RaviJITFunction *f);
typedef struct {
  const char *name;
  decl_func f;
} FunctionDeclaration;

#define str(s) #s

#define decl_func1_(name, ret, arg1, vararg)                      \
  static int name##_decl(lua_State *L, ravi::RaviJITState *jit,   \
                         ravi::RaviJITFunction *f) {              \
    llvm::Type *args[] = {                                        \
        jit->types()->arg1,                                       \
    };                                                            \
    llvm::FunctionType *type =                                    \
        llvm::FunctionType::get(jit->types()->ret, args, vararg); \
    llvm::Function *extfunc = f->addExternFunction(               \
        type, reinterpret_cast<void *>(&name), str(name));        \
    alloc_LLVM_constant(L, extfunc);                              \
    return 1;                                                     \
  }

#define decl_func1v(name, ret, arg1) decl_func1_(name, ret, arg1, true)
#define decl_func1(name, ret, arg1) decl_func1_(name, ret, arg1, false)

#define decl_func2_(name, ret, arg1, arg2, vararg)                \
  static int name##_decl(lua_State *L, ravi::RaviJITState *jit,   \
                         ravi::RaviJITFunction *f) {              \
    llvm::Type *args[] = {                                        \
        jit->types()->arg1, jit->types()->arg2,                   \
    };                                                            \
    llvm::FunctionType *type =                                    \
        llvm::FunctionType::get(jit->types()->ret, args, vararg); \
    llvm::Function *extfunc = f->addExternFunction(               \
        type, reinterpret_cast<void *>(&name), str(name));        \
    alloc_LLVM_constant(L, extfunc);                              \
    return 1;                                                     \
  }

#define decl_func2v(name, ret, arg1, arg2) \
  decl_func2_(name, ret, arg1, arg2, true)
#define decl_func2(name, ret, arg1, arg2) \
  decl_func2_(name, ret, arg1, arg2, false)

#define decl_func3(name, ret, arg1, arg2, arg3)                     \
  static int name##_decl(lua_State *L, ravi::RaviJITState *jit,     \
                         ravi::RaviJITFunction *f) {                \
    llvm::Type *args[] = {                                          \
        jit->types()->arg1, jit->types()->arg2, jit->types()->arg3, \
    };                                                              \
    llvm::FunctionType *type =                                      \
        llvm::FunctionType::get(jit->types()->ret, args, false);    \
    llvm::Function *extfunc = f->addExternFunction(                 \
        type, reinterpret_cast<void *>(&name), str(name));          \
    alloc_LLVM_constant(L, extfunc);                                \
    return 1;                                                       \
  }

#define decl_func4(name, ret, arg1, arg2, arg3, arg4)               \
  static int name##_decl(lua_State *L, ravi::RaviJITState *jit,     \
                         ravi::RaviJITFunction *f) {                \
    llvm::Type *args[] = {                                          \
        jit->types()->arg1, jit->types()->arg2, jit->types()->arg3, \
        jit->types()->arg4,                                         \
    };                                                              \
    llvm::FunctionType *type =                                      \
        llvm::FunctionType::get(jit->types()->ret, args, false);    \
    llvm::Function *extfunc = f->addExternFunction(                 \
        type, reinterpret_cast<void *>(&name), str(name));          \
    alloc_LLVM_constant(L, extfunc);                                \
    return 1;                                                       \
  }

#define decl_voidfunc1(name, arg1) decl_func1(name, C_voidT, arg1)
#define decl_voidfunc2(name, arg1, arg2) decl_func2(name, C_voidT, arg1, arg2)
#define decl_voidfunc3(name, arg1, arg2, arg3) \
  decl_func3(name, C_voidT, arg1, arg2, arg3)

decl_func1v(printf, C_intT, C_pcharT);
decl_func2(lua_absindex, C_intT, plua_StateT, C_intT);
decl_func1(lua_gettop, C_intT, plua_StateT);
decl_voidfunc2(lua_settop, plua_StateT, C_intT);
decl_voidfunc2(lua_pushvalue, plua_StateT, C_intT);
decl_voidfunc3(lua_rotate, plua_StateT, C_intT, C_intT);
decl_voidfunc3(lua_copy, plua_StateT, C_intT, C_intT);
decl_func2(lua_checkstack, C_intT, plua_StateT, C_intT);
decl_voidfunc3(lua_xmove, plua_StateT, plua_StateT, C_intT);
decl_func2(lua_isnumber, C_intT, plua_StateT, C_intT);
decl_func2(lua_isstring, C_intT, plua_StateT, C_intT);
decl_func2(lua_iscfunction, C_intT, plua_StateT, C_intT);
decl_func2(lua_isinteger, C_intT, plua_StateT, C_intT);
decl_func2(lua_isuserdata, C_intT, plua_StateT, C_intT);
decl_func2(lua_type, C_intT, plua_StateT, C_intT);
decl_func2(lua_typename, C_pcharT, plua_StateT, C_intT);
decl_func3(lua_tonumberx, plua_NumberT, plua_StateT, C_intT, C_pintT);
decl_func3(lua_tointegerx, plua_IntegerT, plua_StateT, C_intT, C_pintT);
decl_func2(lua_toboolean, C_intT, plua_StateT, C_intT);
decl_func3(lua_tolstring, C_pcharT, plua_StateT, C_intT, C_psize_t);
decl_func2(lua_rawlen, C_size_t, plua_StateT, C_intT);
decl_func2(lua_tocfunction, plua_CFunctionT, plua_StateT, C_intT);
decl_func2(lua_touserdata, C_pcharT, plua_StateT, C_intT);
decl_func2(lua_tothread, plua_StateT, plua_StateT, C_intT);
decl_func2(lua_topointer, C_pcharT, plua_StateT, C_intT);
decl_voidfunc2(lua_arith, plua_StateT, C_intT);
decl_func3(lua_rawequal, C_intT, plua_StateT, C_intT, C_intT);
decl_func4(lua_compare, C_intT, plua_StateT, C_intT, C_intT, C_intT);
decl_voidfunc1(lua_pushnil, plua_StateT);
decl_voidfunc2(lua_pushnumber, plua_StateT, lua_NumberT);
decl_voidfunc2(lua_pushinteger, plua_StateT, lua_IntegerT);
decl_func3(lua_pushlstring, C_pcharT, plua_StateT, C_pcharT, C_size_t);
decl_func2(lua_pushstring, C_pcharT, plua_StateT, C_pcharT);
decl_func3(lua_pushvfstring, C_pcharT, plua_StateT, C_pcharT, C_pcharT);
decl_func2v(lua_pushfstring, C_pcharT, plua_StateT, C_pcharT);
decl_voidfunc3(lua_pushcclosure, plua_StateT, plua_CFunctionT, C_intT);
decl_voidfunc2(lua_pushboolean, plua_StateT, C_intT);
decl_voidfunc2(lua_pushlightuserdata, plua_StateT, C_pcharT);
decl_func1(lua_pushthread, C_intT, plua_StateT);
decl_func2(lua_getglobal, C_intT, plua_StateT, C_pcharT);
decl_func2(lua_gettable, C_intT, plua_StateT, C_intT);
decl_func3(lua_getfield, C_intT, plua_StateT, C_intT, C_pcharT);
decl_func3(lua_geti, C_intT, plua_StateT, C_intT, lua_IntegerT);
decl_func2(lua_rawget, C_intT, plua_StateT, C_intT);
decl_func3(lua_rawgeti, C_intT, plua_StateT, C_intT, lua_IntegerT);
decl_func3(lua_rawgetp, C_intT, plua_StateT, C_intT, C_pcharT);
decl_func3(lua_createtable, C_intT, plua_StateT, C_intT, C_intT);
decl_func2(lua_newuserdata, C_pcharT, plua_StateT, C_size_t);
decl_func2(lua_getmetatable, C_intT, plua_StateT, C_intT);
decl_func2(lua_getuservalue, C_intT, plua_StateT, C_intT);
decl_voidfunc2(lua_setglobal, plua_StateT, C_pcharT);
decl_voidfunc2(lua_settable, plua_StateT, C_intT);
decl_voidfunc3(lua_setfield, plua_StateT, C_intT, C_pcharT);
decl_voidfunc3(lua_seti, plua_StateT, C_intT, lua_IntegerT);
decl_voidfunc2(lua_rawset, plua_StateT, C_intT);
decl_voidfunc3(lua_rawseti, plua_StateT, C_intT, lua_IntegerT);
decl_voidfunc3(lua_rawsetp, plua_StateT, C_intT, C_pcharT);
decl_func2(lua_setmetatable, C_intT, plua_StateT, C_intT);
decl_voidfunc2(lua_setuservalue, plua_StateT, C_intT);
decl_func3(lua_gc, C_intT, plua_StateT, C_intT, C_intT);
decl_func1(lua_error, C_intT, plua_StateT);
decl_func2(lua_next, C_intT, plua_StateT, C_intT);
decl_voidfunc2(lua_concat, plua_StateT, C_intT);
decl_voidfunc2(lua_len, plua_StateT, C_intT);
decl_func2(lua_stringtonumber, C_size_t, plua_StateT, C_pcharT);

decl_func3(luaL_checklstring, C_pcharT, plua_StateT, C_intT, C_psize_t);
decl_func2(luaL_checkinteger, lua_IntegerT, plua_StateT, C_intT);

/* Sorted array of declared functions */
static FunctionDeclaration builtin_functions[] = {
    {"lua_absindex", lua_absindex_decl},
    {"lua_gettop", lua_gettop_decl},
    {"lua_settop", lua_settop_decl},
    {"lua_pushvalue", lua_pushvalue_decl},
    {"lua_rotate", lua_rotate_decl},
    {"lua_copy", lua_copy_decl},
    {"lua_checkstack", lua_checkstack_decl},
    {"lua_xmove", lua_xmove_decl},
    {"lua_isnumber", lua_isnumber_decl},
    {"lua_isstring", lua_isstring_decl},
    {"lua_iscfunction", lua_iscfunction_decl},
    {"lua_isinteger", lua_isinteger_decl},
    {"lua_isuserdata", lua_isuserdata_decl},
    {"lua_type", lua_type_decl},
    {"lua_typename", lua_typename_decl},
    {"lua_tonumberx", lua_tonumberx_decl},
    {"lua_tointegerx", lua_tointegerx_decl},
    {"lua_toboolean", lua_toboolean_decl},
    {"lua_tolstring", lua_tolstring_decl},
    {"lua_rawlen", lua_rawlen_decl},
    {"lua_tocfunction", lua_tocfunction_decl},
    {"lua_touserdata", lua_touserdata_decl},
    {"lua_tothread", lua_tothread_decl},
    {"lua_topointer", lua_topointer_decl},
    {"lua_arith", lua_arith_decl},
    {"lua_rawequal", lua_rawequal_decl},
    {"lua_compare", lua_compare_decl},
    {"lua_pushnil", lua_pushnil_decl},
    {"lua_pushnumber", lua_pushnumber_decl},
    {"lua_pushinteger", lua_pushinteger_decl},
    {"lua_pushlstring", lua_pushlstring_decl},
    {"lua_pushstring", lua_pushstring_decl},
    {"lua_pushvfstring", lua_pushvfstring_decl},
    {"lua_pushfstring", lua_pushfstring_decl},
    {"lua_pushcclosure", lua_pushcclosure_decl},
    {"lua_pushboolean", lua_pushboolean_decl},
    {"lua_pushlightuserdata", lua_pushlightuserdata_decl},
    {"lua_pushthread", lua_pushthread_decl},
    {"lua_getglobal", lua_getglobal_decl},
    {"lua_gettable", lua_gettable_decl},
    {"lua_getfield", lua_getfield_decl},
    {"lua_geti", lua_geti_decl},
    {"lua_rawget", lua_rawget_decl},
    {"lua_rawgeti", lua_rawgeti_decl},
    {"lua_rawgetp", lua_rawgetp_decl},
    {"lua_createtable", lua_createtable_decl},
    {"lua_newuserdata", lua_newuserdata_decl},
    {"lua_getmetatable", lua_getmetatable_decl},
    {"lua_getuservalue", lua_getuservalue_decl},
    {"lua_setglobal", lua_setglobal_decl},
    {"lua_settable", lua_settable_decl},
    {"lua_setfield", lua_setfield_decl},
    {"lua_seti", lua_seti_decl},
    {"lua_rawset", lua_rawset_decl},
    {"lua_rawseti", lua_rawseti_decl},
    {"lua_rawsetp", lua_rawsetp_decl},
    {"lua_setmetatable", lua_setmetatable_decl},
    {"lua_setuservalue", lua_setuservalue_decl},
    {"lua_gc", lua_gc_decl},
    {"lua_error", lua_error_decl},
    {"lua_next", lua_next_decl},
    {"lua_concat", lua_concat_decl},
    {"lua_len", lua_len_decl},
    {"lua_stringtonumber", lua_stringtonumber_decl},

    {"luaL_checklstring", luaL_checklstring_decl},
    {"luaL_checkinteger", luaL_checkinteger_decl},
    {"printf", printf_decl},
    {nullptr, nullptr}};

/*
 extern(function, name[, type])
 If type is not supplied then name must be one of the predefined
 functions
*/
static int func_addextern(lua_State *L) {
  auto jit = RaviJIT(L);
  MainFunctionHolder *f = check_LLVM_mainfunction(L, 1);
  const char *name = luaL_checkstring(L, 2);
  luaL_argcheck(L, f->compiled_func == nullptr, 1, "function already compiled");
  FunctionTypeHolder *tt = test_LLVM_functiontype(L, 3);
  if (tt) {
    llvm::Constant *c = f->func->module()->getOrInsertFunction(name, tt->type);
    alloc_LLVM_constant(L, c);
    return 1;
  }
  else {
    /* TODO binary search */
    for (int i = 0; builtin_functions[i].name != nullptr; i++) {
      if (strcmp(name, builtin_functions[i].name) == 0) {
        return builtin_functions[i].f(L, jit, f->func);
      }
    }
  }
  return 0;
}

static int irbuilder_stringconstant(lua_State *L) {
  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  const char *string = luaL_checkstring(L, 2);
  alloc_LLVM_value(L, builder->builder->CreateGlobalStringPtr(string));
  return 1;
}

static int context_nullconstant(lua_State *L) {
  PointerTypeHolder *th = check_LLVM_pointertype(L, 2);
  alloc_LLVM_value(L, llvm::ConstantPointerNull::get(th->type));
  return 1;
}

/*
  Get argument of the function
*/
static int func_getarg(lua_State *L) {
  llvm::Function *func = get_function(L, 1);
  int argn = luaL_checkinteger(L, 2);
  int results = 1;
  if (argn <= func->arg_size()) {
    auto B = func->arg_begin();
    auto E = func->arg_end();
    for (int i = 1; B != E; B++, i++) {
      if (i == argn) {
        alloc_LLVM_value(L, &(*B));
        break;
      }
    }
  }
  else {
    results = 0;
  }
  return results;
}

static int irbuilder_call(lua_State *L) {
  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  llvm::Value *func = nullptr;
  ConstantHolder *c = test_LLVM_constant(L, 2);
  if (c)
    func = c->constant;
  else {
    func = get_function(L, 2);
  }
  luaL_argcheck(L, lua_istable(L, 3), 3, "table expected");
  int len = luaL_len(L, 3);
  std::vector<llvm::Value *> elements;
  for (int i = 1; i <= len; i++) {
    lua_rawgeti(L, 3, i);
    llvm::Value *v = get_value(L, -1);
    elements.push_back(v);
    lua_pop(L, 1);
  }
  bool tailcall = false;
  if (lua_istable(L, 4)) l_table_get_bool(L, 4, "tailcall", &tailcall, false);
  llvm::CallInst *inst = builder->builder->CreateCall(func, elements);
  if (tailcall) inst->setTailCall();
  alloc_LLVM_instruction(L, inst);
  return 1;
}

static llvm::Type *get_type(lua_State *L, int idx) {
  TypeHolder *th = test_LLVM_type(L, idx);
  if (th) return th->type;
  PointerTypeHolder *ph = test_LLVM_pointertype(L, idx);
  if (ph) return ph->type;
  StructTypeHolder *sh = test_LLVM_structtype(L, idx);
  if (sh) return sh->type;
  FunctionTypeHolder *fh = test_LLVM_functiontype(L, idx);
  if (fh) return fh->type;
  luaL_argerror(L, idx, "unrecognized type");
  return nullptr;
}

static int func_alloca(lua_State *L) {
  llvm::Function *func = get_function(L, 1);
  llvm::Type *ty = get_type(L, 2);
  llvm::Value *arraysize = nullptr;
  const char *name = nullptr;
  if (lua_gettop(L) >= 3) name = luaL_checkstring(L, 3);
  if (lua_gettop(L) >= 4) arraysize = get_value(L, 4);
  llvm::BasicBlock *block = &(*func->getBasicBlockList().begin());
  luaL_argcheck(L, block != nullptr, 1, "No basic block in function");
  llvm::IRBuilder<> builder(block, block->begin());
  llvm::Instruction *inst = builder.CreateAlloca(ty, arraysize, name);
  alloc_LLVM_instruction(L, inst);
  return 1;
}

static int irbuilder_load(lua_State *L) {
  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  llvm::Value *ptr = get_value(L, 2);
  llvm::Instruction *inst = builder->builder->CreateLoad(ptr);
  alloc_LLVM_instruction(L, inst);
  return 1;
}

static int irbuilder_store(lua_State *L) {
  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  llvm::Value *value = get_value(L, 2);
  llvm::Value *ptr = get_value(L, 3);
  llvm::Instruction *inst = builder->builder->CreateStore(value, ptr);
  alloc_LLVM_instruction(L, inst);
  return 1;
}

static int irbuilder_phi(lua_State *L) {
  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  llvm::Type *ty = get_type(L, 2);
  unsigned int num_values = (unsigned int)luaL_checkinteger(L, 3);
  const char *name = nullptr;
  if (lua_gettop(L) >= 4) { name = luaL_checkstring(L, 4); }
  llvm::PHINode *phi = builder->builder->CreatePHI(ty, num_values, name);
  alloc_LLVM_phinode(L, phi);
  return 1;
}

static int phi_addincoming(lua_State *L) {
  PhiNodeHolder *phi = check_LLVM_phinode(L, 1);
  llvm::Value *value = get_value(L, 2);
  BasicBlockHolder *block = check_LLVM_basicblock(L, 3);
  phi->phi->addIncoming(value, block->b);
  lua_pushinteger(L, phi->phi->getNumOperands());
  return 1;
}

/* ops that take a value and value array as arguments */
#define irbuilder_vva(op)                                            \
  static int irbuilder_##op(lua_State *L) {                          \
    IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);           \
    llvm::Value *c = get_value(L, 2);                                \
    luaL_argcheck(L, lua_istable(L, 3), 3, "table expected");        \
    int len = luaL_len(L, 3);                                        \
    std::vector<llvm::Value *> elements;                             \
    for (int i = 1; i <= len; i++) {                                 \
      lua_rawgeti(L, 3, i);                                          \
      llvm::Value *v = get_value(L, -1);                             \
      elements.push_back(v);                                         \
      lua_pop(L, 1);                                                 \
    }                                                                \
    llvm::Value *result = builder->builder->Create##op(c, elements); \
    alloc_LLVM_value(L, result);                                     \
    return 1;                                                        \
  }

#define irbuilder_binop(op)                                       \
  static int irbuilder_##op(lua_State *L) {                       \
    IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);        \
    llvm::Value *lhs = get_value(L, 2);                           \
    llvm::Value *rhs = get_value(L, 3);                           \
    llvm::Value *result = builder->builder->Create##op(lhs, rhs); \
    alloc_LLVM_value(L, result);                                  \
    return 1;                                                     \
  }

#define irbuilder_unop(op)                                   \
  static int irbuilder_##op(lua_State *L) {                  \
    IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);   \
    llvm::Value *lhs = get_value(L, 2);                      \
    llvm::Value *result = builder->builder->Create##op(lhs); \
    alloc_LLVM_value(L, result);                             \
    return 1;                                                \
  }

#define irbuilder_convert(op)                                     \
  static int irbuilder_##op(lua_State *L) {                       \
    IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);        \
    llvm::Value *lhs = get_value(L, 2);                           \
    llvm::Type *rhs = get_type(L, 3);                             \
    llvm::Value *result = builder->builder->Create##op(lhs, rhs); \
    alloc_LLVM_value(L, result);                                  \
    return 1;                                                     \
  }

irbuilder_vva(InBoundsGEP);
irbuilder_vva(GEP);

irbuilder_binop(ICmpEQ);
irbuilder_binop(ICmpNE);
irbuilder_binop(ICmpUGT);
irbuilder_binop(ICmpUGE);
irbuilder_binop(ICmpULT);
irbuilder_binop(ICmpULE);
irbuilder_binop(ICmpSGT);
irbuilder_binop(ICmpSGE);
irbuilder_binop(ICmpSLT);
irbuilder_binop(ICmpSLE);
irbuilder_binop(FCmpOEQ);
irbuilder_binop(FCmpOGT);
irbuilder_binop(FCmpOGE);
irbuilder_binop(FCmpOLT);
irbuilder_binop(FCmpOLE);
irbuilder_binop(FCmpONE);
irbuilder_binop(FCmpORD);
irbuilder_binop(FCmpUNO);
irbuilder_binop(FCmpUEQ);
irbuilder_binop(FCmpUGT);
irbuilder_binop(FCmpUGE);
irbuilder_binop(FCmpULT);
irbuilder_binop(FCmpULE);
irbuilder_binop(FCmpUNE);

irbuilder_binop(NSWAdd);
irbuilder_binop(NUWAdd);
irbuilder_binop(NSWSub);
irbuilder_binop(NUWSub);
irbuilder_binop(UDiv);
irbuilder_binop(ExactUDiv);
irbuilder_binop(SDiv);
irbuilder_binop(ExactSDiv);
irbuilder_binop(URem);
irbuilder_binop(SRem);

irbuilder_binop(And);
irbuilder_binop(Or);
irbuilder_binop(Xor);

irbuilder_binop(FAdd);
irbuilder_binop(FSub);
irbuilder_binop(FMul);
irbuilder_binop(FDiv);
irbuilder_binop(FRem);

irbuilder_unop(Not);
irbuilder_unop(Neg);
irbuilder_unop(FNeg);

irbuilder_convert(Trunc);
irbuilder_convert(ZExt);
irbuilder_convert(SExt);
irbuilder_convert(ZExtOrTrunc);
irbuilder_convert(SExtOrTrunc);
irbuilder_convert(FPToUI);
irbuilder_convert(FPToSI);
irbuilder_convert(UIToFP);
irbuilder_convert(SIToFP);
irbuilder_convert(FPTrunc);
irbuilder_convert(FPExt);
irbuilder_convert(PtrToInt);
irbuilder_convert(IntToPtr);
irbuilder_convert(BitCast);
irbuilder_convert(SExtOrBitCast);
irbuilder_convert(ZExtOrBitCast);
irbuilder_convert(TruncOrBitCast);
irbuilder_convert(PointerCast);
irbuilder_convert(FPCast);

static const luaL_Reg llvmlib[] = {
    {"context", get_llvm_context}, {"dump", dump_content}, {NULL, NULL}};

static const luaL_Reg structtype_methods[] = {{"setbody", struct_add_members},
                                              {NULL, NULL}};

static const luaL_Reg module_methods[] = {
    {"newfunction", module_newfunction},    {"getfunction", module_getfunction},
    {"compileC", module_compile_C},         {"dump", dump_content},
    {"generatecode", module_generate_code}, {NULL, NULL}};

static const luaL_Reg main_function_methods[] = {
    {"appendblock", func_append_basicblock},
    {"compile", func_compile},
    {"extern", func_addextern},
    {"arg", func_getarg},
    {"module", func_getmodule},
    {"alloca", func_alloca},
    {NULL, NULL}};

static const luaL_Reg function_methods[] = {
    {"appendblock", func_append_basicblock},
    {"arg", func_getarg},
    {"alloca", func_alloca},
    {NULL, NULL}};

static const luaL_Reg context_methods[] = {
    {"irbuilder", context_new_LLVM_irbuilder},
    {"types", context_get_types},
    {"structtype", context_new_struct_type},
    {"pointertype", context_new_pointer_type},
    {"functiontype", context_new_function_type},
    {"lua_CFunction", context_new_lua_CFunction},
    {"basicblock", context_new_basicblock},
    {"intconstant", context_intconstant},
    {"nullconstant", context_nullconstant},
    {"newmodule", context_new_module},
    {NULL, NULL}};

static const luaL_Reg phi_methods[] = {{"addincoming", phi_addincoming},
                                       {NULL, NULL}};

static const luaL_Reg irbuilder_methods[] = {
    {"setinsertpoint", irbuilder_set_current_block},
    {"ret", irbuilder_retval},
    {"stringconstant", irbuilder_stringconstant},
    {"call", irbuilder_call},
    {"br", irbuilder_branch},
    {"condbr", irbuilder_condbranch},
    {"icmpeq", irbuilder_ICmpEQ},
    {"icmpne", irbuilder_ICmpNE},
    {"icmpugt", irbuilder_ICmpUGT},
    {"icmpuge", irbuilder_ICmpUGE},
    {"icmpult", irbuilder_ICmpULT},
    {"icmpule", irbuilder_ICmpULE},
    {"icmpsgt", irbuilder_ICmpSGT},
    {"icmpsge", irbuilder_ICmpSGE},
    {"icmpslt", irbuilder_ICmpSLT},
    {"icmpsle", irbuilder_ICmpSLE},
    {"fcmpoeq", irbuilder_FCmpOEQ},
    {"fcmpogt", irbuilder_FCmpOGT},
    {"fcmpoge", irbuilder_FCmpOGE},
    {"fcmpolt", irbuilder_FCmpOLT},
    {"fcmpole", irbuilder_FCmpOLE},
    {"fcmpone", irbuilder_FCmpONE},
    {"fcmpord", irbuilder_FCmpORD},
    {"fcmpuno", irbuilder_FCmpUNO},
    {"fcmpueq", irbuilder_FCmpUEQ},
    {"fcmpugt", irbuilder_FCmpUGT},
    {"fcmpuge", irbuilder_FCmpUGE},
    {"fcmpult", irbuilder_FCmpULT},
    {"fcmpule", irbuilder_FCmpULE},
    {"fcmpune", irbuilder_FCmpUNE},
    {"nswadd", irbuilder_NSWAdd},
    {"nuwadd", irbuilder_NUWAdd},
    {"nswsub", irbuilder_NSWSub},
    {"nuwsub", irbuilder_NUWSub},
    {"udiv", irbuilder_UDiv},
    {"exactudiv", irbuilder_ExactUDiv},
    {"sdiv", irbuilder_SDiv},
    {"exactsdiv", irbuilder_ExactSDiv},
    {"neg", irbuilder_Neg},
    {"urem", irbuilder_URem},
    {"srem", irbuilder_SRem},
    {"and", irbuilder_And},
    {"or", irbuilder_Or},
    {"xor", irbuilder_Xor},
    {"not", irbuilder_Not},
    {"fadd", irbuilder_FAdd},
    {"fsub", irbuilder_FSub},
    {"fmul", irbuilder_FMul},
    {"fdiv", irbuilder_FDiv},
    {"frem", irbuilder_FRem},
    {"fneg", irbuilder_FNeg},
    {"gep", irbuilder_GEP},
    {"inboundsgep", irbuilder_InBoundsGEP},
    {"trunc", irbuilder_Trunc},
    {"zext", irbuilder_ZExt},
    {"sext", irbuilder_SExt},
    {"zextortrunc", irbuilder_ZExtOrTrunc},
    {"sextortrunc", irbuilder_SExtOrTrunc},
    {"fptoui", irbuilder_FPToUI},
    {"fptosi", irbuilder_FPToSI},
    {"uitofp", irbuilder_UIToFP},
    {"sitofp", irbuilder_SIToFP},
    {"fptrunc", irbuilder_FPTrunc},
    {"fpext", irbuilder_FPExt},
    {"ptrtoint", irbuilder_PtrToInt},
    {"inttoptr", irbuilder_IntToPtr},
    {"bitcast", irbuilder_BitCast},
    {"sextorbitcast", irbuilder_SExtOrBitCast},
    {"zextorbitcast", irbuilder_ZExtOrBitCast},
    {"truncorbitcast", irbuilder_TruncOrBitCast},
    {"pointercast", irbuilder_PointerCast},
    {"fpcast", irbuilder_FPCast},
    {"load", irbuilder_load},
    {"store", irbuilder_store},
    {"phi", irbuilder_phi},
    {NULL, NULL}};

LUAMOD_API int raviopen_llvmluaapi(lua_State *L) {
  luaL_newmetatable(L, LLVM_context);
  lua_pushstring(L, LLVM_context);
  lua_setfield(L, -2, "type");
  lua_pushvalue(L, -1);           /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_setfuncs(L, context_methods, 0);
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_module);
  lua_pushstring(L, LLVM_module);
  lua_setfield(L, -2, "type");
  lua_pushcfunction(L, collect_LLVM_module);
  lua_setfield(L, -2, "__gc");
  lua_pushvalue(L, -1);           /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_setfuncs(L, module_methods, 0);
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_irbuilder);
  lua_pushstring(L, LLVM_irbuilder);
  lua_setfield(L, -2, "type");
  lua_pushcfunction(L, collect_LLVM_irbuilder);
  lua_setfield(L, -2, "__gc");
  lua_pushvalue(L, -1);           /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_setfuncs(L, irbuilder_methods, 0);
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_mainfunction);
  lua_pushstring(L, LLVM_mainfunction);
  lua_setfield(L, -2, "type");
  lua_pushcfunction(L, collect_LLVM_mainfunction);
  lua_setfield(L, -2, "__gc");
  lua_pushvalue(L, -1);           /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_setfuncs(L, main_function_methods, 0);
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_function);
  lua_pushstring(L, LLVM_function);
  lua_setfield(L, -2, "type");
  lua_pushvalue(L, -1);           /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_setfuncs(L, function_methods, 0);
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_type);
  lua_pushstring(L, LLVM_type);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_value);
  lua_pushstring(L, LLVM_value);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_structtype);
  lua_pushstring(L, LLVM_structtype);
  lua_setfield(L, -2, "type");
  lua_pushvalue(L, -1);           /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_setfuncs(L, structtype_methods, 0);
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_pointertype);
  lua_pushstring(L, LLVM_pointertype);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_functiontype);
  lua_pushstring(L, LLVM_functiontype);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_basicblock);
  lua_pushstring(L, LLVM_basicblock);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_constant);
  lua_pushstring(L, LLVM_constant);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_instruction);
  lua_pushstring(L, LLVM_instruction);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  luaL_newmetatable(L, LLVM_phinode);
  lua_pushstring(L, LLVM_phinode);
  lua_setfield(L, -2, "type");
  lua_pushvalue(L, -1);           /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_setfuncs(L, phi_methods, 0);
  lua_pop(L, 1);

  luaL_newlib(L, llvmlib);
  return 1;
}

#ifdef __cplusplus
}
#endif

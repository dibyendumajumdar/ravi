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

#include <ravijit.h>
#include "ravi_llvmcodegen.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lauxlib.h"

#if LUA_VERSION_NUM < 502

// Forward compatibility with Lua 5.2
// Following is not defined in 5.1

#define lua_absindex(L, i)                                                     \
  ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)

#endif

#if (LUA_VERSION_NUM >= 502)

// backward compatibility with Lua 5.1
#undef lua_equal
#define lua_equal(L, idx1, idx2) lua_compare(L, (idx1), (idx2), LUA_OPEQ)

#undef lua_objlen
#define lua_objlen lua_rawlen

#endif

// The normal Lua metatable functions in C use string
// keys - these are expensive as the key needs to be
// converted to Lua string, hash code computed etc.
// Following implementations are taken from a post in
// Lua mailing list (http://lua-users.org/lists/lua-l/2010-11/msg00151.html)
// They use lightuserdata instead of strings to speed
// things up
// meta_key is the key assigned to the meta
// table of the userdata
static int l_newmetatable(lua_State *L, const char *meta_key) {
  lua_pushlightuserdata(L, (void *)meta_key);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (!lua_isnil(L, -1)) // name already in use?
    return 0;            // leave previous value on top, but return 0
  lua_pop(L, 1);         // pop the nil value
  lua_newtable(L);       // create metatable
  lua_pushlightuserdata(L, (void *)meta_key); // meta_key
  lua_pushvalue(L, -2);                       // table
  lua_rawset(L, LUA_REGISTRYINDEX); // assign table to meta_key in the registry
  return 1;
}

// meta_key is the key assigned to the meta table of the userdata
static void l_getmetatable(lua_State *L, const char *meta_key) {
  lua_pushlightuserdata(L, (void *)meta_key); // meta_key
  lua_rawget(L, LUA_REGISTRYINDEX); // obtain the value associated with
                                    // meta_key from registry
}

// arg_index is the position of userdata argument on the stack
// meta_key is the key assigned to the meta table of the userdata
static void *l_testudata(lua_State *L, int arg_index, const char *meta_key) {
  void *p = lua_touserdata(L, arg_index);
  int n = lua_gettop(L);
  if (p != NULL) {                                // value is a userdata?
    if (lua_getmetatable(L, arg_index)) {         // does it have a metatable?
      lua_pushlightuserdata(L, (void *)meta_key); // meta_key
      lua_rawget(
          L,
          LUA_REGISTRYINDEX); // get correct metatable associated with meta_key
      if (!lua_rawequal(L, -1, -2)) // compare: does it have the correct mt?
        p = NULL;
      lua_pop(L, 2); // remove both metatables
    }
  }
  lua_assert(n == lua_gettop(L));
  return p; /* to avoid warnings */
}

// arg_index is the position of userdata argument on the stack
// meta_key is the key assigned to the meta table of the userdata
static void *l_checkudata(lua_State *L, int arg_index, const char *meta_key) {
  void *p = l_testudata(L, arg_index, meta_key);
  if (p == NULL)
    luaL_argerror(L, arg_index, meta_key);
  return p;
}

// Utility to extract a boolean field from a table
bool l_table_get_bool(lua_State *L, int idx, const char *key, bool *result,
                      bool default_value) {
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
static const char *LLVM_function = "LLVMfunction";
static const char *LLVM_type = "LLVMtype";
static const char *LLVM_value = "LLVMvalue";
static const char *LLVM_structtype = "LLVMstructtype";
static const char *LLVM_pointertype = "LLVMpointertype";
static const char *LLVM_functiontype = "LLVMfunctiontype";
static const char *LLVM_basicblock = "LLVMbasicblock";
static const char *LLVM_externfunc = "LLVMexternfunc";
static const char *LLVM_constant = "LLVMconstant";
static const char *LLVM_instruction = "LLVMinstruction";

#define test_LLVM_irbuilder(L, idx)                                            \
  ((IRBuilderHolder *)l_testudata(L, idx, LLVM_irbuilder))
#define check_LLVM_irbuilder(L, idx)                                           \
  ((IRBuilderHolder *)l_checkudata(L, idx, LLVM_irbuilder))

#define test_LLVM_type(L, idx) ((TypeHolder *)l_testudata(L, idx, LLVM_type))
#define check_LLVM_type(L, idx) ((TypeHolder *)l_checkudata(L, idx, LLVM_type))

#define test_LLVM_context(L, idx)                                              \
  ((ContextHolder *)l_testudata(L, idx, LLVM_context))
#define check_LLVM_context(L, idx)                                             \
  ((ContextHolder *)l_checkudata(L, idx, LLVM_context))

#define test_LLVM_structtype(L, idx)                                           \
  ((StructTypeHolder *)l_testudata(L, idx, LLVM_structtype))
#define check_LLVM_structtype(L, idx)                                          \
  ((StructTypeHolder *)l_checkudata(L, idx, LLVM_structtype))

#define test_LLVM_pointertype(L, idx)                                          \
  ((PointerTypeHolder *)l_testudata(L, idx, LLVM_pointertype))
#define check_LLVM_pointertype(L, idx)                                         \
  ((PointerTypeHolder *)l_checkudata(L, idx, LLVM_pointertype))

#define test_LLVM_functiontype(L, idx)                                         \
  ((FunctionTypeHolder *)l_testudata(L, idx, LLVM_functiontype))
#define check_LLVM_functiontype(L, idx)                                        \
  ((FunctionTypeHolder *)l_checkudata(L, idx, LLVM_functiontype))

#define test_LLVM_function(L, idx)                                             \
  ((FunctionHolder *)l_testudata(L, idx, LLVM_function))
#define check_LLVM_function(L, idx)                                            \
  ((FunctionHolder *)l_checkudata(L, idx, LLVM_function))

#define test_LLVM_basicblock(L, idx)                                           \
  ((BasicBlockHolder *)l_testudata(L, idx, LLVM_basicblock))
#define check_LLVM_basicblock(L, idx)                                          \
  ((BasicBlockHolder *)l_checkudata(L, idx, LLVM_basicblock))

#define test_LLVM_value(L, idx) ((ValueHolder *)l_testudata(L, idx, LLVM_value))
#define check_LLVM_value(L, idx)                                               \
  ((ValueHolder *)l_checkudata(L, idx, LLVM_value))

#define test_LLVM_externfunc(L, idx)                                           \
  ((ExternFuncHolder *)l_testudata(L, idx, LLVM_externfunc))
#define check_LLVM_externfunc(L, idx)                                          \
  ((ExternFuncHolder *)l_checkudata(L, idx, LLVM_externfunc))

#define test_LLVM_constant(L, idx)                                             \
  ((ConstantHolder *)l_testudata(L, idx, LLVM_constant))
#define check_LLVM_constant(L, idx)                                            \
  ((ConstantHolder *)l_checkudata(L, idx, LLVM_constant))

#define test_LLVM_instruction(L, idx)                                             \
  ((InstructionHolder *)l_testudata(L, idx, LLVM_instruction))
#define check_LLVM_instruction(L, idx)                                            \
  ((InstructionHolder *)l_checkudata(L, idx, LLVM_instruction))

struct ContextHolder {
  llvm::LLVMContext *context;
};

/* garbage collected */
struct IRBuilderHolder {
  llvm::IRBuilder<> *builder;
};

struct TypeHolder {
  llvm::Type *type;
};

struct ValueHolder {
  llvm::Value *value;
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
struct FunctionHolder {
  ravi::RaviJITFunctionImpl *func;
  lua_CFunction compiled_func;
  llvm::Value *arg1;
};

struct BasicBlockHolder {
  llvm::BasicBlock *b;
};

struct ExternFuncHolder {
  llvm::Function *function;
};

struct ConstantHolder {
  llvm::Constant *constant;
};

struct InstructionHolder {
  llvm::Instruction *i;
};

static int alloc_LLVM_irbuilder(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  IRBuilderHolder *builder =
      (IRBuilderHolder *)lua_newuserdata(L, sizeof(IRBuilderHolder));
  builder->builder = new llvm::IRBuilder<>(jit->context());
  l_getmetatable(L, LLVM_irbuilder);
  lua_setmetatable(L, -2);
  return 1;
}

static int collect_LLVM_irbuilder(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  if (builder->builder) {
    delete builder->builder;
    builder->builder = nullptr;
  }
  return 0;
}

static void alloc_LLVM_type(lua_State *L, ravi::RaviJITStateImpl *jit,
                            llvm::Type *t) {
  TypeHolder *tt = (TypeHolder *)lua_newuserdata(L, sizeof(TypeHolder));
  l_getmetatable(L, LLVM_type);
  lua_setmetatable(L, -2);
  tt->type = t;
}

static void alloc_LLVM_context(lua_State *L, ravi::RaviJITStateImpl *jit,
                               llvm::LLVMContext *c) {
  ContextHolder *h = (ContextHolder *)lua_newuserdata(L, sizeof(ContextHolder));
  l_getmetatable(L, LLVM_context);
  lua_setmetatable(L, -2);
  h->context = c;
}

static void alloc_LLVM_value(lua_State *L, ravi::RaviJITStateImpl *jit,
                             llvm::Value *v) {
  ValueHolder *h = (ValueHolder *)lua_newuserdata(L, sizeof(ValueHolder));
  l_getmetatable(L, LLVM_value);
  lua_setmetatable(L, -2);
  h->value = v;
}

static void alloc_LLVM_structtype(lua_State *L, ravi::RaviJITStateImpl *jit,
                                  llvm::StructType *type) {
  StructTypeHolder *h =
      (StructTypeHolder *)lua_newuserdata(L, sizeof(StructTypeHolder));
  l_getmetatable(L, LLVM_structtype);
  lua_setmetatable(L, -2);
  h->type = type;
}

static void alloc_LLVM_pointertype(lua_State *L, ravi::RaviJITStateImpl *jit,
                                   llvm::PointerType *type) {
  PointerTypeHolder *h =
      (PointerTypeHolder *)lua_newuserdata(L, sizeof(PointerTypeHolder));
  l_getmetatable(L, LLVM_pointertype);
  lua_setmetatable(L, -2);
  h->type = type;
}

static void alloc_LLVM_functiontype(lua_State *L, llvm::FunctionType *type) {
  FunctionTypeHolder *h =
      (FunctionTypeHolder *)lua_newuserdata(L, sizeof(FunctionTypeHolder));
  l_getmetatable(L, LLVM_functiontype);
  lua_setmetatable(L, -2);
  h->type = type;
}

static void alloc_LLVM_externfunc(lua_State *L, llvm::Function *f) {
  ExternFuncHolder *h =
      (ExternFuncHolder *)lua_newuserdata(L, sizeof(ExternFuncHolder));
  l_getmetatable(L, LLVM_externfunc);
  lua_setmetatable(L, -2);
  h->function = f;
}

static void alloc_LLVM_constant(lua_State *L, llvm::Constant *f) {
  ConstantHolder *h =
      (ConstantHolder *)lua_newuserdata(L, sizeof(ConstantHolder));
  l_getmetatable(L, LLVM_constant);
  lua_setmetatable(L, -2);
  h->constant = f;
}

static void alloc_LLVM_instruction(lua_State *L, llvm::Instruction *i) {
  InstructionHolder *h =
    (InstructionHolder *)lua_newuserdata(L, sizeof(InstructionHolder));
  l_getmetatable(L, LLVM_instruction);
  lua_setmetatable(L, -2);
  h->i = i;
}

static FunctionHolder *alloc_LLVM_function(lua_State *L,
                                           ravi::RaviJITStateImpl *jit,
                                           llvm::FunctionType *type,
                                           const char *name) {
  FunctionHolder *h =
      (FunctionHolder *)lua_newuserdata(L, sizeof(FunctionHolder));
  h->func = nullptr;
  h->compiled_func = nullptr;
  h->arg1 = nullptr;
  l_getmetatable(L, LLVM_function);
  lua_setmetatable(L, -2);
  h->func = (ravi::RaviJITFunctionImpl *)jit->createFunction(
      type, llvm::Function::ExternalLinkage, name);
  return h;
}

static void alloc_LLVM_basicblock(lua_State *L, llvm::BasicBlock *b) {
  BasicBlockHolder *h =
      (BasicBlockHolder *)lua_newuserdata(L, sizeof(BasicBlockHolder));
  l_getmetatable(L, LLVM_basicblock);
  lua_setmetatable(L, -2);
  h->b = b;
}

static int collect_LLVM_function(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  FunctionHolder *builder = check_LLVM_function(L, 1);
  if (builder->func) {
    delete builder->func;
    builder->func = nullptr;
    printf("collected function\n");
  }
  return 0;
}

static void alloc_LLVM_intconstant(lua_State *L, ravi::RaviJITStateImpl *jit,
                                   llvm::Value *v) {
  ValueHolder *h = (ValueHolder *)lua_newuserdata(L, sizeof(ValueHolder));
  l_getmetatable(L, LLVM_value);
  lua_setmetatable(L, -2);
  h->value = v;
}

/*
 Create a new struct type
 structtype(name)
*/
static int new_struct_type(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  const char *name = luaL_checkstring(L, 1);
  alloc_LLVM_structtype(L, jit, llvm::StructType::create(jit->context(), name));
  return 1;
}

/*
 Create a pointer type
 pointertype(type)
*/
static int new_pointer_type(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  StructTypeHolder *sth = test_LLVM_structtype(L, 1);
  if (sth) {
    alloc_LLVM_pointertype(L, jit, llvm::PointerType::get(sth->type, 0));
    goto done;
  }
  PointerTypeHolder *ph = test_LLVM_pointertype(L, 1);
  if (ph) {
    alloc_LLVM_pointertype(L, jit, llvm::PointerType::get(ph->type, 0));
    goto done;
  }
  TypeHolder *th = test_LLVM_type(L, 1);
  if (th) {
    alloc_LLVM_pointertype(L, jit, llvm::PointerType::get(th->type, 0));
    goto done;
  }
  FunctionTypeHolder *fth = test_LLVM_functiontype(L, 1);
  if (fth) {
    alloc_LLVM_pointertype(L, jit, llvm::PointerType::get(fth->type, 0));
    goto done;
  }
  luaL_argerror(L, 1, "invalid argument");
  return 0;
done:
  return 1;
}

/*
 Get pre-defined types
*/
static int get_standard_types(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  lua_newtable(L);
  alloc_LLVM_type(L, jit, jit->types()->C_intT);
  lua_setfield(L, -2, "int");
  alloc_LLVM_type(L, jit, jit->types()->lua_NumberT);
  lua_setfield(L, -2, "lua_Number");
  alloc_LLVM_type(L, jit, jit->types()->lua_IntegerT);
  lua_setfield(L, -2, "lua_Integer");
  alloc_LLVM_type(L, jit, jit->types()->lu_byteT);
  lua_setfield(L, -2, "lu_byte");
  alloc_LLVM_pointertype(L, jit, jit->types()->C_pcharT);
  lua_setfield(L, -2, "pchar");
  alloc_LLVM_type(L, jit, jit->types()->C_doubleT);
  lua_setfield(L, -2, "double");
  alloc_LLVM_type(L, jit, jit->types()->C_int64_t);
  lua_setfield(L, -2, "int64_t");
  alloc_LLVM_type(L, jit, jit->types()->C_intptr_t);
  lua_setfield(L, -2, "intptr_t");
  alloc_LLVM_pointertype(L, jit, jit->types()->C_pintT);
  lua_setfield(L, -2, "pint");
  alloc_LLVM_type(L, jit, jit->types()->C_ptrdiff_t);
  lua_setfield(L, -2, "ptrdiff_t");
  alloc_LLVM_type(L, jit, jit->types()->C_shortT);
  lua_setfield(L, -2, "short");
  alloc_LLVM_type(L, jit, jit->types()->C_size_t);
  lua_setfield(L, -2, "size_t");
  alloc_LLVM_pointertype(L, jit, jit->types()->C_psize_t);
  lua_setfield(L, -2, "psize_t");
  alloc_LLVM_type(L, jit, jit->types()->lua_UnsignedT);
  lua_setfield(L, -2, "lua_Unsigned");
  alloc_LLVM_type(L, jit, llvm::Type::getVoidTy(jit->context()));
  lua_setfield(L, -2, "void");
  alloc_LLVM_functiontype(L, jit->types()->lua_CFunctionT);
  lua_setfield(L, -2, "lua_CFunction");

  return 1;
}

static int get_llvm_context(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  alloc_LLVM_context(L, jit, &jit->context());
  return 1;
}

/*
 Dump an LLVM object
*/
static int dump_content(lua_State *L) {
  TypeHolder *th = test_LLVM_type(L, 1);
  if (th) {
    th->type->dump();
    goto done;
  }
  StructTypeHolder *sth = test_LLVM_structtype(L, 1);
  if (sth) {
    sth->type->dump();
    goto done;
  }
  PointerTypeHolder *ph = test_LLVM_pointertype(L, 1);
  if (ph) {
    ph->type->dump();
    goto done;
  }
  FunctionTypeHolder *fh = test_LLVM_functiontype(L, 1);
  if (fh) {
    fh->type->dump();
    goto done;
  }
  FunctionHolder *f = test_LLVM_function(L, 1);
  if (f) {
    f->func->dump();
    goto done;
  }

done:
  return 0;
}

/*
 Add members to a struct
 addmembers(struct, { members })
*/
static int add_members(lua_State *L) {
  StructTypeHolder *sh = check_LLVM_structtype(L, 1);
  luaL_argcheck(L, lua_istable(L, 2), 2, "table expected");
  int len = luaL_len(L, 2);
  std::vector<llvm::Type *> elements;
  for (int i = 1; i <= len; i++) {
    lua_rawgeti(L, 2, i);
    PointerTypeHolder *ph = test_LLVM_pointertype(L, -1);
    if (ph) {
      elements.push_back(ph->type);
      goto done;
    }
    TypeHolder *th = test_LLVM_type(L, -1);
    if (th) {
      elements.push_back(th->type);
      goto done;
    }
    StructTypeHolder *sth = test_LLVM_structtype(L, -1);
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
  if (th) {
    return th->type;
  }
  PointerTypeHolder *ph = test_LLVM_pointertype(L, idx);
  if (ph) {
    return ph->type;
  }
  luaL_argerror(L, idx, "unsupported function return type");
  return nullptr;
}

static llvm::Type *arg_type(lua_State *L, int idx) {
  TypeHolder *th = test_LLVM_type(L, idx);
  if (th) {
    return th->type;
  }
  PointerTypeHolder *ph = test_LLVM_pointertype(L, idx);
  if (ph) {
    return ph->type;
  }
  luaL_argerror(L, idx, "unsupported function argument type");
  return nullptr;
}

/*
  functiontype(rettype, { argtypes } [, { vararg=true })
*/
static int new_function_type(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  llvm::Type *typeret = return_type(L, 1);
  luaL_argcheck(L, lua_istable(L, 2), 2, "table expected");
  int len = luaL_len(L, 2);
  std::vector<llvm::Type *> elements;
  for (int i = 1; i <= len; i++) {
    lua_rawgeti(L, 2, i);
    llvm::Type *t = arg_type(L, -1);
    if (t)
      elements.push_back(t);
    lua_pop(L, 1);
  }
  bool vararg = false;
  if (lua_istable(L, 3))
    l_table_get_bool(L, 3, "vararg", &vararg, false);
  alloc_LLVM_functiontype(L,
                          llvm::FunctionType::get(typeret, elements, vararg));
  return 1;
}

static int new_lua_CFunction(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  const char *name = luaL_checkstring(L, 1);

  FunctionHolder *h =
      alloc_LLVM_function(L, jit, jit->types()->lua_CFunctionT, name);
  /* set L arg */
  h->arg1 = h->func->function()->arg_begin();
  h->arg1->setName("L");
  return 1;
}

static int new_basicblock(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  const char *name = luaL_checkstring(L, 1);

  alloc_LLVM_basicblock(L, llvm::BasicBlock::Create(jit->context(), name));
  return 1;
}

static int append_basicblock(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  FunctionHolder *f = check_LLVM_function(L, 1);
  BasicBlockHolder *b = check_LLVM_basicblock(L, 2);

  f->func->function()->getBasicBlockList().push_back(b->b);
  return 0;
}

static int set_current_block(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  BasicBlockHolder *b = check_LLVM_basicblock(L, 2);
  builder->builder->SetInsertPoint(b->b);

  return 0;
}

static int intconstant(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  int i = (int)luaL_checkinteger(L, 1);
  alloc_LLVM_value(L, jit, llvm::ConstantInt::get(jit->types()->C_intT, i));
  return 1;
}

static int retval(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  ValueHolder *v = test_LLVM_value(L, 2);
  llvm::Instruction *i;
  if (v)
    i = builder->builder->CreateRet(v->value);
  else
    i = builder->builder->CreateRetVoid();
  alloc_LLVM_instruction(L, i);
  return 1;
}

static int branch(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  BasicBlockHolder *b = check_LLVM_basicblock(L, 2);
  llvm::Instruction *i = builder->builder->CreateBr(b->b);
  alloc_LLVM_instruction(L, i);
  return 1;
}

static int condbranch(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  ValueHolder *cond = check_LLVM_value(L, 2);
  BasicBlockHolder *true_br = check_LLVM_basicblock(L, 3);
  BasicBlockHolder *false_br = check_LLVM_basicblock(L, 4);

  llvm::Instruction *i = builder->builder->CreateCondBr(cond->value, true_br->b, false_br->b);
  alloc_LLVM_instruction(L, i);
  return 1;
}

static int compile(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  FunctionHolder *f = check_LLVM_function(L, 1);
  if (!f->compiled_func) {
    f->compiled_func = (lua_CFunction)f->func->compile();
  }

  if (!f->compiled_func)
    return 0;

  lua_pushvalue(L, 1);
  lua_pushcclosure(L, f->compiled_func, 1);

  return 1;
}

typedef int (*decl_func)(lua_State *L, ravi::RaviJITStateImpl *jit,
                         ravi::RaviJITFunctionImpl *f);
typedef struct {
  const char *name;
  decl_func f;
} FunctionDeclaration;

#define str(s) #s

#define decl_func1_(name, ret, arg1, vararg)                                   \
  static int name##_decl(lua_State *L, ravi::RaviJITStateImpl *jit,            \
                         ravi::RaviJITFunctionImpl *f) {                       \
    std::vector<llvm::Type *> args;                                            \
    args.push_back(jit->types()->arg1);                                        \
    llvm::FunctionType *type =                                                 \
        llvm::FunctionType::get(jit->types()->ret, args, vararg);              \
    llvm::Function *extfunc = f->addExternFunction(                            \
        type, reinterpret_cast<void *>(&name), str(name));                     \
    alloc_LLVM_externfunc(L, extfunc);                                         \
    return 1;                                                                  \
  }

#define decl_func1v(name, ret, arg1) decl_func1_(name, ret, arg1, true)
#define decl_func1(name, ret, arg1) decl_func1_(name, ret, arg1, false)

#define decl_func2_(name, ret, arg1, arg2, vararg)                             \
  static int name##_decl(lua_State *L, ravi::RaviJITStateImpl *jit,            \
                         ravi::RaviJITFunctionImpl *f) {                       \
    std::vector<llvm::Type *> args;                                            \
    args.push_back(jit->types()->arg1);                                        \
    args.push_back(jit->types()->arg2);                                        \
    llvm::FunctionType *type =                                                 \
        llvm::FunctionType::get(jit->types()->ret, args, vararg);              \
    llvm::Function *extfunc = f->addExternFunction(                            \
        type, reinterpret_cast<void *>(&name), str(name));                     \
    alloc_LLVM_externfunc(L, extfunc);                                         \
    return 1;                                                                  \
  }

#define decl_func2v(name, ret, arg1, arg2)                                     \
  decl_func2_(name, ret, arg1, arg2, true)
#define decl_func2(name, ret, arg1, arg2)                                      \
  decl_func2_(name, ret, arg1, arg2, false)

#define decl_func3(name, ret, arg1, arg2, arg3)                                \
  static int name##_decl(lua_State *L, ravi::RaviJITStateImpl *jit,            \
                         ravi::RaviJITFunctionImpl *f) {                       \
    std::vector<llvm::Type *> args;                                            \
    args.push_back(jit->types()->arg1);                                        \
    args.push_back(jit->types()->arg2);                                        \
    args.push_back(jit->types()->arg3);                                        \
    llvm::FunctionType *type =                                                 \
        llvm::FunctionType::get(jit->types()->ret, args, false);               \
    llvm::Function *extfunc = f->addExternFunction(                            \
        type, reinterpret_cast<void *>(&name), str(name));                     \
    alloc_LLVM_externfunc(L, extfunc);                                         \
    return 1;                                                                  \
  }

#define decl_func4(name, ret, arg1, arg2, arg3, arg4)                          \
  static int name##_decl(lua_State *L, ravi::RaviJITStateImpl *jit,            \
                         ravi::RaviJITFunctionImpl *f) {                       \
    std::vector<llvm::Type *> args;                                            \
    args.push_back(jit->types()->arg1);                                        \
    args.push_back(jit->types()->arg2);                                        \
    args.push_back(jit->types()->arg3);                                        \
    args.push_back(jit->types()->arg4);                                        \
    llvm::FunctionType *type =                                                 \
        llvm::FunctionType::get(jit->types()->ret, args, false);               \
    llvm::Function *extfunc = f->addExternFunction(                            \
        type, reinterpret_cast<void *>(&name), str(name));                     \
    alloc_LLVM_externfunc(L, extfunc);                                         \
    return 1;                                                                  \
  }

#define decl_voidfunc1(name, arg1)                                             \
  static int name##_decl(lua_State *L, ravi::RaviJITStateImpl *jit,            \
                         ravi::RaviJITFunctionImpl *f) {                       \
    std::vector<llvm::Type *> args;                                            \
    args.push_back(jit->types()->arg1);                                        \
    llvm::FunctionType *type =                                                 \
        llvm::FunctionType::get(jit->types()->C_voidT, args, false);           \
    llvm::Function *extfunc = f->addExternFunction(                            \
        type, reinterpret_cast<void *>(&name), str(name));                     \
    alloc_LLVM_externfunc(L, extfunc);                                         \
    return 1;                                                                  \
  }

#define decl_voidfunc2(name, arg1, arg2)                                       \
  static int name##_decl(lua_State *L, ravi::RaviJITStateImpl *jit,            \
                         ravi::RaviJITFunctionImpl *f) {                       \
    std::vector<llvm::Type *> args;                                            \
    args.push_back(jit->types()->arg1);                                        \
    args.push_back(jit->types()->arg2);                                        \
    llvm::FunctionType *type =                                                 \
        llvm::FunctionType::get(jit->types()->C_voidT, args, false);           \
    llvm::Function *extfunc = f->addExternFunction(                            \
        type, reinterpret_cast<void *>(&name), str(name));                     \
    alloc_LLVM_externfunc(L, extfunc);                                         \
    return 1;                                                                  \
  }

#define decl_voidfunc3(name, arg1, arg2, arg3)                                 \
  static int name##_decl(lua_State *L, ravi::RaviJITStateImpl *jit,            \
                         ravi::RaviJITFunctionImpl *f) {                       \
    std::vector<llvm::Type *> args;                                            \
    args.push_back(jit->types()->arg1);                                        \
    args.push_back(jit->types()->arg2);                                        \
    args.push_back(jit->types()->arg3);                                        \
    llvm::FunctionType *type =                                                 \
        llvm::FunctionType::get(jit->types()->C_voidT, args, false);           \
    llvm::Function *extfunc = f->addExternFunction(                            \
        type, reinterpret_cast<void *>(&name), str(name));                     \
    alloc_LLVM_externfunc(L, extfunc);                                         \
    return 1;                                                                  \
  }

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
decl_func3(luaL_checklstring, C_pcharT, plua_StateT, C_intT, C_psize_t);

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
    {"luaL_checklstring", luaL_checklstring_decl},
    {"printf", printf_decl},
    {nullptr, nullptr}};

/*
 extern(function, name[, type])
 If type is not supplied then name must be one of the predefined
 functions
*/
static int addextern(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  FunctionHolder *f = check_LLVM_function(L, 1);
  const char *name = luaL_checkstring(L, 2);
  luaL_argcheck(L, f->compiled_func == nullptr, 1, "function already compiled");
  FunctionTypeHolder *tt = test_LLVM_functiontype(L, 3);
  if (tt) {
    llvm::Constant *c = f->func->module()->getOrInsertFunction(name, tt->type);
    alloc_LLVM_constant(L, c);
    return 1;
  } else {
    /* TODO binary search */
    for (int i = 0; builtin_functions[i].name != nullptr; i++) {
      if (strcmp(name, builtin_functions[i].name) == 0) {
        return builtin_functions[i].f(L, jit, f->func);
      }
    }
  }
  return 0;
}

static int stringconstant(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  const char *string = luaL_checkstring(L, 2);

  alloc_LLVM_value(L, jit, builder->builder->CreateGlobalStringPtr(string));
  return 1;
}

static int nullconstant(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  PointerTypeHolder *th = check_LLVM_pointertype(L, 1);

  alloc_LLVM_value(L, jit, llvm::ConstantPointerNull::get(th->type));
  return 1;
}

/*
  Get argument of the function
*/
static int arg(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  FunctionHolder *th = check_LLVM_function(L, 1);
  int argn = luaL_checkinteger(L, 2);
  int results = 1;
  if (argn == 1 && th->arg1) {
    alloc_LLVM_value(L, jit, th->arg1);
  } else {
    if (argn <= th->func->function()->arg_size()) {
      auto B = th->func->function()->arg_begin();
      auto E = th->func->function()->arg_end();
      for (int i = 1; B != E; B++, i++) {
        if (i == argn) {
          alloc_LLVM_value(L, jit, B);
          break;
        }
      }
    } else {
      results = 0;
    }
  }
  return results;
}

static int externcall(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;

  ravi::RaviJITStateImpl *jit = (ravi::RaviJITStateImpl *)G->ravi_state->jit;
  if (!jit)
    return 0;

  IRBuilderHolder *builder = check_LLVM_irbuilder(L, 1);
  ExternFuncHolder *f = test_LLVM_externfunc(L, 2);
  ConstantHolder *c = nullptr;
  if (f == nullptr) {
    c = check_LLVM_constant(L, 2);
  }
  luaL_argcheck(L, lua_istable(L, 3), 3, "table expected");
  int len = luaL_len(L, 3);
  std::vector<llvm::Value *> elements;
  for (int i = 1; i <= len; i++) {
    lua_rawgeti(L, 3, i);
    ValueHolder *vh = check_LLVM_value(L, -1);
    elements.push_back(vh->value);
    lua_pop(L, 1);
  }
  llvm::Value *result = builder->builder->CreateCall(
      f != nullptr ? f->function : c->constant, elements);
  alloc_LLVM_value(L, jit, result);
  return 1;
}

static const luaL_Reg llvmlib[] = {{"types", get_standard_types},
                                   {"context", get_llvm_context},
                                   {"irbuilder", alloc_LLVM_irbuilder},
                                   {"structtype", new_struct_type},
                                   {"dump", dump_content},
                                   {"pointertype", new_pointer_type},
                                   {"setbody", add_members},
                                   {"functiontype", new_function_type},
                                   {"lua_CFunction", new_lua_CFunction},
                                   {"basicblock", new_basicblock},
                                   {"appendblock", append_basicblock},
                                   {"setinsertpoint", set_current_block},
                                   {"ret", retval},
                                   {"compile", compile},
                                   {"extern", addextern},
                                   {"intconstant", intconstant},
                                   {"stringconstant", stringconstant},
                                   {"nullconstant", nullconstant},
                                   {"call", externcall},
                                   {"arg", arg},
                                   {"br", branch},
                                   {"condbr", condbranch},
                                   {NULL, NULL}};

LUAMOD_API int raviopen_llvmluaapi(lua_State *L) {
  l_newmetatable(L, LLVM_irbuilder);
  lua_pushstring(L, LLVM_irbuilder);
  lua_setfield(L, -2, "type");
  lua_pushcfunction(L, collect_LLVM_irbuilder);
  lua_setfield(L, -2, "__gc");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_function);
  lua_pushstring(L, LLVM_function);
  lua_setfield(L, -2, "type");
  lua_pushcfunction(L, collect_LLVM_function);
  lua_setfield(L, -2, "__gc");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_type);
  lua_pushstring(L, LLVM_type);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_context);
  lua_pushstring(L, LLVM_context);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_value);
  lua_pushstring(L, LLVM_value);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_structtype);
  lua_pushstring(L, LLVM_structtype);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_pointertype);
  lua_pushstring(L, LLVM_pointertype);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_functiontype);
  lua_pushstring(L, LLVM_functiontype);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_basicblock);
  lua_pushstring(L, LLVM_basicblock);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_externfunc);
  lua_pushstring(L, LLVM_externfunc);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_constant);
  lua_pushstring(L, LLVM_constant);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  l_newmetatable(L, LLVM_instruction);
  lua_pushstring(L, LLVM_instruction);
  lua_setfield(L, -2, "type");
  lua_pop(L, 1);

  luaL_newlib(L, llvmlib);
  return 1;
}

#ifdef __cplusplus
}
#endif

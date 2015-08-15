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
  if (p != NULL) {                                // value is a userdata?
    if (lua_getmetatable(L, arg_index)) {         // does it have a metatable?
      lua_pushlightuserdata(L, (void *)meta_key); // meta_key
      lua_rawget(
          L,
          LUA_REGISTRYINDEX); // get correct metatable associated with meta_key
      if (lua_rawequal(L, -1, -2)) { // compare: does it have the correct mt?
        lua_pop(L, 2);               // remove both metatables
        return p;                    // test ok
      }
    }
  }
  return NULL; /* to avoid warnings */
}

// arg_index is the position of userdata argument on the stack
// meta_key is the key assigned to the meta table of the userdata
static void *l_checkudata(lua_State *L, int arg_index, const char *meta_key) {
  void *p = l_testudata(L, arg_index, meta_key);
  if (p == NULL)
    luaL_argerror(L, arg_index, meta_key);
  return p;
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

struct ContextHolder {
  llvm::LLVMContext *context;
};

struct IRBuilderHolder {
  llvm::IRBuilder<> *builder;
};

struct TypeHolder {
  llvm::Type *type;
};

struct ValueHolder {
  llvm::Value *value;
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
    fprintf(stderr, "collected IRBuilder\n");
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

static const luaL_Reg llvmlib[] = {{"types", get_standard_types},
                                   {"context", get_llvm_context},
                                   {"irbuilder", alloc_LLVM_irbuilder},
                                   {NULL, NULL}};

LUAMOD_API int raviopen_llvmluaapi(lua_State *L) {
  l_newmetatable(L, LLVM_irbuilder);
  lua_pushstring(L, LLVM_irbuilder);
  lua_setfield(L, -2, "type");
  lua_pushcfunction(L, collect_LLVM_irbuilder);
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

  luaL_newlib(L, llvmlib);
  return 1;
}

#ifdef __cplusplus
}
#endif

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
#include <ravi_jitshared.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LUA_CORE

#include "lapi.h"
#include "lauxlib.h"
#include "lfunc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lua.h"

#include <string.h>

// Test if the given function is compiled
static int ravi_is_compiled(lua_State *L) {
  int n = lua_gettop(L);
  luaL_argcheck(L, n == 1, 1, "1 argument expected");
  luaL_argcheck(L, lua_isfunction(L, 1) && !lua_iscfunction(L, 1), 1,
                "argument must be a Lua function");
  void *p = (void *)lua_topointer(L, 1);
  LClosure *l = reinterpret_cast<LClosure *>(p);
  lua_pushboolean(L, l->p->ravi_jit.jit_status == RAVI_JIT_COMPILED);
  return 1;
}

// Utility to extract a integer field from a table
static bool l_table_get_integer(lua_State *L, int tab, const char *key,
                                lua_Integer *result,
                                lua_Integer default_value) {
  bool rc = false;
  lua_pushstring(L, key);
  lua_gettable(L, tab); /* get table[key] */
  if (!lua_isnumber(L, -1))
    *result = default_value;
  else {
    *result = lua_tointeger(L, -1);
    rc = true;
  }
  lua_pop(L, 1); /* remove number */
  return rc;
}

static int ravi_compile_n(lua_State *L) {
  enum { MAX_COMPILES = 100 };
  Proto *functions[MAX_COMPILES];
  int n = 0;
  if (lua_istable(L, 1)) {
    lua_pushnil(L);  // push first key
    while (lua_next(L, 1)) {
      if (n < MAX_COMPILES && lua_isfunction(L, -1) &&
          !lua_iscfunction(L, -1)) {
        void *p = (void *)lua_topointer(L, -1);
        LClosure *l = reinterpret_cast<LClosure *>(p);
        if (!l->p->ravi_jit.jit_function) functions[n++] = l->p;
      }
      lua_pop(L, 1);  // pop value, but keep key
    }
  }
  else {
    luaL_argcheck(L, lua_isfunction(L, 1) && !lua_iscfunction(L, 1), 1,
                  "argument must be a Lua function");
    void *p = (void *)lua_topointer(L, 1);
    LClosure *l = reinterpret_cast<LClosure *>(p);
    functions[n++] = l->p;
  }
  ravi_compile_options_t options = {0, 0, 0, RAVI_CODEGEN_NONE};
  options.manual_request = 1;
  if (lua_istable(L, 2)) {
    lua_Integer ival;
    l_table_get_integer(L, 2, "omitArrayGetRangeCheck",
                        &ival, 0);
    options.omit_array_get_range_check = (int)ival ? 1 : 0;
	l_table_get_integer(L, 2, "inlineLuaArithmeticOperators",
		&ival, 0);
	options.inline_lua_arithmetic_operators = (int)ival ? 1 : 0;
  }
  int result = 0;
  if (n > 0) { result = raviV_compile_n(L, functions, n, &options); }
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
static int ravi_dump_ir(lua_State *L) {
  int n = lua_gettop(L);
  luaL_argcheck(L, n == 1, 1, "1 argument expected");
  luaL_argcheck(L, lua_isfunction(L, 1) && !lua_iscfunction(L, 1), 1,
                "argument must be a Lua function");
  void *p = (void *)lua_topointer(L, 1);
  LClosure *l = reinterpret_cast<LClosure *>(p);
  raviV_dumpIR(L, l->p);
  return 0;
}

// Dump LLVM ASM of the supplied function
// if it has been compiled
static int ravi_dump_asm(lua_State *L) {
  int n = lua_gettop(L);
  luaL_argcheck(L, n == 1, 1, "1 argument expected");
  luaL_argcheck(L, lua_isfunction(L, 1) && !lua_iscfunction(L, 1), 1,
                "argument must be a Lua function");
  void *p = (void *)lua_topointer(L, 1);
  LClosure *l = reinterpret_cast<LClosure *>(p);
  raviV_dumpASM(L, l->p);
  return 0;
}

// Turn on/off auto JIT compilation
static int ravi_auto(lua_State *L) {
  int n = lua_gettop(L);
  lua_pushboolean(L, raviV_getauto(L));
  lua_pushinteger(L, raviV_getmincodesize(L));
  lua_pushinteger(L, raviV_getminexeccount(L));
  if (n >= 1) raviV_setauto(L, lua_toboolean(L, 1));
  int min_code_size = (n >= 2) ? (int)(lua_tointeger(L, 2)) : -1;
  int min_exec_count = (n == 3) ? (int)(lua_tointeger(L, 3)) : -1;
  if (min_code_size >= 1) raviV_setmincodesize(L, min_code_size);
  if (min_exec_count >= 1) raviV_setminexeccount(L, min_exec_count);
  return 3;
}

// Turn on/off the JIT compiler
static int ravi_jitenable(lua_State *L) {
  int n = lua_gettop(L);
  int oldvalue = raviV_getjitenabled(L);
  if (n == 1) {
    int value = lua_toboolean(L, 1);
    raviV_setjitenabled(L, value);
  }
  lua_pushboolean(L, oldvalue != 0);
  return 1;
}

// Set JIT optimizaton level
static int ravi_optlevel(lua_State *L) {
  int n = lua_gettop(L);
  int oldvalue = raviV_getoptlevel(L);
  if (n == 1) {
    int value = lua_tointeger(L, 1);
    raviV_setoptlevel(L, value);
  }
  lua_pushinteger(L, oldvalue);
  return 1;
}

// Set JIT code size level
static int ravi_sizelevel(lua_State *L) {
  int n = lua_gettop(L);
  int oldvalue = raviV_getsizelevel(L);
  if (n == 1) {
    int value = lua_tointeger(L, 1);
    raviV_setsizelevel(L, value);
  }
  lua_pushinteger(L, oldvalue);
  return 1;
}

// Set validation when JIT compiling
static int ravi_validation(lua_State *L) {
  int n = lua_gettop(L);
  int oldvalue = raviV_getvalidation(L);
  if (n == 1) {
    int value = lua_toboolean(L, 1);
    raviV_setvalidation(L, value);
  }
  lua_pushboolean(L, oldvalue);
  return 1;
}

// Set GC step when JIT compiling
static int ravi_gcstep(lua_State *L) {
  int n = lua_gettop(L);
  int oldvalue = raviV_getgcstep(L);
  if (n == 1) {
    int value = lua_tointeger(L, 1);
    raviV_setgcstep(L, value);
  }
  lua_pushboolean(L, oldvalue);
  return 1;
}
  
// Turn on/off the trace hook
static int ravi_traceenable(lua_State *L) {
  int n = lua_gettop(L);
  int oldvalue = raviV_gettraceenabled(L);
  if (n == 1) {
    int value = lua_toboolean(L, 1);
    raviV_settraceenabled(L, value);
  }
  lua_pushboolean(L, oldvalue != 0);
  return 1;
}
  
// Set verbosity
static int ravi_verbosity(lua_State *L) {
  int n = lua_gettop(L);
  int oldvalue = raviV_getverbosity(L);
  if (n == 1) {
    int value = lua_tointeger(L, 1);
    raviV_setverbosity(L, value);
  }
  lua_pushinteger(L, oldvalue);
  return 1;
}

static int ravi_listcode(lua_State *L) {
  luaL_argcheck(L, lua_isfunction(L, 1) && !lua_iscfunction(L, 1), 1,
                "Lua function expected");
  return ravi_list_code(L);
}

static const luaL_Reg ravilib[] = {{"iscompiled", ravi_is_compiled},
                                   {"compile", ravi_compile_n},
                                   {"dumplua", ravi_dump_luacode},
                                   {"dumpllvm", ravi_dump_ir},
                                   {"dumpir", ravi_dump_ir},
                                   {"dumpllvmasm", ravi_dump_asm},
                                   {"dumpasm", ravi_dump_asm},
                                   {"auto", ravi_auto},
                                   {"jit", ravi_jitenable},
                                   {"optlevel", ravi_optlevel},
                                   {"sizelevel", ravi_sizelevel},
                                   {"verbosity", ravi_verbosity},
                                   {"validation", ravi_validation},
                                   {"gcstep", ravi_gcstep},
                                   {"tracehook", ravi_traceenable},
                                   {"listcode", ravi_listcode},
                                   {"limits", ravi_get_limits},
                                   {NULL, NULL}};

#include <math.h> 

LUAMOD_API int raviopen_llvmjit(lua_State *L) {
  luaL_newlib(L, ravilib);
  /* faster calls some maths functions */
  ravi_pushcfastcall(L, NULL, RAVI_TFCF_EXP);
  lua_setfield(L, -2, "exp");
  ravi_pushcfastcall(L, NULL, RAVI_TFCF_LOG);
  lua_setfield(L, -2, "ln");
  return 1;
}

#ifdef __cplusplus
}
#endif

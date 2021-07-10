/******************************************************************************
* Copyright (C) 2020-2021 Dibyendu Majumdar
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
#include "ravi_api.h"

#define LUA_CORE

#include "ravi_mirjit.h"

#include "lua.h"
#include "lapi.h"
#include "lauxlib.h"
#include "lfunc.h"
#include "lmem.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"

#include <string.h>

struct CompilerContext {
  lua_State* L;
  ravi_State* jit;
};

static void debug_message(void* context, const char* filename, long long line, const char* message) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  ravi_writestring(ccontext->L, filename, strlen(filename));
  char temp[80];
  snprintf(temp, sizeof temp, "%lld: ", line);
  ravi_writestring(ccontext->L, temp, strlen(temp));
  ravi_writestring(ccontext->L, message, strlen(message));
  ravi_writeline(ccontext->L);
}

void error_message(void* context, const char* message) {
  struct CompilerContext* ccontext = (struct CompilerContext*)context;
  ravi_writestring(ccontext->L, message, strlen(message));
  ravi_writeline(ccontext->L);
}

static void setup_lua_closure(lua_State* L, LClosure* (*load_in_lua)(lua_State*)) {
  LClosure* cl = load_in_lua(L);
  lua_assert(cl->nupvalues == cl->p->sizeupvalues);
  luaF_initupvals(L, cl);
  ravi_closure_setenv(L);
}

/* JIT compile chunk of code and return function closure, optional options string can include
 * --verbose and --dump-ir.
 */
static int load_and_compile(lua_State* L) {
  const char *s = luaL_checkstring(L, 1);
  const char *options = "";
  if (lua_isstring(L, 2)) {
    options = luaL_checkstring(L, 2);
  }
  struct CompilerContext ccontext = {.L = L, .jit = G(L)->ravi_state};

  struct Ravi_CompilerInterface ravicomp_interface = {.source = s,
                                                      .source_len = strlen(s),
                                                      .source_name = "input",
                                                      .generated_code = NULL,
                                                      .context = &ccontext,
                                                      .debug_message = debug_message,
                                                      .error_message = error_message};
  snprintf(ravicomp_interface.main_func_name, sizeof ravicomp_interface.main_func_name, "__luachunk_%lld",
           ccontext.jit->id++);
  ravicomp_interface.compiler_options = options;
  int rc = raviX_compile(&ravicomp_interface);
  if (ravicomp_interface.generated_code && strstr(options, "--verbose") != NULL) {
    fprintf(stdout, "%s\n", ravicomp_interface.generated_code);
  }
  if (rc == 0) {
#ifdef USE_MIRJIT
    LClosure* (*load_in_lua)(lua_State * L) = NULL;
    mir_prepare(ccontext.jit->jit, 2);
    MIR_module_t M =
        mir_compile_C_module(&ccontext.jit->options, ccontext.jit->jit, ravicomp_interface.generated_code, "input");
    if (M != NULL) {
      load_in_lua = mir_get_func(ccontext.jit->jit, M, ravicomp_interface.main_func_name);
    }
    mir_cleanup(ccontext.jit->jit);
    if (load_in_lua != NULL) {
      setup_lua_closure(L, load_in_lua);
    }
    else {
      rc = -1;
    }
    raviX_release(&ravicomp_interface);
    return rc == 0 ? 1 : 0;
#else
    raviX_release(&ravicomp_interface);
    return 0;
#endif
  }
  else {
    lua_error(L);
    return 0;
  }
}

/* Compile given chunk of code and generate C code, optional arg specifies name of main function */
static int generate(lua_State* L) {
  const char* s = luaL_checkstring(L, 1);
  const char* mainfunc = NULL;
  if (lua_isstring(L, 2)) {
    mainfunc = luaL_checkstring(L, 2);
  }
  struct CompilerContext ccontext = {.L = L, .jit = G(L)->ravi_state};
  struct Ravi_CompilerInterface ravicomp_interface = {.source = s,
                                                      .source_len = strlen(s),
                                                      .source_name = "input",
                                                      .generated_code = NULL,
                                                      .context = &ccontext,
                                                      .compiler_options = "--dump-ir",
                                                      .debug_message = debug_message,
                                                      .error_message = error_message};
  if (mainfunc) {
    snprintf(ravicomp_interface.main_func_name, sizeof ravicomp_interface.main_func_name, "%s", mainfunc);
  }
  else {
    snprintf(ravicomp_interface.main_func_name, sizeof ravicomp_interface.main_func_name, "__luachunk_%lld",
             ccontext.jit->id++);
  }
  int rc = raviX_compile(&ravicomp_interface);
  if (rc == 0) {
    lua_pushstring(L, ravicomp_interface.generated_code);
    raviX_release(&ravicomp_interface);
    return 1;
  }
  else {
    raviX_release(&ravicomp_interface);
    lua_error(L);
    return 0;
  }
}

static const luaL_Reg ravilib[] = {
    {"load", load_and_compile},
    {"compile", generate},
    {NULL, NULL}};

int(raviopen_compiler)(lua_State* L) {
  luaL_newlib(L, ravilib);
  return 1;
}

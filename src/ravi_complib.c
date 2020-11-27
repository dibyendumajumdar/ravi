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

static int load_and_compile(lua_State* L) {
  const char* s = luaL_checkstring(L, 1);
  struct CompilerContext ccontext = {.L = L, .jit = G(L)->ravi_state};

  struct Ravi_CompilerInterface ravicomp_interface = {.source = s,
                                                      .source_len = strlen(s),
                                                      .source_name = "input",
                                                      .generated_code = NULL,
                                                      .context = &ccontext,
                                                      .debug_message = debug_message,
                                                      .error_message = error_message};

  int rc = raviX_compile(&ravicomp_interface);
  if (rc == 0) {
#ifdef USE_MIRJIT
    LClosure* (*load_in_lua)(lua_State * L) = NULL;

    fprintf(stdout, "%s\n", ravicomp_interface.generated_code);

    mir_prepare(ccontext.jit->jit, 2);
    MIR_module_t M =
        mir_compile_C_module(&ccontext.jit->options, ccontext.jit->jit, ravicomp_interface.generated_code, "input");
    if (M != NULL) {
      load_in_lua = mir_get_func(ccontext.jit->jit, M, "load_in_lua");
    }
    mir_cleanup(ccontext.jit->jit);
    if (load_in_lua) {
      LClosure* cl = load_in_lua(L);
      lua_assert(cl->nupvalues == cl->p->sizeupvalues);
      luaF_initupvals(L, cl);
      if (cl->nupvalues >= 1) { /* does it have an upvalue? */
                                /* get global table from registry */
        Table* reg = hvalue(&G(L)->l_registry);
        const TValue* gt = luaH_getint(reg, LUA_RIDX_GLOBALS);
        /* set global table as 1st upvalue of 'f' (may be LUA_ENV) */
        setobj(L, cl->upvals[0]->v, gt);
        luaC_upvalbarrier(L, cl->upvals[0], gt);
      }
    }
    else {
      return 0;
    }
    return 1;
#else
    return 0;
#endif
  }
  else {
    lua_error(L);
    return 0;
  }
}

static const luaL_Reg ravilib[] = {{"load", load_and_compile}, {NULL, NULL}};

int(raviopen_compiler)(lua_State* L) {
  luaL_newlib(L, ravilib);
  return 1;
}
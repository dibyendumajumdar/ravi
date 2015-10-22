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

#include <stddef.h>
#include "ravi_gccjit.h"

ravi_gcc_context_t *ravi_jit_new_context(void) {
  ravi_gcc_context_t *ravi = NULL;
  gcc_jit_context *gcc_ctx = gcc_jit_context_acquire();
  if (!gcc_ctx) {
    fprintf(stderr, "failed to allocate a GCC JIT context\n");
    goto on_error;
  }

  ravi = (ravi_gcc_context_t *)calloc(1, sizeof(ravi_gcc_context_t));
  if (!ravi) {
    fprintf(stderr, "failed to allocate a Ravi JIT context\n");
    goto on_error;
  }
  ravi->context = gcc_ctx;
  ravi->auto_ = false;
  ravi->enabled_ = true;
  ravi->min_code_size_ = 150;
  ravi->min_exec_count_ = 50;
  ravi->opt_level_ = 3;
  ravi->size_level_ = 0;

  if (!ravi_setup_lua_types(ravi)) {
    fprintf(stderr, "failed to setup types\n");
    goto on_error;
  }

  ravi->parent_result_ = gcc_jit_context_compile(ravi->context);
  if (gcc_jit_context_get_first_error(ravi->context)) {
    fprintf(stderr, "aborting due to JIT error: %s\n",
            gcc_jit_context_get_first_error(ravi->context));
    abort();
  }

  return ravi;
on_error:
  if (ravi) {
    ravi_jit_context_free(ravi);
  } else if (gcc_ctx) {
    gcc_jit_context_release(gcc_ctx);
  }
  return NULL;
}

void ravi_jit_context_free(ravi_gcc_context_t *ravi) {
  if (ravi == NULL)
    return;
  if (ravi->parent_result_) {
    gcc_jit_result_release(ravi->parent_result_);
    ravi->parent_result_ = NULL;
  }
  if (ravi->context) {
    gcc_jit_context_release(ravi->context);
    ravi->context = NULL;
  }
  if (ravi->types) {
    free(ravi->types);
    ravi->types = NULL;
  }
  free(ravi);
}

ravi_gcc_codegen_t *ravi_jit_new_codegen(ravi_gcc_context_t *ravi) {
  ravi_gcc_codegen_t *cg = NULL;
  cg = (ravi_gcc_codegen_t *)calloc(1, sizeof(ravi_gcc_codegen_t));
  if (cg == NULL) {
    fprintf(stderr, "error creating a new context: out of memory\n");
    goto on_error;
  }
  cg->id = 1;
  cg->temp[0] = 0;
  cg->ravi = ravi;
  return cg;

on_error:
  if (cg)
    ravi_jit_codegen_free(cg);
  return NULL;
}

void ravi_jit_codegen_free(ravi_gcc_codegen_t *codegen) {
  if (codegen == NULL)
    return;
  free(codegen);
}

bool ravi_jit_has_errored(ravi_gcc_context_t *ravi) {
  const char *msg = gcc_jit_context_get_first_error(ravi->context);
  if (msg) {
    fprintf(stderr, "JIT error: %s\n", msg);
    return true;
  }
  return false;
}

// TODO we probably do not need all the headers
// below

#define LUA_CORE

#include "lua.h"
#include "lobject.h"
#include "lstate.h"
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
void raviV_dumpIR(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}

// Dump the LLVM ASM
void raviV_dumpASM(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}

void raviV_setminexeccount(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return;
  G->ravi_state->jit->min_exec_count_ = value;
}
int raviV_getminexeccount(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;
  return G->ravi_state->jit->min_exec_count_;
}

void raviV_setmincodesize(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return;
  G->ravi_state->jit->min_code_size_ = value;
}
int raviV_getmincodesize(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;
  return G->ravi_state->jit->min_code_size_;
}

void raviV_setauto(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return;
  G->ravi_state->jit->auto_ = value;
}
int raviV_getauto(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;
  return G->ravi_state->jit->auto_;
}

// Turn on/off the JIT compiler
void raviV_setjitenabled(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return;
  G->ravi_state->jit->enabled_ = value;
}
int raviV_getjitenabled(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;
  return G->ravi_state->jit->enabled_;
}

void raviV_setoptlevel(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return;
  G->ravi_state->jit->opt_level_ = value;
}
int raviV_getoptlevel(lua_State *L) {
  global_State *G = G(L);
  if (!G->ravi_state)
    return 0;
  return G->ravi_state->jit->opt_level_;
}

void raviV_setsizelevel(lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getsizelevel(lua_State *L) {
  (void)L;
  return 0;
}

void raviV_setgcstep(lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getgcstep(lua_State *L) {
  (void)L;
  return 0;
}

// Turn on/off the JIT compiler
void raviV_settraceenabled(lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_gettraceenabled(lua_State *L) {
  (void)L;
  return 0;
}
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

  ravi = (ravi_gcc_context_t *) calloc(1, sizeof(ravi_gcc_context_t));
  if (!ravi) {
    fprintf(stderr, "failed to allocate a Ravi JIT context\n");
    goto on_error;
  }
  ravi->context = gcc_ctx;
  ravi->auto_ = false;
  ravi->enabled_ = false;
  ravi->min_code_size_ = 150;
  ravi->min_exec_count_ = 50;
  ravi->opt_level_ = 3;
  ravi->size_level_ = 0;

  if (!ravi_setup_lua_types(ravi)) {
    fprintf(stderr, "failed to setup types\n");
    goto on_error;
  }

  return ravi;
on_error:
  if (ravi) {
    ravi_jit_context_free(ravi);
  }
  else if (gcc_ctx) {
    gcc_jit_context_release(gcc_ctx);
  }
  return NULL;
}

void ravi_jit_context_free(ravi_gcc_context_t *ravi) {
  if (ravi == NULL)
    return;
  if (ravi->context) {
    gcc_jit_context_release(ravi->context);
  }
  if (ravi->types)
    free(ravi->types);
  free(ravi);
}



ravi_gcc_codegen_t *ravi_jit_new_codegen(ravi_gcc_context_t *ravi) {
  (void) ravi;
  return NULL;
}

void ravi_jit_codegen_free(ravi_gcc_context_t *ravi, ravi_gcc_codegen_t *codegen) {
  (void)ravi;
  (void)codegen;
}

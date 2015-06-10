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
  gcc_jit_context *gcc_ctx = gcc_jit_context_acquire();
  if (!gcc_ctx) {
    fprintf(stderr, "failed to allocate a GCC JIT context\n");
    goto on_error;
  }

  ravi_gcc_context_t *ravi = (ravi_gcc_context_t *) calloc(1, sizeof(ravi_gcc_context_t));
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
  assert(ravi != NULL);
  if (ravi->context) {
    gcc_jit_context_release(ravi->context);
  }
  if (ravi->types)
    free(ravi->types);
  free(ravi);
}

bool ravi_setup_lua_types(ravi_gcc_context_t *ravi) {
  ravi->types = (ravi_gcc_types_t *) calloc(1, sizeof(ravi_gcc_types_t));
  if (!ravi->types)
    return false;

  ravi_gcc_types_t *t = ravi->types;

  t->C_doubleT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_DOUBLE);
  t->lua_NumberT = t->C_doubleT;
  t->plua_NumberT = gcc_jit_type_get_pointer(t->lua_NumberT);
  t->pplua_NumberT = gcc_jit_type_get_pointer(t->plua_NumberT);

  t->lua_IntegerT = gcc_jit_context_get_int_type(ravi->context, sizeof(lua_Integer), 1);
  t->plua_IntegerT = gcc_jit_type_get_pointer(t->lua_IntegerT);
  t->pplua_IntegerT = gcc_jit_type_get_pointer(t->plua_IntegerT);

  t->lua_UnsignedT = gcc_jit_context_get_int_type(ravi->context, sizeof(lua_Unsigned), 0);

  t->C_intptr_t = gcc_jit_context_get_int_type(ravi->context, sizeof(intptr_t), 1);
  t->C_size_t = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_SIZE_T);
  t->C_ptrdiff_t = gcc_jit_context_get_int_type(ravi->context, sizeof(ptrdiff_t), 1);
  t->C_int64_t = gcc_jit_context_get_int_type(ravi->context, sizeof(int64_t), 1);
  t->C_intT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_INT);
  t->C_pintT = gcc_jit_type_get_pointer(t->C_intT);
  t->C_shortT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_SHORT);

  t->lu_memT = t->C_size_t;

  t->l_memT = t->C_ptrdiff_t;

  t->tmsT = t->C_intT;

  t->L_UmaxalignT = t->C_doubleT;

  t->lu_byteT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_UNSIGNED_CHAR);
  t->C_charT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_SIGNED_CHAR);
  t->C_pcharT = gcc_jit_type_get_pointer(t->C_charT);

  

  return false;
}
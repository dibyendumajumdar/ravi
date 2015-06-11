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
  if (!ravi)
    return;
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
  t->C_unsigned_intT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_UNSIGNED_INT);

  t->lu_memT = t->C_size_t;

  t->l_memT = t->C_ptrdiff_t;

  t->tmsT = t->C_intT;

  t->L_UmaxalignT = t->C_doubleT;

  t->lu_byteT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_UNSIGNED_CHAR);
  t->C_charT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_SIGNED_CHAR);
  t->C_pcharT = gcc_jit_type_get_pointer(t->C_charT);

  t->C_voidT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_VOID);
  t->C_pvoidT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_VOID_PTR);

  /* typedef unsigned int Instruction */
  t->InstructionT = t->C_unsigned_intT;
  t->pInstructionT = gcc_jit_type_get_pointer(t->InstructionT);

  t->ravitype_tT = gcc_jit_context_get_int_type(ravi->context, sizeof(ravitype_t), 1);

  t->lua_StateT = gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_lua_State");
  t->plua_StateT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->lua_StateT));

  t->lua_KContextT = t->C_ptrdiff_t;

  gcc_jit_type *elements[32];

  /*
   ** Type for C functions registered with Lua
   *  typedef int (*lua_CFunction) (lua_State *L);
   */
  elements[0] = t->plua_StateT;
  t->plua_CFunctionT = gcc_jit_context_new_function_ptr_type(ravi->context, NULL,
                                                             t->C_intT, 1, elements,
                                                             0);

  /*
   ** Type for continuation functions
   * typedef int (*lua_KFunction) (lua_State *L, int status, lua_KContext ctx);
   */
  elements[0] = t->plua_StateT;
  elements[1] = t->C_intT;
  elements[2] = t->lua_KContextT;
  t->plua_KFunctionT = gcc_jit_context_new_function_ptr_type(ravi->context, NULL, t->C_intT,
                                                             3, elements, 0);

  /*
   ** Type for memory-allocation functions
   *  typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);
   */
  elements[0] = t->C_pvoidT;
  elements[1] = t->C_pvoidT;
  elements[2] = t->C_size_t;
  elements[3] = t->C_size_t;
  t->plua_AllocT = gcc_jit_context_new_function_ptr_type(ravi->context, NULL, t->C_voidT,
                                                         4, elements, 0);

  t->lua_DebugT = gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_lua_Debug");
  t->plua_DebugT = gcc_jit_type_get_pointer(t->lua_DebugT);


  /* typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar); */
  elements[0] = t->plua_StateT;
  elements[1] = t->plua_DebugT;
  t->plua_HookT = gcc_jit_context_new_function_ptr_type(ravi->context, NULL, t->C_pvoidT,
                                                        2, elements, 0);

  gcc_jit_field *fields[32];

  // struct GCObject {
  //   GCObject *next;
  //   lu_byte tt;
  //   lu_byte marked
  // };
  t->GCObjectT = gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_GCObject");
  t->pGCObjectT = gcc_jit_type_get_pointer(t->GCObjectT);
  fields[0] = gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  gcc_jit_struct_set_fields(t->GCObjectT, NULL, 3, fields);


  gcc_jit_context_dump_to_file(ravi->context, "dump.txt", 0);
  return false;
}


ravi_gcc_codegen_t *ravi_jit_new_codegen(ravi_gcc_context_t *ravi) {
  return NULL;
}

void ravi_jit_codegen_free(ravi_gcc_context_t *ravi, ravi_gcc_codegen_t *codegen) {

}

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
  t->C_unsigned_shortT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_UNSIGNED_SHORT);
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
  t->plua_DebugT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->lua_DebugT));


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
  t->pGCObjectT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->GCObjectT));
  fields[0] = gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  gcc_jit_struct_set_fields(t->GCObjectT, NULL, 3, fields);

  // union Value {
  //   GCObject *gc;    /* collectable objects */
  //   void *p;         /* light userdata */
  //   int b;           /* booleans */
  //   lua_CFunction f; /* light C functions */
  //   lua_Integer i;   /* integer numbers */
  //   lua_Number n;    /* float numbers */
  // };
  fields[0] = gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "gc");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->C_pvoidT, "p");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "b");
  fields[3] = gcc_jit_context_new_field(ravi->context, NULL, t->plua_CFunctionT, "f");
  fields[4] = gcc_jit_context_new_field(ravi->context, NULL, t->lua_IntegerT, "i");
  fields[5] = gcc_jit_context_new_field(ravi->context, NULL, t->lua_NumberT, "n");
  t->ValueT = gcc_jit_context_new_union_type(ravi->context, NULL, "ravi_Value", 6, fields);


  // NOTE: Following structure changes when NaN tagging is enabled
  // struct TValue {
  //   union Value value_;
  //   int tt_;
  // };
  fields[0] = gcc_jit_context_new_field(ravi->context, NULL, t->ValueT, "value_");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "tt_");
  t->TValueT = gcc_jit_context_new_struct_type(ravi->context, NULL, "ravi_TValue", 2, fields);
  t->pTValueT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->TValueT));

  t->StkIdT = t->pTValueT;

  // typedef struct TString {
  //   GCObject *next;
  //   lu_byte tt;
  //   lu_byte marked
  //   lu_byte extra;  /* reserved words for short strings; "has hash" for longs
  //   */
  //   unsigned int hash;
  //   size_t len;  /* number of characters in string */
  //   struct TString *hnext;  /* linked list for hash table */
  // } TString;
  t->TStringT = gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_TString");
  t->pTStringT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->TStringT));
  fields[0] = gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  fields[3] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "extra");
  fields[4] = gcc_jit_context_new_field(ravi->context, NULL, t->C_unsigned_intT, "hash");
  fields[5] = gcc_jit_context_new_field(ravi->context, NULL, t->C_size_t, "len");
  fields[6] = gcc_jit_context_new_field(ravi->context, NULL, t->pTStringT, "hash");
  gcc_jit_struct_set_fields(t->TStringT, NULL, 7, fields);

  // Table
  t->TableT = gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_Table");
  t->pTableT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->TableT));
  t->ppTableT = gcc_jit_type_get_pointer(t->pTableT);

  //typedef struct Udata {
  //  GCObject *next;
  //  lu_byte tt;
  //  lu_byte marked
  //  lu_byte ttuv_;  /* user value's tag */
  //  struct Table *metatable;
  //  size_t len;  /* number of bytes */
  //  union Value user_;  /* user value */
  //} Udata;
  fields[0] = gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  fields[3] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "ttuv_");
  fields[4] = gcc_jit_context_new_field(ravi->context, NULL, t->pTableT, "metatable");
  fields[5] = gcc_jit_context_new_field(ravi->context, NULL, t->C_size_t, "len");
  fields[6] = gcc_jit_context_new_field(ravi->context, NULL, t->ValueT, "user_");
  t->UdataT = gcc_jit_context_new_struct_type(ravi->context, NULL, "ravi_Udata", 7, fields);


  // typedef struct Upvaldesc {
  //  TString *name;  /* upvalue name (for debug information) */
  //  ravitype_t type;
  //  lu_byte instack;  /* whether it is in stack */
  //  lu_byte idx;  /* index of upvalue (in stack or in outer function's list)
  //  */
  //}Upvaldesc;
  fields[0] = gcc_jit_context_new_field(ravi->context, NULL, t->pTStringT, "name");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->ravitype_tT, "type");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "instack");
  fields[3] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "idx");
  t->UpvaldescT = gcc_jit_context_new_struct_type(ravi->context, NULL, "ravi_Upvaldesc", 4, fields);
  t->pUpvaldescT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->UpvaldescT));

  // typedef struct LocVar {
  //  TString *varname;
  //  int startpc;  /* first point where variable is active */
  //  int endpc;    /* first point where variable is dead */
  //  ravitype_t ravi_type; /* RAVI type of the variable - RAVI_TANY if unknown
  //  */
  //} LocVar;
  fields[0] = gcc_jit_context_new_field(ravi->context, NULL, t->pTStringT, "varname");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "startpc");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "endpc");
  fields[3] = gcc_jit_context_new_field(ravi->context, NULL, t->ravitype_tT, "ravi_type");
  t->LocVarT = gcc_jit_context_new_struct_type(ravi->context, NULL, "ravi_LocVar", 4, fields);
  t->pLocVarT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->LocVarT));

  t->LClosureT = gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_LClosure");
  t->pLClosureT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->LClosureT));
  t->ppLClosureT = gcc_jit_type_get_pointer(t->pLClosureT);
  t->pppLClosureT = gcc_jit_type_get_pointer(t->ppLClosureT);

  //typedef struct RaviJITProto {
  //  lu_byte jit_status; /* 0=not compiled, 1=can't compile, 2=compiled, 3=freed */
  //  lu_byte jit_flags;
  //  unsigned short execution_count;   /* how many times has function been executed */
  //  void *jit_data;
  //  lua_CFunction jit_function;
  //} RaviJITProto;
  fields[0] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "jit_status");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "jit_flags");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->C_unsigned_shortT, "execution_count");
  fields[3] = gcc_jit_context_new_field(ravi->context, NULL, t->C_pvoidT, "jit_data");
  fields[4] = gcc_jit_context_new_field(ravi->context, NULL, t->plua_CFunctionT, "jit_function");
  t->RaviJITProtoT = gcc_jit_context_new_struct_type(ravi->context, NULL, "ravi_RaviJITProto", 5, fields);

  // typedef struct Proto {
  //  CommonHeader;
  //  lu_byte numparams;  /* number of fixed parameters */
  //  lu_byte is_vararg;
  //  lu_byte maxstacksize;  /* maximum stack used by this function */
  //  int sizeupvalues;  /* size of 'upvalues' */
  //  int sizek;  /* size of 'k' */
  //  int sizecode;
  //  int sizelineinfo;
  //  int sizep;  /* size of 'p' */
  //  int sizelocvars;
  //  int linedefined;
  //  int lastlinedefined;
  //  TValue *k;  /* constants used by the function */
  //  Instruction *code;
  //  struct Proto **p;  /* functions defined inside the function */
  //  int *lineinfo;  /* map from opcodes to source lines (debug information) */
  //  LocVar *locvars;  /* information about local variables (debug information)
  //  */
  //  Upvaldesc *upvalues;  /* upvalue information */
  //  struct LClosure *cache;  /* last created closure with this prototype */
  //  TString  *source;  /* used for debug information */
  //  GCObject *gclist;
  //  /* RAVI */
  //  RaviJITProto *ravi_jit;
  //} Proto;
  t->ProtoT = gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_Proto");
  t->pProtoT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->ProtoT));
  t->ppProtoT = gcc_jit_type_get_pointer(t->pProtoT);
  fields[0] = gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  fields[3] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "numparams");
  fields[4] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "is_vararg");
  fields[5] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "maxstacksize");
  fields[6] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizeupvalues");
  fields[7] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizek");
  fields[8] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizecode");
  fields[9] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizelineinfo");
  fields[10] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizep");
  fields[11] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizelocvars");
  fields[12] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "linedefined");
  fields[13] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "lastlinedefined");
  fields[14] = gcc_jit_context_new_field(ravi->context, NULL, t->pTValueT, "k");
  fields[15] = gcc_jit_context_new_field(ravi->context, NULL, t->pInstructionT, "code");
  fields[16] = gcc_jit_context_new_field(ravi->context, NULL, t->ppProtoT, "p");
  fields[17] = gcc_jit_context_new_field(ravi->context, NULL, t->C_pintT, "lineinfo");
  fields[18] = gcc_jit_context_new_field(ravi->context, NULL, t->pLocVarT, "locvars");
  fields[19] = gcc_jit_context_new_field(ravi->context, NULL, t->pUpvaldescT, "upvalues");
  fields[20] = gcc_jit_context_new_field(ravi->context, NULL, t->pLClosureT, "cache");
  fields[21] = gcc_jit_context_new_field(ravi->context, NULL, t->pTStringT, "source");
  fields[22] = gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "gclist");
  fields[23] = gcc_jit_context_new_field(ravi->context, NULL, gcc_jit_struct_as_type(t->RaviJITProtoT), "ravi_jit");
  gcc_jit_struct_set_fields(t->ProtoT, NULL, 24, fields);

  gcc_jit_context_dump_to_file(ravi->context, "dump.txt", 0);
  return false;
}


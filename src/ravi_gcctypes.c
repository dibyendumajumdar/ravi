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
  ravi->types = (ravi_gcc_types_t *)calloc(1, sizeof(ravi_gcc_types_t));
  if (!ravi->types)
    return false;

#ifdef LUA_32BITS
#error 32-bit number is not supported for gcc jit 
#endif

  ravi_gcc_types_t *t = ravi->types;

  t->C_boolT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_BOOL);
  t->C_doubleT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_DOUBLE);
  t->lua_NumberT = t->C_doubleT;
  t->plua_NumberT = gcc_jit_type_get_pointer(t->lua_NumberT);
  t->pplua_NumberT = gcc_jit_type_get_pointer(t->plua_NumberT);

  t->lua_IntegerT =
      gcc_jit_context_get_int_type(ravi->context, sizeof(lua_Integer), 1);
  t->plua_IntegerT = gcc_jit_type_get_pointer(t->lua_IntegerT);
  t->pplua_IntegerT = gcc_jit_type_get_pointer(t->plua_IntegerT);
  //  t->clua_IntegerT = gcc_jit_type_get_const(t->lua_IntegerT);

  t->lua_UnsignedT =
      gcc_jit_context_get_int_type(ravi->context, sizeof(lua_Unsigned), 0);

  t->C_intptr_t =
      gcc_jit_context_get_int_type(ravi->context, sizeof(intptr_t), 1);
  t->C_size_t = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_SIZE_T);
  t->C_ptrdiff_t =
      gcc_jit_context_get_int_type(ravi->context, sizeof(ptrdiff_t), 1);
  t->C_int64_t =
      gcc_jit_context_get_int_type(ravi->context, sizeof(int64_t), 1);
  t->C_intT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_INT);
  t->C_pintT = gcc_jit_type_get_pointer(t->C_intT);
  t->C_shortT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_SHORT);
  t->C_unsigned_shortT =
      gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_UNSIGNED_SHORT);
  t->C_unsigned_intT =
      gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_UNSIGNED_INT);

  t->lu_memT = t->C_size_t;

  t->l_memT = t->C_ptrdiff_t;

  t->tmsT = t->C_intT;

  t->L_UmaxalignT = t->C_doubleT;

  t->lu_byteT =
      gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_UNSIGNED_CHAR);
  t->C_charT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_CHAR);
  t->C_pcharT = gcc_jit_type_get_pointer(t->C_charT);
  t->C_pconstcharT =
      gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_CONST_CHAR_PTR);

  t->C_voidT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_VOID);
  t->C_pvoidT = gcc_jit_context_get_type(ravi->context, GCC_JIT_TYPE_VOID_PTR);

  /* typedef unsigned int Instruction */
  t->InstructionT = t->C_unsigned_intT;
  t->pInstructionT = gcc_jit_type_get_pointer(t->InstructionT);

  t->ravitype_tT =
      gcc_jit_context_get_int_type(ravi->context, sizeof(ravitype_t), 1);

  t->lua_StateT =
      gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_lua_State");
  t->plua_StateT =
      gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->lua_StateT));

  t->lua_KContextT = t->C_ptrdiff_t;

  gcc_jit_type *elements[32];

  /*
   ** Type for C functions registered with Lua
   *  typedef int (*lua_CFunction) (lua_State *L);
   */
  elements[0] = t->plua_StateT;
  t->plua_CFunctionT = gcc_jit_context_new_function_ptr_type(
      ravi->context, NULL, t->C_intT, 1, elements, 0);

  /*
   ** Type for continuation functions
   * typedef int (*lua_KFunction) (lua_State *L, int status, lua_KContext ctx);
   */
  elements[0] = t->plua_StateT;
  elements[1] = t->C_intT;
  elements[2] = t->lua_KContextT;
  t->plua_KFunctionT = gcc_jit_context_new_function_ptr_type(
      ravi->context, NULL, t->C_intT, 3, elements, 0);

  /*
   ** Type for memory-allocation functions
   *  typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t
   *nsize);
   */
  elements[0] = t->C_pvoidT;
  elements[1] = t->C_pvoidT;
  elements[2] = t->C_size_t;
  elements[3] = t->C_size_t;
  t->plua_AllocT = gcc_jit_context_new_function_ptr_type(
      ravi->context, NULL, t->C_voidT, 4, elements, 0);

  t->lua_DebugT =
      gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_lua_Debug");
  t->plua_DebugT =
      gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->lua_DebugT));

  /* typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar); */
  elements[0] = t->plua_StateT;
  elements[1] = t->plua_DebugT;
  t->plua_HookT = gcc_jit_context_new_function_ptr_type(
      ravi->context, NULL, t->C_pvoidT, 2, elements, 0);

  gcc_jit_field *fields[32];

  // struct GCObject {
  //   GCObject *next;
  //   lu_byte tt;
  //   lu_byte marked
  // };
  t->GCObjectT =
      gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_GCObject");
  t->pGCObjectT =
      gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->GCObjectT));
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  gcc_jit_struct_set_fields(t->GCObjectT, NULL, 3, fields);

  // union Value {
  //   GCObject *gc;    /* collectable objects */
  //   void *p;         /* light userdata */
  //   int b;           /* booleans */
  //   lua_CFunction f; /* light C functions */
  //   lua_Integer i;   /* integer numbers */
  //   lua_Number n;    /* float numbers */
  // };
  t->Value_value_gc = fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "gc");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->C_pvoidT, "p");
  t->Value_value_b = fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "b");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->plua_CFunctionT, "f");
  t->Value_value_i = fields[4] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lua_IntegerT, "i");
  t->Value_value_n = fields[5] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lua_NumberT, "n");
  t->ValueT = gcc_jit_context_new_union_type(ravi->context, NULL, "ravi_Value",
                                             6, fields);

  // NOTE: Following structure changes when NaN tagging is enabled
  // struct TValue {
  //   union Value value_;
  //   int tt_;
  // };
  t->Value_value = fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->ValueT, "value_");
  t->Value_tt = fields[1] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "tt_");
  t->TValueT = gcc_jit_context_new_struct_type(ravi->context, NULL,
                                               "ravi_TValue", 2, fields);
  t->pTValueT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->TValueT));
  t->cTValueT = gcc_jit_type_get_const(gcc_jit_struct_as_type(t->TValueT));
  t->pcTValueT = gcc_jit_type_get_pointer(t->cTValueT);

  t->StkIdT = t->pTValueT;

  // typedef struct TString {
  //   GCObject *next;
  //   lu_byte tt;
  //   lu_byte marked
  //   lu_byte extra;  /* reserved words for short strings; "has hash" for longs
  //   */
  //   lu_byte shrlen;  /* length for short strings */
  //   unsigned int hash;
  //   union {
  //     size_t lnglen;  /* length for long strings */
  //     struct TString *hnext;  /* linked list for hash table */
  //   } u;
  // } TString;
  t->TStringT =
      gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_TString");
  t->pTStringT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->TStringT));
  t->ppTStringT = gcc_jit_type_get_pointer(t->pTStringT);
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "extra");
  fields[4] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "shrlen");
  fields[5] = gcc_jit_context_new_field(ravi->context, NULL, t->C_unsigned_intT,
                                        "hash");
  /* union not mapped */
  fields[6] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_size_t, "lnglen");
  gcc_jit_struct_set_fields(t->TStringT, NULL, 7, fields);

  // Table
  t->TableT =
      gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_Table");
  t->pTableT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->TableT));
  t->ppTableT = gcc_jit_type_get_pointer(t->pTableT);

  // typedef struct Udata {
  //  GCObject *next;
  //  lu_byte tt;
  //  lu_byte marked
  //  lu_byte ttuv_;  /* user value's tag */
  //  struct Table *metatable;
  //  size_t len;  /* number of bytes */
  //  union Value user_;  /* user value */
  //} Udata;
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "ttuv_");
  fields[4] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pTableT, "metatable");
  fields[5] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_size_t, "len");
  fields[6] =
      gcc_jit_context_new_field(ravi->context, NULL, t->ValueT, "user_");
  t->UdataT = gcc_jit_context_new_struct_type(ravi->context, NULL, "ravi_Udata",
                                              7, fields);

  // typedef struct Upvaldesc {
  //  TString *name;  /* upvalue name (for debug information) */
  //  ravitype_t type;
  //  lu_byte instack;  /* whether it is in stack */
  //  lu_byte idx;  /* index of upvalue (in stack or in outer function's list)
  //  */
  //}Upvaldesc;
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pTStringT, "name");
  fields[1] =
      gcc_jit_context_new_field(ravi->context, NULL, t->ravitype_tT, "type");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "instack");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "idx");
  t->UpvaldescT = gcc_jit_context_new_struct_type(ravi->context, NULL,
                                                  "ravi_Upvaldesc", 4, fields);
  t->pUpvaldescT =
      gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->UpvaldescT));

  // typedef struct LocVar {
  //  TString *varname;
  //  int startpc;  /* first point where variable is active */
  //  int endpc;    /* first point where variable is dead */
  //  ravitype_t ravi_type; /* RAVI type of the variable - RAVI_TANY if unknown
  //  */
  //} LocVar;
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pTStringT, "varname");
  fields[1] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "startpc");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "endpc");
  fields[3] = gcc_jit_context_new_field(ravi->context, NULL, t->ravitype_tT,
                                        "ravi_type");
  t->LocVarT = gcc_jit_context_new_struct_type(ravi->context, NULL,
                                               "ravi_LocVar", 4, fields);
  t->pLocVarT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->LocVarT));

  t->LClosureT =
      gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_LClosure");
  t->pLClosureT =
      gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->LClosureT));
  t->ppLClosureT = gcc_jit_type_get_pointer(t->pLClosureT);
  t->pppLClosureT = gcc_jit_type_get_pointer(t->ppLClosureT);

  // typedef struct RaviJITProto {
  //  lu_byte jit_status; /* 0=not compiled, 1=can't compile, 2=compiled,
  //  3=freed */
  //  lu_byte jit_flags;
  //  unsigned short execution_count;   /* how many times has function been
  //  executed */
  //  void *jit_data;
  //  lua_CFunction jit_function;
  //} RaviJITProto;
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "jit_status");
  fields[1] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "jit_flags");
  fields[2] = gcc_jit_context_new_field(
      ravi->context, NULL, t->C_unsigned_shortT, "execution_count");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_pvoidT, "jit_data");
  fields[4] = gcc_jit_context_new_field(ravi->context, NULL, t->plua_CFunctionT,
                                        "jit_function");
  t->RaviJITProtoT = gcc_jit_context_new_struct_type(
      ravi->context, NULL, "ravi_RaviJITProto", 5, fields);

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
  t->ProtoT =
      gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_Proto");
  t->pProtoT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->ProtoT));
  t->ppProtoT = gcc_jit_type_get_pointer(t->pProtoT);
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "numparams");
  fields[4] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "is_vararg");
  fields[5] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT,
                                        "maxstacksize");
  fields[6] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizeupvalues");
  fields[7] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizek");
  fields[8] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizecode");
  fields[9] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizelineinfo");
  t->Proto_sizep = fields[10] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizep");
  fields[11] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "sizelocvars");
  fields[12] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "linedefined");
  fields[13] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT,
                                         "lastlinedefined");
  t->LClosure_p_k = fields[14] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pTValueT, "k");
  fields[15] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pInstructionT, "code");
  fields[16] = gcc_jit_context_new_field(ravi->context, NULL, t->ppProtoT, "p");
  fields[17] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_pintT, "lineinfo");
  fields[18] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pLocVarT, "locvars");
  fields[19] = gcc_jit_context_new_field(ravi->context, NULL, t->pUpvaldescT,
                                         "upvalues");
  fields[20] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pLClosureT, "cache");
  fields[21] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pTStringT, "source");
  fields[22] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "gclist");
  fields[23] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->RaviJITProtoT),
      "ravi_jit");
  gcc_jit_struct_set_fields(t->ProtoT, NULL, 24, fields);

  // typedef struct UpVal UpVal;
  t->UpValT =
      gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_UpVal");
  t->pUpValT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->UpValT));

  //#define ClosureHeader CommonHeader; lu_byte nupvalues; GCObject *gclist
  // typedef struct CClosure {
  //  ClosureHeader;
  //  lua_CFunction f;
  //  TValue upvalue[1];  /* list of upvalues */
  //} CClosure;
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "nupvalues");
  fields[4] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "gclist");
  fields[5] =
      gcc_jit_context_new_field(ravi->context, NULL, t->plua_CFunctionT, "f");
  fields[6] = gcc_jit_context_new_field(
      ravi->context, NULL,
      gcc_jit_context_new_array_type(ravi->context, NULL,
                                     gcc_jit_struct_as_type(t->TValueT), 1),
      "upvalue");
  t->CClosureT = gcc_jit_context_new_struct_type(ravi->context, NULL,
                                                 "ravi_CClosure", 7, fields);
  t->pCClosureT =
      gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->CClosureT));

  // typedef struct LClosure {
  //  ClosureHeader;
  //  struct Proto *p;
  //  UpVal *upvals[1];  /* list of upvalues */
  //} LClosure;
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "nupvalues");
  fields[4] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "gclist");
  t->LClosure_p = fields[5] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pProtoT, "p");
  t->LClosure_upvals = fields[6] = gcc_jit_context_new_field(
      ravi->context, NULL,
      gcc_jit_context_new_array_type(ravi->context, NULL, t->pUpValT, 1),
      "upvals");
  gcc_jit_struct_set_fields(t->LClosureT, NULL, 7, fields);

  // typedef union Closure {
  //  CClosure c;
  //  LClosure l;
  //} Closure;

  fields[0] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->CClosureT), "c");
  fields[1] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->LClosureT), "l");
  t->ClosureT = gcc_jit_context_new_union_type(ravi->context, NULL,
                                               "ravi_Closure", 2, fields);
  t->pCClosureT = gcc_jit_type_get_pointer(t->ClosureT);

  // typedef union TKey {
  //  struct {
  //    TValuefields;
  //    int next;  /* for chaining (offset for next node) */
  //  } nk;
  //  TValue tvk;
  //} TKey;

  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->ValueT, "value_");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "tt_");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "next");
  gcc_jit_struct *nk = gcc_jit_context_new_struct_type(
      ravi->context, NULL, "ravi_TKey_nk", 3, fields);

  fields[0] = gcc_jit_context_new_field(ravi->context, NULL,
                                        gcc_jit_struct_as_type(nk), "nk");
  fields[1] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->TValueT), "tvk");
  t->TKeyT = gcc_jit_context_new_union_type(ravi->context, NULL, "ravi_TKey", 2,
                                            fields);
  t->pTKeyT = gcc_jit_type_get_pointer(t->TKeyT);

  // typedef struct Node {
  // TValue i_val;
  // TKey i_key;
  //} Node;
  fields[0] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->TValueT), "i_val");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->TKeyT, "i_key");
  t->NodeT = gcc_jit_context_new_struct_type(ravi->context, NULL, "ravi_Node",
                                             2, fields);
  t->pNodeT = gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->NodeT));

  // typedef struct RaviArray {
  //  char *data;
  //  unsigned int len; /* RAVI len specialization */
  //  unsigned int size; /* amount of memory allocated */
  //  lu_byte array_type; /* RAVI specialization */
  //  lu_byte array_modifier; /* Flags that affect how the array is handled */
  //} RaviArray;

  t->RaviArray_data = fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_pvoidT, "data");
  t->RaviArray_len = fields[1] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_unsigned_intT, "len");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->C_unsigned_intT,
                                        "size");
  t->RaviArray_array_type = fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "array_type");
  fields[4] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT,
                                        "array_modifier");
  t->RaviArrayT = gcc_jit_context_new_struct_type(ravi->context, NULL,
                                                  "ravi_RaviArray", 5, fields);

  // typedef struct Table {
  //  CommonHeader;
  //  lu_byte flags;  /* 1<<p means tagmethod(p) is not present */
  //  lu_byte lsizenode;  /* log2 of size of 'node' array */
  //  unsigned int sizearray;  /* size of 'array' array */
  //  TValue *array;  /* array part */
  //  Node *node;
  //  Node *lastfree;  /* any free position is before this position */
  //  struct Table *metatable;
  //  GCObject *gclist;
  //  RaviArray ravi_array;
  //} Table;
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "flags");
  fields[4] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "lsizenode");
  fields[5] = gcc_jit_context_new_field(ravi->context, NULL, t->C_unsigned_intT,
                                        "sizearray");
  fields[6] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pTValueT, "array");
  fields[7] = gcc_jit_context_new_field(ravi->context, NULL, t->pNodeT, "node");
  fields[8] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pNodeT, "lastfree");
  fields[9] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pTableT, "metatable");
  fields[10] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "gclist");
  t->Table_ravi_array = fields[11] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->RaviArrayT), "ravi_array");
  gcc_jit_struct_set_fields(t->TableT, NULL, 12, fields);

  // struct lua_longjmp;  /* defined in ldo.c */
  t->lua_longjumpT = gcc_jit_context_new_opaque_struct(ravi->context, NULL,
                                                       "ravi_lua_longjmp");
  t->plua_longjumpT =
      gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->lua_longjumpT));

  // lzio.h
  // typedef struct Mbuffer {
  //  char *buffer;
  //  size_t n;
  //  size_t buffsize;
  //} Mbuffer;
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_pcharT, "buffer");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->C_size_t, "n");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_size_t, "buffsize");
  t->MbufferT = gcc_jit_context_new_struct_type(ravi->context, NULL,
                                                "ravi_Mbuffer", 3, fields);

  // typedef struct stringtable {
  //  TString **hash;
  //  int nuse;  /* number of elements */
  //  int size;
  //} stringtable;
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->ppTStringT, "hash");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "nuse");
  fields[2] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "size");
  t->stringtableT = gcc_jit_context_new_struct_type(
      ravi->context, NULL, "ravi_stringtable", 3, fields);

  ///*
  //** Information about a call.
  //** When a thread yields, 'func' is adjusted to pretend that the
  //** top function has only the yielded values in its stack; in that
  //** case, the actual 'func' value is saved in field 'extra'.
  //** When a function calls another with a continuation, 'extra' keeps
  //** the function index so that, in case of errors, the continuation
  //** function can be called with the correct top.
  //*/
  // typedef struct CallInfo {
  //  StkId func;  /* function index in the stack */
  //  StkId	top;  /* top for this function */
  //  struct CallInfo *previous, *next;  /* dynamic call link */
  //  union {
  //    struct {  /* only for Lua functions */
  //      StkId base;  /* base for this function */
  //      const Instruction *savedpc;
  //    } l;
  //    struct {  /* only for C functions */
  //      lua_KFunction k;  /* continuation in case of yields */
  //      ptrdiff_t old_errfunc;
  //      lua_KContext ctx;  /* context info. in case of yields */
  //    } c;
  //  } u;
  //  ptrdiff_t extra;
  //  short nresults;  /* expected number of results from this function */
  //  unsigned short callstatus;
  //  unsigned short stacklevel; /* Ravi extension - stack level, bottom level is 0 */
  //  lu_byte jitstatus; /* Only valid if Lua function - if 1 means JITed - RAVI extension */
  //} CallInfo;

  t->CallInfoT =
      gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_CallInfo");
  t->pCallInfoT =
      gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->CallInfoT));

  t->CallInfo_u_l_base = fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->StkIdT, "base");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->pInstructionT,
                                        "savedpc");
  t->CallInfo_lT = gcc_jit_context_new_struct_type(
      ravi->context, NULL, "ravi_CallInfo_lua", 2, fields);

  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->plua_KFunctionT, "k");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->C_ptrdiff_t,
                                        "old_errfunc");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lua_KContextT, "ctx");
  t->CallInfo_cT = gcc_jit_context_new_struct_type(
      ravi->context, NULL, "ravi_CallInfo_C", 3, fields);

  t->CallInfo_u_l = fields[0] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->CallInfo_lT), "l");
  fields[1] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->CallInfo_cT), "c");
  t->CallInfo_uT = gcc_jit_context_new_union_type(ravi->context, NULL,
                                                  "ravi_CallInfo_u", 2, fields);

  t->CallInfo_func = fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->StkIdT, "func");
  t->CallInfo_top = fields[1] =
      gcc_jit_context_new_field(ravi->context, NULL, t->StkIdT, "top");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pCallInfoT, "previous");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pCallInfoT, "next");
  t->CallInfo_u = fields[4] =
      gcc_jit_context_new_field(ravi->context, NULL, t->CallInfo_uT, "u");
  fields[5] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_ptrdiff_t, "extra");
  fields[6] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_shortT, "nresults");
  fields[7] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_shortT, "callstatus");
  fields[8] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_shortT, "stacklevel");
  fields[9] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "jitstatus");

  gcc_jit_struct_set_fields(t->CallInfoT, NULL, 10, fields);

  // typedef struct ravi_State ravi_State;
  t->ravi_StateT =
      gcc_jit_context_new_opaque_struct(ravi->context, NULL, "ravi_State");
  t->pravi_StateT =
      gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->ravi_StateT));

  // typedef struct global_State global_State;
  t->global_StateT = gcc_jit_context_new_opaque_struct(ravi->context, NULL,
                                                       "ravi_global_State");
  t->pglobal_StateT =
      gcc_jit_type_get_pointer(gcc_jit_struct_as_type(t->global_StateT));

  ///*
  //** 'per thread' state
  //*/
  // struct lua_State {
  //  CommonHeader;
  //  lu_byte status;
  //  StkId top;  /* first free slot in the stack */
  //  global_State *l_G;
  //  CallInfo *ci;  /* call info for current function */
  //  const Instruction *oldpc;  /* last pc traced */
  //  StkId stack_last;  /* last free slot in the stack */
  //  StkId stack;  /* stack base */
  //  UpVal *openupval;  /* list of open upvalues in this stack */
  //  GCObject *gclist;
  //  struct lua_State *twups;  /* list of threads with open upvalues */
  //  struct lua_longjmp *errorJmp;  /* current error recover point */
  //  CallInfo base_ci;  /* CallInfo for first level (C calling Lua) */
  //  lua_Hook hook;
  //  ptrdiff_t errfunc;  /* current error handling function (stack index) */
  //  int stacksize;
  //  int basehookcount;
  //  int hookcount;
  //  unsigned short nny;  /* number of non-yieldable calls in stack */
  //  unsigned short nCcalls;  /* number of nested C calls */
  //  lu_byte hookmask;
  //  lu_byte allowhook;
  //  unsigned short nci;  /* number of items in 'ci' list */
  //};
  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "next");
  fields[1] = gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "tt");
  fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "marked");
  fields[3] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "status");
  t->lua_State_top = fields[4] =
      gcc_jit_context_new_field(ravi->context, NULL, t->StkIdT, "top");
  fields[5] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pglobal_StateT, "l_G");
  t->lua_State_ci = fields[6] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pCallInfoT, "ci");
  fields[7] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pInstructionT, "oldpc");
  fields[8] =
      gcc_jit_context_new_field(ravi->context, NULL, t->StkIdT, "stack_last");
  fields[9] =
      gcc_jit_context_new_field(ravi->context, NULL, t->StkIdT, "stack");
  fields[10] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pUpValT, "openupval");
  fields[11] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pGCObjectT, "gclist");
  fields[12] =
      gcc_jit_context_new_field(ravi->context, NULL, t->plua_StateT, "twups");
  fields[13] = gcc_jit_context_new_field(ravi->context, NULL, t->plua_longjumpT,
                                         "errorJmp");
  fields[14] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->CallInfoT), "base_ci");
  fields[15] =
      gcc_jit_context_new_field(ravi->context, NULL, t->plua_HookT, "hook");
  fields[16] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_ptrdiff_t, "errfunc");
  fields[17] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "stacksize");
  fields[18] = gcc_jit_context_new_field(ravi->context, NULL, t->C_intT,
                                         "basehookcount");
  fields[19] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "hookcount");
  fields[20] = gcc_jit_context_new_field(ravi->context, NULL,
                                         t->C_unsigned_shortT, "nny");
  fields[21] = gcc_jit_context_new_field(ravi->context, NULL,
                                         t->C_unsigned_shortT, "nCcalls");
  fields[22] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "hookmask");
  fields[23] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_byteT, "allowhook");
  fields[24] =
          gcc_jit_context_new_field(ravi->context, NULL, t->C_unsigned_shortT, "nci");
  gcc_jit_struct_set_fields(t->lua_StateT, NULL, 25, fields);

  // struct UpVal {
  //  struct TValue *v;  /* points to stack or to its own value */
  //  lu_mem refcount;  /* reference counter */
  //  union {
  //    struct {  /* (when open) */
  //      struct UpVal *next;  /* linked list */
  //      int touched;  /* mark to avoid cycles with dead threads */
  //    } open;
  //    struct TValue value;  /* the value (when closed) */
  //  } u;
  //};

  fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pUpValT, "next");
  fields[1] =
      gcc_jit_context_new_field(ravi->context, NULL, t->C_intT, "touched");
  t->UpVal_u_openT = gcc_jit_context_new_struct_type(
      ravi->context, NULL, "ravi_UpVal_u_open", 2, fields);

  fields[0] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->UpVal_u_openT), "open");
  t->UpVal_u_value = fields[1] = gcc_jit_context_new_field(
      ravi->context, NULL, gcc_jit_struct_as_type(t->TValueT), "value");
  t->UpVal_uT = gcc_jit_context_new_union_type(ravi->context, NULL,
                                               "ravi_UpVal_u", 2, fields);

  t->UpVal_v = fields[0] =
      gcc_jit_context_new_field(ravi->context, NULL, t->pTValueT, "v");
  fields[1] =
      gcc_jit_context_new_field(ravi->context, NULL, t->lu_memT, "refcount");
  t->UpVal_u = fields[2] =
      gcc_jit_context_new_field(ravi->context, NULL, t->UpVal_uT, "u");
  gcc_jit_struct_set_fields(t->UpValT, NULL, 3, fields);

  gcc_jit_param *params[12];

  // int luaD_poscall (lua_State *L, CallInfo *ci, StkId firstResult, int nres);
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pCallInfoT, "ci");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->StkIdT, "firstResult");
  params[3] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "nres");
  t->luaD_poscallT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_intT, "luaD_poscall",
      4, params, 0);

  // void luaC_upvalbarrier_ (lua_State *L, UpVal *uv)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] = gcc_jit_context_new_param(ravi->context, NULL, t->pUpValT, "uv");
  t->luaC_upvalbarrierT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "luaC_upvalbarrier_", 2, params, 0);

  // int luaD_precall (lua_State *L, StkId func, int nresults, int op_call);
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] = gcc_jit_context_new_param(ravi->context, NULL, t->StkIdT, "func");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "nresults");
  params[3] =
      gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "op_call");
  t->luaD_precallT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_intT, "luaD_precall",
      4, params, 0);

  // void luaD_call (lua_State *L, StkId func, int nResults)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] = gcc_jit_context_new_param(ravi->context, NULL, t->StkIdT, "func");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "nresults");
  t->luaD_callT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_intT, "luaD_call", 3,
      params, 0);

  // void luaV_execute(lua_State L);
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  t->luaV_executeT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_intT, "luaV_execute",
      1, params, 0);

  // void luaF_close (lua_State *L, StkId level)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->StkIdT, "level");
  t->luaF_closeT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT, "luaF_close",
      2, params, 0);

  // TODO const
  // int luaV_equalobj (lua_State *L, const TValue *t1, const TValue *t2)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "t1");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "t2");
  t->luaV_equalobjT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_intT,
      "luaV_equalobj", 3, params, 0);

  // TODO const
  // int luaV_lessthan (lua_State *L, const TValue *l, const TValue *r)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] = gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "l");
  params[2] = gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "r");
  t->luaV_lessthanT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_intT,
      "luaV_lessthan", 3, params, 0);

  // TODO const
  // int luaV_lessequal (lua_State *L, const TValue *l, const TValue *r)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] = gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "l");
  params[2] = gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "r");
  t->luaV_lessequalT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_intT,
      "luaV_lessequal", 3, params, 0);

  // TODO const
  // l_noret luaG_runerror (lua_State *L, const char *fmt, ...)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->C_pconstcharT, "fmt");
  t->luaG_runerrorT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "luaG_runerror", 2, params, 1);

  // TODO const
  // int luaV_forlimit (const TValue *obj, lua_Integer *p, lua_Integer step,
  //                    int *stopnow)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pTValueT, "obj");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_IntegerT, "p");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->lua_IntegerT, "step");
  params[3] =
      gcc_jit_context_new_param(ravi->context, NULL, t->C_pintT, "stopnow");
  t->luaV_forlimitT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_intT,
      "luaV_forlimit", 4, params, 0);

  // TODO const
  // int luaV_tonumber_ (const TValue *obj, lua_Number *n)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "obj");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_NumberT, "n");
  t->luaV_tonumberT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_intT,
      "luaV_tonumber_", 2, params, 0);

  // TODO const
  // int luaV_tointeger_ (const TValue *obj, lua_Integer *p)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "obj");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_IntegerT, "p");
  t->luaV_tointegerT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_intT,
      "luaV_tointeger_", 2, params, 0);

  // TODO const
  // void luaV_objlen (lua_State *L, StkId ra, const TValue *rb)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] = gcc_jit_context_new_param(ravi->context, NULL, t->StkIdT, "ra");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "rb");
  t->luaV_objlenT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT, "luaV_objlen",
      3, params, 0);

  // TODO const
  // void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] = gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "t");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pTValueT, "key");
  params[3] = gcc_jit_context_new_param(ravi->context, NULL, t->StkIdT, "val");
  t->luaV_gettableT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "luaV_gettable", 4, params, 0);

  // TODO const
  // void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] = gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "t");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pTValueT, "key");
  params[3] = gcc_jit_context_new_param(ravi->context, NULL, t->StkIdT, "val");
  t->luaV_settableT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "luaV_settable", 4, params, 0);

  // TODO const
  // void luaT_trybinTM (lua_State *L, const TValue *p1, const TValue *p2,
  //                     StkId res, TMS event);
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "p1");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pcTValueT, "p2");
  params[3] = gcc_jit_context_new_param(ravi->context, NULL, t->StkIdT, "res");
  params[4] = gcc_jit_context_new_param(ravi->context, NULL, t->tmsT, "event");
  t->luaT_trybinTMT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "luaT_trybinTM", 5, params, 0);

  // void raviV_op_loadnil(CallInfo *ci, int a, int b)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pCallInfoT, "ci");
  params[1] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "a");
  params[2] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "b");
  t->raviV_op_loadnilT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviV_op_loadnil", 3, params, 0);

  // void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pCallInfoT, "ci");
  params[2] = gcc_jit_context_new_param(ravi->context, NULL, t->pTValueT, "ra");
  t->raviV_op_newarrayintT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviV_op_newarrayint", 3, params, 0);

  // void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pCallInfoT, "ci");
  params[2] = gcc_jit_context_new_param(ravi->context, NULL, t->pTValueT, "ra");
  t->raviV_op_newarrayfloatT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviV_op_newarrayfloat", 3, params, 0);

  // void raviV_op_newtable(lua_State *L, CallInfo *ci, TValue *ra, int b, int
  // c)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pCallInfoT, "ci");
  params[2] = gcc_jit_context_new_param(ravi->context, NULL, t->pTValueT, "ra");
  params[3] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "b");
  params[4] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "c");
  t->raviV_op_newtableT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviV_op_newtable", 5, params, 0);

  // void raviV_op_setlist(lua_State *L, CallInfo *ci, TValue *ra, int b, int c)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pCallInfoT, "ci");
  params[2] = gcc_jit_context_new_param(ravi->context, NULL, t->pTValueT, "ra");
  params[3] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "b");
  params[4] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "c");
  t->raviV_op_setlistT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviV_op_setlist", 5, params, 0);

  // lua_Integer luaV_div (lua_State *L, lua_Integer m, lua_Integer n)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->lua_IntegerT, "m");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->lua_IntegerT, "n");
  t->luaV_divT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->lua_IntegerT,
      "luaV_div", 3, params, 0);

  // lua_Integer luaV_mod (lua_State *L, lua_Integer m, lua_Integer n)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->lua_IntegerT, "m");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->lua_IntegerT, "n");
  t->luaV_modT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->lua_IntegerT,
      "luaV_mod", 3, params, 0);

  // void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pCallInfoT, "ci");
  params[2] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "a");
  params[3] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "b");
  params[4] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "c");
  t->raviV_op_concatT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviV_op_concat", 5, params, 0);

  // void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int
  // Bx)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pCallInfoT, "ci");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pLClosureT, "cl");
  params[3] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "a");
  params[4] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "Bx");
  t->raviV_op_closureT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviV_op_closure", 5, params, 0);

  // void raviV_op_vararg(lua_State *L, CallInfo *ci, LClosure *cl, int a, int
  // b)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pCallInfoT, "ci");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pLClosureT, "cl");
  params[3] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "a");
  params[4] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "b");
  t->raviV_op_varargT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviV_op_vararg", 5, params, 0);

  // void raviH_set_int(lua_State *L, Table *t, lua_Unsigned key, lua_Integer
  // value);
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pTableT, "table");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->lua_UnsignedT, "key");
  params[3] =
      gcc_jit_context_new_param(ravi->context, NULL, t->lua_IntegerT, "value");
  t->raviH_set_intT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviH_set_int", 4, params, 0);

  // void raviH_set_float(lua_State *L, Table *t, lua_Unsigned key, lua_Number
  // value);
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pTableT, "table");
  params[2] =
      gcc_jit_context_new_param(ravi->context, NULL, t->lua_UnsignedT, "key");
  params[3] =
      gcc_jit_context_new_param(ravi->context, NULL, t->lua_NumberT, "value");
  t->raviH_set_floatT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviH_set_float", 4, params, 0);

  // void raviV_op_setupval(lua_State *L, LClosure *cl, TValue *ra, int b)
  params[0] =
      gcc_jit_context_new_param(ravi->context, NULL, t->plua_StateT, "L");
  params[1] =
      gcc_jit_context_new_param(ravi->context, NULL, t->pLClosureT, "cl");
  params[2] = gcc_jit_context_new_param(ravi->context, NULL, t->pTValueT, "ra");
  params[3] = gcc_jit_context_new_param(ravi->context, NULL, t->C_intT, "b");
  t->raviV_op_setupvalT = gcc_jit_context_new_function(
      ravi->context, NULL, GCC_JIT_FUNCTION_IMPORTED, t->C_voidT,
      "raviV_op_setupval", 4, params, 0);

  params[0] = gcc_jit_context_new_param(ravi->context, NULL, t->C_pconstcharT,
                                        "format");
  t->printfT = gcc_jit_context_new_function(ravi->context, NULL,
                                            GCC_JIT_FUNCTION_IMPORTED,
                                            t->C_intT, "printf", 1, params, 1);

  return ravi_jit_has_errored(ravi) ? false : true;
}

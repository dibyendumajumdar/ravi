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

#ifndef RAVI_RAVI_GCCJIT_H
#define RAVI_RAVI_GCCJIT_H

#ifdef USE_GCCJIT
#include <libgccjit.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO we probably do not need all the headers
// below

#define lvm_c
#define LUA_CORE

#include "lprefix.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"



typedef enum {
  LUA__TNIL = LUA_TNIL,
  LUA__TBOOLEAN = LUA_TBOOLEAN,
  LUA__TLIGHTUSERDATA = LUA_TLIGHTUSERDATA,
  LUA__TNUMBER = LUA_TNUMBER,
  LUA__TSTRING = ctb(LUA_TSTRING),
  LUA__TTABLE = ctb(LUA_TTABLE),
  LUA__TFUNCTION = ctb(LUA_TFUNCTION),
  LUA__TUSERDATA = ctb(LUA_TUSERDATA),
  LUA__TTHREAD = ctb(LUA_TTHREAD),
  LUA__TLCL = ctb(LUA_TLCL),
  LUA__TLCF = LUA_TLCF,
  LUA__TCCL = ctb(LUA_TCCL),
  LUA__TSHRSTR = ctb(LUA_TSHRSTR),
  LUA__TLNGSTR = ctb(LUA_TLNGSTR),
  LUA__TNUMFLT = LUA_TNUMFLT,
  LUA__TNUMINT = LUA_TNUMINT
} lua_typecode_t;


typedef struct ravi_gcc_context_t ravi_gcc_context_t;
typedef struct ravi_gcc_types_t ravi_gcc_types_t;
typedef struct ravi_gcc_codegen_t ravi_gcc_codegen_t;

struct ravi_gcc_types_t {

  gcc_jit_type *C_doubleT;
  gcc_jit_type *C_intptr_t;
  gcc_jit_type *C_size_t;
  gcc_jit_type *C_ptrdiff_t;
  gcc_jit_type *C_int64_t;
  gcc_jit_type *C_shortT;
  gcc_jit_type *C_unsigned_shortT;
  gcc_jit_type *C_voidT;
  gcc_jit_type *C_pvoidT;

  gcc_jit_type *lua_NumberT;
  gcc_jit_type *plua_NumberT;
  gcc_jit_type *pplua_NumberT;

  gcc_jit_type *lua_IntegerT;
  gcc_jit_type *plua_IntegerT;
  gcc_jit_type *pplua_IntegerT;

  gcc_jit_type *lua_UnsignedT;
  gcc_jit_type *lua_KContextT;

  //gcc_jit_function *lua_CFunctionT;
  gcc_jit_type *plua_CFunctionT;

  //gcc_jit_function *lua_KFunctionT;
  gcc_jit_type *plua_KFunctionT;

  //gcc_jit_function *lua_HookT;
  gcc_jit_type *plua_HookT;

  //gcc_jit_function *lua_AllocT;
  gcc_jit_type *plua_AllocT;

  gcc_jit_type *l_memT;
  gcc_jit_type *lu_memT;

  gcc_jit_type *tmsT;

  gcc_jit_type *lu_byteT;
  gcc_jit_type *L_UmaxalignT;
  gcc_jit_type *C_charT;
  gcc_jit_type *C_pcharT;

  gcc_jit_type *C_intT;
  gcc_jit_type *C_pintT;
  gcc_jit_type *C_unsigned_intT;

  gcc_jit_struct *lua_StateT;
  gcc_jit_type *plua_StateT;

  gcc_jit_struct *global_StateT;
  gcc_jit_type *pglobal_StateT;

  gcc_jit_struct *ravi_StateT;
  gcc_jit_type *pravi_StateT;

  gcc_jit_struct *GCObjectT;
  gcc_jit_type *pGCObjectT;

  gcc_jit_type *ValueT;
  gcc_jit_struct *TValueT;
  gcc_jit_type *pTValueT;

  gcc_jit_struct *HiLoT;
  gcc_jit_type *pHiLoT;

  gcc_jit_struct *TStringT;
  gcc_jit_type *pTStringT;
  gcc_jit_type *ppTStringT;

  gcc_jit_struct *UdataT;
  gcc_jit_struct *RaviArrayT;
  gcc_jit_struct *TableT;
  gcc_jit_type *pTableT;
  gcc_jit_type *ppTableT;

  gcc_jit_struct *UpvaldescT;
  gcc_jit_type *pUpvaldescT;

  gcc_jit_type *ravitype_tT;
  gcc_jit_struct *LocVarT;
  gcc_jit_type *pLocVarT;

  gcc_jit_type *InstructionT;
  gcc_jit_type *pInstructionT;
  gcc_jit_struct *LClosureT;
  gcc_jit_type *pLClosureT;
  gcc_jit_type *ppLClosureT;
  gcc_jit_type *pppLClosureT;
  gcc_jit_type *ClosureT;
  gcc_jit_type *pClosureT;

  gcc_jit_struct *RaviJITProtoT;

  gcc_jit_struct *ProtoT;
  gcc_jit_type *pProtoT;
  gcc_jit_type *ppProtoT;

  gcc_jit_struct *UpValT;
  gcc_jit_type *pUpValT;
  gcc_jit_struct *UpVal_u_openT;
  gcc_jit_type *UpVal_uT;

  gcc_jit_struct *CClosureT;
  gcc_jit_type *pCClosureT;

  gcc_jit_type *TKeyT;
  gcc_jit_type *pTKeyT;

  gcc_jit_struct *NodeT;
  gcc_jit_type *pNodeT;

  gcc_jit_struct *lua_DebugT;
  gcc_jit_type *plua_DebugT;

  gcc_jit_struct *lua_longjumpT;
  gcc_jit_type *plua_longjumpT;

  gcc_jit_struct *MbufferT;
  gcc_jit_struct *stringtableT;

  gcc_jit_type *StkIdT;

  gcc_jit_struct *CallInfoT;
  gcc_jit_struct *CallInfo_cT;
  gcc_jit_struct *CallInfo_lT;
  gcc_jit_type *CallInfo_uT;
  gcc_jit_type *pCallInfoT;

  gcc_jit_function *jitFunctionT;

  gcc_jit_function *luaD_poscallT;
  gcc_jit_function *luaD_precallT;
  gcc_jit_function *luaD_callT;
  gcc_jit_function *luaF_closeT;
  gcc_jit_function *luaT_trybinTMT;
  gcc_jit_function *luaG_runerrorT;
  gcc_jit_function *luaV_equalobjT;
  gcc_jit_function *luaV_lessthanT;
  gcc_jit_function *luaV_lessequalT;
  gcc_jit_function *luaV_forlimitT;
  gcc_jit_function *luaV_tonumberT;
  gcc_jit_function *luaV_tointegerT;
  gcc_jit_function *luaV_modT;
  gcc_jit_function *luaV_objlenT;
  gcc_jit_function *luaV_divT;
  gcc_jit_function *luaC_upvalbarrierT;

  gcc_jit_function *luaV_executeT;
  gcc_jit_function *luaV_gettableT;
  gcc_jit_function *luaV_settableT;

  // Following are functions that handle specific bytecodes
  // We cheat for these bytecodes by calling the function that
  // implements it
  gcc_jit_function *raviV_op_newarrayintT;
  gcc_jit_function *raviV_op_newarrayfloatT;
  gcc_jit_function *raviV_op_setlistT;
  gcc_jit_function *raviV_op_newtableT;
  gcc_jit_function *raviV_op_loadnilT;
  gcc_jit_function *raviV_op_concatT;
  gcc_jit_function *raviV_op_closureT;
  gcc_jit_function *raviV_op_varargT;

  gcc_jit_function *raviH_set_intT;
  gcc_jit_function *raviH_set_floatT;
};

struct ravi_gcc_context_t {

  /* parent JIT context - all functions are child contexts
   * of this.
   */
  gcc_jit_context *context;

  /* Lua type definitions */
  ravi_gcc_types_t *types;

  /* Should we auto compile what we can? */
  bool auto_;

  /* Is JIT enabled */
  bool enabled_;

  /* Optimizer level */
  int opt_level_;

  /* Size level */
  int size_level_;

  /* min code size for compilation */
  int min_code_size_;

  /* min execution count for compilation */
  int min_exec_count_;
};


/* Create a new context */
extern ravi_gcc_context_t *ravi_jit_new_context(void);

/* Destroy a context */
extern void ravi_jit_context_free(ravi_gcc_context_t *);

/* Setup Lua types */
extern bool ravi_setup_lua_types(ravi_gcc_context_t *);

extern ravi_gcc_codegen_t *ravi_jit_new_codegen(ravi_gcc_context_t *);

extern void ravi_jit_codegen_free(ravi_gcc_context_t *, ravi_gcc_codegen_t *);

extern bool ravi_jit_has_errored(ravi_gcc_context_t *);

#ifdef __cplusplus
};
#endif

#endif /* USE_GCCJIT */

#endif /* RAVI_RAVI_GCCJIT_H */

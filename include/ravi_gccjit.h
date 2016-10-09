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
#include "ravijit.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO we probably do not need all the headers
// below

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
  RAVI__TLTABLE = ctb(LUA_TTABLE),
  RAVI__TIARRAY = ctb(RAVI_TIARRAY),
  RAVI__TFARRAY = ctb(RAVI_TFARRAY),
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

struct ravi_State {
  ravi_gcc_context_t *jit;
  ravi_gcc_codegen_t *code_generator;
};

struct ravi_gcc_types_t {

  gcc_jit_type *C_boolT;
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
  // gcc_jit_type *clua_IntegerT;

  gcc_jit_type *lua_UnsignedT;
  gcc_jit_type *lua_KContextT;

  // gcc_jit_function *lua_CFunctionT;
  gcc_jit_type *plua_CFunctionT;

  // gcc_jit_function *lua_KFunctionT;
  gcc_jit_type *plua_KFunctionT;

  // gcc_jit_function *lua_HookT;
  gcc_jit_type *plua_HookT;

  // gcc_jit_function *lua_AllocT;
  gcc_jit_type *plua_AllocT;

  gcc_jit_type *l_memT;
  gcc_jit_type *lu_memT;

  gcc_jit_type *tmsT;

  gcc_jit_type *lu_byteT;
  gcc_jit_type *L_UmaxalignT;
  gcc_jit_type *C_charT;
  gcc_jit_type *C_pcharT;
  gcc_jit_type *C_pconstcharT;

  gcc_jit_type *C_intT;
  gcc_jit_type *C_pintT;
  gcc_jit_type *C_unsigned_intT;

  gcc_jit_struct *lua_StateT;
  gcc_jit_type *plua_StateT;

  gcc_jit_field *lua_State_ci;
  gcc_jit_field *lua_State_top;

  gcc_jit_struct *global_StateT;
  gcc_jit_type *pglobal_StateT;

  gcc_jit_struct *ravi_StateT;
  gcc_jit_type *pravi_StateT;

  gcc_jit_struct *GCObjectT;
  gcc_jit_type *pGCObjectT;

  gcc_jit_type *ValueT;
  gcc_jit_struct *TValueT;
  gcc_jit_type *cTValueT;
  gcc_jit_type *pTValueT;
  gcc_jit_type *pcTValueT;

  gcc_jit_field *Value_value;
  gcc_jit_field *Value_value_gc;
  gcc_jit_field *Value_value_n;
  gcc_jit_field *Value_value_i;
  gcc_jit_field *Value_value_b;
  gcc_jit_field *Value_tt;

  gcc_jit_struct *TStringT;
  gcc_jit_type *pTStringT;
  gcc_jit_type *ppTStringT;

  gcc_jit_struct *UdataT;
  gcc_jit_struct *RaviArrayT;
  gcc_jit_field *RaviArray_len;
  gcc_jit_field *RaviArray_data;
  gcc_jit_field *RaviArray_array_type;

  gcc_jit_struct *TableT;
  gcc_jit_type *pTableT;
  gcc_jit_type *ppTableT;
  gcc_jit_field *Table_ravi_array;

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

  gcc_jit_field *LClosure_p;
  gcc_jit_field *LClosure_p_k;
  gcc_jit_field *LClosure_upvals;

  gcc_jit_struct *RaviJITProtoT;

  gcc_jit_struct *ProtoT;
  gcc_jit_type *pProtoT;
  gcc_jit_type *ppProtoT;

  gcc_jit_field *Proto_sizep;

  gcc_jit_struct *UpValT;
  gcc_jit_type *pUpValT;
  gcc_jit_struct *UpVal_u_openT;
  gcc_jit_type *UpVal_uT;

  gcc_jit_field *UpVal_v;
  gcc_jit_field *UpVal_u;
  gcc_jit_field *UpVal_u_value;

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

  gcc_jit_field *CallInfo_func;
  gcc_jit_field *CallInfo_top;
  gcc_jit_field *CallInfo_u;
  gcc_jit_field *CallInfo_u_l;
  gcc_jit_field *CallInfo_u_l_base;

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
  gcc_jit_function *raviV_op_setupvalT;

  gcc_jit_function *raviH_set_intT;
  gcc_jit_function *raviH_set_floatT;

  gcc_jit_function *printfT;
};

struct ravi_gcc_context_t {

  /* parent JIT context - all functions are child contexts
   * of this.
   */
  gcc_jit_context *context;

  gcc_jit_result *parent_result_;

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

struct ravi_gcc_codegen_t {

  ravi_gcc_context_t *ravi;

  char temp[31];

  int id;
};

// struct ravi_gcc_function_t {
//
//  gcc_jit_result *jit_result;
//
//};

typedef struct ravi_branch_def_t {
  // this field is used for all branches
  gcc_jit_block *jmp;

  // These are local variables for a fornum
  // loop
  gcc_jit_lvalue *ilimit;
  gcc_jit_lvalue *istep;
  gcc_jit_lvalue *iidx;
  gcc_jit_lvalue *flimit;
  gcc_jit_lvalue *fstep;
  gcc_jit_lvalue *fidx;

} ravi_branch_def_t;

typedef struct ravi_function_def_t {

  ravi_gcc_context_t *ravi;

  Proto *p;

  char name[31];

  /* Child context for the function being compiled */
  gcc_jit_context *function_context;

  gcc_jit_param *L;

  gcc_jit_function *jit_function;

  gcc_jit_block *entry_block;

  struct ravi_branch_def_t **jmp_targets;

  /* Currently compiled function's stack frame L->ci */
  gcc_jit_lvalue *ci_val;

  /* Currently compiled function's stack value L->ci->func
   * type is LClosure *
   */
  gcc_jit_rvalue *lua_closure;
  gcc_jit_lvalue *lua_closure_val;

  gcc_jit_lvalue *kOne_luaInteger;

  gcc_jit_block *current_block;

  bool current_block_terminated;

  /* The Lua stack base - this can change during execution so needs to be lvalue
   */
  gcc_jit_rvalue *base_ref;
  //  gcc_jit_lvalue *base;

  /* Reference to lua_closure->p - never changes */
  gcc_jit_rvalue *proto;

  /* The Lua constants list for the function - this never changes */
  gcc_jit_rvalue *k;

  /* temporary buffer for creating unique names */
  char buf[80];

  /* counter used for creating unique names */
  unsigned int counter;

  int dump_ir;
  int dump_asm;
  int opt_level;

} ravi_function_def_t;

/* Create a new context */
extern ravi_gcc_context_t *ravi_jit_new_context(void);

/* Destroy a context */
extern void ravi_jit_context_free(ravi_gcc_context_t *);

/* Setup Lua types */
extern bool ravi_setup_lua_types(ravi_gcc_context_t *);

extern ravi_gcc_codegen_t *
ravi_jit_new_codegen(ravi_gcc_context_t *global_context);

extern void ravi_jit_codegen_free(ravi_gcc_codegen_t *);

extern bool ravi_jit_has_errored(ravi_gcc_context_t *);

extern void ravi_emit_load_base(ravi_function_def_t *def);

extern gcc_jit_lvalue *ravi_emit_get_register(ravi_function_def_t *def, int A);

extern gcc_jit_lvalue *ravi_emit_get_constant(ravi_function_def_t *def, int Bx);

extern gcc_jit_lvalue *
ravi_emit_get_register_or_constant(ravi_function_def_t *def, int B);

extern void ravi_emit_set_L_top_toreg(ravi_function_def_t *def, int B);

extern void ravi_emit_refresh_L_top(ravi_function_def_t *def);

extern gcc_jit_lvalue *ravi_emit_get_Proto_sizep(ravi_function_def_t *def);

extern gcc_jit_rvalue *ravi_emit_get_upvals(ravi_function_def_t *def,
                                            int offset);
// Get upval->v
extern gcc_jit_lvalue *ravi_emit_load_upval_v(ravi_function_def_t *def,
                                              gcc_jit_rvalue *pupval);

// Get upval->u.value
extern gcc_jit_lvalue *ravi_emit_load_upval_value(ravi_function_def_t *def,
                                                  gcc_jit_rvalue *pupval);

extern void ravi_set_current_block(ravi_function_def_t *def,
                                   gcc_jit_block *block);

extern gcc_jit_rvalue *ravi_function_call1_rvalue(ravi_function_def_t *def,
                                                  gcc_jit_function *f,
                                                  gcc_jit_rvalue *arg1);

extern gcc_jit_rvalue *ravi_function_call2_rvalue(ravi_function_def_t *def,
                                                  gcc_jit_function *f,
                                                  gcc_jit_rvalue *arg1,
                                                  gcc_jit_rvalue *arg2);

extern gcc_jit_rvalue *ravi_function_call3_rvalue(ravi_function_def_t *def,
                                                  gcc_jit_function *f,
                                                  gcc_jit_rvalue *arg1,
                                                  gcc_jit_rvalue *arg2,
                                                  gcc_jit_rvalue *arg3);

extern gcc_jit_rvalue *
ravi_function_call4_rvalue(ravi_function_def_t *def, gcc_jit_function *f,
                           gcc_jit_rvalue *arg1, gcc_jit_rvalue *arg2,
                           gcc_jit_rvalue *arg3, gcc_jit_rvalue *arg4);

extern gcc_jit_rvalue *
ravi_function_call5_rvalue(ravi_function_def_t *def, gcc_jit_function *f,
                           gcc_jit_rvalue *arg1, gcc_jit_rvalue *arg2,
                           gcc_jit_rvalue *arg3, gcc_jit_rvalue *arg4,
                           gcc_jit_rvalue *arg5);

extern const char *unique_name(ravi_function_def_t *def, const char *prefix,
                               int pc);

extern gcc_jit_rvalue *ravi_emit_num_stack_elements(ravi_function_def_t *def,
                                                    gcc_jit_rvalue *ra);

extern void ravi_emit_struct_assign(ravi_function_def_t *def,
                                    gcc_jit_lvalue *reg_dest,
                                    gcc_jit_lvalue *reg_src);

/* Store an integer value and set type to TNUMINT */
extern gcc_jit_lvalue *ravi_emit_load_reg_i(ravi_function_def_t *def,
                                            gcc_jit_lvalue *reg);

/* Store a number value and set type to TNUMFLT */
extern gcc_jit_lvalue *ravi_emit_load_reg_n(ravi_function_def_t *def,
                                            gcc_jit_lvalue *reg);

/* Get TValue->value_.b */
extern gcc_jit_lvalue *ravi_emit_load_reg_b(ravi_function_def_t *def,
                                            gcc_jit_lvalue *reg);

extern gcc_jit_lvalue *ravi_emit_load_type(ravi_function_def_t *def,
                                           gcc_jit_lvalue *reg);

extern gcc_jit_rvalue *ravi_emit_is_value_of_type(ravi_function_def_t *def,
                                                  gcc_jit_rvalue *value_type,
                                                  int lua_type);

extern gcc_jit_rvalue *
ravi_emit_is_not_value_of_type(ravi_function_def_t *def,
                               gcc_jit_rvalue *value_type, int lua_type);

extern gcc_jit_rvalue *
ravi_emit_is_not_value_of_type_class(ravi_function_def_t *def,
                               gcc_jit_rvalue *value_type, int lua_type);

extern void ravi_emit_store_reg_i_withtype(ravi_function_def_t *def,
                                           gcc_jit_rvalue *ivalue,
                                           gcc_jit_lvalue *reg);

extern void ravi_emit_store_reg_n_withtype(ravi_function_def_t *def,
                                           gcc_jit_rvalue *nvalue,
                                           gcc_jit_lvalue *reg);

extern void ravi_emit_store_reg_b_withtype(ravi_function_def_t *def,
                                           gcc_jit_rvalue *bvalue,
                                           gcc_jit_lvalue *reg);

extern gcc_jit_rvalue *ravi_emit_load_reg_h(ravi_function_def_t *def,
                                            gcc_jit_lvalue *reg);

extern gcc_jit_rvalue *ravi_emit_load_reg_h_floatarray(ravi_function_def_t *def,
                                                       gcc_jit_rvalue *h);

extern gcc_jit_rvalue *ravi_emit_load_reg_h_intarray(ravi_function_def_t *def,
                                                     gcc_jit_rvalue *h);

extern gcc_jit_lvalue *ravi_emit_load_ravi_arraylength(ravi_function_def_t *def,
                                                       gcc_jit_rvalue *h);

extern gcc_jit_lvalue *ravi_emit_load_ravi_arraytype(ravi_function_def_t *def,
                                                     gcc_jit_rvalue *h);

extern gcc_jit_rvalue *ravi_emit_array_get(ravi_function_def_t *def,
                                           gcc_jit_rvalue *ptr,
                                           gcc_jit_rvalue *index);

extern gcc_jit_lvalue *ravi_emit_array_get_ptr(ravi_function_def_t *def,
                                               gcc_jit_rvalue *ptr,
                                               gcc_jit_rvalue *index);

extern gcc_jit_rvalue *ravi_emit_comparison(ravi_function_def_t *def,
                                            enum gcc_jit_comparison op,
                                            gcc_jit_rvalue *a,
                                            gcc_jit_rvalue *b);

extern gcc_jit_lvalue *ravi_emit_tonumtype(ravi_function_def_t *def,
                                           gcc_jit_lvalue *reg,
                                           lua_typecode_t tt, int pc);

extern void ravi_emit_conditional_branch(ravi_function_def_t *def,
                                         gcc_jit_rvalue *cond,
                                         gcc_jit_block *true_block,
                                         gcc_jit_block *false_block);

extern void ravi_emit_branch(ravi_function_def_t *def,
                             gcc_jit_block *target_block);

extern gcc_jit_rvalue *ravi_emit_boolean_testfalse(ravi_function_def_t *def,
                                                   gcc_jit_lvalue *reg,
                                                   bool negate);

extern void ravi_emit_raise_lua_error(ravi_function_def_t *def,
                                      const char *msg);

extern void ravi_emit_RETURN(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_LOADK(ravi_function_def_t *def, int A, int Bx, int pc);

extern void ravi_emit_iFORPREP(ravi_function_def_t *def, int A, int pc,
                               int step_one);

extern void ravi_emit_iFORLOOP(ravi_function_def_t *def, int A, int pc,
                               ravi_branch_def_t *b, int step_one);

extern void ravi_emit_MOVE(ravi_function_def_t *def, int A, int B);

extern void ravi_emit_MOVEI(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_MOVEF(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_LOADNIL(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_LOADFZ(ravi_function_def_t *def, int A, int pc);

extern void ravi_emit_LOADIZ(ravi_function_def_t *def, int A, int pc);

extern void ravi_emit_LOADBOOL(ravi_function_def_t *def, int A, int B, int C,
                               int j, int pc);

// implements EQ, LE and LT - by using the supplied lua function to call.
extern void ravi_emit_EQ_LE_LT(ravi_function_def_t *def, int A, int B, int C,
                               int j, int jA, gcc_jit_function *callee,
                               const char *opname, OpCode op, int pc);

extern void ravi_emit_JMP(ravi_function_def_t *def, int A, int j, int pc);

// Handle OP_CALL
extern void ravi_emit_CALL(ravi_function_def_t *def, int A, int B, int C,
                           int pc);

extern void ravi_emit_GETTABUP(ravi_function_def_t *def, int A, int B, int C,
                               int pc);

extern void ravi_emit_SETTABUP(ravi_function_def_t *def, int A, int B, int C,
                               int pc);

extern void ravi_emit_SETUPVAL(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_GETUPVAL(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_TEST(ravi_function_def_t *def, int A, int B, int C, int j,
                           int jA, int pc);

extern void ravi_emit_TESTSET(ravi_function_def_t *def, int A, int B, int C,
                              int j, int jA, int pc);

extern void ravi_emit_NOT(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_TOFLT(ravi_function_def_t *def, int A, int pc);

extern void ravi_emit_TOINT(ravi_function_def_t *def, int A, int pc);

extern void ravi_emit_CONCAT(ravi_function_def_t *def, int A, int B, int C,
                             int pc);

extern void ravi_emit_CLOSURE(ravi_function_def_t *def, int A, int Bx, int pc);

extern void ravi_emit_VARARG(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_UNMF(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_UNMI(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_ADDFF(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_ADDFI(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_ADDII(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_SUBFF(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_SUBFI(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_SUBIF(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_SUBII(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_DIVFF(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_DIVFI(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_DIVIF(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_DIVII(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_MULFF(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_MULFI(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_MULII(ravi_function_def_t *def, int A, int B, int C,
                            int pc);

extern void ravi_emit_SELF(ravi_function_def_t *def, int A, int B, int C,
                           int pc);

extern void ravi_emit_LEN(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_SETTABLE(ravi_function_def_t *def, int A, int B, int C,
                               int pc);

extern void ravi_emit_GETTABLE(ravi_function_def_t *def, int A, int B, int C,
                               int pc);

extern void ravi_emit_NEWTABLE(ravi_function_def_t *def, int A, int B, int C,
                               int pc);

extern void ravi_emit_SETLIST(ravi_function_def_t *def, int A, int B, int C,
                              int pc);

extern void ravi_emit_TFORCALL(ravi_function_def_t *def, int A, int B, int C,
                               int j, int jA, int pc);

extern void ravi_emit_TFORLOOP(ravi_function_def_t *def, int A, int j, int pc);

extern void ravi_emit_GETTABLE_AI(ravi_function_def_t *def, int A, int B, int C,
                                  int pc, bool omitArrayGetRangeCheck);

extern void ravi_emit_GETTABLE_AF(ravi_function_def_t *def, int A, int B, int C,
                                  int pc, bool omitArrayGetRangeCheck);

extern void ravi_emit_NEWARRAYFLOAT(ravi_function_def_t *def, int A, int pc);

extern void ravi_emit_NEWARRAYINT(ravi_function_def_t *def, int A, int pc);

extern void ravi_emit_TOARRAY(ravi_function_def_t *def, int A,
                              int array_type_expected, const char *errmsg,
                              int pc);

extern void ravi_emit_MOVEAI(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_MOVEAF(ravi_function_def_t *def, int A, int B, int pc);

extern void ravi_emit_SETTABLE_AI_AF(ravi_function_def_t *def, int A, int B,
                                     int C, bool known_tt, lua_typecode_t tt,
                                     int pc);

extern void ravi_emit_ARITH(ravi_function_def_t *def, int A, int B, int C,
                            OpCode op, TMS tms, int pc);

extern void ravi_dump_rvalue(gcc_jit_rvalue *rv);

extern void ravi_dump_lvalue(gcc_jit_lvalue *lv);

extern void ravi_debug_printf(ravi_function_def_t *def, const char *str);

extern void ravi_debug_printf1(ravi_function_def_t *def, const char *str,
                               gcc_jit_rvalue *arg1);

extern void ravi_debug_printf2(ravi_function_def_t *def, const char *str,
                               gcc_jit_rvalue *arg1, gcc_jit_rvalue *arg2);

extern void ravi_debug_printf3(ravi_function_def_t *def, const char *str,
                               gcc_jit_rvalue *arg1, gcc_jit_rvalue *arg2,
                               gcc_jit_rvalue *arg3);

extern void ravi_debug_printf4(ravi_function_def_t *def, const char *str,
                               gcc_jit_rvalue *arg1, gcc_jit_rvalue *arg2,
                               gcc_jit_rvalue *arg3, gcc_jit_rvalue *arg4);

extern gcc_jit_rvalue *ravi_int_constant(ravi_function_def_t *def, int value);

extern gcc_jit_rvalue *ravi_bool_constant(ravi_function_def_t *def, int value);

extern gcc_jit_rvalue *ravi_lua_Integer_constant(ravi_function_def_t *def,
                                                 int value);

extern gcc_jit_rvalue *ravi_lua_Number_constant(ravi_function_def_t *def,
                                                double value);

#ifdef __cplusplus
};
#endif

#endif /* USE_GCCJIT */

#endif /* RAVI_RAVI_GCCJIT_H */

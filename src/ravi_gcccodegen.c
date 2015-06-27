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

#include <ravi_gccjit.h>

// Create a unique function name in the context
// of this generator
static const char *unique_function_name(ravi_function_def_t *def,
                                        ravi_gcc_codegen_t *cg) {
  snprintf(def->name, sizeof def->name, "ravif%d", cg->id++);
  return def->name;
}

const char *unique_name(ravi_function_def_t *def, const char *prefix, int pc) {
  snprintf(def->buf, sizeof def->buf, "%s_%d_%d", prefix, pc, def->counter++);
  return def->buf;
}

// We can only compile a subset of op codes
// and not all features are supported
static bool can_compile(Proto *p) {
  if (p->ravi_jit.jit_status == 1)
    return false;
  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  // Loop over the byte codes; as Lua compiler inserts
  // an extra RETURN op we need to ignore the last op
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode o = GET_OPCODE(i);
    switch (o) {
    case OP_RETURN:
    case OP_LOADK:
    case OP_RAVI_FORLOOP_IP:
    case OP_RAVI_FORLOOP_I1:
    case OP_RAVI_FORPREP_IP:
    case OP_RAVI_FORPREP_I1:
    case OP_MOVE:
    case OP_LOADNIL:
    case OP_RAVI_LOADIZ:
    case OP_RAVI_LOADFZ:
    case OP_CALL:
    case OP_TAILCALL:
    case OP_JMP:
    case OP_EQ:
    case OP_LT:
    case OP_LE:
    case OP_GETTABUP:
    case OP_LOADBOOL:
    case OP_NOT:
    case OP_TEST:
    case OP_TESTSET:
    case OP_RAVI_MOVEI:
    case OP_RAVI_MOVEF:
    case OP_RAVI_TOINT:
    case OP_RAVI_TOFLT:
    case OP_VARARG:
    case OP_CONCAT:
    case OP_CLOSURE:
    case OP_RAVI_ADDFN:
    case OP_RAVI_ADDFF:
    case OP_RAVI_ADDFI:
    case OP_RAVI_ADDII:
    case OP_RAVI_ADDIN:
      break;
    case OP_LOADKX:
    case OP_FORPREP:
    case OP_FORLOOP:
    case OP_TFORCALL:
    case OP_TFORLOOP:
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_MOD:
    case OP_IDIV:
    case OP_UNM:
    case OP_POW:
    case OP_LEN:
    case OP_SETTABLE:
    case OP_GETTABLE:
    case OP_GETUPVAL:
    case OP_SETUPVAL:
    case OP_SETTABUP:
    case OP_NEWTABLE:
    case OP_SETLIST:
    case OP_SELF:
    case OP_RAVI_NEWARRAYI:
    case OP_RAVI_NEWARRAYF:
    case OP_RAVI_SUBFF:
    case OP_RAVI_SUBFI:
    case OP_RAVI_SUBIF:
    case OP_RAVI_SUBII:
    case OP_RAVI_SUBFN:
    case OP_RAVI_SUBNF:
    case OP_RAVI_SUBIN:
    case OP_RAVI_SUBNI:
    case OP_RAVI_MULFN:
    case OP_RAVI_MULIN:
    case OP_RAVI_MULFF:
    case OP_RAVI_MULFI:
    case OP_RAVI_MULII:
    case OP_RAVI_DIVFF:
    case OP_RAVI_DIVFI:
    case OP_RAVI_DIVIF:
    case OP_RAVI_DIVII:
    case OP_RAVI_GETTABLE_AI:
    case OP_RAVI_GETTABLE_AF:
    case OP_RAVI_SETTABLE_AI:
    case OP_RAVI_SETTABLE_AII:
    case OP_RAVI_SETTABLE_AF:
    case OP_RAVI_SETTABLE_AFF:
    case OP_RAVI_TOARRAYI:
    case OP_RAVI_TOARRAYF:
    case OP_RAVI_MOVEAI:
    case OP_RAVI_MOVEAF:
    default: {
      p->ravi_jit.jit_status = 1;
      return false;
    }
    }
  }
  return true;
}

static bool create_function(ravi_gcc_codegen_t *codegen,
                            ravi_function_def_t *def) {

  /* The child context will hold function specific stuff - so that we can
   * release
   * this when we have compiled the function
   */
  def->function_context =
      gcc_jit_context_new_child_context(codegen->ravi->context);
  if (!def->function_context) {
    fprintf(stderr, "error creating child context\n");
    goto on_error;
  }
  // gcc_jit_context_set_bool_option(def->function_context,
  //                                GCC_JIT_BOOL_OPTION_DUMP_EVERYTHING, 1);
  // gcc_jit_context_set_bool_option(def->function_context,
  //                                GCC_JIT_BOOL_OPTION_KEEP_INTERMEDIATES, 1);
  if (def->dump_asm) {
    gcc_jit_context_set_bool_option(def->function_context,
                                    GCC_JIT_BOOL_OPTION_DUMP_GENERATED_CODE, 1);
  }
  gcc_jit_context_set_int_option(def->function_context,
                                 GCC_JIT_INT_OPTION_OPTIMIZATION_LEVEL,
                                 def->opt_level);

  /* each function is given a unique name - as Lua functions are closures and do
   * not really have names */
  unique_function_name(def, codegen);

  /* the function signature is int (*) (lua_State *) */
  gcc_jit_param *param = gcc_jit_context_new_param(
      def->function_context, NULL, codegen->ravi->types->plua_StateT, "L");
  def->L = param; /* store the L parameter as we will need it */
  def->jit_function = gcc_jit_context_new_function(
      def->function_context, NULL, GCC_JIT_FUNCTION_EXPORTED,
      codegen->ravi->types->C_intT, def->name, 1, &param, 0);

  /* The entry block of the function */
  def->entry_block = gcc_jit_function_new_block(def->jit_function, "entry");
  def->current_block = def->entry_block;

  def->base = gcc_jit_function_new_local(def->jit_function, NULL,
                                         def->ravi->types->pTValueT, "base");
  def->lua_closure_val = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->pLClosureT, "cl");

  return true;

on_error:
  return false;
}

/* release the resources allocated when compiling a function
 * note that the compiled function is not released here */
static void free_function_def(ravi_function_def_t *def) {
  if (def->function_context)
    gcc_jit_context_release(def->function_context);
  if (def->jmp_targets) {
    for (int i = 0; i < def->p->sizecode; i++) {
      if (def->jmp_targets[i])
        free(def->jmp_targets[i]);
    }
    free(def->jmp_targets);
  }
}

#define RA(i) (base + GETARG_A(i))
/* to be used after possible stack reallocation */
#define RB(i) check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i))
#define RC(i) check_exp(getCMode(GET_OPCODE(i)) == OpArgR, base + GETARG_C(i))
#define RKB(i)                                                                 \
  check_exp(getBMode(GET_OPCODE(i)) == OpArgK,                                 \
            ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i))
#define RKC(i)                                                                 \
  check_exp(getCMode(GET_OPCODE(i)) == OpArgK,                                 \
            ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i))
#define KBx(i)                                                                 \
  (k + (GETARG_Bx(i) != 0 ? GETARG_Bx(i) - 1 : GETARG_Ax(*ci->u.l.savedpc++)))
/* RAVI */
#define KB(i)                                                                  \
  check_exp(getBMode(GET_OPCODE(i)) == OpArgK, k + INDEXK(GETARG_B(i)))
#define KC(i)                                                                  \
  check_exp(getCMode(GET_OPCODE(i)) == OpArgK, k + INDEXK(GETARG_C(i)))

/* Scan the Lua bytecode to identify the jump targets and pre-create
 * basic blocks for each target. The blocks are saved in an array
 * def->jmp_targets
 * which is indexed by the byte code offset 0 .. p->sizecode-1.
 */
static void scan_jump_targets(ravi_function_def_t *def, Proto *p) {
  // We need to pre-create blocks for jump targets so that we
  // can generate branch instructions in the code
  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  def->jmp_targets =
      (ravi_branch_def_t **)calloc(n, sizeof(ravi_branch_def_t *));
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    switch (op) {
    case OP_LOADBOOL: {
      int C = GETARG_C(i);
      int j = pc + 2; // jump target
      if (C && !def->jmp_targets[j]) {
        def->jmp_targets[j] =
            (ravi_branch_def_t *)calloc(1, sizeof(ravi_branch_def_t));
        def->jmp_targets[j]->jmp = gcc_jit_function_new_block(
            def->jit_function, unique_name(def, "loadbool", j));
      }
    } break;
    case OP_JMP:
    case OP_RAVI_FORPREP_IP:
    case OP_RAVI_FORPREP_I1:
    case OP_RAVI_FORLOOP_IP:
    case OP_RAVI_FORLOOP_I1:
    case OP_FORLOOP:
    case OP_FORPREP:
    case OP_TFORLOOP: {
      const char *targetname = NULL;
      if (op == OP_JMP)
        targetname = "jmp";
      else if (op == OP_FORLOOP || op == OP_RAVI_FORLOOP_IP ||
               op == OP_RAVI_FORLOOP_I1)
        targetname = "forbody";
      else if (op == OP_FORPREP || op == OP_RAVI_FORPREP_IP ||
               op == OP_RAVI_FORPREP_I1)
        targetname = "forloop";
      else
        targetname = "tforbody";
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      if (!def->jmp_targets[j]) {
        def->jmp_targets[j] =
            (ravi_branch_def_t *)calloc(1, sizeof(ravi_branch_def_t));
        def->jmp_targets[j]->jmp = gcc_jit_function_new_block(
            def->jit_function, unique_name(def, targetname, j + 1));
      }
    } break;
    default:
      break;
    }
  }
}

void ravi_emit_raise_lua_error(ravi_function_def_t *def, const char *str) {
  ravi_function_call2_rvalue(
      def, def->ravi->types->luaG_runerrorT, gcc_jit_param_as_rvalue(def->L),
      gcc_jit_context_new_string_literal(def->function_context, str));
}

void ravi_emit_struct_assign(ravi_function_def_t *def, gcc_jit_rvalue *dest,
                             gcc_jit_rvalue *src) {
  //  gcc_jit_block_add_assignment(def->current_block, NULL,
  //                               gcc_jit_rvalue_dereference(dest, NULL),
  //                               gcc_jit_lvalue_as_rvalue(gcc_jit_rvalue_dereference(src,
  //                               NULL)));
  gcc_jit_lvalue *dest_value = gcc_jit_rvalue_dereference_field(
      dest, NULL, def->ravi->types->Value_value);
  gcc_jit_lvalue *dest_value_i = gcc_jit_lvalue_access_field(
      dest_value, NULL, def->ravi->types->Value_value_i);

  gcc_jit_lvalue *src_value = gcc_jit_rvalue_dereference_field(
      src, NULL, def->ravi->types->Value_value);
  gcc_jit_lvalue *src_value_i = gcc_jit_lvalue_access_field(
      src_value, NULL, def->ravi->types->Value_value_i);

  gcc_jit_block_add_assignment(def->current_block, NULL, dest_value_i,
                               gcc_jit_lvalue_as_rvalue(src_value_i));

  gcc_jit_lvalue *dest_tt =
      gcc_jit_rvalue_dereference_field(dest, NULL, def->ravi->types->Value_tt);
  gcc_jit_lvalue *src_tt =
      gcc_jit_rvalue_dereference_field(src, NULL, def->ravi->types->Value_tt);

  gcc_jit_block_add_assignment(def->current_block, NULL, dest_tt,
                               gcc_jit_lvalue_as_rvalue(src_tt));
}

void ravi_dump_rvalue(gcc_jit_rvalue *rv) {
  const char *debugstr =
      gcc_jit_object_get_debug_string(gcc_jit_rvalue_as_object(rv));
  fprintf(stderr, "%s\n", debugstr);
}

void ravi_dump_lvalue(gcc_jit_lvalue *lv) {
  const char *debugstr =
      gcc_jit_object_get_debug_string(gcc_jit_lvalue_as_object(lv));
  fprintf(stderr, "%s\n", debugstr);
}

/* Obtain reference to currently executing function (LClosure*)
 * L->ci->func.value_.gc */
static void emit_ci_func_value_gc_asLClosure(ravi_function_def_t *def,
                                             gcc_jit_lvalue *ci) {
  gcc_jit_lvalue *func = gcc_jit_rvalue_dereference_field(
      gcc_jit_lvalue_as_rvalue(ci), NULL, def->ravi->types->CallInfo_func);
  gcc_jit_lvalue *value = gcc_jit_rvalue_dereference_field(
      gcc_jit_lvalue_as_rvalue(func), NULL, def->ravi->types->Value_value);
  gcc_jit_lvalue *gc = gcc_jit_lvalue_access_field(
      value, NULL, def->ravi->types->Value_value_gc);

  def->lua_closure = gcc_jit_context_new_cast(def->function_context, NULL,
                                              gcc_jit_lvalue_as_rvalue(gc),
                                              def->ravi->types->pLClosureT);
  gcc_jit_block_add_assignment(def->current_block, NULL, def->lua_closure_val,
                               def->lua_closure);
}

/* Obtain reference to L->ci */
static void emit_getL_ci_value(ravi_function_def_t *def) {
  def->ci_val = gcc_jit_rvalue_dereference_field(
      gcc_jit_param_as_rvalue(def->L), NULL, def->ravi->types->lua_State_ci);
}

/* Refresh local copy of L->ci->u.l.base */
void ravi_emit_load_base(ravi_function_def_t *def) {
  gcc_jit_block_add_assignment(def->current_block, NULL, def->base,
                               def->base_ref);
}

// L->top = ci->top
void ravi_emit_refresh_L_top(ravi_function_def_t *def) {
  // Load ci->top
  gcc_jit_lvalue *citop =
      gcc_jit_rvalue_dereference_field(gcc_jit_lvalue_as_rvalue(def->ci_val),
                                       NULL, def->ravi->types->CallInfo_top);

  // Get L->top
  gcc_jit_lvalue *top = gcc_jit_rvalue_dereference_field(
      gcc_jit_param_as_rvalue(def->L), NULL, def->ravi->types->lua_State_top);

  // Assign ci>top to L->top
  gcc_jit_block_add_assignment(def->current_block, NULL, top,
                               gcc_jit_lvalue_as_rvalue(citop));
}

/* Get access to the register identified by A - registers as just &base[offset]
 */
gcc_jit_rvalue *ravi_emit_get_register(ravi_function_def_t *def, int A) {
  /* Note we assume that base is correct */
  gcc_jit_lvalue *reg = gcc_jit_context_new_array_access(
      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(def->base),
      gcc_jit_context_new_rvalue_from_int(def->function_context,
                                          def->ravi->types->C_intT, A));
  return gcc_jit_lvalue_get_address(reg, NULL);
}

/* Get access to a constant identify by Bx */
gcc_jit_rvalue *ravi_emit_get_constant(ravi_function_def_t *def, int Bx) {
  gcc_jit_lvalue *kst = gcc_jit_context_new_array_access(
      def->function_context, NULL, def->k,
      gcc_jit_context_new_rvalue_from_int(def->function_context,
                                          def->ravi->types->C_intT, Bx));
  return gcc_jit_lvalue_get_address(kst, NULL);
}

gcc_jit_rvalue *ravi_emit_get_register_or_constant(ravi_function_def_t *def,
                                                   int B) {
  if (ISK(B))
    return ravi_emit_get_constant(def, INDEXK(B));
  else
    return ravi_emit_get_register(def, B);
}

// L->top = R(B)
void ravi_emit_set_L_top_toreg(ravi_function_def_t *def, int B) {
  // Get pointer to register at R(B)
  gcc_jit_rvalue *reg = ravi_emit_get_register(def, B);
  // Get pointer to L->top
  gcc_jit_lvalue *top = gcc_jit_rvalue_dereference_field(
      gcc_jit_param_as_rvalue(def->L), NULL, def->ravi->types->lua_State_top);
  // L->top = R(B)
  gcc_jit_block_add_assignment(def->current_block, NULL, top, reg);
}

/* Obtain reference to L->ci->u.l.base */
static void emit_getL_base_reference(ravi_function_def_t *def,
                                     gcc_jit_lvalue *ci) {
  gcc_jit_lvalue *u = gcc_jit_rvalue_dereference_field(
      gcc_jit_lvalue_as_rvalue(ci), NULL, def->ravi->types->CallInfo_u);
  gcc_jit_rvalue *u_l = gcc_jit_rvalue_access_field(
      gcc_jit_lvalue_as_rvalue(u), NULL, def->ravi->types->CallInfo_u_l);
  gcc_jit_rvalue *u_l_base = gcc_jit_rvalue_access_field(
      u_l, NULL, def->ravi->types->CallInfo_u_l_base);
  def->base_ref = u_l_base;
  ravi_emit_load_base(def);
}

/* Get TValue->value_.i */
gcc_jit_lvalue *ravi_emit_load_reg_i(ravi_function_def_t *def,
                                     gcc_jit_rvalue *tv) {
  gcc_jit_lvalue *value =
      gcc_jit_rvalue_dereference_field(tv, NULL, def->ravi->types->Value_value);
  gcc_jit_lvalue *i =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_i);
  return i;
}

/* Get TValue->value_.b */
gcc_jit_lvalue *ravi_emit_load_reg_b(ravi_function_def_t *def,
                                     gcc_jit_rvalue *tv) {
  gcc_jit_lvalue *value =
      gcc_jit_rvalue_dereference_field(tv, NULL, def->ravi->types->Value_value);
  gcc_jit_lvalue *i =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_b);
  return i;
}

/* Get TValue->value_.n */
gcc_jit_lvalue *ravi_emit_load_reg_n(ravi_function_def_t *def,
                                     gcc_jit_rvalue *tv) {
  gcc_jit_lvalue *value =
      gcc_jit_rvalue_dereference_field(tv, NULL, def->ravi->types->Value_value);
  gcc_jit_lvalue *n =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_n);
  return n;
}

gcc_jit_lvalue *ravi_emit_load_type(ravi_function_def_t *def,
                                    gcc_jit_rvalue *tv) {
  return gcc_jit_rvalue_dereference_field(tv, NULL, def->ravi->types->Value_tt);
}

gcc_jit_rvalue *ravi_emit_comparison(ravi_function_def_t *def,
                                     enum gcc_jit_comparison op,
                                     gcc_jit_rvalue *a, gcc_jit_rvalue *b) {
  gcc_jit_lvalue *tmp_var = gcc_jit_function_new_local(
      def->jit_function, NULL, def->ravi->types->C_boolT,
      unique_name(def, "comparison", 0));
  gcc_jit_rvalue *cmp =
      gcc_jit_context_new_comparison(def->function_context, NULL, op, a, b);
  gcc_jit_block_add_assignment(def->current_block, NULL, tmp_var, cmp);
  return gcc_jit_lvalue_as_rvalue(tmp_var);
}

gcc_jit_rvalue *ravi_emit_is_value_of_type(ravi_function_def_t *def,
                                           gcc_jit_rvalue *value_type,
                                           int lua_type) {
#if RAVI_NAN_TAGGING
#error NaN tagging not supported
#else
  return ravi_emit_comparison(
      def, GCC_JIT_COMPARISON_EQ, value_type,
      gcc_jit_context_new_rvalue_from_int(def->function_context,
                                          def->ravi->types->C_intT, lua_type));
#endif
}

gcc_jit_rvalue *ravi_emit_is_not_value_of_type(ravi_function_def_t *def,
                                               gcc_jit_rvalue *value_type,
                                               int lua_type) {
#if RAVI_NAN_TAGGING
#error NaN tagging not supported
#else
  return gcc_jit_context_new_unary_op(
      def->function_context, NULL, GCC_JIT_UNARY_OP_LOGICAL_NEGATE,
      def->ravi->types->C_boolT,
      ravi_emit_is_value_of_type(def, value_type, lua_type));
#endif
}

/* Store an integer value and set type to TNUMINT */
void ravi_emit_store_reg_i_withtype(ravi_function_def_t *def,
                                    gcc_jit_rvalue *ivalue,
                                    gcc_jit_rvalue *reg) {
  gcc_jit_lvalue *value = gcc_jit_rvalue_dereference_field(
      reg, NULL, def->ravi->types->Value_value);
  gcc_jit_lvalue *i =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_i);
  gcc_jit_block_add_assignment(def->current_block, NULL, i, ivalue);
  gcc_jit_rvalue *type = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, LUA_TNUMINT);
  gcc_jit_lvalue *tt =
      gcc_jit_rvalue_dereference_field(reg, NULL, def->ravi->types->Value_tt);
  gcc_jit_block_add_assignment(def->current_block, NULL, tt, type);
}

/* Store a boolean value and set type to TBOOLEAN */
void ravi_emit_store_reg_b_withtype(ravi_function_def_t *def,
                                    gcc_jit_rvalue *bvalue,
                                    gcc_jit_rvalue *reg) {
  gcc_jit_lvalue *value = gcc_jit_rvalue_dereference_field(
      reg, NULL, def->ravi->types->Value_value);
  gcc_jit_lvalue *n =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_b);
  gcc_jit_block_add_assignment(def->current_block, NULL, n, bvalue);
  gcc_jit_rvalue *type = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, LUA__TBOOLEAN);
  gcc_jit_lvalue *tt =
      gcc_jit_rvalue_dereference_field(reg, NULL, def->ravi->types->Value_tt);
  gcc_jit_block_add_assignment(def->current_block, NULL, tt, type);
}

/* Store a number value and set type to TNUMFLT */
void ravi_emit_store_reg_n_withtype(ravi_function_def_t *def,
                                    gcc_jit_rvalue *nvalue,
                                    gcc_jit_rvalue *reg) {
  gcc_jit_lvalue *value = gcc_jit_rvalue_dereference_field(
      reg, NULL, def->ravi->types->Value_value);
  gcc_jit_lvalue *n =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_n);
  gcc_jit_block_add_assignment(def->current_block, NULL, n, nvalue);
  gcc_jit_rvalue *type = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, LUA_TNUMFLT);
  gcc_jit_lvalue *tt =
      gcc_jit_rvalue_dereference_field(reg, NULL, def->ravi->types->Value_tt);
  gcc_jit_block_add_assignment(def->current_block, NULL, tt, type);
}

/* Get the Lua function prototype and constants table */
static void emit_get_proto_and_k(ravi_function_def_t *def) {
  gcc_jit_lvalue *proto = gcc_jit_rvalue_dereference_field(
      gcc_jit_lvalue_as_rvalue(def->lua_closure_val), NULL,
      def->ravi->types->LClosure_p);
  def->proto = gcc_jit_lvalue_as_rvalue(proto);
  gcc_jit_lvalue *k = gcc_jit_rvalue_dereference_field(
      def->proto, NULL, def->ravi->types->LClosure_p_k);
  def->k = gcc_jit_lvalue_as_rvalue(k);
}

gcc_jit_lvalue *ravi_emit_get_Proto_sizep(ravi_function_def_t *def) {
  gcc_jit_lvalue *psize = gcc_jit_rvalue_dereference_field(
      def->proto, NULL, def->ravi->types->Proto_sizep);
  return psize;
}

static void link_block(ravi_function_def_t *def, int pc) {
  // If the current bytecode offset pc is on a jump target
  // then we need to insert the block we previously created in
  // scan_jump_targets()
  // and make it the current insert block; also if the previous block
  // is unterminated then we simply provide a branch from previous block to the
  // new block
  if (def->jmp_targets[pc]) {
    assert(def->jmp_targets[pc]->jmp);
    // We are on a jump target
    // Get the block we previously created scan_jump_targets
    gcc_jit_block *block = def->jmp_targets[pc]->jmp;
    if (!def->current_block_terminated) {
      // Previous block not terminated so branch to the
      // new block
      gcc_jit_block_end_with_jump(def->current_block, NULL, block);
    }
    // Now add the new block and make it current
    def->current_block = block;
    def->current_block_terminated = false;
  }
}

gcc_jit_rvalue *
ravi_function_call5_rvalue(ravi_function_def_t *def, gcc_jit_function *f,
                           gcc_jit_rvalue *arg1, gcc_jit_rvalue *arg2,
                           gcc_jit_rvalue *arg3, gcc_jit_rvalue *arg4,
                           gcc_jit_rvalue *arg5) {
  gcc_jit_rvalue *args[5];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  args[3] = arg4;
  args[4] = arg5;
  return gcc_jit_context_new_call(def->function_context, NULL, f, 5, args);
}

gcc_jit_rvalue *
ravi_function_call4_rvalue(ravi_function_def_t *def, gcc_jit_function *f,
                           gcc_jit_rvalue *arg1, gcc_jit_rvalue *arg2,
                           gcc_jit_rvalue *arg3, gcc_jit_rvalue *arg4) {
  gcc_jit_rvalue *args[4];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  args[3] = arg4;
  return gcc_jit_context_new_call(def->function_context, NULL, f, 4, args);
}

gcc_jit_rvalue *ravi_function_call3_rvalue(ravi_function_def_t *def,
                                           gcc_jit_function *f,
                                           gcc_jit_rvalue *arg1,
                                           gcc_jit_rvalue *arg2,
                                           gcc_jit_rvalue *arg3) {
  gcc_jit_rvalue *args[3];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  return gcc_jit_context_new_call(def->function_context, NULL, f, 3, args);
}

gcc_jit_rvalue *ravi_function_call2_rvalue(ravi_function_def_t *def,
                                           gcc_jit_function *f,
                                           gcc_jit_rvalue *arg1,
                                           gcc_jit_rvalue *arg2) {
  gcc_jit_rvalue *args[2];
  args[0] = arg1;
  args[1] = arg2;
  return gcc_jit_context_new_call(def->function_context, NULL, f, 2, args);
}

gcc_jit_rvalue *ravi_function_call1_rvalue(ravi_function_def_t *def,
                                           gcc_jit_function *f,
                                           gcc_jit_rvalue *arg1) {
  gcc_jit_rvalue *args[1];
  args[0] = arg1;
  return gcc_jit_context_new_call(def->function_context, NULL, f, 1, args);
}

gcc_jit_rvalue *ravi_emit_get_upvals(ravi_function_def_t *def, int offset) {
  gcc_jit_lvalue *upvals = gcc_jit_rvalue_dereference_field(
      gcc_jit_lvalue_as_rvalue(def->lua_closure_val), NULL,
      def->ravi->types->LClosure_upvals);
  gcc_jit_lvalue *upval = gcc_jit_context_new_array_access(
      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(upvals),
      gcc_jit_context_new_rvalue_from_int(def->function_context,
                                          def->ravi->types->C_intT, offset));
  return gcc_jit_lvalue_as_rvalue(upval);
}

// Get upval->v
gcc_jit_lvalue *ravi_emit_get_upval_v(ravi_function_def_t *def,
                                      gcc_jit_rvalue *pupval) {
  //   const char *debugstr =
  //   gcc_jit_object_get_debug_string(gcc_jit_rvalue_as_object(pupval));
  //   fprintf(stderr, "%s\n", debugstr);

  return gcc_jit_rvalue_dereference_field(pupval, NULL,
                                          def->ravi->types->UpVal_v);
}

// Get upval->u.value
gcc_jit_lvalue *ravi_emit_get_upval_value(ravi_function_def_t *def,
                                          gcc_jit_rvalue *pupval) {
  gcc_jit_lvalue *u =
      gcc_jit_rvalue_dereference_field(pupval, NULL, def->ravi->types->UpVal_u);
  return gcc_jit_lvalue_access_field(u, NULL, def->ravi->types->UpVal_u_value);
}

void ravi_set_current_block(ravi_function_def_t *def, gcc_jit_block *block) {
  def->current_block = block;
  def->current_block_terminated = false;
}

void ravi_emit_branch(ravi_function_def_t *def, gcc_jit_block *target_block) {
  assert(!def->current_block_terminated);
  gcc_jit_block_end_with_jump(def->current_block, NULL, target_block);
  def->current_block_terminated = true;
}

void ravi_emit_conditional_branch(ravi_function_def_t *def,
                                  gcc_jit_rvalue *cond,
                                  gcc_jit_block *true_block,
                                  gcc_jit_block *false_block) {
  assert(!def->current_block_terminated);
  gcc_jit_block_end_with_conditional(def->current_block, NULL, cond, true_block,
                                     false_block);
  def->current_block_terminated = true;
}

static void init_def(ravi_function_def_t *def, ravi_gcc_context_t *ravi,
                     Proto *p) {
  def->ravi = ravi;
  def->entry_block = NULL;
  def->function_context = NULL;
  def->jit_function = NULL;
  def->jmp_targets = NULL;
  def->ci_val = NULL;
  def->lua_closure = NULL;
  def->current_block = NULL;
  def->proto = NULL;
  def->k = NULL;
  def->base = NULL;
  def->current_block_terminated = false;
  def->buf[0] = 0;
  def->counter = 1;
  def->name[0] = 0;
  def->p = p;
  def->dump_ir = 0;
  def->dump_asm = 0;
}

// Compile a Lua function
// If JIT is turned off then compilation is skipped
// Compilation occurs if either auto compilation is ON (subject to some
// thresholds)
// or if a manual compilation request was made
// Returns true if compilation was successful
int raviV_compile(struct lua_State *L, struct Proto *p, int manual_request,
                  int dump) {
  // Compile given function if possible
  // The p->ravi_jit structure will be updated
  // Note that if a function fails to compile then
  // a flag is set so that it doesn't get compiled again

  if (p->ravi_jit.jit_status == 2)
    return true;
  global_State *G = G(L);
  if (G->ravi_state == NULL)
    return 0;
  if (!G->ravi_state->jit->enabled_) {
    return 0;
  }
  bool doCompile = (bool)manual_request;
  if (!doCompile && G->ravi_state->jit->auto_) {
    if (p->ravi_jit.jit_flags != 0) /* function has fornum loop, so compile */
      doCompile = true;
    else if (p->sizecode >
             G->ravi_state->jit
                 ->min_code_size_) /* function is long so compile */
      doCompile = true;
    else {
      if (p->ravi_jit.execution_count <
          G->ravi_state->jit
              ->min_exec_count_) /* function has been executed many
                                   times so compile */
        p->ravi_jit.execution_count++;
      else
        doCompile = true;
    }
  }
  if (!doCompile)
    return 0;

  int status = 0;

  if (!can_compile(p))
    return 0;

  ravi_State *ravi_state = (ravi_State *)G->ravi_state;
  ravi_gcc_codegen_t *codegen = ravi_state->code_generator;

  ravi_function_def_t def;
  init_def(&def, ravi_state->jit, p);
  def.dump_ir = def.dump_asm = dump;
  def.opt_level = G->ravi_state->jit->opt_level_;

  if (!create_function(codegen, &def)) {
    p->ravi_jit.jit_status = 1; // can't compile
    goto on_error;
  }

  scan_jump_targets(&def, p);

  /* Get L->ci */
  emit_getL_ci_value(&def);

  /* Get L->ci->func as LClosure* */
  emit_ci_func_value_gc_asLClosure(&def, def.ci_val);

  /* Get L->ci->u.l.base */
  emit_getL_base_reference(&def, def.ci_val);

  /* get Lclosure->p and p->k */
  emit_get_proto_and_k(&def);

  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  for (pc = 0; pc < n; pc++) {
    link_block(&def, pc);
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    int A = GETARG_A(i);
    switch (op) {
    case OP_CONCAT: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_CONCAT(&def, A, B, C, pc);
    } break;
    case OP_CLOSURE: {
      int Bx = GETARG_Bx(i);
      ravi_emit_CLOSURE(&def, A, Bx, pc);
    } break;
    case OP_VARARG: {
      int B = GETARG_B(i);
      ravi_emit_VARARG(&def, A, B, pc);
    } break;
    case OP_RAVI_MOVEI: {
      int B = GETARG_B(i);
      ravi_emit_MOVEI(&def, A, B, pc);
    } break;
    case OP_RAVI_MOVEF: {
      int B = GETARG_B(i);
      ravi_emit_MOVEF(&def, A, B, pc);
    } break;
    case OP_RAVI_TOINT: {
      ravi_emit_TOINT(&def, A, pc);
    } break;
    case OP_RAVI_TOFLT: {
      ravi_emit_TOFLT(&def, A, pc);
    } break;
    case OP_RETURN: {
      int B = GETARG_B(i);
      ravi_emit_RETURN(&def, A, B, pc);
    } break;
    case OP_LOADK: {
      int Bx = GETARG_Bx(i);
      ravi_emit_LOADK(&def, A, Bx, pc);
    } break;
    case OP_RAVI_FORPREP_I1:
    case OP_RAVI_FORPREP_IP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      ravi_emit_iFORPREP(&def, A, j, op == OP_RAVI_FORPREP_I1);
    } break;
    case OP_RAVI_FORLOOP_I1:
    case OP_RAVI_FORLOOP_IP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      ravi_emit_iFORLOOP(&def, A, j, def.jmp_targets[pc],
                         op == OP_RAVI_FORLOOP_I1);
    } break;
    case OP_MOVE: {
      int B = GETARG_B(i);
      ravi_emit_MOVE(&def, A, B);
    } break;
    case OP_LOADNIL: {
      int B = GETARG_B(i);
      ravi_emit_LOADNIL(&def, A, B, pc);
    } break;
    case OP_RAVI_LOADFZ: {
      ravi_emit_LOADFZ(&def, A, pc);
    } break;
    case OP_RAVI_LOADIZ: {
      ravi_emit_LOADIZ(&def, A, pc);
    } break;
    case OP_LOADBOOL: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_LOADBOOL(&def, A, B, C, pc + 2, pc);
    } break;
    case OP_RAVI_ADDFN: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_ADDFN(&def, A, B, C, pc);
    } break;
    case OP_RAVI_ADDIN: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_ADDIN(&def, A, B, C, pc);
    } break;
    case OP_JMP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      ravi_emit_JMP(&def, A, j, pc);
    } break;

    case OP_TAILCALL:
    case OP_CALL: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_CALL(&def, A, B, C, pc);
    } break;

    case OP_LT:
    case OP_LE:
    case OP_EQ: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      const char *opname =
          (op == OP_EQ ? "OP_EQ" : (op == OP_LT ? "OP_LT" : "OP_LE"));
      gcc_jit_function *comparison_function =
          (op == OP_EQ ? def.ravi->types->luaV_equalobjT
                       : (op == OP_LT ? def.ravi->types->luaV_lessthanT
                                      : def.ravi->types->luaV_lessequalT));
      // OP_EQ is followed by OP_JMP - we process this
      // along with OP_EQ
      pc++;
      i = code[pc];
      op = GET_OPCODE(i);
      lua_assert(op == OP_JMP);
      int sbx = GETARG_sBx(i);
      // j below is the jump target
      int j = sbx + pc + 1;
      ravi_emit_EQ_LE_LT(&def, A, B, C, j, GETARG_A(i), comparison_function,
                         opname, pc);
    } break;

    case OP_GETTABUP: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_GETTABUP(&def, A, B, C, pc);
    } break;

    case OP_NOT: {
      int B = GETARG_B(i);
      ravi_emit_NOT(&def, A, B, pc);
    } break;
    case OP_TEST: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      // OP_TEST is followed by OP_JMP - we process this
      // along with OP_EQ
      pc++;
      i = code[pc];
      op = GET_OPCODE(i);
      lua_assert(op == OP_JMP);
      int sbx = GETARG_sBx(i);
      // j below is the jump target
      int j = sbx + pc + 1;
      ravi_emit_TEST(&def, A, B, C, j, GETARG_A(i), pc - 1);
    } break;
    case OP_TESTSET: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      // OP_TESTSET is followed by OP_JMP - we process this
      // along with OP_EQ
      pc++;
      i = code[pc];
      op = GET_OPCODE(i);
      lua_assert(op == OP_JMP);
      int sbx = GETARG_sBx(i);
      // j below is the jump target
      int j = sbx + pc + 1;
      ravi_emit_TESTSET(&def, A, B, C, j, GETARG_A(i), pc - 1);
    } break;

    case OP_RAVI_ADDFF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_ADDFF(&def, A, B, C, pc);
    } break;
    case OP_RAVI_ADDFI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_ADDFI(&def, A, B, C, pc);
    } break;
    case OP_RAVI_ADDII: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_ADDII(&def, A, B, C, pc);
    } break;

    default:
      break;
    }
  }

  if (def.dump_ir) {
    gcc_jit_context_dump_to_file(def.function_context, "fdump.txt", 0);
  }
  // gcc_jit_context_dump_reproducer_to_file(def.function_context, "rdump.txt");
  // gcc_jit_context_set_logfile (def.function_context, stderr, 0, 0);

  if (gcc_jit_context_get_first_error(def.function_context)) {
    fprintf(stderr, "aborting due to JIT error: %s\n",
            gcc_jit_context_get_first_error(def.function_context));
    abort();
  }
  gcc_jit_result *compilation_result =
      gcc_jit_context_compile(def.function_context);
  if (gcc_jit_context_get_first_error(def.function_context)) {
    fprintf(stderr, "aborting due to JIT error: %s\n",
            gcc_jit_context_get_first_error(def.function_context));
    abort();
  }

  p->ravi_jit.jit_data = compilation_result;
  p->ravi_jit.jit_function =
      (lua_CFunction)gcc_jit_result_get_code(compilation_result, def.name);
  lua_assert(p->ravi_jit.jit_function);

  if (p->ravi_jit.jit_function == NULL) {
    p->ravi_jit.jit_status = 1; // can't compile
    p->ravi_jit.jit_data = NULL;
    goto on_error;
  } else {
    p->ravi_jit.jit_status = 2;
  }
  status = 1;

on_error:

  free_function_def(&def);

  return status;
}

// Free the JIT compiled function
// Note that this is called by the garbage collector
void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  (void)L;
  if (p->ravi_jit.jit_status == 2) /* compiled */ {
    gcc_jit_result *f = (gcc_jit_result *)(p->ravi_jit.jit_data);
    if (f)
      gcc_jit_result_release(f);
    p->ravi_jit.jit_status = 3;
    p->ravi_jit.jit_function = NULL;
    p->ravi_jit.jit_data = NULL;
    p->ravi_jit.execution_count = 0;
  }
}

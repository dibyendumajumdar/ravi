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
#include <ravijit.h>
#include <assert.h>

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
    case OP_LOADKX:
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
    case OP_RAVI_EQ_II:
    case OP_RAVI_EQ_FF:
    case OP_LT:
    case OP_RAVI_LT_II:
    case OP_RAVI_LT_FF:
    case OP_LE:
    case OP_RAVI_LE_II:
    case OP_RAVI_LE_FF:
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
    case OP_RAVI_ADDFF:
    case OP_RAVI_ADDFI:
    case OP_RAVI_ADDII:
    case OP_RAVI_SUBFF:
    case OP_RAVI_SUBFI:
    case OP_RAVI_SUBIF:
    case OP_RAVI_SUBII:
    case OP_RAVI_MULFF:
    case OP_RAVI_MULFI:
    case OP_RAVI_MULII:
    case OP_RAVI_DIVFF:
    case OP_RAVI_DIVFI:
    case OP_RAVI_DIVIF:
    case OP_RAVI_DIVII:
    case OP_SELF:
    case OP_LEN:
    case OP_SETTABLE:
    case OP_GETTABLE:
    case OP_NEWTABLE:
    case OP_SETLIST:
    case OP_TFORCALL:
    case OP_TFORLOOP:
    case OP_RAVI_NEWARRAYI:
    case OP_RAVI_NEWARRAYF:
    case OP_RAVI_GETTABLE_AI:
    case OP_RAVI_GETTABLE_AF:
    case OP_RAVI_TOARRAYI:
    case OP_RAVI_TOARRAYF:
    case OP_RAVI_MOVEAI:
    case OP_RAVI_MOVEAF:
    case OP_RAVI_SETTABLE_AI:
    case OP_RAVI_SETTABLE_AII:
    case OP_RAVI_SETTABLE_AF:
    case OP_RAVI_SETTABLE_AFF:
    case OP_SETTABUP:
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_GETUPVAL:
    case OP_SETUPVAL:
      break;
    case OP_FORPREP:
    case OP_FORLOOP:
    case OP_MOD:
    case OP_IDIV:
    case OP_UNM:
    case OP_POW:
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
    //    gcc_jit_context_set_bool_option(def->function_context,
    //                                    GCC_JIT_BOOL_OPTION_DUMP_INITIAL_TREE,
    //                                    1);
    gcc_jit_context_set_bool_option(def->function_context,
                                    GCC_JIT_BOOL_OPTION_DUMP_INITIAL_GIMPLE, 1);
  }
  gcc_jit_context_set_bool_allow_unreachable_blocks(def->function_context, 1);
  gcc_jit_context_set_int_option(def->function_context,
                                 GCC_JIT_INT_OPTION_OPTIMIZATION_LEVEL,
                                 def->opt_level);
  // gcc_jit_context_add_command_line_option(def->function_context,
  //                                        "-fno-strict-aliasing");

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

  //  def->base = gcc_jit_function_new_local(def->jit_function, NULL,
  //                                         def->ravi->types->pTValueT,
  //                                         "base");
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
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call2_rvalue(
          def, def->ravi->types->luaG_runerrorT,
          gcc_jit_param_as_rvalue(def->L),
          gcc_jit_context_new_string_literal(def->function_context, str)));
}

void ravi_emit_struct_assign(ravi_function_def_t *def, gcc_jit_lvalue *dest,
                             gcc_jit_lvalue *src) {
  //  gcc_jit_block_add_assignment(def->current_block, NULL,
  //                               gcc_jit_rvalue_dereference(dest, NULL),
  //                               gcc_jit_lvalue_as_rvalue(gcc_jit_rvalue_dereference(src,
  //                               NULL)));
  gcc_jit_lvalue *dest_value =
      gcc_jit_lvalue_access_field(dest, NULL, def->ravi->types->Value_value);
  assert(!gcc_jit_context_get_first_error(def->function_context));

  gcc_jit_lvalue *dest_value_i = gcc_jit_lvalue_access_field(
      dest_value, NULL, def->ravi->types->Value_value_i);

  gcc_jit_lvalue *src_value =
      gcc_jit_lvalue_access_field(src, NULL, def->ravi->types->Value_value);
  assert(!gcc_jit_context_get_first_error(def->function_context));

  gcc_jit_lvalue *src_value_i = gcc_jit_lvalue_access_field(
      src_value, NULL, def->ravi->types->Value_value_i);

  gcc_jit_block_add_assignment(def->current_block, NULL, dest_value_i,
                               gcc_jit_lvalue_as_rvalue(src_value_i));

  gcc_jit_lvalue *dest_tt =
      gcc_jit_lvalue_access_field(dest, NULL, def->ravi->types->Value_tt);
  gcc_jit_lvalue *src_tt =
      gcc_jit_lvalue_access_field(src, NULL, def->ravi->types->Value_tt);

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
  (void)def;
  // gcc_jit_block_add_assignment(def->current_block, NULL, def->base,
  //                             def->base_ref);
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

gcc_jit_rvalue *ravi_emit_array_get(ravi_function_def_t *def,
                                    gcc_jit_rvalue *ptr,
                                    gcc_jit_rvalue *index) {
  /* Note we assume that base is correct */
  gcc_jit_lvalue *el =
      gcc_jit_context_new_array_access(def->function_context, NULL, ptr, index);
  return gcc_jit_lvalue_as_rvalue(el);
}

gcc_jit_lvalue *ravi_emit_array_get_ptr(ravi_function_def_t *def,
                                        gcc_jit_rvalue *ptr,
                                        gcc_jit_rvalue *index) {
  /* Note we assume that base is correct */
  gcc_jit_lvalue *el =
      gcc_jit_context_new_array_access(def->function_context, NULL, ptr, index);
  return el;
}

/* Get access to the register identified by A - registers as just &base[offset]
 */
gcc_jit_lvalue *ravi_emit_get_register(ravi_function_def_t *def, int A) {
  /* Note we assume that base is correct */
  gcc_jit_lvalue *reg = gcc_jit_context_new_array_access(
      //      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(def->base),
      def->function_context, NULL, def->base_ref,
      gcc_jit_context_new_rvalue_from_int(def->function_context,
                                          def->ravi->types->C_intT, A));
  return reg;
}

/* Get access to a constant identify by Bx */
gcc_jit_lvalue *ravi_emit_get_constant(ravi_function_def_t *def, int Bx) {
  gcc_jit_lvalue *kst = gcc_jit_context_new_array_access(
      def->function_context, NULL, def->k,
      gcc_jit_context_new_rvalue_from_int(def->function_context,
                                          def->ravi->types->C_intT, Bx));
  return kst;
}

gcc_jit_lvalue *ravi_emit_get_register_or_constant(ravi_function_def_t *def,
                                                   int B) {
  if (ISK(B))
    return ravi_emit_get_constant(def, INDEXK(B));
  else
    return ravi_emit_get_register(def, B);
}

// L->top = R(B)
void ravi_emit_set_L_top_toreg(ravi_function_def_t *def, int B) {
  // Get pointer to register at R(B)
  gcc_jit_lvalue *reg = ravi_emit_get_register(def, B);
  // Get pointer to L->top
  gcc_jit_lvalue *top = gcc_jit_rvalue_dereference_field(
      gcc_jit_param_as_rvalue(def->L), NULL, def->ravi->types->lua_State_top);
  // L->top = R(B)
  gcc_jit_block_add_assignment(def->current_block, NULL, top,
                               gcc_jit_lvalue_get_address(reg, NULL));
}

gcc_jit_rvalue *ravi_emit_num_stack_elements(ravi_function_def_t *def,
                                             gcc_jit_rvalue *ra) {
  gcc_jit_lvalue *top = gcc_jit_rvalue_dereference_field(
      gcc_jit_param_as_rvalue(def->L), NULL, def->ravi->types->lua_State_top);
  gcc_jit_rvalue *diff = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_MINUS,
      def->ravi->types->C_ptrdiff_t, gcc_jit_lvalue_as_rvalue(top), ra);
  gcc_jit_rvalue *n_elements = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_DIVIDE,
      def->ravi->types->C_ptrdiff_t, diff,
      gcc_jit_context_new_rvalue_from_int(def->function_context,
                                          def->ravi->types->C_ptrdiff_t,
                                          sizeof(TValue)));
  gcc_jit_rvalue *n = gcc_jit_context_new_cast(
      def->function_context, NULL, n_elements, def->ravi->types->C_intT);
  return n;
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
                                     gcc_jit_lvalue *tv) {
  gcc_jit_lvalue *value =
      gcc_jit_lvalue_access_field(tv, NULL, def->ravi->types->Value_value);
  assert(!gcc_jit_context_get_first_error(def->function_context));

  gcc_jit_lvalue *i =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_i);
  return i;
}

/* Get TValue->value_.b */
gcc_jit_lvalue *ravi_emit_load_reg_b(ravi_function_def_t *def,
                                     gcc_jit_lvalue *tv) {
  gcc_jit_lvalue *value =
      gcc_jit_lvalue_access_field(tv, NULL, def->ravi->types->Value_value);
  assert(!gcc_jit_context_get_first_error(def->function_context));
  gcc_jit_lvalue *i =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_b);
  return i;
}

/* Get TValue->value_.n */
gcc_jit_lvalue *ravi_emit_load_reg_n(ravi_function_def_t *def,
                                     gcc_jit_lvalue *tv) {
  gcc_jit_lvalue *value =
      gcc_jit_lvalue_access_field(tv, NULL, def->ravi->types->Value_value);
  assert(!gcc_jit_context_get_first_error(def->function_context));
  gcc_jit_lvalue *n =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_n);
  return n;
}

/* Get (Table *) TValue->value_.gc */
gcc_jit_rvalue *ravi_emit_load_reg_h(ravi_function_def_t *def,
                                     gcc_jit_lvalue *tv) {

  gcc_jit_lvalue *value =
      gcc_jit_lvalue_access_field(tv, NULL, def->ravi->types->Value_value);
  assert(!gcc_jit_context_get_first_error(def->function_context));
  gcc_jit_lvalue *gc = gcc_jit_lvalue_access_field(
      value, NULL, def->ravi->types->Value_value_gc);
  gcc_jit_rvalue *h = gcc_jit_context_new_cast(def->function_context, NULL,
                                               gcc_jit_lvalue_as_rvalue(gc),
                                               def->ravi->types->pTableT);
  return h;
}

/* Get Table->ravi_array.data as lua_Number* */
gcc_jit_rvalue *ravi_emit_load_reg_h_floatarray(ravi_function_def_t *def,
                                                gcc_jit_rvalue *h) {

  gcc_jit_lvalue *value = gcc_jit_rvalue_dereference_field(
      h, NULL, def->ravi->types->Table_ravi_array);
  gcc_jit_lvalue *data = gcc_jit_lvalue_access_field(
      value, NULL, def->ravi->types->RaviArray_data);
  gcc_jit_rvalue *floatarray = gcc_jit_context_new_cast(
      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(data),
      def->ravi->types->plua_NumberT);
  return floatarray;
}

/* Get Table->ravi_array.data as lua_Integer* */
gcc_jit_rvalue *ravi_emit_load_reg_h_intarray(ravi_function_def_t *def,
                                              gcc_jit_rvalue *h) {

  gcc_jit_lvalue *value = gcc_jit_rvalue_dereference_field(
      h, NULL, def->ravi->types->Table_ravi_array);
  gcc_jit_lvalue *data = gcc_jit_lvalue_access_field(
      value, NULL, def->ravi->types->RaviArray_data);
  gcc_jit_rvalue *intarray = gcc_jit_context_new_cast(
      def->function_context, NULL, gcc_jit_lvalue_as_rvalue(data),
      def->ravi->types->plua_IntegerT);
  return intarray;
}

/* Get Table->ravi_array.len */
gcc_jit_lvalue *ravi_emit_load_ravi_arraylength(ravi_function_def_t *def,
                                                gcc_jit_rvalue *h) {

  gcc_jit_lvalue *value = gcc_jit_rvalue_dereference_field(
      h, NULL, def->ravi->types->Table_ravi_array);
  gcc_jit_lvalue *len =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->RaviArray_len);
  return len;
}

/* Get Table->ravi_array.array_type */
gcc_jit_lvalue *ravi_emit_load_ravi_arraytype(ravi_function_def_t *def,
                                              gcc_jit_rvalue *h) {

  gcc_jit_lvalue *value = gcc_jit_rvalue_dereference_field(
      h, NULL, def->ravi->types->Table_ravi_array);
  gcc_jit_lvalue *t = gcc_jit_lvalue_access_field(
      value, NULL, def->ravi->types->RaviArray_array_type);
  return t;
}

gcc_jit_lvalue *ravi_emit_load_type(ravi_function_def_t *def,
                                    gcc_jit_lvalue *reg) {
  return gcc_jit_lvalue_access_field(reg, NULL, def->ravi->types->Value_tt);
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

gcc_jit_rvalue *
ravi_emit_is_not_value_of_type_class(ravi_function_def_t *def,
                               gcc_jit_rvalue *value_type, int lua_type) {
  gcc_jit_rvalue *bit_mask = ravi_int_constant(def, 0x0F);
  gcc_jit_rvalue *novariant_type = gcc_jit_context_new_binary_op(
      def->function_context, NULL, GCC_JIT_BINARY_OP_BITWISE_AND,
      def->ravi->types->C_intT, value_type,
      bit_mask);
  return gcc_jit_context_new_unary_op(
      def->function_context, NULL, GCC_JIT_UNARY_OP_LOGICAL_NEGATE,
      def->ravi->types->C_boolT,
      ravi_emit_is_value_of_type(def, novariant_type, lua_type));
}

/* Store an integer value and set type to TNUMINT */
void ravi_emit_store_reg_i_withtype(ravi_function_def_t *def,
                                    gcc_jit_rvalue *ivalue,
                                    gcc_jit_lvalue *reg) {
  gcc_jit_lvalue *value =
      gcc_jit_lvalue_access_field(reg, NULL, def->ravi->types->Value_value);
  assert(!gcc_jit_context_get_first_error(def->function_context));

  gcc_jit_lvalue *i =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_i);
  gcc_jit_block_add_assignment(def->current_block, NULL, i, ivalue);
  gcc_jit_rvalue *type = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, LUA_TNUMINT);
  gcc_jit_lvalue *tt =
      gcc_jit_lvalue_access_field(reg, NULL, def->ravi->types->Value_tt);
  gcc_jit_block_add_assignment(def->current_block, NULL, tt, type);
}

/* Store a boolean value and set type to TBOOLEAN */
void ravi_emit_store_reg_b_withtype(ravi_function_def_t *def,
                                    gcc_jit_rvalue *bvalue,
                                    gcc_jit_lvalue *reg) {
  gcc_jit_lvalue *value =
      gcc_jit_lvalue_access_field(reg, NULL, def->ravi->types->Value_value);
  assert(!gcc_jit_context_get_first_error(def->function_context));
  gcc_jit_lvalue *n =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_b);
  gcc_jit_block_add_assignment(def->current_block, NULL, n, bvalue);
  gcc_jit_rvalue *type = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, LUA__TBOOLEAN);
  gcc_jit_lvalue *tt =
      gcc_jit_lvalue_access_field(reg, NULL, def->ravi->types->Value_tt);
  gcc_jit_block_add_assignment(def->current_block, NULL, tt, type);
}

/* Store a number value and set type to TNUMFLT */
void ravi_emit_store_reg_n_withtype(ravi_function_def_t *def,
                                    gcc_jit_rvalue *nvalue,
                                    gcc_jit_lvalue *reg) {
  gcc_jit_lvalue *value =
      gcc_jit_lvalue_access_field(reg, NULL, def->ravi->types->Value_value);
  assert(!gcc_jit_context_get_first_error(def->function_context));
  gcc_jit_lvalue *n =
      gcc_jit_lvalue_access_field(value, NULL, def->ravi->types->Value_value_n);
  gcc_jit_block_add_assignment(def->current_block, NULL, n, nvalue);
  gcc_jit_rvalue *type = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, LUA_TNUMFLT);
  gcc_jit_lvalue *tt =
      gcc_jit_lvalue_access_field(reg, NULL, def->ravi->types->Value_tt);
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
      ravi_emit_branch(def, block);
    }
    // Now add the new block and make it current
    ravi_set_current_block(def, block);
  } else if (def->current_block_terminated) {
    gcc_jit_block *block = gcc_jit_function_new_block(
        def->jit_function, unique_name(def, "LINK_BLOCK", pc));
    ravi_set_current_block(def, block);
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
gcc_jit_lvalue *ravi_emit_load_upval_v(ravi_function_def_t *def,
                                       gcc_jit_rvalue *pupval) {
  return gcc_jit_rvalue_dereference_field(pupval, NULL,
                                          def->ravi->types->UpVal_v);
}

// Get upval->u.value
gcc_jit_lvalue *ravi_emit_load_upval_value(ravi_function_def_t *def,
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

gcc_jit_lvalue *ravi_emit_tonumtype(ravi_function_def_t *def,
                                    gcc_jit_lvalue *reg, lua_typecode_t tt,
                                    int pc) {

  gcc_jit_lvalue *value = gcc_jit_function_new_local(
      def->jit_function, NULL,
      tt == LUA__TNUMFLT ? def->ravi->types->lua_NumberT
                         : def->ravi->types->lua_IntegerT,
      unique_name(def, "value", pc));
  gcc_jit_lvalue *reg_type = ravi_emit_load_type(def, reg);

  // Is reg an number?
  gcc_jit_rvalue *cmp1 =
      ravi_emit_is_value_of_type(def, gcc_jit_lvalue_as_rvalue(reg_type), tt);

  gcc_jit_block *convert_reg = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "convert_reg", pc));
  gcc_jit_block *copy_reg = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "copy_reg", pc));
  gcc_jit_block *load_val = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "load_val", pc));
  gcc_jit_block *failed_conversion = gcc_jit_function_new_block(
      def->jit_function, unique_name(def, "if_conversion_failed", pc));

  // If reg is integer then copy reg, else convert reg
  ravi_emit_conditional_branch(def, cmp1, copy_reg, convert_reg);

  // Convert RB
  ravi_set_current_block(def, convert_reg);

  // Do the conversion
  gcc_jit_rvalue *var_istt = ravi_function_call2_rvalue(
      def, tt == LUA__TNUMFLT ? def->ravi->types->luaV_tonumberT
                              : def->ravi->types->luaV_tointegerT,
      gcc_jit_lvalue_get_address(reg, NULL),
      gcc_jit_lvalue_get_address(value, NULL));
  gcc_jit_rvalue *zero = gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_intT, 0);
  gcc_jit_rvalue *conversion_failed =
      ravi_emit_comparison(def, GCC_JIT_COMPARISON_EQ, var_istt, zero);

  // Did conversion fail?
  ravi_emit_conditional_branch(def, conversion_failed, failed_conversion,
                               load_val);

  // Conversion failed, so raise error
  ravi_set_current_block(def, failed_conversion);
  ravi_emit_raise_lua_error(def, tt == LUA__TNUMFLT ? "number expected"
                                                    : "integer expected");
  ravi_emit_branch(def, load_val);

  // Conversion OK
  ravi_set_current_block(def, copy_reg);

  gcc_jit_lvalue *i = tt == LUA__TNUMFLT ? ravi_emit_load_reg_n(def, reg)
                                         : ravi_emit_load_reg_i(def, reg);
  gcc_jit_block_add_assignment(def->current_block, NULL, value,
                               gcc_jit_lvalue_as_rvalue(i));
  ravi_emit_branch(def, load_val);

  ravi_set_current_block(def, load_val);

  return value;
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
  //  def->base = NULL;
  def->current_block_terminated = false;
  def->buf[0] = 0;
  def->counter = 1;
  def->name[0] = 0;
  def->p = p;
  def->dump_ir = 0;
  def->dump_asm = 0;
}

int raviV_compile_n(struct lua_State *L, struct Proto *p[], int n,
  ravi_compile_options_t *options) {
  int count = 0;
  for (int i = 0; i < n; i++) {
    if (raviV_compile(L, p[i], options)) 
      count++;
  }
  return count > 0;
}

// Compile a Lua function
// If JIT is turned off then compilation is skipped
// Compilation occurs if either auto compilation is ON (subject to some
// thresholds)
// or if a manual compilation request was made
// Returns true if compilation was successful
int raviV_compile(struct lua_State *L, struct Proto *p,
                  ravi_compile_options_t *options) {
  // Compile given function if possible
  // The p->ravi_jit structure will be updated
  // Note that if a function fails to compile then
  // a flag is set so that it doesn't get compiled again
  int manual_request = options ? options->manual_request : 0;
  int dump = options ? options->dump_level : 0;
  bool omitArrayGetRangeCheck =
      options ? options->omit_array_get_range_check != 0 : false;
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
    case OP_LOADKX: {
      // OP_LOADKX is followed by OP_EXTRAARG
      Instruction inst = code[++pc];
      int Ax = GETARG_Ax(inst);
      lua_assert(GET_OPCODE(inst) == OP_EXTRAARG);
      ravi_emit_LOADK(&def, A, Ax, pc);
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
    case OP_TFORCALL: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      // OP_TFORCALL is followed by OP_TFORLOOP - we process this
      // along with OP_TFORCALL
      pc++;
      i = code[pc];
      op = GET_OPCODE(i);
      lua_assert(op == OP_TFORLOOP);
      int sbx = GETARG_sBx(i);
      // j below is the jump target
      int j = sbx + pc + 1;
      ravi_emit_TFORCALL(&def, A, B, C, j, GETARG_A(i), pc - 1);
    } break;
    case OP_TFORLOOP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      ravi_emit_TFORLOOP(&def, A, j, pc);
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

    case OP_RAVI_EQ_II:
    case OP_RAVI_EQ_FF:
    case OP_RAVI_LT_II:
    case OP_RAVI_LT_FF:
    case OP_RAVI_LE_II:
    case OP_RAVI_LE_FF:
    case OP_LT:
    case OP_LE:
    case OP_EQ: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      OpCode comparison_op = op;
      const char *opname =
          ((op == OP_EQ || op == OP_RAVI_EQ_II || op == OP_RAVI_EQ_FF)
               ? "OP_EQ"
               : ((op == OP_LT || op == OP_RAVI_LT_II || op == OP_RAVI_LT_FF)
                      ? "OP_LT"
                      : "OP_LE"));
      gcc_jit_function *comparison_function =
          ((op == OP_EQ || op == OP_RAVI_EQ_II || op == OP_RAVI_EQ_FF)
               ? def.ravi->types->luaV_equalobjT
               : ((op == OP_LT || op == OP_RAVI_LT_II || op == OP_RAVI_LT_FF)
                      ? def.ravi->types->luaV_lessthanT
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
                         opname, comparison_op, pc);
    } break;

    case OP_NEWTABLE: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_NEWTABLE(&def, A, B, C, pc);
    } break;
    case OP_RAVI_NEWARRAYI: {
      ravi_emit_NEWARRAYINT(&def, A, pc);
    } break;
    case OP_RAVI_NEWARRAYF: {
      ravi_emit_NEWARRAYFLOAT(&def, A, pc);
    } break;
    case OP_RAVI_GETTABLE_AI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_GETTABLE_AI(&def, A, B, C, pc, omitArrayGetRangeCheck);
    } break;
    case OP_RAVI_GETTABLE_AF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_GETTABLE_AF(&def, A, B, C, pc, omitArrayGetRangeCheck);
    } break;
    case OP_RAVI_TOARRAYI: {
      ravi_emit_TOARRAY(&def, A, RAVI_TARRAYINT, "integer[] expected", pc);
    } break;
    case OP_RAVI_TOARRAYF: {
      ravi_emit_TOARRAY(&def, A, RAVI_TARRAYFLT, "number[] expected", pc);
    } break;
    case OP_RAVI_MOVEAI: {
      int B = GETARG_B(i);
      ravi_emit_MOVEAI(&def, A, B, pc);
    } break;
    case OP_RAVI_MOVEAF: {
      int B = GETARG_B(i);
      ravi_emit_MOVEAF(&def, A, B, pc);
    } break;

    case OP_SETLIST: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_SETLIST(&def, A, B, C, pc);
    } break;
    case OP_GETTABUP: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_GETTABUP(&def, A, B, C, pc);
    } break;
    case OP_GETUPVAL: {
      int B = GETARG_B(i);
      ravi_emit_GETUPVAL(&def, A, B, pc);
    } break;
    case OP_SETTABUP: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_SETTABUP(&def, A, B, C, pc);
    } break;
    case OP_SETUPVAL: {
      int B = GETARG_B(i);
      ravi_emit_SETUPVAL(&def, A, B, pc);
    } break;
    case OP_SELF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_SELF(&def, A, B, C, pc);
    } break;
    case OP_LEN: {
      int B = GETARG_B(i);
      ravi_emit_LEN(&def, A, B, pc);
    } break;
    case OP_SETTABLE: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_SETTABLE(&def, A, B, C, pc);
    } break;
    case OP_GETTABLE: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_GETTABLE(&def, A, B, C, pc);
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
    case OP_RAVI_SUBFF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_SUBFF(&def, A, B, C, pc);
    } break;
    case OP_RAVI_SUBFI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_SUBFI(&def, A, B, C, pc);
    } break;
    case OP_RAVI_SUBIF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_SUBIF(&def, A, B, C, pc);
    } break;
    case OP_RAVI_SUBII: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_SUBII(&def, A, B, C, pc);
    } break;
    case OP_RAVI_MULFF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_MULFF(&def, A, B, C, pc);
    } break;
    case OP_RAVI_MULFI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_MULFI(&def, A, B, C, pc);
    } break;
    case OP_RAVI_MULII: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_MULII(&def, A, B, C, pc);
    } break;

    case OP_RAVI_DIVFF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_DIVFF(&def, A, B, C, pc);
    } break;
    case OP_RAVI_DIVFI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_DIVFI(&def, A, B, C, pc);
    } break;
    case OP_RAVI_DIVIF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_DIVIF(&def, A, B, C, pc);
    } break;
    case OP_RAVI_DIVII: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_DIVII(&def, A, B, C, pc);
    } break;
    case OP_RAVI_SETTABLE_AII:
    case OP_RAVI_SETTABLE_AI: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_SETTABLE_AI_AF(&def, A, B, C, op == OP_RAVI_SETTABLE_AII,
                               LUA__TNUMINT, pc);
    } break;
    case OP_RAVI_SETTABLE_AFF:
    case OP_RAVI_SETTABLE_AF: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_SETTABLE_AI_AF(&def, A, B, C, op == OP_RAVI_SETTABLE_AFF,
                               LUA__TNUMFLT, pc);
    } break;

    case OP_ADD: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_ARITH(&def, A, B, C, OP_ADD, TM_ADD, pc);
    } break;
    case OP_SUB: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_ARITH(&def, A, B, C, OP_SUB, TM_SUB, pc);
    } break;
    case OP_MUL: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_ARITH(&def, A, B, C, OP_MUL, TM_MUL, pc);
    } break;
    case OP_DIV: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      ravi_emit_ARITH(&def, A, B, C, OP_DIV, TM_DIV, pc);
    } break;

    default:
      break;
    }
  }

  if (def.dump_ir) {
    gcc_jit_context_dump_to_file(def.ravi->context, "cdump.txt", 0);
    gcc_jit_context_dump_to_file(def.function_context, "fdump.txt", 0);
    gcc_jit_context_dump_reproducer_to_file(def.function_context, "rdump.txt");
  }
  // gcc_jit_context_set_logfile (def.function_context, stderr, 0, 0);

  if (gcc_jit_context_get_first_error(def.function_context)) {
    fprintf(stderr, "aborting due to JIT error: %s\n",
            gcc_jit_context_get_first_error(def.function_context));
    ravi_print_function(p, 1);
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

void ravi_debug_printf(ravi_function_def_t *def, const char *str) {
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call1_rvalue(
          def, def->ravi->types->printfT,
          gcc_jit_context_new_string_literal(def->function_context, str)));
}

void ravi_debug_printf1(ravi_function_def_t *def, const char *str,
                        gcc_jit_rvalue *arg1) {
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call2_rvalue(
          def, def->ravi->types->printfT,
          gcc_jit_context_new_string_literal(def->function_context, str),
          arg1));
}

void ravi_debug_printf2(ravi_function_def_t *def, const char *str,
                        gcc_jit_rvalue *arg1, gcc_jit_rvalue *arg2) {
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call3_rvalue(
          def, def->ravi->types->printfT,
          gcc_jit_context_new_string_literal(def->function_context, str), arg1,
          arg2));
}

void ravi_debug_printf3(ravi_function_def_t *def, const char *str,
                        gcc_jit_rvalue *arg1, gcc_jit_rvalue *arg2,
                        gcc_jit_rvalue *arg3) {
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call4_rvalue(
          def, def->ravi->types->printfT,
          gcc_jit_context_new_string_literal(def->function_context, str), arg1,
          arg2, arg3));
}

void ravi_debug_printf4(ravi_function_def_t *def, const char *str,
                        gcc_jit_rvalue *arg1, gcc_jit_rvalue *arg2,
                        gcc_jit_rvalue *arg3, gcc_jit_rvalue *arg4) {
  gcc_jit_block_add_eval(
      def->current_block, NULL,
      ravi_function_call5_rvalue(
          def, def->ravi->types->printfT,
          gcc_jit_context_new_string_literal(def->function_context, str), arg1,
          arg2, arg3, arg4));
}

gcc_jit_rvalue *ravi_int_constant(ravi_function_def_t *def, int value) {
  return gcc_jit_context_new_rvalue_from_int(def->function_context,
                                             def->ravi->types->C_intT, value);
}

gcc_jit_rvalue *ravi_bool_constant(ravi_function_def_t *def, int value) {
  return gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->C_boolT, value ? 1 : 0);
}

gcc_jit_rvalue *ravi_lua_Integer_constant(ravi_function_def_t *def, int value) {
  return gcc_jit_context_new_rvalue_from_int(
      def->function_context, def->ravi->types->lua_IntegerT, value);
}

gcc_jit_rvalue *ravi_lua_Number_constant(ravi_function_def_t *def,
                                         double value) {
  return gcc_jit_context_new_rvalue_from_double(
      def->function_context, def->ravi->types->lua_NumberT, value);
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

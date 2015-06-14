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
static const char *unique_function_name(ravi_gcc_codegen_t *cg) {
  snprintf(cg->temp, sizeof cg->temp, "ravif%d", cg->id++);
  return cg->temp;
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
      break;
    case OP_LOADK:
    case OP_LOADKX:
    case OP_LOADNIL:
    case OP_LOADBOOL:
    case OP_CALL:
    case OP_TAILCALL:
    case OP_JMP:
    case OP_EQ:
    case OP_LT:
    case OP_LE:
    case OP_NOT:
    case OP_TEST:
    case OP_TESTSET:
    case OP_FORPREP:
    case OP_FORLOOP:
    case OP_TFORCALL:
    case OP_TFORLOOP:
    case OP_MOVE:
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_MOD:
    case OP_IDIV:
    case OP_UNM:
    case OP_POW:
    case OP_LEN:
    case OP_VARARG:
    case OP_CONCAT:
    case OP_CLOSURE:
    case OP_SETTABLE:
    case OP_GETTABLE:
    case OP_GETUPVAL:
    case OP_SETUPVAL:
    case OP_GETTABUP:
    case OP_SETTABUP:
    case OP_NEWTABLE:
    case OP_SETLIST:
    case OP_SELF:
    case OP_RAVI_NEWARRAYI:
    case OP_RAVI_NEWARRAYF:
    case OP_RAVI_MOVEI:
    case OP_RAVI_MOVEF:
    case OP_RAVI_TOINT:
    case OP_RAVI_TOFLT:
    case OP_RAVI_LOADFZ:
    case OP_RAVI_LOADIZ:
    case OP_RAVI_ADDFN:
    case OP_RAVI_ADDIN:
    case OP_RAVI_ADDFF:
    case OP_RAVI_ADDFI:
    case OP_RAVI_ADDII:
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
    case OP_RAVI_SETTABLE_AF:
    case OP_RAVI_TOARRAYI:
    case OP_RAVI_TOARRAYF:
    case OP_RAVI_MOVEAI:
    case OP_RAVI_MOVEAF:
    case OP_RAVI_FORLOOP_IP:
    case OP_RAVI_FORLOOP_I1:
    case OP_RAVI_FORPREP_IP:
    case OP_RAVI_FORPREP_I1:
    default:
      return false;
    }
  }
  return true;
}

static bool create_function(ravi_gcc_codegen_t *codegen, ravi_function_def_t *def) {

  /* The child context will hold function specific stuff - so that we can release
   * this when we have compiled the function
   */
  def->function_context =
      gcc_jit_context_new_child_context(codegen->ravi->context);
  if (!def->function_context) {
    fprintf(stderr, "error creating child context\n");
    goto on_error;
  }

  /* each function is given a unique name - as Lua functions are closures and do not really have names */
  const char *name = unique_function_name(codegen);

  /* the function signature is int (*) (lua_State *) */
  gcc_jit_param *param = gcc_jit_context_new_param(
      def->function_context, NULL, codegen->ravi->types->plua_StateT, "L");
  def->L = param; /* store the L parameter as we will need it */
  def->jit_function = gcc_jit_context_new_function(
      def->function_context, NULL, GCC_JIT_FUNCTION_INTERNAL,
      codegen->ravi->types->C_intT, name, 1, &param, 0);

  /* The entry block of the function */
  def->entry_block = gcc_jit_function_new_block(def->jit_function, "entry");
  def->current_block = def->entry_block;

  def->base = gcc_jit_function_new_local(def->jit_function, NULL, def->ravi->types->pTValueT, "base");

  return true;

on_error:
  return false;
}

/* release the resources allocated when compiling a function
 * note that the compiled function is not released here */
static void free_function_def(ravi_function_def_t *def) {
  if (def->function_context)
    gcc_jit_context_release(def->function_context);
  if (def->jmp_targets)
    free(def->jmp_targets);
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
 * basic blocks for each target. The blocks are saved in an array def->jmp_targets
 * which is indexed by the byte code offset 0 .. p->sizecode-1.
 */
static void scan_jump_targets(ravi_function_def_t *def, Proto *p) {
  // We need to pre-create blocks for jump targets so that we
  // can generate branch instructions in the code
  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  def->jmp_targets = (gcc_jit_block **)calloc(n, sizeof (gcc_jit_block *));
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    switch (op) {
      case OP_LOADBOOL: {
        int C = GETARG_C(i);
        int j = pc + 2; // jump target
        if (C && !def->jmp_targets[j])
          def->jmp_targets[j] =
                  gcc_jit_function_new_block(def->jit_function, "loadbool");
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
        char temp[80];
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
        // We append the Lua bytecode location to help debug the IR
        snprintf(temp, sizeof temp, "%s%d_", targetname, j + 1);
        //
        if (!def->jmp_targets[j]) {
          def->jmp_targets[j] =
                  gcc_jit_function_new_block(def->jit_function, temp);
        }
      } break;
      default:
        break;
    }
  }
}

/* Obtain reference to currently executing function L->ci->func */
static void emit_ci_func_value_gc_asLClosure(ravi_function_def_t *def,
                                                            gcc_jit_lvalue *ci) {
  gcc_jit_lvalue *gc = gcc_jit_rvalue_dereference_field(gcc_jit_lvalue_as_rvalue(ci), NULL, def->ravi->types->CallInfo_func);
  //const char *debugstr = gcc_jit_object_get_debug_string(gcc_jit_lvalue_as_object(gc));
  //fprintf(stderr, "%s\n", debugstr);
  gcc_jit_rvalue *func = gcc_jit_context_new_cast(def->function_context, NULL, gcc_jit_lvalue_as_rvalue(gc), def->ravi->types->pLClosureT);
  def->lua_closure = func;
}

/* Obtain reference to L->ci */
static void emit_getL_ci_value(ravi_function_def_t *def) {
  def->ci_val = gcc_jit_rvalue_dereference_field(gcc_jit_param_as_rvalue(def->L), NULL, def->ravi->types->lua_State_ci);
}

/* Refresh local copy of L->ci.u.l.base */
static void emit_refresh_base(ravi_function_def_t *def) {
  gcc_jit_block_add_assignment(def->current_block, NULL, def->base, def->base_ref);
}

/* Obtain reference to L->ci.u.l.base */
static void emit_getL_base_reference(ravi_function_def_t *def, gcc_jit_lvalue *ci) {
  gcc_jit_lvalue *u = gcc_jit_rvalue_dereference_field(gcc_jit_lvalue_as_rvalue(ci), NULL, def->ravi->types->CallInfo_u);
  gcc_jit_rvalue *u_l = gcc_jit_rvalue_access_field(gcc_jit_lvalue_as_rvalue(u), NULL, def->ravi->types->CallInfo_u_l);
  gcc_jit_rvalue *u_l_base = gcc_jit_rvalue_access_field(u_l, NULL, def->ravi->types->CallInfo_u_l_base);
  def->base_ref = u_l_base;
  emit_refresh_base(def);
}

/* Get the Lua function prototpe and constants table */
static void emit_get_proto_and_k(ravi_function_def_t *def) {
  gcc_jit_lvalue *proto = gcc_jit_rvalue_dereference_field(def->lua_closure, NULL, def->ravi->types->LClosure_p);
  def->proto = gcc_jit_lvalue_as_rvalue(proto);
  gcc_jit_lvalue *k = gcc_jit_rvalue_dereference_field(def->proto, NULL, def->ravi->types->LClosure_p_k);
  def->k = gcc_jit_lvalue_as_rvalue(k);
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
  (void)L;
  (void)p;
  (void)manual_request;
  (void)dump;

  global_State *G = G(L);
  if (G->ravi_state == NULL || G->ravi_state->jit == NULL)
    return false;

  if (!can_compile(p))
    return false;

  ravi_State *ravi_state = (ravi_State *)G->ravi_state;
  ravi_gcc_codegen_t *codegen = ravi_state->code_generator;

  ravi_function_def_t def;
  def.ravi = ravi_state->jit;
  def.entry_block = NULL;
  def.function_context = NULL;
  def.jit_function = NULL;
  def.jmp_targets = NULL;
  def.ci_val = NULL;
  def.lua_closure = NULL;
  def.current_block = NULL;
  def.proto = NULL;
  def.k = NULL;
  def.base = NULL;

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

on_error:
  gcc_jit_context_dump_to_file(def.function_context, "fdump.txt", 0);
  free_function_def(&def);

  return false;
}

// Free the JIT compiled function
// Note that this is called by the garbage collector
void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  (void)L;
  if (p->ravi_jit.jit_status == 2) /* compiled */ {
    gcc_jit_result *f =
        (gcc_jit_result *)(p->ravi_jit.jit_data);
    if (f)
      gcc_jit_result_release(f);
    p->ravi_jit.jit_status = 3;
    p->ravi_jit.jit_function = NULL;
    p->ravi_jit.jit_data = NULL;
    p->ravi_jit.execution_count = 0;
  }
}

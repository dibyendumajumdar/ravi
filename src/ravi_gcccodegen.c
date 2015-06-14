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

static void ravi_function_free(struct ravi_gcc_function_t *f) {
  if (f) {
    if (f->function_context)
      gcc_jit_context_release(f->function_context);
    if (f->jit_result)
      gcc_jit_result_release(f->jit_result);
    free(f);
  }
}

static ravi_gcc_function_t *create_function(ravi_gcc_codegen_t *codegen) {

  ravi_gcc_function_t *f = NULL;
  gcc_jit_context *function_context =
      gcc_jit_context_new_child_context(codegen->ravi->context);
  if (function_context == NULL) {
    fprintf(stderr, "error creating a new child context\n");
    goto on_error;
  }

  f = (ravi_gcc_function_t *)calloc(1, sizeof(ravi_gcc_function_t));
  if (f == NULL) {
    fprintf(stderr, "error creating a new function\n");
    goto on_error;
  }
  f->function_context = function_context;
  f->ravi = codegen->ravi;
  f->jit_result = NULL;

  const char *name = unique_function_name(codegen);
  gcc_jit_param *param = gcc_jit_context_new_param(
      function_context, NULL, codegen->ravi->types->plua_StateT, "L");
  f->jit_function = gcc_jit_context_new_function(
      f->function_context, NULL, GCC_JIT_FUNCTION_INTERNAL,
      codegen->ravi->types->C_intT, name, 1, &param, 0);

  gcc_jit_context_dump_to_file(function_context, "fdump.txt", 0);

on_error:
  if (f)
    ravi_function_free(f);
  else if (function_context)
    gcc_jit_context_release(function_context);
  return NULL;
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
  // ravi_gcc_context_t *ravi = ravi_state->jit;
  ravi_gcc_codegen_t *codegen = ravi_state->code_generator;

  ravi_gcc_function_t *f = create_function(codegen);

  if (f)
    ravi_function_free(f);

  return false;
}

// Free the JIT compiled function
// Note that this is called by the garbage collector
void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  (void)L;
  if (p->ravi_jit.jit_status == 2) /* compiled */ {
    struct ravi_gcc_function_t *f =
        (struct ravi_gcc_function_t *)(p->ravi_jit.jit_data);
    ravi_function_free(f);
    p->ravi_jit.jit_status = 3;
    p->ravi_jit.jit_function = NULL;
    p->ravi_jit.jit_data = NULL;
    p->ravi_jit.execution_count = 0;
  }
}

/******************************************************************************
 * Copyright (C) 2020-2022 Dibyendu Majumdar
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

/*
This file contains the Linearizer. The goal of the Linearizer is to
generate a linear intermediate representation (IR) from the AST
suitable for further analysis.

The linear IR is organized in basic blocks.
Each proc has an entry block and exit block. Additional blocks
are created as necessary.

Each basic block contains a sequence of instructions.
The final instruction of a block must always be a branch instruction.
*/

#include "parser.h"
#include "linearizer.h"
#include "fnv_hash.h"
#include "ptrlist.h"
#include "graph.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <alloca.h>
#include <unistd.h>
#else
#include <io.h>
#include <malloc.h>
#endif

static void handle_error(CompilerState *compiler_state, const char *msg)
{
	// TODO source and line number
	raviX_buffer_add_string(&compiler_state->error_message, msg);
	longjmp(compiler_state->env, 1);
}

static Pseudo *linearize_expression(Proc *proc, AstNode *expr);
static BasicBlock *create_block(Proc *proc);
static void start_block(Proc *proc, BasicBlock *bb, unsigned line_number);
static void linearize_statement(Proc *proc, AstNode *node);
static void linearize_statement_list(Proc *proc, AstNodeList *list);
static void start_scope(LinearizerState *linearizer, Proc *proc, Scope *scope);
static void end_scope(LinearizerState *linearizer, Proc *proc, unsigned line_number);
static void instruct_br(Proc *proc, Pseudo *pseudo, unsigned line_number);
static bool is_block_terminated(BasicBlock *block);
static Pseudo *instruct_move(Proc *proc, enum opcode op, Pseudo *target, Pseudo *src, unsigned line_number);
static void linearize_function(LinearizerState *linearizer);
static Instruction *allocate_instruction(Proc *proc, enum opcode op, unsigned line_number);
static void free_temp_pseudo(Proc *proc, Pseudo *pseudo, bool free_local);
static Pseudo *indexed_load(Proc *proc, Pseudo *index_pseudo);
static void indexed_store(Proc *proc, Pseudo *index_pseudo, Pseudo *value_pseudo,
			  ravitype_t value_type);

static AstNode* astlist_get(AstNodeList *list, unsigned int i)
{
	return (AstNode*) raviX_ptrlist_nth_entry((struct PtrList *) list, i);
}

/*
 * A simple register assignment system that uses a bit set 256 bits long.
 * It is not the most efficient as we don't use hard-ware intrinsics to scan for
 * next available bit.
 */

#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
#define FIELD_E_SIZEOF(t, f) (sizeof(((t*)0)->f[0]))

enum {
	BITSET_SIZE = FIELD_SIZEOF(PseudoGenerator, bits),    /* total size of bits in #bytes */
	BITSET_ESIZE = FIELD_E_SIZEOF(PseudoGenerator, bits), /* sizeof bits[0] in #bytes */
	N_WORDS = BITSET_SIZE / BITSET_ESIZE,		      /* bits array size in #bytes */
	ESIZE = BITSET_ESIZE * 8,			      /* bits in bits[0] */
	MAXBIT = N_WORDS * ESIZE			      /* total bits */
};

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
static_assert(N_WORDS == 4, "Invalid computation of bitset size"); /* must be kept in sync with PseudoGeneraotor.bits */
#endif

/* Identify the top most register allocated; this is useful when we need
 * to ensure that the next register goes to the top of the stack
 */
static int top_reg(PseudoGenerator *generator)
{
	/* start from the last element of bits */
	for (int i = N_WORDS-1; i >= 0; i--) {
		if (!generator->bits[i]) // no bit set
			continue;
		uint64_t bit = generator->bits[i];
		int x = i * ESIZE - 1; /* x is rightmost bit in bits[i] */
		/* keep getting rid of bits until we have none left */
		while (bit != 0) {
			bit >>= 1;
			x++;
		}
		return x;
	}
	return -1;
}

/**
 * Is given register top of the stack of registers?
 */
static int pseudo_gen_is_top(PseudoGenerator *generator, unsigned reg)
{
	int top = top_reg(generator);
	if (top < 0)
		return 0;
	return top == reg;
}

/**
 * Free the given register
 */
static void pseudo_gen_free(PseudoGenerator *generator, unsigned reg)
{
	assert(reg < MAXBIT);
	unsigned n = reg / ESIZE;
	reg = reg % ESIZE;
	generator->bits[n] &= ~(1ull << reg);
}

/**
 * Allocate a register, if top is specified then ensure it is top of the
 * stack else look for a free register. Not the most efficient as we
 * don't yet use hardware intrinsics for bit scans.
 */
static unsigned pseudo_gen_alloc(PseudoGenerator *generator, bool top)
{
	unsigned reg;
	if (top) {
		int current_top = top_reg(generator);
		reg = current_top + 1;
	} else {
		/* look for the first free reg */
		reg = 0;
		int is_set = 1;
		for (int i = 0; is_set && i < N_WORDS; i++) {
			uint64_t bit = generator->bits[i];
			if (bit == ~0ull) {
				/* all bits set? skip */
				reg += ESIZE;
				continue;
			} 
			for (int j = 0; is_set && j < ESIZE; j++) {
				is_set = (bit & (1ull << j)) != 0;
				if (is_set)
					reg++;
			}
		}
	}
	unsigned i = reg / ESIZE;
	unsigned j = reg % ESIZE;
	generator->bits[i] |= (1ull << j);
	assert(reg <= generator->max_reg);
	if (reg == generator->max_reg)
		generator->max_reg += 1;
	return reg;
}

static void pseudo_gen_check(PseudoGenerator *generator, AstNode *node, const char *desc) {
	unsigned reg = 0;
	for (int i = 0; i < N_WORDS; i++) {
		uint64_t bit = generator->bits[i];
		if (bit == 0ull) {
			/* no bits set? skip */
			reg += ESIZE;
			continue;
		}
		for (int j = 0; j < ESIZE; j++) {
			int is_set = (bit & (1ull << j)) != 0;
			if (is_set)
				fprintf(stderr, "%s: REG %u is in use, line #%d\n", desc, reg, node->line_number);
			reg++;
		}
	}
}


/**
 * Allocates a register by reusing a free'd register if possible otherwise
 * allocating a new one, if top is set then ensure that the register is
 * top of the stack
 */
static inline unsigned allocate_register(PseudoGenerator *generator, bool top)
{
	return pseudo_gen_alloc(generator, top);
}

/**
 * Puts a register in the free list (must not already have been put there).
 */
static inline void free_register(Proc *proc, PseudoGenerator *generator, unsigned reg)
{
	pseudo_gen_free(generator, reg);
}

/* Linearizer initialization  */
LinearizerState *raviX_init_linearizer(CompilerState *compiler_state)
{
	LinearizerState *linearizer = (LinearizerState *)raviX_calloc(1, sizeof(LinearizerState));
	linearizer->compiler_state = compiler_state;
	linearizer->proc_id = 0;
	return linearizer;
}

void raviX_destroy_linearizer(LinearizerState *linearizer)
{
	if (linearizer == NULL)
		return;
	Proc *proc;
	FOR_EACH_PTR(linearizer->all_procs, Proc, proc)
	{
		if (proc->constants)
			raviX_set_destroy(proc->constants, NULL);
		if (proc->cfg) 
			raviX_destroy_graph(proc->cfg);
	}
	END_FOR_EACH_PTR(proc)
	raviX_buffer_free(&linearizer->C_declarations);
	raviX_free(linearizer);
}

static void add_C_declaration(LinearizerState *linearizer, const StringObject *str)
{
	raviX_buffer_add_string(&linearizer->C_declarations, str->str);
}

/**
 * We assume strings are all interned and can be compared by
 * address. Return true if values match else false.
 */
static int compare_constants(const void *a, const void *b)
{
	const Constant *c1 = (const Constant *)a;
	const Constant *c2 = (const Constant *)b;
	if (c1->type != c2->type)
		return 0;
	if (c1->type == RAVI_TNUMINT)
		return c1->i == c2->i;
	else if (c1->type == RAVI_TNUMFLT)
		return c1->n == c2->n;
	else
		return c1->s == c2->s;
}

/**
 * Hashes a constant
 */
static uint32_t hash_constant(const void *c)
{
	const Constant *c1 = (const Constant *)c;
	if (c1->type == RAVI_TNUMINT)
		return (uint32_t)c1->i;
	else if (c1->type == RAVI_TNUMFLT)
		return (uint32_t)c1->n; // FIXME maybe use Lua's hash gen
	else
		return (uint32_t)c1->s->hash;
}

/**
 * Adds a constant to the proc's constant table. The constant is also assigned a
 * pseudo register.
 */
static const Constant *add_constant(Proc *proc, const Constant *c)
{
	SetEntry *entry = raviX_set_search(proc->constants, c);
	if (entry == NULL) {
		int reg = 0;
		/* Assign each type of constant a different range so that if backend
		 * doesn't need to emit the regnum for a particular type it can do so.
		 * If backend needs to emit all constants then 2 of the 3 ranges can
		 * easily adjusted.
		 */
		switch(c->type) {
		case RAVI_TNUMINT:
			reg = proc->num_intconstants++;
			break;
		case RAVI_TNUMFLT:
			reg = proc->num_fltconstants++;
			break;
		default:
			assert(c->type == RAVI_TSTRING);
			reg = proc->num_strconstants++;
			break;
		}
		C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
		Constant *c1 = (Constant *) allocator->calloc(allocator->arena, 1, sizeof(Constant));
		memcpy(c1, c, sizeof(Constant));
		c1->index = reg;
		raviX_set_add(proc->constants, c1);
		return c1;
	} else {
		const Constant *c1 = (Constant *) entry->key;
		return c1;
	}
}

/**
 * Allocates and adds a constant to the Proc's constants table.
 * Input is expected to be EXPR_LITERAL
 */
static const Constant *allocate_constant(Proc *proc, AstNode *node)
{
	assert(node->type == EXPR_LITERAL);
	Constant c = {.type = (uint16_t) node->literal_expr.type.type_code};
	assert(c.type == node->literal_expr.type.type_code);
	if (c.type == RAVI_TNUMINT)
		c.i = node->literal_expr.u.i;
	else if (c.type == RAVI_TNUMFLT)
		c.n = node->literal_expr.u.r;
	else
		c.s = node->literal_expr.u.ts;
	return add_constant(proc, &c);
}

static const Constant *allocate_integer_constant(Proc *proc, int i)
{
	Constant c = {.type = RAVI_TNUMINT, .i = i};
	assert(c.type == RAVI_TNUMINT);
	return add_constant(proc, &c);
}

static const Constant *allocate_string_constant(Proc *proc, const StringObject *s)
{
	Constant c = {.type = RAVI_TSTRING, .s = s};
	assert(c.type == RAVI_TSTRING);
	return add_constant(proc, &c);
}

static inline Pseudo *add_instruction_operand(Proc *proc, Instruction *insn, Pseudo *pseudo)
{
	Pseudo *to_free = NULL;
	if (pseudo->type == PSEUDO_INDEXED) {
		pseudo = indexed_load(proc, pseudo);
		to_free = pseudo;
	}
	raviX_ptrlist_add((PtrList **)&insn->operands, pseudo, proc->linearizer->compiler_state->allocator);
	return to_free;
}

static inline void replace_instruction_operand(Proc *proc, Instruction *insn, Pseudo *old_pseudo, Pseudo *new_pseudo)
{
	raviX_ptrlist_replace((PtrList **)&insn->operands, old_pseudo, new_pseudo, 1);
}

static inline void add_instruction_target(Proc *proc, Instruction *insn, Pseudo *pseudo)
{
	assert(pseudo->type != PSEUDO_INDEXED);
	raviX_ptrlist_add((PtrList **)&insn->targets, pseudo, proc->linearizer->compiler_state->allocator);
}

static Instruction *allocate_instruction(Proc *proc, enum opcode op, unsigned line_number)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	Instruction *insn = (Instruction *) allocator->calloc(allocator->arena, 1, sizeof(Instruction));
	insn->opcode = op;
	insn->line_number = line_number;
	return insn;
}

static void free_instruction_operand_pseudos(Proc *proc, Instruction *insn)
{
	Pseudo *operand;
	Pseudo *last = NULL;
	FOR_EACH_PTR_REVERSE(insn->operands, Pseudo, operand) {
		free_temp_pseudo(proc, operand, false);
		last = operand;
	} END_FOR_EACH_PTR_REVERSE(operand)
	if (last != NULL && last->type == PSEUDO_RANGE_SELECT) {
		free_temp_pseudo(proc, last->range_pseudo, false);
	}
}

/* adds instruction to the current basic block */
static inline void add_instruction(Proc *proc, Instruction *insn)
{
	assert(insn->block == NULL || insn->block == proc->current_bb);
	raviX_ptrlist_add((PtrList **)&proc->current_bb->insns, insn, proc->linearizer->compiler_state->allocator);
	insn->block = proc->current_bb;
}

Instruction *raviX_last_instruction(BasicBlock *block)
{
	if (raviX_ptrlist_size((PtrList *)block->insns) == 0)
		return NULL;
	return (Instruction *)raviX_ptrlist_last((PtrList *)block->insns);
}

static Pseudo *allocate_symbol_pseudo(Proc *proc, LuaSymbol *sym, unsigned reg)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	Pseudo *pseudo = (Pseudo *) allocator->calloc(allocator->arena, 1, sizeof(Pseudo));
	pseudo->type = PSEUDO_SYMBOL;
	pseudo->symbol = sym;
	pseudo->regnum = reg;
	if (sym->symbol_type == SYM_LOCAL) {
		assert(sym->variable.pseudo == NULL);
		sym->variable.pseudo = pseudo;
	}
	return pseudo;
}

static Pseudo *allocate_constant_pseudo(Proc *proc, const Constant *constant)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	Pseudo *pseudo = (Pseudo *) allocator->calloc(allocator->arena, 1, sizeof(Pseudo));
	pseudo->type = PSEUDO_CONSTANT;
	pseudo->constant = constant;
	pseudo->regnum = constant->index;
	return pseudo;
}

static Pseudo *allocate_closure_pseudo(Proc *proc)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	Pseudo *pseudo = (Pseudo *) allocator->calloc(allocator->arena, 1, sizeof(Pseudo));
	pseudo->type = PSEUDO_PROC;
	pseudo->proc = proc;
	return pseudo;
}

static Pseudo *allocate_nil_pseudo(Proc *proc)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	Pseudo *pseudo = (Pseudo *) allocator->calloc(allocator->arena, 1, sizeof(Pseudo));
	pseudo->type = PSEUDO_NIL;
	pseudo->proc = proc;
	return pseudo;
}

static Pseudo *allocate_boolean_pseudo(Proc *proc, bool is_true)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	Pseudo *pseudo = (Pseudo *) allocator->calloc(allocator->arena, 1, sizeof(Pseudo));
	pseudo->type = is_true ? PSEUDO_TRUE : PSEUDO_FALSE;
	pseudo->proc = proc;
	return pseudo;
}

static Pseudo *allocate_block_pseudo(Proc *proc, BasicBlock *block)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	Pseudo *pseudo = (Pseudo *) allocator->calloc(allocator->arena, 1, sizeof(Pseudo));
	pseudo->type = PSEUDO_BLOCK;
	pseudo->block = block;
	return pseudo;
}

/*
We have several types of temp pseudos.
Specific types for floating and integer values so that we can
localise the assignment of these to registers.
The generic 'any' type is used for other types
but has variant called PSEUDO_RANGE. This is used in function calls
to represent multiple return values. Most of the time these get converted
back to normal temp pseudo, but in some cases we need to reference
a particular value in the range and for that we use PSEUDO_RANGE_SELECT.
*/
static Pseudo *allocate_temp_pseudo(Proc *proc, ravitype_t type, bool top)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	PseudoGenerator *gen;
	enum PseudoType pseudo_type;
	switch (type) {
	case RAVI_TNUMFLT:
		gen = &proc->temp_flt_pseudos;
		pseudo_type = PSEUDO_TEMP_FLT;
		break;
	case RAVI_TNUMINT:
	case RAVI_TBOOLEAN:
		gen = &proc->temp_int_pseudos;
		pseudo_type = type == RAVI_TNUMINT ? PSEUDO_TEMP_INT: PSEUDO_TEMP_BOOL;
		break;
	default:
		gen = &proc->temp_pseudos;
		pseudo_type = PSEUDO_TEMP_ANY;
		break;
	}
	unsigned reg = allocate_register(gen, top);
	Pseudo *pseudo = (Pseudo *) allocator->calloc(allocator->arena, 1, sizeof(Pseudo));
	pseudo->type = pseudo_type;
	pseudo->regnum = reg;
	pseudo->temp_for_local = NULL;
	return pseudo;
}

static Pseudo *allocate_range_pseudo(Proc *proc, Pseudo *orig_pseudo)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	Pseudo *pseudo = (Pseudo *) allocator->calloc(allocator->arena, 1, sizeof(Pseudo));
	pseudo->type = PSEUDO_RANGE;
	pseudo->regnum = orig_pseudo->regnum;
	if (orig_pseudo->type == PSEUDO_TEMP_ANY) {
		orig_pseudo->freed = 1; // No need to free the original register
	}
	return pseudo;
}

/*
A PSEUDO_RANGE_SELECT picks or selects a particular offset in the range
specified by a PSEUDO_RANGE. Pick of 0 means pick first value from the range.
*/
Pseudo *raviX_allocate_range_select_pseudo(Proc *proc, Pseudo *range_pseudo, int pick)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	assert(range_pseudo->type == PSEUDO_RANGE);
	Pseudo *pseudo = (Pseudo *) allocator->calloc(allocator->arena, 1, sizeof(Pseudo));
	pseudo->type = PSEUDO_RANGE_SELECT;
	pseudo->regnum = range_pseudo->regnum + pick;
	pseudo->range_pseudo = range_pseudo;
	return pseudo;
}

static inline Pseudo *convert_range_to_temp(Pseudo *pseudo)
{
	assert(pseudo->type == PSEUDO_RANGE);
	pseudo->type = PSEUDO_TEMP_ANY;
	return pseudo;
}

static Pseudo *allocate_indexed_pseudo(Proc *proc)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	Pseudo *pseudo = (Pseudo *) allocator->calloc(allocator->arena, 1, sizeof(Pseudo));
	pseudo->type = PSEUDO_INDEXED;
	return pseudo;
}

static void free_temp_pseudo(Proc *proc, Pseudo *pseudo, bool free_local)
{
	if (pseudo->freed)
		return;
	PseudoGenerator *gen;
	switch (pseudo->type) {
	case PSEUDO_TEMP_FLT:
		gen = &proc->temp_flt_pseudos;
		break;
	case PSEUDO_TEMP_INT:
	case PSEUDO_TEMP_BOOL:
		gen = &proc->temp_int_pseudos;
		break;
	case PSEUDO_RANGE:
	case PSEUDO_TEMP_ANY:
		gen = &proc->temp_pseudos;
		break;
	default:
		// Not a temp, so no need to do anything
		return;
	}
	if (!free_local && pseudo->type != PSEUDO_RANGE && pseudo->temp_for_local) {
		return;
	}
	free_register(proc, gen, pseudo->regnum);
}

/**
 * Allocate a new Proc. If there is a current Proc, then the new Proc gets added to the
 * current Proc's children.
 */
static Proc *allocate_proc(LinearizerState *linearizer, AstNode *function_expr)
{
	C_MemoryAllocator *allocator = linearizer->compiler_state->allocator;
	assert(function_expr->type == EXPR_FUNCTION);
	Proc *proc = (Proc *) allocator->calloc(allocator->arena, 1, sizeof(Proc));
	proc->function_expr = function_expr;
	proc->id = raviX_ptrlist_size((PtrList *)linearizer->all_procs)+1; // so that 0 is not assigned
	function_expr->function_expr.proc_id = proc->id;
	raviX_ptrlist_add((PtrList **)&linearizer->all_procs, proc, linearizer->compiler_state->allocator);
	if (linearizer->current_proc) {
		proc->parent = linearizer->current_proc;
		raviX_ptrlist_add((PtrList **)&linearizer->current_proc->procs, proc,
				  linearizer->compiler_state->allocator);
	}
	proc->constants = raviX_set_create(hash_constant, compare_constants);
	proc->linearizer = linearizer;
	proc->cfg = NULL;
	return proc;
}

static void set_main_proc(LinearizerState *linearizer, Proc *proc)
{
	assert(linearizer->main_proc == NULL);
	assert(linearizer->current_proc == NULL);
	linearizer->main_proc = proc;
	assert(proc->function_expr->function_expr.parent_function == NULL);
}

static inline void set_current_proc(LinearizerState *linearizer, Proc *proc)
{
	linearizer->current_proc = proc;
}

static void instruct_totype(Proc *proc, Pseudo *target, const VariableType *vtype, unsigned line_number)
{
	enum opcode targetop = op_nop;
	switch (vtype->type_code) {
	case RAVI_TNUMFLT:
		targetop = op_toflt;
		break;
	case RAVI_TNUMINT:
		targetop = op_toint;
		break;
	case RAVI_TSTRING:
		targetop = op_tostring;
		break;
	case RAVI_TFUNCTION:
		targetop = op_toclosure;
		break;
	case RAVI_TTABLE:
		targetop = op_totable;
		break;
	case RAVI_TARRAYFLT:
		targetop = op_tofarray;
		break;
	case RAVI_TARRAYINT:
		targetop = op_toiarray;
		break;
	case RAVI_TUSERDATA:
		targetop = op_totype;
		break;
	default:
		return;
	}
	Instruction *insn = allocate_instruction(proc, targetop, line_number);
	Pseudo *tofree = NULL;
	if (targetop == op_totype) {
		assert(vtype->type_name);
		const Constant *tname_constant = allocate_string_constant(proc, vtype->type_name);
		Pseudo *tname_pseudo = allocate_constant_pseudo(proc, tname_constant);
		tofree = add_instruction_operand(proc, insn, tname_pseudo);
	}
	add_instruction_target(proc, insn, target);
	add_instruction(proc, insn);
	assert(!tofree);
	if (tofree)
		free_temp_pseudo(proc, tofree, false);
}

static void linearize_function_args(LinearizerState *linearizer)
{
	Proc *proc = linearizer->current_proc;
	AstNode *func_expr = proc->function_expr;
	LuaSymbol *sym;
	FOR_EACH_PTR(func_expr->function_expr.args, LuaSymbol, sym)
	{
		/* The arg symbols already have register assigned by the local scope */
		assert(sym->variable.pseudo); // We should already have a register assigned
		instruct_totype(proc, sym->variable.pseudo, &sym->variable.value_type, func_expr->line_number);
	}
	END_FOR_EACH_PTR(sym)
}

static void linearize_statement_list(Proc *proc, AstNodeList *list)
{
	AstNode *node;
	FOR_EACH_PTR(list, AstNode, node) { linearize_statement(proc, node); }
	END_FOR_EACH_PTR(node)
}

static Pseudo *linearize_literal(Proc *proc, AstNode *expr)
{
	assert(expr->type == EXPR_LITERAL);
	ravitype_t type = expr->literal_expr.type.type_code;
	Pseudo *pseudo = NULL;
	switch (type) {
	case RAVI_TNUMFLT:
	case RAVI_TNUMINT:
	case RAVI_TSTRING:
		pseudo = allocate_constant_pseudo(proc, allocate_constant(proc, expr));
		break;
	case RAVI_TNIL:
		pseudo = allocate_nil_pseudo(proc);
		break;
	case RAVI_TBOOLEAN:
		pseudo = allocate_boolean_pseudo(proc, expr->literal_expr.u.i);
		break;
	case RAVI_TVARARGS:
		handle_error(proc->linearizer->compiler_state, "Var args not supported");
		break;
	default:
		handle_error(proc->linearizer->compiler_state, "feature not yet implemented");
		break;
	}
	return pseudo;
}

static Pseudo *linearize_unary_operator(Proc *proc, AstNode *node)
{
	UnaryOperatorType op = node->unary_expr.unary_op;
	Pseudo *subexpr = linearize_expression(proc, node->unary_expr.expr);
	ravitype_t subexpr_type = node->unary_expr.expr->common_expr.type.type_code;
	enum opcode targetop = op_nop;
	switch (op) {
	case UNOPR_MINUS:
		if (subexpr_type == RAVI_TNUMINT)
			targetop = op_unmi;
		else if (subexpr_type == RAVI_TNUMFLT)
			targetop = op_unmf;
		else
			targetop = op_unm;
		break;
	case UNOPR_LEN:
		if (subexpr_type == RAVI_TARRAYINT || subexpr_type == RAVI_TARRAYFLT)
			targetop = op_leni;
		else
			targetop = op_len;
		subexpr_type = node->unary_expr.type.type_code;
		break;
	case UNOPR_TO_INTEGER:
		targetop = subexpr_type != RAVI_TNUMINT ? op_toint : op_nop;
		break;
	case UNOPR_TO_NUMBER:
		targetop = subexpr_type != RAVI_TNUMFLT ? op_toflt : op_nop;
		break;
	case UNOPR_TO_CLOSURE:
		targetop = subexpr_type != RAVI_TFUNCTION ? op_toclosure : op_nop;
		break;
	case UNOPR_TO_STRING:
		targetop = subexpr_type != RAVI_TSTRING ? op_tostring : op_nop;
		break;
	case UNOPR_TO_INTARRAY:
		targetop = subexpr_type != RAVI_TARRAYINT ? op_toiarray : op_nop;
		break;
	case UNOPR_TO_NUMARRAY:
		targetop = subexpr_type != RAVI_TARRAYFLT ? op_tofarray : op_nop;
		break;
	case UNOPR_TO_TABLE:
		targetop = subexpr_type != RAVI_TTABLE ? op_totable : op_nop;
		break;
	case UNOPR_TO_TYPE:
		targetop = op_totype;
		break;
	case UNOPR_NOT:
		targetop = op_not;
		break;
	case UNOPR_BNOT:
		targetop = op_bnot;
		break;
	default: {
		char err[100];
		snprintf(err, sizeof err, "unexpected unary op %s", raviX_get_unary_opr_str(op));
		handle_error(proc->linearizer->compiler_state, err);
		break;
	}
	}
	if (targetop == op_nop) {
		return subexpr;
	}
	Instruction *insn = allocate_instruction(proc, targetop, node->line_number);
	Pseudo *target = subexpr;
	Pseudo *tofree1 = NULL;
	if (op == UNOPR_TO_TYPE) {
		const Constant *tname_constant = allocate_string_constant(proc, node->unary_expr.type.type_name);
		Pseudo *tname_pseudo = allocate_constant_pseudo(proc, tname_constant);
		tofree1 = add_instruction_operand(proc, insn, tname_pseudo);
		assert(!tofree1);
	} else if (op == UNOPR_NOT || op == UNOPR_BNOT) {
		tofree1 = add_instruction_operand(proc, insn, target);
		free_temp_pseudo(proc, target, false); //CHECK
		target = allocate_temp_pseudo(proc, RAVI_TANY, false);
	} else if (op == UNOPR_MINUS || op == UNOPR_LEN) {
		tofree1 = add_instruction_operand(proc, insn, target);
		free_temp_pseudo(proc, target, false); //CHECK
		target = allocate_temp_pseudo(proc, subexpr_type, false);
	}
	/* unary ops set their operand in the target so we need to check here that the
	 * operand is not in pending state.
	 */
	if (target->type == PSEUDO_INDEXED) {
		target = indexed_load(proc, target);
	}
	add_instruction_target(proc, insn, target);
	add_instruction(proc, insn);
	if (tofree1)
		free_temp_pseudo(proc, tofree1, false);

	if (targetop == op_toint || targetop == op_toflt) {
		Pseudo *tofree2 = NULL;
		insn = allocate_instruction(proc, op_mov, node->line_number);
		tofree2 = add_instruction_operand(proc, insn, target);
		free_temp_pseudo(proc, target, false); //CHECK
		target = allocate_temp_pseudo(proc, targetop == op_toint ? RAVI_TNUMINT: RAVI_TNUMFLT, false);
		add_instruction_target(proc, insn, target);
		add_instruction(proc, insn);
		if (tofree2)
			free_temp_pseudo(proc, tofree2, false);
	}
	return target;
}

static Pseudo *instruct_move(Proc *proc, enum opcode op, Pseudo *target, Pseudo *src, unsigned line_number)
{
	Instruction *mov = allocate_instruction(proc, op, line_number);
	Pseudo *tofree = add_instruction_operand(proc, mov, src);
	add_instruction_target(proc, mov, target);
	add_instruction(proc, mov);
	if (tofree)
		free_temp_pseudo(proc, tofree, false);
	return target;
}

static void instruct_cbr(Proc *proc, Pseudo *condition_pseudo, BasicBlock *true_block,
			 BasicBlock *false_block, unsigned line_number)
{
	Pseudo *true_pseudo = allocate_block_pseudo(proc, true_block);
	Pseudo *false_pseudo = allocate_block_pseudo(proc, false_block);
	Instruction *insn = allocate_instruction(proc, op_cbr, line_number);
	Pseudo *tofree = add_instruction_operand(proc, insn, condition_pseudo);
	add_instruction_target(proc, insn, true_pseudo);
	add_instruction_target(proc, insn, false_pseudo);
	add_instruction(proc, insn);
	if (tofree)
		free_temp_pseudo(proc, tofree, false);
}

static void instruct_br(Proc *proc, Pseudo *pseudo, unsigned line_number)
{
	assert(pseudo->type == PSEUDO_BLOCK);
	if (is_block_terminated(proc->current_bb)) {
		start_block(proc, create_block(proc), line_number);
	}
	Instruction *insn = allocate_instruction(proc, op_br, line_number);
	add_instruction_target(proc, insn, pseudo);
	add_instruction(proc, insn);
}

// clang-format off
/*
Lua and/or operators are processed so that with 'and' the result is the final 
true value, and with 'or' it is the first true value.

and IR

	result = eval(expr_left);
	if (result)
		goto Lnext:
	else
		goto Ldone;
Lnext:
	result = eval(expr_right);
	goto Ldone;
Ldone:

or IR

	result = eval(expr_left);
	if (result)
		goto Ldone:
	else
		goto Lnext;
Lnext:
	result = eval(expr_right);
	goto Ldone;
Ldone:

*/
// clang-format on
static Pseudo *linearize_bool(Proc *proc, AstNode *node, bool is_and)
{
	AstNode *e1 = node->binary_expr.expr_left;
	AstNode *e2 = node->binary_expr.expr_right;

	BasicBlock *first_block = create_block(proc);
	BasicBlock *end_block = create_block(proc);

	// leave the result register on top of virtual stack
	Pseudo *result = allocate_temp_pseudo(proc, RAVI_TANY, true);
	Pseudo *operand1 = linearize_expression(proc, e1);
	instruct_move(proc, op_mov, result, operand1, node->line_number);
	free_temp_pseudo(proc, operand1, false);
	if (is_and)
		instruct_cbr(proc, result, first_block, end_block, node->line_number); // If first value is true then evaluate the second
	else
		instruct_cbr(proc, result, end_block, first_block, node->line_number);

	start_block(proc, first_block, node->line_number);
	Pseudo *operand2 = linearize_expression(proc, e2);
	instruct_move(proc, op_mov, result, operand2, node->line_number);
	free_temp_pseudo(proc, operand2, false);
	instruct_br(proc, allocate_block_pseudo(proc, end_block), node->line_number);

	start_block(proc, end_block, node->line_number);
	return result;
}

/* Utility to create a binary instruction where operands and target pseudo is known */
static void create_binary_instruction(Proc *proc, enum opcode targetop, Pseudo *operand1,
				      Pseudo *operand2, Pseudo *target, unsigned line_number)
{
	Instruction *insn = allocate_instruction(proc, targetop, line_number);
	Pseudo *tofree1 = add_instruction_operand(proc, insn, operand1);
	Pseudo *tofree2 = add_instruction_operand(proc, insn, operand2);
	add_instruction_target(proc, insn, target);
	add_instruction(proc, insn);
	if (tofree1)
		free_temp_pseudo(proc, tofree1, false);
	if (tofree2)
		free_temp_pseudo(proc, tofree2, false);
}

static Pseudo *linearize_binary_operator(Proc *proc, AstNode *node)
{
	BinaryOperatorType op = node->binary_expr.binary_op;

	if (op == BINOPR_AND) {
		return linearize_bool(proc, node, true);
	} else if (op == BINOPR_OR) {
		return linearize_bool(proc, node, false);
	}

	// TODO check the type is correct for BINOPR_NE
	ravitype_t target_type = node->binary_expr.type.type_code;
	Pseudo *not_target = op == BINOPR_NE ? allocate_temp_pseudo(proc, target_type, true) : NULL;
	Pseudo *target = allocate_temp_pseudo(proc, target_type, true);

	AstNode *e1 = node->binary_expr.expr_left;
	AstNode *e2 = node->binary_expr.expr_right;
	Pseudo *operand1 = linearize_expression(proc, e1);
	Pseudo *operand2 = linearize_expression(proc, e2);

	int targetop;
	switch (op) {
	case BINOPR_ADD:
		targetop = op_add;
		break;
	case BINOPR_SUB:
		targetop = op_sub;
		break;
	case BINOPR_MUL:
		targetop = op_mul;
		break;
	case BINOPR_DIV:
		targetop = op_div;
		break;
	case BINOPR_IDIV:
		targetop = op_idiv;
		break;
	case BINOPR_BAND:
		targetop = op_band;
		break;
	case BINOPR_BOR:
		targetop = op_bor;
		break;
	case BINOPR_BXOR:
		targetop = op_bxor;
		break;
	case BINOPR_SHL:
		targetop = op_shl;
		break;
	case BINOPR_SHR:
		targetop = op_shr;
		break;
	case BINOPR_EQ:
	case BINOPR_NE:
		targetop = op_eq;
		break;
	case BINOPR_LT:
	case BINOPR_GT:
		targetop = op_lt;
		break;
	case BINOPR_LE:
	case BINOPR_GE:
		targetop = op_le;
		break;
	case BINOPR_MOD:
		targetop = op_mod;
		break;
	case BINOPR_POW:
		targetop = op_pow;
		break;
	case BINOPR_CONCAT:
		targetop = op_concat;
		break;
	default: {
		char err[100];
		snprintf(err, sizeof err, "unexpected binary op %s", raviX_get_binary_opr_str(op));
		handle_error(proc->linearizer->compiler_state, err);
		targetop = op_nop;
		break;
	}
	}

	ravitype_t t1 = e1->common_expr.type.type_code;
	ravitype_t t2 = e2->common_expr.type.type_code;

	bool swap = false;
	switch (targetop) {
	case op_add:
	case op_mul:
		swap = t1 == RAVI_TNUMINT && t2 == RAVI_TNUMFLT;
		break;
	case op_lt:
	case op_le:
		swap = op == BINOPR_GT || op == BINOPR_GE;
		break;
	default:
		break;
	}

	if (swap) {
		Pseudo *temp;
		AstNode *ntemp;
		temp = operand1;
		operand1 = operand2;
		operand2 = temp;
		ntemp = e1;
		e1 = e2;
		e2 = ntemp;
		t1 = e1->common_expr.type.type_code;
		t2 = e2->common_expr.type.type_code;
	}

	switch (targetop) {
	case op_add:
	case op_mul:
		if (t1 == RAVI_TNUMFLT && t2 == RAVI_TNUMFLT)
			targetop += 1;
		else if (t1 == RAVI_TNUMFLT && t2 == RAVI_TNUMINT)
			targetop += 2;
		else if (t1 == RAVI_TNUMINT && t2 == RAVI_TNUMINT)
			targetop += 3;
		break;
	case op_div:
	case op_sub:
		if (t1 == RAVI_TNUMFLT && t2 == RAVI_TNUMFLT)
			targetop += 1;
		else if (t1 == RAVI_TNUMFLT && t2 == RAVI_TNUMINT)
			targetop += 2;
		else if (t1 == RAVI_TNUMINT && t2 == RAVI_TNUMFLT)
			targetop += 3;
		else if (t1 == RAVI_TNUMINT && t2 == RAVI_TNUMINT)
			targetop += 4;
		break;
	case op_band:
	case op_bor:
	case op_bxor:
	case op_shl:
	case op_shr:
		if (t1 == RAVI_TNUMINT && t2 == RAVI_TNUMINT)
			targetop += 1;
		break;
	case op_eq:
	case op_le:
	case op_lt:
		if (t1 == RAVI_TNUMINT && t2 == RAVI_TNUMINT)
			targetop += 1;
		else if (t1 == RAVI_TNUMFLT && t2 == RAVI_TNUMFLT)
			targetop += 2;
		break;
	default:
		break;
	}

	free_temp_pseudo(proc, operand1, false);//CHECK
	free_temp_pseudo(proc, operand2, false);//CHECK
	create_binary_instruction(proc, (enum opcode) targetop, operand1, operand2, target, node->line_number);
	if (op == BINOPR_NE) {
		Instruction *not_insn = allocate_instruction(proc, op_not, node->line_number);
		Pseudo *tofree = add_instruction_operand(proc, not_insn, target);
		free_temp_pseudo(proc, target, false);//CHECK
		target = not_target;
		add_instruction_target(proc, not_insn, target);
		add_instruction(proc, not_insn);
		if (tofree)
			free_temp_pseudo(proc, tofree, false);
	}
	return target;
}

/* The concat opcode attempts to help the efficiency of concatenation
 */
static Pseudo *linearize_concat_expression(Proc *proc, AstNode *expr)
{
	Instruction *insn = allocate_instruction(proc, op_concat, expr->line_number);
	ravitype_t target_type = expr->string_concatenation_expr.type.type_code;
	Pseudo *target = allocate_temp_pseudo(proc, target_type, true);
	AstNode *n;
	int N = raviX_ptrlist_size((const PtrList *) expr->string_concatenation_expr.expr_list);
	Pseudo **tofreelist = (Pseudo **)alloca( N* sizeof(Pseudo *));
	Pseudo **operands = (Pseudo **)alloca( N * sizeof(Pseudo *));
	int i = 0;
	FOR_EACH_PTR(expr->string_concatenation_expr.expr_list, AstNode, n) {
		Pseudo *operand = linearize_expression(proc, n);
		Pseudo *tofree =  add_instruction_operand(proc, insn, operand);
		tofreelist[i] = tofree;
		operands[i] = operand;
		i++;
	}
	END_FOR_EACH_PTR(n)
	add_instruction_target(proc, insn, target);
	add_instruction(proc, insn);
	for (i=0; i < N; i++) {
		Pseudo *op = operands[i];
		free_temp_pseudo(proc, op, false);
		Pseudo *tofree = tofreelist[i];
		if (tofree)
			free_temp_pseudo(proc, tofree, false);
	}
	return target;
}

/* generates closure instruction - linearizes a Proc, and then adds instruction to create closure from it */
static Pseudo *linearize_function_expr(Proc *proc, AstNode *expr)
{
	Proc *curproc = proc->linearizer->current_proc;
	Proc *newproc = allocate_proc(proc->linearizer, expr);
	set_current_proc(proc->linearizer, newproc);
	linearize_function(proc->linearizer);
	set_current_proc(proc->linearizer, curproc); // restore the proc
	ravitype_t target_type = expr->function_expr.type.type_code;
	Pseudo *target = allocate_temp_pseudo(proc, target_type, true);
	Pseudo *operand = allocate_closure_pseudo(newproc);
	Instruction *insn = allocate_instruction(proc, op_closure, expr->line_number);
	Pseudo *tofree = add_instruction_operand(proc, insn, operand);
	add_instruction_target(proc, insn, target);
	add_instruction(proc, insn);
	if (tofree)
		free_temp_pseudo(proc, tofree, false);
	return target;
}

/* Create a deferred pseudo (i.e. reg not allocated yet) for a global var access (i.e. _ENV) */
static Pseudo *create_global_indexed_pseudo(Proc *proc, Pseudo *container_pseudo, Pseudo *key_pseudo, unsigned line_number)
{
	assert(container_pseudo->type != PSEUDO_INDEXED);
	assert(key_pseudo->type != PSEUDO_INDEXED);

	Pseudo *index_pseudo = allocate_indexed_pseudo(proc);
	index_pseudo->index_info.line_number = line_number;
	index_pseudo->index_info.container_type = RAVI_TANY;
	index_pseudo->index_info.key_type = RAVI_TSTRING;
	index_pseudo->index_info.container = container_pseudo;
	index_pseudo->index_info.key = key_pseudo;
	index_pseudo->index_info.target_type = RAVI_TANY;
	index_pseudo->index_info.is_global = 1;
	return index_pseudo;
}

/* Create a deferred pseudo (i.e. reg not allocated yet) for a array/table access; we don't know at this
 * point if it will be a load or store
 */
static Pseudo *create_indexed_pseudo(Proc *proc, ravitype_t container_type,
				     Pseudo *container_pseudo, ravitype_t key_type,
				     Pseudo *key_pseudo, ravitype_t target_type, unsigned line_number)
{
	assert(container_pseudo->type != PSEUDO_INDEXED);
	assert(key_pseudo->type != PSEUDO_INDEXED);

	Pseudo *index_pseudo = allocate_indexed_pseudo(proc);
	index_pseudo->index_info.line_number = line_number;
	index_pseudo->index_info.container_type = container_type;
	index_pseudo->index_info.key_type = key_type;
	index_pseudo->index_info.target_type = target_type;
	index_pseudo->index_info.container = container_pseudo;
	index_pseudo->index_info.key = key_pseudo;
	index_pseudo->index_info.is_global = 0;
	return index_pseudo;
}

static Pseudo *linearize_symbol_expression(Proc *proc, AstNode *expr)
{
	LuaSymbol *sym = expr->symbol_expr.var;
	if (sym->symbol_type == SYM_GLOBAL) {
		assert(sym->variable.env);
		const Constant *constant = allocate_string_constant(proc, sym->variable.var_name);
		Pseudo *operand_varname = allocate_constant_pseudo(proc, constant);
		Pseudo* operand_env = allocate_symbol_pseudo(proc, sym->variable.env, 0); // no register
		return create_global_indexed_pseudo(proc, operand_env, operand_varname, expr->line_number);
	} else if (sym->symbol_type == SYM_LOCAL) {
		return sym->variable.pseudo;
	} else if (sym->symbol_type == SYM_UPVALUE) {
		/* upvalue index is the position of upvalue in the function, we treat this as the pseudo register for
		 * the upvalue */
		return allocate_symbol_pseudo(proc, sym, sym->upvalue.upvalue_index);
	} else {
		handle_error(proc->linearizer->compiler_state, "feature not yet implemented");
		return NULL;
	}
}

static Pseudo *indexed_load_from_global(Proc *proc, Pseudo *index_pseudo)
{
	Pseudo *container_pseudo = index_pseudo->index_info.container;
	Pseudo *key_pseudo = index_pseudo->index_info.key;

	assert(container_pseudo->type != PSEUDO_INDEXED);
	assert(key_pseudo->type != PSEUDO_INDEXED);

	Pseudo *target = allocate_temp_pseudo(proc, RAVI_TANY, true);
	Instruction *insn = allocate_instruction(proc, op_loadglobal, index_pseudo->index_info.line_number);

	Pseudo *tofree1 = add_instruction_operand(proc, insn, container_pseudo);
	Pseudo *tofree2 = add_instruction_operand(proc, insn, key_pseudo);
	add_instruction_target(proc, insn, target);
	add_instruction(proc, insn);
	free_temp_pseudo(proc, container_pseudo, false);
	free_temp_pseudo(proc, key_pseudo, false);
	index_pseudo->index_info.used = 1;
	if (tofree1)
		free_temp_pseudo(proc, tofree1, false);
	if (tofree2)
		free_temp_pseudo(proc, tofree2, false);
	return target;
}

static Pseudo *indexed_load(Proc *proc, Pseudo *index_pseudo)
{
	assert(index_pseudo->type == PSEUDO_INDEXED);
	assert(!index_pseudo->index_info.used);
	if (index_pseudo->index_info.is_global)
		return indexed_load_from_global(proc, index_pseudo);

	ravitype_t container_type = index_pseudo->index_info.container_type;
	ravitype_t key_type = index_pseudo->index_info.key_type;
	ravitype_t target_type = index_pseudo->index_info.target_type;
	unsigned line_number = index_pseudo->index_info.line_number;
	Pseudo *container_pseudo = index_pseudo->index_info.container;
	Pseudo *key_pseudo = index_pseudo->index_info.key;

	assert(container_pseudo->type != PSEUDO_INDEXED);
	assert(key_pseudo->type != PSEUDO_INDEXED);

	int op = op_get;
	switch (container_type) {
	case RAVI_TTABLE:
		op = op_tget;
		break;
	case RAVI_TARRAYINT:
		op = op_iaget;
		break;
	case RAVI_TARRAYFLT:
		op = op_faget;
		break;
	default:
		break;
	}
	/* Note we rely upon ordering of enums here */
	switch (key_type) {
	case RAVI_TNUMINT:
		op++;
		break;
	case RAVI_TSTRING:
		assert(container_type != RAVI_TARRAYINT && container_type != RAVI_TARRAYFLT);
		op += 2;
		break;
	default:
		break;
	}
	Instruction *insn = allocate_instruction(proc, (enum opcode)op, line_number);
	Pseudo *tofree1 = add_instruction_operand(proc, insn, container_pseudo);
	Pseudo *tofree2 = add_instruction_operand(proc, insn, key_pseudo);
	free_temp_pseudo(proc, container_pseudo, false);
	free_temp_pseudo(proc, key_pseudo, false);
	index_pseudo->index_info.used = 1;
	if (tofree1)
		free_temp_pseudo(proc, tofree1, false);
	if (tofree2)
		free_temp_pseudo(proc, tofree2, false);
	Pseudo *target_pseudo = allocate_temp_pseudo(proc, target_type, true);
	add_instruction_target(proc, insn, target_pseudo);
	add_instruction(proc, insn);
	return target_pseudo;
}

static void instruct_indexed_store(Proc *proc, ravitype_t table_type, Pseudo *table,
				   Pseudo *index_pseudo, ravitype_t index_type, Pseudo *value_pseudo,
				   ravitype_t value_type, unsigned line_number)
{
	// Insert type assertions if needed
	int op;
	switch (table_type) {
	case RAVI_TARRAYINT:
		op = op_iaput;
		if (value_type == RAVI_TNUMINT) {
			op = op_iaput_ival;
		}
		break;
	case RAVI_TARRAYFLT:
		op = op_faput;
		if (value_type == RAVI_TNUMFLT) {
			op = op_faput_fval;
		}
		break;
	default:
		op = table_type == RAVI_TTABLE ? op_tput : op_put;
		if (index_type == RAVI_TNUMINT) {
			op += 1;
		} else if (index_type == RAVI_TSTRING) {
			op += 2;
		}
		break;
	}

	Instruction *insn = allocate_instruction(proc, (enum opcode) op, line_number);
	add_instruction_target(proc, insn, table);
	add_instruction_target(proc, insn, index_pseudo);
	Pseudo *tofree = add_instruction_operand(proc, insn, value_pseudo);
	add_instruction(proc, insn);
	if (tofree)
		free_temp_pseudo(proc, tofree, false);
}

static void indexed_store_to_global(Proc *proc, Pseudo *index_pseudo, Pseudo *value_pseudo,
			  ravitype_t value_type)
{
	Instruction *insn = allocate_instruction(proc, op_storeglobal, index_pseudo->index_info.line_number);
	Pseudo *tofree = add_instruction_operand(proc, insn, value_pseudo);
	add_instruction_target(proc, insn, index_pseudo->index_info.container);
	add_instruction_target(proc, insn, index_pseudo->index_info.key);
	add_instruction(proc, insn);
	if (tofree)
		free_temp_pseudo(proc, tofree, false);
}

static void indexed_store(Proc *proc, Pseudo *index_pseudo, Pseudo *value_pseudo,
				    ravitype_t value_type)
{
	assert(index_pseudo->type == PSEUDO_INDEXED);
	if (index_pseudo->index_info.is_global) {
		indexed_store_to_global(proc, index_pseudo, value_pseudo, value_type);
		return;
	}
	int op;
	ravitype_t table_type = index_pseudo->index_info.container_type;
	ravitype_t index_type = index_pseudo->index_info.key_type;
	switch (table_type) {
	case RAVI_TARRAYINT:
		op = op_iaput;
		if (value_type == RAVI_TNUMINT) {
			op = op_iaput_ival;
		}
		break;
	case RAVI_TARRAYFLT:
		op = op_faput;
		if (value_type == RAVI_TNUMFLT) {
			op = op_faput_fval;
		}
		break;
	default:
		op = table_type == RAVI_TTABLE ? op_tput : op_put;
		if (index_type == RAVI_TNUMINT) {
			op += 1;
		} else if (index_type == RAVI_TSTRING) {
			op += 2;
		}
		break;
	}
	unsigned line_number = index_pseudo->index_info.line_number;
	Pseudo *container_pseudo = index_pseudo->index_info.container;
	Pseudo *key_pseudo = index_pseudo->index_info.key;

	assert(container_pseudo->type != PSEUDO_INDEXED);
	assert(key_pseudo->type != PSEUDO_INDEXED);

	Instruction *insn = allocate_instruction(proc, op, line_number);
	Pseudo *tofree = add_instruction_operand(proc, insn, value_pseudo);
	add_instruction_target(proc, insn, container_pseudo);
	add_instruction_target(proc, insn, key_pseudo);
	add_instruction(proc, insn);
	if (tofree)
		free_temp_pseudo(proc, tofree, false);
}

static Pseudo *instruct_indexed_load(Proc *proc, ravitype_t container_type,
				     Pseudo *container_pseudo, ravitype_t key_type,
				     Pseudo *key_pseudo, ravitype_t target_type, unsigned line_number)
{
	int op = op_get;
	switch (container_type) {
	case RAVI_TTABLE:
		op = op_tget;
		break;
	case RAVI_TARRAYINT:
		op = op_iaget;
		break;
	case RAVI_TARRAYFLT:
		op = op_faget;
		break;
	default:
		break;
	}
	/* Note we rely upon ordering of enums here */
	switch (key_type) {
	case RAVI_TNUMINT:
		op++;
		break;
	case RAVI_TSTRING:
		assert(container_type != RAVI_TARRAYINT && container_type != RAVI_TARRAYFLT);
		op += 2;
		break;
	default:
		break;
	}
	Instruction *insn = allocate_instruction(proc, (enum opcode)op, line_number);
	Pseudo *tofree1 = add_instruction_operand(proc, insn, container_pseudo);
	Pseudo *tofree2 = add_instruction_operand(proc, insn, key_pseudo);
	if (tofree1)
		free_temp_pseudo(proc, tofree1, false);
	if (tofree2)
		free_temp_pseudo(proc, tofree2, false);
	Pseudo *target_pseudo = allocate_temp_pseudo(proc, target_type, true);
	add_instruction_target(proc, insn, target_pseudo);
	add_instruction(proc, insn);
	return target_pseudo;
}

/**
 * Lua function calls can return multiple values, and the caller decides how many values to accept.
 * We indicate multiple values using a PSEUDO_RANGE.
 * We also handle method call:
 * <pseudo>:name(...) -> is translated to <pseudo>.name(<pseudo>, ...)
 */
static Pseudo *linearize_function_call_expression(Proc *proc, AstNode *expr,
							 AstNode *callsite_expr, Pseudo *callsite_pseudo)
{
	Instruction *insn = allocate_instruction(proc, op_call, expr->line_number);

	Pseudo *self_arg = NULL; /* For method call */
	if (expr->function_call_expr.method_name) {
		const Constant *name_constant =
		    allocate_string_constant(proc, expr->function_call_expr.method_name);
		Pseudo *name_pseudo = allocate_constant_pseudo(proc, name_constant);
		self_arg = callsite_pseudo; /* The original callsite must be passed as 'self' */
		/* create new call site as callsite[name] */
		callsite_pseudo = instruct_indexed_load(proc, callsite_expr->common_expr.type.type_code,
							callsite_pseudo, RAVI_TSTRING, name_pseudo, RAVI_TANY, expr->line_number);
	}

	// Must move call site if necessary after all args etc are evaluated as
	// the evaluations may use temp registers
	// args are pushed to callsite stack by the codegen
	// callsite needs to be at the top of the stack and also must be a temp
	if (callsite_pseudo->type != PSEUDO_TEMP_ANY) {
		Pseudo *temp = allocate_temp_pseudo(proc, RAVI_TANY, true);
		instruct_move(proc, op_mov, temp, callsite_pseudo, expr->line_number);
		callsite_pseudo = temp;
	}

	Pseudo *tofree1 = add_instruction_operand(proc, insn, callsite_pseudo);
	Pseudo *tofree2 = NULL;
	if (self_arg) {
		tofree2 = add_instruction_operand(proc, insn, self_arg);
	}

	AstNode *arg;
	int argc = raviX_ptrlist_size((const PtrList *)expr->function_call_expr.arg_list);
	Pseudo **tofreelist = (Pseudo **) alloca(argc * sizeof(Pseudo *));
	int argi = 0;
	FOR_EACH_PTR(expr->function_call_expr.arg_list, AstNode, arg)
	{
		argc -= 1;
		Pseudo *arg_pseudo = linearize_expression(proc, arg);
		if (argc != 0 && arg_pseudo->type == PSEUDO_RANGE) {
			// Not last one, so range can only be 1
			convert_range_to_temp(arg_pseudo);
		}
		tofreelist[argi++] = add_instruction_operand(proc, insn, arg_pseudo);
	}
	END_FOR_EACH_PTR(arg)

	Pseudo *return_pseudo = allocate_range_pseudo(
	    proc, callsite_pseudo); /* Base reg for function call - where return values will be placed */
	add_instruction_target(proc, insn, return_pseudo);
	add_instruction_target(proc, insn, allocate_constant_pseudo(proc, allocate_integer_constant(proc, expr->function_call_expr.num_results)));
	add_instruction(proc, insn);

	free_instruction_operand_pseudos(proc, insn);
	if (tofree1)
		free_temp_pseudo(proc, tofree1, false);
	if (tofree2)
		free_temp_pseudo(proc, tofree2, false);
	for (int i = 0; i < argi; i++) {
		Pseudo *tofree = tofreelist[i];
		if (tofree)
			free_temp_pseudo(proc, tofree, false);
	}
	return return_pseudo;
}

/*
 * Suffixed expression examples:
 * f()[1]
 * x[1][2]
 * x.y[1]
 *
 * The result type of a suffixed expression may initially be an indexed load, but when used in the context of
 * an assignment statement the load will be converted to a store.
 * Lua parser does this by creating a VINDEXED node which is only converted to load/store
 * when the VINDEXED node is used.
 */
static Pseudo *linearize_suffixedexpr(Proc *proc, AstNode *node)
{
	/* suffixedexp -> primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs } */
	Pseudo *prev_pseudo = linearize_expression(proc, node->suffixed_expr.primary_expr);
	AstNode *prev_node = node->suffixed_expr.primary_expr;
	AstNode *this_node;
	FOR_EACH_PTR(node->suffixed_expr.suffix_list, AstNode, this_node)
	{
		Pseudo *next;
		if (prev_pseudo->type == PSEUDO_RANGE)
			convert_range_to_temp(prev_pseudo);
		else if (prev_pseudo->type == PSEUDO_INDEXED)
			prev_pseudo = indexed_load(proc, prev_pseudo);
		if (this_node->type == EXPR_Y_INDEX || this_node->type == EXPR_FIELD_SELECTOR) {
			Pseudo *key_pseudo = linearize_expression(proc, this_node->index_expr.expr);
			ravitype_t key_type = this_node->index_expr.expr->common_expr.type.type_code;
			if (key_pseudo->type == PSEUDO_INDEXED)
				key_pseudo = indexed_load(proc, key_pseudo);
			next = create_indexed_pseudo(proc, prev_node->common_expr.type.type_code, prev_pseudo, key_type,
						     key_pseudo, this_node->common_expr.type.type_code, node->line_number);
		} else if (this_node->type == EXPR_FUNCTION_CALL) {
			next = linearize_function_call_expression(proc, this_node, prev_node, prev_pseudo);
		} else {
			next = NULL;
			handle_error(proc->linearizer->compiler_state, "Unexpected expr type in suffix list");
		}
		prev_node = this_node;
		prev_pseudo = next;
	}
	END_FOR_EACH_PTR(node)
	return prev_pseudo;
}

static int linearize_indexed_assign(Proc *proc, Pseudo *table, ravitype_t table_type,
				    AstNode *expr, int next)
{
	Pseudo *index_pseudo;
	ravitype_t index_type;
	if (expr->table_elem_assign_expr.key_expr) {
		index_pseudo = linearize_expression(proc, expr->table_elem_assign_expr.key_expr);
		index_type = expr->table_elem_assign_expr.key_expr->index_expr.expr->common_expr.type.type_code;
		// TODO check valid index
	} else {
		const Constant *constant = allocate_integer_constant(proc, next++);
		index_pseudo = allocate_constant_pseudo(proc, constant);
		index_type = RAVI_TNUMINT;
	}
	Pseudo *value_pseudo = linearize_expression(proc, expr->table_elem_assign_expr.value_expr);
	ravitype_t value_type = expr->table_elem_assign_expr.value_expr->common_expr.type.type_code;
	instruct_indexed_store(proc, table_type, table, index_pseudo, index_type, value_pseudo, value_type, expr->line_number);
	free_temp_pseudo(proc, index_pseudo, false);
	free_temp_pseudo(proc, value_pseudo, false);
	return next;
}

static Pseudo *linearize_table_constructor(Proc *proc, AstNode *expr)
{
	/* constructor -> '{' [ field { sep field } [sep] ] '}' where sep -> ',' | ';' */
	Pseudo *target = allocate_temp_pseudo(proc, expr->table_expr.type.type_code, false);
	enum opcode op = op_newtable;
	if (expr->table_expr.type.type_code == RAVI_TARRAYINT)
		op = op_newiarray;
	else if (expr->table_expr.type.type_code == RAVI_TARRAYFLT)
		op = op_newfarray;
	Instruction *insn = allocate_instruction(proc, op, expr->line_number);
	add_instruction_target(proc, insn, target);
	add_instruction(proc, insn);

	AstNode *ia;
	int i = 1;
	FOR_EACH_PTR(expr->table_expr.expr_list, AstNode, ia)
	{
		i = linearize_indexed_assign(proc, target, expr->table_expr.type.type_code, ia, i);
	}
	END_FOR_EACH_PTR(ia)

	return target;
}

/* Used for local variable declarations without an initializer.
 * The Ravi parser doesn't allow this; it requires explicit table
 * constructor. So this is an enhancement.
 */
static Pseudo *linearize_table_constructor_inplace(Proc *proc, Pseudo *target, ravitype_t type_code, unsigned line_number)
{
	enum opcode op = op_newtable;
	if (type_code == RAVI_TARRAYINT)
		op = op_newiarray;
	else if (type_code == RAVI_TARRAYFLT)
		op = op_newfarray;
	Instruction *insn = allocate_instruction(proc, op, line_number);
	add_instruction_target(proc, insn, target);
	add_instruction(proc, insn);
	return target;
}

/** Is the type NIL-able */
static bool is_nillable(const VariableType *var_type)
{
	return var_type->type_code != RAVI_TARRAYFLT && var_type->type_code != RAVI_TARRAYINT &&
	       var_type->type_code != RAVI_TNUMFLT && var_type->type_code != RAVI_TNUMINT;
}

/* Check if we can assign value to variable */
static bool is_compatible(const VariableType *var_type, const VariableType *val_type)
{
	if (var_type->type_code == RAVI_TANY)
		return true;
	if (is_nillable(var_type) && val_type->type_code == RAVI_TNIL)
		return true;
	if (val_type->type_code == var_type->type_code && val_type->type_name == var_type->type_name)
		return true;
	if ((var_type->type_code == RAVI_TNUMFLT && val_type->type_code == RAVI_TNUMINT) ||
	    (var_type->type_code == RAVI_TNUMINT && val_type->type_code == RAVI_TNUMFLT))
		/* Maybe conversion is possible so allow */
		return true;
	return false;
}

static void linearize_store_var(Proc *proc, const VariableType *var_type, Pseudo *var_pseudo,
				const VariableType *val_type, Pseudo *val_pseudo, unsigned line_number)
{
	if (var_pseudo->type == PSEUDO_INDEXED) {
		indexed_store(proc, var_pseudo, val_pseudo, val_type->type_code);
	}
	else {
		assert(var_type->type_code != RAVI_TVARARGS && var_type->type_code != RAVI_TNIL);
		if (!is_compatible(var_type, val_type)) {
			instruct_totype(proc, val_pseudo, var_type, line_number);
			val_type = var_type; // Because of the type assertion!
		}
		enum opcode op = op_mov;
		if (var_type->type_code == RAVI_TNUMINT) {
			op = val_type->type_code == RAVI_TNUMINT ? op_movi : op_movfi;
		} else if (var_type->type_code == RAVI_TNUMFLT) {
			op = val_type->type_code == RAVI_TNUMFLT ? op_movf : op_movif;
		}
		instruct_move(proc, op, var_pseudo, val_pseudo, line_number);
	}
}

static ravitype_t get_type(LuaSymbol *symbol) {
	ravitype_t t = RAVI_TANY;
	if (symbol->symbol_type == SYM_LOCAL) {
		if (symbol->variable.value_type.type_code == RAVI_TNUMINT ||
		    symbol->variable.value_type.type_code == RAVI_TNUMFLT)
			t = symbol->variable.value_type.type_code;
	}
	else if (symbol->symbol_type == SYM_UPVALUE) {
		if (symbol->upvalue.value_type.type_code == RAVI_TNUMINT ||
		    symbol->upvalue.value_type.type_code == RAVI_TNUMFLT)
			t = symbol->upvalue.value_type.type_code;
	}
	return t;
}

static Pseudo *copy_to_temp_if_necessary(Proc *proc, Pseudo *original, unsigned line_number) {
	if (original->type == PSEUDO_SYMBOL) {
		Pseudo *copy = allocate_temp_pseudo(proc, get_type(original->symbol), false);
		instruct_move(proc, op_mov, copy, original, line_number);
		// TODO we may need to set type more specifically
		return copy;
	}
	return original;
}

static void linearize_init(Proc *proc, Pseudo *pseudo, unsigned line_number)
{
	Instruction *insn = allocate_instruction(proc, op_init, line_number);
	add_instruction_target(proc, insn, pseudo);
	add_instruction(proc, insn);
}

struct node_info {
	const VariableType *vartype;
	Pseudo *pseudo;
};

static void linearize_assignment(Proc *proc, AstNodeList *expr_list, struct node_info *varinfo, int nv)
{
	AstNode *expr;

	// Process RHS expressions
	int ne = raviX_ptrlist_size((const PtrList *)expr_list);
	struct node_info *valinfo = (struct node_info *)alloca(ne * sizeof(struct node_info));
	Pseudo *last_val_pseudo = NULL;
	int i = 0;
	unsigned line_number = 0;
	FOR_EACH_PTR(expr_list, AstNode, expr)
	{
		line_number = line_number == 0 ? expr->line_number : line_number;
		Pseudo *val_pseudo = last_val_pseudo = linearize_expression(proc, expr);
		if (val_pseudo->type == PSEUDO_INDEXED)
			val_pseudo = indexed_load(proc, val_pseudo);
		valinfo[i].vartype = &expr->common_expr.type;
		// To support the Lua semantics of x,y = y,x as a swap
		// we need to copy locals to temps
		// TODO can this be optimized to only do when needed
		// See also check_conflict() in Lua parser
		valinfo[i].pseudo = copy_to_temp_if_necessary(proc, val_pseudo, expr->line_number);
		i++;
		if (i < ne && val_pseudo->type == PSEUDO_RANGE) {
			convert_range_to_temp(val_pseudo);
		}
	}
	END_FOR_EACH_PTR(expr)

	/* TODO do we need to insert type assertions in some cases such as function return values ? */

	int note_ne = ne;
	// nv = number of variables (LHS)
	// ne = number of expressions (RHS)
	for (i = 0; i < nv; i++) {
		if (i >= ne) {
			// Went past ne
			if (last_val_pseudo != NULL && last_val_pseudo->type == PSEUDO_RANGE) {
				int pick = i - ne + 1;
				linearize_store_var(proc, varinfo[i].vartype, varinfo[i].pseudo,
						    valinfo[ne-1].vartype,
				    raviX_allocate_range_select_pseudo(proc, last_val_pseudo, pick), line_number);
			} else {
				if (varinfo[i].vartype->type_code == RAVI_TTABLE ||
				    varinfo[i].vartype->type_code == RAVI_TARRAYFLT ||
				    varinfo[i].vartype->type_code == RAVI_TARRAYINT) {
					linearize_table_constructor_inplace(proc, varinfo[i].pseudo, varinfo[i].vartype->type_code, line_number);
				}
				else {
					linearize_init(proc, varinfo[i].pseudo, line_number);
				}
			}
		}
		else {
			Pseudo *range_pseudo = NULL;
			if (valinfo[i].pseudo->type == PSEUDO_RANGE) {
				/* Only the topmost expression can be a range ... assert */
				assert(i == note_ne-1);
				range_pseudo = valinfo[i].pseudo;
				valinfo[i].pseudo = raviX_allocate_range_select_pseudo(proc, valinfo[i].pseudo, 0);
			}
			linearize_store_var(proc, varinfo[i].vartype, varinfo[i].pseudo,
					    valinfo[i].vartype, valinfo[i].pseudo, line_number);
			free_temp_pseudo(proc, valinfo[i].pseudo, false);
			if (range_pseudo) {
				free_temp_pseudo(proc, range_pseudo, false);
			}
		}
	}

	for (int i = ne-1; i >= 0; i--) {
		free_temp_pseudo(proc, valinfo[i].pseudo, false);
	}
	for (int i = nv-1; i >= 0; i--) {
		free_temp_pseudo(proc, varinfo[i].pseudo, false);
	}
	if (last_val_pseudo != NULL && last_val_pseudo->type == PSEUDO_RANGE) {
		free_temp_pseudo(proc, last_val_pseudo, false);
	}
	return;
}

/*
Expression or assignment statement is of the form:

<LHS exp list...> = <RHS exp list...>

Lua requires some special handling of this statement. Firstly
the LHS expressions are evaluated left to right.

The RHS is processed right to left. If there is a corresponding LHS expr
then we need to assign the value of the RHS expr to the LHS expr.
Excess RHS expression results are discarded.
Excess LHS expressions have to be set to the default value.

So for example if we had:

expr1, expr2 = expr3, expr4, expr5

Then following needs to be generated

result1 = eval(expr1)
result2 = eval(expr2)

eval(expr5)
*result2 = eval(expr4)
*result1 = eval(expr3)

Our code generation has an issue:
We initially generate load instructions for LHS expressions.
Subsequently we convert these to store instructions  (marked above with asterisk)

The handling of 'local' and expression statements can be partially combined
because the main difference is the LHS side of it. The rest of the processing has to be
the same.
*/
static void linearize_expression_statement(Proc *proc, AstNode *node)
{
	AstNode *var;

	int nv = raviX_ptrlist_size((const PtrList *)node->expression_stmt.var_expr_list);
	struct node_info *varinfo = (struct node_info *)alloca(nv * sizeof(struct node_info));
	int i = 0;
	FOR_EACH_PTR(node->expression_stmt.var_expr_list, AstNode, var)
	{
		Pseudo *var_pseudo = linearize_expression(proc, var);

		// See test case https://github.com/dibyendumajumdar/ravi/blob/master/tests/comptests/inputs/34_assign.lua
		// We need to ensure that any pending index access in the LHS side is resolved
		// before we evaluate the RHS side
		if (var_pseudo->type == PSEUDO_INDEXED) {
			if (var_pseudo->index_info.container->type == PSEUDO_INDEXED)
				var_pseudo->index_info.container = indexed_load(proc, var_pseudo->index_info.container);
			if (var_pseudo->index_info.key->type == PSEUDO_INDEXED)
				var_pseudo->index_info.key = indexed_load(proc, var_pseudo->index_info.key);
		}

		varinfo[i].vartype = &var->common_expr.type;
		varinfo[i].pseudo = var_pseudo;
		i++;
	}
	END_FOR_EACH_PTR(var)

	linearize_assignment(proc, node->expression_stmt.expr_list, varinfo, nv);
}

static void linearize_local_statement(Proc *proc, AstNode *stmt)
{
	LuaSymbol *sym;

	int nv = raviX_ptrlist_size((const PtrList *)stmt->local_stmt.var_list);
	struct node_info *varinfo = (struct node_info *)alloca(nv * sizeof(struct node_info));
	int i = 0;

	FOR_EACH_PTR(stmt->local_stmt.var_list, LuaSymbol, sym)
	{
		Pseudo *var_pseudo = sym->variable.pseudo;
		assert(var_pseudo);
		varinfo[i].vartype = &sym->variable.value_type;
		varinfo[i].pseudo = var_pseudo;
		i++;
	}
	END_FOR_EACH_PTR(var)

	linearize_assignment(proc, stmt->local_stmt.expr_list, varinfo, nv);
}

static Pseudo *linearize_builtin_expression(Proc *proc, AstNode *expr)
{
	// For now the only built-in is C__new
	if (expr->builtin_expr.token != TOK_C__new) {
		handle_error(proc->linearizer->compiler_state, "feature not yet implemented");
	}
	Instruction *insn = allocate_instruction(proc, op_C__new, expr->line_number);
	const StringObject *type_name = astlist_get(expr->builtin_expr.arg_list, 0)->literal_expr.u.ts;
	Pseudo *tofree1 = add_instruction_operand(proc, insn, allocate_constant_pseudo(proc, allocate_string_constant(proc, type_name)));
	Pseudo *size_expr = linearize_expression(proc, astlist_get(expr->builtin_expr.arg_list, 1));
	Pseudo *tofree2 = add_instruction_operand(proc, insn, size_expr);
	Pseudo *target = allocate_temp_pseudo(proc, RAVI_TUSERDATA, false);
	add_instruction_target(proc, insn, target);
	add_instruction(proc, insn);
	free_temp_pseudo(proc, size_expr, false);
	if (tofree1)
		free_temp_pseudo(proc, tofree1, false);
	if (tofree2)
		free_temp_pseudo(proc, tofree2, false);
	return target;
}

static Pseudo *linearize_expression(Proc *proc, AstNode *expr)
{
	Pseudo *result = NULL;
	switch (expr->type) {
	case EXPR_LITERAL: {
		result = linearize_literal(proc, expr);
	} break;
	case EXPR_BINARY: {
		result = linearize_binary_operator(proc, expr);
	} break;
	case EXPR_FUNCTION: {
		result = linearize_function_expr(proc, expr);
	} break;
	case EXPR_UNARY: {
		result = linearize_unary_operator(proc, expr);
	} break;
	case EXPR_SUFFIXED: {
		result = linearize_suffixedexpr(proc, expr);
	} break;
	case EXPR_SYMBOL: {
		result = linearize_symbol_expression(proc, expr);
	} break;
	case EXPR_TABLE_LITERAL: {
		result = linearize_table_constructor(proc, expr);
	} break;
	case EXPR_Y_INDEX:
	case EXPR_FIELD_SELECTOR: {
		result = linearize_expression(proc, expr->index_expr.expr);
	} break;
	case EXPR_CONCAT: {
		result = linearize_concat_expression(proc, expr);
	} break;
	case EXPR_BUILTIN: {
		result = linearize_builtin_expression(proc, expr);
	} break;
	default:
		handle_error(proc->linearizer->compiler_state, "feature not yet implemented");
		break;
	}
	assert(result);
	if (result->type == PSEUDO_RANGE && expr->common_expr.truncate_results) {
		// Need to truncate the results to 1
		return raviX_allocate_range_select_pseudo(proc, result, 0);
	}
	return result;
}

static void linearize_expr_list(Proc *proc, AstNodeList *expr_list, Instruction *insn,
				PseudoList **pseudo_list)
{
	AstNode *expr;
	int ne = raviX_ptrlist_size((const PtrList *)expr_list);
	FOR_EACH_PTR(expr_list, AstNode, expr)
	{
		ne -= 1;
		Pseudo *pseudo = linearize_expression(proc, expr);
		if (ne != 0 && pseudo->type == PSEUDO_RANGE) {
			convert_range_to_temp(pseudo); // Only accept one result unless it is the last expr
		}
		else if (pseudo->type == PSEUDO_INDEXED) {
			pseudo = indexed_load(proc, pseudo);
		}
		raviX_ptrlist_add((PtrList **)pseudo_list, pseudo, proc->linearizer->compiler_state->allocator);
	}
	END_FOR_EACH_PTR(expr)
}

static void linearize_return(Proc *proc, AstNode *node)
{
	assert(node->type == STMT_RETURN);
	Instruction *insn = allocate_instruction(proc, op_ret, node->line_number);
	linearize_expr_list(proc, node->return_stmt.expr_list, insn, &insn->operands);
	add_instruction_target(proc, insn, allocate_block_pseudo(proc, proc->nodes[EXIT_BLOCK]));
	add_instruction(proc, insn);
	free_instruction_operand_pseudos(proc, insn);
}

/* A block is considered terminated if the last instruction is
   a return or a branch */
static bool is_block_terminated(BasicBlock *block)
{
	Instruction *last_insn = raviX_last_instruction(block);
	if (last_insn == NULL)
		return false;
	if (last_insn->opcode == op_ret || last_insn->opcode == op_cbr || last_insn->opcode == op_br)
		return true;
	return false;
}

static void linearize_test_cond(Proc *proc, AstNode *node, BasicBlock *true_block,
				BasicBlock *false_block)
{
	Pseudo *condition_pseudo = linearize_expression(proc, node->test_then_block.condition);
	instruct_cbr(proc, condition_pseudo, true_block, false_block, node->line_number);
}

/* linearize the 'else if' block */
static void linearize_test_then(Proc *proc, AstNode *node, BasicBlock *true_block,
				BasicBlock *end_block)
{
	start_block(proc, true_block, node->line_number);
	start_scope(proc->linearizer, proc, node->test_then_block.test_then_scope);
	linearize_statement_list(proc, node->test_then_block.test_then_statement_list);
	end_scope(proc->linearizer, proc, node->line_number);
	if (!is_block_terminated(proc->current_bb))
		instruct_br(proc, allocate_block_pseudo(proc, end_block), node->line_number);
}

// clang-format off
/*
The Lua if statement has a complex structure as it is somewhat like
a combination of case and if statement. The if block is followed by
1 or more elseif blocks. Finally we have an optinal else block.
The elseif blocks are like case statements.

Given

if cond1 then
	block for cond1
elseif cond2 then
	block for cond2
else
	block for else
end

We linearize the statement as follows.

B0:
	if cond1 goto Bcond1 else B2;   // Initial if condition

B2:
	if cond2 goto Bcond2 else B3:   // This is an elseif condition

B3:
	<if AST has else>
	goto Belse;
	<else>
	goto Bend;

Bcond1:
	start scope
	block for cond1
	end scope
	goto Bend;

Bcond2:
	start scope
	block for cond2
	end scope
	goto Bend;

Belse:
	start scope
	block for else
	end scope
	goto Bend;

Bend:
*/
// clang-format on
static void linearize_if_statement(Proc *proc, AstNode *ifnode)
{
	BasicBlock *end_block = NULL;
	BasicBlock *else_block = NULL;
	BasicBlockList *if_blocks = NULL;
	BasicBlockList *if_true_blocks = NULL;
	AstNodeList *if_else_stmts = ifnode->if_stmt.if_condition_list;
	AstNodeList *else_stmts = ifnode->if_stmt.else_statement_list;
	Scope *else_scope = ifnode->if_stmt.else_block;

	AstNode *this_node;
	FOR_EACH_PTR(if_else_stmts, AstNode, this_node)
	{
		BasicBlock *block = create_block(proc);
		raviX_ptrlist_add((PtrList **)&if_blocks, block, proc->linearizer->compiler_state->allocator);
	}
	END_FOR_EACH_PTR(this_node)

	FOR_EACH_PTR(if_else_stmts, AstNode, this_node)
	{
		BasicBlock *block = create_block(proc);
		raviX_ptrlist_add((PtrList **)&if_true_blocks, block, proc->linearizer->compiler_state->allocator);
	}
	END_FOR_EACH_PTR(this_node)

	if (ifnode->if_stmt.else_statement_list) {
		else_block = create_block(proc);
	}

	end_block = create_block(proc);

	BasicBlock *true_block = NULL;
	BasicBlock *false_block = NULL;
	BasicBlock *block = NULL;

	{
		PREPARE_PTR_LIST(if_blocks, BasicBlock, block);
		PREPARE_PTR_LIST(if_true_blocks, BasicBlock, true_block);
		FOR_EACH_PTR(if_else_stmts, AstNode, this_node)
		{
			start_block(proc, block, this_node->line_number);
			NEXT_PTR_LIST(BasicBlock, block);
			if (!block) {
				// last one
				if (else_block)
					false_block = else_block;
				else
					false_block = end_block;
			} else {
				false_block = block;
			}
			linearize_test_cond(proc, this_node, true_block, false_block);
			NEXT_PTR_LIST(BasicBlock, true_block);
		}
		END_FOR_EACH_PTR(node)
		FINISH_PTR_LIST(block)
		FINISH_PTR_LIST(true_block)
	}
	{
		PREPARE_PTR_LIST(if_true_blocks, BasicBlock, true_block);
		FOR_EACH_PTR(if_else_stmts, AstNode, this_node)
		{
			linearize_test_then(proc, this_node, true_block, end_block);
			NEXT_PTR_LIST(BasicBlock, true_block);
		}
		END_FOR_EACH_PTR(node)
		FINISH_PTR_LIST(true_block)
	}

	if (else_block) {
		start_block(proc, else_block, ifnode->line_number);
		start_scope(proc->linearizer, proc, else_scope);
		linearize_statement_list(proc, else_stmts);
		end_scope(proc->linearizer, proc, ifnode->line_number);
		if (!is_block_terminated(proc->current_bb))
			instruct_br(proc, allocate_block_pseudo(proc, end_block), ifnode->line_number);
	}

	start_block(proc, end_block, ifnode->line_number);
}

/*
handle label statement.
We start a new block which will get associated with the label.
We have to handle the situation where the label pseudo was already created when we
encountered a goto statement but we did not know the block then.
*/
static void linearize_label_statement(Proc *proc, AstNode *node)
{
	BasicBlock* block;
	if (node->label_stmt.symbol->label.pseudo != NULL) {
		/* This means the block got created when we saw the goto statement, so we just need to make it current */
		assert(node->label_stmt.symbol->label.pseudo->block != NULL);
		block = node->label_stmt.symbol->label.pseudo->block;
		start_block(proc, block, node->line_number);
	}
	else {
		block = proc->current_bb;
		/* If the current block is empty then we can use it as the label target */
		if (raviX_ptrlist_size((const PtrList *)block->insns) > 0) {
			/* Create new block as label target */
			block = create_block(proc);
			start_block(proc, block, node->line_number);
		}
		node->label_stmt.symbol->label.pseudo = allocate_block_pseudo(proc, block);
	}
}

/* TODO move this logic to parser? */
/* Search for a label going up scopes starting from the scope where the goto statement appeared.
 * Also return via min_closing_block the ancestor scope that is greater than or equal to the
 * label scope, and where a local variable escaped.
 */
static LuaSymbol *find_label(Proc *proc, Scope *block,
				     const StringObject *label_name, Scope **min_closing_block)
{
	AstNode *function = block->function; /* We need to stay inside the function when lookng for the label */
	*min_closing_block = NULL;
	while (block != NULL && block->function == function) {
		LuaSymbol *symbol;
		if (block->need_close) {
			*min_closing_block = block;
		}
		FOR_EACH_PTR_REVERSE(block->symbol_list, LuaSymbol, symbol)
		{
			if (symbol->symbol_type == SYM_LABEL && symbol->label.label_name == label_name) {
				return symbol;
			}
		}
		END_FOR_EACH_PTR_REVERSE(symbol)
		block = block->parent;
	}
	return NULL;
}

/*
* Starting from block, go up the hierarchy until target_block and determine the oldest
* ancestor block that has escaped variables and thus needs to be closed.
*/
static Scope *find_min_closing_block(Scope *block, Scope *target_block)
{
	AstNode *function = block->function; /* We need to stay inside the function when lookng for the label */
	Scope *min_closing_block = NULL;
	while (block != NULL && block->function == function) {
		if (block->need_close) {
			min_closing_block = block;
		}
		if (block == target_block)
			break;
		block = block->parent;
	}
	return min_closing_block;
}

/*
 * Checks if a basic block is already closed - for now we check if the last
 * instruction in the block is op_ret, which also handles closing of up-values.
 */
static bool is_already_closed(Proc *proc, BasicBlock *block)
{
	Instruction *last_insn = raviX_last_instruction(block);
	if (last_insn == NULL)
		return false;
	if (last_insn->opcode == op_ret)
		return true;
	if (last_insn->opcode == op_close) {
		// hmmm
		//assert(false);
		// FIXME we can have a block ending in op_close if its the last block
		// and there is an implicit return; amybe an OPT pass can remove these?
	}
	return false;
}

/* Adds a OP_CLOSE instruction at the specified basic block, but only if any local variables in the given
 * scope escaped, i.e. were referenced as upvalues.
 * Note that the proc's current_bb remains unchanged after this call. Normally we would expect
 * the current basic block to be where we insert instructions but in this case there are scenarios
 * such as when processing goto or break statemnt where the close instruction must be added to the
 * the goto / break target block.
 */
static void instruct_close(Proc *proc, BasicBlock *block, Scope *scope, unsigned line_number)
{
	if (is_already_closed(proc, block))
		return;
	/* temporarily make block current */
	BasicBlock *prev_current = proc->current_bb;
	proc->current_bb = block;

	LuaSymbol *symbol;
	FOR_EACH_PTR(scope->symbol_list, LuaSymbol, symbol)
	{
		/* We add the first escaping variable as the operand to op_close.
		 * op_close is meant to scan the stack from that point and close
		 * any open upvalues
		 */
		if (symbol->symbol_type == SYM_LOCAL && symbol->variable.escaped) {
			assert(symbol->variable.pseudo);
			Instruction *insn = allocate_instruction(proc, op_close, line_number);
			add_instruction_operand(proc, insn, symbol->variable.pseudo);
			add_instruction(proc, insn);
			break;
		}
	}
	END_FOR_EACH_PTR(symbol)
	
	/* restore current basic block */
	proc->current_bb = prev_current;
}

/*
When linearizing the goto statement we create a pseudo for the label if it hasn't been already created.
But at this point we may not know the target basic block to goto, which we expect to be filled when the label is
encountered. Of course if the label was linearized before we got to the goto statement then the target block
would already be known and specified in the pseudo.
*/
static void linearize_goto_statement(Proc *proc, const AstNode *node)
{
	if (node->goto_stmt.is_break) {
		if (proc->current_break_target == NULL) {
			handle_error(proc->linearizer->compiler_state, "no current break target");
		}
		/* Find the oldest ancestor scope that may need to be closed */
		Scope *min_closing_block = find_min_closing_block(node->goto_stmt.goto_scope, proc->current_break_scope);
		instruct_br(proc, allocate_block_pseudo(proc, proc->current_break_target), node->line_number);
		start_block(proc, create_block(proc), node->line_number);
		if (min_closing_block) {
			/* Note that the close instruction goes to the target block of the goto */
			instruct_close(proc, proc->current_break_target, min_closing_block, node->line_number);
		}
		return;
	}
	/* The AST does not provide link to the label so we have to search for the label in the goto scope
	   and above */
	if (node->goto_stmt.goto_scope) {
		Scope *min_closing_block = NULL;
		LuaSymbol *symbol = find_label(proc, node->goto_stmt.goto_scope, node->goto_stmt.name, &min_closing_block);
		if (symbol) {
			/* label found */
			if (symbol->label.pseudo == NULL) {
				/* No pseudo? create with target block to be processed later when label is encountered */
				symbol->label.pseudo = allocate_block_pseudo(proc, create_block(proc));
			}
			else {				
				assert(symbol->label.pseudo->block != NULL);
			}
			instruct_br(proc, symbol->label.pseudo, node->line_number);
			start_block(proc, create_block(proc), node->line_number);
			if (min_closing_block) {
				/* Note that the close instruction goes to the target block of the goto */
				instruct_close(proc, symbol->label.pseudo->block, min_closing_block, node->line_number);
			}
			return;
		}
	}
	handle_error(proc->linearizer->compiler_state, "goto label not found");
}

static void linearize_do_statement(Proc *proc, AstNode *node)
{
	assert(node->type == STMT_DO);
	start_scope(proc->linearizer, proc, node->do_stmt.scope);
	linearize_statement_list(proc, node->do_stmt.do_statement_list);
	end_scope(proc->linearizer, proc, node->line_number);
}

//clang-format off
/*
Lua manual states:

	 for v = e1, e2, e3 do block end

is equivalent to the code:

	 do
	   local var, limit, step = tonumber(e1), tonumber(e2), tonumber(e3)
	   if not (var and limit and step) then error() end
	   var = var - step
	   while true do
		 var = var + step
		 if (step >= 0 and var > limit) or (step < 0 and var < limit) then
		   break
		 end
		 local v = var
		 block
	   end
	 end

We do not need local vars to hold var, limit, step as these can be
temporaries.

	step_positive = 0 < step
	var = var - step
	goto L1
L1:
	var = var + step;
	if step_positive goto L2;
		else goto L3;
L2:
	stop = var > limit
	if stop goto Lend
		else goto Lbody
L3:
	stop = var < limit
	if stop goto Lend
		else goto Lbody
Lbody:
	set local symbol in for loop to var
	do body
	goto L1;

Lend:

Above is the general case

When we know the increment to be negative or positive we can simplify.
Example for positive case

	var = var - step
	goto L1
L1:
	var = var + step;
 	goto L2
L2:
	stop = var > limit
	if stop goto Lend
		else goto Lbody
Lbody:
	set local symbol in for loop to var
	do body
	goto L1;
Lend:

Negative case

	var = var - step
	goto L1
L1:
	var = var + step;
	goto L3;
L3:
	stop = var < limit
	if stop goto Lend
		else goto Lbody
Lbody:
	set local symbol in for loop to var
	do body
	goto L1;
Lend:


*/
//clang-format on

static void linearize_for_num_statement_positivestep(Proc *proc, AstNode *node)
{
	start_scope(proc->linearizer, proc, node->for_stmt.for_scope);

	AstNode *index_var_expr = (AstNode *) raviX_ptrlist_nth_entry((PtrList *)node->for_stmt.expr_list, 0);
	AstNode *limit_expr = (AstNode *) raviX_ptrlist_nth_entry((PtrList *)node->for_stmt.expr_list, 1);
	AstNode *step_expr = (AstNode *) raviX_ptrlist_nth_entry((PtrList *)node->for_stmt.expr_list, 2);
	LuaSymbol *var_sym = (LuaSymbol *) raviX_ptrlist_nth_entry((PtrList *)node->for_stmt.symbols, 0);

	if (index_var_expr == NULL || limit_expr == NULL) {
		handle_error(proc->linearizer->compiler_state, "A least index and limit must be supplied");
	}
	Pseudo *t = linearize_expression(proc, index_var_expr);
	if (t->type == PSEUDO_RANGE) {
		convert_range_to_temp(t); // Only accept one result
	}
	Pseudo *index_var_pseudo = allocate_temp_pseudo(proc, RAVI_TNUMINT, false);
	instruct_move(proc, op_mov, index_var_pseudo, t, node->line_number);

	t = linearize_expression(proc, limit_expr);
	if (t->type == PSEUDO_RANGE) {
		convert_range_to_temp(t); // Only accept one result
	}
	Pseudo *limit_pseudo = allocate_temp_pseudo(proc, RAVI_TNUMINT, false);
	instruct_move(proc, op_mov, limit_pseudo, t, node->line_number);

	if (step_expr == NULL)
		t = allocate_constant_pseudo(proc, allocate_integer_constant(proc, 1));
	else {
		t = linearize_expression(proc, step_expr);
		if (t->type == PSEUDO_RANGE) {
			convert_range_to_temp(t); // Only accept one result
		}
	}
	Pseudo *step_pseudo = allocate_temp_pseudo(proc, RAVI_TNUMINT, false);
	instruct_move(proc, op_mov, step_pseudo, t, node->line_number);

	Pseudo *stop_pseudo = allocate_temp_pseudo(proc, RAVI_TBOOLEAN, false);
	create_binary_instruction(proc, op_subii, index_var_pseudo, step_pseudo, index_var_pseudo, node->line_number);

	BasicBlock *L1 = create_block(proc);
	BasicBlock *L2 = create_block(proc);
	BasicBlock *Lbody = create_block(proc);
	BasicBlock *Lend = create_block(proc);
	BasicBlock *previous_break_target = proc->current_break_target;
	Scope *previous_break_scope = proc->current_break_scope;
	proc->current_break_target = Lend;
	proc->current_break_scope = proc->current_scope;

	start_block(proc, L1, node->line_number);
	create_binary_instruction(proc, op_addii, index_var_pseudo, step_pseudo, index_var_pseudo, node->line_number);
	instruct_br(proc, allocate_block_pseudo(proc, L2), node->line_number);

	start_block(proc, L2, node->line_number);
	create_binary_instruction(proc, op_ltii, limit_pseudo, index_var_pseudo, stop_pseudo, node->line_number);
	instruct_cbr(proc, stop_pseudo, Lend, Lbody, node->line_number);

	start_block(proc, Lbody, node->line_number);
	instruct_move(proc, op_mov, var_sym->variable.pseudo, index_var_pseudo, node->line_number);

	start_scope(proc->linearizer, proc, node->for_stmt.for_body);
	linearize_statement_list(proc, node->for_stmt.for_statement_list);
	end_scope(proc->linearizer, proc, node->line_number);

	/* If the fornum block has escaped local vars then we need to close */
	if (proc->current_break_scope->need_close) {
		/* Note we put close instruction in current basic block */
		instruct_close(proc, proc->current_bb, proc->current_break_scope, node->line_number);
	}
	instruct_br(proc, allocate_block_pseudo(proc, L1), node->line_number);

	end_scope(proc->linearizer, proc, node->line_number);

	free_temp_pseudo(proc, stop_pseudo, false);
	free_temp_pseudo(proc, step_pseudo, false);
	free_temp_pseudo(proc, limit_pseudo, false);
	free_temp_pseudo(proc, index_var_pseudo, false);

	start_block(proc, Lend, node->line_number);

	proc->current_break_target = previous_break_target;
	proc->current_break_scope = previous_break_scope;
}

static void linearize_for_num_statement(Proc *proc, AstNode *node)
{
	assert(node->type == STMT_FOR_NUM);

	/* For now we only allow integer expressions */
	AstNode *expr;
	FOR_EACH_PTR(node->for_stmt.expr_list, AstNode, expr)
		{
			if (expr->common_expr.type.type_code != RAVI_TNUMINT) {
				handle_error(proc->linearizer->compiler_state,
					     "Only for loops with integer expressions currently supported");
			}
		}
	END_FOR_EACH_PTR(expr)

	/* Check if we can optimize */
	AstNode *step_expr = (AstNode *) raviX_ptrlist_nth_entry((PtrList *)node->for_stmt.expr_list, 2);
	{
		bool step_known_positive = false;
//		bool step_known_negative = false;
		if (step_expr == NULL) {
			step_known_positive = true;
		} else if (step_expr->type == EXPR_LITERAL) {
			if (step_expr->literal_expr.type.type_code == RAVI_TNUMINT) {
				if (step_expr->literal_expr.u.i > 0)
					step_known_positive = true;
//				else if (step_expr->literal_expr.u.i < 0)
//					step_known_negative = true;
			}
		}
		if (step_known_positive) {
			linearize_for_num_statement_positivestep(proc, node);
			return;
		}
	}

	/* Default case where we do not know if step is negative or positive */
	start_scope(proc->linearizer, proc, node->for_stmt.for_scope);

	AstNode *index_var_expr = (AstNode *) raviX_ptrlist_nth_entry((PtrList *)node->for_stmt.expr_list, 0);
	AstNode *limit_expr = (AstNode *) raviX_ptrlist_nth_entry((PtrList *)node->for_stmt.expr_list, 1);
	LuaSymbol *var_sym = (LuaSymbol *) raviX_ptrlist_nth_entry((PtrList *)node->for_stmt.symbols, 0);

	if (index_var_expr == NULL || limit_expr == NULL) {
		handle_error(proc->linearizer->compiler_state, "A least index and limit must be supplied");
	}

	Pseudo *t = linearize_expression(proc, index_var_expr);
	if (t->type == PSEUDO_RANGE) {
		convert_range_to_temp(t); // Only accept one result
	}
	Pseudo *index_var_pseudo = allocate_temp_pseudo(proc, RAVI_TNUMINT, false);
	instruct_move(proc, op_mov, index_var_pseudo, t, node->line_number);

	t = linearize_expression(proc, limit_expr);
	if (t->type == PSEUDO_RANGE) {
		convert_range_to_temp(t); // Only accept one result
	}
	Pseudo *limit_pseudo = allocate_temp_pseudo(proc, RAVI_TNUMINT, false);
	instruct_move(proc, op_mov, limit_pseudo, t, node->line_number);

	if (step_expr == NULL)
		t = allocate_constant_pseudo(proc, allocate_integer_constant(proc, 1));
	else {
		t = linearize_expression(proc, step_expr);
		if (t->type == PSEUDO_RANGE) {
			convert_range_to_temp(t); // Only accept one result
		}
	}
	Pseudo *step_pseudo = allocate_temp_pseudo(proc, RAVI_TNUMINT, false);
	instruct_move(proc, op_mov, step_pseudo, t, node->line_number);

	Pseudo *step_positive = allocate_temp_pseudo(proc, RAVI_TBOOLEAN, false);
	create_binary_instruction(proc, op_ltii, allocate_constant_pseudo(proc, allocate_integer_constant(proc, 0)),
				  step_pseudo, step_positive, node->line_number);

	Pseudo *stop_pseudo = allocate_temp_pseudo(proc, RAVI_TBOOLEAN, false);
	create_binary_instruction(proc, op_subii, index_var_pseudo, step_pseudo, index_var_pseudo, node->line_number);

	BasicBlock *L1 = create_block(proc);
	BasicBlock *L2 = create_block(proc);
	BasicBlock *L3 = create_block(proc);
	BasicBlock *Lbody = create_block(proc);
	BasicBlock *Lend = create_block(proc);
	BasicBlock *previous_break_target = proc->current_break_target;
	Scope *previous_break_scope = proc->current_break_scope;
	proc->current_break_target = Lend;
	proc->current_break_scope = proc->current_scope;

	start_block(proc, L1, node->line_number);
	create_binary_instruction(proc, op_addii, index_var_pseudo, step_pseudo, index_var_pseudo, node->line_number);
	instruct_cbr(proc, step_positive, L2, L3, node->line_number);

	start_block(proc, L2, node->line_number);
	create_binary_instruction(proc, op_ltii, limit_pseudo, index_var_pseudo, stop_pseudo, node->line_number);
	instruct_cbr(proc, stop_pseudo, Lend, Lbody, node->line_number);

	start_block(proc, L3, node->line_number);
	create_binary_instruction(proc, op_ltii, index_var_pseudo, limit_pseudo, stop_pseudo, node->line_number);
	instruct_cbr(proc, stop_pseudo, Lend, Lbody, node->line_number);

	start_block(proc, Lbody, node->line_number);
	instruct_move(proc, op_mov, var_sym->variable.pseudo, index_var_pseudo, node->line_number);

	start_scope(proc->linearizer, proc, node->for_stmt.for_body);
	linearize_statement_list(proc, node->for_stmt.for_statement_list);
	end_scope(proc->linearizer, proc, node->line_number);

	/* If the fornum block has escaped local vars then we need to close */
	if (proc->current_break_scope->need_close) {
		/* Note we put close instruction in current basic block */
		instruct_close(proc, proc->current_bb, proc->current_break_scope, node->line_number);
	}
	instruct_br(proc, allocate_block_pseudo(proc, L1), node->line_number);

	end_scope(proc->linearizer, proc, node->line_number);

	free_temp_pseudo(proc, stop_pseudo, false);
	free_temp_pseudo(proc, step_positive, false);
	free_temp_pseudo(proc, step_pseudo, false);
	free_temp_pseudo(proc, limit_pseudo, false);
	free_temp_pseudo(proc, index_var_pseudo, false);

	start_block(proc, Lend, node->line_number);

	proc->current_break_target = previous_break_target;
	proc->current_break_scope = previous_break_scope;
}

static void linearize_while_statment(Proc *proc, AstNode *node)
{
	BasicBlock *test_block = create_block(proc);
	BasicBlock *body_block = create_block(proc);
	BasicBlock *end_block = create_block(proc);
	BasicBlock *previous_break_target = proc->current_break_target;
	Scope *previous_break_scope = proc->current_break_scope;
	proc->current_break_target = end_block;
	proc->current_break_scope = node->while_or_repeat_stmt.loop_scope;

	if (node->type == STMT_REPEAT) {
		instruct_br(proc, allocate_block_pseudo(proc, body_block), node->line_number);
	}

	start_block(proc, test_block, node->line_number);
	Pseudo *condition_pseudo = linearize_expression(proc, node->while_or_repeat_stmt.condition);
	instruct_cbr(proc, condition_pseudo, body_block, end_block, node->line_number);
	free_temp_pseudo(proc, condition_pseudo, false);

	start_block(proc, body_block, node->line_number);
	start_scope(proc->linearizer, proc, node->while_or_repeat_stmt.loop_scope);
	linearize_statement_list(proc, node->while_or_repeat_stmt.loop_statement_list);
	end_scope(proc->linearizer, proc, node->line_number);

	/* If the while/repeat block has escaped local vars then we need to close */
	if (proc->current_break_scope->need_close) {
		instruct_close(proc, proc->current_bb, proc->current_break_scope, node->line_number);
	}
	instruct_br(proc, allocate_block_pseudo(proc, test_block), node->line_number);

	start_block(proc, end_block, node->line_number);

	proc->current_break_target = previous_break_target;
	proc->current_break_scope = previous_break_scope;
}

static void linearize_embedded_C_decl(Proc *proc, AstNode *node)
{
	if (proc != proc->linearizer->main_proc) {
		handle_error(proc->linearizer->compiler_state,
			     "Embedded C declarations can only be present in the main chunk");
	}
	add_C_declaration(proc->linearizer, node->embedded_C_stmt.C_src_snippet);
}

static void linearize_embedded_C(Proc *proc, AstNode *node)
{
	if (node->embedded_C_stmt.is_decl) {
		linearize_embedded_C_decl(proc, node);
		return;
	}

	Instruction *insn = allocate_instruction(proc, op_C__unsafe, node->line_number);
	LuaSymbol *sym;
	FOR_EACH_PTR(node->embedded_C_stmt.symbols, LuaSymbol, sym)
	{
		if (sym->symbol_type == SYM_LOCAL) {
			Pseudo *pseudo = sym->variable.pseudo;
			add_instruction_operand(proc, insn, pseudo);
		} else {
			handle_error(proc->linearizer->compiler_state,
				     "Variables referenced by embed C instruction must be locals");
		}
	}
	END_FOR_EACH_PTR(sym)
	add_instruction_target(proc, insn, allocate_constant_pseudo(proc, allocate_string_constant(proc, node->embedded_C_stmt.C_src_snippet)));
	add_instruction(proc, insn);
}

static void linearize_function_statement(Proc *proc, AstNode *node)
{
	/* function funcname funcbody */
	/* funcname ::= Name {‘.’ Name} [‘:’ Name] */

	// Note the similarity of following to the handling of suffixed expressions and assignment expressions
	// In truth we could translate this to an expression statement - the only benefit here is that we
	// do not allow selectors to be arbitrary expressions
	Pseudo *prev_pseudo = linearize_symbol_expression(proc, node->function_stmt.name);
	AstNode *prev_node = node->function_stmt.name;
	AstNode *this_node;
	FOR_EACH_PTR(node->function_stmt.selectors, AstNode, this_node)
	{
		Pseudo *next;
		if (prev_pseudo->type == PSEUDO_INDEXED)
			prev_pseudo = indexed_load(proc, prev_pseudo);
		if (this_node->type == EXPR_FIELD_SELECTOR) {
			Pseudo *key_pseudo = linearize_expression(proc, this_node->index_expr.expr);
			ravitype_t key_type = this_node->index_expr.expr->common_expr.type.type_code;
			next = create_indexed_pseudo(proc, prev_node->common_expr.type.type_code, prev_pseudo, key_type,
						     key_pseudo, this_node->common_expr.type.type_code, node->line_number);
			free_temp_pseudo(proc, key_pseudo, 0);
		} else {
			next = NULL;
			handle_error(proc->linearizer->compiler_state,
				     "Unexpected expr type in function name selector list");
		}
		prev_node = this_node;
		prev_pseudo = next;
	}
	END_FOR_EACH_PTR(this_node)
	// FIXME maybe better to add the method name to the selector list above in the parser
	// then we could have just handled it above rather than as a special case
	if (node->function_stmt.method_name) {
		this_node = node->function_stmt.method_name;
		if (prev_pseudo->type == PSEUDO_INDEXED)
			prev_pseudo = indexed_load(proc, prev_pseudo);
		if (this_node->type == EXPR_FIELD_SELECTOR) {
			Pseudo *key_pseudo = linearize_expression(proc, this_node->index_expr.expr);
			ravitype_t key_type = this_node->index_expr.expr->common_expr.type.type_code;
			prev_pseudo =
			    create_indexed_pseudo(proc, prev_node->common_expr.type.type_code, prev_pseudo, key_type,
						  key_pseudo, this_node->common_expr.type.type_code, node->line_number);
		} else {
			handle_error(proc->linearizer->compiler_state,
				     "Unexpected expr type in function name selector list");
		}
		prev_node = this_node;
	}
	Pseudo *function_pseudo = linearize_function_expr(proc, node->function_stmt.function_expr);
	/* Following will potentially convert load to store */
	linearize_store_var(proc, &prev_node->common_expr.type, prev_pseudo,
			    &node->function_stmt.function_expr->common_expr.type, function_pseudo, node->line_number);
	free_temp_pseudo(proc, prev_pseudo, false);
	free_temp_pseudo(proc, function_pseudo, false);
}

static void linearize_statement(Proc *proc, AstNode *node)
{
	switch (node->type) {
	case AST_NONE: {
		break;
	}
	case STMT_RETURN: {
		linearize_return(proc, node);
		break;
	}
	case STMT_LOCAL: {
		linearize_local_statement(proc, node);
		break;
	}
	case STMT_FUNCTION: {
		linearize_function_statement(proc, node);
		break;
	}
	case STMT_LABEL: {
		linearize_label_statement(proc, node);
		break;
	}
	case STMT_GOTO: {
		linearize_goto_statement(proc, node);
		break;
	}
	case STMT_DO: {
		linearize_do_statement(proc, node);
		break;
	}
	case STMT_EXPR: {
		linearize_expression_statement(proc, node);
		break;
	}
	case STMT_IF: {
		linearize_if_statement(proc, node);
		break;
	}
	case STMT_WHILE:
	case STMT_REPEAT: {
		linearize_while_statment(proc, node);
		break;
	}
	case STMT_FOR_IN: {
		handle_error(proc->linearizer->compiler_state, "STMT_FOR_IN not yet implemented");
		break;
	}
	case STMT_FOR_NUM: {
		linearize_for_num_statement(proc, node);
		break;
	}
	case STMT_EMBEDDED_C: {
		linearize_embedded_C(proc, node);
		break;
	}
	default:
		handle_error(proc->linearizer->compiler_state, "unknown statement type");
		break;
	}
//	pseudo_gen_check(&proc->temp_pseudos, node, "temp_pseudos");
//	pseudo_gen_check(&proc->temp_flt_pseudos, node, "temp_flt_pseudos");
//	pseudo_gen_check(&proc->temp_int_pseudos, node, "temp_int_pseudos");
}

/**
 * Creates and initializes a basic block to be an empty block. Returns the new basic block.
 */
static BasicBlock *create_block(Proc *proc)
{
	C_MemoryAllocator *allocator = proc->linearizer->compiler_state->allocator;
	if (proc->node_count >= proc->allocated) {
		unsigned new_size = proc->allocated + 25;
		BasicBlock **new_data = (BasicBlock **)
		    allocator->calloc(allocator->arena, new_size, sizeof(BasicBlock *));
		assert(new_data != NULL);
		if (proc->node_count > 0) {
			memcpy(new_data, proc->nodes, proc->allocated * sizeof(BasicBlock *));
		}
		proc->allocated = new_size;
		proc->nodes = new_data;
	}
	assert(proc->node_count < proc->allocated);
	BasicBlock *new_block = (BasicBlock *) allocator->calloc(allocator->arena, 1, sizeof(BasicBlock));
	/* note that each block must have an index that can be used to access the block as nodes[index] */
	new_block->index = proc->node_count;
	proc->nodes[proc->node_count++] = new_block;
	return new_block;
}

/**
 * Takes a basic block as an argument and makes it the current block.
 *
 * If the old current block is unterminated then this will terminate that
 * block by adding an unconditional branch to the new current block.
 *
 * All future instructions will be added to the end of the new current block
 */
static void start_block(Proc *proc, BasicBlock *bb_to_start, unsigned line_number)
{
	if (proc->current_bb && !is_block_terminated(proc->current_bb)) {
		instruct_br(proc, allocate_block_pseudo(proc, bb_to_start), line_number);
	}
	proc->current_bb = bb_to_start;
}

/**
 * Create the initial blocks entry and exit for the proc.
 * sets current block to entry block.
 */
static void initialize_graph(Proc *proc)
{
	assert(proc != NULL);
	BasicBlock *entry = create_block(proc);
	assert(entry->index == ENTRY_BLOCK);
	BasicBlock *exit = create_block(proc);
	assert(exit->index == EXIT_BLOCK);
	start_block(proc, entry, proc->function_expr->line_number);
}

/**
 * Makes given scope the current scope, and allocates registers for locals.
 */
static void start_scope(LinearizerState *linearizer, Proc *proc, Scope *scope)
{
	proc->current_scope = scope;
	LuaSymbol *sym;
	FOR_EACH_PTR(scope->symbol_list, LuaSymbol, sym)
	{
		if (sym->symbol_type == SYM_LOCAL) {
			uint8_t reg;
			if (!sym->variable.escaped && !sym->variable.function_parameter &&
			    (sym->variable.value_type.type_code == RAVI_TNUMFLT ||
			     sym->variable.value_type.type_code == RAVI_TNUMINT)) {
				Pseudo *pseudo;
				if (sym->variable.value_type.type_code == RAVI_TNUMFLT)
					pseudo = allocate_temp_pseudo(proc, RAVI_TNUMFLT, true);
				else
					pseudo = allocate_temp_pseudo(proc, RAVI_TNUMINT, true);
				sym->variable.pseudo = pseudo;
				pseudo->temp_for_local = sym; /* Note that this temp is for a local */
			}
			else {
				reg = allocate_register(&proc->local_pseudos, true);
				allocate_symbol_pseudo(proc, sym, reg);
			}
		}
	}
	END_FOR_EACH_PTR(sym)
}

/**
 * Deallocate local registers when the scope ends, in reverse order
 * so that we have a stack discipline, and then changes current scope to be the
 * parent scope.
 */
static void end_scope(LinearizerState *linearizer, Proc *proc, unsigned line_number)
{
	Scope *scope = proc->current_scope;
	LuaSymbol *sym;
	if (scope->need_close) {
		instruct_close(proc, proc->current_bb, scope, line_number);
	}
	FOR_EACH_PTR_REVERSE(scope->symbol_list, LuaSymbol, sym)
	{
		if (sym->symbol_type == SYM_LOCAL) {
			Pseudo *pseudo = sym->variable.pseudo;
			if (pseudo->type == PSEUDO_SYMBOL) {
				assert(pseudo && pseudo->type == PSEUDO_SYMBOL && pseudo->symbol == sym);
				free_register(proc, &proc->local_pseudos, pseudo->regnum);
			}
			else if (pseudo->type == PSEUDO_TEMP_INT || pseudo->type == PSEUDO_TEMP_FLT || pseudo->type == PSEUDO_TEMP_BOOL) {
				assert(sym == pseudo->temp_for_local);
				free_temp_pseudo(proc, sym->variable.pseudo, true);
			}
			else {
				assert(false);
			}
		}
	}
	END_FOR_EACH_PTR_REVERSE(sym)
	proc->current_scope = scope->parent;
}

static void linearize_function(LinearizerState *linearizer)
{
	Proc *proc = linearizer->current_proc;
	assert(proc != NULL);
	AstNode *func_expr = proc->function_expr;
	assert(func_expr->type == EXPR_FUNCTION);
	initialize_graph(proc);
	assert(proc->node_count >= 2);
	assert(proc->nodes[ENTRY_BLOCK] != NULL);
	assert(proc->nodes[EXIT_BLOCK] != NULL);
	start_scope(linearizer, proc, func_expr->function_expr.main_block);
	linearize_function_args(linearizer);
	linearize_statement_list(proc, func_expr->function_expr.function_statement_list);
	end_scope(linearizer, proc, func_expr->line_number);
	if (!is_block_terminated(proc->current_bb)) {
		Instruction *insn = allocate_instruction(proc, op_ret, func_expr->line_number);
		add_instruction_target(proc, insn, allocate_block_pseudo(proc, proc->nodes[EXIT_BLOCK]));
		add_instruction(proc, insn);
	}
}

void raviX_output_pseudo(const Pseudo *pseudo, TextBuffer *mb)
{
	switch (pseudo->type) {
	case PSEUDO_CONSTANT: {
		const Constant *constant = pseudo->constant;
		const char *tc = "";
		if (constant->type == RAVI_TNUMFLT) {
			raviX_buffer_add_double(mb, constant->n);
			tc = "flt";
		} else if (constant->type == RAVI_TNUMINT) {
			raviX_buffer_add_fstring(mb, "%lld", (long long)constant->i);
			tc = "int";
		} else {
			raviX_buffer_add_fstring(mb, "'%s'", constant->s->str);
			tc = "s";
		}
		raviX_buffer_add_fstring(mb, " K%s(%d)", tc, pseudo->regnum);
	} break;
	case PSEUDO_TEMP_INT:
		raviX_buffer_add_fstring(mb, "Tint(%d)", pseudo->regnum);
		break;
	case PSEUDO_TEMP_BOOL:
		raviX_buffer_add_fstring(mb, "Tbool(%d)", pseudo->regnum);
		break;
	case PSEUDO_TEMP_FLT:
		raviX_buffer_add_fstring(mb, "Tflt(%d)", pseudo->regnum);
		break;
	case PSEUDO_TEMP_ANY:
		raviX_buffer_add_fstring(mb, "T(%d)", pseudo->regnum);
		break;
	case PSEUDO_RANGE_SELECT:
		raviX_buffer_add_fstring(mb, "T(%d[%d..])", pseudo->regnum, pseudo->range_pseudo->regnum);
		break;
	case PSEUDO_PROC:
		raviX_buffer_add_fstring(mb, "Proc%%%d", pseudo->proc->id);
		break;
	case PSEUDO_NIL:
		raviX_buffer_add_string(mb, "nil");
		break;
	case PSEUDO_FALSE:
		raviX_buffer_add_string(mb, "false");
		break;
	case PSEUDO_TRUE:
		raviX_buffer_add_string(mb, "true");
		break;
	case PSEUDO_SYMBOL:
		switch (pseudo->symbol->symbol_type) {
		case SYM_LOCAL: {
			raviX_buffer_add_fstring(mb, "local(%s, %d)", pseudo->symbol->variable.var_name->str,
						 pseudo->regnum);
			break;
		}
		case SYM_UPVALUE: {
			if (pseudo->symbol->upvalue.target_variable->symbol_type == SYM_LOCAL) {
				raviX_buffer_add_fstring(mb, "Upval(%u, Proc%%%d, %s)", pseudo->regnum,
						 pseudo->symbol->upvalue.target_variable->variable.block->function->function_expr.proc_id,
						 pseudo->symbol->upvalue.target_variable->variable.var_name->str);
			}
			else if (pseudo->symbol->upvalue.target_variable->symbol_type == SYM_ENV) {
				raviX_buffer_add_fstring(mb, "Upval(%s)", 
						 pseudo->symbol->upvalue.target_variable->variable.var_name->str);
			}
			break;
		}
		case SYM_GLOBAL: {
			raviX_buffer_add_string(mb, pseudo->symbol->variable.var_name->str);
			break;
		}
		default:
			// handle_error(proc->linearizer->compiler_state, "feature not yet implemented");
			abort();
		}
		break;
	case PSEUDO_BLOCK: {
		raviX_buffer_add_fstring(mb, "L%d", pseudo->block ? (int)pseudo->block->index : -1);
		break;
	}
	case PSEUDO_RANGE: {
		raviX_buffer_add_fstring(mb, "T(%d..)", pseudo->regnum);
		break;
	}
	case PSEUDO_INDEXED: {
		fprintf(stderr, mb->buf);
		assert(false);
		break;
	}
	}
}

// Simple optimization pass that replaces use of upvalues that point to literals
// it doesn't try to remove the upvalues from the proc definition yet

static inline bool is_replaceable_upvalue(Pseudo *pseudo) {
	return pseudo->type == PSEUDO_SYMBOL
	       && pseudo->symbol->symbol_type == SYM_UPVALUE
	       && pseudo->symbol->upvalue.target_variable->variable.literal_initializer
	       && !pseudo->symbol->upvalue.target_variable->variable.modified;
}

static inline bool is_replaceable_type(AstNode *litexpr) {
	return litexpr->common_expr.type.type_code == RAVI_TNUMINT ||
	       litexpr->common_expr.type.type_code == RAVI_TNUMFLT ||
	       litexpr->common_expr.type.type_code == RAVI_TSTRING;
}

static void do_replace_literal_upvalues_in_instruction(Proc *proc, Instruction *insn) {
	PseudoList *list = insn->operands;
	Pseudo *pseudo;
	FOR_EACH_PTR(list, Pseudo, pseudo)
	{
		if (is_replaceable_upvalue(pseudo)) {
			AstNode *litexpr = pseudo->symbol->upvalue.target_variable->variable.literal_initializer;
			assert(litexpr);
			if (is_replaceable_type(litexpr)) {
				Pseudo *replacement = allocate_constant_pseudo(proc, allocate_constant(proc, litexpr));
				REPLACE_CURRENT_PTR(Pseudo, pseudo, replacement);
			}
		}
	}
	END_FOR_EACH_PTR(pseudo)
}

static void do_replace_literal_upvalues_in_block(Proc *proc, BasicBlock *bb) {
	InstructionList *list = bb->insns;
	Instruction *insn;
	FOR_EACH_PTR(list, Instruction, insn) {
	    do_replace_literal_upvalues_in_instruction(proc, insn);
	}
	END_FOR_EACH_PTR(insn)
}

static void do_replace_literal_upvalues(Proc *proc)
{
	BasicBlock *bb;
	for (int i = 0; i < (int)proc->node_count; i++) {
		bb = proc->nodes[i];
		do_replace_literal_upvalues_in_block(proc, bb);
	}
}

static void replace_literal_upvalues(Proc *proc)
{
	do_replace_literal_upvalues(proc);
	Proc *child_proc;
	FOR_EACH_PTR(proc->procs, Proc, child_proc) { replace_literal_upvalues(child_proc); }
	END_FOR_EACH_PTR(childproc)
}

void raviX_optimize_upvalues(LinearizerState *linearizer) {
	replace_literal_upvalues(linearizer->main_proc);
}

////////////// end of optimization of upvalues

static const char *op_codenames[] = {
    "NOOP",	  "RET",    "ADD",  "ADDff",  "ADDfi",	    "ADDii",	 "SUB",	      "SUBff",	   "SUBfi",
    "SUBif",	  "SUBii",  "MUL",  "MULff",  "MULfi",	    "MULii",	 "DIV",	      "DIVff",	   "DIVfi",
    "DIVif",	  "DIVii",  "IDIV", "BAND",   "BANDii",	    "BOR",	 "BORii",     "BXOR",	   "BXORii",
    "SHL",	  "SHLii",  "SHR",  "SHRii",  "EQ",	    "EQii",	 "EQff",      "LT",	   "LIii",
    "LTff",	  "LE",	    "LEii", "LEff",   "MOD",	    "POW",	 "CLOSURE",   "UNM",	   "UNMi",
    "UNMf",	  "LEN",    "LENi", "TOINT",  "TOFLT",	    "TOCLOSURE", "TOSTRING",  "TOIARRAY",  "TOFARRAY",
    "TOTABLE",	  "TOTYPE", "NOT",  "BNOT",   "LOADGLOBAL", "NEWTABLE",	 "NEWIARRAY", "NEWFARRAY", "PUT",
    "PUTik",	  "PUTsk",  "TPUT", "TPUTik", "TPUTsk",	    "IAPUT",	 "IAPUTiv",   "FAPUT",	   "FAPUTfv",
    "CBR",	  "BR",	    "MOV",  "MOVi",   "MOVif",	    "MOVf",	 "MOVfi",     "CALL",	   "GET",
    "GETik",	  "GETsk",  "TGET", "TGETik", "TGETsk",	    "IAGET",	 "IAGETik",   "FAGET",	   "FAGETik",
    "STOREGLOBAL", "CLOSE", "CONCAT", "INIT", "C__UNSAFE",  "C__NEW"};

static void output_pseudo_list(PseudoList *list, TextBuffer *mb)
{
	Pseudo *pseudo;
	raviX_buffer_add_string(mb, " {");
	int i = 0;
	FOR_EACH_PTR(list, Pseudo, pseudo)
	{
		if (i > 0)
			raviX_buffer_add_string(mb, ", ");
		raviX_output_pseudo(pseudo, mb);
		i++;
	}
	END_FOR_EACH_PTR(pseudo)
	raviX_buffer_add_string(mb, "}");
}

const char *raviX_opcode_name(unsigned int opcode) {
	return op_codenames[opcode];
}

/*
Outputs a single instruction. A prefix and suffix can be prepended/appended for formatting reasons.
Each instruction is output in following format.
instruction_name { operands } { targets }
*/
static void output_instruction(Instruction *insn, TextBuffer *mb, const char *prefix, const char *suffix)
{
	raviX_buffer_add_fstring(mb, "%s%s", prefix, op_codenames[insn->opcode]);
	if (insn->operands) {
		output_pseudo_list(insn->operands, mb);
	}
	if (insn->opcode == op_C__unsafe) {
		// special handling as we don't want to output all the C code
		raviX_buffer_add_string(mb, " { C code }");
	}
	else if (insn->targets) {
		output_pseudo_list(insn->targets, mb);
	}
	raviX_buffer_add_string(mb, suffix);
}

void raviX_output_instruction(Instruction *insn, TextBuffer *mb)
{
	output_instruction(insn, mb, "", "");
}

/* Outputs linear IR instructions in text format, each instruction is prefixed and suffixed by given strings */
static void output_instructions(InstructionList *list, TextBuffer *mb, const char *prefix, const char *suffix)
{
	Instruction *insn;
	FOR_EACH_PTR(list, Instruction, insn) { output_instruction(insn, mb, prefix, suffix); }
	END_FOR_EACH_PTR(insn)
}

/* Outputs the linear IR instructions within a basic block
in textual format.
*/
static void output_basic_block(Proc *proc, BasicBlock *bb, TextBuffer *mb)
{
	raviX_buffer_add_fstring(mb, "L%d", bb->index);
	if (bb->index == ENTRY_BLOCK) {
		raviX_buffer_add_string(mb, " (entry)\n");
	} else if (bb->index == EXIT_BLOCK) {
		raviX_buffer_add_string(mb, " (exit)\n");
	} else {
		raviX_buffer_add_string(mb, "\n");
	}
	output_instructions(bb->insns, mb, "\t", "\n");
}

/* Outputs the basic block and instructions inside it as an HTML table. This is
meant to be used in .dot files, hence the table is formatted in a way that works
with graphviz tool */
void raviX_output_basic_block_as_table(Proc *proc, BasicBlock *bb, TextBuffer *mb)
{
	raviX_buffer_add_string(mb, "<TABLE BORDER=\"1\" CELLBORDER=\"0\">\n");
	raviX_buffer_add_fstring(mb, "<TR><TD><B>L%d</B></TD></TR>\n", bb->index);
	output_instructions(bb->insns, mb, "<TR><TD>", "</TD></TR>\n");
	raviX_buffer_add_string(mb, "</TABLE>");
}

/* Outputs the linear IR for the given proc in a text format. */
static void output_proc(Proc *proc, TextBuffer *mb)
{
	BasicBlock *bb;
	raviX_buffer_add_fstring(mb, "define Proc%%%d\n", proc->id);
	for (int i = 0; i < (int)proc->node_count; i++) {
		bb = proc->nodes[i];
		output_basic_block(proc, bb, mb);
	}
}

/* 
Generates the linear IR for the current parse tree (AST).
Returns 0 on success.
*/
int raviX_ast_linearize(LinearizerState *linearizer)
{
	Proc *proc = allocate_proc(linearizer, linearizer->compiler_state->main_function);
	set_main_proc(linearizer, proc);
	set_current_proc(linearizer, proc);
	int rc = setjmp(linearizer->compiler_state->env);
	if (rc == 0) {
		linearize_function(linearizer);
	}
	return rc;
}

/* Outputs the linear IR in text format to the given text buffer. Procs are output
recursively starting at the main proc representing the Lua chunk.
*/
void raviX_show_linearizer(LinearizerState *linearizer, TextBuffer *mb)
{
	output_proc(linearizer->main_proc, mb);
	Proc *proc;
	FOR_EACH_PTR(linearizer->all_procs, Proc, proc)
	{
		if (proc == linearizer->main_proc)
			continue;
		output_proc(proc, mb);
	}
	END_FOR_EACH_PTR(proc)
}

/* Outputs the linear IR in text format to the given file */
void raviX_output_linearizer(LinearizerState *linearizer, FILE *fp)
{
	TextBuffer mb;
	raviX_buffer_init(&mb, 4096);
	raviX_show_linearizer(linearizer, &mb);
	fputs(mb.buf, fp);
	raviX_buffer_free(&mb);
}

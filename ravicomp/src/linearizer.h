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

#ifndef ravicomp_LINEARIZER_H
#define ravicomp_LINEARIZER_H

#include "ravi_compiler.h"

#include "allocate.h"
#include "common.h"
#include "membuf.h"
#include "parser.h"
#include "ptrlist.h"

/*
Linearizer component is responsible for translating the abstract syntax tree to
a Linear intermediate representation (IR).
*/
typedef struct Instruction Instruction;
typedef struct BasicBlock BasicBlock;
typedef struct Proc Proc;
typedef struct Constant Constant;
typedef struct Graph Graph;

DECLARE_PTR_LIST(InstructionList, Instruction);
DECLARE_PTR_LIST(PseudoList, Pseudo);
DECLARE_PTR_LIST(ProcList, Proc);
DECLARE_PTR_LIST(StringObjectList, StringObject);

/* order is important here ! */
enum opcode {
	op_nop,
	op_ret,
	op_add,
	op_addff,
	op_addfi,
	op_addii,
	op_sub,
	op_subff,
	op_subfi,
	op_subif,
	op_subii,
	op_mul,
	op_mulff,
	op_mulfi,
	op_mulii,
	op_div,
	op_divff,
	op_divfi,
	op_divif,
	op_divii,
	op_idiv,
	op_band,
	op_bandii,
	op_bor,
	op_borii,
	op_bxor,
	op_bxorii,
	op_shl,
	op_shlii,
	op_shr,
	op_shrii,
	op_eq,
	op_eqii,
	op_eqff,
	op_lt,
	op_ltii,
	op_ltff,
	op_le,
	op_leii,
	op_leff,
	op_mod,
	op_pow,
	op_closure,
	op_unm,
	op_unmi,
	op_unmf,
	op_len,
	op_leni,
	op_toint,
	op_toflt,
	op_toclosure,
	op_tostring,
	op_toiarray,
	op_tofarray,
	op_totable,
	op_totype,
	op_not,
	op_bnot,
	op_loadglobal,
	op_newtable,
	op_newiarray,
	op_newfarray,
	op_put, /* target is any */
	op_put_ikey,
	op_put_skey,
	op_tput, /* target is table */
	op_tput_ikey,
	op_tput_skey,
	op_iaput, /* target is integer[]*/
	op_iaput_ival,
	op_faput, /* target is number[] */
	op_faput_fval,
	op_cbr,
	op_br,
	op_mov,
	op_movi,
	op_movif, /* int to float if compatible else error */
	op_movf,
	op_movfi, /* float to int if compatible else error */
	op_call,
	op_get,
	op_get_ikey,
	op_get_skey,
	op_tget,
	op_tget_ikey,
	op_tget_skey,
	op_iaget,
	op_iaget_ikey,
	op_faget,
	op_faget_ikey,
	op_storeglobal,
	op_close,
	op_concat,
	op_init,
	op_C__unsafe,
	op_C__new
	/* TODO need opcode for C declarations */
};

/*
 * The IR instructions use operands and targets of type pseudo, which
 * is a way of referencing different types of objects.
 */
enum PseudoType {
	PSEUDO_SYMBOL,	  /* An object of type lua_symbol representing local variable or upvalue, always refers to Lua
			     stack relative to 'base' */
	PSEUDO_TEMP_FLT,  /* A floating point temp - may also be used for locals that don't escape - refers to C var */
	PSEUDO_TEMP_INT,  /* An integer temp - may also be used for locals that don't escape - refers to C var */
	PSEUDO_TEMP_BOOL, /* An integer temp but restricted to 1 and 0  - refers to C var, shares the virtual C stack
			     with PSEUDO_TEMP_INT */
	PSEUDO_TEMP_ANY,  /* A temp of any type - will always be on Lua stack relative to 'base' */
	PSEUDO_CONSTANT,  /* A literal value */
	PSEUDO_PROC,	  /* A proc / function */
	PSEUDO_NIL,	  /* Literal */
	PSEUDO_TRUE,	  /* Literal */
	PSEUDO_FALSE,	  /* Literal */
	PSEUDO_BLOCK,	  /* Points to a basic block, used as targets for jumps */
	PSEUDO_RANGE,	  /* Represents a range of registers from a certain starting register on Lua stack relative to
			     'base' */
	PSEUDO_RANGE_SELECT, /* Picks a certain register from a range, resolves to register on Lua stack, relative to
				'base' */
	/* TODO we need a type for var args */
	PSEUDO_LUASTACK /* Specifies a Lua stack position - not used by linearizer - for use by codegen. This is
			   relative to CI->func rather than 'base' */
};

/* A pseudo represents an operand/target of an instruction, and can point to
 * different things such as virtual registers, basic blocks, symbols, etc.
 */
struct Pseudo {
	unsigned type : 4, regnum : 16, freed : 1;
	Instruction *insn; /* instruction that created this pseudo */
	union {
		LuaSymbol *symbol;	   /* PSEUDO_SYMBOL */
		const Constant *constant;  /* PSEUDO_CONSTANT */
		LuaSymbol *temp_for_local; /* PSEUDO_TEMP - if the temp represents a local */
		Proc *proc;		   /* PSEUDO_PROC */
		BasicBlock *block;	   /* PSEUDO_BLOCK */
		Pseudo *range_pseudo;	   /* PSEUDO_RANGE_SELECT */
		int stackidx;		   /* PSEUDO_LUASTACK */
	};
};

/* single intermediate code (IR) instruction.
 * All instructions have a uniform structure. There is an opcode, and
 * optional operands and targets. A target is mostly like an output but can also
 * be other things like branch targets.
 */
struct Instruction {
	unsigned opcode : 8;
	PseudoList *operands;
	PseudoList *targets;
	BasicBlock *block; /* owning block */
	unsigned line_number; /* Carry forward line number info */
};

/* Basic block.
 * We don't store CFG info in basic blocks, instead the CFG data structure just
 * references blocks by the block's index.
 */
struct BasicBlock {
	nodeId_t index;		/* The index of the block is a key to enable retrieving the block from its compiler_state */
	InstructionList *insns; /* Note that if number of instructions is 0 then the block was logically deleted */
};
DECLARE_PTR_LIST(BasicBlockList, BasicBlock);

typedef struct PseudoGenerator {
	uint64_t bits[4]; /* bitset of registers */
	unsigned max_reg;
} PseudoGenerator;
static inline unsigned raviX_max_reg(PseudoGenerator *generator) { return generator->max_reg; }

struct Constant {
	uint16_t type;	/* ravitype_t RAVI_TNUMINT, RAVI_TNUMFLT or RAVI_TSTRING */
	uint16_t index; /* index number starting from 0 assigned to each constant - acts like a reg num.
			 * Each type will be assigned separate range */
	union {
		lua_Integer i;
		lua_Number n;
		const StringObject *s;
	};
};

/* Proc represent a Ravi/Lua function. Procs can have nested procs, or children.
 * All functions are anonymous in the language, therefore a Proc also lacks a user defined name, but we
 * do assign a ID to each proc so that we can reference them, and we need names in the generated C code too.
 */
struct Proc {
	// TODO fields below are mixed up between those that are used temporarily by the linearizer /
	// code generator and those that actually define the proc.
	// We should put them into distinct groups.
	unsigned node_count;
	unsigned allocated;
	BasicBlock **nodes;
	uint32_t id; /* ID for the proc */
	LinearizerState *linearizer;
	ProcList *procs;	/* procs defined in this proc */
	Proc *parent;		/* enclosing proc */
	AstNode *function_expr; /* function ast that we are compiling */
	Scope *current_scope;
	BasicBlock *current_bb;
	BasicBlock *current_break_target; /* track the current break target, previous target must be saved /
						     restored in stack discipline */
	Scope *current_break_scope;	  /* as above track the block scope */
	PseudoGenerator local_pseudos;	  /* locals */
	PseudoGenerator temp_int_pseudos; /* temporaries known to be integer type */
	PseudoGenerator temp_flt_pseudos; /* temporaries known to be number type */
	PseudoGenerator temp_pseudos;	  /* All other temporaries */
	Set *constants;			  /* constants used by this proc */
	uint16_t num_intconstants;
	uint16_t num_fltconstants;
	uint16_t num_strconstants;
	Graph *cfg;	   /* place holder for control flow graph; the linearizer does not create this */
	char funcname[30]; /* Each proc needs a name inside a C module - name is a short string */
	void *userdata;	   /* For use by code generator */
};

struct LinearizerState {
	CompilerState *compiler_state;
	Proc *main_proc;     /* The root of the compiled chunk of code */
	ProcList *all_procs; /* All procs allocated by the linearizer */
	Proc *current_proc;  /* proc being compiled */
	uint32_t proc_id;
	TextBuffer C_declarations; /* List of top level C declarations to be added to generated code, build from C__decl statements; TODO need op code for this */
};

// Get string name of an op code
const char *raviX_opcode_name(unsigned int opcode);
// Output a pseudo in textual form
void raviX_output_pseudo(const Pseudo *pseudo, TextBuffer *buffer);
// Output the entire IR in textual form.
void raviX_show_linearizer(LinearizerState *linearizer, TextBuffer *buffer);
// Output an instruction's textual representation to buffer
void raviX_output_instruction(Instruction *insn, TextBuffer *buffer);
// Specialized output format that is similar to HTML Table but is
// designed primarily for graphwiz dot format
void raviX_output_basic_block_as_table(Proc *proc, BasicBlock *bb, TextBuffer *mb);
// fix a range pseudo
Pseudo *raviX_allocate_range_select_pseudo(Proc *proc, Pseudo *range_pseudo, int pick);

Instruction *raviX_last_instruction(BasicBlock *block);

#endif
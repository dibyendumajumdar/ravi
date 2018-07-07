#ifndef DMR_C_LINEARIZE_H
#define DMR_C_LINEARIZE_H

/*
* Linearize - walk the parse tree and generate a linear version
* of it and the basic blocks.
*
* Copyright (C) 2004 Linus Torvalds
* Copyright (C) 2004 Christopher Li
*/

#include <lib.h>
#include <allocate.h>
#include <token.h>
#include <parse.h>
#include <symbol.h>

#ifdef __cplusplus
extern "C" {
#endif

struct instruction;

DECLARE_PTR_LIST(basic_block_list, struct basic_block);
DECLARE_PTR_LIST(instruction_list, struct instruction);
DECLARE_PTR_LIST(multijmp_list, struct multijmp);
DECLARE_PTR_LIST(pseudo_list, struct pseudo);

struct pseudo_user {
	struct instruction *insn;
	pseudo_t *userp;
};

DECLARE_PTR_LIST(pseudo_user_list, struct pseudo_user);

enum pseudo_type {
	PSEUDO_VOID,
	PSEUDO_REG,
	PSEUDO_SYM,
	PSEUDO_VAL,
	PSEUDO_ARG,
	PSEUDO_PHI,
};

/* Have you ever heard of "static single assignment" or SSA form? 
   struct pseudo represents one of those single-assignment variables. 
   Each one has a pointer to the symbol it represents (which may 
   have many pseudos referencing it). Each one also has a pointer 
   to the instruction that defines it.*/
struct pseudo {
	int nr;
	enum pseudo_type type;
	int size; /* OP_SETVAL only */
	struct pseudo_user_list *users; /* pseudo_user list */
	struct ident *ident;
	union {
		struct symbol *sym;	     // PSEUDO_SYM, VAL & ARG
		struct instruction *def; // PSEUDO_REG & PHI
		long long value;	     // PSEUDO_VAL
	};
	DMRC_BACKEND_TYPE priv;
	DMRC_BACKEND_TYPE priv2;	/* FIXME - we use this to save ptr to allocated stack in PHI instructions (nanojit) */
};

struct linearizer_state_t {
	struct allocator pseudo_allocator;
	struct allocator pseudo_user_allocator;
	struct allocator asm_constraint_allocator;
	struct allocator asm_rules_allocator;
	struct allocator multijmp_allocator;
	struct allocator basic_block_allocator;
	struct allocator entrypoint_allocator;
	struct allocator instruction_allocator;

	struct pseudo void_pseudo;
	struct position current_pos;

	int repeat_phase;
	unsigned long bb_generation;
	int liveness_changed;

	struct pseudo_list **live_list;
	struct pseudo_list *dead_list;

#define MAX_VAL_HASH 64
	struct pseudo_list *prev[MAX_VAL_HASH]; /* from pseudo_t value_pseudo(long long val) in linearize.c */
	int nr;	/* pseudo number */
	int bb_nr; /* basic block number */
	char buffer[4096*4];
	int n;
	char pseudo_buffer[4][64];
#define INSN_HASH_SIZE 256
	struct instruction_list *insn_hash_table[INSN_HASH_SIZE];
};

#define VOID_PSEUDO(C) (&C->L->void_pseudo)

struct multijmp {
	struct basic_block *target;
	long long begin, end;
};

struct asm_constraint {
	pseudo_t pseudo;
	const char *constraint;
	const struct ident *ident;
};

DECLARE_PTR_LIST(asm_constraint_list, struct asm_constraint);

struct asm_rules {
	struct asm_constraint_list *inputs; /* list of asm_constraint */
	struct asm_constraint_list *outputs; /* list of asm_constraint */
	struct asm_constraint_list *clobbers; /* list of asm_constraint */
};

struct instruction {
	unsigned opcode:8,
		 size:24;
	struct basic_block *bb;
	struct position pos;
	struct symbol *type;
	union {
		pseudo_t target;
		pseudo_t cond;		/* for branch and switch */
	};
	union {
		struct /* entrypoint */ {
			struct pseudo_list *arg_list; /* pseudo list */
		};
		struct /* branch */ {
			struct basic_block *bb_true, *bb_false;
		};
		struct /* switch */ {
			struct multijmp_list *multijmp_list;
		};
		struct /* phi_node */ {
			struct pseudo_list *phi_list; /* pseudo list */
		};
		struct /* phi source */ {
			pseudo_t phi_src;
			struct instruction_list *phi_users; /* instruction list */
		};
		struct /* unops */ {
			pseudo_t src;
			struct symbol *orig_type;	/* casts */
			unsigned int offset;		/* memops */
		};
		struct /* binops and sel */ {
			pseudo_t src1, src2, src3;
		};
		struct /* slice */ {
			pseudo_t base;
			unsigned from, len;
		};
		struct /* setval */ {
			pseudo_t symbol;		/* Subtle: same offset as "src" !! */
			struct expression *val;
		};
		struct /* call */ {
			pseudo_t func;
			struct pseudo_list *arguments; /* instruction list */
			struct symbol *fntype;
		};
		struct /* context */ {
			int increment;
			int check;
			struct expression *context_expr;
		};
		struct /* asm */ {
			const char *string;
			struct asm_rules *asm_rules;
		};
	};
};

enum opcode {
	OP_BADOP,

	/* Entry */
	OP_ENTRY,

	/* Terminator */
	OP_TERMINATOR,
	OP_RET = OP_TERMINATOR,
	OP_BR,
	OP_CBR,
	OP_SWITCH,
	OP_INVOKE,
	OP_COMPUTEDGOTO,
	OP_UNWIND,
	OP_TERMINATOR_END = OP_UNWIND,
	
	/* Binary */
	OP_BINARY,
	OP_ADD = OP_BINARY,
	OP_SUB,
	OP_MULU, OP_MULS,
	OP_DIVU, OP_DIVS,
	OP_MODU, OP_MODS,
	OP_SHL,
	OP_LSR, OP_ASR,
	
	/* Logical */
	OP_AND,
	OP_OR,
	OP_XOR,
	OP_AND_BOOL,
	OP_OR_BOOL,
	OP_BINARY_END = OP_OR_BOOL,

	/* Binary comparison */
	OP_BINCMP,
	OP_SET_EQ = OP_BINCMP,
	OP_SET_NE,
	OP_SET_LE,
	OP_SET_GE,
	OP_SET_LT,
	OP_SET_GT,
	OP_SET_B,
	OP_SET_A,
	OP_SET_BE,
	OP_SET_AE,
	OP_BINCMP_END = OP_SET_AE,

	/* Uni */
	OP_NOT,
	OP_NEG,

	/* Select - three input values */
	OP_SEL,
	
	/* Memory */
	OP_MALLOC,
	OP_FREE,
	OP_ALLOCA,
	OP_LOAD,
	OP_STORE,
	OP_SETVAL,
	OP_SYMADDR,
	OP_GET_ELEMENT_PTR,

	/* Other */
	OP_PHI,
	OP_PHISOURCE,
	OP_CAST,
	OP_SCAST,
	OP_FPCAST,
	OP_PTRCAST,
	OP_INLINED_CALL,
	OP_CALL,
	OP_VANEXT,
	OP_VAARG,
	OP_SLICE,
	OP_SNOP,
	OP_LNOP,
	OP_NOP,
	OP_DEATHNOTE,
	OP_ASM,

	/* Sparse tagging (line numbers, context, whatever) */
	OP_CONTEXT,
	OP_RANGE,

	/* Needed to translate SSA back to normal form */
	OP_COPY,
};

/*
A basic block represents a series of instructions with no branches. 
Straight-line code. A branch only occurs at the end of a basic block, 
and branches can only target the beginning of a basic block. Typically, 
a conditional will consist of a basic block leading up to the branch, 
a basic block for the true case, a basic block for the false case, 
and a basic block where the two paths merge back together. Either the true 
or the false case may not exist. A loop will normally have a basic block 
for the loop body, which can branch to the top at the end or continue 
to the next basic block. So basic blocks represent a node in the control 
flow graph. The edges in that graph lead from one basic block to a 
basic block which can follow it in the execution of the program.
*/
struct basic_block {
	struct position pos;
	unsigned long generation;
	int context;
	struct entrypoint *ep;
	struct basic_block_list *parents; /* basic_block sources */ /* predecessors */
	struct basic_block_list *children; /* basic_block destinations */ /* successors */
	struct instruction_list *insns;	/* Linear list of instructions */
	struct pseudo_list *needs, *defines; /* pseudo lists */
    /* TODO Following fields are used by the codegen backends.
       In Sparse this is a union but we need the nr field 
       for NanoJIT backend's liveness analysis in addition to 
       creating unique labels.
    */
	//union {
		unsigned int nr;	/* unique id for label's names */
		DMRC_BACKEND_TYPE priv;
	//};
};

static inline int dmrC_instruction_list_size(struct instruction_list *list)
{
	return ptrlist_size((struct ptr_list *)list);
}

static inline int dmrC_pseudo_list_size(struct pseudo_list *list)
{
	return ptrlist_size((struct ptr_list *)list);
}

static inline int dmrC_bb_list_size(struct basic_block_list *list)
{
	return ptrlist_size((struct ptr_list *)list);
}

static inline void dmrC_free_instruction_list(struct pseudo_list **head)
{
	ptrlist_remove_all((struct ptr_list **)head);
}

static inline struct instruction * dmrC_delete_last_instruction(struct instruction_list **head)
{
	return (struct instruction *) ptrlist_undo_last((struct ptr_list **)head);
}

static inline struct basic_block * dmrC_delete_last_basic_block(struct basic_block_list **head)
{
	return (struct basic_block *) ptrlist_delete_last((struct ptr_list **)head);
}

static inline struct basic_block *dmrC_first_basic_block(struct basic_block_list *head)
{
	return (struct basic_block *) ptrlist_first((struct ptr_list *)head);
}

static inline struct instruction *dmrC_last_instruction(struct instruction_list *head)
{
	return (struct instruction *) ptrlist_last((struct ptr_list *)head);
}

static inline struct instruction *dmrC_first_instruction(struct instruction_list *head)
{
	return (struct instruction *) ptrlist_first((struct ptr_list *)head);
}

static inline pseudo_t dmrC_first_pseudo(struct pseudo_list *head)
{
	return (pseudo_t) ptrlist_first((struct ptr_list *)head);
}

static inline void dmrC_concat_basic_block_list(struct basic_block_list *from, struct basic_block_list **to)
{
	ptrlist_concat((struct ptr_list *)from, (struct ptr_list **)to);
}

static inline void dmrC_concat_instruction_list(struct instruction_list *from, struct instruction_list **to)
{
	ptrlist_concat((struct ptr_list *)from, (struct ptr_list **)to);
}

static inline int dmrC_is_branch_goto(struct instruction *br)
{
	return br && br->opcode==OP_BR && (!br->bb_true || !br->bb_false);
}

static inline void dmrC_add_bb(struct dmr_C *C, struct basic_block_list **list, struct basic_block *bb)
{
	ptrlist_add((struct ptr_list **)list, bb, &C->ptrlist_allocator);
}

static inline void dmrC_add_instruction(struct dmr_C *C, struct instruction_list **list, struct instruction *insn)
{
	ptrlist_add((struct ptr_list **)list, insn, &C->ptrlist_allocator);
}

static inline void dmrC_add_multijmp(struct dmr_C *C, struct multijmp_list **list, struct multijmp *multijmp)
{
	ptrlist_add((struct ptr_list **)list, multijmp, &C->ptrlist_allocator);
}

static inline pseudo_t *dmrC_add_pseudo(struct dmr_C *C, struct pseudo_list **list, pseudo_t pseudo)
{
	return (pseudo_t *) ptrlist_add((struct ptr_list **)list, pseudo, &C->ptrlist_allocator);
}

static inline int dmrC_remove_pseudo(struct pseudo_list **list, pseudo_t pseudo)
{
	return ptrlist_remove((struct ptr_list **)list, pseudo, 0) != 0;
}

static inline int dmrC_bb_terminated(struct basic_block *bb)
{
	struct instruction *insn;
	if (!bb)
		return 0;
	insn = dmrC_last_instruction(bb->insns);
	return insn && insn->opcode >= OP_TERMINATOR
	            && insn->opcode <= OP_TERMINATOR_END;
}

static inline int dmrC_bb_reachable(struct basic_block *bb)
{
	return bb != NULL;
}


static inline void dmrC_add_pseudo_user_ptr(struct dmr_C *C, struct pseudo_user *user, struct pseudo_user_list **list)
{
	ptrlist_add((struct ptr_list **)list, user, &C->ptrlist_allocator);
}

static inline int dmrC_has_use_list(pseudo_t p)
{
	return (p && p->type != PSEUDO_VOID && p->type != PSEUDO_VAL);
}

static inline struct pseudo_user *dmrC_alloc_pseudo_user(struct dmr_C *C, struct instruction *insn, pseudo_t *pp)
{
	struct pseudo_user *user = (struct pseudo_user *) dmrC_allocator_allocate(&C->L->pseudo_user_allocator, 0);
	user->userp = pp;
	user->insn = insn;
	return user;
}

static inline void dmrC_use_pseudo(struct dmr_C *C, struct instruction *insn, pseudo_t p, pseudo_t *pp)
{
	*pp = p;
	if (dmrC_has_use_list(p))
		dmrC_add_pseudo_user_ptr(C, dmrC_alloc_pseudo_user(C, insn, pp), &p->users);
}

static inline void dmrC_remove_bb_from_list(struct basic_block_list **list, struct basic_block *entry, int count)
{
	ptrlist_remove((struct ptr_list **)list, entry, count);
}

static inline void dmrC_replace_bb_in_list(struct basic_block_list **list,
	struct basic_block *old, struct basic_block *newbb, int count)
{
	ptrlist_replace((struct ptr_list **)list, old, newbb, count);
}

struct entrypoint {
	struct symbol *name;
	struct symbol_list *syms; /* symbol list */
	struct pseudo_list *accesses; /* pseudo list */
	struct basic_block_list *bbs; /* basic_block list */
	struct basic_block *active;
	struct instruction *entry;
};

extern void dmrC_insert_select(struct dmr_C *C, struct basic_block *bb, struct instruction *br, struct instruction *phi, pseudo_t if_true, pseudo_t if_false);
extern void dmrC_insert_branch(struct dmr_C *C, struct basic_block *bb, struct instruction *br, struct basic_block *target);
// From Luc: sssa-mini
struct instruction *dmrC_alloc_phisrc(struct dmr_C *C, pseudo_t pseudo, struct symbol *type);
pseudo_t dmrC_insert_phi_node(struct dmr_C *C, struct basic_block *bb, struct symbol *type);
pseudo_t dmrC_alloc_phi(struct dmr_C *C, struct basic_block *source, pseudo_t pseudo, struct symbol *type);
pseudo_t dmrC_alloc_pseudo(struct dmr_C *C, struct instruction *def);
pseudo_t dmrC_value_pseudo(struct dmr_C *C, struct symbol *type, long long val);
unsigned int dmrC_value_size(long long value);

struct entrypoint *dmrC_linearize_symbol(struct dmr_C *C, struct symbol *sym);
int dmrC_unssa(struct dmr_C *C, struct entrypoint *ep);
void dmrC_show_entry(struct dmr_C *C, struct entrypoint *ep);
const char *dmrC_show_pseudo(struct dmr_C *C, pseudo_t pseudo);
void dmrC_show_bb(struct dmr_C *C, struct basic_block *bb);
const char *dmrC_show_instruction(struct dmr_C *C, struct instruction *insn);

void dmrC_convert_instruction_target(struct dmr_C *C, struct instruction *insn, pseudo_t src);
void dmrC_kill_use(struct dmr_C *C, pseudo_t *usep);
void dmrC_kill_insn(struct dmr_C *C, struct instruction *, int force);
void dmrC_kill_unreachable_bbs(struct dmr_C *C, struct entrypoint *ep);
void dmrC_kill_bb(struct dmr_C *C, struct basic_block *);

static inline void dmrC_kill_instruction(struct dmr_C *C, struct instruction *insn)
{
	dmrC_kill_insn(C, insn, 0);
}
static inline void dmrC_kill_instruction_force(struct dmr_C *C, struct instruction *insn)
{
	dmrC_kill_insn(C, insn, 1);
}

void dmrC_clear_liveness(struct entrypoint *ep);
void dmrC_track_pseudo_liveness(struct dmr_C *C, struct entrypoint *ep);
void dmrC_track_pseudo_death(struct dmr_C *C, struct entrypoint *ep);
void dmrC_track_phi_uses(struct dmr_C *C, struct instruction *insn);

void dmrC_init_linearizer(struct dmr_C *C);
void dmrC_destroy_linearizer(struct dmr_C *C);

#define dmrC_hashval(x) ((unsigned long)(((uintptr_t)(x))))

#define REPEAT_CSE		1
#define REPEAT_SYMBOL_CLEANUP	2
#define REPEAT_CFG_CLEANUP	3


#ifdef __cplusplus
}
#endif


#endif /* LINEARIZE_H */


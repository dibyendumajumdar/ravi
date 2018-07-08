/*
 * Register - track pseudo usage, maybe eventually try to do register
 * allocation.
 *
 * Copyright (C) 2004 Linus Torvalds
 */

#include <assert.h>

#include <port.h>
#include <parse.h>
#include <expression.h>
#include <linearize.h>
//#include <flow.h>

static void phi_defines(struct dmr_C *C, struct instruction * phi_node, pseudo_t target,
	void (*defines)(struct dmr_C *C, struct basic_block *, pseudo_t))
{
	pseudo_t phi;
	FOR_EACH_PTR(phi_node->phi_list, phi) {
		struct instruction *def;
		if (phi == VOID_PSEUDO(C))
			continue;
		def = phi->def;
		if (!def || !def->bb)
			continue;
		defines(C, def->bb, target);
	} END_FOR_EACH_PTR(phi);
}

static void asm_liveness(struct dmr_C *C, struct basic_block *bb, struct instruction *insn,
	void (*def)(struct dmr_C *C, struct basic_block *, pseudo_t),
	void (*use)(struct dmr_C *C, struct basic_block *, pseudo_t))
{
	struct asm_constraint *entry;

	FOR_EACH_PTR(insn->asm_rules->inputs, entry) {
		use(C, bb, entry->pseudo);
	} END_FOR_EACH_PTR(entry);
		
	FOR_EACH_PTR(insn->asm_rules->outputs, entry) {
		def(C, bb, entry->pseudo);
	} END_FOR_EACH_PTR(entry);
}

static void track_instruction_usage(struct dmr_C *C, struct basic_block *bb, struct instruction *insn,
	void (*def)(struct dmr_C *C, struct basic_block *, pseudo_t),
	void (*use)(struct dmr_C *C, struct basic_block *, pseudo_t))
{
	#define USES(x)		use(C, bb, insn->x)
	#define DEFINES(x)	def(C, bb, insn->x)

	switch (insn->opcode) {
	case OP_RET:
		USES(src);
		break;

	case OP_CBR: 
	case OP_SWITCH:
		USES(cond);
		break;

	case OP_COMPUTEDGOTO:
		USES(target);
		break;
	
	/* Binary */
	case OP_ADD:
	case OP_SUB:
	case OP_MULU:
	case OP_MULS:
	case OP_DIVU:
	case OP_DIVS:
	case OP_MODU:
	case OP_MODS:
	case OP_SHL:
	case OP_LSR:
	case OP_ASR:
	case OP_AND:
	case OP_OR:
	case OP_XOR:
	case OP_AND_BOOL:
	case OP_OR_BOOL:
	case OP_SET_EQ:
	case OP_SET_NE:
	case OP_SET_LE:
	case OP_SET_GE:
	case OP_SET_LT:
	case OP_SET_GT:
	case OP_SET_B:
	case OP_SET_A:
	case OP_SET_BE:
	case OP_SET_AE:
		USES(src1); USES(src2); DEFINES(target);
		break;

	/* Uni */
	case OP_NOT: case OP_NEG:
		USES(src1); DEFINES(target);
		break;

	case OP_SEL:
		USES(src1); USES(src2); USES(src3); DEFINES(target);
		break;
	
	/* Memory */
	case OP_LOAD:
		USES(src); DEFINES(target);
		break;

	case OP_STORE:
		USES(src); USES(target);
		break;

	case OP_SETVAL:
		DEFINES(target);
		break;

	case OP_SYMADDR:
		USES(symbol); DEFINES(target);
		break;

	/* Other */
	case OP_PHI:
		/* Phi-nodes are "backwards" nodes. Their def doesn't matter */
		phi_defines(C, insn, insn->target, def);
		break;

	case OP_PHISOURCE:
		/*
		 * We don't care about the phi-source define, they get set
		 * up and expanded by the OP_PHI
		 */
		USES(phi_src);
		break;

	case OP_CAST:
	case OP_SCAST:
	case OP_FPCAST:
	case OP_PTRCAST:
		USES(src); DEFINES(target);
		break;

	case OP_CALL: {
		pseudo_t arg;
		USES(func);
		if (insn->target != VOID_PSEUDO(C))
			DEFINES(target);
		FOR_EACH_PTR(insn->arguments, arg) {
			use(C, bb, arg);
		} END_FOR_EACH_PTR(arg);
		break;
	}

	case OP_SLICE:
		USES(base); DEFINES(target);
		break;

	case OP_ASM:
		asm_liveness(C, bb, insn, def, use);
		break;

	case OP_RANGE:
		USES(src1); USES(src2); USES(src3);
		break;

	case OP_BADOP:
	case OP_INVOKE:
	case OP_UNWIND:
	case OP_MALLOC:
	case OP_FREE:
	case OP_ALLOCA:
	case OP_GET_ELEMENT_PTR:
	case OP_VANEXT:
	case OP_VAARG:
	case OP_SNOP:
	case OP_LNOP:
	case OP_NOP:
	case OP_CONTEXT:
		break;
	}
}

int dmrC_pseudo_in_list(struct pseudo_list *list, pseudo_t pseudo)
{
	pseudo_t old;
	FOR_EACH_PTR(list,old) {
		if (old == pseudo)
			return 1;
	} END_FOR_EACH_PTR(old);   
	return 0;
}

static void add_pseudo_exclusive(struct dmr_C *C, struct pseudo_list **list, pseudo_t pseudo)
{
	if (!dmrC_pseudo_in_list(*list, pseudo)) {
		C->L->liveness_changed = 1;
		dmrC_add_pseudo(C, list, pseudo);
	}
}

static inline int trackable_pseudo(pseudo_t pseudo)
{
	return pseudo && (pseudo->type == PSEUDO_REG || pseudo->type == PSEUDO_ARG);
}

static void insn_uses(struct dmr_C *C, struct basic_block *bb, pseudo_t pseudo)
{
	if (trackable_pseudo(pseudo)) {
		struct instruction *def = pseudo->def;
		if (pseudo->type != PSEUDO_REG || def->bb != bb || def->opcode == OP_PHI)
			add_pseudo_exclusive(C, &bb->needs, pseudo);
	}
}

static void insn_defines(struct dmr_C *C, struct basic_block *bb, pseudo_t pseudo)
{
	assert(trackable_pseudo(pseudo));
	dmrC_add_pseudo(C, &bb->defines, pseudo);
}

static void track_bb_liveness(struct dmr_C *C, struct basic_block *bb)
{
	pseudo_t needs;

	FOR_EACH_PTR(bb->needs, needs) {
		struct basic_block *parent;
		FOR_EACH_PTR(bb->parents, parent) {
			if (!dmrC_pseudo_in_list(parent->defines, needs)) {
				add_pseudo_exclusive(C, &parent->needs, needs);
			}
		} END_FOR_EACH_PTR(parent);
	} END_FOR_EACH_PTR(needs);
}

/*
 * We need to clear the liveness information if we 
 * are going to re-run it.
 */
void dmrC_clear_liveness(struct entrypoint *ep)
{
	struct basic_block *bb;

	FOR_EACH_PTR(ep->bbs, bb) {
		ptrlist_remove_all((struct ptr_list **)&bb->needs);
		ptrlist_remove_all((struct ptr_list **)&bb->defines);
	} END_FOR_EACH_PTR(bb);
}

/*
 * Track inter-bb pseudo liveness. The intra-bb case
 * is purely local information.
 */
void dmrC_track_pseudo_liveness(struct dmr_C *C, struct entrypoint *ep)
{
	struct basic_block *bb;

	/* Add all the bb pseudo usage */
	FOR_EACH_PTR(ep->bbs, bb) {
		struct instruction *insn;
		FOR_EACH_PTR(bb->insns, insn) {
			if (!insn->bb)
				continue;
			assert(insn->bb == bb);
			track_instruction_usage(C, bb, insn, insn_defines, insn_uses);
		} END_FOR_EACH_PTR(insn);
	} END_FOR_EACH_PTR(bb);

	/* Calculate liveness.. */
	do {
		C->L->liveness_changed = 0;
		FOR_EACH_PTR_REVERSE(ep->bbs, bb) {
			track_bb_liveness(C, bb);
		} END_FOR_EACH_PTR_REVERSE(bb);
	} while (C->L->liveness_changed);

	/* Remove the pseudos from the "defines" list that are used internally */
	FOR_EACH_PTR(ep->bbs, bb) {
		pseudo_t def;
		FOR_EACH_PTR(bb->defines, def) {
			struct basic_block *child;
			FOR_EACH_PTR(bb->children, child) {
				if (dmrC_pseudo_in_list(child->needs, def))
					goto is_used;
			} END_FOR_EACH_PTR(child);
			DELETE_CURRENT_PTR(def);
is_used:
		;
		} END_FOR_EACH_PTR(def);
		ptrlist_pack((struct ptr_list **)&bb->defines);
	} END_FOR_EACH_PTR(bb);
}

static void merge_pseudo_list(struct dmr_C *C, struct pseudo_list *src, struct pseudo_list **dest)
{
	pseudo_t pseudo;
	FOR_EACH_PTR(src, pseudo) {
		add_pseudo_exclusive(C, dest, pseudo);
	} END_FOR_EACH_PTR(pseudo);
}

void dmrC_track_phi_uses(struct dmr_C *C, struct instruction *insn)
{
	pseudo_t phi;
	FOR_EACH_PTR(insn->phi_list, phi) {
		struct instruction *def;
		if (phi == VOID_PSEUDO(C) || !phi->def)
			continue;
		def = phi->def;
		assert(def->opcode == OP_PHISOURCE);
        dmrC_add_instruction(C, &def->phi_users, insn);
	} END_FOR_EACH_PTR(phi);
}

static void track_bb_phi_uses(struct dmr_C *C, struct basic_block *bb)
{
	struct instruction *insn;
	FOR_EACH_PTR(bb->insns, insn) {
		if (insn->bb && insn->opcode == OP_PHI)
			dmrC_track_phi_uses(C, insn);
	} END_FOR_EACH_PTR(insn);
}

static void death_def(struct dmr_C *C, struct basic_block *bb, pseudo_t pseudo)
{
	(void) C;
	(void) bb;
	(void) pseudo;
}

static void death_use(struct dmr_C *C, struct basic_block *bb, pseudo_t pseudo)
{
	(void) C;
	(void) bb;
	if (trackable_pseudo(pseudo) && !dmrC_pseudo_in_list(*C->L->live_list, pseudo)) {
		dmrC_add_pseudo(C, &C->L->dead_list, pseudo);
		dmrC_add_pseudo(C, C->L->live_list, pseudo);
	}
}

static void track_pseudo_death_bb(struct dmr_C *C, struct basic_block *bb)
{
	struct pseudo_list *live = NULL;
	struct basic_block *child;
	struct instruction *insn;

	FOR_EACH_PTR(bb->children, child) {
		merge_pseudo_list(C, child->needs, &live);
	} END_FOR_EACH_PTR(child);

	C->L->live_list = &live;
	FOR_EACH_PTR_REVERSE(bb->insns, insn) {
		if (!insn->bb)
			continue;

		C->L->dead_list = NULL;
		track_instruction_usage(C, bb, insn, death_def, death_use);
		if (C->L->dead_list) {
			pseudo_t dead;
			FOR_EACH_PTR(C->L->dead_list, dead) {
				struct instruction *deathnote = dmrC_allocator_allocate(&C->L->instruction_allocator, 0);
				deathnote->bb = bb;
				deathnote->opcode = OP_DEATHNOTE;
				deathnote->target = dead;
				INSERT_CURRENT(deathnote, insn);
			} END_FOR_EACH_PTR(dead);
			ptrlist_remove_all((struct ptr_list **)&C->L->dead_list);
		}
	} END_FOR_EACH_PTR_REVERSE(insn);
	ptrlist_remove_all((struct ptr_list **)&live);
}

void dmrC_track_pseudo_death(struct dmr_C *C, struct entrypoint *ep)
{
	struct basic_block *bb;

	FOR_EACH_PTR(ep->bbs, bb) {
		track_bb_phi_uses(C, bb);
	} END_FOR_EACH_PTR(bb);

	FOR_EACH_PTR(ep->bbs, bb) {
		track_pseudo_death_bb(C, bb);
	} END_FOR_EACH_PTR(bb);
}

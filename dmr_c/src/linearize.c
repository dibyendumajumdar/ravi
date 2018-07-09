/*
 * Linearize - walk the parse tree and generate a linear version 
 * of it and the basic blocks. 
 *
 * Copyright (C) 2004 Linus Torvalds
 * Copyright (C) 2004 Christopher Li
 */

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <port.h>
#include <parse.h>
#include <expression.h>
#include <linearize.h>
#if 0
#include <flow.h>
#endif
#include <target.h>

static pseudo_t linearize_statement(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt);
static pseudo_t linearize_expression(struct dmr_C *C, struct entrypoint *ep, struct expression *expr);

static pseudo_t add_binary_op(struct dmr_C *C, struct entrypoint *ep, struct symbol *ctype, int op, pseudo_t left, pseudo_t right);
static pseudo_t add_setval(struct dmr_C *C, struct entrypoint *ep, struct symbol *ctype, struct expression *val);
static pseudo_t linearize_one_symbol(struct dmr_C *C, struct entrypoint *ep, struct symbol *sym);

struct access_data;
static pseudo_t add_load(struct dmr_C *C, struct entrypoint *ep, struct access_data *);
static pseudo_t linearize_initializer(struct dmr_C *C, struct entrypoint *ep, struct expression *initializer, struct access_data *);
static pseudo_t cast_pseudo(struct dmr_C *C, struct entrypoint *ep, pseudo_t src, struct symbol *from, struct symbol *to);


static struct instruction *alloc_instruction(struct dmr_C *C, int opcode, int size)
{
	struct instruction * insn = (struct instruction *) dmrC_allocator_allocate(&C->L->instruction_allocator, 0);
	insn->opcode = opcode;
	insn->size = size;
	insn->pos = C->L->current_pos;
	return insn;
}

static inline int type_size(struct symbol *type)
{
	return type ? type->bit_size > 0 ? type->bit_size : 0 : 0;
}

static struct instruction *alloc_typed_instruction(struct dmr_C *C, int opcode, struct symbol *type)
{
	struct instruction *insn = alloc_instruction(C, opcode, type_size(type));
	insn->type = type;
	return insn;
}

static struct entrypoint *alloc_entrypoint(struct dmr_C *C)
{
	return (struct entrypoint *) dmrC_allocator_allocate(&C->L->entrypoint_allocator, 0);
}

static struct basic_block *alloc_basic_block(struct dmr_C *C, struct entrypoint *ep, struct position pos)
{
	struct basic_block *bb = (struct basic_block *) dmrC_allocator_allocate(&C->L->basic_block_allocator, 0);
	bb->context = -1;
	bb->pos = pos;
	bb->ep = ep;
	bb->nr = C->L->bb_nr++;
	return bb;
}

static struct multijmp *alloc_multijmp(struct dmr_C *C, struct basic_block *target, long long begin, long long end)
{
	struct multijmp *multijmp = (struct multijmp *)dmrC_allocator_allocate(&C->L->multijmp_allocator, 0);
	multijmp->target = target;
	multijmp->begin = begin;
	multijmp->end = end;
	return multijmp;
}

#if 0
static inline int regno(pseudo_t n)
{
	int retval = -1;
	if (n && n->type == PSEUDO_REG)
		retval = n->nr;
	return retval;
}
#endif

const char *dmrC_show_pseudo(struct dmr_C *C, pseudo_t pseudo)
{
	char *buf;
	int i;

	if (!pseudo)
		return "no pseudo";
	if (pseudo == VOID_PSEUDO(C))
		return "VOID";
	buf = C->L->pseudo_buffer[3 & ++C->L->n];
	switch(pseudo->type) {
	case PSEUDO_SYM: {
		struct symbol *sym = pseudo->sym;
		struct expression *expr;

		if (sym->bb_target) {
			snprintf(buf, 64, ".L%u", sym->bb_target->nr);
			break;
		}
		if (sym->ident) {
			snprintf(buf, 64, "%s", dmrC_show_ident(C, sym->ident));
			break;
		}
		expr = sym->initializer;
		snprintf(buf, 64, "<anon symbol:%p>", sym);
		if (expr) {
			switch (expr->type) {
			case EXPR_VALUE:
				snprintf(buf, 64, "<symbol value: %lld>", expr->value);
				break;
			case EXPR_STRING:
				return dmrC_show_string(C, expr->string);
			default:
				break;
			}
		}
		break;
	}
	case PSEUDO_REG:
		i = snprintf(buf, 64, "%%r%d", pseudo->nr);
		if (pseudo->ident)
			sprintf(buf+i, "(%s)", dmrC_show_ident(C, pseudo->ident));
		break;
	case PSEUDO_VAL: {
		long long value = pseudo->value;
		if (value > 1000 || value < -1000)
			snprintf(buf, 64, "$%#llx", value);
		else
			snprintf(buf, 64, "$%lld", value);
		break;
	}
	case PSEUDO_ARG:
		snprintf(buf, 64, "%%arg%d", pseudo->nr);
		break;
	case PSEUDO_PHI:
		i = snprintf(buf, 64, "%%phi%d", pseudo->nr);
		if (pseudo->ident)
			sprintf(buf+i, "(%s)", dmrC_show_ident(C, pseudo->ident));
		break;
	default:
		snprintf(buf, 64, "<bad pseudo type %d>", pseudo->type);
	}
	return buf;
}

static const char *opcodes[] = {
	[OP_BADOP] = "bad_op",

	/* Fn entrypoint */
	[OP_ENTRY] = "<entry-point>",

	/* Terminator */
	[OP_RET] = "ret",
	[OP_BR] = "br",
	[OP_CBR] = "cbr",
	[OP_SWITCH] = "switch",
	[OP_INVOKE] = "invoke",
	[OP_COMPUTEDGOTO] = "jmp *",
	[OP_UNWIND] = "unwind",
	
	/* Binary */
	[OP_ADD] = "add",
	[OP_SUB] = "sub",
	[OP_MULU] = "mulu",
	[OP_MULS] = "muls",
	[OP_DIVU] = "divu",
	[OP_DIVS] = "divs",
	[OP_MODU] = "modu",
	[OP_MODS] = "mods",
	[OP_SHL] = "shl",
	[OP_LSR] = "lsr",
	[OP_ASR] = "asr",
	
	/* Logical */
	[OP_AND] = "and",
	[OP_OR] = "or",
	[OP_XOR] = "xor",
	[OP_AND_BOOL] = "and-bool",
	[OP_OR_BOOL] = "or-bool",

	/* Binary comparison */
	[OP_SET_EQ] = "seteq",
	[OP_SET_NE] = "setne",
	[OP_SET_LE] = "setle",
	[OP_SET_GE] = "setge",
	[OP_SET_LT] = "setlt",
	[OP_SET_GT] = "setgt",
	[OP_SET_B] = "setb",
	[OP_SET_A] = "seta",
	[OP_SET_BE] = "setbe",
	[OP_SET_AE] = "setae",

	/* Uni */
	[OP_NOT] = "not",
	[OP_NEG] = "neg",

	/* Special three-input */
	[OP_SEL] = "select",
	
	/* Memory */
	[OP_MALLOC] = "malloc",
	[OP_FREE] = "free",
	[OP_ALLOCA] = "alloca",
	[OP_LOAD] = "load",
	[OP_STORE] = "store",
	[OP_SETVAL] = "set",
	[OP_SYMADDR] = "symaddr",
	[OP_GET_ELEMENT_PTR] = "getelem",

	/* Other */
	[OP_PHI] = "phi",
	[OP_PHISOURCE] = "phisrc",
	[OP_CAST] = "cast",
	[OP_SCAST] = "scast",
	[OP_FPCAST] = "fpcast",
	[OP_PTRCAST] = "ptrcast",
	[OP_INLINED_CALL] = "# call",
	[OP_CALL] = "call",
	[OP_VANEXT] = "va_next",
	[OP_VAARG] = "va_arg",
	[OP_SLICE] = "slice",
	[OP_SNOP] = "snop",
	[OP_LNOP] = "lnop",
	[OP_NOP] = "nop",
	[OP_DEATHNOTE] = "dead",
	[OP_ASM] = "asm",

	/* Sparse tagging (line numbers, context, whatever) */
	[OP_CONTEXT] = "context",
	[OP_RANGE] = "range-check",

	[OP_COPY] = "copy",
};

static char *show_asm_constraints(struct dmr_C *C, char *buf, const char *sep, struct asm_constraint_list *list)
{
	struct asm_constraint *entry;

	FOR_EACH_PTR(list, entry) {
		buf += sprintf(buf, "%s\"%s\"", sep, entry->constraint);
		if (entry->pseudo)
			buf += sprintf(buf, " (%s)", dmrC_show_pseudo(C, entry->pseudo));
		if (entry->ident)
			buf += sprintf(buf, " [%s]", dmrC_show_ident(C, entry->ident));
		sep = ", ";		
	} END_FOR_EACH_PTR(entry);
	return buf;
}

static char *show_asm(struct dmr_C *C, char *buf, struct instruction *insn)
{
	struct asm_rules *rules = insn->asm_rules;

	buf += sprintf(buf, "\"%s\"", insn->string);
	buf = show_asm_constraints(C, buf, "\n\t\tout: ", rules->outputs);
	buf = show_asm_constraints(C, buf, "\n\t\tin: ", rules->inputs);
	buf = show_asm_constraints(C, buf, "\n\t\tclobber: ", rules->clobbers);
	return buf;
}

const char *dmrC_show_instruction(struct dmr_C *C, struct instruction *insn)
{
	int opcode = insn->opcode;
	char *buf;

	buf = C->L->buffer;
	if (!insn->bb)
		buf += sprintf(buf, "# ");

	if (opcode < (int)ARRAY_SIZE(opcodes)) {
		const char *op = opcodes[opcode];
		if (!op)
			buf += sprintf(buf, "opcode:%d", opcode);
		else
			buf += sprintf(buf, "%s", op);
		if (insn->type && dmrC_is_float_type(C->S, insn->type))
			buf += sprintf(buf, ".f%d", insn->size);
		else if (insn->size)
			buf += sprintf(buf, ".%d", insn->size);
		if (insn->type) {
			if (dmrC_is_ptr_type(insn->type))
				buf += sprintf(buf, "*");
		}

		memset(buf, ' ', 20);
		buf++;
	}

	if (buf < C->L->buffer + 12)
		buf = C->L->buffer + 12;
	switch (opcode) {
	case OP_RET:
		if (insn->src && insn->src != VOID_PSEUDO(C))
			buf += sprintf(buf, "%s", dmrC_show_pseudo(C, insn->src));
		break;
	case OP_CBR:
		buf += sprintf(buf, "%s, .L%u, .L%u", dmrC_show_pseudo(C, insn->cond), insn->bb_true->nr, insn->bb_false->nr);
		break;

	case OP_BR:
		buf += sprintf(buf, ".L%u", insn->bb_true->nr);
		break;

	case OP_SYMADDR: {
		struct symbol *sym = insn->symbol->sym;
		buf += sprintf(buf, "%s <- ", dmrC_show_pseudo(C, insn->target));

		if (!insn->bb && !sym)
			break;
		if (sym->bb_target) {
			buf += sprintf(buf, ".L%u", sym->bb_target->nr);
			break;
		}
		if (sym->ident) {
			buf += sprintf(buf, "%s", dmrC_show_ident(C, sym->ident));
			break;
		}
		buf += sprintf(buf, "<anon symbol:%p>", sym);
		break;
	}
		
	case OP_SETVAL: {
		struct expression *expr = insn->val;
		struct symbol *sym;
		buf += sprintf(buf, "%s <- ", dmrC_show_pseudo(C, insn->target));

		if (!expr) {
			buf += sprintf(buf, "%s", "<none>");
			break;
		}
			
		switch (expr->type) {
		case EXPR_VALUE:
			buf += sprintf(buf, "%lld", expr->value);
			break;
		case EXPR_FVALUE:
			buf += sprintf(buf, "%Lf", expr->fvalue);
			break;
		case EXPR_STRING:
			buf += sprintf(buf, "%.40s", dmrC_show_string(C, expr->string));
			break;
		case EXPR_SYMBOL:
			buf += sprintf(buf, "%s", dmrC_show_ident(C, expr->symbol->ident));
			break;
		case EXPR_LABEL:
			sym = expr->symbol;
			if (sym->bb_target)
				buf += sprintf(buf, ".L%u", sym->bb_target->nr);
			break;
		default:
			buf += sprintf(buf, "SETVAL EXPR TYPE %d", expr->type);
		}
		break;
	}
	case OP_SWITCH: {
		struct multijmp *jmp;
		buf += sprintf(buf, "%s", dmrC_show_pseudo(C, insn->cond));
		FOR_EACH_PTR(insn->multijmp_list, jmp) {
			if (jmp->begin == jmp->end)
				buf += sprintf(buf, ", %lld -> .L%u", jmp->begin, jmp->target->nr);
			else if (jmp->begin < jmp->end)
				buf += sprintf(buf, ", %lld ... %lld -> .L%u", jmp->begin, jmp->end, jmp->target->nr);
			else
				buf += sprintf(buf, ", default -> .L%u", jmp->target->nr);
		} END_FOR_EACH_PTR(jmp);
		break;
	}
	case OP_COMPUTEDGOTO: {
		struct multijmp *jmp;
		buf += sprintf(buf, "%s", dmrC_show_pseudo(C, insn->target));
		FOR_EACH_PTR(insn->multijmp_list, jmp) {
			buf += sprintf(buf, ", .L%u", jmp->target->nr);
		} END_FOR_EACH_PTR(jmp);
		break;
	}

	case OP_PHISOURCE: {
		struct instruction *phi;
		buf += sprintf(buf, "%s <- %s    ", dmrC_show_pseudo(C, insn->target), dmrC_show_pseudo(C, insn->phi_src));
		FOR_EACH_PTR(insn->phi_users, phi) {
			buf += sprintf(buf, " (%s)", dmrC_show_pseudo(C, phi->target));
		} END_FOR_EACH_PTR(phi);
		break;
	}

	case OP_PHI: {
		pseudo_t phi;
		const char *s = " <-";
		buf += sprintf(buf, "%s", dmrC_show_pseudo(C, insn->target));
		FOR_EACH_PTR(insn->phi_list, phi) {
			buf += sprintf(buf, "%s %s", s, dmrC_show_pseudo(C, phi));
			s = ",";
		} END_FOR_EACH_PTR(phi);
		break;
	}	
	case OP_LOAD: case OP_LNOP:
		buf += sprintf(buf, "%s <- %d[%s]", dmrC_show_pseudo(C, insn->target), insn->offset, dmrC_show_pseudo(C, insn->src));
		if (insn->orig_type) {
			struct symbol *sym = insn->orig_type;
			if (sym->ident) {
				buf += sprintf(buf, "; %s", dmrC_show_ident(C, sym->ident));
			}
			else {
				buf += sprintf(buf, "; <anon symbol:%p>", sym);
			}
		}
		break;
	case OP_STORE: case OP_SNOP:
		buf += sprintf(buf, "%s -> %d[%s]", dmrC_show_pseudo(C, insn->target), insn->offset, dmrC_show_pseudo(C, insn->src));
		if (insn->orig_type) {
			struct symbol *sym = insn->orig_type;
			if (sym->ident) {
				buf += sprintf(buf, "; %s", dmrC_show_ident(C, sym->ident));
			}
			else {
				buf += sprintf(buf, "; <anon symbol:%p>", sym);
			}
		}
		break;
	case OP_INLINED_CALL:
	case OP_CALL: {
		if (insn->target && insn->target != VOID_PSEUDO(C))
			buf += sprintf(buf, "%s <- ", dmrC_show_pseudo(C, insn->target));
		buf += sprintf(buf, "%s", dmrC_show_pseudo(C, insn->func));
		{
			struct pseudo *arg;
			FOR_EACH_PTR(insn->arguments, arg) {
				buf += sprintf(buf, ", %s", dmrC_show_pseudo(C, arg));
			} END_FOR_EACH_PTR(arg);
		}
		break;
	}
	case OP_CAST:
	case OP_SCAST:
	case OP_FPCAST:
	case OP_PTRCAST:
		buf += sprintf(buf, "%s <- (%d) %s",
			dmrC_show_pseudo(C, insn->target),
			type_size(insn->orig_type),
			dmrC_show_pseudo(C, insn->src));
		break;
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

		buf += sprintf(buf, "%s <- %s, %s", dmrC_show_pseudo(C, insn->target), dmrC_show_pseudo(C, insn->src1), dmrC_show_pseudo(C, insn->src2));
		break;

	case OP_SEL:
		buf += sprintf(buf, "%s <- %s, %s, %s", dmrC_show_pseudo(C, insn->target),
			dmrC_show_pseudo(C, insn->src1), dmrC_show_pseudo(C, insn->src2), dmrC_show_pseudo(C, insn->src3));
		break;

	case OP_SLICE:
		buf += sprintf(buf, "%s <- %s, %d, %d", dmrC_show_pseudo(C, insn->target), dmrC_show_pseudo(C, insn->base), insn->from, insn->len);
		break;

	case OP_NOT: case OP_NEG:
		buf += sprintf(buf, "%s <- %s", dmrC_show_pseudo(C, insn->target), dmrC_show_pseudo(C, insn->src1));
		break;

	case OP_CONTEXT:
		buf += sprintf(buf, "%s%d", insn->check ? "check: " : "", insn->increment);
		break;
	case OP_RANGE:
		buf += sprintf(buf, "%s between %s..%s", dmrC_show_pseudo(C, insn->src1), dmrC_show_pseudo(C, insn->src2), dmrC_show_pseudo(C, insn->src3));
		break;
	case OP_NOP:
		buf += sprintf(buf, "%s <- %s", dmrC_show_pseudo(C, insn->target), dmrC_show_pseudo(C, insn->src1));
		break;
	case OP_DEATHNOTE:
		buf += sprintf(buf, "%s", dmrC_show_pseudo(C, insn->target));
		break;
	case OP_ASM:
		buf = show_asm(C, buf, insn);
		break;
	case OP_COPY:
		buf += sprintf(buf, "%s <- %s", dmrC_show_pseudo(C, insn->target), dmrC_show_pseudo(C, insn->src));
		break;
	default:
		break;
	}

	if (buf >= C->L->buffer + sizeof(C->L->buffer))
		dmrC_die(C, "instruction buffer overflowed %td\n", buf - C->L->buffer);
	do { --buf; } while (*buf == ' ');
	*++buf = 0;
	return C->L->buffer;
}

void dmrC_show_bb(struct dmr_C *C, struct basic_block *bb)
{
	struct instruction *insn;

	printf(".L%u:\n", bb->nr);
	if (C->verbose) {
		pseudo_t needs, defines;
		printf("%s:%d\n", dmrC_stream_name(C, bb->pos.stream), bb->pos.line);

		FOR_EACH_PTR(bb->needs, needs) {
			struct instruction *def = needs->def;
			if (def->opcode != OP_PHI) {
				printf("  **uses %s (from .L%u)**\n", dmrC_show_pseudo(C, needs), def->bb->nr);
			} else {
				pseudo_t phi;
				const char *sep = " ";
				printf("  **uses %s (from", dmrC_show_pseudo(C, needs));
				FOR_EACH_PTR(def->phi_list, phi) {
					if (phi == VOID_PSEUDO(C))
						continue;
					printf("%s(%s:.L%u)", sep, dmrC_show_pseudo(C, phi), phi->def->bb->nr);
					sep = ", ";
				} END_FOR_EACH_PTR(phi);		
				printf(")**\n");
			}
		} END_FOR_EACH_PTR(needs);

		FOR_EACH_PTR(bb->defines, defines) {
			printf("  **defines %s **\n", dmrC_show_pseudo(C, defines));
		} END_FOR_EACH_PTR(defines);

		if (bb->parents) {
			struct basic_block *from;
			FOR_EACH_PTR(bb->parents, from) {
				printf("  **from .L%u (%s:%d:%d)**\n", from->nr,
					dmrC_stream_name(C, from->pos.stream), from->pos.line, from->pos.pos);
			} END_FOR_EACH_PTR(from);
		}

		if (bb->children) {
			struct basic_block *to;
			FOR_EACH_PTR(bb->children, to) {
				printf("  **to .L%u (%s:%d:%d)**\n", to->nr,
					dmrC_stream_name(C, to->pos.stream), to->pos.line, to->pos.pos);
			} END_FOR_EACH_PTR(to);
		}
	}

	FOR_EACH_PTR(bb->insns, insn) {
		if (!insn->bb && C->verbose < 2)
			continue;
		printf("\t%s\n", dmrC_show_instruction(C, insn));
	} END_FOR_EACH_PTR(insn);
	if (!dmrC_bb_terminated(bb))
		printf("\tEND\n");
}

static void show_symbol_usage(struct dmr_C *C, pseudo_t pseudo)
{
	struct pseudo_user *pu;

	if (pseudo) {
		FOR_EACH_PTR(pseudo->users, pu) {
			printf("\t%s\n", dmrC_show_instruction(C, pu->insn));
		} END_FOR_EACH_PTR(pu);
	}
}

void dmrC_show_entry(struct dmr_C *C, struct entrypoint *ep)
{
	struct symbol *sym;
	struct basic_block *bb;

	printf("%s:\n", dmrC_show_ident(C, ep->name->ident));

	if (C->verbose) {
		printf("ep %p: %s\n", ep, dmrC_show_ident(C, ep->name->ident));

		FOR_EACH_PTR(ep->syms, sym) {
			if (!sym->pseudo)
				continue;
			if (!sym->pseudo->users)
				continue;
			printf("   sym: %p %s\n", sym, dmrC_show_ident(C, sym->ident));
			if (sym->ctype.modifiers & (MOD_EXTERN | MOD_STATIC | MOD_ADDRESSABLE))
				printf("\texternal visibility\n");
			show_symbol_usage(C, sym->pseudo);
		} END_FOR_EACH_PTR(sym);

		printf("\n");
	}

	FOR_EACH_PTR(ep->bbs, bb) {
		if (!bb)
			continue;
		if (!bb->parents && !bb->children && !bb->insns && C->verbose < 2)
			continue;
		dmrC_show_bb(C, bb);
		printf("\n");
	} END_FOR_EACH_PTR(bb);

	printf("\n");
}

static void bind_label(struct dmr_C *C, struct symbol *label, struct basic_block *bb, struct position pos)
{
	if (label->bb_target)
		dmrC_warning(C, pos, "label '%s' already bound", dmrC_show_ident(C, label->ident));
	label->bb_target = bb;
}

static struct basic_block * get_bound_block(struct dmr_C *C, struct entrypoint *ep, struct symbol *label)
{
	struct basic_block *bb = label->bb_target;

	if (!bb) {
		bb = alloc_basic_block(C, ep, label->pos);
		label->bb_target = bb;
	}
	return bb;
}

static void finish_block(struct entrypoint *ep)
{
	struct basic_block *src = ep->active;
	if (dmrC_bb_reachable(src))
		ep->active = NULL;
}

static void add_goto(struct dmr_C *C, struct entrypoint *ep, struct basic_block *dst)
{
	struct basic_block *src = ep->active;
	if (dmrC_bb_reachable(src)) {
		struct instruction *br = alloc_instruction(C, OP_BR, 0);
		br->bb_true = dst;
		dmrC_add_bb(C, &dst->parents, src);
		dmrC_add_bb(C, &src->children, dst);
		br->bb = src;
		dmrC_add_instruction(C, &src->insns, br);
		ep->active = NULL;
	}
}

static void add_one_insn(struct dmr_C *C, struct entrypoint *ep, struct instruction *insn)
{
	struct basic_block *bb = ep->active;    

	if (dmrC_bb_reachable(bb)) {
		insn->bb = bb;
		dmrC_add_instruction(C, &bb->insns, insn);
	}
}

static void set_activeblock(struct dmr_C *C, struct entrypoint *ep, struct basic_block *bb)
{
	if (!dmrC_bb_terminated(ep->active))
		add_goto(C, ep, bb);

	ep->active = bb;
	if (dmrC_bb_reachable(bb))
		dmrC_add_bb(C, &ep->bbs, bb);
}

static void remove_parent(struct dmr_C *C, struct basic_block *child, struct basic_block *parent)
{
	dmrC_remove_bb_from_list(&child->parents, parent, 1);
	if (!child->parents)
		C->L->repeat_phase |= REPEAT_CFG_CLEANUP;
}

/* Change a "switch" into a branch */
void dmrC_insert_branch(struct dmr_C *C, struct basic_block *bb, struct instruction *jmp, struct basic_block *target)
{
	struct instruction *br, *old;
	struct basic_block *child;

	/* Remove the switch */
	old = dmrC_delete_last_instruction(&bb->insns);
	assert(old == jmp);
	dmrC_kill_instruction(C, old);

	br = alloc_instruction(C, OP_BR, 0);
	br->bb = bb;
	br->bb_true = target;
	dmrC_add_instruction(C, &bb->insns, br);

	FOR_EACH_PTR(bb->children, child) {
		if (child == target) {
			target = NULL;	/* Trigger just once */
			continue;
		}
		DELETE_CURRENT_PTR(child);
		remove_parent(C, child, bb);
	} END_FOR_EACH_PTR(child);
	ptrlist_pack((struct ptr_list **)&bb->children);
}
	

void dmrC_insert_select(struct dmr_C *C, struct basic_block *bb, struct instruction *br, struct instruction *phi_node, pseudo_t if_true, pseudo_t if_false)
{
	pseudo_t target;
	struct instruction *select;

	/* Remove the 'br' */
	dmrC_delete_last_instruction(&bb->insns);

	select = alloc_typed_instruction(C, OP_SEL, phi_node->type);
	select->bb = bb;

	assert(br->cond);
	dmrC_use_pseudo(C, select, br->cond, &select->src1);

	target = phi_node->target;
	assert(target->def == phi_node);
	select->target = target;
	target->def = select;

	dmrC_use_pseudo(C, select, if_true, &select->src2);
	dmrC_use_pseudo(C, select, if_false, &select->src3);

	dmrC_add_instruction(C, &bb->insns, select);
	dmrC_add_instruction(C, &bb->insns, br);
}

static inline int bb_empty(struct basic_block *bb)
{
	return !bb->insns;
}

/* Add a label to the currently active block, return new active block */
static struct basic_block * add_label(struct dmr_C *C, struct entrypoint *ep, struct symbol *label)
{
	struct basic_block *bb = label->bb_target;

	if (bb) {
		set_activeblock(C, ep, bb);
		return bb;
	}
	bb = ep->active;
	if (!dmrC_bb_reachable(bb) || !bb_empty(bb)) {
		bb = alloc_basic_block(C, ep, label->pos);
		set_activeblock(C, ep, bb);
	}
	label->bb_target = bb;
	return bb;
}

static void add_branch(struct dmr_C *C, struct entrypoint *ep, struct expression *expr, pseudo_t cond, struct basic_block *bb_true, struct basic_block *bb_false)
{
        (void)expr;
	struct basic_block *bb = ep->active;
	struct instruction *br;

	if (dmrC_bb_reachable(bb)) {
       		br = alloc_instruction(C, OP_CBR, 0);
		dmrC_use_pseudo(C, br, cond, &br->cond);
		br->bb_true = bb_true;
		br->bb_false = bb_false;
		dmrC_add_bb(C, &bb_true->parents, bb);
		dmrC_add_bb(C, &bb_false->parents, bb);
		dmrC_add_bb(C, &bb->children, bb_true);
		dmrC_add_bb(C, &bb->children, bb_false);
		add_one_insn(C, ep, br);
	}
}

/* Dummy pseudo allocator */
pseudo_t dmrC_alloc_pseudo(struct dmr_C *C, struct instruction *def)
{
	struct pseudo * pseudo = (pseudo_t)dmrC_allocator_allocate(&C->L->pseudo_allocator, 0);
	pseudo->type = PSEUDO_REG;
	pseudo->nr = ++C->L->nr;
	pseudo->def = def;
	return pseudo;
}

static pseudo_t symbol_pseudo(struct dmr_C *C, struct entrypoint *ep, struct symbol *sym)
{
	pseudo_t pseudo;

	if (!sym)
		return VOID_PSEUDO(C);

	pseudo = sym->pseudo;
	if (!pseudo) {
		pseudo = (pseudo_t)dmrC_allocator_allocate(&C->L->pseudo_allocator, 0);
		pseudo->nr = -1;
		pseudo->type = PSEUDO_SYM;
		pseudo->sym = sym;
		pseudo->ident = sym->ident;
		sym->pseudo = pseudo;
		dmrC_add_pseudo(C, &ep->accesses, pseudo);
	}
	/* Symbol pseudos have neither nr, usage nor def */
	return pseudo;
}

unsigned int dmrC_value_size(long long value)
{
	value >>= 8;
	if (!value)
		return 8;
	value >>= 8;
	if (!value)
		return 16;
	value >>= 16;
	if (!value)
		return 32;
	return 64;
}

pseudo_t dmrC_value_pseudo(struct dmr_C *C, struct symbol *type, long long val)
{
	int hash = val & (MAX_VAL_HASH-1);
	struct pseudo_list **list = C->L->prev + hash;
	int size = type ? type->bit_size : dmrC_value_size(val);
	pseudo_t pseudo;

	assert(size == -1 || size <= (int)(sizeof(long long) * 8));

	FOR_EACH_PTR(*list, pseudo) {
		if (pseudo->value == val && pseudo->size == size)
			return pseudo;
	} END_FOR_EACH_PTR(pseudo);

	pseudo = (pseudo_t)dmrC_allocator_allocate(&C->L->pseudo_allocator, 0);
	pseudo->type = PSEUDO_VAL;
	pseudo->value = val;
	pseudo->size = size;
	dmrC_add_pseudo(C, list, pseudo);

	/* Value pseudos have neither nr, usage nor def */
	return pseudo;
}

static pseudo_t argument_pseudo(struct dmr_C *C, struct entrypoint *ep, int nr, struct symbol *arg)
{
	pseudo_t pseudo = (pseudo_t)dmrC_allocator_allocate(&C->L->pseudo_allocator, 0);
	struct instruction *entry = ep->entry;

	pseudo->type = PSEUDO_ARG;
	pseudo->nr = nr;
	pseudo->sym = arg;
	dmrC_add_pseudo(C, &entry->arg_list, pseudo);

	/* Argument pseudos have neither usage nor def */
	return pseudo;
}

// From Luc: sssa-mini
struct instruction *dmrC_alloc_phisrc(struct dmr_C *C, pseudo_t pseudo, struct symbol *type)
{
	struct instruction *insn = alloc_typed_instruction(C, OP_PHISOURCE, type);
	pseudo_t phi = (pseudo_t)dmrC_allocator_allocate(&C->L->pseudo_allocator, 0);
	static int nr = 0;

	phi->type = PSEUDO_PHI;
	phi->nr = ++nr;
	phi->def = insn;

	dmrC_use_pseudo(C, insn, pseudo, &insn->phi_src);
	insn->target = phi;
	return insn;
}

// From Luc: sssa-mini
pseudo_t dmrC_alloc_phi(struct dmr_C *C, struct basic_block *source, pseudo_t pseudo, struct symbol *type)
{
	struct instruction *insn;

	if (!source)
		return VOID_PSEUDO(C);

	insn = dmrC_alloc_phisrc(C, pseudo, type);
	insn->bb = source;
	dmrC_add_instruction(C, &source->insns, insn);
	return insn->target;
}

// From Luc: sssa-mini
pseudo_t dmrC_insert_phi_node(struct dmr_C *C, struct basic_block *bb, struct symbol *type)
{
	struct instruction *phi_node = alloc_typed_instruction(C, OP_PHI, type);
	struct instruction *insn;
	pseudo_t phi;

	phi = dmrC_alloc_pseudo(C, phi_node);
	phi_node->target = phi;
	phi_node->bb = bb;

	FOR_EACH_PTR(bb->insns, insn) {
		enum opcode op = insn->opcode;
		if (op == OP_ENTRY || op == OP_PHI)
			continue;
		INSERT_CURRENT(phi_node, insn);
		return phi;
	} END_FOR_EACH_PTR(insn);

	dmrC_add_instruction(C, &bb->insns, phi_node);
	return phi;
}

/*
 * We carry the "access_data" structure around for any accesses,
 * which simplifies things a lot. It contains all the access
 * information in one place.
 */
struct access_data {
	struct symbol *result_type;	// result ctype
	struct symbol *source_type;	// source ctype
	pseudo_t address;		// pseudo containing address ..
	unsigned int offset;		// byte offset
	struct position pos;
};

static void finish_address_gen(struct entrypoint *ep, struct access_data *ad)
{
        (void)ep;
        (void)ad;
}

static int linearize_simple_address(struct dmr_C *C, struct entrypoint *ep,
	struct expression *addr,
	struct access_data *ad)
{
	if (addr->type == EXPR_SYMBOL) {
		linearize_one_symbol(C, ep, addr->symbol);
		ad->address = symbol_pseudo(C, ep, addr->symbol);
		return 1;
	}
	if (addr->type == EXPR_BINOP) {
		if (addr->right->type == EXPR_VALUE) {
			if (addr->op == '+') {
				ad->offset += (unsigned int) dmrC_get_expression_value(C, addr->right);
				return linearize_simple_address(C, ep, addr->left, ad);
			}
		}
	}
	ad->address = linearize_expression(C, ep, addr);
	return 1;
}

static struct symbol *base_type(struct dmr_C *C, struct symbol *sym)
{
        (void)C;
	struct symbol *base = sym;

	if (sym) {
		if (sym->type == SYM_NODE)
			base = base->ctype.base_type;
		if (base->type == SYM_BITFIELD)
			return base->ctype.base_type;
	}
	return sym;
}

static int linearize_address_gen(struct dmr_C *C, struct entrypoint *ep,
	struct expression *expr,
	struct access_data *ad)
{
	struct symbol *ctype = expr->ctype;

	if (!ctype)
		return 0;
	ad->pos = expr->pos;
	ad->result_type = ctype;
	ad->source_type = base_type(C, ctype);
	if (expr->type == EXPR_PREOP && expr->op == '*')
		return linearize_simple_address(C, ep, expr->unop, ad);

	dmrC_warning(C, expr->pos, "generating address of non-lvalue (%d)", expr->type);
	return 0;
}

/* Try to get the actual struct type associated with a load or store */
static struct symbol *get_base_symbol_type(struct dmr_C *C, struct access_data *ad) {
        (void) C;
	struct symbol *orig_type = NULL;
	if (ad->address->type == PSEUDO_SYM) {
		orig_type = ad->address->sym;
	}
	else if (ad->address->type == PSEUDO_REG) {
		if (ad->address->def->opcode == OP_LOAD) {
			orig_type = ad->address->def->orig_type;
		}
	}
	if (orig_type) {
		if (orig_type->type == SYM_NODE)
			orig_type = orig_type->ctype.base_type;
		if (orig_type->type == SYM_PTR)
			orig_type = orig_type->ctype.base_type;
		//dmrC_show_type(C, orig_type);
	}
	return orig_type;
}

static pseudo_t add_load(struct dmr_C *C, struct entrypoint *ep, struct access_data *ad)
{
	struct instruction *insn;
	pseudo_t new;

	insn = alloc_typed_instruction(C, OP_LOAD, ad->source_type);
	/* save the address symbol so that backend can see what symbol we are accessing */
	insn->orig_type = get_base_symbol_type(C, ad);
	new = dmrC_alloc_pseudo(C, insn);

	insn->target = new;
	insn->offset = ad->offset;
	dmrC_use_pseudo(C, insn, ad->address, &insn->src);
	add_one_insn(C, ep, insn);
	return new;
}

static void add_store(struct dmr_C *C, struct entrypoint *ep, struct access_data *ad, pseudo_t value)
{
	struct basic_block *bb = ep->active;
	if (dmrC_bb_reachable(bb)) {
		struct instruction *store = alloc_typed_instruction(C, OP_STORE, ad->source_type);
		store->offset = ad->offset;
		/* save the address symbol so that backend can see what symbol we are accessing */
		store->orig_type = get_base_symbol_type(C, ad);
		dmrC_use_pseudo(C, store, value, &store->target);
		dmrC_use_pseudo(C, store, ad->address, &store->src);
		add_one_insn(C, ep, store);
	}
}

static pseudo_t linearize_store_gen(struct dmr_C *C, struct entrypoint *ep,
		pseudo_t value,
		struct access_data *ad)
{
	pseudo_t store = value;

	if (type_size(ad->source_type) != type_size(ad->result_type)) {
		struct symbol *ctype = ad->result_type;
		unsigned int shift = ctype->bit_offset;
		unsigned int size = ctype->bit_size;
		pseudo_t orig = add_load(C, ep, ad);
		unsigned long long mask = (1ULL << size) - 1;

		if (shift) {
			store = add_binary_op(C, ep, ad->source_type, OP_SHL, value, dmrC_value_pseudo(C, ctype, shift));
			mask <<= shift;
		}
		orig = add_binary_op(C, ep, ad->source_type, OP_AND, orig, dmrC_value_pseudo(C, ctype, ~mask));
		store = add_binary_op(C, ep, ad->source_type, OP_OR, orig, store);
	}
	add_store(C, ep, ad, store);
	return value;
}

static pseudo_t add_binary_op(struct dmr_C *C, struct entrypoint *ep, struct symbol *ctype, int op, pseudo_t left, pseudo_t right)
{
	struct instruction *insn = alloc_typed_instruction(C, op, ctype);
	pseudo_t target = dmrC_alloc_pseudo(C, insn);
	insn->target = target;
	dmrC_use_pseudo(C, insn, left, &insn->src1);
	dmrC_use_pseudo(C, insn, right, &insn->src2);
	add_one_insn(C, ep, insn);
	return target;
}

static pseudo_t add_setval(struct dmr_C *C, struct entrypoint *ep, struct symbol *ctype, struct expression *val)
{
	struct instruction *insn = alloc_typed_instruction(C, OP_SETVAL, ctype);
	pseudo_t target = dmrC_alloc_pseudo(C, insn);
	insn->target = target;
	insn->val = val;
	add_one_insn(C, ep, insn);
	return target;
}

static pseudo_t add_symbol_address(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	struct instruction *insn = alloc_typed_instruction(C, OP_SYMADDR, expr->ctype);
	struct symbol *sym = expr->symbol;
	pseudo_t target = dmrC_alloc_pseudo(C, insn);

	insn->target = target;
	dmrC_use_pseudo(C, insn, symbol_pseudo(C, ep, sym), &insn->symbol);
	add_one_insn(C, ep, insn);
	return target;
}

static pseudo_t linearize_load_gen(struct dmr_C *C, struct entrypoint *ep, struct access_data *ad)
{
	struct symbol *ctype = ad->result_type;
	pseudo_t new = add_load(C, ep, ad);

	if (ctype->bit_offset) {
		pseudo_t shift = dmrC_value_pseudo(C, ctype, ctype->bit_offset);
		pseudo_t newval = add_binary_op(C, ep, ad->source_type, OP_LSR, new, shift);
		new = newval;
	}
		
	if (ctype->bit_size != type_size(ad->source_type))
		new = cast_pseudo(C, ep, new, ad->source_type, ad->result_type);
	return new;
}

static pseudo_t linearize_access(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	struct access_data ad;
	pseudo_t value;
        
        memset(&ad, 0, sizeof ad);
	if (!linearize_address_gen(C, ep, expr, &ad))
		return VOID_PSEUDO(C);
	value = linearize_load_gen(C, ep, &ad);
	finish_address_gen(ep, &ad);
	return value;
}

/* FIXME: FP */
static pseudo_t linearize_inc_dec(struct dmr_C *C, struct entrypoint *ep, struct expression *expr, int postop)
{
	struct access_data ad;
	pseudo_t old, new, one;
        
        memset(&ad, 0, sizeof ad);
	int op = expr->op == SPECIAL_INCREMENT ? OP_ADD : OP_SUB;

	if (!linearize_address_gen(C, ep, expr->unop, &ad))
		return VOID_PSEUDO(C);

	old = linearize_load_gen(C, ep, &ad);
	one = dmrC_value_pseudo(C, expr->ctype, expr->op_value);
	new = add_binary_op(C, ep, expr->ctype, op, old, one);
	linearize_store_gen(C, ep, new, &ad);
	finish_address_gen(ep, &ad);
	return postop ? old : new;
}

static pseudo_t add_uniop(struct dmr_C *C, struct entrypoint *ep, struct expression *expr, int op, pseudo_t src)
{
	struct instruction *insn = alloc_typed_instruction(C, op, expr->ctype);
	pseudo_t new = dmrC_alloc_pseudo(C, insn);

	insn->target = new;
	dmrC_use_pseudo(C, insn, src, &insn->src1);
	add_one_insn(C, ep, insn);
	return new;
}

static pseudo_t linearize_slice(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	pseudo_t pre = linearize_expression(C, ep, expr->base);
	struct instruction *insn = alloc_typed_instruction(C, OP_SLICE, expr->ctype);
	pseudo_t new = dmrC_alloc_pseudo(C, insn);

	insn->target = new;
	insn->from = expr->r_bitpos;
	insn->len = expr->r_nrbits;
	dmrC_use_pseudo(C, insn, pre, &insn->base);
	add_one_insn(C, ep, insn);
	return new;
}

static pseudo_t linearize_regular_preop(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	pseudo_t pre = linearize_expression(C, ep, expr->unop);
	switch (expr->op) {
	case '+':
		return pre;
	case '!': {
		pseudo_t zero = dmrC_value_pseudo(C, expr->ctype, 0);
		return add_binary_op(C, ep, expr->ctype, OP_SET_EQ, pre, zero);
	}
	case '~':
		return add_uniop(C, ep, expr, OP_NOT, pre);
	case '-':
		return add_uniop(C, ep, expr, OP_NEG, pre);
	}
	return VOID_PSEUDO(C);
}

static pseudo_t linearize_preop(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	/*
	 * '*' is an lvalue access, and is fundamentally different
	 * from an arithmetic operation. Maybe it should have an
	 * expression type of its own..
	 */
	if (expr->op == '*')
		return linearize_access(C, ep, expr);
	if (expr->op == SPECIAL_INCREMENT || expr->op == SPECIAL_DECREMENT)
		return linearize_inc_dec(C, ep, expr, 0);
	return linearize_regular_preop(C, ep, expr);
}

static pseudo_t linearize_postop(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	return linearize_inc_dec(C, ep, expr, 1);
}	

/*
 * Casts to pointers are "less safe" than other casts, since
 * they imply type-unsafe accesses. "void *" is a special
 * case, since you can't access through it anyway without another
 * cast.
 */
static struct instruction *alloc_cast_instruction(struct dmr_C *C, struct symbol *src, struct symbol *ctype)
{
	int opcode = OP_CAST;
	// https://patchwork.kernel.org/patch/9516077/
	struct symbol *base = ctype;

	if (src->ctype.modifiers & MOD_SIGNED)
		opcode = OP_SCAST;
	if (base->type == SYM_NODE)
		base = base->ctype.base_type;
	if (base->type == SYM_PTR) {
		base = base->ctype.base_type;
		if (base != &C->S->void_ctype)
			opcode = OP_PTRCAST;
	} else if (base->ctype.base_type == &C->S->fp_type)
		opcode = OP_FPCAST;
	return alloc_typed_instruction(C, opcode, ctype);
}

static pseudo_t cast_pseudo(struct dmr_C *C, struct entrypoint *ep, pseudo_t src, struct symbol *from, struct symbol *to)
{
	pseudo_t result;
	struct instruction *insn;

	if (src == VOID_PSEUDO(C))
		return VOID_PSEUDO(C);
	if (!from || !to)
		return VOID_PSEUDO(C);
	if (from->bit_size < 0 || to->bit_size < 0)
		return VOID_PSEUDO(C);
	insn = alloc_cast_instruction(C, from, to);
	result = dmrC_alloc_pseudo(C, insn);
	insn->target = result;
	insn->orig_type = from;
	dmrC_use_pseudo(C, insn, src, &insn->src);
	add_one_insn(C, ep, insn);
	return result;
}

static int opcode_sign(struct dmr_C *C, int opcode, struct symbol *ctype)
{
        (void)C;
	if (ctype && (ctype->ctype.modifiers & MOD_SIGNED)) {
		switch(opcode) {
		case OP_MULU: case OP_DIVU: case OP_MODU: case OP_LSR:
			opcode++;
		}
	}
	return opcode;
}

static inline pseudo_t add_convert_to_bool(struct dmr_C *C, struct entrypoint *ep, pseudo_t src, struct symbol *type)
{
	pseudo_t zero;
	int op;

	if (dmrC_is_bool_type(C->S, type))
		return src;
	zero = dmrC_value_pseudo(C, type, 0);
	op = OP_SET_NE;
	return add_binary_op(C, ep, &C->S->bool_ctype, op, src, zero);
}

static pseudo_t linearize_expression_to_bool(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	pseudo_t dst;
	dst = linearize_expression(C, ep, expr);
	dst = add_convert_to_bool(C, ep, dst, expr->ctype);
	return dst;
}
static pseudo_t linearize_assignment(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	struct access_data ad;
	struct expression *target = expr->left;
	struct expression *src = expr->right;
	struct symbol *ctype;
	pseudo_t value;

        memset(&ad, 0, sizeof ad);
	value = linearize_expression(C, ep, src);
	if (!target || !linearize_address_gen(C, ep, target, &ad))
		return value;
	if (expr->op != '=') {
		pseudo_t oldvalue = linearize_load_gen(C, ep, &ad);
		pseudo_t dst;
		static const int op_trans[] = {
			[SPECIAL_ADD_ASSIGN - SPECIAL_BASE] = OP_ADD,
			[SPECIAL_SUB_ASSIGN - SPECIAL_BASE] = OP_SUB,
			[SPECIAL_MUL_ASSIGN - SPECIAL_BASE] = OP_MULU,
			[SPECIAL_DIV_ASSIGN - SPECIAL_BASE] = OP_DIVU,
			[SPECIAL_MOD_ASSIGN - SPECIAL_BASE] = OP_MODU,
			[SPECIAL_SHL_ASSIGN - SPECIAL_BASE] = OP_SHL,
			[SPECIAL_SHR_ASSIGN - SPECIAL_BASE] = OP_LSR,
			[SPECIAL_AND_ASSIGN - SPECIAL_BASE] = OP_AND,
			[SPECIAL_OR_ASSIGN  - SPECIAL_BASE] = OP_OR,
			[SPECIAL_XOR_ASSIGN - SPECIAL_BASE] = OP_XOR
		};
		int opcode;

		if (!src)
			return VOID_PSEUDO(C);

		ctype = src->ctype;
		oldvalue = cast_pseudo(C, ep, oldvalue, target->ctype, ctype);
		opcode = opcode_sign(C, op_trans[expr->op - SPECIAL_BASE], ctype);
		dst = add_binary_op(C, ep, ctype, opcode, oldvalue, value);

		value = cast_pseudo(C, ep, dst, ctype, expr->ctype);
	}
	value = linearize_store_gen(C, ep, value, &ad);
	finish_address_gen(ep, &ad);
	return value;
}

static pseudo_t linearize_call_expression(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	struct expression *arg, *fn;
	struct instruction *insn = alloc_typed_instruction(C, OP_CALL, expr->ctype);
	pseudo_t retval, call;
	struct ctype *ctype = NULL;
	struct symbol *fntype;
	struct context *context;

	if (!expr->ctype) {
		dmrC_warning(C, expr->pos, "call with no type!");
		return VOID_PSEUDO(C);
	}

	// first generate all the parameters
	FOR_EACH_PTR(expr->args, arg) {
		pseudo_t new = linearize_expression(C, ep, arg);
		dmrC_use_pseudo(C, insn, new, dmrC_add_pseudo(C, &insn->arguments, new));
	} END_FOR_EACH_PTR(arg);

	fn = expr->fn;

	if (fn->ctype)
		ctype = &fn->ctype->ctype;

	fntype = fn->ctype;
	if (fntype) {
		if (fntype->type == SYM_NODE)
			fntype = fntype->ctype.base_type;
	}
	insn->fntype = fntype;

	if (fn->type == EXPR_PREOP) {
		if (fn->unop->type == EXPR_SYMBOL) {
			struct symbol *sym = fn->unop->symbol;
			if (sym->ctype.base_type->type == SYM_FN)
				fn = fn->unop;
		}
	}
	if (fn->type == EXPR_SYMBOL) {
		call = symbol_pseudo(C, ep, fn->symbol);
	} else {
		call = linearize_expression(C, ep, fn);
	}
	dmrC_use_pseudo(C, insn, call, &insn->func);
	retval = VOID_PSEUDO(C);
	if (expr->ctype != &C->S->void_ctype)
		retval = dmrC_alloc_pseudo(C, insn);
	insn->target = retval;
	add_one_insn(C, ep, insn);

	if (ctype) {
		FOR_EACH_PTR(ctype->contexts, context) {
			int in = context->in;
			int out = context->out;
			int check = 0;
			int context_diff;
			if (in < 0) {
				check = 1;
				in = 0;
			}
			if (out < 0) {
				check = 0;
				out = 0;
			}
			context_diff = out - in;
			if (check || context_diff) {
				insn = alloc_instruction(C, OP_CONTEXT, 0);
				insn->increment = context_diff;
				insn->check = check;
				insn->context_expr = context->context_expr;
				add_one_insn(C, ep, insn);
			}
		} END_FOR_EACH_PTR(context);
	}

	return retval;
}

static pseudo_t linearize_binop_bool(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	pseudo_t src1, src2, dst;
	int op = (expr->op == SPECIAL_LOGICAL_OR) ? OP_OR_BOOL : OP_AND_BOOL;

	src1 = linearize_expression_to_bool(C, ep, expr->left);
	src2 = linearize_expression_to_bool(C, ep, expr->right);
	dst = add_binary_op(C, ep, &C->S->bool_ctype, op, src1, src2);
	if (expr->ctype != &C->S->bool_ctype)
		dst = cast_pseudo(C, ep, dst, &C->S->bool_ctype, expr->ctype);
	return dst;
}
static pseudo_t linearize_binop(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	pseudo_t src1, src2, dst;
	static const int opcode[] = {
		['+'] = OP_ADD, ['-'] = OP_SUB,
		['*'] = OP_MULU, ['/'] = OP_DIVU,
		['%'] = OP_MODU, ['&'] = OP_AND,
		['|'] = OP_OR,  ['^'] = OP_XOR,
		[SPECIAL_LEFTSHIFT] = OP_SHL,
		[SPECIAL_RIGHTSHIFT] = OP_LSR,
	};
	int op;

	src1 = linearize_expression(C, ep, expr->left);
	src2 = linearize_expression(C, ep, expr->right);
	op = opcode_sign(C, opcode[expr->op], expr->ctype);
	dst = add_binary_op(C, ep, expr->ctype, op, src1, src2);
	return dst;
}

static pseudo_t linearize_logical_branch(struct dmr_C *C, struct entrypoint *ep, struct expression *expr, struct basic_block *bb_true, struct basic_block *bb_false);

static pseudo_t linearize_cond_branch(struct dmr_C *C, struct entrypoint *ep, struct expression *expr, struct basic_block *bb_true, struct basic_block *bb_false);

static pseudo_t linearize_select(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	pseudo_t cond, true, false, res;
	struct instruction *insn;

	true = linearize_expression(C, ep, expr->cond_true);
	false = linearize_expression(C, ep, expr->cond_false);
	cond = linearize_expression(C, ep, expr->conditional);

	insn = alloc_typed_instruction(C, OP_SEL, expr->ctype);
	if (!expr->cond_true)
		true = cond;
	dmrC_use_pseudo(C, insn, cond, &insn->src1);
	dmrC_use_pseudo(C, insn, true, &insn->src2);
	dmrC_use_pseudo(C, insn, false, &insn->src3);

	res = dmrC_alloc_pseudo(C, insn);
	insn->target = res;
	add_one_insn(C, ep, insn);
	return res;
}

static pseudo_t add_join_conditional(struct dmr_C *C, struct entrypoint *ep, struct expression *expr,
				     pseudo_t phi1, pseudo_t phi2)
{
	pseudo_t target;
	struct instruction *phi_node;

	if (phi1 == VOID_PSEUDO(C))
		return phi2;
	if (phi2 == VOID_PSEUDO(C))
		return phi1;

	phi_node = alloc_typed_instruction(C, OP_PHI, expr->ctype);
	dmrC_use_pseudo(C, phi_node, phi1, dmrC_add_pseudo(C, &phi_node->phi_list, phi1));
	dmrC_use_pseudo(C, phi_node, phi2, dmrC_add_pseudo(C, &phi_node->phi_list, phi2));
	phi_node->target = target = dmrC_alloc_pseudo(C, phi_node);
	add_one_insn(C, ep, phi_node);
	return target;
}	

static pseudo_t linearize_short_conditional(struct dmr_C *C, struct entrypoint *ep, struct expression *expr,
					    struct expression *cond,
					    struct expression *expr_false)
{
	pseudo_t src1, src2;
	struct basic_block *bb_false;
	struct basic_block *merge = alloc_basic_block(C, ep, expr->pos);
	pseudo_t phi1, phi2;

	if (!expr_false || !ep->active)
		return VOID_PSEUDO(C);

	bb_false = alloc_basic_block(C, ep, expr_false->pos);
	src1 = linearize_expression(C, ep, cond);
	phi1 = dmrC_alloc_phi(C, ep->active, src1, expr->ctype);
	add_branch(C, ep, expr, src1, merge, bb_false);

	set_activeblock(C, ep, bb_false);
	src2 = linearize_expression(C, ep, expr_false);
	phi2 = dmrC_alloc_phi(C, ep->active, src2, expr->ctype);
	set_activeblock(C, ep, merge);

	return add_join_conditional(C, ep, expr, phi1, phi2);
}

static pseudo_t linearize_conditional(struct dmr_C *C, struct entrypoint *ep, struct expression *expr,
				      struct expression *cond,
				      struct expression *expr_true,
				      struct expression *expr_false)
{
	pseudo_t src1, src2;
	pseudo_t phi1, phi2;
	struct basic_block *bb_true, *bb_false, *merge;

	if (!cond || !expr_true || !expr_false || !ep->active)
		return VOID_PSEUDO(C);
	bb_true = alloc_basic_block(C, ep, expr_true->pos);
	bb_false = alloc_basic_block(C, ep, expr_false->pos);
	merge = alloc_basic_block(C, ep, expr->pos);

	linearize_cond_branch(C, ep, cond, bb_true, bb_false);

	set_activeblock(C, ep, bb_true);
	src1 = linearize_expression(C, ep, expr_true);
	phi1 = dmrC_alloc_phi(C, ep->active, src1, expr->ctype);
	add_goto(C, ep, merge); 

	set_activeblock(C, ep, bb_false);
	src2 = linearize_expression(C, ep, expr_false);
	phi2 = dmrC_alloc_phi(C, ep->active, src2, expr->ctype);
	set_activeblock(C, ep, merge);

	return add_join_conditional(C, ep, expr, phi1, phi2);
}

static pseudo_t linearize_logical(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	struct expression *shortcut;

	shortcut = dmrC_alloc_const_expression(C, expr->pos, expr->op == SPECIAL_LOGICAL_OR);
	shortcut->ctype = expr->ctype;
	if (expr->op == SPECIAL_LOGICAL_OR)
		return linearize_conditional(C, ep, expr, expr->left, shortcut, expr->right);
	return linearize_conditional(C, ep, expr, expr->left, expr->right, shortcut);
}

static pseudo_t linearize_compare(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	static const int cmpop[] = {
		['>'] = OP_SET_GT, ['<'] = OP_SET_LT,
		[SPECIAL_EQUAL] = OP_SET_EQ,
		[SPECIAL_NOTEQUAL] = OP_SET_NE,
		[SPECIAL_GTE] = OP_SET_GE,
		[SPECIAL_LTE] = OP_SET_LE,
		[SPECIAL_UNSIGNED_LT] = OP_SET_B,
		[SPECIAL_UNSIGNED_GT] = OP_SET_A,
		[SPECIAL_UNSIGNED_LTE] = OP_SET_BE,
		[SPECIAL_UNSIGNED_GTE] = OP_SET_AE,
	};

	pseudo_t src1 = linearize_expression(C, ep, expr->left);
	pseudo_t src2 = linearize_expression(C, ep, expr->right);
	pseudo_t dst = add_binary_op(C, ep, expr->ctype, cmpop[expr->op], src1, src2);
	return dst;
}


static pseudo_t linearize_cond_branch(struct dmr_C *C, struct entrypoint *ep, struct expression *expr, struct basic_block *bb_true, struct basic_block *bb_false)
{
	pseudo_t cond;

	if (!expr || !dmrC_bb_reachable(ep->active))
		return VOID_PSEUDO(C);

	switch (expr->type) {

	case EXPR_STRING:
	case EXPR_VALUE:
		add_goto(C, ep, expr->value ? bb_true : bb_false);
		return VOID_PSEUDO(C);

	case EXPR_FVALUE:
		add_goto(C, ep, expr->fvalue ? bb_true : bb_false);
		return VOID_PSEUDO(C);
		
	case EXPR_LOGICAL:
		linearize_logical_branch(C, ep, expr, bb_true, bb_false);
		return VOID_PSEUDO(C);

	case EXPR_COMPARE:
		cond = linearize_compare(C, ep, expr);
		add_branch(C, ep, expr, cond, bb_true, bb_false);
		break;
		
	case EXPR_PREOP:
		if (expr->op == '!')
			return linearize_cond_branch(C, ep, expr->unop, bb_false, bb_true);
		/* fall through */
	default: {
		cond = linearize_expression(C, ep, expr);
		add_branch(C, ep, expr, cond, bb_true, bb_false);

		return VOID_PSEUDO(C);
	}
	}
	return VOID_PSEUDO(C);
}


	
static pseudo_t linearize_logical_branch(struct dmr_C *C, struct entrypoint *ep, struct expression *expr, struct basic_block *bb_true, struct basic_block *bb_false)
{
	struct basic_block *next = alloc_basic_block(C, ep, expr->pos);

	if (expr->op == SPECIAL_LOGICAL_OR)
		linearize_cond_branch(C, ep, expr->left, bb_true, next);
	else
		linearize_cond_branch(C, ep, expr->left, next, bb_false);
	set_activeblock(C, ep, next);
	linearize_cond_branch(C, ep, expr->right, bb_true, bb_false);
	return VOID_PSEUDO(C);
}

static pseudo_t linearize_cast(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	pseudo_t src;
	struct expression *orig = expr->cast_expression;

	if (!orig)
		return VOID_PSEUDO(C);

	src = linearize_expression(C, ep, orig);
	return cast_pseudo(C, ep, src, orig->ctype, expr->ctype);
}

static pseudo_t linearize_position(struct dmr_C *C, struct entrypoint *ep, struct expression *pos, struct access_data *ad)
{
	struct expression *init_expr = pos->init_expr;

	ad->offset = pos->init_offset;
	ad->source_type = base_type(C, init_expr->ctype);
	ad->result_type = init_expr->ctype;
	return linearize_initializer(C, ep, init_expr, ad);
}

static pseudo_t linearize_initializer(struct dmr_C *C, struct entrypoint *ep, struct expression *initializer, struct access_data *ad)
{
	switch (initializer->type) {
	case EXPR_INITIALIZER: {
		struct expression *expr;
		FOR_EACH_PTR(initializer->expr_list, expr) {
			linearize_initializer(C, ep, expr, ad);
		} END_FOR_EACH_PTR(expr);
		break;
	}
	case EXPR_POS:
		linearize_position(C, ep, initializer, ad);
		break;
	default: {
		pseudo_t value = linearize_expression(C, ep, initializer);
		ad->source_type = base_type(C, initializer->ctype);
		ad->result_type = initializer->ctype;
		linearize_store_gen(C, ep, value, ad);
		return value;
	}
	}

	return VOID_PSEUDO(C);
}

static void linearize_argument(struct dmr_C *C, struct entrypoint *ep, struct symbol *arg, int nr)
{
	struct access_data ad;

        memset(&ad, 0, sizeof ad);
	ad.source_type = arg;
	ad.result_type = arg;
	ad.address = symbol_pseudo(C, ep, arg);
	linearize_store_gen(C, ep, argument_pseudo(C, ep, nr, arg), &ad);
	finish_address_gen(ep, &ad);
}

static pseudo_t linearize_expression(struct dmr_C *C, struct entrypoint *ep, struct expression *expr)
{
	if (!expr)
		return VOID_PSEUDO(C);

	C->L->current_pos = expr->pos;
	switch (expr->type) {
	case EXPR_SYMBOL:
		linearize_one_symbol(C, ep, expr->symbol);
		return add_symbol_address(C, ep, expr);

	case EXPR_VALUE:
		return dmrC_value_pseudo(C, expr->ctype, expr->value);

	case EXPR_STRING: case EXPR_FVALUE: case EXPR_LABEL:
		return add_setval(C, ep, expr->ctype, expr);

	case EXPR_STATEMENT:
		return linearize_statement(C, ep, expr->statement);

	case EXPR_CALL:
		return linearize_call_expression(C, ep, expr);

	case EXPR_BINOP:
		if (expr->op == SPECIAL_LOGICAL_AND || expr->op == SPECIAL_LOGICAL_OR)
			return linearize_binop_bool(C, ep, expr);
		return linearize_binop(C, ep, expr);

	case EXPR_LOGICAL:
		return linearize_logical(C, ep, expr);

	case EXPR_COMPARE:
		return  linearize_compare(C, ep, expr);

	case EXPR_SELECT:
		return	linearize_select(C, ep, expr);

	case EXPR_CONDITIONAL:
		if (!expr->cond_true)
			return linearize_short_conditional(C, ep, expr, expr->conditional, expr->cond_false);

		return  linearize_conditional(C, ep, expr, expr->conditional,
					      expr->cond_true, expr->cond_false);

	case EXPR_COMMA:
		linearize_expression(C, ep, expr->left);
		return linearize_expression(C, ep, expr->right);

	case EXPR_ASSIGNMENT:
		return linearize_assignment(C, ep, expr);

	case EXPR_PREOP:
		return linearize_preop(C, ep, expr);

	case EXPR_POSTOP:
		return linearize_postop(C, ep, expr);

	case EXPR_CAST:
	case EXPR_FORCE_CAST:
	case EXPR_IMPLIED_CAST:
		return linearize_cast(C, ep, expr);
	
	case EXPR_SLICE:
		return linearize_slice(C, ep, expr);

	case EXPR_INITIALIZER:
	case EXPR_POS:
		dmrC_warning(C, expr->pos, "unexpected initializer expression (%d %d)", expr->type, expr->op);
		return VOID_PSEUDO(C);
	default: 
		dmrC_warning(C, expr->pos, "unknown expression (%d %d)", expr->type, expr->op);
		return VOID_PSEUDO(C);
	}
	return VOID_PSEUDO(C);
}

static pseudo_t linearize_one_symbol(struct dmr_C *C, struct entrypoint *ep, struct symbol *sym)
{
	struct access_data ad;
	pseudo_t value;
        
        memset(&ad, 0, sizeof ad);
	if (!sym || !sym->initializer || sym->initialized)
		return VOID_PSEUDO(C);

	/* We need to output these puppies some day too.. */
	if (sym->ctype.modifiers & (MOD_STATIC | MOD_TOPLEVEL))
		return VOID_PSEUDO(C);

	sym->initialized = 1;
	ad.address = symbol_pseudo(C, ep, sym);
	value = linearize_initializer(C, ep, sym->initializer, &ad);
	finish_address_gen(ep, &ad);
	return value;
}

static pseudo_t linearize_compound_statement(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt)
{
	pseudo_t pseudo;
	struct statement *s;
	struct symbol *ret = stmt->ret;

	pseudo = VOID_PSEUDO(C);
	FOR_EACH_PTR(stmt->stmts, s) {
		pseudo = linearize_statement(C, ep, s);
	} END_FOR_EACH_PTR(s);

	if (ret) {
		struct basic_block *bb = add_label(C, ep, ret);
		struct instruction *phi_node = dmrC_first_instruction(bb->insns);

		if (!phi_node)
			return pseudo;
#if 0
		/* https://github.com/lucvoo/sparse/commit/1609176c9 */
		if (dmrC_pseudo_list_size(phi_node->phi_list)==1) {
			pseudo = dmrC_first_pseudo(phi_node->phi_list);
			assert(pseudo->type == PSEUDO_PHI);
			return pseudo->def->src1;
		}
#endif
		return phi_node->target;
	}

	return pseudo;
}

static pseudo_t linearize_inlined_call(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt)
{
	struct instruction *insn = alloc_instruction(C, OP_INLINED_CALL, 0);
	struct statement *args = stmt->args;
	struct basic_block *bb;
	pseudo_t pseudo;

	if (args) {
		struct symbol *sym;

		dmrC_concat_symbol_list(args->declaration, &ep->syms);
		FOR_EACH_PTR(args->declaration, sym) {
			pseudo_t value = linearize_one_symbol(C, ep, sym);
			dmrC_use_pseudo(C, insn, value, dmrC_add_pseudo(C, &insn->arguments, value));
		} END_FOR_EACH_PTR(sym);
	}

	insn->target = pseudo = linearize_compound_statement(C, ep, stmt);
	dmrC_use_pseudo(C, insn, symbol_pseudo(C, ep, stmt->inline_fn), &insn->func);
	bb = ep->active;
	if (bb && !bb->insns)
		bb->pos = stmt->pos;
	add_one_insn(C, ep, insn);
	return pseudo;
}

static pseudo_t linearize_context(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt)
{
	struct instruction *insn = alloc_instruction(C, OP_CONTEXT, 0);
	struct expression *expr = stmt->expression;
	int value = 0;

	if (expr->type == EXPR_VALUE)
		value = (int) expr->value;

	insn->increment = value;
	insn->context_expr = stmt->context;
	add_one_insn(C, ep, insn);
	return VOID_PSEUDO(C);
}

static pseudo_t linearize_range(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt)
{
	struct instruction *insn = alloc_instruction(C, OP_RANGE, 0);

	dmrC_use_pseudo(C, insn, linearize_expression(C, ep, stmt->range_expression), &insn->src1);
	dmrC_use_pseudo(C, insn, linearize_expression(C, ep, stmt->range_low), &insn->src2);
	dmrC_use_pseudo(C, insn, linearize_expression(C, ep, stmt->range_high), &insn->src3);
	add_one_insn(C, ep, insn);
	return VOID_PSEUDO(C);
}

static void add_asm_input(struct dmr_C *C, struct entrypoint *ep, struct instruction *insn, struct expression *expr,
	const char *constraint, const struct ident *ident)
{
	pseudo_t pseudo = linearize_expression(C, ep, expr);
	struct asm_constraint *rule = (struct asm_constraint *)dmrC_allocator_allocate(&C->L->asm_constraint_allocator, 0);

	rule->ident = ident;
	rule->constraint = constraint;
	dmrC_use_pseudo(C, insn, pseudo, &rule->pseudo);
	ptrlist_add((struct ptr_list **)&insn->asm_rules->inputs, rule, &C->ptrlist_allocator);
}

static void add_asm_output(struct dmr_C *C, struct entrypoint *ep, struct instruction *insn, struct expression *expr,
	const char *constraint, const struct ident *ident)
{
	struct access_data ad;
	pseudo_t pseudo = dmrC_alloc_pseudo(C, insn);
	struct asm_constraint *rule;

        memset(&ad, 0, sizeof ad);
	if (!expr || !linearize_address_gen(C, ep, expr, &ad))
		return;
	linearize_store_gen(C, ep, pseudo, &ad);
	finish_address_gen(ep, &ad);
	rule = (struct asm_constraint *) dmrC_allocator_allocate(&C->L->asm_constraint_allocator, 0);
	rule->ident = ident;
	rule->constraint = constraint;
	dmrC_use_pseudo(C, insn, pseudo, &rule->pseudo);
	ptrlist_add((struct ptr_list **)&insn->asm_rules->outputs, rule, &C->ptrlist_allocator);
}

static pseudo_t linearize_asm_statement(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt)
{
	int state;
	struct expression *expr;
	struct instruction *insn;
	struct asm_rules *rules;
	const char *constraint;
	struct ident *ident;

	insn = alloc_instruction(C, OP_ASM, 0);
	expr = stmt->asm_string;
	if (!expr || expr->type != EXPR_STRING) {
		dmrC_warning(C, stmt->pos, "expected string in inline asm");
		return VOID_PSEUDO(C);
	}
	insn->string = expr->string->data;

	rules = (struct asm_rules *) dmrC_allocator_allocate(&C->L->asm_rules_allocator, 0);
	insn->asm_rules = rules;

	/* Gather the inputs.. */
	state = 0;
	ident = NULL;
	constraint = NULL;
	FOR_EACH_PTR(stmt->asm_inputs, expr) {
		switch (state) {
		case 0:	/* Identifier */
			state = 1;
			ident = (struct ident *)expr;
			continue;

		case 1:	/* Constraint */
			state = 2;
			constraint = expr ? expr->string->data : "";
			continue;

		case 2:	/* Expression */
			state = 0;
			add_asm_input(C, ep, insn, expr, constraint, ident);
		}
	} END_FOR_EACH_PTR(expr);

	add_one_insn(C, ep, insn);

	/* Assign the outputs */
	state = 0;
	ident = NULL;
	constraint = NULL;
	FOR_EACH_PTR(stmt->asm_outputs, expr) {
		switch (state) {
		case 0:	/* Identifier */
			state = 1;
			ident = (struct ident *)expr;
			continue;

		case 1:	/* Constraint */
			state = 2;
			constraint = expr ? expr->string->data : "";
			continue;

		case 2:
			state = 0;
			add_asm_output(C, ep, insn, expr, constraint, ident);
		}
	} END_FOR_EACH_PTR(expr);

	return VOID_PSEUDO(C);
}

static int multijmp_cmp(void *ud, const void *_a, const void *_b)
{
        (void) ud;
	const struct multijmp *a = (const struct multijmp *)_a;
	const struct multijmp *b = (const struct multijmp *)_b;

	// "default" case?
	if (a->begin > a->end) {
		if (b->begin > b->end)
			return 0;
		return 1;
	}
	if (b->begin > b->end)
		return -1;
	if (a->begin == b->begin) {
		if (a->end == b->end)
			return 0;
		return (a->end < b->end) ? -1 : 1;
	}
	return a->begin < b->begin ? -1 : 1;
}

static void sort_switch_cases(struct dmr_C *C, struct instruction *insn)
{
	ptrlist_sort((struct ptr_list **)&insn->multijmp_list, C, multijmp_cmp);
}

static pseudo_t linearize_declaration(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt)
{
	struct symbol *sym;

	dmrC_concat_symbol_list(stmt->declaration, &ep->syms);

	FOR_EACH_PTR(stmt->declaration, sym) {
		linearize_one_symbol(C, ep, sym);
	} END_FOR_EACH_PTR(sym);
	return VOID_PSEUDO(C);
}

static pseudo_t linearize_return(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt)
{
	struct expression *expr = stmt->expression;
	struct basic_block *bb_return = get_bound_block(C, ep, stmt->ret_target);
	struct basic_block *active;
	pseudo_t src = linearize_expression(C, ep, expr);
	active = ep->active;
	if (active && src != VOID_PSEUDO(C)) {
		struct instruction *phi_node = dmrC_first_instruction(bb_return->insns);
		pseudo_t phi;
		if (!phi_node) {
			phi_node = alloc_typed_instruction(C, OP_PHI, expr->ctype);
			phi_node->target = dmrC_alloc_pseudo(C, phi_node);
			phi_node->bb = bb_return;
			dmrC_add_instruction(C, &bb_return->insns, phi_node);
		}
		phi = dmrC_alloc_phi(C, active, src, expr->ctype);
		phi->ident = C->S->return_ident;
		dmrC_use_pseudo(C, phi_node, phi, dmrC_add_pseudo(C, &phi_node->phi_list, phi));
	}
	add_goto(C, ep, bb_return);
	return VOID_PSEUDO(C);
}

static pseudo_t linearize_switch(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt)
{
	struct symbol *sym;
	struct instruction *switch_ins;
	struct basic_block *switch_end = alloc_basic_block(C, ep, stmt->pos);
	struct basic_block *active, *default_case;
	struct expression *expr = stmt->switch_expression;
	struct multijmp *jmp;
	pseudo_t pseudo;

	pseudo = linearize_expression(C, ep, expr);

	active = ep->active;
	if (!dmrC_bb_reachable(active))
		return VOID_PSEUDO(C);

	switch_ins = alloc_typed_instruction(C, OP_SWITCH, expr->ctype);
	dmrC_use_pseudo(C, switch_ins, pseudo, &switch_ins->cond);
	add_one_insn(C, ep, switch_ins);
	finish_block(ep);

	default_case = NULL;
	FOR_EACH_PTR(stmt->switch_case->symbol_list, sym) {
		struct statement *case_stmt = sym->stmt;
		struct basic_block *bb_case = get_bound_block(C, ep, sym);

		if (!case_stmt->case_expression) {
			default_case = bb_case;
			continue;
		} else {
			long long begin, end;

			begin = end = case_stmt->case_expression->value;
			if (case_stmt->case_to)
				end = case_stmt->case_to->value;
			if (begin > end)
				jmp = alloc_multijmp(C, bb_case, end, begin);
			else
				jmp = alloc_multijmp(C, bb_case, begin, end);

		}
		dmrC_add_multijmp(C, &switch_ins->multijmp_list, jmp);
		dmrC_add_bb(C, &bb_case->parents, active);
		dmrC_add_bb(C, &active->children, bb_case);
	} END_FOR_EACH_PTR(sym);

	bind_label(C, stmt->switch_break, switch_end, stmt->pos);

	/* And linearize the actual statement */
	linearize_statement(C, ep, stmt->switch_statement);
	set_activeblock(C, ep, switch_end);

	if (!default_case)
		default_case = switch_end;

	jmp = alloc_multijmp(C, default_case, 1, 0);
	dmrC_add_multijmp(C, &switch_ins->multijmp_list, jmp);
	dmrC_add_bb(C, &default_case->parents, active);
	dmrC_add_bb(C, &active->children, default_case);
	sort_switch_cases(C, switch_ins);

	return VOID_PSEUDO(C);
}

static pseudo_t linearize_iterator(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt)
{
	struct statement  *pre_statement = stmt->iterator_pre_statement;
	struct expression *pre_condition = stmt->iterator_pre_condition;
	struct statement  *statement = stmt->iterator_statement;
	struct statement  *post_statement = stmt->iterator_post_statement;
	struct expression *post_condition = stmt->iterator_post_condition;
	struct basic_block *loop_top, *loop_body, *loop_continue, *loop_end;
	struct symbol *sym;

	FOR_EACH_PTR(stmt->iterator_syms, sym) {
		linearize_one_symbol(C, ep, sym);
	} END_FOR_EACH_PTR(sym);

	dmrC_concat_symbol_list(stmt->iterator_syms, &ep->syms);
	linearize_statement(C, ep, pre_statement);

	loop_body = loop_top = alloc_basic_block(C, ep, stmt->pos);
	loop_continue = alloc_basic_block(C, ep, stmt->pos);
	loop_end = alloc_basic_block(C, ep, stmt->pos);

	/* An empty post-condition means that it's the same as the pre-condition */
	if (!post_condition) {
		loop_top = alloc_basic_block(C, ep, stmt->pos);
		set_activeblock(C, ep, loop_top);
	}

	if (pre_condition)
			linearize_cond_branch(C, ep, pre_condition, loop_body, loop_end);

	bind_label(C, stmt->iterator_continue, loop_continue, stmt->pos);
	bind_label(C, stmt->iterator_break, loop_end, stmt->pos);

	set_activeblock(C, ep, loop_body);
	linearize_statement(C, ep, statement);
	add_goto(C, ep, loop_continue);

	set_activeblock(C, ep, loop_continue);
	linearize_statement(C, ep, post_statement);
	if (!post_condition)
		add_goto(C, ep, loop_top);
	else
		linearize_cond_branch(C, ep, post_condition, loop_top, loop_end);
	set_activeblock(C, ep, loop_end);

	return VOID_PSEUDO(C);
}

static pseudo_t linearize_statement(struct dmr_C *C, struct entrypoint *ep, struct statement *stmt)
{
	struct basic_block *bb;

	if (!stmt)
		return VOID_PSEUDO(C);

	bb = ep->active;
	if (bb && !bb->insns)
		bb->pos = stmt->pos;
	C->L->current_pos = stmt->pos;

	switch (stmt->type) {
	case STMT_NONE:
		break;

	case STMT_DECLARATION:
		return linearize_declaration(C, ep, stmt);

	case STMT_CONTEXT:
		return linearize_context(C, ep, stmt);

	case STMT_RANGE:
		return linearize_range(C, ep, stmt);

	case STMT_EXPRESSION:
		return linearize_expression(C, ep, stmt->expression);

	case STMT_ASM:
		return linearize_asm_statement(C, ep, stmt);

	case STMT_RETURN:
		return linearize_return(C, ep, stmt);

	case STMT_CASE: {
		add_label(C, ep, stmt->case_label);
		linearize_statement(C, ep, stmt->case_statement);
		break;
	}

	case STMT_LABEL: {
		struct symbol *label = stmt->label_identifier;

		if (label->used) {
			bb = add_label(C, ep, label);
		}
		return 	linearize_statement(C, ep, stmt->label_statement);
	}

	case STMT_GOTO: {
		struct symbol *sym;
		struct expression *expr;
		struct instruction *goto_ins;
		struct basic_block *active;
		pseudo_t pseudo;

		active = ep->active;
		if (!dmrC_bb_reachable(active))
			break;

		if (stmt->goto_label) {
			add_goto(C, ep, get_bound_block(C, ep, stmt->goto_label));
			break;
		}

		expr = stmt->goto_expression;
		if (!expr)
			break;

		/* This can happen as part of simplification */
		if (expr->type == EXPR_LABEL) {
			add_goto(C, ep, get_bound_block(C, ep, expr->label_symbol));
			break;
		}

		pseudo = linearize_expression(C, ep, expr);
		goto_ins = alloc_instruction(C, OP_COMPUTEDGOTO, 0);
		dmrC_use_pseudo(C, goto_ins, pseudo, &goto_ins->target);
		add_one_insn(C, ep, goto_ins);

		FOR_EACH_PTR(stmt->target_list, sym) {
			struct basic_block *bb_computed = get_bound_block(C, ep, sym);
			struct multijmp *jmp = alloc_multijmp(C, bb_computed, 1, 0);
			dmrC_add_multijmp(C, &goto_ins->multijmp_list, jmp);
			dmrC_add_bb(C, &bb_computed->parents, ep->active);
			dmrC_add_bb(C, &active->children, bb_computed);
		} END_FOR_EACH_PTR(sym);

		finish_block(ep);
		break;
	}

	case STMT_COMPOUND:
		if (stmt->inline_fn)
			return linearize_inlined_call(C, ep, stmt);
		return linearize_compound_statement(C, ep, stmt);

	/*
	 * This could take 'likely/unlikely' into account, and
	 * switch the arms around appropriately..
	 */
	case STMT_IF: {
		struct basic_block *bb_true, *bb_false, *endif;
 		struct expression *cond = stmt->if_conditional;

		bb_true = alloc_basic_block(C, ep, stmt->pos);
		bb_false = endif = alloc_basic_block(C, ep, stmt->pos);

 		linearize_cond_branch(C, ep, cond, bb_true, bb_false);

		set_activeblock(C, ep, bb_true);
 		linearize_statement(C, ep, stmt->if_true);
 
 		if (stmt->if_false) {
			endif = alloc_basic_block(C, ep, stmt->pos);
			add_goto(C, ep, endif);
			set_activeblock(C, ep, bb_false);
 			linearize_statement(C, ep, stmt->if_false);
		}
		set_activeblock(C, ep, endif);
		break;
	}

	case STMT_SWITCH:
		return linearize_switch(C, ep, stmt);

	case STMT_ITERATOR:
		return linearize_iterator(C, ep, stmt);

	default:
		break;
	}
	return VOID_PSEUDO(C);
}

static struct entrypoint *linearize_fn(struct dmr_C *C, struct symbol *sym, struct symbol *base_type)
{
	struct entrypoint *ep;
	struct basic_block *bb;
	struct symbol *arg;
	struct instruction *entry;
	pseudo_t result;
	int i;

	if (!base_type->stmt)
		return NULL;

	ep = alloc_entrypoint(C);
	bb = alloc_basic_block(C, ep, sym->pos);
	
	ep->name = sym;
	sym->ep = ep;
	set_activeblock(C, ep, bb);

	entry = alloc_instruction(C, OP_ENTRY, 0);
	add_one_insn(C, ep, entry);
	ep->entry = entry;

	dmrC_concat_symbol_list(base_type->arguments, &ep->syms);

	/* FIXME!! We should do something else about varargs.. */
	i = 0;
	FOR_EACH_PTR(base_type->arguments, arg) {
		linearize_argument(C, ep, arg, ++i);
	} END_FOR_EACH_PTR(arg);

	result = linearize_statement(C, ep, base_type->stmt);
	if (dmrC_bb_reachable(ep->active) && !dmrC_bb_terminated(ep->active)) {
		struct symbol *ret_type = base_type->ctype.base_type;
		struct instruction *insn = alloc_typed_instruction(C, OP_RET, ret_type);

		if (type_size(ret_type) > 0)
			dmrC_use_pseudo(C, insn, result, &insn->src);
		add_one_insn(C, ep, insn);
	}

	int show_details = C->verbose > 2;

	if (show_details) {
		printf("%s(%d): pre dmrC_kill_unreachable_bbs()\n", __FILE__, __LINE__);
		dmrC_show_entry(C, ep);
	}
	if (C->fdump_linearize) {
		if (C->fdump_linearize == 2)
			return ep;
		dmrC_show_entry(C, ep);
	}

	
	/*
	 * Do trivial flow simplification - branches to
	 * branches, kill dead basicblocks etc
	 */
	dmrC_kill_unreachable_bbs(C, ep);

#if 0
	if (C->optimize) {

		if (show_details) {
			printf("%s(%d): pre dmrC_simplify_symbol_usage()\n", __FILE__, __LINE__);
			dmrC_show_entry(C, ep);
		}

		/*
		 * Turn symbols into pseudos
		 */
		dmrC_simplify_symbol_usage(C, ep);
	repeat:
		/*
		 * Remove trivial instructions, and try to CSE
		 * the rest.
		 */
		do {
			if (show_details) {
				printf("%s(%d): pre dmrC_cleanup_and_cse()\n", __FILE__, __LINE__);
				dmrC_show_entry(C, ep);
			}
			dmrC_cleanup_and_cse(C, ep);
			if (show_details) {
				printf("%s(%d): pre dmrC_pack_basic_blocks()\n", __FILE__, __LINE__);
				dmrC_show_entry(C, ep);
			}
			dmrC_pack_basic_blocks(C, ep);
		} while (C->L->repeat_phase & REPEAT_CSE);

		if (show_details) {
			printf("%s(%d): pre dmrC_kill_unreachable_bbs() and dmrC_vrfy_flow()\n", __FILE__, __LINE__);
			dmrC_show_entry(C, ep);
		}
		dmrC_kill_unreachable_bbs(C, ep);
		dmrC_vrfy_flow(ep);

		if (show_details) {
			printf("%s(%d): pre clear_symbol_pseudos()\n", __FILE__, __LINE__);
			dmrC_show_entry(C, ep);
		}

		/* Cleanup */
		clear_symbol_pseudos(ep);

		if (show_details) {
			printf("%s(%d): pre dmrC_track_pseudo_liveness()\n", __FILE__, __LINE__);
			dmrC_show_entry(C, ep);
		}

		/* And track pseudo register usage */
		dmrC_track_pseudo_liveness(C, ep);

		if (show_details) {
			printf("%s(%d): pre dmrC_simplify_flow()\n", __FILE__, __LINE__);
			dmrC_show_entry(C, ep);
		}
		/*
		 * Some flow optimizations can only effectively
		 * be done when we've done liveness analysis. But
		 * if they trigger, we need to start all over
		 * again
		 */
		if (dmrC_simplify_flow(C, ep)) {

			if (show_details) {
				printf("%s(%d): pre dmrC_clear_liveness()\n", __FILE__, __LINE__);
				dmrC_show_entry(C, ep);
			}

			dmrC_clear_liveness(ep);
			goto repeat;
		}
	}
#endif

	/* Finally, add deathnotes to pseudos now that we have them */
	if (C->dbg_dead)
		/* Note that this sets phi_users list on phisrc instructions which are relied upon by the 
		   LLVM backend */
		dmrC_track_pseudo_death(C, ep);

	return ep;
}

struct entrypoint *dmrC_linearize_symbol(struct dmr_C *C, struct symbol *sym)
{
	struct symbol *base_type;

	if (!sym)
		return NULL;
	C->L->current_pos = sym->pos;
	base_type = sym->ctype.base_type;
	if (!base_type)
		return NULL;
	if (base_type->type == SYM_FN)
		return linearize_fn(C, sym, base_type);
	return NULL;
}

static void mark_bb_reachable(struct basic_block *bb, unsigned long generation)
{
	struct basic_block *child;

	if (bb->generation == generation)
		return;
	bb->generation = generation;
	FOR_EACH_PTR(bb->children, child) {
		mark_bb_reachable(child, generation);
	} END_FOR_EACH_PTR(child);
}

void dmrC_kill_unreachable_bbs(struct dmr_C *C, struct entrypoint *ep)
{
	struct basic_block *bb;
	unsigned long generation = ++C->L->bb_generation;

	mark_bb_reachable(ep->entry->bb, generation);
	FOR_EACH_PTR(ep->bbs, bb) {
		if (bb->generation == generation)
			continue;
		/* Mark it as being dead */
		dmrC_kill_bb(C, bb);
		bb->ep = NULL;
		DELETE_CURRENT_PTR(bb);
	} END_FOR_EACH_PTR(bb);
	ptrlist_pack((struct ptr_list **) &ep->bbs);
}

static int delete_pseudo_user_list_entry(struct dmr_C *C, struct pseudo_user_list **list, pseudo_t *entry, int count)
{
        (void)C;
	struct pseudo_user *pu;

	FOR_EACH_PTR(*list, pu) {
		if (pu->userp == entry) {
			MARK_CURRENT_DELETED(struct pseudo_user *, pu);
			if (!--count)
				goto out;
		}
	} END_FOR_EACH_PTR(pu);
	assert(count <= 0);
out:
	if (ptrlist_size((struct ptr_list *)*list) == 0)
		*list = NULL;
	return count;
}

static inline void remove_usage(struct dmr_C *C, pseudo_t p, pseudo_t *usep)
{
	if (dmrC_has_use_list(p)) {
		delete_pseudo_user_list_entry(C, &p->users, usep, 1);
		if (!p->users)
			dmrC_kill_instruction(C, p->def);
	}
}

static inline void concat_user_list(struct pseudo_user_list *src, struct pseudo_user_list **dst)
{
	ptrlist_concat((struct ptr_list *)src, (struct ptr_list **)dst);
}

void dmrC_convert_instruction_target(struct dmr_C *C, struct instruction *insn, pseudo_t src)
{
	pseudo_t target;
	struct pseudo_user *pu;
	/*
	 * Go through the "insn->users" list and replace them all..
	 */
	target = insn->target;
	if (target == src)
		return;
	FOR_EACH_PTR(target->users, pu) {
		if (*pu->userp != VOID_PSEUDO(C)) {
			assert(*pu->userp == target);
			*pu->userp = src;
		}
	} END_FOR_EACH_PTR(pu);
	if (dmrC_has_use_list(src))
		concat_user_list(target->users, &src->users);
	target->users = NULL;
}

static void kill_defs(struct dmr_C *C, struct instruction *insn)
{
	pseudo_t target = insn->target;

	if (!dmrC_has_use_list(target))
		return;
	if (target->def != insn)
		return;

	dmrC_convert_instruction_target(C, insn, VOID_PSEUDO(C));
}

void dmrC_kill_bb(struct dmr_C *C, struct basic_block *bb)
{
	struct instruction *insn;
	struct basic_block *child, *parent;

	FOR_EACH_PTR(bb->insns, insn) {
		dmrC_kill_instruction_force(C, insn);
		kill_defs(C, insn);
		/*
		 * We kill unreachable instructions even if they
		 * otherwise aren't "killable" (e.g. volatile loads)
		 */
	} END_FOR_EACH_PTR(insn);
	bb->insns = NULL;

	FOR_EACH_PTR(bb->children, child) {
		dmrC_remove_bb_from_list(&child->parents, bb, 0);
	} END_FOR_EACH_PTR(child);
	bb->children = NULL;

	FOR_EACH_PTR(bb->parents, parent) {
		dmrC_remove_bb_from_list(&parent->children, bb, 0);
	} END_FOR_EACH_PTR(parent);
	bb->parents = NULL;
}


void dmrC_kill_use(struct dmr_C *C, pseudo_t *usep)
{
	if (usep) {
		pseudo_t p = *usep;
		*usep = VOID_PSEUDO(C);
		remove_usage(C, p, usep);
	}
}

static void kill_use_list(struct dmr_C *C, struct pseudo_list *list)
{
	pseudo_t p;
	FOR_EACH_PTR(list, p) {
		if (p == VOID_PSEUDO(C))
			continue;
		dmrC_kill_use(C, THIS_ADDRESS(pseudo_t, p));
	} END_FOR_EACH_PTR(p);
}

/*
 * kill an instruction:
 * - remove it from its bb
 * - remove the usage of all its operands
 * If forse is zero, the normal case, the function only for
 * instructions free of (possible) side-effects. Otherwise
 * the function does that unconditionally (must only be used
 * for unreachable instructions.
 */
void dmrC_kill_insn(struct dmr_C *C, struct instruction *insn, int force)
{
	if (!insn || !insn->bb)
		return;

	switch (insn->opcode) {
	case OP_SEL:
	case OP_RANGE:
		dmrC_kill_use(C, &insn->src3);
		/* fall through */

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

		/* Logical */
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
		dmrC_kill_use(C, &insn->src2);
		/* fall through */

	case OP_CAST:
	case OP_SCAST:
	case OP_FPCAST:
	case OP_PTRCAST:
	case OP_SETVAL:
	case OP_NOT: case OP_NEG:
	case OP_SLICE:
		dmrC_kill_use(C, &insn->src1);
		break;

	case OP_PHI:
		kill_use_list(C, insn->phi_list);
		break;
	case OP_PHISOURCE:
		dmrC_kill_use(C, &insn->phi_src);
		break;

	case OP_SYMADDR:
		C->L->repeat_phase |= REPEAT_SYMBOL_CLEANUP;
		break;

	case OP_CBR:
		/* fall through */
	case OP_COMPUTEDGOTO:
		dmrC_kill_use(C, &insn->cond);
		break;

	case OP_CALL:
		if (!force) {
			/* a "pure" function can be killed too */
			if (!(insn->func->type == PSEUDO_SYM))
				return;
			if (!(insn->func->sym->ctype.modifiers & MOD_PURE))
				return;
		}
		kill_use_list(C, insn->arguments);
		if (insn->func->type == PSEUDO_REG)
			dmrC_kill_use(C, &insn->func);
		break;

	case OP_LOAD:
		if (!force && insn->type->ctype.modifiers & MOD_VOLATILE)
			return;
		dmrC_kill_use(C, &insn->src);
		break;

	case OP_STORE:
		if (!force)
			return;
		dmrC_kill_use(C, &insn->src);
		dmrC_kill_use(C, &insn->target);
		break;

	case OP_ENTRY:
		/* ignore */
		return;

	case OP_BR:
	default:
		break;
	}

	insn->bb = NULL;
	C->L->repeat_phase |= REPEAT_CSE;
	return;
}

void dmrC_init_linearizer(struct dmr_C *C) {
	struct linearizer_state_t *L = (struct linearizer_state_t *)calloc(1, sizeof(struct linearizer_state_t));
	dmrC_allocator_init(&L->asm_rules_allocator, "asm rules", sizeof(struct asm_rules),
		__alignof__(struct asm_rules), CHUNK);
	dmrC_allocator_init(&L->pseudo_allocator, "pseudos", sizeof(struct pseudo),
		__alignof__(struct pseudo), CHUNK);
	dmrC_allocator_init(&L->pseudo_user_allocator, "pseudo_users", sizeof(struct pseudo_user),
		__alignof__(struct pseudo_user), CHUNK);
	dmrC_allocator_init(&L->asm_constraint_allocator, "asm_constraints", sizeof(struct asm_constraint),
		__alignof__(struct asm_constraint), CHUNK);
	dmrC_allocator_init(&L->multijmp_allocator, "multijmps", sizeof(struct multijmp),
		__alignof__(struct multijmp), CHUNK);
	dmrC_allocator_init(&L->basic_block_allocator, "basic_blocks", sizeof(struct basic_block),
		__alignof__(struct basic_block), CHUNK);
	dmrC_allocator_init(&L->entrypoint_allocator, "entrypoints", sizeof(struct entrypoint),
		__alignof__(struct entrypoint), CHUNK);
	dmrC_allocator_init(&L->instruction_allocator, "instructions", sizeof(struct instruction),
		__alignof__(struct instruction), CHUNK);
	C->L = L;
}

void dmrC_destroy_linearizer(struct dmr_C *C) {
	struct linearizer_state_t *L = C->L;
	assert(L);
	dmrC_allocator_destroy(&L->asm_rules_allocator);
	dmrC_allocator_destroy(&L->pseudo_allocator);
	dmrC_allocator_destroy(&L->pseudo_user_allocator);
	dmrC_allocator_destroy(&L->asm_constraint_allocator);
	dmrC_allocator_destroy(&L->multijmp_allocator);
	dmrC_allocator_destroy(&L->basic_block_allocator);
	dmrC_allocator_destroy(&L->entrypoint_allocator);
	dmrC_allocator_destroy(&L->instruction_allocator);
	free(L);
	C->L = NULL;
}
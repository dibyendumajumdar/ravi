/*
 * sparse/expand.c
 *
 * Copyright (C) 2003 Transmeta Corp.
 *               2003-2004 Linus Torvalds
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * expand constant expressions.
 */
 /*
 * This version is part of the dmr_c project.
 * Copyright (C) 2017 Dibyendu Majumdar
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>

#include <port.h>
#include <lib.h>
#include <allocate.h>
#include <parse.h>
#include <token.h>
#include <symbol.h>
#include <target.h>
#include <expression.h>
#include <expand.h>

static int expand_expression(struct dmr_C *C, struct expression *);
static int expand_statement(struct dmr_C *C, struct statement *);
static int conservative;

static int expand_symbol_expression(struct dmr_C *C, struct expression *expr)
{
	struct symbol *sym = expr->symbol;

	if (sym == &C->S->zero_int) {
		if (C->Wundef)
			dmrC_warning(C, expr->pos, "undefined preprocessor identifier '%s'", dmrC_show_ident(C, expr->symbol_name));
		expr->type = EXPR_VALUE;
		expr->value = 0;
		expr->taint = 0;
		return 0;
	}
	/* The cost of a symbol expression is lower for on-stack symbols */
	return (sym->ctype.modifiers & (MOD_STATIC | MOD_EXTERN)) ? 2 : 1;
}

static long long get_longlong(struct dmr_C *C, struct expression *expr)
{
	int no_expand = expr->ctype->ctype.modifiers & MOD_UNSIGNED;
	long long mask = 1ULL << (expr->ctype->bit_size - 1);
	long long value = expr->value;
	long long ormask, andmask;
	(void)C;

	if (!(value & mask))
		no_expand = 1;
	andmask = mask | (mask-1);
	ormask = ~andmask;
	if (no_expand)
		ormask = 0;
	return (value & andmask) | ormask;
}

void dmrC_cast_value(struct dmr_C *C, struct expression *expr, struct symbol *newtype,
		struct expression *old, struct symbol *oldtype)
{
	int old_size = oldtype->bit_size;
	int new_size = newtype->bit_size;
	long long value, mask, signmask;
	long long oldmask, oldsignmask, dropped;

	if (dmrC_is_float_type(C->S, newtype) ||
	    dmrC_is_float_type(C->S, oldtype))
		goto Float;

	// For pointers and integers, we can just move the value around
	expr->type = EXPR_VALUE;
	expr->taint = old->taint;
	if (old_size == new_size) {
		expr->value = old->value;
		return;
	}

	// expand it to the full "long long" value
	value = get_longlong(C, old);

Int:
	// _Bool requires a zero test rather than truncation.
	if (dmrC_is_bool_type(C->S, newtype)) {
		expr->value = !!value;
		if (!conservative && value != 0 && value != 1)
			dmrC_warning(C, old->pos, "odd constant _Bool cast (%llx becomes 1)", value);
		return;
	}

	// Truncate it to the new size
	signmask = 1ULL << (new_size-1);
	mask = signmask | (signmask-1);
	expr->value = value & mask;

	// Stop here unless checking for truncation
	if (!C->Wcast_truncate || conservative)
		return;
	
	// Check if we dropped any bits..
	oldsignmask = 1ULL << (old_size-1);
	oldmask = oldsignmask | (oldsignmask-1);
	dropped = oldmask & ~mask;

	// OK if the bits were (and still are) purely sign bits
	if (value & dropped) {
		if (!(value & oldsignmask) || !(value & signmask) || (value & dropped) != dropped)
			dmrC_warning(C, old->pos, "cast truncates bits from constant value (%llx becomes %llx)",
				value & oldmask,
				value & mask);
	}
	return;

Float:
	if (!dmrC_is_float_type(C->S, newtype)) {
		value = (long long)old->fvalue;
		expr->type = EXPR_VALUE;
		expr->taint = 0;
		goto Int;
	}

	if (!dmrC_is_float_type(C->S, oldtype))
		expr->fvalue = (long double)get_longlong(C, old);
	else
		expr->fvalue = old->fvalue;

	if (!(newtype->ctype.modifiers & MOD_LONGLONG) && \
	    !(newtype->ctype.modifiers & MOD_LONGLONGLONG)) {
		if ((newtype->ctype.modifiers & MOD_LONG))
			expr->fvalue = (double)expr->fvalue;
		else
			expr->fvalue = (float)expr->fvalue;
	}
	expr->type = EXPR_FVALUE;
}

static int check_shift_count(struct dmr_C *C, struct expression *expr, struct symbol *ctype, unsigned int count)
{
	dmrC_warning(C, expr->pos, "shift too big (%u) for type %s", count, dmrC_show_typename(C, ctype));
	count &= ctype->bit_size-1;
	return count;
}

/*
 * CAREFUL! We need to get the size and sign of the
 * result right!
 */
#define CONVERT(op,s)	(((op)<<1)+(s))
#define SIGNED(op)	CONVERT(op, 1)
#define UNSIGNED(op)	CONVERT(op, 0)
static int simplify_int_binop(struct dmr_C *C, struct expression *expr, struct symbol *ctype)
{
	struct expression *left = expr->left, *right = expr->right;
	unsigned long long v, l, r, mask;
	signed long long sl, sr;
	int is_signed;

	if (right->type != EXPR_VALUE)
		return 0;
	r = right->value;
	if (expr->op == SPECIAL_LEFTSHIFT || expr->op == SPECIAL_RIGHTSHIFT) {
		if ((int)r >= ctype->bit_size) {
			if (conservative)
				return 0;
			r = check_shift_count(C, expr, ctype, (unsigned int)r);
			right->value = r;
		}
	}
	if (left->type != EXPR_VALUE)
		return 0;
	l = left->value; r = right->value;
	is_signed = !(ctype->ctype.modifiers & MOD_UNSIGNED);
	mask = 1ULL << (ctype->bit_size-1);
	sl = l; sr = r;
	if (is_signed && (sl & mask))
		sl |= ~(mask-1);
	if (is_signed && (sr & mask))
		sr |= ~(mask-1);
	
	switch (CONVERT(expr->op,is_signed)) {
	case SIGNED('+'):
	case UNSIGNED('+'):
		v = l + r;
		break;

	case SIGNED('-'):
	case UNSIGNED('-'):
		v = l - r;
		break;

	case SIGNED('&'):
	case UNSIGNED('&'):
		v = l & r;
		break;

	case SIGNED('|'):
	case UNSIGNED('|'):
		v = l | r;
		break;

	case SIGNED('^'):
	case UNSIGNED('^'):
		v = l ^ r;
		break;

	case SIGNED('*'):
		v = sl * sr;
		break;

	case UNSIGNED('*'):
		v = l * r;
		break;

	case SIGNED('/'):
		if (!r)
			goto Div;
		if (l == mask && sr == -1)
			goto Overflow;
		v = sl / sr;
		break;

	case UNSIGNED('/'):
		if (!r) goto Div;
		v = l / r; 
		break;

	case SIGNED('%'):
		if (!r)
			goto Div;
		if (l == mask && sr == -1)
			goto Overflow;
		v = sl % sr;
		break;

	case UNSIGNED('%'):
		if (!r) goto Div;
		v = l % r;
		break;

	case SIGNED(SPECIAL_LEFTSHIFT):
	case UNSIGNED(SPECIAL_LEFTSHIFT):
		v = l << r;
		break; 

	case SIGNED(SPECIAL_RIGHTSHIFT):
		v = sl >> r;
		break;

	case UNSIGNED(SPECIAL_RIGHTSHIFT):
		v = l >> r;
		break;

	default:
		return 0;
	}
	mask = mask | (mask-1);
	expr->value = v & mask;
	expr->type = EXPR_VALUE;
	expr->taint = left->taint | right->taint;
	return 1;
Div:
	if (!conservative)
		dmrC_warning(C, expr->pos, "division by zero");
	return 0;
Overflow:
	if (!conservative)
		dmrC_warning(C, expr->pos, "constant integer operation overflow");
	return 0;
}

static int simplify_cmp_binop(struct dmr_C *C, struct expression *expr, struct symbol *ctype)
{
	struct expression *left = expr->left, *right = expr->right;
	unsigned long long l, r, mask;
	signed long long sl, sr;

	(void) C;
	if (left->type != EXPR_VALUE || right->type != EXPR_VALUE)
		return 0;
	l = left->value; r = right->value;
	mask = 1ULL << (ctype->bit_size-1);
	sl = l; sr = r;
	if (sl & mask)
		sl |= ~(mask-1);
	if (sr & mask)
		sr |= ~(mask-1);
	switch (expr->op) {
	case '<':		expr->value = sl < sr; break;
	case '>':		expr->value = sl > sr; break;
	case SPECIAL_LTE:	expr->value = sl <= sr; break;
	case SPECIAL_GTE:	expr->value = sl >= sr; break;
	case SPECIAL_EQUAL:	expr->value = l == r; break;
	case SPECIAL_NOTEQUAL:	expr->value = l != r; break;
	case SPECIAL_UNSIGNED_LT:expr->value = l < r; break;
	case SPECIAL_UNSIGNED_GT:expr->value = l > r; break;
	case SPECIAL_UNSIGNED_LTE:expr->value = l <= r; break;
	case SPECIAL_UNSIGNED_GTE:expr->value = l >= r; break;
	}
	expr->type = EXPR_VALUE;
	expr->taint = left->taint | right->taint;
	return 1;
}

static int simplify_float_binop(struct dmr_C *C, struct expression *expr)
{
	struct expression *left = expr->left, *right = expr->right;
	unsigned long mod = expr->ctype->ctype.modifiers;
	long double l, r, res;

	if (left->type != EXPR_FVALUE || right->type != EXPR_FVALUE)
		return 0;

	l = left->fvalue;
	r = right->fvalue;

	if (mod & MOD_LONGLONG) {
		switch (expr->op) {
		case '+':	res = l + r; break;
		case '-':	res = l - r; break;
		case '*':	res = l * r; break;
		case '/':	if (!r) goto Div;
				res = l / r; break;
		default: return 0;
		}
	} else if (mod & MOD_LONG) {
		switch (expr->op) {
		case '+':	res = (double) l + (double) r; break;
		case '-':	res = (double) l - (double) r; break;
		case '*':	res = (double) l * (double) r; break;
		case '/':	if (!r) goto Div;
				res = (double) l / (double) r; break;
		default: return 0;
		}
	} else {
		switch (expr->op) {
		case '+':	res = (float)l + (float)r; break;
		case '-':	res = (float)l - (float)r; break;
		case '*':	res = (float)l * (float)r; break;
		case '/':	if (!r) goto Div;
				res = (float)l / (float)r; break;
		default: return 0;
		}
	}
	expr->type = EXPR_FVALUE;
	expr->fvalue = res;
	return 1;
Div:
	if (!conservative)
		dmrC_warning(C, expr->pos, "division by zero");
	return 0;
}

static int simplify_float_cmp(struct dmr_C *C, struct expression *expr, struct symbol *ctype)
{
        (void) C;
        (void)ctype;
	struct expression *left = expr->left, *right = expr->right;
	long double l, r;

	if (left->type != EXPR_FVALUE || right->type != EXPR_FVALUE)
		return 0;

	l = left->fvalue;
	r = right->fvalue;
	switch (expr->op) {
	case '<':		expr->value = l < r; break;
	case '>':		expr->value = l > r; break;
	case SPECIAL_LTE:	expr->value = l <= r; break;
	case SPECIAL_GTE:	expr->value = l >= r; break;
	case SPECIAL_EQUAL:	expr->value = l == r; break;
	case SPECIAL_NOTEQUAL:	expr->value = l != r; break;
	}
	expr->type = EXPR_VALUE;
	expr->taint = 0;
	return 1;
}

static int expand_binop(struct dmr_C *C, struct expression *expr)
{
	int cost;

	cost = expand_expression(C, expr->left);
	cost += expand_expression(C, expr->right);
	if (simplify_int_binop(C, expr, expr->ctype))
		return 0;
	if (simplify_float_binop(C, expr))
		return 0;
	return cost + 1;
}

static int expand_logical(struct dmr_C *C, struct expression *expr)
{
	struct expression *left = expr->left;
	struct expression *right;
	int cost, rcost;

	/* Do immediate short-circuiting ... */
	cost = expand_expression(C, left);
	if (left->type == EXPR_VALUE) {
		if (expr->op == SPECIAL_LOGICAL_AND) {
			if (!left->value) {
				expr->type = EXPR_VALUE;
				expr->value = 0;
				expr->taint = left->taint;
				return 0;
			}
		} else {
			if (left->value) {
				expr->type = EXPR_VALUE;
				expr->value = 1;
				expr->taint = left->taint;
				return 0;
			}
		}
	}

	right = expr->right;
	rcost = expand_expression(C, right);
	if (left->type == EXPR_VALUE && right->type == EXPR_VALUE) {
		/*
		 * We know the left value doesn't matter, since
		 * otherwise we would have short-circuited it..
		 */
		expr->type = EXPR_VALUE;
		expr->value = right->value != 0;
		expr->taint = left->taint | right->taint;
		return 0;
	}

	/*
	 * If the right side is safe and cheaper than a branch,
	 * just avoid the branch and turn it into a regular binop
	 * style SAFELOGICAL.
	 */
	if (rcost < BRANCH_COST) {
		expr->type = EXPR_BINOP;
		rcost -= BRANCH_COST - 1;
	}

	return cost + BRANCH_COST + rcost;
}

static int expand_comma(struct dmr_C *C, struct expression *expr)
{
	int cost;

	cost = expand_expression(C, expr->left);
	cost += expand_expression(C, expr->right);
	if (expr->left->type == EXPR_VALUE || expr->left->type == EXPR_FVALUE) {
		unsigned flags = expr->flags;
		unsigned taint;
		taint = expr->left->type == EXPR_VALUE ? expr->left->taint : 0;
		*expr = *expr->right;
		expr->flags = flags;
		if (expr->type == EXPR_VALUE)
			expr->taint |= Taint_comma | taint;
	}
	return cost;
}

#define MOD_IGN (MOD_VOLATILE | MOD_CONST)

static int compare_types(struct dmr_C *C, int op, struct symbol *left, struct symbol *right)
{
	struct ctype c1 = {.base_type = left};
	struct ctype c2 = {.base_type = right};
	switch (op) {
	case SPECIAL_EQUAL:
		return !dmrC_type_difference(C, &c1, &c2, MOD_IGN, MOD_IGN);
	case SPECIAL_NOTEQUAL:
		return dmrC_type_difference(C, &c1, &c2, MOD_IGN, MOD_IGN) != NULL;
	case '<':
		return left->bit_size < right->bit_size;
	case '>':
		return left->bit_size > right->bit_size;
	case SPECIAL_LTE:
		return left->bit_size <= right->bit_size;
	case SPECIAL_GTE:
		return left->bit_size >= right->bit_size;
	}
	return 0;
}

static int expand_compare(struct dmr_C *C, struct expression *expr)
{
	struct expression *left = expr->left, *right = expr->right;
	int cost;

	cost = expand_expression(C, left);
	cost += expand_expression(C, right);

	if (left && right) {
		/* Type comparison? */
		if (left->type == EXPR_TYPE && right->type == EXPR_TYPE) {
			int op = expr->op;
			expr->type = EXPR_VALUE;
			expr->value = compare_types(C, op, left->symbol, right->symbol);
			expr->taint = 0;
			return 0;
		}
		if (simplify_cmp_binop(C, expr, left->ctype))
			return 0;
		if (simplify_float_cmp(C, expr, left->ctype))
			return 0;
	}
	return cost + 1;
}

static int expand_conditional(struct dmr_C *C, struct expression *expr)
{
	struct expression *cond = expr->conditional;
	struct expression *truee = expr->cond_true;
	struct expression *falsee = expr->cond_false;
	int cost, cond_cost;

	cond_cost = expand_expression(C, cond);
	if (cond->type == EXPR_VALUE) {
		unsigned flags = expr->flags;
		if (!cond->value)
			truee = falsee;
		if (!truee)
			truee = cond;
		cost = expand_expression(C, truee);
		*expr = *truee;
		expr->flags = flags;
		if (expr->type == EXPR_VALUE)
			expr->taint |= cond->taint;
		return cost;
	}

	cost = expand_expression(C, truee);
	cost += expand_expression(C, falsee);

	if (cost < SELECT_COST) {
		expr->type = EXPR_SELECT;
		cost -= BRANCH_COST - 1;
	}

	return cost + cond_cost + BRANCH_COST;
}
		
static int expand_assignment(struct dmr_C *C, struct expression *expr)
{
	expand_expression(C, expr->left);
	expand_expression(C, expr->right);
	return SIDE_EFFECTS;
}

static int expand_addressof(struct dmr_C *C, struct expression *expr)
{
	return expand_expression(C, expr->unop);
}

/*
 * Look up a trustable initializer value at the requested offset.
 *
 * Return NULL if no such value can be found or statically trusted.
 *
 * FIXME!! We should check that the size is right!
 */
static struct expression *constant_symbol_value(struct dmr_C *C, struct symbol *sym, int offset)
{
	struct expression *value;
        (void)C;

	if (sym->ctype.modifiers & (MOD_ASSIGNED | MOD_ADDRESSABLE))
		return NULL;
	value = sym->initializer;
	if (!value)
		return NULL;
	if (value->type == EXPR_INITIALIZER) {
		struct expression *entry;
		FOR_EACH_PTR(value->expr_list, entry) {
			if (entry->type != EXPR_POS) {
				if (offset)
					continue;
				return entry;
			}
			if ((int) entry->init_offset < offset)
				continue;
			if ((int) entry->init_offset > offset)
				return NULL;
			return entry->init_expr;
		} END_FOR_EACH_PTR(entry);
		return NULL;
	}
	return value;
}

static int expand_dereference(struct dmr_C *C, struct expression *expr)
{
	struct expression *unop = expr->unop;
	unsigned int offset;

	expand_expression(C, unop);

	/*
	 * NOTE! We get a bogus warning right now for some special
	 * cases: apparently I've screwed up the optimization of
	 * a zero-offset dereference, and the ctype is wrong.
	 *
	 * Leave the warning in anyway, since this is also a good
	 * test for me to get the type evaluation right..
	 */
	if (expr->ctype->ctype.modifiers & MOD_NODEREF)
		dmrC_warning(C, unop->pos, "dereference of noderef expression");

	/*
	 * Is it "symbol" or "symbol + offset"?
	 */
	offset = 0;
	if (unop->type == EXPR_BINOP && unop->op == '+') {
		struct expression *right = unop->right;
		if (right->type == EXPR_VALUE) {
			offset = (unsigned int) right->value;
			unop = unop->left;
		}
	}

	if (unop->type == EXPR_SYMBOL) {
		struct symbol *sym = unop->symbol;
		struct expression *value = constant_symbol_value(C, sym, offset);

		/* Const symbol with a constant initializer? */
		if (value) {
			/* FIXME! We should check that the size is right! */
			if (value->type == EXPR_VALUE) {
				/*
				During the expansion of a dereference, it's if the initializer
				which corrrespond to the offset we're interested is a constant.
				In which case this dereference can be avoided and the value
				given in the initializer can be used instead.

				However, it's not enough to check for the offset since for bitfields
				several are placed at the same offset.
				Hence refuse such expansion if the constant value correspond
				to a bitfield.
				*/
				if (dmrC_is_bitfield_type(value->ctype))
					return UNSAFE;
				expr->type = EXPR_VALUE;
				expr->value = value->value;
				expr->taint = 0;
				return 0;
			} else if (value->type == EXPR_FVALUE) {
				expr->type = EXPR_FVALUE;
				expr->fvalue = value->fvalue;
				return 0;
			}
		}

		/* Direct symbol dereference? Cheap and safe */
		return (sym->ctype.modifiers & (MOD_STATIC | MOD_EXTERN)) ? 2 : 1;
	}

	return UNSAFE;
}

static int simplify_preop(struct dmr_C *C, struct expression *expr)
{
	struct expression *op = expr->unop;
	unsigned long long v, mask;

	if (op->type != EXPR_VALUE)
		return 0;

	mask = 1ULL << (expr->ctype->bit_size-1);
	v = op->value;
	switch (expr->op) {
	case '+': break;
	case '-':
		if (v == mask && !(expr->ctype->ctype.modifiers & MOD_UNSIGNED))
			goto Overflow;
		v = -v;
		break;
	case '!': v = !v; break;
	case '~': v = ~v; break;
	default: return 0;
	}
	mask = mask | (mask-1);
	expr->value = v & mask;
	expr->type = EXPR_VALUE;
	expr->taint = op->taint;
	return 1;

Overflow:
	if (!conservative)
		dmrC_warning(C, expr->pos, "constant integer operation overflow");
	return 0;
}

static int simplify_float_preop(struct dmr_C *C, struct expression *expr)
{
        (void) C;
	struct expression *op = expr->unop;
	long double v;

	if (op->type != EXPR_FVALUE)
		return 0;
	v = op->fvalue;
	switch (expr->op) {
	case '+': break;
	case '-': v = -v; break;
	default: return 0;
	}
	expr->fvalue = v;
	expr->type = EXPR_FVALUE;
	return 1;
}

/*
 * Unary post-ops: x++ and x--
 */
static int expand_postop(struct dmr_C *C, struct expression *expr)
{
	expand_expression(C, expr->unop);
	return SIDE_EFFECTS;
}

static int expand_preop(struct dmr_C *C, struct expression *expr)
{
	int cost;

	switch (expr->op) {
	case '*':
		return expand_dereference(C, expr);

	case '&':
		return expand_addressof(C, expr);

	case SPECIAL_INCREMENT:
	case SPECIAL_DECREMENT:
		/*
		 * From a type evaluation standpoint the preops are
		 * the same as the postops
		 */
		return expand_postop(C, expr);

	default:
		break;
	}
	cost = expand_expression(C, expr->unop);

	if (simplify_preop(C, expr))
		return 0;
	if (simplify_float_preop(C, expr))
		return 0;
	return cost + 1;
}

static int expand_arguments(struct dmr_C *C, struct expression_list *head)
{
	int cost = 0;
	struct expression *expr;

	FOR_EACH_PTR (head, expr) {
		cost += expand_expression(C, expr);
	} END_FOR_EACH_PTR(expr);
	return cost;
}

static int expand_cast(struct dmr_C *C, struct expression *expr)
{
	int cost;
	struct expression *target = expr->cast_expression;

	cost = expand_expression(C, target);

	/* Simplify normal integer casts.. */
	if (target->type == EXPR_VALUE || target->type == EXPR_FVALUE) {
		dmrC_cast_value(C, expr, expr->ctype, target, target->ctype);
		return 0;
	}
	return cost + 1;
}

/*
 * expand a call expression with a symbol. This
 * should expand builtins.
 */
static int expand_symbol_call(struct dmr_C *C, struct expression *expr, int cost)
{
	struct expression *fn = expr->fn;
	struct symbol *ctype = fn->ctype;

	if (fn->type != EXPR_PREOP)
		return SIDE_EFFECTS;

	if (ctype->op && ctype->op->expand)
		return ctype->op->expand(C, expr, cost);

	if (ctype->ctype.modifiers & MOD_PURE)
		return cost + 1;

	return SIDE_EFFECTS;
}

static int expand_call(struct dmr_C *C, struct expression *expr)
{
	int cost;
	struct symbol *sym;
	struct expression *fn = expr->fn;

	cost = expand_arguments(C, expr->args);
	sym = fn->ctype;
	if (!sym) {
		dmrC_expression_error(C, expr, "function has no type");
		return SIDE_EFFECTS;
	}
	if (sym->type == SYM_NODE)
		return expand_symbol_call(C, expr, cost);

	return SIDE_EFFECTS;
}

static int expand_expression_list(struct dmr_C *C, struct expression_list *list)
{
	int cost = 0;
	struct expression *expr;

	FOR_EACH_PTR(list, expr) {
		cost += expand_expression(C, expr);
	} END_FOR_EACH_PTR(expr);
	return cost;
}

/* 
 * We can simplify nested position expressions if
 * this is a simple (single) positional expression.
 */
static int expand_pos_expression(struct dmr_C *C, struct expression *expr)
{
	struct expression *nested = expr->init_expr;
	unsigned long offset = expr->init_offset;
	int nr = expr->init_nr;

	if (nr == 1) {
		switch (nested->type) {
		case EXPR_POS:
			offset += nested->init_offset;
			*expr = *nested;
			expr->init_offset = offset;
			nested = expr;
			break;

		case EXPR_INITIALIZER: {
			struct expression *reuse = nested, *entry;
			*expr = *nested;
			FOR_EACH_PTR(expr->expr_list, entry) {
				if (entry->type == EXPR_POS) {
					entry->init_offset += offset;
				} else {
					if (!reuse) {
						/*
						 * This happens rarely, but it can happen
						 * with bitfields that are all at offset
						 * zero..
						 */
						reuse = dmrC_alloc_expression(C, entry->pos, EXPR_POS);
					}
					reuse->type = EXPR_POS;
					reuse->ctype = entry->ctype;
					reuse->init_offset = offset;
					reuse->init_nr = 1;
					reuse->init_expr = entry;
					REPLACE_CURRENT_PTR(struct expression *, entry, reuse);
					reuse = NULL;
				}
			} END_FOR_EACH_PTR(entry);
			nested = expr;
			break;
		}

		default:
			break;
		}
	}
	return expand_expression(C, nested);
}

static unsigned long bit_offset(struct dmr_C *C, const struct expression *expr)
{
	unsigned long offset = 0;
	while (expr->type == EXPR_POS) {
		offset += dmrC_bytes_to_bits(C->target, expr->init_offset);
		expr = expr->init_expr;
	}
	if (expr && expr->ctype)
		offset += expr->ctype->bit_offset;
	return offset;
}

static unsigned long bit_range(const struct expression *expr)
{
	unsigned long range = 0;
	unsigned long size = 0;
	while (expr->type == EXPR_POS) {
		unsigned long nr = expr->init_nr;
		size = expr->ctype->bit_size;
		range += (nr - 1) * size;
		expr = expr->init_expr;
	}
	range += size;
	return range;
}

static int compare_expressions(void *userdata, const void *_a, const void *_b)
{
	struct dmr_C *C = (struct dmr_C *) userdata;
	const struct expression *a = (const struct expression *)_a;
	const struct expression *b = (const struct expression *)_b;
	unsigned long a_pos = bit_offset(C, a);
	unsigned long b_pos = bit_offset(C, b);

	return (a_pos < b_pos) ? -1 : (a_pos == b_pos) ? 0 : 1;
}

static void sort_expression_list(struct dmr_C *C, struct expression_list **list)
{
	ptrlist_sort((struct ptr_list **)list, C, compare_expressions);
}

static void verify_nonoverlapping(struct dmr_C *C, struct expression_list **list, struct expression *expr)
{
	struct expression *a = NULL;
	unsigned long max = 0;
	unsigned long whole = expr->ctype->bit_size;
	struct expression *b;

	if (!C->Woverride_init)
		return;

	FOR_EACH_PTR(*list, b) {
		unsigned long off, end;
		if (!b->ctype || !b->ctype->bit_size)
			continue;
		off = bit_offset(C, b);
		if (a && off < max) {
			dmrC_warning(C, a->pos, "Initializer entry defined twice");
			dmrC_info(C, b->pos, "  also defined here");
			if (!C->Woverride_init_all)
				return;
		}
		end = off + bit_range(b);
		if (!a && !C->Woverride_init_whole_range) {
			// If first entry is the whole range, do not let
			// any warning about it (this allow to initialize
			// an array with some default value and then override
			// some specific entries).
			if (off == 0 && end == whole)
				continue;
		}
		if (end > max) {
			max = end;
			a = b;
		}
	} END_FOR_EACH_PTR(b);
}

static int expand_expression(struct dmr_C *C, struct expression *expr)
{
	if (!expr)
		return 0;
	if (!expr->ctype || expr->ctype == &C->S->bad_ctype)
		return UNSAFE;

	switch (expr->type) {
	case EXPR_VALUE:
	case EXPR_FVALUE:
	case EXPR_STRING:
		return 0;
	case EXPR_TYPE:
	case EXPR_SYMBOL:
		return expand_symbol_expression(C, expr);
	case EXPR_BINOP:
		return expand_binop(C, expr);

	case EXPR_LOGICAL:
		return expand_logical(C, expr);

	case EXPR_COMMA:
		return expand_comma(C, expr);

	case EXPR_COMPARE:
		return expand_compare(C, expr);

	case EXPR_ASSIGNMENT:
		return expand_assignment(C, expr);

	case EXPR_PREOP:
		return expand_preop(C, expr);

	case EXPR_POSTOP:
		return expand_postop(C, expr);

	case EXPR_CAST:
	case EXPR_FORCE_CAST:
	case EXPR_IMPLIED_CAST:
		return expand_cast(C, expr);

	case EXPR_CALL:
		return expand_call(C, expr);

	case EXPR_DEREF:
		dmrC_warning(C, expr->pos, "we should not have an EXPR_DEREF left at expansion time");
		return UNSAFE;

	case EXPR_SELECT:
	case EXPR_CONDITIONAL:
		return expand_conditional(C, expr);

	case EXPR_STATEMENT: {
		struct statement *stmt = expr->statement;
		int cost = expand_statement(C, stmt);

		if (stmt->type == STMT_EXPRESSION && stmt->expression)
			*expr = *stmt->expression;
		return cost;
	}

	case EXPR_LABEL:
		return 0;

	case EXPR_INITIALIZER:
		sort_expression_list(C, &expr->expr_list);
		verify_nonoverlapping(C, &expr->expr_list, expr);
		return expand_expression_list(C, expr->expr_list);

	case EXPR_IDENTIFIER:
		return UNSAFE;

	case EXPR_INDEX:
		return UNSAFE;

	case EXPR_SLICE:
		return expand_expression(C, expr->base) + 1;

	case EXPR_POS:
		return expand_pos_expression(C, expr);

	case EXPR_SIZEOF:
	case EXPR_PTRSIZEOF:
	case EXPR_ALIGNOF:
	case EXPR_OFFSETOF:
		dmrC_expression_error(C, expr, "internal front-end error: sizeof in expansion?");
		return UNSAFE;
	}
	return SIDE_EFFECTS;
}

static void expand_const_expression(struct dmr_C *C, struct expression *expr, const char *where)
{
	if (expr) {
		expand_expression(C, expr);
		if (expr->type != EXPR_VALUE)
			dmrC_expression_error(C, expr, "Expected constant expression in %s", where);
	}
}

int dmrC_expand_symbol(struct dmr_C *C, struct symbol *sym)
{
	int retval;
	struct symbol *base_type;

	if (!sym)
		return 0;
	base_type = sym->ctype.base_type;
	if (!base_type)
		return 0;

	retval = expand_expression(C, sym->initializer);
	/* expand the body of the symbol */
	if (base_type->type == SYM_FN) {
		if (base_type->stmt)
			expand_statement(C, base_type->stmt);
	}
	return retval;
}

static void expand_return_expression(struct dmr_C *C, struct statement *stmt)
{
	expand_expression(C, stmt->expression);
}

static int expand_if_statement(struct dmr_C *C, struct statement *stmt)
{
	struct expression *expr = stmt->if_conditional;

	if (!expr || !expr->ctype || expr->ctype == &C->S->bad_ctype)
		return UNSAFE;

	expand_expression(C, expr);

/* This is only valid if nobody jumps into the "dead" side */
#if 0
	/* Simplify constant conditionals without even evaluating the false side */
	if (expr->type == EXPR_VALUE) {
		struct statement *simple;
		simple = expr->value ? stmt->if_true : stmt->if_false;

		/* Nothing? */
		if (!simple) {
			stmt->type = STMT_NONE;
			return 0;
		}
		expand_statement(simple);
		*stmt = *simple;
		return SIDE_EFFECTS;
	}
#endif
	expand_statement(C, stmt->if_true);
	expand_statement(C, stmt->if_false);
	return SIDE_EFFECTS;
}

/*
 * Expanding a compound statement is really just
 * about adding up the costs of each individual
 * statement.
 *
 * We also collapse a simple compound statement:
 * this would trigger for simple inline functions,
 * except we would have to check the "return"
 * symbol usage. Next time.
 */
static int expand_compound(struct dmr_C *C, struct statement *stmt)
{
	struct statement *s, *last;
	int cost, statements;

	if (stmt->ret)
		dmrC_expand_symbol(C, stmt->ret);

	last = stmt->args;
	cost = expand_statement(C, last);
	statements = last != NULL;
	FOR_EACH_PTR(stmt->stmts, s) {
		statements++;
		last = s;
		cost += expand_statement(C, s);
	} END_FOR_EACH_PTR(s);

	if (statements == 1 && !stmt->ret)
		*stmt = *last;

	return cost;
}

static int expand_statement(struct dmr_C *C, struct statement *stmt)
{
	if (!stmt)
		return 0;

	switch (stmt->type) {
	case STMT_DECLARATION: {
		struct symbol *sym;
		FOR_EACH_PTR(stmt->declaration, sym) {
			dmrC_expand_symbol(C, sym);
		} END_FOR_EACH_PTR(sym);
		return SIDE_EFFECTS;
	}

	case STMT_RETURN:
		expand_return_expression(C, stmt);
		return SIDE_EFFECTS;

	case STMT_EXPRESSION:
		return expand_expression(C, stmt->expression);

	case STMT_COMPOUND:
		return expand_compound(C, stmt);

	case STMT_IF:
		return expand_if_statement(C, stmt);

	case STMT_ITERATOR:
		expand_expression(C, stmt->iterator_pre_condition);
		expand_expression(C, stmt->iterator_post_condition);
		expand_statement(C, stmt->iterator_pre_statement);
		expand_statement(C, stmt->iterator_statement);
		expand_statement(C, stmt->iterator_post_statement);
		return SIDE_EFFECTS;

	case STMT_SWITCH:
		expand_expression(C, stmt->switch_expression);
		expand_statement(C, stmt->switch_statement);
		return SIDE_EFFECTS;

	case STMT_CASE:
		expand_const_expression(C, stmt->case_expression, "case statement");
		expand_const_expression(C, stmt->case_to, "case statement");
		expand_statement(C, stmt->case_statement);
		return SIDE_EFFECTS;

	case STMT_LABEL:
		expand_statement(C, stmt->label_statement);
		return SIDE_EFFECTS;

	case STMT_GOTO:
		expand_expression(C, stmt->goto_expression);
		return SIDE_EFFECTS;

	case STMT_NONE:
		break;
	case STMT_ASM:
		/* FIXME! Do the asm parameter evaluation! */
		break;
	case STMT_CONTEXT:
		expand_expression(C, stmt->expression);
		break;
	case STMT_RANGE:
		expand_expression(C, stmt->range_expression);
		expand_expression(C, stmt->range_low);
		expand_expression(C, stmt->range_high);
		break;
	}
	return SIDE_EFFECTS;
}

static inline int bad_integer_constant_expression(struct dmr_C *C, struct expression *expr)
{
        (void) C;
	if (!(expr->flags & Int_const_expr))
		return 1;
	if (expr->taint & Taint_comma)
		return 1;
	return 0;
}

static long long __get_expression_value(struct dmr_C *C, struct expression *expr, int strict)
{
	long long value, mask;
	struct symbol *ctype;

	if (!expr)
		return 0;
	ctype = dmrC_evaluate_expression(C, expr);
	if (!ctype) {
		dmrC_expression_error(C, expr, "bad constant expression type");
		return 0;
	}
	expand_expression(C, expr);
	if (expr->type != EXPR_VALUE) {
		if (strict != 2)
			dmrC_expression_error(C, expr, "bad constant expression");
		return 0;
	}
	if ((strict == 1) && bad_integer_constant_expression(C, expr)) {
		dmrC_expression_error(C, expr, "bad integer constant expression");
		return 0;
	}

	value = expr->value;
	mask = 1ULL << (ctype->bit_size-1);

	if (value & mask) {
		while (ctype->type != SYM_BASETYPE)
			ctype = ctype->ctype.base_type;
		if (!(ctype->ctype.modifiers & MOD_UNSIGNED))
			value = value | mask | ~(mask-1);
	}
	return value;
}

long long dmrC_get_expression_value(struct dmr_C *C, struct expression *expr)
{
	return __get_expression_value(C, expr, 0);
}

long long dmrC_const_expression_value(struct dmr_C *C, struct expression *expr)
{
	return __get_expression_value(C, expr, 1);
}

long long dmrC_get_expression_value_silent(struct dmr_C *C, struct expression *expr)
{

	return __get_expression_value(C, expr, 2);
}

int dmrC_expr_truth_value(struct dmr_C *C, struct expression *expr)
{
	const int saved = conservative;
	struct symbol *ctype;

	if (!expr)
		return 0;

	ctype = dmrC_evaluate_expression(C, expr);
	if (!ctype)
		return -1;

	conservative = 1;
	expand_expression(C, expr);
	conservative = saved;

redo:
	switch (expr->type) {
	case EXPR_COMMA:
		expr = expr->right;
		goto redo;
	case EXPR_VALUE:
		return expr->value != 0;
	case EXPR_FVALUE:
		return expr->fvalue != 0;
	default:
		return -1;
	}
}

int dmrC_is_zero_constant(struct dmr_C *C, struct expression *expr)
{
	const int saved = conservative;
	conservative = 1;
	expand_expression(C, expr);
	conservative = saved;
	return expr->type == EXPR_VALUE && !expr->value;
}

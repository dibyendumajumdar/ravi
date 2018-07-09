/*
 * sparse/evaluate.c
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
 * Evaluate constant expressions.
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
#include <target.h>

static struct symbol *degenerate(struct dmr_C *C, struct expression *expr);
static struct symbol *evaluate_symbol(struct dmr_C *C, struct symbol *sym);
static void examine_fn_arguments(struct dmr_C *C, struct symbol *fn);
static struct symbol *cast_to_bool(struct dmr_C *C, struct expression *expr);

static struct symbol *evaluate_symbol_expression(struct dmr_C *C, struct expression *expr)
{
	struct expression *addr;
	struct symbol *sym = expr->symbol;
	struct symbol *base_type;

	if (!sym) {
		dmrC_expression_error(C, expr, "undefined identifier '%s'", dmrC_show_ident(C, expr->symbol_name));
		return NULL;
	}

	dmrC_examine_symbol_type(C->S, sym);

	base_type = dmrC_get_base_type(C->S, sym);
	if (!base_type) {
		dmrC_expression_error(C, expr, "identifier '%s' has no type", dmrC_show_ident(C, expr->symbol_name));
		return NULL;
	}

	addr = dmrC_alloc_expression(C, expr->pos, EXPR_SYMBOL);
	addr->symbol = sym;
	addr->symbol_name = expr->symbol_name;
	addr->ctype = &C->S->lazy_ptr_ctype;	/* Lazy evaluation: we need to do a proper job if somebody does &sym */
	expr->type = EXPR_PREOP;
	expr->op = '*';
	expr->unop = addr;

	/* The type of a symbol is the symbol itself! */
	expr->ctype = sym;
	return sym;
}

static struct symbol *evaluate_string(struct dmr_C *C, struct expression *expr)
{
	struct symbol *sym = dmrC_alloc_symbol(C->S, expr->pos, SYM_NODE);
	struct symbol *array = dmrC_alloc_symbol(C->S, expr->pos, SYM_ARRAY);
	struct expression *addr = dmrC_alloc_expression(C, expr->pos, EXPR_SYMBOL);
	struct expression *initstr = dmrC_alloc_expression(C, expr->pos, EXPR_STRING);
	unsigned int length = expr->string->length;

	sym->array_size = dmrC_alloc_const_expression(C, expr->pos, length);
	sym->bit_size = dmrC_bytes_to_bits(C->target, length);
	sym->ctype.alignment = 1;
	sym->string = 1;
	sym->ctype.modifiers = MOD_STATIC;
	sym->ctype.base_type = array;
	sym->initializer = initstr;

	initstr->ctype = sym;
	initstr->string = expr->string;

	array->array_size = sym->array_size;
	array->bit_size = dmrC_bytes_to_bits(C->target, length);
	array->ctype.alignment = 1;
	array->ctype.modifiers = MOD_STATIC;
	array->ctype.base_type = &C->S->char_ctype;
	
	addr->symbol = sym;
	addr->ctype = &C->S->lazy_ptr_ctype;

	expr->type = EXPR_PREOP;
	expr->op = '*';
	expr->unop = addr;  
	expr->ctype = sym;
	return sym;
}

/* type has come from classify_type and is an integer type */
static inline struct symbol *integer_promotion(struct dmr_C *C, struct symbol *type)
{
	unsigned long mod =  type->ctype.modifiers;
	int width = type->bit_size;

	/*
	 * Bitfields always promote to the base type,
	 * even if the bitfield might be bigger than
	 * an "int".
	 */
	if (type->type == SYM_BITFIELD) {
		type = type->ctype.base_type;
	}
	mod = type->ctype.modifiers;
	if (width < C->target->bits_in_int)
		return &C->S->int_ctype;

	/* If char/short has as many bits as int, it still gets "promoted" */
	if (mod & (MOD_CHAR | MOD_SHORT)) {
		if (mod & MOD_UNSIGNED)
			return &C->S->uint_ctype;
		return &C->S->int_ctype;
	}
	return type;
}

/*
 * integer part of usual arithmetic conversions:
 *	integer promotions are applied
 *	if left and right are identical, we are done
 *	if signedness is the same, convert one with lower rank
 *	unless unsigned argument has rank lower than signed one, convert the
 *	signed one.
 *	if signed argument is bigger than unsigned one, convert the unsigned.
 *	otherwise, convert signed.
 *
 * Leaving aside the integer promotions, that is equivalent to
 *	if identical, don't convert
 *	if left is bigger than right, convert right
 *	if right is bigger than left, convert right
 *	otherwise, if signedness is the same, convert one with lower rank
 *	otherwise convert the signed one.
 */
static struct symbol *bigger_int_type(struct dmr_C *C, struct symbol *left, struct symbol *right)
{
	unsigned long lmod, rmod;

	left = integer_promotion(C, left);
	right = integer_promotion(C, right);

	if (left == right)
		goto left;

	if (left->bit_size > right->bit_size)
		goto left;

	if (right->bit_size > left->bit_size)
		goto right;

	lmod = left->ctype.modifiers;
	rmod = right->ctype.modifiers;
	if ((lmod ^ rmod) & MOD_UNSIGNED) {
		if (lmod & MOD_UNSIGNED)
			goto left;
	} else if ((lmod & ~rmod) & (MOD_LONG_ALL))
		goto left;
right:
	left = right;
left:
	return left;
}

static int same_cast_type(struct dmr_C *C, struct symbol *orig, struct symbol *news)
{
	(void)C;
	return orig->bit_size == news->bit_size &&
	       orig->bit_offset == news->bit_offset;
}

static struct symbol *base_type(struct symbol *node, unsigned long *modp, unsigned long *asp)
{
	unsigned long mod, as;

	mod = 0; as = 0;
	while (node) {
		mod |= node->ctype.modifiers;
		as |= node->ctype.as;
		if (node->type == SYM_NODE) {
			node = node->ctype.base_type;
			continue;
		}
		break;
	}
	*modp = mod & ~MOD_IGNORE;
	*asp = as;
	return node;
}

static int is_same_type(struct dmr_C *C, struct expression *expr, struct symbol *news)
{
	struct symbol *old = expr->ctype;
	unsigned long oldmod, newmod, oldas, newas;

	old = base_type(old, &oldmod, &oldas);
	news = base_type(news, &newmod, &newas);

	/* Same base type, same address space? */
	if (old == news && oldas == newas) {
		unsigned long difmod;

		/* Check the modifier bits. */
		difmod = (oldmod ^ newmod) & ~MOD_NOCAST;

		/* Exact same type? */
		if (!difmod)
			return 1;

		/*
		 * Not the same type, but differs only in "const".
		 * Don't warn about MOD_NOCAST.
		 */
		if (difmod == MOD_CONST)
			return 0;
	}
	if ((oldmod | newmod) & MOD_NOCAST) {
		const char *tofrom = "to/from";
		if (!(newmod & MOD_NOCAST))
			tofrom = "from";
		if (!(oldmod & MOD_NOCAST))
			tofrom = "to";
		dmrC_warning(C, expr->pos, "implicit cast %s nocast type", tofrom);
	}
	return 0;
}

static void
warn_for_different_enum_types (struct dmr_C *C, struct position pos,
			       struct symbol *typea,
			       struct symbol *typeb)
{
	if (!C->Wenum_mismatch)
		return;
	if (typea->type == SYM_NODE)
		typea = typea->ctype.base_type;
	if (typeb->type == SYM_NODE)
		typeb = typeb->ctype.base_type;

	if (typea == typeb)
		return;

	if (typea->type == SYM_ENUM && typeb->type == SYM_ENUM) {
		dmrC_warning(C, pos, "mixing different enum types");
		dmrC_info(C, pos, "    %s versus", dmrC_show_typename(C, typea));
		dmrC_info(C, pos, "    %s", dmrC_show_typename(C, typeb));
	}
}

/*
 * This gets called for implicit casts in assignments and
 * integer promotion. We often want to try to move the
 * cast down, because the ops involved may have been
 * implicitly cast up, and we can get rid of the casts
 * early.
 */
static struct expression * cast_to(struct dmr_C *C, struct expression *old, struct symbol *type)
{
	struct expression *expr;

	warn_for_different_enum_types (C, old->pos, old->ctype, type);

	if (old->ctype != &C->S->null_ctype && is_same_type(C, old, type))
		return old;

	/*
	 * See if we can simplify the op. Move the cast down.
	 */
	switch (old->type) {
	case EXPR_PREOP:
		if (old->ctype->bit_size < type->bit_size)
			break;
		if (old->op == '~') {
			old->ctype = type;
			old->unop = cast_to(C, old->unop, type);
			return old;
		}
		break;

	case EXPR_IMPLIED_CAST:
		warn_for_different_enum_types(C, old->pos, old->ctype, type);

		if (old->ctype->bit_size >= type->bit_size) {
			struct expression *orig = old->cast_expression;
			if (same_cast_type(C, orig->ctype, type))
				return orig;
			if (old->ctype->bit_offset == type->bit_offset) {
				old->ctype = type;
				old->cast_type = type;
				return old;
			}
		}
		break;

	default:
		/* nothing */;
	}

	expr = dmrC_alloc_expression(C, old->pos, EXPR_IMPLIED_CAST);
	expr->flags = old->flags;
	expr->ctype = type;
	expr->cast_type = type;
	expr->cast_expression = old;

	if (dmrC_is_bool_type(C->S, type))
		cast_to_bool(C, expr);

	return expr;
}

enum {
	TYPE_NUM = 1,
	TYPE_BITFIELD = 2,
	TYPE_RESTRICT = 4,
	TYPE_FLOAT = 8,
	TYPE_PTR = 16,
	TYPE_COMPOUND = 32,
	TYPE_FOULED = 64,
	TYPE_FN = 128,
};

static inline int classify_type(struct dmr_C *C, struct symbol *type, struct symbol **base)
{
	static int type_class[SYM_BAD + 1] = {
		[SYM_PTR] = TYPE_PTR,
		[SYM_FN] = TYPE_PTR | TYPE_FN,
		[SYM_ARRAY] = TYPE_PTR | TYPE_COMPOUND,
		[SYM_STRUCT] = TYPE_COMPOUND,
		[SYM_UNION] = TYPE_COMPOUND,
		[SYM_BITFIELD] = TYPE_NUM | TYPE_BITFIELD,
		[SYM_RESTRICT] = TYPE_NUM | TYPE_RESTRICT,
		[SYM_FOULED] = TYPE_NUM | TYPE_RESTRICT | TYPE_FOULED,
	};
	if (type->type == SYM_NODE)
		type = type->ctype.base_type;
	if (type->type == SYM_TYPEOF) {
		type = dmrC_evaluate_expression(C, type->initializer);
		if (!type)
			type = &C->S->bad_ctype;
		else if (type->type == SYM_NODE)
			type = type->ctype.base_type;
	}
	if (type->type == SYM_ENUM)
		type = type->ctype.base_type;
	*base = type;
	if (type->type == SYM_BASETYPE) {
		if (type->ctype.base_type == &C->S->int_type)
			return TYPE_NUM;
		if (type->ctype.base_type == &C->S->fp_type)
			return TYPE_NUM | TYPE_FLOAT;
	}
	return type_class[type->type];
}

#define is_int(klass) ((klass & (TYPE_NUM | TYPE_FLOAT)) == TYPE_NUM)

static inline int is_string_type(struct dmr_C *C, struct symbol *type)
{
	if (type->type == SYM_NODE)
		type = type->ctype.base_type;
	return type->type == SYM_ARRAY && dmrC_is_byte_type(C->target, type->ctype.base_type);
}

static struct symbol *bad_expr_type(struct dmr_C *C, struct expression *expr)
{
	dmrC_sparse_error(C, expr->pos, "incompatible types for operation (%s)", dmrC_show_special(C, expr->op));
	switch (expr->type) {
	case EXPR_BINOP:
	case EXPR_COMPARE:
		dmrC_info(C, expr->pos, "   left side has type %s", dmrC_show_typename(C, expr->left->ctype));
		dmrC_info(C, expr->pos, "   right side has type %s", dmrC_show_typename(C, expr->right->ctype));
		break;
	case EXPR_PREOP:
	case EXPR_POSTOP:
		dmrC_info(C, expr->pos, "   argument has type %s", dmrC_show_typename(C, expr->unop->ctype));
		break;
	default:
		break;
	}

	expr->flags = 0;
	return expr->ctype = &C->S->bad_ctype;
}

static int restricted_value(struct expression *v, struct symbol *type)
{
	(void)type;
	if (v->type != EXPR_VALUE)
		return 1;
	if (v->value != 0)
		return 1;
	return 0;
}

static int restricted_binop(int op)
{
	switch (op) {
		case '&':
		case '=':
		case SPECIAL_AND_ASSIGN:
		case SPECIAL_OR_ASSIGN:
		case SPECIAL_XOR_ASSIGN:
			return 1;	/* unfoul */
		case '|':
		case '^':
		case '?':
			return 2;	/* keep fouled */
		case SPECIAL_EQUAL:
		case SPECIAL_NOTEQUAL:
			return 3;	/* warn if fouled */
		default:
			return 0;	/* warn */
	}
}

static int restricted_unop(struct dmr_C *C, int op, struct symbol **type)
{
	if (op == '~') {
		if ((*type)->bit_size < C->target->bits_in_int)
			*type = dmrC_befoul(C->S, *type);
		return 0;
	} if (op == '+')
		return 0;
	return 1;
}

/* type should be SYM_FOULED */
static inline struct symbol *unfoul(struct symbol *type)
{
	return type->ctype.base_type;
}

static struct symbol *restricted_binop_type(struct dmr_C *C, int op,
					struct expression *left,
					struct expression *right,
					int lclass, int rclass,
					struct symbol *ltype,
					struct symbol *rtype)
{
        (void) C;
	struct symbol *ctype = NULL;
	if (lclass & TYPE_RESTRICT) {
		if (rclass & TYPE_RESTRICT) {
			if (ltype == rtype) {
				ctype = ltype;
			} else if (lclass & TYPE_FOULED) {
				if (unfoul(ltype) == rtype)
					ctype = ltype;
			} else if (rclass & TYPE_FOULED) {
				if (unfoul(rtype) == ltype)
					ctype = rtype;
			}
		} else {
			if (!restricted_value(right, ltype))
				ctype = ltype;
		}
	} else if (!restricted_value(left, rtype))
		ctype = rtype;

	if (ctype) {
		switch (restricted_binop(op)) {
		case 1:
			if ((lclass ^ rclass) & TYPE_FOULED)
				ctype = unfoul(ctype);
			break;
		case 3:
			if (!(lclass & rclass & TYPE_FOULED))
				break;
		case 0:
			ctype = NULL;
		default:
			break;
		}
	}

	return ctype;
}

static inline void unrestrict(struct dmr_C *C, struct expression *expr,
			      int klass, struct symbol **ctype)
{
	if (klass & TYPE_RESTRICT) {
		if (klass & TYPE_FOULED)
			*ctype = unfoul(*ctype);
		dmrC_warning(C, expr->pos, "%s degrades to integer",
			dmrC_show_typename(C, *ctype));
		*ctype = (*ctype)->ctype.base_type; /* get to arithmetic type */
	}
}

static struct symbol *usual_conversions(struct dmr_C *C, int op,
					struct expression *left,
					struct expression *right,
					int lclass, int rclass,
					struct symbol *ltype,
					struct symbol *rtype)
{
	struct symbol *ctype;

	warn_for_different_enum_types(C, right->pos, left->ctype, right->ctype);

	if ((lclass | rclass) & TYPE_RESTRICT)
		goto Restr;

Normal:
	if (!(lclass & TYPE_FLOAT)) {
		if (!(rclass & TYPE_FLOAT))
			return bigger_int_type(C, ltype, rtype);
		else
			return rtype;
	} else if (rclass & TYPE_FLOAT) {
		unsigned long lmod = ltype->ctype.modifiers;
		unsigned long rmod = rtype->ctype.modifiers;
		if (rmod & ~lmod & (MOD_LONG_ALL))
			return rtype;
		else
			return ltype;
	} else
		return ltype;

Restr:
	ctype = restricted_binop_type(C, op, left, right,
				      lclass, rclass, ltype, rtype);
	if (ctype)
		return ctype;

	unrestrict(C, left, lclass, &ltype);
	unrestrict(C, right, rclass, &rtype);

	goto Normal;
}

static inline int lvalue_expression(struct dmr_C *C, struct expression *expr)
{
        (void) C;
	return expr->type == EXPR_PREOP && expr->op == '*';
}

static struct symbol *evaluate_ptr_add(struct dmr_C *C, struct expression *expr, struct symbol *itype)
{
	struct expression *index = expr->right;
	struct symbol *ctype, *base;
	int multiply;

	classify_type(C, degenerate(C, expr->left), &ctype);
	base = dmrC_examine_pointer_target(C->S, ctype);

	if (!base) {
		dmrC_expression_error(C, expr, "missing type information");
		return NULL;
	}
	if (dmrC_is_function(base)) {
		dmrC_expression_error(C, expr, "arithmetics on pointers to functions");
		return NULL;
	}

	/* Get the size of whatever the pointer points to */
	multiply = dmrC_is_void_type(C->S, base) ? 1 : dmrC_bits_to_bytes(C->target, base->bit_size);

	if (ctype == &C->S->null_ctype)
		ctype = &C->S->ptr_ctype;
	expr->ctype = ctype;

	if (multiply == 1 && itype->bit_size >= C->target->bits_in_pointer)
		return ctype;

	if (index->type == EXPR_VALUE) {
		struct expression *val = dmrC_alloc_expression(C, expr->pos, EXPR_VALUE);
		unsigned long long v = index->value, mask;
		mask = 1ULL << (itype->bit_size - 1);
		if (v & mask)
			v |= -mask;
		else
			v &= mask - 1;
		v *= multiply;
		mask = 1ULL << (C->target->bits_in_pointer - 1);
		v &= mask | (mask - 1);
		val->value = v;
		val->ctype = C->target->ssize_t_ctype;
		expr->right = val;
		return ctype;
	}

	if (itype->bit_size < C->target->bits_in_pointer)
		index = cast_to(C, index, C->target->ssize_t_ctype);

	if (multiply > 1) {
		struct expression *val = dmrC_alloc_expression(C, expr->pos, EXPR_VALUE);
		struct expression *mul = dmrC_alloc_expression(C, expr->pos, EXPR_BINOP);

		val->ctype = C->target->ssize_t_ctype;
		val->value = multiply;

		mul->op = '*';
		mul->ctype = C->target->ssize_t_ctype;
		mul->left = index;
		mul->right = val;
		index = mul;
	}

	expr->right = index;
	return ctype;
}


#define MOD_IGN (MOD_VOLATILE | MOD_CONST | MOD_PURE)

const char *dmrC_type_difference(struct dmr_C *C, struct ctype *c1, struct ctype *c2,
	unsigned long mod1, unsigned long mod2)
{
	unsigned long as1 = c1->as, as2 = c2->as;
	struct symbol *t1 = c1->base_type;
	struct symbol *t2 = c2->base_type;
	int move1 = 1, move2 = 1;
	mod1 |= c1->modifiers;
	mod2 |= c2->modifiers;
	for (;;) {
		unsigned long diff;
		int type;
		struct symbol *base1 = t1->ctype.base_type;
		struct symbol *base2 = t2->ctype.base_type;

		/*
		 * FIXME! Collect alignment and context too here!
		 */
		if (move1) {
			if (t1 && t1->type != SYM_PTR) {
				mod1 |= t1->ctype.modifiers;
				as1 |= t1->ctype.as;
			}
			move1 = 0;
		}

		if (move2) {
			if (t2 && t2->type != SYM_PTR) {
				mod2 |= t2->ctype.modifiers;
				as2 |= t2->ctype.as;
			}
			move2 = 0;
		}

		if (t1 == t2)
			break;
		if (!t1 || !t2)
			return "different types";

		if (t1->type == SYM_NODE || t1->type == SYM_ENUM) {
			t1 = base1;
			move1 = 1;
			if (!t1)
				return "bad types";
			continue;
		}

		if (t2->type == SYM_NODE || t2->type == SYM_ENUM) {
			t2 = base2;
			move2 = 1;
			if (!t2)
				return "bad types";
			continue;
		}

		move1 = move2 = 1;
		type = t1->type;
		if (type != t2->type)
			return "different base types";

		switch (type) {
		default:
			dmrC_sparse_error(C, t1->pos,
				     "internal error: bad type in derived(%d)",
				     type);
			return "bad types";
		case SYM_RESTRICT:
			return "different base types";
		case SYM_UNION:
		case SYM_STRUCT:
			/* allow definition of incomplete structs and unions */
			if (t1->ident == t2->ident)
			  return NULL;
			return "different base types";
		case SYM_ARRAY:
			/* XXX: we ought to compare sizes */
			break;
		case SYM_PTR:
			if (as1 != as2)
				return "different address spaces";
			/* MOD_SPECIFIER is due to idiocy in parse.c */
			if ((mod1 ^ mod2) & ~MOD_IGNORE & ~MOD_SPECIFIER)
				return "different modifiers";
			/* we could be lazier here */
			base1 = dmrC_examine_pointer_target(C->S, t1);
			base2 = dmrC_examine_pointer_target(C->S, t2);
			mod1 = t1->ctype.modifiers;
			as1 = t1->ctype.as;
			mod2 = t2->ctype.modifiers;
			as2 = t2->ctype.as;
			break;
		case SYM_FN: {
			struct symbol *arg1, *arg2;
			int i;

			if (as1 != as2)
				return "different address spaces";
			if ((mod1 ^ mod2) & ~MOD_IGNORE & ~MOD_SIGNEDNESS)
				return "different modifiers";
			mod1 = t1->ctype.modifiers;
			as1 = t1->ctype.as;
			mod2 = t2->ctype.modifiers;
			as2 = t2->ctype.as;

			if (t1->variadic != t2->variadic)
				return "incompatible variadic arguments";
			examine_fn_arguments(C, t1);
			examine_fn_arguments(C, t2);
			PREPARE_PTR_LIST(t1->arguments, arg1);
			PREPARE_PTR_LIST(t2->arguments, arg2);
			i = 1;
			for (;;) {
				const char *diffstr;
				if (!arg1 && !arg2)
					break;
				if (!arg1 || !arg2)
					return "different argument counts";
				diffstr = dmrC_type_difference(C, &arg1->ctype,
							  &arg2->ctype,
							  MOD_IGN, MOD_IGN);
				if (diffstr) {
					////// FIXME
					static char argdiff[80];
					sprintf(argdiff, "incompatible argument %d (%s)", i, diffstr);
					return argdiff;
				}
				NEXT_PTR_LIST(arg1);
				NEXT_PTR_LIST(arg2);
				i++;
			}
			FINISH_PTR_LIST(arg2);
			FINISH_PTR_LIST(arg1);
			break;
		}
		case SYM_BASETYPE:
			if (as1 != as2)
				return "different address spaces";
			if (base1 != base2)
				return "different base types";
			diff = (mod1 ^ mod2) & ~MOD_IGNORE;
			if (!diff)
				return NULL;
			if (diff & MOD_SIZE)
				return "different type sizes";
			else if (diff & ~MOD_SIGNEDNESS)
				return "different modifiers";
			else
				return "different signedness";
		}
		t1 = base1;
		t2 = base2;
	}
	if (as1 != as2)
		return "different address spaces";
	if ((mod1 ^ mod2) & ~MOD_IGNORE & ~MOD_SIGNEDNESS)
		return "different modifiers";
	return NULL;
}

static void bad_null(struct dmr_C *C, struct expression *expr)
{
	if (C->Wnon_pointer_null)
		dmrC_warning(C, expr->pos, "Using plain integer as NULL pointer");
}

static unsigned long target_qualifiers(struct dmr_C *C, struct symbol *type)
{
        (void) C;
	unsigned long mod = type->ctype.modifiers & MOD_IGN;
	if (type->ctype.base_type && type->ctype.base_type->type == SYM_ARRAY)
		mod = 0;
	return mod;
}

static struct symbol *evaluate_ptr_sub(struct dmr_C *C, struct expression *expr)
{
	const char *typediff;
	struct symbol *ltype, *rtype;
	struct expression *l = expr->left;
	struct expression *r = expr->right;
	struct symbol *lbase;

	classify_type(C, degenerate(C, l), &ltype);
	classify_type(C, degenerate(C, r), &rtype);

	lbase = dmrC_examine_pointer_target(C->S, ltype);
	dmrC_examine_pointer_target(C->S, rtype);
	typediff = dmrC_type_difference(C, &ltype->ctype, &rtype->ctype,
				   target_qualifiers(C, rtype),
				   target_qualifiers(C, ltype));
	if (typediff)
		dmrC_expression_error(C, expr, "subtraction of different types can't work (%s)", typediff);

	if (dmrC_is_function(lbase)) {
		dmrC_expression_error(C, expr, "subtraction of functions? Share your drugs");
		return NULL;
	}

	expr->ctype = C->target->ssize_t_ctype;
	if (lbase->bit_size > C->target->bits_in_char) {
		struct expression *sub = dmrC_alloc_expression(C, expr->pos, EXPR_BINOP);
		struct expression *div = expr;
		struct expression *val = dmrC_alloc_expression(C, expr->pos, EXPR_VALUE);
		unsigned long value = dmrC_bits_to_bytes(C->target, lbase->bit_size);

		val->ctype = C->target->size_t_ctype;
		val->value = value;

		if (value & (value-1)) {
			if (C->Wptr_subtraction_blows)
				dmrC_warning(C, expr->pos, "potentially expensive pointer subtraction");
		}

		sub->op = '-';
		sub->ctype = C->target->ssize_t_ctype;
		sub->left = l;
		sub->right = r;

		div->op = '/';
		div->left = sub;
		div->right = val;
	}
		
	return C->target->ssize_t_ctype;
}

#define is_safe_type(type) ((type)->ctype.modifiers & MOD_SAFE)

static struct symbol *evaluate_conditional(struct dmr_C *C, struct expression *expr, int iterator)
{
	struct symbol *ctype;

	if (!expr)
		return NULL;

	if (!iterator && expr->type == EXPR_ASSIGNMENT && expr->op == '=')
		dmrC_warning(C, expr->pos, "assignment expression in conditional");

	ctype = dmrC_evaluate_expression(C, expr);
	if (ctype) {
		if (is_safe_type(ctype))
			dmrC_warning(C, expr->pos, "testing a 'safe expression'");
		if (dmrC_is_func_type(ctype)) {
			if (C->Waddress)
				dmrC_warning(C, expr->pos, "the address of %s will always evaluate as true", "a function");
		}
		else if (dmrC_is_array_type(ctype)) {
			if (C->Waddress)
				dmrC_warning(C, expr->pos, "the address of %s will always evaluate as true", "an array");
		}
		else if (!dmrC_is_scalar_type(C->S, ctype)) {
			dmrC_sparse_error(C, expr->pos, "incorrect type in conditional");
			dmrC_info(C, expr->pos, "   got %s", dmrC_show_typename(C, ctype));
			ctype = NULL;
		}
	}
	ctype = degenerate(C, expr);

	return ctype;
}

static struct symbol *evaluate_logical(struct dmr_C *C, struct expression *expr)
{
	if (!evaluate_conditional(C, expr->left, 0))
		return NULL;
	if (!evaluate_conditional(C, expr->right, 0))
		return NULL;

	/* the result is int [6.5.13(3), 6.5.14(3)] */
	expr->ctype = &C->S->int_ctype;
	if (expr->flags) {
		if (!(expr->left->flags & expr->right->flags & Int_const_expr))
			expr->flags = 0;
	}
	return &C->S->int_ctype;
}

static struct symbol *evaluate_binop(struct dmr_C *C, struct expression *expr)
{
	struct symbol *ltype, *rtype, *ctype;
	int lclass = classify_type(C, expr->left->ctype, &ltype);
	int rclass = classify_type(C, expr->right->ctype, &rtype);
	int op = expr->op;

	if (expr->flags) {
		if (!(expr->left->flags & expr->right->flags & Int_const_expr))
			expr->flags = 0;
	}

	/* number op number */
	if (lclass & rclass & TYPE_NUM) {
		if ((lclass | rclass) & TYPE_FLOAT) {
			switch (op) {
			case '+': case '-': case '*': case '/':
				break;
			default:
				return bad_expr_type(C, expr);
			}
		}

		if (op == SPECIAL_LEFTSHIFT || op == SPECIAL_RIGHTSHIFT) {
			// shifts do integer promotions, but that's it.
			unrestrict(C, expr->left, lclass, &ltype);
			unrestrict(C, expr->right, rclass, &rtype);
			ctype = ltype = integer_promotion(C, ltype);
			rtype = integer_promotion(C, rtype);
		} else {
			// The rest do usual conversions
			const unsigned left_not  = expr->left->type == EXPR_PREOP
			                           && expr->left->op == '!';
			const unsigned right_not = expr->right->type == EXPR_PREOP
			                           && expr->right->op == '!';
			if ((op == '&' || op == '|') && (left_not || right_not))
				dmrC_warning(C, expr->pos, "dubious: %sx %c %sy",
				        left_not ? "!" : "",
					op,
					right_not ? "!" : "");

			ltype = usual_conversions(C, op, expr->left, expr->right,
						  lclass, rclass, ltype, rtype);
			ctype = rtype = ltype;
		}

		expr->left = cast_to(C, expr->left, ltype);
		expr->right = cast_to(C, expr->right, rtype);
		expr->ctype = ctype;
		return ctype;
	}

	/* pointer (+|-) integer */
	if (lclass & TYPE_PTR && is_int(rclass) && (op == '+' || op == '-')) {
		unrestrict(C, expr->right, rclass, &rtype);
		return evaluate_ptr_add(C, expr, rtype);
	}

	/* integer + pointer */
	if (rclass & TYPE_PTR && is_int(lclass) && op == '+') {
		struct expression *index = expr->left;
		unrestrict(C, index, lclass, &ltype);
		expr->left = expr->right;
		expr->right = index;
		return evaluate_ptr_add(C, expr, ltype);
	}

	/* pointer - pointer */
	if (lclass & rclass & TYPE_PTR && expr->op == '-')
		return evaluate_ptr_sub(C, expr);

	return bad_expr_type(C, expr);
}

static struct symbol *evaluate_comma(struct dmr_C *C, struct expression *expr)
{
	expr->ctype = degenerate(C, expr->right);
	if (expr->ctype == &C->S->null_ctype)
		expr->ctype = &C->S->ptr_ctype;
	expr->flags &= expr->left->flags & expr->right->flags;
	return expr->ctype;
}

static int modify_for_unsigned(struct dmr_C *C, int op)
{
        (void) C;
	if (op == '<')
		op = SPECIAL_UNSIGNED_LT;
	else if (op == '>')
		op = SPECIAL_UNSIGNED_GT;
	else if (op == SPECIAL_LTE)
		op = SPECIAL_UNSIGNED_LTE;
	else if (op == SPECIAL_GTE)
		op = SPECIAL_UNSIGNED_GTE;
	return op;
}

static inline int is_null_pointer_constant(struct dmr_C *C, struct expression *e)
{
	if (e->ctype == &C->S->null_ctype)
		return 1;
	if (!(e->flags & Int_const_expr))
		return 0;
	return dmrC_is_zero_constant(C, e) ? 2 : 0;
}

static struct symbol *evaluate_compare(struct dmr_C *C, struct expression *expr)
{
	struct expression *left = expr->left, *right = expr->right;
	struct symbol *ltype, *rtype, *lbase, *rbase;
	int lclass = classify_type(C, degenerate(C, left), &ltype);
	int rclass = classify_type(C, degenerate(C, right), &rtype);
	struct symbol *ctype;
	const char *typediff;

	if (expr->flags) {
		if (!(expr->left->flags & expr->right->flags & Int_const_expr))
			expr->flags = 0;
	}

	/* Type types? */
	if (dmrC_is_type_type(ltype) && dmrC_is_type_type(rtype))
		goto OK;

	if (is_safe_type(left->ctype) || is_safe_type(right->ctype))
		dmrC_warning(C, expr->pos, "testing a 'safe expression'");

	/* number on number */
	if (lclass & rclass & TYPE_NUM) {
		ctype = usual_conversions(C, expr->op, expr->left, expr->right,
					  lclass, rclass, ltype, rtype);
		expr->left = cast_to(C, expr->left, ctype);
		expr->right = cast_to(C, expr->right, ctype);
		if (ctype->ctype.modifiers & MOD_UNSIGNED)
			expr->op = modify_for_unsigned(C, expr->op);
		goto OK;
	}

	/* at least one must be a pointer */
	if (!((lclass | rclass) & TYPE_PTR))
		return bad_expr_type(C, expr);

	/* equality comparisons can be with null pointer constants */
	if (expr->op == SPECIAL_EQUAL || expr->op == SPECIAL_NOTEQUAL) {
		int is_null1 = is_null_pointer_constant(C, left);
		int is_null2 = is_null_pointer_constant(C, right);
		if (is_null1 == 2)
			bad_null(C, left);
		if (is_null2 == 2)
			bad_null(C, right);
		if (is_null1 && is_null2) {
			int positive = expr->op == SPECIAL_EQUAL;
			expr->type = EXPR_VALUE;
			expr->value = positive;
			goto OK;
		}
		if (is_null1 && (rclass & TYPE_PTR)) {
			left = cast_to(C, left, rtype);
			goto OK;
		}
		if (is_null2 && (lclass & TYPE_PTR)) {
			right = cast_to(C, right, ltype);
			goto OK;
		}
	}
	/* both should be pointers */
	if (!(lclass & rclass & TYPE_PTR))
		return bad_expr_type(C, expr);
	expr->op = modify_for_unsigned(C, expr->op);

	lbase = dmrC_examine_pointer_target(C->S, ltype);
	rbase = dmrC_examine_pointer_target(C->S, rtype);

	/* they also have special treatment for pointers to void */
	if (expr->op == SPECIAL_EQUAL || expr->op == SPECIAL_NOTEQUAL) {
		if (ltype->ctype.as == rtype->ctype.as) {
			if (lbase == &C->S->void_ctype) {
				right = cast_to(C, right, ltype);
				goto OK;
			}
			if (rbase == &C->S->void_ctype) {
				left = cast_to(C, left, rtype);
				goto OK;
			}
		}
	}

	typediff = dmrC_type_difference(C, &ltype->ctype, &rtype->ctype,
				   target_qualifiers(C, rtype),
				   target_qualifiers(C, ltype));
	if (!typediff)
		goto OK;

	dmrC_expression_error(C, expr, "incompatible types in comparison expression (%s)", typediff);
	return NULL;

OK:
	/* the result is int [6.5.8(6), 6.5.9(3)]*/
	expr->ctype = &C->S->int_ctype;
	return &C->S->int_ctype;
}

/*
 * NOTE! The degenerate case of "x ? : y", where we don't
 * have a true case, this will possibly promote "x" to the
 * same type as "y", and thus _change_ the conditional
 * test in the expression. But since promotion is "safe"
 * for testing, that's OK.
 */
static struct symbol *evaluate_conditional_expression(struct dmr_C *C, struct expression *expr)
{
	struct expression **truee;
	struct symbol *ctype, *ltype, *rtype, *lbase, *rbase;
	int lclass, rclass;
	const char * typediff;
	int qual;

	if (!evaluate_conditional(C, expr->conditional, 0))
		return NULL;
	if (!dmrC_evaluate_expression(C, expr->cond_false))
		return NULL;

	ctype = degenerate(C, expr->conditional);
	rtype = degenerate(C, expr->cond_false);

	truee = &expr->conditional;
	ltype = ctype;
	if (expr->cond_true) {
		if (!dmrC_evaluate_expression(C, expr->cond_true))
			return NULL;
		ltype = degenerate(C, expr->cond_true);
		truee = &expr->cond_true;
	}

	if (expr->flags) {
		int flags = expr->conditional->flags & Int_const_expr;
		flags &= (*truee)->flags & expr->cond_false->flags;
		if (!flags)
			expr->flags = 0;
	}

	lclass = classify_type(C, ltype, &ltype);
	rclass = classify_type(C, rtype, &rtype);
	if (lclass & rclass & TYPE_NUM) {
		ctype = usual_conversions(C, '?', *truee, expr->cond_false,
					  lclass, rclass, ltype, rtype);
		*truee = cast_to(C, *truee, ctype);
		expr->cond_false = cast_to(C, expr->cond_false, ctype);
		goto out;
	}

	if ((lclass | rclass) & TYPE_PTR) {
		int is_null1 = is_null_pointer_constant(C, *truee);
		int is_null2 = is_null_pointer_constant(C, expr->cond_false);

		if (is_null1 && is_null2) {
			*truee = cast_to(C, *truee, &C->S->ptr_ctype);
			expr->cond_false = cast_to(C, expr->cond_false, &C->S->ptr_ctype);
			ctype = &C->S->ptr_ctype;
			goto out;
		}
		if (is_null1 && (rclass & TYPE_PTR)) {
			if (is_null1 == 2)
				bad_null(C, *truee);
			*truee = cast_to(C, *truee, rtype);
			ctype = rtype;
			goto out;
		}
		if (is_null2 && (lclass & TYPE_PTR)) {
			if (is_null2 == 2)
				bad_null(C, expr->cond_false);
			expr->cond_false = cast_to(C, expr->cond_false, ltype);
			ctype = ltype;
			goto out;
		}
		if (!(lclass & rclass & TYPE_PTR)) {
			typediff = "different types";
			goto Err;
		}
		/* OK, it's pointer on pointer */
		if (ltype->ctype.as != rtype->ctype.as) {
			typediff = "different address spaces";
			goto Err;
		}

		/* need to be lazier here */
		lbase = dmrC_examine_pointer_target(C->S, ltype);
		rbase = dmrC_examine_pointer_target(C->S, rtype);
		qual = target_qualifiers(C, ltype) | target_qualifiers(C, rtype);

		if (lbase == &C->S->void_ctype) {
			/* XXX: pointers to function should warn here */
			ctype = ltype;
			goto Qual;

		}
		if (rbase == &C->S->void_ctype) {
			/* XXX: pointers to function should warn here */
			ctype = rtype;
			goto Qual;
		}
		/* XXX: that should be pointer to composite */
		ctype = ltype;
		typediff = dmrC_type_difference(C, &ltype->ctype, &rtype->ctype,
					   qual, qual);
		if (!typediff)
			goto Qual;
		goto Err;
	}

	/* void on void, struct on same struct, union on same union */
	if (ltype == rtype) {
		ctype = ltype;
		goto out;
	}
	typediff = "different base types";

Err:
	dmrC_expression_error(C, expr, "incompatible types in conditional expression (%s)", typediff);
	/*
	 * if the condition is constant, the type is in fact known
	 * so use it, as gcc & clang do.
	 */
	switch (dmrC_expr_truth_value(C, expr->conditional)) {
	case 1:	expr->ctype = ltype;
		break;
	case 0: expr->ctype = rtype;
		break;
	default:
		break;
	}
	return NULL;

out:
	expr->ctype = ctype;
	return ctype;

Qual:
	if (qual & ~ctype->ctype.modifiers) {
		struct symbol *sym = dmrC_alloc_symbol(C->S, ctype->pos, SYM_PTR);
		*sym = *ctype;
		sym->ctype.modifiers |= qual;
		ctype = sym;
	}
	*truee = cast_to(C, *truee, ctype);
	expr->cond_false = cast_to(C, expr->cond_false, ctype);
	goto out;
}

/* FP assignments can not do modulo or bit operations */
static int compatible_float_op(int op)
{
	return	op == SPECIAL_ADD_ASSIGN ||
		op == SPECIAL_SUB_ASSIGN ||
		op == SPECIAL_MUL_ASSIGN ||
		op == SPECIAL_DIV_ASSIGN;
}

static int evaluate_assign_op(struct dmr_C *C, struct expression *expr)
{
	struct symbol *target = expr->left->ctype;
	struct symbol *source = expr->right->ctype;
	struct symbol *t, *s;
	int tclass = classify_type(C, target, &t);
	int sclass = classify_type(C, source, &s);
	int op = expr->op;

	if (tclass & sclass & TYPE_NUM) {
		if (tclass & TYPE_FLOAT && !compatible_float_op(op)) {
			dmrC_expression_error(C, expr, "invalid assignment");
			return 0;
		}
		if (tclass & TYPE_RESTRICT) {
			if (!restricted_binop(op)) {
				dmrC_warning(C, expr->pos, "bad assignment (%s) to %s",
					dmrC_show_special(C, op), dmrC_show_typename(C, t));
				expr->right = cast_to(C, expr->right, target);
				return 0;
			}
			/* allowed assignments unfoul */
			if (sclass & TYPE_FOULED && unfoul(s) == t)
				goto Cast;
			if (!restricted_value(expr->right, t))
				return 1;
		} else if (!(sclass & TYPE_RESTRICT))
			goto usual;
		/* source and target would better be identical restricted */
		if (t == s)
			return 1;
		dmrC_warning(C, expr->pos, "invalid assignment: %s", dmrC_show_special(C, op));
		dmrC_info(C, expr->pos, "   left side has type %s", dmrC_show_typename(C, t));
		dmrC_info(C, expr->pos, "   right side has type %s", dmrC_show_typename(C, s));
		expr->right = cast_to(C, expr->right, target);
		return 0;
	}
	if (tclass == TYPE_PTR && is_int(sclass)) {
		if (op == SPECIAL_ADD_ASSIGN || op == SPECIAL_SUB_ASSIGN) {
			unrestrict(C, expr->right, sclass, &s);
			evaluate_ptr_add(C, expr, s);
			return 1;
		}
		dmrC_expression_error(C, expr, "invalid pointer assignment");
		return 0;
	}

	dmrC_expression_error(C, expr, "invalid assignment");
	return 0;

usual:
	target = usual_conversions(C, op, expr->left, expr->right,
				tclass, sclass, target, source);
Cast:
	expr->right = cast_to(C, expr->right, target);
	return 1;
}

static int whitelist_pointers(struct dmr_C *C, struct symbol *t1, struct symbol *t2)
{
	if (t1 == t2)
		return 0;	/* yes, 0 - we don't want a cast_to here */
	if (t1 == &C->S->void_ctype)
		return 1;
	if (t2 == &C->S->void_ctype)
		return 1;
	if (classify_type(C, t1, &t1) != TYPE_NUM)
		return 0;
	if (classify_type(C, t2, &t2) != TYPE_NUM)
		return 0;
	if (t1 == t2)
		return 1;
	if (t1->ctype.modifiers & t2->ctype.modifiers & MOD_CHAR)
		return 1;
	if ((t1->ctype.modifiers ^ t2->ctype.modifiers) & MOD_SIZE)
		return 0;
	return !C->Wtypesign;
}

static int check_assignment_types(struct dmr_C *C, struct symbol *target, struct expression **rp,
	const char **typediff)
{
	struct symbol *source = degenerate(C, *rp);
	struct symbol *t, *s;
	int tclass = classify_type(C, target, &t);
	int sclass = classify_type(C, source, &s);

	if (tclass & sclass & TYPE_NUM) {
		if (tclass & TYPE_RESTRICT) {
			/* allowed assignments unfoul */
			if (sclass & TYPE_FOULED && unfoul(s) == t)
				goto Cast;
			if (!restricted_value(*rp, target))
				return 1;
			if (s == t)
				return 1;
		} else if (!(sclass & TYPE_RESTRICT))
			goto Cast;
		if (t == &C->S->bool_ctype) {
			if (dmrC_is_fouled_type(s))
				dmrC_warning(C, (*rp)->pos, "%s degrades to integer",
					dmrC_show_typename(C, s->ctype.base_type));
			goto Cast;
		}
		*typediff = "different base types";
		return 0;
	}

	if (tclass == TYPE_PTR) {
		unsigned long mod1, mod2;
		struct symbol *b1, *b2;
		// NULL pointer is always OK
		int is_null = is_null_pointer_constant(C, *rp);
		if (is_null) {
			if (is_null == 2)
				bad_null(C, *rp);
			goto Cast;
		}
		if (!(sclass & TYPE_PTR)) {
			*typediff = "different base types";
			return 0;
		}
		b1 = dmrC_examine_pointer_target(C->S, t);
		b2 = dmrC_examine_pointer_target(C->S, s);
		mod1 = target_qualifiers(C, t);
		mod2 = target_qualifiers(C, s);
		if (whitelist_pointers(C, b1, b2)) {
			/*
			 * assignments to/from void * are OK, provided that
			 * we do not remove qualifiers from pointed to [C]
			 * or mix address spaces [sparse].
			 */
			if (t->ctype.as != s->ctype.as) {
				*typediff = "different address spaces";
				return 0;
			}
			/*
			 * If this is a function pointer assignment, it is
			 * actually fine to assign a pointer to const data to
			 * it, as a function pointer points to const data
			 * implicitly, i.e., dereferencing it does not produce
			 * an lvalue.
			 */
			if (b1->type == SYM_FN)
				mod1 |= MOD_CONST;
			if (mod2 & ~mod1) {
				*typediff = "different modifiers";
				return 0;
			}
			goto Cast;
		}
		/* It's OK if the target is more volatile or const than the source */
		*typediff = dmrC_type_difference(C, &t->ctype, &s->ctype, 0, mod1);
		if (*typediff)
			return 0;
		return 1;
	}

	if ((tclass & TYPE_COMPOUND) && s == t)
		return 1;

	if (tclass & TYPE_NUM) {
		/* XXX: need to turn into comparison with NULL */
		if (t == &C->S->bool_ctype && (sclass & TYPE_PTR))
			goto Cast;
		*typediff = "different base types";
		return 0;
	}
	*typediff = "invalid types";
	return 0;
Cast:
	*rp = cast_to(C, *rp, target);
	return 1;
}

static int compatible_assignment_types(struct dmr_C *C, struct expression *expr, struct symbol *target,
	struct expression **rp, const char *where)
{
	const char *typediff;
	struct symbol *source = degenerate(C, *rp);

	if (!check_assignment_types(C, target, rp, &typediff)) {
		dmrC_warning(C, expr->pos, "incorrect type in %s (%s)", where, typediff);
		dmrC_info(C, expr->pos, "   expected %s", dmrC_show_typename(C, target));
		dmrC_info(C, expr->pos, "   got %s", dmrC_show_typename(C, source));
		*rp = cast_to(C, *rp, target);
		return 0;
	}

	return 1;
}

static int compatible_transparent_union(struct dmr_C *C, struct symbol *target,
	struct expression **rp)
{
	struct symbol *t, *member;
	classify_type(C, target, &t);
	if (t->type != SYM_UNION || !t->transparent_union)
		return 0;

	FOR_EACH_PTR(t->symbol_list, member) {
		const char *typediff;
		if (check_assignment_types(C, member, rp, &typediff))
			return 1;
	} END_FOR_EACH_PTR(member);

	return 0;
}

static int compatible_argument_type(struct dmr_C *C, struct expression *expr, struct symbol *target,
	struct expression **rp, const char *where)
{
	if (compatible_transparent_union(C, target, rp))
		return 1;

	return compatible_assignment_types(C, expr, target, rp, where);
}
static void mark_assigned(struct dmr_C *C, struct expression *expr)
{
	struct symbol *sym;

	if (!expr)
		return;
	switch (expr->type) {
	case EXPR_SYMBOL:
		sym = expr->symbol;
		if (!sym)
			return;
		if (sym->type != SYM_NODE)
			return;
		sym->ctype.modifiers |= MOD_ASSIGNED;
		return;

	case EXPR_BINOP:
		mark_assigned(C, expr->left);
		mark_assigned(C, expr->right);
		return;
	case EXPR_CAST:
	case EXPR_FORCE_CAST:
		mark_assigned(C, expr->cast_expression);
		return;
	case EXPR_SLICE:
		mark_assigned(C, expr->base);
		return;
	default:
		/* Hmm? */
		return;
	}
}

static void evaluate_assign_to(struct dmr_C *C, struct expression *left, struct symbol *type)
{
	if (type->ctype.modifiers & MOD_CONST)
		dmrC_expression_error(C, left, "assignment to const expression");

	/* We know left is an lvalue, so it's a "preop-*" */
	mark_assigned(C, left->unop);
}

static struct symbol *evaluate_assignment(struct dmr_C *C, struct expression *expr)
{
	struct expression *left = expr->left;
	struct expression *where = expr;
	struct symbol *ltype;

	if (!lvalue_expression(C, left)) {
		dmrC_expression_error(C, expr, "not an lvalue");
		return NULL;
	}

	ltype = left->ctype;

	if (expr->op != '=') {
		if (!evaluate_assign_op(C, expr))
			return NULL;
	} else {
		if (!compatible_assignment_types(C, where, ltype, &expr->right, "assignment"))
			return NULL;
	}

	evaluate_assign_to(C, left, ltype);

	expr->ctype = ltype;
	return ltype;
}

static void examine_fn_arguments(struct dmr_C *C, struct symbol *fn)
{
	struct symbol *s;

	FOR_EACH_PTR(fn->arguments, s) {
		struct symbol *arg = evaluate_symbol(C, s);
		/* Array/function arguments silently degenerate into pointers */
		if (arg) {
			struct symbol *ptr;
			switch(arg->type) {
			case SYM_ARRAY:
			case SYM_FN:
				ptr = dmrC_alloc_symbol(C->S, s->pos, SYM_PTR);
				if (arg->type == SYM_ARRAY)
					ptr->ctype = arg->ctype;
				else
					ptr->ctype.base_type = arg;
				ptr->ctype.as |= s->ctype.as;
				ptr->ctype.modifiers |= s->ctype.modifiers & MOD_PTRINHERIT;

				s->ctype.base_type = ptr;
				s->ctype.as = 0;
				s->ctype.modifiers &= ~MOD_PTRINHERIT;
				s->bit_size = 0;
				s->examined = 0;
				dmrC_examine_symbol_type(C->S, s);
				break;
			default:
				/* nothing */
				break;
			}
		}
	} END_FOR_EACH_PTR(s);
}

static struct symbol *convert_to_as_mod(struct dmr_C *C, struct symbol *sym, int as, int mod)
{
	/* Take the modifiers of the pointer, and apply them to the member */
	mod |= sym->ctype.modifiers;
	if ((int)(sym->ctype.as) != as || (int)(sym->ctype.modifiers) != mod) {
		struct symbol *newsym = dmrC_alloc_symbol(C->S, sym->pos, SYM_NODE);
		*newsym = *sym;
		newsym->ctype.as = as;
		newsym->ctype.modifiers = mod;
		sym = newsym;
	}
	return sym;
}

static struct symbol *create_pointer(struct dmr_C *C, struct expression *expr, struct symbol *sym, int degenerate)
{
	struct symbol *node = dmrC_alloc_symbol(C->S, expr->pos, SYM_NODE);
	struct symbol *ptr = dmrC_alloc_symbol(C->S, expr->pos, SYM_PTR);

	node->ctype.base_type = ptr;
	ptr->bit_size = C->target->bits_in_pointer;
	ptr->ctype.alignment = C->target->pointer_alignment;

	node->bit_size = C->target->bits_in_pointer;
	node->ctype.alignment = C->target->pointer_alignment;

	dmrC_access_symbol(C->S, sym);
	if (sym->ctype.modifiers & MOD_REGISTER) {
		dmrC_warning(C, expr->pos, "taking address of 'register' variable '%s'", dmrC_show_ident(C, sym->ident));
		sym->ctype.modifiers &= ~MOD_REGISTER;
	}
	if (sym->type == SYM_NODE) {
		ptr->ctype.as |= sym->ctype.as;
		ptr->ctype.modifiers |= sym->ctype.modifiers & MOD_PTRINHERIT;
		sym = sym->ctype.base_type;
	}
	if (degenerate && sym->type == SYM_ARRAY) {
		ptr->ctype.as |= sym->ctype.as;
		ptr->ctype.modifiers |= sym->ctype.modifiers & MOD_PTRINHERIT;
		sym = sym->ctype.base_type;
	}
	ptr->ctype.base_type = sym;

	return node;
}

/* Arrays degenerate into pointers on pointer arithmetic */
static struct symbol *degenerate(struct dmr_C *C, struct expression *expr)
{
	struct symbol *ctype, *base;

	if (!expr)
		return NULL;
	ctype = expr->ctype;
	if (!ctype)
		return NULL;
	base = dmrC_examine_symbol_type(C->S, ctype);
	if (ctype->type == SYM_NODE)
		base = ctype->ctype.base_type;
	/*
	 * Arrays degenerate into pointers to the entries, while
	 * functions degenerate into pointers to themselves.
	 * If array was part of non-lvalue compound, we create a copy
	 * of that compound first and then act as if we were dealing with
	 * the corresponding field in there.
	 */
	switch (base->type) {
	case SYM_ARRAY:
		if (expr->type == EXPR_SLICE) {
			struct symbol *a = dmrC_alloc_symbol(C->S, expr->pos, SYM_NODE);
			struct expression *e0, *e1, *e2, *e3, *e4;

			a->ctype.base_type = expr->base->ctype;
			a->bit_size = expr->base->ctype->bit_size;
			a->array_size = expr->base->ctype->array_size;

			e0 = dmrC_alloc_expression(C, expr->pos, EXPR_SYMBOL);
			e0->symbol = a;
			e0->ctype = &C->S->lazy_ptr_ctype;

			e1 = dmrC_alloc_expression(C, expr->pos, EXPR_PREOP);
			e1->unop = e0;
			e1->op = '*';
			e1->ctype = expr->base->ctype;	/* XXX */

			e2 = dmrC_alloc_expression(C, expr->pos, EXPR_ASSIGNMENT);
			e2->left = e1;
			e2->right = expr->base;
			e2->op = '=';
			e2->ctype = expr->base->ctype;

			if (expr->r_bitpos) {
				e3 = dmrC_alloc_expression(C, expr->pos, EXPR_BINOP);
				e3->op = '+';
				e3->left = e0;
				e3->right = dmrC_alloc_const_expression(C, expr->pos,
							dmrC_bits_to_bytes(C->target, expr->r_bitpos));
				e3->ctype = &C->S->lazy_ptr_ctype;
			} else {
				e3 = e0;
			}

			e4 = dmrC_alloc_expression(C, expr->pos, EXPR_COMMA);
			e4->left = e2;
			e4->right = e3;
			e4->ctype = &C->S->lazy_ptr_ctype;

			expr->unop = e4;
			expr->type = EXPR_PREOP;
			expr->op = '*';
		}
	case SYM_FN:
		if (expr->op != '*' || expr->type != EXPR_PREOP) {
			dmrC_expression_error(C, expr, "strange non-value function or array");
			return &C->S->bad_ctype;
		}
		*expr = *expr->unop;
		ctype = create_pointer(C, expr, ctype, 1);
		expr->ctype = ctype;
	default:
		/* nothing */;
	}
	return ctype;
}

static struct symbol *evaluate_addressof(struct dmr_C *C, struct expression *expr)
{
	struct expression *op = expr->unop;
	struct symbol *ctype;

	if (op->op != '*' || op->type != EXPR_PREOP) {
		dmrC_expression_error(C, expr, "not addressable");
		return NULL;
	}
	ctype = op->ctype;
	*expr = *op->unop;
	expr->flags = 0;

	if (expr->type == EXPR_SYMBOL) {
		struct symbol *sym = expr->symbol;
		sym->ctype.modifiers |= MOD_ADDRESSABLE;
	}

	/*
	 * symbol expression evaluation is lazy about the type
	 * of the sub-expression, so we may have to generate
	 * the type here if so..
	 */
	if (expr->ctype == &C->S->lazy_ptr_ctype) {
		ctype = create_pointer(C, expr, ctype, 0);
		expr->ctype = ctype;
	}
	return expr->ctype;
}


static struct symbol *evaluate_dereference(struct dmr_C *C, struct expression *expr)
{
	struct expression *op = expr->unop;
	struct symbol *ctype = op->ctype, *node, *target;

	/* Simplify: *&(expr) => (expr) */
	if (op->type == EXPR_PREOP && op->op == '&') {
		*expr = *op->unop;
		expr->flags = 0;
		return expr->ctype;
	}

	dmrC_examine_symbol_type(C->S, ctype);

	/* Dereferencing a node drops all the node information. */
	if (ctype->type == SYM_NODE)
		ctype = ctype->ctype.base_type;

	node = dmrC_alloc_symbol(C->S, expr->pos, SYM_NODE);
	target = ctype->ctype.base_type;

	switch (ctype->type) {
	default:
		dmrC_expression_error(C, expr, "cannot dereference this type");
		return NULL;
	case SYM_PTR:
		node->ctype.modifiers = target->ctype.modifiers & MOD_SPECIFIER;
		dmrC_merge_type(node, ctype);
		break;

	case SYM_ARRAY:
		if (!lvalue_expression(C, op)) {
			dmrC_expression_error(C, op, "non-lvalue array??");
			return NULL;
		}

		/* Do the implied "addressof" on the array */
		*op = *op->unop;

		/*
		 * When an array is dereferenced, we need to pick
		 * up the attributes of the original node too..
		 */
		dmrC_merge_type(node, op->ctype);
		dmrC_merge_type(node, ctype);
		break;
	}

	node->bit_size = target->bit_size;
	node->array_size = target->array_size;

	expr->ctype = node;
	return node;
}

/*
 * Unary post-ops: x++ and x--
 */
static struct symbol *evaluate_postop(struct dmr_C *C, struct expression *expr)
{
	struct expression *op = expr->unop;
	struct symbol *ctype = op->ctype;
	int klass = classify_type(C, ctype, &ctype);
	int multiply = 0;

	if (!klass || klass & TYPE_COMPOUND) {
		dmrC_expression_error(C, expr, "need scalar for ++/--");
		return NULL;
	}
	if (!lvalue_expression(C, expr->unop)) {
		dmrC_expression_error(C, expr, "need lvalue expression for ++/--");
		return NULL;
	}

	if ((klass & TYPE_RESTRICT) && restricted_unop(C, expr->op, &ctype))
		unrestrict(C, expr, klass, &ctype);

	if (klass & TYPE_NUM) {
		multiply = 1;
	} else if (klass == TYPE_PTR) {
		struct symbol *target = dmrC_examine_pointer_target(C->S, ctype);
		if (!dmrC_is_function(target))
			multiply = dmrC_bits_to_bytes(C->target, target->bit_size);
	}

	if (multiply) {
		evaluate_assign_to(C, op, op->ctype);
		expr->op_value = multiply;
		expr->ctype = ctype;
		return ctype;
	}

	dmrC_expression_error(C, expr, "bad argument type for ++/--");
	return NULL;
}

static struct symbol *evaluate_sign(struct dmr_C *C, struct expression *expr)
{
	struct symbol *ctype = expr->unop->ctype;
	int klass = classify_type(C, ctype, &ctype);
	if (expr->flags && !(expr->unop->flags & Int_const_expr))
		expr->flags = 0;
	/* should be an arithmetic type */
	if (!(klass & TYPE_NUM))
		return bad_expr_type(C, expr);
	if (klass & TYPE_RESTRICT)
		goto Restr;
Normal:
	if (!(klass & TYPE_FLOAT)) {
		ctype = integer_promotion(C, ctype);
		expr->unop = cast_to(C, expr->unop, ctype);
	} else if (expr->op != '~') {
		/* no conversions needed */
	} else {
		return bad_expr_type(C, expr);
	}
	if (expr->op == '+')
		*expr = *expr->unop;
	expr->ctype = ctype;
	return ctype;
Restr:
	if (restricted_unop(C, expr->op, &ctype))
		unrestrict(C, expr, klass, &ctype);
	goto Normal;
}

static struct symbol *evaluate_preop(struct dmr_C *C, struct expression *expr)
{
	struct symbol *ctype = expr->unop->ctype;

	switch (expr->op) {
	case '(':
		*expr = *expr->unop;
		return ctype;

	case '+':
	case '-':
	case '~':
		return evaluate_sign(C, expr);

	case '*':
		return evaluate_dereference(C, expr);

	case '&':
		return evaluate_addressof(C, expr);

	case SPECIAL_INCREMENT:
	case SPECIAL_DECREMENT:
		/*
		 * From a type evaluation standpoint the preops are
		 * the same as the postops
		 */
		return evaluate_postop(C, expr);

	case '!':
		if (expr->flags && !(expr->unop->flags & Int_const_expr))
			expr->flags = 0;
		if (is_safe_type(ctype))
			dmrC_warning(C, expr->pos, "testing a 'safe expression'");
		if (dmrC_is_float_type(C->S, ctype)) {
			struct expression *arg = expr->unop;
			expr->type = EXPR_COMPARE;
			expr->op = SPECIAL_EQUAL;
			expr->left = arg;
			expr->right = dmrC_alloc_expression(C, expr->pos, EXPR_FVALUE);
			expr->right->ctype = ctype;
			expr->right->fvalue = 0;
		} else if (dmrC_is_fouled_type(ctype)) {
			dmrC_warning(C, expr->pos, "%s degrades to integer",
				dmrC_show_typename(C, ctype->ctype.base_type));
		}
		/* the result is int [6.5.3.3(5)]*/
		ctype = &C->S->int_ctype;
		break;

	default:
		break;
	}
	expr->ctype = ctype;
	return ctype;
}

static struct symbol *find_identifier(struct dmr_C *C, struct ident *ident, struct symbol_list *_list, int *offset)
{
	struct ptr_list *head = (struct ptr_list *)_list;
	struct ptr_list *list = head;

	if (!head)
		return NULL;
	do {
		int i;
		for (i = 0; i < list->nr_; i++) {
			struct symbol *sym = (struct symbol *) list->list_[i];
			if (sym->ident) {
				if (sym->ident != ident)
					continue;
				*offset = sym->offset;
				return sym;
			} else {
				struct symbol *ctype = sym->ctype.base_type;
				struct symbol *sub;
				if (!ctype)
					continue;
				if (ctype->type != SYM_UNION && ctype->type != SYM_STRUCT)
					continue;
				sub = find_identifier(C, ident, ctype->symbol_list, offset);
				if (!sub)
					continue;
				*offset += sym->offset;
				return sub;
			}	
		}
	} while ((list = list->next_) != head);
	return NULL;
}

static struct expression *evaluate_offset(struct dmr_C *C, struct expression *expr, unsigned long offset)
{
	struct expression *add;

	/*
	 * Create a new add-expression
	 *
	 * NOTE! Even if we just add zero, we need a new node
	 * for the member pointer, since it has a different
	 * type than the original pointer. We could make that
	 * be just a cast, but the fact is, a node is a node,
	 * so we might as well just do the "add zero" here.
	 */
	add = dmrC_alloc_expression(C, expr->pos, EXPR_BINOP);
	add->op = '+';
	add->left = expr;
	add->right = dmrC_alloc_expression(C, expr->pos, EXPR_VALUE);
	add->right->ctype = &C->S->int_ctype;
	add->right->value = offset;

	/*
	 * The ctype of the pointer will be lazily evaluated if
	 * we ever take the address of this member dereference..
	 */
	add->ctype = &C->S->lazy_ptr_ctype;
	return add;
}

/* structure/union dereference */
static struct symbol *evaluate_member_dereference(struct dmr_C *C, struct expression *expr)
{
	int offset;
	struct symbol *ctype, *member;
	struct expression *deref = expr->deref, *add;
	struct ident *ident = expr->member;
	unsigned int mod;
	int address_space;

	if (!dmrC_evaluate_expression(C, deref))
		return NULL;
	if (!ident) {
		dmrC_expression_error(C, expr, "bad member name");
		return NULL;
	}

	ctype = deref->ctype;
	dmrC_examine_symbol_type(C->S, ctype);
	address_space = ctype->ctype.as;
	mod = ctype->ctype.modifiers;
	if (ctype->type == SYM_NODE) {
		ctype = ctype->ctype.base_type;
		address_space |= ctype->ctype.as;
		mod |= ctype->ctype.modifiers;
	}
	if (!ctype || (ctype->type != SYM_STRUCT && ctype->type != SYM_UNION)) {
		dmrC_expression_error(C, expr, "expected structure or union");
		return NULL;
	}
	offset = 0;
	member = find_identifier(C, ident, ctype->symbol_list, &offset);
	if (!member) {
		const char *type = ctype->type == SYM_STRUCT ? "struct" : "union";
		const char *name = "<unnamed>";
		int namelen = 9;
		if (ctype->ident) {
			name = ctype->ident->name;
			namelen = ctype->ident->len;
		}
		if (ctype->symbol_list)
			dmrC_expression_error(C, expr, "no member '%s' in %s %.*s",
				dmrC_show_ident(C, ident), type, namelen, name);
		else
			dmrC_expression_error(C, expr, "using member '%s' in "
				"incomplete %s %.*s", dmrC_show_ident(C, ident),
				type, namelen, name);
		return NULL;
	}

	/*
	 * The member needs to take on the address space and modifiers of
	 * the "parent" type.
	 */
	member = convert_to_as_mod(C, member, address_space, mod);
	ctype = dmrC_get_base_type(C->S, member);

	if (!lvalue_expression(C, deref)) {
		if (deref->type != EXPR_SLICE) {
			expr->base = deref;
			expr->r_bitpos = 0;
		} else {
			expr->base = deref->base;
			expr->r_bitpos = deref->r_bitpos;
		}
		expr->r_bitpos += dmrC_bytes_to_bits(C->target, offset);
		expr->type = EXPR_SLICE;
		expr->r_nrbits = member->bit_size;
		expr->r_bitpos += member->bit_offset;
		expr->ctype = member;
		return member;
	}

	deref = deref->unop;
	expr->deref = deref;

	add = evaluate_offset(C, deref, offset);
	expr->type = EXPR_PREOP;
	expr->op = '*';
	expr->unop = add;

	expr->ctype = member;
	return member;
}

static int is_promoted(struct dmr_C *C, struct expression *expr)
{
        (void) C;
	while (1) {
		switch (expr->type) {
		case EXPR_BINOP:
		case EXPR_SELECT:
		case EXPR_CONDITIONAL:
			return 1;
		case EXPR_COMMA:
			expr = expr->right;
			continue;
		case EXPR_PREOP:
			switch (expr->op) {
			case '(':
				expr = expr->unop;
				continue;
			case '+':
			case '-':
			case '~':
				return 1;
			default:
				return 0;
			}
		default:
			return 0;
		}
	}
}


static struct symbol *evaluate_cast(struct dmr_C *C, struct expression *);

static struct symbol *evaluate_type_information(struct dmr_C *C, struct expression *expr)
{
	struct symbol *sym = expr->cast_type;
	if (!sym) {
		sym = dmrC_evaluate_expression(C, expr->cast_expression);
		if (!sym)
			return NULL;
		/*
		 * Expressions of restricted types will possibly get
		 * promoted - check that here
		 */
		if (dmrC_is_restricted_type(sym)) {
			if (sym->bit_size < C->target->bits_in_int && is_promoted(C, expr))
				sym = &C->S->int_ctype;
		} else if (dmrC_is_fouled_type(sym)) {
			sym = &C->S->int_ctype;
		}
	}
	dmrC_examine_symbol_type(C->S, sym);
	if (dmrC_is_bitfield_type(sym)) {
		dmrC_expression_error(C, expr, "trying to examine bitfield type");
		return NULL;
	}
	return sym;
}

static struct symbol *evaluate_sizeof(struct dmr_C *C, struct expression *expr)
{
	struct symbol *type;
	int size;

	type = evaluate_type_information(C, expr);
	if (!type)
		return NULL;

	size = type->bit_size;

	if (size < 0 && dmrC_is_void_type(C->S, type)) {
		dmrC_warning(C, expr->pos, "expression using sizeof(void)");
		size = C->target->bits_in_char;
	}

	if (size == 1 && dmrC_is_bool_type(C->S, type)) {
		if (C->Wsizeof_bool)
			dmrC_warning(C, expr->pos, "expression using sizeof bool");
		size = C->target->bits_in_char;
	}

	if (dmrC_is_function(type->ctype.base_type)) {
		dmrC_warning(C, expr->pos, "expression using sizeof on a function");
		size = C->target->bits_in_char;
	}

	if ((size < 0) || (size & (C->target->bits_in_char - 1)))
		dmrC_expression_error(C, expr, "cannot size expression");

	expr->type = EXPR_VALUE;
	expr->value = dmrC_bits_to_bytes(C->target, size);
	expr->taint = 0;
	expr->ctype = C->target->size_t_ctype;
	return C->target->size_t_ctype;
}

static struct symbol *evaluate_ptrsizeof(struct dmr_C *C, struct expression *expr)
{
	struct symbol *type;
	int size;

	type = evaluate_type_information(C, expr);
	if (!type)
		return NULL;

	if (type->type == SYM_NODE)
		type = type->ctype.base_type;
	if (!type)
		return NULL;
	switch (type->type) {
	case SYM_ARRAY:
		break;
	case SYM_PTR:
		type = dmrC_get_base_type(C->S, type);
		if (type)
			break;
	default:
		dmrC_expression_error(C, expr, "expected pointer expression");
		return NULL;
	}
	size = type->bit_size;
	if (size & (C->target->bits_in_char-1))
		size = 0;
	expr->type = EXPR_VALUE;
	expr->value = dmrC_bits_to_bytes(C->target, size);
	expr->taint = 0;
	expr->ctype = C->target->size_t_ctype;
	return C->target->size_t_ctype;
}

static struct symbol *evaluate_alignof(struct dmr_C *C, struct expression *expr)
{
	struct symbol *type;

	type = evaluate_type_information(C, expr);
	if (!type)
		return NULL;

	expr->type = EXPR_VALUE;
	expr->value = type->ctype.alignment;
	expr->taint = 0;
	expr->ctype = C->target->size_t_ctype;
	return C->target->size_t_ctype;
}

static int evaluate_arguments(struct dmr_C *C, struct symbol *fn, struct expression_list *head)
{
	struct expression *expr;
	struct symbol_list *argument_types = fn->arguments;
	struct symbol *argtype;
	int i = 1;

	PREPARE_PTR_LIST(argument_types, argtype);
	FOR_EACH_PTR (head, expr) {
		struct expression **p = THIS_ADDRESS(struct expression*, expr);
		struct symbol *ctype, *target;
		ctype = dmrC_evaluate_expression(C, expr);

		if (!ctype)
			return 0;

		target = argtype;
		if (!target) {
			struct symbol *type;
			int klass = classify_type(C, ctype, &type);
			if (is_int(klass)) {
				*p = cast_to(C, expr, integer_promotion(C, type));
			} else if (klass & TYPE_FLOAT) {
				unsigned long mod = type->ctype.modifiers;
				if (!(mod & (MOD_LONG_ALL)))
					*p = cast_to(C, expr, &C->S->double_ctype);
			} else if (klass & TYPE_PTR) {
				if (expr->ctype == &C->S->null_ctype)
					*p = cast_to(C, expr, &C->S->ptr_ctype);
				else
					degenerate(C, expr);
			}
		} else if (!target->forced_arg){
			/// FIXME
			static char where[30];
			dmrC_examine_symbol_type(C->S, target);
			sprintf(where, "argument %d", i);
			compatible_argument_type(C, expr, target, p, where);
		}

		i++;
		NEXT_PTR_LIST(argtype);
	} END_FOR_EACH_PTR(expr);
	FINISH_PTR_LIST(argtype);
	return 1;
}

static void convert_index(struct dmr_C *C, struct expression *e)
{
	struct expression *child = e->idx_expression;
	unsigned from = e->idx_from;
	unsigned to = e->idx_to + 1;
	e->type = EXPR_POS;
	e->init_offset = from * dmrC_bits_to_bytes(C->target, e->ctype->bit_size);
	e->init_nr = to - from;
	e->init_expr = child;
}

static void convert_ident(struct dmr_C *C, struct expression *e)
{
        (void) C;
	struct expression *child = e->ident_expression;
	int offset = e->offset;

	e->type = EXPR_POS;
	e->init_offset = offset;
	e->init_nr = 1;
	e->init_expr = child;
}

static void convert_designators(struct dmr_C *C, struct expression *e)
{
	while (e) {
		if (e->type == EXPR_INDEX)
			convert_index(C, e);
		else if (e->type == EXPR_IDENTIFIER)
			convert_ident(C, e);
		else
			break;
		e = e->init_expr;
	}
}

static void excess(struct dmr_C *C, struct expression *e, const char *s)
{
	dmrC_warning(C, e->pos, "excessive elements in %s initializer", s);
}

/*
 * implicit designator for the first element
 */
static struct expression *first_subobject(struct dmr_C *C, struct symbol *ctype, int klass,
					  struct expression **v)
{
	struct expression *e = *v, *newe;

	if (ctype->type == SYM_NODE)
		ctype = ctype->ctype.base_type;

	if (klass & TYPE_PTR) { /* array */
		if (!ctype->bit_size)
			return NULL;
		newe = dmrC_alloc_expression(C, e->pos, EXPR_INDEX);
		newe->idx_expression = e;
		newe->ctype = ctype->ctype.base_type;
	} else  {
		struct symbol *field, *p;
		PREPARE_PTR_LIST(ctype->symbol_list, p);
		while (p && !p->ident && dmrC_is_bitfield_type(p))
			NEXT_PTR_LIST(p);
		field = p;
		FINISH_PTR_LIST(p);
		if (!field)
			return NULL;
		newe = dmrC_alloc_expression(C, e->pos, EXPR_IDENTIFIER);
		newe->ident_expression = e;
		newe->field = newe->ctype = field;
		newe->offset = field->offset;
	}
	*v = newe;
	return newe;
}

/*
 * sanity-check explicit designators; return the innermost one or NULL
 * in case of error.  Assign types.
 */
static struct expression *check_designators(struct dmr_C *C, struct expression *e,
					    struct symbol *ctype)
{
	struct expression *last = NULL;
	const char *err;
	while (1) {
		if (ctype->type == SYM_NODE)
			ctype = ctype->ctype.base_type;
		if (e->type == EXPR_INDEX) {
			struct symbol *type;
			if (ctype->type != SYM_ARRAY) {
				err = "array index in non-array";
				break;
			}
			type = ctype->ctype.base_type;
			if (ctype->bit_size >= 0 && type->bit_size >= 0) {
				unsigned offset = (unsigned) dmrC_array_element_offset(C->target, type->bit_size, e->idx_to);
				if (offset >= (unsigned) (ctype->bit_size)) {
					err = "index out of bounds in";
					break;
				}
			}
			e->ctype = ctype = type;
			ctype = type;
			last = e;
			if (!e->idx_expression) {
				err = "invalid";
				break;
			}
			e = e->idx_expression;
		} else if (e->type == EXPR_IDENTIFIER) {
			int offset = 0;
			if (ctype->type != SYM_STRUCT && ctype->type != SYM_UNION) {
				err = "field name not in struct or union";
				break;
			}
			ctype = find_identifier(C, e->expr_ident, ctype->symbol_list, &offset);
			if (!ctype) {
				err = "unknown field name in";
				break;
			}
			e->offset = offset;
			e->field = e->ctype = ctype;
			last = e;
			if (!e->ident_expression) {
				err = "invalid";
				break;
			}
			e = e->ident_expression;
		} else if (e->type == EXPR_POS) {
			err = "internal front-end error: EXPR_POS in";
			break;
		} else
			return last;
	}
	dmrC_expression_error(C, e, "%s initializer", err);
	return NULL;
}

/*
 * choose the next subobject to initialize.
 *
 * Get designators for next element, switch old ones to EXPR_POS.
 * Return the resulting expression or NULL if we'd run out of subobjects.
 * The innermost designator is returned in *v.  Designators in old
 * are assumed to be already sanity-checked.
 */
static struct expression *next_designators(struct dmr_C *C, struct expression *old,
			     struct symbol *ctype,
			     struct expression *e, struct expression **v)
{
	struct expression *newe = NULL;

	if (!old)
		return NULL;
	if (old->type == EXPR_INDEX) {
		struct expression *copy;
		unsigned n;

		copy = next_designators(C, old->idx_expression,
					old->ctype, e, v);
		if (!copy) {
			n = old->idx_to + 1;
			if ((int)dmrC_array_element_offset(C->target, old->ctype->bit_size, n) == ctype->bit_size) {
				convert_index(C, old);
				return NULL;
			}
			copy = e;
			*v = newe = dmrC_alloc_expression(C, e->pos, EXPR_INDEX);
		} else {
			n = old->idx_to;
			newe = dmrC_alloc_expression(C, e->pos, EXPR_INDEX);
		}

		newe->idx_from = newe->idx_to = n;
		newe->idx_expression = copy;
		newe->ctype = old->ctype;
		convert_index(C, old);
	} else if (old->type == EXPR_IDENTIFIER) {
		struct expression *copy;
		struct symbol *field;
		int offset = 0;

		copy = next_designators(C, old->ident_expression,
					old->ctype, e, v);
		if (!copy) {
			field = old->field->next_subobject;
			if (!field) {
				convert_ident(C, old);
				return NULL;
			}
			copy = e;
			*v = newe = dmrC_alloc_expression(C, e->pos, EXPR_IDENTIFIER);
			/*
			 * We can't necessarily trust "field->offset",
			 * because the field might be in an anonymous
			 * union, and the field offset is then the offset
			 * within that union.
			 *
			 * The "old->offset - old->field->offset"
			 * would be the offset of such an anonymous
			 * union.
			 */
			offset = old->offset - old->field->offset;
		} else {
			field = old->field;
			newe = dmrC_alloc_expression(C, e->pos, EXPR_IDENTIFIER);
		}

		newe->field = field;
		newe->expr_ident = field->ident;
		newe->ident_expression = copy;
		newe->ctype = field;
		newe->offset = field->offset + offset;
		convert_ident(C, old);
	}
	return newe;
}

static int handle_simple_initializer(struct dmr_C *C, struct expression **ep, int nested,
				     int klass, struct symbol *ctype);

/*
 * deal with traversing subobjects [6.7.8(17,18,20)]
 */
static void handle_list_initializer(struct dmr_C *C, struct expression *expr,
				    int klass, struct symbol *ctype)
{
	struct expression *e, *last = NULL, *top = NULL, *next;
	int jumped = 0;

	FOR_EACH_PTR(expr->expr_list, e) {
		struct expression **v;
		struct symbol *type;
		int lclass;

		if (e->type != EXPR_INDEX && e->type != EXPR_IDENTIFIER) {
			struct symbol *struct_sym;
			if (!top) {
				top = e;
				last = first_subobject(C, ctype, klass, &top);
			} else {
				last = next_designators(C, last, ctype, e, &top);
			}
			if (!last) {
				excess(C, e, klass & TYPE_PTR ? "array" :
							"struct or union");
				DELETE_CURRENT_PTR(e);
				continue;
			}
			struct_sym = ctype->type == SYM_NODE ? ctype->ctype.base_type : ctype;
			if (C->Wdesignated_init && struct_sym->designated_init)
				dmrC_warning(C, e->pos, "%s%.*s%spositional init of field in %s %s, declared with attribute designated_init",
					ctype->ident ? "in initializer for " : "",
					ctype->ident ? ctype->ident->len : 0,
					ctype->ident ? ctype->ident->name : "",
					ctype->ident ? ": " : "",
					dmrC_get_type_name(struct_sym->type),
					dmrC_show_ident(C, struct_sym->ident));
			if (jumped) {
				dmrC_warning(C, e->pos, "advancing past deep designator");
				jumped = 0;
			}
			REPLACE_CURRENT_PTR(struct expression *, e, last);
		} else {
			next = check_designators(C, e, ctype);
			if (!next) {
				DELETE_CURRENT_PTR(e);
				continue;
			}
			top = next;
			/* deeper than one designator? */
			jumped = top != e;
			convert_designators(C, last);
			last = e;
		}

found:
		lclass = classify_type(C, top->ctype, &type);
		if (top->type == EXPR_INDEX)
			v = &top->idx_expression;
		else
			v = &top->ident_expression;

		if (handle_simple_initializer(C, v, 1, lclass, top->ctype))
			continue;

		if (!(lclass & TYPE_COMPOUND)) {
			dmrC_warning(C, e->pos, "bogus scalar initializer");
			DELETE_CURRENT_PTR(e);
			continue;
		}

		next = first_subobject(C, type, lclass, v);
		if (next) {
			dmrC_warning(C, e->pos, "missing braces around initializer");
			top = next;
			goto found;
		}

		DELETE_CURRENT_PTR(e);
		excess(C, e, lclass & TYPE_PTR ? "array" : "struct or union");

	} END_FOR_EACH_PTR(e);

	convert_designators(C, last);
	expr->ctype = ctype;
}

static int is_string_literal(struct dmr_C *C, struct expression **v)
{
	struct expression *e = *v;
	while (e && e->type == EXPR_PREOP && e->op == '(')
		e = e->unop;
	if (!e || e->type != EXPR_STRING)
		return 0;
	if (e != *v && C->Wparen_string)
		dmrC_warning(C, e->pos,
			"array initialized from parenthesized string constant");
	*v = e;
	return 1;
}

/*
 * We want a normal expression, possibly in one layer of braces.  Warn
 * if the latter happens inside a list (it's legal, but likely to be
 * an effect of screwup).  In case of anything not legal, we are definitely
 * having an effect of screwup, so just fail and let the caller warn.
 */
static struct expression *handle_scalar(struct dmr_C *C, struct expression *e, int nested)
{
	struct expression *v = NULL, *p;
	int count = 0;

	/* normal case */
	if (e->type != EXPR_INITIALIZER)
		return e;

	FOR_EACH_PTR(e->expr_list, p) {
		if (!v)
			v = p;
		count++;
	} END_FOR_EACH_PTR(p);
	if (count != 1)
		return NULL;
	switch(v->type) {
	case EXPR_INITIALIZER:
	case EXPR_INDEX:
	case EXPR_IDENTIFIER:
		return NULL;
	default:
		break;
	}
	if (nested)
		dmrC_warning(C, e->pos, "braces around scalar initializer");
	return v;
}

/*
 * deal with the cases that don't care about subobjects:
 * scalar <- assignment expression, possibly in braces [6.7.8(11)]
 * character array <- string literal, possibly in braces [6.7.8(14)]
 * struct or union <- assignment expression of compatible type [6.7.8(13)]
 * compound type <- initializer list in braces [6.7.8(16)]
 * The last one punts to handle_list_initializer() which, in turn will call
 * us for individual elements of the list.
 *
 * We do not handle 6.7.8(15) (wide char array <- wide string literal) for
 * the lack of support of wide char stuff in general.
 *
 * One note: we need to take care not to evaluate a string literal until
 * we know that we *will* handle it right here.  Otherwise we would screw
 * the cases like struct { struct {char s[10]; ...} ...} initialized with
 * { "string", ...} - we need to preserve that string literal recognizable
 * until we dig into the inner struct.
 */
static int handle_simple_initializer(struct dmr_C *C, struct expression **ep, int nested,
				     int klass, struct symbol *ctype)
{
	int is_string = is_string_type(C, ctype);
	struct expression *e = *ep, *p;
	struct symbol *type;

	if (!e)
		return 0;

	/* scalar */
	if (!(klass & TYPE_COMPOUND)) {
		e = handle_scalar(C, e, nested);
		if (!e)
			return 0;
		*ep = e;
		if (!dmrC_evaluate_expression(C, e))
			return 1;
		compatible_assignment_types(C, e, ctype, ep, "initializer");
		return 1;
	}

	/*
	 * sublist; either a string, or we dig in; the latter will deal with
	 * pathologies, so we don't need anything fancy here.
	 */
	if (e->type == EXPR_INITIALIZER) {
		if (is_string) {
			struct expression *v = NULL;
			int count = 0;

			FOR_EACH_PTR(e->expr_list, p) {
				if (!v)
					v = p;
				count++;
			} END_FOR_EACH_PTR(p);
			if (count == 1 && is_string_literal(C, &v)) {
				*ep = e = v;
				goto String;
			}
		}
		handle_list_initializer(C, e, klass, ctype);
		return 1;
	}

	/* string */
	if (is_string_literal(C, &e)) {
		/* either we are doing array of char, or we'll have to dig in */
		if (is_string) {
			*ep = e;
			goto String;
		}
		return 0;
	}
	/* struct or union can be initialized by compatible */
	if (klass != TYPE_COMPOUND)
		return 0;
	type = dmrC_evaluate_expression(C, e);
	if (!type)
		return 0;
	if (ctype->type == SYM_NODE)
		ctype = ctype->ctype.base_type;
	if (type->type == SYM_NODE)
		type = type->ctype.base_type;
	if (ctype == type)
		return 1;
	return 0;

String:
	p = dmrC_alloc_expression(C, e->pos, EXPR_STRING);
	*p = *e;
	type = dmrC_evaluate_expression(C, p);
	if (ctype->bit_size != -1) {
		if (ctype->bit_size + C->target->bits_in_char < type->bit_size)
			dmrC_warning(C, e->pos,
				"too long initializer-string for array of char");
		else if (C->Winit_cstring && ctype->bit_size + C->target->bits_in_char == type->bit_size) {
			dmrC_warning(C, e->pos,
				"too long initializer-string for array of char(no space for nul char)");
		}
	}
	*ep = p;
	return 1;
}

static void evaluate_initializer(struct dmr_C *C, struct symbol *ctype, struct expression **ep)
{
	struct symbol *type;
	int klass = classify_type(C, ctype, &type);
	if (!handle_simple_initializer(C, ep, 0, klass, ctype))
		dmrC_expression_error(C, *ep, "invalid initializer");
}

static struct symbol *cast_to_bool(struct dmr_C *C, struct expression *expr)
{
	struct expression *old = expr->cast_expression;
	struct expression *zero;
	struct symbol *otype;
	int oclass = classify_type(C, degenerate(C, old), &otype);
	struct symbol *ctype;

	if (oclass & TYPE_COMPOUND)
		return NULL;

	zero = dmrC_alloc_const_expression(C, expr->pos, 0);
	expr->op = SPECIAL_NOTEQUAL;
	ctype = usual_conversions(C, expr->op, old, zero,
			oclass, TYPE_NUM, otype, zero->ctype);
	expr->type = EXPR_COMPARE;
	expr->left = cast_to(C, old, ctype);
	expr->right = cast_to(C, zero, ctype);

	return expr->ctype;
}
static struct symbol *evaluate_cast(struct dmr_C *C, struct expression *expr)
{
	struct expression *target = expr->cast_expression;
	struct symbol *ctype;
	struct symbol *t1, *t2;
	int class1, class2;
	int as1 = 0, as2 = 0;

	if (!target)
		return NULL;

	/*
	 * Special case: a cast can be followed by an
	 * initializer, in which case we need to pass
	 * the type value down to that initializer rather
	 * than trying to evaluate it as an expression
	 *
	 * A more complex case is when the initializer is
	 * dereferenced as part of a post-fix expression.
	 * We need to produce an expression that can be dereferenced.
	 */
	if (target->type == EXPR_INITIALIZER) {
		struct symbol *sym = expr->cast_type;
		struct expression *addr = dmrC_alloc_expression(C, expr->pos, EXPR_SYMBOL);

		sym->initializer = target;
		evaluate_symbol(C, sym);

		addr->ctype = &C->S->lazy_ptr_ctype;	/* Lazy eval */
		addr->symbol = sym;

		expr->type = EXPR_PREOP;
		expr->op = '*';
		expr->unop = addr;
		expr->ctype = sym;

		return sym;
	}

	ctype = dmrC_examine_symbol_type(C->S, expr->cast_type);
	expr->ctype = ctype;
	expr->cast_type = ctype;

	dmrC_evaluate_expression(C, target);
	degenerate(C, target);

	class1 = classify_type(C, ctype, &t1);

	/* cast to non-integer type -> not an integer constant expression */
	if (!is_int(class1))
		expr->flags = 0;
	/* if argument turns out to be not an integer constant expression *and*
	   it was not a floating literal to start with -> too bad */
	else if (expr->flags == Int_const_expr &&
		!(target->flags & Int_const_expr))
		expr->flags = 0;
	/*
	 * You can always throw a value away by casting to
	 * "void" - that's an implicit "force". Note that
	 * the same is _not_ true of "void *".
	 */
	if (t1 == &C->S->void_ctype)
		goto out;

	if (class1 & (TYPE_COMPOUND | TYPE_FN))
		dmrC_warning(C, expr->pos, "cast to non-scalar");

	t2 = target->ctype;
	if (!t2) {
		dmrC_expression_error(C, expr, "cast from unknown type");
		goto out;
	}
	class2 = classify_type(C, t2, &t2);

	if (class2 & TYPE_COMPOUND)
		dmrC_warning(C, expr->pos, "cast from non-scalar");

	if (expr->type == EXPR_FORCE_CAST)
		goto out;

	/* allowed cast unfouls */
	if (class2 & TYPE_FOULED)
		t2 = unfoul(t2);

	if (t1 != t2) {
		if ((class1 & TYPE_RESTRICT) && restricted_value(target, t1))
			dmrC_warning(C, expr->pos, "cast to %s",
				dmrC_show_typename(C, t1));
		if (class2 & TYPE_RESTRICT) {
			if (t1 == &C->S->bool_ctype) {
				if (class2 & TYPE_FOULED)
					dmrC_warning(C, expr->pos, "%s degrades to integer",
						dmrC_show_typename(C, t2));
			}
			else {
				dmrC_warning(C, expr->pos, "cast from %s",
					dmrC_show_typename(C, t2));
			}
		}
	}

	if (t1 == &C->S->ulong_ctype)
		as1 = -1;
	else if (class1 == TYPE_PTR) {
		dmrC_examine_pointer_target(C->S, t1);
		as1 = t1->ctype.as;
	}

	if (t2 == &C->S->ulong_ctype)
		as2 = -1;
	else if (class2 == TYPE_PTR) {
		dmrC_examine_pointer_target(C->S, t2);
		as2 = t2->ctype.as;
	}

	if (!as1 && as2 > 0)
		dmrC_warning(C, expr->pos, "cast removes address space of expression");
	if (as1 > 0 && as2 > 0 && as1 != as2)
		dmrC_warning(C, expr->pos, "cast between address spaces (<asn:%d>-><asn:%d>)", as2, as1);
	if (as1 > 0 && !as2 &&
	    !is_null_pointer_constant(C, target) && C->Wcast_to_as)
		dmrC_warning(C, expr->pos,
			"cast adds address space to expression (<asn:%d>)", as1);

	if (!(t1->ctype.modifiers & MOD_PTRINHERIT) && class1 == TYPE_PTR &&
	    !as1 && (target->flags & Int_const_expr)) {
		if (t1->ctype.base_type == &C->S->void_ctype) {
			if (dmrC_is_zero_constant(C, target)) {
				/* NULL */
				expr->type = EXPR_VALUE;
				expr->ctype = &C->S->null_ctype;
				expr->value = 0;
				return expr->ctype;
			}
		}
	}

	if (t1 == &C->S->bool_ctype)
		cast_to_bool(C, expr);

out:
	return ctype;
}

/*
 * Evaluate a call expression with a symbol. This
 * should expand inline functions, and evaluate
 * builtins.
 */
static int evaluate_symbol_call(struct dmr_C *C, struct expression *expr)
{
	struct expression *fn = expr->fn;
	struct symbol *ctype = fn->ctype;

	if (fn->type != EXPR_PREOP)
		return 0;

	if (ctype->op && ctype->op->evaluate)
		return ctype->op->evaluate(C, expr);

	if (ctype->ctype.modifiers & MOD_INLINE) {
		int ret;
		struct symbol *curr = C->current_fn;

		if (ctype->definition)
			ctype = ctype->definition;

		C->current_fn = ctype->ctype.base_type;

		ret = dmrC_inline_function(C, expr, ctype);

		/* restore the old function */
		C->current_fn = curr;
		return ret;
	}

	return 0;
}

static struct symbol *evaluate_call(struct dmr_C *C, struct expression *expr)
{
	int args, fnargs;
	struct symbol *ctype, *sym;
	struct expression *fn = expr->fn;
	struct expression_list *arglist = expr->args;

	if (!dmrC_evaluate_expression(C, fn))
		return NULL;
	sym = ctype = fn->ctype;
	if (ctype->type == SYM_NODE)
		ctype = ctype->ctype.base_type;
	if (ctype->type == SYM_PTR)
		ctype = dmrC_get_base_type(C->S, ctype);

	if (ctype->type != SYM_FN) {
		struct expression *arg;
		dmrC_expression_error(C, expr, "not a function %s",
			     dmrC_show_ident(C, sym->ident));
		/* do typechecking in arguments */
		FOR_EACH_PTR (arglist, arg) {
			dmrC_evaluate_expression(C, arg);
		} END_FOR_EACH_PTR(arg);
		return NULL;
	}

	examine_fn_arguments(C, ctype);
	if (sym->type == SYM_NODE && fn->type == EXPR_PREOP &&
	    sym->op && sym->op->args) {
		if (!sym->op->args(C, expr))
			return NULL;
	} else {
		if (!evaluate_arguments(C, ctype, arglist))
			return NULL;
		args = dmrC_expression_list_size(expr->args);
		fnargs = dmrC_symbol_list_size(ctype->arguments);
		if (args < fnargs)
			dmrC_expression_error(C, expr,
				     "not enough arguments for function %s",
				     dmrC_show_ident(C, sym->ident));
		if (args > fnargs && !ctype->variadic)
			dmrC_expression_error(C, expr,
				     "too many arguments for function %s",
				     dmrC_show_ident(C, sym->ident));
	}
	if (sym->type == SYM_NODE) {
		if (evaluate_symbol_call(C, expr))
			return expr->ctype;
	}
	expr->ctype = ctype->ctype.base_type;
	return expr->ctype;
}

static struct symbol *evaluate_offsetof(struct dmr_C *C, struct expression *expr)
{
	struct expression *e = expr->down;
	struct symbol *ctype = expr->in;
	int klass;

	if (expr->op == '.') {
		struct symbol *field;
		int offset = 0;
		if (!ctype) {
			dmrC_expression_error(C, expr, "expected structure or union");
			return NULL;
		}
		dmrC_examine_symbol_type(C->S, ctype);
		klass = classify_type(C, ctype, &ctype);
		if (klass != TYPE_COMPOUND) {
			dmrC_expression_error(C, expr, "expected structure or union");
			return NULL;
		}

		field = find_identifier(C, expr->ident, ctype->symbol_list, &offset);
		if (!field) {
			dmrC_expression_error(C, expr, "unknown member");
			return NULL;
		}
		ctype = field;
		expr->type = EXPR_VALUE;
		expr->flags = Int_const_expr;
		expr->value = offset;
		expr->taint = 0;
		expr->ctype = C->target->size_t_ctype;
	} else {
		if (!ctype) {
			dmrC_expression_error(C, expr, "expected structure or union");
			return NULL;
		}
		dmrC_examine_symbol_type(C->S, ctype);
		klass = classify_type(C, ctype, &ctype);
		if (klass != (TYPE_COMPOUND | TYPE_PTR)) {
			dmrC_expression_error(C, expr, "expected array");
			return NULL;
		}
		ctype = ctype->ctype.base_type;
		if (!expr->index) {
			expr->type = EXPR_VALUE;
			expr->flags = Int_const_expr;
			expr->value = 0;
			expr->taint = 0;
			expr->ctype = C->target->size_t_ctype;
		} else {
			struct expression *idx = expr->index, *m;
			struct symbol *i_type = dmrC_evaluate_expression(C, idx);
			int i_class = classify_type(C, i_type, &i_type);
			if (!is_int(i_class)) {
				dmrC_expression_error(C, expr, "non-integer index");
				return NULL;
			}
			unrestrict(C, idx, i_class, &i_type);
			idx = cast_to(C, idx, C->target->size_t_ctype);
			m = dmrC_alloc_const_expression(C, expr->pos,
						   dmrC_bits_to_bytes(C->target, ctype->bit_size));
			m->ctype = C->target->size_t_ctype;
			m->flags = Int_const_expr;
			expr->type = EXPR_BINOP;
			expr->left = idx;
			expr->right = m;
			expr->op = '*';
			expr->ctype = C->target->size_t_ctype;
			expr->flags = m->flags & idx->flags & Int_const_expr;
		}
	}
	if (e) {
		struct expression *copy = (struct expression *)dmrC_allocator_allocate(&C->expression_allocator, 0);
		*copy = *expr;
		if (e->type == EXPR_OFFSETOF)
			e->in = ctype;
		if (!dmrC_evaluate_expression(C, e))
			return NULL;
		expr->type = EXPR_BINOP;
		expr->flags = e->flags & copy->flags & Int_const_expr;
		expr->op = '+';
		expr->ctype = C->target->size_t_ctype;
		expr->left = copy;
		expr->right = e;
	}
	return C->target->size_t_ctype;
}

struct symbol *dmrC_evaluate_expression(struct dmr_C *C, struct expression *expr)
{
	if (!expr)
		return NULL;
	if (expr->ctype)
		return expr->ctype;

	switch (expr->type) {
	case EXPR_VALUE:
	case EXPR_FVALUE:
		dmrC_expression_error(C, expr, "value expression without a type");
		return NULL;
	case EXPR_STRING:
		return evaluate_string(C, expr);
	case EXPR_SYMBOL:
		return evaluate_symbol_expression(C, expr);
	case EXPR_BINOP:
		if (!dmrC_evaluate_expression(C, expr->left))
			return NULL;
		if (!dmrC_evaluate_expression(C, expr->right))
			return NULL;
		return evaluate_binop(C, expr);
	case EXPR_LOGICAL:
		return evaluate_logical(C, expr);
	case EXPR_COMMA:
		dmrC_evaluate_expression(C, expr->left);
		if (!dmrC_evaluate_expression(C, expr->right))
			return NULL;
		return evaluate_comma(C, expr);
	case EXPR_COMPARE:
		if (!dmrC_evaluate_expression(C, expr->left))
			return NULL;
		if (!dmrC_evaluate_expression(C, expr->right))
			return NULL;
		return evaluate_compare(C, expr);
	case EXPR_ASSIGNMENT:
		if (!dmrC_evaluate_expression(C, expr->left))
			return NULL;
		if (!dmrC_evaluate_expression(C, expr->right))
			return NULL;
		return evaluate_assignment(C, expr);
	case EXPR_PREOP:
		if (!dmrC_evaluate_expression(C, expr->unop))
			return NULL;
		return evaluate_preop(C, expr);
	case EXPR_POSTOP:
		if (!dmrC_evaluate_expression(C, expr->unop))
			return NULL;
		return evaluate_postop(C, expr);
	case EXPR_CAST:
	case EXPR_FORCE_CAST:
	case EXPR_IMPLIED_CAST:
		return evaluate_cast(C, expr);
	case EXPR_SIZEOF:
		return evaluate_sizeof(C, expr);
	case EXPR_PTRSIZEOF:
		return evaluate_ptrsizeof(C, expr);
	case EXPR_ALIGNOF:
		return evaluate_alignof(C, expr);
	case EXPR_DEREF:
		return evaluate_member_dereference(C, expr);
	case EXPR_CALL:
		return evaluate_call(C, expr);
	case EXPR_SELECT:
	case EXPR_CONDITIONAL:
		return evaluate_conditional_expression(C, expr);
	case EXPR_STATEMENT:
		expr->ctype = dmrC_evaluate_statement(C, expr->statement);
		return expr->ctype;

	case EXPR_LABEL:
		expr->ctype = &C->S->ptr_ctype;
		return &C->S->ptr_ctype;

	case EXPR_TYPE:
		/* Evaluate the type of the symbol .. */
		evaluate_symbol(C, expr->symbol);
		/* .. but the type of the _expression_ is a "type" */
		expr->ctype = &C->S->type_ctype;
		return &C->S->type_ctype;

	case EXPR_OFFSETOF:
		return evaluate_offsetof(C, expr);

	/* These can not exist as stand-alone expressions */
	case EXPR_INITIALIZER:
	case EXPR_IDENTIFIER:
	case EXPR_INDEX:
	case EXPR_POS:
		dmrC_expression_error(C, expr, "internal front-end error: initializer in expression");
		return NULL;
	case EXPR_SLICE:
		dmrC_expression_error(C, expr, "internal front-end error: SLICE re-evaluated");
		return NULL;
	}
	return NULL;
}

static void check_duplicates(struct dmr_C *C, struct symbol *sym)
{
	int declared = 0;
	struct symbol *next = sym;
	int initialized = sym->initializer != NULL;

	while ((next = next->same_symbol) != NULL) {
		const char *typediff;
		evaluate_symbol(C, next);
		if (initialized && next->initializer) {
			dmrC_sparse_error(C, sym->pos, "symbol '%s' has multiple initializers (originally initialized at %s:%d)",
				dmrC_show_ident(C, sym->ident),
				dmrC_stream_name(C, next->pos.stream), next->pos.line);
			/* Only warn once */
			initialized = 0;
		}
		declared++;
		typediff = dmrC_type_difference(C, &sym->ctype, &next->ctype, 0, 0);
		if (typediff) {
			dmrC_sparse_error(C, sym->pos, "symbol '%s' redeclared with different type (originally declared at %s:%d) - %s",
				dmrC_show_ident(C, sym->ident),
				dmrC_stream_name(C, next->pos.stream), next->pos.line, typediff);
			return;
		}
	}
	if (!declared) {
		unsigned long mod = sym->ctype.modifiers;
		if (mod & (MOD_STATIC | MOD_REGISTER))
			return;
		if (!(mod & MOD_TOPLEVEL))
			return;
		if (!C->Wdecl)
			return;
		if (sym->ident == C->S->main_ident)
			return;
		dmrC_warning(C, sym->pos, "symbol '%s' was not declared. Should it be static?", dmrC_show_ident(C, sym->ident));
	}
}

static struct symbol *evaluate_symbol(struct dmr_C *C, struct symbol *sym)
{
	struct symbol *base_type;

	if (!sym)
		return sym;
	if (sym->evaluated)
		return sym;
	sym->evaluated = 1;

	sym = dmrC_examine_symbol_type(C->S, sym);
	base_type = dmrC_get_base_type(C->S, sym);
	if (!base_type)
		return NULL;

	/* Evaluate the initializers */
	if (sym->initializer)
		evaluate_initializer(C, sym, &sym->initializer);

	/* And finally, evaluate the body of the symbol too */
	if (base_type->type == SYM_FN) {
		struct symbol *curr = C->current_fn;

		if (sym->definition && sym->definition != sym)
			return evaluate_symbol(C, sym->definition);

		C->current_fn = base_type;

		examine_fn_arguments(C, base_type);
		if (!base_type->stmt && base_type->inline_stmt)
			dmrC_uninline(C, sym);
		if (base_type->stmt)
			dmrC_evaluate_statement(C, base_type->stmt);

		C->current_fn = curr;
	}

	return base_type;
}

void dmrC_evaluate_symbol_list(struct dmr_C *C, struct symbol_list *list)
{
	struct symbol *sym;

	FOR_EACH_PTR(list, sym) {
		C->has_error &= ~ERROR_CURR_PHASE;
		evaluate_symbol(C, sym);
		check_duplicates(C, sym);
	} END_FOR_EACH_PTR(sym); 
}

static struct symbol *evaluate_return_expression(struct dmr_C *C, struct statement *stmt)
{
	struct expression *expr = stmt->expression;
	struct symbol *fntype;

	dmrC_evaluate_expression(C, expr);
	fntype = C->current_fn->ctype.base_type;
	if (!fntype || fntype == &C->S->void_ctype) {
		if (expr && expr->ctype != &C->S->void_ctype)
			dmrC_expression_error(C, expr, "return expression in %s function", fntype?"void":"typeless");
		if (expr && C->Wreturn_void)
			dmrC_warning(C, stmt->pos, "returning void-valued expression");
		return NULL;
	}

	if (!expr) {
		dmrC_sparse_error(C, stmt->pos, "return with no return value");
		return NULL;
	}
	if (!expr->ctype)
		return NULL;
	compatible_assignment_types(C, expr, fntype, &stmt->expression, "return expression");
	return NULL;
}

static void evaluate_if_statement(struct dmr_C *C, struct statement *stmt)
{
	if (!stmt->if_conditional)
		return;

	evaluate_conditional(C, stmt->if_conditional, 0);
	dmrC_evaluate_statement(C, stmt->if_true);
	dmrC_evaluate_statement(C, stmt->if_false);
}

static void evaluate_iterator(struct dmr_C *C, struct statement *stmt)
{
	dmrC_evaluate_symbol_list(C, stmt->iterator_syms);
	evaluate_conditional(C, stmt->iterator_pre_condition, 1);
	evaluate_conditional(C, stmt->iterator_post_condition,1);
	dmrC_evaluate_statement(C, stmt->iterator_pre_statement);
	dmrC_evaluate_statement(C, stmt->iterator_statement);
	dmrC_evaluate_statement(C, stmt->iterator_post_statement);
}

static void verify_output_constraint(struct dmr_C *C, struct expression *expr, const char *constraint)
{
	switch (*constraint) {
	case '=':	/* Assignment */
	case '+':	/* Update */
		break;
	default:
		dmrC_expression_error(C, expr, "output constraint is not an assignment constraint (\"%s\")", constraint);
	}
}

static void verify_input_constraint(struct dmr_C *C, struct expression *expr, const char *constraint)
{
	switch (*constraint) {
	case '=':	/* Assignment */
	case '+':	/* Update */
		dmrC_expression_error(C, expr, "input constraint with assignment (\"%s\")", constraint);
	}
}

static void evaluate_asm_statement(struct dmr_C *C, struct statement *stmt)
{
	struct expression *expr;
	struct symbol *sym;
	int state;

	expr = stmt->asm_string;
	if (!expr || expr->type != EXPR_STRING) {
		dmrC_sparse_error(C, stmt->pos, "need constant string for inline asm");
		return;
	}

	state = 0;
	FOR_EACH_PTR(stmt->asm_outputs, expr) {
		switch (state) {
		case 0: /* Identifier */
			state = 1;
			continue;

		case 1: /* Constraint */
			state = 2;
			if (!expr || expr->type != EXPR_STRING) {
				dmrC_sparse_error(C, expr ? expr->pos : stmt->pos, "asm output constraint is not a string");
				*THIS_ADDRESS(struct expression *, expr) = NULL;
				continue;
			}
			verify_output_constraint(C, expr, expr->string->data);
			continue;

		case 2: /* Expression */
			state = 0;
			if (!dmrC_evaluate_expression(C, expr))
				return;
			if (!lvalue_expression(C, expr))
				dmrC_warning(C, expr->pos, "asm output is not an lvalue");
			evaluate_assign_to(C, expr, expr->ctype);
			continue;
		}
	} END_FOR_EACH_PTR(expr);

	state = 0;
	FOR_EACH_PTR(stmt->asm_inputs, expr) {
		switch (state) {
		case 0: /* Identifier */
			state = 1;
			continue;

		case 1:	/* Constraint */
			state = 2;
			if (!expr || expr->type != EXPR_STRING) {
				dmrC_sparse_error(C, expr ? expr->pos : stmt->pos, "asm input constraint is not a string");
				*THIS_ADDRESS(struct expression *, expr) = NULL;
				continue;
			}
			verify_input_constraint(C, expr, expr->string->data);
			continue;

		case 2: /* Expression */
			state = 0;
			if (!dmrC_evaluate_expression(C, expr))
				return;
			continue;
		}
	} END_FOR_EACH_PTR(expr);

	FOR_EACH_PTR(stmt->asm_clobbers, expr) {
		if (!expr) {
			dmrC_sparse_error(C, stmt->pos, "bad asm clobbers");
			return;
		}
		if (expr->type == EXPR_STRING)
			continue;
		dmrC_expression_error(C, expr, "asm clobber is not a string");
	} END_FOR_EACH_PTR(expr);


	FOR_EACH_PTR(stmt->asm_labels, sym) {
		if (!sym || sym->type != SYM_LABEL) {
			dmrC_sparse_error(C, stmt->pos, "bad asm label");
			return;
		}
	} END_FOR_EACH_PTR(sym);
}

static void evaluate_case_statement(struct dmr_C *C, struct statement *stmt)
{
	dmrC_evaluate_expression(C, stmt->case_expression);
	dmrC_evaluate_expression(C, stmt->case_to);
	dmrC_evaluate_statement(C, stmt->case_statement);
}

static void check_case_type(struct dmr_C *C, struct expression *switch_expr,
			    struct expression *case_expr,
			    struct expression **enumcase)
{
	struct symbol *switch_type, *case_type;
	int sclass, cclass;

	if (!case_expr)
		return;

	switch_type = switch_expr->ctype;
	case_type = dmrC_evaluate_expression(C, case_expr);

	if (!switch_type || !case_type)
		goto Bad;
	if (enumcase) {
		if (*enumcase)
			warn_for_different_enum_types(C, case_expr->pos, case_type, (*enumcase)->ctype);
		else if (dmrC_is_enum_type(case_type))
			*enumcase = case_expr;
	}

	sclass = classify_type(C, switch_type, &switch_type);
	cclass = classify_type(C, case_type, &case_type);

	/* both should be arithmetic */
	if (!(sclass & cclass & TYPE_NUM))
		goto Bad;

	/* neither should be floating */
	if ((sclass | cclass) & TYPE_FLOAT)
		goto Bad;

	/* if neither is restricted, we are OK */
	if (!((sclass | cclass) & TYPE_RESTRICT))
		return;

	if (!restricted_binop_type(C, SPECIAL_EQUAL, case_expr, switch_expr,
				   cclass, sclass, case_type, switch_type)) {
		unrestrict(C, case_expr, cclass, &case_type);
		unrestrict(C, switch_expr, sclass, &switch_type);
	}
	return;

Bad:
	dmrC_expression_error(C, case_expr, "incompatible types for 'case' statement");
}

static void evaluate_switch_statement(struct dmr_C *C, struct statement *stmt)
{
	struct symbol *sym;
	struct expression *enumcase = NULL;
	struct expression **enumcase_holder = &enumcase;
	struct expression *sel = stmt->switch_expression;

	dmrC_evaluate_expression(C, sel);
	dmrC_evaluate_statement(C, stmt->switch_statement);
	if (!sel)
		return;
	if (sel->ctype && dmrC_is_enum_type(sel->ctype))
		enumcase_holder = NULL; /* Only check cases against switch */

	FOR_EACH_PTR(stmt->switch_case->symbol_list, sym) {
		struct statement *case_stmt = sym->stmt;
		check_case_type(C, sel, case_stmt->case_expression, enumcase_holder);
		check_case_type(C, sel, case_stmt->case_to, enumcase_holder);
	} END_FOR_EACH_PTR(sym);
}

static void evaluate_goto_statement(struct dmr_C *C, struct statement *stmt)
{
	struct symbol *label = stmt->goto_label;

	if (label && !label->stmt && !dmrC_lookup_keyword(label->ident, NS_KEYWORD))
		dmrC_sparse_error(C, stmt->pos, "label '%s' was not declared", dmrC_show_ident(C, label->ident));

	dmrC_evaluate_expression(C, stmt->goto_expression);
}

struct symbol *dmrC_evaluate_statement(struct dmr_C *C, struct statement *stmt)
{
	if (!stmt)
		return NULL;

	switch (stmt->type) {
	case STMT_DECLARATION: {
		struct symbol *s;
		FOR_EACH_PTR(stmt->declaration, s) {
			evaluate_symbol(C, s);
		} END_FOR_EACH_PTR(s);
		return NULL;
	}

	case STMT_RETURN:
		return evaluate_return_expression(C, stmt);

	case STMT_EXPRESSION:
		if (!dmrC_evaluate_expression(C, stmt->expression))
			return NULL;
		if (stmt->expression->ctype == &C->S->null_ctype)
			stmt->expression = cast_to(C, stmt->expression, &C->S->ptr_ctype);
		return degenerate(C, stmt->expression);

	case STMT_COMPOUND: {
		struct statement *s;
		struct symbol *type = NULL;

		/* Evaluate the return symbol in the compound statement */
		evaluate_symbol(C, stmt->ret);

		/*
		 * Then, evaluate each statement, making the type of the
		 * compound statement be the type of the last statement
		 */
		type = dmrC_evaluate_statement(C, stmt->args);
		FOR_EACH_PTR(stmt->stmts, s) {
			type = dmrC_evaluate_statement(C, s);
		} END_FOR_EACH_PTR(s);
		if (!type)
			type = &C->S->void_ctype;
		return type;
	}
	case STMT_IF:
		evaluate_if_statement(C, stmt);
		return NULL;
	case STMT_ITERATOR:
		evaluate_iterator(C, stmt);
		return NULL;
	case STMT_SWITCH:
		evaluate_switch_statement(C, stmt);
		return NULL;
	case STMT_CASE:
		evaluate_case_statement(C, stmt);
		return NULL;
	case STMT_LABEL:
		return dmrC_evaluate_statement(C, stmt->label_statement);
	case STMT_GOTO:
		evaluate_goto_statement(C, stmt);
		return NULL;
	case STMT_NONE:
		break;
	case STMT_ASM:
		evaluate_asm_statement(C, stmt);
		return NULL;
	case STMT_CONTEXT:
		dmrC_evaluate_expression(C, stmt->expression);
		return NULL;
	case STMT_RANGE:
		dmrC_evaluate_expression(C, stmt->range_expression);
		dmrC_evaluate_expression(C, stmt->range_low);
		dmrC_evaluate_expression(C, stmt->range_high);
		return NULL;
	}
	return NULL;
}

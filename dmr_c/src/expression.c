/*
 * sparse/expression.c
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
 * This is the expression parsing part of parsing C.
 */
 /*
 * This version is part of the dmr_c project.
 * Copyright (C) 2017 Dibyendu Majumdar
 */
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

#include <port.h>
#include <lib.h>
#include <allocate.h>
#include <token.h>
#include <parse.h>
#include <symbol.h>
#include <scope.h>
#include <expression.h>
#include <target.h>
#include <char.h>

static int match_oplist(int op, ...)
{
	va_list args;
	int nextop;

	va_start(args, op);
	do {
		nextop = va_arg(args, int);
	} while (nextop != 0 && nextop != op);
	va_end(args);

	return nextop != 0;
}

static struct token *comma_expression(struct dmr_C *C, struct token *, struct expression **);

struct token *dmrC_parens_expression(struct dmr_C *C, struct token *token, struct expression **expr, const char *where)
{
	token = dmrC_expect_token(C, token, '(', where);
	if (dmrC_match_op(token, '{')) {
		struct expression *e = dmrC_alloc_expression(C, token->pos, EXPR_STATEMENT);
		struct statement *stmt = dmrC_alloc_statement(C, token->pos, STMT_COMPOUND);
		*expr = e;
		e->statement = stmt;
		dmrC_start_symbol_scope(C);
		token = dmrC_compound_statement(C, token->next, stmt);
		dmrC_end_symbol_scope(C);
		token = dmrC_expect_token(C, token, '}', "at end of statement expression");
	} else
		token = dmrC_parse_expression(C, token, expr);
	return dmrC_expect_token(C, token, ')', where);
}

/*
 * Handle __func__, __FUNCTION__ and __PRETTY_FUNCTION__ token
 * conversion
 */
static struct symbol *handle_func(struct dmr_C *C, struct token *token)
{
	struct ident *ident = token->ident;
	struct symbol *decl, *array;
	struct string *string;
	int len;

	if (ident != C->S->__func___ident &&
	    ident != C->S->__FUNCTION___ident &&
	    ident != C->S->__PRETTY_FUNCTION___ident)
		return NULL;

	if (!C->current_fn || !C->current_fn->ident)
		return NULL;

	/* OK, it's one of ours */
	array = dmrC_alloc_symbol(C->S, token->pos, SYM_ARRAY);
	array->ctype.base_type = &C->S->char_ctype;
	array->ctype.alignment = 1;
	array->endpos = token->pos;
	decl = dmrC_alloc_symbol(C->S, token->pos, SYM_NODE);
	decl->ctype.base_type = array;
	decl->ctype.alignment = 1;
	decl->ctype.modifiers = MOD_STATIC;
	decl->endpos = token->pos;

	/* function-scope, but in NS_SYMBOL */
	dmrC_bind_symbol(C->S, decl, ident, NS_LABEL);
	decl->ns = NS_SYMBOL;

	len = C->current_fn->ident->len;
	string = (struct string *)dmrC_allocator_allocate(&C->string_allocator, len + 1);
	memcpy(string->data, C->current_fn->ident->name, len);
	string->data[len] = 0;
	string->length = len + 1;

	decl->initializer = dmrC_alloc_expression(C, token->pos, EXPR_STRING);
	decl->initializer->string = string;
	decl->initializer->ctype = decl;
	decl->array_size = dmrC_alloc_const_expression(C, token->pos, len + 1);
	array->array_size = decl->array_size;
	decl->bit_size = array->bit_size = dmrC_bytes_to_bits(C->target, len + 1);

	return decl;
}

static struct token *parse_type(struct dmr_C *C, struct token *token, struct expression **tree)
{
	struct symbol *sym;
	*tree = dmrC_alloc_expression(C, token->pos, EXPR_TYPE);
	(*tree)->flags = Int_const_expr; /* sic */
	token = dmrC_typename(C, token, &sym, NULL);
	if (sym->ident)
		dmrC_sparse_error(C, token->pos,
			     "type expression should not include identifier "
			     "\"%s\"", sym->ident->name);
	(*tree)->symbol = sym;
	return token;
}

static struct token *builtin_types_compatible_p_expr(struct dmr_C *C, struct token *token,
						     struct expression **tree)
{
	struct expression *expr = dmrC_alloc_expression(
		C, token->pos, EXPR_COMPARE);
	expr->flags = Int_const_expr;
	expr->op = SPECIAL_EQUAL;
	token = token->next;
	if (!dmrC_match_op(token, '('))
		return dmrC_expect_token(C, token, '(',
			      "after __builtin_types_compatible_p");
	token = token->next;
	token = parse_type(C, token, &expr->left);
	if (!dmrC_match_op(token, ','))
		return dmrC_expect_token(C, token, ',',
			      "in __builtin_types_compatible_p");
	token = token->next;
	token = parse_type(C, token, &expr->right);
	if (!dmrC_match_op(token, ')'))
		return dmrC_expect_token(C, token, ')',
			      "at end of __builtin_types_compatible_p");
	token = token->next;
	
	*tree = expr;
	return token;
}

static struct token *builtin_offsetof_expr(struct dmr_C *C, struct token *token,
					   struct expression **tree)
{
	struct expression *expr = NULL;
	struct expression **p = &expr;
	struct symbol *sym;
	int op = '.';

	token = token->next;
	if (!dmrC_match_op(token, '('))
		return dmrC_expect_token(C, token, '(', "after __builtin_offset");

	token = token->next;
	token = dmrC_typename(C, token, &sym, NULL);
	if (sym->ident)
		dmrC_sparse_error(C, token->pos,
			     "type expression should not include identifier "
			     "\"%s\"", sym->ident->name);

	if (!dmrC_match_op(token, ','))
		return dmrC_expect_token(C, token, ',', "in __builtin_offset");

	while (1) {
		struct expression *e;
		switch (op) {
		case ')':
			expr->in = sym;
			*tree = expr;
		default:
			return dmrC_expect_token(C, token, ')', "at end of __builtin_offset");
		case SPECIAL_DEREFERENCE:
			e = dmrC_alloc_expression(C, token->pos, EXPR_OFFSETOF);
			e->flags = Int_const_expr;
			e->op = '[';
			*p = e;
			p = &e->down;
			/* fall through */
		case '.':
			token = token->next;
			e = dmrC_alloc_expression(C, token->pos, EXPR_OFFSETOF);
			e->flags = Int_const_expr;
			e->op = '.';
			if (dmrC_token_type(token) != TOKEN_IDENT) {
				dmrC_sparse_error(C, token->pos, "Expected member name");
				return token;
			}
			e->ident = token->ident;
			token = token->next;
			break;
		case '[':
			token = token->next;
			e = dmrC_alloc_expression(C, token->pos, EXPR_OFFSETOF);
			e->flags = Int_const_expr;
			e->op = '[';
			token = dmrC_parse_expression(C, token, &e->index);
			token = dmrC_expect_token(C, token, ']',
					"at end of array dereference");
			if (!e->index)
				return token;
		}
		*p = e;
		p = &e->down;
		op = dmrC_token_type(token) == TOKEN_SPECIAL ? token->special : 0;
	}
}

#ifndef ULLONG_MAX
#define ULLONG_MAX (~0ULL)
#endif

static unsigned long long parse_num(const char *nptr, char **end)
{
	if (nptr[0] == '0' && tolower((unsigned char)nptr[1]) == 'b')
		return strtoull(&nptr[2], end, 2);
	return strtoull(nptr, end, 0);
}

static void get_number_value(struct dmr_C *C, struct expression *expr, struct token *token)
{
	const char *str = token->number;
	unsigned long long value;
	char *end;
	int size = 0, want_unsigned = 0;
	int overflow = 0, do_warn = 0;
	int try_unsigned = 1;
	int bits;

	errno = 0;
	value = parse_num(str, &end);
	if (end == str)
		goto Float;
	if (value == ULLONG_MAX && errno == ERANGE)
		overflow = 1;
	while (1) {
		char c = *end++;
		if (!c) {
			break;
		} else if (c == 'u' || c == 'U') {
			if (want_unsigned)
				goto Enoint;
			want_unsigned = 1;
		} else if (c == 'l' || c == 'L') {
			if (size)
				goto Enoint;
			size = 1;
			if (*end == c) {
				size = 2;
				end++;
			}
		} else
			goto Float;
	}
	if (overflow)
		goto Eoverflow;
	/* OK, it's a valid integer */
	/* decimals can be unsigned only if directly specified as such */
	if (str[0] != '0' && !want_unsigned)
		try_unsigned = 0;
	if (!size) {
		bits = C->target->bits_in_int - 1;
		if (!(value & (~1ULL << bits))) {
			if (!(value & (1ULL << bits))) {
				goto got_it;
			} else if (try_unsigned) {
				want_unsigned = 1;
				goto got_it;
			}
		}
		size = 1;
		do_warn = 1;
	}
	if (size < 2) {
		bits = C->target->bits_in_long - 1;
		if (!(value & (~1ULL << bits))) {
			if (!(value & (1ULL << bits))) {
				goto got_it;
			} else if (try_unsigned) {
				want_unsigned = 1;
				goto got_it;
			}
			do_warn |= 2;
		}
		size = 2;
		do_warn |= 1;
	}
	bits = C->target->bits_in_longlong - 1;
	if (value & (~1ULL << bits))
		goto Eoverflow;
	if (!(value & (1ULL << bits)))
		goto got_it;
	if (!try_unsigned)
		dmrC_warning(C, expr->pos, "decimal constant %s is too big for long long",
			dmrC_show_token(C, token));
	want_unsigned = 1;
got_it:
	if (do_warn)
		dmrC_warning(C, expr->pos, "constant %s is so big it is%s%s%s",
			dmrC_show_token(C, token),
			want_unsigned ? " unsigned":"",
			size > 0 ? " long":"",
			size > 1 ? " long":"");
	if (do_warn & 2)
		dmrC_warning(C, expr->pos,
			"decimal constant %s is between LONG_MAX and ULONG_MAX."
			" For C99 that means long long, C90 compilers are very "
			"likely to produce unsigned long (and a warning) here",
			dmrC_show_token(C, token));
        expr->type = EXPR_VALUE;
	expr->flags = Int_const_expr;
        expr->ctype = dmrC_ctype_integer(C, size, want_unsigned);
        expr->value = value;
	return;
Eoverflow:
	dmrC_error_die(C, expr->pos, "constant %s is too big even for unsigned long long",
			dmrC_show_token(C, token));
	return;
Float:
	expr->fvalue = dmrC_string_to_ld(str, &end);
	if (str == end)
		goto Enoint;

	if (*end && end[1])
		goto Enoint;

	if (*end == 'f' || *end == 'F')
		expr->ctype = &C->S->float_ctype;
	else if (*end == 'l' || *end == 'L')
		expr->ctype = &C->S->ldouble_ctype;
	else if (!*end)
		expr->ctype = &C->S->double_ctype;
	else
		goto Enoint;

	expr->flags = Float_literal;
	expr->type = EXPR_FVALUE;
	return;

Enoint:
	dmrC_error_die(C, expr->pos, "constant %s is not a valid number", dmrC_show_token(C, token));
}

struct token *dmrC_primary_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	struct expression *expr = NULL;

	switch (dmrC_token_type(token)) {
	case TOKEN_CHAR:
	case TOKEN_CHAR_EMBEDDED_0:
	case TOKEN_CHAR_EMBEDDED_1:
	case TOKEN_CHAR_EMBEDDED_2:
	case TOKEN_CHAR_EMBEDDED_3:
	case TOKEN_WIDE_CHAR:
	case TOKEN_WIDE_CHAR_EMBEDDED_0:
	case TOKEN_WIDE_CHAR_EMBEDDED_1:
	case TOKEN_WIDE_CHAR_EMBEDDED_2:
	case TOKEN_WIDE_CHAR_EMBEDDED_3:
		expr = dmrC_alloc_expression(C, token->pos, EXPR_VALUE);   
		expr->flags = Int_const_expr;
		expr->ctype = dmrC_token_type(token) < TOKEN_WIDE_CHAR ? &C->S->int_ctype : &C->S->long_ctype;
		dmrC_get_char_constant(C, token, &expr->value);
		token = token->next;
		break;

	case TOKEN_NUMBER:
		expr = dmrC_alloc_expression(C, token->pos, EXPR_VALUE);
		get_number_value(C, expr, token); /* will see if it's an integer */
		token = token->next;
		break;

	case TOKEN_ZERO_IDENT: {
		expr = dmrC_alloc_expression(C, token->pos, EXPR_SYMBOL);
		expr->flags = Int_const_expr;
		expr->ctype = &C->S->int_ctype;
		expr->symbol = &C->S->zero_int;
		expr->symbol_name = token->ident;
		token = token->next;
		break;
	}

	case TOKEN_IDENT: {
		struct symbol *sym = dmrC_lookup_symbol(token->ident, NS_SYMBOL | NS_TYPEDEF);
		struct token *next = token->next;

		if (!sym) {
			sym = handle_func(C, token);
			if (token->ident == C->S->__builtin_types_compatible_p_ident) {
				token = builtin_types_compatible_p_expr(C, token, &expr);
				break;
			}
			if (token->ident == C->S->__builtin_offsetof_ident) {
				token = builtin_offsetof_expr(C, token, &expr);
				break;
			}
		} else if (sym->enum_member) {
			expr = dmrC_alloc_expression(C, token->pos, EXPR_VALUE);
			*expr = *sym->initializer;
			/* we want the right position reported, thus the copy */
			expr->pos = token->pos;
			expr->flags = Int_const_expr;
			token = next;
			break;
		}

		expr = dmrC_alloc_expression(C, token->pos, EXPR_SYMBOL);

		/*
		 * We support types as real first-class citizens, with type
		 * comparisons etc:
		 *
		 *	if (typeof(a) == int) ..
		 */
		if (sym && sym->ns == NS_TYPEDEF) {
			dmrC_sparse_error(C, token->pos, "typename in expression");
			sym = NULL;
		}
		expr->symbol_name = token->ident;
		expr->symbol = sym;
		token = next;
		break;
	}

	case TOKEN_STRING:
	case TOKEN_WIDE_STRING:
		expr = dmrC_alloc_expression(C, token->pos, EXPR_STRING);
		token = dmrC_get_string_constant(C, token, expr);
		break;

	case TOKEN_SPECIAL:
		if (token->special == '(') {
			expr = dmrC_alloc_expression(C, token->pos, EXPR_PREOP);
			expr->op = '(';
			token = dmrC_parens_expression(C, token, &expr->unop, "in expression");
			if (expr->unop)
				expr->flags = expr->unop->flags;
			break;
		}
		if (token->special == '[' && dmrC_lookup_type(token->next)) {
			expr = dmrC_alloc_expression(C, token->pos, EXPR_TYPE);
			expr->flags = Int_const_expr; /* sic */
			token = dmrC_typename(C, token->next, &expr->symbol, NULL);
			token = dmrC_expect_token(C, token, ']', "in type expression");
			break;
		}
			
	default:
		;
	}
	*tree = expr;
	return token;
}

static struct token *expression_list(struct dmr_C *C, struct token *token, struct expression_list **list)
{
	while (!dmrC_match_op(token, ')')) {
		struct expression *expr = NULL;
		token = dmrC_assignment_expression(C, token, &expr);
		if (!expr)
			break;
		dmrC_add_expression(C, list, expr);
		if (!dmrC_match_op(token, ','))
			break;
		token = token->next;
	}
	return token;
}

/*
 * extend to deal with the ambiguous C grammar for parsing
 * a cast expressions followed by an initializer.
 */
static struct token *postfix_expression(struct dmr_C *C, struct token *token, struct expression **tree, struct expression *cast_init_expr)
{
	struct expression *expr = cast_init_expr;

	if (!expr)
		token = dmrC_primary_expression(C, token, &expr);

	while (expr && dmrC_token_type(token) == TOKEN_SPECIAL) {
		switch (token->special) {
		case '[': {			/* Array dereference */
			struct expression *deref = dmrC_alloc_expression(C, token->pos, EXPR_PREOP);
			struct expression *add = dmrC_alloc_expression(C, token->pos, EXPR_BINOP);

			deref->op = '*';
			deref->unop = add;

			add->op = '+';
			add->left = expr;
			token = dmrC_parse_expression(C, token->next, &add->right);
			token = dmrC_expect_token(C, token, ']', "at end of array dereference");
			expr = deref;
			continue;
		}
		case SPECIAL_INCREMENT:		/* Post-increment */
		case SPECIAL_DECREMENT:	{	/* Post-decrement */
			struct expression *post = dmrC_alloc_expression(C, token->pos, EXPR_POSTOP);
			post->op = token->special;
			post->unop = expr;
			expr = post;
			token = token->next;
			continue;
		}
		case SPECIAL_DEREFERENCE: {	/* Structure pointer member dereference */
			/* "x->y" is just shorthand for "(*x).y" */
			struct expression *inner = dmrC_alloc_expression(C, token->pos, EXPR_PREOP);
			inner->op = '*';
			inner->unop = expr;
			expr = inner;
		}
		/* Fall through!! */
		case '.': {			/* Structure member dereference */
			struct expression *deref = dmrC_alloc_expression(C, token->pos, EXPR_DEREF);
			deref->op = '.';
			deref->deref = expr;
			token = token->next;
			if (dmrC_token_type(token) != TOKEN_IDENT) {
				dmrC_sparse_error(C, token->pos, "Expected member name");
				break;
			}
			deref->member = token->ident;
			token = token->next;
			expr = deref;
			continue;
		}

		case '(': {			/* Function call */
			struct expression *call = dmrC_alloc_expression(C, token->pos, EXPR_CALL);
			call->op = '(';
			call->fn = expr;
			token = expression_list(C, token->next, &call->args);
			token = dmrC_expect_token(C, token, ')', "in function call");
			expr = call;
			continue;
		}

		default:
			break;
		}
		break;
	}
	*tree = expr;
	return token;
}

static struct token *cast_expression(struct dmr_C *C, struct token *token, struct expression **tree);
static struct token *unary_expression(struct dmr_C *C, struct token *token, struct expression **tree);

static struct token *type_info_expression(struct dmr_C *C, struct token *token,
	struct expression **tree, int type)
{
	struct expression *expr = dmrC_alloc_expression(C, token->pos, type);
	struct token *p;

	*tree = expr;
	expr->flags = Int_const_expr; /* XXX: VLA support will need that changed */
	token = token->next;
	if (!dmrC_match_op(token, '(') || !dmrC_lookup_type(token->next))
		return unary_expression(C, token, &expr->cast_expression);
	p = token;
	token = dmrC_typename(C, token->next, &expr->cast_type, NULL);

	if (!dmrC_match_op(token, ')')) {
		static const char * error[] = {
			[EXPR_SIZEOF] = "at end of sizeof",
			[EXPR_ALIGNOF] = "at end of __alignof__",
			[EXPR_PTRSIZEOF] = "at end of __sizeof_ptr__"
		};
		return dmrC_expect_token(C, token, ')', error[type]);
	}

	token = token->next;
	/*
	 * C99 ambiguity: the typename might have been the beginning
	 * of a typed initializer expression..
	 */
	if (dmrC_match_op(token, '{')) {
		struct expression *cast = dmrC_alloc_expression(C, p->pos, EXPR_CAST);
		cast->cast_type = expr->cast_type;
		expr->cast_type = NULL;
		expr->cast_expression = cast;
		token = dmrC_initializer(C, &cast->cast_expression, token);
		token = postfix_expression(C, token, &expr->cast_expression, cast);
	}
	return token;
}

static struct token *unary_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	if (dmrC_token_type(token) == TOKEN_IDENT) {
		struct ident *ident = token->ident;
		if (ident->reserved) {
			const struct {
				struct ident *id;
				int type;
			} type_information[] = {
				{ C->S->sizeof_ident, EXPR_SIZEOF },
				{ C->S->__alignof___ident, EXPR_ALIGNOF },
				{ C->S->__alignof_ident, EXPR_ALIGNOF },
				{ C->S->_Alignof_ident, EXPR_ALIGNOF },
				{ C->S->__sizeof_ptr___ident, EXPR_PTRSIZEOF },
			};
			int i;
			for (i = 0; i < (int)ARRAY_SIZE(type_information); i++) {
				if (ident == type_information[i].id)
					return type_info_expression(C, token, tree, type_information[i].type);
			}
		}
	}

	if (dmrC_token_type(token) == TOKEN_SPECIAL) {
		if (match_oplist(token->special,
		    SPECIAL_INCREMENT, SPECIAL_DECREMENT,
		    '&', '*', 0)) {
		    	struct expression *unop;
			struct expression *unary;
			struct token *next;

			next = cast_expression(C, token->next, &unop);
			if (!unop) {
				dmrC_sparse_error(C, token->pos, "Syntax error in unary expression");
				*tree = NULL;
				return next;
			}
			unary = dmrC_alloc_expression(C, token->pos, EXPR_PREOP);
			unary->op = token->special;
			unary->unop = unop;
			*tree = unary;
			return next;
		}
		/* possibly constant ones */
		if (match_oplist(token->special, '+', '-', '~', '!', 0)) {
		    	struct expression *unop;
			struct expression *unary;
			struct token *next;

			next = cast_expression(C, token->next, &unop);
			if (!unop) {
				dmrC_sparse_error(C, token->pos, "Syntax error in unary expression");
				*tree = NULL;
				return next;
			}
			unary = dmrC_alloc_expression(C, token->pos, EXPR_PREOP);
			unary->op = token->special;
			unary->unop = unop;
			unary->flags = unop->flags & Int_const_expr;
			*tree = unary;
			return next;
		}
		/* Gcc extension: &&label gives the address of a label */
		if (dmrC_match_op(token, SPECIAL_LOGICAL_AND) &&
		    dmrC_token_type(token->next) == TOKEN_IDENT) {
			struct expression *label = dmrC_alloc_expression(C, token->pos, EXPR_LABEL);
			struct symbol *sym = dmrC_label_symbol(C, token->next);
			if (!(sym->ctype.modifiers & MOD_ADDRESSABLE)) {
				sym->ctype.modifiers |= MOD_ADDRESSABLE;
				dmrC_add_symbol(C, &C->P->function_computed_target_list, sym);
			}
			label->label_symbol = sym;
			*tree = label;
			return token->next->next;
		}
						
	}
			
	return postfix_expression(C, token, tree, NULL);
}

/*
 * Ambiguity: a '(' can be either a cast-expression or
 * a primary-expression depending on whether it is followed
 * by a type or not. 
 *
 * additional ambiguity: a "cast expression" followed by
 * an initializer is really a postfix-expression.
 */
static struct token *cast_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	if (dmrC_match_op(token, '(')) {
		struct token *next = token->next;
		if (dmrC_lookup_type(next)) {
			struct expression *cast = dmrC_alloc_expression(C, next->pos, EXPR_CAST);
			struct expression *v;
			struct symbol *sym;
			int is_force;

			token = dmrC_typename(C, next, &sym, &is_force);
			cast->cast_type = sym;
			token = dmrC_expect_token(C, token, ')', "at end of cast operator");
			if (dmrC_match_op(token, '{')) {
				if (is_force)
					dmrC_warning(C, sym->pos,
						"[force] in compound literal");
				token = dmrC_initializer(C, &cast->cast_expression, token);
				return postfix_expression(C, token, tree, cast);
			}
			*tree = cast;
			if (is_force)
				cast->type = EXPR_FORCE_CAST;
			token = cast_expression(C, token, &v);
			if (!v)
				return token;
			cast->cast_expression = v;
			if (v->flags & Int_const_expr)
				cast->flags = Int_const_expr;
			else if (v->flags & Float_literal) /* and _not_ int */
				cast->flags = Int_const_expr | Float_literal;
			return token;
		}
	}
	return unary_expression(C, token, tree);
}

/*
 * Generic left-to-right binop parsing
 *
 * This _really_ needs to be inlined, because that makes the inner
 * function call statically deterministic rather than a totally
 * unpredictable indirect call. But gcc-3 is so "clever" that it
 * doesn't do so by default even when you tell it to inline it.
 *
 * Making it a macro avoids the inlining problem, and also means
 * that we can pass in the op-comparison as an expression rather
 * than create a data structure for it.
 */

#define LR_BINOP_EXPRESSION(C, __token, tree, type, inner, compare)	\
	struct expression *left = NULL;					\
	struct token * next = inner(C, __token, &left);			\
									\
	if (left) {							\
		while (dmrC_token_type(next) == TOKEN_SPECIAL) {		\
			struct expression *top, *right = NULL;		\
			int op = next->special;				\
									\
			if (!(compare))					\
				goto out;				\
			top = dmrC_alloc_expression(C, next->pos, type);	\
			next = inner(C, next->next, &right);		\
			if (!right) {					\
				dmrC_sparse_error(C, next->pos, "No right hand side of '%s'-expression", dmrC_show_special(C, op));	\
				break;					\
			}						\
			top->flags = left->flags & right->flags		\
						& Int_const_expr;	\
			top->op = op;					\
			top->left = left;				\
			top->right = right;				\
			left = top;					\
		}							\
	}								\
out:									\
	*tree = left;							\
	return next;							\

static struct token *multiplicative_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C, 
		token, tree, EXPR_BINOP, cast_expression,
		(op == '*') || (op == '/') || (op == '%')
	);
}

static struct token *additive_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C, 
		token, tree, EXPR_BINOP, multiplicative_expression,
		(op == '+') || (op == '-')
	);
}

static struct token *shift_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C, 
		token, tree, EXPR_BINOP, additive_expression,
		(op == SPECIAL_LEFTSHIFT) || (op == SPECIAL_RIGHTSHIFT)
	);
}

static struct token *relational_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C, 
		token, tree, EXPR_COMPARE, shift_expression,
		(op == '<') || (op == '>') ||
		(op == SPECIAL_LTE) || (op == SPECIAL_GTE)
	);
}

static struct token *equality_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C, 
		token, tree, EXPR_COMPARE, relational_expression,
		(op == SPECIAL_EQUAL) || (op == SPECIAL_NOTEQUAL)
	);
}

static struct token *bitwise_and_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C,
		token, tree, EXPR_BINOP, equality_expression,
		(op == '&')
	);
}

static struct token *bitwise_xor_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C,
		token, tree, EXPR_BINOP, bitwise_and_expression,
		(op == '^')
	);
}

static struct token *bitwise_or_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C,
		token, tree, EXPR_BINOP, bitwise_xor_expression,
		(op == '|')
	);
}

static struct token *logical_and_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C,
		token, tree, EXPR_LOGICAL, bitwise_or_expression,
		(op == SPECIAL_LOGICAL_AND)
	);
}

static struct token *logical_or_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C,
		token, tree, EXPR_LOGICAL, logical_and_expression,
		(op == SPECIAL_LOGICAL_OR)
	);
}

struct token *dmrC_conditional_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	token = logical_or_expression(C, token, tree);
	if (*tree && dmrC_match_op(token, '?')) {
		struct expression *expr = dmrC_alloc_expression(C, token->pos, EXPR_CONDITIONAL);
		expr->op = token->special;
		expr->left = *tree;
		*tree = expr;
		token = dmrC_parse_expression(C, token->next, &expr->cond_true);
		token = dmrC_expect_token(C, token, ':', "in conditional expression");
		token = dmrC_conditional_expression(C, token, &expr->cond_false);
		if (expr->left && expr->cond_false) {
			int is_const = expr->left->flags &
					expr->cond_false->flags &
					Int_const_expr;
			if (expr->cond_true)
				is_const &= expr->cond_true->flags;
			expr->flags = is_const;
		}
	}
	return token;
}

struct token *dmrC_assignment_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	token = dmrC_conditional_expression(C, token, tree);
	if (*tree && dmrC_token_type(token) == TOKEN_SPECIAL) {
		static const int assignments[] = {
			'=',
			SPECIAL_ADD_ASSIGN, SPECIAL_SUB_ASSIGN,
			SPECIAL_MUL_ASSIGN, SPECIAL_DIV_ASSIGN,
			SPECIAL_MOD_ASSIGN, SPECIAL_SHL_ASSIGN,
			SPECIAL_SHR_ASSIGN, SPECIAL_AND_ASSIGN,
			SPECIAL_OR_ASSIGN,  SPECIAL_XOR_ASSIGN };
		int i, op = token->special;
		for (i = 0; i < (int)ARRAY_SIZE(assignments); i++)
			if (assignments[i] == op) {
				struct expression * expr = dmrC_alloc_expression(C, token->pos, EXPR_ASSIGNMENT);
				expr->left = *tree;
				expr->op = op;
				*tree = expr;
				return dmrC_assignment_expression(C, token->next, &expr->right);
			}
	}
	return token;
}

static struct token *comma_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	LR_BINOP_EXPRESSION(C,
		token, tree, EXPR_COMMA, dmrC_assignment_expression,
		(op == ',')
	);
}

struct token *dmrC_parse_expression(struct dmr_C *C, struct token *token, struct expression **tree)
{
	return comma_expression(C, token,tree);
}



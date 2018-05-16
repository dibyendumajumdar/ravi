#ifndef DMR_C_EXPRESSION_H
#define DMR_C_EXPRESSION_H
/*
* sparse/expression.h
*
* Copyright (C) 2003 Transmeta Corp.
*               2003 Linus Torvalds
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
* Declarations and helper functions for expression parsing.
*/
/*
* This version is part of the dmr_c project.
* Copyright (C) 2017 Dibyendu Majumdar
*/


#include <allocate.h>
#include <lib.h>
#include <symbol.h>

#ifdef __cplusplus
extern "C" {
#endif

enum expression_type {
	EXPR_VALUE = 1,
	EXPR_STRING,
	EXPR_SYMBOL,
	EXPR_TYPE,
	EXPR_BINOP,
	EXPR_ASSIGNMENT,
	EXPR_LOGICAL,
	EXPR_DEREF,
	EXPR_PREOP,
	EXPR_POSTOP,
	EXPR_CAST,
	EXPR_FORCE_CAST,
	EXPR_IMPLIED_CAST,
	EXPR_SIZEOF,
	EXPR_ALIGNOF,
	EXPR_PTRSIZEOF,
	EXPR_CONDITIONAL,
	EXPR_SELECT,		// a "safe" conditional expression
	EXPR_STATEMENT,
	EXPR_CALL,
	EXPR_COMMA,
	EXPR_COMPARE,
	EXPR_LABEL,
	EXPR_INITIALIZER,	// initializer list
	EXPR_IDENTIFIER,	// identifier in initializer
	EXPR_INDEX,		// index in initializer
	EXPR_POS,		// position in initializer
	EXPR_FVALUE,
	EXPR_SLICE,
	EXPR_OFFSETOF,
};

enum {
	Int_const_expr = 1,
	Float_literal = 2,
}; /* for expr->flags */

enum {
	Taint_comma = 1,
}; /* for expr->taint */

DECLARE_PTR_LIST(expression_list, struct expression);

struct expression {
	enum expression_type type:8;
	unsigned flags:8;
	int op;
	struct position pos;
	struct symbol *ctype;
	union {
		// EXPR_VALUE
		struct {
			unsigned long long value;
			unsigned taint;
		};

		// EXPR_FVALUE
		long double fvalue;

		// EXPR_STRING
		struct {
			int wide;
			struct string *string;
		};

		// EXPR_UNOP, EXPR_PREOP and EXPR_POSTOP
		struct /* unop */ {
			struct expression *unop;
			unsigned long op_value;
		};

		// EXPR_SYMBOL, EXPR_TYPE
		struct /* symbol_arg */ {
			struct symbol *symbol;
			struct ident *symbol_name;
		};

		// EXPR_STATEMENT
		struct statement *statement;

		// EXPR_BINOP, EXPR_COMMA, EXPR_COMPARE, EXPR_LOGICAL and EXPR_ASSIGNMENT
		struct /* binop_arg */ {
			struct expression *left, *right;
		};
		// EXPR_DEREF
		struct /* deref_arg */ {
			struct expression *deref;
			struct ident *member;
		};
		// EXPR_SLICE
		struct /* slice */ {
			struct expression *base;
			unsigned r_bitpos, r_nrbits;
		};
		// EXPR_CAST and EXPR_SIZEOF
		struct /* cast_arg */ {
			struct symbol *cast_type;
			struct expression *cast_expression;
		};
		// EXPR_CONDITIONAL
		// EXPR_SELECT
		struct /* conditional_expr */ {
			struct expression *conditional, *cond_true, *cond_false;
		};
		// EXPR_CALL
		struct /* call_expr */ {
			struct expression *fn;
			struct expression_list *args;
		};
		// EXPR_LABEL
		struct /* label_expr */ {
			struct symbol *label_symbol;
		};
		// EXPR_INITIALIZER
		struct expression_list *expr_list;
		// EXPR_IDENTIFIER
		struct /* ident_expr */ {
			int offset;
			struct ident *expr_ident;
			struct symbol *field;
			struct expression *ident_expression;
		};
		// EXPR_INDEX
		struct /* index_expr */ {
			unsigned int idx_from, idx_to;
			struct expression *idx_expression;
		};
		// EXPR_POS
		struct /* initpos_expr */ {
			unsigned int init_offset, init_nr;
			struct expression *init_expr;
		};
		// EXPR_OFFSETOF
		struct {
			struct symbol *in;
			struct expression *down;
			union {
				struct ident *ident;
				struct expression *index;
			};
		};
	};
};

long long dmrC_get_expression_value_silent(struct dmr_C *C, struct expression *expr);
extern struct symbol *dmrC_evaluate_expression(struct dmr_C *C, struct expression *);
long long dmrC_get_expression_value(struct dmr_C *C, struct expression *);

/* Constant expression values */
int dmrC_is_zero_constant(struct dmr_C *C, struct expression *);
int dmrC_expr_truth_value(struct dmr_C *C, struct expression *expr);
long long dmrC_const_expression_value(struct dmr_C *C, struct expression *);

/* Expression parsing */
struct token *dmrC_conditional_expression(struct dmr_C *C, struct token *token,
				     struct expression **tree);
struct token *dmrC_primary_expression(struct dmr_C *C, struct token *token, struct expression **tree);
struct token *dmrC_parens_expression(struct dmr_C *C, struct token *token, struct expression **expr,
				const char *where);
struct token *dmrC_assignment_expression(struct dmr_C *C, struct token *token,
				    struct expression **tree);

extern void dmrC_evaluate_symbol_list(struct dmr_C *C, struct symbol_list *list);
extern struct symbol *dmrC_evaluate_statement(struct dmr_C *C, struct statement *stmt);

extern int dmrC_expand_symbol(struct dmr_C *C, struct symbol *);

static inline struct expression *dmrC_alloc_expression(struct dmr_C *C, struct position pos, int type)
{
	struct expression *expr = (struct expression *)dmrC_allocator_allocate(&C->expression_allocator, 0);
	expr->type = (enum expression_type)type;
	expr->pos = pos;
	return expr;
}

static inline struct expression *dmrC_alloc_const_expression(struct dmr_C *C, struct position pos,
							int value)
{
	struct expression *expr = (struct expression *)dmrC_allocator_allocate(&C->expression_allocator, 0);
	expr->type = EXPR_VALUE;
	expr->pos = pos;
	expr->value = value;
	expr->ctype = &C->S->int_ctype;
	return expr;
}

/* Type name parsing */
struct token *dmrC_typename(struct dmr_C *C, struct token *, struct symbol **, int *);

static inline int dmrC_lookup_type(struct token *token)
{
	if (token->pos.type == TOKEN_IDENT) {
		struct symbol *sym = dmrC_lookup_symbol(
		    token->ident,
		    (enum namespace_type)(NS_SYMBOL | NS_TYPEDEF));
		return sym && (sym->ns & NS_TYPEDEF);
	}
	return 0;
}

/* Statement parsing */
struct statement *dmrC_alloc_statement(struct dmr_C *C, struct position pos, int type);
struct token *dmrC_initializer(struct dmr_C *C, struct expression **tree, struct token *token);
struct token *dmrC_compound_statement(struct dmr_C *C, struct token *, struct statement *);

/* The preprocessor calls this 'dmrC_constant_expression()' */
#define dmrC_constant_expression(C, token, tree) dmrC_conditional_expression(C, token, tree)

/* Cast folding of constant values.. */
void dmrC_cast_value(struct dmr_C *C, struct expression *expr, struct symbol *newtype,
		struct expression *old, struct symbol *oldtype);

static inline struct expression *dmrC_first_expression(struct expression_list *head)
{
	return (struct expression *) ptrlist_first((struct ptr_list *)head);
}

#ifdef __cplusplus
}
#endif

#endif

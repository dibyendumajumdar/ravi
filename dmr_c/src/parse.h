#ifndef DMR_C_PARSE_H
#define DMR_C_PARSE_H
/*
* Basic parsing data structures. Statements and symbols.
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
*/
/*
* This version is part of the dmr_c project.
* Copyright (C) 2017 Dibyendu Majumdar
*/

#include <lib.h>
#include <symbol.h>

#ifdef __cplusplus
extern "C" {
#endif


enum statement_type {
	STMT_NONE,
	STMT_DECLARATION,
	STMT_EXPRESSION,
	STMT_COMPOUND,
	STMT_IF,
	STMT_RETURN,
	STMT_CASE,
	STMT_SWITCH,
	STMT_ITERATOR,
	STMT_LABEL,
	STMT_GOTO,
	STMT_ASM,
	STMT_CONTEXT,
	STMT_RANGE,
};

DECLARE_PTR_LIST(statement_list, struct statement);

struct statement {
	enum statement_type type;
	struct position pos;
	union {
		struct /* declaration */ {
			struct symbol_list *declaration;
		};
		struct {
			struct expression *expression;
			struct expression *context;
		};
		struct /* return_statement */ {
			struct expression *ret_value;
			struct symbol *ret_target;
		};
		struct /* if_statement */ {
			struct expression *if_conditional;
			struct statement *if_true;
			struct statement *if_false;
		};
		struct /* compound_struct */ {
			struct statement_list *stmts;
			struct symbol *ret;
			struct symbol *inline_fn;
			struct statement *args;
		};
		struct /* labeled_struct */ {
			struct symbol *label_identifier;
			struct statement *label_statement;
		};
		struct /* case_struct */ {
			struct expression *case_expression;
			struct expression *case_to;
			struct statement *case_statement;
			struct symbol *case_label;
		};
		struct /* switch_struct */ {
			struct expression *switch_expression;
			struct statement *switch_statement;
			struct symbol *switch_break, *switch_case;
		};
		struct /* iterator_struct */ {
			struct symbol *iterator_break;
			struct symbol *iterator_continue;
			struct symbol_list *iterator_syms;
			struct statement *iterator_pre_statement;
			struct expression *iterator_pre_condition;

			struct statement  *iterator_statement;

			struct statement  *iterator_post_statement;
			struct expression *iterator_post_condition;
		};
		struct /* goto_struct */ {
			struct symbol *goto_label;

			/* computed gotos have these: */
			struct expression *goto_expression;
			struct symbol_list *target_list;
		};
		struct /* asm */ {
			struct expression *asm_string;
			struct expression_list *asm_outputs;
			struct expression_list *asm_inputs;
			struct expression_list *asm_clobbers;
			struct symbol_list *asm_labels;
		};
		struct /* range */ {
			struct expression *range_expression;
			struct expression *range_low;
			struct expression *range_high;
		};
	};
};

struct parse_state_t {
	struct symbol_list **function_symbol_list;
	struct symbol_list *function_computed_target_list;
	struct statement_list *function_computed_goto_list;
	struct symbol * int_types[4];
	struct symbol * signed_types[5];
	struct symbol * unsigned_types[5];
	struct symbol * real_types[3];
	struct symbol * char_types[3];
	struct symbol ** types[7];
};

extern void dmrC_init_parser(struct dmr_C *C, int stream);
void dmrC_destroy_parser(struct dmr_C *C);

extern struct token *dmrC_parse_expression(struct dmr_C *C, struct token *, struct expression **);
extern struct symbol *dmrC_label_symbol(struct dmr_C *C, struct token *token);

extern int dmrC_show_statement(struct dmr_C *C, struct statement *);
extern int dmrC_show_expression(struct dmr_C *C, struct expression *);
typedef void(*validate_decl_t)(struct dmr_C *C, struct symbol *decl);
extern struct token *dmrC_external_declaration(struct dmr_C *C, struct token *token, struct symbol_list **symbol_list, validate_decl_t);

extern struct symbol *dmrC_ctype_integer(struct dmr_C *C, int size, int want_unsigned);

extern void dmrC_copy_statement(struct dmr_C *C, struct statement *src, struct statement *dst);
extern int dmrC_inline_function(struct dmr_C *C, struct expression *expr, struct symbol *sym);
extern void dmrC_uninline(struct dmr_C *C, struct symbol *sym);

static inline void dmrC_add_statement(struct dmr_C *C, struct statement_list **list, struct statement *stmt)
{
	ptrlist_add((struct ptr_list **)list, stmt, &C->ptrlist_allocator);
}

static inline void dmrC_add_expression(struct dmr_C *C, struct expression_list **list, struct expression *expr)
{
	ptrlist_add((struct ptr_list **)list, expr, &C->ptrlist_allocator);
}

static inline int dmrC_expression_list_size(struct expression_list *list)
{
	return ptrlist_size((struct ptr_list *)list);
}

extern int dmrC_test_parse();

#ifdef __cplusplus
}
#endif


#endif /* PARSE_H */

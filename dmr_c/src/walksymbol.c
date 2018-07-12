#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <walksymbol.h>

static void walk_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor);
static void walk_statement(struct dmr_C *C, struct statement *stmt, struct symbol_visitor *visitor);
static void walk_symbol_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor);
static void walk_assignment_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor);
static void walk_binary_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor);
static void walk_preop_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor);
static void walk_postop_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor);
static void walk_call_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor);
static void walk_cast_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor);
static void walk_conditional_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor);
static void walk_label_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor);
static void walk_initializer_expression(struct dmr_C *C, struct expression *expr, struct symbol *ctype,
					struct symbol_visitor *visitor);
static void walk_label(struct dmr_C *C, struct symbol *label, struct symbol_visitor *visitor);
static void walk_return_statement(struct dmr_C *C, struct statement *stmt, struct symbol_visitor *visitor);
static void walk_iterator_statement(struct dmr_C *C, struct statement *stmt, struct symbol_visitor *visitor);
static void walk_switch_statement(struct dmr_C *C, struct statement *stmt, struct symbol_visitor *visitor);

void walk_return_statement(struct dmr_C *C, struct statement *stmt, struct symbol_visitor *visitor)
{
	struct expression *expr = stmt->ret_value;
	struct symbol *target = stmt->ret_target;

	if (expr && expr->ctype) {
		walk_expression(C, expr, visitor);
	}
	visitor->begin_statement(visitor->data, STMT_GOTO);
	dmrC_walk_symbol(C, target, visitor);
	visitor->end_statement(visitor->data);
}

void walk_label(struct dmr_C *C, struct symbol *label, struct symbol_visitor *visitor)
{
	visitor->begin_label(visitor->data, dmrC_show_ident(C, label->ident));
	dmrC_walk_symbol(C, label, visitor);
	visitor->end_label(visitor->data);
}

void walk_iterator_statement(struct dmr_C *C, struct statement *stmt, struct symbol_visitor *visitor)
{
	struct statement *pre_statement = stmt->iterator_pre_statement;
	struct expression *pre_condition = stmt->iterator_pre_condition;
	struct statement *statement = stmt->iterator_statement;
	struct statement *post_statement = stmt->iterator_post_statement;
	struct expression *post_condition = stmt->iterator_post_condition;
	dmrC_walk_symbol_list(C, stmt->iterator_syms, visitor);
	if (pre_statement) {
		visitor->begin_iterator_prestatement(visitor->data);
		walk_statement(C, pre_statement, visitor);
		visitor->end_iterator_prestatement(visitor->data);
	}
	if (pre_condition) {
		visitor->begin_iterator_precondition(visitor->data);
		walk_expression(C, pre_condition, visitor);
		visitor->end_iterator_precondition(visitor->data);
	}
	visitor->begin_iterator_statement(visitor->data);
	walk_statement(C, statement, visitor);
	visitor->end_iterator_statement(visitor->data);
	if (stmt->iterator_continue->used) {
		walk_label(C, stmt->iterator_continue, visitor);
	}
	if (post_statement) {
		visitor->begin_iterator_poststatement(visitor->data);
		walk_statement(C, post_statement, visitor);
		visitor->end_iterator_poststatement(visitor->data);
	}
	if (post_condition) {
		visitor->begin_iterator_postcondition(visitor->data);
		walk_expression(C, post_condition, visitor);
		visitor->end_iterator_postcondition(visitor->data);
	}
	if (stmt->iterator_break->used) {
		walk_label(C, stmt->iterator_break, visitor);
	}
}

void walk_switch_statement(struct dmr_C *C, struct statement *stmt, struct symbol_visitor *visitor)
{
	walk_expression(C, stmt->switch_expression, visitor);
	struct symbol *sym;

	/*
	 * Debugging only: Check that the case list is correct
	 * by printing it out.
	 *
	 * This is where a _real_ back-end would go through the
	 * cases to decide whether to use a lookup table or a
	 * series of comparisons etc
	 */
	FOR_EACH_PTR(stmt->switch_case->symbol_list, sym)
	{
		struct statement *case_stmt = sym->stmt;
		struct expression *expr = case_stmt->case_expression;
		struct expression *to = case_stmt->case_to;

		if (!expr) {
			visitor->begin_default_case(visitor->data);
		} else {
			if (expr->type == EXPR_VALUE) {
				if (to) {
					if (to->type == EXPR_VALUE) {
						visitor->begin_case_range(visitor->data, expr->value, to->value);
					}
				} else {
					visitor->begin_case_value(visitor->data, expr->value);
				}
			}
		}
		dmrC_walk_symbol(C, sym, visitor);
		visitor->end_case(visitor->data);
	}
	END_FOR_EACH_PTR(sym);

	walk_statement(C, stmt->switch_statement, visitor);

	if (stmt->switch_break->used)
		walk_label(C, stmt->switch_break, visitor);
}

void walk_statement(struct dmr_C *C, struct statement *stmt, struct symbol_visitor *visitor)
{
	if (!stmt)
		return;

	visitor->begin_statement(visitor->data, stmt->type);
	switch (stmt->type) {
	case STMT_DECLARATION:
		dmrC_walk_symbol_list(C, stmt->declaration, visitor);
		break;
	case STMT_RETURN:
		walk_return_statement(C, stmt, visitor);
		break;
	case STMT_COMPOUND: {
		struct statement *s;
		// if (stmt->inline_fn) {
		//	dmrC_show_statement(C, stmt->args);
		//	printf("\tbegin_inline \t%s\n", dmrC_show_ident(C,
		// stmt->inline_fn->ident));
		//}
		FOR_EACH_PTR(stmt->stmts, s) { walk_statement(C, s, visitor); }
		END_FOR_EACH_PTR(s);
		if (stmt->ret) {
			walk_label(C, stmt->ret, visitor);
		}
		// if (stmt->inline_fn)
		//	printf("\tend_inlined\t%s\n", dmrC_show_ident(C,
		// stmt->inline_fn->ident));
		break;
	}

	case STMT_EXPRESSION:
		walk_expression(C, stmt->expression, visitor);
		break;
	case STMT_IF: {
		struct expression *cond = stmt->if_conditional;
		walk_expression(C, cond, visitor);
		visitor->begin_if_then(visitor->data);
		walk_statement(C, stmt->if_true, visitor);
		visitor->end_if_then(visitor->data);
		if (stmt->if_false) {
			visitor->begin_if_else(visitor->data);
			walk_statement(C, stmt->if_false, visitor);
			visitor->end_if_else(visitor->data);
		}
		break;
	}
	case STMT_SWITCH:
		walk_switch_statement(C, stmt, visitor);
		break;

	case STMT_CASE:
		walk_label(C, stmt->case_label, visitor);
		walk_statement(C, stmt->case_statement, visitor);
		break;

	case STMT_ITERATOR:
		walk_iterator_statement(C, stmt, visitor);
		break;

	case STMT_NONE:
		break;

	case STMT_LABEL:
		visitor->begin_label(visitor->data, dmrC_show_ident(C, stmt->label_identifier->ident));
		dmrC_walk_symbol(C, stmt->label_identifier, visitor);
		visitor->end_label(visitor->data);
		walk_statement(C, stmt->label_statement, visitor);
		break;

	case STMT_GOTO:
		if (stmt->goto_expression) {
			walk_expression(C, stmt->goto_expression, visitor);
		} else {
			dmrC_walk_symbol(C, stmt->goto_label, visitor);
		}
		break;
	case STMT_ASM:
		// printf("\tasm( .... )\n");
		break;
	case STMT_CONTEXT: {
		// int val = dmrC_show_expression(C, stmt->expression);
		// printf("\tcontext( %d )\n", val);
		break;
	}
	case STMT_RANGE: {
		// int val = dmrC_show_expression(C, stmt->range_expression);
		// int low = dmrC_show_expression(C, stmt->range_low);
		// int high = dmrC_show_expression(C, stmt->range_high);
		// printf("\trange( %d %d-%d)\n", val, low, high);
		break;
	}
	}
	visitor->end_statement(visitor->data);
}

void walk_symbol_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor)
{
	assert(expr->type == EXPR_SYMBOL);
	struct symbol *sym = expr->symbol;
	if (!sym)
		return;
	dmrC_walk_symbol(C, sym, visitor);
}

void walk_assignment_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor)
{
	assert(expr->type == EXPR_ASSIGNMENT);
	if (!expr->ctype)
		return;

	int op = expr->op;
	visitor->begin_assignment_expression(visitor->data, expr->type, op);
	walk_expression(C, expr->left, visitor);
	walk_expression(C, expr->right, visitor);
	visitor->end_assignment_expression(visitor->data);
}

void walk_binary_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor)
{
	assert(expr->type == EXPR_BINOP || expr->type == EXPR_COMPARE || expr->type == EXPR_LOGICAL);
	if (!expr->ctype)
		return;

	int op = expr->op;
	visitor->begin_binop_expression(visitor->data, expr->type, op);
	walk_expression(C, expr->left, visitor);
	walk_expression(C, expr->right, visitor);
	visitor->end_binop_expression(visitor->data);
}

void walk_preop_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor)
{
	assert(expr->type == EXPR_PREOP);
	visitor->begin_preop_expression(visitor->data, expr->type, expr->op);
	walk_expression(C, expr->unop, visitor);
	visitor->end_preop_expression(visitor->data);
}

void walk_postop_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor)
{
	assert(expr->type == EXPR_POSTOP);
	visitor->begin_postop_expression(visitor->data, expr->type, expr->op);
	walk_expression(C, expr->unop, visitor);
	visitor->end_postop_expression(visitor->data);
}

void walk_call_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor)
{
	assert(expr->type == EXPR_CALL);
	if (!expr->ctype) {
		return;
	}
	struct symbol *direct;
	struct expression *arg, *fn;
	fn = expr->fn;

	/* Remove dereference, if any */
	direct = NULL;
	if (fn->type == EXPR_PREOP) {
		if (fn->unop->type == EXPR_SYMBOL) {
			struct symbol *sym = fn->unop->symbol;
			if (sym->ctype.base_type->type == SYM_FN)
				direct = sym;
		}
	}
	if (direct && !direct->aux) {
		dmrC_walk_symbol(C, direct, visitor);
	}
	if (direct) {
		visitor->begin_direct_call_expression(visitor->data, expr->type, dmrC_show_ident(C, direct->ident));
	} else {
		visitor->begin_indirect_call_expression(visitor->data, expr->type);
		walk_expression(C, fn, visitor);
	}
	int n = 0;
	FOR_EACH_PTR(expr->args, arg)
	{
		visitor->begin_callarg_expression(visitor->data, expr->type, ++n);
		walk_expression(C, arg, visitor);
		visitor->end_callarg_expression(visitor->data);
	}
	END_FOR_EACH_PTR(arg);
	visitor->end_call_expression(visitor->data);
}

void walk_cast_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor)
{
	struct symbol *old_type, *new_type;
	int oldbits, newbits;
	int is_signed;

	old_type = expr->cast_expression->ctype;
	new_type = expr->cast_type;

	oldbits = old_type->bit_size;
	newbits = new_type->bit_size;
	is_signed = dmrC_is_signed_type(old_type);
	visitor->begin_cast_expression(visitor->data, expr->type, oldbits, newbits, !is_signed);
	walk_expression(C, expr->cast_expression, visitor);
	dmrC_walk_symbol(C, new_type, visitor);
	visitor->end_cast_expression(visitor->data);
}

void walk_conditional_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor)
{
	visitor->begin_conditional_expression(visitor->data, expr->type);
	walk_expression(C, expr->conditional, visitor);
	walk_expression(C, expr->cond_true, visitor);
	walk_expression(C, expr->cond_false, visitor);
	visitor->end_conditional_expression(visitor->data);
}

void walk_label_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor)
{
	visitor->begin_label_expression(visitor->data, expr->type);
	dmrC_walk_symbol(C, expr->label_symbol, visitor);
	visitor->end_label_expression(visitor->data);
}

static void walk_initialization(struct dmr_C *C, struct symbol *sym, struct expression *expr,
				struct symbol_visitor *visitor)
{
	if (!expr->ctype)
		return;
	visitor->begin_initialization(visitor->data, expr->type);
	walk_expression(C, expr, visitor);
	dmrC_walk_symbol(C, sym, visitor);
	visitor->end_initialization(visitor->data);
}

static void walk_position_expression(struct dmr_C *C, struct expression *expr, struct symbol *base,
				     struct symbol_visitor *visitor)
{
	struct symbol *ctype = expr->init_expr->ctype;
	int bit_offset;

	bit_offset = ctype ? ctype->bit_offset : -1;
	visitor->begin_expression_position(visitor->data, EXPR_POS, expr->init_offset, bit_offset,
					   dmrC_show_ident(C, base->ident));
	walk_expression(C, expr->init_expr, visitor);
	visitor->end_expression_position(visitor->data);
}

void walk_initializer_expression(struct dmr_C *C, struct expression *expr, struct symbol *ctype,
				 struct symbol_visitor *visitor)
{
	struct expression *entry;

	FOR_EACH_PTR(expr->expr_list, entry)
	{

	again:
		// Nested initializers have their positions already
		// recursively calculated - just output them too
		if (entry->type == EXPR_INITIALIZER) {
			walk_initializer_expression(C, entry, ctype, visitor);
			continue;
		}

		// Initializer indexes and identifiers should
		// have been evaluated to EXPR_POS
		if (entry->type == EXPR_IDENTIFIER) {
			visitor->do_expression_identifier(visitor->data, entry->type,
							  dmrC_show_ident(C, entry->expr_ident));
			entry = entry->ident_expression;
			goto again;
		}

		if (entry->type == EXPR_INDEX) {
			visitor->do_expression_index(visitor->data, entry->type, entry->idx_from, entry->idx_to);
			entry = entry->idx_expression;
			goto again;
		}
		if (entry->type == EXPR_POS) {
			walk_position_expression(C, entry, ctype, visitor);
			continue;
		}
		walk_initialization(C, ctype, entry, visitor);
	}
	END_FOR_EACH_PTR(entry);
}

void walk_expression(struct dmr_C *C, struct expression *expr, struct symbol_visitor *visitor)
{
	if (!expr)
		return;
	if (!expr->ctype)
		return;

	visitor->begin_expression(visitor->data, expr->type);
	switch (expr->type) {
	case EXPR_CALL:
		walk_call_expression(C, expr, visitor);
		break;

	case EXPR_ASSIGNMENT:
		walk_assignment_expression(C, expr, visitor);
		break;

	case EXPR_COMMA:
		// return show_comma(C, expr);
		break;
	case EXPR_BINOP:
	case EXPR_COMPARE:
	case EXPR_LOGICAL:
		walk_binary_expression(C, expr, visitor);
		break;
	case EXPR_PREOP:
		walk_preop_expression(C, expr, visitor);
		break;
	case EXPR_POSTOP:
		walk_postop_expression(C, expr, visitor);
		break;
	case EXPR_SYMBOL:
		walk_symbol_expression(C, expr, visitor);
		break;
	case EXPR_DEREF:
	case EXPR_SIZEOF:
	case EXPR_PTRSIZEOF:
	case EXPR_ALIGNOF:
	case EXPR_OFFSETOF:
		break;
	case EXPR_CAST:
	case EXPR_FORCE_CAST:
	case EXPR_IMPLIED_CAST:
		walk_cast_expression(C, expr, visitor);
		break;
	case EXPR_VALUE:
		visitor->int_literal(visitor->data, expr->value, expr->ctype->bit_size,
				     expr->ctype->ctype.modifiers & MOD_UNSIGNED);
		break;
	case EXPR_FVALUE:
		visitor->float_literal(visitor->data, expr->fvalue, expr->ctype->bit_size);
		break;
	case EXPR_STRING:
		visitor->string_literal(visitor->data, dmrC_show_string(C, expr->string));
		break;
	case EXPR_INITIALIZER:
		walk_initializer_expression(C, expr, expr->ctype, visitor);
		break;
	case EXPR_SELECT:
	case EXPR_CONDITIONAL:
		walk_conditional_expression(C, expr, visitor);
		break;
	case EXPR_STATEMENT:
		// return show_statement_expr(C, expr);
		break;
	case EXPR_LABEL:
		walk_label_expression(C, expr, visitor);
		break;
	case EXPR_SLICE:
		// return show_slice(C, expr);
		break;

		// None of these should exist as direct expressions: they are
		// only valid as sub-expressions of initializers.
	case EXPR_POS:
	case EXPR_IDENTIFIER:
	case EXPR_INDEX:
	case EXPR_TYPE:
		break;
	}
	visitor->end_expression(visitor->data);
}

void dmrC_walk_symbol_list(struct dmr_C *C, struct symbol_list *list, struct symbol_visitor *visitor)
{
	struct symbol *sym;

	FOR_EACH_PTR(list, sym) { dmrC_walk_symbol(C, sym, visitor); }
	END_FOR_EACH_PTR(sym);
}

void dmrC_walk_symbol(struct dmr_C *C, struct symbol *sym, struct symbol_visitor *visitor)
{
	if (!sym)
		return;
	if (sym->type != SYM_BASETYPE) {
		if (sym->aux) {
			/* already visited */
			const char *name = sym->ident ? dmrC_show_ident(C, sym->ident) : "";
			visitor->reference_symbol(visitor->data, (uint64_t)sym->aux, name);
			return;
		}
		visitor->id++;
		sym->aux = (void *)visitor->id;
	}
	char name[80] = {0};
	if (sym->ident)
		snprintf(name, sizeof name, "%s", dmrC_show_ident(C, sym->ident));
	else if (sym->type == SYM_BASETYPE)
		snprintf(name, sizeof name, "%s", dmrC_builtin_typename(C, sym));
	struct symbol_info syminfo = {.id = (uint64_t)sym->aux,
				      .name = name,
				      .symbol_namespace = sym->ns,
				      .symbol_type = sym->type,
				      .alignment = sym->ctype.alignment,
				      .pos = sym->pos,
				      .bit_size = sym->bit_size,
				      .offset = sym->offset};
	if (sym->ns == NS_STRUCT && dmrC_is_bitfield_type(sym)) {
		syminfo.bit_offset = sym->bit_offset;
	}
	if (sym->array_size) {
		syminfo.array_size = dmrC_get_expression_value(C, sym->array_size);
	}

	visitor->begin_symbol(visitor->data, &syminfo);

	if (sym->type == SYM_STRUCT || sym->type == SYM_UNION) {
		struct symbol *member;
		visitor->begin_struct_members(visitor->data, &syminfo);
		FOR_EACH_PTR(sym->symbol_list, member) { dmrC_walk_symbol(C, member, visitor); }
		END_FOR_EACH_PTR(member);
		visitor->end_struct_members(visitor->data);
	}

	if (sym->type == SYM_FN) {
		struct symbol *arg;
		visitor->begin_func_arguments(visitor->data, &syminfo);
		FOR_EACH_PTR(sym->arguments, arg) { dmrC_walk_symbol(C, arg, visitor); }
		END_FOR_EACH_PTR(member);
		visitor->end_func_arguments(visitor->data);
	}

	// Is there a base type?
	if (sym->type != SYM_BASETYPE && sym->ctype.base_type) {
		if (sym->type == SYM_FN)
			visitor->begin_func_returntype(visitor->data, &syminfo);
		else
			visitor->begin_basetype(visitor->data, &syminfo);
		dmrC_walk_symbol(C, sym->ctype.base_type, visitor);
		if (sym->type == SYM_FN)
			visitor->end_func_returntype(visitor->data);
		else
			visitor->end_basetype(visitor->data);
	}

	if (sym->type == SYM_FN) {
		if (sym->stmt) {
			visitor->begin_func_body(visitor->data, &syminfo);
			walk_statement(C, sym->stmt, visitor);
			visitor->end_func_body(visitor->data);
		}
	}
	if (sym->initializer) {
		visitor->begin_initializer(visitor->data, &syminfo);
		walk_expression(C, sym->initializer, visitor);
		visitor->end_initializer(visitor->data);
	}
	visitor->end_symbol(visitor->data);
}

static void begin_symbol_default(void *data, struct symbol_info *syminfo)
{
	(void)data;
	(void)syminfo;
}
static void end_symbol_default(void *data) { (void)data; }
static void begin_members_default(void *data, struct symbol_info *syminfo)
{
	(void)data;
	(void)syminfo;
}
static void end_members_default(void *data) { (void)data; }
static void begin_arguments_default(void *data, struct symbol_info *syminfo)
{
	(void)data;
	(void)syminfo;
}
static void end_arguments_default(void *data) { (void)data; }
static void reference_symbol_default(void *data, uint64_t id, const char *name)
{
	(void)data;
	(void)id;
	(void)name;
}
static void begin_body_default(void *data, struct symbol_info *syminfo)
{
	(void)data;
	(void)syminfo;
}
static void end_body_default(void *data) { (void)data; }
static void begin_func_returntype_default(void *data, struct symbol_info *syminfo)
{
	(void)data;
	(void)syminfo;
}
static void end_func_returntype_default(void *data) { (void)data; }
static void begin_basetype_default(void *data, struct symbol_info *syminfo)
{
	(void)data;
	(void)syminfo;
}
static void end_basetype_default(void *data) { (void)data; }
static void begin_initializer_default(void *data, struct symbol_info *syminfo)
{
	(void)data;
	(void)syminfo;
}
static void end_initializer_default(void *data) { (void)data; }
static void string_expression_default(void *data, const char *str)
{
	(void)data;
	(void)str;
}
static void int_literal_default(void *data, long long value, int bit_size, bool is_unsigned)
{
	(void)data;
	(void)value;
	(void)bit_size;
	(void)is_unsigned;
}
static void float_literal_default(void *data, long double fvalue, int bit_size)
{
	(void)data;
	(void)fvalue;
	(void)bit_size;
}
static void begin_statement_default(void *data, enum statement_type statement_type)
{
	(void)data;
	(void)statement_type;
}
static void end_statement_default(void *data) { (void)data; }
static void begin_expression_default(void *data, enum expression_type expr_type)
{
	(void)data;
	(void)expr_type;
}
static void end_expression_default(void *data) { (void)data; }
static void begin_assignment_expression_default(void *data, enum expression_type expr_type, int op)
{
	(void)data;
	(void)expr_type;
	(void)op;
}
static void end_assignment_expression_default(void *data) { (void)data; }
static void begin_binop_expression_default(void *data, enum expression_type expr_type, int op)
{
	(void)data;
	(void)expr_type;
	(void)op;
}
static void end_binop_expression_default(void *data) { (void)data; }
static void begin_preop_expression_default(void *data, enum expression_type expr_type, int op)
{
	(void)data;
	(void)expr_type;
	(void)op;
}
static void end_preop_expression_default(void *data) { (void)data; }
static void begin_postop_expression_default(void *data, enum expression_type expr_type, int op)
{
	(void)data;
	(void)expr_type;
	(void)op;
}
static void end_postop_expression_default(void *data) { (void)data; }
static void begin_direct_call_expression_default(void *data, enum expression_type expr_type, const char *name)
{
	(void)data;
	(void)expr_type;
	(void)name;
}
static void begin_indirect_call_expression_default(void *data, enum expression_type expr_type)
{
	(void)data;
	(void)expr_type;
}
static void end_call_expression_default(void *data) { (void)data; }

static void begin_callarg_expression_default(void *data, enum expression_type expr_type, int argpos)
{
	(void)data;
	(void)expr_type;
	(void)argpos;
}
static void end_callarg_expression_default(void *data) { (void)data; }

static void begin_cast_expression_default(void *data, enum expression_type expr_type, int oldbits, int newbits,
					  bool is_unsigned)
{
	(void)data;
	(void)expr_type;
	(void)oldbits;
	(void)newbits;
	(void)is_unsigned;
}
static void end_cast_expression_default(void *data) { (void)data; }
static void begin_conditional_expression_default(void *data, enum expression_type expr_type)
{
	(void)data;
	(void)expr_type;
}
static void end_conditional_expression_default(void *data) { (void)data; }
static void begin_label_expression_default(void *data, enum expression_type expr_type)
{
	(void)data;
	(void)expr_type;
}
static void end_label_expression_default(void *data) { (void)data; }
static void do_expression_identifier_default(void *data, enum expression_type expr_type, const char *ident)
{
	(void)data;
	(void)expr_type;
	(void)ident;
}
static void do_expression_index_default(void *data, enum expression_type expr_type, unsigned from, unsigned to)
{
	(void)data;
	(void)expr_type;
	(void)from;
	(void)to;
}
static void begin_expression_position_default(void *data, enum expression_type expr_type, unsigned init_offset,
					      int bit_offset, const char *ident)
{
	(void)data;
	(void)expr_type;
	(void)init_offset;
	(void)ident;
	(void)bit_offset;
}
static void end_expression_position_default(void *data) { (void)data; }
static void begin_initialization_default(void *data, enum expression_type expr_type)
{
	(void)data;
	(void)expr_type;
}
static void end_initialization_default(void *data) { (void)data; }
static void begin_label_default(void *data, const char *name)
{
	(void)data;
	(void)name;
}
static void end_label_default(void *data) { (void)data; }
static void begin_iterator_prestatement_default(void *data) { (void)data; }
static void end_iterator_prestatement_default(void *data) { (void)data; }
static void begin_iterator_precondition_default(void *data) { (void)data; }
static void end_iterator_precondition_default(void *data) { (void)data; }
static void begin_iterator_statement_default(void *data) { (void)data; }
static void end_iterator_statement_default(void *data) { (void)data; }
static void begin_iterator_postcondition_default(void *data) { (void)data; }
static void end_iterator_postcondition_default(void *data) { (void)data; }
static void begin_iterator_poststatement_default(void *data) { (void)data; }
static void end_iterator_poststatement_default(void *data) { (void)data; }
static void begin_case_value_default(void *data, long long value)
{
	(void)data;
	(void)value;
}
static void begin_case_range_default(void *data, long long from, long long to)
{
	(void)data;
	(void)from;
	(void)to;
}
static void begin_default_case_default(void *data) { (void)data; }
static void end_case_default(void *data) { (void)data; }

static void begin_if_then_default(void *data) { (void)data; }
static void end_if_then_default(void *data) { (void)data; }
static void begin_if_else_default(void *data) { (void)data; }
static void end_if_else_default(void *data) { (void)data; }

void dmrC_init_symbol_visitor(struct symbol_visitor *visitor)
{
	visitor->data = NULL;
	visitor->id = 0;
	visitor->begin_symbol = begin_symbol_default;
	visitor->end_symbol = end_symbol_default;
	visitor->begin_struct_members = begin_members_default;
	visitor->end_struct_members = end_members_default;
	visitor->begin_func_arguments = begin_arguments_default;
	visitor->end_func_arguments = end_arguments_default;
	visitor->reference_symbol = reference_symbol_default;
	visitor->begin_func_body = begin_body_default;
	visitor->end_func_body = end_body_default;
	visitor->begin_func_returntype = begin_func_returntype_default;
	visitor->end_func_returntype = end_func_returntype_default;
	visitor->begin_basetype = begin_basetype_default;
	visitor->end_basetype = end_basetype_default;
	visitor->begin_initializer = begin_initializer_default;
	visitor->end_initializer = end_initializer_default;
	visitor->string_literal = string_expression_default;
	visitor->float_literal = float_literal_default;
	visitor->int_literal = int_literal_default;
	visitor->begin_statement = begin_statement_default;
	visitor->end_statement = end_statement_default;
	visitor->begin_expression = begin_expression_default;
	visitor->end_expression = end_expression_default;
	visitor->begin_assignment_expression = begin_assignment_expression_default;
	visitor->end_assignment_expression = end_assignment_expression_default;
	visitor->begin_binop_expression = begin_binop_expression_default;
	visitor->end_binop_expression = end_binop_expression_default;
	visitor->begin_preop_expression = begin_preop_expression_default;
	visitor->end_preop_expression = end_preop_expression_default;
	visitor->begin_postop_expression = begin_postop_expression_default;
	visitor->end_postop_expression = end_postop_expression_default;
	visitor->begin_direct_call_expression = begin_direct_call_expression_default;
	visitor->begin_indirect_call_expression = begin_indirect_call_expression_default;
	visitor->end_call_expression = end_call_expression_default;
	visitor->begin_callarg_expression = begin_callarg_expression_default;
	visitor->end_callarg_expression = end_callarg_expression_default;
	visitor->begin_cast_expression = begin_cast_expression_default;
	visitor->end_cast_expression = end_cast_expression_default;
	visitor->begin_conditional_expression = begin_conditional_expression_default;
	visitor->end_conditional_expression = end_conditional_expression_default;
	visitor->begin_label_expression = begin_label_expression_default;
	visitor->end_label_expression = end_label_expression_default;
	visitor->do_expression_identifier = do_expression_identifier_default;
	visitor->do_expression_index = do_expression_index_default;
	visitor->begin_expression_position = begin_expression_position_default;
	visitor->end_expression_position = end_expression_position_default;
	visitor->begin_initialization = begin_initialization_default;
	visitor->end_initialization = end_initialization_default;
	visitor->begin_label = begin_label_default;
	visitor->end_label = end_label_default;
	visitor->begin_iterator_prestatement = begin_iterator_prestatement_default;
	visitor->end_iterator_prestatement = end_iterator_prestatement_default;
	visitor->begin_iterator_precondition = begin_iterator_precondition_default;
	visitor->end_iterator_precondition = end_iterator_precondition_default;
	visitor->begin_iterator_statement = begin_iterator_statement_default;
	visitor->end_iterator_statement = end_iterator_statement_default;
	visitor->begin_iterator_postcondition = begin_iterator_postcondition_default;
	visitor->end_iterator_postcondition = end_iterator_postcondition_default;
	visitor->begin_iterator_poststatement = begin_iterator_poststatement_default;
	visitor->end_iterator_poststatement = end_iterator_poststatement_default;
	visitor->begin_case_value = begin_case_value_default;
	visitor->begin_case_range = begin_case_range_default;
	visitor->begin_default_case = begin_default_case_default;
	visitor->end_case = end_case_default;
	visitor->begin_if_then = begin_if_then_default;
	visitor->end_if_then = end_if_then_default;
	visitor->begin_if_else = begin_if_else_default;
	visitor->end_if_else = end_if_else_default;
}

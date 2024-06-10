/******************************************************************************
 * Copyright (C) 2020-2024 Dibyendu Majumdar
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

#include "parser.h"

/*
The Type checker walks through the AST and annotates nodes with type information.
It also checks that the operations are valid.

There are following assumptions made about the code generator backend.

a) Function arguments that have type info must be asserted at runtime
b) Local variable assignments in 'local' or expression statements must be asserted by the backend
c) We allow assigning integer value to number and vice versa in the AST but the code generator must assert this is valid
d) Any unassigned local vars that have type info must be set to valid initial values.

None of these operations are explicit in the AST.
*/

static bool is_type_same(const VariableType *a, const VariableType *b)
{
	// String comparion of type_name relies upon strings being interned
	return a->type_code == b->type_code && a->type_name == b->type_name;
}

static void handle_error(CompilerState *compiler_state, const char *msg)
{
	// TODO source and line number
	raviX_buffer_add_string(&compiler_state->error_message, msg);
	longjmp(compiler_state->env, 1);
}

/* Type checker - WIP  */
static void typecheck_ast_node(CompilerState *compiler_state, AstNode *function, AstNode *node);

/* Type checker - WIP  */
static void typecheck_ast_list(CompilerState *compiler_state, AstNode *function, AstNodeList *list)
{
	AstNode *node;
	FOR_EACH_PTR(list, AstNode, node) { typecheck_ast_node(compiler_state, function, node); }
	END_FOR_EACH_PTR(node);
}

/* Type checker - WIP  */
static void typecheck_unary_operator(CompilerState *compiler_state, AstNode *function, AstNode *node)
{
	UnaryOperatorType op = node->unary_expr.unary_op;
	typecheck_ast_node(compiler_state, function, node->unary_expr.expr);
	ravitype_t subexpr_type = node->unary_expr.expr->common_expr.type.type_code;
	switch (op) {
	case UNOPR_MINUS:
		if (subexpr_type == RAVI_TNUMINT) {
			set_type(&node->unary_expr.type, RAVI_TNUMINT);
		} else if (subexpr_type == RAVI_TNUMFLT) {
			set_type(&node->unary_expr.type, RAVI_TNUMFLT);
		}
		break;
	case UNOPR_LEN:
		if (subexpr_type == RAVI_TARRAYINT || subexpr_type == RAVI_TARRAYFLT) {
			set_type(&node->unary_expr.type, RAVI_TNUMINT);
		}
		break;
	case UNOPR_TO_INTEGER:
		if (node->unary_expr.expr->common_expr.type.type_code != RAVI_TANY &&
		    node->unary_expr.expr->common_expr.type.type_code != RAVI_TNUMINT) {
			handle_error(compiler_state, "Cannot convert type to integer");
		}
		set_type(&node->unary_expr.type, RAVI_TNUMINT);
		break;
	case UNOPR_TO_NUMBER:
		if (node->unary_expr.expr->common_expr.type.type_code != RAVI_TANY &&
		    node->unary_expr.expr->common_expr.type.type_code != RAVI_TNUMINT &&
		    node->unary_expr.expr->common_expr.type.type_code != RAVI_TNUMFLT) {
			handle_error(compiler_state, "Cannot convert type to number");
		}
		set_type(&node->unary_expr.type, RAVI_TNUMFLT);
		break;
	case UNOPR_TO_CLOSURE:
		if (node->unary_expr.expr->common_expr.type.type_code != RAVI_TANY &&
		    node->unary_expr.expr->common_expr.type.type_code != RAVI_TFUNCTION) {
			handle_error(compiler_state, "Cannot convert type to function");
		}
		set_type(&node->unary_expr.type, RAVI_TFUNCTION);
		break;
	case UNOPR_TO_STRING:
		if (node->unary_expr.expr->common_expr.type.type_code != RAVI_TANY &&
		    node->unary_expr.expr->common_expr.type.type_code != RAVI_TSTRING) {
			handle_error(compiler_state, "Cannot convert type to string");
		}
		set_type(&node->unary_expr.type, RAVI_TSTRING);
		break;
	case UNOPR_TO_INTARRAY:
		if (node->unary_expr.expr->type == EXPR_TABLE_LITERAL &&
		    (node->unary_expr.expr->table_expr.inferred_type_code == RAVI_TARRAYINT ||
		     raviX_ptrlist_size((PtrList *)node->unary_expr.expr->table_expr.expr_list) == 0)) {
			set_type(&node->unary_expr.expr->table_expr.type, RAVI_TARRAYINT);
		} else if (node->unary_expr.expr->common_expr.type.type_code != RAVI_TANY &&
			   node->unary_expr.expr->common_expr.type.type_code != RAVI_TARRAYINT) {
			handle_error(compiler_state, "Cannot convert type to integer[]");
		}
		set_type(&node->unary_expr.type, RAVI_TARRAYINT);
		break;
	case UNOPR_TO_NUMARRAY:
		if (node->unary_expr.expr->type == EXPR_TABLE_LITERAL &&
		    (node->unary_expr.expr->table_expr.inferred_type_code == RAVI_TARRAYFLT ||
		     raviX_ptrlist_size((PtrList *)node->unary_expr.expr->table_expr.expr_list) == 0)) {
			set_type(&node->unary_expr.expr->table_expr.type, RAVI_TARRAYFLT);
		} else if (node->unary_expr.expr->common_expr.type.type_code != RAVI_TANY &&
			   node->unary_expr.expr->common_expr.type.type_code != RAVI_TARRAYFLT) {
			handle_error(compiler_state, "Cannot convert type to number[]");
		}
		set_type(&node->unary_expr.type, RAVI_TARRAYFLT);
		break;
	case UNOPR_TO_TABLE:
		if (node->unary_expr.expr->common_expr.type.type_code != RAVI_TANY &&
		    node->unary_expr.expr->common_expr.type.type_code != RAVI_TTABLE) {
			handle_error(compiler_state, "Cannot convert type to table");
		}
		set_type(&node->unary_expr.type, RAVI_TTABLE);
		break;
	case UNOPR_TO_TYPE:
		if (node->unary_expr.expr->common_expr.type.type_code != RAVI_TANY &&
		    node->unary_expr.expr->common_expr.type.type_code != RAVI_TUSERDATA) {
			handle_error(compiler_state, "Cannot convert type to usertype");
		}
		assert(node->unary_expr.type.type_name != NULL); // Should already be set by the parser
		set_typecode(&node->unary_expr.type, RAVI_TUSERDATA);
		break;
	default:
		break;
	}
}

/* Type checker - WIP  */
static void typecheck_binary_operator(CompilerState *compiler_state, AstNode *function, AstNode *node)
{
	BinaryOperatorType op = node->binary_expr.binary_op;
	AstNode *e1 = node->binary_expr.expr_left;
	AstNode *e2 = node->binary_expr.expr_right;
	typecheck_ast_node(compiler_state, function, e1);
	typecheck_ast_node(compiler_state, function, e2);
	switch (op) {
	case BINOPR_ADD:
	case BINOPR_SUB:
	case BINOPR_MUL:
	case BINOPR_DIV:
		if (e1->common_expr.type.type_code == RAVI_TNUMFLT && e2->common_expr.type.type_code == RAVI_TNUMFLT)
			set_typecode(&node->binary_expr.type, RAVI_TNUMFLT);
		else if (e1->common_expr.type.type_code == RAVI_TNUMFLT &&
			 e2->common_expr.type.type_code == RAVI_TNUMINT)
			set_typecode(&node->binary_expr.type, RAVI_TNUMFLT);
		else if (e1->common_expr.type.type_code == RAVI_TNUMINT &&
			 e2->common_expr.type.type_code == RAVI_TNUMFLT)
			set_typecode(&node->binary_expr.type, RAVI_TNUMFLT);
		else if (op != BINOPR_DIV && e1->common_expr.type.type_code == RAVI_TNUMINT &&
			 e2->common_expr.type.type_code == RAVI_TNUMINT)
			set_typecode(&node->binary_expr.type, RAVI_TNUMINT);
		else if (op == BINOPR_DIV && e1->common_expr.type.type_code == RAVI_TNUMINT &&
			 e2->common_expr.type.type_code == RAVI_TNUMINT)
			set_typecode(&node->binary_expr.type, RAVI_TNUMFLT);
		break;
	case BINOPR_IDIV:
		if (e1->common_expr.type.type_code == RAVI_TNUMINT && e2->common_expr.type.type_code == RAVI_TNUMINT)
			set_typecode(&node->binary_expr.type, RAVI_TNUMINT);
		// FIXME missing cases
		break;
	case BINOPR_BAND:
	case BINOPR_BOR:
	case BINOPR_BXOR:
	case BINOPR_SHL:
	case BINOPR_SHR:
		if ((e1->common_expr.type.type_code == RAVI_TNUMFLT ||
		     e1->common_expr.type.type_code == RAVI_TNUMINT) &&
		    (e2->common_expr.type.type_code == RAVI_TNUMFLT || e2->common_expr.type.type_code == RAVI_TNUMINT))
			set_typecode(&node->binary_expr.type, RAVI_TNUMINT);
		break;
	case BINOPR_EQ:
	case BINOPR_NE:
	case BINOPR_GE:
	case BINOPR_GT:
	case BINOPR_LE:
	case BINOPR_LT:
		/* This case is not handled in default parser - why? */
		if ((e1->common_expr.type.type_code == RAVI_TNUMINT || e1->common_expr.type.type_code == RAVI_TNUMFLT ||
		     e1->common_expr.type.type_code == RAVI_TBOOLEAN) &&
		    (e2->common_expr.type.type_code == RAVI_TNUMFLT || e2->common_expr.type.type_code == RAVI_TNUMINT ||
		     e2->common_expr.type.type_code == RAVI_TBOOLEAN))
			set_typecode(&node->binary_expr.type, RAVI_TBOOLEAN);
		break;
	case BINOPR_POW:
		if ((e1->common_expr.type.type_code == RAVI_TNUMFLT ||
		     e1->common_expr.type.type_code == RAVI_TNUMINT) &&
		    (e2->common_expr.type.type_code == RAVI_TNUMFLT || e2->common_expr.type.type_code == RAVI_TNUMINT))
			set_typecode(&node->binary_expr.type, RAVI_TNUMFLT);
		break;
	case BINOPR_MOD:
		if (e1->common_expr.type.type_code == RAVI_TNUMINT && e2->common_expr.type.type_code == RAVI_TNUMINT)
			set_typecode(&node->binary_expr.type, RAVI_TNUMINT);
		else if ((e1->common_expr.type.type_code == RAVI_TNUMINT &&
			  e2->common_expr.type.type_code == RAVI_TNUMFLT) ||
			 (e1->common_expr.type.type_code == RAVI_TNUMFLT &&
			  e2->common_expr.type.type_code == RAVI_TNUMINT))
			set_typecode(&node->binary_expr.type, RAVI_TNUMFLT);
		break;
	default:
		set_typecode(&node->binary_expr.type, RAVI_TANY);
		break;
	}
}

static bool is_unindexable_type(VariableType *type)
{
	switch (type->type_code) {
	case RAVI_TNUMFLT:
	case RAVI_TNUMINT:
	case RAVI_TBOOLEAN:
	case RAVI_TNIL:
		return true;
	default:
		return false;
	}
}

/*
 * Suffixed expression examples:
 * f()[1]
 * x[1][2]
 * x.y[1]
 */
static void typecheck_suffixedexpr(CompilerState *compiler_state, AstNode *function, AstNode *node)
{
	typecheck_ast_node(compiler_state, function, node->suffixed_expr.primary_expr);
	AstNode *prev_node = node->suffixed_expr.primary_expr;
	AstNode *this_node;
	FOR_EACH_PTR(node->suffixed_expr.suffix_list, AstNode, this_node)
	{
		typecheck_ast_node(compiler_state, function, this_node);
		if (this_node->type == EXPR_Y_INDEX) {
			if (prev_node->common_expr.type.type_code == RAVI_TARRAYFLT) {
				if (this_node->index_expr.expr->common_expr.type.type_code == RAVI_TNUMINT) {
					set_typecode(&this_node->index_expr.type, RAVI_TNUMFLT);
				} else {
					handle_error(compiler_state, "invalid type in index");
				}
			} else if (prev_node->common_expr.type.type_code == RAVI_TARRAYINT) {
				if (this_node->index_expr.expr->common_expr.type.type_code == RAVI_TNUMINT) {
					set_typecode(&this_node->index_expr.type, RAVI_TNUMINT);
				} else {
					handle_error(compiler_state, "invalid type in index");
				}
			} else if (is_unindexable_type(&prev_node->common_expr.type)) {
				handle_error(compiler_state, "invalid type in index");
			}
		}
		prev_node = this_node;
	}
	END_FOR_EACH_PTR(node);
	copy_type(&node->suffixed_expr.type, &prev_node->common_expr.type);
}

static void typecheck_var_assignment(CompilerState *compiler_state, VariableType *var_type, AstNode *expr,
				     const StringObject *var_name)
{
	if (var_type->type_code == RAVI_TANY)
		// Any value can be assigned to type ANY
		return;
	const char *variable_name = var_name ? var_name->str : "unknown-TODO";
	VariableType *expr_type = &expr->common_expr.type;

	// If expr is ANY type then allow it as a runtime check will be
	// done
	if (expr_type->type_code == RAVI_TANY) {
		return;
	}

	if (var_type->type_code == RAVI_TNUMINT) {
		/* if the expr is of type number or # operator then insert @integer operator */
		if (expr_type->type_code == RAVI_TNUMFLT ||
		    (expr->type == EXPR_UNARY && expr->unary_expr.unary_op == UNOPR_LEN)) {
			/* Okay, but backend must do appropriate conversion */
			;
		} else if (expr_type->type_code != RAVI_TNUMINT) {
			char tempbuf[256];
			snprintf(tempbuf, sizeof tempbuf, "%d: Assignment to local symbol %s is not type compatible\n",
				 expr->line_number, variable_name);
			handle_error(compiler_state, tempbuf);
		}
		return;
	}
	if (var_type->type_code == RAVI_TNUMFLT) {
		if (expr_type->type_code == RAVI_TNUMINT) {
			/* Okay, but backend must do appropriate conversion */
			;
		} else if (expr_type->type_code != RAVI_TNUMFLT) {
			char tempbuf[256];
			snprintf(tempbuf, sizeof tempbuf, "%d: Assignment to local symbol %s is not type compatible\n",
				 expr->line_number, variable_name);
			handle_error(compiler_state, tempbuf);
		}
		return;
	}
	if (var_type->type_code == RAVI_TARRAYFLT) {
		if (expr->type == EXPR_TABLE_LITERAL && expr_type->type_code == RAVI_TTABLE &&
		    (expr->table_expr.inferred_type_code == RAVI_TARRAYFLT ||
		     raviX_ptrlist_size((PtrList *)expr->table_expr.expr_list) == 0)) {
			// Note following updates the AST node too
			expr_type->type_code = RAVI_TARRAYFLT;
		}
	} else if (var_type->type_code == RAVI_TARRAYINT) {
		if (expr->type == EXPR_TABLE_LITERAL && expr_type->type_code == RAVI_TTABLE &&
		    (expr->table_expr.inferred_type_code == RAVI_TARRAYINT ||
		     raviX_ptrlist_size((PtrList *)expr->table_expr.expr_list) == 0)) {
			// Note following updates the AST node too
			expr_type->type_code = RAVI_TARRAYINT;
		}
	}
	if (expr_type->type_code == RAVI_TNIL && (var_type->type_code & RAVI_TM_NIL) != 0) {
		// NILable type so allow nil
		return;
	}
	// all other types must strictly match
	if (!is_type_same(var_type, expr_type)) { // We should probably check type convert-ability here
		char tempbuf[256];
		snprintf(tempbuf, sizeof tempbuf, "%d: Assignment to local symbol %s is not type compatible\n",
			 expr->line_number, variable_name);
		handle_error(compiler_state, tempbuf);
	}
}

/**
 * If the local symbol was initialized with a constant (literal) and if the
 * variable is never subsequently modified then we can assign the type of the
 * constant to the local symbol
 */
static void typecheck_local_symbol(LuaSymbol *sym) {
	if (sym->symbol_type != SYM_LOCAL
	    || !sym->variable.literal_initializer
	    || sym->variable.modified)
		return;
	AstNode *litexpr = sym->variable.literal_initializer;
	ravitype_t type_code = litexpr->common_expr.type.type_code;
	if (type_code == RAVI_TNUMINT ||
	       type_code == RAVI_TNUMFLT ||
	       type_code == RAVI_TSTRING) {
		if (sym->variable.value_type.type_code == RAVI_TANY)
			sym->variable.value_type.type_code = type_code;
	}
}

static void typecheck_local_statement(CompilerState *compiler_state, AstNode *function, AstNode *node)
{
	// The local vars should already be annotated
	// We need to typecheck the expressions to the right of =
	// Then we need to ensure that the assignments are valid
	// We can perhaps insert type assertions where we have a mismatch?

	typecheck_ast_list(compiler_state, function, node->local_stmt.expr_list);

	LuaSymbol *var;
	AstNode *expr;
	PREPARE_PTR_LIST(node->local_stmt.var_list, LuaSymbol, var);
	PREPARE_PTR_LIST(node->local_stmt.expr_list, AstNode, expr);

	for (;;) {
		if (!var || !expr)
			break;
		VariableType *var_type = &var->variable.value_type;
		const StringObject *var_name = var->variable.var_name;

		typecheck_local_symbol(var);
		typecheck_var_assignment(compiler_state, var_type, expr, var_name);

		NEXT_PTR_LIST(LuaSymbol, var);
		NEXT_PTR_LIST(AstNode, expr);
	}
}

static void typecheck_expr_statement(CompilerState *compiler_state, AstNode *function, AstNode *node)
{
	if (node->expression_stmt.var_expr_list)
		typecheck_ast_list(compiler_state, function, node->expression_stmt.var_expr_list);
	typecheck_ast_list(compiler_state, function, node->expression_stmt.expr_list);

	if (!node->expression_stmt.var_expr_list)
		return;

	AstNode *var;
	AstNode *expr;
	PREPARE_PTR_LIST(node->expression_stmt.var_expr_list, AstNode, var);
	PREPARE_PTR_LIST(node->local_stmt.expr_list, AstNode, expr);

	for (;;) {
		if (!var || !expr)
			break;

		VariableType *var_type = &var->common_expr.type;
		const StringObject *var_name = NULL; // FIXME how do we get this?

		typecheck_var_assignment(compiler_state, var_type, expr, var_name);

		NEXT_PTR_LIST(AstNode, var);
		NEXT_PTR_LIST(AstNode, expr);
	}
}

static void typecheck_for_in_statment(CompilerState *compiler_state, AstNode *function, AstNode *node)
{
	typecheck_ast_list(compiler_state, function, node->for_stmt.expr_list);
	typecheck_ast_list(compiler_state, function, node->for_stmt.for_statement_list);
}

static void typecheck_for_num_statment(CompilerState *compiler_state, AstNode *function, AstNode *node)
{
	typecheck_ast_list(compiler_state, function, node->for_stmt.expr_list);
	AstNode *expr;
	enum { I = 1, F = 2, A = 4 }; /* bits representing integer, number, any */
	int index_type = 0;
	FOR_EACH_PTR(node->for_stmt.expr_list, AstNode, expr)
	{
		switch (expr->common_expr.type.type_code) {
		case RAVI_TNUMFLT:
			index_type |= F;
			break;
		case RAVI_TNUMINT:
			index_type |= I;
			break;
		default:
			index_type |= A;
			break;
		}
		if ((index_type & A) != 0)
			break;
	}
	END_FOR_EACH_PTR(expr)
	if ((index_type & A) == 0) { /* not any */
		/* for I+F we use F */
		ravitype_t symbol_type = index_type == I ? RAVI_TNUMINT : RAVI_TNUMFLT;
		LuaSymbolList *symbols = node->for_stmt.symbols;
		LuaSymbol *sym;
		/* actually there will be only index variable */
		FOR_EACH_PTR(symbols, LuaSymbol, sym)
		{
			if (sym->symbol_type == SYM_LOCAL) {
				set_typecode(&sym->variable.value_type, symbol_type);
			} else {
				assert(0); /* cannot happen */
			}
		}
		END_FOR_EACH_PTR(sym)
	}
	else {
		FOR_EACH_PTR(node->for_stmt.expr_list, AstNode, expr)
		{
			if (expr->common_expr.type.type_code != RAVI_TNUMINT) {
				AstNode *cast_to_int_expr = raviX_cast_to_integer(compiler_state, expr);
				REPLACE_CURRENT_PTR(AstNode, expr, cast_to_int_expr);
			}
		}
		END_FOR_EACH_PTR(expr)
	}
	typecheck_ast_list(compiler_state, function, node->for_stmt.for_statement_list);
}

static void typecheck_if_statement(CompilerState *compiler_state, AstNode *function, AstNode *node)
{
	AstNode *test_then_block;
	FOR_EACH_PTR(node->if_stmt.if_condition_list, AstNode, test_then_block)
	{
		typecheck_ast_node(compiler_state, function, test_then_block->test_then_block.condition);
		typecheck_ast_list(compiler_state, function, test_then_block->test_then_block.test_then_statement_list);
	}
	END_FOR_EACH_PTR(node)
	if (node->if_stmt.else_statement_list) {
		typecheck_ast_list(compiler_state, function, node->if_stmt.else_statement_list);
	}
}

static void typecheck_while_or_repeat_statement(CompilerState *compiler_state, AstNode *function, AstNode *node)
{
	typecheck_ast_node(compiler_state, function, node->while_or_repeat_stmt.condition);
	if (node->while_or_repeat_stmt.loop_statement_list) {
		typecheck_ast_list(compiler_state, function, node->while_or_repeat_stmt.loop_statement_list);
	}
}

static void infer_table_type(CompilerState *compiler_state, AstNode *function, AstNode *table_expr)
{
	AstNode *node;
	unsigned values_tag = 0;
	FOR_EACH_PTR(table_expr->table_expr.expr_list, AstNode, node)
	{
		if (node->type != EXPR_TABLE_ELEMENT_ASSIGN || node->table_elem_assign_expr.key_expr != NULL) {
			values_tag |= 4;
			continue;
		}
		if (node->common_expr.type.type_code == RAVI_TNUMINT)
			values_tag |= 1;
		else if (node->common_expr.type.type_code == RAVI_TNUMFLT)
			values_tag |= 2;
		else
			values_tag |= 4;
	}
	END_FOR_EACH_PTR(node)
	if (values_tag == 1)
		table_expr->table_expr.inferred_type_code = RAVI_TARRAYINT;
	else if (values_tag == 2)
		table_expr->table_expr.inferred_type_code = RAVI_TARRAYFLT;
}

/* Type checker - WIP  */
static void typecheck_ast_node(CompilerState *compiler_state, AstNode *function, AstNode *node)
{
	switch (node->type) {
	case EXPR_FUNCTION: {
		/* args need type assertions but those have no ast - i.e. code gen should do it */
		typecheck_ast_list(compiler_state, function, node->function_expr.function_statement_list);
		break;
	}
	case AST_NONE: {
		break;
	}
	case STMT_RETURN: {
		typecheck_ast_list(compiler_state, function, node->return_stmt.expr_list);
		break;
	}
	case STMT_LOCAL: {
		typecheck_local_statement(compiler_state, function, node);
		break;
	}
	case STMT_FUNCTION: {
		typecheck_ast_node(compiler_state, function, node->function_stmt.function_expr);
		break;
	}
	case STMT_LABEL: {
		break;
	}
	case STMT_GOTO: {
		break;
	}
	case STMT_DO: {
		typecheck_ast_list(compiler_state, function, node->do_stmt.do_statement_list);
		break;
	}
	case STMT_EXPR: {
		typecheck_expr_statement(compiler_state, function, node);
		break;
	}
	case STMT_IF: {
		typecheck_if_statement(compiler_state, function, node);
		break;
	}
	case STMT_WHILE:
	case STMT_REPEAT: {
		typecheck_while_or_repeat_statement(compiler_state, function, node);
		break;
	}
	case STMT_FOR_IN: {
		typecheck_for_in_statment(compiler_state, function, node);
		break;
	}
	case STMT_FOR_NUM: {
		typecheck_for_num_statment(compiler_state, function, node);
		break;
	}
	case STMT_EMBEDDED_C: {
		break;
	}
	case EXPR_SUFFIXED: {
		typecheck_suffixedexpr(compiler_state, function, node);
		break;
	}
	case EXPR_FUNCTION_CALL: {
		if (node->function_call_expr.method_name) {
		} else {
		}
		typecheck_ast_list(compiler_state, function, node->function_call_expr.arg_list);
		break;
	}
	case EXPR_SYMBOL: {
		/* symbol type should have been set when symbol was created */
		if (node->symbol_expr.var->symbol_type != SYM_LABEL) {
			copy_type(&node->symbol_expr.type, &node->symbol_expr.var->variable.value_type);
		} else {
			// TODO can this happen?
			node->symbol_expr.type.type_code = RAVI_TANY;
		}
		break;
	}
	case EXPR_BINARY: {
		typecheck_binary_operator(compiler_state, function, node);
		break;
	}
	case EXPR_UNARY: {
		typecheck_unary_operator(compiler_state, function, node);
		break;
	}
	case EXPR_LITERAL: {
		/* type set during parsing */
		break;
	}
	case EXPR_FIELD_SELECTOR: {
		typecheck_ast_node(compiler_state, function, node->index_expr.expr);
		break;
	}
	case EXPR_Y_INDEX: {
		typecheck_ast_node(compiler_state, function, node->index_expr.expr);
		break;
	}
	case EXPR_TABLE_ELEMENT_ASSIGN: {
		if (node->table_elem_assign_expr.key_expr) {
			typecheck_ast_node(compiler_state, function, node->table_elem_assign_expr.key_expr);
		}
		typecheck_ast_node(compiler_state, function, node->table_elem_assign_expr.value_expr);
		copy_type(&node->table_elem_assign_expr.type,
			  &node->table_elem_assign_expr.value_expr->common_expr.type);
		break;
	}
	case EXPR_TABLE_LITERAL: {
		typecheck_ast_list(compiler_state, function, node->table_expr.expr_list);
		infer_table_type(compiler_state, function, node);
		break;
	}
	case EXPR_BUILTIN:
		break;
	default:
		assert(0);
	}
}

/* Type checker - WIP  */
static void typecheck_function(CompilerState *compiler_state, AstNode *func)
{
	typecheck_ast_list(compiler_state, func, func->function_expr.function_statement_list);
}

/* Type checker - WIP  */
int raviX_ast_typecheck(CompilerState *compiler_state)
{
	AstNode *main_function = compiler_state->main_function;
	raviX_buffer_reset(&compiler_state->error_message);
	int rc = setjmp(compiler_state->env);
	if (rc == 0) {
		typecheck_function(compiler_state, main_function);
	}
	return rc;
}

/*
Copyright (C) 2018-2020 Dibyendu Majumdar
*/
#include <parser.h>

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

static void handle_error(CompilerState *container, const char *msg)
{
	// TODO source and line number
	raviX_buffer_add_string(&container->error_message, msg);
	longjmp(container->env, 1);
}

/* Type checker - WIP  */
static void typecheck_ast_node(CompilerState *container, AstNode *function, AstNode *node);

/* Type checker - WIP  */
static void typecheck_ast_list(CompilerState *container, AstNode *function, AstNodeList *list)
{
	AstNode *node;
	FOR_EACH_PTR(list, node) { typecheck_ast_node(container, function, node); }
	END_FOR_EACH_PTR(node);
}

/* Type checker - WIP  */
static void typecheck_unary_operator(CompilerState *container, AstNode *function, AstNode *node)
{
	UnaryOperatorType op = node->unary_expr.unary_op;
	typecheck_ast_node(container, function, node->unary_expr.expr);
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
		set_type(&node->unary_expr.type, RAVI_TNUMINT);
		break;
	case UNOPR_TO_NUMBER:
		set_type(&node->unary_expr.type, RAVI_TNUMFLT);
		break;
	case UNOPR_TO_CLOSURE:
		set_type(&node->unary_expr.type, RAVI_TFUNCTION);
		break;
	case UNOPR_TO_STRING:
		set_type(&node->unary_expr.type, RAVI_TSTRING);
		break;
	case UNOPR_TO_INTARRAY:
		set_type(&node->unary_expr.type, RAVI_TARRAYINT);
		if (node->unary_expr.expr->type == EXPR_TABLE_LITERAL) {
			set_type(&node->unary_expr.expr->table_expr.type, RAVI_TARRAYINT);
		}
		break;
	case UNOPR_TO_NUMARRAY:
		set_type(&node->unary_expr.type, RAVI_TARRAYFLT);
		if (node->unary_expr.expr->type == EXPR_TABLE_LITERAL) {
			set_type(&node->unary_expr.expr->table_expr.type, RAVI_TARRAYFLT);
		}
		break;
	case UNOPR_TO_TABLE:
		set_type(&node->unary_expr.type, RAVI_TTABLE);
		break;
	case UNOPR_TO_TYPE:
		assert(node->unary_expr.type.type_name != NULL); // Should already be set by the parser
		set_typecode(&node->unary_expr.type, RAVI_TUSERDATA);
		break;
	default:
		break;
	}
}

/* Type checker - WIP  */
static void typecheck_binary_operator(CompilerState *container, AstNode *function,
				      AstNode *node)
{
	BinaryOperatorType op = node->binary_expr.binary_op;
	AstNode *e1 = node->binary_expr.expr_left;
	AstNode *e2 = node->binary_expr.expr_right;
	typecheck_ast_node(container, function, e1);
	typecheck_ast_node(container, function, e2);
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
static void typecheck_suffixedexpr(CompilerState *container, AstNode *function, AstNode *node)
{
	typecheck_ast_node(container, function, node->suffixed_expr.primary_expr);
	AstNode *prev_node = node->suffixed_expr.primary_expr;
	AstNode *this_node;
	FOR_EACH_PTR(node->suffixed_expr.suffix_list, this_node)
	{
		typecheck_ast_node(container, function, this_node);
		if (this_node->type == EXPR_Y_INDEX) {
			if (prev_node->common_expr.type.type_code == RAVI_TARRAYFLT) {
				if (this_node->index_expr.expr->common_expr.type.type_code == RAVI_TNUMINT) {
					set_typecode(&this_node->index_expr.type, RAVI_TNUMFLT);
				} else {
					handle_error(container, "invalid type in index");
				}
			} else if (prev_node->common_expr.type.type_code == RAVI_TARRAYINT) {
				if (this_node->index_expr.expr->common_expr.type.type_code == RAVI_TNUMINT) {
					set_typecode(&this_node->index_expr.type, RAVI_TNUMINT);
				} else {
					handle_error(container, "invalid type in index");
				}
			} else if (is_unindexable_type(&prev_node->common_expr.type)) {
				handle_error(container, "invalid type in index");
			}
		}
		prev_node = this_node;
	}
	END_FOR_EACH_PTR(node);
	copy_type(&node->suffixed_expr.type, &prev_node->common_expr.type);
}

static void typecheck_var_assignment(CompilerState *container, VariableType *var_type, AstNode *expr,
				     const StringObject *var_name)
{
	if (var_type->type_code == RAVI_TANY)
		// Any value can be assigned to type ANY
		return;
	const char *variable_name = var_name ? var_name->str : "unknown-TODO";
	VariableType *expr_type = &expr->common_expr.type;

	if (var_type->type_code == RAVI_TNUMINT) {
		/* if the expr is of type number or # operator then insert @integer operator */
		if (expr_type->type_code == RAVI_TNUMFLT ||
		    (expr->type == EXPR_UNARY && expr->unary_expr.unary_op == UNOPR_LEN)) {
			/* Okay, but backend must do appropriate conversion */
			;
		} else if (expr_type->type_code != RAVI_TNUMINT) {
			fprintf(stderr, "Assignment to local symbol %s is not type compatible\n", variable_name);
		}
		return;
	}
	if (var_type->type_code == RAVI_TNUMFLT) {
		if (expr_type->type_code == RAVI_TNUMINT) {
			/* Okay, but backend must do appropriate conversion */
			;
		} else if (expr_type->type_code != RAVI_TNUMFLT) {
			fprintf(stderr, "Assignment to local symbol %s is not type compatible\n", variable_name);
		}
		return;
	}
	// all other types must strictly match
	if (!is_type_same(var_type, expr_type)) { // We should probably check type convert-ability here
		fprintf(stderr, "Assignment to local symbol %s is not type compatible\n", variable_name);
	}
}

static void typecheck_local_statement(CompilerState *container, AstNode *function,
				      AstNode *node)
{
	// The local vars should already be annotated
	// We need to typecheck the expressions to the right of =
	// Then we need to ensure that the assignments are valid
	// We can perhaps insert type assertions where we have a mismatch?

	typecheck_ast_list(container, function, node->local_stmt.expr_list);

	LuaSymbol *var;
	AstNode *expr;
	PREPARE_PTR_LIST(node->local_stmt.var_list, var);
	PREPARE_PTR_LIST(node->local_stmt.expr_list, expr);

	for (;;) {
		if (!var || !expr)
			break;

		VariableType *var_type = &var->variable.value_type;
		const StringObject *var_name = var->variable.var_name;

		typecheck_var_assignment(container, var_type, expr, var_name);

		NEXT_PTR_LIST(var);
		NEXT_PTR_LIST(expr);
	}
}

static void typecheck_expr_statement(CompilerState *container, AstNode *function, AstNode *node)
{
	if (node->expression_stmt.var_expr_list)
		typecheck_ast_list(container, function, node->expression_stmt.var_expr_list);
	typecheck_ast_list(container, function, node->expression_stmt.expr_list);

	if (!node->expression_stmt.var_expr_list)
		return;

	AstNode *var;
	AstNode *expr;
	PREPARE_PTR_LIST(node->expression_stmt.var_expr_list, var);
	PREPARE_PTR_LIST(node->local_stmt.expr_list, expr);

	for (;;) {
		if (!var || !expr)
			break;

		VariableType *var_type = &var->common_expr.type;
		const StringObject *var_name = NULL; // FIXME how do we get this?

		typecheck_var_assignment(container, var_type, expr, var_name);

		NEXT_PTR_LIST(var);
		NEXT_PTR_LIST(expr);
	}
}

static void typecheck_for_in_statment(CompilerState *container, AstNode *function,
				      AstNode *node)
{
	typecheck_ast_list(container, function, node->for_stmt.expr_list);
	typecheck_ast_list(container, function, node->for_stmt.for_statement_list);
}

static void typecheck_for_num_statment(CompilerState *container, AstNode *function,
				       AstNode *node)
{
	typecheck_ast_list(container, function, node->for_stmt.expr_list);
	AstNode *expr;
	enum { I = 1, F = 2, A = 4 }; /* bits representing integer, number, any */
	int index_type = 0;
	FOR_EACH_PTR(node->for_stmt.expr_list, expr)
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
	END_FOR_EACH_PTR(expr);
	if ((index_type & A) == 0) { /* not any */
		/* for I+F we use F */
		ravitype_t symbol_type = index_type == I ? RAVI_TNUMINT : RAVI_TNUMFLT;
		LuaSymbolList *symbols = node->for_stmt.symbols;
		LuaSymbol *sym;
		/* actually there will be only index variable */
		FOR_EACH_PTR(symbols, sym)
		{
			if (sym->symbol_type == SYM_LOCAL) {
				set_typecode(&sym->variable.value_type, symbol_type);
			} else {
				assert(0); /* cannot happen */
			}
		}
		END_FOR_EACH_PTR(sym);
	}
	typecheck_ast_list(container, function, node->for_stmt.for_statement_list);
}

static void typecheck_if_statement(CompilerState *container, AstNode *function, AstNode *node)
{
	AstNode *test_then_block;
	FOR_EACH_PTR(node->if_stmt.if_condition_list, test_then_block)
	{
		typecheck_ast_node(container, function, test_then_block->test_then_block.condition);
		typecheck_ast_list(container, function, test_then_block->test_then_block.test_then_statement_list);
	}
	END_FOR_EACH_PTR(node);
	if (node->if_stmt.else_statement_list) {
		typecheck_ast_list(container, function, node->if_stmt.else_statement_list);
	}
}

static void typecheck_while_or_repeat_statement(CompilerState *container, AstNode *function,
						AstNode *node)
{
	typecheck_ast_node(container, function, node->while_or_repeat_stmt.condition);
	if (node->while_or_repeat_stmt.loop_statement_list) {
		typecheck_ast_list(container, function, node->while_or_repeat_stmt.loop_statement_list);
	}
}

/* Type checker - WIP  */
static void typecheck_ast_node(CompilerState *container, AstNode *function, AstNode *node)
{
	switch (node->type) {
	case EXPR_FUNCTION: {
		/* args need type assertions but those have no ast - i.e. code gen should do it */
		typecheck_ast_list(container, function, node->function_expr.function_statement_list);
		break;
	}
	case AST_NONE: {
		break;
	}
	case STMT_RETURN: {
		typecheck_ast_list(container, function, node->return_stmt.expr_list);
		break;
	}
	case STMT_LOCAL: {
		typecheck_local_statement(container, function, node);
		break;
	}
	case STMT_FUNCTION: {
		typecheck_ast_node(container, function, node->function_stmt.function_expr);
		break;
	}
	case STMT_LABEL: {
		break;
	}
	case STMT_GOTO: {
		break;
	}
	case STMT_DO: {
		break;
	}
	case STMT_EXPR: {
		typecheck_expr_statement(container, function, node);
		break;
	}
	case STMT_IF: {
		typecheck_if_statement(container, function, node);
		break;
	}
	case STMT_WHILE:
	case STMT_REPEAT: {
		typecheck_while_or_repeat_statement(container, function, node);
		break;
	}
	case STMT_FOR_IN: {
		typecheck_for_in_statment(container, function, node);
		break;
	}
	case STMT_FOR_NUM: {
		typecheck_for_num_statment(container, function, node);
		break;
	}
	case EXPR_SUFFIXED: {
		typecheck_suffixedexpr(container, function, node);
		break;
	}
	case EXPR_FUNCTION_CALL: {
		if (node->function_call_expr.method_name) {
		} else {
		}
		typecheck_ast_list(container, function, node->function_call_expr.arg_list);
		break;
	}
	case EXPR_SYMBOL: {
		/* symbol type should have been set when symbol was created */
		if (node->symbol_expr.var->symbol_type != SYM_LABEL) {
			copy_type(&node->symbol_expr.type, &node->symbol_expr.var->variable.value_type);
		}
		else {
			// TODO can this happen?
			node->symbol_expr.type.type_code = RAVI_TANY;
		}
		break;
	}
	case EXPR_BINARY: {
		typecheck_binary_operator(container, function, node);
		break;
	}
	case EXPR_UNARY: {
		typecheck_unary_operator(container, function, node);
		break;
	}
	case EXPR_LITERAL: {
		/* type set during parsing */
		break;
	}
	case EXPR_FIELD_SELECTOR: {
		typecheck_ast_node(container, function, node->index_expr.expr);
		break;
	}
	case EXPR_Y_INDEX: {
		typecheck_ast_node(container, function, node->index_expr.expr);
		break;
	}
	case EXPR_TABLE_ELEMENT_ASSIGN: {
		if (node->table_elem_assign_expr.key_expr) {
			typecheck_ast_node(container, function, node->table_elem_assign_expr.key_expr);
		}
		typecheck_ast_node(container, function, node->table_elem_assign_expr.value_expr);
		copy_type(&node->table_elem_assign_expr.type, &node->table_elem_assign_expr.value_expr->common_expr.type);
		break;
	}
	case EXPR_TABLE_LITERAL: {
		typecheck_ast_list(container, function, node->table_expr.expr_list);
		break;
	}
	default:
		assert(0);
	}
}

/* Type checker - WIP  */
static void typecheck_function(CompilerState *container, AstNode *func)
{
	typecheck_ast_list(container, func, func->function_expr.function_statement_list);
}

/* Type checker - WIP  */
int raviX_ast_typecheck(CompilerState *container)
{
	AstNode *main_function = container->main_function;
	raviX_buffer_reset(&container->error_message);
	int rc = setjmp(container->env);
	if (rc == 0) {
		typecheck_function(container, main_function);
	}
	return rc;
}

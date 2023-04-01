/******************************************************************************
 * Copyright (C) 2018-2023 Dibyendu Majumdar
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
/* Portions Copyright (C) 1994-2019 Lua.org, PUC-Rio.*/

#include "parser.h"

#include <math.h>
#include <string.h>

static void process_expression_list(CompilerState *compiler_state, AstNodeList *node);
static void process_statement_list(CompilerState *compiler_state, AstNodeList *node);
static void process_statement(CompilerState *compiler_state, AstNode *node);

static int symbollist_num_symbols(LuaSymbolList *list)
{
	return raviX_ptrlist_size((struct PtrList *) list);
}

static void process_expression(CompilerState *compiler_state, AstNode *node)
{
	switch (node->type) {
	case EXPR_FUNCTION:
		process_statement_list(compiler_state, node->function_expr.function_statement_list);
		break;
	case EXPR_SUFFIXED:
		process_expression(compiler_state, node->suffixed_expr.primary_expr);
		if (node->suffixed_expr.suffix_list) {
			process_expression_list(compiler_state, node->suffixed_expr.suffix_list);
		}
		break;
	case EXPR_FUNCTION_CALL:
		process_expression_list(compiler_state, node->function_call_expr.arg_list);
		break;
	case EXPR_SYMBOL:
		break;
	case EXPR_BINARY:
		process_expression(compiler_state, node->binary_expr.expr_left);
		process_expression(compiler_state, node->binary_expr.expr_right);
		break;
	case EXPR_CONCAT:
		process_expression_list(compiler_state, node->string_concatenation_expr.expr_list);
		break;
	case EXPR_UNARY:
		process_expression(compiler_state, node->unary_expr.expr);
		break;
	case EXPR_LITERAL:
		break;
	case EXPR_FIELD_SELECTOR:
		process_expression(compiler_state, node->index_expr.expr);
		break;
	case EXPR_Y_INDEX:
		process_expression(compiler_state, node->index_expr.expr);
		break;
	case EXPR_TABLE_ELEMENT_ASSIGN:
		if (node->table_elem_assign_expr.key_expr) {
			process_expression(compiler_state, node->table_elem_assign_expr.key_expr);
		}
		process_expression(compiler_state, node->table_elem_assign_expr.value_expr);
		break;
	case EXPR_TABLE_LITERAL:
		process_expression_list(compiler_state, node->table_expr.expr_list);
		break;
	case EXPR_BUILTIN:
		break;
	default:
		assert(0);
		break;
	}
}

static void process_expression_list(CompilerState *compiler_state, AstNodeList *list)
{
	AstNode *node;
	FOR_EACH_PTR(list, AstNode, node) { process_expression(compiler_state, node); }
	END_FOR_EACH_PTR(node);
}

static void process_statement_list(CompilerState *compiler_state, AstNodeList *list)
{
	AstNode *node;
	FOR_EACH_PTR(list, AstNode, node) { process_statement(compiler_state, node); }
	END_FOR_EACH_PTR(node);
}

static void add_ast_node(CompilerState *compiler_state, AstNodeList **list, AstNode *node)
{
	raviX_ptrlist_add((PtrList **)list, node, compiler_state->allocator);
}

static AstNode *allocate_expr_ast_node(CompilerState *compiler_state, enum AstNodeType type)
{
	AstNode *node = raviX_allocate_ast_node_at_line(compiler_state, type, 0);
	node->common_expr.truncate_results = 0;
	set_typecode(&node->common_expr.type, RAVI_TANY);
	return node;
}

/* true */
static AstNode *true_expression(CompilerState *compiler_state)
{
	AstNode *expr = allocate_expr_ast_node(compiler_state, EXPR_LITERAL);
	set_type(&expr->literal_expr.type, RAVI_TBOOLEAN);
	expr->literal_expr.u.i = 1; /* initialize */
	return expr;
}

/* nil */
static AstNode *nil_expression(CompilerState *compiler_state)
{
	AstNode *expr = allocate_expr_ast_node(compiler_state, EXPR_LITERAL);
	set_type(&expr->literal_expr.type, RAVI_TNIL);
	expr->literal_expr.u.i = 0; /* initialize */
	return expr;
}

static AstNode *make_symbol_expr(CompilerState *compiler_state, LuaSymbol *symbol)
{
	AstNode *symbol_expr = allocate_expr_ast_node(compiler_state, EXPR_SYMBOL);
	symbol_expr->symbol_expr.type = symbol->variable.value_type;
	symbol_expr->symbol_expr.var = symbol;
	return symbol_expr;
}

/* fsym(ssym, varsym) */
static AstNode *call_iterator_function(CompilerState *compiler_state, LuaSymbol *fsym, LuaSymbol *ssym, LuaSymbol *varsym, int nvars)
{
	AstNode *suffixed_expr = allocate_expr_ast_node(compiler_state, EXPR_SUFFIXED);
	suffixed_expr->suffixed_expr.primary_expr = make_symbol_expr(compiler_state, fsym);
	set_type(&suffixed_expr->suffixed_expr.type, RAVI_TANY);
	suffixed_expr->suffixed_expr.suffix_list = NULL;

	AstNode *call_expr = allocate_expr_ast_node(compiler_state, EXPR_FUNCTION_CALL);
	call_expr->function_call_expr.method_name = NULL;
	call_expr->function_call_expr.arg_list = NULL;
	call_expr->function_call_expr.num_results = nvars; /* We want same number of results as there are variables */
	set_type(&call_expr->function_call_expr.type, RAVI_TANY);

	add_ast_node(compiler_state, &call_expr->function_call_expr.arg_list, make_symbol_expr(compiler_state, ssym));
	add_ast_node(compiler_state, &call_expr->function_call_expr.arg_list, make_symbol_expr(compiler_state, varsym));

	add_ast_node(compiler_state, &suffixed_expr->suffixed_expr.suffix_list, call_expr);

	return suffixed_expr;
}

static inline LuaSymbol *get_first_node(LuaSymbolList *list)
{
	return (LuaSymbol *)raviX_ptrlist_first((PtrList *)list);
}

/* symbol == nil */
static AstNode *var_is_nil(CompilerState *compiler_state, LuaSymbol *symbol)
{
	AstNode *binexpr = allocate_expr_ast_node(compiler_state, EXPR_BINARY);
	binexpr->binary_expr.expr_left = make_symbol_expr(compiler_state, symbol);
	binexpr->binary_expr.expr_right = nil_expression(compiler_state);
	binexpr->binary_expr.binary_op = BINOPR_EQ;
	return binexpr;
}

/* break */
static AstNode *break_statment(CompilerState *compiler_state, Scope *goto_scope)
{
	const StringObject *label = raviX_create_string(compiler_state, "break", sizeof "break");
	AstNode *goto_stmt = raviX_allocate_ast_node_at_line(compiler_state, STMT_GOTO, 0); // TODO line number
	goto_stmt->goto_stmt.name = label;
	goto_stmt->goto_stmt.is_break = 1;
	goto_stmt->goto_stmt.goto_scope = goto_scope;
	return goto_stmt;
}

// clang-format off
/*
Lower generic for a do block with a while loop as described in Lua 5.3 manual.

A for statement like

     for var_1, ···, var_n in explist do block end

is equivalent to the code:

     do
       local f, s, var = explist
       while true do
         local var_1, ···, var_n = f(s, var)
         if var_1 == nil then break end
         var = var_1
         block
       end
     end

Note the following:

    * explist is evaluated only once. Its results are an iterator function, a state, and an initial value for the first iterator variable.
    * f, s, and var are invisible variables. The names are here for explanatory purposes only.
    * You can use break to exit a for loop.
    The loop variables var_i are local to the loop; you cannot use their values after the for ends. If you need these values, then
    assign them to other variables before breaking or exiting the loop.
*/
// clang-format on
static AstNode * lower_for_in_statement(CompilerState *compiler_state, AstNode *node)
{
	ForStatement *for_stmt = &node->for_stmt;
	AstNode *function = for_stmt->for_scope->function;
	Scope *for_scope = for_stmt->for_scope;
	Scope *for_body_scope = for_stmt->for_body;

//	fprintf(stderr, "for parent scope = %p\n", for_scope->parent);
//	fprintf(stderr, "for scope = %p\n", for_scope);
//	fprintf(stderr, "for body scope = %p\n", for_body_scope);
//	fprintf(stderr, "for body parent scope = %p\n", for_body_scope->parent);


	// FIXME - the for variables must be removed from parent scope

	// Create do block
	AstNode *do_stmt = raviX_allocate_ast_node_at_line(compiler_state, STMT_DO, node->line_number);
	do_stmt->do_stmt.do_statement_list = NULL;

	// The do block will get a new scope
	// The original for block scope will go into the child while statement block
	Scope *do_scope = raviX_allocate_scope(compiler_state, function, for_scope->parent);
	do_stmt->do_stmt.scope = do_scope;

	// do block has 3 hidden locals
	const char f[] = "(for_f)";
	const char s[] = "(for_s)";
	const char var[] = "(for_var)";

	AstNode *local_stmt = raviX_allocate_ast_node_at_line(compiler_state, STMT_LOCAL, node->line_number);
	local_stmt->local_stmt.var_list = NULL;
	local_stmt->local_stmt.expr_list = for_stmt->expr_list;

	LuaSymbol *fsym = raviX_new_local_symbol(compiler_state, do_scope, raviX_create_string(compiler_state, f, sizeof f-1), RAVI_TANY, NULL);
	LuaSymbol *ssym = raviX_new_local_symbol(compiler_state, do_scope, raviX_create_string(compiler_state, s, sizeof s-1), RAVI_TANY, NULL);
	LuaSymbol *varsym = raviX_new_local_symbol(compiler_state, do_scope, raviX_create_string(compiler_state, var, sizeof var-1), RAVI_TANY, NULL);

	fsym->variable.modified = 1;
	ssym->variable.modified = 1;
	varsym->variable.modified = 1;

	raviX_add_symbol(compiler_state, &local_stmt->local_stmt.var_list, fsym);
	raviX_add_symbol(compiler_state, &do_scope->symbol_list, fsym);
	raviX_add_symbol(compiler_state, &local_stmt->local_stmt.var_list, ssym);
	raviX_add_symbol(compiler_state, &do_scope->symbol_list, ssym);
	raviX_add_symbol(compiler_state, &local_stmt->local_stmt.var_list, varsym);
	raviX_add_symbol(compiler_state, &do_scope->symbol_list, varsym);

	add_ast_node(compiler_state, &do_stmt->do_stmt.do_statement_list, local_stmt);

	// Create while block

	AstNode *while_stmt = raviX_allocate_ast_node_at_line(compiler_state, STMT_WHILE, node->line_number);
	Scope *while_scope = for_scope; // Original scope of the generic for statement
	while_scope->parent = do_scope; // change the original scope's parent to be the do block
	while_stmt->while_or_repeat_stmt.loop_scope = while_scope;
	while_stmt->while_or_repeat_stmt.loop_statement_list = NULL;
	while_stmt->while_or_repeat_stmt.condition = true_expression(compiler_state);
	add_ast_node(compiler_state, &do_stmt->do_stmt.do_statement_list, while_stmt);

	// Call the iterator function

	AstNode *local_stmt2 = raviX_allocate_ast_node_at_line(compiler_state, STMT_LOCAL, node->line_number);
	local_stmt2->local_stmt.var_list = for_stmt->symbols;
	local_stmt2->local_stmt.expr_list = NULL;
	AstNode *call_expr = call_iterator_function(compiler_state, fsym, ssym, varsym, symbollist_num_symbols(for_stmt->symbols));
	add_ast_node(compiler_state, &local_stmt2->local_stmt.expr_list, call_expr);
	add_ast_node(compiler_state, &while_stmt->while_or_repeat_stmt.loop_statement_list, local_stmt2);

	LuaSymbol *var1sym = get_first_node(for_stmt->symbols);

	// if var_1 == nil break
	AstNode *if_stmt = raviX_allocate_ast_node_at_line(compiler_state, STMT_IF, node->line_number);
	if_stmt->if_stmt.if_condition_list = NULL;
	if_stmt->if_stmt.else_block = NULL;
	if_stmt->if_stmt.else_statement_list = NULL;

	AstNode *test_then_block =
	    raviX_allocate_ast_node_at_line(compiler_state, STMT_TEST_THEN, node->line_number);			       // This is not an AST node on its own
	test_then_block->test_then_block.condition = var_is_nil(compiler_state, var1sym); /* read condition */
	test_then_block->test_then_block.test_then_scope = NULL;
	test_then_block->test_then_block.test_then_statement_list = NULL;

	Scope *if_scope = raviX_allocate_scope(compiler_state, function, while_scope);
	test_then_block->test_then_block.test_then_scope = if_scope;
	AstNode *gotostmt = break_statment(compiler_state, if_scope); /* handle goto/break */
	add_ast_node(compiler_state, &test_then_block->test_then_block.test_then_statement_list, gotostmt);
	add_ast_node(compiler_state, &if_stmt->if_stmt.if_condition_list, test_then_block);
	add_ast_node(compiler_state, &while_stmt->while_or_repeat_stmt.loop_statement_list, if_stmt);

	// assign var_1 to (for_var)

	AstNode *assignstmt = raviX_allocate_ast_node_at_line(compiler_state, STMT_EXPR, node->line_number);
	assignstmt->expression_stmt.var_expr_list = NULL;
	assignstmt->expression_stmt.expr_list = NULL;
	add_ast_node(compiler_state, &assignstmt->expression_stmt.var_expr_list, make_symbol_expr(compiler_state, varsym));
	add_ast_node(compiler_state, &assignstmt->expression_stmt.expr_list, make_symbol_expr(compiler_state, get_first_node(for_stmt->symbols)));
	add_ast_node(compiler_state, &while_stmt->while_or_repeat_stmt.loop_statement_list, assignstmt);

	// Copy the original block in for
	// Create do block
	AstNode *while_do_stmt = raviX_allocate_ast_node_at_line(compiler_state, STMT_DO, node->line_number);
	while_do_stmt->do_stmt.do_statement_list = NULL;
	while_do_stmt->do_stmt.scope = for_body_scope;
	add_ast_node(compiler_state, &while_stmt->while_or_repeat_stmt.loop_statement_list, while_do_stmt);

	while_do_stmt->do_stmt.do_statement_list = for_stmt->for_statement_list;
	for_stmt->for_statement_list = NULL;
	for_stmt->for_body = NULL;
	for_stmt->for_scope = NULL;

//	fprintf(stderr, "do parent scope = %p\n", do_scope->parent);
//	fprintf(stderr, "do scope = %p\n", do_scope);
//	fprintf(stderr, "while scope = %p\n", while_scope);
//	fprintf(stderr, "while parent scope = %p\n", while_scope->parent);
//	fprintf(stderr, "if scope = %p\n", if_scope);
//	fprintf(stderr, "if parent scope = %p\n", if_scope->parent);
//	fprintf(stderr, "while do scope = %p\n", for_body_scope);
//	fprintf(stderr, "while do scope parent = %p\n", for_body_scope->parent);


	// Replace the original generic for ast with the new do block
	*node = *do_stmt;
	return node;
}

static void process_statement(CompilerState *compiler_state, AstNode *node)
{
	switch (node->type) {
	case AST_NONE:
		break;
	case STMT_RETURN:
		process_expression_list(compiler_state, node->return_stmt.expr_list);
		break;
	case STMT_LOCAL:
		process_expression_list(compiler_state, node->local_stmt.expr_list);
		break;
	case STMT_FUNCTION:
		process_expression(compiler_state, node->function_stmt.function_expr);
		break;
	case STMT_LABEL:
	case STMT_GOTO:
		break;
	case STMT_DO:
		process_statement_list(compiler_state, node->do_stmt.do_statement_list);
		break;
	case STMT_EXPR:
		if (node->expression_stmt.var_expr_list) {
			process_expression_list(compiler_state, node->expression_stmt.var_expr_list);
		}
		process_expression_list(compiler_state, node->expression_stmt.expr_list);
		break;
	case STMT_IF: {
		AstNode *test_then_block;
		FOR_EACH_PTR(node->if_stmt.if_condition_list, AstNode, test_then_block)
		{
			process_expression(compiler_state, test_then_block->test_then_block.condition);
			process_statement_list(compiler_state, test_then_block->test_then_block.test_then_statement_list);
		}
		END_FOR_EACH_PTR(node);
		if (node->if_stmt.else_block) {
			process_statement_list(compiler_state, node->if_stmt.else_statement_list);
		}
		break;
	}
	case STMT_WHILE:
		process_expression(compiler_state, node->while_or_repeat_stmt.condition);
		process_statement_list(compiler_state, node->while_or_repeat_stmt.loop_statement_list);
		break;
	case STMT_REPEAT:
		process_statement_list(compiler_state, node->while_or_repeat_stmt.loop_statement_list);
		process_expression(compiler_state, node->while_or_repeat_stmt.condition);
		break;
	case STMT_FOR_NUM:
		process_expression_list(compiler_state, node->for_stmt.expr_list);
		process_statement_list(compiler_state, node->for_stmt.for_statement_list);
		break;
	case STMT_FOR_IN:
		node = lower_for_in_statement(compiler_state, node);
		assert(node->type == STMT_DO);
		process_statement_list(compiler_state, node->do_stmt.do_statement_list);
		break;
	case STMT_EMBEDDED_C:
		break;
	default:
		fprintf(stderr, "AST = %d\n", node->type);
		assert(0);
		break;
	}
}

int raviX_ast_lower(CompilerState *compiler_state)
{
	int rc = setjmp(compiler_state->env);
	if (rc == 0) {
		process_expression(compiler_state, compiler_state->main_function);
	} else {
		// dump it?
	}
	return rc;
}

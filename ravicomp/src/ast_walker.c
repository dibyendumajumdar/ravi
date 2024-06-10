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
#include <ravi_compiler.h>

#include "parser.h"

const FunctionExpression *raviX_ast_get_main_function(const CompilerState *compiler_state)
{
	return &compiler_state->main_function->function_expr;
}
const VariableType *raviX_function_type(const FunctionExpression *function_expression)
{
	return &function_expression->type;
}
bool raviX_function_is_vararg(const FunctionExpression *function_expression)
{
	return function_expression->is_vararg;
}
bool raviX_function_is_method(const FunctionExpression *function_expression)
{
	return function_expression->is_method;
}
const FunctionExpression *raviX_function_parent(const FunctionExpression *function_expression)
{
	if (function_expression->parent_function == NULL)
		return NULL;
	else
		return &function_expression->parent_function->function_expr;
}
void raviX_function_foreach_child(const FunctionExpression *function_expression, void *userdata,
				  void (*callback)(void *userdata,
						   const FunctionExpression *function_expression))
{
	AstNode *node;
	FOR_EACH_PTR(function_expression->child_functions, AstNode, node) { callback(userdata, &node->function_expr); }
	END_FOR_EACH_PTR(node)
}
const Scope *raviX_function_scope(const FunctionExpression *function_expression)
{
	return function_expression->main_block;
}
void raviX_function_foreach_statement(const FunctionExpression *function_expression, void *userdata,
				      void (*callback)(void *userdata, const Statement *statement))
{
	AstNode *node;
	FOR_EACH_PTR(function_expression->function_statement_list, AstNode, node)
	{
		assert(node->type <= STMT_EXPR);
		callback(userdata, (Statement *)node);
	}
	END_FOR_EACH_PTR(node)
}
enum AstNodeType raviX_statement_type(const Statement *statement) { return statement->type; }
void raviX_function_foreach_argument(const FunctionExpression *function_expression, void *userdata,
				     void (*callback)(void *userdata, const LuaVariableSymbol *symbol))
{
	LuaSymbol *symbol;
	FOR_EACH_PTR(function_expression->args, LuaSymbol, symbol) { callback(userdata, &symbol->variable); }
	END_FOR_EACH_PTR(symbol)
}
void raviX_function_foreach_local(const FunctionExpression *function_expression, void *userdata,
				  void (*callback)(void *userdata, const LuaVariableSymbol *lua_local_symbol))
{
//	LuaSymbol *symbol;
//	FOR_EACH_PTR(function_expression->locals, LuaSymbol, symbol) { callback(userdata, &symbol->variable); }
//	END_FOR_EACH_PTR(symbol)
}
void raviX_function_foreach_upvalue(const FunctionExpression *function_expression, void *userdata,
				    void (*callback)(void *userdata, const LuaUpvalueSymbol *symbol))
{
	LuaSymbol *symbol;
	FOR_EACH_PTR(function_expression->upvalues, LuaSymbol, symbol) { callback(userdata, &symbol->upvalue); }
	END_FOR_EACH_PTR(symbol)
}

const StringObject *raviX_variable_symbol_name(const LuaVariableSymbol *lua_local_symbol)
{
	return lua_local_symbol->var_name;
}

const VariableType *raviX_variable_symbol_type(const LuaVariableSymbol *lua_local_symbol)
{
	return &lua_local_symbol->value_type;
}

const Scope *raviX_variable_symbol_scope(const LuaVariableSymbol *lua_local_symbol)
{
	return lua_local_symbol->block;
}
bool raviX_variable_symbol_is_function_parameter(const LuaVariableSymbol *lua_local_symbol)
{
	return lua_local_symbol->function_parameter ? true : false;
}
bool raviX_variable_symbol_has_escaped(const LuaVariableSymbol *lua_local_symbol)
{
	return lua_local_symbol->escaped ? true : false;
}

#define n(v) ((AstNode *)v)
const ReturnStatement *raviX_return_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_RETURN);
	return &n(stmt)->return_stmt;
}
const LabelStatement *raviX_label_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_LABEL);
	return &n(stmt)->label_stmt;
}
const GotoStatement *raviX_goto_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_GOTO);
	return &n(stmt)->goto_stmt;
}
const LocalStatement *raviX_local_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_LOCAL);
	return &n(stmt)->local_stmt;
}
const ExpressionStatement *raviX_expression_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_EXPR);
	return &n(stmt)->expression_stmt;
}
const FunctionStatement *raviX_function_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_FUNCTION);
	return &n(stmt)->function_stmt;
}
const DoStatement *raviX_do_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_DO);
	return &n(stmt)->do_stmt;
}
const TestThenStatement *raviX_test_then_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_TEST_THEN);
	return &n(stmt)->test_then_block;
}
const IfStatement *raviX_if_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_IF);
	return &n(stmt)->if_stmt;
}
const WhileOrRepeatStatement *raviX_while_or_repeat_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_WHILE || stmt->type == STMT_REPEAT);
	return &n(stmt)->while_or_repeat_stmt;
}
const ForStatement *raviX_for_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_FOR_IN || stmt->type == STMT_FOR_NUM);
	return &n(stmt)->for_stmt;
}
const EmbeddedCStatement *raviX_embedded_C_statement(const Statement *stmt)
{
	assert(stmt->type == STMT_EMBEDDED_C);
	return &n(stmt)->embedded_C_stmt;
}
void raviX_embedded_C_statement_foreach_symbol(const EmbeddedCStatement *statement, void *userdata,
					       void (*callback)(void *, const LuaVariableSymbol *expr))
{
	LuaSymbol *symbol;
	FOR_EACH_PTR(statement->symbols, LuaSymbol, symbol)
	{
		assert(symbol->symbol_type == SYM_LOCAL);
		callback(userdata, &symbol->variable);
	}
	END_FOR_EACH_PTR(symbol)
}
const StringObject *raviX_embedded_C_statement_C_source(const EmbeddedCStatement *statement)
{
	return statement->C_src_snippet;
}

enum AstNodeType raviX_expression_type(const Expression *expression) { return expression->type; }
const LiteralExpression *raviX_literal_expression(const Expression *expr)
{
	assert(expr->type == EXPR_LITERAL);
	return &n(expr)->literal_expr;
}
const SymbolExpression *raviX_symbol_expression(const Expression *expr)
{
	assert(expr->type == EXPR_SYMBOL);
	return &n(expr)->symbol_expr;
}
const IndexExpression *raviX_index_expression(const Expression *expr)
{
	assert(expr->type == EXPR_Y_INDEX || expr->type == EXPR_FIELD_SELECTOR);
	return &n(expr)->index_expr;
}
const UnaryExpression *raviX_unary_expression(const Expression *expr)
{
	assert(expr->type == EXPR_UNARY);
	return &n(expr)->unary_expr;
}
const BinaryExpression *raviX_binary_expression(const Expression *expr)
{
	assert(expr->type == EXPR_BINARY);
	return &n(expr)->binary_expr;
}
const FunctionExpression *raviX_function_expression(const Expression *expr)
{
	assert(expr->type == EXPR_FUNCTION);
	return &n(expr)->function_expr;
}
const TableElementAssignmentExpression *
raviX_table_element_assignment_expression(const Expression *expr)
{
	assert(expr->type == EXPR_TABLE_ELEMENT_ASSIGN);
	return &n(expr)->table_elem_assign_expr;
}
const TableLiteralExpression *raviX_table_literal_expression(const Expression *expr)
{
	assert(expr->type == EXPR_TABLE_LITERAL);
	return &n(expr)->table_expr;
}
const SuffixedExpression *raviX_suffixed_expression(const Expression *expr)
{
	assert(expr->type == EXPR_SUFFIXED);
	return &n(expr)->suffixed_expr;
}
const FunctionCallExpression *raviX_function_call_expression(const Expression *expr)
{
	assert(expr->type == EXPR_FUNCTION_CALL);
	return &n(expr)->function_call_expr;
}
const StringConcatenationExpression *raviX_string_concatenation_expression(const Expression *expr)
{
	assert(expr->type == EXPR_CONCAT);
	return &n(expr)->string_concatenation_expr;
}
const BuiltinExpression *raviX_builtin_expression(const Expression *expr)
{
	assert(expr->type == EXPR_BUILTIN);
	return &n(expr)->builtin_expr;
}
#undef n

void raviX_return_statement_foreach_expression(const ReturnStatement *statement, void *userdata,
					       void (*callback)(void *, const Expression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(statement->expr_list, AstNode, node)
	{
		assert(node->type >= EXPR_LITERAL && node->type <= EXPR_BUILTIN);
		callback(userdata, (Expression *)node);
	}
	END_FOR_EACH_PTR(node)
}

const StringObject *raviX_label_statement_label_name(const LabelStatement *statement)
{
	return statement->symbol->label.label_name;
}
const Scope *raviX_label_statement_label_scope(const LabelStatement *statement)
{
	return statement->symbol->label.block;
}

const StringObject *raviX_goto_statement_label_name(const GotoStatement *statement)
{
	return statement->name;
}
const Scope *raviX_goto_statement_scope(const GotoStatement *statement)
{
	return statement->goto_scope;
}
bool raviX_goto_statement_is_break(const GotoStatement *statement) { return statement->is_break; }

void raviX_local_statement_foreach_expression(const LocalStatement *statement, void *userdata,
					      void (*callback)(void *, const Expression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(statement->expr_list, AstNode, node)
	{
		assert(node->type >= EXPR_LITERAL && node->type <= EXPR_BUILTIN);
		callback(userdata, (Expression *)node);
	}
	END_FOR_EACH_PTR(node)
}
void raviX_local_statement_foreach_symbol(const LocalStatement *statement, void *userdata,
					  void (*callback)(void *, const LuaVariableSymbol *expr))
{
	LuaSymbol *symbol;
	FOR_EACH_PTR(statement->var_list, LuaSymbol, symbol)
	{
		assert(symbol->symbol_type == SYM_LOCAL);
		callback(userdata, &symbol->variable);
	}
	END_FOR_EACH_PTR(node)
}
void raviX_expression_statement_foreach_lhs_expression(const ExpressionStatement *statement, void *userdata,
						       void (*callback)(void *, const Expression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(statement->var_expr_list, AstNode, node)
	{
		assert(node->type >= EXPR_LITERAL && node->type <= EXPR_BUILTIN);
		callback(userdata, (Expression *)node);
	}
	END_FOR_EACH_PTR(node)
}
void raviX_expression_statement_foreach_rhs_expression(const ExpressionStatement *statement, void *userdata,
						       void (*callback)(void *, const Expression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(statement->expr_list, AstNode, node)
	{
		assert(node->type >= EXPR_LITERAL && node->type <= EXPR_BUILTIN);
		callback(userdata, (Expression *)node);
	}
	END_FOR_EACH_PTR(node)
}
const SymbolExpression *raviX_function_statement_name(const FunctionStatement *statement)
{
	assert(statement->name->type == EXPR_SYMBOL);
	return &statement->name->symbol_expr;
}
bool raviX_function_statement_is_method(const FunctionStatement *statement)
{
	return statement->method_name != NULL;
}
const IndexExpression *raviX_function_statement_method_name(const FunctionStatement *statement)
{
	assert(statement->method_name->type == EXPR_Y_INDEX || statement->method_name->type == EXPR_FIELD_SELECTOR);
	return &statement->method_name->index_expr;
}
bool raviX_function_statement_has_selectors(const FunctionStatement *statement)
{
	return statement->selectors != NULL;
}
void raviX_function_statement_foreach_selector(const FunctionStatement *statement, void *userdata,
					       void (*callback)(void *, const IndexExpression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(statement->selectors, AstNode, node)
	{
		assert(/* node->type == EXPR_Y_INDEX ||*/ node->type == EXPR_FIELD_SELECTOR);
		callback(userdata, &node->index_expr);
	}
	END_FOR_EACH_PTR(node)
}
const FunctionExpression *raviX_function_ast(const FunctionStatement *statement)
{
	assert(statement->function_expr->type == EXPR_FUNCTION);
	return &statement->function_expr->function_expr;
}
const Scope *raviX_do_statement_scope(const DoStatement *statement) { return statement->scope; }
void raviX_do_statement_foreach_statement(const DoStatement *statement, void *userdata,
					  void (*callback)(void *userdata, const Statement *statement))
{
	AstNode *node;
	FOR_EACH_PTR(statement->do_statement_list, AstNode, node)
	{
		assert(node->type <= STMT_EXPR);
		callback(userdata, (Statement *)node);
	}
	END_FOR_EACH_PTR(node)
}
const Scope *raviX_test_then_statement_scope(const TestThenStatement *statement)
{
	return statement->test_then_scope;
}
void raviX_test_then_statement_foreach_statement(const TestThenStatement *statement, void *userdata,
						 void (*callback)(void *userdata, const Statement *statement))
{
	AstNode *node;
	FOR_EACH_PTR(statement->test_then_statement_list, AstNode, node)
	{
		assert(node->type <= STMT_EXPR);
		callback(userdata, (Statement *)node);
	}
	END_FOR_EACH_PTR(node)
}
const Expression *raviX_test_then_statement_condition(const TestThenStatement *statement)
{
	assert(statement->condition->type >= EXPR_LITERAL && statement->condition->type <= EXPR_BUILTIN);
	return (Expression *)statement->condition;
}
void raviX_if_statement_foreach_test_then_statement(const IfStatement *statement, void *userdata,
						    void (*callback)(void *, const TestThenStatement *stmt))
{
	AstNode *node;
	FOR_EACH_PTR(statement->if_condition_list, AstNode, node)
	{
		assert(node->type == STMT_TEST_THEN);
		callback(userdata, &node->test_then_block);
	}
	END_FOR_EACH_PTR(node)
}
const Scope *raviX_if_then_statement_else_scope(const IfStatement *statement)
{
	return statement->else_block;
}
void raviX_if_statement_foreach_else_statement(const IfStatement *statement, void *userdata,
					       void (*callback)(void *userdata, const Statement *statement))
{
	AstNode *node;
	FOR_EACH_PTR(statement->else_statement_list, AstNode, node)
	{
		assert(node->type <= STMT_EXPR);
		callback(userdata, (Statement *)node);
	}
	END_FOR_EACH_PTR(node)
}

const Expression *raviX_while_or_repeat_statement_condition(const WhileOrRepeatStatement *statement)
{
	assert(statement->condition->type >= EXPR_LITERAL && statement->condition->type <= EXPR_BUILTIN);
	return (Expression *)statement->condition;
}
const Scope *raviX_while_or_repeat_statement_scope(const WhileOrRepeatStatement *statement)
{
	return statement->loop_scope;
}
void raviX_while_or_repeat_statement_foreach_statement(const WhileOrRepeatStatement *statement,
						       void *userdata,
						       void (*callback)(void *userdata,
									const Statement *statement))
{
	AstNode *node;
	FOR_EACH_PTR(statement->loop_statement_list, AstNode, node)
	{
		assert(node->type <= STMT_EXPR);
		callback(userdata, (Statement *)node);
	}
	END_FOR_EACH_PTR(node)
}
const Scope *raviX_for_statement_scope(const ForStatement *statement)
{
	return statement->for_scope;
}
void raviX_for_statement_foreach_symbol(const ForStatement *statement, void *userdata,
					void (*callback)(void *, const LuaVariableSymbol *expr))
{
	LuaSymbol *symbol;
	FOR_EACH_PTR(statement->symbols, LuaSymbol, symbol)
	{
		assert(symbol->symbol_type == SYM_LOCAL);
		callback(userdata, &symbol->variable);
	}
	END_FOR_EACH_PTR(node)
}
void raviX_for_statement_foreach_expression(const ForStatement *statement, void *userdata,
					    void (*callback)(void *, const Expression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(statement->expr_list, AstNode, node)
	{
		assert(node->type >= EXPR_LITERAL && node->type <= EXPR_BUILTIN);
		callback(userdata, (Expression *)node);
	}
	END_FOR_EACH_PTR(node)
}
const Scope *raviX_for_statement_body_scope(const ForStatement *statement)
{
	return statement->for_body;
}
void raviX_for_statement_body_foreach_statement(const ForStatement *statement, void *userdata,
						void (*callback)(void *userdata, const Statement *statement))
{
	AstNode *node;
	FOR_EACH_PTR(statement->for_statement_list, AstNode, node)
	{
		assert(node->type <= STMT_EXPR);
		callback(userdata, (Statement *)node);
	}
	END_FOR_EACH_PTR(node)
}
const VariableType *raviX_literal_expression_type(const LiteralExpression *expression)
{
	return &expression->type;
}
const SemInfo *raviX_literal_expression_literal(const LiteralExpression *expression) { return &expression->u; }
const VariableType *raviX_symbol_expression_type(const SymbolExpression *expression)
{
	return &expression->type;
}
const LuaSymbol *raviX_symbol_expression_symbol(const SymbolExpression *expression)
{
	return expression->var;
}
const VariableType *raviX_index_expression_type(const IndexExpression *expression)
{
	return &expression->type;
}
const Expression *raviX_index_expression_expression(const IndexExpression *expression)
{
	assert(expression->expr->type >= EXPR_LITERAL && expression->expr->type <= EXPR_BUILTIN);
	return (const Expression *)expression->expr;
}
const VariableType *raviX_unary_expression_type(const UnaryExpression *expression)
{
	return &expression->type;
}
const Expression *raviX_unary_expression_expression(const UnaryExpression *expression)
{
	assert(expression->expr->type >= EXPR_LITERAL && expression->expr->type <= EXPR_BUILTIN);
	return (const Expression *)expression->expr;
}
UnaryOperatorType raviX_unary_expression_operator(const UnaryExpression *expression)
{
	return expression->unary_op;
}
const VariableType *raviX_binary_expression_type(const BinaryExpression *expression)
{
	return &expression->type;
}
const Expression *raviX_binary_expression_left_expression(const BinaryExpression *expression)
{
	assert(expression->expr_left->type >= EXPR_LITERAL && expression->expr_left->type <= EXPR_BUILTIN);
	return (const Expression *)expression->expr_left;
}
const Expression *raviX_binary_expression_right_expression(const BinaryExpression *expression)
{
	assert(expression->expr_right->type >= EXPR_LITERAL && expression->expr_right->type <= EXPR_BUILTIN);
	return (const Expression *)expression->expr_right;
}
BinaryOperatorType raviX_binary_expression_operator(const BinaryExpression *expression)
{
	return expression->binary_op;
}
const VariableType *
raviX_table_element_assignment_expression_type(const TableElementAssignmentExpression *expression)
{
	return &expression->type;
}
const Expression *
raviX_table_element_assignment_expression_key(const TableElementAssignmentExpression *expression)
{
	if (!expression->key_expr)
		return NULL;
	assert(expression->key_expr->type >= EXPR_LITERAL && expression->key_expr->type <= EXPR_BUILTIN);
	return (const Expression *)expression->key_expr;
}
const Expression *
raviX_table_element_assignment_expression_value(const TableElementAssignmentExpression *expression)
{
	assert(expression->value_expr->type >= EXPR_LITERAL && expression->value_expr->type <= EXPR_BUILTIN);
	return (const Expression *)expression->value_expr;
}
const VariableType *raviX_table_literal_expression_type(const TableLiteralExpression *expression)
{
	return &expression->type;
}
void raviX_table_literal_expression_foreach_element(
    const TableLiteralExpression *expression, void *userdata,
    void (*callback)(void *, const TableElementAssignmentExpression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(expression->expr_list, AstNode, node)
	{
		assert(node->type == EXPR_TABLE_ELEMENT_ASSIGN);
		callback(userdata, &node->table_elem_assign_expr);
	}
	END_FOR_EACH_PTR(node)
}

const VariableType *raviX_suffixed_expression_type(const SuffixedExpression *expression)
{
	return &expression->type;
}
const Expression *raviX_suffixed_expression_primary(const SuffixedExpression *expression)
{
	assert(expression->primary_expr->type >= EXPR_LITERAL && expression->primary_expr->type <= EXPR_BUILTIN);
	return (const Expression *)expression->primary_expr;
}
void raviX_suffixed_expression_foreach_suffix(const SuffixedExpression *expression, void *userdata,
					      void (*callback)(void *, const Expression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(expression->suffix_list, AstNode, node)
	{
		assert(node->type >= EXPR_LITERAL && node->type <= EXPR_BUILTIN);
		callback(userdata, (Expression *)node);
	}
	END_FOR_EACH_PTR(node)
}

const VariableType *raviX_function_call_expression_type(const FunctionCallExpression *expression)
{
	return &expression->type;
}
// Can return NULL
const StringObject *
raviX_function_call_expression_method_name(const FunctionCallExpression *expression)
{
	return expression->method_name;
}
void raviX_function_call_expression_foreach_argument(const FunctionCallExpression *expression, void *userdata,
						     void (*callback)(void *, const Expression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(expression->arg_list, AstNode, node)
	{
		assert(node->type >= EXPR_LITERAL && node->type <= EXPR_BUILTIN);
		callback(userdata, (Expression *)node);
	}
	END_FOR_EACH_PTR(node)
}
void
raviX_string_concatenation_expression_foreach_argument(const StringConcatenationExpression *expression, void *userdata,
						       void (*callback)(void *, const Expression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(expression->expr_list, AstNode, node)
	{
		assert(node->type >= EXPR_LITERAL && node->type <= EXPR_BUILTIN);
		callback(userdata, (Expression *)node);
	}
	END_FOR_EACH_PTR(node)
}
int raviX_builtin_expression_token(const BuiltinExpression *expression)
{
	return expression->token;
}
void raviX_builtin_expression_foreach_argument(const BuiltinExpression *expression, void *userdata,
					  void (*callback)(void *, const Expression *expr))
{
	AstNode *node;
	FOR_EACH_PTR(expression->arg_list, AstNode, node)
	{
		assert(node->type >= EXPR_LITERAL && node->type <= EXPR_BUILTIN);
		callback(userdata, (Expression *)node);
	}
	END_FOR_EACH_PTR(node)
}
const FunctionExpression *raviX_scope_owning_function(const Scope *scope)
{
	assert(scope->function->type == EXPR_FUNCTION);
	return &scope->function->function_expr;
}
const Scope *raviX_scope_parent_scope(const Scope *scope)
{
	return scope->parent;
}
void raviX_scope_foreach_symbol(const Scope *scope, void *userdata,
						void (*callback)(void *userdata, const LuaSymbol *symbol))
{
	LuaSymbol *symbol;
	FOR_EACH_PTR(scope->symbol_list, LuaSymbol, symbol) { callback(userdata, symbol); }
	END_FOR_EACH_PTR(node)
}
bool raviX_scope_needs_closing(const Scope *scope)
{
	return scope->need_close ? true: false;
}
enum SymbolType raviX_symbol_type(const LuaSymbol *symbol) { return symbol->symbol_type; }
const LuaVariableSymbol *raviX_symbol_variable(const LuaSymbol *symbol)
{
	assert(symbol->symbol_type == SYM_GLOBAL || symbol->symbol_type == SYM_LOCAL);
	return &symbol->variable;
}
const LuaUpvalueSymbol *raviX_symbol_upvalue(const LuaSymbol *symbol)
{
	assert(symbol->symbol_type == SYM_UPVALUE);
	return &symbol->upvalue;
}
const LuaLabelSymbol *raviX_symbol_label(const LuaSymbol *symbol)
{
	assert(symbol->symbol_type == SYM_LABEL);
	return &symbol->label;
}
const StringObject *raviX_label_name(const LuaLabelSymbol *symbol) { return symbol->label_name; }
const Scope *raviX_label_scope(const LuaLabelSymbol *symbol) { return symbol->block; }
const VariableType *raviX_upvalue_symbol_type(const LuaUpvalueSymbol *symbol)
{
	return &symbol->value_type;
}
const LuaVariableSymbol *raviX_upvalue_target_variable(const LuaUpvalueSymbol *symbol)
{
	if (symbol->target_variable->symbol_type == SYM_ENV) {
		assert(symbol->target_variable_function == NULL);
		return NULL;
	}
	assert(symbol->target_variable->symbol_type == SYM_LOCAL);
	return &symbol->target_variable->variable;
}
const FunctionExpression *raviX_upvalue_target_function(const LuaUpvalueSymbol *symbol)
{
	if (symbol->target_variable->symbol_type == SYM_ENV) {
		assert(symbol->target_variable_function == NULL);
		return NULL;
	}
	assert(symbol->target_variable_function->type == EXPR_FUNCTION);
	return &symbol->target_variable_function->function_expr;
}
unsigned raviX_upvalue_index(const LuaUpvalueSymbol *symbol) { return symbol->upvalue_index; }

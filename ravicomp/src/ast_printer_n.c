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
#include "parser.h"

static void printf_buf(TextBuffer *buf, const char *format, ...)
{
	static const char *PADDING = "                                                                                ";
	char tbuf[128] = {0};
	va_list ap;
	const char *cp;
	va_start(ap, format);
	for (cp = format; *cp; cp++) {
		if (cp[0] == '%' && cp[1] == 'p') { /* padding */
			int level = va_arg(ap, int);
			snprintf(tbuf, sizeof tbuf, "%.*s", level, PADDING);
			raviX_buffer_add_string(buf, tbuf);
			cp++;
		} else if (cp[0] == '%' && cp[1] == 't') { /* string_object */
			const StringObject *s = va_arg(ap, const StringObject *);
			raviX_buffer_add_string(buf, s->str);
			cp++;
		} else if (cp[0] == '%' && cp[1] == 'T') { /* VariableType */
			const VariableType *type;
			type = va_arg(ap, const VariableType *);
			if (type->type_code == RAVI_TUSERDATA) {
				const StringObject *s = type->type_name;
				if (s != NULL)
					raviX_buffer_add_string(buf, s->str);
				else
					raviX_buffer_add_string(buf, "userdata");
			} else {
				raviX_buffer_add_string(buf, raviX_get_type_name(type->type_code));
			}
			cp++;
		} else if (cp[0] == '%' && cp[1] == 's') { /* const char * */
			const char *s;
			s = va_arg(ap, const char *);
			raviX_buffer_add_string(buf, s);
			cp++;
		} else if (cp[0] == '%' && cp[1] == 'c') { /* comment */
			const char *s;
			s = va_arg(ap, const char *);
			raviX_buffer_add_fstring(buf, "--%s", s);
			cp++;
		} else if (cp[0] == '%' && cp[1] == 'i') { /* integer */
			lua_Integer i;
			i = va_arg(ap, lua_Integer);
			raviX_buffer_add_longlong(buf, i);
			cp++;
		} else if (cp[0] == '%' && cp[1] == 'f') { /* float */
			double d;
			d = va_arg(ap, double);
			raviX_buffer_add_double(buf, d);
			cp++;
		} else if (cp[0] == '%' && cp[1] == 'b') { /* boolean */
			lua_Integer i;
			i = va_arg(ap, lua_Integer);
			raviX_buffer_add_bool(buf, i != 0);
			cp++;
		} else if (cp[0] == '%' && cp[1] == 'P') { /* pointer */
			void *p;
			p = va_arg(ap, void *);
			raviX_buffer_add_fstring(buf, "%lld", (intptr_t)p);
			cp++;
		} else {
			raviX_buffer_add_char(buf, *cp);
		}
	}
	va_end(ap);
}

static void dump_ast_node_list(TextBuffer *buf, AstNodeList *list, int level, const char *delimiter)
{
	AstNode *node;
	bool is_first = true;
	FOR_EACH_PTR(list, AstNode, node)
	{
		if (is_first)
			is_first = false;
		else if (delimiter)
			printf_buf(buf, "%p%s\n", level, delimiter);
		raviX_dump_ast_node(buf, node, level + 1);
	}
	END_FOR_EACH_PTR(node)
}

static void dump_ast_node_list_as_ids(TextBuffer *buf, AstNodeList *list, const char *delimiter)
{
	AstNode *node;
	bool is_first = true;
	FOR_EACH_PTR(list, AstNode, node)
	{
		if (is_first)
			is_first = false;
		else if (delimiter)
			printf_buf(buf, "%s", delimiter);
		printf_buf(buf, "%P", node);
	}
	END_FOR_EACH_PTR(node)
}

static void dump_statement_list(TextBuffer *buf, AstNodeList *statement_list, int level)
{
	dump_ast_node_list(buf, statement_list, level, ",");
}

static inline const char *get_as_str(const StringObject *ts) { return ts ? ts->str : ""; }

static void dump_symbol(TextBuffer *buf, LuaSymbol *sym, int level)
{
	switch (sym->symbol_type) {
	case SYM_ENV: {
		printf_buf(buf, "%pSymEnv {\n", level);
		printf_buf(buf, "%psymbol_id = %P,\n", level + 1, sym);
		printf_buf(buf, "%pname = '%t',\n", level + 1, sym->variable.var_name);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1,
			   raviX_get_type_name(sym->variable.value_type.type_code));
		printf_buf(buf, "%ptype_name = %s,\n", level + 1, get_as_str(sym->variable.value_type.type_name));
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case SYM_GLOBAL: {
		printf_buf(buf, "%pSymGlobal {\n", level);
		printf_buf(buf, "%psymbol_id = %P,\n", level + 1, sym);
		printf_buf(buf, "%pname = '%t',\n", level + 1, sym->variable.var_name);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1,
			   raviX_get_type_name(sym->variable.value_type.type_code));
		printf_buf(buf, "%ptype_name = '%s'\n", level + 1, get_as_str(sym->variable.value_type.type_name));
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case SYM_LOCAL: {
		printf_buf(buf, "%pSymLocal {\n", level);
		printf_buf(buf, "%psymbol_id = %P,\n", level + 1, sym);
		printf_buf(buf, "%pname = '%t',\n", level + 1, sym->variable.var_name);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1,
			   raviX_get_type_name(sym->variable.value_type.type_code));
		printf_buf(buf, "%ptype_name = '%s',\n", level + 1, get_as_str(sym->variable.value_type.type_name));
		printf_buf(buf, "%pis_modified = %b,\n", level + 1, (lua_Integer)sym->variable.modified);
		printf_buf(buf, "%pis_function_argument = %b,\n", level + 1,
			   (lua_Integer)sym->variable.function_parameter);
		printf_buf(buf, "%pis_escaped = %b,\n", level + 1, (lua_Integer)sym->variable.escaped);
		printf_buf(buf, "%pscope_id = %P\n", level + 1, (lua_Integer)sym->variable.block);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case SYM_UPVALUE: {
		printf_buf(buf, "%pSymUpvalue {\n", level);
		printf_buf(buf, "%ptarget_symbol_id = %P,\n", level + 1, sym->upvalue.target_variable);
		printf_buf(buf, "%ptarget_variable_name = %t,\n", level + 1,
			   sym->upvalue.target_variable->variable.var_name);
		printf_buf(buf, "%ptarget_type_code = %s,\n", level + 1,
			   raviX_get_type_name(sym->upvalue.target_variable->variable.value_type.type_code));
		printf_buf(buf, "%ptarget_type_name = '%s',\n", level + 1,
			   get_as_str(sym->upvalue.target_variable->variable.value_type.type_name));
		printf_buf(buf, "%pupvalue_index = %i\n", level + 1,
			   (lua_Integer)(sym->upvalue.upvalue_index));
		printf_buf(buf, "%p}\n", level);
		break;
	}
	default:
		assert(0);
	}
}

static void dump_symbol_name(TextBuffer *buf, LuaSymbol *sym)
{
	switch (sym->symbol_type) {
	case SYM_LOCAL: {
		printf_buf(buf, "SymLocalRef { name='%t', symbol_id = %P }", sym->variable.var_name, sym);
		break;
	}
	case SYM_ENV: {
		printf_buf(buf, "SymEnvRef { name='%t', symbol_id = %P }", sym->variable.var_name, sym);
		break;
	}
	case SYM_GLOBAL: {
		printf_buf(buf, "SymGlobalRef { name='%t', symbol_id = %P }", sym->variable.var_name, sym);
		break;
	}
	case SYM_UPVALUE: {
		if (sym->upvalue.target_variable->symbol_type == SYM_ENV) {
			printf_buf(buf, "SymUpvalRef { name='%t', symbol_id = %P, env = true }",
				   sym->upvalue.target_variable->variable.var_name, sym->upvalue.target_variable);
		} else {
			printf_buf(buf, "SymUpvalRef { name='%t', symbol_id = %P }",
				   sym->upvalue.target_variable->variable.var_name, sym->upvalue.target_variable);
		}
		break;
	}
	case SYM_LABEL: {
		printf_buf(buf, "SymLabelRef { name='%t', symbol_id = %P }", sym->label.label_name, sym);
		break;
	}
	default:
		assert(0);
	}
}

static void dump_symbol_list(TextBuffer *buf, LuaSymbolList *list, int level, const char *delimiter)
{
	LuaSymbol *node;
	bool is_first = true;
	FOR_EACH_PTR(list, LuaSymbol, node)
	{
		if (is_first)
			is_first = false;
		else if (delimiter)
			printf_buf(buf, "%p%s\n", level, delimiter);
		dump_symbol(buf, node, level + 1);
	}
	END_FOR_EACH_PTR(node);
}

static void dump_symbol_name_list(TextBuffer *buf, LuaSymbolList *list, int level, const char *delimiter)
{
	LuaSymbol *node;
	bool is_first = true;
	FOR_EACH_PTR(list, LuaSymbol, node)
	{
		if (is_first || !delimiter) {
			printf_buf(buf, "%p", level);
			is_first = false;
		}
		else {
			printf_buf(buf, "%p%s", level, delimiter);
		}
		dump_symbol_name(buf, node);
		printf_buf(buf, "\n");
	}
	END_FOR_EACH_PTR(node);
}

static void dump_scope(TextBuffer *buf, Scope *scope, int level)
{
	printf_buf(buf, "%pScope {\n", level);
	printf_buf(buf, "%pscope_id = %P,\n", level + 1, scope);
	printf_buf(buf, "%pfunction_id = %P,\n", level + 1, scope->function);
	printf_buf(buf, "%pparent_scope_id = %P,\n", level + 1, scope->parent);
	printf_buf(buf, "%psymbols = {\n", level + 1);
	dump_symbol_list(buf, scope->symbol_list, level + 2, ",");
	printf_buf(buf, "%p},\n", level + 1);
	printf_buf(buf, "%pneed_close = %b,\n", level + 1, (lua_Integer)scope->need_close);
	printf_buf(buf, "%p}\n", level);
}

void raviX_dump_ast_node(TextBuffer *buf, AstNode *node, int level)
{
	switch (node->type) {
	case EXPR_FUNCTION: {
		printf_buf(buf, "%pExprFunction {\n", level);
		printf_buf(buf, "%pfunction_id = %P,\n", level + 1, node);
		printf_buf(buf, "%pparent_function_id = %P,\n", level + 1, node->function_expr.parent_function);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1,
			   raviX_get_type_name(node->function_expr.type.type_code));
		printf_buf(buf, "%ptype_name = '%s',\n", level + 1, get_as_str(node->function_expr.type.type_name));
		printf_buf(buf, "%pis_vararg = %b,\n", level + 1, (lua_Integer)node->function_expr.is_vararg);
		printf_buf(buf, "%pis_method = %b,\n", level + 1, (lua_Integer)node->function_expr.is_method);
		printf_buf(buf, "%pneed_close = %b,\n", level + 1, (lua_Integer)node->function_expr.need_close);
		if (node->function_expr.args) {
			printf_buf(buf, "%pargs = {\n", level + 1);
			dump_symbol_name_list(buf, node->function_expr.args, level + 2, ",");
			printf_buf(buf, "%p},\n", level + 1);
		}
		if (node->function_expr.upvalues) {
			printf_buf(buf, "%pupvalues = {\n", level + 1);
			dump_symbol_list(buf, node->function_expr.upvalues, level + 2, ",");
			printf_buf(buf, "%p},\n", level + 1);
		}
		printf_buf(buf, "%pmain_block = \n", level + 1);
		dump_scope(buf, node->function_expr.main_block, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%pstatements = {\n", level + 1);
		dump_statement_list(buf, node->function_expr.function_statement_list, level + 1);
		printf_buf(buf, "%p},\n", level + 1);
		if (node->function_expr.child_functions) {
			printf_buf(buf, "%pchild_functions = { ", level+1);
			dump_ast_node_list_as_ids(buf, node->function_expr.child_functions, ",");
			printf_buf(buf, " }\n");
		}
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case AST_NONE:
		break;
	case STMT_RETURN: {
		printf_buf(buf, "%pStmtReturn {", level);
		if (node->return_stmt.expr_list) {
			printf_buf(buf, "\n");
			dump_ast_node_list(buf, node->return_stmt.expr_list, level + 1, ",");
			printf_buf(buf, "%p}\n", level);
		} else {
			printf_buf(buf, " }\n");
		}
		break;
	}
	case STMT_LOCAL: {
		printf_buf(buf, "%pStmtLocal {\n", level);
		printf_buf(buf, "%pvar_list = {\n", level + 1);
		dump_symbol_name_list(buf, node->local_stmt.var_list, level + 2, ",");
		printf_buf(buf, "%p},\n", level + 1);
		if (node->local_stmt.expr_list) {
			printf_buf(buf, "%pexpr_list = {\n", level + 1);
			dump_ast_node_list(buf, node->local_stmt.expr_list, level + 2, ",");
			printf_buf(buf, "%p},\n", level + 1);
		}
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_FUNCTION: {
		printf_buf(buf, "%pStmtFunction {\n", level);
		printf_buf(buf, "%pname = \n", level+1);
		raviX_dump_ast_node(buf, node->function_stmt.name, level+2);
		printf_buf(buf, "%p,\n", level+1);
		if (node->function_stmt.selectors) {
			printf_buf(buf, "%pselectors = {\n", level+1);
			dump_ast_node_list(buf, node->function_stmt.selectors, level + 2, ",");
			printf_buf(buf, "%p},\n", level+1);
		}
		if (node->function_stmt.method_name) {
			printf_buf(buf, "%pmethod_name = \n", level+1);
			raviX_dump_ast_node(buf, node->function_stmt.method_name, level+2);
			printf_buf(buf, "%p,\n", level+1);
		}
		printf_buf(buf, "%pfunction_expr =\n", level + 1);
		raviX_dump_ast_node(buf, node->function_stmt.function_expr, level + 2);
		printf_buf(buf, "%p\n", level+1);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_LABEL: {
		printf_buf(buf, "%pStmtLabel {\n", level);
		printf_buf(buf, "%plabel_name = '%t'\n", level + 1, node->label_stmt.symbol->label.label_name);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_GOTO: {
		/* STMT_GOTO */
		printf_buf(buf, "%pStmtGoto {\n", level);
		printf_buf(buf, "%pis_break = %b,\n", level + 1, (lua_Integer)node->goto_stmt.is_break);
		printf_buf(buf, "%plabel_name = '%t',\n", level + 1, node->goto_stmt.name);
		printf_buf(buf, "%pgoto_scope_id = %P\n", level + 1, node->goto_stmt.goto_scope);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_DO: {
		printf_buf(buf, "%pStmtDo {\n", level);
		printf_buf(buf, "%pscope = \n", level + 1);
		dump_scope(buf, node->do_stmt.scope, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%pstatements = {\n", level + 1);
		dump_statement_list(buf, node->do_stmt.do_statement_list, level + 1);
		printf_buf(buf, "%p},\n", level + 1);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_EXPR: {
		printf_buf(buf, "%pStmtExpr {\n", level);
		if (node->expression_stmt.var_expr_list) {
			printf_buf(buf, "%pvar_expr_list = {\n", level + 1);
			dump_ast_node_list(buf, node->expression_stmt.var_expr_list, level + 2, ",");
			printf_buf(buf, "%p},\n", level + 1);
		}
		printf_buf(buf, "%pexpr_list = {\n", level + 1);
		dump_ast_node_list(buf, node->expression_stmt.expr_list, level + 2, ",");
		printf_buf(buf, "%p},\n", level + 1);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_IF: {
		printf_buf(buf, "%pStmtIf {\n", level);
		AstNode *test_then_block;
		bool is_first = true;
		FOR_EACH_PTR(node->if_stmt.if_condition_list, AstNode, test_then_block)
		{
			printf_buf(buf, "%p%s {\n", level + 1, (is_first ? "If" : "ElseIf"));
			printf_buf(buf, "%pscope = \n", level + 2);
			dump_scope(buf, test_then_block->test_then_block.test_then_scope, level + 3);
			printf_buf(buf, "%p,\n", level + 2);
			printf_buf(buf, "%pcondition = \n", level + 2);
			raviX_dump_ast_node(buf, test_then_block->test_then_block.condition, level + 3);
			printf_buf(buf, "%p,\n", level + 2);
			printf_buf(buf, "%pstatements = {\n", level + 2);
			dump_statement_list(buf, test_then_block->test_then_block.test_then_statement_list, level + 3);
			printf_buf(buf, "%p},\n", level + 2);
			printf_buf(buf, "%p},\n", level + 1);
			if (is_first) {
				is_first = false;
			}
		}
		END_FOR_EACH_PTR(node)
		if (node->if_stmt.else_block) {
			printf_buf(buf, "%pElse {\n", level + 1);
			printf_buf(buf, "%pscope = \n", level + 2);
			dump_scope(buf, node->if_stmt.else_block, level + 3);
			printf_buf(buf, "%p,\n", level + 2);
			printf_buf(buf, "%pstatements = {\n", level + 2);
			dump_statement_list(buf, node->if_stmt.else_statement_list, level + 3);
			printf_buf(buf, "%p},\n", level + 2);
			printf_buf(buf, "%p},\n", level + 1);
		}
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_WHILE: {
		printf_buf(buf, "%pStmtWhile {\n", level);
		printf_buf(buf, "%ploop_scope = \n", level + 1);
		dump_scope(buf, node->while_or_repeat_stmt.loop_scope, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%pcondition = \n", level + 1);
		raviX_dump_ast_node(buf, node->while_or_repeat_stmt.condition, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%pstatements = {\n", level + 1);
		dump_statement_list(buf, node->while_or_repeat_stmt.loop_statement_list, level + 1);
		printf_buf(buf, "%p},\n", level + 1);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_REPEAT: {
		printf_buf(buf, "%pStmtRepeat {\n", level);
		printf_buf(buf, "%ploop_scope = \n", level + 1);
		dump_scope(buf, node->while_or_repeat_stmt.loop_scope, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%pstatements = {\n", level + 1);
		dump_statement_list(buf, node->while_or_repeat_stmt.loop_statement_list, level + 1);
		printf_buf(buf, "%p},\n", level + 1);
		printf_buf(buf, "%pcondition = \n", level + 1);
		raviX_dump_ast_node(buf, node->while_or_repeat_stmt.condition, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_FOR_IN: {
		printf_buf(buf, "%pStmtForIn {\n", level);
		printf_buf(buf, "%pfor_scope = \n", level + 1);
		dump_scope(buf, node->for_stmt.for_scope, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%psymbols = {\n", level + 1);
		dump_symbol_name_list(buf, node->for_stmt.symbols, level + 2, ",");
		printf_buf(buf, "%p},\n", level + 1);
		printf_buf(buf, "%pbody_scope = \n", level + 1);
		dump_scope(buf, node->for_stmt.for_body, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%pstatements = {\n", level + 1);
		dump_statement_list(buf, node->for_stmt.for_statement_list, level + 1);
		printf_buf(buf, "%p},\n", level + 1);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_FOR_NUM: {
		printf_buf(buf, "%pStmtForNum {\n", level);
		printf_buf(buf, "%pfor_scope = \n", level + 1);
		dump_scope(buf, node->for_stmt.for_scope, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%psymbols = {\n", level + 1);
		dump_symbol_name_list(buf, node->for_stmt.symbols, level + 2, ",");
		printf_buf(buf, "%p},\n", level + 1);
		printf_buf(buf, "%pbody_scope = \n", level + 1);
		dump_scope(buf, node->for_stmt.for_body, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%pstatements = {\n", level + 1);
		dump_statement_list(buf, node->for_stmt.for_statement_list, level + 1);
		printf_buf(buf, "%p},\n", level + 1);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case STMT_EMBEDDED_C: {
		printf_buf(buf, "%pStmtEmbeddedC {\n", level);
		// print_symbol_list(buf, node->embedded_C_stmt.symbols, level + 1, ",");
		// printf_buf(buf, "%p  ) '%t'\n", level, node->embedded_C_stmt.C_src_snippet);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_SUFFIXED: {
		printf_buf(buf, "%pExprSuffixed {\n", level);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1,
			   raviX_get_type_name(node->suffixed_expr.type.type_code));
		printf_buf(buf, "%ptype_name = '%s',\n", level + 1, get_as_str(node->suffixed_expr.type.type_name));
		printf_buf(buf, "%pprimary = \n", level + 1);
		raviX_dump_ast_node(buf, node->suffixed_expr.primary_expr, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		if (node->suffixed_expr.suffix_list) {
			printf_buf(buf, "%psuffix = {\n", level + 1);
			dump_ast_node_list(buf, node->suffixed_expr.suffix_list, level + 2, ",");
			printf_buf(buf, "%p}\n", level + 1);
		}
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_FUNCTION_CALL: {
		printf_buf(buf, "%pExprCall {\n", level);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1,
			   raviX_get_type_name(node->function_call_expr.type.type_code));
		printf_buf(buf, "%ptype_name = '%s',\n", level + 1,
			   get_as_str(node->function_call_expr.type.type_name));
		if (node->function_call_expr.method_name) {
			printf_buf(buf, "%pmethod_name = '%t',\n", level + 1, node->function_call_expr.method_name);
		}
		if (node->function_call_expr.arg_list) {
			printf_buf(buf, "%parg_list = {\n", level + 1);
			dump_ast_node_list(buf, node->function_call_expr.arg_list, level + 2, ",");
			printf_buf(buf, "%p},\n", level + 1);
		}
		printf_buf(buf, "%pnum_results = %i\n", level, node->function_call_expr.num_results);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_SYMBOL: {
		printf_buf(buf, "%p", level);
		dump_symbol_name(buf, node->symbol_expr.var);
		printf_buf(buf, "\n");
		break;
	}
	case EXPR_BINARY: {
		printf_buf(buf, "%pExprBinary {\n", level);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1,
			   raviX_get_type_name(node->binary_expr.type.type_code));
		printf_buf(buf, "%ptype_name = '%s',\n", level + 1, get_as_str(node->binary_expr.type.type_name));
		printf_buf(buf, "%pop = '%s',\n", level + 1, raviX_get_binary_opr_str(node->binary_expr.binary_op));
		printf_buf(buf, "%pleft = \n", level + 1);
		raviX_dump_ast_node(buf, node->binary_expr.expr_left, level + 2);
		printf_buf(buf, "%p,\n", level + 1);
		printf_buf(buf, "%pright = \n", level + 1);
		raviX_dump_ast_node(buf, node->binary_expr.expr_right, level + 2);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_UNARY: {
		printf_buf(buf, "%pExprUnary {\n", level);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1, raviX_get_type_name(node->unary_expr.type.type_code));
		printf_buf(buf, "%ptype_name = '%s',\n", level + 1, get_as_str(node->unary_expr.type.type_name));
		printf_buf(buf, "%pop = '%s',\n", level + 1, raviX_get_unary_opr_str(node->unary_expr.unary_op));
		printf_buf(buf, "%pexpr = \n", level + 1);
		raviX_dump_ast_node(buf, node->unary_expr.expr, level + 2);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_LITERAL: {
		printf_buf(buf, "%pExprLit {\n", level);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1,
			   raviX_get_type_name(node->literal_expr.type.type_code));
		switch (node->literal_expr.type.type_code) {
		case RAVI_TNIL:
			printf_buf(buf, "%pvalue = nil\n", level + 1);
			break;
		case RAVI_TBOOLEAN:
			printf_buf(buf, "%pvalue = %b\n", level + 1, node->literal_expr.u.i);
			break;
		case RAVI_TNUMINT:
			printf_buf(buf, "%pvalue = %i\n", level + 1, node->literal_expr.u.i);
			break;
		case RAVI_TNUMFLT:
			printf_buf(buf, "%pvalue = %f\n", level + 1, node->literal_expr.u.r);
			break;
		case RAVI_TSTRING:
			printf_buf(buf, "%pvalue = '%t'\n", level + 1, node->literal_expr.u.ts);
			break;
		case RAVI_TVARARGS:
			printf_buf(buf, "%pvalue = ...\n", level + 1);
			break;
		default:
			assert(0);
		}
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_FIELD_SELECTOR: {
		printf_buf(buf, "%pExprSelector {\n", level);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1, raviX_get_type_name(node->index_expr.type.type_code));
		printf_buf(buf, "%ptype_name = '%s',\n", level + 1, get_as_str(node->index_expr.type.type_name));
		printf_buf(buf, "%pselector = \n", level + 1);
		raviX_dump_ast_node(buf, node->index_expr.expr, level + 2);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_Y_INDEX: {
		printf_buf(buf, "%pExprIndex {\n", level);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1, raviX_get_type_name(node->index_expr.type.type_code));
		printf_buf(buf, "%ptype_name = '%s',\n", level + 1, get_as_str(node->index_expr.type.type_name));
		printf_buf(buf, "%pindex = \n", level + 1);
		raviX_dump_ast_node(buf, node->index_expr.expr, level + 2);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_TABLE_ELEMENT_ASSIGN: {
		printf_buf(buf, "%pExprTableAssign {\n", level);
		if (node->table_elem_assign_expr.key_expr) {
			printf_buf(buf, "%pkey = \n", level + 1);
			raviX_dump_ast_node(buf, node->table_elem_assign_expr.key_expr, level + 2);
			printf_buf(buf, "%p,\n", level + 1);
		}
		printf_buf(buf, "%pvalue = \n", level+1);
		raviX_dump_ast_node(buf, node->table_elem_assign_expr.value_expr, level + 2);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_TABLE_LITERAL: {
		printf_buf(buf, "%pExprTableLit {\n", level);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1, raviX_get_type_name(node->table_expr.type.type_code));
		printf_buf(buf, "%ptype_name = '%s',\n", level + 1, get_as_str(node->table_expr.type.type_name));
		printf_buf(buf, "%pexpr_list = {\n", level + 1);
		dump_ast_node_list(buf, node->table_expr.expr_list, level + 1, ",");
		printf_buf(buf, "%p}\n", level + 1);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_CONCAT: {
		printf_buf(buf, "%pExprConcat {\n", level);
		printf_buf(buf, "%ptype_code = %s,\n", level + 1,
			   raviX_get_type_name(node->string_concatenation_expr.type.type_code));
		printf_buf(buf, "%ptype_name = '%s',\n", level + 1,
			   get_as_str(node->string_concatenation_expr.type.type_name));
		printf_buf(buf, "%pexpr_list = {\n", level + 1);
		dump_ast_node_list(buf, node->string_concatenation_expr.expr_list, level + 1, ",");
		printf_buf(buf, "%p}\n", level + 1);
		printf_buf(buf, "%p}\n", level);
		break;
	}
	case EXPR_BUILTIN: {
		printf_buf(buf, "%pExprBuiltin {", level);
		//		printf_buf(buf, "%p%s %c%T\n", level, "C__new(", "", &node->builtin_expr.type);
		// TODO print contents
		printf_buf(buf, "%p}\n", level);
		break;
	}
	default:
		printf_buf(buf, "%pUnsupported node type %d\n", level, node->type);
		assert(0);
	}
}

void raviX_dump_ast(CompilerState *compiler_state, FILE *fp)
{
	TextBuffer mbuf;
	raviX_buffer_init(&mbuf, 1024);
	raviX_dump_ast_to_buffer(compiler_state, &mbuf);
	fputs(mbuf.buf, fp);
	raviX_buffer_free(&mbuf);
}

void raviX_dump_ast_to_buffer(CompilerState *compiler_state, TextBuffer *mbuf)
{
	raviX_dump_ast_node(mbuf, compiler_state->main_function, 0);
}
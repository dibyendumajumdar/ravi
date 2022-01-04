/******************************************************************************
 * Copyright (C) 2020-2022 Dibyendu Majumdar
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

#ifndef ravicomp_PARSER_H
#define ravicomp_PARSER_H

/*
 * Internal header file for the implementation.
 * The data structures defined here are private.
 */

#include "ravi_compiler.h"

#include "allocate.h"
#include "membuf.h"
#include "ptrlist.h"
#include "set.h"

#include <assert.h>
#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum { MAXVARS = 125 };
#define LUA_ENV "_ENV"
#define LUA_MAXINTEGER		LLONG_MAX
#define LUA_MININTEGER		LLONG_MIN

typedef unsigned long long lua_Unsigned;
typedef unsigned char lu_byte;

//////////////////////////

typedef struct AstNode AstNode;

/*
 * Encapsulate all the compiler state.
 * All memory is held by this object or sub-objects. Memory is freed when
 * the object is destroyed.
 */
struct CompilerState {
	C_MemoryAllocator *allocator;
	Set *strings;
	AstNode *main_function;
	LinearizerState *linearizer;
	int (*error_handler)(const char *fmt, ...);
	TextBuffer buff;		 /* temp storage for literals, used by the lexer and parser */
	jmp_buf env;		 /* For error handling */
	TextBuffer error_message; /* For error handling, error message is saved here */
	bool killed;		 /* flag to check if this is already destroyed */
	const StringObject *_ENV; /* name of the env variable */
};

/* number of reserved words */
#define NUM_RESERVED ((int)(TOK_while - FIRST_RESERVED + 1))

/* state of the lexer plus state of the parser when shared by all
   functions */
struct LexerState {
	int current;	 /* current character (charint) */
	int linenumber;	 /* input line counter */
	int lastline;	 /* line of last token 'consumed' */
	Token t;	 /* current token */
	Token lookahead; /* look ahead token */
	CompilerState *compiler_state;
	const char *buf;
	size_t bufsize;
	size_t n;
	const char *p;
	TextBuffer *buff;    /* buffer for tokens, points to the buffer in compiler_state */
	const char *source; /* current source name */
	const char *envn;   /* environment variable name */
};
void raviX_syntaxerror(LexerState *ls, const char *msg);

DECLARE_PTR_LIST(AstNodeList, AstNode);

/* RAVI: Following are the types we will use
** use in parsing. The rationale for types is
** performance - as of now these are the only types that
** we care about from a performance point of view - if any
** other types appear then they are all treated as ANY
**/
typedef enum {
	RAVI_TI_NIL,
	RAVI_TI_FALSE,
	RAVI_TI_TRUE,
	RAVI_TI_INTEGER,
	RAVI_TI_FLOAT,
	RAVI_TI_INTEGER_ARRAY,
	RAVI_TI_FLOAT_ARRAY,
	RAVI_TI_TABLE,
	RAVI_TI_STRING,
	RAVI_TI_FUNCTION,
	RAVI_TI_USERDATA,
	RAVI_TI_OTHER,
	RAVI_TI_VARARGS,
} ravi_type_index;

typedef uint32_t ravi_type_map;

#define RAVI_TM_NIL (((ravi_type_map)1)<<RAVI_TI_NIL)
#define RAVI_TM_FALSE (((ravi_type_map)1)<<RAVI_TI_FALSE)
#define RAVI_TM_TRUE (((ravi_type_map)1)<<RAVI_TI_TRUE)
#define RAVI_TM_INTEGER (((ravi_type_map)1)<<RAVI_TI_INTEGER)
#define RAVI_TM_FLOAT (((ravi_type_map)1)<<RAVI_TI_FLOAT)
#define RAVI_TM_INTEGER_ARRAY (((ravi_type_map)1)<<RAVI_TI_INTEGER_ARRAY)
#define RAVI_TM_FLOAT_ARRAY (((ravi_type_map)1)<<RAVI_TI_FLOAT_ARRAY)
#define RAVI_TM_TABLE (((ravi_type_map)1)<<RAVI_TI_TABLE)
#define RAVI_TM_STRING (((ravi_type_map)1)<<RAVI_TI_STRING)
#define RAVI_TM_FUNCTION (((ravi_type_map)1)<<RAVI_TI_FUNCTION)
#define RAVI_TM_USERDATA (((ravi_type_map)1)<<RAVI_TI_USERDATA)
#define RAVI_TM_OTHER (((ravi_type_map)1)<<RAVI_TI_OTHER)
#define RAVI_TM_VARARGS (((ravi_type_map)1)<<RAVI_TI_VARARGS)

#define RAVI_TM_FALSISH (RAVI_TM_NIL | RAVI_TM_FALSE)
#define RAVI_TM_TRUISH (~RAVI_TM_FALSISH)
#define RAVI_TM_BOOLEAN (RAVI_TM_FALSE | RAVI_TM_TRUE)
#define RAVI_TM_NUMBER (RAVI_TM_INTEGER | RAVI_TM_FLOAT)
#define RAVI_TM_INDEXABLE (RAVI_TM_INTEGER_ARRAY | RAVI_TM_FLOAT_ARRAY | RAVI_TM_TABLE)
#define RAVI_TM_STRING_OR_NIL (RAVI_TM_STRING | RAVI_TM_NIL)
#define RAVI_TM_FUNCTION_OR_NIL (RAVI_TM_FUNCTION | RAVI_TM_NIL)
#define RAVI_TM_BOOLEAN_OR_NIL (RAVI_TM_BOOLEAN | RAVI_TM_NIL)
#define RAVI_TM_USERDATA_OR_NIL (RAVI_TM_USERDATA | RAVI_TM_NIL)
#define RAVI_TM_ANY (~0)

typedef enum {
	RAVI_TNIL = RAVI_TM_NIL,        /* NIL */
	RAVI_TNUMINT = RAVI_TM_INTEGER, /* integer number */
	RAVI_TNUMFLT = RAVI_TM_FLOAT,   /* floating point number */
	RAVI_TNUMBER = RAVI_TM_NUMBER,
	RAVI_TARRAYINT = RAVI_TM_INTEGER_ARRAY,   /* array of ints */
	RAVI_TARRAYFLT = RAVI_TM_FLOAT_ARRAY,     /* array of doubles */
	RAVI_TTABLE = RAVI_TM_TABLE,              /* Lua table */
	RAVI_TSTRING = RAVI_TM_STRING_OR_NIL,     /* string */
	RAVI_TFUNCTION = RAVI_TM_FUNCTION_OR_NIL, /* Lua or C Function */
	RAVI_TBOOLEAN = RAVI_TM_BOOLEAN_OR_NIL,   /* boolean */
	RAVI_TTRUE = RAVI_TM_TRUE,
	RAVI_TFALSE = RAVI_TM_FALSE,
	RAVI_TUSERDATA = RAVI_TM_USERDATA_OR_NIL, /* userdata or lightuserdata */
	RAVI_TVARARGS = RAVI_TM_VARARGS,
	RAVI_TANY = RAVI_TM_ANY,                  /* Lua dynamic type */
} ravitype_t;

/* Lua type info. We need to support user defined types too which are known by name */
struct VariableType {
	ravitype_t type_code;
	/* type name for user defined types; used to lookup metatable in registry, only set when type_code is
	 * RAVI_TUSERDATA */
	const StringObject *type_name;
};

typedef struct Pseudo Pseudo;
DECLARE_PTR_LIST(LuaSymbolList, LuaSymbol);

struct LuaVariableSymbol {
	VariableType value_type;
	const StringObject *var_name; /* name of the variable */
	Scope *block; /* NULL if global symbol, as globals are never added to a scope */
	LuaSymbol *env; /* Only applicable for global symbols - this should point to _ENV */
	unsigned escaped: 1, /* Has one or more up-value references */
		function_parameter: 1; /* Is a function parameter */
	Pseudo *pseudo;	   /* backend data for the symbol */
};
struct LuaLabelSymbol {
	const StringObject *label_name;
	Scope *block;
	Pseudo* pseudo;     /* backend data for the symbol */
};
struct LuaUpvalueSymbol {
	VariableType value_type;
	LuaSymbol *target_variable;	   /* variable reference */
	AstNode *target_variable_function; /* Where the target variable lives */
	unsigned upvalue_index : 16,   /* index of the upvalue in the function where this upvalue occurs */
	    is_in_parent_stack : 1,    /* 1 if yes - populated by code generator only */
	    parent_upvalue_index : 15; /* if !is_in_parent_stack then upvalue index in parent - populated by code generator only */
};
/* A symbol is a name recognised in Ravi/Lua code*/
struct LuaSymbol {
	enum SymbolType symbol_type;
	union {
		LuaVariableSymbol variable;
		LuaLabelSymbol label;
		LuaUpvalueSymbol upvalue;
	};
};
struct Scope {
	AstNode *function;	     /* function owning this block - of type FUNCTION_EXPR */
	Scope *parent;	     /* parent block, may belong to parent function */
	LuaSymbolList *symbol_list; /* symbols defined in this block */
	unsigned need_close: 1;              /* When we exit scope of this block do the upvalues need to be closed? */
};

/*STMT_RETURN */
struct ReturnStatement {
	AstNodeList *expr_list;
};
/* STMT_LABEL */
struct LabelStatement {
	LuaSymbol *symbol;
};
/* STMT_GOTO */
struct GotoStatement {
	unsigned is_break : 1; /* is this a break statement */
	const StringObject *name; /* target label, used to resolve the goto destination */
	Scope* goto_scope;   /* The scope of the goto statement */
};
/* STMT_LOCAL local variable declarations */
struct LocalStatement {
	LuaSymbolList *var_list;
	AstNodeList *expr_list;
};
/* STMT_EXPR: Also covers assignments */
struct ExpressionStatement {
	AstNodeList *var_expr_list; /* Optional var expressions, comma separated */
	AstNodeList *expr_list;     /* Comma separated expressions */
};
struct FunctionStatement {
	AstNode *name;		 /* base symbol to be looked up - symbol_expression */
	AstNodeList *selectors; /* Optional list of index_expression(s) */
	AstNode *method_name;	 /* Optional - index_expression */
	AstNode *function_expr;	 /* Function's AST - function_expression */
};
struct DoStatement {
	Scope *scope;		 /* The do statement only creates a new scope */
	AstNodeList *do_statement_list; /* statements in this block */
};
/* Used internally in if_stmt, not an independent AST node */
struct TestThenStatement {
	AstNode *condition;
	Scope *test_then_scope;
	AstNodeList *test_then_statement_list; /* statements in this block */
};
struct IfStatement {
	AstNodeList *if_condition_list; /* Actually a list of test_then_blocks */
	Scope *else_block;
	AstNodeList *else_statement_list; /* statements in this block */
};
struct WhileOrRepeatStatement {
	AstNode *condition;
	Scope *loop_scope;
	AstNodeList *loop_statement_list; /* statements in this block */
};
/* Used for both generic and numeric for loops */
struct ForStatement {
	Scope* for_scope; /* encapsulates the entire for statement */
	LuaSymbolList *symbols;
	AstNodeList *expr_list;
	Scope *for_body;
	AstNodeList *for_statement_list; /* statements in this block */
};
/* for embedded C */
struct EmbeddedCStatement {
	LuaSymbolList *symbols; // Ravi symbols that can be referenced in the embedded C code; only applicable if !is_decl (C__unsafe)
	const StringObject *C_src_snippet; // C source snippet - if is_decl then should only have type definitions
	bool is_decl; // true if the snippet is declarations (C__decl)
};

/* To access the type field common to all expr objects */
/* all expr types must be compatible with base_expression */

#define BASE_EXPRESSION_FIELDS VariableType type; unsigned truncate_results: 1

typedef struct BaseExpression {
	BASE_EXPRESSION_FIELDS;
} BaseExpression;

struct LiteralExpression {
	BASE_EXPRESSION_FIELDS;
	SemInfo u;
};
/* primaryexp -> NAME | '(' expr ')', NAME is parsed as EXPR_SYMBOL */
struct SymbolExpression {
	BASE_EXPRESSION_FIELDS;
	LuaSymbol *var;
};
/* EXPR_Y_INDEX or EXPR_FIELD_SELECTOR */
struct IndexExpression {
	BASE_EXPRESSION_FIELDS;
	AstNode *expr; /* '[' expr ']' */
};
/* EXPR_UNARY */
struct UnaryExpression {
	BASE_EXPRESSION_FIELDS;
	UnaryOperatorType unary_op;
	AstNode *expr;
};
struct BinaryExpression {
	BASE_EXPRESSION_FIELDS;
	BinaryOperatorType binary_op;
	AstNode *expr_left;
	AstNode *expr_right;
};
/* Special handling of string concatenation for efficiency. */
struct StringConcatenationExpression {
	BASE_EXPRESSION_FIELDS;
	AstNodeList *expr_list;
};
struct FunctionExpression {
	BASE_EXPRESSION_FIELDS;
	unsigned is_vararg : 1;
	unsigned is_method : 1;
	unsigned need_close : 1;
	uint32_t proc_id; /* Backend allocated id */
	AstNode *parent_function;	       /* parent function or NULL if main chunk */
	Scope *main_block;		       /* the function's main block */
	AstNodeList *function_statement_list; /* statements in this block */
	LuaSymbolList
	    *args; /* arguments, also must be part of the function block's symbol list */
	AstNodeList *child_functions; /* child functions declared in this function */
	LuaSymbolList *upvalues;      /* List of upvalues */
	//LuaSymbolList *locals;	       /* List of locals */
};
/* Assign values in table constructor */
/* EXPR_TABLE_ELEMENT_ASSIGN - used in table constructor */
struct TableElementAssignmentExpression {
	BASE_EXPRESSION_FIELDS;
	AstNode *key_expr; /* If NULL means this is a list field with next available index,
							else specifies index expression */
	AstNode *value_expr;
};
/* constructor -> '{' [ field { sep field } [sep] ] '}' where sep -> ',' | ';' */
/* table constructor expression EXPR_TABLE_LITERAL occurs in function call and simple expr */
struct TableLiteralExpression {
	BASE_EXPRESSION_FIELDS;
	AstNodeList *expr_list;
	ravitype_t inferred_type_code; /* If literal has values then try to infer if integer[], number[] or table */
};
/* suffixedexp -> primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs } */
/* suffix_list may have EXPR_FIELD_SELECTOR, EXPR_Y_INDEX, EXPR_FUNCTION_CALL */
struct SuffixedExpression {
	BASE_EXPRESSION_FIELDS;
	AstNode *primary_expr;
	AstNodeList *suffix_list;
};
struct FunctionCallExpression {
	/* Note that in Ravi the results from a function call must be type asserted during assignment to
	 * variables. This is not explicit in the AST but is required to ensure that function return
	 * values do not overwrite the type of the variables in an inconsistent way.
	 */
	BASE_EXPRESSION_FIELDS;
	const StringObject *method_name; /* Optional method_name */
	AstNodeList *arg_list;		 /* Call arguments */
	int num_results;			 /* How many results do we expect, -1 means all available results */
};
struct BuiltinExpression {
	/* Currently only for C__new but potentially could be other builtins */
	BASE_EXPRESSION_FIELDS;
	int token;
	AstNodeList *arg_list;
};
#undef BASE_EXPRESSION_FIELDS

/* ALL AST nodes start with following fields */
#define BASE_AST_FIELDS enum AstNodeType type; unsigned line_number
/* Statement AST nodes have following common fields.
 */
struct Statement {
	BASE_AST_FIELDS;
};
/* Expression AST nodes have the following common fields
*/
struct Expression {
	BASE_AST_FIELDS;
	BaseExpression common_expr;
};

/* The parse tree is made up of ast_node objects. Some of the ast_nodes reference the appropriate block
scopes but not all scopes may be referenced. The tree captures Lua syntax tree - i.e. statements such as
while, repeat, and for are captured in the way user uses them and not the way Lua generates code. Potentially
we can have a transformation step to convert to a tree that is more like the code generation

The ast_node must be aligned with Expression for expressions, and with Statement for statements.
*/
struct AstNode {
	BASE_AST_FIELDS;
	union {
		ReturnStatement return_stmt; /*STMT_RETURN */
		LabelStatement label_stmt; /* STMT_LABEL */
		GotoStatement goto_stmt; /* STMT_GOTO */
		LocalStatement local_stmt; /* STMT_LOCAL local variable declarations */
		ExpressionStatement expression_stmt;
		FunctionStatement function_stmt;
		DoStatement do_stmt;
		TestThenStatement test_then_block;
		IfStatement if_stmt;
		WhileOrRepeatStatement while_or_repeat_stmt;
		ForStatement for_stmt;
		EmbeddedCStatement embedded_C_stmt;
		BaseExpression common_expr;
		LiteralExpression literal_expr;
		SymbolExpression symbol_expr;
		IndexExpression index_expr;
		UnaryExpression unary_expr;
		BinaryExpression binary_expr;
		FunctionExpression function_expr; /* a literal expression whose result is a value of type function */
		TableElementAssignmentExpression table_elem_assign_expr;
		TableLiteralExpression table_expr;
		SuffixedExpression suffixed_expr;
		FunctionCallExpression function_call_expr;
		StringConcatenationExpression string_concatenation_expr;
		BuiltinExpression builtin_expr;
	};
};
#undef BASE_AST_FIELDS

static inline void set_typecode(VariableType *vt, ravitype_t t) { vt->type_code = t; }
static inline void set_type(VariableType *vt, ravitype_t t)
{
	vt->type_code = t;
	vt->type_name = NULL;
}
static inline void set_typename(VariableType *vt, ravitype_t t, const StringObject *name)
{
	vt->type_code = t;
	vt->type_name = name;
}
/* copy type from b to a */
static inline void copy_type(VariableType *a, const VariableType *b)
{
	a->type_code = b->type_code;
	a->type_name = b->type_name;
}

typedef struct ParserState {
	LexerState *ls;
	CompilerState *compiler_state;
	AstNode *current_function;
	Scope *current_scope;
} ParserState;

void raviX_print_ast_node(TextBuffer *buf, AstNode *node, int level); /* output the AST structure recursively */
const char *raviX_get_type_name(ravitype_t tt);

int raviX_ast_simplify(CompilerState* compiler_state);
int raviX_ast_lower(CompilerState *compiler_state);

////////////////////////// Internal stuff

//AstNode *raviX_allocate_ast_node(ParserState *parser, enum AstNodeType type);
Scope *raviX_allocate_scope(CompilerState *compiler_state, AstNode *function, Scope *parent_scope);
LuaSymbol *raviX_new_local_symbol(CompilerState *compiler_state, Scope *scope, const StringObject *name, ravitype_t tt,
				  const StringObject *usertype);
void raviX_add_symbol(CompilerState *compiler_state, LuaSymbolList **list, LuaSymbol *sym);
AstNode *raviX_allocate_ast_node_at_line(CompilerState *compiler_state, enum AstNodeType type, int line_num);

#endif

#ifndef ravi_ast_h
#define ravi_ast_h

/*
A parser and syntax tree builder for Ravi. This is work in progress.
Once ready it will be used to create a new byte code generator for Ravi.

The parser will perform following actions:

a) Generate syntax tree
b) Perform type checking (Ravi enhancement)
*/

#define LUA_CORE
#include "lprefix.h"
#include "lua.h"

#include "lcode.h"
#include "ldo.h"
#include "lstring.h"
#include "ltable.h"
#include "lauxlib.h"

#include "ravi_ast.h"
#include "ravi_membuf.h"

#include "allocate.h"
#include "ptrlist.h"

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MAXVARS 125

//////////////////////////

struct lua_symbol_list;

/*
 * Userdata object to hold the abstract syntax tree;
 * All memory is held by this object. Memory is freed when
 * the object is GC collected; or when
 * ast_container:release() method is called
 * by user.
 */
struct ast_container {
  struct allocator ast_node_allocator;
  struct allocator ptrlist_allocator;
  struct allocator block_scope_allocator;
  struct allocator symbol_allocator;
  struct ast_node *main_function;
  bool killed; /* flag to check if this is already destroyed */
};

struct ast_node;
DECLARE_PTR_LIST(ast_node_list, struct ast_node);

struct var_type;
DECLARE_PTR_LIST(var_type_list, struct var_type);

/* Lua type info. We need to support user defined types too which are known by name */
struct var_type {
  ravitype_t type_code;
  const TString *type_name; /* type name for user defined types; used to lookup metatable in registry, only set when
                               type_code is RAVI_TUSERDATA */
};

struct lua_symbol;
DECLARE_PTR_LIST(lua_symbol_list, struct lua_symbol);

struct block_scope;

/* Types of symbols */
enum symbol_type {
  SYM_LOCAL,
  SYM_UPVALUE,
  SYM_GLOBAL, /* Global symbols are never added to a scope so they are always looked up */
  SYM_LABEL
};

/* A symbol is a name recognised in Ravi/Lua code*/
struct lua_symbol {
  enum symbol_type symbol_type;
  struct var_type value_type;
  union {
    struct {
      const TString *var_name;   /* name of the variable */
      struct block_scope *block; /* NULL if global symbol, as globals are never added to a scope */
    } var;
    struct {
      const TString *label_name;
      struct block_scope *block;
    } label;
    struct {
      struct lua_symbol *var;    /* variable reference */
      struct ast_node *function; /* Where the upvalue lives */
    } upvalue;
  };
};

struct block_scope {
  struct ast_node *function;           /* function owning this block - of type FUNCTION_EXPR */
  struct block_scope *parent;          /* parent block, may belong to parent function */
  struct lua_symbol_list *symbol_list; /* symbols defined in this block */
};

enum ast_node_type {
  AST_NONE, /* Used when the node doesn't represent an AST such as test_then_block. */
  AST_RETURN_STMT,
  AST_GOTO_STMT,
  AST_LABEL_STMT,
  AST_DO_STMT,
  AST_LOCAL_STMT,
  AST_FUNCTION_STMT,
  AST_IF_STMT,
  AST_WHILE_STMT,
  AST_FORIN_STMT,
  AST_FORNUM_STMT,
  AST_REPEAT_STMT,
  AST_EXPR_STMT, /* Also used for assignment statements */
  AST_LITERAL_EXPR,
  AST_SYMBOL_EXPR,
  AST_Y_INDEX_EXPR,        /* [] operator */
  AST_FIELD_SELECTOR_EXPR, /* table field access - '.' or ':' operator */
  AST_INDEXED_ASSIGN_EXPR, /* table value assign in table constructor */
  AST_SUFFIXED_EXPR,
  AST_UNARY_EXPR,
  AST_BINARY_EXPR,
  AST_FUNCTION_EXPR, /* function literal */
  AST_TABLE_EXPR,    /* table constructor */
  AST_FUNCTION_CALL_EXPR
};

/* The parse tree is made up of ast_node objects. Some of the ast_nodes reference the appropriate block
scopes but not all scopes may be referenced. The tree captures Lua syntax tree - i.e. statements such as
while, repeat, and for are captured in the way user uses them and not the way Lua generates code. Potentially
we can have a transformation step to convert to a tree that is more like the code generation */
struct ast_node {
  enum ast_node_type type;
  union {
    struct {
      struct ast_node_list *expr_list;
    } return_stmt;
    struct {
      struct lua_symbol *symbol;
    } label_stmt;
    struct {
      const TString *name;         /* target label, used to resolve the goto destination */
      struct ast_node *label_stmt; /* Initially this will be NULL; set by a separate pass */
    } goto_stmt;
    struct {
      struct lua_symbol_list *var_list;
      struct ast_node_list *expr_list;
    } local_stmt; /* local declarations */
    struct {
      struct ast_node_list *var_expr_list; /* Optional var expressions, comma separated */
      struct ast_node_list *expr_list;     /* Comma separated expressions */
    } expression_stmt;                     /* Also covers assignments */
    struct {
      struct ast_node *name;           /* base symbol to be looked up */
      struct ast_node_list *selectors; /* Optional */
      struct ast_node *method_name;    /* Optional */
      struct ast_node *function_expr;  /* Function's AST */
    } function_stmt;
    struct {
      struct block_scope *scope;               /* The do statement only creates a new scope */
      struct ast_node_list *do_statement_list; /* statements in this block */
    } do_stmt;
    struct {
      struct ast_node *condition;
      struct block_scope *test_then_scope;
      struct ast_node_list *test_then_statement_list; /* statements in this block */
    } test_then_block;                                /* Used internally in if_stmt, not an independent AST node */
    struct {
      struct ast_node_list *if_condition_list; /* Actually a list of test_then_blocks */
      struct block_scope *else_block;
      struct ast_node_list *else_statement_list; /* statements in this block */
    } if_stmt;
    struct {
      struct ast_node *condition;
      struct block_scope *loop_scope;
      struct ast_node_list *loop_statement_list; /* statements in this block */
    } while_or_repeat_stmt;
    struct {
      struct lua_symbol_list *symbols;
      struct ast_node_list *expr_list;
      struct block_scope *for_body;
      struct ast_node_list *for_statement_list; /* statements in this block */
    } for_stmt;                                 /* Used for both generic and numeric for loops */
    struct {
      struct var_type type;
    } common_expr; /* To access the type field common to all expr objects */
    /* all expr types must be compatible with common_expr */
    struct {
      struct var_type type;
      union {
        lua_Integer i;
        lua_Number n;
        const TString *s;
      } u;
    } literal_expr;
    struct {
      struct var_type type;
      struct lua_symbol *var;
    } symbol_expr;
    struct {
      struct var_type type;
      struct ast_node *expr; /* '[' expr ']' */
    } index_expr;
    struct {
      struct var_type type;
      UnOpr unary_op;
      struct ast_node *expr;
    } unary_expr;
    struct {
      struct var_type type;
      BinOpr binary_op;
      struct ast_node *expr_left;
      struct ast_node *expr_right;
    } binary_expr;
    struct {
      struct var_type type;
      unsigned int is_vararg : 1;
      unsigned int is_method : 1;
      struct ast_node *parent_function;              /* parent function or NULL if main chunk */
      struct block_scope *main_block;                /* the function's main block */
      struct ast_node_list *function_statement_list; /* statements in this block */
      struct lua_symbol_list *args;          /* arguments, also must be part of the function block's symbol list */
      struct ast_node_list *child_functions; /* child functions declared in this function */
      struct lua_symbol_list *upvalues;      /* List of upvalues */
      struct lua_symbol_list *locals;        /* List of locals */
    } function_expr;                         /* a literal expression whose result is a value of type function */
    struct {
      struct var_type type;
      struct ast_node *
          index_expr; /* If NULL means this is a list field with next available index, else specfies index expression */
      struct ast_node *value_expr;
    } indexed_assign_expr; /* Assign values in table constructor */
    struct {
      struct var_type type;
      struct ast_node_list *expr_list;
    } table_expr; /* table constructor expression */
    struct {
      struct var_type type;
      struct ast_node *primary_expr;
      struct ast_node_list *suffix_list;
    } suffixed_expr;
    struct {
      /* Note that in Ravi the results from a function call must be type asserted during assignment to variables.
       * This is not explicit in the AST but is required to ensure that function return values do not
       * overwrite the type of the variables in an inconsistent way.
       */
      struct var_type type;
      TString *method_name;           /* Optional method_name */
      struct ast_node_list *arg_list; /* Call arguments */
    } function_call_expr;
  };
};

#define set_typecode(vt, t) (vt).type_code = t
#define set_type(vt, t) (vt).type_code = t, (vt).type_name = NULL
#define set_typename(vt, t, name) (vt).type_code = t, (vt).type_name = (name)
#define is_type_same(a, b) ((a).type_code == (b).type_code && (a).type_name == (b).type_name)
#define copy_type(a, b) (a).type_code = (b).type_code, (a).type_name = (b).type_name

struct parser_state {
  LexState *ls;
  struct ast_container *container;
  struct ast_node *current_function;
  struct block_scope *current_scope;
};

LUAMOD_API int raviopen_ast_library(lua_State *L);

void raviA_print_ast_node(membuff_t *buf, struct ast_node *node, int level); /* output the AST structure recusrively */
void raviA_ast_typecheck(struct ast_container *container); /* Perform type checks and assign types to AST */

/*
Linearizer
*/
struct instruction;
struct node;
struct basic_block;
struct edge;
struct pseudo;
struct cfg;
struct proc;

DECLARE_PTR_LIST(instruction_list, struct instruction);
DECLARE_PTR_LIST(edge_list, struct edge);
DECLARE_PTR_LIST(pseudo_list, struct pseudo);
DECLARE_PTR_LIST(proc_list, struct proc);

#define container_of(ptr, type, member) ((type *)((char *)(ptr)-offsetof(type, member)))

enum opcode { OP_NOP };

/* pseudo represents a pseudo (virtual) register */
struct pseudo {
  unsigned regnum : 16;
};

/* single instruction */
struct instruction {
  unsigned opcode : 8;
};

struct edge {
  struct node *from;
  struct node *to;
};

#define NODE_FIELDS       \
  uint32_t index;         \
  struct edge_list *pred; \
  struct edge_list *succ

struct node {
  NODE_FIELDS;
};

/* Basic block is a specialization of node */
struct basic_block {
  NODE_FIELDS;
  struct instruction_list *insns;
};

#define CFG_FIELDS    \
  unsigned node_count; \
  struct node **nodes; \
  struct node *entry; \
  struct node *exit

struct cfg {
  CFG_FIELDS;
};

/* proc is a type of cfg */
struct proc {
  CFG_FIELDS;
  struct proc_list* procs; /* procs defined in this proc */
  struct proc* parent;     /* enclosing proc */
  struct ast_node* function_expr; /* function ast that we are compiling */
};

static inline struct basic_block * n2bb(struct node *n) { return (struct basic_block *)n; }
static inline struct node * bb2n(struct basic_block *bb) { return (struct node *)bb; }


struct linearizer {
  struct allocator instruction_allocator;
  struct allocator edge_allocator;
  struct allocator pseudo_allocator;
  struct allocator ptrlist_allocator;
  struct allocator basic_block_allocator;
  struct allocator proc_allocator;
  struct ast_container *ast_container;
  struct proc* main_proc; /* The root of the compiled chunk of code */
  struct proc_list *all_procs; /* All procs allocated by the linearizer */
  struct proc *current_proc;  /* proc being compiled */
};

void raviA_ast_linearize(struct linearizer *linearizer, struct ast_container *container);

#endif
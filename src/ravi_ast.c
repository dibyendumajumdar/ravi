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
      struct ast_node_list *exprlist;
    } return_stmt;
    struct {
      struct lua_symbol *symbol;
    } label_stmt;
    struct {
      const TString *name;         /* target label, used to resolve the goto destination */
      struct ast_node *label_stmt; /* Initially this will be NULL; set by a separate pass */
    } goto_stmt;
    struct {
      struct lua_symbol_list *vars;
      struct ast_node_list *exprlist;
    } local_stmt;
    struct {
      struct ast_node_list *var_expr_list; /* Optional var expressions, comma separated */
      struct ast_node_list *exr_list;      /* Comma separated expressions */
    } expression_stmt;                     /* Also covers assignments*/
    struct {
      struct ast_node *name;           /* base symbol to be looked up */
      struct ast_node_list *selectors; /* Optional */
      struct ast_node *methodname;     /* Optional */
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
      struct ast_node_list *expressions;
      struct block_scope *for_body;
      struct ast_node_list *for_statement_list; /* statements in this block */
    } for_stmt;                                 /* Used for both generic and numeric for loops */
    struct {
      struct var_type type;
    } common_expr; /* To access the type field common to all expr objects */
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
      struct ast_node *exprleft;
      struct ast_node *exprright;
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
      struct var_type type;
      TString *methodname;            /* Optional methodname */
      struct ast_node_list *arg_list; /* Call arguments */
    } function_call_expr;
  };
};

#define set_typecode(vt, t) \
  (vt).type_code = t
#define set_type(vt, t) \
  (vt).type_code = t,   \
  (vt).type_name = NULL
#define set_typename(vt, t, name) \
  (vt).type_code = t,             \
  (vt).type_name = (name)
#define is_type_same(a, b) ((a).type_code == (b).type_code && (a).type_name == (b).type_name)
#define copy_type(a, b)          \
  (a).type_code = (b).type_code, \
  (a).type_name = (b).type_name

struct parser_state {
  LexState *ls;
  struct ast_container *container;
  struct ast_node *current_function;
  struct block_scope *current_scope;
};

static void add_symbol(struct ast_container *container, struct lua_symbol_list **list, struct lua_symbol *sym) {
  ptrlist_add((struct ptr_list **)list, sym, &container->ptrlist_allocator);
}

static void add_ast_node(struct ast_container *container, struct ast_node_list **list, struct ast_node *node) {
  ptrlist_add((struct ptr_list **)list, node, &container->ptrlist_allocator);
}

/* forward declarations */
static struct ast_node *parse_expression(struct parser_state *);
static void parse_statement_list(struct parser_state *, struct ast_node_list **list);
static struct ast_node *parse_statement(struct parser_state *);
static struct ast_node *new_function(struct parser_state *parser);
static struct ast_node *end_function(struct parser_state *parser);
static struct block_scope *new_scope(struct parser_state *parser);
static void end_scope(struct parser_state *parser);
static struct ast_node *new_literal_expression(struct parser_state *parser, ravitype_t type);
static struct ast_node *generate_label(struct parser_state *parser, TString *label);
static struct ast_container *new_ast_container(lua_State *L);
static void do_typechecks(struct ast_container *container);

static l_noret error_expected(LexState *ls, int token) {
  luaX_syntaxerror(ls, luaO_pushfstring(ls->L, "%s expected", luaX_token2str(ls, token)));
}

static int testnext(LexState *ls, int c) {
  if (ls->t.token == c) {
    luaX_next(ls);
    return 1;
  }
  else
    return 0;
}

static void check(LexState *ls, int c) {
  if (ls->t.token != c)
    error_expected(ls, c);
}

static void checknext(LexState *ls, int c) {
  check(ls, c);
  luaX_next(ls);
}

/*============================================================*/
/* GRAMMAR RULES */
/*============================================================*/

/*
** check whether current token is in the follow set of a block.
** 'until' closes syntactical blocks, but do not close scope,
** so it is handled in separate.
*/
static int block_follow(LexState *ls, int withuntil) {
  switch (ls->t.token) {
    case TK_ELSE:
    case TK_ELSEIF:
    case TK_END:
    case TK_EOS:
      return 1;
    case TK_UNTIL:
      return withuntil;
    default:
      return 0;
  }
}

static void check_match(LexState *ls, int what, int who, int where) {
  if (!testnext(ls, what)) {
    if (where == ls->linenumber)
      error_expected(ls, what);
    else {
      luaX_syntaxerror(ls, luaO_pushfstring(ls->L, "%s expected (to close %s at line %d)", luaX_token2str(ls, what),
                                            luaX_token2str(ls, who), where));
    }
  }
}

/* Check that current token is a name, and advance */
static TString *check_name_and_next(LexState *ls) {
  TString *ts;
  check(ls, TK_NAME);
  ts = ls->t.seminfo.ts;
  luaX_next(ls);
  return ts;
}

/* Check that current token is a name, and advance */
static TString *str_checkname(LexState *ls) {
  TString *ts;
  check(ls, TK_NAME);
  ts = ls->t.seminfo.ts;
  luaX_next(ls);
  return ts;
}

/* create a new local variable in function scope, and set the
 * variable type (RAVI - added type tt) */
static struct lua_symbol *new_local_symbol(struct parser_state *parser, TString *name, ravitype_t tt,
                                           TString *usertype) {
  struct block_scope *scope = parser->current_scope;
  struct lua_symbol *symbol = dmrC_allocator_allocate(&parser->container->symbol_allocator, 0);
  set_typename(symbol->value_type, tt, usertype);
  symbol->symbol_type = SYM_LOCAL;
  symbol->var.block = scope;
  symbol->var.var_name = name;
  add_symbol(parser->container, &scope->symbol_list, symbol);  // Add to the end of the symbol list
  add_symbol(parser->container, &scope->function->function_expr.locals, symbol);
  // Note that Lua allows multiple local declarations of the same name
  // so a new instance just gets added to the end
  return symbol;
}

/* create a new label */
static struct lua_symbol *new_label(struct parser_state *parser, TString *name) {
  struct block_scope *scope = parser->current_scope;
  assert(scope);
  struct lua_symbol *symbol = dmrC_allocator_allocate(&parser->container->symbol_allocator, 0);
  set_type(symbol->value_type, RAVI_TANY);
  symbol->symbol_type = SYM_LABEL;
  symbol->label.block = scope;
  symbol->label.label_name = name;
  add_symbol(parser->container, &scope->symbol_list,
             symbol);  // Add to the end of the symbol list
                       // Note that Lua allows multiple local declarations of the same name
                       // so a new instance just gets added to the end
  return symbol;
}

/* create a new local variable
 */
static struct lua_symbol *new_localvarliteral_(struct parser_state *parser, const char *name, size_t sz) {
  return new_local_symbol(parser, luaX_newstring(parser->ls, name, sz), RAVI_TANY, NULL);
}

/* create a new local variable
 */
#define new_localvarliteral(parser, name) new_localvarliteral_(parser, "" name, (sizeof(name) / sizeof(char)) - 1)

static struct lua_symbol *search_for_variable_in_block(struct block_scope *scope, const TString *varname) {
  struct lua_symbol *symbol;
  // Lookup in reverse order so that we discover the
  // most recently added local symbol - as Lua allows same
  // symbol to be declared local more than once in a scope
  // Should also work with nesting as the function when parsed
  // will only know about vars declared in parent function until
  // now.
  FOR_EACH_PTR_REVERSE(scope->symbol_list, symbol) {
    switch (symbol->symbol_type) {
      case SYM_LOCAL: {
        if (varname == symbol->var.var_name) {
          return symbol;
        }
        break;
      }
      default:
        break;
    }
  }
  END_FOR_EACH_PTR_REVERSE(symbol);
  return NULL;
}

// static struct lua_symbol *search_for_label_in_block(struct block_scope *scope, const TString *name) {
//  struct lua_symbol *symbol;
//  // Lookup in reverse order so that we discover the
//  // most recently added local symbol - as Lua allows same
//  // symbol to be declared local more than once in a scope
//  // Should also work with nesting as the function when parsed
//  // will only know about vars declared in parent function until
//  // now.
//  FOR_EACH_PTR_REVERSE(scope->symbol_list, symbol) {
//    switch (symbol->symbol_type) {
//      case SYM_LABEL: {
//        if (name == symbol->var.var_name) { return symbol; }
//        break;
//      }
//      default: break;
//    }
//  }
//  END_FOR_EACH_PTR_REVERSE(symbol);
//  return NULL;
//}

/* Each function has a list of upvalues, searches this list for given name
 */
static struct lua_symbol *search_upvalue_in_function(struct ast_node *function, const TString *name) {
  struct lua_symbol *symbol;
  FOR_EACH_PTR(function->function_expr.upvalues, symbol) {
    switch (symbol->symbol_type) {
      case SYM_UPVALUE: {
        assert(symbol->upvalue.var->symbol_type == SYM_LOCAL);
        if (name == symbol->upvalue.var->var.var_name) {
          return symbol;
        }
        break;
      }
      default:
        break;
    }
  }
  END_FOR_EACH_PTR(symbol);
  return NULL;
}

/* Each function has a list of upvalues, searches this list for given name, and adds it if not found.
 * Returns true if added, false means the function already has the upvalue.
 */
static bool add_upvalue_in_function(struct parser_state *parser, struct ast_node *function, struct lua_symbol *sym) {
  struct lua_symbol *symbol;
  FOR_EACH_PTR(function->function_expr.upvalues, symbol) {
    switch (symbol->symbol_type) {
      case SYM_UPVALUE: {
        assert(symbol->upvalue.var->symbol_type == SYM_LOCAL);
        if (sym == symbol->upvalue.var) {
          return false;
        }
        break;
      }
      default:
        break;
    }
  }
  END_FOR_EACH_PTR(symbol);
  struct lua_symbol *upvalue = dmrC_allocator_allocate(&parser->container->symbol_allocator, 0);
  upvalue->symbol_type = SYM_UPVALUE;
  upvalue->upvalue.var = sym;
  upvalue->upvalue.function = function;
  copy_type(upvalue->value_type, sym->value_type);
  add_symbol(parser->container, &function->function_expr.upvalues, upvalue);
  return true;
}

/* Searches for a variable starting from current scope, going up the
 * scope chain within the current function. If the variable is not found in any scope of the function, then
 * search the function's upvalue list. Repeat the exercise in parent function until either
 * the symbol is found or we exhaust the search. NULL is returned if search was
 * exhausted.
 */
static struct lua_symbol *search_for_variable(struct parser_state *parser, const TString *varname, bool *is_local) {
  *is_local = false;
  struct block_scope *current_scope = parser->current_scope;
  struct ast_node *start_function = parser->current_function;
  assert(current_scope && current_scope->function == parser->current_function);
  while (current_scope) {
    struct ast_node *current_function = current_scope->function;
    while (current_scope && current_function == current_scope->function) {
      struct lua_symbol *symbol = search_for_variable_in_block(current_scope, varname);
      if (symbol) {
        *is_local = (current_function == start_function);
        return symbol;
      }
      current_scope = current_scope->parent;
    }
    // search upvalues in the function
    struct lua_symbol *symbol = search_upvalue_in_function(current_function, varname);
    if (symbol)
      return symbol;
    // try in parent function
  }
  return NULL;
}

/* Searches for a label in current function
 */
// static struct lua_symbol *search_for_label(struct parser_state *parser, const TString *name) {
//  struct block_scope *current_scope = parser->current_scope;
//  while (current_scope && current_scope->function == parser->current_function) {
//    struct lua_symbol *symbol = search_for_label_in_block(current_scope, name);
//    if (symbol) return symbol;
//    current_scope = current_scope->parent;
//  }
//  return NULL;
//}

/* Adds an upvalue to current_function and its parents until var_function; var_function being where the symbol
 * exists as a local or an upvalue. If the symbol is found in a function's upvalue list then there is no need to
 * check parent functions.
 */
static void add_upvalue_in_levels_upto(struct parser_state *parser, struct ast_node *current_function,
                                       struct ast_node *var_function, struct lua_symbol *symbol) {
  assert(current_function != var_function);
  while (current_function && current_function != var_function) {
    bool added = add_upvalue_in_function(parser, current_function, symbol);
    if (!added)
      // this function already has it so we are done
      break;
    current_function = current_function->function_expr.parent_function;
  }
}

/* Creates a symbol reference to the name; the returned symbol reference
 * may be local, upvalue or global.
 */
static struct ast_node *new_symbol_reference(struct parser_state *parser) {
  TString *varname = check_name_and_next(parser->ls);
  bool is_local = false;
  struct lua_symbol *symbol = search_for_variable(parser, varname, &is_local);
  if (symbol) {
    // we found a local or upvalue
    if (!is_local) {
      // If the local symbol occurred in a parent function then we
      // need to construct an upvalue. Lua requires that the upvalue be
      // added to all functions in the tree up to the function where the local
      // is defined.
      add_upvalue_in_levels_upto(parser, parser->current_function, symbol->var.block->function, symbol);
      // TODO Following search could be avoided if above returned the symbol
      symbol = search_upvalue_in_function(parser->current_function, varname);
    }
    else if (symbol->symbol_type == SYM_UPVALUE && symbol->upvalue.function != parser->current_function) {
      // We found an upvalue but it is not at the same level
      // Ensure all levels have the upvalue
      add_upvalue_in_levels_upto(parser, parser->current_function, symbol->upvalue.function, symbol->upvalue.var);
      // TODO Following search could be avoided if above returned the symbol
      symbol = search_upvalue_in_function(parser->current_function, varname);
    }
  }
  else {
    // Return global symbol
    struct lua_symbol *global = dmrC_allocator_allocate(&parser->container->symbol_allocator, 0);
    global->symbol_type = SYM_GLOBAL;
    global->var.var_name = varname;
    global->var.block = NULL;
    set_type(global->value_type, RAVI_TANY);  // Globals are always ANY type
    // We don't add globals to any scope so that they are
    // always looked up
    symbol = global;
  }
  struct ast_node *symbol_expr = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  symbol_expr->type = AST_SYMBOL_EXPR;
  symbol_expr->symbol_expr.type = symbol->value_type;
  symbol_expr->symbol_expr.var = symbol;
  return symbol_expr;
}

/*============================================================*/
/* GRAMMAR RULES */
/*============================================================*/

static struct ast_node *new_string_literal(struct parser_state *parser, const TString *ts) {
  struct ast_node *node = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  node->type = AST_LITERAL_EXPR;
  set_type(node->literal_expr.type, RAVI_TSTRING);
  node->literal_expr.u.s = ts;
  return node;
}

static struct ast_node *new_field_selector(struct parser_state *parser, const TString *ts) {
  struct ast_node *index = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  index->type = AST_FIELD_SELECTOR_EXPR;
  index->index_expr.expr = new_string_literal(parser, ts);
  set_type(index->index_expr.type, RAVI_TANY);
  return index;
}

/*
 * Parse ['.' | ':'] NAME
 */
static struct ast_node *parse_field_selector(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* fieldsel -> ['.' | ':'] NAME */
  luaX_next(ls); /* skip the dot or colon */
  TString *ts = check_name_and_next(ls);
  return new_field_selector(parser, ts);
}

/*
 * Parse '[' expr ']
 */
static struct ast_node *parse_yindex(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* index -> '[' expr ']' */
  luaX_next(ls); /* skip the '[' */
  struct ast_node *expr = parse_expression(parser);
  checknext(ls, ']');

  struct ast_node *index = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  index->type = AST_Y_INDEX_EXPR;
  index->index_expr.expr = expr;
  set_type(index->index_expr.type, RAVI_TANY);
  return index;
}

/*
** {======================================================================
** Rules for Constructors
** =======================================================================
*/

static struct ast_node *new_indexed_assign_expr(struct parser_state *parser, struct ast_node *index_expr,
                                                struct ast_node *value_expr) {
  struct ast_node *set = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  set->type = AST_INDEXED_ASSIGN_EXPR;
  set->indexed_assign_expr.index_expr = index_expr;
  set->indexed_assign_expr.value_expr = value_expr;
  set->indexed_assign_expr.type = value_expr->common_expr.type; /* type of indexed assignment is same as the value*/
  return set;
}

static struct ast_node *parse_recfield(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* recfield -> (NAME | '['exp1']') = exp1 */
  struct ast_node *index_expr;
  if (ls->t.token == TK_NAME) {
    const TString *ts = check_name_and_next(ls);
    index_expr = new_field_selector(parser, ts);
  }
  else /* ls->t.token == '[' */
    index_expr = parse_yindex(parser);
  checknext(ls, '=');
  struct ast_node *value_expr = parse_expression(parser);
  return new_indexed_assign_expr(parser, index_expr, value_expr);
}

static struct ast_node *parse_listfield(struct parser_state *parser) {
  /* listfield -> exp */
  struct ast_node *value_expr = parse_expression(parser);
  return new_indexed_assign_expr(parser, NULL, value_expr);
}

static struct ast_node *parse_field(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* field -> listfield | recfield */
  switch (ls->t.token) {
    case TK_NAME: {                  /* may be 'listfield' or 'recfield' */
      if (luaX_lookahead(ls) != '=') /* expression? */
        return parse_listfield(parser);
      else
        return parse_recfield(parser);
      break;
    }
    case '[': {
      return parse_recfield(parser);
      break;
    }
    default: {
      return parse_listfield(parser);
      break;
    }
  }
  return NULL;
}

static struct ast_node *parse_table_constructor(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* constructor -> '{' [ field { sep field } [sep] ] '}'
  sep -> ',' | ';' */
  int line = ls->linenumber;
  checknext(ls, '{');
  struct ast_node *table_expr = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  set_type(table_expr->table_expr.type, RAVI_TTABLE);
  table_expr->table_expr.expr_list = NULL;
  table_expr->type = AST_TABLE_EXPR;
  do {
    if (ls->t.token == '}')
      break;
    struct ast_node *field_expr = parse_field(parser);
    add_ast_node(parser->container, &table_expr->table_expr.expr_list, field_expr);
  } while (testnext(ls, ',') || testnext(ls, ';'));
  check_match(ls, '}', '{', line);
  return table_expr;
}

/* }====================================================================== */

/*
 * We would like to allow user defined types to contain the sequence
 * NAME [. NAME]+
 * The initial NAME is supplied.
 * Returns extended name.
 * Note that the returned string will be anchored in the Lexer and must
 * be anchored somewhere else by the time parsing finishes
 */
static TString *user_defined_type_name(LexState *ls, TString *typename) {
  size_t len = 0;
  if (testnext(ls, '.')) {
    char buffer[128] = {0};
    const char *str = getstr(typename);
    len = strlen(str);
    if (len >= sizeof buffer) {
      luaX_syntaxerror(ls, "User defined type name is too long");
      return typename;
    }
    snprintf(buffer, sizeof buffer, "%s", str);
    do {
      typename = str_checkname(ls);
      str = getstr(typename);
      size_t newlen = len + strlen(str) + 1;
      if (newlen >= sizeof buffer) {
        luaX_syntaxerror(ls, "User defined type name is too long");
        return typename;
      }
      snprintf(buffer + len, sizeof buffer - len, ".%s", str);
      len = newlen;
    } while (testnext(ls, '.'));
    typename = luaX_newstring(ls, buffer, strlen(buffer));
  }
  return typename;
}

/* RAVI Parse
 *   name : type
 *   where type is 'integer', 'integer[]',
 *                 'number', 'number[]'
 */
static struct lua_symbol *declare_local_variable(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* assume a dynamic type */
  ravitype_t tt = RAVI_TANY;
  TString *name = check_name_and_next(ls);
  TString *pusertype = NULL;
  if (testnext(ls, ':')) {
    TString *typename = str_checkname(ls); /* we expect a type name */
    const char *str = getstr(typename);
    /* following is not very nice but easy as
     * the lexer doesn't need to be changed
     */
    if (strcmp(str, "integer") == 0)
      tt = RAVI_TNUMINT;
    else if (strcmp(str, "number") == 0)
      tt = RAVI_TNUMFLT;
    else if (strcmp(str, "closure") == 0)
      tt = RAVI_TFUNCTION;
    else if (strcmp(str, "table") == 0)
      tt = RAVI_TTABLE;
    else if (strcmp(str, "string") == 0)
      tt = RAVI_TSTRING;
    else if (strcmp(str, "boolean") == 0)
      tt = RAVI_TBOOLEAN;
    else if (strcmp(str, "any") == 0)
      tt = RAVI_TANY;
    else {
      /* default is a userdata type */
      tt = RAVI_TUSERDATA;
      typename = user_defined_type_name(ls, typename);
      // str = getstr(typename);
      pusertype = typename;
    }
    if (tt == RAVI_TNUMFLT || tt == RAVI_TNUMINT) {
      /* if we see [] then it is an array type */
      if (testnext(ls, '[')) {
        checknext(ls, ']');
        tt = (tt == RAVI_TNUMFLT) ? RAVI_TARRAYFLT : RAVI_TARRAYINT;
      }
    }
  }
  return new_local_symbol(parser, name, tt, pusertype);
}

static bool parse_parameter_list(struct parser_state *parser, struct lua_symbol_list **list) {
  LexState *ls = parser->ls;
  /* parlist -> [ param { ',' param } ] */
  int nparams = 0;
  bool is_vararg = false;
  if (ls->t.token != ')') { /* is 'parlist' not empty? */
    do {
      switch (ls->t.token) {
        case TK_NAME: { /* param -> NAME */
                        /* RAVI change - add type */
          struct lua_symbol *symbol = declare_local_variable(parser);
          add_symbol(parser->container, list, symbol);
          nparams++;
          break;
        }
        case TK_DOTS: { /* param -> '...' */
          luaX_next(ls);
          is_vararg = true; /* declared vararg */
          break;
        }
        default:
          luaX_syntaxerror(ls, "<name> or '...' expected");
      }
    } while (!is_vararg && testnext(ls, ','));
  }
  return is_vararg;
}

static void parse_function_body(struct parser_state *parser, struct ast_node *func_ast, int ismethod, int line) {
  LexState *ls = parser->ls;
  /* body ->  '(' parlist ')' block END */
  checknext(ls, '(');
  if (ismethod) {
    struct lua_symbol *symbol = new_localvarliteral(parser, "self"); /* create 'self' parameter */
    add_symbol(parser->container, &func_ast->function_expr.args, symbol);
  }
  bool is_vararg = parse_parameter_list(parser, &func_ast->function_expr.args);
  func_ast->function_expr.is_vararg = is_vararg;
  func_ast->function_expr.is_method = ismethod;
  checknext(ls, ')');
  parse_statement_list(parser, &func_ast->function_expr.function_statement_list);
  check_match(ls, TK_END, TK_FUNCTION, line);
}

/* parse expression list */
static int parse_expression_list(struct parser_state *parser, struct ast_node_list **list) {
  LexState *ls = parser->ls;
  /* explist -> expr { ',' expr } */
  int n = 1; /* at least one expression */
  struct ast_node *expr = parse_expression(parser);
  add_ast_node(parser->container, list, expr);
  while (testnext(ls, ',')) {
    expr = parse_expression(parser);
    add_ast_node(parser->container, list, expr);
    n++;
  }
  return n;
}

/* parse function arguments */
static struct ast_node *parse_function_call(struct parser_state *parser, TString *methodname, int line) {
  LexState *ls = parser->ls;
  struct ast_node *call_expr = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  call_expr->type = AST_FUNCTION_CALL_EXPR;
  call_expr->function_call_expr.methodname = methodname;
  call_expr->function_call_expr.arg_list = NULL;
  set_type(call_expr->function_call_expr.type, RAVI_TANY);
  switch (ls->t.token) {
    case '(': { /* funcargs -> '(' [ explist ] ')' */
      luaX_next(ls);
      if (ls->t.token == ')') /* arg list is empty? */
        ;
      else {
        parse_expression_list(parser, &call_expr->function_call_expr.arg_list);
      }
      check_match(ls, ')', '(', line);
      break;
    }
    case '{': { /* funcargs -> constructor */
      struct ast_node *table_expr = parse_table_constructor(parser);
      add_ast_node(parser->container, &call_expr->function_call_expr.arg_list, table_expr);
      break;
    }
    case TK_STRING: { /* funcargs -> STRING */
      struct ast_node *string_expr = new_literal_expression(parser, RAVI_TSTRING);
      string_expr->literal_expr.u.s = ls->t.seminfo.ts;
      add_ast_node(parser->container, &call_expr->function_call_expr.arg_list, string_expr);
      luaX_next(ls);
      break;
    }
    default: {
      luaX_syntaxerror(ls, "function arguments expected");
    }
  }
  return call_expr;
}

/*
** {======================================================================
** Expression parsing
** =======================================================================
*/

/* primary expression - name or subexpression */
static struct ast_node *parse_primary_expression(struct parser_state *parser) {
  LexState *ls = parser->ls;
  struct ast_node *primary_expr = NULL;
  /* primaryexp -> NAME | '(' expr ')' */
  switch (ls->t.token) {
    case '(': {
      int line = ls->linenumber;
      luaX_next(ls);
      primary_expr = parse_expression(parser);
      check_match(ls, ')', '(', line);
      break;
    }
    case TK_NAME: {
      primary_expr = new_symbol_reference(parser);
      break;
    }
    default: {
      luaX_syntaxerror(ls, "unexpected symbol");
    }
  }
  assert(primary_expr);
  return primary_expr;
}

/* variable or field access or function call */
static struct ast_node *parse_suffixed_expression(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* suffixedexp ->
  primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs } */
  int line = ls->linenumber;
  struct ast_node *suffixed_expr = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  suffixed_expr->suffixed_expr.primary_expr = parse_primary_expression(parser);
  suffixed_expr->type = AST_SUFFIXED_EXPR;
  suffixed_expr->suffixed_expr.type = suffixed_expr->suffixed_expr.primary_expr->common_expr.type;
  suffixed_expr->suffixed_expr.suffix_list = NULL;
  for (;;) {
    switch (ls->t.token) {
      case '.': { /* fieldsel */
        struct ast_node *suffix = parse_field_selector(parser);
        add_ast_node(parser->container, &suffixed_expr->suffixed_expr.suffix_list, suffix);
        set_type(suffixed_expr->suffixed_expr.type, RAVI_TANY);
        break;
      }
      case '[': { /* '[' exp1 ']' */
        struct ast_node *suffix = parse_yindex(parser);
        add_ast_node(parser->container, &suffixed_expr->suffixed_expr.suffix_list, suffix);
        set_type(suffixed_expr->suffixed_expr.type, RAVI_TANY);
        break;
      }
      case ':': { /* ':' NAME funcargs */
        luaX_next(ls);
        TString *methodname = check_name_and_next(ls);
        struct ast_node *suffix = parse_function_call(parser, methodname, line);
        add_ast_node(parser->container, &suffixed_expr->suffixed_expr.suffix_list, suffix);
        break;
      }
      case '(':
      case TK_STRING:
      case '{': { /* funcargs */
        struct ast_node *suffix = parse_function_call(parser, NULL, line);
        add_ast_node(parser->container, &suffixed_expr->suffixed_expr.suffix_list, suffix);
        break;
      }
      default:
        return suffixed_expr;
    }
  }
}

static struct ast_node *new_literal_expression(struct parser_state *parser, ravitype_t type) {
  struct ast_node *expr = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  expr->type = AST_LITERAL_EXPR;
  set_type(expr->literal_expr.type, type);
  expr->literal_expr.u.i = 0; /* initialize */
  return expr;
}

static struct ast_node *parse_simple_expression(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* simpleexp -> FLT | INT | STRING | NIL | TRUE | FALSE | ... |
  constructor | FUNCTION body | suffixedexp */
  struct ast_node *expr = NULL;
  switch (ls->t.token) {
    case TK_FLT: {
      expr = new_literal_expression(parser, RAVI_TNUMFLT);
      expr->literal_expr.u.n = ls->t.seminfo.r;
      break;
    }
    case TK_INT: {
      expr = new_literal_expression(parser, RAVI_TNUMINT);
      expr->literal_expr.u.i = ls->t.seminfo.i;
      break;
    }
    case TK_STRING: {
      expr = new_literal_expression(parser, RAVI_TSTRING);
      expr->literal_expr.u.s = ls->t.seminfo.ts;
      break;
    }
    case TK_NIL: {
      expr = new_literal_expression(parser, RAVI_TNIL);
      expr->literal_expr.u.i = -1;
      break;
    }
    case TK_TRUE: {
      expr = new_literal_expression(parser, RAVI_TBOOLEAN);
      expr->literal_expr.u.i = 1;
      break;
    }
    case TK_FALSE: {
      expr = new_literal_expression(parser, RAVI_TBOOLEAN);
      expr->literal_expr.u.i = 0;
      break;
    }
    case TK_DOTS: { /* vararg */
      // Not handled yet
      expr = NULL;
      break;
    }
    case '{': { /* constructor */
      return parse_table_constructor(parser);
    }
    case TK_FUNCTION: {
      luaX_next(ls);
      struct ast_node *function_ast = new_function(parser);
      parse_function_body(parser, function_ast, 0, ls->linenumber);
      end_function(parser);
      return function_ast;
    }
    default: {
      return parse_suffixed_expression(parser);
    }
  }
  luaX_next(ls);
  return expr;
}

static UnOpr get_unary_opr(int op) {
  switch (op) {
    case TK_NOT:
      return OPR_NOT;
    case '-':
      return OPR_MINUS;
    case '~':
      return OPR_BNOT;
    case '#':
      return OPR_LEN;
    case TK_TO_INTEGER:
      return OPR_TO_INTEGER;
    case TK_TO_NUMBER:
      return OPR_TO_NUMBER;
    case TK_TO_INTARRAY:
      return OPR_TO_INTARRAY;
    case TK_TO_NUMARRAY:
      return OPR_TO_NUMARRAY;
    case TK_TO_TABLE:
      return OPR_TO_TABLE;
    case TK_TO_STRING:
      return OPR_TO_STRING;
    case TK_TO_CLOSURE:
      return OPR_TO_CLOSURE;
    case '@':
      return OPR_TO_TYPE;
    default:
      return OPR_NOUNOPR;
  }
}

static BinOpr get_binary_opr(int op) {
  switch (op) {
    case '+':
      return OPR_ADD;
    case '-':
      return OPR_SUB;
    case '*':
      return OPR_MUL;
    case '%':
      return OPR_MOD;
    case '^':
      return OPR_POW;
    case '/':
      return OPR_DIV;
    case TK_IDIV:
      return OPR_IDIV;
    case '&':
      return OPR_BAND;
    case '|':
      return OPR_BOR;
    case '~':
      return OPR_BXOR;
    case TK_SHL:
      return OPR_SHL;
    case TK_SHR:
      return OPR_SHR;
    case TK_CONCAT:
      return OPR_CONCAT;
    case TK_NE:
      return OPR_NE;
    case TK_EQ:
      return OPR_EQ;
    case '<':
      return OPR_LT;
    case TK_LE:
      return OPR_LE;
    case '>':
      return OPR_GT;
    case TK_GE:
      return OPR_GE;
    case TK_AND:
      return OPR_AND;
    case TK_OR:
      return OPR_OR;
    default:
      return OPR_NOBINOPR;
  }
}

static const struct {
  lu_byte left;  /* left priority for each binary operator */
  lu_byte right; /* right priority */
} priority[] = {
    /* ORDER OPR */
    {10, 10}, {10, 10},         /* '+' '-' */
    {11, 11}, {11, 11},         /* '*' '%' */
    {14, 13},                   /* '^' (right associative) */
    {11, 11}, {11, 11},         /* '/' '//' */
    {6, 6},   {4, 4},   {5, 5}, /* '&' '|' '~' */
    {7, 7},   {7, 7},           /* '<<' '>>' */
    {9, 8},                     /* '..' (right associative) */
    {3, 3},   {3, 3},   {3, 3}, /* ==, <, <= */
    {3, 3},   {3, 3},   {3, 3}, /* ~=, >, >= */
    {2, 2},   {1, 1}            /* and, or */
};

#define UNARY_PRIORITY 12 /* priority for unary operators */

/*
** subexpr -> (simpleexp | unop subexpr) { binop subexpr }
** where 'binop' is any binary operator with a priority higher than 'limit'
*/
static struct ast_node *parse_sub_expression(struct parser_state *parser, int limit, BinOpr *untreated_op) {
  LexState *ls = parser->ls;
  BinOpr op;
  UnOpr uop;
  struct ast_node *expr = NULL;
  uop = get_unary_opr(ls->t.token);
  if (uop != OPR_NOUNOPR) {
    // RAVI change - get usertype if @<name>
    TString *usertype = NULL;
    if (uop == OPR_TO_TYPE) {
      usertype = ls->t.seminfo.ts;
      luaX_next(ls);
      // Check and expand to extended name if necessary
      usertype = user_defined_type_name(ls, usertype);
    }
    else {
      luaX_next(ls);
    }
    BinOpr ignored;
    struct ast_node *subexpr = parse_sub_expression(parser, UNARY_PRIORITY, &ignored);
    expr = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
    expr->type = AST_UNARY_EXPR;
    expr->unary_expr.expr = subexpr;
    expr->unary_expr.unary_op = uop;
	expr->unary_expr.type.type_name = usertype;
  }
  else {
    expr = parse_simple_expression(parser);
  }
  /* expand while operators have priorities higher than 'limit' */
  op = get_binary_opr(ls->t.token);
  while (op != OPR_NOBINOPR && priority[op].left > limit) {
    BinOpr nextop;
    luaX_next(ls);
    /* read sub-expression with higher priority */
    struct ast_node *exprright = parse_sub_expression(parser, priority[op].right, &nextop);

    struct ast_node *binexpr = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
    binexpr->type = AST_BINARY_EXPR;
    binexpr->binary_expr.exprleft = expr;
    binexpr->binary_expr.exprright = exprright;
    binexpr->binary_expr.binary_op = op;
    expr = binexpr;                                                // Becomes the left expr for next iteration
    op = nextop;
  }
  *untreated_op = op; /* return first untreated operator */
  return expr;
}

static struct ast_node *parse_expression(struct parser_state *parser) {
  BinOpr ignored;
  return parse_sub_expression(parser, 0, &ignored);
}

/* }==================================================================== */

/*
** {======================================================================
** Rules for Statements
** =======================================================================
*/

static struct block_scope *parse_block(struct parser_state *parser, struct ast_node_list **statement_list) {
  /* block -> statlist */
  struct block_scope *scope = new_scope(parser);
  parse_statement_list(parser, statement_list);
  end_scope(parser);
  return scope;
}

/* parse condition in a repeat statement or an if control structure
 * called by repeatstat(), test_then_block()
 */
static struct ast_node *parse_condition(struct parser_state *parser) {
  /* cond -> exp */
  return parse_expression(parser); /* read condition */
}

static struct ast_node *parse_goto_statment(struct parser_state *parser) {
  LexState *ls = parser->ls;
  TString *label;
  if (testnext(ls, TK_GOTO))
    label = check_name_and_next(ls);
  else {
    luaX_next(ls); /* skip break */
    label = luaX_newstring(ls, "break", sizeof "break");
  }
  // Resolve labels in the end?
  struct ast_node *goto_stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  goto_stmt->type = AST_GOTO_STMT;
  goto_stmt->goto_stmt.name = label;
  goto_stmt->goto_stmt.label_stmt = NULL;  // unresolved
  return goto_stmt;
}

/* skip no-op statements */
static void skip_noop_statements(struct parser_state *parser) {
  LexState *ls = parser->ls;
  while (ls->t.token == ';')  //  || ls->t.token == TK_DBCOLON)
    parse_statement(parser);
}

static struct ast_node *generate_label(struct parser_state *parser, TString *label) {
  struct lua_symbol *symbol = new_label(parser, label);
  struct ast_node *label_stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  label_stmt->type = AST_LABEL_STMT;
  label_stmt->label_stmt.symbol = symbol;
  return label_stmt;
}

static struct ast_node *parse_label_statement(struct parser_state *parser, TString *label, int line) {
  (void)line;
  LexState *ls = parser->ls;
  /* label -> '::' NAME '::' */
  checknext(ls, TK_DBCOLON); /* skip double colon */
  /* create new entry for this label */
  struct ast_node *label_stmt = generate_label(parser, label);
  skip_noop_statements(parser); /* skip other no-op statements */
  return label_stmt;
}

static struct ast_node *parse_while_statement(struct parser_state *parser, int line) {
  LexState *ls = parser->ls;
  /* whilestat -> WHILE cond DO block END */
  luaX_next(ls); /* skip WHILE */
  struct ast_node *stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  stmt->type = AST_WHILE_STMT;
  stmt->while_or_repeat_stmt.loop_scope = NULL;
  stmt->while_or_repeat_stmt.loop_statement_list = NULL;
  stmt->while_or_repeat_stmt.condition = parse_condition(parser);
  checknext(ls, TK_DO);
  stmt->while_or_repeat_stmt.loop_scope = parse_block(parser, &stmt->while_or_repeat_stmt.loop_statement_list);
  check_match(ls, TK_END, TK_WHILE, line);
  return stmt;
}

static struct ast_node *parse_repeat_statement(struct parser_state *parser, int line) {
  LexState *ls = parser->ls;
  /* repeatstat -> REPEAT block UNTIL cond */
  luaX_next(ls); /* skip REPEAT */
  struct ast_node *stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  stmt->type = AST_REPEAT_STMT;
  stmt->while_or_repeat_stmt.condition = NULL;
  stmt->while_or_repeat_stmt.loop_statement_list = NULL;
  stmt->while_or_repeat_stmt.loop_scope = new_scope(parser); /* scope block */
  parse_statement_list(parser, &stmt->while_or_repeat_stmt.loop_statement_list);
  check_match(ls, TK_UNTIL, TK_REPEAT, line);
  stmt->while_or_repeat_stmt.condition = parse_condition(parser); /* read condition (inside scope block) */
  end_scope(parser);
  return stmt;
}

/* parse a for loop body for both versions of the for loop */
static void parse_forbody(struct parser_state *parser, struct ast_node *stmt, int line, int nvars, int isnum) {
  (void)line;
  (void)nvars;
  (void)isnum;
  LexState *ls = parser->ls;
  /* forbody -> DO block */
  checknext(ls, TK_DO);
  stmt->for_stmt.for_body = parse_block(parser, &stmt->for_stmt.for_statement_list);
}

/* parse a numerical for loop */
static void parse_fornum_statement(struct parser_state *parser, struct ast_node *stmt, TString *varname, int line) {
  LexState *ls = parser->ls;
  /* fornum -> NAME = exp1,exp1[,exp1] forbody */
  add_symbol(parser->container, &stmt->for_stmt.symbols, new_local_symbol(parser, varname, RAVI_TANY, NULL));
  checknext(ls, '=');
  /* get the type of each expression */
  add_ast_node(parser->container, &stmt->for_stmt.expressions, parse_expression(parser)); /* initial value */
  checknext(ls, ',');
  add_ast_node(parser->container, &stmt->for_stmt.expressions, parse_expression(parser)); /* limit */
  if (testnext(ls, ',')) {
    add_ast_node(parser->container, &stmt->for_stmt.expressions, parse_expression(parser)); /* optional step */
  }
  parse_forbody(parser, stmt, line, 1, 1);
}

/* parse a generic for loop */
static void parse_for_list(struct parser_state *parser, struct ast_node *stmt, TString *indexname) {
  LexState *ls = parser->ls;
  /* forlist -> NAME {,NAME} IN explist forbody */
  int nvars = 4; /* gen, state, control, plus at least one declared var */
  /* create declared variables */
  add_symbol(parser->container, &stmt->for_stmt.symbols, new_local_symbol(parser, indexname, RAVI_TANY, NULL));
  while (testnext(ls, ',')) {
    add_symbol(parser->container, &stmt->for_stmt.symbols,
               new_local_symbol(parser, check_name_and_next(ls), RAVI_TANY, NULL));
    nvars++;
  }
  checknext(ls, TK_IN);
  parse_expression_list(parser, &stmt->for_stmt.expressions);
  int line = ls->linenumber;
  parse_forbody(parser, stmt, line, nvars - 3, 0);
}

/* initial parsing of a for loop - calls fornum() or forlist() */
static struct ast_node *parse_for_statement(struct parser_state *parser, int line) {
  LexState *ls = parser->ls;
  /* forstat -> FOR (fornum | forlist) END */
  TString *varname;
  struct ast_node *stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  stmt->type = AST_NONE;
  stmt->for_stmt.symbols = NULL;
  stmt->for_stmt.expressions = NULL;
  stmt->for_stmt.for_body = NULL;
  stmt->for_stmt.for_statement_list = NULL;
  new_scope(parser);                 // For the loop variables
  luaX_next(ls);                     /* skip 'for' */
  varname = check_name_and_next(ls); /* first variable name */
  switch (ls->t.token) {
    case '=':
      stmt->type = AST_FORNUM_STMT;
      parse_fornum_statement(parser, stmt, varname, line);
      break;
    case ',':
    case TK_IN:
      stmt->type = AST_FORIN_STMT;
      parse_for_list(parser, stmt, varname);
      break;
    default:
      luaX_syntaxerror(ls, "'=' or 'in' expected");
  }
  check_match(ls, TK_END, TK_FOR, line);
  end_scope(parser);
  return stmt;
}

/* parse if cond then block - called from parse_if_statement() */
static struct ast_node *parse_if_cond_then_block(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* test_then_block -> [IF | ELSEIF] cond THEN block */
  luaX_next(ls); /* skip IF or ELSEIF */
  struct ast_node *test_then_block = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  test_then_block->type = AST_NONE;                                      // This is not an AST node on its own
  test_then_block->test_then_block.condition = parse_expression(parser); /* read condition */
  test_then_block->test_then_block.test_then_scope = NULL;
  test_then_block->test_then_block.test_then_statement_list = NULL;
  checknext(ls, TK_THEN);
  if (ls->t.token == TK_GOTO || ls->t.token == TK_BREAK) {
    test_then_block->test_then_block.test_then_scope = new_scope(parser);
    struct ast_node *stmt = parse_goto_statment(parser); /* handle goto/break */
    add_ast_node(parser->container, &test_then_block->test_then_block.test_then_statement_list, stmt);
    skip_noop_statements(parser); /* skip other no-op statements */
    if (block_follow(ls, 0)) {    /* 'goto' is the entire block? */
      end_scope(parser);
      return test_then_block; /* and that is it */
    }
    else { /* must skip over 'then' part if condition is false */
      ;
    }
  }
  else { /* regular case (not goto/break) */
    test_then_block->test_then_block.test_then_scope = new_scope(parser);
  }
  parse_statement_list(parser, &test_then_block->test_then_block.test_then_statement_list); /* 'then' part */
  end_scope(parser);
  return test_then_block;
}

static struct ast_node *parse_if_statement(struct parser_state *parser, int line) {
  LexState *ls = parser->ls;
  /* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
  struct ast_node *stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  stmt->type = AST_IF_STMT;
  stmt->if_stmt.if_condition_list = NULL;
  stmt->if_stmt.else_block = NULL;
  stmt->if_stmt.else_statement_list = NULL;
  struct ast_node *test_then_block = parse_if_cond_then_block(parser); /* IF cond THEN block */
  add_ast_node(parser->container, &stmt->if_stmt.if_condition_list, test_then_block);
  while (ls->t.token == TK_ELSEIF) {
    test_then_block = parse_if_cond_then_block(parser); /* ELSEIF cond THEN block */
    add_ast_node(parser->container, &stmt->if_stmt.if_condition_list, test_then_block);
  }
  if (testnext(ls, TK_ELSE))
    stmt->if_stmt.else_block = parse_block(parser, &stmt->if_stmt.else_statement_list); /* 'else' part */
  check_match(ls, TK_END, TK_IF, line);
  return stmt;
}

static struct ast_node *parse_local_function_statement(struct parser_state *parser) {
  LexState *ls = parser->ls;
  struct lua_symbol *symbol =
      new_local_symbol(parser, check_name_and_next(ls), RAVI_TFUNCTION, NULL); /* new local variable */
  struct ast_node *function_ast = new_function(parser);
  parse_function_body(parser, function_ast, 0, ls->linenumber); /* function created in next register */
  end_function(parser);
  struct ast_node *stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  stmt->type = AST_LOCAL_STMT;
  stmt->local_stmt.vars = NULL;
  stmt->local_stmt.exprlist = NULL;
  add_symbol(parser->container, &stmt->local_stmt.vars, symbol);
  add_ast_node(parser->container, &stmt->local_stmt.exprlist, function_ast);
  return stmt;
}

static struct ast_node *parse_local_statement(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* stat -> LOCAL NAME {',' NAME} ['=' explist] */
  struct ast_node *node = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  node->type = AST_LOCAL_STMT;
  node->local_stmt.vars = NULL;
  node->local_stmt.exprlist = NULL;
  int nvars = 0;
  do {
    /* local name : type = value */
    struct lua_symbol *symbol = declare_local_variable(parser);
    add_symbol(parser->container, &node->local_stmt.vars, symbol);
    nvars++;
    if (nvars >= MAXVARS)
      luaX_syntaxerror(ls, "too many local variables");
  } while (testnext(ls, ','));
  if (testnext(ls, '=')) /* nexps = */
    parse_expression_list(parser, &node->local_stmt.exprlist);
  else {
    /* nexps = 0; */
    ;
  }
  return node;
}

/* parse a function name specification with base symbol, optional selectors and optional method name
 */
static struct ast_node *parse_function_name(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* funcname -> NAME {fieldsel} [':' NAME] */
  struct ast_node *function_stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  function_stmt->type = AST_FUNCTION_STMT;
  function_stmt->function_stmt.function_expr = NULL;
  function_stmt->function_stmt.methodname = NULL;
  function_stmt->function_stmt.selectors = NULL;
  function_stmt->function_stmt.name = new_symbol_reference(parser);
  while (ls->t.token == '.') {
    add_ast_node(parser->container, &function_stmt->function_stmt.selectors, parse_field_selector(parser));
  }
  if (ls->t.token == ':') {
    function_stmt->function_stmt.methodname = parse_field_selector(parser);
  }
  return function_stmt;
}

static struct ast_node *parse_function_statement(struct parser_state *parser, int line) {
  LexState *ls = parser->ls;
  /* funcstat -> FUNCTION funcname body */
  luaX_next(ls); /* skip FUNCTION */
  struct ast_node *function_stmt = parse_function_name(parser);
  int ismethod = function_stmt->function_stmt.methodname != NULL;
  struct ast_node *function_ast = new_function(parser);
  parse_function_body(parser, function_ast, ismethod, line);
  end_function(parser);
  function_stmt->function_stmt.function_expr = function_ast;
  return function_stmt;
}

/* parse function call with no returns or assignment statement */
static struct ast_node *parse_expression_statement(struct parser_state *parser) {
  struct ast_node *stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  stmt->type = AST_EXPR_STMT;
  stmt->expression_stmt.var_expr_list = NULL;
  stmt->expression_stmt.exr_list = NULL;
  LexState *ls = parser->ls;
  /* stat -> func | assignment */
  /* Until we see '=' we do not know if this is an assignment or expr list*/
  struct ast_node_list *current_list = NULL;
  add_ast_node(parser->container, &current_list, parse_suffixed_expression(parser));
  while (testnext(ls, ',')) { /* assignment -> ',' suffixedexp assignment */
    add_ast_node(parser->container, &current_list, parse_suffixed_expression(parser));
  }
  if (ls->t.token == '=') { /* stat -> assignment ? */
    checknext(ls, '=');
    stmt->expression_stmt.var_expr_list = current_list;
    current_list = NULL;
    parse_expression_list(parser, &current_list);
  }
  stmt->expression_stmt.exr_list = current_list;
  // TODO Check that if not assignment then it is a function call
  return stmt;
}

static struct ast_node *parse_return_statement(struct parser_state *parser) {
  LexState *ls = parser->ls;
  /* stat -> RETURN [explist] [';'] */
  struct ast_node *return_stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  return_stmt->type = AST_RETURN_STMT;
  return_stmt->return_stmt.exprlist = NULL;
  if (block_follow(ls, 1) || ls->t.token == ';')
    /* nret = 0*/; /* return no values */
  else {
    /*nret = */
    parse_expression_list(parser, &return_stmt->return_stmt.exprlist); /* optional return values */
  }
  testnext(ls, ';'); /* skip optional semicolon */
  return return_stmt;
}

static struct ast_node *parse_do_statement(struct parser_state *parser, int line) {
  luaX_next(parser->ls); /* skip DO */
  struct ast_node *stmt = dmrC_allocator_allocate(&parser->container->ast_node_allocator, 0);
  stmt->type = AST_DO_STMT;
  stmt->do_stmt.do_statement_list = NULL;
  stmt->do_stmt.scope = parse_block(parser, &stmt->do_stmt.do_statement_list);
  check_match(parser->ls, TK_END, TK_DO, line);
  return stmt;
}

/* parse a statement */
static struct ast_node *parse_statement(struct parser_state *parser) {
  LexState *ls = parser->ls;
  int line = ls->linenumber; /* may be needed for error messages */
  struct ast_node *stmt = NULL;
  switch (ls->t.token) {
    case ';': {      /* stat -> ';' (empty statement) */
      luaX_next(ls); /* skip ';' */
      break;
    }
    case TK_IF: { /* stat -> ifstat */
      stmt = parse_if_statement(parser, line);
      break;
    }
    case TK_WHILE: { /* stat -> whilestat */
      stmt = parse_while_statement(parser, line);
      break;
    }
    case TK_DO: { /* stat -> DO block END */
      stmt = parse_do_statement(parser, line);
      break;
    }
    case TK_FOR: { /* stat -> forstat */
      stmt = parse_for_statement(parser, line);
      break;
    }
    case TK_REPEAT: { /* stat -> repeatstat */
      stmt = parse_repeat_statement(parser, line);
      break;
    }
    case TK_FUNCTION: { /* stat -> funcstat */
      stmt = parse_function_statement(parser, line);
      break;
    }
    case TK_LOCAL: {                 /* stat -> localstat */
      luaX_next(ls);                 /* skip LOCAL */
      if (testnext(ls, TK_FUNCTION)) /* local function? */
        stmt = parse_local_function_statement(parser);
      else
        stmt = parse_local_statement(parser);
      break;
    }
    case TK_DBCOLON: { /* stat -> label */
      luaX_next(ls);   /* skip double colon */
      stmt = parse_label_statement(parser, check_name_and_next(ls), line);
      break;
    }
    case TK_RETURN: { /* stat -> retstat */
      luaX_next(ls);  /* skip RETURN */
      stmt = parse_return_statement(parser);
      break;
    }
    case TK_BREAK:  /* stat -> breakstat */
    case TK_GOTO: { /* stat -> 'goto' NAME */
      stmt = parse_goto_statment(parser);
      break;
    }
    default: { /* stat -> func | assignment */
      stmt = parse_expression_statement(parser);
      break;
    }
  }
  return stmt;
}

/* Parses a sequence of statements */
/* statlist -> { stat [';'] } */
static void parse_statement_list(struct parser_state *parser, struct ast_node_list **list) {
  LexState *ls = parser->ls;
  while (!block_follow(ls, 1)) {
    bool was_return = ls->t.token == TK_RETURN;
    struct ast_node *stmt = parse_statement(parser);
    if (stmt)
      add_ast_node(parser->container, list, stmt);
    if (was_return)
      break; /* 'return' must be last statement */
  }
}

/* Starts a new scope. If the current function has no main block
 * defined then the new scope becomes its main block. The new scope
 * gets existing scope as its parent even if that belongs to parent
 * function.
 */
static struct block_scope *new_scope(struct parser_state *parser) {
  struct ast_container *container = parser->container;
  struct block_scope *scope = dmrC_allocator_allocate(&container->block_scope_allocator, 0);
  scope->symbol_list = NULL;
  // scope->do_statement_list = NULL;
  scope->function = parser->current_function;
  assert(scope->function && scope->function->type == AST_FUNCTION_EXPR);
  scope->parent = parser->current_scope;
  parser->current_scope = scope;
  if (!parser->current_function->function_expr.main_block)
    parser->current_function->function_expr.main_block = scope;
  return scope;
}

static void end_scope(struct parser_state *parser) {
  assert(parser->current_scope);
  struct block_scope *scope = parser->current_scope;
  parser->current_scope = scope->parent;
  assert(parser->current_scope != NULL || scope == parser->current_function->function_expr.main_block);
}

/* Creates a new function AST node and starts the function scope.
New function becomes child of current function if any, and scope is linked
to previous scope which may be of parent function.
*/
static struct ast_node *new_function(struct parser_state *parser) {
  struct ast_container *container = parser->container;
  struct ast_node *node = dmrC_allocator_allocate(&container->ast_node_allocator, 0);
  node->type = AST_FUNCTION_EXPR;
  set_type(node->function_expr.type, RAVI_TFUNCTION);
  node->function_expr.is_method = false;
  node->function_expr.is_vararg = false;
  node->function_expr.args = NULL;
  node->function_expr.child_functions = NULL;
  node->function_expr.upvalues = NULL;
  node->function_expr.locals = NULL;
  node->function_expr.main_block = NULL;
  node->function_expr.function_statement_list = NULL;
  node->function_expr.parent_function = parser->current_function;
  if (parser->current_function) {
    // Make this function a child of current function
    add_ast_node(parser->container, &parser->current_function->function_expr.child_functions, node);
  }
  parser->current_function = node;
  new_scope(parser); /* Start function scope */
  return node;
}

/* Ends the function node and closes the scope for the function. The
 * function being closed becomes the current AST node, while parent function/scope
 * become current function/scope.
 */
static struct ast_node *end_function(struct parser_state *parser) {
  assert(parser->current_function);
  end_scope(parser);
  struct ast_node *function = parser->current_function;
  parser->current_function = function->function_expr.parent_function;
  return function;
}

/* mainfunc() equivalent - parses a Lua script, also known as chunk.
The code is wrapped in a vararg function */
static void parse_lua_chunk(struct parser_state *parser) {
  luaX_next(parser->ls);                                   /* read first token */
  parser->container->main_function = new_function(parser); /* vararg function wrapper */
  parser->container->main_function->function_expr.is_vararg = true;
  parse_statement_list(parser, &parser->container->main_function->function_expr.function_statement_list);
  end_function(parser);
  assert(parser->current_function == NULL);
  assert(parser->current_scope == NULL);
  check(parser->ls, TK_EOS);
  do_typechecks(parser->container);
}

static void parser_state_init(struct parser_state *parser, LexState *ls, struct ast_container *container) {
  parser->ls = ls;
  parser->container = container;
  parser->current_function = NULL;
  parser->current_scope = NULL;
}

static const char *type_name(ravitype_t tt) {
  switch (tt) {
    case RAVI_TANY:
      return "any";
    case RAVI_TNIL:
      return "nil";
    case RAVI_TBOOLEAN:
      return "boolean";
    case RAVI_TNUMFLT:
      return "number";
    case RAVI_TNUMINT:
      return "integer";
    case RAVI_TTABLE:
      return "table";
    case RAVI_TSTRING:
      return "string";
    case RAVI_TARRAYINT:
      return "integer[]";
    case RAVI_TARRAYFLT:
      return "number[]";
    case RAVI_TFUNCTION:
      return "closure";
    case RAVI_TUSERDATA:
      return "";
    default:
      return "";
  }
}

static void printf_buf(membuff_t *buf, const char *format, ...) {
  static const char *PADDING = "                                                                                ";
  char tbuf[128] = {0};
  va_list ap;
  const char *cp;
  va_start(ap, format);
  for (cp = format; *cp; cp++) {
    if (cp[0] == '%' && cp[1] == 'p') { /* padding */
      int level = va_arg(ap, int);
      snprintf(tbuf, sizeof tbuf, "%.*s", level, PADDING);
      membuff_add_string(buf, tbuf);
      cp++;
    }
    else if (cp[0] == '%' && cp[1] == 't') { /* TString */
      const TString *ts;
      ts = va_arg(ap, const TString *);
      const char *s = getstr(ts);
      membuff_add_string(buf, s);
      cp++;
    }
    else if (cp[0] == '%' && cp[1] == 'T') { /* struct var_type */
      const struct var_type *type;
      type = va_arg(ap, const struct var_type *);
      if (type->type_code == RAVI_TUSERDATA) {
        const char *s = getstr(type->type_name);
        membuff_add_string(buf, s);
      }
      else {
        membuff_add_string(buf, type_name(type->type_code));
      }
      cp++;
    }
    else if (cp[0] == '%' && cp[1] == 's') { /* const char * */
      const char *s;
      s = va_arg(ap, const char *);
      membuff_add_string(buf, s);
      cp++;
    }
    else if (cp[0] == '%' && cp[1] == 'c') { /* comment */
      const char *s;
      s = va_arg(ap, const char *);
      membuff_add_fstring(buf, "--%s", s);
      cp++;
    }
    else if (cp[0] == '%' && cp[1] == 'i') { /* integer */
      lua_Integer i;
      i = va_arg(ap, lua_Integer);
      membuff_add_longlong(buf, i);
      cp++;
    }
    else if (cp[0] == '%' && cp[1] == 'f') { /* float */
      double d;
      d = va_arg(ap, double);
      membuff_add_fstring(buf, "%.16f", d);
      cp++;
    }
    else if (cp[0] == '%' && cp[1] == 'b') { /* boolean */
      lua_Integer i;
      i = va_arg(ap, lua_Integer);
      membuff_add_bool(buf, i != 0);
      cp++;
    }
    else {
      membuff_add_char(buf, *cp);
    }
  }
  va_end(ap);
}

static void print_ast_node(membuff_t *buf, struct ast_node *node, int level);

static void print_ast_node_list(membuff_t *buf, struct ast_node_list *list, int level, const char *delimiter) {
  struct ast_node *node;
  bool is_first = true;
  FOR_EACH_PTR(list, node) {
    if (is_first)
      is_first = false;
    else if (delimiter)
      printf_buf(buf, "%p%s\n", level, delimiter);
    print_ast_node(buf, node, level + 1);
  }
  END_FOR_EACH_PTR(node);
}

static void print_statement_list(membuff_t *buf, struct ast_node_list *statement_list, int level) {
  print_ast_node_list(buf, statement_list, level + 1, NULL);
}

static inline const char *get_as_str(const TString *ts) { return ts ? getstr(ts) : ""; }

static void print_symbol(membuff_t *buf, struct lua_symbol *sym, int level) {
  switch (sym->symbol_type) {
    case SYM_GLOBAL: {
      printf_buf(buf, "%p%t %c %s %s\n", level, sym->var.var_name, "global symbol",
                 raviY_typename(sym->value_type.type_code), get_as_str(sym->value_type.type_name));
      break;
    }
    case SYM_LOCAL: {
      printf_buf(buf, "%p%t %c %s %s\n", level, sym->var.var_name, "local symbol",
                 raviY_typename(sym->value_type.type_code), get_as_str(sym->value_type.type_name));
      break;
    }
    case SYM_UPVALUE: {
      printf_buf(buf, "%p%t %c %s %s\n", level, sym->upvalue.var->var.var_name, "upvalue",
                 raviY_typename(sym->upvalue.var->value_type.type_code),
                 get_as_str(sym->upvalue.var->value_type.type_name));
      break;
    }
    default:
      assert(0);
  }
}

static void print_symbol_name(membuff_t *buf, struct lua_symbol *sym) {
  switch (sym->symbol_type) {
    case SYM_LOCAL:
    case SYM_GLOBAL: {
      printf_buf(buf, "%t", sym->var.var_name);
      break;
    }
    case SYM_UPVALUE: {
      printf_buf(buf, "%t", sym->upvalue.var->var.var_name);
      break;
    }
    default:
      assert(0);
  }
}

static void print_symbol_list(membuff_t *buf, struct lua_symbol_list *list, int level, const char *delimiter) {
  struct lua_symbol *node;
  bool is_first = true;
  FOR_EACH_PTR(list, node) {
    if (is_first)
      is_first = false;
    else if (delimiter)
      printf_buf(buf, "%p%s\n", level, delimiter);
    print_symbol(buf, node, level + 1);
  }
  END_FOR_EACH_PTR(node);
}

static void print_symbol_names(membuff_t *buf, struct lua_symbol_list *list) {
  struct lua_symbol *node;
  bool is_first = true;
  FOR_EACH_PTR(list, node) {
    if (is_first)
      is_first = false;
    else
      printf_buf(buf, ", ");
    print_symbol_name(buf, node);
  }
  END_FOR_EACH_PTR(node);
}

static const char *get_unary_opr_str(UnOpr op) {
  switch (op) {
    case OPR_NOT:
      return "not";
    case OPR_MINUS:
      return "-";
    case OPR_BNOT:
      return "~";
    case OPR_LEN:
      return "#";
    case OPR_TO_INTEGER:
      return "@integer";
    case OPR_TO_NUMBER:
      return "@number";
    case OPR_TO_INTARRAY:
      return "@integer[]";
    case OPR_TO_NUMARRAY:
      return "@number[]";
    case OPR_TO_TABLE:
      return "@table";
    case OPR_TO_CLOSURE:
      return "@closure";
    case OPR_TO_STRING:
      return "@string";
    case OPR_TO_TYPE:
      return "@<usertype>";
    default:
      return "";
  }
}

static const char *get_binary_opr_str(BinOpr op) {
  switch (op) {
    case OPR_ADD:
      return "+";
    case OPR_SUB:
      return "-";
    case OPR_MUL:
      return "*";
    case OPR_MOD:
      return "%";
    case OPR_POW:
      return "^";
    case OPR_DIV:
      return "/";
    case OPR_IDIV:
      return "//";
    case OPR_BAND:
      return "&";
    case OPR_BOR:
      return "|";
    case OPR_BXOR:
      return "~";
    case OPR_SHL:
      return "<<";
    case OPR_SHR:
      return ">>";
    case OPR_CONCAT:
      return "..";
    case OPR_NE:
      return "~=";
    case OPR_EQ:
      return "==";
    case OPR_LT:
      return "<";
    case OPR_LE:
      return "<=";
    case OPR_GT:
      return ">";
    case OPR_GE:
      return ">=";
    case OPR_AND:
      return "and";
    case OPR_OR:
      return "or";
    default:
      return "";
  }
}

static void print_ast_node(membuff_t *buf, struct ast_node *node, int level) {
  switch (node->type) {
    case AST_FUNCTION_EXPR: {
      if (node->function_expr.args) {
        printf_buf(buf, "%pfunction(\n", level);
        print_symbol_list(buf, node->function_expr.args, level + 1, ",");
        printf_buf(buf, "%p)\n", level);
      }
      else {
        printf_buf(buf, "%pfunction()\n", level);
      }
      if (node->function_expr.locals) {
        printf_buf(buf, "%p%c ", level, "locals ");
        print_symbol_names(buf, node->function_expr.locals);
        printf_buf(buf, "\n");
      }
      if (node->function_expr.upvalues) {
        printf_buf(buf, "%p%c ", level, "upvalues ");
        print_symbol_names(buf, node->function_expr.upvalues);
        printf_buf(buf, "\n");
      }
      print_statement_list(buf, node->function_expr.function_statement_list, level);
      printf_buf(buf, "%pend\n", level);
      break;
    }
    case AST_NONE:
      break;
    case AST_RETURN_STMT: {
      printf_buf(buf, "%preturn\n", level);
      print_ast_node_list(buf, node->return_stmt.exprlist, level + 1, ",");
      break;
    }
    case AST_LOCAL_STMT: {
      printf_buf(buf, "%plocal\n", level);
      printf_buf(buf, "%p%c\n", level, "[symbols]");
      print_symbol_list(buf, node->local_stmt.vars, level + 1, ",");
      if (node->local_stmt.exprlist) {
        printf_buf(buf, "%p%c\n", level, "[expressions]");
        print_ast_node_list(buf, node->local_stmt.exprlist, level + 1, ",");
      }
      break;
    }
    case AST_FUNCTION_STMT: {
      print_ast_node(buf, node->function_stmt.name, level);
      if (node->function_stmt.selectors) {
        printf_buf(buf, "%p%c\n", level + 1, "[selectors]");
        print_ast_node_list(buf, node->function_stmt.selectors, level + 2, NULL);
      }
      if (node->function_stmt.methodname) {
        printf_buf(buf, "%p%c\n", level + 1, "[method name]");
        print_ast_node(buf, node->function_stmt.methodname, level + 2);
      }
      printf_buf(buf, "%p=\n", level + 1);
      print_ast_node(buf, node->function_stmt.function_expr, level + 2);
      break;
    }
    case AST_LABEL_STMT: {
      printf_buf(buf, "%p::%t::\n", level, node->label_stmt.symbol->label.label_name);
      break;
    }
    case AST_GOTO_STMT: {
      printf_buf(buf, "%pgoto %t\n", level, node->goto_stmt.name);
      break;
    }
    case AST_DO_STMT: {
      printf_buf(buf, "%pdo\n", level);
      print_ast_node_list(buf, node->do_stmt.do_statement_list, level + 1, NULL);
      printf_buf(buf, "%pend\n", level);
      break;
    }
    case AST_EXPR_STMT: {
      printf_buf(buf, "%p%c\n", level, "[expression statement start]");
      if (node->expression_stmt.var_expr_list) {
        printf_buf(buf, "%p%c\n", level + 1, "[var list start]");
        print_ast_node_list(buf, node->expression_stmt.var_expr_list, level + 2, ",");
        printf_buf(buf, "%p= %c\n", level + 1, "[var list end]");
      }
      printf_buf(buf, "%p%c\n", level + 1, "[expression list start]");
      print_ast_node_list(buf, node->expression_stmt.exr_list, level + 2, ",");
      printf_buf(buf, "%p%c\n", level + 1, "[expression list end]");
      printf_buf(buf, "%p%c\n", level, "[expression statement end]");
      break;
    }
    case AST_IF_STMT: {
      struct ast_node *test_then_block;
      bool is_first = true;
      FOR_EACH_PTR(node->if_stmt.if_condition_list, test_then_block) {
        if (is_first) {
          is_first = false;
          printf_buf(buf, "%pif\n", level);
        }
        else
          printf_buf(buf, "%pelseif\n", level);
        print_ast_node(buf, test_then_block->test_then_block.condition, level + 1);
        printf_buf(buf, "%pthen\n", level);
        print_ast_node_list(buf, test_then_block->test_then_block.test_then_statement_list, level + 1, NULL);
      }
      END_FOR_EACH_PTR(node);
      if (node->if_stmt.else_block) {
        printf_buf(buf, "%pelse\n", level);
        print_ast_node_list(buf, node->if_stmt.else_statement_list, level + 1, NULL);
      }
      printf_buf(buf, "%pend\n", level);
      break;
    }
    case AST_WHILE_STMT: {
      printf_buf(buf, "%pwhile\n", level);
      print_ast_node(buf, node->while_or_repeat_stmt.condition, level + 1);
      printf_buf(buf, "%pdo\n", level);
      print_ast_node_list(buf, node->while_or_repeat_stmt.loop_statement_list, level + 1, NULL);
      printf_buf(buf, "%pend\n", level);
      break;
    }
    case AST_REPEAT_STMT: {
      printf_buf(buf, "%prepeat\n", level);
      print_ast_node_list(buf, node->while_or_repeat_stmt.loop_statement_list, level + 1, NULL);
      printf_buf(buf, "%puntil\n", level);
      print_ast_node(buf, node->while_or_repeat_stmt.condition, level + 1);
      printf_buf(buf, "%p%c\n", level, "[repeat end]");
      break;
    }
    case AST_FORIN_STMT: {
      printf_buf(buf, "%pfor\n", level);
      print_symbol_list(buf, node->for_stmt.symbols, level + 1, ",");
      printf_buf(buf, "%pin\n", level);
      print_ast_node_list(buf, node->for_stmt.expressions, level + 1, ",");
      printf_buf(buf, "%pdo\n", level);
      print_statement_list(buf, node->for_stmt.for_statement_list, level + 1);
      printf_buf(buf, "%pend\n", level);
      break;
    }
    case AST_FORNUM_STMT: {
      printf_buf(buf, "%pfor\n", level);
      print_symbol_list(buf, node->for_stmt.symbols, level + 1, NULL);
      printf_buf(buf, "%p=\n", level);
      print_ast_node_list(buf, node->for_stmt.expressions, level + 1, ",");
      printf_buf(buf, "%pdo\n", level);
      print_statement_list(buf, node->for_stmt.for_statement_list, level + 1);
      printf_buf(buf, "%pend\n", level);
      break;
    }
    case AST_SUFFIXED_EXPR: {
      printf_buf(buf, "%p%c\n", level, "[primary start]");
      print_ast_node(buf, node->suffixed_expr.primary_expr, level + 1);
      printf_buf(buf, "%p%c\n", level, "[primary end]");
      if (node->suffixed_expr.suffix_list) {
        printf_buf(buf, "%p%c\n", level, "[suffix list start]");
        print_ast_node_list(buf, node->suffixed_expr.suffix_list, level + 1, NULL);
        printf_buf(buf, "%p%c\n", level, "[suffix list end]");
      }
      break;
    }
    case AST_FUNCTION_CALL_EXPR: {
      printf_buf(buf, "%p%c\n", level, "[function call start]");
      if (node->function_call_expr.methodname) {
        printf_buf(buf, "%p: %t (\n", level + 1, node->function_call_expr.methodname);
      }
      else {
        printf_buf(buf, "%p(\n", level + 1);
      }
      print_ast_node_list(buf, node->function_call_expr.arg_list, level + 2, ",");
      printf_buf(buf, "%p)\n", level + 1);
      printf_buf(buf, "%p%c\n", level, "[function call end]");
      break;
    }
    case AST_SYMBOL_EXPR: {
      print_symbol(buf, node->symbol_expr.var, level + 1);
      break;
    }
    case AST_BINARY_EXPR: {
      printf_buf(buf, "%p%c\n", level, "[binary expr start]");
      print_ast_node(buf, node->binary_expr.exprleft, level + 1);
      printf_buf(buf, "%p%s\n", level, get_binary_opr_str(node->binary_expr.binary_op));
      print_ast_node(buf, node->binary_expr.exprright, level + 1);
      printf_buf(buf, "%p%c\n", level, "[binary expr end]");
      break;
    }
    case AST_UNARY_EXPR: {
      printf_buf(buf, "%p%c %T\n", level, "[unary expr start]", &node->unary_expr.type);
      printf_buf(buf, "%p%s\n", level, get_unary_opr_str(node->unary_expr.unary_op));
      print_ast_node(buf, node->unary_expr.expr, level + 1);
      printf_buf(buf, "%p%c\n", level, "[unary expr end]");
      break;
    }
    case AST_LITERAL_EXPR: {
      printf_buf(buf, "%p", level);
      switch (node->literal_expr.type.type_code) {
        case RAVI_TNIL:
          printf_buf(buf, "nil");
          break;
        case RAVI_TBOOLEAN:
          printf_buf(buf, "%b", node->literal_expr.u.i);
          break;
        case RAVI_TNUMINT:
          printf_buf(buf, "%i", node->literal_expr.u.i);
          break;
        case RAVI_TNUMFLT:
          printf_buf(buf, "%f", node->literal_expr.u.n);
          break;
        case RAVI_TSTRING:
          printf_buf(buf, "'%t'", node->literal_expr.u.s);
          break;
        default:
          assert(0);
      }
      printf_buf(buf, "\n");
      break;
    }
    case AST_FIELD_SELECTOR_EXPR: {
      printf_buf(buf, "%p%c\n", level, "[field selector start]");
      printf_buf(buf, "%p.\n", level + 1);
      print_ast_node(buf, node->index_expr.expr, level + 2);
      printf_buf(buf, "%p%c\n", level, "[field selector end]");
      break;
    }
    case AST_Y_INDEX_EXPR: {
      printf_buf(buf, "%p%c\n", level, "[Y index start]");
      printf_buf(buf, "%p[\n", level + 1);
      print_ast_node(buf, node->index_expr.expr, level + 2);
      printf_buf(buf, "%p]\n", level + 1);
      printf_buf(buf, "%p%c\n", level, "[Y index end]");
      break;
    }
    case AST_INDEXED_ASSIGN_EXPR: {
      printf_buf(buf, "%p%c\n", level, "[indexed assign start]");
      if (node->indexed_assign_expr.index_expr) {
        printf_buf(buf, "%p%c\n", level, "[index start]");
        print_ast_node(buf, node->indexed_assign_expr.index_expr, level + 1);
        printf_buf(buf, "%p%c\n", level, "[index end]");
      }
      printf_buf(buf, "%p%c\n", level, "[value start]");
      print_ast_node(buf, node->indexed_assign_expr.value_expr, level + 1);
      printf_buf(buf, "%p%c\n", level, "[value end]");
      printf_buf(buf, "%p%c\n", level, "[indexed assign end]");
      break;
    }
    case AST_TABLE_EXPR: {
      printf_buf(buf, "%p{ %c %T\n", level, "[table constructor start]", &node->table_expr.type);
      print_ast_node_list(buf, node->table_expr.expr_list, level + 1, ",");
      printf_buf(buf, "%p} %c\n", level, "[table constructor end]");
      break;
    }
    default:
      printf_buf(buf, "%pUnsupported node type %d\n", level, node->type);
      assert(0);
  }
}

/*
** Parse the given source 'chunk' and build an abstract
** syntax tree; return 0 on success / non-zero return code on
** failure
** On success will push a userdata object containing the abstract
** syntax tree.
** On failure push an error message.
*/
static int parse_to_ast(lua_State *L, ZIO *z, Mbuffer *buff, const char *name, int firstchar) {
  struct ast_container *container = new_ast_container(L);
  LexState lexstate;
  lexstate.h = luaH_new(L); /* create table for scanner */
  sethvalue(L, L->top, lexstate.h);
  setuservalue(L, uvalue(L->top - 1), L->top); /* set the table as container's uservalue */
  luaD_inctop(L);
  TString *src = luaS_new(L, name); /* create and anchor TString */
  setsvalue(L, L->top, src);
  luaD_inctop(L);
  lexstate.buff = buff;
  lexstate.dyd = NULL; /* Unlike standard Lua parser / code generator we do not use this */
  luaX_setinput(L, &lexstate, z, src, firstchar);
  struct parser_state parser_state;
  parser_state_init(&parser_state, &lexstate, container);
  lua_lock(L);                     // Workaround for ZIO (used by lexer) which assumes lua_lock()
  parse_lua_chunk(&parser_state);  // FIXME must be protected call
  lua_unlock(L);
  L->top--; /* remove source name */
  L->top--; /* remove scanner table */
  return 0; /* OK */
}

/* Type checker - WIP  */
static void typecheck_ast_node(struct ast_node *function, struct ast_node *node);

/* Type checker - WIP  */
static void typecheck_ast_list(struct ast_node *function, struct ast_node_list *list) {
  struct ast_node *node;
  FOR_EACH_PTR(list, node) { typecheck_ast_node(function, node); }
  END_FOR_EACH_PTR(node);
}

/* Type checker - WIP  */
static void typecheck_unaryop(struct ast_node *function, struct ast_node *node) {
  UnOpr op = node->unary_expr.unary_op;
  typecheck_ast_node(function, node->unary_expr.expr);
  ravitype_t subexpr_type = node->unary_expr.expr->common_expr.type.type_code;
  switch (op) {
    case OPR_MINUS:
      if (subexpr_type == RAVI_TNUMINT) {
        set_type(node->unary_expr.type, RAVI_TNUMINT);
      }
      else if (subexpr_type == RAVI_TNUMFLT) {
        set_type(node->unary_expr.type, RAVI_TNUMFLT);
      }
      break;
    case OPR_LEN:
      if (subexpr_type == RAVI_TARRAYINT || subexpr_type == RAVI_TARRAYFLT) {
        set_type(node->unary_expr.type, RAVI_TNUMINT);
      }
      else if (subexpr_type == RAVI_TTABLE) {
        // FIXME we need insert a TO_INT expression;
      }
      break;
    case OPR_TO_INTEGER:
      set_type(node->unary_expr.type, RAVI_TNUMINT);
      break;
    case OPR_TO_NUMBER:
      set_type(node->unary_expr.type, RAVI_TNUMFLT);
      break;
    case OPR_TO_CLOSURE:
      set_type(node->unary_expr.type, RAVI_TFUNCTION);
      break;
    case OPR_TO_STRING:
      set_type(node->unary_expr.type, RAVI_TSTRING);
      break;
    case OPR_TO_INTARRAY:
      set_type(node->unary_expr.type, RAVI_TARRAYINT);
      if (node->unary_expr.expr->type == AST_TABLE_EXPR) {
        set_type(node->unary_expr.expr->table_expr.type, RAVI_TARRAYINT);
      }
      break;
    case OPR_TO_NUMARRAY:
      set_type(node->unary_expr.type, RAVI_TARRAYFLT);
      if (node->unary_expr.expr->type == AST_TABLE_EXPR) {
        set_type(node->unary_expr.expr->table_expr.type, RAVI_TARRAYFLT);
      }
      break;
    case OPR_TO_TABLE:
      set_type(node->unary_expr.type, RAVI_TTABLE);
      break;
    case OPR_TO_TYPE:
      lua_assert(node->unary_expr.type.type_name != NULL);  // Should already be set by the parser
      set_typecode(node->unary_expr.type, RAVI_TUSERDATA);
      break;
    default:
      break;
  }
}
/* Type checker - WIP  */
static void typecheck_ast_node(struct ast_node *function, struct ast_node *node) {
  switch (node->type) {
    case AST_FUNCTION_EXPR: {
      typecheck_ast_list(function, node->function_expr.function_statement_list);
      break;
    }
    case AST_NONE:
      break;
    case AST_RETURN_STMT: {
      typecheck_ast_list(function, node->return_stmt.exprlist);
      break;
    }
    case AST_LOCAL_STMT: {
      break;
    }
    case AST_FUNCTION_STMT: {
      typecheck_ast_node(function, node->function_stmt.function_expr);
      break;
    }
    case AST_LABEL_STMT: {
      break;
    }
    case AST_GOTO_STMT: {
      break;
    }
    case AST_DO_STMT: {
      break;
    }
    case AST_EXPR_STMT: {
      break;
    }
    case AST_IF_STMT: {
      struct ast_node *test_then_block;
      FOR_EACH_PTR(node->if_stmt.if_condition_list, test_then_block) {}
      END_FOR_EACH_PTR(node);
      if (node->if_stmt.else_block) {
      }
      break;
    }
    case AST_WHILE_STMT: {
      break;
    }
    case AST_REPEAT_STMT: {
      break;
    }
    case AST_FORIN_STMT: {
      break;
    }
    case AST_FORNUM_STMT: {
      break;
    }
    case AST_SUFFIXED_EXPR: {
      if (node->suffixed_expr.suffix_list) {
      }
      break;
    }
    case AST_FUNCTION_CALL_EXPR: {
      if (node->function_call_expr.methodname) {
      }
      else {
      }
      break;
    }
    case AST_SYMBOL_EXPR: {
      break;
    }
    case AST_BINARY_EXPR: {
      break;
    }
    case AST_UNARY_EXPR: {
      typecheck_unaryop(function, node);
      break;
    }
    case AST_LITERAL_EXPR: {
      break;
    }
    case AST_FIELD_SELECTOR_EXPR: {
      break;
    }
    case AST_Y_INDEX_EXPR: {
      break;
    }
    case AST_INDEXED_ASSIGN_EXPR: {
      if (node->indexed_assign_expr.index_expr) {
      }
      break;
    }
    case AST_TABLE_EXPR: {
      break;
    }
    default:
      assert(0);
  }
}


/* Type checker - WIP  */
static void typecheck_function(struct ast_node *func) {
  typecheck_ast_list(func, func->function_expr.function_statement_list);
}

/* Type checker - WIP  */
static void do_typechecks(struct ast_container *container) {
  struct ast_node *main_function = container->main_function;
  typecheck_function(main_function);
}

/*
** ravi parser context
*/
struct parser_context { /* data to 'f_parser' */
  ZIO *z;
  Mbuffer buff; /* dynamic structure used by the scanner */
  Dyndata dyd;  /* dynamic structures used by the parser */
  const char *mode;
  const char *name;
};

static void checkmode(lua_State *L, const char *mode, const char *x) {
  if (mode && strchr(mode, x[0]) == NULL) {
    luaO_pushfstring(L, "attempt to load a %s chunk (mode is '%s')", x, mode);
    luaD_throw(L, LUA_ERRSYNTAX);
  }
}

static void ravi_parser_func(lua_State *L, void *ud) {
  struct parser_context *p = cast(struct parser_context *, ud);
  lua_lock(L);         // Workaroud for zgetc() which assumes lua_lock()
  int c = zgetc(p->z); /* read first character */
  lua_unlock(L);
  checkmode(L, p->mode, "text");
  parse_to_ast(L, p->z, &p->buff, p->name, c);
}

static int protected_ast_builder(lua_State *L, ZIO *z, const char *name, const char *mode) {
  struct parser_context p;
  int status;
  L->nny++; /* cannot yield during parsing */
  p.z = z;
  p.name = name;
  p.mode = mode;
  p.dyd.actvar.arr = NULL;
  p.dyd.actvar.size = 0;
  p.dyd.gt.arr = NULL;
  p.dyd.gt.size = 0;
  p.dyd.label.arr = NULL;
  p.dyd.label.size = 0;
  luaZ_initbuffer(L, &p.buff);
  status = luaD_pcall(L, ravi_parser_func, &p, savestack(L, L->top), L->errfunc);
  luaZ_freebuffer(L, &p.buff);
  luaM_freearray(L, p.dyd.actvar.arr, p.dyd.actvar.size);
  luaM_freearray(L, p.dyd.gt.arr, p.dyd.gt.size);
  luaM_freearray(L, p.dyd.label.arr, p.dyd.label.size);
  L->nny--;
  return status;
}

/*
**
** Builds an Abstract Syntax Tree (encapsulated in userdata type) from the given
** input buffer. This function returns 0 if all OK
* - On success a userdata object representing the tree,
*   else an error message will be pushed on to the stack
**
** This is part of the new experimental (wip) implementation of new
** parser and code generator
*/
static int build_ast_from_reader(lua_State *L, lua_Reader reader, void *data, const char *chunkname, const char *mode) {
  ZIO z;
  int status;
  if (!chunkname)
    chunkname = "?";
  luaZ_init(L, &z, reader, data);
  status = protected_ast_builder(L, &z, chunkname, mode);
  return status;
}

/*
** reserved slot, above all arguments, to hold a copy of the returned
** string to avoid it being collected while parsed. 'load' has four
** optional arguments (chunk, source name, mode, and environment).
*/
#define RESERVEDSLOT 5

/*
** Reader for generic 'load' function: 'lua_load' uses the
** stack for internal stuff, so the reader cannot change the
** stack top. Instead, it keeps its resulting string in a
** reserved slot inside the stack.
*/
static const char *generic_reader(lua_State *L, void *ud, size_t *size) {
  (void)(ud); /* not used */
  luaL_checkstack(L, 2, "too many nested functions");
  lua_pushvalue(L, 1); /* get function */
  lua_call(L, 0, 1);   /* call it */
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1); /* pop result */
    *size = 0;
    return NULL;
  }
  else if (!lua_isstring(L, -1))
    luaL_error(L, "reader function must return a string");
  lua_replace(L, RESERVEDSLOT); /* save string in reserved slot */
  return lua_tolstring(L, RESERVEDSLOT, size);
}

typedef struct string_buffer {
  const char *s;
  size_t size;
} String_buffer;

/* lua_Reader implementation */
static const char *string_reader(lua_State *L, void *ud, size_t *size) {
  String_buffer *ls = (String_buffer *)ud;
  (void)L; /* not used */
  if (ls->size == 0)
    return NULL;
  *size = ls->size;
  ls->size = 0;
  return ls->s;
}

/*
 * Builds an Abstract Syntax Tree (encapsulated in userdata type) from the given
 * input buffer. This function returns 0 if all OK
 * - On success a userdata object representing the tree,
 *   else an error message will be pushed on to the stack
 */
static int build_ast_from_buffer(lua_State *L, const char *buff, size_t size, const char *name, const char *mode) {
  String_buffer ls;
  ls.s = buff;
  ls.size = size;
  return build_ast_from_reader(L, string_reader, &ls, name, mode);
}

static int build_ast(lua_State *L) {
  int status;
  size_t l;
  const char *s = lua_tolstring(L, 1, &l);
  const char *mode = luaL_optstring(L, 3, "bt");
  // int env = (!lua_isnone(L, 4) ? 4 : 0);  /* 'env' index or 0 if no 'env' */
  if (s != NULL) { /* loading a string? */
    const char *chunkname = luaL_optstring(L, 2, s);
    status = build_ast_from_buffer(L, s, l, chunkname, mode);
  }
  else { /* loading from a reader function */
    const char *chunkname = luaL_optstring(L, 2, "=(load)");
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_settop(L, RESERVEDSLOT); /* create reserved slot */
    status = build_ast_from_reader(L, generic_reader, NULL, chunkname, mode);
  }
  if (status != 0) {
    lua_pushnil(L);
    lua_insert(L, -2); /* put before error message */
    return 2;          /* return nil plus error message */
  }
  return 1;
}

static const char *AST_type = "Ravi.AST";

#define test_Ravi_AST(L, idx) ((struct ast_container *)luaL_testudata(L, idx, AST_type))
#define check_Ravi_AST(L, idx) ((struct ast_container *)luaL_checkudata(L, idx, AST_type))

/* Converts the AST to a string representation */
static int ast_container_to_string(lua_State *L) {
  struct ast_container *container = check_Ravi_AST(L, 1);
  membuff_t mbuf;
  membuff_init(&mbuf, 1024);
  print_ast_node(&mbuf, container->main_function, 0);
  lua_pushstring(L, mbuf.buf);
  membuff_free(&mbuf);
  return 1;
}

static struct ast_container *new_ast_container(lua_State *L) {
  struct ast_container *container = (struct ast_container *)lua_newuserdata(L, sizeof(struct ast_container));
  dmrC_allocator_init(&container->ast_node_allocator, "ast nodes", sizeof(struct ast_node), sizeof(double), CHUNK);
  dmrC_allocator_init(&container->ptrlist_allocator, "ptrlists", sizeof(struct ptr_list), sizeof(double), CHUNK);
  dmrC_allocator_init(&container->block_scope_allocator, "block scopes", sizeof(struct block_scope), sizeof(double),
                      CHUNK);
  dmrC_allocator_init(&container->symbol_allocator, "symbols", sizeof(struct lua_symbol), sizeof(double), CHUNK);
  container->main_function = NULL;
  container->killed = false;
  luaL_getmetatable(L, AST_type);
  lua_setmetatable(L, -2);
  return container;
}

/* __gc function for AST */
static int collect_ast_container(lua_State *L) {
  struct ast_container *container = check_Ravi_AST(L, 1);
  if (!container->killed) {
    dmrC_allocator_destroy(&container->symbol_allocator);
    dmrC_allocator_destroy(&container->block_scope_allocator);
    dmrC_allocator_destroy(&container->ast_node_allocator);
    dmrC_allocator_destroy(&container->ptrlist_allocator);
    container->killed = true;
  }
  return 0;
}

static const luaL_Reg container_methods[] = {
    {"tostring", ast_container_to_string}, {"release", collect_ast_container}, {NULL, NULL}};

static const luaL_Reg astlib[] = {
    /* Entrypoint for new AST */
    {"parse", build_ast},
    {NULL, NULL}};

LUAMOD_API int raviopen_ast_library(lua_State *L) {
  luaL_newmetatable(L, AST_type);
  lua_pushcfunction(L, collect_ast_container);
  lua_setfield(L, -2, "__gc");
  lua_pushvalue(L, -1);           /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_setfuncs(L, container_methods, 0);
  lua_pop(L, 1);

  luaL_newlib(L, astlib);
  return 1;
}

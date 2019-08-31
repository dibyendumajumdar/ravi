#include <ravi_ast.h>
#include "ravi_ast.h"

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
static void typecheck_binaryop(struct ast_node *function, struct ast_node *node) {
  BinOpr op = node->binary_expr.binary_op;
  struct ast_node *e1 = node->binary_expr.expr_left;
  struct ast_node *e2 = node->binary_expr.expr_right;
  typecheck_ast_node(function, e1);
  typecheck_ast_node(function, e2);
  switch (op) {
    case OPR_ADD:
    case OPR_SUB:
    case OPR_MUL:
    case OPR_DIV:
      if (e1->common_expr.type.type_code == RAVI_TNUMFLT && e2->common_expr.type.type_code == RAVI_TNUMFLT)
        set_typecode(node->binary_expr.type, RAVI_TNUMFLT);
      else if (e1->common_expr.type.type_code == RAVI_TNUMFLT && e2->common_expr.type.type_code == RAVI_TNUMINT)
        set_typecode(node->binary_expr.type, RAVI_TNUMFLT);
      else if (e1->common_expr.type.type_code == RAVI_TNUMINT && e2->common_expr.type.type_code == RAVI_TNUMFLT)
        set_typecode(node->binary_expr.type, RAVI_TNUMFLT);
      else if (op != OPR_DIV && e1->common_expr.type.type_code == RAVI_TNUMINT &&
               e2->common_expr.type.type_code == RAVI_TNUMINT)
        set_typecode(node->binary_expr.type, RAVI_TNUMINT);
      else if (op == OPR_DIV && e1->common_expr.type.type_code == RAVI_TNUMINT &&
               e2->common_expr.type.type_code == RAVI_TNUMINT)
        set_typecode(node->binary_expr.type, RAVI_TNUMFLT);
      break;
    case OPR_IDIV:
      if (e1->common_expr.type.type_code == RAVI_TNUMINT && e2->common_expr.type.type_code == RAVI_TNUMINT)
        set_typecode(node->binary_expr.type, RAVI_TNUMINT);
      // FIXME missing cases
      break;
    case OPR_BAND:
    case OPR_BOR:
    case OPR_BXOR:
    case OPR_SHL:
    case OPR_SHR:
      if ((e1->common_expr.type.type_code == RAVI_TNUMFLT || e1->common_expr.type.type_code == RAVI_TNUMINT) &&
          (e2->common_expr.type.type_code == RAVI_TNUMFLT || e2->common_expr.type.type_code == RAVI_TNUMINT))
        set_typecode(node->binary_expr.type, RAVI_TNUMINT);
      break;
    case OPR_EQ:
    case OPR_NE:
    case OPR_GE:
    case OPR_GT:
    case OPR_LE:
    case OPR_LT:
      /* This case is not handled in default parser - why? */
      if ((e1->common_expr.type.type_code == RAVI_TNUMINT || e1->common_expr.type.type_code == RAVI_TNUMFLT ||
           e1->common_expr.type.type_code == RAVI_TBOOLEAN) &&
          (e2->common_expr.type.type_code == RAVI_TNUMFLT || e2->common_expr.type.type_code == RAVI_TNUMINT ||
           e2->common_expr.type.type_code == RAVI_TBOOLEAN))
        set_typecode(node->binary_expr.type, RAVI_TBOOLEAN);
      break;
    case OPR_POW:
      if ((e1->common_expr.type.type_code == RAVI_TNUMFLT || e1->common_expr.type.type_code == RAVI_TNUMINT) &&
          (e2->common_expr.type.type_code == RAVI_TNUMFLT || e2->common_expr.type.type_code == RAVI_TNUMINT))
        set_typecode(node->binary_expr.type, RAVI_TNUMFLT);
      break;
    case OPR_MOD:
      if (e1->common_expr.type.type_code == RAVI_TNUMINT && e2->common_expr.type.type_code == RAVI_TNUMINT)
        set_typecode(node->binary_expr.type, RAVI_TNUMINT);
      else if ((e1->common_expr.type.type_code == RAVI_TNUMINT && e2->common_expr.type.type_code == RAVI_TNUMFLT) ||
               (e1->common_expr.type.type_code == RAVI_TNUMFLT && e2->common_expr.type.type_code == RAVI_TNUMINT))
        set_typecode(node->binary_expr.type, RAVI_TNUMFLT);
      break;
    default:
      set_typecode(node->binary_expr.type, RAVI_TANY);
      break;
  }
}

static bool is_unindexable_type(struct var_type *type) {
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
static void typecheck_suffixedexpr(struct ast_node *function, struct ast_node *node) {
  typecheck_ast_node(function, node->suffixed_expr.primary_expr);
  struct ast_node *prev_node = node->suffixed_expr.primary_expr;
  struct ast_node *this_node;
  FOR_EACH_PTR(node->suffixed_expr.suffix_list, this_node) {
    typecheck_ast_node(function, this_node);
    if (this_node->type == AST_Y_INDEX_EXPR) {
      if (prev_node->common_expr.type.type_code == RAVI_TARRAYFLT) {
        if (this_node->index_expr.expr->common_expr.type.type_code == RAVI_TNUMINT) {
          set_typecode(this_node->index_expr.type, RAVI_TNUMFLT);
        }
        else {
          // FIXME Error
        }
      }
      else if (prev_node->common_expr.type.type_code == RAVI_TARRAYINT) {
        if (this_node->index_expr.expr->common_expr.type.type_code == RAVI_TNUMINT) {
          set_typecode(this_node->index_expr.type, RAVI_TNUMINT);
        }
        else {
          // FIXME Error
        }
      }
      else if (is_unindexable_type(&prev_node->common_expr.type)) {
        // FIXME Error
      }
    }
    prev_node = this_node;
  }
  END_FOR_EACH_PTR(node);
  copy_type(node->suffixed_expr.type, prev_node->common_expr.type);
}

static void typecheck_var_assignment(struct var_type *var_type, struct var_type *expr_type, const char *var_name) {
  if (var_type->type_code == RAVI_TANY)
    // Any value can be assigned to type ANY
    return;

  if (var_type->type_code == RAVI_TNUMINT || var_type->type_code == RAVI_TNUMFLT) {
    if (expr_type->type_code == RAVI_TNUMINT || expr_type->type_code == RAVI_TNUMFLT) {
      // Okay
      // TODO insert a typecast?
    }
    else {
      fprintf(stderr, "Assignment to local symbol %s is not type compatible\n", var_name);
    }
    return;
  }

  // all other types must strictly match
  if (!is_type_same(*var_type, *expr_type)) {  // We should probably check type convertability here
    fprintf(stderr, "Assignment to local symbol %s is not type compatible\n", var_name);
  }
}

static void typecheck_local_statement(struct ast_node *function, struct ast_node *node) {
  // The local vars should already be annotated
  // We need to typecheck the expressions to the right of =
  // Then we need to ensure that the assignments are valid
  // We can perhaps insert type assertions where we have a mismatch?

  typecheck_ast_list(function, node->local_stmt.expr_list);

  struct lua_symbol *var;
  struct ast_node *expr;
  PREPARE_PTR_LIST(node->local_stmt.var_list, var);
  PREPARE_PTR_LIST(node->local_stmt.expr_list, expr);

  for (;;) {
    if (!var || !expr)
      break;

    struct var_type *var_type = &var->value_type;
    struct var_type *expr_type = &expr->common_expr.type;
    const char *var_name = getstr(var->var.var_name);

    typecheck_var_assignment(var_type, expr_type, var_name);

    NEXT_PTR_LIST(var);
    NEXT_PTR_LIST(expr);
  }
}

static void typecheck_expr_statement(struct ast_node *function, struct ast_node *node) {
  if (node->expression_stmt.var_expr_list)
    typecheck_ast_list(function, node->expression_stmt.var_expr_list);
  typecheck_ast_list(function, node->expression_stmt.expr_list);

  if (!node->expression_stmt.var_expr_list)
    return;

  struct ast_node *var;
  struct ast_node *expr;
  PREPARE_PTR_LIST(node->expression_stmt.var_expr_list, var);
  PREPARE_PTR_LIST(node->local_stmt.expr_list, expr);

  for (;;) {
    if (!var || !expr)
      break;

    struct var_type *var_type = &var->common_expr.type;
    struct var_type *expr_type = &expr->common_expr.type;
    const char *var_name = "";  // FIXME how do we get this?

    typecheck_var_assignment(var_type, expr_type, var_name);

    NEXT_PTR_LIST(var);
    NEXT_PTR_LIST(expr);
  }
}

static void typecheck_for_statment(struct ast_node *function, struct ast_node *node) {
  typecheck_ast_list(function, node->for_stmt.expr_list);
  if (node->type != AST_FORNUM_STMT) {
    typecheck_ast_list(function, node->for_stmt.for_statement_list);
    return;
  }
  /* for num case, the index variable's type */
  struct ast_node *expr;
  ravitype_t index_type = RAVI_TNUMINT;
  int match_count = 0; /* number of expression matching index type */
  /* First expression decides between int / float types. We allow int or float in subsequent
   * expressions ... TODO may need changing.
   */
  FOR_EACH_PTR(node->for_stmt.expr_list, expr) {
    if (expr->common_expr.type.type_code == RAVI_TNUMFLT) {
      if (index_type != RAVI_TNUMFLT) {
        index_type = (match_count == 0) ? RAVI_TNUMFLT : RAVI_TANY;
      }
    }
    else if (expr->common_expr.type.type_code != RAVI_TNUMINT) {
      index_type = RAVI_TANY;
    }
    if (index_type == RAVI_TANY)
      break;
    match_count++;
  }
  END_FOR_EACH_PTR(expr);
  if (index_type == RAVI_TNUMINT || index_type == RAVI_TNUMFLT) {
    struct lua_symbol_list *symbols = node->for_stmt.symbols;
    struct lua_symbol *sym;
    /* actually there will be only index variable */
    FOR_EACH_PTR(symbols, sym) {
      if (sym->symbol_type == SYM_LOCAL) {
        set_typecode(sym->value_type, index_type);
      }
    }
    END_FOR_EACH_PTR(sym);
  }
  typecheck_ast_list(function, node->for_stmt.for_statement_list);
}

/* Type checker - WIP  */
static void typecheck_ast_node(struct ast_node *function, struct ast_node *node) {
  switch (node->type) {
    case AST_FUNCTION_EXPR: {
      typecheck_ast_list(function, node->function_expr.function_statement_list);
      break;
    }
    case AST_NONE: {
      break;
    }
    case AST_RETURN_STMT: {
      typecheck_ast_list(function, node->return_stmt.expr_list);
      break;
    }
    case AST_LOCAL_STMT: {
      typecheck_local_statement(function, node);
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
      typecheck_expr_statement(function, node);
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
      typecheck_for_statment(function, node);
      break;
    }
    case AST_FORNUM_STMT: {
      typecheck_for_statment(function, node);
      break;
    }
    case AST_SUFFIXED_EXPR: {
      typecheck_suffixedexpr(function, node);
      break;
    }
    case AST_FUNCTION_CALL_EXPR: {
      if (node->function_call_expr.method_name) {
      }
      else {
      }
      typecheck_ast_list(function, node->function_call_expr.arg_list);
      break;
    }
    case AST_SYMBOL_EXPR: {
      /* symbol type should have been set when symbol was created */
      copy_type(node->symbol_expr.type, node->symbol_expr.var->value_type);
      break;
    }
    case AST_BINARY_EXPR: {
      typecheck_binaryop(function, node);
      break;
    }
    case AST_UNARY_EXPR: {
      typecheck_unaryop(function, node);
      break;
    }
    case AST_LITERAL_EXPR: {
      /* type set during parsing */
      break;
    }
    case AST_FIELD_SELECTOR_EXPR: {
      typecheck_ast_node(function, node->index_expr.expr);
      break;
    }
    case AST_Y_INDEX_EXPR: {
      typecheck_ast_node(function, node->index_expr.expr);
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
void raviA_ast_typecheck(struct ast_container *container) {
  struct ast_node *main_function = container->main_function;
  typecheck_function(main_function);
}

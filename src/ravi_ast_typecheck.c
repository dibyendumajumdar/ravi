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
void raviA_ast_typecheck(struct ast_container *container) {
  struct ast_node *main_function = container->main_function;
  typecheck_function(main_function);
}

#include <ravi_ast.h>
#include "ravi_ast.h"

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


static void print_ast_node_list(membuff_t *buf, struct ast_node_list *list, int level, const char *delimiter) {
  struct ast_node *node;
  bool is_first = true;
  FOR_EACH_PTR(list, node) {
    if (is_first)
      is_first = false;
    else if (delimiter)
      printf_buf(buf, "%p%s\n", level, delimiter);
    raviA_print_ast_node(buf, node, level + 1);
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

void raviA_print_ast_node(membuff_t *buf, struct ast_node *node, int level) {
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
      print_ast_node_list(buf, node->return_stmt.expr_list, level + 1, ",");
      break;
    }
    case AST_LOCAL_STMT: {
      printf_buf(buf, "%plocal\n", level);
      printf_buf(buf, "%p%c\n", level, "[symbols]");
      print_symbol_list(buf, node->local_stmt.var_list, level + 1, ",");
      if (node->local_stmt.expr_list) {
        printf_buf(buf, "%p%c\n", level, "[expressions]");
        print_ast_node_list(buf, node->local_stmt.expr_list, level + 1, ",");
      }
      break;
    }
    case AST_FUNCTION_STMT: {
      raviA_print_ast_node(buf, node->function_stmt.name, level);
      if (node->function_stmt.selectors) {
        printf_buf(buf, "%p%c\n", level + 1, "[selectors]");
        print_ast_node_list(buf, node->function_stmt.selectors, level + 2, NULL);
      }
      if (node->function_stmt.method_name) {
        printf_buf(buf, "%p%c\n", level + 1, "[method name]");
        raviA_print_ast_node(buf, node->function_stmt.method_name, level + 2);
      }
      printf_buf(buf, "%p=\n", level + 1);
      raviA_print_ast_node(buf, node->function_stmt.function_expr, level + 2);
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
      print_ast_node_list(buf, node->expression_stmt.expr_list, level + 2, ",");
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
        raviA_print_ast_node(buf, test_then_block->test_then_block.condition, level + 1);
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
      raviA_print_ast_node(buf, node->while_or_repeat_stmt.condition, level + 1);
      printf_buf(buf, "%pdo\n", level);
      print_ast_node_list(buf, node->while_or_repeat_stmt.loop_statement_list, level + 1, NULL);
      printf_buf(buf, "%pend\n", level);
      break;
    }
    case AST_REPEAT_STMT: {
      printf_buf(buf, "%prepeat\n", level);
      print_ast_node_list(buf, node->while_or_repeat_stmt.loop_statement_list, level + 1, NULL);
      printf_buf(buf, "%puntil\n", level);
      raviA_print_ast_node(buf, node->while_or_repeat_stmt.condition, level + 1);
      printf_buf(buf, "%p%c\n", level, "[repeat end]");
      break;
    }
    case AST_FORIN_STMT: {
      printf_buf(buf, "%pfor\n", level);
      print_symbol_list(buf, node->for_stmt.symbols, level + 1, ",");
      printf_buf(buf, "%pin\n", level);
      print_ast_node_list(buf, node->for_stmt.expr_list, level + 1, ",");
      printf_buf(buf, "%pdo\n", level);
      print_statement_list(buf, node->for_stmt.for_statement_list, level + 1);
      printf_buf(buf, "%pend\n", level);
      break;
    }
    case AST_FORNUM_STMT: {
      printf_buf(buf, "%pfor\n", level);
      print_symbol_list(buf, node->for_stmt.symbols, level + 1, NULL);
      printf_buf(buf, "%p=\n", level);
      print_ast_node_list(buf, node->for_stmt.expr_list, level + 1, ",");
      printf_buf(buf, "%pdo\n", level);
      print_statement_list(buf, node->for_stmt.for_statement_list, level + 1);
      printf_buf(buf, "%pend\n", level);
      break;
    }
    case AST_SUFFIXED_EXPR: {
      printf_buf(buf, "%p%c %T\n", level, "[suffixed expr start]", &node->suffixed_expr.type);
      printf_buf(buf, "%p%c\n", level+1, "[primary start]");
      raviA_print_ast_node(buf, node->suffixed_expr.primary_expr, level + 2);
      printf_buf(buf, "%p%c\n", level+1, "[primary end]");
      if (node->suffixed_expr.suffix_list) {
        printf_buf(buf, "%p%c\n", level+1, "[suffix list start]");
        print_ast_node_list(buf, node->suffixed_expr.suffix_list, level + 2, NULL);
        printf_buf(buf, "%p%c\n", level+1, "[suffix list end]");
      }
      printf_buf(buf, "%p%c\n", level, "[suffixed expr end]");
      break;
    }
    case AST_FUNCTION_CALL_EXPR: {
      printf_buf(buf, "%p%c\n", level, "[function call start]");
      if (node->function_call_expr.method_name) {
        printf_buf(buf, "%p: %t (\n", level + 1, node->function_call_expr.method_name);
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
      printf_buf(buf, "%p%c %T\n", level, "[binary expr start]", &node->binary_expr.type);
      raviA_print_ast_node(buf, node->binary_expr.expr_left, level + 1);
      printf_buf(buf, "%p%s\n", level, get_binary_opr_str(node->binary_expr.binary_op));
      raviA_print_ast_node(buf, node->binary_expr.expr_right, level + 1);
      printf_buf(buf, "%p%c\n", level, "[binary expr end]");
      break;
    }
    case AST_UNARY_EXPR: {
      printf_buf(buf, "%p%c %T\n", level, "[unary expr start]", &node->unary_expr.type);
      printf_buf(buf, "%p%s\n", level, get_unary_opr_str(node->unary_expr.unary_op));
      raviA_print_ast_node(buf, node->unary_expr.expr, level + 1);
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
      printf_buf(buf, "%p%c %T\n", level, "[field selector start]", &node->index_expr.type);
      printf_buf(buf, "%p.\n", level + 1);
      raviA_print_ast_node(buf, node->index_expr.expr, level + 2);
      printf_buf(buf, "%p%c\n", level, "[field selector end]");
      break;
    }
    case AST_Y_INDEX_EXPR: {
      printf_buf(buf, "%p%c %T\n", level, "[Y index start]", &node->index_expr.type);
      printf_buf(buf, "%p[\n", level + 1);
      raviA_print_ast_node(buf, node->index_expr.expr, level + 2);
      printf_buf(buf, "%p]\n", level + 1);
      printf_buf(buf, "%p%c\n", level, "[Y index end]");
      break;
    }
    case AST_INDEXED_ASSIGN_EXPR: {
      printf_buf(buf, "%p%c\n", level, "[indexed assign start]");
      if (node->indexed_assign_expr.index_expr) {
        printf_buf(buf, "%p%c\n", level, "[index start]");
        raviA_print_ast_node(buf, node->indexed_assign_expr.index_expr, level + 1);
        printf_buf(buf, "%p%c\n", level, "[index end]");
      }
      printf_buf(buf, "%p%c\n", level, "[value start]");
      raviA_print_ast_node(buf, node->indexed_assign_expr.value_expr, level + 1);
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


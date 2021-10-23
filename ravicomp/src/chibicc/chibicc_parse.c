/*
Adapted from https://github.com/rui314/chibicc

MIT License

Copyright (c) 2019 Rui Ueyama

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


// This file contains a recursive descent parser for C.
//
// Most functions in this file are named after the symbols they are
// supposed to read from an input token list. For example, stmt() is
// responsible for reading a statement from a token list. The function
// then construct an AST node representing a statement.
//
// Each function conceptually returns two values, an AST node and
// remaining part of the input tokens. Since C doesn't support
// multiple return values, the remaining tokens are returned to the
// caller via a pointer argument.
//
// Input tokens are represented by a linked list. Unlike many recursive
// descent parsers, we don't have the notion of the "input token stream".
// Most parsing functions don't change the global state of the parser.
// So it is very easy to lookahead arbitrary number of tokens in this
// parser.

#include "chibicc.h"

// Variable attributes such as typedef or extern.
typedef struct {
  bool is_typedef;
  bool is_static;
  bool is_extern;
  bool is_inline;
  bool is_tls;
  int align;
} VarAttr;

// This struct represents a variable initializer. Since initializers
// can be nested (e.g. `int x[2][2] = {{1, 2}, {3, 4}}`), this struct
// is a tree data structure.
typedef struct Initializer Initializer;
struct Initializer {
  Initializer *next;
  C_Type *ty;
  C_Token *tok;
  bool is_flexible;

  // If it's not an aggregate type and has an initializer,
  // `expr` has an initialization expression.
  C_Node *expr;

  // If it's an initializer for an aggregate type (e.g. array or struct),
  // `children` has initializers for its children.
  Initializer **children;

  // Only one member can be initialized for a union.
  // `mem` is used to clarify which member is initialized.
  C_Member *mem;
};

// For local variable initializer.
typedef struct InitDesg InitDesg;
struct InitDesg {
  InitDesg *next;
  int idx;
  C_Member *member;
  C_Obj *var;
};


static bool is_typename(C_Parser *parser, C_Token *tok);
static C_Type *declspec(C_Parser *parser, C_Token **rest, C_Token *tok, VarAttr *attr);
static C_Type *typename(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Type *enum_specifier(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Type *typeof_specifier(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Type *type_suffix(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty);
static C_Type *declarator(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty);
static C_Node *declaration(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *basety, VarAttr *attr);
static void array_initializer2(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init, int i);
static void struct_initializer2(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init, C_Member *mem);
static void initializer2(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init);
static Initializer *initializer(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty, C_Type **new_ty);
static C_Node *lvar_initializer(C_Parser *parser, C_Token **rest, C_Token *tok, C_Obj *var);
static void gvar_initializer(C_Parser *parser, C_Token **rest, C_Token *tok, C_Obj *var);
static C_Node *compound_stmt(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *stmt(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *expr_stmt(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *expr(C_Parser *parser, C_Token **rest, C_Token *tok);
static int64_t eval(C_Parser *parser, C_Node *node);
static int64_t eval2(C_Parser *parser, C_Node *node, char ***label);
static int64_t eval_rval(C_Parser *parser, C_Node *node, char ***label);
static bool is_const_expr(C_Parser *parser, C_Node *node);
static C_Node *assign(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *logor(C_Parser *parser, C_Token **rest, C_Token *tok);
static double eval_double(C_Parser *parser, C_Node *node);
static C_Node *conditional(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *logand(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *bitor(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *bitxor(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *bitand(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *equality(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *relational(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *shift(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *add(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *new_add(C_Parser *parser, C_Node *lhs, C_Node *rhs, C_Token *tok);
static C_Node *new_sub(C_Parser *parser, C_Node *lhs, C_Node *rhs, C_Token *tok);
static C_Node *mul(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *cast(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Member *get_struct_member(C_Parser *parser, C_Type *ty, C_Token *tok);
static C_Type *struct_decl(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Type *union_decl(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *postfix(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *funcall(C_Parser *parser, C_Token **rest, C_Token *tok, C_Node *node);
static C_Node *unary(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Node *primary(C_Parser *parser, C_Token **rest, C_Token *tok);
static C_Token *parse_typedef(C_Parser *parser, C_Token *tok, C_Type *basety);
static bool is_function(C_Parser *parser, C_Token *tok);
static C_Token *function(C_Parser *parser, C_Token *tok, C_Type *basety, VarAttr *attr);
static C_Token *global_variable(C_Parser *parser, C_Token *tok, C_Type *basety, VarAttr *attr);

static int align_down(int n, int align) {
  return C_align_to(n - align + 1, align);
}

static char *str_dup(struct C_MemoryAllocator *allocator, const char *temp, size_t len) {
  char *p = (char *) allocator->calloc(allocator->arena, 1, len+1);
  memcpy(p, temp, len);
  p[len] = 0;
  return p;
}

C_Scope *C_global_scope(C_Parser *parser) {
  C_Scope *sc = parser->memory_allocator->calloc(parser->memory_allocator->arena, 1, sizeof(C_Scope));
  sc->vars.allocator = parser->memory_allocator;
  sc->tags.allocator = parser->memory_allocator;
  return sc;
}

static void enter_scope(C_Parser *parser) {
  C_Scope *sc = parser->memory_allocator->calloc(parser->memory_allocator->arena, 1, sizeof(C_Scope));
  sc->vars.allocator = parser->memory_allocator;
  sc->tags.allocator = parser->memory_allocator;
  sc->next = parser->scope;
  parser->scope = sc;
}

static void leave_scope(C_Parser *parser) {
  parser->scope = parser->scope->next;
}

// Find a variable by name.
static C_VarScope *find_var(C_Parser *parser, C_Token *tok) {
  for (C_Scope *sc = parser->scope; sc; sc = sc->next) {
    C_VarScope *sc2 = hashmap_get2(&sc->vars, tok->loc, tok->len);
    if (sc2)
      return sc2;
  }
  return NULL;
}

static C_Type *find_tag(C_Parser *parser, C_Token *tok) {
  for (C_Scope *sc = parser->scope; sc; sc = sc->next) {
    C_Type *ty = hashmap_get2(&sc->tags, tok->loc, tok->len);
    if (ty)
      return ty;
  }
  return NULL;
}

static C_Node *new_node(C_Parser *parser, C_NodeKind kind, C_Token *tok) {
  C_Node *node = parser->memory_allocator->calloc(parser->memory_allocator->arena,1, sizeof(C_Node));
  node->kind = kind;
  node->tok = tok;
  return node;
}

static C_Node *new_binary(C_Parser *parser, C_NodeKind kind, C_Node *lhs, C_Node *rhs, C_Token *tok) {
  C_Node *node = new_node(parser, kind, tok);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static C_Node *new_unary(C_Parser *parser, C_NodeKind kind, C_Node *expr, C_Token *tok) {
  C_Node *node = new_node(parser, kind, tok);
  node->lhs = expr;
  return node;
}

static C_Node *new_num(C_Parser *parser, int64_t val, C_Token *tok) {
  C_Node *node = new_node(parser, ND_NUM, tok);
  node->val = val;
  return node;
}

static C_Node *new_long(C_Parser *parser, int64_t val, C_Token *tok) {
  C_Node *node = new_node(parser, ND_NUM, tok);
  node->val = val;
  node->ty = C_ty_long;
  return node;
}

static C_Node *new_ulong(C_Parser *parser, long val, C_Token *tok) {
  C_Node *node = new_node(parser, ND_NUM, tok);
  node->val = val;
  node->ty = C_ty_ulong;
  return node;
}

static C_Node *new_var_node(C_Parser *parser, C_Obj *var, C_Token *tok) {
  C_Node *node = new_node(parser, ND_VAR, tok);
  node->var = var;
  return node;
}

static C_Node *new_vla_ptr(C_Parser *parser, C_Obj *var, C_Token *tok) {
  C_Node *node = new_node(parser, ND_VLA_PTR, tok);
  node->var = var;
  return node;
}

C_Node *C_new_cast(C_Parser *parser, C_Node *expr, C_Type *ty) {
  C_add_type(parser, expr);

  C_Node *node = parser->memory_allocator->calloc(parser->memory_allocator->arena,1, sizeof(C_Node));
  node->kind = ND_CAST;
  node->tok = expr->tok;
  node->lhs = expr;
  node->ty = C_copy_type(parser, ty);
  return node;
}

static C_VarScope *push_scope(C_Parser *parser, char *name) {
  C_VarScope *sc = parser->memory_allocator->calloc(parser->memory_allocator->arena,1, sizeof(C_VarScope));
  hashmap_put(&parser->scope->vars, name, sc);
  return sc;
}

static Initializer *new_initializer(C_Parser *parser, C_Type *ty, bool is_flexible) {
  Initializer *init = parser->memory_allocator->calloc(parser->memory_allocator->arena,1, sizeof(Initializer));
  init->ty = ty;

  if (ty->kind == TY_ARRAY) {
    if (is_flexible && ty->size < 0) {
      init->is_flexible = true;
      return init;
    }

    init->children = parser->memory_allocator->calloc(parser->memory_allocator->arena, ty->array_len, sizeof(Initializer *));
    for (int i = 0; i < ty->array_len; i++)
      init->children[i] = new_initializer(parser, ty->base, false);
    return init;
  }

  if (ty->kind == TY_STRUCT || ty->kind == TY_UNION) {
    // Count the number of struct members.
    int len = 0;
    for (C_Member *mem = ty->members; mem; mem = mem->next)
      len++;

    init->children = parser->memory_allocator->calloc(parser->memory_allocator->arena, len, sizeof(Initializer *));

    for (C_Member *mem = ty->members; mem; mem = mem->next) {
      if (is_flexible && ty->is_flexible && !mem->next) {
        Initializer *child = parser->memory_allocator->calloc(parser->memory_allocator->arena, 1, sizeof(Initializer));
        child->ty = mem->ty;
        child->is_flexible = true;
        init->children[mem->idx] = child;
      } else {
        init->children[mem->idx] = new_initializer(parser, mem->ty, false);
      }
    }
    return init;
  }

  return init;
}

static C_Obj *new_var(C_Parser *parser, char *name, C_Type *ty) {
  C_Obj *var = parser->memory_allocator->calloc(parser->memory_allocator->arena, 1, sizeof(C_Obj));
  var->name = name;
  var->ty = ty;
  var->align = ty->align;
  push_scope(parser, name)->var = var;
  return var;
}

static C_Obj *new_lvar(C_Parser *parser, char *name, C_Type *ty) {
  C_Obj *var = new_var(parser, name, ty);
  var->is_local = true;
  var->next = parser->locals;
  parser->locals = var;
  return var;
}

static C_Obj *new_gvar(C_Parser *parser, char *name, C_Type *ty) {
  C_Obj *var = new_var(parser, name, ty);
  var->next = parser->globals;
  var->is_static = true;
  var->is_definition = true;
  parser->globals = var;
  return var;
}

static char *new_unique_name(mspace arena) {
  static int id = 0;
  char temp[64];

  snprintf(temp, sizeof temp, ".L..%d", id++);
  return str_dup(arena, temp, strlen(temp));
}

static C_Obj *new_anon_gvar(C_Parser *parser, C_Type *ty) {
  return new_gvar(parser, new_unique_name(parser->memory_allocator), ty);
}

static C_Obj *new_string_literal(C_Parser *parser, char *p, C_Type *ty) {
  C_Obj *var = new_anon_gvar(parser, ty);
  var->init_data = p;
  return var;
}

static char *get_ident(C_Parser *parser, C_Token *tok) {
  if (tok->kind != TK_IDENT)
    C_error_tok(parser, tok, "expected an identifier");
  return str_dup(parser->memory_allocator, tok->loc, tok->len);
}

static C_Type *find_typedef(C_Parser *parser, C_Token *tok) {
  if (tok->kind == TK_IDENT) {
    C_VarScope *sc = find_var(parser, tok);
    if (sc)
      return sc->type_def;
  }
  return NULL;
}

static void push_tag_scope(C_Parser *parser, C_Token *tok, C_Type *ty) {
  hashmap_put2(&parser->scope->tags, tok->loc, tok->len, ty);
}

// declspec = ("void" | "_Bool" | "char" | "short" | "int" | "long"
//             | "typedef" | "static" | "extern" | "inline"
//             | "_Thread_local" | "__thread"
//             | "signed" | "unsigned"
//             | struct-decl | union-decl | typedef-name
//             | enum-specifier | typeof-specifier
//             | "const" | "volatile" | "auto" | "register" | "restrict"
//             | "__restrict" | "__restrict__" | "_Noreturn")+
//
// The order of typenames in a type-specifier doesn't matter. For
// example, `int long static` means the same as `static long int`.
// That can also be written as `static long` because you can omit
// `int` if `long` or `short` are specified. However, something like
// `char int` is not a valid type specifier. We have to accept only a
// limited combinations of the typenames.
//
// In this function, we count the number of occurrences of each typename
// while keeping the "current" type object that the typenames up
// until that point represent. When we reach a non-typename token,
// we returns the current type object.
static C_Type *declspec(C_Parser *parser, C_Token **rest, C_Token *tok, VarAttr *attr) {
  // We use a single integer as counters for all typenames.
  // For example, bits 0 and 1 represents how many times we saw the
  // keyword "void" so far. With this, we can use a switch statement
  // as you can see below.
  enum {
    VOID     = 1 << 0,
    BOOL     = 1 << 2,
    CHAR     = 1 << 4,
    SHORT    = 1 << 6,
    INT      = 1 << 8,
    LONG     = 1 << 10,
    FLOAT    = 1 << 12,
    DOUBLE   = 1 << 14,
    OTHER    = 1 << 16,
    SIGNED   = 1 << 17,
    UNSIGNED = 1 << 18,
  };

  C_Type *ty = C_ty_int;
  int counter = 0;
  bool is_atomic = false;

  while (is_typename(parser, tok)) {
    // Handle storage class specifiers.
    if (C_equal(tok, "typedef") || C_equal(tok, "static") || C_equal(tok, "extern") || C_equal(tok, "inline") ||
        C_equal(tok, "_Thread_local") || C_equal(tok, "__thread")) {
      if (!attr)
        C_error_tok(parser, tok, "storage class specifier is not allowed in this context");

      if (C_equal(tok, "typedef"))
        attr->is_typedef = true;
      else if (C_equal(tok, "static"))
        attr->is_static = true;
      else if (C_equal(tok, "extern"))
        attr->is_extern = true;
      else if (C_equal(tok, "inline"))
        attr->is_inline = true;
      else
        attr->is_tls = true;

      if (attr->is_typedef &&
          attr->is_static + attr->is_extern + attr->is_inline + attr->is_tls > 1)
        C_error_tok(parser, tok,
        "typedef may not be used together with static,"
        " extern, inline, __thread or _Thread_local");
      tok = tok->next;
      continue;
    }

    // These keywords are recognized but ignored.
    if (C_consume(&tok, tok, "const") || C_consume(&tok, tok, "volatile") || C_consume(&tok, tok, "auto") ||
        C_consume(&tok, tok, "register") || C_consume(&tok, tok, "restrict") || C_consume(&tok, tok, "__restrict") ||
        C_consume(&tok, tok, "__restrict__") || C_consume(&tok, tok, "_Noreturn"))
      continue;

    if (C_equal(tok, "_Atomic")) {
      tok = tok->next;
      if (C_equal(tok, "(")) {
        ty = typename(parser, &tok, tok->next);
        tok = C_skip(parser, tok, ")");
      }
      is_atomic = true;
      continue;
    }

    if (C_equal(tok, "_Alignas")) {
      if (!attr)
        C_error_tok(parser, tok, "_Alignas is not allowed in this context");
      tok = C_skip(parser, tok->next, "(");

      if (is_typename(parser, tok))
        attr->align = typename(parser, &tok, tok)->align;
      else
        attr->align = C_const_expr(parser, &tok, tok);
      tok = C_skip(parser, tok, ")");
      continue;
    }

    // Handle user-defined types.
    C_Type *ty2 = find_typedef(parser, tok);
    if (C_equal(tok, "struct") || C_equal(tok, "union") || C_equal(tok, "enum") || C_equal(tok, "typeof") || ty2) {
      if (counter)
        break;

      if (C_equal(tok, "struct")) {
        ty = struct_decl(parser, &tok, tok->next);
      } else if (C_equal(tok, "union")) {
        ty = union_decl(parser, &tok, tok->next);
      } else if (C_equal(tok, "enum")) {
        ty = enum_specifier(parser, &tok, tok->next);
      } else if (C_equal(tok, "typeof")) {
        ty = typeof_specifier(parser, &tok, tok->next);
      } else {
        ty = ty2;
        tok = tok->next;
      }

      counter += OTHER;
      continue;
    }

    // Handle built-in types.
    if (C_equal(tok, "void"))
      counter += VOID;
    else if (C_equal(tok, "_Bool"))
      counter += BOOL;
    else if (C_equal(tok, "char"))
      counter += CHAR;
    else if (C_equal(tok, "short"))
      counter += SHORT;
    else if (C_equal(tok, "int"))
      counter += INT;
    else if (C_equal(tok, "long"))
      counter += LONG;
    else if (C_equal(tok, "float"))
      counter += FLOAT;
    else if (C_equal(tok, "double"))
      counter += DOUBLE;
    else if (C_equal(tok, "signed"))
      counter |= SIGNED;
    else if (C_equal(tok, "unsigned"))
      counter |= UNSIGNED;
    else
      unreachable(parser);

    switch (counter) {
    case VOID:
      ty = C_ty_void;
      break;
    case BOOL:
      ty = C_ty_bool;
      break;
    case CHAR:
    case SIGNED + CHAR:
      ty = C_ty_char;
      break;
    case UNSIGNED + CHAR:
      ty = C_ty_uchar;
      break;
    case SHORT:
    case SHORT + INT:
    case SIGNED + SHORT:
    case SIGNED + SHORT + INT:
      ty = C_ty_short;
      break;
    case UNSIGNED + SHORT:
    case UNSIGNED + SHORT + INT:
      ty = C_ty_ushort;
      break;
    case INT:
    case SIGNED:
    case SIGNED + INT:
      ty = C_ty_int;
      break;
    case UNSIGNED:
    case UNSIGNED + INT:
      ty = C_ty_uint;
      break;
    case LONG:
    case LONG + INT:
    case LONG + LONG:
    case LONG + LONG + INT:
    case SIGNED + LONG:
    case SIGNED + LONG + INT:
    case SIGNED + LONG + LONG:
    case SIGNED + LONG + LONG + INT:
      ty = C_ty_long;
      break;
    case UNSIGNED + LONG:
    case UNSIGNED + LONG + INT:
    case UNSIGNED + LONG + LONG:
    case UNSIGNED + LONG + LONG + INT:
      ty = C_ty_ulong;
      break;
    case FLOAT:
      ty = C_ty_float;
      break;
    case DOUBLE:
      ty = C_ty_double;
      break;
    case LONG + DOUBLE:
      ty = C_ty_ldouble;
      break;
    default:
      C_error_tok(parser, tok, "invalid type");
    }

    tok = tok->next;
  }

  if (is_atomic) {
    ty = C_copy_type(parser, ty);
    ty->is_atomic = true;
  }

  *rest = tok;
  return ty;
}

// func-params = ("void" | param ("," param)* ("," "...")?)? ")"
// param       = declspec declarator
static C_Type *func_params(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty) {
  if (C_equal(tok, "void") && C_equal(tok->next, ")")) {
    *rest = tok->next->next;
    return C_func_type(parser, ty);
  }

  C_Type head = {0};
  C_Type *cur = &head;
  bool is_variadic = false;

  while (!C_equal(tok, ")")) {
    if (cur != &head)
      tok = C_skip(parser, tok, ",");

    if (C_equal(tok, "...")) {
      is_variadic = true;
      tok = tok->next;
      C_skip(parser, tok, ")");
      break;
    }

    C_Type *ty2 = declspec(parser, &tok, tok, NULL);
    ty2 = declarator(parser, &tok, tok, ty2);

    C_Token *name = ty2->name;

    if (ty2->kind == TY_ARRAY) {
      // "array of T" is converted to "pointer to T" only in the parameter
      // context. For example, *argv[] is converted to **argv by this.
      ty2 = C_pointer_to(parser, ty2->base);
      ty2->name = name;
    } else if (ty2->kind == TY_FUNC) {
      // Likewise, a function is converted to a pointer to a function
      // only in the parameter context.
      ty2 = C_pointer_to(parser, ty2);
      ty2->name = name;
    }

    cur = cur->next = C_copy_type(parser, ty2);
  }

  if (cur == &head)
    is_variadic = true;

  ty = C_func_type(parser, ty);
  ty->params = head.next;
  ty->is_variadic = is_variadic;
  *rest = tok->next;
  return ty;
}

// array-dimensions = ("static" | "restrict")* const-expr? "]" type-suffix
static C_Type *array_dimensions(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty) {
  while (C_equal(tok, "static") || C_equal(tok, "restrict"))
    tok = tok->next;

  if (C_equal(tok, "]")) {
    ty = type_suffix(parser, rest, tok->next, ty);
    return C_array_of(parser, ty, -1);
  }

  C_Node *expr = conditional(parser, &tok, tok);
  tok = C_skip(parser, tok, "]");
  ty = type_suffix(parser, rest, tok, ty);

  if (ty->kind == TY_VLA || !is_const_expr(parser, expr))
    return C_vla_of(parser, ty, expr);
  return C_array_of(parser, ty, eval(parser, expr));
}

// type-suffix = "(" func-params
//             | "[" array-dimensions
//             | ε
static C_Type *type_suffix(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty) {
  if (C_equal(tok, "("))
    return func_params(parser, rest, tok->next, ty);

  if (C_equal(tok, "["))
    return array_dimensions(parser, rest, tok->next, ty);

  *rest = tok;
  return ty;
}

// pointers = ("*" ("const" | "volatile" | "restrict")*)*
static C_Type *pointers(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty) {
  while (C_consume(&tok, tok, "*")) {
    ty = C_pointer_to(parser, ty);
    while (C_equal(tok, "const") || C_equal(tok, "volatile") || C_equal(tok, "restrict") || C_equal(tok, "__restrict") ||
      C_equal(tok, "__restrict__"))
      tok = tok->next;
  }
  *rest = tok;
  return ty;
}

// declarator = pointers ("(" ident ")" | "(" declarator ")" | ident) type-suffix
static C_Type *declarator(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty) {
  ty = pointers(parser, &tok, tok, ty);

  if (C_equal(tok, "(")) {
    C_Token *start = tok;
    C_Type dummy = {0};
    declarator(parser, &tok, start->next, &dummy);
    tok = C_skip(parser, tok, ")");
    ty = type_suffix(parser, rest, tok, ty);
    return declarator(parser, &tok, start->next, ty);
  }

  C_Token *name = NULL;
  C_Token *name_pos = tok;

  if (tok->kind == TK_IDENT) {
    name = tok;
    tok = tok->next;
  }

  ty = type_suffix(parser, rest, tok, ty);
  ty->name = name;
  ty->name_pos = name_pos;
  return ty;
}

// abstract-declarator = pointers ("(" abstract-declarator ")")? type-suffix
static C_Type *abstract_declarator(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty) {
  ty = pointers(parser, &tok, tok, ty);

  if (C_equal(tok, "(")) {
    C_Token *start = tok;
    C_Type dummy = {0};
    abstract_declarator(parser, &tok, start->next, &dummy);
    tok = C_skip(parser, tok, ")");
    ty = type_suffix(parser, rest, tok, ty);
    return abstract_declarator(parser, &tok, start->next, ty);
  }

  return type_suffix(parser, rest, tok, ty);
}

// type-name = declspec abstract-declarator
static C_Type *typename(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Type *ty = declspec(parser, &tok, tok, NULL);
  return abstract_declarator(parser, rest, tok, ty);
}

static bool is_end(C_Token *tok) {
  return C_equal(tok, "}") || (C_equal(tok, ",") && C_equal(tok->next, "}"));
}

static bool consume_end(C_Token **rest, C_Token *tok) {
  if (C_equal(tok, "}")) {
    *rest = tok->next;
    return true;
  }

  if (C_equal(tok, ",") && C_equal(tok->next, "}")) {
    *rest = tok->next->next;
    return true;
  }

  return false;
}

// enum-specifier = ident? "{" enum-list? "}"
//                | ident ("{" enum-list? "}")?
//
// enum-list      = ident ("=" num)? ("," ident ("=" num)?)* ","?
static C_Type *enum_specifier(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Type *ty = C_enum_type(parser);

  // Read a struct tag.
  C_Token *tag = NULL;
  if (tok->kind == TK_IDENT) {
    tag = tok;
    tok = tok->next;
  }

  if (tag && !C_equal(tok, "{")) {
    C_Type *ty = find_tag(parser, tag);
    if (!ty)
      C_error_tok(parser, tag, "unknown enum type");
    if (ty->kind != TY_ENUM)
      C_error_tok(parser, tag, "not an enum tag");
    *rest = tok;
    return ty;
  }

  tok = C_skip(parser, tok, "{");

  // Read an enum-list.
  int i = 0;
  int val = 0;
  while (!consume_end(rest, tok)) {
    if (i++ > 0)
      tok = C_skip(parser, tok, ",");

    char *name = get_ident(parser, tok);
    tok = tok->next;

    if (C_equal(tok, "="))
      val = C_const_expr(parser, &tok, tok->next);

    C_VarScope *sc = push_scope(parser, name);
    sc->enum_ty = ty;
    sc->enum_val = val++;
  }

  if (tag)
    push_tag_scope(parser, tag, ty);
  return ty;
}

// typeof-specifier = "(" (expr | typename) ")"
static C_Type *typeof_specifier(C_Parser *parser, C_Token **rest, C_Token *tok) {
  tok = C_skip(parser, tok, "(");

  C_Type *ty;
  if (is_typename(parser, tok)) {
    ty = typename(parser, &tok, tok);
  } else {
    C_Node *node = expr(parser, &tok, tok);
    C_add_type(parser, node);
    ty = node->ty;
  }
  *rest = C_skip(parser, tok, ")");
  return ty;
}

// Generate code for computing a VLA size.
static C_Node *compute_vla_size(C_Parser *parser, C_Type *ty, C_Token *tok) {
  C_Node *node = new_node(parser, ND_NULL_EXPR, tok);
  if (ty->base)
    node = new_binary(parser, ND_COMMA, node, compute_vla_size(parser, ty->base, tok), tok);

  if (ty->kind != TY_VLA)
    return node;

  C_Node *base_sz;
  if (ty->base->kind == TY_VLA)
    base_sz = new_var_node(parser, ty->base->vla_size, tok);
  else
    base_sz = new_num(parser, ty->base->size, tok);

  ty->vla_size = new_lvar(parser, "", C_ty_ulong);
  C_Node *expr = new_binary(parser, ND_ASSIGN, new_var_node(parser, ty->vla_size, tok),
                          new_binary(parser, ND_MUL, ty->vla_len, base_sz, tok),
                          tok);
  return new_binary(parser, ND_COMMA, node, expr, tok);
}

static C_Node *new_alloca(C_Parser *parser, C_Node *sz) {
  C_Node *node = new_unary(parser, ND_FUNCALL, new_var_node(parser, parser->builtin_alloca, sz->tok), sz->tok);
  node->func_ty = parser->builtin_alloca->ty;
  node->ty = parser->builtin_alloca->ty->return_ty;
  node->args = sz;
  C_add_type(parser, sz);
  return node;
}

// declaration = declspec (declarator ("=" expr)? ("," declarator ("=" expr)?)*)? ";"
static C_Node *declaration(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *basety, VarAttr *attr) {
  C_Node head = {0};
  C_Node *cur = &head;
  int i = 0;

  while (!C_equal(tok, ";")) {
    if (i++ > 0)
      tok = C_skip(parser, tok, ",");

    C_Type *ty = declarator(parser, &tok, tok, basety);
    if (ty->kind == TY_VOID)
      C_error_tok(parser, tok, "variable declared void");
    if (!ty->name)
      C_error_tok(parser, ty->name_pos, "variable name omitted");

    if (attr && attr->is_static) {
      // static local variable
      C_Obj *var = new_anon_gvar(parser, ty);
      push_scope(parser, get_ident(parser, ty->name))->var = var;
      if (C_equal(tok, "="))
        gvar_initializer(parser, &tok, tok->next, var);
      continue;
    }

    // Generate code for computing a VLA size. We need to do this
    // even if ty is not VLA because ty may be a pointer to VLA
    // (e.g. int (*foo)[n][m] where n and m are variables.)
    cur = cur->next = new_unary(parser, ND_EXPR_STMT, compute_vla_size(parser, ty, tok), tok);

    if (ty->kind == TY_VLA) {
      if (C_equal(tok, "="))
        C_error_tok(parser, tok, "variable-sized object may not be initialized");

      // Variable length arrays (VLAs) are translated to alloca() calls.
      // For example, `int x[n+2]` is translated to `tmp = n + 2,
      // x = alloca(tmp)`.
      C_Obj *var = new_lvar(parser, get_ident(parser, ty->name), ty);
      C_Token *tok = ty->name;
      C_Node *expr = new_binary(parser, ND_ASSIGN, new_vla_ptr(parser, var, tok),
                              new_alloca(parser, new_var_node(parser, ty->vla_size, tok)),
                              tok);

      cur = cur->next = new_unary(parser, ND_EXPR_STMT, expr, tok);
      continue;
    }

    C_Obj *var = new_lvar(parser, get_ident(parser, ty->name), ty);
    if (attr && attr->align)
      var->align = attr->align;

    if (C_equal(tok, "=")) {
      C_Node *expr = lvar_initializer(parser, &tok, tok->next, var);
      cur = cur->next = new_unary(parser, ND_EXPR_STMT, expr, tok);
    }

    if (var->ty->size < 0)
      C_error_tok(parser, ty->name, "variable has incomplete type");
    if (var->ty->kind == TY_VOID)
      C_error_tok(parser, ty->name, "variable declared void");
  }

  C_Node *node = new_node(parser, ND_BLOCK, tok);
  node->body = head.next;
  *rest = tok->next;
  return node;
}

static C_Token *skip_excess_element(C_Parser *parser, C_Token *tok) {
  if (C_equal(tok, "{")) {
    tok = skip_excess_element(parser, tok->next);
    return C_skip(parser, tok, "}");
  }

  assign(parser, &tok, tok);
  return tok;
}

// string-initializer = string-literal
static void string_initializer(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init) {
  if (init->is_flexible)
    *init = *new_initializer(parser, C_array_of(parser, init->ty->base, tok->ty->array_len), false);

  int len = MIN(init->ty->array_len, tok->ty->array_len);

  switch (init->ty->base->size) {
  case 1: {
    char *str = tok->str;
    for (int i = 0; i < len; i++)
      init->children[i]->expr = new_num(parser, str[i], tok);
    break;
  }
  case 2: {
    uint16_t *str = (uint16_t *)tok->str;
    for (int i = 0; i < len; i++)
      init->children[i]->expr = new_num(parser, str[i], tok);
    break;
  }
  case 4: {
    uint32_t *str = (uint32_t *)tok->str;
    for (int i = 0; i < len; i++)
      init->children[i]->expr = new_num(parser, str[i], tok);
    break;
  }
  default:
    unreachable(parser);
  }

  *rest = tok->next;
}

// array-designator = "[" const-expr "]"
//
// C99 added the designated initializer to the language, which allows
// programmers to move the "cursor" of an initializer to any element.
// The syntax looks like this:
//
//   int x[10] = { 1, 2, [5]=3, 4, 5, 6, 7 };
//
// `[5]` moves the cursor to the 5th element, so the 5th element of x
// is set to 3. Initialization then continues forward in order, so
// 6th, 7th, 8th and 9th elements are initialized with 4, 5, 6 and 7,
// respectively. Unspecified elements (in this case, 3rd and 4th
// elements) are initialized with zero.
//
// Nesting is allowed, so the following initializer is valid:
//
//   int x[5][10] = { [5][8]=1, 2, 3 };
//
// It sets x[5][8], x[5][9] and x[6][0] to 1, 2 and 3, respectively.
//
// Use `.fieldname` to move the cursor for a struct initializer. E.g.
//
//   struct { int a, b, c; } x = { .c=5 };
//
// The above initializer sets x.c to 5.
static void array_designator(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty, int *begin, int *end) {
  *begin = C_const_expr(parser, &tok, tok->next);
  if (*begin >= ty->array_len)
    C_error_tok(parser, tok, "array designator index exceeds array bounds");

  if (C_equal(tok, "...")) {
    *end = C_const_expr(parser, &tok, tok->next);
    if (*end >= ty->array_len)
      C_error_tok(parser, tok, "array designator index exceeds array bounds");
    if (*end < *begin)
      C_error_tok(parser, tok, "array designator range [%d, %d] is empty", *begin, *end);
  } else {
    *end = *begin;
  }

  *rest = C_skip(parser, tok, "]");
}

// struct-designator = "." ident
static C_Member *struct_designator(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty) {
  C_Token *start = tok;
  tok = C_skip(parser, tok, ".");
  if (tok->kind != TK_IDENT)
    C_error_tok(parser, tok, "expected a field designator");

  for (C_Member *mem = ty->members; mem; mem = mem->next) {
    // Anonymous struct member
    if (mem->ty->kind == TY_STRUCT && !mem->name) {
      if (get_struct_member(parser, mem->ty, tok)) {
        *rest = start;
        return mem;
      }
      continue;
    }

    // Regular struct member
    if (mem->name->len == tok->len && !strncmp(mem->name->loc, tok->loc, tok->len)) {
      *rest = tok->next;
      return mem;
    }
  }

  C_error_tok(parser, tok, "struct has no such member");
}

// designation = ("[" const-expr "]" | "." ident)* "="? initializer
static void designation(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init) {
  if (C_equal(tok, "[")) {
    if (init->ty->kind != TY_ARRAY)
      C_error_tok(parser, tok, "array index in non-array initializer");

    int begin, end;
    array_designator(parser, &tok, tok, init->ty, &begin, &end);

    C_Token *tok2;
    for (int i = begin; i <= end; i++)
      designation(parser, &tok2, tok, init->children[i]);
    array_initializer2(parser, rest, tok2, init, begin + 1);
    return;
  }

  if (C_equal(tok, ".") && init->ty->kind == TY_STRUCT) {
    C_Member *mem = struct_designator(parser, &tok, tok, init->ty);
    designation(parser, &tok, tok, init->children[mem->idx]);
    init->expr = NULL;
    struct_initializer2(parser, rest, tok, init, mem->next);
    return;
  }

  if (C_equal(tok, ".") && init->ty->kind == TY_UNION) {
    C_Member *mem = struct_designator(parser, &tok, tok, init->ty);
    init->mem = mem;
    designation(parser, rest, tok, init->children[mem->idx]);
    return;
  }

  if (C_equal(tok, "."))
    C_error_tok(parser, tok, "field name not in struct or union initializer");

  if (C_equal(tok, "="))
    tok = tok->next;
  initializer2(parser, rest, tok, init);
}

// An array length can be omitted if an array has an initializer
// (e.g. `int x[] = {1,2,3}`). If it's omitted, count the number
// of initializer elements.
static int count_array_init_elements(C_Parser *parser, C_Token *tok, C_Type *ty) {
  bool first = true;
  Initializer *dummy = new_initializer(parser, ty->base, true);

  int i = 0, max = 0;

  while (!consume_end(&tok, tok)) {
    if (!first)
      tok = C_skip(parser, tok, ",");
    first = false;

    if (C_equal(tok, "[")) {
      i = C_const_expr(parser, &tok, tok->next);
      if (C_equal(tok, "..."))
        i = C_const_expr(parser, &tok, tok->next);
      tok = C_skip(parser, tok, "]");
      designation(parser, &tok, tok, dummy);
    } else {
      initializer2(parser, &tok, tok, dummy);
    }

    i++;
    max = MAX(max, i);
  }
  return max;
}

// array-initializer1 = "{" initializer ("," initializer)* ","? "}"
static void array_initializer1(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init) {
  tok = C_skip(parser, tok, "{");

  if (init->is_flexible) {
    int len = count_array_init_elements(parser, tok, init->ty);
    *init = *new_initializer(parser, C_array_of(parser, init->ty->base, len), false);
  }

  bool first = true;

  if (init->is_flexible) {
    int len = count_array_init_elements(parser, tok, init->ty);
    *init = *new_initializer(parser, C_array_of(parser, init->ty->base, len), false);
  }

  for (int i = 0; !consume_end(rest, tok); i++) {
    if (!first)
      tok = C_skip(parser, tok, ",");
    first = false;

    if (C_equal(tok, "[")) {
      int begin, end;
      array_designator(parser, &tok, tok, init->ty, &begin, &end);

      C_Token *tok2;
      for (int j = begin; j <= end; j++)
        designation(parser, &tok2, tok, init->children[j]);
      tok = tok2;
      i = end;
      continue;
    }

    if (i < init->ty->array_len)
      initializer2(parser, &tok, tok, init->children[i]);
    else
      tok = skip_excess_element(parser, tok);
  }
}

// array-initializer2 = initializer ("," initializer)*
static void array_initializer2(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init, int i) {
  if (init->is_flexible) {
    int len = count_array_init_elements(parser, tok, init->ty);
    *init = *new_initializer(parser, C_array_of(parser, init->ty->base, len), false);
  }

  for (; i < init->ty->array_len && !is_end(tok); i++) {
    C_Token *start = tok;
    if (i > 0)
      tok = C_skip(parser, tok, ",");

    if (C_equal(tok, "[") || C_equal(tok, ".")) {
      *rest = start;
      return;
    }

    initializer2(parser, &tok, tok, init->children[i]);
  }
  *rest = tok;
}

// struct-initializer1 = "{" initializer ("," initializer)* ","? "}"
static void struct_initializer1(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init) {
  tok = C_skip(parser, tok, "{");

  C_Member *mem = init->ty->members;
  bool first = true;

  while (!consume_end(rest, tok)) {
    if (!first)
      tok = C_skip(parser, tok, ",");
    first = false;

    if (C_equal(tok, ".")) {
      mem = struct_designator(parser, &tok, tok, init->ty);
      designation(parser, &tok, tok, init->children[mem->idx]);
      mem = mem->next;
      continue;
    }

    if (mem) {
      initializer2(parser, &tok, tok, init->children[mem->idx]);
      mem = mem->next;
    } else {
      tok = skip_excess_element(parser, tok);
    }
  }
}

// struct-initializer2 = initializer ("," initializer)*
static void struct_initializer2(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init, C_Member *mem) {
  bool first = true;

  for (; mem && !is_end(tok); mem = mem->next) {
    C_Token *start = tok;

    if (!first)
      tok = C_skip(parser, tok, ",");
    first = false;

    if (C_equal(tok, "[") || C_equal(tok, ".")) {
      *rest = start;
      return;
    }

    initializer2(parser, &tok, tok, init->children[mem->idx]);
  }
  *rest = tok;
}

static void union_initializer(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init) {
  // Unlike structs, union initializers take only one initializer,
  // and that initializes the first union member by default.
  // You can initialize other member using a designated initializer.
  if (C_equal(tok, "{") && C_equal(tok->next, ".")) {
    C_Member *mem = struct_designator(parser, &tok, tok->next, init->ty);
    init->mem = mem;
    designation(parser, &tok, tok, init->children[mem->idx]);
    *rest = C_skip(parser, tok, "}");
    return;
  }

  init->mem = init->ty->members;

  if (C_equal(tok, "{")) {
    initializer2(parser, &tok, tok->next, init->children[0]);
    C_consume(&tok, tok, ",");
    *rest = C_skip(parser, tok, "}");
  } else {
    initializer2(parser, rest, tok, init->children[0]);
  }
}

// initializer = string-initializer | array-initializer
//             | struct-initializer | union-initializer
//             | assign
static void initializer2(C_Parser *parser, C_Token **rest, C_Token *tok, Initializer *init) {
  if (init->ty->kind == TY_ARRAY && tok->kind == TK_STR) {
    string_initializer(parser, rest, tok, init);
    return;
  }

  if (init->ty->kind == TY_ARRAY) {
    if (C_equal(tok, "{"))
      array_initializer1(parser, rest, tok, init);
    else
      array_initializer2(parser, rest, tok, init, 0);
    return;
  }

  if (init->ty->kind == TY_STRUCT) {
    if (C_equal(tok, "{")) {
      struct_initializer1(parser, rest, tok, init);
      return;
    }

    // A struct can be initialized with another struct. E.g.
    // `struct T x = y;` where y is a variable of type `struct T`.
    // Handle that case first.
    C_Node *expr = assign(parser, rest, tok);
    C_add_type(parser, expr);
    if (expr->ty->kind == TY_STRUCT) {
      init->expr = expr;
      return;
    }

    struct_initializer2(parser, rest, tok, init, init->ty->members);
    return;
  }

  if (init->ty->kind == TY_UNION) {
    union_initializer(parser, rest, tok, init);
    return;
  }

  if (C_equal(tok, "{")) {
    // An initializer for a scalar variable can be surrounded by
    // braces. E.g. `int x = {3};`. Handle that case.
    initializer2(parser, &tok, tok->next, init);
    *rest = C_skip(parser, tok, "}");
    return;
  }

  init->expr = assign(parser, rest, tok);
}

static C_Type *copy_struct_type(C_Parser *parser, C_Type *ty) {
  ty = C_copy_type(parser, ty);

  C_Member head = {0};
  C_Member *cur = &head;
  for (C_Member *mem = ty->members; mem; mem = mem->next) {
    C_Member *m = parser->memory_allocator->calloc(parser->memory_allocator->arena, 1, sizeof(C_Member));
    *m = *mem;
    cur = cur->next = m;
  }

  ty->members = head.next;
  return ty;
}

static Initializer *initializer(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty, C_Type **new_ty) {
  Initializer *init = new_initializer(parser, ty, true);
  initializer2(parser, rest, tok, init);

  if ((ty->kind == TY_STRUCT || ty->kind == TY_UNION) && ty->is_flexible) {
    ty = copy_struct_type(parser, ty);

    C_Member *mem = ty->members;
    while (mem->next)
      mem = mem->next;
    mem->ty = init->children[mem->idx]->ty;
    ty->size += mem->ty->size;

    *new_ty = ty;
    return init;
  }

  *new_ty = init->ty;
  return init;
}

static C_Node *init_desg_expr(C_Parser *parser, InitDesg *desg, C_Token *tok) {
  if (desg->var)
    return new_var_node(parser, desg->var, tok);

  if (desg->member) {
    C_Node *node = new_unary(parser, ND_MEMBER, init_desg_expr(parser, desg->next, tok), tok);
    node->member = desg->member;
    return node;
  }

  C_Node *lhs = init_desg_expr(parser, desg->next, tok);
  C_Node *rhs = new_num(parser, desg->idx, tok);
  return new_unary(parser, ND_DEREF, new_add(parser, lhs, rhs, tok), tok);
}

static C_Node *create_lvar_init(C_Parser *parser, Initializer *init, C_Type *ty, InitDesg *desg, C_Token *tok) {
  if (ty->kind == TY_ARRAY) {
    C_Node *node = new_node(parser, ND_NULL_EXPR, tok);
    for (int i = 0; i < ty->array_len; i++) {
      InitDesg desg2 = {desg, i};
      C_Node *rhs = create_lvar_init(parser, init->children[i], ty->base, &desg2, tok);
      node = new_binary(parser, ND_COMMA, node, rhs, tok);
    }
    return node;
  }

  if (ty->kind == TY_STRUCT && !init->expr) {
    C_Node *node = new_node(parser, ND_NULL_EXPR, tok);

    for (C_Member *mem = ty->members; mem; mem = mem->next) {
      InitDesg desg2 = {desg, 0, mem};
      C_Node *rhs = create_lvar_init(parser, init->children[mem->idx], mem->ty, &desg2, tok);
      node = new_binary(parser, ND_COMMA, node, rhs, tok);
    }
    return node;
  }

  if (ty->kind == TY_UNION) {
    C_Member *mem = init->mem ? init->mem : ty->members;
    InitDesg desg2 = {desg, 0, mem};
    return create_lvar_init(parser, init->children[mem->idx], mem->ty, &desg2, tok);
  }

  if (!init->expr)
    return new_node(parser, ND_NULL_EXPR, tok);

  C_Node *lhs = init_desg_expr(parser, desg, tok);
  return new_binary(parser, ND_ASSIGN, lhs, init->expr, tok);
}

// A variable definition with an initializer is a shorthand notation
// for a variable definition followed by assignments. This function
// generates assignment expressions for an initializer. For example,
// `int x[2][2] = {{6, 7}, {8, 9}}` is converted to the following
// expressions:
//
//   x[0][0] = 6;
//   x[0][1] = 7;
//   x[1][0] = 8;
//   x[1][1] = 9;
static C_Node *lvar_initializer(C_Parser *parser, C_Token **rest, C_Token *tok, C_Obj *var) {
  Initializer *init = initializer(parser, rest, tok, var->ty, &var->ty);
  InitDesg desg = {NULL, 0, NULL, var};

  // If a partial initializer list is given, the standard requires
  // that unspecified elements are set to 0. Here, we simply
  // zero-initialize the entire memory region of a variable before
  // initializing it with user-supplied values.
  C_Node *lhs = new_node(parser, ND_MEMZERO, tok);
  lhs->var = var;

  C_Node *rhs = create_lvar_init(parser, init, var->ty, &desg, tok);
  return new_binary(parser, ND_COMMA, lhs, rhs, tok);
}

static uint64_t read_buf(C_Parser *parser, char *buf, int sz) {
  if (sz == 1)
    return *buf;
  if (sz == 2)
    return *(uint16_t *)buf;
  if (sz == 4)
    return *(uint32_t *)buf;
  if (sz == 8)
    return *(uint64_t *)buf;
  unreachable(parser);
}

static void write_buf(C_Parser *parser, char *buf, uint64_t val, int sz) {
  if (sz == 1)
    *buf = val;
  else if (sz == 2)
    *(uint16_t *)buf = val;
  else if (sz == 4)
    *(uint32_t *)buf = val;
  else if (sz == 8)
    *(uint64_t *)buf = val;
  else
    unreachable(parser);
}

static C_Relocation *
write_gvar_data(C_Parser *parser, C_Relocation *cur, Initializer *init, C_Type *ty, char *buf, int offset) {
  if (ty->kind == TY_ARRAY) {
    int sz = ty->base->size;
    for (int i = 0; i < ty->array_len; i++)
      cur = write_gvar_data(parser, cur, init->children[i], ty->base, buf, offset + sz * i);
    return cur;
  }

  if (ty->kind == TY_STRUCT) {
    for (C_Member *mem = ty->members; mem; mem = mem->next) {
      if (mem->is_bitfield) {
        C_Node *expr = init->children[mem->idx]->expr;
        if (!expr)
          break;

        char *loc = buf + offset + mem->offset;
        uint64_t oldval = read_buf(parser, loc, mem->ty->size);
        uint64_t newval = eval(parser, expr);
        uint64_t mask = (1L << mem->bit_width) - 1;
        uint64_t combined = oldval | ((newval & mask) << mem->bit_offset);
        write_buf(parser, loc, combined, mem->ty->size);
      } else {
        cur = write_gvar_data(parser, cur, init->children[mem->idx], mem->ty, buf,
                              offset + mem->offset);
      }
    }
    return cur;
  }

  if (ty->kind == TY_UNION) {
    if (!init->mem)
      return cur;
    return write_gvar_data(parser, cur, init->children[init->mem->idx],
                           init->mem->ty, buf, offset);
  }

  if (!init->expr)
    return cur;

  if (ty->kind == TY_FLOAT) {
    *(float *)(buf + offset) = eval_double(parser, init->expr);
    return cur;
  }

  if (ty->kind == TY_DOUBLE) {
    *(double *)(buf + offset) = eval_double(parser, init->expr);
    return cur;
  }

  char **label = NULL;
  uint64_t val = eval2(parser, init->expr, &label);

  if (!label) {
    write_buf(parser, buf + offset, val, ty->size);
    return cur;
  }

  C_Relocation *rel = parser->memory_allocator->calloc(parser->memory_allocator->arena, 1, sizeof(C_Relocation));
  rel->offset = offset;
  rel->label = label;
  rel->addend = val;
  cur->next = rel;
  return cur->next;
}

// Initializers for global variables are evaluated at compile-time and
// embedded to .data section. This function serializes Initializer
// objects to a flat byte array. It is a compile error if an
// initializer list contains a non-constant expression.
static void gvar_initializer(C_Parser *parser, C_Token **rest, C_Token *tok, C_Obj *var) {
  Initializer *init = initializer(parser, rest, tok, var->ty, &var->ty);

  C_Relocation head = {0};
  char *buf = parser->memory_allocator->calloc(parser->memory_allocator->arena, 1, var->ty->size);
  write_gvar_data(parser, &head, init, var->ty, buf, 0);
  var->init_data = buf;
  var->rel = head.next;
}

// Returns true if a given token represents a type.
static bool is_typename(C_Parser *parser, C_Token *tok) {
  if (parser->typewords.capacity == 0) {
    static char *kw[] = {
      "void", "_Bool", "char", "short", "int", "long", "struct", "union",
      "typedef", "enum", "static", "extern", "_Alignas", "signed", "unsigned",
      "const", "volatile", "auto", "register", "restrict", "__restrict",
      "__restrict__", "_Noreturn", "float", "double", "typeof", "inline",
      "_Thread_local", "__thread", "_Atomic",
    };

    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
      hashmap_put(&parser->typewords, kw[i], (void *)1);
  }
  return hashmap_get2(&parser->typewords, tok->loc, tok->len) || find_typedef(parser, tok);
}

// asm-stmt = "asm" ("volatile" | "inline")* "(" string-literal ")"
static C_Node *asm_stmt(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = new_node(parser, ND_ASM, tok);
  tok = tok->next;

  while (C_equal(tok, "volatile") || C_equal(tok, "inline"))
    tok = tok->next;

  tok = C_skip(parser, tok, "(");
  if (tok->kind != TK_STR || tok->ty->base->kind != TY_CHAR)
    C_error_tok(parser, tok, "expected string literal");
  node->asm_str = tok->str;
  *rest = C_skip(parser, tok->next, ")");
  return node;
}

// stmt = "return" expr? ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "switch" "(" expr ")" stmt
//      | "case" const-expr ("..." const-expr)? ":" stmt
//      | "default" ":" stmt
//      | "for" "(" expr-stmt expr? ";" expr? ")" stmt
//      | "while" "(" expr ")" stmt
//      | "do" stmt "while" "(" expr ")" ";"
//      | "asm" asm-stmt
//      | "goto" (ident | "*" expr) ";"
//      | "break" ";"
//      | "continue" ";"
//      | ident ":" stmt
//      | "{" compound-stmt
//      | expr-stmt
static C_Node *stmt(C_Parser *parser, C_Token **rest, C_Token *tok) {
  if (C_equal(tok, "return")) {
    C_Node *node = new_node(parser, ND_RETURN, tok);
    if (C_consume(rest, tok->next, ";"))
      return node;

    C_Node *exp = expr(parser, &tok, tok->next);
    *rest = C_skip(parser, tok, ";");

    C_add_type(parser, exp);
    C_Type *ty = parser->current_fn->ty->return_ty;
    if (ty->kind != TY_STRUCT && ty->kind != TY_UNION)
      exp = C_new_cast(parser, exp, parser->current_fn->ty->return_ty);

    node->lhs = exp;
    return node;
  }

  if (C_equal(tok, "if")) {
    C_Node *node = new_node(parser, ND_IF, tok);
    tok = C_skip(parser, tok->next, "(");
    node->cond = expr(parser, &tok, tok);
    tok = C_skip(parser, tok, ")");
    node->then = stmt(parser, &tok, tok);
    if (C_equal(tok, "else"))
      node->els = stmt(parser, &tok, tok->next);
    *rest = tok;
    return node;
  }

  if (C_equal(tok, "switch")) {
    C_Node *node = new_node(parser, ND_SWITCH, tok);
    tok = C_skip(parser, tok->next, "(");
    node->cond = expr(parser, &tok, tok);
    tok = C_skip(parser, tok, ")");

    C_Node *sw = parser->current_switch;
    parser->current_switch = node;

    char *brk = parser->brk_label;
    parser->brk_label = node->brk_label = new_unique_name(parser->memory_allocator);

    node->then = stmt(parser, rest, tok);

    parser->current_switch = sw;
    parser->brk_label = brk;
    return node;
  }

  if (C_equal(tok, "case")) {
    if (!parser->current_switch)
      C_error_tok(parser, tok, "stray case");

    C_Node *node = new_node(parser, ND_CASE, tok);
    int begin = C_const_expr(parser, &tok, tok->next);
    int end;

    if (C_equal(tok, "...")) {
      // [GNU] Case ranges, e.g. "case 1 ... 5:"
      end = C_const_expr(parser, &tok, tok->next);
      if (end < begin)
        C_error_tok(parser, tok, "empty case range specified");
    } else {
      end = begin;
    }

    tok = C_skip(parser, tok, ":");
    node->label = new_unique_name(parser->memory_allocator);
    node->lhs = stmt(parser, rest, tok);
    node->begin = begin;
    node->end = end;
    node->case_next = parser->current_switch->case_next;
    parser->current_switch->case_next = node;
    return node;
  }

  if (C_equal(tok, "default")) {
    if (!parser->current_switch)
      C_error_tok(parser, tok, "stray default");

    C_Node *node = new_node(parser, ND_CASE, tok);
    tok = C_skip(parser, tok->next, ":");
    node->label = new_unique_name(parser->memory_allocator);
    node->lhs = stmt(parser, rest, tok);
    parser->current_switch->default_case = node;
    return node;
  }

  if (C_equal(tok, "for")) {
    C_Node *node = new_node(parser, ND_FOR, tok);
    tok = C_skip(parser, tok->next, "(");

    enter_scope(parser);

    char *brk = parser->brk_label;
    char *cont = parser->cont_label;
    parser->brk_label = node->brk_label = new_unique_name(parser->memory_allocator);
    parser->cont_label = node->cont_label = new_unique_name(parser->memory_allocator);

    if (is_typename(parser, tok)) {
      C_Type *basety = declspec(parser, &tok, tok, NULL);
      node->init = declaration(parser, &tok, tok, basety, NULL);
    } else {
      node->init = expr_stmt(parser, &tok, tok);
    }

    if (!C_equal(tok, ";"))
      node->cond = expr(parser, &tok, tok);
    tok = C_skip(parser, tok, ";");

    if (!C_equal(tok, ")"))
      node->inc = expr(parser, &tok, tok);
    tok = C_skip(parser, tok, ")");

    node->then = stmt(parser, rest, tok);

    leave_scope(parser);
    parser->brk_label = brk;
    parser->cont_label = cont;
    return node;
  }

  if (C_equal(tok, "while")) {
    C_Node *node = new_node(parser, ND_FOR, tok);
    tok = C_skip(parser, tok->next, "(");
    node->cond = expr(parser, &tok, tok);
    tok = C_skip(parser, tok, ")");

    char *brk = parser->brk_label;
    char *cont = parser->cont_label;
    parser->brk_label = node->brk_label = new_unique_name(parser->memory_allocator);
    parser->cont_label = node->cont_label = new_unique_name(parser->memory_allocator);

    node->then = stmt(parser, rest, tok);

    parser->brk_label = brk;
    parser->cont_label = cont;
    return node;
  }

  if (C_equal(tok, "do")) {
    C_Node *node = new_node(parser, ND_DO, tok);

    char *brk = parser->brk_label;
    char *cont = parser->cont_label;
    parser->brk_label = node->brk_label = new_unique_name(parser->memory_allocator);
    parser->cont_label = node->cont_label = new_unique_name(parser->memory_allocator);

    node->then = stmt(parser, &tok, tok->next);

    parser->brk_label = brk;
    parser->cont_label = cont;

    tok = C_skip(parser, tok, "while");
    tok = C_skip(parser, tok, "(");
    node->cond = expr(parser, &tok, tok);
    tok = C_skip(parser, tok, ")");
    *rest = C_skip(parser, tok, ";");
    return node;
  }

  if (C_equal(tok, "asm"))
    return asm_stmt(parser, rest, tok);

  if (C_equal(tok, "goto")) {
    if (C_equal(tok->next, "*")) {
      // [GNU] `goto *ptr` jumps to the address specified by `ptr`.
      C_Node *node = new_node(parser, ND_GOTO_EXPR, tok);
      node->lhs = expr(parser, &tok, tok->next->next);
      *rest = C_skip(parser, tok, ";");
      return node;
    }

    C_Node *node = new_node(parser, ND_GOTO, tok);
    node->label = get_ident(parser, tok->next);
    node->goto_next = parser->gotos;
    parser->gotos = node;
    *rest = C_skip(parser, tok->next->next, ";");
    return node;
  }

  if (C_equal(tok, "break")) {
    if (!parser->brk_label)
      C_error_tok(parser, tok, "stray break");
    C_Node *node = new_node(parser, ND_GOTO, tok);
    node->unique_label = parser->brk_label;
    *rest = C_skip(parser, tok->next, ";");
    return node;
  }

  if (C_equal(tok, "continue")) {
    if (!parser->cont_label)
      C_error_tok(parser, tok, "stray continue");
    C_Node *node = new_node(parser, ND_GOTO, tok);
    node->unique_label = parser->cont_label;
    *rest = C_skip(parser, tok->next, ";");
    return node;
  }

  if (tok->kind == TK_IDENT && C_equal(tok->next, ":")) {
    C_Node *node = new_node(parser, ND_LABEL, tok);
    node->label = str_dup(parser->memory_allocator, tok->loc, tok->len);
    node->unique_label = new_unique_name(parser->memory_allocator);
    node->lhs = stmt(parser, rest, tok->next->next);
    node->goto_next = parser->labels;
    parser->labels = node;
    return node;
  }

  if (C_equal(tok, "{"))
    return compound_stmt(parser, rest, tok->next);

  return expr_stmt(parser, rest, tok);
}

// compound-stmt = (typedef | declaration | stmt)* "}"
static C_Node *compound_stmt(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = new_node(parser, ND_BLOCK, tok);
  C_Node head = {0};
  C_Node *cur = &head;

  enter_scope(parser);

  while ((!parser->embedded_mode || parser->embedded_mode && tok->kind != TK_EOF) && !C_equal(tok, "}")) {
    if (is_typename(parser, tok) && !C_equal(tok->next, ":")) {
      VarAttr attr = {0};
      C_Type *basety = declspec(parser, &tok, tok, &attr);

      if (attr.is_typedef) {
        tok = parse_typedef(parser, tok, basety);
        continue;
      }

      if (is_function(parser, tok)) {
        tok = function(parser, tok, basety, &attr);
        continue;
      }

      if (attr.is_extern) {
        tok = global_variable(parser, tok, basety, &attr);
        continue;
      }

      cur = cur->next = declaration(parser, &tok, tok, basety, &attr);
    } else {
      cur = cur->next = stmt(parser, &tok, tok);
    }
    C_add_type(parser, cur);
  }

  leave_scope(parser);

  node->body = head.next;
  *rest = tok->next;
  return node;
}

// expr-stmt = expr? ";"
static C_Node *expr_stmt(C_Parser *parser, C_Token **rest, C_Token *tok) {
  if (C_equal(tok, ";")) {
    *rest = tok->next;
    return new_node(parser, ND_BLOCK, tok);
  }

  C_Node *node = new_node(parser, ND_EXPR_STMT, tok);
  node->lhs = expr(parser, &tok, tok);
  *rest = C_skip(parser, tok, ";");
  return node;
}

// expr = assign ("," expr)?
static C_Node *expr(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = assign(parser, &tok, tok);

  if (C_equal(tok, ","))
    return new_binary(parser, ND_COMMA, node, expr(parser, rest, tok->next), tok);

  *rest = tok;
  return node;
}

static int64_t eval(C_Parser *parser, C_Node *node) {
  return eval2(parser, node, NULL);
}

// Evaluate a given node as a constant expression.
//
// A constant expression is either just a number or ptr+n where ptr
// is a pointer to a global variable and n is a postiive/negative
// number. The latter form is accepted only as an initialization
// expression for a global variable.
static int64_t eval2(C_Parser *parser, C_Node *node, char ***label) {
  C_add_type(parser, node);

  if (C_is_flonum(node->ty))
    return eval_double(parser, node);

  switch (node->kind) {
  case ND_ADD:
    return eval2(parser, node->lhs, label) + eval(parser, node->rhs);
  case ND_SUB:
    return eval2(parser, node->lhs, label) - eval(parser, node->rhs);
  case ND_MUL:
    return eval(parser, node->lhs) * eval(parser, node->rhs);
  case ND_DIV:
    if (node->ty->is_unsigned)
      return (uint64_t)eval(parser, node->lhs) / eval(parser, node->rhs);
    return eval(parser, node->lhs) / eval(parser, node->rhs);
  case ND_NEG:
    return -eval(parser, node->lhs);
  case ND_MOD:
    if (node->ty->is_unsigned)
      return (uint64_t)eval(parser, node->lhs) % eval(parser, node->rhs);
    return eval(parser, node->lhs) % eval(parser, node->rhs);
  case ND_BITAND:
    return eval(parser, node->lhs) & eval(parser, node->rhs);
  case ND_BITOR:
    return eval(parser, node->lhs) | eval(parser, node->rhs);
  case ND_BITXOR:
    return eval(parser, node->lhs) ^ eval(parser, node->rhs);
  case ND_SHL:
    return eval(parser, node->lhs) << eval(parser, node->rhs);
  case ND_SHR:
    if (node->ty->is_unsigned && node->ty->size == 8)
      return (uint64_t)eval(parser, node->lhs) >> eval(parser, node->rhs);
    return eval(parser, node->lhs) >> eval(parser, node->rhs);
  case ND_EQ:
    return eval(parser, node->lhs) == eval(parser, node->rhs);
  case ND_NE:
    return eval(parser, node->lhs) != eval(parser, node->rhs);
  case ND_LT:
    if (node->lhs->ty->is_unsigned)
      return (uint64_t)eval(parser, node->lhs) < eval(parser, node->rhs);
    return eval(parser, node->lhs) < eval(parser, node->rhs);
  case ND_LE:
    if (node->lhs->ty->is_unsigned)
      return (uint64_t)eval(parser, node->lhs) <= eval(parser, node->rhs);
    return eval(parser, node->lhs) <= eval(parser, node->rhs);
  case ND_COND:
    return eval(parser, node->cond) ? eval2(parser, node->then, label) : eval2(parser, node->els, label);
  case ND_COMMA:
    return eval2(parser, node->rhs, label);
  case ND_NOT:
    return !eval(parser, node->lhs);
  case ND_BITNOT:
    return ~eval(parser, node->lhs);
  case ND_LOGAND:
    return eval(parser, node->lhs) && eval(parser, node->rhs);
  case ND_LOGOR:
    return eval(parser, node->lhs) || eval(parser, node->rhs);
  case ND_CAST: {
    int64_t val = eval2(parser, node->lhs, label);
    if (C_is_integer(node->ty)) {
      switch (node->ty->size) {
      case 1: return node->ty->is_unsigned ? (uint8_t)val : (int8_t)val;
      case 2: return node->ty->is_unsigned ? (uint16_t)val : (int16_t)val;
      case 4: return node->ty->is_unsigned ? (uint32_t)val : (int32_t)val;
      }
    }
    return val;
  }
  case ND_ADDR:
    return eval_rval(parser, node->lhs, label);
  case ND_LABEL_VAL:
    *label = &node->unique_label;
    return 0;
  case ND_MEMBER:
    if (!label)
      C_error_tok(parser, node->tok, "not a compile-time constant");
    if (node->ty->kind != TY_ARRAY)
      C_error_tok(parser, node->tok, "invalid initializer");
    return eval_rval(parser, node->lhs, label) + node->member->offset;
  case ND_VAR:
    if (!label)
      C_error_tok(parser, node->tok, "not a compile-time constant");
    if (node->var->ty->kind != TY_ARRAY && node->var->ty->kind != TY_FUNC)
      C_error_tok(parser, node->tok, "invalid initializer");
    *label = &node->var->name;
    return 0;
  case ND_NUM:
    return node->val;
  }

  C_error_tok(parser, node->tok, "not a compile-time constant");
}

static int64_t eval_rval(C_Parser *parser, C_Node *node, char ***label) {
  switch (node->kind) {
  case ND_VAR:
    if (node->var->is_local)
      C_error_tok(parser, node->tok, "not a compile-time constant");
    *label = &node->var->name;
    return 0;
  case ND_DEREF:
    return eval2(parser, node->lhs, label);
  case ND_MEMBER:
    return eval_rval(parser, node->lhs, label) + node->member->offset;
  }

  C_error_tok(parser, node->tok, "invalid initializer");
}

static bool is_const_expr(C_Parser *parser, C_Node *node) {
  C_add_type(parser, node);

  switch (node->kind) {
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
  case ND_SHL:
  case ND_SHR:
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_LOGAND:
  case ND_LOGOR:
    return is_const_expr(parser, node->lhs) && is_const_expr(parser, node->rhs);
  case ND_COND:
    if (!is_const_expr(parser, node->cond))
      return false;
    return is_const_expr(parser, eval(parser, node->cond) ? node->then : node->els);
  case ND_COMMA:
    return is_const_expr(parser, node->rhs);
  case ND_NEG:
  case ND_NOT:
  case ND_BITNOT:
  case ND_CAST:
    return is_const_expr(parser, node->lhs);
  case ND_NUM:
    return true;
  }

  return false;
}

int64_t C_const_expr(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = conditional(parser, rest, tok);
  return eval(parser, node);
}

static double eval_double(C_Parser *parser, C_Node *node) {
  C_add_type(parser, node);

  if (C_is_integer(node->ty)) {
    if (node->ty->is_unsigned)
      return (unsigned long)eval(parser, node);
    return eval(parser, node);
  }

  switch (node->kind) {
  case ND_ADD:
    return eval_double(parser, node->lhs) + eval_double(parser, node->rhs);
  case ND_SUB:
    return eval_double(parser, node->lhs) - eval_double(parser, node->rhs);
  case ND_MUL:
    return eval_double(parser, node->lhs) * eval_double(parser, node->rhs);
  case ND_DIV:
    return eval_double(parser, node->lhs) / eval_double(parser, node->rhs);
  case ND_NEG:
    return -eval_double(parser, node->lhs);
  case ND_COND:
    return eval_double(parser, node->cond) ? eval_double(parser, node->then) : eval_double(parser, node->els);
  case ND_COMMA:
    return eval_double(parser, node->rhs);
  case ND_CAST:
    if (C_is_flonum(node->lhs->ty))
      return eval_double(parser, node->lhs);
    return eval(parser, node->lhs);
  case ND_NUM:
    return node->fval;
  }

  C_error_tok(parser, node->tok, "not a compile-time constant");
}

// Convert op= operators to expressions containing an assignment.
//
// In general, `A op= C` is converted to ``tmp = &A, *tmp = *tmp op B`.
// However, if a given expression is of form `A.x op= C`, the input is
// converted to `tmp = &A, (*tmp).x = (*tmp).x op C` to handle assignments
// to bitfields.
static C_Node *to_assign(C_Parser *parser, C_Node *binary) {
  C_add_type(parser, binary->lhs);
  C_add_type(parser, binary->rhs);
  C_Token *tok = binary->tok;

  // Convert `A.x op= C` to `tmp = &A, (*tmp).x = (*tmp).x op C`.
  if (binary->lhs->kind == ND_MEMBER) {
    C_Obj *var = new_lvar(parser, "", C_pointer_to(parser, binary->lhs->lhs->ty));

    C_Node *expr1 = new_binary(parser, ND_ASSIGN, new_var_node(parser, var, tok),
                             new_unary(parser, ND_ADDR, binary->lhs->lhs, tok), tok);

    C_Node *expr2 = new_unary(parser, ND_MEMBER,
                            new_unary(parser, ND_DEREF, new_var_node(parser, var, tok), tok),
                            tok);
    expr2->member = binary->lhs->member;

    C_Node *expr3 = new_unary(parser, ND_MEMBER,
                            new_unary(parser, ND_DEREF, new_var_node(parser, var, tok), tok),
                            tok);
    expr3->member = binary->lhs->member;

    C_Node *expr4 = new_binary(parser, ND_ASSIGN, expr2,
                             new_binary(parser, binary->kind, expr3, binary->rhs, tok),
                             tok);

    return new_binary(parser, ND_COMMA, expr1, expr4, tok);
  }

  // If A is an atomic type, Convert `A op= B` to
  //
  // ({
  //   T1 *addr = &A; T2 val = (B); T1 old = *addr; T1 new;
  //   do {
  //    new = old op val;
  //   } while (!atomic_compare_exchange_strong(addr, &old, new));
  //   new;
  // })
  if (binary->lhs->ty->is_atomic) {
    C_Node head = {0};
    C_Node *cur = &head;

    C_Obj *addr = new_lvar(parser, "", C_pointer_to(parser, binary->lhs->ty));
    C_Obj *val = new_lvar(parser, "", binary->rhs->ty);
    C_Obj *old = new_lvar(parser, "", binary->lhs->ty);
    C_Obj *new = new_lvar(parser, "", binary->lhs->ty);

    cur = cur->next =
      new_unary(parser, ND_EXPR_STMT,
                new_binary(parser, ND_ASSIGN, new_var_node(parser, addr, tok),
                           new_unary(parser, ND_ADDR, binary->lhs, tok), tok),
                tok);

    cur = cur->next =
      new_unary(parser, ND_EXPR_STMT,
                new_binary(parser, ND_ASSIGN, new_var_node(parser, val, tok), binary->rhs, tok),
                tok);

    cur = cur->next =
      new_unary(parser, ND_EXPR_STMT,
                new_binary(parser, ND_ASSIGN, new_var_node(parser, old, tok),
                           new_unary(parser, ND_DEREF, new_var_node(parser, addr, tok), tok), tok),
                tok);

    C_Node *loop = new_node(parser, ND_DO, tok);
    loop->brk_label = new_unique_name(parser->memory_allocator);
    loop->cont_label = new_unique_name(parser->memory_allocator);

    C_Node *body = new_binary(parser, ND_ASSIGN,
                            new_var_node(parser, new, tok),
                            new_binary(parser, binary->kind, new_var_node(parser, old, tok),
                                       new_var_node(parser, val, tok), tok),
                            tok);

    loop->then = new_node(parser, ND_BLOCK, tok);
    loop->then->body = new_unary(parser, ND_EXPR_STMT, body, tok);

    C_Node *cas = new_node(parser, ND_CAS, tok);
    cas->cas_addr = new_var_node(parser, addr, tok);
    cas->cas_old = new_unary(parser, ND_ADDR, new_var_node(parser, old, tok), tok);
    cas->cas_new = new_var_node(parser, new, tok);
    loop->cond = new_unary(parser, ND_NOT, cas, tok);

    cur = cur->next = loop;
    cur = cur->next = new_unary(parser, ND_EXPR_STMT, new_var_node(parser, new, tok), tok);

    C_Node *node = new_node(parser, ND_STMT_EXPR, tok);
    node->body = head.next;
    return node;
  }

  // Convert `A op= B` to ``tmp = &A, *tmp = *tmp op B`.
  C_Obj *var = new_lvar(parser, "", C_pointer_to(parser, binary->lhs->ty));

  C_Node *expr1 = new_binary(parser, ND_ASSIGN, new_var_node(parser, var, tok),
                           new_unary(parser, ND_ADDR, binary->lhs, tok), tok);

  C_Node *expr2 =
    new_binary(parser, ND_ASSIGN,
               new_unary(parser, ND_DEREF, new_var_node(parser, var, tok), tok),
               new_binary(parser, binary->kind,
                          new_unary(parser, ND_DEREF, new_var_node(parser, var, tok), tok),
                          binary->rhs,
                          tok),
               tok);

  return new_binary(parser, ND_COMMA, expr1, expr2, tok);
}

// assign    = conditional (assign-op assign)?
// assign-op = "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^="
//           | "<<=" | ">>="
static C_Node *assign(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = conditional(parser, &tok, tok);

  if (C_equal(tok, "="))
    return new_binary(parser, ND_ASSIGN, node, assign(parser, rest, tok->next), tok);

  if (C_equal(tok, "+="))
    return to_assign(parser, new_add(parser, node, assign(parser, rest, tok->next), tok));

  if (C_equal(tok, "-="))
    return to_assign(parser, new_sub(parser, node, assign(parser, rest, tok->next), tok));

  if (C_equal(tok, "*="))
    return to_assign(parser, new_binary(parser, ND_MUL, node, assign(parser, rest, tok->next), tok));

  if (C_equal(tok, "/="))
    return to_assign(parser, new_binary(parser, ND_DIV, node, assign(parser, rest, tok->next), tok));

  if (C_equal(tok, "%="))
    return to_assign(parser, new_binary(parser, ND_MOD, node, assign(parser, rest, tok->next), tok));

  if (C_equal(tok, "&="))
    return to_assign(parser, new_binary(parser, ND_BITAND, node, assign(parser, rest, tok->next), tok));

  if (C_equal(tok, "|="))
    return to_assign(parser, new_binary(parser, ND_BITOR, node, assign(parser, rest, tok->next), tok));

  if (C_equal(tok, "^="))
    return to_assign(parser, new_binary(parser, ND_BITXOR, node, assign(parser, rest, tok->next), tok));

  if (C_equal(tok, "<<="))
    return to_assign(parser, new_binary(parser, ND_SHL, node, assign(parser, rest, tok->next), tok));

  if (C_equal(tok, ">>="))
    return to_assign(parser, new_binary(parser, ND_SHR, node, assign(parser, rest, tok->next), tok));

  *rest = tok;
  return node;
}

// conditional = logor ("?" expr? ":" conditional)?
static C_Node *conditional(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *cond = logor(parser, &tok, tok);

  if (!C_equal(tok, "?")) {
    *rest = tok;
    return cond;
  }

  if (C_equal(tok->next, ":")) {
    // [GNU] Compile `a ?: b` as `tmp = a, tmp ? tmp : b`.
    C_add_type(parser, cond);
    C_Obj *var = new_lvar(parser, "", cond->ty);
    C_Node *lhs = new_binary(parser, ND_ASSIGN, new_var_node(parser, var, tok), cond, tok);
    C_Node *rhs = new_node(parser, ND_COND, tok);
    rhs->cond = new_var_node(parser, var, tok);
    rhs->then = new_var_node(parser, var, tok);
    rhs->els = conditional(parser, rest, tok->next->next);
    return new_binary(parser, ND_COMMA, lhs, rhs, tok);
  }

  C_Node *node = new_node(parser, ND_COND, tok);
  node->cond = cond;
  node->then = expr(parser, &tok, tok->next);
  tok = C_skip(parser, tok, ":");
  node->els = conditional(parser, rest, tok);
  return node;
}

// logor = logand ("||" logand)*
static C_Node *logor(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = logand(parser, &tok, tok);
  while (C_equal(tok, "||")) {
    C_Token *start = tok;
    node = new_binary(parser, ND_LOGOR, node, logand(parser, &tok, tok->next), start);
  }
  *rest = tok;
  return node;
}

// logand = bitor ("&&" bitor)*
static C_Node *logand(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = bitor(parser, &tok, tok);
  while (C_equal(tok, "&&")) {
    C_Token *start = tok;
    node = new_binary(parser, ND_LOGAND, node, bitor(parser, &tok, tok->next), start);
  }
  *rest = tok;
  return node;
}

// bitor = bitxor ("|" bitxor)*
static C_Node *bitor(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = bitxor(parser, &tok, tok);
  while (C_equal(tok, "|")) {
    C_Token *start = tok;
    node = new_binary(parser, ND_BITOR, node, bitxor(parser, &tok, tok->next), start);
  }
  *rest = tok;
  return node;
}

// bitxor = bitand ("^" bitand)*
static C_Node *bitxor(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = bitand(parser, &tok, tok);
  while (C_equal(tok, "^")) {
    C_Token *start = tok;
    node = new_binary(parser, ND_BITXOR, node, bitand(parser, &tok, tok->next), start);
  }
  *rest = tok;
  return node;
}

// bitand = equality ("&" equality)*
static C_Node *bitand(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = equality(parser, &tok, tok);
  while (C_equal(tok, "&")) {
    C_Token *start = tok;
    node = new_binary(parser, ND_BITAND, node, equality(parser, &tok, tok->next), start);
  }
  *rest = tok;
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
static C_Node *equality(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = relational(parser, &tok, tok);

  for (;;) {
    C_Token *start = tok;

    if (C_equal(tok, "==")) {
      node = new_binary(parser, ND_EQ, node, relational(parser, &tok, tok->next), start);
      continue;
    }

    if (C_equal(tok, "!=")) {
      node = new_binary(parser, ND_NE, node, relational(parser, &tok, tok->next), start);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// relational = shift ("<" shift | "<=" shift | ">" shift | ">=" shift)*
static C_Node *relational(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = shift(parser, &tok, tok);

  for (;;) {
    C_Token *start = tok;

    if (C_equal(tok, "<")) {
      node = new_binary(parser, ND_LT, node, shift(parser, &tok, tok->next), start);
      continue;
    }

    if (C_equal(tok, "<=")) {
      node = new_binary(parser, ND_LE, node, shift(parser, &tok, tok->next), start);
      continue;
    }

    if (C_equal(tok, ">")) {
      node = new_binary(parser, ND_LT, shift(parser, &tok, tok->next), node, start);
      continue;
    }

    if (C_equal(tok, ">=")) {
      node = new_binary(parser, ND_LE, shift(parser, &tok, tok->next), node, start);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// shift = add ("<<" add | ">>" add)*
static C_Node *shift(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = add(parser, &tok, tok);

  for (;;) {
    C_Token *start = tok;

    if (C_equal(tok, "<<")) {
      node = new_binary(parser, ND_SHL, node, add(parser, &tok, tok->next), start);
      continue;
    }

    if (C_equal(tok, ">>")) {
      node = new_binary(parser, ND_SHR, node, add(parser, &tok, tok->next), start);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// In C, `+` operator is overloaded to perform the pointer arithmetic.
// If p is a pointer, p+n adds not n but sizeof(*p)*n to the value of p,
// so that p+n points to the location n elements (not bytes) ahead of p.
// In other words, we need to scale an integer value before adding to a
// pointer value. This function takes care of the scaling.
static C_Node *new_add(C_Parser *parser, C_Node *lhs, C_Node *rhs, C_Token *tok) {
  C_add_type(parser, lhs);
  C_add_type(parser, rhs);

  // num + num
  if (C_is_numeric(lhs->ty) && C_is_numeric(rhs->ty))
    return new_binary(parser, ND_ADD, lhs, rhs, tok);

  if (lhs->ty->base && rhs->ty->base)
    C_error_tok(parser, tok, "invalid operands");
  if (!lhs->ty->base && !rhs->ty->base)
    C_error_tok(parser, tok, "invalid operands");

  // Canonicalize `num + ptr` to `ptr + num`.
  if (!lhs->ty->base && rhs->ty->base) {
    C_Node *tmp = lhs;
    lhs = rhs;
    rhs = tmp;
  }

  // VLA + num
  if (lhs->ty->base->kind == TY_VLA) {
    rhs = new_binary(parser, ND_MUL, rhs, new_var_node(parser, lhs->ty->base->vla_size, tok), tok);
    return new_binary(parser, ND_ADD, lhs, rhs, tok);
  }

  // ptr + num
  rhs = new_binary(parser, ND_MUL, rhs, new_long(parser, lhs->ty->base->size, tok), tok);
  return new_binary(parser, ND_ADD, lhs, rhs, tok);
}

// Like `+`, `-` is overloaded for the pointer type.
static C_Node *new_sub(C_Parser *parser, C_Node *lhs, C_Node *rhs, C_Token *tok) {
  C_add_type(parser, lhs);
  C_add_type(parser, rhs);

  // num - num
  if (C_is_numeric(lhs->ty) && C_is_numeric(rhs->ty))
    return new_binary(parser, ND_SUB, lhs, rhs, tok);

  // VLA + num
  if (lhs->ty->base->kind == TY_VLA) {
    rhs = new_binary(parser, ND_MUL, rhs, new_var_node(parser, lhs->ty->base->vla_size, tok), tok);
    C_add_type(parser, rhs);
    C_Node *node = new_binary(parser, ND_SUB, lhs, rhs, tok);
    node->ty = lhs->ty;
    return node;
  }

  // ptr - num
  if (lhs->ty->base && C_is_integer(rhs->ty)) {
    rhs = new_binary(parser, ND_MUL, rhs, new_long(parser, lhs->ty->base->size, tok), tok);
    C_add_type(parser, rhs);
    C_Node *node = new_binary(parser, ND_SUB, lhs, rhs, tok);
    node->ty = lhs->ty;
    return node;
  }

  // ptr - ptr, which returns how many elements are between the two.
  if (lhs->ty->base && rhs->ty->base) {
    C_Node *node = new_binary(parser, ND_SUB, lhs, rhs, tok);
    node->ty = C_ty_long;
    return new_binary(parser, ND_DIV, node, new_num(parser, lhs->ty->base->size, tok), tok);
  }

  C_error_tok(parser, tok, "invalid operands");
}

// add = mul ("+" mul | "-" mul)*
static C_Node *add(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = mul(parser, &tok, tok);

  for (;;) {
    C_Token *start = tok;

    if (C_equal(tok, "+")) {
      node = new_add(parser, node, mul(parser, &tok, tok->next), start);
      continue;
    }

    if (C_equal(tok, "-")) {
      node = new_sub(parser, node, mul(parser, &tok, tok->next), start);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// mul = cast ("*" cast | "/" cast | "%" cast)*
static C_Node *mul(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Node *node = cast(parser, &tok, tok);

  for (;;) {
    C_Token *start = tok;

    if (C_equal(tok, "*")) {
      node = new_binary(parser, ND_MUL, node, cast(parser, &tok, tok->next), start);
      continue;
    }

    if (C_equal(tok, "/")) {
      node = new_binary(parser, ND_DIV, node, cast(parser, &tok, tok->next), start);
      continue;
    }

    if (C_equal(tok, "%")) {
      node = new_binary(parser, ND_MOD, node, cast(parser, &tok, tok->next), start);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// cast = "(" type-name ")" cast | unary
static C_Node *cast(C_Parser *parser, C_Token **rest, C_Token *tok) {
  if (C_equal(tok, "(") && is_typename(parser, tok->next)) {
    C_Token *start = tok;
    C_Type *ty = typename(parser, &tok, tok->next);
    tok = C_skip(parser, tok, ")");

    // compound literal
    if (C_equal(tok, "{"))
      return unary(parser, rest, start);

    // type cast
    C_Node *node = C_new_cast(parser, cast(parser, rest, tok), ty);
    node->tok = start;
    return node;
  }

  return unary(parser, rest, tok);
}

// unary = ("+" | "-" | "*" | "&" | "!" | "~") cast
//       | ("++" | "--") unary
//       | "&&" ident
//       | postfix
static C_Node *unary(C_Parser *parser, C_Token **rest, C_Token *tok) {
  if (C_equal(tok, "+"))
    return cast(parser, rest, tok->next);

  if (C_equal(tok, "-"))
    return new_unary(parser, ND_NEG, cast(parser, rest, tok->next), tok);

  if (C_equal(tok, "&")) {
    C_Node *lhs = cast(parser, rest, tok->next);
    C_add_type(parser, lhs);
    if (lhs->kind == ND_MEMBER && lhs->member->is_bitfield)
      C_error_tok(parser, tok, "cannot take address of bitfield");
    return new_unary(parser, ND_ADDR, lhs, tok);
  }

  if (C_equal(tok, "*")) {
    // [https://www.sigbus.info/n1570#6.5.3.2p4] This is an oddity
    // in the C spec, but dereferencing a function shouldn't do
    // anything. If foo is a function, `*foo`, `**foo` or `*****foo`
    // are all equivalent to just `foo`.
    C_Node *node = cast(parser, rest, tok->next);
    C_add_type(parser, node);
    if (node->ty->kind == TY_FUNC)
      return node;
    return new_unary(parser, ND_DEREF, node, tok);
  }

  if (C_equal(tok, "!"))
    return new_unary(parser, ND_NOT, cast(parser, rest, tok->next), tok);

  if (C_equal(tok, "~"))
    return new_unary(parser, ND_BITNOT, cast(parser, rest, tok->next), tok);

  // Read ++i as i+=1
  if (C_equal(tok, "++"))
    return to_assign(parser, new_add(parser, unary(parser, rest, tok->next), new_num(parser, 1, tok), tok));

  // Read --i as i-=1
  if (C_equal(tok, "--"))
    return to_assign(parser, new_sub(parser, unary(parser, rest, tok->next), new_num(parser, 1, tok), tok));

  // [GNU] labels-as-values
  if (C_equal(tok, "&&")) {
    C_Node *node = new_node(parser, ND_LABEL_VAL, tok);
    node->label = get_ident(parser, tok->next);
    node->goto_next = parser->gotos;
    parser->gotos = node;
    *rest = tok->next->next;
    return node;
  }

  return postfix(parser, rest, tok);
}

// struct-members = (declspec declarator (","  declarator)* ";")*
static void struct_members(C_Parser *parser, C_Token **rest, C_Token *tok, C_Type *ty) {
  C_Member head = {0};
  C_Member *cur = &head;
  int idx = 0;

  while (!C_equal(tok, "}")) {
    VarAttr attr = {0};
    C_Type *basety = declspec(parser, &tok, tok, &attr);
    bool first = true;

    // Anonymous struct member
    if ((basety->kind == TY_STRUCT || basety->kind == TY_UNION) && C_consume(&tok, tok, ";")) {
      C_Member *mem = parser->memory_allocator->calloc(parser->memory_allocator->arena, 1, sizeof(C_Member));
      mem->ty = basety;
      mem->idx = idx++;
      mem->align = attr.align ? attr.align : mem->ty->align;
      cur = cur->next = mem;
      continue;
    }

    // Regular struct members
    while (!C_consume(&tok, tok, ";")) {
      if (!first)
        tok = C_skip(parser, tok, ",");
      first = false;

      C_Member *mem = parser->memory_allocator->calloc(parser->memory_allocator->arena, 1, sizeof(C_Member));
      mem->ty = declarator(parser, &tok, tok, basety);
      mem->name = mem->ty->name;
      mem->idx = idx++;
      mem->align = attr.align ? attr.align : mem->ty->align;

      if (C_consume(&tok, tok, ":")) {
        mem->is_bitfield = true;
        mem->bit_width = C_const_expr(parser, &tok, tok);
      }

      cur = cur->next = mem;
    }
  }

  // If the last element is an array of incomplete type, it's
  // called a "flexible array member". It should behave as if
  // if were a zero-sized array.
  if (cur != &head && cur->ty->kind == TY_ARRAY && cur->ty->array_len < 0) {
    cur->ty = C_array_of(parser, cur->ty->base, 0);
    ty->is_flexible = true;
  }

  *rest = tok->next;
  ty->members = head.next;
}

// attribute = ("__attribute__" "(" "(" "packed" ")" ")")*
static C_Token *attribute_list(C_Parser *parser, C_Token *tok, C_Type *ty) {
  while (C_consume(&tok, tok, "__attribute__")) {
    tok = C_skip(parser, tok, "(");
    tok = C_skip(parser, tok, "(");

    bool first = true;

    while (!C_consume(&tok, tok, ")")) {
      if (!first)
        tok = C_skip(parser, tok, ",");
      first = false;

      if (C_consume(&tok, tok, "packed")) {
        ty->is_packed = true;
        continue;
      }

      if (C_consume(&tok, tok, "aligned")) {
        tok = C_skip(parser, tok, "(");
        ty->align = C_const_expr(parser, &tok, tok);
        tok = C_skip(parser, tok, ")");
        continue;
      }

      C_error_tok(parser, tok, "unknown attribute");
    }

    tok = C_skip(parser, tok, ")");
  }

  return tok;
}

// struct-union-decl = attribute? ident? ("{" struct-members)?
static C_Type *struct_union_decl(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Type *ty = C_struct_type(parser);
  tok = attribute_list(parser, tok, ty);

  // Read a tag.
  C_Token *tag = NULL;
  if (tok->kind == TK_IDENT) {
    tag = tok;
    tok = tok->next;
  }

  if (tag && !C_equal(tok, "{")) {
    *rest = tok;

    C_Type *ty2 = find_tag(parser, tag);
    if (ty2)
      return ty2;

    ty->size = -1;
    push_tag_scope(parser, tag, ty);
    return ty;
  }

  tok = C_skip(parser, tok, "{");

  // Construct a struct object.
  struct_members(parser, &tok, tok, ty);
  *rest = attribute_list(parser, tok, ty);

  if (tag) {
    // If this is a redefinition, overwrite a previous type.
    // Otherwise, register the struct type.
    C_Type *ty2 = hashmap_get2(&parser->scope->tags, tag->loc, tag->len);
    if (ty2) {
      *ty2 = *ty;
      return ty2;
    }

    push_tag_scope(parser, tag, ty);
  }

  return ty;
}

// struct-decl = struct-union-decl
static C_Type *struct_decl(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Type *ty = struct_union_decl(parser, rest, tok);
  ty->kind = TY_STRUCT;

  if (ty->size < 0)
    return ty;

  // Assign offsets within the struct to members.
  int bits = 0;

  for (C_Member *mem = ty->members; mem; mem = mem->next) {
    if (mem->is_bitfield && mem->bit_width == 0) {
      // Zero-width anonymous bitfield has a special meaning.
      // It affects only alignment.
      bits = C_align_to(bits, mem->ty->size * 8);
    } else if (mem->is_bitfield) {
      int sz = mem->ty->size;
      if (bits / (sz * 8) != (bits + mem->bit_width - 1) / (sz * 8))
        bits = C_align_to(bits, sz * 8);

      mem->offset = align_down(bits / 8, sz);
      mem->bit_offset = bits % (sz * 8);
      bits += mem->bit_width;
    } else {
      if (!ty->is_packed)
        bits = C_align_to(bits, mem->align * 8);
      mem->offset = bits / 8;
      bits += mem->ty->size * 8;
    }

    if (!ty->is_packed && ty->align < mem->align)
      ty->align = mem->align;
  }

  ty->size = C_align_to(bits, ty->align * 8) / 8;
  return ty;
}

// union-decl = struct-union-decl
static C_Type *union_decl(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Type *ty = struct_union_decl(parser, rest, tok);
  ty->kind = TY_UNION;

  if (ty->size < 0)
    return ty;

  // If union, we don't have to assign offsets because they
  // are already initialized to zero. We need to compute the
  // alignment and the size though.
  for (C_Member *mem = ty->members; mem; mem = mem->next) {
    if (ty->align < mem->align)
      ty->align = mem->align;
    if (ty->size < mem->ty->size)
      ty->size = mem->ty->size;
  }
  ty->size = C_align_to(ty->size, ty->align);
  return ty;
}

// Find a struct member by name.
static C_Member *get_struct_member(C_Parser *parser, C_Type *ty, C_Token *tok) {
  for (C_Member *mem = ty->members; mem; mem = mem->next) {
    // Anonymous struct member
    if ((mem->ty->kind == TY_STRUCT || mem->ty->kind == TY_UNION) &&
        !mem->name) {
      if (get_struct_member(parser, mem->ty, tok))
        return mem;
      continue;
    }

    // Regular struct member
    if (mem->name->len == tok->len &&
        !strncmp(mem->name->loc, tok->loc, tok->len))
      return mem;
  }
  return NULL;
}

// Create a node representing a struct member access, such as foo.bar
// where foo is a struct and bar is a member name.
//
// C has a feature called "anonymous struct" which allows a struct to
// have another unnamed struct as a member like this:
//
//   struct { struct { int a; }; int b; } x;
//
// The members of an anonymous struct belong to the outer struct's
// member namespace. Therefore, in the above example, you can access
// member "a" of the anonymous struct as "x.a".
//
// This function takes care of anonymous structs.
static C_Node *struct_ref(C_Parser *parser, C_Node *node, C_Token *tok) {
  C_add_type(parser, node);
  if (node->ty->kind != TY_STRUCT && node->ty->kind != TY_UNION)
    C_error_tok(parser, node->tok, "not a struct nor a union");

  C_Type *ty = node->ty;

  for (;;) {
    C_Member *mem = get_struct_member(parser, ty, tok);
    if (!mem)
      C_error_tok(parser, tok, "no such member");
    node = new_unary(parser, ND_MEMBER, node, tok);
    node->member = mem;
    if (mem->name)
      break;
    ty = mem->ty;
  }
  return node;
}

// Convert A++ to `(typeof A)((A += 1) - 1)`
static C_Node *new_inc_dec(C_Parser *parser, C_Node *node, C_Token *tok, int addend) {
  C_add_type(parser, node);
  return C_new_cast(parser,
                    new_add(parser, to_assign(parser, new_add(parser, node, new_num(parser, addend, tok), tok)),
                            new_num(parser, -addend, tok), tok),
                    node->ty);
}

// postfix = "(" type-name ")" "{" initializer-list "}"
//         = ident "(" func-args ")" postfix-tail*
//         | primary postfix-tail*
//
// postfix-tail = "[" expr "]"
//              | "(" func-args ")"
//              | "." ident
//              | "->" ident
//              | "++"
//              | "--"
static C_Node *postfix(C_Parser *parser, C_Token **rest, C_Token *tok) {
  if (C_equal(tok, "(") && is_typename(parser, tok->next)) {
    // Compound literal
    C_Token *start = tok;
    C_Type *ty = typename(parser, &tok, tok->next);
    tok = C_skip(parser, tok, ")");

    if (parser->scope->next == NULL) {
      C_Obj *var = new_anon_gvar(parser, ty);
      gvar_initializer(parser, rest, tok, var);
      return new_var_node(parser,var, start);
    }

    C_Obj *var = new_lvar(parser, "", ty);
    C_Node *lhs = lvar_initializer(parser, rest, tok, var);
    C_Node *rhs = new_var_node(parser, var, tok);
    return new_binary(parser, ND_COMMA, lhs, rhs, start);
  }

  C_Node *node = primary(parser, &tok, tok);

  for (;;) {
    if (C_equal(tok, "(")) {
      node = funcall(parser, &tok, tok->next, node);
      continue;
    }

    if (C_equal(tok, "[")) {
      // x[y] is short for *(x+y)
      C_Token *start = tok;
      C_Node *idx = expr(parser, &tok, tok->next);
      tok = C_skip(parser, tok, "]");
      node = new_unary(parser, ND_DEREF, new_add(parser, node, idx, start), start);
      continue;
    }

    if (C_equal(tok, ".")) {
      node = struct_ref(parser, node, tok->next);
      tok = tok->next->next;
      continue;
    }

    if (C_equal(tok, "->")) {
      // x->y is short for (*x).y
      node = new_unary(parser, ND_DEREF, node, tok);
      node = struct_ref(parser, node, tok->next);
      tok = tok->next->next;
      continue;
    }

    if (C_equal(tok, "++")) {
      node = new_inc_dec(parser, node, tok, 1);
      tok = tok->next;
      continue;
    }

    if (C_equal(tok, "--")) {
      node = new_inc_dec(parser, node, tok, -1);
      tok = tok->next;
      continue;
    }

    *rest = tok;
    return node;
  }
}

// funcall = (assign ("," assign)*)? ")"
static C_Node *funcall(C_Parser *parser, C_Token **rest, C_Token *tok, C_Node *fn) {
  C_add_type(parser, fn);

  if (fn->ty->kind != TY_FUNC &&
      (fn->ty->kind != TY_PTR || fn->ty->base->kind != TY_FUNC))
    C_error_tok(parser, fn->tok, "not a function");

  C_Type *ty = (fn->ty->kind == TY_FUNC) ? fn->ty : fn->ty->base;
  C_Type *param_ty = ty->params;

  C_Node head = {0};
  C_Node *cur = &head;

  while (!C_equal(tok, ")")) {
    if (cur != &head)
      tok = C_skip(parser, tok, ",");

    C_Node *arg = assign(parser, &tok, tok);
    C_add_type(parser, arg);

    if (!param_ty && !ty->is_variadic)
      C_error_tok(parser, tok, "too many arguments");

    if (param_ty) {
      if (param_ty->kind != TY_STRUCT && param_ty->kind != TY_UNION)
        arg = C_new_cast(parser, arg, param_ty);
      param_ty = param_ty->next;
    } else if (arg->ty->kind == TY_FLOAT) {
      // If parameter type is omitted (e.g. in "..."), float
      // arguments are promoted to double.
      arg = C_new_cast(parser, arg, C_ty_double);
    }

    cur = cur->next = arg;
  }

  if (param_ty)
    C_error_tok(parser, tok, "too few arguments");

  *rest = C_skip(parser, tok, ")");

  C_Node *node = new_unary(parser, ND_FUNCALL, fn, tok);
  node->func_ty = ty;
  node->ty = ty->return_ty;
  node->args = head.next;

  // If a function returns a struct, it is caller's responsibility
  // to allocate a space for the return value.
  if (node->ty->kind == TY_STRUCT || node->ty->kind == TY_UNION)
    node->ret_buffer = new_lvar(parser, "", node->ty);
  return node;
}

// generic-selection = "(" assign "," generic-assoc ("," generic-assoc)* ")"
//
// generic-assoc = type-name ":" assign
//               | "default" ":" assign
static C_Node *generic_selection(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Token *start = tok;
  tok = C_skip(parser, tok, "(");

  C_Node *ctrl = assign(parser, &tok, tok);
  C_add_type(parser, ctrl);

  C_Type *t1 = ctrl->ty;
  if (t1->kind == TY_FUNC)
    t1 = C_pointer_to(parser, t1);
  else if (t1->kind == TY_ARRAY)
    t1 = C_pointer_to(parser, t1->base);

  C_Node *ret = NULL;

  while (!C_consume(rest, tok, ")")) {
    tok = C_skip(parser, tok, ",");

    if (C_equal(tok, "default")) {
      tok = C_skip(parser, tok->next, ":");
      C_Node *node = assign(parser, &tok, tok);
      if (!ret)
        ret = node;
      continue;
    }

    C_Type *t2 = typename(parser, &tok, tok);
    tok = C_skip(parser, tok, ":");
    C_Node *node = assign(parser, &tok, tok);
    if (C_is_compatible(t1, t2))
      ret = node;
  }

  if (!ret)
    C_error_tok(parser, start,
                "controlling expression type not compatible with"
                " any generic association type");
  return ret;
}

// primary = "(" "{" stmt+ "}" ")"
//         | "(" expr ")"
//         | "sizeof" "(" type-name ")"
//         | "sizeof" unary
//         | "_Alignof" "(" type-name ")"
//         | "_Alignof" unary
//         | "_Generic" generic-selection
//         | "__builtin_types_compatible_p" "(" type-name, type-name, ")"
//         | "__builtin_reg_class" "(" type-name ")"
//         | ident
//         | str
//         | num
static C_Node *primary(C_Parser *parser, C_Token **rest, C_Token *tok) {
  C_Token *start = tok;

  if (C_equal(tok, "(") && C_equal(tok->next, "{")) {
    // This is a GNU statement expresssion.
    C_Node *node = new_node(parser, ND_STMT_EXPR, tok);
    node->body = compound_stmt(parser, &tok, tok->next->next)->body;
    *rest = C_skip(parser, tok, ")");
    return node;
  }

  if (C_equal(tok, "(")) {
    C_Node *node = expr(parser, &tok, tok->next);
    *rest = C_skip(parser, tok, ")");
    return node;
  }

  if (C_equal(tok, "sizeof") && C_equal(tok->next, "(") && is_typename(parser, tok->next->next)) {
    C_Type *ty = typename(parser, &tok, tok->next->next);
    *rest = C_skip(parser, tok, ")");

    if (ty->kind == TY_VLA) {
      if (ty->vla_size)
        return new_var_node(parser, ty->vla_size, tok);

      C_Node *lhs = compute_vla_size(parser, ty, tok);
      C_Node *rhs = new_var_node(parser, ty->vla_size, tok);
      return new_binary(parser, ND_COMMA, lhs, rhs, tok);
    }

    return new_ulong(parser, ty->size, start);
  }

  if (C_equal(tok, "sizeof")) {
    C_Node *node = unary(parser, rest, tok->next);
    C_add_type(parser, node);
    if (node->ty->kind == TY_VLA)
      return new_var_node(parser, node->ty->vla_size, tok);
    return new_ulong(parser, node->ty->size, tok);
  }

  if (C_equal(tok, "_Alignof") && C_equal(tok->next, "(") && is_typename(parser, tok->next->next)) {
    C_Type *ty = typename(parser, &tok, tok->next->next);
    *rest = C_skip(parser, tok, ")");
    return new_ulong(parser, ty->align, tok);
  }

  if (C_equal(tok, "_Alignof")) {
    C_Node *node = unary(parser, rest, tok->next);
    C_add_type(parser, node);
    return new_ulong(parser, node->ty->align, tok);
  }

  if (C_equal(tok, "_Generic"))
    return generic_selection(parser, rest, tok->next);

  if (C_equal(tok, "__builtin_types_compatible_p")) {
    tok = C_skip(parser, tok->next, "(");
    C_Type *t1 = typename(parser, &tok, tok);
    tok = C_skip(parser, tok, ",");
    C_Type *t2 = typename(parser, &tok, tok);
    *rest = C_skip(parser, tok, ")");
    return new_num(parser, C_is_compatible(t1, t2), start);
  }

  if (C_equal(tok, "__builtin_reg_class")) {
    tok = C_skip(parser, tok->next, "(");
    C_Type *ty = typename(parser, &tok, tok);
    *rest = C_skip(parser, tok, ")");

    if (C_is_integer(ty) || ty->kind == TY_PTR)
      return new_num(parser, 0, start);
    if (C_is_flonum(ty))
      return new_num(parser, 1, start);
    return new_num(parser, 2, start);
  }

  if (C_equal(tok, "__builtin_compare_and_swap")) {
    C_Node *node = new_node(parser, ND_CAS, tok);
    tok = C_skip(parser, tok->next, "(");
    node->cas_addr = assign(parser, &tok, tok);
    tok = C_skip(parser, tok, ",");
    node->cas_old = assign(parser, &tok, tok);
    tok = C_skip(parser, tok, ",");
    node->cas_new = assign(parser, &tok, tok);
    *rest = C_skip(parser, tok, ")");
    return node;
  }

  if (C_equal(tok, "__builtin_atomic_exchange")) {
    C_Node *node = new_node(parser, ND_EXCH, tok);
    tok = C_skip(parser, tok->next, "(");
    node->lhs = assign(parser, &tok, tok);
    tok = C_skip(parser, tok, ",");
    node->rhs = assign(parser, &tok, tok);
    *rest = C_skip(parser, tok, ")");
    return node;
  }

  if (tok->kind == TK_IDENT) {
    // Variable or enum constant
    C_VarScope *sc = find_var(parser, tok);
    *rest = tok->next;

    // For "static inline" function
    if (sc && sc->var && sc->var->is_function) {
      if (parser->current_fn)
        strarray_push(parser->memory_allocator, &parser->current_fn->refs, sc->var->name);
      else
        sc->var->is_root = true;
    }

    if (sc) {
      if (sc->var)
        return new_var_node(parser, sc->var, tok);
      if (sc->enum_ty)
        return new_num(parser, sc->enum_val, tok);
    }

    if (C_equal(tok->next, "("))
      C_error_tok(parser, tok, "implicit declaration of a function");
    C_error_tok(parser, tok, "undefined variable");
  }

  if (tok->kind == TK_STR) {
    C_Obj *var = new_string_literal(parser, tok->str, tok->ty);
    *rest = tok->next;
    return new_var_node(parser, var, tok);
  }

  if (tok->kind == TK_NUM) {
    C_Node *node;
    if (C_is_flonum(tok->ty)) {
      node = new_node(parser, ND_NUM, tok);
      node->fval = tok->fval;
    } else {
      node = new_num(parser, tok->val, tok);
    }

    node->ty = tok->ty;
    *rest = tok->next;
    return node;
  }

  C_error_tok(parser, tok, "expected an expression");
}

static C_Token *parse_typedef(C_Parser *parser, C_Token *tok, C_Type *basety) {
  bool first = true;

  while (!C_consume(&tok, tok, ";")) {
    if (!first)
      tok = C_skip(parser, tok, ",");
    first = false;

    C_Type *ty = declarator(parser, &tok, tok, basety);
    if (!ty->name)
      C_error_tok(parser, ty->name_pos, "typedef name omitted");
    push_scope(parser, get_ident(parser, ty->name))->type_def = ty;
  }
  return tok;
}

static void create_param_lvars(C_Parser *parser, C_Type *param) {
  if (param) {
    create_param_lvars(parser, param->next);
    if (!param->name)
      C_error_tok(parser, param->name_pos, "parameter name omitted");
    new_lvar(parser, get_ident(parser, param->name), param);
  }
}

// This function matches gotos or labels-as-values with labels.
//
// We cannot resolve gotos as we C_parse a function because gotos
// can refer a label that appears later in the function.
// So, we need to do this after we C_parse the entire function.
static void resolve_goto_labels(C_Parser *parser) {
  for (C_Node *x = parser->gotos; x; x = x->goto_next) {
    for (C_Node *y = parser->labels; y; y = y->goto_next) {
      if (!strcmp(x->label, y->label)) {
        x->unique_label = y->unique_label;
        break;
      }
    }

    if (x->unique_label == NULL)
      C_error_tok(parser, x->tok->next, "use of undeclared label");
  }

  parser->gotos = parser->labels = NULL;
}

static C_Obj *find_func(C_Parser *parser, char *name) {
  C_Scope *sc = parser->scope;
  while (sc->next)
    sc = sc->next;

  C_VarScope *sc2 = hashmap_get(&sc->vars, name);
  if (sc2 && sc2->var && sc2->var->is_function)
    return sc2->var;
  return NULL;
}

static void mark_live(C_Parser *parser, C_Obj *var) {
  if (!var->is_function || var->is_live)
    return;
  var->is_live = true;

  for (int i = 0; i < var->refs.len; i++) {
    C_Obj *fn = find_func(parser, var->refs.data[i]);
    if (fn)
      mark_live(parser, fn);
  }
}

static C_Token *function(C_Parser *parser, C_Token *tok, C_Type *basety, VarAttr *attr) {
  C_Type *ty = declarator(parser, &tok, tok, basety);
  if (!ty->name)
    C_error_tok(parser, ty->name_pos, "function name omitted");
  char *name_str = get_ident(parser, ty->name);

  C_Obj *fn = find_func(parser, name_str);
  if (fn) {
    // Redeclaration
    if (!fn->is_function)
      C_error_tok(parser, tok, "redeclared as a different kind of symbol");
    if (fn->is_definition && C_equal(tok, "{"))
      C_error_tok(parser, tok, "redefinition of %s", name_str);
    if (!fn->is_static && attr->is_static)
      C_error_tok(parser, tok, "static declaration follows a non-static declaration");
    fn->is_definition = fn->is_definition || C_equal(tok, "{");
  } else {
    fn = new_gvar(parser, name_str, ty);
    fn->is_function = true;
    fn->is_definition = C_equal(tok, "{");
    fn->is_static = attr->is_static || (attr->is_inline && !attr->is_extern);
    fn->is_inline = attr->is_inline;
  }

  fn->is_root = !(fn->is_static && fn->is_inline);

  if (C_consume(&tok, tok, ";"))
    return tok;

  parser->current_fn = fn;
  parser->locals = NULL;
  enter_scope(parser);
  create_param_lvars(parser, ty->params);

  // A buffer for a struct/union return value is passed
  // as the hidden first parameter.
  C_Type *rty = ty->return_ty;
  if ((rty->kind == TY_STRUCT || rty->kind == TY_UNION) && rty->size > 16)
    new_lvar(parser, "", C_pointer_to(parser, rty));

  fn->params = parser->locals;

  if (ty->is_variadic)
    fn->va_area = new_lvar(parser, "__va_area__", C_array_of(parser, C_ty_char, 136));
  fn->alloca_bottom = new_lvar(parser, "__alloca_size__", C_pointer_to(parser, C_ty_char));

  tok = C_skip(parser, tok, "{");

  // [https://www.sigbus.info/n1570#6.4.2.2p1] "__func__" is
  // automatically defined as a local variable containing the
  // current function name.
  push_scope(parser, "__func__")->var =
    new_string_literal(parser, fn->name, C_array_of(parser, C_ty_char, strlen(fn->name) + 1));

  // [GNU] __FUNCTION__ is yet another name of __func__.
  push_scope(parser, "__FUNCTION__")->var =
    new_string_literal(parser, fn->name, C_array_of(parser, C_ty_char, strlen(fn->name) + 1));

  fn->body = compound_stmt(parser, &tok, tok);
  fn->locals = parser->locals;
  leave_scope(parser);
  resolve_goto_labels(parser);
  return tok;
}

static C_Token *global_variable(C_Parser *parser, C_Token *tok, C_Type *basety, VarAttr *attr) {
  bool first = true;

  while (!C_consume(&tok, tok, ";")) {
    if (!first)
      tok = C_skip(parser, tok, ",");
    first = false;

    C_Type *ty = declarator(parser, &tok, tok, basety);
    if (!ty->name)
      C_error_tok(parser, ty->name_pos, "variable name omitted");

    C_Obj *var = new_gvar(parser, get_ident(parser, ty->name), ty);
    var->is_definition = !attr->is_extern;
    var->is_static = attr->is_static;
    var->is_tls = attr->is_tls;
    if (attr->align)
      var->align = attr->align;

    if (C_equal(tok, "="))
      gvar_initializer(parser, &tok, tok->next, var);
    else if (!attr->is_extern && !attr->is_tls)
      var->is_tentative = true;
  }
  return tok;
}

// Lookahead tokens and returns true if a given token is a start
// of a function definition or declaration.
static bool is_function(C_Parser *parser, C_Token *tok) {
  if (C_equal(tok, ";"))
    return false;

  C_Type dummy = {0};
  C_Type *ty = declarator(parser, &tok, tok, &dummy);
  return ty->kind == TY_FUNC;
}

// Remove redundant tentative definitions.
static void scan_globals(C_Parser *parser) {
  C_Obj head;
  C_Obj *cur = &head;

  for (C_Obj *var = parser->globals; var; var = var->next) {
    if (!var->is_tentative) {
      cur = cur->next = var;
      continue;
    }

    // Find another definition of the same identifier.
    C_Obj *var2 = parser->globals;
    for (; var2; var2 = var2->next)
      if (var != var2 && var2->is_definition && !strcmp(var->name, var2->name))
        break;

    // If there's another definition, the tentative definition
    // is redundant
    if (!var2)
      cur = cur->next = var;
  }

  cur->next = NULL;
  parser->globals = head.next;
}

static void declare_builtin_functions(C_Parser *parser) {
  C_Type *ty = C_func_type(parser, C_pointer_to(parser, C_ty_void));
  ty->params = C_copy_type(parser, C_ty_int);
  parser->builtin_alloca = new_gvar(parser, "alloca", ty);
  parser->builtin_alloca->is_definition = false;
}

#ifdef RAVI_EXTENSIONS
C_Obj *C_create_function(C_Scope *globalScope, C_Parser *parser, char *name_str) {
  C_Obj *fn = new_gvar(parser, name_str, C_func_type(parser, C_ty_void));
  fn->is_function = true;
  fn->is_definition = true;
  fn->is_static = true;
  fn->is_inline = false;
  parser->current_fn = fn;
  parser->locals = NULL;
  parser->gotos = NULL;
  parser->labels = NULL;
  parser->brk_label = NULL;
  parser->cont_label = NULL;
  parser->current_switch = NULL;
  return fn;
}

C_Node *C_parse_compound_statement(C_Scope *globalScope, C_Parser *parser, C_Token *tok) {
  if (setjmp(parser->env) != 0) {
    return NULL;
  }
  parser->scope = globalScope;
  return compound_stmt(parser, &tok, tok);
}
#endif

// program = (typedef | function-definition | global-variable)*
C_Obj *C_parse(C_Scope *globalScope, C_Parser *parser, C_Token *tok) {
  if (setjmp(parser->env) != 0) {
    return NULL;
  }

  parser->scope = globalScope;

  declare_builtin_functions(parser);
  parser->globals = NULL;

  while (tok->kind != TK_EOF) {
    VarAttr attr = {0};
    C_Type *basety = declspec(parser, &tok, tok, &attr);

    // Typedef
    if (attr.is_typedef) {
      tok = parse_typedef(parser, tok, basety);
      continue;
    }

    // Function
    if (is_function(parser, tok)) {
      tok = function(parser, tok, basety, &attr);
      continue;
    }

    // Global variable
    tok = global_variable(parser, tok, basety, &attr);
  }

  for (C_Obj *var = parser->globals; var; var = var->next)
    if (var->is_root)
      mark_live(parser, var);

  // Remove redundant tentative definitions.
  scan_globals(parser);
  return parser->globals;
}

void C_parser_init(C_Parser *parser, C_MemoryAllocator *allocator) {
  memset(parser, 0, sizeof *parser);
  parser->memory_allocator = allocator;
  parser->keywords.allocator = allocator;
  parser->typewords.allocator = allocator;
}

void C_parser_destroy(C_Parser *parser) {
  parser->memory_allocator = NULL;
}
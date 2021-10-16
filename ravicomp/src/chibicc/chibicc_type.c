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

#include "chibicc.h"

C_Type *C_ty_void = &(C_Type){TY_VOID, 1, 1};
C_Type *C_ty_bool = &(C_Type){TY_BOOL, 1, 1};

C_Type *C_ty_char = &(C_Type){TY_CHAR, 1, 1};
C_Type *C_ty_short = &(C_Type){TY_SHORT, 2, 2};
C_Type *C_ty_int = &(C_Type){TY_INT, 4, 4};
C_Type *C_ty_long = &(C_Type){TY_LONG, 8, 8};

C_Type *C_ty_uchar = &(C_Type){TY_CHAR, 1, 1, true};
C_Type *C_ty_ushort = &(C_Type){TY_SHORT, 2, 2, true};
C_Type *C_ty_uint = &(C_Type){TY_INT, 4, 4, true};
C_Type *C_ty_ulong = &(C_Type){TY_LONG, 8, 8, true};

C_Type *C_ty_float = &(C_Type){TY_FLOAT, 4, 4};
C_Type *C_ty_double = &(C_Type){TY_DOUBLE, 8, 8};
C_Type *C_ty_ldouble = &(C_Type){TY_LDOUBLE, 16, 16};

static C_Type *new_type(C_Parser *parser, TypeKind kind, int size, int align) {
  C_Type *ty = mspace_calloc(parser->arena, 1, sizeof(C_Type));
  ty->kind = kind;
  ty->size = size;
  ty->align = align;
  return ty;
}

bool C_is_integer(C_Type *ty) {
  TypeKind k = ty->kind;
  return k == TY_BOOL || k == TY_CHAR || k == TY_SHORT ||
         k == TY_INT  || k == TY_LONG || k == TY_ENUM;
}

bool C_is_flonum(C_Type *ty) {
  return ty->kind == TY_FLOAT || ty->kind == TY_DOUBLE ||
         ty->kind == TY_LDOUBLE;
}

bool C_is_numeric(C_Type *ty) {
  return C_is_integer(ty) || C_is_flonum(ty);
}

bool C_is_compatible(C_Type *t1, C_Type *t2) {
  if (t1 == t2)
    return true;

  if (t1->origin)
    return C_is_compatible(t1->origin, t2);

  if (t2->origin)
    return C_is_compatible(t1, t2->origin);

  if (t1->kind != t2->kind)
    return false;

  switch (t1->kind) {
  case TY_CHAR:
  case TY_SHORT:
  case TY_INT:
  case TY_LONG:
    return t1->is_unsigned == t2->is_unsigned;
  case TY_FLOAT:
  case TY_DOUBLE:
  case TY_LDOUBLE:
    return true;
  case TY_PTR:
    return C_is_compatible(t1->base, t2->base);
  case TY_FUNC: {
    if (!C_is_compatible(t1->return_ty, t2->return_ty))
      return false;
    if (t1->is_variadic != t2->is_variadic)
      return false;

    C_Type *p1 = t1->params;
    C_Type *p2 = t2->params;
    for (; p1 && p2; p1 = p1->next, p2 = p2->next)
      if (!C_is_compatible(p1, p2))
        return false;
    return p1 == NULL && p2 == NULL;
  }
  case TY_ARRAY:
    if (!C_is_compatible(t1->base, t2->base))
      return false;
    return t1->array_len < 0 && t2->array_len < 0 &&
           t1->array_len == t2->array_len;
  }
  return false;
}

C_Type *C_copy_type(C_Parser *parser, C_Type *ty) {
  C_Type *ret = mspace_calloc(parser->arena, 1, sizeof(C_Type));
  *ret = *ty;
  ret->origin = ty;
  return ret;
}

C_Type *C_pointer_to(C_Parser *parser, C_Type *base) {
  C_Type *ty = new_type(parser, TY_PTR, 8, 8);
  ty->base = base;
  ty->is_unsigned = true;
  return ty;
}

C_Type *C_func_type(C_Parser *parser, C_Type *return_ty) {
  // The C spec disallows sizeof(<function type>), but
  // GCC allows that and the expression is evaluated to 1.
  C_Type *ty = new_type(parser, TY_FUNC, 1, 1);
  ty->return_ty = return_ty;
  return ty;
}

C_Type *C_array_of(C_Parser *parser, C_Type *base, int size) {
  C_Type *ty = new_type(parser, TY_ARRAY, base->size * size, base->align);
  ty->base = base;
  ty->array_len = size;
  return ty;
}

C_Type *C_vla_of(C_Parser *parser, C_Type *base, C_Node *expr) {
  C_Type *ty = new_type(parser, TY_VLA, 8, 8);
  ty->base = base;
  ty->vla_len = expr;
  return ty;
}

C_Type *C_enum_type(C_Parser *parser) {
  return new_type(parser, TY_ENUM, 4, 4);
}

C_Type *C_struct_type(C_Parser *parser) {
  return new_type(parser, TY_STRUCT, 0, 1);
}

static C_Type *get_common_type(C_Parser *parser, C_Type *ty1, C_Type *ty2) {
  if (ty1->base)
    return C_pointer_to(parser, ty1->base);

  if (ty1->kind == TY_FUNC)
    return C_pointer_to(parser, ty1);
  if (ty2->kind == TY_FUNC)
    return C_pointer_to(parser, ty2);

  if (ty1->kind == TY_LDOUBLE || ty2->kind == TY_LDOUBLE)
    return C_ty_ldouble;
  if (ty1->kind == TY_DOUBLE || ty2->kind == TY_DOUBLE)
    return C_ty_double;
  if (ty1->kind == TY_FLOAT || ty2->kind == TY_FLOAT)
    return C_ty_float;

  if (ty1->size < 4)
    ty1 = C_ty_int;
  if (ty2->size < 4)
    ty2 = C_ty_int;

  if (ty1->size != ty2->size)
    return (ty1->size < ty2->size) ? ty2 : ty1;

  if (ty2->is_unsigned)
    return ty2;
  return ty1;
}

// For many binary operators, we implicitly promote operands so that
// both operands have the same type. Any integral type smaller than
// int is always promoted to int. If the type of one operand is larger
// than the other's (e.g. "long" vs. "int"), the smaller operand will
// be promoted to match with the other.
//
// This operation is called the "usual arithmetic conversion".
static void usual_arith_conv(C_Parser *parser, C_Node **lhs, C_Node **rhs) {
  C_Type *ty = get_common_type(parser, (*lhs)->ty, (*rhs)->ty);
  *lhs = C_new_cast(parser, *lhs, ty);
  *rhs = C_new_cast(parser, *rhs, ty);
}

void C_add_type(C_Parser *parser, C_Node *node) {
  if (!node || node->ty)
    return;

  C_add_type(parser, node->lhs);
  C_add_type(parser, node->rhs);
  C_add_type(parser, node->cond);
  C_add_type(parser, node->then);
  C_add_type(parser, node->els);
  C_add_type(parser, node->init);
  C_add_type(parser, node->inc);

  for (C_Node *n = node->body; n; n = n->next)
    C_add_type(parser, n);
  for (C_Node *n = node->args; n; n = n->next)
    C_add_type(parser, n);

  switch (node->kind) {
  case ND_NUM:
    node->ty = C_ty_int;
    return;
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_MOD:
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
    usual_arith_conv(parser, &node->lhs, &node->rhs);
    node->ty = node->lhs->ty;
    return;
  case ND_NEG: {
    C_Type *ty = get_common_type(parser, C_ty_int, node->lhs->ty);
    node->lhs = C_new_cast(parser, node->lhs, ty);
    node->ty = ty;
    return;
  }
  case ND_ASSIGN:
    if (node->lhs->ty->kind == TY_ARRAY)
      C_error_tok(parser, node->lhs->tok, "not an lvalue");
    if (node->lhs->ty->kind != TY_STRUCT)
      node->rhs = C_new_cast(parser, node->rhs, node->lhs->ty);
    node->ty = node->lhs->ty;
    return;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
    usual_arith_conv(parser, &node->lhs, &node->rhs);
    node->ty = C_ty_int;
    return;
  case ND_FUNCALL:
    node->ty = node->func_ty->return_ty;
    return;
  case ND_NOT:
  case ND_LOGOR:
  case ND_LOGAND:
    node->ty = C_ty_int;
    return;
  case ND_BITNOT:
  case ND_SHL:
  case ND_SHR:
    node->ty = node->lhs->ty;
    return;
  case ND_VAR:
  case ND_VLA_PTR:
    node->ty = node->var->ty;
    return;
  case ND_COND:
    if (node->then->ty->kind == TY_VOID || node->els->ty->kind == TY_VOID) {
      node->ty = C_ty_void;
    } else {
      usual_arith_conv(parser, &node->then, &node->els);
      node->ty = node->then->ty;
    }
    return;
  case ND_COMMA:
    node->ty = node->rhs->ty;
    return;
  case ND_MEMBER:
    node->ty = node->member->ty;
    return;
  case ND_ADDR: {
    C_Type *ty = node->lhs->ty;
    if (ty->kind == TY_ARRAY)
      node->ty = C_pointer_to(parser, ty->base);
    else
      node->ty = C_pointer_to(parser, ty);
    return;
  }
  case ND_DEREF:
    if (!node->lhs->ty->base)
      C_error_tok(parser, node->tok, "invalid pointer dereference");
    if (node->lhs->ty->base->kind == TY_VOID)
      C_error_tok(parser, node->tok, "dereferencing a void pointer");

    node->ty = node->lhs->ty->base;
    return;
  case ND_STMT_EXPR:
    if (node->body) {
      C_Node *stmt = node->body;
      while (stmt->next)
        stmt = stmt->next;
      if (stmt->kind == ND_EXPR_STMT) {
        node->ty = stmt->lhs->ty;
        return;
      }
    }
    C_error_tok(parser, node->tok, "statement expression returning void is not supported");
    return;
  case ND_LABEL_VAL:
    node->ty = C_pointer_to(parser, C_ty_void);
    return;
  case ND_CAS:
    C_add_type(parser, node->cas_addr);
    C_add_type(parser, node->cas_old);
    C_add_type(parser, node->cas_new);
    node->ty = C_ty_bool;

    if (node->cas_addr->ty->kind != TY_PTR)
      C_error_tok(parser, node->cas_addr->tok, "pointer expected");
    if (node->cas_old->ty->kind != TY_PTR)
      C_error_tok(parser, node->cas_old->tok, "pointer expected");
    return;
  case ND_EXCH:
    if (node->lhs->ty->kind != TY_PTR)
      C_error_tok(parser, node->cas_addr->tok, "pointer expected");
    node->ty = node->lhs->ty->base;
    return;
  }
}

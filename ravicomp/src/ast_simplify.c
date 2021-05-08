/******************************************************************************
 * Copyright (C) 2018-2021 Dibyendu Majumdar
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
/* Replace constant expressions with constants, and simply any other expressions if possible */

#include <parser.h>

#include <math.h>
#include <string.h>

static void process_expression_list(CompilerState *container, AstNodeList *node);
static void process_statement_list(CompilerState *container, AstNodeList *node);
static void process_statement(CompilerState *container, AstNode *node);

#define l_mathop(op) op

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
/* number of bits in an integer */
#define NBITS ((int)(sizeof(lua_Integer) * CHAR_BIT))
/*
@@ lua_numbertointeger converts a float number with an integral value
** to an integer, or returns 0 if float is not within the range of
** a lua_Integer.  (The range comparisons are tricky because of
** rounding. The tests here assume a two-complement representation,
** where MININTEGER always has an exact representation as a float;
** MAXINTEGER may not have one, and therefore its conversion to float
** may have an ill-defined value.)
*/
#define lua_numbertointeger(n, p)                                                                                      \
	((n) >= (lua_Number)(LUA_MININTEGER) && (n) < -(lua_Number)(LUA_MININTEGER) && (*(p) = (lua_Integer)(n), 1))

/*
** Rounding modes for float->integer coercion
*/
typedef enum {
	F2Ieq,	  /* no rounding; accepts only integral values */
	F2Ifloor, /* takes the floor of the number */
	F2Iceil	  /* takes the ceil of the number */
} F2Imod;

#if !defined(LUA_FLOORN2I)
#define LUA_FLOORN2I F2Ieq
#endif

/*
** The luai_num* macros define the primitive operations over numbers.
*/

/* floor division (defined as 'floor(a/b)') */
#if !defined(luai_numidiv)
#define luai_numidiv(a, b) (l_floor(luai_numdiv(a, b)))
#endif

/* float division */
#if !defined(luai_numdiv)
#define luai_numdiv(a, b) ((a) / (b))
#endif

/*
** modulo: defined as 'a - floor(a/b)*b'; this definition gives NaN when
** 'b' is huge, but the result should be 'a'. 'fmod' gives the result of
** 'a - trunc(a/b)*b', and therefore must be corrected when 'trunc(a/b)
** ~= floor(a/b)'. That happens when the division has a non-integer
** negative result, which is equivalent to the test below.
*/
#if !defined(luai_nummod)
#define luai_nummod(a, b, m)                                                                                           \
	{                                                                                                              \
		(m) = l_mathop(fmod)(a, b);                                                                            \
		if ((m) * (b) < 0)                                                                                     \
			(m) += (b);                                                                                    \
	}
#endif
#define l_floor(x) (l_mathop(floor)(x))

/* exponentiation */
#if !defined(luai_numpow)
#define luai_numpow(a, b) (l_mathop(pow)(a, b))
#endif

/* the others are quite standard operations */
#if !defined(luai_numadd)
#define luai_numadd(a, b) ((a) + (b))
#define luai_numsub(a, b) ((a) - (b))
#define luai_nummul(a, b) ((a) * (b))
#define luai_numunm(a) (-(a))
#define luai_numeq(a, b) ((a) == (b))
#define luai_numlt(a, b) ((a) < (b))
#define luai_numle(a, b) ((a) <= (b))
#define luai_numisnan(a) (!luai_numeq((a), (a)))
#endif

/* cast a signed lua_Integer to lua_Unsigned */
#if !defined(l_castS2U)
#define l_castS2U(i) ((lua_Unsigned)(i))
#endif

/*
** cast a lua_Unsigned to a signed lua_Integer; this cast is
** not strict ISO C, but two-complement architectures should
** work fine.
*/
#if !defined(l_castU2S)
#define l_castU2S(i) ((lua_Integer)(i))
#endif

/*
** macros to improve jump prediction (used mainly for error handling)
*/
#if !defined(likely)

#if defined(__GNUC__)
#define likely(x) (__builtin_expect(((x) != 0), 1))
#define unlikely(x) (__builtin_expect(((x) != 0), 0))
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#endif

#define cast(t, exp) ((t)(exp))
#define cast_num(i) cast(lua_Number, (i))
#define cast_int(i) cast(int, (i))

#define ttisfloat(o) (o->type.type_code == RAVI_TNUMFLT)
#define ttisinteger(o) (o->type.type_code == RAVI_TNUMINT)
#define fltvalue(o) (o->u.r)
#define ivalue(o) (o->u.i)
#define setivalue(o, v) (o->type.type_code = RAVI_TNUMINT, o->u.i = (v))
#define setfltvalue(o, v) (o->type.type_code = RAVI_TNUMFLT, o->u.r = (v))

/* convert an object to a float (without string coercion) */
#define tonumberns(o, n) (ttisfloat(o) ? ((n) = fltvalue(o), 1) : (ttisinteger(o) ? ((n) = cast_num(ivalue(o)), 1) : 0))

/* convert an object to an integer (including string coercion) */
#define tointeger(o, i) (RAVI_LIKELY(ttisinteger(o)) ? (*(i) = ivalue(o), 1) : luaV_tointeger(o, i, LUA_FLOORN2I))

/* convert an object to an integer (without string coercion) */
#define tointegerns(o, i) (ttisinteger(o) ? (*(i) = ivalue(o), 1) : luaV_tointegerns(o, i, LUA_FLOORN2I))

static void handle_error(CompilerState *container, const char *msg)
{
	// TODO source and line number
	raviX_buffer_add_string(&container->error_message, msg);
	longjmp(container->env, 1);
}

/*
** Integer division; return 'm // n', that is, floor(m/n).
** C division truncates its result (rounds towards zero).
** 'floor(q) == trunc(q)' when 'q >= 0' or when 'q' is integer,
** otherwise 'floor(q) == trunc(q) - 1'.
*/
static lua_Integer luaV_idiv(CompilerState *compiler_state, lua_Integer m, lua_Integer n)
{
	if (unlikely(l_castS2U(n) + 1u <= 1u)) { /* special cases: -1 or 0 */
		if (n == 0)
			handle_error(compiler_state, "attempt to divide by zero");
		return 0 - m; /* n==-1; avoid overflow with 0x80000...//-1 */
	} else {
		lua_Integer q = m / n;	       /* perform C division */
		if ((m ^ n) < 0 && m % n != 0) /* 'm/n' would be negative non-integer? */
			q -= 1;		       /* correct result for different rounding */
		return q;
	}
}

/*
** Integer modulus; return 'm % n'. (Assume that C '%' with
** negative operands follows C99 behavior. See previous comment
** about luaV_idiv.)
*/
static lua_Integer luaV_mod(CompilerState *compiler_state, lua_Integer m, lua_Integer n)
{
	if (unlikely(l_castS2U(n) + 1u <= 1u)) { /* special cases: -1 or 0 */
		if (n == 0)
			handle_error(compiler_state, "attempt to perform 'n%%0'");
		return 0; /* m % -1 == 0; avoid overflow with 0x80000...%-1 */
	} else {
		lua_Integer r = m % n;
		if (r != 0 && (r ^ n) < 0) /* 'm/n' would be non-integer negative? */
			r += n;		   /* correct result for different rounding */
		return r;
	}
}

/*
** Float modulus
*/
static lua_Number luaV_modf(lua_Number m, lua_Number n)
{
	lua_Number r;
	luai_nummod(m, n, r);
	return r;
}

/*
** Shift left operation. (Shift right just negates 'y'.)
*/
static lua_Integer luaV_shiftl(lua_Integer x, lua_Integer y)
{
	if (y < 0) { /* shift right? */
		if (y <= -NBITS)
			return 0;
		else
			return x >> (-y);
	} else { /* shift left */
		if (y >= NBITS)
			return 0;
		else
			return x << y;
	}
}

static lua_Integer intarith(CompilerState *compiler_state, int op, lua_Integer v1, lua_Integer v2)
{
	switch (op) {
	case BINOPR_ADD:
		return v1 + v2;
	case BINOPR_SUB:
		return v1 - v2;
	case BINOPR_MUL:
		return v1 * v2;
	case BINOPR_MOD:
		return luaV_mod(compiler_state, v1, v2);
	case BINOPR_IDIV:
		return luaV_idiv(compiler_state, v1, v2);
	case BINOPR_BAND:
		return v1 & v2;
	case BINOPR_BOR:
		return v1 | v2;
	case BINOPR_BXOR:
		return v1 ^ v2;
	case BINOPR_SHL:
		return luaV_shiftl(v1, v2);
	case BINOPR_SHR:
		return luaV_shiftl(v1, -v2);
	case UNOPR_MINUS:
		return 0 - v1;
	case UNOPR_BNOT:
		return ~l_castS2U(0) ^ v1;
	default:
		assert(0);
		return 0;
	}
}

static lua_Number numarith(CompilerState *compiler_state, int op, lua_Number v1, lua_Number v2)
{
	switch (op) {
	case BINOPR_ADD:
		return luai_numadd(v1, v2);
	case BINOPR_SUB:
		return luai_numsub(v1, v2);
	case BINOPR_MUL:
		return luai_nummul(v1, v2);
	case BINOPR_DIV:
		return luai_numdiv(v1, v2);
	case BINOPR_POW:
		return luai_numpow(v1, v2);
	case BINOPR_IDIV:
		return luai_numidiv(v1, v2);
	case UNOPR_MINUS:
		return luai_numunm(v1);
	case BINOPR_MOD:
		return luaV_modf(v1, v2);
	default:
		assert(0);
		return 0;
	}
}

/*
** try to convert a float to an integer, rounding according to 'mode'.
*/
static int luaV_flttointeger(lua_Number n, lua_Integer *p, F2Imod mode)
{
	lua_Number f = l_floor(n);
	if (n != f) { /* not an integral value? */
		if (mode == F2Ieq)
			return 0;	  /* fails if mode demands integral value */
		else if (mode == F2Iceil) /* needs ceil? */
			f += 1;		  /* convert floor to ceil (remember: n != f) */
	}
	return lua_numbertointeger(f, p);
}

/*
** try to convert a value to an integer, rounding according to 'mode',
** without string coercion.
** ("Fast track" handled by macro 'tointegerns'.)
*/
static int luaV_tointegerns(const LiteralExpression *obj, lua_Integer *p, F2Imod mode)
{
	if (ttisfloat(obj))
		return luaV_flttointeger(fltvalue(obj), p, mode);
	else if (ttisinteger(obj)) {
		*p = ivalue(obj);
		return 1;
	} else
		return 0;
}

static int luaO_rawarith(CompilerState *compiler_state, int op, const LiteralExpression *p1,
			 const LiteralExpression *p2, LiteralExpression *res)
{
	switch (op) {
	case BINOPR_BAND:
	case BINOPR_BOR:
	case BINOPR_BXOR:
	case BINOPR_SHL:
	case BINOPR_SHR:
	case UNOPR_BNOT: { /* operate only on integers */
		lua_Integer i1;
		lua_Integer i2;
		if (tointegerns(p1, &i1) && tointegerns(p2, &i2)) {
			setivalue(res, intarith(compiler_state, op, i1, i2));
			return 1;
		} else
			return 0; /* fail */
	}
	case BINOPR_DIV:
	case BINOPR_POW: { /* operate only on floats */
		lua_Number n1;
		lua_Number n2;
		if (tonumberns(p1, n1) && tonumberns(p2, n2)) {
			setfltvalue(res, numarith(compiler_state, op, n1, n2));
			return 1;
		} else
			return 0; /* fail */
	}
	default: { /* other operations */
		lua_Number n1;
		lua_Number n2;
		if (ttisinteger(p1) && ttisinteger(p2)) {
			setivalue(res, intarith(compiler_state, op, ivalue(p1), ivalue(p2)));
			return 1;
		} else if (tonumberns(p1, n1) && tonumberns(p2, n2)) {
			setfltvalue(res, numarith(compiler_state, op, n1, n2));
			return 1;
		} else
			return 0; /* fail */
	}
	}
}

static void process_expression(CompilerState *container, AstNode *node)
{
	switch (node->type) {
	case EXPR_FUNCTION:
		process_statement_list(container, node->function_expr.function_statement_list);
		break;
	case EXPR_SUFFIXED:
		process_expression(container, node->suffixed_expr.primary_expr);
		if (node->suffixed_expr.suffix_list) {
			process_expression_list(container, node->suffixed_expr.suffix_list);
		} else {
			// We can simplify and get rid of the suffixed expr
			// TODO free primary_expr
			memcpy(node, node->suffixed_expr.primary_expr, sizeof(AstNode));
		}
		break;
	case EXPR_FUNCTION_CALL:
		process_expression_list(container, node->function_call_expr.arg_list);
		break;
	case EXPR_SYMBOL:
		break;
	case EXPR_BINARY:
		process_expression(container, node->binary_expr.expr_left);
		process_expression(container, node->binary_expr.expr_right);
		if (node->binary_expr.expr_left->type == EXPR_LITERAL &&
		    node->binary_expr.expr_right->type == EXPR_LITERAL && 
			node->binary_expr.binary_op >= BINOPR_ADD &&
			node->binary_expr.binary_op <= BINOPR_SHR) {
			LiteralExpression result = { { RAVI_TANY } };
			if (luaO_rawarith(container, node->binary_expr.binary_op,
					  &node->binary_expr.expr_left->literal_expr,
					  &node->binary_expr.expr_right->literal_expr, &result)) {
				node->type = EXPR_LITERAL;
				node->literal_expr.type.type_code = result.type.type_code;
				if (node->literal_expr.type.type_code == RAVI_TNUMFLT)
					node->literal_expr.u.r = result.u.r;
				else {
					assert(node->literal_expr.type.type_code == RAVI_TNUMINT);
					node->literal_expr.u.i = result.u.i;
				}
				// TODO free expr_left and expr_right
			}
		}
		break;
	case EXPR_UNARY:
		process_expression(container, node->unary_expr.expr);
		if (node->unary_expr.expr->type == EXPR_LITERAL && 
			(node->unary_expr.unary_op == UNOPR_BNOT || node->unary_expr.unary_op == UNOPR_MINUS)) {
			LiteralExpression result = { { RAVI_TANY } };
			if (luaO_rawarith(container, node->unary_expr.unary_op, &node->unary_expr.expr->literal_expr,
					  &node->unary_expr.expr->literal_expr, &result)) {
				node->type = EXPR_LITERAL;
				node->literal_expr.type.type_code = result.type.type_code;
				if (node->literal_expr.type.type_code == RAVI_TNUMFLT)
					node->literal_expr.u.r = result.u.r;
				else {
					assert(node->literal_expr.type.type_code == RAVI_TNUMINT);
					node->literal_expr.u.i = result.u.i;
				}
				// TODO free unary_expr.expr
			}
		}
		break;
	case EXPR_LITERAL:
		break;
	case EXPR_FIELD_SELECTOR:
		process_expression(container, node->index_expr.expr);
		break;
	case EXPR_Y_INDEX:
		process_expression(container, node->index_expr.expr);
		break;
	case EXPR_TABLE_ELEMENT_ASSIGN:
		if (node->table_elem_assign_expr.key_expr) {
			process_expression(container, node->table_elem_assign_expr.key_expr);
		}
		process_expression(container, node->table_elem_assign_expr.value_expr);
		break;
	case EXPR_TABLE_LITERAL:
		process_expression_list(container, node->table_expr.expr_list);
		break;
	default:
		assert(0);
		break;
	}
}

static void process_expression_list(CompilerState *container, AstNodeList *list)
{
	AstNode *node;
	FOR_EACH_PTR(list, AstNode, node) { process_expression(container, node); }
	END_FOR_EACH_PTR(node);
}

static void process_statement_list(CompilerState *container, AstNodeList *list)
{
	AstNode *node;
	FOR_EACH_PTR(list, AstNode, node) { process_statement(container, node); }
	END_FOR_EACH_PTR(node);
}

static void process_statement(CompilerState *container, AstNode *node)
{
	switch (node->type) {
	case AST_NONE:
		break;
	case STMT_RETURN:
		process_expression_list(container, node->return_stmt.expr_list);
		break;
	case STMT_LOCAL:
		process_expression_list(container, node->local_stmt.expr_list);
		break;
	case STMT_FUNCTION:
		process_expression(container, node->function_stmt.function_expr);
		break;
	case STMT_LABEL:
	case STMT_GOTO:
		break;
	case STMT_DO:
		process_statement_list(container, node->do_stmt.do_statement_list);
		break;
	case STMT_EXPR:
		if (node->expression_stmt.var_expr_list) {
			process_expression_list(container, node->expression_stmt.var_expr_list);
		}
		process_expression_list(container, node->expression_stmt.expr_list);
		break;
	case STMT_IF: {
		AstNode *test_then_block;
		FOR_EACH_PTR(node->if_stmt.if_condition_list, AstNode, test_then_block)
		{
			process_expression(container, test_then_block->test_then_block.condition);
			process_statement_list(container, test_then_block->test_then_block.test_then_statement_list);
		}
		END_FOR_EACH_PTR(node);
		if (node->if_stmt.else_block) {
			process_statement_list(container, node->if_stmt.else_statement_list);
		}
		break;
	}
	case STMT_WHILE:
		process_expression(container, node->while_or_repeat_stmt.condition);
		process_statement_list(container, node->while_or_repeat_stmt.loop_statement_list);
		break;
	case STMT_REPEAT:
		process_statement_list(container, node->while_or_repeat_stmt.loop_statement_list);
		process_expression(container, node->while_or_repeat_stmt.condition);
		break;
	case STMT_FOR_IN:
	case STMT_FOR_NUM:
		process_expression_list(container, node->for_stmt.expr_list);
		process_statement_list(container, node->for_stmt.for_statement_list);
		break;
	default:
		fprintf(stderr, "AST = %d\n", node->type);
		assert(0);
		break;
	}
}

int raviX_ast_simplify(CompilerState *container)
{
	int rc = setjmp(container->env);
	if (rc == 0) {
		process_expression(container, container->main_function);
	} else {
		// dump it?
	}
	return rc;
}

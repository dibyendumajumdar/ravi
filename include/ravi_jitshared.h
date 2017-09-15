/******************************************************************************
* Copyright (C) 2015-2017 Dibyendu Majumdar
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

#ifndef RAVI_JITSHARED_H
#define RAVI_JITSHARED_H

#ifdef __cplusplus
extern "C" {
#endif

// TODO we probably do not need all the headers
// below

#define LUA_CORE

#include "lprefix.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "lua.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"
#include "lauxlib.h"

#include <ravi_membuf.h>

#define RA(i) (base + GETARG_A(i))
/* to be used after possible stack reallocation */
#define RB(i) check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i))
#define RC(i) check_exp(getCMode(GET_OPCODE(i)) == OpArgR, base + GETARG_C(i))
#define RKB(i)                                 \
  check_exp(getBMode(GET_OPCODE(i)) == OpArgK, \
            ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i))
#define RKC(i)                                 \
  check_exp(getCMode(GET_OPCODE(i)) == OpArgK, \
            ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i))
#define KBx(i) \
  (k + (GETARG_Bx(i) != 0 ? GETARG_Bx(i) - 1 : GETARG_Ax(*ci->u.l.savedpc++)))
/* RAVI */
#define KB(i) \
  check_exp(getBMode(GET_OPCODE(i)) == OpArgK, k + INDEXK(GETARG_B(i)))
#define KC(i) \
  check_exp(getCMode(GET_OPCODE(i)) == OpArgK, k + INDEXK(GETARG_C(i)))

enum ravi_codegen_type {
	RAVI_CODEGEN_NONE = 0,
	RAVI_CODEGEN_HEADER_ONLY = 1,
	RAVI_CODEGEN_FUNCTION_ONLY = 2,
	RAVI_CODEGEN_ALL = 3,
};

struct ravi_compile_options_t {
	/* Is this a manual request? */
	int manual_request;

	/* Should range check be omitted when compiling array access */
	int omit_array_get_range_check;

	/* Should the compiler dump generated code ? */
	int dump_level;

	/* Should the compiler validate the generated code ? */
	int verification_level;

	enum ravi_codegen_type codegen_type;
};

LUAI_FUNC bool raviJ_cancompile(Proto *p);

// Convert a Lua function to C code
// Returns true if compilation was successful
// If successful then buf will be set
LUAI_FUNC bool raviJ_codegen(struct lua_State *L, struct Proto *p,
	struct ravi_compile_options_t *options, const char *fname, membuff_t *buf);

#ifdef __cplusplus
};
#endif

#endif 

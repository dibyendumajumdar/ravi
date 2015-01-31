/*
** $Id: lopcodes.c,v 1.54 2014/11/02 19:19:04 roberto Exp $
** Opcodes for Lua virtual machine
** See Copyright Notice in lua.h
*/

#define lopcodes_c
#define LUA_CORE

#include "lprefix.h"

#include "lopcodes.h"
#include "lobject.h"

#include <string.h>

/* ORDER OP */

LUAI_DDEF const char *const luaP_opnames[NUM_OPCODES+1] = {
  "MOVE",
  "LOADK",
  "LOADKX",
  "LOADBOOL",
  "LOADNIL",
  "GETUPVAL",
  "GETTABUP",
  "GETTABLE",
  "SETTABUP",
  "SETUPVAL",
  "SETTABLE",
  "NEWTABLE",
  "SELF",
  "ADD",
  "SUB",
  "MUL",
  "MOD",
  "POW",
  "DIV",
  "IDIV",
  "BAND",
  "BOR",
  "BXOR",
  "SHL",
  "SHR",
  "UNM",
  "BNOT",
  "NOT",
  "LEN",
  "CONCAT",
  "JMP",
  "EQ",
  "LT",
  "LE",
  "TEST",
  "TESTSET",
  "CALL",
  "TAILCALL",
  "RETURN",
  "FORLOOP",
  "FORPREP",
  "TFORCALL",
  "TFORLOOP",
  "SETLIST",
  "CLOSURE",
  "VARARG",
  "EXTRAARG",

  "NEWARRAYI", /* A R(A) := array of int */
  "NEWARRAYF", /* A R(A) := array of float */

  "LOADIZ", /*  A R(A) := tointeger(0)		*/
  "LOADFZ", /*  A R(A) := tonumber(0)		*/

  "UNMF",   /*	A B	R(A) := -R(B) floating point      */
  "UNMI",   /*  A B R(A) := -R(B) integer */

  "ADDFF",  /*	A B C	R(A) := RK(B) + RK(C)		*/
  "ADDFI",  /*	A B C	R(A) := RK(B) + RK(C)		*/
  "ADDII",  /*	A B C	R(A) := RK(B) + RK(C)	  */
  "ADDFN",  /*	A B C	R(A) := RK(B) + C				*/
  "ADDIN",  /*	A B C	R(A) := RK(B) + C				*/

  "SUBFF",  /*	A B C	R(A) := RK(B) - RK(C)		*/
  "SUBFI",  /*	A B C	R(A) := RK(B) - RK(C)		*/
  "SUBIF",  /*	A B C	R(A) := RK(B) - RK(C)	  */
  "SUBII",  /*	A B C	R(A) := RK(B) - RK(C)		*/
  "SUBFN",  /*	A B C	R(A) := RK(B) - C				*/
  "SUBNF",  /*	A B C	R(A) := B - RK(C)				*/
  "SUBIN",  /*	A B C	R(A) := RK(B) - C				*/
  "SUBNI",  /*	A B C	R(A) := B - RK(C)				*/

  "MULFF",  /*	A B C	R(A) := RK(B) * RK(C)		*/
  "MULFI",  /*	A B C	R(A) := RK(B) * RK(C)		*/
  "MULII",  /*	A B C	R(A) := RK(B) * RK(C)	  */
  "MULFN",  /*	A B C	R(A) := RK(B) * C				*/
  "MULIN",  /*	A B C	R(A) := RK(B) * C				*/

  "DIVFF",  /*	A B C	R(A) := RK(B) / RK(C)		*/
  "DIVFI",  /*	A B C	R(A) := RK(B) / RK(C)		*/
  "DIVIF",  /*	A B C	R(A) := RK(B) / RK(C)	  */
  "DIVII",  /*	A B C	R(A) := RK(B) / RK(C)		*/

  "TOINT", /* A R(A) := toint(R(A)) */
  "TOFLT", /* A R(A) := tofloat(R(A)) */
  "TOARRAYI", /* A R(A) := to_arrayi(R(A)) */
  "TOARRAYF", /* A R(A) := to_arrayf(R(A)) */

  "MOVEI",  /*	A B	R(A) := R(B)					*/
  "MOVEF",  /*	A B	R(A) := R(B)					*/
  "MOVEAI", /* A B R(A) := R(B), check R(B) is array of int */
  "MOVEAF", /* A B R(A) := R(B), check R(B) is array of floats */

  "GETTABLE_AI",/*	A B C	R(A) := R(B)[RK(C)] where R(B) is array of integers and RK(C) is int */
  "GETTABLE_AF",/*	A B C	R(A) := R(B)[RK(C)] where R(B) is array of floats and RK(C) is int */

  "SETTABLE_AI",/*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of ints, and RK(C) is an int */
  "SETTABLE_AF",/*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of floats, and RK(C) is an float */

  NULL
};


#define opmode(t,a,b,c,m) (((t)<<7) | ((a)<<6) | ((b)<<4) | ((c)<<2) | (m))

LUAI_DDEF const lu_byte luaP_opmodes[NUM_OPCODES] = {
/*       T  A    B       C     mode		   opcode	*/
  opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_MOVE */
 ,opmode(0, 1, OpArgK, OpArgN, iABx)		/* OP_LOADK */
 ,opmode(0, 1, OpArgN, OpArgN, iABx)		/* OP_LOADKX */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_LOADBOOL */
 ,opmode(0, 1, OpArgU, OpArgN, iABC)		/* OP_LOADNIL */
 ,opmode(0, 1, OpArgU, OpArgN, iABC)		/* OP_GETUPVAL */
 ,opmode(0, 1, OpArgU, OpArgK, iABC)		/* OP_GETTABUP */
 ,opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_GETTABLE */
 ,opmode(0, 0, OpArgK, OpArgK, iABC)		/* OP_SETTABUP */
 ,opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_SETUPVAL */
 ,opmode(0, 0, OpArgK, OpArgK, iABC)		/* OP_SETTABLE */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_NEWTABLE */
 ,opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_SELF */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_ADD */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_SUB */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_MUL */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_MOD */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_POW */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_DIV */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_IDIV */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_BAND */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_BOR */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_BXOR */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_SHL */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_SHR */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_UNM */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_BNOT */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_NOT */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_LEN */
 ,opmode(0, 1, OpArgR, OpArgR, iABC)		/* OP_CONCAT */
 ,opmode(0, 0, OpArgR, OpArgN, iAsBx)		/* OP_JMP */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_EQ */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_LT */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_LE */
 ,opmode(1, 0, OpArgN, OpArgU, iABC)		/* OP_TEST */
 ,opmode(1, 1, OpArgR, OpArgU, iABC)		/* OP_TESTSET */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_CALL */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_TAILCALL */
 ,opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_RETURN */
 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_FORLOOP */
 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_FORPREP */
 ,opmode(0, 0, OpArgN, OpArgU, iABC)		/* OP_TFORCALL */
 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_TFORLOOP */
 ,opmode(0, 0, OpArgU, OpArgU, iABC)		/* OP_SETLIST */
 ,opmode(0, 1, OpArgU, OpArgN, iABx)		/* OP_CLOSURE */
 ,opmode(0, 1, OpArgU, OpArgN, iABC)		/* OP_VARARG */
 ,opmode(0, 0, OpArgU, OpArgU, iAx)		    /* OP_EXTRAARG */

 ,opmode(0, 1, OpArgN, OpArgN, iABC) /* OP_RAVI_NEWARRAYI A R(A) := array of int */
 ,opmode(0, 1, OpArgN, OpArgN, iABC) /* OP_RAVI_NEWARRAYF A R(A) := array of float */

 ,opmode(0, 1, OpArgN, OpArgN, iABC) /*	OP_RAVI_LOADIZ A R(A) := tointeger(0)		*/
 ,opmode(0, 1, OpArgN, OpArgN, iABC) /*	OP_RAVI_LOADFZ A R(A) := tonumber(0)		*/

 ,opmode(0, 1, OpArgR, OpArgN, iABC) /* OP_RAVI_UNMF A B	   R(A) := -R(B) floating point      */
 ,opmode(0, 1, OpArgR, OpArgN, iABC) /* OP_RAVI_UNMI A B     R(A) := -R(B) integer */

 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_ADDFF	A B C	R(A) := RK(B) + RK(C)		*/
 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_ADDFI	A B C	R(A) := RK(B) + RK(C)		*/
 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_ADDII	A B C	R(A) := RK(B) + RK(C)	  */
 , opmode(0, 1, OpArgK, OpArgU, iABC)/* OP_RAVI_ADDFN	A B C	R(A) := RK(B) + C				*/
 , opmode(0, 1, OpArgK, OpArgU, iABC)/* OP_RAVI_ADDIN	A B C	R(A) := RK(B) + C				*/

 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_SUBFF	A B C	R(A) := RK(B) - RK(C)		*/
 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_SUBFI	A B C	R(A) := RK(B) - RK(C)		*/
 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_SUBIF	A B C	R(A) := RK(B) - RK(C)	  */
 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_SUBII	A B C	R(A) := RK(B) - RK(C)		*/
 , opmode(0, 1, OpArgK, OpArgU, iABC)/* OP_RAVI_SUBFN	A B C	R(A) := RK(B) - C				*/
 , opmode(0, 1, OpArgU, OpArgK, iABC)/* OP_RAVI_SUBNF	A B C	R(A) := B - RK(C)				*/
 , opmode(0, 1, OpArgK, OpArgU, iABC)/* OP_RAVI_SUBIN	A B C	R(A) := RK(B) - C				*/
 , opmode(0, 1, OpArgU, OpArgK, iABC)/* OP_RAVI_SUBNI	A B C	R(A) := B - RK(C)				*/

 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_MULFF	A B C	R(A) := RK(B) * RK(C)		*/
 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_MULFI	A B C	R(A) := RK(B) * RK(C)		*/
 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_MULII	A B C	R(A) := RK(B) * RK(C)	  */
 , opmode(0, 1, OpArgK, OpArgU, iABC)/* OP_RAVI_MULFN	A B C	R(A) := RK(B) * C				*/
 , opmode(0, 1, OpArgK, OpArgU, iABC)/* OP_RAVI_MULIN	A B C	R(A) := RK(B) * C				*/

 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_DIVFF	A B C	R(A) := RK(B) / RK(C)		*/
 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_DIVFI	A B C	R(A) := RK(B) / RK(C)		*/
 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_DIVIF	A B C	R(A) := RK(B) / RK(C)	  */
 , opmode(0, 1, OpArgK, OpArgK, iABC)/* OP_RAVI_DIVII	A B C	R(A) := RK(B) / RK(C)		*/

 , opmode(0, 1, OpArgN, OpArgN, iABC) /* OP_RAVI_TOINT  A R(A) := toint(R(A)) */
 , opmode(0, 1, OpArgN, OpArgN, iABC) /* OP_RAVI_TOFLT  A R(A) := tonumber(R(A)) */
 , opmode(0, 1, OpArgN, OpArgN, iABC) /* OP_RAVI_TOARRAYI A R(A) := check_array_of_int(R(A)) */
 , opmode(0, 1, OpArgN, OpArgN, iABC) /* OP_RAVI_TOARRAYF A R(A) := check_array_of_float(R(A)) */

 , opmode(0, 1, OpArgR, OpArgN, iABC) /* OP_RAVI_MOVEI	A B	R(A) := tointeger(R(B))	*/
 , opmode(0, 1, OpArgR, OpArgN, iABC) /* OP_RAVI_MOVEF	A B	R(A) := tonumber(R(B)) */
 , opmode(0, 1, OpArgR, OpArgN, iABC) /* OP_RAVI_MOVEAI A B R(A) := R(B), check R(B) is array of int */
 , opmode(0, 1, OpArgR, OpArgN, iABC) /* OP_RAVI_MOVEAF A B R(A) := R(B), check R(B) is array of floats */

 , opmode(0, 1, OpArgR, OpArgK, iABC) /* OP_RAVI_GETTABLE_AI A B C	R(A) := R(B)[RK(C)] where R(B) is array of integers and RK(C) is int */
 , opmode(0, 1, OpArgR, OpArgK, iABC) /* OP_RAVI_GETTABLE_AF A B C	R(A) := R(B)[RK(C)] where R(B) is array of floats and RK(C) is int */

 , opmode(0, 0, OpArgK, OpArgK, iABC) /* OP_RAVI_SETTABLE_AI A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of ints, and RK(C) is an int */
 , opmode(0, 0, OpArgK, OpArgK, iABC) /* OP_RAVI_SETTABLE_AF A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of floats, and RK(C) is an float */

};


#define MYK(x)		(-1-(x))

static const char* PrintRaviCode(char *buf, size_t n, Instruction i) {
  OpCode o = GET_OPCODE(i);
  int a = GETARG_A(i);
  int b = GETARG_B(i);
  int c = GETARG_C(i);
  snprintf(buf, n, "%s ", luaP_opnames[o]);
  switch (getOpMode(o)) {
  case iABC:
    snprintf(buf + strlen(buf), n - strlen(buf), "A=%d", a);
    if (getBMode(o) != OpArgN)
      snprintf(buf + strlen(buf), n - strlen(buf), " B=%d", getBMode(o) == OpArgK ? (MYK(INDEXK(b))) : b);
    if (getCMode(o) != OpArgN)
      snprintf(buf + strlen(buf), n - strlen(buf), " C=%d", getCMode(o) == OpArgK ? (MYK(INDEXK(c))) : c);
    break;
  default:
    break;
  }
  return buf;
}

const char* raviP_instruction_to_str(char *buf, size_t n, Instruction i) {
  OpCode o = GET_OPCODE(i);
  if (o >= OP_RAVI_UNMF) 
    return PrintRaviCode(buf, n, i);
  int a = GETARG_A(i);
  int b = GETARG_B(i);
  int c = GETARG_C(i);
  int ax = GETARG_Ax(i);
  int bx = GETARG_Bx(i);
  int sbx = GETARG_sBx(i);
  snprintf(buf, n, "%s ", luaP_opnames[o]);
  switch (getOpMode(o)) {
  case iABC:
    snprintf(buf+strlen(buf), n-strlen(buf), "A=%d", a);
    if (getBMode(o) != OpArgN)
      snprintf(buf + strlen(buf), n - strlen(buf), " B=%d", ISK(b) ? (MYK(INDEXK(b))) : b);
    if (getCMode(o) != OpArgN)
      snprintf(buf + strlen(buf), n - strlen(buf), " C=%d", ISK(c) ? (MYK(INDEXK(c))) : c);
    break;
  case iABx:
    snprintf(buf + strlen(buf), n - strlen(buf), "A=%d", a);
    if (getBMode(o) == OpArgK)
      snprintf(buf + strlen(buf), n - strlen(buf), " Bx=%d", MYK(bx));
    if (getBMode(o) == OpArgU)
      snprintf(buf + strlen(buf), n - strlen(buf), " Bx=%d", bx);
    break;
  case iAsBx:
    snprintf(buf + strlen(buf), n - strlen(buf), "As=%d Bx=%d", a, sbx);
    break;
  case iAx:
    snprintf(buf + strlen(buf), n - strlen(buf), "Ax=%d", MYK(ax));
    break;
  }
  return buf;
}

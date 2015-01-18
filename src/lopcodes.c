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

  "UNMF",  /*	A B	R(A) := -R(B) floating point      */
  "UNMI",  /*   A B R(A) := -R(B) integer */

  "ADDFFKK",/*	A B C	R(A) := Kst(B) + Kst(C)				*/
  "ADDFFKR",/*	A B C	R(A) := Kst(B) + R(C)				*/
  "ADDFFRK",/*	A B C	R(A) := R(B) + Kst(C)				*/
  "ADDFFRR",/*	A B C	R(A) := R(B) + R(C)				*/
  "ADDFIKK",/*	A B C	R(A) := Kst(B) + Kst(C)				*/
  "ADDFIKR",/*	A B C	R(A) := Kst(B) + R(C)				*/
  "ADDFIRK",/*	A B C	R(A) := R(B) + Kst(C)				*/
  "ADDFIRR",/*	A B C	R(A) := R(B) + R(C)				*/
  "ADDIFKK",/*	A B C	R(A) := Kst(B) + Kst(C)				*/
  "ADDIFKR",/*	A B C	R(A) := Kst(B) + R(C)				*/
  "ADDIFRK",/*	A B C	R(A) := R(B) + Kst(C)				*/
  "ADDIFRR",/*	A B C	R(A) := R(B) + R(C)				*/
  "ADDIIKK",/*	A B C	R(A) := Kst(B) + Kst(C)				*/
  "ADDIIKR",/*	A B C	R(A) := Kst(B) + R(C)				*/
  "ADDIIRK",/*	A B C	R(A) := R(B) + Kst(C)				*/
  "ADDIIRR",/*	A B C	R(A) := R(B) + R(C)				*/

  "SUBFFKK",/*	A B C	R(A) := Kst(B) - Kst(C)				*/
  "SUBFFKR",/*	A B C	R(A) := Kst(B) - R(C)				*/
  "SUBFFRK",/*	A B C	R(A) := R(B) - Kst(C)				*/
  "SUBFFRR",/*	A B C	R(A) := R(B) - R(C)				*/
  "SUBFIKK",/*	A B C	R(A) := Kst(B) - Kst(C)				*/
  "SUBFIKR",/*	A B C	R(A) := Kst(B) - R(C)				*/
  "SUBFIRK",/*	A B C	R(A) := R(B) - Kst(C)				*/
  "SUBFIRR",/*	A B C	R(A) := R(B) - R(C)				*/
  "SUBIFKK",/*	A B C	R(A) := Kst(B) - Kst(C)				*/
  "SUBIFKR",/*	A B C	R(A) := Kst(B) - R(C)				*/
  "SUBIFRK",/*	A B C	R(A) := R(B) - Kst(C)				*/
  "SUBIFRR",/*	A B C	R(A) := R(B) - R(C)				*/
  "SUBIIKK",/*	A B C	R(A) := Kst(B) - Kst(C)				*/
  "SUBIIKR",/*	A B C	R(A) := Kst(B) - R(C)				*/
  "SUBIIRK",/*	A B C	R(A) := R(B) - Kst(C)				*/
  "SUBIIRR",/*	A B C	R(A) := R(B) - R(C)				*/

  "MULFFKK",/*	A B C	R(A) := Kst(B) * Kst(C)				*/
  "MULFFKR",/*	A B C	R(A) := Kst(B) * R(C)				*/
  "MULFFRK",/*	A B C	R(A) := R(B) * Kst(C)				*/
  "MULFFRR",/*	A B C	R(A) := R(B) * R(C)				*/
  "MULFIKK",/*	A B C	R(A) := Kst(B) * Kst(C)				*/
  "MULFIKR",/*	A B C	R(A) := Kst(B) * R(C)				*/
  "MULFIRK",/*	A B C	R(A) := R(B) * Kst(C)				*/
  "MULFIRR",/*	A B C	R(A) := R(B) * R(C)				*/
  "MULIFKK",/*	A B C	R(A) := Kst(B) * Kst(C)				*/
  "MULIFKR",/*	A B C	R(A) := Kst(B) * R(C)				*/
  "MULIFRK",/*	A B C	R(A) := R(B) * Kst(C)				*/
  "MULIFRR",/*	A B C	R(A) := R(B) * R(C)				*/
  "MULIIKK",/*	A B C	R(A) := Kst(B) * Kst(C)				*/
  "MULIIKR",/*	A B C	R(A) := Kst(B) * R(C)				*/
  "MULIIRK",/*	A B C	R(A) := R(B) * Kst(C)				*/
  "MULIIRR",/*	A B C	R(A) := R(B) * R(C)				*/

  "DIVFFKK",/*	A B C	R(A) := Kst(B) / Kst(C)				*/
  "DIVFFKR",/*	A B C	R(A) := Kst(B) / R(C)				*/
  "DIVFFRK",/*	A B C	R(A) := R(B) / Kst(C)				*/
  "DIVFFRR",/*	A B C	R(A) := R(B) / R(C)				*/
  "DIVFIKK",/*	A B C	R(A) := Kst(B) / Kst(C)				*/
  "DIVFIKR",/*	A B C	R(A) := Kst(B) / R(C)				*/
  "DIVFIRK",/*	A B C	R(A) := R(B) / Kst(C)				*/
  "DIVFIRR",/*	A B C	R(A) := R(B) / R(C)				*/
  "DIVIFKK",/*	A B C	R(A) := Kst(B) / Kst(C)				*/
  "DIVIFKR",/*	A B C	R(A) := Kst(B) / R(C)				*/
  "DIVIFRK",/*	A B C	R(A) := R(B) / Kst(C)				*/
  "DIVIFRR",/*	A B C	R(A) := R(B) / R(C)				*/
  "DIVIIKK",/*	A B C	R(A) := Kst(B) / Kst(C)				*/
  "DIVIIKR",/*	A B C	R(A) := Kst(B) / R(C)				*/
  "DIVIIRK",/*	A B C	R(A) := R(B) / Kst(C)				*/
  "DIVIIRR",/*	A B C	R(A) := R(B) / R(C)				*/

  "EQFFKK",/*	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
  "EQFFKR",/*	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
  "EQFFRK",/*	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
  "EQFFRR",/*	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/
  "EQIIKK",/*	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
  "EQIIKR",/*	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
  "EQIIRK",/*	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
  "EQIIRR",/*	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/

  "LTFFKK",/*	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
  "LTFFKR",/*	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
  "LTFFRK",/*	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
  "LTFFRR",/*	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/
  "LTIIKK",/*	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
  "LTIIKR",/*	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
  "LTIIRK",/*	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
  "LTIIRR",/*	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/

  "LEFFKK",/*	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
  "LEFFKR",/*	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
  "LEFFRK",/*	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
  "LEFFRR",/*	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/
  "LEIIKK",/*	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
  "LEIIKR",/*	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
  "LEIIRK",/*	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
  "LEIIRR",/*	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/

  "ARRAYGET_SIK",/*	A B C	R(A) := R(B)[Kst(C)]				*/
  "ARRAYGET_SIR",/*	A B C	R(A) := R(B)[R(C)]				*/
  "ARRAYGET_IIK",/*	A B C	R(A) := R(B)[Kst(C)]				*/
  "ARRAYGET_IIR",/*	A B C	R(A) := R(B)[R(C)]				*/
  "ARRAYGET_FIK",/*	A B C	R(A) := R(B)[Kst(C)]				*/
  "ARRAYGET_FIR",/*	A B C	R(A) := R(B)[R(C)]				*/
  "ARRAYGET_LIK",/*	A B C	R(A) := R(B)[Kst(C)]				*/
  "ARRAYGET_LIR",/*	A B C	R(A) := R(B)[R(C)]				*/

  "ARRAYSET_ISKK",/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
  "ARRAYSET_ISKR",/*	A B C	R(A)[Kst(B)] := R(C)				*/
  "ARRAYSET_ISRK",/*	A B C	R(A)[R(B)] := Kst(C)				*/
  "ARRAYSET_ISRR",/*	A B C	R(A)[R(B)] := R(C)				*/
  "ARRAYSET_IIKK",/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
  "ARRAYSET_IIKR",/*	A B C	R(A)[Kst(B)] := R(C)				*/
  "ARRAYSET_IIRK",/*	A B C	R(A)[R(B)] := Kst(C)				*/
  "ARRAYSET_IIRR",/*	A B C	R(A)[R(B)] := R(C)				*/
  "ARRAYSET_IFKK",/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
  "ARRAYSET_IFKR",/*	A B C	R(A)[Kst(B)] := R(C)				*/
  "ARRAYSET_IFRK",/*	A B C	R(A)[R(B)] := Kst(C)				*/
  "ARRAYSET_IFRR",/*	A B C	R(A)[R(B)] := R(C)				*/
  "ARRAYSET_ILKK",/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
  "ARRAYSET_ILKR",/*	A B C	R(A)[Kst(B)] := R(C)				*/
  "ARRAYSET_ILRK",/*	A B C	R(A)[R(B)] := Kst(C)				*/
  "ARRAYSET_ILRR",/*	A B C	R(A)[R(B)] := R(C)				*/

  "TOINT", /* A R(A) := toint(R(A)) */
  "TOFLT", /* A R(A) := tofloat(R(A)) */

  "MOVEI", /*	A B	R(A) := R(B)					*/
  "MOVEF", /*	A B	R(A) := R(B)					*/

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

 ,opmode(0, 1, OpArgR, OpArgN, iABC)/* OP_RAVI_UNMF	A B	    R(A) := -R(B) floating point      */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)/* OP_RAVI_UNMI A B     R(A) := -R(B) integer */

 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_ADDFFKK	A B C	R(A) := Kst(B) + Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_ADDFFKR	A B C	R(A) := Kst(B) + R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_ADDFFRK	A B C	R(A) := R(B) + Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_ADDFFRR	A B C	R(A) := R(B) + R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_ADDFIKK	A B C	R(A) := Kst(B) + Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_ADDFIKR	A B C	R(A) := Kst(B) + R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_ADDFIRK	A B C	R(A) := R(B) + Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_ADDFIRR	A B C	R(A) := R(B) + R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_ADDIFKK	A B C	R(A) := Kst(B) + Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_ADDIFKR	A B C	R(A) := Kst(B) + R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_ADDIFRK	A B C	R(A) := R(B) + Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_ADDIFRR	A B C	R(A) := R(B) + R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_ADDIIKK	A B C	R(A) := Kst(B) + Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_ADDIIKR	A B C	R(A) := Kst(B) + R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_ADDIIRK	A B C	R(A) := R(B) + Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_ADDIIRR	A B C	R(A) := R(B) + R(C)				*/

 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_SUBFFKK	A B C	R(A) := Kst(B) - Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_SUBFFKR	A B C	R(A) := Kst(B) - R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_SUBFFRK	A B C	R(A) := R(B) - Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_SUBFFRR	A B C	R(A) := R(B) - R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_SUBFIKK	A B C	R(A) := Kst(B) - Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_SUBFIKR	A B C	R(A) := Kst(B) - R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_SUBFIRK	A B C	R(A) := R(B) - Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_SUBFIRR	A B C	R(A) := R(B) - R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_SUBIFKK	A B C	R(A) := Kst(B) - Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_SUBIFKR	A B C	R(A) := Kst(B) - R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_SUBIFRK	A B C	R(A) := R(B) - Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_SUBIFRR	A B C	R(A) := R(B) - R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_SUBIIKK	A B C	R(A) := Kst(B) - Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_SUBIIKR	A B C	R(A) := Kst(B) - R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_SUBIIRK	A B C	R(A) := R(B) - Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_SUBIIRR	A B C	R(A) := R(B) - R(C)				*/

 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_MULFFKK	A B C	R(A) := Kst(B) * Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_MULFFKR	A B C	R(A) := Kst(B) * R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_MULFFRK	A B C	R(A) := R(B) * Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_MULFFRR	A B C	R(A) := R(B) * R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_MULFIKK	A B C	R(A) := Kst(B) * Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_MULFIKR	A B C	R(A) := Kst(B) * R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_MULFIRK	A B C	R(A) := R(B) * Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_MULFIRR	A B C	R(A) := R(B) * R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_MULIFKK	A B C	R(A) := Kst(B) * Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_MULIFKR	A B C	R(A) := Kst(B) * R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_MULIFRK	A B C	R(A) := R(B) * Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_MULIFRR	A B C	R(A) := R(B) * R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_MULIIKK	A B C	R(A) := Kst(B) * Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_MULIIKR	A B C	R(A) := Kst(B) * R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_MULIIRK	A B C	R(A) := R(B) * Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_MULIIRR	A B C	R(A) := R(B) * R(C)				*/

 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_DIVFFKK	A B C	R(A) := Kst(B) / Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_DIVFFKR	A B C	R(A) := Kst(B) / R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_DIVFFRK	A B C	R(A) := R(B) / Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_DIVFFRR	A B C	R(A) := R(B) / R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_DIVFIKK	A B C	R(A) := Kst(B) / Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_DIVFIKR	A B C	R(A) := Kst(B) / R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_DIVFIRK	A B C	R(A) := R(B) / Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_DIVFIRR	A B C	R(A) := R(B) / R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_DIVIFKK	A B C	R(A) := Kst(B) / Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_DIVIFKR	A B C	R(A) := Kst(B) / R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_DIVIFRK	A B C	R(A) := R(B) / Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_DIVIFRR	A B C	R(A) := R(B) / R(C)				*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC) /*RAVI_DIVIIKK	A B C	R(A) := Kst(B) / Kst(C)				*/
 ,opmode(0, 1, OpArgK, OpArgR, iABC) /*RAVI_DIVIIKR	A B C	R(A) := Kst(B) / R(C)				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_DIVIIRK	A B C	R(A) := R(B) / Kst(C)				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_DIVIIRR	A B C	R(A) := R(B) / R(C)				*/

 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_EQFFKK	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_EQFFKR	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_EQFFRK	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_EQFFRR	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_EQIIKK	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_EQIIKR	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_EQIIRK	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_EQIIRR	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/

 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LTFFKK	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LTFFKR	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LTFFRK	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LTFFRR	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LTIIKK	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LTIIKR	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LTIIRK	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LTIIRR	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/

 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LEFFKK	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LEFFKR	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LEFFRK	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LEFFRR	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LEIIKK	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LEIIKR	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LEIIRK	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LEIIRR	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/

 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_ARRAYGET_SIK	A B C	R(A) := R(B)[Kst(C)]				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_ARRAYGET_SIR	A B C	R(A) := R(B)[R(C)]				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_ARRAYGET_IIK	A B C	R(A) := R(B)[Kst(C)]				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_ARRAYGET_IIR	A B C	R(A) := R(B)[R(C)]				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_ARRAYGET_FIK	A B C	R(A) := R(B)[Kst(C)]				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_ARRAYGET_FIR	A B C	R(A) := R(B)[R(C)]				*/
 ,opmode(0, 1, OpArgR, OpArgK, iABC) /*RAVI_ARRAYGET_LIK	A B C	R(A) := R(B)[Kst(C)]				*/
 ,opmode(0, 1, OpArgR, OpArgR, iABC) /*RAVI_ARRAYGET_LIR	A B C	R(A) := R(B)[R(C)]				*/

 ,opmode(0, 0, OpArgK, OpArgK, iABC) /*RAVI_ARRAYSET_ISKK	A B C	R(A)[Kst(B)] := Kst(C)				*/
 ,opmode(0, 0, OpArgK, OpArgR, iABC) /*RAVI_ARRAYSET_ISKR	A B C	R(A)[Kst(B)] := R(C)				*/
 ,opmode(0, 0, OpArgR, OpArgK, iABC) /*RAVI_ARRAYSET_ISRK	A B C	R(A)[R(B)] := Kst(C)				*/
 ,opmode(0, 0, OpArgR, OpArgR, iABC) /*RAVI_ARRAYSET_ISRR	A B C	R(A)[R(B)] := R(C)				*/
 ,opmode(0, 0, OpArgK, OpArgK, iABC) /*RAVI_ARRAYSET_IIKK	A B C	R(A)[Kst(B)] := Kst(C)				*/
 ,opmode(0, 0, OpArgK, OpArgR, iABC) /*RAVI_ARRAYSET_IIKR	A B C	R(A)[Kst(B)] := R(C)				*/
 ,opmode(0, 0, OpArgR, OpArgK, iABC) /*RAVI_ARRAYSET_IIRK	A B C	R(A)[R(B)] := Kst(C)				*/
 ,opmode(0, 0, OpArgR, OpArgR, iABC) /*RAVI_ARRAYSET_IIRR	A B C	R(A)[R(B)] := R(C)				*/
 ,opmode(0, 0, OpArgK, OpArgK, iABC) /*RAVI_ARRAYSET_IFKK	A B C	R(A)[Kst(B)] := Kst(C)				*/
 ,opmode(0, 0, OpArgK, OpArgR, iABC) /*RAVI_ARRAYSET_IFKR	A B C	R(A)[Kst(B)] := R(C)				*/
 ,opmode(0, 0, OpArgR, OpArgK, iABC) /*RAVI_ARRAYSET_IFRK	A B C	R(A)[R(B)] := Kst(C)				*/
 ,opmode(0, 0, OpArgR, OpArgR, iABC) /*RAVI_ARRAYSET_IFRR	A B C	R(A)[R(B)] := R(C)				*/
 ,opmode(0, 0, OpArgK, OpArgK, iABC) /*RAVI_ARRAYSET_ILKK	A B C	R(A)[Kst(B)] := Kst(C)				*/
 ,opmode(0, 0, OpArgK, OpArgR, iABC) /*RAVI_ARRAYSET_ILKR	A B C	R(A)[Kst(B)] := R(C)				*/
 ,opmode(0, 0, OpArgR, OpArgK, iABC) /*RAVI_ARRAYSET_ILRK	A B C	R(A)[R(B)] := Kst(C)				*/
 ,opmode(0, 0, OpArgR, OpArgR, iABC) /*RAVI_ARRAYSET_ILRR	A B C	R(A)[R(B)] := R(C)				*/

 ,opmode(0, 1, OpArgU, OpArgU, iABC) /* OP_RAVI_TOINT  A R(A) := toint(R(A)) */
 ,opmode(0, 1, OpArgU, OpArgU, iABC) /* OP_RAVI_TOFLT  A R(A) := tonumber(R(A)) */

 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_RAVI_MOVEI	A B	R(A) := tointeger(R(B))					*/
 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_RAVI_MOVEF	A B	R(A) := tonumber(R(B))					*/

};

LUAI_DDEF const lu_byte luaP_optypes[NUM_OPCODES] = {
  LUA_TNONE		/* OP_MOVE */
  , LUA_TNONE		/* OP_LOADK */
  , LUA_TNONE		/* OP_LOADKX */
  , LUA_TNONE		/* OP_LOADBOOL */
  , LUA_TNONE		/* OP_LOADNIL */
  , LUA_TNONE		/* OP_GETUPVAL */
  , LUA_TNONE		/* OP_GETTABUP */
  , LUA_TNONE		/* OP_GETTABLE */
  , LUA_TNONE		/* OP_SETTABUP */
  , LUA_TNONE		/* OP_SETUPVAL */
  , LUA_TNONE		/* OP_SETTABLE */
  , LUA_TNONE		/* OP_NEWTABLE */
  , LUA_TNONE		/* OP_SELF */
  , LUA_TNONE		/* OP_ADD */
  , LUA_TNONE		/* OP_SUB */
  , LUA_TNONE		/* OP_MUL */
  , LUA_TNONE		/* OP_MOD */
  , LUA_TNONE		/* OP_POW */
  , LUA_TNONE		/* OP_DIV */
  , LUA_TNONE		/* OP_IDIV */
  , LUA_TNONE		/* OP_BAND */
  , LUA_TNONE		/* OP_BOR */
  , LUA_TNONE		/* OP_BXOR */
  , LUA_TNONE		/* OP_SHL */
  , LUA_TNONE		/* OP_SHR */
  , LUA_TNONE		/* OP_UNM */
  , LUA_TNONE		/* OP_BNOT */
  , LUA_TNONE		/* OP_NOT */
  , LUA_TNONE		/* OP_LEN */
  , LUA_TNONE		/* OP_CONCAT */
  , LUA_TNONE		/* OP_JMP */
  , LUA_TNONE		/* OP_EQ */
  , LUA_TNONE		/* OP_LT */
  , LUA_TNONE		/* OP_LE */
  , LUA_TNONE		/* OP_TEST */
  , LUA_TNONE		/* OP_TESTSET */
  , LUA_TNONE		/* OP_CALL */
  , LUA_TNONE		/* OP_TAILCALL */
  , LUA_TNONE		/* OP_RETURN */
  , LUA_TNONE		/* OP_FORLOOP */
  , LUA_TNONE		/* OP_FORPREP */
  , LUA_TNONE		/* OP_TFORCALL */
  , LUA_TNONE		/* OP_TFORLOOP */
  , LUA_TNONE		/* OP_SETLIST */
  , LUA_TNONE		/* OP_CLOSURE */
  , LUA_TNONE		/* OP_VARARG */
  , LUA_TNONE		    /* OP_EXTRAARG */

  , LUA_TNUMFLT/* OP_RAVI_UNMF	A B	    R(A) := -R(B) floating point      */
  , LUA_TNUMINT/* OP_RAVI_UNMI A B     R(A) := -R(B) integer */

  , LUA_TNUMFLT /*RAVI_ADDFFKK	A B C	R(A) := Kst(B) + Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDFFKR	A B C	R(A) := Kst(B) + R(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDFFRK	A B C	R(A) := R(B) + Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDFFRR	A B C	R(A) := R(B) + R(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDFIKK	A B C	R(A) := Kst(B) + Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDFIKR	A B C	R(A) := Kst(B) + R(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDFIRK	A B C	R(A) := R(B) + Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDFIRR	A B C	R(A) := R(B) + R(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDIFKK	A B C	R(A) := Kst(B) + Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDIFKR	A B C	R(A) := Kst(B) + R(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDIFRK	A B C	R(A) := R(B) + Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_ADDIFRR	A B C	R(A) := R(B) + R(C)				*/
  , LUA_TNUMINT /*RAVI_ADDIIKK	A B C	R(A) := Kst(B) + Kst(C)				*/
  , LUA_TNUMINT /*RAVI_ADDIIKR	A B C	R(A) := Kst(B) + R(C)				*/
  , LUA_TNUMINT /*RAVI_ADDIIRK	A B C	R(A) := R(B) + Kst(C)				*/
  , LUA_TNUMINT /*RAVI_ADDIIRR	A B C	R(A) := R(B) + R(C)				*/

  , LUA_TNUMFLT /*RAVI_SUBFFKK	A B C	R(A) := Kst(B) - Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBFFKR	A B C	R(A) := Kst(B) - R(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBFFRK	A B C	R(A) := R(B) - Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBFFRR	A B C	R(A) := R(B) - R(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBFIKK	A B C	R(A) := Kst(B) - Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBFIKR	A B C	R(A) := Kst(B) - R(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBFIRK	A B C	R(A) := R(B) - Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBFIRR	A B C	R(A) := R(B) - R(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBIFKK	A B C	R(A) := Kst(B) - Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBIFKR	A B C	R(A) := Kst(B) - R(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBIFRK	A B C	R(A) := R(B) - Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_SUBIFRR	A B C	R(A) := R(B) - R(C)				*/
  , LUA_TNUMINT /*RAVI_SUBIIKK	A B C	R(A) := Kst(B) - Kst(C)				*/
  , LUA_TNUMINT /*RAVI_SUBIIKR	A B C	R(A) := Kst(B) - R(C)				*/
  , LUA_TNUMINT /*RAVI_SUBIIRK	A B C	R(A) := R(B) - Kst(C)				*/
  , LUA_TNUMINT /*RAVI_SUBIIRR	A B C	R(A) := R(B) - R(C)				*/

  , LUA_TNUMFLT /*RAVI_MULFFKK	A B C	R(A) := Kst(B) * Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_MULFFKR	A B C	R(A) := Kst(B) * R(C)				*/
  , LUA_TNUMFLT /*RAVI_MULFFRK	A B C	R(A) := R(B) * Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_MULFFRR	A B C	R(A) := R(B) * R(C)				*/
  , LUA_TNUMFLT /*RAVI_MULFIKK	A B C	R(A) := Kst(B) * Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_MULFIKR	A B C	R(A) := Kst(B) * R(C)				*/
  , LUA_TNUMFLT /*RAVI_MULFIRK	A B C	R(A) := R(B) * Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_MULFIRR	A B C	R(A) := R(B) * R(C)				*/
  , LUA_TNUMFLT /*RAVI_MULIFKK	A B C	R(A) := Kst(B) * Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_MULIFKR	A B C	R(A) := Kst(B) * R(C)				*/
  , LUA_TNUMFLT /*RAVI_MULIFRK	A B C	R(A) := R(B) * Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_MULIFRR	A B C	R(A) := R(B) * R(C)				*/
  , LUA_TNUMINT /*RAVI_MULIIKK	A B C	R(A) := Kst(B) * Kst(C)				*/
  , LUA_TNUMINT /*RAVI_MULIIKR	A B C	R(A) := Kst(B) * R(C)				*/
  , LUA_TNUMINT /*RAVI_MULIIRK	A B C	R(A) := R(B) * Kst(C)				*/
  , LUA_TNUMINT /*RAVI_MULIIRR	A B C	R(A) := R(B) * R(C)				*/

  , LUA_TNUMFLT /*RAVI_DIVFFKK	A B C	R(A) := Kst(B) / Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVFFKR	A B C	R(A) := Kst(B) / R(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVFFRK	A B C	R(A) := R(B) / Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVFFRR	A B C	R(A) := R(B) / R(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVFIKK	A B C	R(A) := Kst(B) / Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVFIKR	A B C	R(A) := Kst(B) / R(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVFIRK	A B C	R(A) := R(B) / Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVFIRR	A B C	R(A) := R(B) / R(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVIFKK	A B C	R(A) := Kst(B) / Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVIFKR	A B C	R(A) := Kst(B) / R(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVIFRK	A B C	R(A) := R(B) / Kst(C)				*/
  , LUA_TNUMFLT /*RAVI_DIVIFRR	A B C	R(A) := R(B) / R(C)				*/
  , LUA_TNUMINT /*RAVI_DIVIIKK	A B C	R(A) := Kst(B) / Kst(C)				*/
  , LUA_TNUMINT /*RAVI_DIVIIKR	A B C	R(A) := Kst(B) / R(C)				*/
  , LUA_TNUMINT /*RAVI_DIVIIRK	A B C	R(A) := R(B) / Kst(C)				*/
  , LUA_TNUMINT /*RAVI_DIVIIRR	A B C	R(A) := R(B) / R(C)				*/

  , LUA_TNONE /*RAVI_EQFFKK	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_EQFFKR	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_EQFFRK	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_EQFFRR	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_EQIIKK	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_EQIIKR	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_EQIIRK	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_EQIIRR	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/

  , LUA_TNONE /*RAVI_LTFFKK	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LTFFKR	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LTFFRK	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LTFFRR	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LTIIKK	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LTIIKR	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LTIIRK	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LTIIRR	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/

  , LUA_TNONE /*RAVI_LEFFKK	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LEFFKR	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LEFFRK	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LEFFRR	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LEIIKK	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LEIIKR	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LEIIRK	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
  , LUA_TNONE /*RAVI_LEIIRR	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/

  , LUA_TSTRING /*RAVI_ARRAYGET_SIK	A B C	R(A) := R(B)[Kst(C)]				*/
  , LUA_TSTRING /*RAVI_ARRAYGET_SIR	A B C	R(A) := R(B)[R(C)]				*/
  , LUA_TNUMINT /*RAVI_ARRAYGET_IIK	A B C	R(A) := R(B)[Kst(C)]				*/
  , LUA_TNUMINT /*RAVI_ARRAYGET_IIR	A B C	R(A) := R(B)[R(C)]				*/
  , LUA_TNUMFLT /*RAVI_ARRAYGET_FIK	A B C	R(A) := R(B)[Kst(C)]				*/
  , LUA_TNUMFLT /*RAVI_ARRAYGET_FIR	A B C	R(A) := R(B)[R(C)]				*/
  , LUA_TFUNCTION /*RAVI_ARRAYGET_LIK	A B C	R(A) := R(B)[Kst(C)]				*/
  , LUA_TFUNCTION /*RAVI_ARRAYGET_LIR	A B C	R(A) := R(B)[R(C)]				*/

  , LUA_TNONE /*RAVI_ARRAYSET_ISKK	A B C	R(A)[Kst(B)] := Kst(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_ISKR	A B C	R(A)[Kst(B)] := R(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_ISRK	A B C	R(A)[R(B)] := Kst(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_ISRR	A B C	R(A)[R(B)] := R(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_IIKK	A B C	R(A)[Kst(B)] := Kst(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_IIKR	A B C	R(A)[Kst(B)] := R(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_IIRK	A B C	R(A)[R(B)] := Kst(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_IIRR	A B C	R(A)[R(B)] := R(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_IFKK	A B C	R(A)[Kst(B)] := Kst(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_IFKR	A B C	R(A)[Kst(B)] := R(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_IFRK	A B C	R(A)[R(B)] := Kst(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_IFRR	A B C	R(A)[R(B)] := R(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_ILKK	A B C	R(A)[Kst(B)] := Kst(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_ILKR	A B C	R(A)[Kst(B)] := R(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_ILRK	A B C	R(A)[R(B)] := Kst(C)				*/
  , LUA_TNONE /*RAVI_ARRAYSET_ILRR	A B C	R(A)[R(B)] := R(C)				*/

  , LUA_TNUMINT /* OP_RAVI_TOINT  A R(A) := toint(R(A)) */
  , LUA_TNUMFLT /* OP_RAVI_TOFLT  A R(A) := tofloat(R(A)) */

  , LUA_TNUMINT		/* OP_RAVI_MOVEI	A B	R(A) := tointeger(R(B))					*/
  , LUA_TNUMFLT		/* OP_RAVI_MOVEF	A B	R(A) := tonumber(R(B))					*/

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
  }
  return buf;
}

const char* print_instruction(char *buf, size_t n, Instruction i) {
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
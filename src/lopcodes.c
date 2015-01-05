/*
** $Id: lopcodes.c,v 1.54 2014/11/02 19:19:04 roberto Exp $
** Opcodes for Lua virtual machine
** See Copyright Notice in lua.h
*/

#define lopcodes_c
#define LUA_CORE

#include "lprefix.h"


#include "lopcodes.h"


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

  "OP_RAVI",        /* Extension point for RAVI */

  "OP_RAVI_UNMF",  /*	A B	R(A) := -R(B) floating point      */
  "OP_RAVI_UNMI",  /*   A B R(A) := -R(B) integer */

  "OP_RAVI_ADDFFKK",/*	A B C	R(A) := Kst(B) + Kst(C)				*/
  "OP_RAVI_ADDFFKR",/*	A B C	R(A) := Kst(B) + R(C)				*/
  "OP_RAVI_ADDFFRK",/*	A B C	R(A) := R(B) + Kst(C)				*/
  "OP_RAVI_ADDFFRR",/*	A B C	R(A) := R(B) + R(C)				*/
  "OP_RAVI_ADDFIKK",/*	A B C	R(A) := Kst(B) + Kst(C)				*/
  "OP_RAVI_ADDFIKR",/*	A B C	R(A) := Kst(B) + R(C)				*/
  "OP_RAVI_ADDFIRK",/*	A B C	R(A) := R(B) + Kst(C)				*/
  "OP_RAVI_ADDFIRR",/*	A B C	R(A) := R(B) + R(C)				*/
  "OP_RAVI_ADDIFKK",/*	A B C	R(A) := Kst(B) + Kst(C)				*/
  "OP_RAVI_ADDIFKR",/*	A B C	R(A) := Kst(B) + R(C)				*/
  "OP_RAVI_ADDIFRK",/*	A B C	R(A) := R(B) + Kst(C)				*/
  "OP_RAVI_ADDIFRR",/*	A B C	R(A) := R(B) + R(C)				*/
  "OP_RAVI_ADDIIKK",/*	A B C	R(A) := Kst(B) + Kst(C)				*/
  "OP_RAVI_ADDIIKR",/*	A B C	R(A) := Kst(B) + R(C)				*/
  "OP_RAVI_ADDIIRK",/*	A B C	R(A) := R(B) + Kst(C)				*/
  "OP_RAVI_ADDIIRR",/*	A B C	R(A) := R(B) + R(C)				*/

  "OP_RAVI_SUBFFKK",/*	A B C	R(A) := Kst(B) - Kst(C)				*/
  "OP_RAVI_SUBFFKR",/*	A B C	R(A) := Kst(B) - R(C)				*/
  "OP_RAVI_SUBFFRK",/*	A B C	R(A) := R(B) - Kst(C)				*/
  "OP_RAVI_SUBFFRR",/*	A B C	R(A) := R(B) - R(C)				*/
  "OP_RAVI_SUBFIKK",/*	A B C	R(A) := Kst(B) - Kst(C)				*/
  "OP_RAVI_SUBFIKR",/*	A B C	R(A) := Kst(B) - R(C)				*/
  "OP_RAVI_SUBFIRK",/*	A B C	R(A) := R(B) - Kst(C)				*/
  "OP_RAVI_SUBFIRR",/*	A B C	R(A) := R(B) - R(C)				*/
  "OP_RAVI_SUBIFKK",/*	A B C	R(A) := Kst(B) - Kst(C)				*/
  "OP_RAVI_SUBIFKR",/*	A B C	R(A) := Kst(B) - R(C)				*/
  "OP_RAVI_SUBIFRK",/*	A B C	R(A) := R(B) - Kst(C)				*/
  "OP_RAVI_SUBIFRR",/*	A B C	R(A) := R(B) - R(C)				*/
  "OP_RAVI_SUBIIKK",/*	A B C	R(A) := Kst(B) - Kst(C)				*/
  "OP_RAVI_SUBIIKR",/*	A B C	R(A) := Kst(B) - R(C)				*/
  "OP_RAVI_SUBIIRK",/*	A B C	R(A) := R(B) - Kst(C)				*/
  "OP_RAVI_SUBIIRR",/*	A B C	R(A) := R(B) - R(C)				*/

  "OP_RAVI_MULFFKK",/*	A B C	R(A) := Kst(B) * Kst(C)				*/
  "OP_RAVI_MULFFKR",/*	A B C	R(A) := Kst(B) * R(C)				*/
  "OP_RAVI_MULFFRK",/*	A B C	R(A) := R(B) * Kst(C)				*/
  "OP_RAVI_MULFFRR",/*	A B C	R(A) := R(B) * R(C)				*/
  "OP_RAVI_MULFIKK",/*	A B C	R(A) := Kst(B) * Kst(C)				*/
  "OP_RAVI_MULFIKR",/*	A B C	R(A) := Kst(B) * R(C)				*/
  "OP_RAVI_MULFIRK",/*	A B C	R(A) := R(B) * Kst(C)				*/
  "OP_RAVI_MULFIRR",/*	A B C	R(A) := R(B) * R(C)				*/
  "OP_RAVI_MULIFKK",/*	A B C	R(A) := Kst(B) * Kst(C)				*/
  "OP_RAVI_MULIFKR",/*	A B C	R(A) := Kst(B) * R(C)				*/
  "OP_RAVI_MULIFRK",/*	A B C	R(A) := R(B) * Kst(C)				*/
  "OP_RAVI_MULIFRR",/*	A B C	R(A) := R(B) * R(C)				*/
  "OP_RAVI_MULIIKK",/*	A B C	R(A) := Kst(B) * Kst(C)				*/
  "OP_RAVI_MULIIKR",/*	A B C	R(A) := Kst(B) * R(C)				*/
  "OP_RAVI_MULIIRK",/*	A B C	R(A) := R(B) * Kst(C)				*/
  "OP_RAVI_MULIIRR",/*	A B C	R(A) := R(B) * R(C)				*/

  "OP_RAVI_DIVFFKK",/*	A B C	R(A) := Kst(B) / Kst(C)				*/
  "OP_RAVI_DIVFFKR",/*	A B C	R(A) := Kst(B) / R(C)				*/
  "OP_RAVI_DIVFFRK",/*	A B C	R(A) := R(B) / Kst(C)				*/
  "OP_RAVI_DIVFFRR",/*	A B C	R(A) := R(B) / R(C)				*/
  "OP_RAVI_DIVFIKK",/*	A B C	R(A) := Kst(B) / Kst(C)				*/
  "OP_RAVI_DIVFIKR",/*	A B C	R(A) := Kst(B) / R(C)				*/
  "OP_RAVI_DIVFIRK",/*	A B C	R(A) := R(B) / Kst(C)				*/
  "OP_RAVI_DIVFIRR",/*	A B C	R(A) := R(B) / R(C)				*/
  "OP_RAVI_DIVIFKK",/*	A B C	R(A) := Kst(B) / Kst(C)				*/
  "OP_RAVI_DIVIFKR",/*	A B C	R(A) := Kst(B) / R(C)				*/
  "OP_RAVI_DIVIFRK",/*	A B C	R(A) := R(B) / Kst(C)				*/
  "OP_RAVI_DIVIFRR",/*	A B C	R(A) := R(B) / R(C)				*/
  "OP_RAVI_DIVIIKK",/*	A B C	R(A) := Kst(B) / Kst(C)				*/
  "OP_RAVI_DIVIIKR",/*	A B C	R(A) := Kst(B) / R(C)				*/
  "OP_RAVI_DIVIIRK",/*	A B C	R(A) := R(B) / Kst(C)				*/
  "OP_RAVI_DIVIIRR",/*	A B C	R(A) := R(B) / R(C)				*/

  "OP_RAVI_EQFFKK",/*	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_EQFFKR",/*	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
  "OP_RAVI_EQFFRK",/*	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_EQFFRR",/*	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/
  "OP_RAVI_EQFIKK",/*	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_EQFIKR",/*	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
  "OP_RAVI_EQFIRK",/*	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_EQFIRR",/*	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/
  "OP_RAVI_EQIFKK",/*	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_EQIFKR",/*	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
  "OP_RAVI_EQIFRK",/*	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_EQIFRR",/*	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/
  "OP_RAVI_EQIIKK",/*	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_EQIIKR",/*	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
  "OP_RAVI_EQIIRK",/*	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_EQIIRR",/*	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/

  "OP_RAVI_LTFFKK",/*	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LTFFKR",/*	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
  "OP_RAVI_LTFFRK",/*	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LTFFRR",/*	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/
  "OP_RAVI_LTFIKK",/*	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LTFIKR",/*	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
  "OP_RAVI_LTFIRK",/*	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LTFIRR",/*	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/
  "OP_RAVI_LTIFKK",/*	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LTIFKR",/*	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
  "OP_RAVI_LTIFRK",/*	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LTIFRR",/*	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/
  "OP_RAVI_LTIIKK",/*	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LTIIKR",/*	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
  "OP_RAVI_LTIIRK",/*	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LTIIRR",/*	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/

  "OP_RAVI_LEFFKK",/*	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LEFFKR",/*	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
  "OP_RAVI_LEFFRK",/*	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LEFFRR",/*	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/
  "OP_RAVI_LEFIKK",/*	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LEFIKR",/*	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
  "OP_RAVI_LEFIRK",/*	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LEFIRR",/*	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/
  "OP_RAVI_LEIFKK",/*	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LEIFKR",/*	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
  "OP_RAVI_LEIFRK",/*	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LEIFRR",/*	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/
  "OP_RAVI_LEIIKK",/*	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LEIIKR",/*	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
  "OP_RAVI_LEIIRK",/*	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
  "OP_RAVI_LEIIRR",/*	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/

  "OP_RAVI_ARRAYGET_SIK",/*	A B C	R(A) := R(B)[Kst(C)]				*/
  "OP_RAVI_ARRAYGET_SIR",/*	A B C	R(A) := R(B)[R(C)]				*/
  "OP_RAVI_ARRAYGET_IIK",/*	A B C	R(A) := R(B)[Kst(C)]				*/
  "OP_RAVI_ARRAYGET_IIR",/*	A B C	R(A) := R(B)[R(C)]				*/
  "OP_RAVI_ARRAYGET_FIK",/*	A B C	R(A) := R(B)[Kst(C)]				*/
  "OP_RAVI_ARRAYGET_FIR",/*	A B C	R(A) := R(B)[R(C)]				*/
  "OP_RAVI_ARRAYGET_LIK",/*	A B C	R(A) := R(B)[Kst(C)]				*/
  "OP_RAVI_ARRAYGET_LIR",/*	A B C	R(A) := R(B)[R(C)]				*/

  "OP_RAVI_ARRAYSET_ISKK",/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
  "OP_RAVI_ARRAYSET_ISKR",/*	A B C	R(A)[Kst(B)] := R(C)				*/
  "OP_RAVI_ARRAYSET_ISRK",/*	A B C	R(A)[R(B)] := Kst(C)				*/
  "OP_RAVI_ARRAYSET_ISRR",/*	A B C	R(A)[R(B)] := R(C)				*/
  "OP_RAVI_ARRAYSET_IIKK",/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
  "OP_RAVI_ARRAYSET_IIKR",/*	A B C	R(A)[Kst(B)] := R(C)				*/
  "OP_RAVI_ARRAYSET_IIRK",/*	A B C	R(A)[R(B)] := Kst(C)				*/
  "OP_RAVI_ARRAYSET_IIRR",/*	A B C	R(A)[R(B)] := R(C)				*/
  "OP_RAVI_ARRAYSET_IFKK",/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
  "OP_RAVI_ARRAYSET_IFKR",/*	A B C	R(A)[Kst(B)] := R(C)				*/
  "OP_RAVI_ARRAYSET_IFRK",/*	A B C	R(A)[R(B)] := Kst(C)				*/
  "OP_RAVI_ARRAYSET_IFRR",/*	A B C	R(A)[R(B)] := R(C)				*/
  "OP_RAVI_ARRAYSET_ILKK",/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
  "OP_RAVI_ARRAYSET_ILKR",/*	A B C	R(A)[Kst(B)] := R(C)				*/
  "OP_RAVI_ARRAYSET_ILRK",/*	A B C	R(A)[R(B)] := Kst(C)				*/
  "OP_RAVI_ARRAYSET_ILRR",/*	A B C	R(A)[R(B)] := R(C)				*/

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

 ,opmode(0, 0, OpArgU, OpArgU, iAx) /* OP_RAVI Extension point for RAVI */

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
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_EQFIKK	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_EQFIKR	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_EQFIRK	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_EQFIRR	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_EQIFKK	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_EQIFKR	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_EQIFRK	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_EQIFRR	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_EQIIKK	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_EQIIKR	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_EQIIRK	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_EQIIRR	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/

 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LTFFKK	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LTFFKR	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LTFFRK	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LTFFRR	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LTFIKK	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LTFIKR	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LTFIRK	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LTFIRR	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LTIFKK	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LTIFKR	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LTIFRK	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LTIFRR	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LTIIKK	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LTIIKR	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LTIIRK	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LTIIRR	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/

 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LEFFKK	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LEFFKR	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LEFFRK	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LEFFRR	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LEFIKK	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LEFIKR	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LEFIRK	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LEFIRR	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgK, iABC) /*RAVI_LEIFKK	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgK, OpArgR, iABC) /*RAVI_LEIFKR	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgK, iABC) /*RAVI_LEIFRK	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
 ,opmode(1, 0, OpArgR, OpArgR, iABC) /*RAVI_LEIFRR	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/
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

};


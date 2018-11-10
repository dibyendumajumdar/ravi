/*
** $Id: lopcodes.c,v 1.55.1.1 2017/04/19 17:20:42 roberto Exp $
** Opcodes for Lua virtual machine
** See Copyright Notice in lua.h
*/

/*
** Portions Copyright (C) 2015-2017 Dibyendu Majumdar
*/


#define lopcodes_c
#define LUA_CORE

#include "lprefix.h"


#include <stddef.h>

#include "lopcodes.h"
#include "lobject.h"
#include "lstate.h"
#include "ldebug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

  "NEW_IARRAY", /* A R(A) := array of int */
  "NEW_FARRAY", /* A R(A) := array of float */

  "LOADIZ", /*  A R(A) := tointeger(0)		*/
  "LOADFZ", /*  A R(A) := tonumber(0)		*/

  "UNMF",   /*	A B	R(A) := -R(B) floating point      */
  "UNMI",   /*  A B R(A) := -R(B) integer */

  "ADDFF",  /*	A B C	R(A) := RK(B) + RK(C)		*/
  "ADDFI",  /*	A B C	R(A) := RK(B) + RK(C)		*/
  "ADDII",  /*	A B C	R(A) := RK(B) + RK(C)	  */

  "SUBFF",  /*	A B C	R(A) := RK(B) - RK(C)		*/
  "SUBFI",  /*	A B C	R(A) := RK(B) - RK(C)		*/
  "SUBIF",  /*	A B C	R(A) := RK(B) - RK(C)	  */
  "SUBII",  /*	A B C	R(A) := RK(B) - RK(C)		*/

  "MULFF",  /*	A B C	R(A) := RK(B) * RK(C)		*/
  "MULFI",  /*	A B C	R(A) := RK(B) * RK(C)		*/
  "MULII",  /*	A B C	R(A) := RK(B) * RK(C)	  */

  "DIVFF",  /*	A B C	R(A) := RK(B) / RK(C)		*/
  "DIVFI",  /*	A B C	R(A) := RK(B) / RK(C)		*/
  "DIVIF",  /*	A B C	R(A) := RK(B) / RK(C)	  */
  "DIVII",  /*	A B C	R(A) := RK(B) / RK(C)		*/

  "TOINT", /* A R(A) := toint(R(A)) */
  "TOFLT", /* A R(A) := tofloat(R(A)) */
  "TOIARRAY", /* A R(A) := to_arrayi(R(A)) */
  "TOFARRAY", /* A R(A) := to_arrayf(R(A)) */
  "TOTAB",     /* A R(A) := to_table(R(A)) */
  "TOSTRING",
  "TOCLOSURE",
  "TOTYPE",

  "MOVEI",  /*	A B	R(A) := R(B)					*/
  "MOVEF",  /*	A B	R(A) := R(B)					*/
  "MOVEIARRAY", /* A B R(A) := R(B), check R(B) is array of int */
  "MOVEFARRAY", /* A B R(A) := R(B), check R(B) is array of floats */
  "MOVETAB",   /* A B R(A) := R(B), check R(B) is a table */

  "IARRAY_GET",/*	A B C	R(A) := R(B)[RK(C)] where R(B) is array of integers and RK(C) is int */
  "FARRAY_GET",/*	A B C	R(A) := R(B)[RK(C)] where R(B) is array of floats and RK(C) is int */

  "IARRAY_SET",/*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of ints, and RK(C) is an int */
  "FARRAY_SET",/*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of floats, and RK(C) is an float */
  "IARRAY_SETI",/*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of ints, and RK(C) is an int */
  "FARRAY_SETF",/*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of floats, and RK(C) is an float */

  "FORLOOP_IP",
  "FORLOOP_I1",
  "FORPREP_IP",
  "FORPREP_I1",

  "SETUPVALI", /*	A B	UpValue[B] := tointeger(R(A))			*/
  "SETUPVALF", /*	A B	UpValue[B] := tonumber(R(A))			*/
  "SETUPVAL_IARRAY",  /*	A B	UpValue[B] := toarrayint(R(A))			*/
  "SETUPVAL_FARRAY",  /*	A B	UpValue[B] := toarrayflt(R(A))			*/
  "SETUPVALT", /*	A B	UpValue[B] := to_table(R(A))			*/

  "BAND_II",/*	A B C	R(A) := RK(B) & RK(C)				*/
  "BOR_II", /*	A B C	R(A) := RK(B) | RK(C)				*/
  "BXOR_II",/*	A B C	R(A) := RK(B) ~ RK(C)				*/
  "SHL_II", /*	A B C	R(A) := RK(B) << RK(C)				*/
  "SHR_II", /*	A B C	R(A) := RK(B) >> RK(C)				*/
  "BNOT_I", /*	A B	R(A) := ~R(B)					*/

  "EQ_II",  /*	A B C	if ((RK(B) == RK(C)) ~= A) then pc++		*/
  "EQ_FF",  /*	A B C	if ((RK(B) == RK(C)) ~= A) then pc++		*/
  "LT_II",  /*	A B C	if ((RK(B) <  RK(C)) ~= A) then pc++		*/
  "LT_FF",  /*	A B C	if ((RK(B) <  RK(C)) ~= A) then pc++		*/
  "LE_II",  /*	A B C	if ((RK(B) <= RK(C)) ~= A) then pc++		*/
  "LE_FF",  /*	A B C	if ((RK(B) <= RK(C)) ~= A) then pc++		*/
  
  "TABLE_GETFIELD", /*	A B C	R(A) := R(B)[RK(C)], string key   */
  "TABLE_SETFIELD", /*	A B C	R(A)[RK(B)] := RK(C), string key  */
  "TABLE_SELF_SK",    /* A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)]		*/

  "GETI", /*	A B C	R(A) := R(B)[RK(C)], integer key	*/
  "SETI", /*	A B C	R(A)[RK(B)] := RK(C), integer key	*/
  "GETFIELD", /* _SK */ /*	A B C	R(A) := R(B)[RK(C)], string key   */
  "SELF_SK",    /* _SK*/ /* A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)]		*/
  "SETFIELD", /*_SK */ /*	A B C	R(A)[RK(B)] := RK(C), string key  */
  "GETTABUP_SK",
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
 ,opmode(0, 0, OpArgU, OpArgU, iAx)		  /* OP_EXTRAARG */

 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_NEW_IARRAY A R(A) := array of int */
 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_NEW_FARRAY A R(A) := array of float */

 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_LOADIZ A R(A) := tointeger(0)		*/
 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_LOADFZ A R(A) := tonumber(0)		*/

 ,opmode(0, 1, OpArgR, OpArgN, iABC)    /* OP_RAVI_UNMF A B	R(A) := -R(B) floating point      */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)    /* OP_RAVI_UNMI A B R(A) := -R(B) integer */

 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_ADDFF	A B C	R(A) := RK(B) + RK(C)		*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_ADDFI	A B C	R(A) := RK(B) + RK(C)		*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_ADDII	A B C	R(A) := RK(B) + RK(C)	  */

 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_SUBFF	A B C	R(A) := RK(B) - RK(C)		*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_SUBFI	A B C	R(A) := RK(B) - RK(C)		*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_SUBIF	A B C	R(A) := RK(B) - RK(C)	  */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_SUBII	A B C	R(A) := RK(B) - RK(C)		*/

 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_MULFF	A B C	R(A) := RK(B) * RK(C)		*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_MULFI	A B C	R(A) := RK(B) * RK(C)		*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_MULII	A B C	R(A) := RK(B) * RK(C)	  */

 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_DIVFF	A B C	R(A) := RK(B) / RK(C)		*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_DIVFI	A B C	R(A) := RK(B) / RK(C)		*/
 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_DIVIF	A B C	R(A) := RK(B) / RK(C)	  */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)    /* OP_RAVI_DIVII	A B C	R(A) := RK(B) / RK(C)		*/

 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_TOINT  A R(A) := toint(R(A)) */
 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_TOFLT  A R(A) := tonumber(R(A)) */
 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_TOIARRAY A R(A) := check_array_of_int(R(A)) */
 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_TOFARRAY A R(A) := check_array_of_float(R(A)) */
 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_TOTAB A R(A) := check_table(R(A)) */
 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_TOSTRING */
 ,opmode(0, 1, OpArgN, OpArgN, iABC)    /* OP_RAVI_TOCLOSURE */
 ,opmode(0, 1, OpArgK, OpArgN, iABx)	/* OP_RAVI_TOTYPE */

 ,opmode(0, 1, OpArgR, OpArgN, iABC)    /* OP_RAVI_MOVEI	A B	R(A) := tointeger(R(B))	*/
 ,opmode(0, 1, OpArgR, OpArgN, iABC)    /* OP_RAVI_MOVEF	A B	R(A) := tonumber(R(B)) */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)    /* OP_RAVI_MOVEIARRAY A B R(A) := R(B), check R(B) is array of int */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)    /* OP_RAVI_MOVEFARRAY A B R(A) := R(B), check R(B) is array of floats */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)    /* OP_RAVI_MOVETAB A B R(A) := R(B), check R(B) is a table */

 ,opmode(0, 1, OpArgR, OpArgK, iABC)    /* OP_RAVI_IARRAY_GET A B C	R(A) := R(B)[RK(C)] where R(B) is array of integers and RK(C) is int */
 ,opmode(0, 1, OpArgR, OpArgK, iABC)    /* OP_RAVI_FARRAY_GET A B C	R(A) := R(B)[RK(C)] where R(B) is array of floats and RK(C) is int */

 ,opmode(0, 0, OpArgK, OpArgK, iABC)    /* OP_RAVI_IARRAY_SET A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of ints, and RK(C) is an int */
 ,opmode(0, 0, OpArgK, OpArgK, iABC)    /* OP_RAVI_FARRAY_SET A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of floats, and RK(C) is an float */
 ,opmode(0, 0, OpArgK, OpArgK, iABC)    /* OP_RAVI_IARRAY_SETI A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of ints, and RK(C) is an int */
 ,opmode(0, 0, OpArgK, OpArgK, iABC)    /* OP_RAVI_FARRAY_SETF A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of floats, and RK(C) is an float */

 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_RAVI_FORLOOP_IP */
 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_RAVI_FORLOOP_I1 */
 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_RAVI_FORPREP_IP */
 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_RAVI_FORPREP_I1 */

 ,opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_RAVI_SETUPVALI */
 ,opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_RAVI_SETUPVALF */
 ,opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_RAVI_SETUPVAL_IARRAY */
 ,opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_RAVI_SETUPVAL_FARRAY */
 ,opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_RAVI_SETUPVALT */

 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_RAVI_BAND_II */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_RAVI_BOR_II */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_RAVI_BXOR_II */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_RAVI_SHL_II */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_RAVI_SHR_II */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_RAVI_BNOT_I */

 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_RAVI_EQ_II */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_RAVI_EQ_FF */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_RAVI_LT_II */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_RAVI_LT_FF */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_RAVI_LE_II */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_RAVI_LE_FF */

 ,opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_RAVI_TABLE_GETFIELD */
 ,opmode(0, 0, OpArgK, OpArgK, iABC)		/* OP_RAVI_TABLE_SETFIELD */
 ,opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_RAVI_TABLE_SELF_SK */

 ,opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_RAVI_GETI */
 ,opmode(0, 0, OpArgK, OpArgK, iABC)		/* OP_RAVI_SETI */
 ,opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_RAVI_GETFIELD */
 ,opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_RAVI_SELF_SK */
 ,opmode(0, 0, OpArgK, OpArgK, iABC)		/* OP_RAVI_SETFIELD */
 ,opmode(0, 1, OpArgU, OpArgK, iABC)		/* OP_RAVI_GETTABUP_SK */
};


#define UPVALNAME(x) ((f->upvalues[x].name) ? getstr(f->upvalues[x].name) : "-")
#define MYK(x)		(-1-(x))
#define VOID(p)		((const void*)(p))

const char* raviP_instruction_to_str(char *buf, size_t n, Instruction i) {
  OpCode o = GET_OPCODE(i);
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
      snprintf(buf + strlen(buf), n - strlen(buf), " B=%d", (getBMode(o) == OpArgK && ISK(b)) ? (MYK(INDEXK(b))) : b);
    if (getCMode(o) != OpArgN)
      snprintf(buf + strlen(buf), n - strlen(buf), " C=%d", (getCMode(o) == OpArgK && ISK(c)) ? (MYK(INDEXK(c))) : c);
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

static void PrintString(const TString* ts)
{
 const char* s=getstr(ts);
 size_t i,n=tsslen(ts);
 printf("%c",'"');
 for (i=0; i<n; i++)
 {
  int c=(int)(unsigned char)s[i];
  switch (c)
  {
   case '"':  printf("\\\""); break;
   case '\\': printf("\\\\"); break;
   case '\a': printf("\\a"); break;
   case '\b': printf("\\b"); break;
   case '\f': printf("\\f"); break;
   case '\n': printf("\\n"); break;
   case '\r': printf("\\r"); break;
   case '\t': printf("\\t"); break;
   case '\v': printf("\\v"); break;
   default:	if (isprint(c))
   			printf("%c",c);
		else
			printf("\\%03d",c);
  }
 }
 printf("%c",'"');
}

static void PrintConstant(const Proto* f, int i)
{
 const TValue* o=&f->k[i];
 switch (ttype(o))
 {
  case LUA_TNIL:
	printf("nil");
	break;
  case LUA_TBOOLEAN:
	printf(bvalue(o) ? "true" : "false");
	break;
  case LUA_TNUMFLT:
	{
	char buff[100];
	sprintf(buff,LUA_NUMBER_FMT,fltvalue(o));
	printf("%s",buff);
	if (buff[strspn(buff,"-0123456789")]=='\0') printf(".0");
	break;
	}
  case LUA_TNUMINT:
	printf(LUA_INTEGER_FMT,ivalue(o));
	break;
  case LUA_TSHRSTR: case LUA_TLNGSTR:
	PrintString(tsvalue(o));
	break;
  default:				/* cannot happen */
	printf("? type=%d",ttype(o));
	break;
 }
}

static void PrintCode(const Proto* f)
{
 const Instruction* code=f->code;
 int pc,n=f->sizecode;
 for (pc=0; pc<n; pc++)
 {
  Instruction i=code[pc];
  OpCode o=GET_OPCODE(i);
  int a=GETARG_A(i);
  int b=GETARG_B(i);
  int c=GETARG_C(i);
  int ax=GETARG_Ax(i);
  int bx=GETARG_Bx(i);
  int sbx=GETARG_sBx(i);
  int line=getfuncline(f,pc);
  printf("\t%d\t",pc+1);
  if (line>0) printf("[%d]\t",line); else printf("[-]\t");
  printf("%-9s\t",luaP_opnames[o]);
  switch (getOpMode(o))
  {
   case iABC:
    printf("%d",a);
    if (getBMode(o)!=OpArgN) printf(" %d", (getBMode(o) == OpArgK && ISK(b)) ? (MYK(INDEXK(b))) : b);
    if (getCMode(o)!=OpArgN) printf(" %d", (getCMode(o) == OpArgK && ISK(c)) ? (MYK(INDEXK(c))) : c);
    break;
   case iABx:
    printf("%d",a);
    if (getBMode(o)==OpArgK) printf(" %d",MYK(bx));
    if (getBMode(o)==OpArgU) printf(" %d",bx);
    break;
   case iAsBx:
    printf("%d %d",a,sbx);
    break;
   case iAx:
    printf("%d",MYK(ax));
    break;
  }
  switch (o)
  {
   case OP_LOADK:
    printf("\t; "); PrintConstant(f,bx);
    break;
   case OP_GETUPVAL:
   case OP_RAVI_SETUPVALI:
   case OP_RAVI_SETUPVALF:
   case OP_RAVI_SETUPVAL_IARRAY:
   case OP_RAVI_SETUPVAL_FARRAY:
   case OP_RAVI_SETUPVALT:
   case OP_SETUPVAL:
    printf("\t; %s",UPVALNAME(b));
    break;
   case OP_RAVI_GETTABUP_SK:
   case OP_GETTABUP:
    printf("\t; %s",UPVALNAME(b));
    if (ISK(c)) { printf(" "); PrintConstant(f,INDEXK(c)); }
    break;
   case OP_SETTABUP:
    printf("\t; %s",UPVALNAME(a));
    if (ISK(b)) { printf(" "); PrintConstant(f,INDEXK(b)); }
    if (ISK(c)) { printf(" "); PrintConstant(f,INDEXK(c)); }
    break;
   case OP_GETTABLE:
   case OP_RAVI_GETI:
   case OP_RAVI_TABLE_GETFIELD:
   case OP_RAVI_FARRAY_GET:
   case OP_RAVI_IARRAY_GET:
   case OP_SELF:
   case OP_RAVI_GETFIELD:
   case OP_RAVI_TABLE_SELF_SK:
   case OP_RAVI_SELF_SK:
    if (ISK(c)) { printf("\t; "); PrintConstant(f,INDEXK(c)); }
    break;
   case OP_SETTABLE:
   case OP_RAVI_SETI:
   case OP_RAVI_TABLE_SETFIELD:
   case OP_RAVI_SETFIELD:
   case OP_RAVI_FARRAY_SET:
   case OP_RAVI_FARRAY_SETF:
   case OP_RAVI_IARRAY_SET:
   case OP_RAVI_IARRAY_SETI:
   case OP_ADD:
   case OP_RAVI_ADDFF:
   case OP_RAVI_ADDFI:
   case OP_RAVI_ADDII:
   case OP_SUB:
   case OP_RAVI_SUBFF:
   case OP_RAVI_SUBFI:
   case OP_RAVI_SUBIF:
   case OP_RAVI_SUBII:
   case OP_MUL:
   case OP_RAVI_MULFF:
   case OP_RAVI_MULFI:
   case OP_RAVI_MULII:
   case OP_POW:
   case OP_DIV:
   case OP_RAVI_DIVFF:
   case OP_RAVI_DIVFI:
   case OP_RAVI_DIVIF:
   case OP_RAVI_DIVII:
   case OP_IDIV:
   case OP_BAND:
   case OP_RAVI_BAND_II:
   case OP_BOR:
   case OP_RAVI_BOR_II:
   case OP_BXOR:
   case OP_RAVI_BXOR_II:
   case OP_SHL:
   case OP_RAVI_SHL_II:
   case OP_SHR:
   case OP_RAVI_SHR_II:
   case OP_EQ:
   case OP_RAVI_EQ_II:
   case OP_RAVI_EQ_FF:
   case OP_LT:
   case OP_RAVI_LT_II:
   case OP_RAVI_LT_FF:
   case OP_LE:
   case OP_RAVI_LE_II:
   case OP_RAVI_LE_FF:
     if (ISK(b) || ISK(c))
    {
     printf("\t; ");
     if (ISK(b)) PrintConstant(f,INDEXK(b)); else printf("-");
     printf(" ");
     if (ISK(c)) PrintConstant(f,INDEXK(c)); else printf("-");
    }
    break;
   case OP_JMP:
   case OP_FORLOOP:
   case OP_RAVI_FORLOOP_IP:
   case OP_RAVI_FORLOOP_I1:
   case OP_FORPREP:
   case OP_RAVI_FORPREP_IP:
   case OP_RAVI_FORPREP_I1:
   case OP_TFORLOOP:
    printf("\t; to %d",sbx+pc+2);
    break;
   case OP_CLOSURE:
    printf("\t; %p",VOID(f->p[bx]));
    break;
   case OP_SETLIST:
    if (c==0) printf("\t; %d",(int)code[++pc]); else printf("\t; %d",c);
    break;
   case OP_EXTRAARG:
    printf("\t; "); PrintConstant(f,ax);
    break;
   default:
    break;
  }
  printf("\n");
 }
}

#define SS(x)	((x==1)?"":"s")
#define S(x)	(int)(x),SS(x)

static void PrintHeader(const Proto* f)
{
 const char* s=f->source ? getstr(f->source) : "=?";
 if (*s=='@' || *s=='=')
  s++;
 else if (*s==LUA_SIGNATURE[0])
  s="(bstring)";
 else
  s="(string)";
 printf("\n%s <%s:%d,%d> (%d instruction%s at %p)\n",
 	(f->linedefined==0)?"main":"function",s,
	f->linedefined,f->lastlinedefined,
	S(f->sizecode),VOID(f));
 printf("%d%s param%s, %d slot%s, %d upvalue%s, ",
	(int)(f->numparams),f->is_vararg?"+":"",SS(f->numparams),
	S(f->maxstacksize),S(f->sizeupvalues));
 printf("%d local%s, %d constant%s, %d function%s\n",
	S(f->sizelocvars),S(f->sizek),S(f->sizep));
}

static void PrintDebug(const Proto* f)
{
 int i,n;
 n=f->sizek;
 printf("constants (%d) for %p:\n",n,VOID(f));
 for (i=0; i<n; i++)
 {
  printf("\t%d\t",i+1);
  PrintConstant(f,i);
  printf("\n");
 }
 n=f->sizelocvars;
 printf("locals (%d) for %p:\n",n,VOID(f));
 for (i=0; i<n; i++)
 {
  printf("\t%d\t%s\t%d\t%d\n",
  i,getstr(f->locvars[i].varname),f->locvars[i].startpc+1,f->locvars[i].endpc+1);
 }
 n=f->sizeupvalues;
 printf("upvalues (%d) for %p:\n",n,VOID(f));
 for (i=0; i<n; i++)
 {
  printf("\t%d\t%s\t%d\t%d\n",
  i,UPVALNAME(i),f->upvalues[i].instack,f->upvalues[i].idx);
 }
}

void ravi_print_function(const Proto* f, int full)
{
  int i, n = f->sizep;
  PrintHeader(f);
  PrintCode(f);
  if (full) PrintDebug(f);
  for (i = 0; i<n; i++) ravi_print_function(f->p[i], full);
}

#define toproto(L,i) getproto(L->top+(i))

void ravi_dump_function(lua_State *L)
{
  Proto* f;
  f = toproto(L, -1);
  ravi_print_function(f, 1);
}

static void setnameval(lua_State *L, const char *name, int val) {
  lua_pushstring(L, name);
  lua_pushinteger(L, val);
  lua_settable(L, -3);
}

#define obj_at(L,k)	(L->ci->func + (k))
#define getfuncline(f,pc)	(((f)->lineinfo) ? (f)->lineinfo[pc] : -1)

static char *buildop2(Proto *p, int pc, char *buff, size_t len) {
  int line = getfuncline(p, pc);
  char tbuf[100];
  raviP_instruction_to_str(tbuf, sizeof tbuf, p->code[pc]);
  snprintf(buff, len, "(%4d) %4d - %s", line, pc, tbuf);
  return buff;
}

int ravi_list_code(lua_State *L) {
  int pc;
  Proto *p;
  p = getproto(obj_at(L, 1));
  lua_newtable(L);
  setnameval(L, "maxstack", p->maxstacksize);
  setnameval(L, "numparams", p->numparams);
  for (pc = 0; pc<p->sizecode; pc++) {
    char buff[100];
    lua_pushinteger(L, pc + 1);
    lua_pushstring(L, buildop2(p, pc, buff, sizeof buff));
    lua_settable(L, -3);
  }
  return 1;
}

int ravi_get_limits(lua_State *L) {
  lua_createtable(L, 0, 5);
  setnameval(L, "BITS_INT", LUAI_BITSINT);
  setnameval(L, "MAXARG_Ax", MAXARG_Ax);
  setnameval(L, "MAXARG_Bx", MAXARG_Bx);
  setnameval(L, "MAXARG_sBx", MAXARG_sBx);
  setnameval(L, "BITS_INT", LUAI_BITSINT);
  setnameval(L, "LFPF", LFIELDS_PER_FLUSH);
  setnameval(L, "NUM_OPCODES", NUM_OPCODES);
  return 1;
}

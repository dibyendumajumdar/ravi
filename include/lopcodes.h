/*
** $Id: lopcodes.h,v 1.149.1.1 2017/04/19 17:20:42 roberto Exp $
** Opcodes for Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef lopcodes_h
#define lopcodes_h

#include "llimits.h"


/*===========================================================================
  We assume that instructions are unsigned numbers.
  All instructions have an opcode in the first 8 bits.
  Instructions can have the following fields:
	'A' : 8 bits (7 bits used)
	'B' : 8 bits
	'C' : 8 bits
	'Ax' : 24 bits ('A', 'B', and 'C' together)
	'Bx' : 16 bits ('B' and 'C' together)
	'sBx' : signed Bx

  Above is based on LuaJIT scheme but unlike LuaJIT A is actually
  represented in 7 bits.
  A signed argument is represented in excess K; that is, the number
  value is the unsigned value minus K. K is exactly the maximum value
  for that argument (so that -max is represented by 0, and +max is
  represented by 2*max), which is half the maximum for the corresponding
  unsigned argument.
===========================================================================*/


enum OpMode {iABC, iABx, iAsBx, iAx};  /* basic instruction format */

#include "ravi_arch.h"

/*

The bytecode layout here uses LuaJIT inspired format.

+---+---+---+----+
| B | C | A | Op |
+---+---+---+----+
| Bx    | A | Op |
+-------+---+----+
| Ax        | Op |
+-----------+----+

*/

#define MAXARG_A 0x7f
#define MAXARG_B 0xff
#define MAXARG_C 0xff
#define MAXARG_Bx 0xffff
#define MAXARG_Ax 0xffffff

#define GET_OPCODE(i)	cast(OpCode, ((i)&0xff))
#define GETARG_A(i)	cast(int, ((i)>>8)&0x7f)
#define GETARG_B(i)	cast(int, (i)>>24)
#define GETARG_C(i)	cast(int, ((i)>>16)&0xff)
#define GETARG_Bx(i)	cast(int, (i)>>16)
#define GETARG_Ax(i)	cast(int, (i)>>8)
#define GETARG_sBx(i)	(((int)GETARG_Bx(i))-MAXARG_sBx)
#define MAXARG_sBx      0x8000

#define setbc_byte(p, x, ofs) \
  ((lu_byte *)(&(p)))[RAVI_ENDIAN_SELECT(ofs, 3-ofs)] = ((lu_byte)cast(Instruction, x))

#define SET_OPCODE(p, x) setbc_byte(p, (x), 0)
#define SETARG_A(p, x)   setbc_byte(p, ((x)&0x7f), 1)
#define SETARG_B(p, x)   setbc_byte(p, (x), 3)
#define SETARG_C(p, x)   setbc_byte(p, (x), 2)
#define SETARG_Bx(p, x) \
  ((unsigned short *)(&(p)))[RAVI_ENDIAN_SELECT(1, 0)] = (unsigned short)(cast(Instruction, x))
#define SETARG_sBx(p, x) SETARG_Bx(p, cast(unsigned int, cast(Instruction, x)+MAXARG_sBx))
#define SETARG_Ax(p, x) p = (cast(Instruction, p)&0xff | (cast(Instruction, x)<<8))


#define CREATE_ABC(o,a,b,c)	((cast(Instruction, o)) \
			| (cast(Instruction, a)<<8) \
			| (cast(Instruction, b)<<24) \
			| (cast(Instruction, c)<<16))

#define CREATE_ABx(o,a,bc)	((cast(Instruction, o)) \
			| (cast(Instruction, a)<<8) \
			| (cast(Instruction, bc)<<16))

#define CREATE_Ax(o,a)		((cast(Instruction, o)) \
			| (cast(Instruction, a)<<8))

/* this bit 1 means constant (0 means register) */
#define BITRK		0x80

/* test whether value is a constant */
#define ISK(x)		((x) & BITRK)

/* gets the index of the constant */
#define INDEXK(r)	((int)(r) & ~BITRK)

#if !defined(MAXINDEXRK)  /* (for debugging only) */
#define MAXINDEXRK	(BITRK - 1)
#endif

/* code a constant index as a RK value */
#define RKASK(x)	((x) | BITRK)


/*
** invalid register that fits in 8 bits
*/
#define NO_REG		MAXARG_A


/*
** R(x) - register
** Kst(x) - constant (in constant table)
** RK(x) == if ISK(x) then Kst(INDEXK(x)) else R(x)
*/


/*
** grep "ORDER OP" if you change these enums
*/

typedef enum {
/*----------------------------------------------------------------------
name		args	description
------------------------------------------------------------------------*/
OP_MOVE,/*	A B	R(A) := R(B)					*/
OP_LOADK,/*	A Bx	R(A) := Kst(Bx)					*/
OP_LOADKX,/*	A 	R(A) := Kst(extra arg)				*/
OP_LOADBOOL,/*	A B C	R(A) := (Bool)B; if (C) pc++			*/
OP_LOADNIL,/*	A B	R(A), R(A+1), ..., R(A+B) := nil		*/
OP_GETUPVAL,/*	A B	R(A) := UpValue[B]				*/

OP_GETTABUP,/*	A B C	R(A) := UpValue[B][RK(C)]			*/
OP_GETTABLE,/*	A B C	R(A) := R(B)[RK(C)]				*/

OP_SETTABUP,/*	A B C	UpValue[A][RK(B)] := RK(C)			*/
OP_SETUPVAL,/*	A B	UpValue[B] := R(A)				*/
OP_SETTABLE,/*	A B C	R(A)[RK(B)] := RK(C)				*/

OP_NEWTABLE,/*	A B C	R(A) := {} (size = B,C)				*/

OP_SELF,/*	A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)]		*/

OP_ADD,/*	A B C	R(A) := RK(B) + RK(C)				*/
OP_SUB,/*	A B C	R(A) := RK(B) - RK(C)				*/
OP_MUL,/*	A B C	R(A) := RK(B) * RK(C)				*/
OP_MOD,/*	A B C	R(A) := RK(B) % RK(C)				*/
OP_POW,/*	A B C	R(A) := RK(B) ^ RK(C)				*/
OP_DIV,/*	A B C	R(A) := RK(B) / RK(C)				*/
OP_IDIV,/*	A B C	R(A) := RK(B) // RK(C)				*/
OP_BAND,/*	A B C	R(A) := RK(B) & RK(C)				*/
OP_BOR,/*	A B C	R(A) := RK(B) | RK(C)				*/
OP_BXOR,/*	A B C	R(A) := RK(B) ~ RK(C)				*/
OP_SHL,/*	A B C	R(A) := RK(B) << RK(C)				*/
OP_SHR,/*	A B C	R(A) := RK(B) >> RK(C)				*/
OP_UNM,/*	A B	R(A) := -R(B)					*/
OP_BNOT,/*	A B	R(A) := ~R(B)					*/
OP_NOT,/*	A B	R(A) := not R(B)				*/
OP_LEN,/*	A B	R(A) := length of R(B)				*/

OP_CONCAT,/*	A B C	R(A) := R(B).. ... ..R(C)			*/

OP_JMP,/*	A sBx	pc+=sBx; if (A) close all upvalues >= R(A - 1)	*/
OP_EQ,/*	A B C	if ((RK(B) == RK(C)) ~= A) then pc++		*/
OP_LT,/*	A B C	if ((RK(B) <  RK(C)) ~= A) then pc++		*/
OP_LE,/*	A B C	if ((RK(B) <= RK(C)) ~= A) then pc++		*/

OP_TEST,/*	A C	if not (R(A) <=> C) then pc++			*/
OP_TESTSET,/*	A B C	if (R(B) <=> C) then R(A) := R(B) else pc++	*/

OP_CALL,/*	A B C	R(A), ... ,R(A+C-2) := R(A)(R(A+1), ... ,R(A+B-1)) */
OP_TAILCALL,/*	A B C	return R(A)(R(A+1), ... ,R(A+B-1))		*/
OP_RETURN,/*	A B	return R(A), ... ,R(A+B-2)	(see note)	*/

OP_FORLOOP,/*	A sBx	R(A)+=R(A+2);
			if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) }*/
OP_FORPREP,/*	A sBx	R(A)-=R(A+2); pc+=sBx				*/

OP_TFORCALL,/*	A C	R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2));	*/
OP_TFORLOOP,/*	A sBx	if R(A+1) ~= nil then { R(A)=R(A+1); pc += sBx }*/

OP_SETLIST,/*	A B C	R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B	*/

OP_CLOSURE,/*	A Bx	R(A) := closure(KPROTO[Bx])			*/

OP_VARARG,/*	A B	R(A), R(A+1), ..., R(A+B-2) = vararg		*/
OP_EXTRAARG,/*	Ax	extra (larger) argument for previous opcode	*/

OP_RAVI_NEW_IARRAY, /* A R(A) := array of int */
OP_RAVI_NEW_FARRAY, /* A R(A) := array of float */

OP_RAVI_LOADIZ,  /*	A R(A) := tointeger(0)		*/
OP_RAVI_LOADFZ,  /*	A R(A) := tonumber(0)		*/

OP_RAVI_UNMF,   /*	A B	R(A) := -R(B) floating point      */
OP_RAVI_UNMI,   /*  A B R(A) := -R(B) integer */

OP_RAVI_ADDFF,  /*	A B C	R(A) := RK(B) + RK(C)		*/
OP_RAVI_ADDFI,  /*	A B C	R(A) := RK(B) + RK(C)		*/
OP_RAVI_ADDII,  /*	A B C	R(A) := RK(B) + RK(C)	  */

OP_RAVI_SUBFF,  /*	A B C	R(A) := RK(B) - RK(C)		*/
OP_RAVI_SUBFI,  /*	A B C	R(A) := RK(B) - RK(C)		*/
OP_RAVI_SUBIF,  /*	A B C	R(A) := RK(B) - RK(C)	  */
OP_RAVI_SUBII,  /*	A B C	R(A) := RK(B) - RK(C)		*/

OP_RAVI_MULFF,  /*	A B C	R(A) := RK(B) * RK(C)		*/
OP_RAVI_MULFI,  /*	A B C	R(A) := RK(B) * RK(C)		*/
OP_RAVI_MULII,  /*	A B C	R(A) := RK(B) * RK(C)	  */

OP_RAVI_DIVFF,  /*	A B C	R(A) := RK(B) / RK(C)		*/
OP_RAVI_DIVFI,  /*	A B C	R(A) := RK(B) / RK(C)		*/
OP_RAVI_DIVIF,  /*	A B C	R(A) := RK(B) / RK(C)	  */
OP_RAVI_DIVII,  /*	A B C	R(A) := RK(B) / RK(C)		*/

OP_RAVI_TOINT, /* A R(A) := toint(R(A)) */
OP_RAVI_TOFLT, /* A R(A) := tofloat(R(A)) */
OP_RAVI_TOIARRAY, /* A R(A) := to_arrayi(R(A)) */
OP_RAVI_TOFARRAY, /* A R(A) := to_arrayf(R(A)) */
OP_RAVI_TOTAB,    /* A R(A) := to_table(R(A)) */
OP_RAVI_TOSTRING, /* A R(A) := assert_string(R(A)) */
OP_RAVI_TOCLOSURE, /* A R(A) := assert_closure(R(A)) */
OP_RAVI_TOTYPE,  /* A R(A) := assert_usertype(R(A)), where usertype has metatable in Lua registry */

OP_RAVI_MOVEI,       /*	A B	R(A) := R(B), check R(B) is int	*/
OP_RAVI_MOVEF,       /*	A B	R(A) := R(B), check R(B) is float */
OP_RAVI_MOVEIARRAY,  /* A B R(A) := R(B), check R(B) is array of int */
OP_RAVI_MOVEFARRAY,  /* A B R(A) := R(B), check R(B) is array of floats */
OP_RAVI_MOVETAB,     /* A B R(A) := R(B), check R(B) is a table */

OP_RAVI_IARRAY_GET,  /*	A B C	R(A) := R(B)[RK(C)] where R(B) is array of integers and RK(C) is int */
OP_RAVI_FARRAY_GET,  /*	A B C	R(A) := R(B)[RK(C)] where R(B) is array of floats and RK(C) is int */

OP_RAVI_IARRAY_SET,  /*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of ints */
OP_RAVI_FARRAY_SET,  /*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of floats */
OP_RAVI_IARRAY_SETI, /*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of ints, and RK(C) is an int */
OP_RAVI_FARRAY_SETF, /*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of floats, and RK(C) is an float */

OP_RAVI_FORLOOP_IP,  /* As FORLOOP, but with integer index and positive integer step */
OP_RAVI_FORLOOP_I1,  /* As FORLOOP, but with integer index 1 and step 1 */
OP_RAVI_FORPREP_IP,  /* As FORPREP, but with integer index and positive integer step */
OP_RAVI_FORPREP_I1,  /* As FORPREP, but with integer index 1 and step 1 */

OP_RAVI_SETUPVALI,  /*	A B	UpValue[B] := tointeger(R(A))			*/
OP_RAVI_SETUPVALF,  /*	A B	UpValue[B] := tonumber(R(A))			*/
OP_RAVI_SETUPVAL_IARRAY,  /*	A B	UpValue[B] := toarrayint(R(A))			*/
OP_RAVI_SETUPVAL_FARRAY,  /*	A B	UpValue[B] := toarrayflt(R(A))			*/
OP_RAVI_SETUPVALT,/*	A B	UpValue[B] := to_table(R(A))			*/

OP_RAVI_BAND_II,/*	A B C	R(A) := RK(B) & RK(C)				*/
OP_RAVI_BOR_II, /*	A B C	R(A) := RK(B) | RK(C)				*/
OP_RAVI_BXOR_II,/*	A B C	R(A) := RK(B) ~ RK(C)				*/
OP_RAVI_SHL_II, /*	A B C	R(A) := RK(B) << RK(C)				*/
OP_RAVI_SHR_II, /*	A B C	R(A) := RK(B) >> RK(C)				*/
OP_RAVI_BNOT_I, /*	A B	R(A) := ~R(B)					*/

OP_RAVI_EQ_II,/*	A B C	if ((RK(B) == RK(C)) ~= A) then pc++		*/
OP_RAVI_EQ_FF,/*	A B C	if ((RK(B) == RK(C)) ~= A) then pc++		*/
OP_RAVI_LT_II,/*	A B C	if ((RK(B) <  RK(C)) ~= A) then pc++		*/
OP_RAVI_LT_FF,/*	A B C	if ((RK(B) <  RK(C)) ~= A) then pc++		*/
OP_RAVI_LE_II,/*	A B C	if ((RK(B) <= RK(C)) ~= A) then pc++		*/
OP_RAVI_LE_FF,/*	A B C	if ((RK(B) <= RK(C)) ~= A) then pc++		*/
  
/* Following op codes are specialised when it is known that indexing is being
   done on a table and the key is known type */
OP_RAVI_TABLE_GETFIELD,/*	A B C	R(A) := R(B)[RK(C)], string key, R(B) references a table */
OP_RAVI_TABLE_SETFIELD,/*	A B C	R(A)[RK(B)] := RK(C), string key, R(A) references a table  */
OP_RAVI_TABLE_SELF_SK, /*	A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)], string key, R(B) references a table */

/* Following opcodes are specialized for indexing where the
   key is known to be string or integer but the variable may or may not be 
   a table */
OP_RAVI_GETI,         /*	A B C	R(A) := R(B)[RK(C)], integer key */
OP_RAVI_SETI,         /*	A B C	R(A)[RK(B)] := RK(C), integer key */
OP_RAVI_GETFIELD,     /*	A B C	R(A) := R(B)[RK(C)], string key  */
OP_RAVI_SELF_SK,      /*	A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)], string key */
OP_RAVI_SETFIELD,     /*	A B C	R(A)[RK(B)] := RK(C), string key */
OP_RAVI_GETTABUP_SK,  /*	A B C	R(A) := UpValue[B][RK(C)], string key */

} OpCode;


#define NUM_OPCODES	(cast(int, OP_RAVI_GETTABUP_SK) + 1)

/*===========================================================================
  Notes:
  (*) In OP_CALL, if (B == 0) then B = top. If (C == 0), then 'top' is
  set to last_result+1, so next open instruction (OP_CALL, OP_RETURN,
  OP_SETLIST) may use 'top'.

  (*) In OP_VARARG, if (B == 0) then use actual number of varargs and
  set top (like in OP_CALL with C == 0).

  (*) In OP_RETURN, if (B == 0) then return up to 'top'.

  (*) In OP_SETLIST, if (B == 0) then B = 'top'; if (C == 0) then next
  'instruction' is EXTRAARG(real C).

  (*) In OP_LOADKX, the next 'instruction' is always EXTRAARG.

  (*) For comparisons, A specifies what condition the test should accept
  (true or false).

  (*) All 'skips' (pc++) assume that next instruction is a jump.

===========================================================================*/


/*
** masks for instruction properties. The format is:
** bits 0-1: op mode
** bits 2-3: C arg mode
** bits 4-5: B arg mode
** bit 6: instruction set register A
** bit 7: operator is a test (next instruction must be a jump)
*/

enum OpArgMask {
  OpArgN,  /* argument is not used */
  OpArgU,  /* argument is used */
  OpArgR,  /* argument is a register or a jump offset */
  OpArgK   /* argument is a constant or register/constant */
};

LUAI_DDEC const lu_byte luaP_opmodes[NUM_OPCODES];

#define getOpMode(m)	(cast(enum OpMode, luaP_opmodes[m] & 3))
#define getBMode(m)	(cast(enum OpArgMask, (luaP_opmodes[m] >> 4) & 3))
#define getCMode(m)	(cast(enum OpArgMask, (luaP_opmodes[m] >> 2) & 3))
#define testAMode(m)	(luaP_opmodes[m] & (1 << 6))
#define testTMode(m)	(luaP_opmodes[m] & (1 << 7))


LUAI_DDEC const char *const luaP_opnames[NUM_OPCODES+1];  /* opcode names */


/* number of list items to accumulate before a SETLIST instruction */
#define LFIELDS_PER_FLUSH	50


/* Ravi extension  - prints the given instruction to the supplied buffer */
LUAI_FUNC const char* raviP_instruction_to_str(char *buf, size_t n, Instruction i);

#endif

/*
** $Id: lopcodes.h,v 1.149 2016/07/19 17:12:21 roberto Exp $
** Opcodes for Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef lopcodes_h
#define lopcodes_h

#include "llimits.h"


/*===========================================================================
  We assume that instructions are unsigned numbers.
  All instructions have an opcode in the first 6 bits.
  Instructions can have the following fields:
	'A' : 8 bits
	'B' : 9 bits
	'C' : 9 bits
	'Ax' : 26 bits ('A', 'B', and 'C' together)
	'Bx' : 18 bits ('B' and 'C' together)
	'sBx' : signed Bx

  A signed argument is represented in excess K; that is, the number
  value is the unsigned value minus K. K is exactly the maximum value
  for that argument (so that -max is represented by 0, and +max is
  represented by 2*max), which is half the maximum for the corresponding
  unsigned argument.
===========================================================================*/


enum OpMode {iABC, iABx, iAsBx, iAx};  /* basic instruction format */


/*
** size and position of opcode arguments.
*/
/** RAVI changes **/
#define SIZE_C		8
#define SIZE_B		8
#define SIZE_Bx		(SIZE_C + SIZE_B)
#define SIZE_A		7
#define SIZE_Ax		(SIZE_C + SIZE_B + SIZE_A)

#define SIZE_OP		9

#define POS_OP		0
#define POS_A		(POS_OP + SIZE_OP)
#define POS_C		(POS_A + SIZE_A)
#define POS_B		(POS_C + SIZE_C)
#define POS_Bx		POS_C
#define POS_Ax		POS_A


/*
** limits for opcode arguments.
** we use (signed) int to manipulate most arguments,
** so they must fit in LUAI_BITSINT-1 bits (-1 for sign)
*/
#if SIZE_Bx < LUAI_BITSINT-1
#define MAXARG_Bx        ((1<<SIZE_Bx)-1)
#define MAXARG_sBx        (MAXARG_Bx>>1)         /* 'sBx' is signed */
#else
#define MAXARG_Bx        MAX_INT
#define MAXARG_sBx        MAX_INT
#endif

#if SIZE_Ax < LUAI_BITSINT-1
#define MAXARG_Ax	((1<<SIZE_Ax)-1)
#else
#define MAXARG_Ax	MAX_INT
#endif


#define MAXARG_A        ((1<<SIZE_A)-1)
#define MAXARG_B        ((1<<SIZE_B)-1)
#define MAXARG_C        ((1<<SIZE_C)-1)


/* creates a mask with 'n' 1 bits at position 'p' */
#define MASK1(n,p)	((~((~(Instruction)0)<<(n)))<<(p))

/* creates a mask with 'n' 0 bits at position 'p' */
#define MASK0(n,p)	(~MASK1(n,p))

/*
** the following macros help to manipulate instructions
*/

#define GET_OPCODE(i)	(cast(OpCode, ((i)>>POS_OP) & MASK1(SIZE_OP,0)))
#define SET_OPCODE(i,o)	((i) = (((i)&MASK0(SIZE_OP,POS_OP)) | \
		((cast(Instruction, o)<<POS_OP)&MASK1(SIZE_OP,POS_OP))))

#define getarg(i,pos,size)	(cast(int, ((i)>>pos) & MASK1(size,0)))
#define setarg(i,v,pos,size)	((i) = (((i)&MASK0(size,pos)) | \
                ((cast(Instruction, v)<<pos)&MASK1(size,pos))))

#define GETARG_A(i)	getarg(i, POS_A, SIZE_A)
#define SETARG_A(i,v)	setarg(i, v, POS_A, SIZE_A)

#define GETARG_B(i)	getarg(i, POS_B, SIZE_B)
#define SETARG_B(i,v)	setarg(i, v, POS_B, SIZE_B)

#define GETARG_C(i)	getarg(i, POS_C, SIZE_C)
#define SETARG_C(i,v)	setarg(i, v, POS_C, SIZE_C)

#define GETARG_Bx(i)	getarg(i, POS_Bx, SIZE_Bx)
#define SETARG_Bx(i,v)	setarg(i, v, POS_Bx, SIZE_Bx)

#define GETARG_Ax(i)	getarg(i, POS_Ax, SIZE_Ax)
#define SETARG_Ax(i,v)	setarg(i, v, POS_Ax, SIZE_Ax)

#define GETARG_sBx(i)	(GETARG_Bx(i)-MAXARG_sBx)
#define SETARG_sBx(i,b)	SETARG_Bx((i),cast(unsigned int, (b)+MAXARG_sBx))


#define CREATE_ABC(o,a,b,c)	((cast(Instruction, o)<<POS_OP) \
			| (cast(Instruction, a)<<POS_A) \
			| (cast(Instruction, b)<<POS_B) \
			| (cast(Instruction, c)<<POS_C))

#define CREATE_ABx(o,a,bc)	((cast(Instruction, o)<<POS_OP) \
			| (cast(Instruction, a)<<POS_A) \
			| (cast(Instruction, bc)<<POS_Bx))

#define CREATE_Ax(o,a)		((cast(Instruction, o)<<POS_OP) \
			| (cast(Instruction, a)<<POS_Ax))


/*
** Macros to operate RK indices
*/

/* this bit 1 means constant (0 means register) */
#define BITRK		(1 << (SIZE_B - 1))

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

OP_RAVI_NEWARRAYI, /* A R(A) := array of int */
OP_RAVI_NEWARRAYF, /* A R(A) := array of float */

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
OP_RAVI_TOARRAYI, /* A R(A) := to_arrayi(R(A)) */
OP_RAVI_TOARRAYF, /* A R(A) := to_arrayf(R(A)) */
OP_RAVI_TOTAB,    /* A R(A) := to_table(R(A)) */

OP_RAVI_MOVEI, /*	A B	R(A) := R(B), check R(B) is int	*/
OP_RAVI_MOVEF, /*	A B	R(A) := R(B), check R(B) is float */
OP_RAVI_MOVEAI, /* A B R(A) := R(B), check R(B) is array of int */
OP_RAVI_MOVEAF, /* A B R(A) := R(B), check R(B) is array of floats */
OP_RAVI_MOVETAB,  /* A B R(A) := R(B), check R(B) is a table */

OP_RAVI_GETTABLE_AI,/*	A B C	R(A) := R(B)[RK(C)] where R(B) is array of integers and RK(C) is int */
OP_RAVI_GETTABLE_AF,/*	A B C	R(A) := R(B)[RK(C)] where R(B) is array of floats and RK(C) is int */

OP_RAVI_SETTABLE_AI,/*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of ints */
OP_RAVI_SETTABLE_AF,/*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of floats */
OP_RAVI_SETTABLE_AII,/*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of ints, and RK(C) is an int */
OP_RAVI_SETTABLE_AFF,/*	A B C	R(A)[RK(B)] := RK(C) where RK(B) is an int, R(A) is array of floats, and RK(C) is an float */

OP_RAVI_FORLOOP_IP,
OP_RAVI_FORLOOP_I1,
OP_RAVI_FORPREP_IP,
OP_RAVI_FORPREP_I1,

OP_RAVI_SETUPVALI,  /*	A B	UpValue[B] := tointeger(R(A))			*/
OP_RAVI_SETUPVALF,  /*	A B	UpValue[B] := tonumber(R(A))			*/
OP_RAVI_SETUPVALAI,  /*	A B	UpValue[B] := toarrayint(R(A))			*/
OP_RAVI_SETUPVALAF,  /*	A B	UpValue[B] := toarrayflt(R(A))			*/
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
OP_RAVI_GETTABLE_S,/*	A B C	R(A) := R(B)[RK(C)], string key, known table */
OP_RAVI_SETTABLE_S,/*	A B C	R(A)[RK(B)] := RK(C), string key, known table  */
OP_RAVI_SELF_S,/*	A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)], string key, known table */

/* Following opcodes are specialized for indexing where the
   key is known to be string but the variable may or may not be 
   a table */
OP_RAVI_GETTABLE_I,/*	A B C	R(A) := R(B)[RK(C)], integer key, known table */
OP_RAVI_SETTABLE_I,/*	A B C	R(A)[RK(B)] := RK(C), integer key, known table */
OP_RAVI_GETTABLE_SK, /*	A B C	R(A) := R(B)[RK(C)], string key   */
OP_RAVI_SELF_SK,     /*	A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)], string key */
OP_RAVI_SETTABLE_SK, /*	A B C	R(A)[RK(B)] := RK(C), string key */
OP_RAVI_GETTABUP_SK, /*	A B C	R(A) := UpValue[B][RK(C)], string key */

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

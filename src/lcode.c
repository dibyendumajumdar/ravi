/*
** $Id: lcode.c,v 2.112.1.1 2017/04/19 17:20:42 roberto Exp $
** Code generator for Lua
** See Copyright Notice in lua.h
*/

/*
** Portions Copyright (C) 2015-2022 Dibyendu Majumdar
*/

#define lcode_c
#define LUA_CORE

#include "lprefix.h"


#include <math.h>
#include <stdlib.h>

#include "lua.h"

#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "llex.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"


/* Maximum number of registers in a Lua function (must fit in operand A) */
#define MAXREGS		125
/* RAVI change; #define MAXREGS		255 */


#define hasjumps(e)	((e)->t != (e)->f)


/*
** If expression is a numeric constant, fills 'v' with its value
** and returns 1. Otherwise, returns 0.
*/
static int tonumeral(const expdesc *e, TValue *v) {
  if (hasjumps(e))
    return 0;  /* not a numeral */
  switch (e->k) {
    case VKINT:
      if (v) setivalue(v, e->u.ival);
      return 1;
    case VKFLT:
      if (v) setfltvalue(v, e->u.nval);
      return 1;
    default: return 0;
  }
}


/*
** Create a OP_LOADNIL instruction, but try to optimize: if the previous
** instruction is also OP_LOADNIL and ranges are compatible, adjust
** range of previous instruction instead of emitting a new one. (For
** instance, 'local a; local b' will generate a single opcode.)
*/
void luaK_nil (FuncState *fs, int from, int n) {
  Instruction *previous;
  int l = from + n - 1;  /* last register to set nil */
  if (fs->pc > fs->lasttarget) {  /* no jumps to current position? */
    previous = &fs->f->code[fs->pc-1];
    if (GET_OPCODE(*previous) == OP_LOADNIL) {  /* previous is LOADNIL? */
      int pfrom = GETARG_A(*previous);  /* get previous range */
      int pl = pfrom + GETARG_B(*previous);
      if ((pfrom <= from && from <= pl + 1) ||
          (from <= pfrom && pfrom <= l + 1)) {  /* can connect both? */
        if (pfrom < from) from = pfrom;  /* from = min(from, pfrom) */
        if (pl > l) l = pl;  /* l = max(l, pl) */
        SETARG_A(*previous, from);
        DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set A to %d\n", fs->pc - 1, *previous, from));
        SETARG_B(*previous, l - from);
        DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set B to %d\n", fs->pc - 1, *previous, (l - from)));
        return;
      }
    }  /* else go through */
  }
  luaK_codeABC(fs, OP_LOADNIL, from, n - 1, 0);  /* else no optimization */
}


/*
** Gets the destination address of a jump instruction. Used to traverse
** a list of jumps.
*/
static int getjump (FuncState *fs, int pc) {
  int offset = GETARG_sBx(fs->f->code[pc]);
  if (offset == NO_JUMP)  /* point to itself represents end of list */
    return NO_JUMP;  /* end of list */
  else
    return (pc+1)+offset;  /* turn offset into absolute position */
}


/*
** Fix jump instruction at position 'pc' to jump to 'dest'.
** (Jump addresses are relative in Lua)
*/
static void fixjump (FuncState *fs, int pc, int dest) {
  Instruction *jmp = &fs->f->code[pc];
  int offset = dest - (pc + 1);
  lua_assert(dest != NO_JUMP);
  if (abs(offset) > MAXARG_sBx)
    luaX_syntaxerror(fs->ls, "control structure too long");
  SETARG_sBx(*jmp, offset);
}


/*
** Concatenate jump-list 'l2' into jump-list 'l1'
*/
void luaK_concat (FuncState *fs, int *l1, int l2) {
  if (l2 == NO_JUMP) return;  /* nothing to concatenate? */
  else if (*l1 == NO_JUMP)  /* no original list? */
    *l1 = l2;  /* 'l1' points to 'l2' */
  else {
    int list = *l1;
    int next;
    while ((next = getjump(fs, list)) != NO_JUMP)  /* find last element */
      list = next;
    fixjump(fs, list, l2);  /* last element links to 'l2' */
  }
}


/*
** Create a jump instruction and return its position, so its destination
** can be fixed later (with 'fixjump'). If there are jumps to
** this position (kept in 'jpc'), link them all together so that
** 'patchlistaux' will fix all them directly to the final destination.
*/
int luaK_jump (FuncState *fs) {
  int jpc = fs->jpc;  /* save list of jumps to here */
  int j;
  fs->jpc = NO_JUMP;  /* no more jumps to here */
  j = luaK_codeAsBx(fs, OP_JMP, 0, NO_JUMP);
  luaK_concat(fs, &j, jpc);  /* keep them on hold */
  return j;
}


/*
** Code a 'return' instruction
*/
void luaK_ret (FuncState *fs, int first, int nret) {
  luaK_codeABC(fs, OP_RETURN, first, nret+1, 0);
}


/*
** Code a "conditional jump", that is, a test or comparison opcode
** followed by a jump. Return jump position.
*/
static int condjump (FuncState *fs, OpCode op, int A, int B, int C) {
  luaK_codeABC(fs, op, A, B, C);
  return luaK_jump(fs);
}


/*
** returns current 'pc' and marks it as a jump target (to avoid wrong
** optimizations with consecutive instructions not in the same basic block).
*/
int luaK_getlabel (FuncState *fs) {
  fs->lasttarget = fs->pc;
  return fs->pc;
}


/*
** Returns the position of the instruction "controlling" a given
** jump (that is, its condition), or the jump itself if it is
** unconditional.
*/
static Instruction *getjumpcontrol (FuncState *fs, int pc) {
  Instruction *pi = &fs->f->code[pc];
  if (pc >= 1 && testTMode(GET_OPCODE(*(pi-1))))
    return pi-1;
  else
    return pi;
}


/*
** Patch destination register for a TESTSET instruction.
** If instruction in position 'node' is not a TESTSET, return 0 ("fails").
** Otherwise, if 'reg' is not 'NO_REG', set it as the destination
** register. Otherwise, change instruction to a simple 'TEST' (produces
** no register value)
*/
static int patchtestreg (FuncState *fs, int node, int reg) {
  Instruction *i = getjumpcontrol(fs, node);
  if (GET_OPCODE(*i) != OP_TESTSET)
    return 0;  /* cannot patch other instructions */
  if (reg != NO_REG && reg != GETARG_B(*i)) {
    SETARG_A(*i, reg);
    DEBUG_CODEGEN(raviY_printf(fs, "[?]* %o ; set A to %d\n", *i, reg));
  }
  else {
     /* no register to put value or register already has the value;
        change instruction to simple test */
    *i = CREATE_ABC(OP_TEST, GETARG_B(*i), 0, GETARG_C(*i));
    DEBUG_CODEGEN(raviY_printf(fs, "[?]* %o ; generate OP_TEST\n", *i));
  }
  return 1;
}


/*
** Traverse a list of tests ensuring no one produces a value
*/
static void removevalues (FuncState *fs, int list) {
  for (; list != NO_JUMP; list = getjump(fs, list))
      patchtestreg(fs, list, NO_REG);
}


/*
** Traverse a list of tests, patching their destination address and
** registers: tests producing values jump to 'vtarget' (and put their
** values in 'reg'), other tests jump to 'dtarget'.
*/
static void patchlistaux (FuncState *fs, int list, int vtarget, int reg,
                          int dtarget) {
  while (list != NO_JUMP) {
    int next = getjump(fs, list);
    if (patchtestreg(fs, list, reg))
      fixjump(fs, list, vtarget);  /* Jump to target with value. */
    else
      fixjump(fs, list, dtarget);  /* jump to default target */
    list = next;
  }
}


/*
** Ensure all pending jumps to current position are fixed (jumping
** to current position with no values) and reset list of pending
** jumps
*/
static void dischargejpc (FuncState *fs) {
  patchlistaux(fs, fs->jpc, fs->pc, NO_REG, fs->pc);
  fs->jpc = NO_JUMP;
}


/*
** Add elements in 'list' to list of pending jumps to "here"
** (current position)
*/
void luaK_patchtohere (FuncState *fs, int list) {
  luaK_getlabel(fs);  /* mark "here" as a jump target */
  luaK_concat(fs, &fs->jpc, list);
}


/*
** Path all jumps in 'list' to jump to 'target'.
** (The assert means that we cannot fix a jump to a forward address
** because we only know addresses once code is generated.)
*/
void luaK_patchlist (FuncState *fs, int list, int target) {
  if (target == fs->pc)  /* 'target' is current position? */
    luaK_patchtohere(fs, list);  /* add list to pending jumps */
  else {
    lua_assert(target < fs->pc);
    patchlistaux(fs, list, target, NO_REG, target);
  }
}


/*
** Path all jumps in 'list' to close upvalues up to given 'level'
** (The assertion checks that jumps either were closing nothing
** or were closing higher levels, from inner blocks.)
*/
void luaK_patchclose (FuncState *fs, int list, int level) {
  level++;  /* argument is +1 to reserve 0 as non-op */
  for (; list != NO_JUMP; list = getjump(fs, list)) {
    lua_assert(GET_OPCODE(fs->f->code[list]) == OP_JMP &&
                (GETARG_A(fs->f->code[list]) == 0 ||
                 GETARG_A(fs->f->code[list]) >= level));
    SETARG_A(fs->f->code[list], level);
    DEBUG_CODEGEN(raviY_printf(fs, "[?]* %o ; set A to %d\n", fs->f->code[list], level));
  }
}


/*
** Emit instruction 'i', checking for array sizes and saving also its
** line information. Return 'i' position.
*/
static int luaK_code (FuncState *fs, Instruction i) {
  Proto *f = fs->f;
  dischargejpc(fs);  /* 'pc' will change */
  /* put new instruction in code array */
  luaM_growvector(fs->ls->L, f->code, fs->pc, f->sizecode, Instruction,
                  MAX_INT, "opcodes");
  f->code[fs->pc] = i;
  DEBUG_CODEGEN(raviY_printf(fs, "[%d] %o\n", fs->pc, i));
  /* save corresponding line information */
  luaM_growvector(fs->ls->L, f->lineinfo, fs->pc, f->sizelineinfo, int,
                  MAX_INT, "opcodes");
  f->lineinfo[fs->pc] = fs->ls->lastline;
  return fs->pc++;
}


/*
** Format and emit an 'iABC' instruction. (Assertions check consistency
** of parameters versus opcode.)
*/
int luaK_codeABC (FuncState *fs, OpCode o, int a, int b, int c) {
  lua_assert(getOpMode(o) == iABC);
  lua_assert(getBMode(o) != OpArgN || b == 0);
  lua_assert(getCMode(o) != OpArgN || c == 0);
  lua_assert(a <= MAXARG_A && b <= MAXARG_B && c <= MAXARG_C);
  return luaK_code(fs, CREATE_ABC(o, a, b, c));
}


/*
** Format and emit an 'iABx' instruction.
*/
int luaK_codeABx (FuncState *fs, OpCode o, int a, unsigned int bc) {
  lua_assert(getOpMode(o) == iABx || getOpMode(o) == iAsBx);
  lua_assert(getCMode(o) == OpArgN);
  lua_assert(a <= MAXARG_A && bc <= MAXARG_Bx);
  return luaK_code(fs, CREATE_ABx(o, a, bc));
}


/*
** Emit an "extra argument" instruction (format 'iAx')
*/
static int codeextraarg (FuncState *fs, int a) {
  lua_assert(a <= MAXARG_Ax);
  return luaK_code(fs, CREATE_Ax(OP_EXTRAARG, a));
}


/*
** Emit a "load constant" instruction, using either 'OP_LOADK'
** (if constant index 'k' fits in 18 bits) or an 'OP_LOADKX'
** instruction with "extra argument".
*/
int luaK_codek (FuncState *fs, int reg, int k) {
  if (k <= MAXARG_Bx)
    return luaK_codeABx(fs, OP_LOADK, reg, k);
  else {
    int p = luaK_codeABx(fs, OP_LOADKX, reg, 0);
    codeextraarg(fs, k);
    return p;
  }
}


/*
** Check register-stack level, keeping track of its maximum size
** in field 'maxstacksize'
*/
void luaK_checkstack (FuncState *fs, int n) {
  int newstack = fs->freereg + n;
  if (newstack > fs->f->maxstacksize) {
    if (newstack >= MAXREGS)
      luaX_syntaxerror(fs->ls,
        "function or expression needs too many registers");
    fs->f->maxstacksize = cast_byte(newstack);
  }
}


/*
** Reserve 'n' registers in register stack
*/
void luaK_reserveregs (FuncState *fs, int n) {
  luaK_checkstack(fs, n);
  fs->freereg += n;
}


/*
** Free register 'reg', if it is neither a constant index nor
** a local variable.
)
*/
static void freereg (FuncState *fs, int reg) {
  if (!ISK(reg) && reg >= fs->nactvar) {
    fs->freereg--;
    lua_assert(reg == fs->freereg);
  }
}


/*
** Free register used by expression 'e' (if any)
*/
static void freeexp (FuncState *fs, expdesc *e) {
  if (e->k == VNONRELOC)
    freereg(fs, e->u.info);
}


/*
** Free registers used by expressions 'e1' and 'e2' (if any) in proper
** order.
*/
static void freeexps (FuncState *fs, expdesc *e1, expdesc *e2) {
  int r1 = (e1->k == VNONRELOC) ? e1->u.info : -1;
  int r2 = (e2->k == VNONRELOC) ? e2->u.info : -1;
  if (r1 > r2) {
    freereg(fs, r1);
    freereg(fs, r2);
  }
  else {
    freereg(fs, r2);
    freereg(fs, r1);
  }
}


/*
** Add constant 'v' to prototype's list of constants (field 'k').
** Use scanner's table to cache position of constants in constant list
** and try to reuse constants. Because some values should not be used
** as keys (nil cannot be a key, integer keys can collapse with float
** keys), the caller must provide a useful 'key' for indexing the cache.
*/
static int addk (FuncState *fs, TValue *key, TValue *v) {
  lua_State *L = fs->ls->L;
  Proto *f = fs->f;
  TValue *idx = luaH_set(L, fs->ls->h, key);  /* index scanner table */
  int k, oldsize;
  if (ttisinteger(idx)) {  /* is there an index there? */
    k = cast_int(ivalue(idx));
    /* correct value? (warning: must distinguish floats from integers!) */
    if (k < fs->nk && ttype(&f->k[k]) == ttype(v) &&
                      luaV_rawequalobj(&f->k[k], v))
      return k;  /* reuse index */
  }
  /* constant not found; create a new entry */
  oldsize = f->sizek;
  k = fs->nk;
  /* numerical value does not need GC barrier;
     table has no metatable, so it does not need to invalidate cache */
  setivalue(idx, k);
  luaM_growvector(L, f->k, k, f->sizek, TValue, MAXARG_Ax, "constants");
  while (oldsize < f->sizek) setnilvalue(&f->k[oldsize++]);
  setobj(L, &f->k[k], v);
  fs->nk++;
  luaC_barrier(L, f, v);
  return k;
}


/*
** Add a string to list of constants and return its index.
*/
int luaK_stringK (FuncState *fs, TString *s) {
  TValue o;
  setsvalue(fs->ls->L, &o, s);
  return addk(fs, &o, &o);  /* use string itself as key */
}


/*
** Add an integer to list of constants and return its index.
** Integers use userdata as keys to avoid collision with floats with
** same value; conversion to 'void*' is used only for hashing, so there
** are no "precision" problems.
*/
int luaK_intK (FuncState *fs, lua_Integer n) {
  TValue k, o;
  setpvalue(&k, cast(void*, cast(size_t, n)));
  setivalue(&o, n);
  return addk(fs, &k, &o);
}

/*
** Add a float to list of constants and return its index.
*/
static int luaK_numberK (FuncState *fs, lua_Number r) {
  TValue o;
  setfltvalue(&o, r);
  return addk(fs, &o, &o);  /* use number itself as key */
}


/*
** Add a boolean to list of constants and return its index.
*/
static int boolK (FuncState *fs, int b) {
  TValue o;
  setbvalue(&o, b);
  return addk(fs, &o, &o);  /* use boolean itself as key */
}


/*
** Add nil to list of constants and return its index.
*/
static int nilK (FuncState *fs) {
  TValue k, v;
  setnilvalue(&v);
  /* cannot use nil as key; instead use table itself to represent nil */
  sethvalue(fs->ls->L, &k, fs->ls->h);
  return addk(fs, &k, &v);
}


/*
** Fix an expression to return the number of results 'nresults'.
** Either 'e' is a multi-ret expression (function call or vararg)
** or 'nresults' is LUA_MULTRET (as any expression can satisfy that).
*/
void luaK_setreturns (FuncState *fs, expdesc *e, int nresults) {
  if (e->k == VCALL) {  /* expression is an open function call? */
    SETARG_C(getinstruction(fs, e), nresults + 1);
    DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set C to %d\n", e->u.info, getinstruction(fs,e), nresults+1));
  }
  else if (e->k == VVARARG) {
    Instruction *pc = &getinstruction(fs, e);
    SETARG_B(*pc, nresults + 1);
    DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set B to %d\n", e->u.info, *pc, nresults + 1));
    SETARG_A(*pc, fs->freereg);
    DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set A to %d\n", e->u.info, *pc, fs->freereg));
    luaK_reserveregs(fs, 1);
  }
  else lua_assert(nresults == LUA_MULTRET);
}


/*
** Fix an expression to return one result.
** If expression is not a multi-ret expression (function call or
** vararg), it already returns one result, so nothing needs to be done.
** Function calls become VNONRELOC expressions (as its result comes
** fixed in the base register of the call), while vararg expressions
** become VRELOCABLE (as OP_VARARG puts its results where it wants).
** (Calls are created returning one result, so that does not need
** to be fixed.)
*/
void luaK_setoneret (FuncState *fs, expdesc *e) {
  if (e->k == VCALL) {  /* expression is an open function call? */
    /* already returns 1 value */
    lua_assert(GETARG_C(getinstruction(fs, e)) == 2);
    e->k = VNONRELOC;  /* result has fixed position */
    e->u.info = GETARG_A(getinstruction(fs, e));
    DEBUG_EXPR(raviY_printf(fs, "luaK_setoneret (VCALL->VNONRELOC) %e\n", e));
  }
  else if (e->k == VVARARG) {
    SETARG_B(getinstruction(fs, e), 2);
    DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set B to 2\n", e->u.info, getinstruction(fs,e)));
    e->k = VRELOCABLE;  /* can relocate its simple result */
    DEBUG_EXPR(raviY_printf(fs, "luaK_setoneret (VVARARG->VRELOCABLE) %e\n", e));
  }
}

/**
 * Is the operand a reference to a short string constant?
 */
static int isshortstr(FuncState *fs, int kk) {
  if (ISK(kk)) {
    Proto *f = fs->f;
    kk = INDEXK(kk);
    lua_assert(kk >= 0 && kk < f->sizek);
    return ttisshrstring(&f->k[kk]);
  }
  return 0;
}

void luaK_dischargevars (FuncState *fs, expdesc *e) {
  switch (e->k) {
    case VLOCAL: {  /* already in a register */
      e->k = VNONRELOC;  /* becomes a non-relocatable value */
      DEBUG_EXPR(raviY_printf(fs, "luaK_dischargevars (VLOCAL->VNONRELOC) %e\n", e));
      break;
    }
    case VUPVAL: {  /* move value to some (pending) register */
      e->u.info = luaK_codeABC(fs, OP_GETUPVAL, 0, e->u.info, 0);
      e->k = VRELOCABLE;
      DEBUG_EXPR(raviY_printf(fs, "luaK_dischargevars (VUPVAL->VRELOCABLE) %e\n", e));
      break;
    }
    case VINDEXED: {
      OpCode op;
      freereg(fs, e->u.ind.idx);
      if (e->u.ind.vt == VLOCAL) {  /* is 't' in a register? */
        freereg(fs, e->u.ind.t);
        /* TODO we should do this for upvalues too */
        /* table access - set specialized op codes if array types are detected */
        if (e->u.ind.key_ravi_type_map == RAVI_TM_INTEGER) {
          if (e->ravi_type_map == RAVI_TM_FLOAT_ARRAY) {
            op = OP_RAVI_FARRAY_GET;
          } else if (e->ravi_type_map == RAVI_TM_INTEGER_ARRAY) {
            op = OP_RAVI_IARRAY_GET;
          } else {
            op = OP_RAVI_GETI;
          }
        } else if (e->u.ind.key_ravi_type_map == RAVI_TM_STRING && isshortstr(fs, e->u.ind.idx)) {
          op = e->ravi_type_map == RAVI_TM_TABLE ? OP_RAVI_TABLE_GETFIELD : OP_RAVI_GETFIELD;
        } else {
          op = OP_GETTABLE;
        }
      }
      else {
        lua_assert(e->u.ind.vt == VUPVAL);
        if (e->u.ind.key_ravi_type_map == RAVI_TM_STRING && isshortstr(fs, e->u.ind.idx))
          op = OP_RAVI_GETTABUP_SK;
        else
          op = OP_GETTABUP;  /* 't' is in an upvalue */
      }
      ravi_type_map result_type = 0;
      if (e->ravi_type_map & (~(RAVI_TM_INTEGER_ARRAY | RAVI_TM_FLOAT_ARRAY))) {
        result_type = RAVI_TM_ANY;
      } else {
        if (e->ravi_type_map & RAVI_TM_INTEGER_ARRAY) result_type |= RAVI_TM_INTEGER;
        if (e->ravi_type_map & RAVI_TM_FLOAT_ARRAY) result_type |= RAVI_TM_FLOAT;
      }
      e->ravi_type_map = result_type;
      e->u.info = luaK_codeABC(fs, op, 0, e->u.ind.t, e->u.ind.idx);
      e->k = VRELOCABLE;
      DEBUG_EXPR(raviY_printf(fs, "luaK_dischargevars (VINDEXED->VRELOCABLE) %e\n", e));
      break;
    }
    case VVARARG: case VCALL: {
      luaK_setoneret(fs, e);
      break;
    }
    default: break;  /* there is one value available (somewhere) */
  }
}


/*
** Ensures expression value is in register 'reg' (and therefore
** 'e' will become a non-relocatable expression).
*/
static void discharge2reg (FuncState *fs, expdesc *e, int reg) {
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VNIL: {
      luaK_nil(fs, reg, 1);
      break;
    }
    case VFALSE: case VTRUE: {
      luaK_codeABC(fs, OP_LOADBOOL, reg, e->k == VTRUE, 0);
      break;
    }
    case VK: {
      luaK_codek(fs, reg, e->u.info);
      break;
    }
    case VKFLT: {
      luaK_codek(fs, reg, luaK_numberK(fs, e->u.nval));
      break;
    }
    case VKINT: {
      luaK_codek(fs, reg, luaK_intK(fs, e->u.ival));
      break;
    }
    case VRELOCABLE: {
      Instruction *pc = &getinstruction(fs, e);
      SETARG_A(*pc, reg);  /* instruction will put result in 'reg' */
      DEBUG_EXPR(raviY_printf(fs, "discharge2reg (VRELOCABLE set arg A) %e\n", e));
      DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set A to %d\n", e->u.info, *pc, reg));
      break;
    }
    case VNONRELOC: {
      if (reg != e->u.info) {
        /* code a MOVEI or MOVEF if the target register is a local typed variable */
        TString *usertype = NULL;
        int ravi_type = raviY_get_register_typeinfo(fs, reg, &usertype);
        switch (ravi_type) {
        case RAVI_TM_INTEGER:
          luaK_codeABC(fs, OP_RAVI_MOVEI, reg, e->u.info, 0);
          break;
        case RAVI_TM_FLOAT:
          luaK_codeABC(fs, OP_RAVI_MOVEF, reg, e->u.info, 0);
          break;
        case RAVI_TM_INTEGER_ARRAY:
          luaK_codeABC(fs, OP_RAVI_MOVEIARRAY, reg, e->u.info, 0);
          break;
        case RAVI_TM_FLOAT_ARRAY:
          luaK_codeABC(fs, OP_RAVI_MOVEFARRAY, reg, e->u.info, 0);
          break;
        case RAVI_TM_TABLE:
          luaK_codeABC(fs, OP_RAVI_MOVETAB, reg, e->u.info, 0);
          break;
        default:
          luaK_codeABC(fs, OP_MOVE, reg, e->u.info, 0);
          if (ravi_type == RAVI_TM_STRING_OR_NIL)
            luaK_codeABC(fs, OP_RAVI_TOSTRING, reg, 0, 0);
          else if (ravi_type == RAVI_TM_FUNCTION_OR_NIL)
            luaK_codeABC(fs, OP_RAVI_TOCLOSURE, reg, 0, 0);
          else if (ravi_type == RAVI_TM_USERDATA_OR_NIL && usertype)
            luaK_codeABx(fs, OP_RAVI_TOTYPE, reg, luaK_stringK(fs, usertype));
          break;
        }
      }
      break;
    }
    default: {
      lua_assert(e->k == VJMP);
      return;  /* nothing to do... */
    }
  }
  e->u.info = reg;
  e->k = VNONRELOC;
}


/*
** Ensures expression value is in any register.
*/
static void discharge2anyreg (FuncState *fs, expdesc *e) {
  if (e->k != VNONRELOC) {  /* no fixed register yet? */
    luaK_reserveregs(fs, 1);  /* get a register */
    discharge2reg(fs, e, fs->freereg-1);  /* put value there */
  }
}


static int code_loadbool (FuncState *fs, int A, int b, int jump) {
  luaK_getlabel(fs);  /* those instructions may be jump targets */
  return luaK_codeABC(fs, OP_LOADBOOL, A, b, jump);
}


/*
** check whether list has any jump that do not produce a value
** or produce an inverted value
*/
static int need_value (FuncState *fs, int list) {
  for (; list != NO_JUMP; list = getjump(fs, list)) {
    Instruction i = *getjumpcontrol(fs, list);
    if (GET_OPCODE(i) != OP_TESTSET) return 1;
  }
  return 0;  /* not found */
}


/*
** Ensures final expression result (including results from its jump
** lists) is in register 'reg'.
** If expression has jumps, need to patch these jumps either to
** its final position or to "load" instructions (for those tests
** that do not produce values).
*/
static void exp2reg (FuncState *fs, expdesc *e, int reg) {
  discharge2reg(fs, e, reg);
  if (e->k == VJMP)  /* expression itself is a test? */
    luaK_concat(fs, &e->t, e->u.info);  /* put this jump in 't' list */
  if (hasjumps(e)) {
    int final;  /* position after whole expression */
    int p_f = NO_JUMP;  /* position of an eventual LOAD false */
    int p_t = NO_JUMP;  /* position of an eventual LOAD true */
    if (need_value(fs, e->t) || need_value(fs, e->f)) {
      int fj = (e->k == VJMP) ? NO_JUMP : luaK_jump(fs);
      p_f = code_loadbool(fs, reg, 0, 1);
      p_t = code_loadbool(fs, reg, 1, 0);
      luaK_patchtohere(fs, fj);
    }
    final = luaK_getlabel(fs);
    patchlistaux(fs, e->f, final, reg, p_f);
    patchlistaux(fs, e->t, final, reg, p_t);
  }
  e->f = e->t = NO_JUMP;
  e->u.info = reg;
  e->k = VNONRELOC;
}


/*
** Ensures final expression result (including results from its jump
** lists) is in next available register.
*/
void luaK_exp2nextreg (FuncState *fs, expdesc *e) {
  luaK_dischargevars(fs, e);
  freeexp(fs, e);
  luaK_reserveregs(fs, 1);
  exp2reg(fs, e, fs->freereg - 1);
}


/*
** Ensures final expression result (including results from its jump
** lists) is in some (any) register and return that register.
*/
int luaK_exp2anyreg (FuncState *fs, expdesc *e) {
  luaK_dischargevars(fs, e);
  if (e->k == VNONRELOC) {  /* expression already has a register? */
    if (!hasjumps(e))  /* no jumps? */
      return e->u.info;  /* result is already in a register */
    if (e->u.info >= fs->nactvar) {  /* reg. is not a local? */
      exp2reg(fs, e, e->u.info);  /* put final result in it */
      return e->u.info;
    }
  }
  luaK_exp2nextreg(fs, e);  /* otherwise, use next available register */
  return e->u.info;
}


/*
** Ensures final expression result is either in a register or in an
** upvalue.
*/
void luaK_exp2anyregup (FuncState *fs, expdesc *e) {
  if (e->k != VUPVAL || hasjumps(e))
    luaK_exp2anyreg(fs, e);
}


/*
** Ensures final expression result is either in a register or it is
** a constant.
*/
void luaK_exp2val (FuncState *fs, expdesc *e) {
  if (hasjumps(e))
    luaK_exp2anyreg(fs, e);
  else
    luaK_dischargevars(fs, e);
}


/*
** Ensures final expression result is in a valid R/K index
** (that is, it is either in a register or in 'k' with an index
** in the range of R/K indices).
** Returns R/K index.
*/
int luaK_exp2RK (FuncState *fs, expdesc *e) {
  luaK_exp2val(fs, e);
  switch (e->k) {  /* move constants to 'k' */
    case VTRUE: e->u.info = boolK(fs, 1); goto vk;
    case VFALSE: e->u.info = boolK(fs, 0); goto vk;
    case VNIL: e->u.info = nilK(fs); goto vk;
    case VKINT: e->u.info = luaK_intK(fs, e->u.ival); goto vk;
    case VKFLT: e->u.info = luaK_numberK(fs, e->u.nval); goto vk;
    case VK:
     vk:
      e->k = VK;
      if (e->u.info <= MAXINDEXRK)  /* constant fits in 'argC'? */
        return RKASK(e->u.info);
      else break;
    default: break;
  }
  /* not a constant in the right range: put it in a register */
  return luaK_exp2anyreg(fs, e);
}

static void check_valid_store(FuncState *fs, expdesc *var, expdesc *ex) {
  /* VNONRELOC means we have fixed register and do we know the type? */
  if (ex->k == VNONRELOC &&
      (var->ravi_type_map == RAVI_TM_FLOAT ||
       var->ravi_type_map == RAVI_TM_INTEGER ||
       var->ravi_type_map == RAVI_TM_FLOAT_ARRAY ||
       var->ravi_type_map == RAVI_TM_INTEGER_ARRAY ||
       var->ravi_type_map == RAVI_TM_TABLE ||
       var->ravi_type_map == RAVI_TM_STRING_OR_NIL ||
       var->ravi_type_map == RAVI_TM_FUNCTION_OR_NIL ||
       var->ravi_type_map == RAVI_TM_USERDATA_OR_NIL)) {
    /* handled by MOVEI, MOVEF, MOVEIARRAY, MOVEFARRAY at runtime */
    return;
  }
  ravi_type_map ex_ravi_type_map = ex->ravi_type_map;
  if (ex->k == VINDEXED) {
    if (ex_ravi_type_map == RAVI_TM_INTEGER_ARRAY) {
      ex_ravi_type_map = RAVI_TM_INTEGER;
    } else if (ex_ravi_type_map == RAVI_TM_FLOAT_ARRAY) {
      ex_ravi_type_map = RAVI_TM_FLOAT;
    } else {
      ex_ravi_type_map = RAVI_TM_ANY;
    }
  }
  if (var->ravi_type_map == RAVI_TM_FLOAT) {
    if (ex_ravi_type_map == RAVI_TM_FLOAT)
      return;
    if (ex_ravi_type_map == RAVI_TM_ANY) {
      int e = luaK_exp2RK(fs, ex);
      luaK_codeABC(fs, OP_RAVI_TOFLT, e, 0, 0);
      return;
    }
    luaX_syntaxerror(
      fs->ls,
      luaO_pushfstring(
      fs->ls->L,
      "Invalid assignment: number expected"));
  }
  else if (var->ravi_type_map == RAVI_TM_INTEGER) {
    if (ex_ravi_type_map == RAVI_TM_INTEGER)
      return;
    if (ex_ravi_type_map == RAVI_TM_ANY) {
       int e = luaK_exp2RK(fs, ex);
      luaK_codeABC(fs, OP_RAVI_TOINT, e, 0, 0);
      return;
    }
    luaX_syntaxerror(
      fs->ls,
      luaO_pushfstring(
      fs->ls->L,
      "Invalid assignment: integer expected"));
  }
  else if ((var->ravi_type_map & (~(RAVI_TM_INTEGER_ARRAY | RAVI_TM_FLOAT_ARRAY | RAVI_TM_TABLE))) == 0) {
    if (ex_ravi_type_map == var->ravi_type_map)
      return;
    if (ex_ravi_type_map == RAVI_TM_ANY) {
      OpCode opCode;
      if (var->ravi_type_map & RAVI_TM_INTEGER_ARRAY)
        opCode = OP_RAVI_TOIARRAY;
      else if (var->ravi_type_map & RAVI_TM_FLOAT_ARRAY)
        opCode = OP_RAVI_TOFARRAY;
      else
        opCode = OP_RAVI_TOTAB;
      int e = luaK_exp2RK(fs, ex);
      luaK_codeABC(fs, opCode, e, 0, 0);
      return;
    }
    luaX_syntaxerror(
      fs->ls,
      luaO_pushfstring(
      fs->ls->L,
      "Invalid assignment: %s expected",
      var->ravi_type_map == RAVI_TM_TABLE ? "table" : (var->ravi_type_map == RAVI_TM_FLOAT_ARRAY ? "number[]" : "integer[]")));
  }
  else if (var->ravi_type_map == RAVI_TM_STRING_OR_NIL) {
    if ((ex_ravi_type_map & ~RAVI_TM_STRING_OR_NIL) == 0)
      return;
    if (ex_ravi_type_map == RAVI_TM_ANY) {
      int e = luaK_exp2RK(fs, ex);
      luaK_codeABC(fs, OP_RAVI_TOSTRING, e, 0, 0);
      return;
    }
    luaX_syntaxerror(
      fs->ls,
      luaO_pushfstring(
      fs->ls->L,
      "Invalid assignment: string expected"));
  }
  else if (var->ravi_type_map == RAVI_TM_FUNCTION_OR_NIL) {
    if ((ex_ravi_type_map & ~RAVI_TM_FUNCTION_OR_NIL) == 0)
      return;
    if (ex_ravi_type_map == RAVI_TM_ANY) {
      int e = luaK_exp2RK(fs, ex);
      luaK_codeABC(fs, OP_RAVI_TOCLOSURE, e, 0, 0);
      return;
    }
    luaX_syntaxerror(
      fs->ls,
      luaO_pushfstring(
      fs->ls->L,
      "Invalid assignment: function expected"));
  }
  else if (var->ravi_type_map == RAVI_TM_USERDATA_OR_NIL) {
    if ((ex_ravi_type_map & ~RAVI_TM_USERDATA_OR_NIL) == 0 &&
      (!(ex_ravi_type_map & RAVI_TM_USERDATA) || (var->usertype && var->usertype == ex->usertype)))
      return;
    if (ex_ravi_type_map == RAVI_TM_ANY) {
      int e = luaK_exp2RK(fs, ex);
      luaK_codeABx(fs, OP_RAVI_TOTYPE, e, luaK_stringK(fs, var->usertype));
      return;
    }
    luaX_syntaxerror(
      fs->ls,
      luaO_pushfstring(
      fs->ls->L,
      "Invalid assignment: usertype %s expected", (var->usertype ? getstr(var->usertype) : "UNKNOWN")));
  }
}

static OpCode check_valid_setupval(FuncState *fs, expdesc *var, expdesc *ex,
                                   int reg) {
  OpCode op = OP_SETUPVAL;
  if ((var->ravi_type_map == RAVI_TM_INTEGER || var->ravi_type_map == RAVI_TM_FLOAT ||
       var->ravi_type_map == RAVI_TM_INTEGER_ARRAY || var->ravi_type_map == RAVI_TM_FLOAT_ARRAY ||
       var->ravi_type_map == RAVI_TM_TABLE || var->ravi_type_map == RAVI_TM_STRING_OR_NIL ||
       var->ravi_type_map == RAVI_TM_FUNCTION_OR_NIL || var->ravi_type_map == RAVI_TM_USERDATA_OR_NIL) &&
      ex->ravi_type_map & ~var->ravi_type_map) {
    if (var->ravi_type_map == RAVI_TM_INTEGER)
      op = OP_RAVI_SETUPVALI;
    else if (var->ravi_type_map == RAVI_TM_FLOAT)
      op = OP_RAVI_SETUPVALF;
    else if (var->ravi_type_map == RAVI_TM_INTEGER_ARRAY)
      op = OP_RAVI_SETUPVAL_IARRAY;
    else if (var->ravi_type_map == RAVI_TM_FLOAT_ARRAY)
      op = OP_RAVI_SETUPVAL_FARRAY;
    else if (var->ravi_type_map == RAVI_TM_TABLE)
      op = OP_RAVI_SETUPVALT;
    else if (var->ravi_type_map == RAVI_TM_STRING_OR_NIL)
      luaK_codeABC(fs, OP_RAVI_TOSTRING, reg, 0, 0);
    else if (var->ravi_type_map == RAVI_TM_FUNCTION_OR_NIL)
      luaK_codeABC(fs, OP_RAVI_TOCLOSURE, reg, 0, 0);
    else if (var->ravi_type_map == RAVI_TM_USERDATA_OR_NIL) {
      TString *usertype = fs->f->upvalues[var->u.info].usertype;
      luaK_codeABx(fs, OP_RAVI_TOTYPE, reg, luaK_stringK(fs, usertype));
    }
    else {
      char var_type_map_str[RAVI_TYPEMAP_MAX_LEN];
      char ex_type_map_str[RAVI_TYPEMAP_MAX_LEN];
      raviY_typemap_string(var->ravi_type_map, var_type_map_str);
      raviY_typemap_string(ex->ravi_type_map, ex_type_map_str);

      luaX_syntaxerror(fs->ls, luaO_pushfstring(fs->ls->L,
                                                "Invalid assignment of "
                                                "upvalue: upvalue type "
                                                "%s, expression type %s",
                                                var_type_map_str,
                                                ex_type_map_str));
    }
  }
  return op;
}

/* Emit store for LHS expression. */
void luaK_storevar (FuncState *fs, expdesc *var, expdesc *ex) {
  switch (var->k) {
    case VLOCAL: {
      check_valid_store(fs, var, ex);
      freeexp(fs, ex);
      exp2reg(fs, ex, var->u.info);  /* compute 'ex' into proper place */
      return;
    }
    case VUPVAL: {
      int e = luaK_exp2anyreg(fs, ex);
      OpCode op = check_valid_setupval(fs, var, ex, e);
      luaK_codeABC(fs, op, e, var->u.info, 0);
      break;
    }
    case VINDEXED: {
      OpCode op;
      int e = luaK_exp2RK(fs, ex);
      if (var->u.ind.vt == VLOCAL) {
        /* table value set - if array access then use specialized versions */
        if (var->u.ind.key_ravi_type_map == RAVI_TM_INTEGER) {
          if (var->ravi_type_map == RAVI_TM_FLOAT_ARRAY) {
                op = ex->ravi_type_map == RAVI_TM_FLOAT ? OP_RAVI_FARRAY_SETF : OP_RAVI_FARRAY_SET;
          } else if (var->ravi_type_map == RAVI_TM_INTEGER_ARRAY) {
                op = ex->ravi_type_map == RAVI_TM_INTEGER ? OP_RAVI_IARRAY_SETI : OP_RAVI_IARRAY_SET;
          } else {
            op = OP_RAVI_SETI;
          }
        } else if (var->u.ind.key_ravi_type_map == RAVI_TM_STRING && isshortstr(fs, var->u.ind.idx)) {
          op = var->ravi_type_map == RAVI_TM_TABLE ? OP_RAVI_TABLE_SETFIELD : OP_RAVI_SETFIELD;
        } else {
          op = OP_SETTABLE;
        }
      } else {
        op = OP_SETTABUP;
      }
      luaK_codeABC(fs, op, var->u.ind.t, var->u.ind.idx, e);
      break;
    }
    default: lua_assert(0);  /* invalid var kind to store */
  }
  freeexp(fs, ex);
}


/*
** Emit SELF instruction (convert expression 'e' into 'e:key(e,').
*/
void luaK_self (FuncState *fs, expdesc *e, expdesc *key) {
  int ereg;
  /* Ravi extension:
     If we only know the key is a string constant
     we emit specialized bytecode OP_RAVI_SELF_SK
     If we also know that the variable is a table 
     then we emit OP_RAVI_TABLE_SELF_SK
  */
  int is_string_constant_key =
    key->k == VK &&
    key->ravi_type_map == RAVI_TM_STRING &&
    ttisshrstring(&fs->f->k[key->u.info]);
  luaK_exp2anyreg(fs, e);
  // The check below needs to be 
  // after exp2anyreg as this can modify e->ravi_type
  int table_and_string =
	  e->ravi_type_map == RAVI_TM_TABLE &&
	  is_string_constant_key;
  ereg = e->u.info;  /* register where 'e' was placed */
  freeexp(fs, e);
  e->u.info = fs->freereg;  /* base register for op_self */
  e->k = VNONRELOC;  /* self expression has a fixed register */
  luaK_reserveregs(fs, 2);  /* function and 'self' produced by op_self */
  luaK_codeABC(fs, table_and_string ? OP_RAVI_TABLE_SELF_SK : 
                                      (is_string_constant_key ? OP_RAVI_SELF_SK : OP_SELF), 
                                      e->u.info, ereg, luaK_exp2RK(fs, key));
  freeexp(fs, key);
}


/*
** Negate condition 'e' (where 'e' is a comparison).
*/
static void negatecondition (FuncState *fs, expdesc *e) {
  Instruction *pc = getjumpcontrol(fs, e->u.info);
  lua_assert(testTMode(GET_OPCODE(*pc)) && GET_OPCODE(*pc) != OP_TESTSET &&
                                           GET_OPCODE(*pc) != OP_TEST);
  SETARG_A(*pc, !(GETARG_A(*pc)));
  DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set A to %d\n", e->u.info, *pc, GETARG_A(*pc)));
}


/*
** Emit instruction to jump if 'e' is 'cond' (that is, if 'cond'
** is true, code will jump if 'e' is true.) Return jump position.
** Optimize when 'e' is 'not' something, inverting the condition
** and removing the 'not'.
*/
static int jumponcond (FuncState *fs, expdesc *e, int cond) {
  if (e->k == VRELOCABLE) {
    Instruction ie = getinstruction(fs, e);
    if (GET_OPCODE(ie) == OP_NOT) {
      fs->pc--;  /* remove previous OP_NOT */
      return condjump(fs, OP_TEST, GETARG_B(ie), 0, !cond);
    }
    /* else go through */
  }
  discharge2anyreg(fs, e);
  freeexp(fs, e);
  return condjump(fs, OP_TESTSET, NO_REG, e->u.info, cond);
}


/*
** Emit code to go through if 'e' is true, jump otherwise.
*/
void luaK_goiftrue (FuncState *fs, expdesc *e) {
  int pc;  /* pc of new jump */
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VJMP: {  /* condition? */
      negatecondition(fs, e);  /* jump when it is false */
      pc = e->u.info;  /* save jump position */
      break;
    }
    case VK: case VKFLT: case VKINT: case VTRUE: {
      pc = NO_JUMP;  /* always true; do nothing */
      break;
    }
    default: {
      if (e->ravi_type_map & RAVI_TM_FALSISH) {
        pc = jumponcond(fs, e, 0); /* jump when false */
      }
      else {
        pc = NO_JUMP; /* always true; do nothing */
      }
      break;
    }
  }
  luaK_concat(fs, &e->f, pc);  /* insert new jump in false list */
  luaK_patchtohere(fs, e->t);  /* true list jumps to here (to go through) */
  e->t = NO_JUMP;
}


/*
** Emit code to go through if 'e' is false, jump otherwise.
*/
void luaK_goiffalse (FuncState *fs, expdesc *e) {
  int pc;  /* pc of new jump */
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VJMP: {
      pc = e->u.info;  /* already jump if true */
      break;
    }
    case VNIL: case VFALSE: {
      pc = NO_JUMP;  /* always false; do nothing */
      break;
    }
    default: {
      if (e->ravi_type_map & RAVI_TM_TRUISH) {
        pc = jumponcond(fs, e, 1); /* jump if true */
      }
      else {
        pc = NO_JUMP; /* always false; do nothing */
      }
      break;
    }
  }
  luaK_concat(fs, &e->t, pc);  /* insert new jump in 't' list */
  luaK_patchtohere(fs, e->f);  /* false list jumps to here (to go through) */
  e->f = NO_JUMP;
}


/*
** Code 'not e', doing constant folding.
*/
static void codenot (FuncState *fs, expdesc *e) {
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VNIL: case VFALSE: {
      e->k = VTRUE;  /* true == not nil == not false */
      break;
    }
    case VK: case VKFLT: case VKINT: case VTRUE: {
      e->k = VFALSE;  /* false == not "x" == not 0.5 == not 1 == not true */
      break;
    }
    case VJMP: {
      negatecondition(fs, e);
      break;
    }
    case VRELOCABLE:
    case VNONRELOC: {
      discharge2anyreg(fs, e);
      freeexp(fs, e);
      e->u.info = luaK_codeABC(fs, OP_NOT, 0, e->u.info, 0);
      e->k = VRELOCABLE;
      break;
    }
    default: lua_assert(0);  /* cannot happen */
  }
  e->ravi_type_map = ((e->ravi_type_map & RAVI_TM_TRUISH) ? RAVI_TM_FALSE : 0) |
                     ((e->ravi_type_map & RAVI_TM_FALSISH) ? RAVI_TM_TRUE : 0);
  /* interchange true and false lists */
  { int temp = e->f; e->f = e->t; e->t = temp; }
  removevalues(fs, e->f);  /* values are useless when negated */
  removevalues(fs, e->t);
}


/*
** Create expression 't[k]'. 't' must have its final result already in a
** register or upvalue.
*/
void luaK_indexed (FuncState *fs, expdesc *t, expdesc *k) {
  lua_assert(!hasjumps(t) && (vkisinreg(t->k) || t->k == VUPVAL));
  t->u.ind.t = t->u.info;  /* register or upvalue index */
  t->u.ind.key_ravi_type_map = k->ravi_type_map;   /* RAVI record the key type */
  t->u.ind.usertype = k->usertype; /* RAVI record the key type */
  t->u.ind.idx = luaK_exp2RK(fs, k);  /* R/K index for key */
  t->u.ind.vt = (t->k == VUPVAL) ? VUPVAL : VLOCAL;
  t->k = VINDEXED;
}


/*
** Return false if folding can raise an error.
** Bitwise operations need operands convertible to integers; division
** operations cannot have 0 as divisor.
*/
static int validop (int op, TValue *v1, TValue *v2) {
  switch (op) {
    case LUA_OPBAND: case LUA_OPBOR: case LUA_OPBXOR:
    case LUA_OPSHL: case LUA_OPSHR: case LUA_OPBNOT: {  /* conversion errors */
      lua_Integer i;
      return (tointeger(v1, &i) && tointeger(v2, &i));
    }
    case LUA_OPDIV: case LUA_OPIDIV: case LUA_OPMOD:  /* division by 0 */
      return (nvalue(v2) != 0);
    default: return 1;  /* everything else is valid */
  }
}


/*
** Try to "constant-fold" an operation; return 1 iff successful.
** (In this case, 'e1' has the final result.)
*/
static int constfolding (FuncState *fs, int op, expdesc *e1,
                                                const expdesc *e2) {
  TValue v1, v2, res;
  if (!tonumeral(e1, &v1) || !tonumeral(e2, &v2) || !validop(op, &v1, &v2))
    return 0;  /* non-numeric operands or not safe to fold */
  luaO_arith(fs->ls->L, op, &v1, &v2, &res);  /* does operation */
  if (ttisinteger(&res)) {
    e1->k = VKINT;
    e1->u.ival = ivalue(&res);
    e1->ravi_type_map = RAVI_TM_INTEGER;
  }
  else {  /* folds neither NaN nor 0.0 (to avoid problems with -0.0) */
    lua_Number n = fltvalue(&res);
    if (luai_numisnan(n) || n == 0)
      return 0;
    e1->k = VKFLT;
    e1->u.nval = n;
    e1->ravi_type_map = RAVI_TM_FLOAT;
  }
  return 1;
}


/*
** Emit code for unary expressions that "produce values"
** (everything but 'not').
** Expression to produce final result will be encoded in 'e'.
*/
static void codeunexpval (FuncState *fs, OpCode op, expdesc *e, int line) {
  int r = luaK_exp2anyreg(fs, e);  /* opcodes operate only on registers */
  freeexp(fs, e);
  switch (op) {
    case OP_BNOT:
      if (e->ravi_type_map == RAVI_TM_INTEGER) {
        e->u.info = luaK_codeABC(fs, OP_RAVI_BNOT_I, 0, r, 0);
        break;
      }
      e->u.info = luaK_codeABC(fs, OP_BNOT, 0, r, 0);
      e->ravi_type_map = e->ravi_type_map == RAVI_TM_FLOAT ? RAVI_TM_INTEGER : RAVI_TM_ANY;
      break;
    case OP_LEN:
      e->u.info = luaK_codeABC(fs, OP_LEN, 0, r, 0);
      if ((e->ravi_type_map & (~(RAVI_TM_INTEGER_ARRAY | RAVI_TM_FLOAT_ARRAY | RAVI_TM_STRING))) == 0) {
        e->ravi_type_map = RAVI_TM_INTEGER;
      }
      else if ((e->ravi_type_map & (~(RAVI_TM_INTEGER_ARRAY | RAVI_TM_FLOAT_ARRAY | RAVI_TM_STRING | RAVI_TM_TABLE))) == 0) {
        e->k = VRELOCABLE;
        luaK_exp2anyreg(fs, e);
        /* This is not incompatible with lua since a type annotation is require to get here or the table trivially has
         * no metatable */
        luaK_codeABC(fs, OP_RAVI_TOINT, e->u.info, 0, 0);
        e->ravi_type_map = RAVI_TM_INTEGER;
        luaK_fixline(fs, line);
        return;
      }
      else {
        e->ravi_type_map = RAVI_TM_ANY;
      }
      break;
    case OP_UNM:
      e->u.info = luaK_codeABC(fs, OP_UNM, 0, r, 0);
      if (e->ravi_type_map & ~RAVI_TM_NUMBER) {
        e->ravi_type_map = RAVI_TM_ANY;
      }
      break;
    default:
      e->u.info = luaK_codeABC(fs, op, 0, r, 0);
      e->ravi_type_map = RAVI_TM_ANY;
  }
  e->k = VRELOCABLE;  /* all those operations are relocatable */
  luaK_fixline(fs, line);
}

/*
** Emit code for binary expressions that "produce values"
** (everything but logical operators 'and'/'or' and comparison
** operators).
** Expression to produce final result will be encoded in 'e1'.
** Because 'luaK_exp2RK' can free registers, its calls must be
** in "stack order" (that is, first on 'e2', which may have more
** recent registers to be released).
*/
static void codebinexpval (FuncState *fs, OpCode op,
                           expdesc *e1, expdesc *e2, int line) {
  /* Note that the order below is important - see Lua 5.3.3 bug list*/
  int rk2 = luaK_exp2RK(fs, e2);  /* both operands are "RK" */
  int rk1 = luaK_exp2RK(fs, e1);
  freeexps(fs, e1, e2);

#define RAVI_OPCODE_SPECIALIZED(op, t) OP_RAVI_##op##t
#define RAVI_OPCODE_GENERIC(op, t) OP_##op
#define RAVI_COMMUTATIVE(op, t) luaK_codeABC(fs, t(op, FI), 0, rk2, rk1)
#define RAVI_NON_COMMUTATIVE(op, t) luaK_codeABC(fs, t(op, IF), 0, rk1, rk2)
#define RAVI_GEN_ARITH(op, co, ii, t)                                      \
  case OP_##op:                                                            \
    if (e1->ravi_type_map == RAVI_TM_FLOAT) {                              \
      if (e2->ravi_type_map == RAVI_TM_FLOAT) {                            \
        e1->u.info = luaK_codeABC(fs, t(op, FF), 0, rk1, rk2);             \
        break;                                                             \
      }                                                                    \
      else if (e2->ravi_type_map == RAVI_TM_INTEGER) {                     \
        e1->u.info = luaK_codeABC(fs, t(op, FI), 0, rk1, rk2);             \
        break;                                                             \
      }                                                                    \
    }                                                                      \
    else if (e1->ravi_type_map == RAVI_TM_INTEGER) {                       \
      if (e2->ravi_type_map == RAVI_TM_FLOAT) {                            \
        e1->u.info = co(op, t);                                            \
        e1->ravi_type_map = RAVI_TM_FLOAT;                                 \
        break;                                                             \
      }                                                                    \
      else if (e2->ravi_type_map == RAVI_TM_INTEGER) {                     \
        e1->u.info = luaK_codeABC(fs, t(op, II), 0, rk1, rk2);             \
        e1->ravi_type_map = ii;                                            \
        break;                                                             \
      }                                                                    \
    }                                                                      \
    e1->u.info = luaK_codeABC(fs, OP_##op, 0, rk1, rk2);                   \
    if ((e1->ravi_type_map & (~(RAVI_TM_FLOAT | RAVI_TM_INTEGER))) == 0 && \
        (e2->ravi_type_map & (~(RAVI_TM_FLOAT | RAVI_TM_INTEGER))) == 0) { \
      if (e1->ravi_type_map & e2->ravi_type_map & RAVI_TM_INTEGER) {       \
        e1->ravi_type_map = RAVI_TM_FLOAT | ii;                            \
      }                                                                    \
      else {                                                               \
        e1->ravi_type_map = RAVI_TM_FLOAT;                                 \
      }                                                                    \
    }                                                                      \
    else {                                                                 \
      e1->ravi_type_map = RAVI_TM_ANY;                                     \
    }                                                                      \
    break

#define RAVI_GEN_INT_OP(op)                                                                                  \
  case OP_##op:                                                                                              \
    if (e1->ravi_type_map == RAVI_TM_INTEGER && e2->ravi_type_map == RAVI_TM_INTEGER) {                      \
      e1->u.info = luaK_codeABC(fs, OP_RAVI_##op##_II, 0, rk1, rk2);                                         \
    }                                                                                                        \
    else if ((e1->ravi_type_map & (~RAVI_TM_NUMBER)) == 0 && (e2->ravi_type_map & (~RAVI_TM_NUMBER)) == 0) { \
      e1->u.info = luaK_codeABC(fs, OP_##op, 0, rk1, rk2);                                                   \
      e1->ravi_type_map = RAVI_TM_INTEGER;                                                                   \
    }                                                                                                        \
    else {                                                                                                   \
      e1->u.info = luaK_codeABC(fs, OP_##op, 0, rk1, rk2);                                                   \
      e1->ravi_type_map = RAVI_TM_ANY;                                                                       \
    }                                                                                                        \
    break

  switch (op) {
    RAVI_GEN_ARITH(ADD, RAVI_COMMUTATIVE, RAVI_TM_INTEGER, RAVI_OPCODE_SPECIALIZED);
    RAVI_GEN_ARITH(SUB, RAVI_NON_COMMUTATIVE, RAVI_TM_INTEGER, RAVI_OPCODE_SPECIALIZED);
    RAVI_GEN_ARITH(MUL, RAVI_COMMUTATIVE, RAVI_TM_INTEGER, RAVI_OPCODE_SPECIALIZED);
    RAVI_GEN_ARITH(DIV, RAVI_NON_COMMUTATIVE, RAVI_TM_FLOAT, RAVI_OPCODE_SPECIALIZED);
    RAVI_GEN_ARITH(IDIV, RAVI_NON_COMMUTATIVE, RAVI_TM_INTEGER, RAVI_OPCODE_GENERIC);
    RAVI_GEN_ARITH(MOD, RAVI_NON_COMMUTATIVE, RAVI_TM_INTEGER, RAVI_OPCODE_GENERIC);
    RAVI_GEN_ARITH(POW, RAVI_NON_COMMUTATIVE, RAVI_TM_FLOAT, RAVI_OPCODE_GENERIC);
    RAVI_GEN_INT_OP(BAND);
    RAVI_GEN_INT_OP(BOR);
    RAVI_GEN_INT_OP(BXOR);
    RAVI_GEN_INT_OP(SHL);
    RAVI_GEN_INT_OP(SHR);
    case OP_CONCAT:
      e1->u.info = luaK_codeABC(fs, op, 0, rk1, rk2);
      if ((e1->ravi_type_map & (~(RAVI_TM_STRING | RAVI_TM_INTEGER | RAVI_TM_FLOAT))) == 0 &&
          (e2->ravi_type_map & (~(RAVI_TM_STRING | RAVI_TM_INTEGER | RAVI_TM_FLOAT))) == 0) {
        e1->ravi_type_map = RAVI_TM_STRING;
      }
      else {
        e1->ravi_type_map = RAVI_TM_ANY;
      }
      break;
    default:
      e1->u.info = luaK_codeABC(fs, op, 0, rk1, rk2);
      e1->ravi_type_map = RAVI_TM_ANY;
  }

  e1->k = VRELOCABLE;  /* all those operations are relocatable */
  luaK_fixline(fs, line);
}


static OpCode get_type_specific_comp_op(OpCode op, ravi_type_map o1_tm, ravi_type_map o2_tm) {
  if (op == OP_EQ) {
    if (o1_tm == RAVI_TM_INTEGER && o2_tm == RAVI_TM_INTEGER)
      op = OP_RAVI_EQ_II;
    else if (o1_tm == RAVI_TM_FLOAT && o2_tm == RAVI_TM_FLOAT)
      op = OP_RAVI_EQ_FF;
  }
  else if (op == OP_LT) {
    if (o1_tm == RAVI_TM_INTEGER && o2_tm == RAVI_TM_INTEGER)
      op = OP_RAVI_LT_II;
    else if (o1_tm == RAVI_TM_FLOAT && o2_tm == RAVI_TM_FLOAT)
      op = OP_RAVI_LT_FF;
  }
  else if (op == OP_LE) {
    if (o1_tm == RAVI_TM_INTEGER && o2_tm == RAVI_TM_INTEGER)
      op = OP_RAVI_LE_II;
    else if (o1_tm == RAVI_TM_FLOAT && o2_tm == RAVI_TM_FLOAT)
      op = OP_RAVI_LE_FF;
  }
  return op;  
}

/*
** Emit code for comparisons.
** 'e1' was already put in R/K form by 'luaK_infix'.
*/
static void codecomp (FuncState *fs, BinOpr opr, expdesc *e1, expdesc *e2) {
  DEBUG_EXPR(raviY_printf(fs, "Comparison of %e and %e\n", e1, e2));
  int rk1 = (e1->k == VK) ? RKASK(e1->u.info)
                          : check_exp(e1->k == VNONRELOC, e1->u.info);
  ravi_type_map rk1_tm = e1->ravi_type_map;
  int rk2 = luaK_exp2RK(fs, e2);
  ravi_type_map rk2_tm = e2->ravi_type_map;
  freeexps(fs, e1, e2);
  switch (opr) {
    case OPR_NE: {  /* '(a ~= b)' ==> 'not (a == b)' */
      OpCode op = get_type_specific_comp_op(OP_EQ, rk1_tm, rk2_tm);
      e1->u.info = condjump(fs, op, 0, rk1, rk2);
      break;
    }
    case OPR_GT: case OPR_GE: {
      /* '(a > b)' ==> '(b < a)';  '(a >= b)' ==> '(b <= a)' */
      OpCode op = cast(OpCode, (opr - OPR_NE) + OP_EQ);
      op = get_type_specific_comp_op(op, rk2_tm, rk1_tm);
      e1->u.info = condjump(fs, op, 1, rk2, rk1);  /* invert operands */
      break;
    }
    default: {  /* '==', '<', '<=' use their own opcodes */
      OpCode op = cast(OpCode, (opr - OPR_EQ) + OP_EQ);
      op = get_type_specific_comp_op(op, rk1_tm, rk2_tm);
      e1->u.info = condjump(fs, op, 1, rk1, rk2);
      break;
    }
  }
  e1->k = VJMP;
  e1->ravi_type_map = RAVI_TM_BOOLEAN;
}


/*
** This is a Ravi extension to implement the type
** assertion operators. A type assertion operator is a Unary 
** operator that takes one operand - a register location - and
** asserts that the register contains a value of the required type.
 */
static void code_type_assertion(FuncState *fs, UnOpr op, expdesc *e, TString *usertype) {
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VKFLT: {
      if (op == OPR_TO_NUMBER) {
        lua_assert(e->ravi_type_map == RAVI_TM_FLOAT);
        return;
      }
      break;
    }
    case VKINT: {
      if (op == OPR_TO_INTEGER) {
        lua_assert(e->ravi_type_map == RAVI_TM_INTEGER);
        return;
      }
      break;
    }
    case VK: {
      if (op == OPR_TO_STRING) {
        if (e->ravi_type_map == RAVI_TM_STRING)
	  return;
      }
      break;
    }
    case VRELOCABLE:
    case VNONRELOC: {
      discharge2anyreg(fs, e);
      OpCode opcode;
      ravi_type_map tm;
      if (op == OPR_TO_NUMBER && e->ravi_type_map != RAVI_TM_FLOAT) {
        opcode = OP_RAVI_TOFLT;
        tm = RAVI_TM_FLOAT;
      }
      else if (op == OPR_TO_INTEGER && e->ravi_type_map != RAVI_TM_INTEGER) {
        opcode = OP_RAVI_TOINT;
        tm = RAVI_TM_INTEGER;
      }
      else if (op == OPR_TO_INTARRAY && e->ravi_type_map != RAVI_TM_INTEGER_ARRAY) {
        if (e->ravi_type_map == RAVI_TM_TABLE && e->pc >= 0) {
          Instruction *i = &fs->f->code[e->pc];
          if (GET_OPCODE(*i) == OP_NEWTABLE) {
            SET_OPCODE(*i, OP_RAVI_NEW_IARRAY);
            e->ravi_type_map = RAVI_TM_INTEGER_ARRAY;
            DEBUG_EXPR(raviY_printf(fs, "code_type_assertion (OP_NEWTABLE to OP_RAVI_NEW_IARRAY) %e\n", e));
          }
          return;
        }
        opcode = OP_RAVI_TOIARRAY;
        tm = RAVI_TM_INTEGER_ARRAY;
      }
      else if (op == OPR_TO_NUMARRAY && e->ravi_type_map != RAVI_TM_FLOAT_ARRAY) {
        if (e->ravi_type_map == RAVI_TM_TABLE && e->pc >= 0) {
          Instruction *i = &fs->f->code[e->pc];
          if (GET_OPCODE(*i) == OP_NEWTABLE) {
            SET_OPCODE(*i, OP_RAVI_NEW_FARRAY);
            e->ravi_type_map = RAVI_TM_FLOAT_ARRAY;
            DEBUG_EXPR(raviY_printf(fs, "code_type_assertion (OP_NEWTABLE to OP_RAVI_NEW_IARRAY) %e\n", e));
          }
          return;
        }
        opcode = OP_RAVI_TOFARRAY;
        tm = RAVI_TM_FLOAT_ARRAY;
      }
      else if (op == OPR_TO_TABLE && e->ravi_type_map != RAVI_TM_TABLE) {
        opcode = OP_RAVI_TOTAB;
        tm = RAVI_TM_TABLE;
      }
      else if (op == OPR_TO_STRING && (e->ravi_type_map & (~RAVI_TM_STRING_OR_NIL)) != 0) {
        opcode = OP_RAVI_TOSTRING;
        tm = RAVI_TM_STRING_OR_NIL;
      }
      else if (op == OPR_TO_CLOSURE && (e->ravi_type_map & (~RAVI_TM_FUNCTION_OR_NIL)) != 0) {
        opcode = OP_RAVI_TOCLOSURE;
        tm = RAVI_TM_FUNCTION_OR_NIL;
      }
      else if (op == OPR_TO_TYPE) {
        opcode = OP_RAVI_TOTYPE;
        tm = RAVI_TM_USERDATA_OR_NIL;
      }
      else {
        /* nothing to do*/
        return;
      }
      /* Must already be NONRELOC */
      if (opcode == OP_RAVI_TOTYPE) {
        luaK_codeABx(fs, opcode, e->u.info, luaK_stringK(fs, usertype));
      }
      else 
        luaK_codeABC(fs, opcode, e->u.info, 0, 0);
      e->ravi_type_map = tm;
      e->k = VNONRELOC; 
      if (opcode == OP_RAVI_TOTYPE)
        e->usertype = usertype;
      return;
    }
    default: break;
  }
  luaX_syntaxerror(fs->ls, "invalid type assertion");  
}

/*
** Apply prefix operation 'op' to expression 'e'.
*/
void luaK_prefix (FuncState *fs, UnOpr op, expdesc *e, int line, TString *usertype) {
  expdesc ef = {.ravi_type_map = RAVI_TM_ANY,
                .pc = -1,
                .t = NO_JUMP,
                .f = NO_JUMP,
                .k = VKINT,
                .u.ival = 0};  /* fake 2nd operand */
  switch (op) {
    case OPR_MINUS: case OPR_BNOT:  /* use 'ef' as fake 2nd operand */
      if (constfolding(fs, op + LUA_OPUNM, e, &ef))
        break;
      /* FALLTHROUGH */
    case OPR_LEN:
      codeunexpval(fs, cast(OpCode, op + OP_UNM), e, line);
      break;
    case OPR_TO_INTEGER: case OPR_TO_NUMBER: case OPR_TO_INTARRAY:
    case OPR_TO_NUMARRAY: case OPR_TO_TABLE: case OPR_TO_STRING: case OPR_TO_CLOSURE:
      code_type_assertion(fs, op, e, NULL); break;
    case OPR_TO_TYPE:
      code_type_assertion(fs, op, e, usertype); break;      
    case OPR_NOT: codenot(fs, e); break;
    default: lua_assert(0);
  }
}


/*
** Process 1st operand 'v' of binary operation 'op' before reading
** 2nd operand.
*/
void luaK_infix (FuncState *fs, BinOpr op, expdesc *v) {
  switch (op) {
    case OPR_AND: {
      luaK_goiftrue(fs, v);  /* go ahead only if 'v' is true */
      break;
    }
    case OPR_OR: {
      luaK_goiffalse(fs, v);  /* go ahead only if 'v' is false */
      break;
    }
    case OPR_CONCAT: {
      luaK_exp2nextreg(fs, v);  /* operand must be on the 'stack' */
      break;
    }
    case OPR_ADD: case OPR_SUB:
    case OPR_MUL: case OPR_DIV: case OPR_IDIV:
    case OPR_MOD: case OPR_POW:
    case OPR_BAND: case OPR_BOR: case OPR_BXOR:
    case OPR_SHL: case OPR_SHR: {
      if (!tonumeral(v, NULL))
        luaK_exp2RK(fs, v);
      /* else keep numeral, which may be folded with 2nd operand */
      break;
    }
    default: {
      luaK_exp2RK(fs, v);
      break;
    }
  }
}


/*
** Finalize code for binary operation, after reading 2nd operand.
** For '(a .. b .. c)' (which is '(a .. (b .. c))', because
** concatenation is right associative), merge second CONCAT into first
** one.
*/
void luaK_posfix (FuncState *fs, BinOpr op,
                  expdesc *e1, expdesc *e2, int line) {
  switch (op) {
    case OPR_AND: {
      lua_assert(e1->t == NO_JUMP);  /* list closed by 'luK_infix' */
      luaK_dischargevars(fs, e2);
      luaK_concat(fs, &e2->f, e1->f);
      if (e1->ravi_type_map & RAVI_TM_TRUISH) {
        e2->ravi_type_map |= e1->ravi_type_map & RAVI_TM_FALSISH;
      } else {
        e2->ravi_type_map = e1->ravi_type_map & RAVI_TM_FALSISH;
      }
      *e1 = *e2;
      break;
    }
    case OPR_OR: {
      lua_assert(e1->f == NO_JUMP);  /* list closed by 'luK_infix' */
      luaK_dischargevars(fs, e2);
      luaK_concat(fs, &e2->t, e1->t);
      if (e1->ravi_type_map & e2->ravi_type_map & RAVI_TM_USERDATA) {
        if (e1->usertype != e2->usertype)
          e2->usertype = NULL;
      }
      if (e1->ravi_type_map & RAVI_TM_FALSISH) {
        e2->ravi_type_map |= e1->ravi_type_map & RAVI_TM_TRUISH;
      } else {
        e2->ravi_type_map = e1->ravi_type_map & RAVI_TM_TRUISH;
      }
      *e1 = *e2;
      break;
    }
    case OPR_CONCAT: {
      luaK_exp2val(fs, e2);
      if (e2->k == VRELOCABLE &&
          GET_OPCODE(getinstruction(fs, e2)) == OP_CONCAT) {
        lua_assert(e1->u.info == GETARG_B(getinstruction(fs, e2))-1);
        freeexp(fs, e1);
        SETARG_B(getinstruction(fs, e2), e1->u.info);
        DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set A to %d\n", e2->u.info, getinstruction(fs,e2), e1->u.info));
        e1->k = VRELOCABLE; e1->u.info = e2->u.info;
        if (e2->ravi_type_map == RAVI_TM_STRING &&
            (e1->ravi_type_map & (~(RAVI_TM_STRING | RAVI_TM_INTEGER | RAVI_TM_FLOAT))) == 0) {
          e1->ravi_type_map = RAVI_TM_STRING;
        }
        else {
          e1->ravi_type_map = RAVI_TM_ANY;
        }
      }
      else {
        luaK_exp2nextreg(fs, e2);  /* operand must be on the 'stack' */
        codebinexpval(fs, OP_CONCAT, e1, e2, line);
      }
      break;
    }
    case OPR_ADD: case OPR_SUB: case OPR_MUL: case OPR_DIV:
    case OPR_IDIV: case OPR_MOD: case OPR_POW:
    case OPR_BAND: case OPR_BOR: case OPR_BXOR:
    case OPR_SHL: case OPR_SHR: {
      if (!constfolding(fs, op + LUA_OPADD, e1, e2))
        codebinexpval(fs, cast(OpCode, op + OP_ADD), e1, e2, line);
      break;
    }
    case OPR_EQ: case OPR_LT: case OPR_LE:
    case OPR_NE: case OPR_GT: case OPR_GE: {
      codecomp(fs, op, e1, e2);
      break;
    }
    default: lua_assert(0);
  }
}


/*
** Change line information associated with current position.
*/
void luaK_fixline (FuncState *fs, int line) {
  fs->f->lineinfo[fs->pc - 1] = line;
}


/*
** Emit a SETLIST instruction.
** 'base' is register that keeps table;
** 'nelems' is #table plus those to be stored now;
** 'tostore' is number of values (in registers 'base + 1',...) to add to
** table (or LUA_MULTRET to add up to stack top).
*/
void luaK_setlist (FuncState *fs, int base, int nelems, int tostore) {
  int c =  (nelems - 1)/LFIELDS_PER_FLUSH + 1;
  int b = (tostore == LUA_MULTRET) ? 0 : tostore;
  lua_assert(tostore != 0 && tostore <= LFIELDS_PER_FLUSH);
  if (c <= MAXARG_C)
    luaK_codeABC(fs, OP_SETLIST, base, b, c);
  else if (c <= MAXARG_Ax) {
    luaK_codeABC(fs, OP_SETLIST, base, b, 0);
    codeextraarg(fs, c);
  }
  else
    luaX_syntaxerror(fs->ls, "constructor too long");
  fs->freereg = base + 1;  /* free registers with list values */
}


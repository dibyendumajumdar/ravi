/*
** $Id: lcode.c,v 2.99 2014/12/29 16:49:25 roberto Exp $
** Code generator for Lua
** See Copyright Notice in lua.h
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


/* Maximum number of registers in a Lua function */
#define MAXREGS		MAXARG_A
/* RAVI change; was 250 */


#define hasjumps(e)	((e)->t != (e)->f)


static int tonumeral(expdesc *e, TValue *v) {
  if (e->t != NO_JUMP || e->f != NO_JUMP)
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

/* Emit bytecode to set a range of registers to nil. */
void luaK_nil (FuncState *fs, int from, int n) {
  Instruction *previous;
  int l = from + n - 1;  /* last register to set nil */
  if (fs->pc > fs->lasttarget) {  /* no jumps to current position? */
    previous = &fs->f->code[fs->pc-1];
    if (GET_OPCODE(*previous) == OP_LOADNIL) { /* Try to merge with the previous instruction. */
      int pfrom = GETARG_A(*previous);
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

/* Emit unconditional branch. */
int luaK_jump (FuncState *fs) {
  int jpc = fs->jpc;  /* save list of jumps to here */
  int j;
  fs->jpc = NO_JUMP;
  j = luaK_codeAsBx(fs, OP_JMP, 0, NO_JUMP);
  luaK_concat(fs, &j, jpc);  /* keep them on hold */
  return j;
}


void luaK_ret (FuncState *fs, int first, int nret) {
  luaK_codeABC(fs, OP_RETURN, first, nret+1, 0);
}

/* Emit conditional branch. */
static int condjump (FuncState *fs, OpCode op, int A, int B, int C) {
  luaK_codeABC(fs, op, A, B, C);
  return luaK_jump(fs);
}

/* Patch jump instruction to target. */
static void fixjump (FuncState *fs, int pc, int dest) {
  Instruction *jmp = &fs->f->code[pc];
  int offset = dest-(pc+1);
  lua_assert(dest != NO_JUMP);
  if (abs(offset) > MAXARG_sBx)
    luaX_syntaxerror(fs->ls, "control structure too long");
  SETARG_sBx(*jmp, offset);
  DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set sBx to %d\n", pc, *jmp, offset));
}


/*
** returns current 'pc' and marks it as a jump target (to avoid wrong
** optimizations with consecutive instructions not in the same basic block).
*/
int luaK_getlabel (FuncState *fs) {
  fs->lasttarget = fs->pc;
  return fs->pc;
}


static int getjump (FuncState *fs, int pc) {
  int offset = GETARG_sBx(fs->f->code[pc]);
  if (offset == NO_JUMP)  /* point to itself represents end of list */
    return NO_JUMP;  /* end of list */
  else
    return (pc+1)+offset;  /* turn offset into absolute position */
}


static Instruction *getjumpcontrol (FuncState *fs, int pc) {
  Instruction *pi = &fs->f->code[pc];
  if (pc >= 1 && testTMode(GET_OPCODE(*(pi-1))))
    return pi-1;
  else
    return pi;
}


/*
** check whether list has any jump that do not produce a value
** (or produce an inverted value)
*/
static int need_value (FuncState *fs, int list) {
  for (; list != NO_JUMP; list = getjump(fs, list)) {
    Instruction i = *getjumpcontrol(fs, list);
    if (GET_OPCODE(i) != OP_TESTSET) return 1;
  }
  return 0;  /* not found */
}

/* Patch register of test instructions. */
static int patchtestreg (FuncState *fs, int node, int reg) {
  Instruction *i = getjumpcontrol(fs, node);
  if (GET_OPCODE(*i) != OP_TESTSET)
    return 0;  /* cannot patch other instructions */
  if (reg != NO_REG && reg != GETARG_B(*i)) {
    SETARG_A(*i, reg);
    DEBUG_CODEGEN(raviY_printf(fs, "[?]* %o ; set A to %d\n", *i, reg));
  }
  else  /* no register to put value or register already has the value */ {
    *i = CREATE_ABC(OP_TEST, GETARG_B(*i), 0, GETARG_C(*i));
    DEBUG_CODEGEN(raviY_printf(fs, "[?]* %o ; generate OP_TEST\n", *i));
  }

  return 1;
}

/* Drop values for all instructions on jump list. */
static void removevalues (FuncState *fs, int list) {
  for (; list != NO_JUMP; list = getjump(fs, list))
      patchtestreg(fs, list, NO_REG);
}

/* Patch jump list and preserve produced values. */
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


static void dischargejpc (FuncState *fs) {
  patchlistaux(fs, fs->jpc, fs->pc, NO_REG, fs->pc);
  fs->jpc = NO_JUMP;
}


void luaK_patchlist (FuncState *fs, int list, int target) {
  if (target == fs->pc)
    luaK_patchtohere(fs, list);
  else {
    lua_assert(target < fs->pc);
    patchlistaux(fs, list, target, NO_REG, target);
  }
}


void luaK_patchclose (FuncState *fs, int list, int level) {
  level++;  /* argument is +1 to reserve 0 as non-op */
  while (list != NO_JUMP) {
    int next = getjump(fs, list);
    lua_assert(GET_OPCODE(fs->f->code[list]) == OP_JMP &&
                (GETARG_A(fs->f->code[list]) == 0 ||
                 GETARG_A(fs->f->code[list]) >= level));
    SETARG_A(fs->f->code[list], level);
    DEBUG_CODEGEN(raviY_printf(fs, "[?]* %o ; set A to %d\n", fs->f->code[list], level));
    list = next;
  }
}


void luaK_patchtohere (FuncState *fs, int list) {
  luaK_getlabel(fs);
  luaK_concat(fs, &fs->jpc, list);
}


void luaK_concat (FuncState *fs, int *l1, int l2) {
  if (l2 == NO_JUMP) return;
  else if (*l1 == NO_JUMP)
    *l1 = l2;
  else {
    int list = *l1;
    int next;
    while ((next = getjump(fs, list)) != NO_JUMP)  /* find last element */
      list = next;
    fixjump(fs, list, l2);
  }
}


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

/* Code arith where C is a constant */
int luaK_codeABKi(FuncState *fs, OpCode o, int a, int b, int c) {
  TValue *v;
  int i;
  lua_Integer val;
  lua_assert(ISK(c));
  i = INDEXK(c);
  lua_assert(i >= 0 && i < fs->nk);
  v = &fs->f->k[i];
  lua_assert(ttisinteger(v));
  val = ivalue(v);
  /* if the value is upto MAXINDEXRK then it means we can
   * hold it in operand C  - operand C reserves 1 bit for indicating
   * constants hence MAXINDEXRK and not MAXARG_C
   */
  if (val >= 0 && val <= MAXINDEXRK) {
    if (o == OP_RAVI_ADDFI) {
      o = OP_RAVI_ADDFN;
      c = (int)val;
    }
    else if (o == OP_RAVI_ADDII) {
      o = OP_RAVI_ADDIN;
      c = (int)val;
    }
    else if (o == OP_RAVI_MULFI) {
      o = OP_RAVI_MULFN;
      c = (int)val;
    }
    else if (o == OP_RAVI_MULII) {
      o = OP_RAVI_MULIN;
      c = (int)val;
    }
  }
  return luaK_codeABC(fs, o, a, b, c);
}

int luaK_codeABC (FuncState *fs, OpCode o, int a, int b, int c) {
  lua_assert(getOpMode(o) == iABC);
  lua_assert(getBMode(o) != OpArgN || b == 0);
  lua_assert(getCMode(o) != OpArgN || c == 0);
  lua_assert(a <= MAXARG_A && b <= MAXARG_B && c <= MAXARG_C);
  return luaK_code(fs, CREATE_ABC(o, a, b, c));
}


int luaK_codeABx (FuncState *fs, OpCode o, int a, unsigned int bc) {
  lua_assert(getOpMode(o) == iABx || getOpMode(o) == iAsBx);
  lua_assert(getCMode(o) == OpArgN);
  lua_assert(a <= MAXARG_A && bc <= MAXARG_Bx);
  return luaK_code(fs, CREATE_ABx(o, a, bc));
}


static int codeextraarg (FuncState *fs, int a) {
  lua_assert(a <= MAXARG_Ax);
  return luaK_code(fs, CREATE_Ax(OP_EXTRAARG, a));
}

/* code access to a constant */
int luaK_codek (FuncState *fs, int reg, int k) {
  if (k <= MAXARG_Bx)
    return luaK_codeABx(fs, OP_LOADK, reg, k);
  else {
    int p = luaK_codeABx(fs, OP_LOADKX, reg, 0);
    codeextraarg(fs, k);
    return p;
  }
}


void luaK_checkstack (FuncState *fs, int n) {
  int newstack = fs->freereg + n;
  if (newstack > fs->f->maxstacksize) {
    if (newstack >= MAXREGS)
      luaX_syntaxerror(fs->ls, "function or expression too complex");
    fs->f->maxstacksize = cast_byte(newstack);
  }
}

/* Reserve registers. */
void luaK_reserveregs (FuncState *fs, int n) {
  luaK_checkstack(fs, n);
  fs->freereg += n;
}

/* Free register. */
static void freereg (FuncState *fs, int reg) {
  if (!ISK(reg) && reg >= fs->nactvar) {
    fs->freereg--;
    lua_assert(reg == fs->freereg);
  }
}

/* Free register for expression. */
static void freeexp (FuncState *fs, expdesc *e) {
  if (e->k == VNONRELOC)
    freereg(fs, e->u.info);
}


/* Add constant to constant array (ensures uniqueness of each constant)
** Use scanner's table to cache position of constants in constant list
** and try to reuse constants
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

/* add string constant */
int luaK_stringK (FuncState *fs, TString *s) {
  TValue o;
  setsvalue(fs->ls->L, &o, s);
  return addk(fs, &o, &o);
}


/* Add integer constant
** Integers use userdata as keys to avoid collision with floats with same
** value; conversion to 'void*' used only for hashing, no "precision"
** problems
*/
int luaK_intK (FuncState *fs, lua_Integer n) {
  TValue k, o;
  setpvalue(&k, cast(void*, cast(size_t, n)));
  setivalue(&o, n);
  return addk(fs, &k, &o);
}

/* add number constant */
static int luaK_numberK (FuncState *fs, lua_Number r) {
  TValue o;
  setfltvalue(&o, r);
  return addk(fs, &o, &o);
}

/* add boolean constant */
static int boolK (FuncState *fs, int b) {
  TValue o;
  setbvalue(&o, b);
  return addk(fs, &o, &o);
}

/* add nil constant */
static int nilK (FuncState *fs) {
  TValue k, v;
  setnilvalue(&v);
  /* cannot use nil as key; instead use table itself to represent nil */
  sethvalue(fs->ls->L, &k, fs->ls->h);
  return addk(fs, &k, &v);
}


void luaK_setreturns (FuncState *fs, expdesc *e, int nresults) {
  if (e->k == VCALL) {  /* expression is an open function call? */
    SETARG_C(getcode(fs, e), nresults+1);
    DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set C to %d\n", e->u.info, getcode(fs,e), nresults+1));
  }
  else if (e->k == VVARARG) {
    SETARG_B(getcode(fs, e), nresults+1);
    DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set B to %d\n", e->u.info, getcode(fs,e), nresults + 1));
    SETARG_A(getcode(fs, e), fs->freereg);
    DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set A to %d\n", e->u.info, getcode(fs,e), fs->freereg));
    luaK_reserveregs(fs, 1);
  }
}


void luaK_setoneret (FuncState *fs, expdesc *e) {
  if (e->k == VCALL) {  /* expression is an open function call? */
    e->k = VNONRELOC;
    e->u.info = GETARG_A(getcode(fs, e));
    DEBUG_EXPR(raviY_printf(fs, "luaK_setoneret (VCALL->VNONRELOC) %e\n", e));
  }
  else if (e->k == VVARARG) {
    SETARG_B(getcode(fs, e), 2);
    DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set B to 2\n", e->u.info, getcode(fs,e)));
    e->k = VRELOCABLE;  /* can relocate its simple result */
    DEBUG_EXPR(raviY_printf(fs, "luaK_setoneret (VVARARG->VNONRELOC) %e\n", e));
  }
}


void luaK_dischargevars (FuncState *fs, expdesc *e) {
  switch (e->k) {
    case VLOCAL: {
      e->k = VNONRELOC;
      DEBUG_EXPR(raviY_printf(fs, "luaK_dischargevars (VLOCAL->VNONRELOC) %e\n", e));
      break;
    }
    case VUPVAL: {
      e->u.info = luaK_codeABC(fs, OP_GETUPVAL, 0, e->u.info, 0);
      e->k = VRELOCABLE;
      DEBUG_EXPR(raviY_printf(fs, "luaK_dischargevars (VUPVAL->VRELOCABLE) %e\n", e));
      break;
    }
    case VINDEXED: {
      OpCode op = OP_GETTABUP;  /* assume 't' is in an upvalue */
      freereg(fs, e->u.ind.idx);
      if (e->u.ind.vt == VLOCAL) {  /* 't' is in a register? */
        freereg(fs, e->u.ind.t);
        /* table access - set specialized op codes if array types are detected */
        if (e->ravi_type == RAVI_TARRAYFLT && e->u.ind.key_type == RAVI_TNUMINT)
          op = OP_RAVI_GETTABLE_AF;
        else if (e->ravi_type == RAVI_TARRAYINT && e->u.ind.key_type == RAVI_TNUMINT)
          op = OP_RAVI_GETTABLE_AI;
        else
          op = OP_GETTABLE;
        if (e->ravi_type == RAVI_TARRAYFLT || e->ravi_type == RAVI_TARRAYINT)
          /* set the type of resulting expression */
          e->ravi_type = e->ravi_type == RAVI_TARRAYFLT ? RAVI_TNUMFLT : RAVI_TNUMINT;
      }
      e->u.info = luaK_codeABC(fs, op, 0, e->u.ind.t, e->u.ind.idx);
      e->k = VRELOCABLE;
      DEBUG_EXPR(raviY_printf(fs, "luaK_dischargevars (VINDEXED->VRELOCABLE) %e\n", e));
      break;
    }
    case VVARARG:
    case VCALL: {
      luaK_setoneret(fs, e);
      break;
    }
    default: break;  /* there is one value available (somewhere) */
  }
}


static int code_label (FuncState *fs, int A, int b, int jump) {
  luaK_getlabel(fs);  /* those instructions may be jump targets */
  return luaK_codeABC(fs, OP_LOADBOOL, A, b, jump);
}


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
      Instruction *pc = &getcode(fs, e);
      SETARG_A(*pc, reg);
      DEBUG_EXPR(raviY_printf(fs, "discharge2reg (VRELOCABLE set arg A) %e\n", e));
      DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set A to %d\n", e->u.info, *pc, reg));
      break;
    }
    case VNONRELOC: {
      if (reg != e->u.info) {
        /* code a MOVEI or MOVEF if the target register is a local typed variable */
        int ravi_type = raviY_get_register_typeinfo(fs, reg);
        switch (ravi_type) {
        case RAVI_TNUMINT:
          luaK_codeABC(fs, OP_RAVI_MOVEI, reg, e->u.info, 0);
          break;
        case RAVI_TNUMFLT:
          luaK_codeABC(fs, OP_RAVI_MOVEF, reg, e->u.info, 0);
          break;
        case RAVI_TARRAYINT:
          luaK_codeABC(fs, OP_RAVI_MOVEAI, reg, e->u.info, 0);
          break;
        case RAVI_TARRAYFLT:
          luaK_codeABC(fs, OP_RAVI_MOVEAF, reg, e->u.info, 0);
          break;
        default:
          luaK_codeABC(fs, OP_MOVE, reg, e->u.info, 0);
          break;
        }
      }
      break;
    }
    default: {
      lua_assert(e->k == VVOID || e->k == VJMP);
      return;  /* nothing to do... */
    }
  }
  e->u.info = reg;
  e->k = VNONRELOC;
}


static void discharge2anyreg (FuncState *fs, expdesc *e) {
  if (e->k != VNONRELOC) {
    luaK_reserveregs(fs, 1);
    discharge2reg(fs, e, fs->freereg-1);
  }
}

/* Discharge an expression to a specific register. */
static void exp2reg (FuncState *fs, expdesc *e, int reg) {
  discharge2reg(fs, e, reg);
  if (e->k == VJMP)
    luaK_concat(fs, &e->t, e->u.info);  /* put this jump in 't' list */
  if (hasjumps(e)) {
    int final;  /* position after whole expression */
    int p_f = NO_JUMP;  /* position of an eventual LOAD false */
    int p_t = NO_JUMP;  /* position of an eventual LOAD true */
    if (need_value(fs, e->t) || need_value(fs, e->f)) {
      int fj = (e->k == VJMP) ? NO_JUMP : luaK_jump(fs);
      p_f = code_label(fs, reg, 0, 1);
      p_t = code_label(fs, reg, 1, 0);
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

/* Discharge an expression to the next free register. */
void luaK_exp2nextreg (FuncState *fs, expdesc *e) {
  luaK_dischargevars(fs, e);
  freeexp(fs, e);
  luaK_reserveregs(fs, 1);
  exp2reg(fs, e, fs->freereg - 1);
}

/* Discharge an expression to any register. */
int luaK_exp2anyreg (FuncState *fs, expdesc *e) {
  luaK_dischargevars(fs, e);
  if (e->k == VNONRELOC) {
    if (!hasjumps(e)) return e->u.info;  /* exp is already in a register */
    if (e->u.info >= fs->nactvar) {  /* reg. is not a local? */
      exp2reg(fs, e, e->u.info);  /* put value on it */
      return e->u.info;
    }
  }
  luaK_exp2nextreg(fs, e);  /* default */
  return e->u.info;
}


void luaK_exp2anyregup (FuncState *fs, expdesc *e) {
  if (e->k != VUPVAL || hasjumps(e))
    luaK_exp2anyreg(fs, e);
}

/* Partially discharge expression to a value. */
void luaK_exp2val (FuncState *fs, expdesc *e) {
  if (hasjumps(e))
    luaK_exp2anyreg(fs, e);
  else
    luaK_dischargevars(fs, e);
}

/* convert from expression to register or constant value ? */
int luaK_exp2RK (FuncState *fs, expdesc *e) {
  luaK_exp2val(fs, e);
  switch (e->k) {
    case VTRUE:
    case VFALSE:
    case VNIL: {
      if (fs->nk <= MAXINDEXRK) {  /* constant fits in RK operand? */
        e->u.info = (e->k == VNIL) ? nilK(fs) : boolK(fs, (e->k == VTRUE));
        e->k = VK;
        return RKASK(e->u.info);
      }
      else break;
    }
    case VKINT: {
      e->u.info = luaK_intK(fs, e->u.ival);
      e->k = VK;
      goto vk;
    }
    case VKFLT: {
      e->u.info = luaK_numberK(fs, e->u.nval);
      e->k = VK;
      /* go through */
    }
    case VK: {
     vk:
      if (e->u.info <= MAXINDEXRK)  /* constant fits in 'argC'? */
        return RKASK(e->u.info);
      else break;
    }
    default: break;
  }
  /* not a constant in the right range: put it in a register */
  return luaK_exp2anyreg(fs, e);
}

static void check_valid_store(FuncState *fs, expdesc *var, expdesc *ex) {
#if RAVI_ENABLED
  /* VNONRELOC means we have fixed register and do we know the type? */
  if (ex->k == VNONRELOC && (var->ravi_type == RAVI_TNUMFLT || var->ravi_type == RAVI_TNUMINT || var->ravi_type == RAVI_TARRAYFLT || var->ravi_type == RAVI_TARRAYINT)) {
    /* handled by MOVEI, MOVEF, MOVEAI, MOVEAF at runtime */
    return;
  }
  if (var->ravi_type == RAVI_TNUMFLT) {
    if (ex->ravi_type == RAVI_TNUMFLT)
      return;
    if (ex->k == VINDEXED && ex->ravi_type == RAVI_TARRAYFLT)
      return;
    luaX_syntaxerror(
      fs->ls,
      luaO_pushfstring(
      fs->ls->L,
      "Invalid assignment of type: var type %d, expression type %d",
      var->ravi_type,
      ex->ravi_type));
  }
  else if (var->ravi_type == RAVI_TNUMINT) {
    if (ex->ravi_type == RAVI_TNUMINT)
      return;
    if (ex->k == VINDEXED && ex->ravi_type == RAVI_TARRAYINT)
      return;
    luaX_syntaxerror(
      fs->ls,
      luaO_pushfstring(
      fs->ls->L,
      "Invalid assignment of type: var type %d, expression type %d",
      var->ravi_type,
      ex->ravi_type));
  }
  else if (var->ravi_type == RAVI_TARRAYFLT || var->ravi_type == RAVI_TARRAYINT) {
    if (ex->ravi_type == var->ravi_type)
      return;
    luaX_syntaxerror(
      fs->ls,
      luaO_pushfstring(
      fs->ls->L,
      "Invalid assignment of type: var type %d, expression type %d",
      var->ravi_type,
      ex->ravi_type));
  }
#endif
}

/* Emit store for LHS expression. */
void luaK_storevar (FuncState *fs, expdesc *var, expdesc *ex) {
  switch (var->k) {
    case VLOCAL: {
      check_valid_store(fs, var, ex);
      freeexp(fs, ex);
      exp2reg(fs, ex, var->u.info);
      return;
    }
    case VUPVAL: {
      int e = luaK_exp2anyreg(fs, ex);
      luaK_codeABC(fs, OP_SETUPVAL, e, var->u.info, 0);
      break;
    }
    case VINDEXED: {
      OpCode op = (var->u.ind.vt == VLOCAL) ? OP_SETTABLE : OP_SETTABUP;
      if (op == OP_SETTABLE) {
        /* table value set - if array access then use specialized versions */
        if (var->ravi_type == RAVI_TARRAYFLT && var->u.ind.key_type == RAVI_TNUMINT)
          op = OP_RAVI_SETTABLE_AF;
        else if (var->ravi_type == RAVI_TARRAYINT && var->u.ind.key_type == RAVI_TNUMINT)
          op = OP_RAVI_SETTABLE_AI;
      }
      int e = luaK_exp2RK(fs, ex);
      luaK_codeABC(fs, op, var->u.ind.t, var->u.ind.idx, e);
      break;
    }
    default: {
      lua_assert(0);  /* invalid var kind to store */
      break;
    }
  }
  freeexp(fs, ex);
}


void luaK_self (FuncState *fs, expdesc *e, expdesc *key) {
  int ereg;
  luaK_exp2anyreg(fs, e);
  ereg = e->u.info;  /* register where 'e' was placed */
  freeexp(fs, e);
  e->u.info = fs->freereg;  /* base register for op_self */
  e->k = VNONRELOC;
  luaK_reserveregs(fs, 2);  /* function and 'self' produced by op_self */
  luaK_codeABC(fs, OP_SELF, e->u.info, ereg, luaK_exp2RK(fs, key));
  freeexp(fs, key);
}


static void invertjump (FuncState *fs, expdesc *e) {
  Instruction *pc = getjumpcontrol(fs, e->u.info);
  lua_assert(testTMode(GET_OPCODE(*pc)) && GET_OPCODE(*pc) != OP_TESTSET &&
                                           GET_OPCODE(*pc) != OP_TEST);
  SETARG_A(*pc, !(GETARG_A(*pc)));
  DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set A to %d\n", e->u.info, *pc, GETARG_A(*pc)));
}


static int jumponcond (FuncState *fs, expdesc *e, int cond) {
  if (e->k == VRELOCABLE) {
    Instruction ie = getcode(fs, e);
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


void luaK_goiftrue (FuncState *fs, expdesc *e) {
  int pc;  /* pc of last jump */
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VJMP: {
      invertjump(fs, e);
      pc = e->u.info;
      break;
    }
    case VK: case VKFLT: case VKINT: case VTRUE: {
      pc = NO_JUMP;  /* always true; do nothing */
      break;
    }
    default: {
      pc = jumponcond(fs, e, 0);
      break;
    }
  }
  luaK_concat(fs, &e->f, pc);  /* insert last jump in 'f' list */
  luaK_patchtohere(fs, e->t);
  e->t = NO_JUMP;
}


void luaK_goiffalse (FuncState *fs, expdesc *e) {
  int pc;  /* pc of last jump */
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VJMP: {
      pc = e->u.info;
      break;
    }
    case VNIL: case VFALSE: {
      pc = NO_JUMP;  /* always false; do nothing */
      break;
    }
    default: {
      pc = jumponcond(fs, e, 1);
      break;
    }
  }
  luaK_concat(fs, &e->t, pc);  /* insert last jump in 't' list */
  luaK_patchtohere(fs, e->f);
  e->f = NO_JUMP;
}


static void codenot (FuncState *fs, expdesc *e) {
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VNIL: case VFALSE: {
      e->k = VTRUE;
      e->ravi_type = RAVI_TANY; /* RAVI TODO */
      break;
    }
    case VK: case VKFLT: case VKINT: case VTRUE: {
      e->k = VFALSE;
      e->ravi_type = RAVI_TANY; /* RAVI TODO*/
      break;
    }
    case VJMP: {
      invertjump(fs, e);
      break;
    }
    case VRELOCABLE:
    case VNONRELOC: {
      discharge2anyreg(fs, e);
      freeexp(fs, e);
      e->u.info = luaK_codeABC(fs, OP_NOT, 0, e->u.info, 0);
      e->k = VRELOCABLE;
      e->ravi_type = RAVI_TANY; /* RAVI TODO */
      break;
    }
    default: {
      lua_assert(0);  /* cannot happen */
      break;
    }
  }
  /* interchange true and false lists */
  { int temp = e->f; e->f = e->t; e->t = temp; }
  removevalues(fs, e->f);
  removevalues(fs, e->t);
}


void luaK_indexed (FuncState *fs, expdesc *t, expdesc *k) {
  lua_assert(!hasjumps(t));
  t->u.ind.t = t->u.info;
  t->u.ind.key_type = k->ravi_type;   /* RAVI record the key type */
  t->u.ind.idx = luaK_exp2RK(fs, k);
  t->u.ind.vt = (t->k == VUPVAL) ? VUPVAL
                                 : check_exp(vkisinreg(t->k), VLOCAL);
  t->k = VINDEXED;
}


/*
** return false if folding can raise an error
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
    default: {
      return op >= LUA_OPADD ? 1 : 0;  /* everything else is valid - guard against RAVI operators */
    }
  }
}


/*
** Try to "constant-fold" an operation; return 1 if successful
*/
static int constfolding (FuncState *fs, int op, expdesc *e1, expdesc *e2) {
  TValue v1, v2, res;
  if (!tonumeral(e1, &v1) || !tonumeral(e2, &v2) || !validop(op, &v1, &v2))
    return 0;  /* non-numeric operands or not safe to fold */
  luaO_arith(fs->ls->L, op, &v1, &v2, &res);  /* does operation */
  if (ttisinteger(&res)) {
    e1->k = VKINT;
    e1->u.ival = ivalue(&res);
    e1->ravi_type = RAVI_TNUMINT;
  }
  else {  /* folds neither NaN nor 0.0 (to avoid collapsing with -0.0) */
    lua_Number n = fltvalue(&res);
    if (luai_numisnan(n) || n == 0)
      return 0;
    e1->k = VKFLT;
    e1->u.nval = n;
    e1->ravi_type = RAVI_TNUMFLT;
  }
  return 1;
}

/* translate to standard Lua arithmetic codes */
static int getarithop(OpCode op) {
  if (op < OP_EXTRAARG)
    return op - OP_ADD + LUA_OPADD;

  /* following are RAVI specific opcodes */
  if (op >= OP_RAVI_ADDFF && op < OP_RAVI_ADDIN)
    return LUA_OPADD;
  else if (op >= OP_RAVI_SUBFF && op <= OP_RAVI_SUBNI)
    return LUA_OPSUB;
  else if (op >= OP_RAVI_MULFF && op <= OP_RAVI_MULIN)
    return LUA_OPMUL;
  else if (op >= OP_RAVI_DIVFF && op <= OP_RAVI_DIVII)
    return LUA_OPDIV;

  /* should we ever get here?*/
  return LUA_OPADD - 1;
}

/* generate ADD and MUL codes - for ADD supply offset as 0, and for
 * MUL supply the difference between the two
 */
static void generate_binarithop(FuncState *fs, expdesc *e1, expdesc *e2, int o1,
                                int o2, int offset) {
  if (e1->ravi_type == RAVI_TNUMFLT && e2->ravi_type == RAVI_TNUMFLT) {
    e1->u.info = luaK_codeABC(fs, OP_RAVI_ADDFF + offset, 0, o1, o2);
  } else if (e1->ravi_type == RAVI_TNUMFLT && e2->ravi_type == RAVI_TNUMINT) {
    if (ISK(o2)) {
      e1->u.info = luaK_codeABKi(fs, OP_RAVI_ADDFI + offset, 0, o1,
                                 o2); /* generate opcode */
    } else {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_ADDFI + offset, 0, o1,
                                o2); /* generate opcode */
    }
  } else if (e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMFLT) {
    if (ISK(o1)) {
      e1->u.info = luaK_codeABKi(fs, OP_RAVI_ADDFI + offset, 0, o2,
                                 o1); /* generate opcode */
    } else {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_ADDFI + offset, 0, o2,
                                o1); /* generate opcode */
    }
  } else if (e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMINT) {
    if (ISK(o1)) {
      e1->u.info = luaK_codeABKi(fs, OP_RAVI_ADDII + offset, 0, o2,
                                 o1); /* generate opcode */
    } else if (ISK(o2)) {
      e1->u.info = luaK_codeABKi(fs, OP_RAVI_ADDII + offset, 0, o1,
                                 o2); /* generate opcode */
    } else {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_ADDII + offset, 0, o1,
                                o2); /* generate opcode */
    }
  }
}

/*
** Code for binary and unary expressions that "produce values"
** (arithmetic operations, bitwise operations, concat, length). First
** try to do constant folding (only for numeric [arithmetic and
** bitwise] operations, which is what 'lua_arith' accepts).
** Expression to produce final result will be encoded in 'e1'.
*/
static void codeexpval (FuncState *fs, OpCode op,
                        expdesc *e1, expdesc *e2, int line) {
  lua_assert(op >= OP_ADD);
  if (op <= OP_BNOT && constfolding(fs, getarithop(op), e1, e2))
    return;  /* result has been folded */
  else {
    int o1, o2;
    int isbinary = 1;
    /* move operands to registers (if needed) */
    if (op == OP_UNM || op == OP_BNOT || op == OP_LEN) {  /* unary op? */
      o2 = 0;  /* no second expression */
      o1 = luaK_exp2anyreg(fs, e1);  /* cannot operate on constants */
      isbinary = 0;
    }
    else {  /* regular case (binary operators) */
      o2 = luaK_exp2RK(fs, e2);  /* both operands are "RK" */
      o1 = luaK_exp2RK(fs, e1);
    }
    if (o1 > o2) {  /* free registers in proper order */
      freeexp(fs, e1);
      freeexp(fs, e2);
    }
    else {
      freeexp(fs, e2);
      freeexp(fs, e1);
    }
#if RAVI_ENABLED
    if (op == OP_ADD && 
      (e1->ravi_type == RAVI_TNUMFLT || e1->ravi_type == RAVI_TNUMINT) &&
      (e2->ravi_type == RAVI_TNUMFLT || e2->ravi_type == RAVI_TNUMINT))
      generate_binarithop(fs, e1, e2, o1, o2, 0);
    else if (op == OP_MUL &&
      (e1->ravi_type == RAVI_TNUMFLT || e1->ravi_type == RAVI_TNUMINT) &&
      (e2->ravi_type == RAVI_TNUMFLT || e2->ravi_type == RAVI_TNUMINT))
      generate_binarithop(fs, e1, e2, o1, o2, OP_RAVI_MULFF - OP_RAVI_ADDFF);

    /* todo optimize the SUB opcodes when constant is small */
    else if (op == OP_SUB && e1->ravi_type == RAVI_TNUMFLT && e2->ravi_type == RAVI_TNUMFLT) {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_SUBFF, 0, o1, o2); 
    }
    else if (op == OP_SUB && e1->ravi_type == RAVI_TNUMFLT && e2->ravi_type == RAVI_TNUMINT) {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_SUBFI, 0, o1, o2); 
    }
    else if (op == OP_SUB && e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMFLT) {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_SUBIF, 0, o1, o2);  
    }
    else if (op == OP_SUB && e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMINT) {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_SUBII, 0, o1, o2); 
    }

    else if (op == OP_DIV && e1->ravi_type == RAVI_TNUMFLT && e2->ravi_type == RAVI_TNUMFLT) {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_DIVFF, 0, o1, o2);
    }
    else if (op == OP_DIV && e1->ravi_type == RAVI_TNUMFLT && e2->ravi_type == RAVI_TNUMINT) {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_DIVFI, 0, o1, o2);
    }
    else if (op == OP_DIV && e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMFLT) {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_DIVIF, 0, o1, o2);
    }
    else if (op == OP_DIV && e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMINT) {
      e1->u.info = luaK_codeABC(fs, OP_RAVI_DIVII, 0, o1, o2);
    }

    else {
#endif
      e1->u.info = luaK_codeABC(fs, op, 0, o1, o2);  /* generate opcode */
#if RAVI_ENABLED
    }
#endif
    e1->k = VRELOCABLE;  /* all those operations are relocable */
    if (isbinary) {
      if ((op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV) &&
        e1->ravi_type == RAVI_TNUMFLT && e2->ravi_type == RAVI_TNUMFLT)
        e1->ravi_type = RAVI_TNUMFLT;
      else if ((op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV) 
        && e1->ravi_type == RAVI_TNUMFLT && e2->ravi_type == RAVI_TNUMINT)
        e1->ravi_type = RAVI_TNUMFLT;
      else if ((op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV) 
        && e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMFLT)
        e1->ravi_type = RAVI_TNUMFLT;
      else if ((op == OP_ADD || op == OP_SUB || op == OP_MUL) 
        && e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMINT)
        e1->ravi_type = RAVI_TNUMINT;
      else if ((op == OP_DIV) 
        && e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMINT)
        e1->ravi_type = RAVI_TNUMFLT;
      else
        e1->ravi_type = RAVI_TANY;
    }
    luaK_fixline(fs, line);
  }
}

/* Emit comparison operator. */
static void codecomp (FuncState *fs, OpCode op, int cond, expdesc *e1,
                                                          expdesc *e2) {
  int o1 = luaK_exp2RK(fs, e1);
  int o2 = luaK_exp2RK(fs, e2);
  freeexp(fs, e2);
  freeexp(fs, e1);
  if (cond == 0 && op != OP_EQ) {
    int temp;  /* exchange args to replace by '<' or '<=' */
    temp = o1; o1 = o2; o2 = temp;  /* o1 <==> o2 */
    cond = 1;
  }
  e1->u.info = condjump(fs, op, cond, o1, o2);
  e1->k = VJMP;
  e1->ravi_type = RAVI_TANY;
}


void luaK_prefix (FuncState *fs, UnOpr op, expdesc *e, int line) {
  expdesc e2;
  e2.ravi_type = RAVI_TANY;
  e2.t = e2.f = NO_JUMP; e2.k = VKINT; e2.u.ival = 0;
  switch (op) {
    case OPR_MINUS: case OPR_BNOT: case OPR_LEN: {
      codeexpval(fs, cast(OpCode, (op - OPR_MINUS) + OP_UNM), e, &e2, line);
      break;
    }
    case OPR_NOT: codenot(fs, e); break;
    default: lua_assert(0);
  }
}


void luaK_infix (FuncState *fs, BinOpr op, expdesc *v) {
  switch (op) {
    case OPR_AND: {
      luaK_goiftrue(fs, v);
      break;
    }
    case OPR_OR: {
      luaK_goiffalse(fs, v);
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
      if (!tonumeral(v, NULL)) luaK_exp2RK(fs, v);
      break;
    }
    default: {
      luaK_exp2RK(fs, v);
      break;
    }
  }
}


void luaK_posfix (FuncState *fs, BinOpr op,
                  expdesc *e1, expdesc *e2, int line) {
  switch (op) {
    case OPR_AND: {
      lua_assert(e1->t == NO_JUMP);  /* list must be closed */
      luaK_dischargevars(fs, e2);
      luaK_concat(fs, &e2->f, e1->f);
      e2->ravi_type = e1->ravi_type;  /* RAVI TODO why ? this seems to be needed but don't understand reason */
      *e1 = *e2;
      break;
    }
    case OPR_OR: {
      lua_assert(e1->f == NO_JUMP);  /* list must be closed */
      luaK_dischargevars(fs, e2);
      luaK_concat(fs, &e2->t, e1->t);
      e2->ravi_type = e1->ravi_type; /* RAVI TODO why ? this seems to be needed but don't understand reason */
      *e1 = *e2;
      break;
    }
    case OPR_CONCAT: {
      luaK_exp2val(fs, e2);
      if (e2->k == VRELOCABLE && GET_OPCODE(getcode(fs, e2)) == OP_CONCAT) {
        lua_assert(e1->u.info == GETARG_B(getcode(fs, e2))-1);
        freeexp(fs, e1);
        SETARG_B(getcode(fs, e2), e1->u.info);
        DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set A to %d\n", e2->u.info, getcode(fs,e2), e1->u.info));
        e1->k = VRELOCABLE; e1->u.info = e2->u.info;
        e1->ravi_type = RAVI_TANY; /* RAVI TODO check */
      }
      else {
        luaK_exp2nextreg(fs, e2);  /* operand must be on the 'stack' */
        codeexpval(fs, OP_CONCAT, e1, e2, line);
      }
      break;
    }
    case OPR_ADD: case OPR_SUB: case OPR_MUL: case OPR_DIV:
    case OPR_IDIV: case OPR_MOD: case OPR_POW:
    case OPR_BAND: case OPR_BOR: case OPR_BXOR:
    case OPR_SHL: case OPR_SHR: {
      codeexpval(fs, cast(OpCode, (op - OPR_ADD) + OP_ADD), e1, e2, line);
      break;
    }
    case OPR_EQ: case OPR_LT: case OPR_LE: {
      codecomp(fs, cast(OpCode, op - OPR_EQ + OP_EQ), 1, e1, e2);
      break;
    }
    case OPR_NE: case OPR_GT: case OPR_GE: {
      codecomp(fs, cast(OpCode, op - OPR_NE + OP_EQ), 0, e1, e2);
      break;
    }
    default: lua_assert(0);
  }
}


void luaK_fixline (FuncState *fs, int line) {
  fs->f->lineinfo[fs->pc - 1] = line;
}


void luaK_setlist (FuncState *fs, int base, int nelems, int tostore) {
  int c =  (nelems - 1)/LFIELDS_PER_FLUSH + 1;
  int b = (tostore == LUA_MULTRET) ? 0 : tostore;
  lua_assert(tostore != 0);
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


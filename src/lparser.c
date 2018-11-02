/*
** $Id: lparser.c,v 2.155.1.2 2017/04/29 18:11:40 roberto Exp $
** Lua Parser
** See Copyright Notice in lua.h
*/

/*
** Portions Copyright (C) 2015-2017 Dibyendu Majumdar 
*/

#define lparser_c
#define LUA_CORE

#include "lprefix.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "lua.h"

#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "llex.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"

/* RAVI - only global (sorry!) but in this case the global is fine */
int ravi_parser_debug = 0;

/* maximum number of local variables per function (must be smaller
   than 250, due to the bytecode format) */
#define MAXVARS		125 
/* RAVI change; #define MAXVARS		200 */


#define hasmultret(k)		((k) == VCALL || (k) == VVARARG)


/* because all strings are unified by the scanner, the parser
   can use pointer equality for string equality */
#define eqstr(a,b)	((a) == (b))


/*
** nodes for block list (list of active blocks)
*/
typedef struct BlockCnt {
  struct BlockCnt *previous;  /* chain */
  int firstlabel;  /* index of first label in this block */
  int firstgoto;  /* index of first pending goto in this block */
  lu_byte nactvar;  /* # active locals outside the block */
  lu_byte upval;  /* true if some variable in the block is an upvalue */
  lu_byte isloop;  /* true if 'block' is a loop */
} BlockCnt;

/* RAVI set debug level */
void ravi_set_debuglevel(int level) { ravi_parser_debug = level; }

/* RAVI - return the type name */
const char *raviY_typename(ravitype_t tt) {
  switch (tt) {
  case RAVI_TNIL:
    return "nil";
  case RAVI_TBOOLEAN:
    return "boolean";
  case RAVI_TNUMFLT:
    return "number";
  case RAVI_TNUMINT:
    return "integer";
  case RAVI_TSTRING:
    return "string";
  case RAVI_TFUNCTION:
    return "function";
  case RAVI_TARRAYINT:
    return "integer[]";
  case RAVI_TARRAYFLT:
    return "number[]";
  case RAVI_TUSERDATA:
    return "userdata";
  case RAVI_TTABLE:
    return "table";
  default:
    return "?";
  }
}

/* RAVI - prints a Lua expression node */
static void print_expdesc(FILE *fp, FuncState *fs, const expdesc *e) {
  char buf[80] = {0};
  switch (e->k) {
  case VVOID:
    fprintf(fp, "{p=%p, k=VVOID, type=%s}", e, raviY_typename(e->ravi_type));
    break;
  case VNIL:
    fprintf(fp, "{p=%p, k=VNIL, type=%s}", e, raviY_typename(e->ravi_type));
    break;
  case VTRUE:
    fprintf(fp, "{p=%p, k=VTRUE, type=%s}", e, raviY_typename(e->ravi_type));
    break;
  case VFALSE:
    fprintf(fp, "{p=%p, k=VFALSE, type=%s}", e, raviY_typename(e->ravi_type));
    break;
  case VK:
    fprintf(fp, "{p=%p, k=VK, Kst=%d, type=%s}", e, e->u.info,
            raviY_typename(e->ravi_type));
    break;
  case VKFLT:
    fprintf(fp, "{p=%p, k=VKFLT, n=%f, type=%s}", e, e->u.nval,
            raviY_typename(e->ravi_type));
    break;
  case VKINT:
    fprintf(fp, "{p=%p, k=VKINT, n=%lld, type=%s}", e, (long long)e->u.ival,
            raviY_typename(e->ravi_type));
    break;
  case VNONRELOC:
    fprintf(fp, "{p=%p, k=VNONRELOC, register=%d %s, type=%s, pc=%d}", e, e->u.info,
            raviY_typename(raviY_get_register_typeinfo(fs, e->u.info, NULL)),
            raviY_typename(e->ravi_type),
            e->pc);
    break;
  case VLOCAL:
    fprintf(fp, "{p=%p, k=VLOCAL, register=%d, type=%s}", e, e->u.info,
            raviY_typename(e->ravi_type));
    break;
  case VUPVAL:
    fprintf(fp, "{p=%p, k=VUPVAL, idx=%d, type=%s}", e, e->u.info,
            raviY_typename(e->ravi_type));
    break;
  case VINDEXED:
    fprintf(fp,
            "{p=%p, k=VINDEXED, tablereg=%d, indexreg=%d, vtype=%s, type=%s}",
            e, e->u.ind.t, e->u.ind.idx,
            (e->u.ind.vt == VLOCAL) ? "VLOCAL" : "VUPVAL",
            raviY_typename(e->ravi_type));
    break;
  case VJMP:
    fprintf(fp, "{p=%p, k=VJMP, pc=%d, instruction=(%s), type=%s}", e,
            e->u.info,
            raviP_instruction_to_str(buf, sizeof buf, getinstruction(fs, e)),
            raviY_typename(e->ravi_type));
    break;
  case VRELOCABLE:
    fprintf(fp, "{p=%p, k=VRELOCABLE, pc=%d, instruction=(%s), type=%s, pc=%d}", e,
            e->u.info,
            raviP_instruction_to_str(buf, sizeof buf, getinstruction(fs, e)),
            raviY_typename(e->ravi_type),
            e->pc);
    break;
  case VCALL:
    fprintf(
        fp, "{p=%p, k=VCALL, pc=%d, instruction=(%s %s), type=%s}", e,
        e->u.info, raviP_instruction_to_str(buf, sizeof buf, getinstruction(fs, e)),
        raviY_typename(raviY_get_register_typeinfo(fs, GETARG_A(getinstruction(fs, e)), NULL)),
        raviY_typename(e->ravi_type));
    break;
  case VVARARG:
    fprintf(fp, "{p=%p, k=VVARARG, pc=%d, instruction=(%s), type=%s}", e,
            e->u.info,
            raviP_instruction_to_str(buf, sizeof buf, getinstruction(fs, e)),
            raviY_typename(e->ravi_type));
    break;
  }
}

/* RAVI - printf type utility for debugging */
void raviY_printf(FuncState *fs, const char *format, ...) {
  char buf[80] = {0};
  va_list ap;
  const char *cp;
  printf("%s(%d) ", getstr(fs->ls->source), fs->ls->lastline);
  va_start(ap, format);
  for (cp = format; *cp; cp++) {
    if (cp[0] == '%' && cp[1] == 'e') {
      expdesc *e;
      e = va_arg(ap, expdesc *);
      print_expdesc(stdout, fs, e);
      cp++;
    } else if (cp[0] == '%' && cp[1] == 'v') {
      LocVar *v;
      v = va_arg(ap, LocVar *);
      const char *s = getstr(v->varname);
      printf("var={%s startpc=%d endpc=%d, type=%s}", s, v->startpc, v->endpc,
             raviY_typename(v->ravi_type));
      cp++;
    } else if (cp[0] == '%' && cp[1] == 'o') {
      Instruction i;
      i = va_arg(ap, Instruction);
      raviP_instruction_to_str(buf, sizeof buf, i);
      fputs(buf, stdout);
      cp++;
    } else if (cp[0] == '%' && cp[1] == 'd') {
      int i;
      i = va_arg(ap, int);
      printf("%d", i);
      cp++;
    } else if (cp[0] == '%' && cp[1] == 's') {
      const char *s;
      s = va_arg(ap, const char *);
      fputs(s, stdout);
      cp++;
    } else if (cp[0] == '%' && cp[1] == 'f') {
      double d;
      d = va_arg(ap, double);
      printf("%f", d);
      cp++;
    } else if (cp[0] == '%' && cp[1] == 't') {
      ravitype_t i;
      i = va_arg(ap, ravitype_t);
      fputs(raviY_typename(i), stdout);
      cp++;
    } else {
      fputc(*cp, stdout);
    }
  }
  va_end(ap);
  fflush(stdout);
}

/*
** prototypes for recursive non-terminal functions
*/
static void statement (LexState *ls);
static void expr (LexState *ls, expdesc *v);
static LocVar *getlocvar(FuncState *fs, int i);

/* semantic error */
static l_noret semerror (LexState *ls, const char *msg) {
  ls->t.token = 0;  /* remove "near <token>" from final message */
  luaX_syntaxerror(ls, msg);
}


static l_noret error_expected (LexState *ls, int token) {
  luaX_syntaxerror(ls,
      luaO_pushfstring(ls->L, "%s expected", luaX_token2str(ls, token)));
}


static l_noret errorlimit (FuncState *fs, int limit, const char *what) {
  lua_State *L = fs->ls->L;
  const char *msg;
  int line = fs->f->linedefined;
  const char *where = (line == 0)
                      ? "main function"
                      : luaO_pushfstring(L, "function at line %d", line);
  msg = luaO_pushfstring(L, "too many %s (limit is %d) in %s",
                             what, limit, where);
  luaX_syntaxerror(fs->ls, msg);
}


static void checklimit (FuncState *fs, int v, int l, const char *what) {
  if (v > l) errorlimit(fs, l, what);
}


static int testnext (LexState *ls, int c) {
  if (ls->t.token == c) {
    luaX_next(ls);
    return 1;
  }
  else return 0;
}


static void check (LexState *ls, int c) {
  if (ls->t.token != c)
    error_expected(ls, c);
}


static void checknext (LexState *ls, int c) {
  check(ls, c);
  luaX_next(ls);
}


#define check_condition(ls,c,msg)	{ if (!(c)) luaX_syntaxerror(ls, msg); }



static void check_match (LexState *ls, int what, int who, int where) {
  if (!testnext(ls, what)) {
    if (where == ls->linenumber)
      error_expected(ls, what);
    else {
      luaX_syntaxerror(ls, luaO_pushfstring(ls->L,
             "%s expected (to close %s at line %d)",
              luaX_token2str(ls, what), luaX_token2str(ls, who), where));
    }
  }
}

/* Check that current token is a name, and advance */
static TString *str_checkname (LexState *ls) {
  TString *ts;
  check(ls, TK_NAME);
  ts = ls->t.seminfo.ts;
  luaX_next(ls);
  return ts;
}

/* Initialize expression, type of expression stored in e->ravi_type,
 * expression kind in e->k, e->u.info may have a register 
 * or bytecode 
 */
static void init_exp (expdesc *e, expkind k, int info, ravitype_t tt, TString *usertype) {
  e->f = e->t = NO_JUMP;
  e->k = k;
  e->u.info = info;
  /* RAVI change; added type */
  e->ravi_type = tt;
  e->usertype = usertype;
  e->pc = -1;
  e->u.ind.usertype = NULL; /* Just for safey */
}

/* create a string constant expression, constant's location stored in
 * e->u.info, e->ravi_type = RAVI_TSTRING, e->k = VK
 */
static void codestring (LexState *ls, expdesc *e, TString *s) {
  init_exp(e, VK, luaK_stringK(ls->fs, s), RAVI_TSTRING, NULL);
}

/* verify that current token is a string, create a string constant
 * expression, e->u.info holds location of constant, 
 * e->ravi_type = RAVI_TSTRING
 */
static void checkname (LexState *ls, expdesc *e) {
  codestring(ls, e, str_checkname(ls));
}

/* create a local variable in function scope, return the
 * variable's index in ls->f->locvars.
 * RAVI change - added the type of the variable.
 */
static int registerlocalvar (LexState *ls, TString *varname, unsigned int ravi_type, TString *usertype) {
  FuncState *fs = ls->fs;
  Proto *f = fs->f;
  int oldsize = f->sizelocvars;
  luaM_growvector(ls->L, f->locvars, fs->nlocvars, f->sizelocvars,
                  LocVar, SHRT_MAX, "local variables");
  while (oldsize < f->sizelocvars) {
    /* RAVI change initialize */
    f->locvars[oldsize].startpc = -1;
    f->locvars[oldsize].endpc = -1;
    f->locvars[oldsize].ravi_type = RAVI_TANY;
    f->locvars[oldsize].usertype = NULL;
    f->locvars[oldsize++].varname = NULL;
  }
  f->locvars[fs->nlocvars].varname = varname;
  f->locvars[fs->nlocvars].ravi_type = ravi_type;
  if (ravi_type == RAVI_TUSERDATA && usertype != NULL) {
    // Store a reference to the usertype name
    f->locvars[fs->nlocvars].usertype = usertype;
  }
  luaC_objbarrier(ls->L, f, varname);
  return fs->nlocvars++;
}

/* create a new local variable in function scope, and set the
 * variable type (RAVI - added type tt) */
static void new_localvar (LexState *ls, TString *name, ravitype_t tt, TString *usertype) {
  FuncState *fs = ls->fs;
  Dyndata *dyd = ls->dyd;
  /* register variable and get its index */
  /* RAVI change - record type info for local variable */
  int reg = registerlocalvar(ls, name, tt, usertype);
  checklimit(fs, dyd->actvar.n + 1 - fs->firstlocal,
                  MAXVARS, "local variables");
  luaM_growvector(ls->L, dyd->actvar.arr, dyd->actvar.n + 1,
                  dyd->actvar.size, Vardesc, MAX_INT, "local variables");
  /* variable will be placed at stack position dyd->actvar.n */
  dyd->actvar.arr[dyd->actvar.n].idx = cast(short, reg);
  DEBUG_VARS(raviY_printf(fs, "new_localvar -> registering %v fs->f->locvars[%d] at ls->dyd->actvar.arr[%d]\n", &fs->f->locvars[reg], reg, dyd->actvar.n));
  dyd->actvar.n++;
  DEBUG_VARS(raviY_printf(fs, "new_localvar -> ls->dyd->actvar.n set to %d\n", dyd->actvar.n));
}

/* create a new local variable 
 */
static void new_localvarliteral_ (LexState *ls, const char *name, size_t sz) {
  /* RAVI change - add type */
  new_localvar(ls, luaX_newstring(ls, name, sz), RAVI_TANY, NULL);
}

/* create a new local variable 
*/
#define new_localvarliteral(ls,name) \
	new_localvarliteral_(ls, "" name, (sizeof(name)/sizeof(char))-1)

/* obtain the details of a local variable given the local register
 * where the variable is stored
 */
static LocVar *getlocvar (FuncState *fs, int i) {
  /* convert from local stack position i to global 
   * and then retrieve the index
   * of the variable in f->locvars
   */
  int idx = fs->ls->dyd->actvar.arr[fs->firstlocal + i].idx;
  lua_assert(idx < fs->nlocvars);
  return &fs->f->locvars[idx];
}

/* RAVI translate from local register to local variable index
 */
static int register_to_locvar_index(FuncState *fs, int reg) {
  int idx;
  lua_assert(reg >= 0 && (fs->firstlocal + reg) < fs->ls->dyd->actvar.n);
  /* Get the LocVar associated with the register */
  idx = fs->ls->dyd->actvar.arr[fs->firstlocal + reg].idx;
  lua_assert(idx < fs->nlocvars);
  return idx;
}

/* RAVI get type of a register - if the register is not allocated
 * to an active local variable, then return RAVI_TANY else
 * return the type associated with the variable.
 * This is a RAVI function
 */
ravitype_t raviY_get_register_typeinfo(FuncState *fs, int reg, TString **pusertype) {
  int idx;
  LocVar *v;
  /* Due to the way Lua parser works it is not safe to look beyond nactvar */
  if (reg < 0 || reg >= fs->nactvar ||
      (fs->firstlocal + reg) >= fs->ls->dyd->actvar.n) {
    return RAVI_TANY;
  }
  /* Get the LocVar associated with the register */
  idx = fs->ls->dyd->actvar.arr[fs->firstlocal + reg].idx;
  lua_assert(idx < fs->nlocvars);
  v = &fs->f->locvars[idx];
  if (pusertype != NULL)
    *pusertype = v->usertype;
  /* Variable in scope so return the type if we know it */
  return v->ravi_type;
}

/* moves the active variable watermark (nactvar) to cover the 
 * local variables in the current declaration. Also
 * sets the starting code location (set to current instruction) 
 * for nvars new local variables 
 */
static void adjustlocalvars (LexState *ls, int nvars) {
  FuncState *fs = ls->fs;
  fs->nactvar = cast_byte(fs->nactvar + nvars);
  for (; nvars; nvars--) {
    getlocvar(fs, fs->nactvar - nvars)->startpc = fs->pc;
  }
  DEBUG_VARS(raviY_printf(fs, "adjustlocalvars -> set fs->nactvar to %d\n", fs->nactvar));
}

/* removes local variables from the stack and 
 * also sets the ending location - i.e. the instruction where the
 * variable scope ends - for each variable
 */
static void removevars (FuncState *fs, int tolevel) {
  fs->ls->dyd->actvar.n -= (fs->nactvar - tolevel);
  while (fs->nactvar > tolevel)
    getlocvar(fs, --fs->nactvar)->endpc = fs->pc;
  DEBUG_VARS(raviY_printf(fs, "removevars -> reset fs->nactvar to %d\n", fs->nactvar));
}

/* search for an upvalue by name, return location if
 * found else -1
 */
static int searchupvalue (FuncState *fs, TString *name) {
  int i;
  Upvaldesc *up = fs->f->upvalues;
  for (i = 0; i < fs->nups; i++) {
    if (eqstr(up[i].name, name)) return i;
  }
  return -1;  /* not found */
}

/* create a new upvalue */
static int newupvalue (FuncState *fs, TString *name, expdesc *v) {
  Proto *f = fs->f;
  int oldsize = f->sizeupvalues;
  checklimit(fs, fs->nups + 1, MAXUPVAL, "upvalues");
  luaM_growvector(fs->ls->L, f->upvalues, fs->nups, f->sizeupvalues,
                  Upvaldesc, MAXUPVAL, "upvalues");
  while (oldsize < f->sizeupvalues) {
    f->upvalues[oldsize].name = NULL;
    f->upvalues[oldsize++].usertype = NULL;
  }
  f->upvalues[fs->nups].instack = (v->k == VLOCAL);
  f->upvalues[fs->nups].idx = cast_byte(v->u.info);
  f->upvalues[fs->nups].name = name;
  f->upvalues[fs->nups].ravi_type = v->ravi_type;
  f->upvalues[fs->nups].usertype = v->usertype;
  luaC_objbarrier(fs->ls->L, f, name);
  return fs->nups++;
}

/* search for a loal variable - return -1 if not 
 * found, return var location if found
 */
static int searchvar (FuncState *fs, TString *n) {
  int i;
  for (i = cast_int(fs->nactvar) - 1; i >= 0; i--) {
    if (eqstr(n, getlocvar(fs, i)->varname))
      return i;
  }
  return -1;  /* not found */
}


/*
  Mark block where variable at given level was defined
  (to emit close instructions later).
*/
static void markupval (FuncState *fs, int level) {
  BlockCnt *bl = fs->bl;
  while (bl->nactvar > level)
    bl = bl->previous;
  bl->upval = 1;
}


/*
  Find variable with given name 'n'. If it is an upvalue, add this
  upvalue into all intermediate functions.
*/
static void singlevaraux (FuncState *fs, TString *n, expdesc *var, int base) {
  if (fs == NULL)  /* no more levels? */
    init_exp(var, VVOID, 0, RAVI_TANY, NULL);  /* default is global */
  else {
    int v = searchvar(fs, n);  /* look up locals at current level */
    if (v >= 0) {  /* found? */
      /* RAVI set type of local var / expr if possible */
      TString *usertype = NULL;
      ravitype_t tt = raviY_get_register_typeinfo(fs, v, &usertype);
      init_exp(var, VLOCAL, v, tt, usertype);  /* variable is local, RAVI set type */
      if (!base)
        markupval(fs, v);  /* local will be used as an upval */
    }
    else {  /* not found as local at current level; try upvalues */
      int idx = searchupvalue(fs, n);  /* try existing upvalues */
      if (idx < 0) {  /* not found? */
        singlevaraux(fs->prev, n, var, 0);  /* try upper levels */
        if (var->k == VVOID)  /* not found? */
          return;  /* it is a global */
        /* else was LOCAL or UPVAL */
        idx  = newupvalue(fs, n, var);  /* will be a new upvalue */
      }
      init_exp(var, VUPVAL, idx, fs->f->upvalues[idx].ravi_type, fs->f->upvalues[idx].usertype); /* RAVI : set upvalue type */
    }
  }
}

/* intialize var with the variable name - may be local,
 * global or upvalue - note that var->k will be set to
 * VLOCAL (local var), or VINDEXED or VUPVAL? TODO check
 */
static void singlevar (LexState *ls, expdesc *var) {
  TString *varname = str_checkname(ls);
  FuncState *fs = ls->fs;
  singlevaraux(fs, varname, var, 1);
  if (var->k == VVOID) {  /* global name? */
    expdesc key = {.ravi_type = RAVI_TANY, .pc = -1};
    singlevaraux(fs, ls->envn, var, 1);  /* get environment variable */
    lua_assert(var->k != VVOID);  /* this one must exist */
    codestring(ls, &key, varname);  /* key is variable name */
    luaK_indexed(fs, var, &key);  /* env[varname] */
  }
}

/* RAVI code an instruction to coerce the type, reg is the register, 
   and ravi_type is the type we want */
static void ravi_code_typecoersion(LexState *ls, int reg, ravitype_t ravi_type, TString *typename /* only if tt is USERDATA */) {
  /* do we need to convert ? */
  if (ravi_type == RAVI_TNUMFLT || ravi_type == RAVI_TNUMINT)
    /* code an instruction to convert in place */
    luaK_codeABC(ls->fs,
                 ravi_type == RAVI_TNUMFLT ? OP_RAVI_TOFLT : OP_RAVI_TOINT, reg,
                 0, 0);
  else if (ravi_type == RAVI_TARRAYINT || ravi_type == RAVI_TARRAYFLT)
    luaK_codeABC(ls->fs, ravi_type == RAVI_TARRAYINT ? OP_RAVI_TOIARRAY
                                                     : OP_RAVI_TOFARRAY,
                 reg, 0, 0);
  else if (ravi_type == RAVI_TTABLE)
    luaK_codeABC(ls->fs, OP_RAVI_TOTAB,
                 reg, 0, 0);
  else if (ravi_type == RAVI_TUSERDATA)
    luaK_codeABx(ls->fs, OP_RAVI_TOTYPE,
	    reg, luaK_stringK(ls->fs, typename));
  else if (ravi_type == RAVI_TSTRING)
    luaK_codeABC(ls->fs, OP_RAVI_TOSTRING,
	    reg, 0, 0);
  else if (ravi_type == RAVI_TFUNCTION)
    luaK_codeABC(ls->fs, OP_RAVI_TOCLOSURE,
	    reg, 0, 0);
}

/* RAVI code an instruction to initialize a scalar typed value
   For array and table types however raise an error as uninitialized value
   would cause a null pointer and therefore memory fault
 */
static void ravi_code_setzero(FuncState *fs, int reg, ravitype_t ravi_type, TString *usertype) {
  (void) usertype;
  if (ravi_type == RAVI_TNUMFLT || ravi_type == RAVI_TNUMINT)
    /* code an instruction to convert in place */
    luaK_codeABC(fs, ravi_type == RAVI_TNUMFLT ? OP_RAVI_LOADFZ : OP_RAVI_LOADIZ, reg, 0, 0);
  else if (ravi_type == RAVI_TARRAYFLT)
    luaX_syntaxerror(fs->ls, "uninitialized number[] in local variable");
  else if (ravi_type == RAVI_TARRAYINT)
    luaX_syntaxerror(fs->ls, "uninitialized integer[] in local variable");
  else if (ravi_type == RAVI_TTABLE)
    luaX_syntaxerror(fs->ls, "uninitialized table in local variable");
}


/* Generate instructions for converting types 
 * This is needed post a function call to handle
 * variable number of return values
 * n = number of return values to adjust 
 */
static void ravi_coercetype(LexState *ls, expdesc *v, int n)
{
  if (v->k != VCALL || n <= 0) return;
  /* For local variable declarations that call functions e.g.
  * local i = func()
  * Lua ensures that the function returns values to register assigned to variable i
  * and above so that no separate OP_MOVE instruction is necessary. So that means that
  * we need to coerce the return values in situ.
  */
  Instruction *pc = &getinstruction(ls->fs, v); /* Obtain the instruction for OP_CALL */
  lua_assert(GET_OPCODE(*pc) == OP_CALL);
  int a = GETARG_A(*pc); /* function return values will be placed from register pointed by A and upwards */
  /* all return values that are going to be assigned to typed local vars must be converted to the correct type */
  int i;
  for (i = a + 1; i < a + n; i++) {
    /* Since this is called when parsing local statements the variable may not yet
     * have a register assigned to it so we can't use raviY_get_register_typeinfo()
     * here. Instead we need to check the variable definition - so we 
     * first convert from local register to variable index.
     */
    int idx = register_to_locvar_index(ls->fs, i);
    ravitype_t ravi_type = ls->fs->f->locvars[idx].ravi_type;  /* get variable's type */
    TString *usertype = ls->fs->f->locvars[idx].usertype;
    /* do we need to convert ? */
    ravi_code_typecoersion(ls, i, ravi_type, usertype);
  }
}

static void ravi_setzero(FuncState *fs, int from, int n) {
  int last = from + n - 1;  /* last register to set nil */
  int i;
  for (i = from; i <= last; i++) {
    /* Since this is called when parsing local statements the variable may not yet
    * have a register assigned to it so we can't use raviY_get_register_typeinfo()
    * here. Instead we need to check the variable definition - so we
    * first convert from local register to variable index.
    */
    int idx = register_to_locvar_index(fs, i);
    ravitype_t ravi_type = fs->f->locvars[idx].ravi_type;  /* get variable's type */
    TString *usertype = fs->f->locvars[idx].usertype;
    /* do we need to convert ? */
    ravi_code_setzero(fs, i, ravi_type, usertype);
  }
}

static void localvar_adjust_assign(LexState *ls, int nvars, int nexps, expdesc *e) {
  FuncState *fs = ls->fs;
  int extra = nvars - nexps;
  if (hasmultret(e->k)) {
    extra++;  /* includes call itself */
    if (extra < 0) extra = 0;
    /* following adjusts the C operand in the OP_CALL instruction */
    luaK_setreturns(fs, e, extra);  /* last exp. provides the difference */

    /* Since we did not know how many return values to process in localvar_explist() we
    * need to add instructions for type coercions at this stage for any remaining
    * variables
    */
    ravi_coercetype(ls, e, extra);

    if (extra > 1) luaK_reserveregs(fs, extra - 1);
  }
  else {
    if (e->k != VVOID) luaK_exp2nextreg(fs, e);  /* close last expression */
    if (extra > 0) {
      int reg = fs->freereg;
      luaK_reserveregs(fs, extra);
      /* RAVI TODO for typed variables we should not set to nil? */
      luaK_nil(fs, reg, extra);

      /* typed variables that are primitives cannot be set to nil so
       * we need to emit instructions to initialise them to default values 
       */
      ravi_setzero(fs, reg, extra);
    }
  }
  if (nexps > nvars)
    ls->fs->freereg -= nexps - nvars;  /* remove extra values */
}

static void adjust_assign (LexState *ls, int nvars, int nexps, expdesc *e) {
  FuncState *fs = ls->fs;
  int extra = nvars - nexps;
  if (hasmultret(e->k)) {
    extra++;  /* includes call itself */
    if (extra < 0) extra = 0;
    /* following adjusts the C operand in the OP_CALL instruction */
    luaK_setreturns(fs, e, extra);  /* last exp. provides the difference */
    if (extra > 1) luaK_reserveregs(fs, extra-1);
  }
  else {
    if (e->k != VVOID) luaK_exp2nextreg(fs, e);  /* close last expression */
    if (extra > 0) {
      int reg = fs->freereg;
      luaK_reserveregs(fs, extra);
      /* RAVI TODO for typed variables we should not set to nil? */
      luaK_nil(fs, reg, extra);
    }
  }
  if (nexps > nvars)
    ls->fs->freereg -= nexps - nvars;  /* remove extra values */
}


static void enterlevel (LexState *ls) {
  lua_State *L = ls->L;
  ++L->nCcalls;
  checklimit(ls->fs, L->nCcalls, LUAI_MAXCCALLS, "C levels");
}


#define leavelevel(ls)	((ls)->L->nCcalls--)


static void closegoto (LexState *ls, int g, Labeldesc *label) {
  int i;
  FuncState *fs = ls->fs;
  Labellist *gl = &ls->dyd->gt;
  Labeldesc *gt = &gl->arr[g];
  lua_assert(eqstr(gt->name, label->name));
  if (gt->nactvar < label->nactvar) {
    TString *vname = getlocvar(fs, gt->nactvar)->varname;
    const char *msg = luaO_pushfstring(ls->L,
      "<goto %s> at line %d jumps into the scope of local '%s'",
      getstr(gt->name), gt->line, getstr(vname));
    semerror(ls, msg);
  }
  luaK_patchlist(fs, gt->pc, label->pc);
  /* remove goto from pending list */
  for (i = g; i < gl->n - 1; i++)
    gl->arr[i] = gl->arr[i + 1];
  gl->n--;
}


/*
** try to close a goto with existing labels; this solves backward jumps
*/
static int findlabel (LexState *ls, int g) {
  int i;
  BlockCnt *bl = ls->fs->bl;
  Dyndata *dyd = ls->dyd;
  Labeldesc *gt = &dyd->gt.arr[g];
  /* check labels in current block for a match */
  for (i = bl->firstlabel; i < dyd->label.n; i++) {
    Labeldesc *lb = &dyd->label.arr[i];
    if (eqstr(lb->name, gt->name)) {  /* correct label? */
      if (gt->nactvar > lb->nactvar &&
          (bl->upval || dyd->label.n > bl->firstlabel))
        luaK_patchclose(ls->fs, gt->pc, lb->nactvar);
      closegoto(ls, g, lb);  /* close it */
      return 1;
    }
  }
  return 0;  /* label not found; cannot close goto */
}


static int newlabelentry (LexState *ls, Labellist *l, TString *name,
                          int line, int pc) {
  int n = l->n;
  luaM_growvector(ls->L, l->arr, n, l->size,
                  Labeldesc, SHRT_MAX, "labels/gotos");
  l->arr[n].name = name;
  l->arr[n].line = line;
  l->arr[n].nactvar = ls->fs->nactvar;
  l->arr[n].pc = pc;
  l->n = n + 1;
  return n;
}


/*
** check whether new label 'lb' matches any pending gotos in current
** block; solves forward jumps
*/
static void findgotos (LexState *ls, Labeldesc *lb) {
  Labellist *gl = &ls->dyd->gt;
  int i = ls->fs->bl->firstgoto;
  while (i < gl->n) {
    if (eqstr(gl->arr[i].name, lb->name))
      closegoto(ls, i, lb);
    else
      i++;
  }
}


/*
** export pending gotos to outer level, to check them against
** outer labels; if the block being exited has upvalues, and
** the goto exits the scope of any variable (which can be the
** upvalue), close those variables being exited.
*/
static void movegotosout (FuncState *fs, BlockCnt *bl) {
  int i = bl->firstgoto;
  Labellist *gl = &fs->ls->dyd->gt;
  /* correct pending gotos to current block and try to close it
     with visible labels */
  while (i < gl->n) {
    Labeldesc *gt = &gl->arr[i];
    if (gt->nactvar > bl->nactvar) {
      if (bl->upval)
        luaK_patchclose(fs, gt->pc, bl->nactvar);
      gt->nactvar = bl->nactvar;
    }
    if (!findlabel(fs->ls, i))
      i++;  /* move to next one */
  }
}


static void enterblock (FuncState *fs, BlockCnt *bl, lu_byte isloop) {
  bl->isloop = isloop;
  bl->nactvar = fs->nactvar;
  bl->firstlabel = fs->ls->dyd->label.n;
  bl->firstgoto = fs->ls->dyd->gt.n;
  bl->upval = 0;
  bl->previous = fs->bl;
  fs->bl = bl;
  lua_assert(fs->freereg == fs->nactvar);
}


/*
** create a label named 'break' to resolve break statements
*/
static void breaklabel (LexState *ls) {
  TString *n = luaS_new(ls->L, "break");
  int l = newlabelentry(ls, &ls->dyd->label, n, 0, ls->fs->pc);
  findgotos(ls, &ls->dyd->label.arr[l]);
}

/*
** generates an error for an undefined 'goto'; choose appropriate
** message when label name is a reserved word (which can only be 'break')
*/
static l_noret undefgoto (LexState *ls, Labeldesc *gt) {
  const char *msg = isreserved(gt->name)
                    ? "<%s> at line %d not inside a loop"
                    : "no visible label '%s' for <goto> at line %d";
  msg = luaO_pushfstring(ls->L, msg, getstr(gt->name), gt->line);
  semerror(ls, msg);
}


static void leaveblock (FuncState *fs) {
  BlockCnt *bl = fs->bl;
  LexState *ls = fs->ls;
  if (bl->previous && bl->upval) {
    /* create a 'jump to here' to close upvalues */
    int j = luaK_jump(fs);
    luaK_patchclose(fs, j, bl->nactvar);
    luaK_patchtohere(fs, j);
  }
  if (bl->isloop)
    breaklabel(ls);  /* close pending breaks */
  fs->bl = bl->previous;
  removevars(fs, bl->nactvar);
  lua_assert(bl->nactvar == fs->nactvar);
  fs->freereg = fs->nactvar;  /* free registers */
  ls->dyd->label.n = bl->firstlabel;  /* remove local labels */
  if (bl->previous)  /* inner block? */
    movegotosout(fs, bl);  /* update pending gotos to outer block */
  else if (bl->firstgoto < ls->dyd->gt.n)  /* pending gotos in outer block? */
    undefgoto(ls, &ls->dyd->gt.arr[bl->firstgoto]);  /* error */
}


/*
** adds a new prototype into list of prototypes
*/
static Proto *addprototype (LexState *ls) {
  Proto *clp;
  lua_State *L = ls->L;
  FuncState *fs = ls->fs;
  Proto *f = fs->f;  /* prototype of current function */
  if (fs->np >= f->sizep) {
    int oldsize = f->sizep;
    luaM_growvector(L, f->p, fs->np, f->sizep, Proto *, MAXARG_Bx, "functions");
    while (oldsize < f->sizep)
      f->p[oldsize++] = NULL;
  }
  f->p[fs->np++] = clp = luaF_newproto(L);
  luaC_objbarrier(L, f, clp);
  return clp;
}


/*
** codes instruction to create new closure in parent function.
** The OP_CLOSURE instruction must use the last available register,
** so that, if it invokes the GC, the GC knows which registers
** are in use at that time.
*/
static void codeclosure (LexState *ls, expdesc *v) {
  FuncState *fs = ls->fs->prev;
  init_exp(v, VRELOCABLE, luaK_codeABx(fs, OP_CLOSURE, 0, fs->np - 1), RAVI_TFUNCTION, NULL);
  luaK_exp2nextreg(fs, v);  /* fix it at the last register */
  DEBUG_VARS(raviY_printf(ls->fs, "codeclosure -> closure created %e\n", v));
}


static void open_func (LexState *ls, FuncState *fs, BlockCnt *bl) {
  Proto *f;
  fs->prev = ls->fs;  /* linked list of funcstates */
  fs->ls = ls;
  ls->fs = fs;
  fs->pc = 0;
  fs->lasttarget = 0;
  fs->jpc = NO_JUMP;
  fs->freereg = 0;
  fs->nk = 0;
  fs->np = 0;
  fs->nups = 0;
  fs->nlocvars = 0;
  fs->nactvar = 0;
  fs->firstlocal = ls->dyd->actvar.n;
  fs->bl = NULL;
  f = fs->f;
  f->source = ls->source;
  f->maxstacksize = 2;  /* registers 0/1 are always valid */
  enterblock(fs, bl, 0);
  DEBUG_VARS(raviY_printf(fs, "open_func -> fs->firstlocal set to %d (ls->dyd->actvar.n), and fs->nactvar reset to 0\n", ls->dyd->actvar.n));
}


static void close_func (LexState *ls) {
  lua_State *L = ls->L;
  FuncState *fs = ls->fs;
  Proto *f = fs->f;
  luaK_ret(fs, 0, 0);  /* final return */
  leaveblock(fs);
  luaM_reallocvector(L, f->code, f->sizecode, fs->pc, Instruction);
  f->sizecode = fs->pc;
  luaM_reallocvector(L, f->lineinfo, f->sizelineinfo, fs->pc, int);
  f->sizelineinfo = fs->pc;
  luaM_reallocvector(L, f->k, f->sizek, fs->nk, TValue);
  f->sizek = fs->nk;
  luaM_reallocvector(L, f->p, f->sizep, fs->np, Proto *);
  f->sizep = fs->np;
  luaM_reallocvector(L, f->locvars, f->sizelocvars, fs->nlocvars, LocVar);
  f->sizelocvars = fs->nlocvars;
  luaM_reallocvector(L, f->upvalues, f->sizeupvalues, fs->nups, Upvaldesc);
  f->sizeupvalues = fs->nups;
  lua_assert(fs->bl == NULL);
  ls->fs = fs->prev;
  luaC_checkGC(L);
}



/*============================================================*/
/* GRAMMAR RULES */
/*============================================================*/


/*
** check whether current token is in the follow set of a block.
** 'until' closes syntactical blocks, but do not close scope,
** so it is handled in separate.
*/
static int block_follow (LexState *ls, int withuntil) {
  switch (ls->t.token) {
    case TK_ELSE: case TK_ELSEIF:
    case TK_END: case TK_EOS:
      return 1;
    case TK_UNTIL: return withuntil;
    default: return 0;
  }
}


static void statlist (LexState *ls) {
  /* statlist -> { stat [';'] } */
  while (!block_follow(ls, 1)) {
    if (ls->t.token == TK_RETURN) {
      statement(ls);
      return;  /* 'return' must be last statement */
    }
    statement(ls);
  }
}


static void fieldsel (LexState *ls, expdesc *v) {
  /* fieldsel -> ['.' | ':'] NAME */
  FuncState *fs = ls->fs;
  expdesc key = {.ravi_type = RAVI_TANY, .pc = -1};
  luaK_exp2anyregup(fs, v);
  luaX_next(ls);  /* skip the dot or colon */
  checkname(ls, &key);
  luaK_indexed(fs, v, &key);
}


static void yindex (LexState *ls, expdesc *v) {
  /* index -> '[' expr ']' */
  luaX_next(ls);  /* skip the '[' */
  expr(ls, v);
  luaK_exp2val(ls->fs, v);
  checknext(ls, ']');
}


/*
** {======================================================================
** Rules for Constructors
** =======================================================================
*/


struct ConsControl {
  expdesc v;  /* last list item read */
  expdesc *t;  /* table descriptor */
  int nh;  /* total number of 'record' elements */
  int na;  /* total number of array elements */
  int tostore;  /* number of array elements pending to be stored */
};


static void recfield (LexState *ls, struct ConsControl *cc) {
  /* recfield -> (NAME | '['exp1']') = exp1 */
  FuncState *fs = ls->fs;
  int reg = ls->fs->freereg;
  expdesc key = {.ravi_type = RAVI_TANY, .pc = -1},
          val = {.ravi_type = RAVI_TANY, .pc = -1};
  int rkkey;
  if (ls->t.token == TK_NAME) {
    checklimit(fs, cc->nh, MAX_INT, "items in a constructor");
    checkname(ls, &key);
  }
  else  /* ls->t.token == '[' */
    yindex(ls, &key);
  cc->nh++;
  checknext(ls, '=');
  rkkey = luaK_exp2RK(fs, &key);
  expr(ls, &val);
  luaK_codeABC(fs, OP_SETTABLE, cc->t->u.info, rkkey, luaK_exp2RK(fs, &val));
  fs->freereg = reg;  /* free registers */
}


static void closelistfield (FuncState *fs, struct ConsControl *cc) {
  if (cc->v.k == VVOID) return;  /* there is no list item */
  luaK_exp2nextreg(fs, &cc->v);
  cc->v.k = VVOID;
  if (cc->tostore == LFIELDS_PER_FLUSH) {
    luaK_setlist(fs, cc->t->u.info, cc->na, cc->tostore);  /* flush */
    cc->tostore = 0;  /* no more items pending */
  }
}


static void lastlistfield (FuncState *fs, struct ConsControl *cc) {
  if (cc->tostore == 0) return;
  if (hasmultret(cc->v.k)) {
    luaK_setmultret(fs, &cc->v);
    luaK_setlist(fs, cc->t->u.info, cc->na, LUA_MULTRET);
    cc->na--;  /* do not count last expression (unknown number of elements) */
  }
  else {
    if (cc->v.k != VVOID)
      luaK_exp2nextreg(fs, &cc->v);
    luaK_setlist(fs, cc->t->u.info, cc->na, cc->tostore);
  }
}


static void listfield (LexState *ls, struct ConsControl *cc) {
  /* listfield -> exp */
  expr(ls, &cc->v);
  checklimit(ls->fs, cc->na, MAX_INT, "items in a constructor");
  cc->na++;
  cc->tostore++;
}


static void field (LexState *ls, struct ConsControl *cc) {
  /* field -> listfield | recfield */
  switch(ls->t.token) {
    case TK_NAME: {  /* may be 'listfield' or 'recfield' */
      if (luaX_lookahead(ls) != '=')  /* expression? */
        listfield(ls, cc);
      else
        recfield(ls, cc);
      break;
    }
    case '[': {
      recfield(ls, cc);
      break;
    }
    default: {
      listfield(ls, cc);
      break;
    }
  }
}


static void constructor (LexState *ls, expdesc *t) {
  /* constructor -> '{' [ field { sep field } [sep] ] '}'
     sep -> ',' | ';' */
  FuncState *fs = ls->fs;
  int line = ls->linenumber;
  int pc = luaK_codeABC(fs, OP_NEWTABLE, 0, 0, 0);
  struct ConsControl cc;
  cc.na = cc.nh = cc.tostore = 0;
  cc.t = t;
  init_exp(t, VRELOCABLE, pc, RAVI_TTABLE, NULL); /* RAVI initial type may be modified */
  t->pc = pc; /* RAVI save pc of OP_NEWTABLE instruction so that the correct type can be set later */
  init_exp(&cc.v, VVOID, 0, RAVI_TANY, NULL);  /* no value (yet) */
  luaK_exp2nextreg(ls->fs, t);  /* fix it at stack top */
  checknext(ls, '{');
  do {
    lua_assert(cc.v.k == VVOID || cc.tostore > 0);
    if (ls->t.token == '}') break;
    closelistfield(fs, &cc);
    field(ls, &cc);
  } while (testnext(ls, ',') || testnext(ls, ';'));
  check_match(ls, '}', '{', line);
  lastlistfield(fs, &cc);
  SETARG_B(fs->f->code[pc], luaO_int2fb(cc.na)); /* set initial array size */
  SETARG_C(fs->f->code[pc], luaO_int2fb(cc.nh));  /* set initial table size */
  DEBUG_EXPR(raviY_printf(ls->fs, "constructor (OP_NEWTABLE pc = %d) %e\n", pc, t);)
}

/* }====================================================================== */

/*
 * We would like to allow user defined types to contain the sequence
 * NAME [. NAME]+
 * The initial NAME is supplied.
 * Returns extended name.
 * Note that the returned string will be anchored in the Lexer and must 
 * be anchored somewhere else by the time parsing finishes
 */
static TString *user_defined_type_name(LexState *ls, TString *typename) {
	size_t len = 0;
	if (testnext(ls, '.')) {
		char buffer[128] = { 0 };
		const char *str = getstr(typename);
		len = strlen(str);
		if (len >= sizeof buffer) {
			luaX_syntaxerror(ls, "User defined type name is too long");
			return typename;
		}
		snprintf(buffer, sizeof buffer, "%s", str);
		do {
			typename = str_checkname(ls);
			str = getstr(typename);
			size_t newlen = len + strlen(str) + 1;
			if (newlen >= sizeof buffer) {
				luaX_syntaxerror(ls, "User defined type name is too long");
				return typename;
			}
			snprintf(buffer + len, sizeof buffer - len, ".%s", str);
			len = newlen;
		} while (testnext(ls, '.'));
		typename = luaX_newstring(ls, buffer, strlen(buffer));
	}
	return typename;
}

/* RAVI Parse
 *   name : type
 *   where type is 'integer', 'integer[]',
 *                 'number', 'number[]'
 */
static ravitype_t declare_localvar(LexState *ls, TString **pusertype) {
  /* RAVI change - add type */
  TString *name = str_checkname(ls);
  /* assume a dynamic type */
  ravitype_t tt = RAVI_TANY;
  /* if the variable name is followed by a colon then we have a type
   * specifier
   */

  if (testnext(ls, ':')) {
    TString *typename = str_checkname(ls); /* we expect a type name */
    const char *str = getstr(typename);
    /* following is not very nice but easy as
     * the lexer doesn't need to be changed
     */
    if (strcmp(str, "integer") == 0)
      tt = RAVI_TNUMINT;
    else if (strcmp(str, "number") == 0)
      tt = RAVI_TNUMFLT;
    else if (strcmp(str, "closure") == 0)
      tt = RAVI_TFUNCTION;
    else if (strcmp(str, "table") == 0)
      tt = RAVI_TTABLE;
    else if (strcmp(str, "string") == 0)
      tt = RAVI_TSTRING;
    else if (strcmp(str, "boolean") == 0)
      tt = RAVI_TBOOLEAN;
    else if (strcmp(str, "any") == 0)
      tt = RAVI_TANY;
    else {
      /* default is a userdata type */
      tt = RAVI_TUSERDATA;
      typename = user_defined_type_name(ls, typename);
      str = getstr(typename);
      *pusertype = typename;
    }
    if (tt == RAVI_TNUMFLT || tt == RAVI_TNUMINT) {
      /* if we see [] then it is an array type */
      if (testnext(ls, '[')) {
        checknext(ls, ']');
        tt = (tt == RAVI_TNUMFLT) ? RAVI_TARRAYFLT : RAVI_TARRAYINT;
      }
    }
  }
  new_localvar(ls, name, tt, *pusertype);
  return tt;
}

static void parlist (LexState *ls) {
  /* parlist -> [ param { ',' param } ] */
  FuncState *fs = ls->fs;
  Proto *f = fs->f;
  int nparams = 0;
  enum { N = MAXVARS + 10 };
  int vars[N] = { 0 };
  TString *typenames[N] = { NULL };
  f->is_vararg = 0;
  if (ls->t.token != ')') {  /* is 'parlist' not empty? */
    do {
      switch (ls->t.token) {
        case TK_NAME: {  /* param -> NAME */
          /* RAVI change - add type */
          vars[nparams] = declare_localvar(ls, &typenames[nparams]);
          nparams++;
          break;
        }
        case TK_DOTS: {  /* param -> '...' */
          luaX_next(ls);
          f->is_vararg = 1;  /* declared vararg */
          break;
        }
        default: luaX_syntaxerror(ls, "<name> or '...' expected");
      }
    } while (!f->is_vararg && testnext(ls, ','));
  }
  adjustlocalvars(ls, nparams);
  f->numparams = cast_byte(fs->nactvar);
  luaK_reserveregs(fs, fs->nactvar);  /* reserve register for parameters */
  for (int i = 0; i < f->numparams; i++) {
    TString *usertype = NULL;
    ravitype_t tt = raviY_get_register_typeinfo(fs, i, &usertype);
    lua_assert((i < nparams && vars[i] == (int)tt) || 1);
    lua_assert((i < nparams && usertype == typenames[i]) || 1);
    DEBUG_VARS(raviY_printf(fs, "Parameter [%d] = %v\n", i + 1, getlocvar(fs, i)));
    /* do we need to convert ? */
    ravi_code_typecoersion(ls, i, tt, usertype);
  }
}


static void body (LexState *ls, expdesc *e, int ismethod, int line) {
  /* body ->  '(' parlist ')' block END */
  FuncState new_fs;
  BlockCnt bl;
  new_fs.f = addprototype(ls);
  new_fs.f->linedefined = line;
  open_func(ls, &new_fs, &bl);
  checknext(ls, '(');
  if (ismethod) {
    new_localvarliteral(ls, "self");  /* create 'self' parameter */
    adjustlocalvars(ls, 1);
  }
  parlist(ls);
  checknext(ls, ')');
  statlist(ls);
  new_fs.f->lastlinedefined = ls->linenumber;
  check_match(ls, TK_END, TK_FUNCTION, line);
  codeclosure(ls, e);
  close_func(ls);
}

/* parse expression list */
static int explist (LexState *ls, expdesc *v) {
  /* explist -> expr { ',' expr } */
  int n = 1;  /* at least one expression */
  expr(ls, v);
  while (testnext(ls, ',')) {
    luaK_exp2nextreg(ls->fs, v);
    expr(ls, v);
    n++;
  }
  return n;
}

/* check that the type of expression 'v' matches the type
* expected in var_types[] array where 'n' is the variable we are
* checking. The arrays 'var_types' and 'usertypes' are needed as 
* 'v' may be a function call returning multiple values, in which case
* we need to check all returned values against the expected types.
*/
static void ravi_typecheck(LexState *ls, expdesc *v, int *var_types,
                           TString **usertypes, int nvars, int n) {
  /* NOTE that 'v' may not have register assigned yet */  
  ravitype_t vartype = var_types[n];
  if (n < nvars && vartype != RAVI_TANY && v->ravi_type != vartype) {
    if (v->ravi_type != vartype &&
        (vartype == RAVI_TARRAYFLT || vartype == RAVI_TARRAYINT) &&
        v->k == VNONRELOC) {
      /* as the bytecode for generating a table is already emitted by this stage
       * we have to amend the generated byte code - not sure if there is a
       * better approach. The location of the OP_NEWTABLE instruction is in
       * v->pc and we check that this has the same destination register as
       * v->u.info which is our variable */ 
       // local a:int[] = { 1 } 
       //                     ^ We are just past this 
       //                       and about to assign to a
      int ok = 0;
      if (v->pc >= 0) {
        Instruction *pc =
            &ls->fs->f->code[v->pc]; /* Get the OP_NEWTABLE instruction */
        OpCode op = GET_OPCODE(*pc);
        if (op == OP_NEWTABLE) { /* check before making changes */
          int reg = GETARG_A(*pc);
          if (reg ==
              v->u.info) { /* double check that register is as expected */
            op = (vartype == RAVI_TARRAYINT) ? OP_RAVI_NEW_IARRAY
                                             : OP_RAVI_NEW_FARRAY;
            SET_OPCODE(*pc, op); /* modify opcode */
            DEBUG_CODEGEN(
                raviY_printf(ls->fs, "[%d]* %o ; modify opcode\n", v->pc, *pc));
            ok = 1;
          }
        }
      }
      if (!ok) 
        luaX_syntaxerror(ls, "expecting array initializer");
    }
    /* if we are calling a function then convert return types */
    else if (v->ravi_type != vartype &&
             (vartype == RAVI_TNUMFLT || vartype == RAVI_TNUMINT ||
              vartype == RAVI_TARRAYFLT || vartype == RAVI_TARRAYINT ||
              vartype == RAVI_TTABLE || vartype == RAVI_TSTRING ||
              vartype == RAVI_TFUNCTION || vartype == RAVI_TUSERDATA) &&
             v->k == VCALL) {
      /* For local variable declarations that call functions e.g.
       * local i = func()
       * Lua ensures that the function returns values to register assigned to
       * variable i and above so that no separate OP_MOVE instruction is
       * necessary. So that means that we need to coerce the return values
       * in situ.
       */
      Instruction *pc =
          &getinstruction(ls->fs, v); /* Obtain the instruction for OP_CALL */
      lua_assert(GET_OPCODE(*pc) == OP_CALL);
      int a = GETARG_A(*pc); /* function return values will be placed from
                                register pointed by A and upwards */
      int nrets = GETARG_C(*pc) - 1;
      /* operand C contains number of return values expected  */
      /* Note that at this stage nrets is always 1 - as Lua patches in the this
       * value for the last function call in a
       * variable declaration statement in adjust_assign and
       * localvar_adjust_assign */
      /* all return values that are going to be assigned to typed local vars
       * must be converted to the correct type */
      int i;
      for (i = n; i < (n + nrets); i++)
        /* do we need to convert ? */
        ravi_code_typecoersion(ls, a + (i - n), var_types[i], NULL);
    }
    else if ((vartype == RAVI_TNUMFLT || vartype == RAVI_TNUMINT) &&
             v->k == VINDEXED) {
      if ((vartype == RAVI_TNUMFLT && v->ravi_type != RAVI_TARRAYFLT) ||
          (vartype == RAVI_TNUMINT && v->ravi_type != RAVI_TARRAYINT))
        luaX_syntaxerror(ls, "Invalid local assignment");
    }
    else if ((vartype == RAVI_TSTRING && v->ravi_type != RAVI_TSTRING) ||
             (vartype == RAVI_TFUNCTION && v->ravi_type != RAVI_TFUNCTION) ||
             vartype == RAVI_TUSERDATA) {
      TString *usertype = usertypes[n]; // NULL if var_types[n] is not userdata
      /* we need to make sure that a register is assigned to 'v'
        so that we can emit type assertion instructions. This would have 
	normally happened in the calling function but we do it early here - 
	possibly missing some optimization opportunity (i.e. avoiding register
	assignment) */
      luaK_exp2nextreg(ls->fs, v);
      ravi_code_typecoersion(ls, v->u.info, vartype, usertype);
    }
    else {
      luaX_syntaxerror(ls, "Invalid local assignment");
    }
  }
}

/* parse expression list, and validate that the expressions match expected
 * types provided in vars array. This is a modified version of explist() to be
 * used to local variable declaration statement only.
 */
static int localvar_explist(LexState *ls, expdesc *v, int *vars, TString** usertypes, int nvars) {
  /* explist -> expr { ',' expr } */
  int n = 1;  /* at least one expression */
  expr(ls, v);
  ravi_typecheck(ls, v, vars, usertypes, nvars, 0);
  while (testnext(ls, ',')) {
    luaK_exp2nextreg(ls->fs, v);
    expr(ls, v);
    ravi_typecheck(ls, v, vars, usertypes, nvars, n);
    n++;
  }
  return n;
}

/* parse function arguments */
static void funcargs (LexState *ls, expdesc *f, int line) {
  FuncState *fs = ls->fs;
  expdesc args = {.ravi_type = RAVI_TANY, .pc = -1};
  int base, nparams;
  switch (ls->t.token) {
    case '(': {  /* funcargs -> '(' [ explist ] ')' */
      luaX_next(ls);
      if (ls->t.token == ')')  /* arg list is empty? */
        args.k = VVOID;
      else {
        explist(ls, &args);
        luaK_setmultret(fs, &args);
      }
      check_match(ls, ')', '(', line);
      break;
    }
    case '{': {  /* funcargs -> constructor */
      constructor(ls, &args);
      break;
    }
    case TK_STRING: {  /* funcargs -> STRING */
      codestring(ls, &args, ls->t.seminfo.ts);
      luaX_next(ls);  /* must use 'seminfo' before 'next' */
      break;
    }
    default: {
      luaX_syntaxerror(ls, "function arguments expected");
    }
  }
  lua_assert(f->k == VNONRELOC);
  base = f->u.info;  /* base register for call */
  if (hasmultret(args.k))
    nparams = LUA_MULTRET;  /* open call */
  else {
    if (args.k != VVOID)
      luaK_exp2nextreg(fs, &args);  /* close last argument */
    nparams = fs->freereg - (base+1);
  }
  init_exp(f, VCALL, luaK_codeABC(fs, OP_CALL, base, nparams + 1, 2), RAVI_TANY, NULL); /* RAVI TODO return value from function call not known */
  luaK_fixline(fs, line);
  fs->freereg = base+1;  /* call remove function and arguments and leaves
                            (unless changed) one result */
}




/*
** {======================================================================
** Expression parsing
** =======================================================================
*/

/* primary expression - name or subexpression */
static void primaryexp (LexState *ls, expdesc *v) {
  /* primaryexp -> NAME | '(' expr ')' */
  switch (ls->t.token) {
    case '(': {
      int line = ls->linenumber;
      luaX_next(ls);
      expr(ls, v);
      check_match(ls, ')', '(', line);
      luaK_dischargevars(ls->fs, v);
      return;
    }
    case TK_NAME: {
      singlevar(ls, v);
      return;
    }
    default: {
      luaX_syntaxerror(ls, "unexpected symbol");
    }
  }
}

/* variable or field access or function call */
static void suffixedexp (LexState *ls, expdesc *v) {
  /* suffixedexp ->
       primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs } */
  FuncState *fs = ls->fs;
  int line = ls->linenumber;
  primaryexp(ls, v);
  for (;;) {
    switch (ls->t.token) {
      case '.': {  /* fieldsel */
        fieldsel(ls, v);
        break;
      }
      case '[': {  /* '[' exp1 ']' */
        expdesc key = {.ravi_type = RAVI_TANY, .pc = -1};
        luaK_exp2anyregup(fs, v);
        yindex(ls, &key);
        luaK_indexed(fs, v, &key);
        break;
      }
      case ':': {  /* ':' NAME funcargs */
        expdesc key = {.ravi_type = RAVI_TANY, .pc = -1};
        luaX_next(ls);
        checkname(ls, &key);
        luaK_self(fs, v, &key);
        funcargs(ls, v, line);
        break;
      }
      case '(': case TK_STRING: case '{': {  /* funcargs */
        luaK_exp2nextreg(fs, v);
        funcargs(ls, v, line);
        break;
      }
      default: return;
    }
  }
}


static void simpleexp (LexState *ls, expdesc *v) {
  /* simpleexp -> FLT | INT | STRING | NIL | TRUE | FALSE | ... |
                  constructor | FUNCTION body | suffixedexp */
  switch (ls->t.token) {
    case TK_FLT: {
      init_exp(v, VKFLT, 0, RAVI_TNUMFLT, NULL);
      v->u.nval = ls->t.seminfo.r;
      break;
    }
    case TK_INT: {
      init_exp(v, VKINT, 0, RAVI_TNUMINT, NULL);
      v->u.ival = ls->t.seminfo.i;
      break;
    }
    case TK_STRING: {
      codestring(ls, v, ls->t.seminfo.ts);
      break;
    }
    case TK_NIL: {
      init_exp(v, VNIL, 0, RAVI_TNIL, NULL);
      break;
    }
    case TK_TRUE: {
      init_exp(v, VTRUE, 0, RAVI_TANY, NULL); /* RAVI TODO */
      break;
    }
    case TK_FALSE: {
      init_exp(v, VFALSE, 0, RAVI_TANY, NULL); /* RAVI TODO */
      break;
    }
    case TK_DOTS: {  /* vararg */
      FuncState *fs = ls->fs;
      check_condition(ls, fs->f->is_vararg,
                      "cannot use '...' outside a vararg function");
      init_exp(v, VVARARG, luaK_codeABC(fs, OP_VARARG, 0, 1, 0), RAVI_TANY, NULL);
      break;
    }
    case '{': {  /* constructor */
      constructor(ls, v);
      return;
    }
    case TK_FUNCTION: {
      luaX_next(ls);
      body(ls, v, 0, ls->linenumber);
      return;
    }
    default: {
      suffixedexp(ls, v);
      return;
    }
  }
  luaX_next(ls);
}


static UnOpr getunopr (int op) {
  switch (op) {
    case TK_NOT: return OPR_NOT;
    case '-': return OPR_MINUS;
    case '~': return OPR_BNOT;
    case '#': return OPR_LEN;
    case TK_TO_INTEGER: return OPR_TO_INTEGER;
    case TK_TO_NUMBER: return OPR_TO_NUMBER;
    case TK_TO_INTARRAY: return OPR_TO_INTARRAY;
    case TK_TO_NUMARRAY: return OPR_TO_NUMARRAY;
    case TK_TO_TABLE: return OPR_TO_TABLE;
    case TK_TO_STRING: return OPR_TO_STRING;
    case TK_TO_CLOSURE: return OPR_TO_CLOSURE;
    case '@': return OPR_TO_TYPE;
    default: return OPR_NOUNOPR;
  }
}


static BinOpr getbinopr (int op) {
  switch (op) {
    case '+': return OPR_ADD;
    case '-': return OPR_SUB;
    case '*': return OPR_MUL;
    case '%': return OPR_MOD;
    case '^': return OPR_POW;
    case '/': return OPR_DIV;
    case TK_IDIV: return OPR_IDIV;
    case '&': return OPR_BAND;
    case '|': return OPR_BOR;
    case '~': return OPR_BXOR;
    case TK_SHL: return OPR_SHL;
    case TK_SHR: return OPR_SHR;
    case TK_CONCAT: return OPR_CONCAT;
    case TK_NE: return OPR_NE;
    case TK_EQ: return OPR_EQ;
    case '<': return OPR_LT;
    case TK_LE: return OPR_LE;
    case '>': return OPR_GT;
    case TK_GE: return OPR_GE;
    case TK_AND: return OPR_AND;
    case TK_OR: return OPR_OR;
    default: return OPR_NOBINOPR;
  }
}


static const struct {
  lu_byte left;  /* left priority for each binary operator */
  lu_byte right; /* right priority */
} priority[] = {  /* ORDER OPR */
   {10, 10}, {10, 10},           /* '+' '-' */
   {11, 11}, {11, 11},           /* '*' '%' */
   {14, 13},                  /* '^' (right associative) */
   {11, 11}, {11, 11},           /* '/' '//' */
   {6, 6}, {4, 4}, {5, 5},   /* '&' '|' '~' */
   {7, 7}, {7, 7},           /* '<<' '>>' */
   {9, 8},                   /* '..' (right associative) */
   {3, 3}, {3, 3}, {3, 3},   /* ==, <, <= */
   {3, 3}, {3, 3}, {3, 3},   /* ~=, >, >= */
   {2, 2}, {1, 1}            /* and, or */
};

#define UNARY_PRIORITY	12  /* priority for unary operators */


/*
** subexpr -> (simpleexp | unop subexpr) { binop subexpr }
** where 'binop' is any binary operator with a priority higher than 'limit'
*/
static BinOpr subexpr (LexState *ls, expdesc *v, int limit) {
  BinOpr op;
  UnOpr uop;
  enterlevel(ls);
  uop = getunopr(ls->t.token);
  if (uop != OPR_NOUNOPR) {
    int line = ls->linenumber;
	// RAVI change - get usertype if @<name>
    TString *usertype = NULL;
    if (uop == OPR_TO_TYPE) {
      usertype = ls->t.seminfo.ts;
      luaX_next(ls);
      // Check and expand to extended name if necessary
      usertype = user_defined_type_name(ls, usertype);
    }
    else {
      luaX_next(ls);
    }
    subexpr(ls, v, UNARY_PRIORITY);
    luaK_prefix(ls->fs, uop, v, line, usertype);
  }
  else {
    simpleexp(ls, v);
    DEBUG_EXPR(raviY_printf(ls->fs, "subexpr -> simpleexpr %e\n", v));
  }
  /* expand while operators have priorities higher than 'limit' */
  op = getbinopr(ls->t.token);
  while (op != OPR_NOBINOPR && priority[op].left > limit) {
    expdesc v2 = {.ravi_type = RAVI_TANY, .pc = -1};
    BinOpr nextop;
    int line = ls->linenumber;
    luaX_next(ls);
    luaK_infix(ls->fs, op, v);
    /* read sub-expression with higher priority */
    nextop = subexpr(ls, &v2, priority[op].right);
    DEBUG_EXPR(raviY_printf(ls->fs, "subexpr-> %e binop(%d) %e\n", v, (int)op, &v2));
    /* 
    The bool 'and' and 'or' operators preserve the type of the
    expression that gets selected, so if these are both integer or number types
    then we know result will be integer or number, else the result is
    unpredictable so we set both expressions to RAVI_TANY
    */
    if (op == OPR_AND || op == OPR_OR) {
      if (v->ravi_type != v2.ravi_type || (v->ravi_type != RAVI_TNUMINT && v->ravi_type != RAVI_TNUMFLT)) {
        v->ravi_type = RAVI_TANY;
        v2.ravi_type = RAVI_TANY;
      }
    }
    luaK_posfix(ls->fs, op, v, &v2, line);
    DEBUG_EXPR(raviY_printf(ls->fs, "subexpr-> after posfix %e\n", v));
    op = nextop;
  }
  leavelevel(ls);
  return op;  /* return first untreated operator */
}


static void expr (LexState *ls, expdesc *v) {
  subexpr(ls, v, 0);
}

/* }==================================================================== */



/*
** {======================================================================
** Rules for Statements
** =======================================================================
*/


static void block (LexState *ls) {
  /* block -> statlist */
  FuncState *fs = ls->fs;
  BlockCnt bl;
  enterblock(fs, &bl, 0);
  statlist(ls);
  leaveblock(fs);
}


/*
** structure to chain all variables in the left-hand side of an
** assignment
*/
struct LHS_assign {
  struct LHS_assign *prev;
  expdesc v;  /* variable (global, local, upvalue, or indexed) */
};


/*
** check whether, in an assignment to an upvalue/local variable, the
** upvalue/local variable is begin used in a previous assignment to a
** table. If so, save original upvalue/local value in a safe place and
** use this safe copy in the previous assignment.
*/
static void check_conflict (LexState *ls, struct LHS_assign *lh, expdesc *v) {
  FuncState *fs = ls->fs;
  int extra = fs->freereg;  /* eventual position to save local variable */
  int conflict = 0;
  for (; lh; lh = lh->prev) {  /* check all previous assignments */
    if (lh->v.k == VINDEXED) {  /* assigning to a table? */
      /* table is the upvalue/local being assigned now? */
      if (lh->v.u.ind.vt == v->k && lh->v.u.ind.t == v->u.info) {
        conflict = 1;
        lh->v.u.ind.vt = VLOCAL;
        lh->v.u.ind.t = extra;  /* previous assignment will use safe copy */
      }
      /* index is the local being assigned? (index cannot be upvalue) */
      if (v->k == VLOCAL && lh->v.u.ind.idx == v->u.info) {
        conflict = 1;
        lh->v.u.ind.idx = extra;  /* previous assignment will use safe copy */
      }
    }
  }
  if (conflict) {
    /* copy upvalue/local value to a temporary (in position 'extra') */
    OpCode op = (v->k == VLOCAL) ? OP_MOVE : OP_GETUPVAL;
    luaK_codeABC(fs, op, extra, v->u.info, 0);
    luaK_reserveregs(fs, 1);
  }
}

/* parse assignment (not part of local statement) - for each variable
 * on the left and side this is called recursively with increasing nvars.
 * The final recursive call parses the rhs.
 */
static void assignment (LexState *ls, struct LHS_assign *lh, int nvars) {
  expdesc e = {.ravi_type = RAVI_TANY, .pc = -1};
  check_condition(ls, vkisvar(lh->v.k), "syntax error");
  if (testnext(ls, ',')) {  /* assignment -> ',' suffixedexp assignment */
    struct LHS_assign nv = { .v.ravi_type = RAVI_TANY, .v.pc = -1 };
    //nv.v.ravi_type = RAVI_TANY;
    //nv.v.pc = -1;
    nv.prev = lh;
    suffixedexp(ls, &nv.v);
    DEBUG_EXPR(raviY_printf(ls->fs, "assignment -> suffixedexp %e\n", &nv.v));
    if (nv.v.k != VINDEXED)
      check_conflict(ls, lh, &nv.v);
    checklimit(ls->fs, nvars + ls->L->nCcalls, LUAI_MAXCCALLS,
                    "C levels");
    assignment(ls, &nv, nvars+1);
  }
  else {  /* assignment -> '=' explist */
    int nexps;
    checknext(ls, '=');
    nexps = explist(ls, &e);
    DEBUG_EXPR(raviY_printf(ls->fs, "assignment -> = explist %e\n", &e));
    if (nexps != nvars) {
      adjust_assign(ls, nvars, nexps, &e);
    }
    else {
      luaK_setoneret(ls->fs, &e);  /* close last expression */
      luaK_storevar(ls->fs, &lh->v, &e);
      DEBUG_EXPR(raviY_printf(ls->fs, "assignment -> lhs = %e, rhs = %e\n", &lh->v, &e));
      return;  /* avoid default */
    }
  }
  init_exp(&e, VNONRELOC, ls->fs->freereg-1, RAVI_TANY, NULL);  /* default assignment */
  luaK_storevar(ls->fs, &lh->v, &e);
  DEBUG_EXPR(raviY_printf(ls->fs, "assignment lhs = %e, rhs = %e\n", &lh->v, &e));
}

/* parse condition in a repeat statement or an if control structure
 * called by repeatstat(), test_then_block()
 */
static int cond (LexState *ls) {
  /* cond -> exp */
  expdesc v = {.ravi_type = RAVI_TANY, .pc = -1};
  expr(ls, &v);                   /* read condition */
  if (v.k == VNIL) v.k = VFALSE;  /* 'falses' are all equal here */
  luaK_goiftrue(ls->fs, &v);
  return v.f;
}


static void gotostat (LexState *ls, int pc) {
  int line = ls->linenumber;
  TString *label;
  int g;
  if (testnext(ls, TK_GOTO))
    label = str_checkname(ls);
  else {
    luaX_next(ls);  /* skip break */
    label = luaS_new(ls->L, "break");
  }
  g = newlabelentry(ls, &ls->dyd->gt, label, line, pc);
  findlabel(ls, g);  /* close it if label already defined */
}


/* check for repeated labels on the same block */
static void checkrepeated (FuncState *fs, Labellist *ll, TString *label) {
  int i;
  for (i = fs->bl->firstlabel; i < ll->n; i++) {
    if (eqstr(label, ll->arr[i].name)) {
      const char *msg = luaO_pushfstring(fs->ls->L,
                          "label '%s' already defined on line %d",
                          getstr(label), ll->arr[i].line);
      semerror(fs->ls, msg);
    }
  }
}


/* skip no-op statements */
static void skipnoopstat (LexState *ls) {
  while (ls->t.token == ';' || ls->t.token == TK_DBCOLON)
    statement(ls);
}


static void labelstat (LexState *ls, TString *label, int line) {
  /* label -> '::' NAME '::' */
  FuncState *fs = ls->fs;
  Labellist *ll = &ls->dyd->label;
  int l;  /* index of new label being created */
  checkrepeated(fs, ll, label);  /* check for repeated labels */
  checknext(ls, TK_DBCOLON);  /* skip double colon */
  /* create new entry for this label */
  l = newlabelentry(ls, ll, label, line, luaK_getlabel(fs));
  skipnoopstat(ls);  /* skip other no-op statements */
  if (block_follow(ls, 0)) {  /* label is last no-op statement in the block? */
    /* assume that locals are already out of scope */
    ll->arr[l].nactvar = fs->bl->nactvar;
  }
  findgotos(ls, &ll->arr[l]);
}

/* parse a while-do control structure, body processed by block()
 * called by statement()
 */
static void whilestat (LexState *ls, int line) {
  /* whilestat -> WHILE cond DO block END */
  FuncState *fs = ls->fs;
  int whileinit;
  int condexit;
  BlockCnt bl;
  luaX_next(ls);  /* skip WHILE */
  whileinit = luaK_getlabel(fs);
  condexit = cond(ls);
  enterblock(fs, &bl, 1);
  checknext(ls, TK_DO);
  block(ls);
  luaK_jumpto(fs, whileinit);
  check_match(ls, TK_END, TK_WHILE, line);
  leaveblock(fs);
  luaK_patchtohere(fs, condexit);  /* false conditions finish the loop */
}

/* parse a repeat-until control structure, body parsed by statlist()
 * called by statement()
 */
static void repeatstat (LexState *ls, int line) {
  /* repeatstat -> REPEAT block UNTIL cond */
  int condexit;
  FuncState *fs = ls->fs;
  int repeat_init = luaK_getlabel(fs);
  BlockCnt bl1, bl2;
  enterblock(fs, &bl1, 1);  /* loop block */
  enterblock(fs, &bl2, 0);  /* scope block */
  luaX_next(ls);  /* skip REPEAT */
  statlist(ls);
  check_match(ls, TK_UNTIL, TK_REPEAT, line);
  condexit = cond(ls);  /* read condition (inside scope block) */
  if (bl2.upval)  /* upvalues? */
    luaK_patchclose(fs, condexit, bl2.nactvar);
  leaveblock(fs);  /* finish scope */
  luaK_patchlist(fs, condexit, repeat_init);  /* close the loop */
  leaveblock(fs);  /* finish loop */
}

typedef struct Fornuminfo {
  ravitype_t type;
  int is_constant;
  int int_value;
} Fornuminfo;

/* parse the single expressions needed in numerical for loops
 * called by fornum()
 */
static int exp1 (LexState *ls, Fornuminfo *info) {
  /* Since the local variable in a fornum loop is local to the loop and does
   * not use any variable in outer scope we don't need to check its
   * type - also the loop is already optimised so no point trying to
   * optimise the iteration variable
   */
  expdesc e = {.ravi_type = RAVI_TANY, .pc = -1};
  int reg;
  int expect_int = 0;
  if (ls->t.token == '#')
    expect_int = 1;
  expr(ls, &e);
  DEBUG_EXPR(raviY_printf(ls->fs, "fornum exp -> %e\n", &e));
  info->is_constant = (e.k == VKINT);
  info->int_value = info->is_constant ? e.u.ival : 0;
  luaK_exp2nextreg(ls->fs, &e);
  lua_assert(e.k == VNONRELOC);
  reg = e.u.info;
  if (expect_int && e.ravi_type != RAVI_TNUMINT) {
    luaK_codeABC(ls->fs, OP_RAVI_TOINT, reg, 0, 0);
    info->type = RAVI_TNUMINT;
  }
  else {
    info->type = e.ravi_type;
  }
  return reg;
}

/* parse a for loop body for both versions of the for loop
 * called by fornum(), forlist()
 */
static void forbody (LexState *ls, int base, int line, int nvars, int isnum, Fornuminfo *info) {
  /* forbody -> DO block */
  BlockCnt bl;
  OpCode forprep_inst = OP_FORPREP, forloop_inst = OP_FORLOOP;
  FuncState *fs = ls->fs;
  int prep, endfor;
  adjustlocalvars(ls, 3);  /* control variables */
  checknext(ls, TK_DO);
  if (isnum) {
    ls->fs->f->ravi_jit.jit_flags = RAVI_JIT_FLAG_HASFORLOOP;
    if (info && info->is_constant && info->int_value > 1) {
      forprep_inst = OP_RAVI_FORPREP_IP;
      forloop_inst = OP_RAVI_FORLOOP_IP;
    }
    else if (info && info->is_constant && info->int_value == 1) {
      forprep_inst = OP_RAVI_FORPREP_I1;
      forloop_inst = OP_RAVI_FORLOOP_I1;
    }
  }
  prep = isnum ? luaK_codeAsBx(fs, forprep_inst, base, NO_JUMP) : luaK_jump(fs);
  enterblock(fs, &bl, 0);  /* scope for declared variables */
  adjustlocalvars(ls, nvars);
  luaK_reserveregs(fs, nvars);
  block(ls);
  leaveblock(fs);  /* end of scope for declared variables */
  luaK_patchtohere(fs, prep);
  if (isnum)  /* numeric for? */
    endfor = luaK_codeAsBx(fs, forloop_inst, base, NO_JUMP);
  else {  /* generic for */
    luaK_codeABC(fs, OP_TFORCALL, base, 0, nvars);
    luaK_fixline(fs, line);
    endfor = luaK_codeAsBx(fs, OP_TFORLOOP, base + 2, NO_JUMP);
  }
  luaK_patchlist(fs, endfor, prep + 1);
  luaK_fixline(fs, line);
}

/* parse a numerical for loop, calls forbody()
 * called from forstat()
 */
static void fornum (LexState *ls, TString *varname, int line) {
  /* fornum -> NAME = exp1,exp1[,exp1] forbody */
  FuncState *fs = ls->fs;
  int base = fs->freereg;
  new_localvarliteral(ls, "(for index)");
  new_localvarliteral(ls, "(for limit)");
  new_localvarliteral(ls, "(for step)");
  new_localvar(ls, varname, RAVI_TANY, NULL);
  /* The fornum sets up its own variables as above.
     These are expected to hold numeric values - but from Ravi's
     point of view we need to know if the variable is an integer or
     double. So we need to check if this can be determined from the
     fornum expressions. If we can then we will set the 
     fornum variables to the type we discover.
  */
  int var_idx = fs->nlocvars - 4; /* note location of idx variable */
  checknext(ls, '=');
  /* get the type of each expression */
  Fornuminfo tidx = { RAVI_TANY,0,0 }, tlimit = { RAVI_TANY,0,0 }, tstep = { RAVI_TNUMINT,0,0 };
  Fornuminfo *info = NULL;
  exp1(ls, &tidx);  /* initial value */
  checknext(ls, ',');
  exp1(ls, &tlimit);  /* limit */
  if (testnext(ls, ','))
    exp1(ls, &tstep);  /* optional step */
  else {  /* default step = 1 */
    tstep.is_constant = 1;
    tstep.int_value = 1;
    luaK_codek(fs, fs->freereg, luaK_intK(fs, 1));
    luaK_reserveregs(fs, 1);
  }
  if (tidx.type == tlimit.type && tlimit.type == tstep.type && 
     (tidx.type == RAVI_TNUMFLT || tidx.type == RAVI_TNUMINT)) {
    LocVar *vidx, *vlimit, *vstep, *vvar;
    if (tidx.type == RAVI_TNUMINT && tstep.is_constant)
      info = &tstep;
    /* Note that as locvars may be reallocated while creating variables
       therefore we access the variables here */
    vidx = &fs->f->locvars[var_idx]; /* index variable - not yet active so get it from locvars*/
    vlimit = &fs->f->locvars[var_idx+1]; /* index variable - not yet active so get it from locvars*/
    vstep = &fs->f->locvars[var_idx+2]; /* index variable - not yet active so get it from locvars*/
    vvar = &fs->f->locvars[var_idx+3]; /* index variable - not yet active so get it from locvars*/
                                              /* Ok so we have an integer or double */
    vidx->ravi_type = vlimit->ravi_type = vstep->ravi_type = vvar->ravi_type = tidx.type;
    DEBUG_VARS(raviY_printf(fs, "fornum -> setting type for index %v\n", vidx));
    DEBUG_VARS(raviY_printf(fs, "fornum -> setting type for limit %v\n", vlimit));
    DEBUG_VARS(raviY_printf(fs, "fornum -> setting type for step %v\n", vstep));
    DEBUG_VARS(raviY_printf(fs, "fornum -> setting type for variable %v\n", vvar));
  }
  forbody(ls, base, line, 1, 1, info);
}

/* parse a generic for loop, calls forbody() 
 * called from forstat()
 */
static void forlist (LexState *ls, TString *indexname) {
  /* forlist -> NAME {,NAME} IN explist forbody */
  FuncState *fs = ls->fs;
  expdesc e = {.ravi_type = RAVI_TANY, .pc = -1};
  int nvars = 4; /* gen, state, control, plus at least one declared var */
  int line;
  int base = fs->freereg;  
  /* create control variables */
  new_localvarliteral(ls, "(for generator)");
  new_localvarliteral(ls, "(for state)");
  new_localvarliteral(ls, "(for control)");
  /* create declared variables */
  new_localvar(ls, indexname, RAVI_TANY, NULL); /* RAVI TODO for name:type syntax? */
  while (testnext(ls, ',')) {
    new_localvar(ls, str_checkname(ls), RAVI_TANY, NULL); /* RAVI change - add type */
    nvars++;
  }
  checknext(ls, TK_IN);
  line = ls->linenumber;
  adjust_assign(ls, 3, explist(ls, &e), &e);
  luaK_checkstack(fs, 3);  /* extra space to call generator */
  forbody(ls, base, line, nvars - 3, 0, NULL);
}

/* initial parsing of a for loop - calls fornum() or forlist()
 * called from statement()
 */
static void forstat (LexState *ls, int line) {
  /* forstat -> FOR (fornum | forlist) END */
  FuncState *fs = ls->fs;
  TString *varname;
  BlockCnt bl;
  enterblock(fs, &bl, 1);  /* scope for loop and control variables */
  luaX_next(ls);  /* skip 'for' */
  varname = str_checkname(ls);  /* first variable name */
  switch (ls->t.token) {
    case '=': fornum(ls, varname, line); break;
    case ',': case TK_IN: forlist(ls, varname); break;
    default: luaX_syntaxerror(ls, "'=' or 'in' expected");
  }
  check_match(ls, TK_END, TK_FOR, line);
  leaveblock(fs);  /* loop scope ('break' jumps to this point) */
}

/* parse if cond then block - called from ifstat() */
static void test_then_block (LexState *ls, int *escapelist) {
  /* test_then_block -> [IF | ELSEIF] cond THEN block */
  BlockCnt bl;
  FuncState *fs = ls->fs;
  expdesc v = {.ravi_type = RAVI_TANY, .pc = -1};
  int jf;         /* instruction to skip 'then' code (if condition is false) */
  luaX_next(ls);  /* skip IF or ELSEIF */
  expr(ls, &v);  /* read condition */
  checknext(ls, TK_THEN);
  if (ls->t.token == TK_GOTO || ls->t.token == TK_BREAK) {
    luaK_goiffalse(ls->fs, &v);  /* will jump to label if condition is true */
    enterblock(fs, &bl, 0);  /* must enter block before 'goto' */
    gotostat(ls, v.t);  /* handle goto/break */
    while (testnext(ls, ';')) {}  /* skip colons */
    if (block_follow(ls, 0)) {  /* 'goto' is the entire block? */
      leaveblock(fs);
      return;  /* and that is it */
    }
    else  /* must skip over 'then' part if condition is false */
      jf = luaK_jump(fs);
  }
  else {  /* regular case (not goto/break) */
    luaK_goiftrue(ls->fs, &v);  /* skip over block if condition is false */
    enterblock(fs, &bl, 0);
    jf = v.f;
  }
  statlist(ls);  /* 'then' part */
  leaveblock(fs);
  if (ls->t.token == TK_ELSE ||
      ls->t.token == TK_ELSEIF)  /* followed by 'else'/'elseif'? */
    luaK_concat(fs, escapelist, luaK_jump(fs));  /* must jump over it */
  luaK_patchtohere(fs, jf);
}

/* parse an if control structure - called from statement() */
static void ifstat (LexState *ls, int line) {
  /* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
  FuncState *fs = ls->fs;
  int escapelist = NO_JUMP;  /* exit list for finished parts */
  test_then_block(ls, &escapelist);  /* IF cond THEN block */
  while (ls->t.token == TK_ELSEIF)
    test_then_block(ls, &escapelist);  /* ELSEIF cond THEN block */
  if (testnext(ls, TK_ELSE))
    block(ls);  /* 'else' part */
  check_match(ls, TK_END, TK_IF, line);
  luaK_patchtohere(fs, escapelist);  /* patch escape list to 'if' end */
}

/* parse a local function statement - called from statement() */
static void localfunc (LexState *ls) {
  expdesc b = {.ravi_type = RAVI_TANY, .pc = -1};
  FuncState *fs = ls->fs;
  /* RAVI change - add type */
  new_localvar(ls, str_checkname(ls), RAVI_TFUNCTION, NULL);  /* new local variable */
  adjustlocalvars(ls, 1);  /* enter its scope */
  body(ls, &b, 0, ls->linenumber);  /* function created in next register */
  /* debug information will only see the variable after this point! */
  getlocvar(fs, b.u.info)->startpc = fs->pc;
}

/* parse a local variable declaration statement - called from statement() */
static void localstat (LexState *ls) {
  /* stat -> LOCAL NAME {',' NAME} ['=' explist] */
  int nvars = 0;
  int nexps;
  expdesc e = { .ravi_type = RAVI_TANY,.pc = -1 };
  /* RAVI while declaring locals we need to gather the types
   * so that we can check any assignments later on.
   * TODO we may be able to use register_typeinfo() here
   * instead.
   */
  enum { N = MAXVARS + 10 };
  int vars[N] = { 0 };
  TString *usertypes[N] = { NULL };
  do {
    /* RAVI changes start */
    /* local name : type = value */
    vars[nvars] = declare_localvar(ls, &usertypes[nvars]);
    /* RAVI changes end */
    nvars++;
    if (nvars >= N)
      luaX_syntaxerror(ls, "too many local variables");
  } while (testnext(ls, ','));
  if (testnext(ls, '='))
    nexps = localvar_explist(ls, &e, vars, usertypes, nvars);
  else {
    e.k = VVOID;
    nexps = 0;
  }
  localvar_adjust_assign(ls, nvars, nexps, &e);
  adjustlocalvars(ls, nvars);
}

/* parse a function name specification - called from funcstat() 
 * returns boolean value - true if function is a method
 */
static int funcname (LexState *ls, expdesc *v) {
  /* funcname -> NAME {fieldsel} [':' NAME] */
  int ismethod = 0;
  singlevar(ls, v);
  while (ls->t.token == '.')
    fieldsel(ls, v);
  if (ls->t.token == ':') {
    ismethod = 1;
    fieldsel(ls, v);
  }
  return ismethod;
}

/* parse a function statement - called from statement() */
static void funcstat (LexState *ls, int line) {
  /* funcstat -> FUNCTION funcname body */
  int ismethod;
  expdesc v = {.ravi_type = RAVI_TANY, .pc = -1},
          b = {.ravi_type = RAVI_TANY, .pc = -1};
  luaX_next(ls); /* skip FUNCTION */
  ismethod = funcname(ls, &v);
  DEBUG_VARS(raviY_printf(ls->fs, "funcstat -> declaring function %e\n", &v));
  body(ls, &b, ismethod, line);
  luaK_storevar(ls->fs, &v, &b);
  luaK_fixline(ls->fs, line);  /* definition "happens" in the first line */
}

/* parse function call with no returns or assignment statement - called from statement() */
static void exprstat (LexState *ls) {
  /* stat -> func | assignment */
  FuncState *fs = ls->fs;
  struct LHS_assign v;
  v.v.ravi_type = RAVI_TANY;
  suffixedexp(ls, &v.v);
  if (ls->t.token == '=' || ls->t.token == ',') { /* stat -> assignment ? */
    v.prev = NULL;
    assignment(ls, &v, 1);
  }
  else {  /* stat -> func */
    check_condition(ls, v.v.k == VCALL, "syntax error");
    SETARG_C(getinstruction(fs, &v.v), 1);  /* call statement uses no results */
  }
}

/* parse return statement - called from statement() */
static void retstat (LexState *ls) {
  /* stat -> RETURN [explist] [';'] */
  FuncState *fs = ls->fs;
  expdesc e = {.ravi_type = RAVI_TANY, .pc = -1};
  int first, nret; /* registers with returned values */
  if (block_follow(ls, 1) || ls->t.token == ';')
    first = nret = 0;  /* return no values */
  else {
    nret = explist(ls, &e);  /* optional return values */
    if (hasmultret(e.k)) {
      luaK_setmultret(fs, &e);
      if (e.k == VCALL && nret == 1) {  /* tail call? */
        SET_OPCODE(getinstruction(fs,&e), OP_TAILCALL);
        lua_assert(GETARG_A(getinstruction(fs,&e)) == fs->nactvar);
      }
      first = fs->nactvar;
      nret = LUA_MULTRET;  /* return all values */
    }
    else {
      if (nret == 1)  /* only one single value? */
        first = luaK_exp2anyreg(fs, &e);
      else {
        luaK_exp2nextreg(fs, &e);  /* values must go to the stack */
        first = fs->nactvar;  /* return all active values */
        lua_assert(nret == fs->freereg - first);
      }
    }
  }
  luaK_ret(fs, first, nret);
  testnext(ls, ';');  /* skip optional semicolon */
}

/* parse a statement */
static void statement (LexState *ls) {
  int line = ls->linenumber;  /* may be needed for error messages */
  enterlevel(ls);
  switch (ls->t.token) {
    case ';': {  /* stat -> ';' (empty statement) */
      luaX_next(ls);  /* skip ';' */
      break;
    }
    case TK_IF: {  /* stat -> ifstat */
      ifstat(ls, line);
      break;
    }
    case TK_WHILE: {  /* stat -> whilestat */
      whilestat(ls, line);
      break;
    }
    case TK_DO: {  /* stat -> DO block END */
      luaX_next(ls);  /* skip DO */
      block(ls);
      check_match(ls, TK_END, TK_DO, line);
      break;
    }
    case TK_FOR: {  /* stat -> forstat */
      forstat(ls, line);
      break;
    }
    case TK_REPEAT: {  /* stat -> repeatstat */
      repeatstat(ls, line);
      break;
    }
    case TK_FUNCTION: {  /* stat -> funcstat */
      funcstat(ls, line);
      break;
    }
    case TK_LOCAL: {  /* stat -> localstat */
      luaX_next(ls);  /* skip LOCAL */
      if (testnext(ls, TK_FUNCTION))  /* local function? */
        localfunc(ls);
      else
        localstat(ls);
      break;
    }
    case TK_DBCOLON: {  /* stat -> label */
      luaX_next(ls);  /* skip double colon */
      labelstat(ls, str_checkname(ls), line);
      break;
    }
    case TK_RETURN: {  /* stat -> retstat */
      luaX_next(ls);  /* skip RETURN */
      retstat(ls);
      break;
    }
    case TK_BREAK:   /* stat -> breakstat */
    case TK_GOTO: {  /* stat -> 'goto' NAME */
      gotostat(ls, luaK_jump(ls->fs));
      break;
    }
    default: {  /* stat -> func | assignment */
      exprstat(ls);
      break;
    }
  }
  lua_assert(ls->fs->f->maxstacksize >= ls->fs->freereg &&
             ls->fs->freereg >= ls->fs->nactvar);
  ls->fs->freereg = ls->fs->nactvar;  /* free registers */
  leavelevel(ls);
}

/* }====================================================================== */


/*
** compiles the main function, which is a regular vararg function with an
** upvalue named LUA_ENV
*/
static void mainfunc (LexState *ls, FuncState *fs) {
  BlockCnt bl;
  expdesc v = {.ravi_type = RAVI_TANY, .pc = -1};
  open_func(ls, fs, &bl);
  fs->f->is_vararg = 1;  /* main function is always declared vararg */
  init_exp(&v, VLOCAL, 0, RAVI_TANY, NULL);  /* create and... - RAVI TODO var arg is unknown type */
  newupvalue(fs, ls->envn, &v);  /* ...set environment upvalue */
  luaX_next(ls);  /* read first token */
  statlist(ls);  /* parse main body */
  check(ls, TK_EOS);
  close_func(ls);
}


LClosure *luaY_parser (lua_State *L, ZIO *z, Mbuffer *buff,
                       Dyndata *dyd, const char *name, int firstchar) {
  LexState lexstate;
  FuncState funcstate;
  LClosure *cl = luaF_newLclosure(L, 1);  /* create main closure */
  setclLvalue(L, L->top, cl);  /* anchor it (to avoid being collected) */
  luaD_inctop(L);
  lexstate.h = luaH_new(L);  /* create table for scanner */
  sethvalue(L, L->top, lexstate.h);  /* anchor it */
  luaD_inctop(L);
  funcstate.f = cl->p = luaF_newproto(L);
  funcstate.f->source = luaS_new(L, name);  /* create and anchor TString */
  lua_assert(iswhite(funcstate.f));  /* do not need barrier here */
  lexstate.buff = buff;
  lexstate.dyd = dyd;
  dyd->actvar.n = dyd->gt.n = dyd->label.n = 0;
  luaX_setinput(L, &lexstate, z, funcstate.f->source, firstchar);
  mainfunc(&lexstate, &funcstate);
  lua_assert(!funcstate.prev && funcstate.nups == 1 && !lexstate.fs);
  /* all scopes should be correctly finished */
  lua_assert(dyd->actvar.n == 0 && dyd->gt.n == 0 && dyd->label.n == 0);
  L->top--;  /* remove scanner's table */
  return cl;  /* closure is on the stack, too */
}


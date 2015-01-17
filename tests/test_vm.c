#ifdef _WIN32
// FIXME a hack 
#define LUA_BUILD_AS_DLL
#endif

#include "lprefix.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"

#define VOID(p)		((const void*)(p))

static void PrintString(const TString* ts)
{
  const char* s = getstr(ts);
  size_t i, n = ts->len;
  printf("%c", '"');
  for (i = 0; i<n; i++)
  {
    int c = (int)(unsigned char)s[i];
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
      printf("%c", c);
              else
                printf("\\%03d", c);
    }
  }
  printf("%c", '"');
}

static void PrintConstant(const Proto* f, int i)
{
  const TValue* o = &f->k[i];
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
    sprintf(buff, LUA_NUMBER_FMT, fltvalue(o));
    printf("%s", buff);
    if (buff[strspn(buff, "-0123456789")] == '\0') printf(".0");
    break;
  }
  case LUA_TNUMINT:
    printf(LUA_INTEGER_FMT, ivalue(o));
    break;
  case LUA_TSHRSTR: case LUA_TLNGSTR:
    PrintString(tsvalue(o));
    break;
  default:				/* cannot happen */
    printf("? type=%d", ttype(o));
    break;
  }
}

#define UPVALNAME(x) ((f->upvalues[x].name) ? getstr(f->upvalues[x].name) : "-")
#define MYK(x)		(-1-(x))

static void PrintRaviCode(const Proto* f, Instruction i, int pc)
{
  OpCode o = GET_OPCODE(i);
  int a = GETARG_A(i);
  int b = GETARG_B(i);
  int c = GETARG_C(i);
  int line = getfuncline(f, pc);
  printf("\t%d\t", pc + 1);
  if (line>0) printf("[%d]\t", line); else printf("[-]\t");
  printf("%-9s\t", luaP_opnames[o]);
  switch (getOpMode(o))
  {
  case iABC:
    printf("%d", a);
    if (getBMode(o) != OpArgN) printf(" %d", getBMode(o) == OpArgK ? (MYK(INDEXK(b))) : b);
    if (getCMode(o) != OpArgN) printf(" %d", getCMode(o) == OpArgK ? (MYK(INDEXK(c))) : c);
    break;
  }
  printf("\n");
}


static void PrintCode(const Proto* f)
{
  const Instruction* code = f->code;
  int pc, n = f->sizecode;
  for (pc = 0; pc<n; pc++)
  {
    Instruction i = code[pc];
    OpCode o = GET_OPCODE(i);
    if (o >= OP_RAVI_UNMF) {
      PrintRaviCode(f, i, pc);
      continue;
    }
    int a = GETARG_A(i);
    int b = GETARG_B(i);
    int c = GETARG_C(i);
    int ax = GETARG_Ax(i);
    int bx = GETARG_Bx(i);
    int sbx = GETARG_sBx(i);
    int line = getfuncline(f, pc);
    printf("\t%d\t", pc + 1);
    if (line>0) printf("[%d]\t", line); else printf("[-]\t");
    printf("%-9s\t", luaP_opnames[o]);
    switch (getOpMode(o))
    {
    case iABC:
      printf("%d", a);
      if (getBMode(o) != OpArgN) printf(" %d", ISK(b) ? (MYK(INDEXK(b))) : b);
      if (getCMode(o) != OpArgN) printf(" %d", ISK(c) ? (MYK(INDEXK(c))) : c);
      break;
    case iABx:
      printf("%d", a);
      if (getBMode(o) == OpArgK) printf(" %d", MYK(bx));
      if (getBMode(o) == OpArgU) printf(" %d", bx);
      break;
    case iAsBx:
      printf("%d %d", a, sbx);
      break;
    case iAx:
      printf("%d", MYK(ax));
      break;
    }
    switch (o)
    {
    case OP_LOADK:
      printf("\t; "); PrintConstant(f, bx);
      break;
    case OP_GETUPVAL:
    case OP_SETUPVAL:
      printf("\t; %s", UPVALNAME(b));
      break;
    case OP_GETTABUP:
      printf("\t; %s", UPVALNAME(b));
      if (ISK(c)) { printf(" "); PrintConstant(f, INDEXK(c)); }
      break;
    case OP_SETTABUP:
      printf("\t; %s", UPVALNAME(a));
      if (ISK(b)) { printf(" "); PrintConstant(f, INDEXK(b)); }
      if (ISK(c)) { printf(" "); PrintConstant(f, INDEXK(c)); }
      break;
    case OP_GETTABLE:
    case OP_SELF:
      if (ISK(c)) { printf("\t; "); PrintConstant(f, INDEXK(c)); }
      break;
    case OP_SETTABLE:
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_POW:
    case OP_DIV:
    case OP_IDIV:
    case OP_BAND:
    case OP_BOR:
    case OP_BXOR:
    case OP_SHL:
    case OP_SHR:
    case OP_EQ:
    case OP_LT:
    case OP_LE:
      if (ISK(b) || ISK(c))
      {
        printf("\t; ");
        if (ISK(b)) PrintConstant(f, INDEXK(b)); else printf("-");
        printf(" ");
        if (ISK(c)) PrintConstant(f, INDEXK(c)); else printf("-");
      }
      break;
    case OP_JMP:
    case OP_FORLOOP:
    case OP_FORPREP:
    case OP_TFORLOOP:
      printf("\t; to %d", sbx + pc + 2);
      break;
    case OP_CLOSURE:
      printf("\t; %p", VOID(f->p[bx]));
      break;
    case OP_SETLIST:
      if (c == 0) printf("\t; %d", (int)code[++pc]); else printf("\t; %d", c);
      break;
    case OP_EXTRAARG:
      printf("\t; "); PrintConstant(f, ax);
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
  const char* s = f->source ? getstr(f->source) : "=?";
  if (*s == '@' || *s == '=')
    s++;
  else if (*s == LUA_SIGNATURE[0])
    s = "(bstring)";
  else
    s = "(string)";
  printf("\n%s <%s:%d,%d> (%d instruction%s at %p)\n",
    (f->linedefined == 0) ? "main" : "function", s,
    f->linedefined, f->lastlinedefined,
    S(f->sizecode), VOID(f));
  printf("%d%s param%s, %d slot%s, %d upvalue%s, ",
    (int)(f->numparams), f->is_vararg ? "+" : "", SS(f->numparams),
    S(f->maxstacksize), S(f->sizeupvalues));
  printf("%d local%s, %d constant%s, %d function%s\n",
    S(f->sizelocvars), S(f->sizek), S(f->sizep));
}

static void PrintDebug(const Proto* f)
{
  int i, n;
  n = f->sizek;
  printf("constants (%d) for %p:\n", n, VOID(f));
  for (i = 0; i<n; i++)
  {
    printf("\t%d\t", i + 1);
    PrintConstant(f, i);
    printf("\n");
  }
  n = f->sizelocvars;
  printf("locals (%d) for %p:\n", n, VOID(f));
  for (i = 0; i<n; i++)
  {
    printf("\t%d\t%s\t%d\t%d\n",
      i, getstr(f->locvars[i].varname), f->locvars[i].startpc + 1, f->locvars[i].endpc + 1);
  }
  n = f->sizeupvalues;
  printf("upvalues (%d) for %p:\n", n, VOID(f));
  for (i = 0; i<n; i++)
  {
    printf("\t%d\t%s\t%d\t%d\n",
      i, UPVALNAME(i), f->upvalues[i].instack, f->upvalues[i].idx);
  }
}

static void PrintFunction(const Proto* f, int full)
{
  int i, n = f->sizep;
  PrintHeader(f);
  PrintCode(f);
  if (full) PrintDebug(f);
  for (i = 0; i<n; i++) PrintFunction(f->p[i], full);
}

#define toproto(L,i) getproto(L->top+(i))

static void DumpFunction(lua_State *L)
{
  Proto* f;
  f = toproto(L, -1);
  PrintFunction(f, 1);
}

typedef struct {
    lua_State *L;
    CallInfo *ci;
    Closure *cl;
    TValue *stack;
    Instruction *instructions;
    Proto *p;
} LuaHandle;

static LuaHandle *alloc_handle()
{
    LuaHandle *h = (LuaHandle *)calloc(1, sizeof(LuaHandle));
    h->L = (lua_State *)calloc(1, sizeof(lua_State));
    h->ci = (CallInfo *)calloc(10, sizeof(CallInfo));
    h->cl = (Closure *)calloc(1, sizeof(Closure));
    h->stack = (TValue *)calloc(10, sizeof(TValue));
    h->instructions = (Instruction *)calloc(10, sizeof(Instruction));
    h->p = (Proto *)calloc(1, sizeof(Proto));

    h->L->stack = h->stack;
    h->L->stacksize = 10;
    h->L->stack_last = h->L->stack + 10 - 1;
    h->p->code = &h->instructions[0];

    h->cl->l.p = h->p; /* Dummy proto */
    h->stack[0].value_.gc = (GCObject *)h->cl;  /* Dummy function at stack position 0 */
    h->ci->u.l.base = &h->stack[1]; /* base for this function */
    h->ci->func = &h->stack[0]; /* function index in the stack */
    h->ci->top = &h->stack[1]; /* top for this function, 1 past the function */
    h->ci->u.l.savedpc = h->cl->l.p->code;
    h->L->ci = h->ci;
    h->L->top = &h->stack[1]; 
    return h;
}

static void free_handle(LuaHandle *h)
{
    free(h->p);
    free(h->instructions);
    free(h->stack);
    free(h->cl);
    free(h->ci);
    free(h->L);
    free(h);
}

static int test_binintop(OpCode o, int p, int q, int expected)
{
    LuaHandle *h = alloc_handle();
    LuaHandle copyh = *h; /* save contents */
    int rc = 0;

    /* Here we simulate following
    * local p = x
    * local q = y
    * return x+y
    *
    * So we need three registers
    * p in register 0
    * q in register 1
    * return value must be register 2
    */

    int A = 2;
    int B = 1;
    int C = 0;
    int RETURN_ITEMS = 1;
    /* register A (2) will hold return value */
    /* register B (1) will hold q */
    /* register C (0) will hold p */
    h->instructions[0] = CREATE_ABC(o, A, B, C);
    /* return register 2 (A) */
    h->instructions[1] = CREATE_ABC(OP_RETURN, A, RETURN_ITEMS+1, 0);

    /* set register 0 (p) */
    setivalue(&h->stack[1], p);
    /* set register 1 (q) */
    setivalue(&h->stack[2], q);

    h->p->maxstacksize = 3; /* need three registers */
    h->ci->top = h->ci->u.l.base + h->p->maxstacksize;
    h->ci->nresults = 1;
    h->p->sizecode = 2;

    DumpFunction(h->L);
    h->L->top = h->ci->top;

    luaV_execute(h->L);

    if (h->L->top != &h->L->stack[1])
        rc = 1;
    if (h->L->ci != NULL)
        rc = 1;
    if (!ttisinteger(&h->stack[0]) ||
        ivalue(&h->stack[0]) != expected)
        rc = 1;

    *h = copyh; /* restore contents */
    free_handle(h);
    return rc;
}

static int test_unmf()
{
    LuaHandle *h = alloc_handle();
    LuaHandle copyh = *h; /* save contents */
    int rc = 0;

    /* Here we simulate following
     * local p = 56.65
     * local q = -p
     * return q
     * 
     * So we need two registers
     * p in register 0
     * q in register 1
     * return value must be register 1
     */

    /* register 1 holds q, so A = 1 */
    /* register 0 holds p, so B = 0 */
    h->instructions[0] = CREATE_ABC(OP_RAVI_UNMF, 1, 0, 0);
    /* return register 1 (q) */
    h->instructions[1] = CREATE_ABC(OP_RETURN, 1, 2, 0);

    /* set register 0 (p) */
    h->stack[1].tt_ = LUA_TNUMFLT;
    h->stack[1].value_.n = 56.65;

    h->p->maxstacksize = 2; /* need two registers */
    h->ci->top = h->ci->u.l.base + h->p->maxstacksize;
    h->ci->nresults = 1;
    h->p->sizecode = 2;
    DumpFunction(h->L);
    h->L->top = h->ci->top;
    luaV_execute(h->L);

    if (h->L->top != &h->L->stack[1])
        rc = 1;
    if (h->L->ci != NULL)
        rc = 1;
    if (h->stack[0].tt_ != LUA_TNUMFLT ||
        h->stack[0].value_.n != -56.65)
        rc = 1;

    *h = copyh; /* restore contents */
    free_handle(h);
    return rc;
}


static int test_return1()
{
    LuaHandle *h = alloc_handle();
    LuaHandle copyh = *h; /* save contents */
    int rc = 0;

    /* just a return instruction - B = 1 means no return values */
    h->instructions[0] = CREATE_ABC(OP_RETURN, 0, 2, 0);
    h->stack[1].tt_ = LUA_TNUMFLT;
    h->stack[1].value_.n = 56.65;
    h->ci->nresults = 1;
    h->p->sizecode = 1;
    DumpFunction(h->L);

    h->L->top++;
    luaV_execute(h->L);

    if (h->L->top != &h->L->stack[1])
        rc = 1;
    if (h->L->ci != NULL)
        rc = 1;
    if (h->stack[0].tt_ != LUA_TNUMFLT ||
        h->stack[0].value_.n != 56.65)
        rc = 1;

    *h = copyh; /* restore contents */
    free_handle(h);
    return rc;
}

static int test_return0()
{
    LuaHandle *h = alloc_handle();
    LuaHandle copyh = *h; /* save contents */
    int rc = 0;

    /* just a return instruction - B = 1 means no return values */
    h->instructions[0] = CREATE_ABC(OP_RETURN, 0, 1, 0);
    luaV_execute(h->L);

    if (h->L->top != h->L->stack)
        rc = 1;
    if (h->L->ci != NULL)
        rc = 1;

    *h = copyh; /* restore contents */
    free_handle(h);
    return rc;
}

/* test supplied lua code compiles */
static int test_luacomp1(const char *code)
{
    int rc = 0;
    lua_State *L;
    L = luaL_newstate();
    if (luaL_loadbuffer(L, code, strlen(code), "testfunc") != 0) {
      rc = 1;
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
      lua_pop(L, 1);  /* pop error message from the stack */
    }
    else
      DumpFunction(L);
    lua_close(L);
    return rc;
}

/* test supplied lua code compiles */
static int test_luacompexec1(const char *code, int expected)
{
  int rc = 0;
  lua_State *L;
  L = luaL_newstate();
  if (luaL_loadbuffer(L, code, strlen(code), "testfunc") != 0) {
    rc = 1;
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    lua_pop(L, 1);  /* pop error message from the stack */
  }
  else {
    DumpFunction(L);
    time_t start = time(NULL);
    if (lua_pcall(L, 0, 1, 0) != 0) {
      rc = 1;
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
    }
    else {
      time_t end = time(NULL);
      stackDump(L, "after executing function");
      printf("time taken = %f\n", difftime(end, start));
      lua_Integer got = 0;
      if (lua_isboolean(L, -1))
        got = lua_toboolean(L, -1) ? 1 : 0;
      else
        got = lua_tointeger(L, -1);
      if (got != expected) {
        rc = 1;
      }
    }
  }
  lua_close(L);
  return rc;
}



int main(const char *argv[]) 
{
    int failures = 0;
    failures += test_luacomp1("local i:int, d:double = f()");
    failures += test_luacomp1("local i:int,j:double,k:int = f(), g()");
    failures += test_luacomp1("local f = function(); return; end; local d:double, j:int = f(); return d");
    failures += test_luacomp1("local d = f()");
    failures += test_luacomp1("local d, e; d, e = f(), g()");
    failures += test_return0();
    failures += test_return1();
    failures += test_unmf();
    failures += test_binintop(OP_RAVI_ADDIIRR, 6, 7, 13);
    failures += test_binintop(OP_RAVI_MULIIRR, 6, 7, 42);
    failures += test_luacomp1("return (-1.25 or -4)+0");
    failures += test_luacomp1("f = nil; local f; function f(a); end");
    failures += test_luacomp1("local max, min = 0x7fffffff, -0x80000000; assert(string.format(\"%d\", min) == \"-2147483648\"); max, min = 0x7fffffffffffffff, -0x8000000000000000; if max > 2.0 ^ 53 then; end;");
    failures += test_luacomp1("local function F (m); local function round(m); m = m + 0.04999; return format(\"%.1f\", m);end; end");
    failures += test_luacomp1("local b:int = 6; local i:int = 5+b; return i");
    failures += test_luacompexec1("local b:int = 6; local i:int = 5+b; return i", 11);
    failures += test_luacomp1("local f = function(); end");
    failures += test_luacomp1("local b:int = 6; b = nil; return i") == RAVI_ENABLED ? 0 : 1; /* should fail */
    failures += test_luacomp1("local f = function(); local function y() ; end; end");
    failures += test_luacompexec1("return -(1 or 2)", -1);
    failures += test_luacompexec1("return (1 and 2)+(-1.25 or -4) == 0.75", 1);
    failures += test_luacomp1("local a=1; if a==0 then; a = 2; else a=3; end;");
    failures += test_luacomp1("local f = function(); return; end; local d:double = 5.0; d = f(); return d") == RAVI_ENABLED ? 0 : 1;
    failures += test_luacompexec1("local i, j:double; j=0.0; for i=1,1000000000 do; j = j+1; end; return j", 1000000000);
    failures += test_luacompexec1("local i, j:int; j=0; for i=1,1000000000 do; j = j+1; end; return j", 1000000000);
    failures += test_luacomp1("local f = function(); return; end; local d = 5.0; d = f(); return d");
    printf("Number of opcodes %d\n", NUM_OPCODES);
    printf("LUA_TNUMFLT = %d\n", LUA_TNUMFLT);
    printf("LUA_TNUMINT = %d\n", LUA_TNUMINT);

    return failures ? 1 : 0;
}
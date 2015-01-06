#define lvm_c
#define LUA_CORE

#include "lprefix.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"

#include <stdlib.h>

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
    h->instructions[0] = RAVI_CREATE_A(o, A);
    /* register B (1) will hold q */
    /* register C (0) will hold p */
    h->instructions[1] = RAVI_CREATE_BC(B, C);
    /* return register 2 (A) */
    h->instructions[2] = CREATE_ABC(OP_RETURN, A, RETURN_ITEMS+1, 0);

    /* set register 0 (p) */
    setivalue(&h->stack[1], p);
    /* set register 1 (q) */
    setivalue(&h->stack[2], q);

    h->p->maxstacksize = 3; /* need three registers */
    h->ci->top = h->ci->u.l.base + h->p->maxstacksize;
    h->L->top = h->ci->top;
    h->ci->nresults = 1;
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

    printf("Number of opcodes %d\n", NUM_OPCODES);

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
    h->instructions[0] = RAVI_CREATE_A(OP_RAVI_UNMF, 1);
    h->instructions[1] = RAVI_CREATE_BC(0, 0);
    /* return register 1 (q) */
    h->instructions[2] = CREATE_ABC(OP_RETURN, 1, 2, 0);

    /* set register 0 (p) */
    h->stack[1].tt_ = LUA_TNUMFLT;
    h->stack[1].value_.n = 56.65;

    h->p->maxstacksize = 2; /* need two registers */
    h->ci->top = h->ci->u.l.base + h->p->maxstacksize;
    h->L->top = h->ci->top;
    h->ci->nresults = 1;
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
    h->L->top++;
    h->ci->nresults = 1;
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


int main(const char *argv[]) 
{
    int failures = 0;
    failures += test_return0();
    failures += test_return1();
    failures += test_unmf();
    failures += test_binintop(OP_RAVI_ADDIIRR, 6, 7, 13);
    failures += test_binintop(OP_RAVI_MULIIRR, 6, 7, 42);


    return failures ? 1 : 0;
}
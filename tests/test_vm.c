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

    /* R(1) = -R(0) */
    /* register 1 holds q */
    /* register 0 holds p */
    h->instructions[0] = CREATE_ABC(OP_UNMF, 1, 0, 0);
    /* return register 1 (q) */
    h->instructions[1] = CREATE_ABC(OP_RETURN, 1, 2, 0);

    if (GETARG_A(h->instructions[0]) != 1)
        return 1;
    if (GETARG_B(h->instructions[0]) != 0)
        return 1;

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

    return failures ? 1 : 0;
}
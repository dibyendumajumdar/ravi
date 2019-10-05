/*
** $Id: lfunc.c,v 2.45 2014/11/02 19:19:04 roberto Exp $
** Auxiliary functions to manipulate prototypes and closures
** See Copyright Notice in lua.h
*/

/*
** Portions Copyright (C) 2015-2017 Dibyendu Majumdar
*/


#define lfunc_c
#define LUA_CORE

#include "lprefix.h"


#include <stddef.h>

#include "lua.h"

#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"

#include "ravijit.h"

CClosure *luaF_newCclosure (lua_State *L, int n) {
  GCObject *o = luaC_newobj(L, LUA_TCCL, sizeCclosure(n));
  CClosure *c = gco2ccl(o);
  c->nupvalues = cast_byte(n);
  return c;
}


LClosure *luaF_newLclosure (lua_State *L, int n) {
  GCObject *o = luaC_newobj(L, LUA_TLCL, sizeLclosure(n));
  LClosure *c = gco2lcl(o);
  c->p = NULL;
  c->nupvalues = cast_byte(n);
  while (n--) c->upvals[n] = NULL;
  return c;
}

/*
** fill a closure with new closed upvalues
*/
void luaF_initupvals (lua_State *L, LClosure *cl) {
  int i;
  for (i = 0; i < cl->nupvalues; i++) {
    UpVal *uv = luaM_new(L, UpVal);
    uv->refcount = 1;
    uv->v = &uv->u.value;  /* make it closed */
    setnilvalue(uv->v);
    cl->upvals[i] = uv;
  }
}


UpVal *luaF_findupval (lua_State *L, StkId level) {
  UpVal **pp = &L->openupval;
  UpVal *p;
  UpVal *uv;
  lua_assert(isintwups(L) || L->openupval == NULL);
  while (*pp != NULL && (p = *pp)->v >= level) {
    lua_assert(upisopen(p));
    if (p->v == level && !p->flags)  /* found a corresponding upvalue that is not a deferred value? */ {
      return p; /* return it */
    }
    pp = &p->u.open.next;
  }
  /* not found: create a new upvalue */
  uv = luaM_new(L, UpVal);
  uv->refcount = 0;
  uv->flags = 0;
  uv->u.open.next = *pp;  /* link it to list of open upvalues */
  uv->u.open.touched = 1;
  *pp = uv;
  uv->v = level;  /* current value lives in the stack */
  if (!isintwups(L)) {  /* thread not in list of threads with upvalues? */
    L->twups = G(L)->twups;  /* link it to the list */
    G(L)->twups = L;
  }
  return uv;
}

static void callclose (lua_State *L, void *ud) {
  luaD_callnoyield(L, cast(StkId, ud), 0);
}

static int calldeferredfunction (lua_State *L, UpVal *uv, StkId level, int status) {
  StkId func = level + 1;  /* save slot for old error message */
  setobj2s(L, func, uv->v);  /* will call it */
  if (status != LUA_OK)  /* was there an error? */
    luaD_seterrorobj(L, status, level);  /* save error message */
  else
    setnilvalue(level);
  setobjs2s(L, func + 1, level);  /* error msg. as argument */
  L->top = func + 2;  /* add function and argument */
  if (status == LUA_OK)  /* not in "error mode"? */
    callclose(L, func);  /* call closing method */
  else {  /* already inside error handler; cannot raise another error */
    int newstatus = luaD_pcall(L, callclose, func, savestack(L, level), 0);
    if (newstatus != LUA_OK)  /* error when closing? */
      status = newstatus;  /* this will be the new error */
  }
  return status;
}

int luaF_close (lua_State *L, StkId level, int status) {
  UpVal *uv;
  while (L->openupval != NULL && (uv = L->openupval)->v >= level) {
    lua_assert(upisopen(uv));
    L->openupval = uv->u.open.next;  /* remove from 'open' list */
    if (uv->refcount == 0) {         /* no references? */
      if (uv->flags && ttisfunction(uv->v)) {
        ptrdiff_t levelrel = savestack(L, level);
        status = calldeferredfunction(L, uv, uv->v, status);
        level = restorestack(L, levelrel);
      }
      luaM_free(L, uv);              /* free upvalue */
    }
    else {
      setobj(L, &uv->u.value, uv->v);  /* move value to upvalue slot */
      uv->v = &uv->u.value;  /* now current value lives here */
      luaC_upvalbarrier(L, uv);
    }
  }
  return status;
}


Proto *luaF_newproto (lua_State *L) {
  GCObject *o = luaC_newobj(L, LUA_TPROTO, sizeof(Proto));
  Proto *f = gco2p(o);
  f->k = NULL;
  f->sizek = 0;
  f->p = NULL;
  f->sizep = 0;
  f->code = NULL;
  f->cache = NULL;
  f->sizecode = 0;
  f->lineinfo = NULL;
  f->sizelineinfo = 0;
  f->upvalues = NULL;
  f->sizeupvalues = 0;
  f->numparams = 0;
  f->is_vararg = 0;
  f->maxstacksize = 0;
  f->locvars = NULL;
  f->sizelocvars = 0;
  f->linedefined = 0;
  f->lastlinedefined = 0;
  f->source = NULL;
  f->ravi_jit.jit_data = NULL;
  f->ravi_jit.jit_function = NULL;
  f->ravi_jit.jit_status = RAVI_JIT_NOT_COMPILED; /* not compiled */
  f->ravi_jit.jit_flags = RAVI_JIT_FLAG_NONE; /* no loop */
  f->ravi_jit.execution_count = 0; /* number of times function execution (capped) */
  return f;
}


void luaF_freeproto (lua_State *L, Proto *f) {
  raviV_freeproto(L, f);
  luaM_freearray(L, f->code, f->sizecode);
  luaM_freearray(L, f->p, f->sizep);
  luaM_freearray(L, f->k, f->sizek);
  luaM_freearray(L, f->lineinfo, f->sizelineinfo);
  luaM_freearray(L, f->locvars, f->sizelocvars);
  luaM_freearray(L, f->upvalues, f->sizeupvalues);
  luaM_free(L, f);
}


/*
** Look for n-th local variable at line 'line' in function 'func'.
** Returns NULL if not found.
** RAVI extension - also return the known type if any
*/
const char *luaF_getlocalname (const Proto *f, int local_number, int pc, ravitype_t *type) {
  int i;
  for (i = 0; i<f->sizelocvars && f->locvars[i].startpc <= pc; i++) {
    if (pc < f->locvars[i].endpc) {  /* is variable active? */
      local_number--;
      if (local_number == 0) {
        if (f->locvars[i].varname == NULL)
          break;
        *type = f->locvars[i].ravi_type;
        return getstr(f->locvars[i].varname);
      }
    }
  }
  *type = RAVI_TANY;
  return NULL;  /* not found */
}



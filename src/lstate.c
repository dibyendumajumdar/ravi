/*
** $Id: lstate.c,v 2.133.1.1 2017/04/19 17:39:34 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/

#define lstate_c
#define LUA_CORE

#include "lprefix.h"


#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "lua.h"

#include "lapi.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "llex.h"
#include "lmem.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"

#include "ravi_jit.h"
#include "ravi_profile.h"
#include "ravi_alloc.h"


/*
** a macro to help the creation of a unique random seed when a state is
** created; the seed is used to randomize hashes.
*/
#if !defined(luai_makeseed)
#include <time.h>
#define luai_makeseed()		cast(unsigned int, time(NULL))
#endif



/*
** thread state + extra space
*/
typedef struct LX {
  lu_byte extra_[LUA_EXTRASPACE];
  lua_State l;
} LX;


/*
** Main thread combines a thread state and the global state
*/
typedef struct LG {
  LX l;
  global_State g;
} LG;



#define fromstate(L)	(cast(LX *, cast(lu_byte *, (L)) - offsetof(LX, l)))


/*
** Compute an initial seed as random as possible. Rely on Address Space
** Layout Randomization (if present) to increase randomness..
*/
#define addbuff(b,p,e) \
  { size_t t = cast(size_t, e); \
    memcpy(b + p, &t, sizeof(t)); p += sizeof(t); }

static unsigned int makeseed (lua_State *L) {
  char buff[4 * sizeof(size_t)];
  unsigned int h = luai_makeseed();
  int p = 0;
  addbuff(buff, p, L);  /* heap variable */
  addbuff(buff, p, &h);  /* local variable */
  addbuff(buff, p, luaO_nilobject);  /* global variable */
  addbuff(buff, p, &lua_newstate);  /* public function */
  lua_assert(p == sizeof(buff));
  return luaS_hash(buff, p, h);
}


/*
** set GCdebt to a new value keeping the value (totalbytes + GCdebt)
** invariant (and avoiding underflows in 'totalbytes')
*/
void luaE_setdebt (global_State *g, l_mem debt) {
  l_mem tb = gettotalbytes(g);
  lua_assert(tb > 0);
  if (debt < tb - MAX_LMEM)
    debt = tb - MAX_LMEM;  /* will make 'totalbytes == MAX_LMEM' */
  g->totalbytes = tb - debt;
  g->GCdebt = debt;
}


CallInfo *luaE_extendCI (lua_State *L) {
  CallInfo *ci = luaM_new(L, CallInfo);
  ci->jitstatus = 0;
  ci->stacklevel = L->ci->stacklevel+1;
  lua_assert(L->ci->next == NULL);
  L->ci->next = ci;
  ci->previous = L->ci;
  ci->next = NULL;
  ci->magic = 42;
  L->nci++;
  return ci;
}


/*
** free all CallInfo structures not in use by a thread
*/
void luaE_freeCI (lua_State *L) {
  CallInfo *ci = L->ci;
  CallInfo *next = ci->next;
  ci->next = NULL;
  while ((ci = next) != NULL) {
    next = ci->next;
    luaM_free(L, ci);
    L->nci--;
  }
}


/*
** free half of the CallInfo structures not in use by a thread
*/
void luaE_shrinkCI (lua_State *L) {
  CallInfo *ci = L->ci;
  CallInfo *next2;  /* next's next */
  /* while there are two nexts */
  while (ci->next != NULL && (next2 = ci->next->next) != NULL) {
    luaM_free(L, ci->next);  /* free next */
    L->nci--;
    ci->next = next2;  /* remove 'next' from the list */
    next2->previous = ci;
    ci = next2;  /* keep next's next */
  }
}


static void stack_init (lua_State *L1, lua_State *L) {
  int i; CallInfo *ci;
  /* initialize stack array */
  L1->stack = luaM_newvector(L, BASIC_STACK_SIZE, TValue);
  L1->stacksize = BASIC_STACK_SIZE;
  for (i = 0; i < BASIC_STACK_SIZE; i++)
    setnilvalue(L1->stack + i);  /* erase new stack */
  L1->top = L1->stack;
  L1->stack_last = L1->stack + L1->stacksize - EXTRA_STACK;
  /* initialize first ci */
  ci = &L1->base_ci;
  ci->next = ci->previous = NULL;
  ci->callstatus = 0;
  ci->jitstatus = 0; /* RAVI extension */
  ci->magic = 42; /* RAVI extension */
  ci->func = L1->top;
  setnilvalue(L1->top++);  /* 'function' entry for this 'ci' */
  ci->top = L1->top + LUA_MINSTACK;
  L1->ci = ci;
}


static void freestack (lua_State *L) {
  if (L->stack == NULL)
    return;  /* stack not completely built yet */
  L->ci = &L->base_ci;  /* free the entire 'ci' list */
  luaE_freeCI(L);
  lua_assert(L->nci == 0);
  luaM_freearray(L, L->stack, L->stacksize);  /* free stack array */
}


/*
** Create registry table and its predefined values
*/
static void init_registry (lua_State *L, global_State *g) {
  TValue temp;
  /* create registry */
  Table *registry = luaH_new(L);
  sethvalue(L, &g->l_registry, registry);
  luaH_resize(L, registry, LUA_RIDX_LAST, 0);
  /* registry[LUA_RIDX_MAINTHREAD] = L */
  setthvalue(L, &temp, L);  /* temp = L */
  luaH_setint(L, registry, LUA_RIDX_MAINTHREAD, &temp);
  /* registry[LUA_RIDX_GLOBALS] = table of globals */
  sethvalue(L, &temp, luaH_new(L));  /* temp = new table (global table) */
  luaH_setint(L, registry, LUA_RIDX_GLOBALS, &temp);
}


/*
** open parts of the state that may cause memory-allocation errors.
** ('g->version' != NULL flags that the state was completely build)
*/
static void f_luaopen (lua_State *L, void *ud) {
  global_State *g = G(L);
  UNUSED(ud);
  stack_init(L, L);  /* init stack */
  init_registry(L, g);
  luaS_init(L);
  luaT_init(L);
  luaX_init(L);
  g->gcstp = 0;  /* allow gc */
  g->version = lua_version(NULL);
  luai_userstateopen(L);
}


/*
** preinitialize a thread with consistent values without allocating
** any memory (to avoid errors)
*/
static void preinit_thread (lua_State *L, global_State *g) {
  G(L) = g;
  L->stack = NULL;
  L->ci = NULL;
  L->nci = 0;
  L->stacksize = 0;
  L->twups = L;  /* thread has no upvalues */
  L->errorJmp = NULL;
  L->nCcalls = 0;
  L->hook = NULL;
  L->hookmask = 0;
  L->basehookcount = 0;
  L->allowhook = 1;
  resethookcount(L);
  L->openupval = NULL;
  L->nny = 1;
  L->status = LUA_OK;
  L->errfunc = 0;
  L->base_ci.stacklevel = 0; /* RAVI base stack level */
  L->base_ci.magic = 42;
  L->magic = 42; /* RAVI extension */
}

void *ravi_alloc_f(void *msp, void *ptr, size_t osize, size_t nsize)
{
  (void)osize;
  if (nsize == 0) {
    mspace_free(msp, ptr);
    return NULL;
  } else if (ptr == NULL) {
    return mspace_malloc(msp, nsize);
  } else {
    return mspace_realloc(msp, ptr, nsize);
  }
}


static void close_state (lua_State *L) {
  global_State *g = G(L);
#ifdef RAVI_DEFER_STATEMENT
  luaF_close(L, L->stack, CLOSEPROTECT);  /* close all upvalues for this thread */
#else
  luaF_close(L, L->stack);  /* close all upvalues for this thread */
#endif
  luaC_freeallobjects(L);  /* collect all objects */
  if (g->version)  /* closing a fully built state? */
    luai_userstateclose(L);
  luaM_freearray(L, G(L)->strt.hash, G(L)->strt.size);
  freestack(L);
  lua_assert(gettotalbytes(g) == sizeof(LG));
  raviV_close(L);
  if (g->frealloc == ravi_alloc_f) /* Using LuaJIt allocator? */
    destroy_mspace(g->ud);
  else
    (*g->frealloc)(g->ud, fromstate(L), sizeof(LG), 0);  /* free main block */
}


LUA_API lua_State *lua_newthread (lua_State *L) {
  global_State *g;
  lua_State *L1;
  lua_lock(L);
  g = G(L);
  luaC_checkGC(L);
  /* create new thread */
  L1 = &cast(LX *, luaM_newobject(L, LUA_TTHREAD, sizeof(LX)))->l;
  L1->marked = luaC_white(g);
  L1->tt = LUA_TTHREAD;
  /* link it on list 'allgc' */
  L1->next = g->allgc;
  g->allgc = obj2gco(L1);
  /* anchor it on L stack */
  setthvalue(L, L->top, L1);
  api_incr_top(L);
  preinit_thread(L1, g);
  L1->hookmask = L->hookmask;
  L1->basehookcount = L->basehookcount;
  L1->hook = L->hook;
  resethookcount(L1);
  /* initialize L1 extra space */
  memcpy(lua_getextraspace(L1), lua_getextraspace(g->mainthread),
         LUA_EXTRASPACE);
  luai_userstatethread(L, L1);
  stack_init(L1, L);  /* init stack */
  lua_unlock(L);
  return L1;
}


void luaE_freethread (lua_State *L, lua_State *L1) {
  LX *l = fromstate(L1);
#ifdef RAVI_DEFER_STATEMENT
  luaF_close(L1, L1->stack, NOCLOSINGMETH);  /* close all upvalues for this thread */
#else
  luaF_close(L1, L1->stack);  /* close all upvalues for this thread */
#endif
  lua_assert(L1->openupval == NULL);
  luai_userstatefree(L, L1);
  freestack(L1);
  luaM_free(L, l);
}

#ifdef RAVI_DEFER_STATEMENT
int lua_resetthread (lua_State *L) {
  CallInfo *ci;
  int status;
  lua_lock(L);
  L->ci = ci = &L->base_ci;  /* unwind CallInfo list */
  setnilvalue(L->stack);  /* 'function' entry for basic 'ci' */
  ci->func = L->stack;
  ci->callstatus = 0;
  status = luaF_close(L, L->stack, CLOSEPROTECT);
  if (status != CLOSEPROTECT)  /* real errors? */
    luaD_seterrorobj(L, status, L->stack + 1);
  else {
    status = LUA_OK;
    L->top = L->stack + 1;
  }
  ci->top = L->top + LUA_MINSTACK;
  L->status = status;
  lua_unlock(L);
  return status;
}
#endif

/* TODO following should probably not live here*/

static void raviE_default_writestring(const char *s, size_t l) {
  fwrite(s, sizeof(char), l, stdout);
}

static void raviE_default_writeline(void) {
  fwrite("\n", sizeof(char), 1, stdout);
  fflush(stdout);
}

void raviE_default_writestringerror(const char *fmt, const char *p) {
  fprintf(stderr, fmt, p);
  fflush(stderr);
}

LUA_API lua_State *lua_newstate (lua_Alloc f, void *ud) {
  int i;
  lua_State *L;
  global_State *g;

  LG *l = cast(LG *, (*f)(ud, NULL, LUA_TTHREAD, sizeof(LG)));
  if (l == NULL) return NULL;
  L = &l->l.l;
  g = &l->g;
  g->ravi_writeline = raviE_default_writeline;
  g->ravi_writestring = raviE_default_writestring;
  g->ravi_writestringerror = raviE_default_writestringerror;
  g->ravi_debugger_data = NULL;
  L->next = NULL;
  L->tt = LUA_TTHREAD;
  g->currentwhite = bitmask(WHITE0BIT);
  L->marked = luaC_white(g);
  preinit_thread(L, g);
  g->allgc = obj2gco(L);  /* by now, only object is the main thread */
  L->next = NULL;
  g->frealloc = f;
  g->ud = ud;
  g->mainthread = L;
  g->seed = makeseed(L);
  g->gcstp = GCSTPGC;  /* no GC while building state */
  g->strt.size = g->strt.nuse = 0;
  g->strt.hash = NULL;
  setnilvalue(&g->l_registry);
  g->panic = NULL;
  g->version = NULL;
  g->gcstate = GCSpause;
  g->gckind = KGC_INC;
  g->gcstopem = 0;
  g->gcemergency = 0;
  g->finobj = g->tobefnz = g->fixedgc = NULL;
  g->firstold1 = g->survival = g->old1 = g->reallyold = NULL;
  g->finobjsur = g->finobjold1 = g->finobjrold = NULL;
  g->sweepgc = NULL;
  g->gray = g->grayagain = NULL;
  g->weak = g->ephemeron = g->allweak = NULL;
  g->twups = NULL;
  g->totalbytes = sizeof(LG);
  g->GCdebt = 0;
  g->lastatomic = 0;
  setgcparam(g->gcpause, LUAI_GCPAUSE);
  setgcparam(g->gcstepmul, LUAI_GCMUL);
  g->gcstepsize = LUAI_GCSTEPSIZE;
  setgcparam(g->genmajormul, LUAI_GENMAJORMUL);
  g->genminormul = LUAI_GENMINORMUL;
  for (i=0; i < LUA_NUMTAGS; i++) g->mt[i] = NULL;
  g->ravi_state = NULL;
  raviV_initjit(L);
  if (luaD_rawrunprotected(L, f_luaopen, NULL) != LUA_OK) {
    /* memory allocation error: free partial state */
    close_state(L);
    L = NULL;
  }
#if RAVI_BYTECODE_PROFILING_ENABLED
  raviV_init_profiledata(L);
#endif
  return L;
}


LUA_API void lua_close (lua_State *L) {
  L = G(L)->mainthread;  /* only the main thread can be closed */
  lua_lock(L);
#if RAVI_BYTECODE_PROFILING_ENABLED
  raviV_destroy_profiledata(L);
#endif
  close_state(L);
}



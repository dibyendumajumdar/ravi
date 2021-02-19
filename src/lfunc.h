/*
** $Id: lfunc.h,v 2.15.1.1 2017/04/19 17:39:34 roberto Exp $
** Auxiliary functions to manipulate prototypes and closures
** See Copyright Notice in lua.h
*/

#ifndef lfunc_h
#define lfunc_h


#include "lobject.h"


#define sizeCclosure(n)	(cast(int, sizeof(CClosure)) + \
                         cast(int, sizeof(TValue)*((n)-1)))

#define sizeLclosure(n)	(cast(int, sizeof(LClosure)) + \
                         cast(int, sizeof(TValue *)*((n)-1)))


/* test whether thread is in 'twups' list */
#define isintwups(L)	(L->twups != L)


/*
** maximum number of upvalues in a closure (both C and Lua). (Value
** must fit in a VM register.)
*/
#define MAXUPVAL	125
/* RAVI change; #define MAXUPVAL	255 */


#define upisopen(up)	((up)->v != &(up)->u.value)

#ifdef RAVI_DEFER_STATEMENT
/*
** Special "status" for 'luaF_close'
*/

/* close upvalues without running their closing methods */
#define NOCLOSINGMETH	(-1)

/* close upvalues running all closing methods in protected mode */
#define CLOSEPROTECT	(-2)
#endif

LUAI_FUNC Proto *luaF_newproto (lua_State *L);
LUAI_FUNC CClosure *luaF_newCclosure (lua_State *L, int nelems);
LUAI_FUNC LClosure *luaF_newLclosure (lua_State *L, int nelems);
LUAI_FUNC void luaF_initupvals (lua_State *L, LClosure *cl);
LUAI_FUNC UpVal *luaF_findupval (lua_State *L, StkId level);
#ifdef RAVI_DEFER_STATEMENT
LUAI_FUNC int luaF_close (lua_State *L, StkId level, int status);
#else
LUAI_FUNC void luaF_close (lua_State *L, StkId level);
#endif
LUAI_FUNC void luaF_freeproto (lua_State *L, Proto *f);
/* The additional type argument is a Ravi extension */
LUAI_FUNC const char *luaF_getlocalname (const Proto *func, int local_number,
                                         int pc, ravi_type_map* type, TString **usertype);


#endif

/*
** $Id: lvm.h,v 2.41 2016/12/22 13:08:50 roberto Exp $
** Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef lvm_h
#define lvm_h


#include "ldo.h"
#include "lobject.h"
#include "ltm.h"


#if !defined(LUA_NOCVTN2S)
#define cvt2str(o)	ttisnumber(o)
#else
#define cvt2str(o)	0	/* no conversion from numbers to strings */
#endif


#if !defined(LUA_NOCVTS2N)
#define cvt2num(o)	ttisstring(o)
#else
#define cvt2num(o)	0	/* no conversion from strings to numbers */
#endif


/*
** You can define LUA_FLOORN2I if you want to convert floats to integers
** by flooring them (instead of raising an error if they are not
** integral values)
*/
#if !defined(LUA_FLOORN2I)
#define LUA_FLOORN2I		0
#endif


#define tonumber(o,n) \
  (RAVI_LIKELY(ttisfloat(o)) ? (*(n) = fltvalue(o), 1) : luaV_tonumber_(o,n))

#define tointeger(o,i) \
  (RAVI_LIKELY(ttisinteger(o)) ? (*(i) = ivalue(o), 1) : luaV_tointeger(o,i,LUA_FLOORN2I))

#define intop(op,v1,v2) l_castU2S(l_castS2U(v1) op l_castS2U(v2))

#define luaV_rawequalobj(t1,t2)		luaV_equalobj(NULL,t1,t2)


/*
** fast track for 'gettable': if 't' is a table and 't[k]' is not nil,
** return 1 with 'slot' pointing to 't[k]' (final result).  Otherwise,
** return 0 (meaning it will have to check metamethod) with 'slot'
** pointing to a nil 't[k]' (if 't' is a table) or NULL (otherwise).
** 'f' is the raw get function to use.
*/
#define luaV_fastget(L,t,k,slot,f) \
  (!ttistable(t)  \
   ? (slot = NULL, 0)  /* not a table; 'slot' is NULL and result is 0 */  \
   : (slot = f(hvalue(t), k),  /* else, do raw access */  \
      !ttisnil(slot)))  /* result not nil? */

/*
** standard implementation for 'gettable'
** RAVI change - renamed as we need luaV_gettable to be 
** an exported function
*/
#define luaV_fastgettable(L,t,k,v) { const TValue *slot; \
  if (luaV_fastget(L,t,k,slot,luaH_get)) { setobj2s(L, v, slot); } \
  else luaV_finishget(L,t,k,v,slot); }


/*
** Fast track for set table. If 't' is a table and 't[k]' is not nil,
** call GC barrier, do a raw 't[k]=v', and return true; otherwise,
** return false with 'slot' equal to NULL (if 't' is not a table) or
** 'nil'. (This is needed by 'luaV_finishget'.) Note that, if the macro
** returns true, there is no need to 'invalidateTMcache', because the
** call is not creating a new entry.
*/
#define luaV_fastset(L,t,k,slot,f,v) \
  (!ttistable(t) \
   ? (slot = NULL, 0) \
   : (slot = f(hvalue(t), k), \
     ttisnil(slot) ? 0 \
     : (luaC_barrierback(L, hvalue(t), v), \
        setobj2t(L, cast(TValue *,slot), v), \
        1)))

/*
** RAVI change - renamed as we need luaV_settable to be 
** an exported function
*/
#define luaV_fastsettable(L,t,k,v) { const TValue *slot; \
  if (!luaV_fastset(L,t,k,slot,luaH_get,v)) \
    luaV_finishset(L,t,k,v,slot); }
  


LUAI_FUNC int luaV_equalobj (lua_State *L, const TValue *t1, const TValue *t2);
LUAI_FUNC int luaV_lessthan (lua_State *L, const TValue *l, const TValue *r);
LUAI_FUNC int luaV_lessequal (lua_State *L, const TValue *l, const TValue *r);
LUAI_FUNC int luaV_tonumber_ (const TValue *obj, lua_Number *n);
LUAI_FUNC int luaV_tointeger (const TValue *obj, lua_Integer *p, int mode);
/** RAVI changes start **/
LUAI_FUNC int luaV_tointeger_(const TValue *obj, lua_Integer *p);
LUAI_FUNC void luaV_gettable (lua_State *L, const TValue *t, TValue *key,
                                            StkId val);
LUAI_FUNC void luaV_settable (lua_State *L, const TValue *t, TValue *key,
                                            StkId val);
/** RAVI changed end **/
LUAI_FUNC void luaV_finishget (lua_State *L, const TValue *t, TValue *key,
                               StkId val, const TValue *slot);
LUAI_FUNC void luaV_finishset (lua_State *L, const TValue *t, TValue *key,
                               StkId val, const TValue *slot);
LUAI_FUNC void luaV_finishOp (lua_State *L);
/* RAVI change: the int return value is a Ravi extension */
LUAI_FUNC int luaV_execute (lua_State *L);
LUAI_FUNC void luaV_concat (lua_State *L, int total);
LUAI_FUNC lua_Integer luaV_div (lua_State *L, lua_Integer x, lua_Integer y);
LUAI_FUNC lua_Integer luaV_mod (lua_State *L, lua_Integer x, lua_Integer y);
LUAI_FUNC lua_Integer luaV_shiftl (lua_Integer x, lua_Integer y);
LUAI_FUNC void luaV_objlen (lua_State *L, StkId ra, const TValue *rb);

/* RAVI changes start for JIT */
/* The following expose some of the VM opcodes for the JIT compiler */
LUAI_FUNC int luaV_forlimit(const TValue *obj, lua_Integer *p, lua_Integer step,
  int *stopnow);
LUAI_FUNC void raviV_op_loadnil(CallInfo *ci, int a, int b);
LUAI_FUNC void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra);
LUAI_FUNC void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra);
LUAI_FUNC void raviV_op_newtable(lua_State *L, CallInfo *ci, TValue *ra, int b, int c);
LUAI_FUNC void raviV_op_setlist(lua_State *L, CallInfo *ci, TValue *ra, int b, int c);
LUAI_FUNC void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c);
LUAI_FUNC void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int Bx);
LUAI_FUNC void raviV_op_vararg(lua_State *L, CallInfo *ci, LClosure *cl, int a, int b);
LUAI_FUNC void raviV_op_setupvali(lua_State *L, LClosure *cl, TValue *ra, int b);
LUAI_FUNC void raviV_op_setupvalf(lua_State *L, LClosure *cl, TValue *ra, int b);
LUAI_FUNC void raviV_op_setupvalai(lua_State *L, LClosure *cl, TValue *ra, int b);
LUAI_FUNC void raviV_op_setupvalaf(lua_State *L, LClosure *cl, TValue *ra, int b);
LUAI_FUNC void raviV_op_setupvalt(lua_State *L, LClosure *cl, TValue *ra, int b);
LUAI_FUNC void raviV_op_setupval(lua_State *L, LClosure *cl, TValue *ra, int b);
LUAI_FUNC void raviV_op_add(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
LUAI_FUNC void raviV_op_sub(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
LUAI_FUNC void raviV_op_mul(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
LUAI_FUNC void raviV_op_div(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
LUAI_FUNC void raviV_op_shl(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
LUAI_FUNC void raviV_op_shr(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
LUAI_FUNC void raviV_op_bor(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
LUAI_FUNC void raviV_op_bxor(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
LUAI_FUNC void raviV_op_band(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
LUAI_FUNC void raviV_op_bnot(lua_State *L, TValue *ra, TValue *rb);
LUAI_FUNC void raviV_gettable_sskey(lua_State *L, const TValue *t, TValue *key, StkId val);
LUAI_FUNC void raviV_settable_sskey(lua_State *L, const TValue *t, TValue *key, StkId val);
LUAI_FUNC void raviV_gettable_i(lua_State *L, const TValue *t, TValue *key, StkId val);
LUAI_FUNC void raviV_settable_i(lua_State *L, const TValue *t, TValue *key, StkId val);

#endif

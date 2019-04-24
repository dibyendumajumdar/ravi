/*
** $Id: lvm.c,v 2.268.1.1 2017/04/19 17:39:34 roberto Exp $
** Lua virtual machine
** See Copyright Notice in lua.h
*/

/*
** Portions Copyright (C) 2015-2017 Dibyendu Majumdar
*/


#define lvm_c
#define LUA_CORE

#include "lprefix.h"

#include <float.h>
#include <limits.h>
#include <math.h>
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
#include "ravi_profile.h"

/* limit for table tag-method chains (to avoid loops) */
#define MAXTAGLOOP	2000



/*
** 'l_intfitsf' checks whether a given integer can be converted to a
** float without rounding. Used in comparisons. Left undefined if
** all integers fit in a float precisely.
*/
#if !defined(l_intfitsf)

/* number of bits in the mantissa of a float */
#define NBM		(l_mathlim(MANT_DIG))

/*
** Check whether some integers may not fit in a float, that is, whether
** (maxinteger >> NBM) > 0 (that implies (1 << NBM) <= maxinteger).
** (The shifts are done in parts to avoid shifting by more than the size
** of an integer. In a worst case, NBM == 113 for long double and
** sizeof(integer) == 32.)
*/
#if ((((LUA_MAXINTEGER >> (NBM / 4)) >> (NBM / 4)) >> (NBM / 4)) \
	>> (NBM - (3 * (NBM / 4))))  >  0

#define l_intfitsf(i)  \
  (-((lua_Integer)1 << NBM) <= (i) && (i) <= ((lua_Integer)1 << NBM))

#endif

#endif



/*
** Try to convert a value to a float. The float case is already handled
** by the macro 'tonumber'.
*/
int luaV_tonumber_ (const TValue *obj, lua_Number *n) {
  TValue v;
  if (ttisinteger(obj)) {
    *n = cast_num(ivalue(obj));
    return 1;
  }
  else if (cvt2num(obj) &&  /* string coercible to number? */
            luaO_str2num(svalue(obj), &v) == vslen(obj) + 1) {
    *n = nvalue(&v);  /* convert result of 'luaO_str2num' to a float */
    return 1;
  }
  else
    return 0;  /* conversion failed */
}


/*
** try to convert a float to an integer, rounding according to 'mode':
** mode == 0: accepts only integral values
** mode == 1: takes the floor of the number
** mode == 2: takes the ceil of the number
*/
int luaV_flttointeger (const TValue *obj, lua_Integer *p, int mode) {
  if (!ttisfloat(obj))
    return 0;
  else {
    lua_Number n = fltvalue(obj);
    lua_Number f = l_floor(n);
    if (n != f) {  /* not an integral value? */
      if (mode == 0) return 0;  /* fails if mode demands integral value */
      else if (mode > 1)  /* needs ceil? */
        f += 1;  /* convert floor to ceil (remember: n != f) */
    }
    return lua_numbertointeger(f, p);
  }
}


/*
** try to convert a value to an integer. ("Fast track" is handled
** by macro 'tointeger'.)
*/
int luaV_tointeger (const TValue *obj, lua_Integer *p, int mode) {
  TValue v;
  if (cvt2num(obj) && luaO_str2num(svalue(obj), &v) == vslen(obj) + 1)
    obj = &v;  /* change string to its corresponding number */
  if (ttisinteger(obj)) {
    *p = ivalue(obj);
    return 1;
  }
  else
    return luaV_flttointeger(obj, p, mode);
}


/*
** try to convert a value to an integer
*/
int luaV_tointeger_ (const TValue *obj, lua_Integer *p) {
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
}


/*
** Try to convert a 'for' limit to an integer, preserving the
** semantics of the loop.
** (The following explanation assumes a non-negative step; it is valid
** for negative steps mutatis mutandis.)
** If the limit can be converted to an integer, rounding down, that is
** it.
** Otherwise, check whether the limit can be converted to a number.  If
** the number is too large, it is OK to set the limit as LUA_MAXINTEGER,
** which means no limit.  If the number is too negative, the loop
** should not run, because any initial integer value is larger than the
** limit. So, it sets the limit to LUA_MININTEGER. 'stopnow' corrects
** the extreme case when the initial value is LUA_MININTEGER, in which
** case the LUA_MININTEGER limit would still run the loop once.
*/
int luaV_forlimit (const TValue *obj, lua_Integer *p, lua_Integer step,
                     int *stopnow) {
  *stopnow = 0;  /* usually, let loops run */
  if (ttisinteger(obj))
    *p = ivalue(obj);
  else if (!luaV_tointeger(obj, p, (step < 0 ? 2 : 1))) {
    /* not coercible to in integer */
    lua_Number n;  /* try to convert to float */
    if (!tonumber(obj, &n)) /* cannot convert to float? */
      return 0;  /* not a number */
    if (luai_numlt(0, n)) {  /* if true, float is larger than max integer */
      *p = LUA_MAXINTEGER;
      if (step < 0) *stopnow = 1;
    }
    else {  /* float is smaller than min integer */
      *p = LUA_MININTEGER;
      if (step >= 0) *stopnow = 1;
    }
  }
  return 1;
}


/*
** Finish the table access 'val = t[key]'.
** if 'slot' is NULL, 't' is not a table; otherwise, 'slot' points to
** t[k] entry (which must be nil).
*/
void luaV_finishget (lua_State *L, const TValue *t, TValue *key, StkId val,
                      const TValue *slot) {
  int loop;  /* counter to avoid infinite loops */
  const TValue *tm;  /* metamethod */
  for (loop = 0; loop < MAXTAGLOOP; loop++) {
    if (slot == NULL) {  /* 't' is not a table? */
      lua_assert(!ttistable(t));
      tm = luaT_gettmbyobj(L, t, TM_INDEX);
      if (ttisnil(tm))
        luaG_typeerror(L, t, "index");  /* no metamethod */
      /* else will try the metamethod */
    }
    else {  /* 't' is a table */
      lua_assert(ttisnil(slot));
      tm = fasttm(L, hvalue(t)->metatable, TM_INDEX);  /* table's metamethod */
      if (tm == NULL) {  /* no metamethod? */
        setnilvalue(val);  /* result is nil */
        return;
      }
      /* else will try the metamethod */
    }
    if (ttisfunction(tm)) {  /* is metamethod a function? */
      luaT_callTM(L, tm, t, key, val, 1);  /* call it */
      return;
    }
    t = tm;  /* else try to access 'tm[key]' */
    if (luaV_fastget(L,t,key,slot,luaH_get)) {  /* fast track? */
      setobj2s(L, val, slot);  /* done */
      return;
    }
    /* else repeat (tail call 'luaV_finishget') */
  }
  luaG_runerror(L, "'__index' chain too long; possible loop");
}


/*
** Finish a table assignment 't[key] = val'.
** If 'slot' is NULL, 't' is not a table.  Otherwise, 'slot' points
** to the entry 't[key]', or to 'luaO_nilobject' if there is no such
** entry.  (The value at 'slot' must be nil, otherwise 'luaV_fastget'
** would have done the job.)
*/
void luaV_finishset (lua_State *L, const TValue *t, TValue *key,
                     StkId val, const TValue *slot) {
  int loop;  /* counter to avoid infinite loops */
  for (loop = 0; loop < MAXTAGLOOP; loop++) {
    const TValue *tm;  /* '__newindex' metamethod */
    if (slot != NULL) {  /* is 't' a table? */
      Table *h = hvalue(t);  /* save 't' table */
      lua_assert(ttisnil(slot));  /* old value must be nil */
      tm = fasttm(L, h->metatable, TM_NEWINDEX);  /* get metamethod */
      if (tm == NULL) {  /* no metamethod? */
        if (slot == luaO_nilobject)  /* no previous entry? */
          slot = luaH_newkey(L, h, key);  /* create one */
        /* no metamethod and (now) there is an entry with given key */
        setobj2t(L, cast(TValue *, slot), val);  /* set its new value */
        invalidateTMcache(h);
        luaC_barrierback(L, h, val);
        return;
      }
      /* else will try the metamethod */
    }
    else {  /* not a table; check metamethod */
      if (ttisnil(tm = luaT_gettmbyobj(L, t, TM_NEWINDEX)))
        luaG_typeerror(L, t, "index");
    }
    /* try the metamethod */
    if (ttisfunction(tm)) {
      luaT_callTM(L, tm, t, key, val, 0);
      return;
    }
    t = tm;  /* else repeat assignment over 'tm' */
    if (luaV_fastget(L, t, key, slot, luaH_get)) {
      luaV_finishfastset(L, t, slot, val);
      return;  /* done */
    }
    /* else 'return luaV_finishset(L, t, key, val, slot)' (loop) */
  }
  luaG_runerror(L, "'__newindex' chain too long; possible loop");
}

#define GETTABLE_INLINE_(L, t, key, val, protect)                \
  if (RAVI_LIKELY(ttisLtable(t))) {                              \
    const TValue *aux;                                           \
    if (ttisinteger(key)) {                                      \
      lua_Integer idx = ivalue(key);                             \
      Table *h = hvalue(t);                                      \
      if (RAVI_LIKELY(l_castS2U(idx - 1) < h->sizearray))        \
        aux = &h->array[idx - 1];                                \
      else                                                       \
        aux = luaH_getint(h, idx);                               \
    }                                                            \
    else                                                         \
      aux = luaH_get(hvalue(t), key);                            \
    if (RAVI_LIKELY(!ttisnil(aux))) { setobj2s(L, val, aux); }   \
    else                                                         \
      protect(luaV_finishget(L, t, key, val, aux));              \
  }                                                              \
  else if (ttisfarray(t)) {                                      \
    if (!ttisinteger(key)) luaG_typeerror(L, key, "index");      \
    Table *h = hvalue(t);                                        \
    raviH_get_float_inline(L, h, ivalue(key), val);              \
  }                                                              \
  else if (ttisiarray(t)) {                                      \
    if (!ttisinteger(key)) luaG_typeerror(L, key, "index");      \
    Table *h = hvalue(t);                                        \
    raviH_get_int_inline(L, h, ivalue(key), val);                \
  }                                                              \
  else {                                                         \
    protect(luaV_finishget(L, t, key, val, NULL));               \
  }

#define GETTABLE_INLINE(L, t, key, val) GETTABLE_INLINE_(L, t, key, val, Unprotect)
#define GETTABLE_INLINE_PROTECTED(L, t, key, val) GETTABLE_INLINE_(L, t, key, val, Protect)

#define GETTABLE_INLINE_I_(L, t, key, val, protect)              \
  if (RAVI_LIKELY(ttisLtable(t))) {                              \
    lua_Integer idx = ivalue(key);                               \
    Table *h = hvalue(t);                                        \
    const TValue *v;                                             \
    if (RAVI_LIKELY(l_castS2U(idx - 1) < h->sizearray))          \
      v = &h->array[idx - 1];                                    \
    else                                                         \
      v = luaH_getint(h, idx);                                   \
    if (RAVI_LIKELY(!ttisnil(v))) {                              \
      setobj2s(L, val, v);                                       \
    }                                                            \
    else                                                         \
      protect(luaV_finishget(L, t, key, val, v));                \
  }                                                              \
  else if (ttisfarray(t)) {                                      \
    Table *h = hvalue(t);                                        \
    raviH_get_float_inline(L, h, ivalue(key), val);              \
  }                                                              \
  else if (ttisiarray(t)) {                                      \
    Table *h = hvalue(t);                                        \
    raviH_get_int_inline(L, h, ivalue(key), val);                \
  }                                                              \
  else {                                                         \
    protect(luaV_finishget(L, t, key, val, NULL));               \
  }

#define GETTABLE_INLINE_I(L, t, key, val) GETTABLE_INLINE_I_(L, t, key, val, Unprotect)
#define GETTABLE_INLINE_PROTECTED_I(L, t, key, val) GETTABLE_INLINE_I_(L, t, key, val, Protect)

#define GETTABLE_INLINE_SSKEY_(L, t, key, val, protect)            \
  if (RAVI_LIKELY(ttisLtable(t))) {                                \
    const TValue *aux = luaH_getshortstr(hvalue(t), tsvalue(key)); \
    if (RAVI_LIKELY(!ttisnil(aux))) { setobj2s(L, val, aux); }     \
    else                                                           \
      protect(luaV_finishget(L, t, key, val, aux));                \
  }                                                                \
  else if (ttistable(t))  {                                        \
    luaG_typeerror(L, t, "incorrectly index");                     \
  }                                                                \
  else {                                                           \
    protect(luaV_finishget(L, t, key, val, NULL));                 \
  }

#define GETTABLE_INLINE_SSKEY_PROTECTED(L, t, key, val) GETTABLE_INLINE_SSKEY_(L, t, key, val, Protect)
#define GETTABLE_INLINE_SSKEY(L, t, key, val) GETTABLE_INLINE_SSKEY_(L, t, key, val, Unprotect)

#define SETTABLE_INLINE_(L, t, key, val, protect)                           \
  if (RAVI_LIKELY(ttisLtable(t))) {                                         \
    const TValue *slot;                                                     \
    if (ttisinteger(key)) {                                                 \
      lua_Integer idx = ivalue(key);                                        \
      Table *h = hvalue(t);                                                 \
      if (RAVI_LIKELY(l_castS2U(idx - 1) < h->sizearray))                   \
        slot = &h->array[idx - 1];                                          \
      else                                                                  \
        slot = luaH_getint(h, idx);                                         \
    }                                                                       \
    else                                                                    \
      slot = luaH_get(hvalue(t), key);                                      \
    if (!ttisnil(slot)) {                                                   \
      setobj2t(L, cast(TValue *, slot), val);                               \
      luaC_barrierback(L, hvalue(t), val);                                  \
    }                                                                       \
    else {                                                                  \
      protect(luaV_finishset(L, t, key, val, slot));                        \
    }                                                                       \
  }                                                                         \
  else if (ttisfarray(t)) {                                                 \
    Table *h = hvalue(t);                                                   \
    if (!ttisinteger(key)) luaG_typeerror(L, key, "index");                 \
    if (ttisfloat(val)) {                                                   \
      raviH_set_float_inline(L, h, ivalue(key), fltvalue(val));             \
    }                                                                       \
    else if (ttisinteger(val)) {                                            \
      raviH_set_float_inline(L, h, ivalue(key), (lua_Number)(ivalue(val))); \
    }                                                                       \
    else {                                                                  \
      lua_Number d = 0.0;                                                   \
      if (luaV_tonumber_(val, &d)) {                                        \
        raviH_set_float_inline(L, h, ivalue(key), d);                       \
      }                                                                     \
      else                                                                  \
        luaG_runerror(L, "value cannot be converted to number");            \
    }                                                                       \
  }                                                                         \
  else if (ttisiarray(t)) {                                                 \
    Table *h = hvalue(t);                                                   \
    if (!ttisinteger(key)) luaG_typeerror(L, key, "index");                 \
    if (ttisinteger(val)) {                                                 \
      raviH_set_int_inline(L, h, ivalue(key), ivalue(val));                 \
    }                                                                       \
    else {                                                                  \
      lua_Integer i = 0;                                                    \
      if (luaV_tointeger_(val, &i)) {                                       \
        raviH_set_int_inline(L, h, ivalue(key), i);                         \
      }                                                                     \
      else                                                                  \
        luaG_runerror(L, "value cannot be converted to integer");           \
    }                                                                       \
  }                                                                         \
  else {                                                                    \
    protect(luaV_finishset(L, t, key, val, NULL));                          \
  }

#define SETTABLE_INLINE_PROTECTED(L, t, key, val) SETTABLE_INLINE_(L, t, key, val, Protect) 
#define SETTABLE_INLINE(L, t, key, val) SETTABLE_INLINE_(L, t, key, val, Unprotect)

#define SETTABLE_INLINE_I_(L, t, key, val, protect)                         \
  if (RAVI_LIKELY(ttisLtable(t))) {                                         \
    lua_Integer idx = ivalue(key);                                          \
    Table *h = hvalue(t);                                                   \
    const TValue *slot;                                                     \
    if (RAVI_LIKELY(l_castS2U(idx - 1) < h->sizearray))                     \
      slot = &h->array[idx - 1];                                            \
    else                                                                    \
      slot = luaH_getint(h, idx);                                           \
    if (!ttisnil(slot)) {                                                   \
      setobj2t(L, cast(TValue *, slot), val);                               \
      luaC_barrierback(L, h, val);                                          \
    }                                                                       \
    else {                                                                  \
      protect(luaV_finishset(L, t, key, val, slot));                        \
    }                                                                       \
  }                                                                         \
  else if (ttisfarray(t)) {                                                 \
    Table *h = hvalue(t);                                                   \
    if (ttisfloat(val)) {                                                   \
      raviH_set_float_inline(L, h, ivalue(key), fltvalue(val));             \
    }                                                                       \
    else if (ttisinteger(val)) {                                            \
      raviH_set_float_inline(L, h, ivalue(key), (lua_Number)(ivalue(val))); \
    }                                                                       \
    else {                                                                  \
      lua_Number d = 0.0;                                                   \
      if (luaV_tonumber_(val, &d)) {                                        \
        raviH_set_float_inline(L, h, ivalue(key), d);                       \
      }                                                                     \
      else                                                                  \
        luaG_runerror(L, "value cannot be converted to number");            \
    }                                                                       \
  }                                                                         \
  else if (ttisiarray(t)) {                                                 \
    Table *h = hvalue(t);                                                   \
    if (ttisinteger(val)) {                                                 \
      raviH_set_int_inline(L, h, ivalue(key), ivalue(val));                 \
    }                                                                       \
    else {                                                                  \
      lua_Integer i = 0;                                                    \
      if (luaV_tointeger_(val, &i)) {                                       \
        raviH_set_int_inline(L, h, ivalue(key), i);                         \
      }                                                                     \
      else                                                                  \
        luaG_runerror(L, "value cannot be converted to integer");           \
    }                                                                       \
  }                                                                         \
  else {                                                                    \
    protect(luaV_finishset(L, t, key, val, NULL));                          \
  }

#define SETTABLE_INLINE_PROTECTED_I(L, t, key, val) SETTABLE_INLINE_I_(L, t, key, val, Protect) 
#define SETTABLE_INLINE_I(L, t, key, val) SETTABLE_INLINE_I_(L, t, key, val, Unprotect)


#define SETTABLE_INLINE_SSKEY_(L, t, key, val, protect)             \
  if (RAVI_LIKELY(ttisLtable(t))) {                                 \
    const TValue *slot = luaH_getshortstr(hvalue(t), tsvalue(key)); \
    if (!ttisnil(slot)) {                                           \
      setobj2t(L, cast(TValue *, slot), val);                       \
      luaC_barrierback(L, hvalue(t), val);                          \
    }                                                               \
    else {                                                          \
      protect(luaV_finishset(L, t, key, val, slot));                \
    }                                                               \
  }                                                                 \
  else if (ttistable(t))  {                                         \
    luaG_typeerror(L, t, "incorrectly index");                      \
  }                                                                 \
  else {                                                            \
    protect(luaV_finishset(L, t, key, val, NULL));                  \
  }

#define SETTABLE_INLINE_SSKEY_PROTECTED(L, t, key, val) SETTABLE_INLINE_SSKEY_(L, t, key, val, Protect)
#define SETTABLE_INLINE_SSKEY(L, t, key, val) SETTABLE_INLINE_SSKEY_(L, t, key, val, Unprotect)


#define Unprotect(x) x

/*
** Main function for table access (invoking metamethods if needed).
** Compute 'val = t[key]'
** In Lua 5.3.2 this function is a macro but we need it to be a function
** so that JIT code can invoke it
*/
void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val) {
  GETTABLE_INLINE(L, t, key, val);
}


/*
** Main function for table assignment (invoking metamethods if needed).
** Compute 't[key] = val'
** In Lua 5.3.2 this function is a macro but we need it to be a function
** so that JIT code can invoke it
*/
void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val) {
  SETTABLE_INLINE(L, t, key, val);
}


/*
** Compare two strings 'ls' x 'rs', returning an integer smaller-equal-
** -larger than zero if 'ls' is smaller-equal-larger than 'rs'.
** The code is a little tricky because it allows '\0' in the strings
** and it uses 'strcoll' (to respect locales) for each segments
** of the strings.
*/
static int l_strcmp (const TString *ls, const TString *rs) {
  const char *l = getstr(ls);
  size_t ll = tsslen(ls);
  const char *r = getstr(rs);
  size_t lr = tsslen(rs);
  for (;;) {  /* for each segment */
    int temp = strcoll(l, r);
    if (temp != 0)  /* not equal? */
      return temp;  /* done */
    else {  /* strings are equal up to a '\0' */
      size_t len = strlen(l);  /* index of first '\0' in both strings */
      if (len == lr)  /* 'rs' is finished? */
        return (len == ll) ? 0 : 1;  /* check 'ls' */
      else if (len == ll)  /* 'ls' is finished? */
        return -1;  /* 'ls' is smaller than 'rs' ('rs' is not finished) */
      /* both strings longer than 'len'; go on comparing after the '\0' */
      len++;
      l += len; ll -= len; r += len; lr -= len;
    }
  }
}


/*
** Check whether integer 'i' is less than float 'f'. If 'i' has an
** exact representation as a float ('l_intfitsf'), compare numbers as
** floats. Otherwise, if 'f' is outside the range for integers, result
** is trivial. Otherwise, compare them as integers. (When 'i' has no
** float representation, either 'f' is "far away" from 'i' or 'f' has
** no precision left for a fractional part; either way, how 'f' is
** truncated is irrelevant.) When 'f' is NaN, comparisons must result
** in false.
*/
static int LTintfloat (lua_Integer i, lua_Number f) {
#if defined(l_intfitsf)
  if (!l_intfitsf(i)) {
    if (f >= -cast_num(LUA_MININTEGER))  /* -minint == maxint + 1 */
      return 1;  /* f >= maxint + 1 > i */
    else if (f > cast_num(LUA_MININTEGER))  /* minint < f <= maxint ? */
      return (i < cast(lua_Integer, f));  /* compare them as integers */
    else  /* f <= minint <= i (or 'f' is NaN)  -->  not(i < f) */
      return 0;
  }
#endif
  return luai_numlt(cast_num(i), f);  /* compare them as floats */
}


/*
** Check whether integer 'i' is less than or equal to float 'f'.
** See comments on previous function.
*/
static int LEintfloat (lua_Integer i, lua_Number f) {
#if defined(l_intfitsf)
  if (!l_intfitsf(i)) {
    if (f >= -cast_num(LUA_MININTEGER))  /* -minint == maxint + 1 */
      return 1;  /* f >= maxint + 1 > i */
    else if (f >= cast_num(LUA_MININTEGER))  /* minint <= f <= maxint ? */
      return (i <= cast(lua_Integer, f));  /* compare them as integers */
    else  /* f < minint <= i (or 'f' is NaN)  -->  not(i <= f) */
      return 0;
  }
#endif
  return luai_numle(cast_num(i), f);  /* compare them as floats */
}


/*
** Return 'l < r', for numbers.
*/
static int LTnum (const TValue *l, const TValue *r) {
  lua_assert(ttisnumber(l) && ttisnumber(r));
  if (ttisinteger(l)) {
    lua_Integer li = ivalue(l);
    if (ttisinteger(r))
      return li < ivalue(r);  /* both are integers */
    else  /* 'l' is int and 'r' is float */
      return LTintfloat(li, fltvalue(r));  /* l < r ? */
  }
  else {
    lua_Number lf = fltvalue(l);  /* 'l' must be float */
    if (ttisfloat(r))
      return luai_numlt(lf, fltvalue(r));  /* both are float */
    else if (luai_numisnan(lf))  /* 'r' is int and 'l' is float */
      return 0;  /* NaN < i is always false */
    else  /* without NaN, (l < r)  <-->  not(r <= l) */
      return !LEintfloat(ivalue(r), lf);  /* not (r <= l) ? */
  }
}


/*
** Return 'l <= r', for numbers.
*/
static int LEnum (const TValue *l, const TValue *r) {
  lua_assert(ttisnumber(l) && ttisnumber(r));
  if (ttisinteger(l)) {
    lua_Integer li = ivalue(l);
    if (ttisinteger(r))
      return li <= ivalue(r);  /* both are integers */
    else  /* 'l' is int and 'r' is float */
      return LEintfloat(li, fltvalue(r));  /* l <= r ? */
  }
  else {
    lua_Number lf = fltvalue(l);  /* 'l' must be float */
    if (ttisfloat(r))
      return luai_numle(lf, fltvalue(r));  /* both are float */
    else if (luai_numisnan(lf))  /* 'r' is int and 'l' is float */
      return 0;  /*  NaN <= i is always false */
    else  /* without NaN, (l <= r)  <-->  not(r < l) */
      return !LTintfloat(ivalue(r), lf);  /* not (r < l) ? */
  }
}


/*
** Main operation less than; return 'l < r'.
*/
int luaV_lessthan (lua_State *L, const TValue *l, const TValue *r) {
  int res;
  if (ttisnumber(l) && ttisnumber(r))  /* both operands are numbers? */
    return LTnum(l, r);
  else if (ttisstring(l) && ttisstring(r))  /* both are strings? */
    return l_strcmp(tsvalue(l), tsvalue(r)) < 0;
  else if ((res = luaT_callorderTM(L, l, r, TM_LT)) < 0)  /* no metamethod? */
    luaG_ordererror(L, l, r);  /* error */
  return res;
}


/*
** Main operation less than or equal to; return 'l <= r'. If it needs
** a metamethod and there is no '__le', try '__lt', based on
** l <= r iff !(r < l) (assuming a total order). If the metamethod
** yields during this substitution, the continuation has to know
** about it (to negate the result of r<l); bit CIST_LEQ in the call
** status keeps that information.
*/
int luaV_lessequal (lua_State *L, const TValue *l, const TValue *r) {
  int res;
  if (ttisnumber(l) && ttisnumber(r))  /* both operands are numbers? */
    return LEnum(l, r);
  else if (ttisstring(l) && ttisstring(r))  /* both are strings? */
    return l_strcmp(tsvalue(l), tsvalue(r)) <= 0;
  else if ((res = luaT_callorderTM(L, l, r, TM_LE)) >= 0)  /* try 'le' */
    return res;
  else {  /* try 'lt': */
    L->ci->callstatus |= CIST_LEQ;  /* mark it is doing 'lt' for 'le' */
    res = luaT_callorderTM(L, r, l, TM_LT);
    L->ci->callstatus ^= CIST_LEQ;  /* clear mark */
    if (res < 0)
      luaG_ordererror(L, l, r);
    return !res;  /* result is negated */
  }
}


/*
** Main operation for equality of Lua values; return 't1 == t2'.
** L == NULL means raw equality (no metamethods)
*/
int luaV_equalobj (lua_State *L, const TValue *t1, const TValue *t2) {
  const TValue *tm;
  if (ttype(t1) != ttype(t2)) {  /* not the same variant? */
    if (ttnov(t1) != ttnov(t2) || ttnov(t1) != LUA_TNUMBER)
      return 0;  /* only numbers can be equal with different variants */
    else {  /* two numbers with different variants */
      lua_Integer i1, i2;  /* compare them as integers */
      return (tointegerns(t1, &i1) && tointegerns(t2, &i2) && i1 == i2);
    }
  }
  /* values have same type and same variant */
  switch (ttype(t1)) {
    case LUA_TNIL: return 1;
    case LUA_TNUMINT: return (ivalue(t1) == ivalue(t2));
    case LUA_TNUMFLT: return luai_numeq(fltvalue(t1), fltvalue(t2));
    case LUA_TBOOLEAN: return bvalue(t1) == bvalue(t2);  /* true must be 1 !! */
    case LUA_TLIGHTUSERDATA: return pvalue(t1) == pvalue(t2);
    case LUA_TLCF: return fvalue(t1) == fvalue(t2);
	case RAVI_TFCF: return fcfvalue(t1) == fcfvalue(t2);
    case LUA_TSHRSTR: return eqshrstr(tsvalue(t1), tsvalue(t2));
    case LUA_TLNGSTR: return luaS_eqlngstr(tsvalue(t1), tsvalue(t2));
    case LUA_TUSERDATA: {
      if (uvalue(t1) == uvalue(t2)) return 1;
      else if (L == NULL) return 0;
      tm = fasttm(L, uvalue(t1)->metatable, TM_EQ);
      if (tm == NULL)
        tm = fasttm(L, uvalue(t2)->metatable, TM_EQ);
      break;  /* will try TM */
    }
    case RAVI_TIARRAY:
    case RAVI_TFARRAY: {
      if (hvalue(t1) == hvalue(t2)) return 1;
      else return 0;
    }
    case LUA_TTABLE: {
      if (hvalue(t1) == hvalue(t2)) return 1;
      else if (L == NULL) return 0;
      tm = fasttm(L, hvalue(t1)->metatable, TM_EQ);
      if (tm == NULL)
        tm = fasttm(L, hvalue(t2)->metatable, TM_EQ);
      break;  /* will try TM */
    }
    default:
      return gcvalue(t1) == gcvalue(t2);
  }
  if (tm == NULL)  /* no TM? */
    return 0;  /* objects are different */
  luaT_callTM(L, tm, t1, t2, L->top, 1);  /* call TM */
  return !l_isfalse(L->top);
}


/* macro used by 'luaV_concat' to ensure that element at 'o' is a string */
#define tostring(L,o)  \
	(ttisstring(o) || (cvt2str(o) && (luaO_tostring(L, o), 1)))

#define isemptystr(o)	(ttisshrstring(o) && tsvalue(o)->shrlen == 0)

/* copy strings in stack from top - n up to top - 1 to buffer */
static void copy2buff (StkId top, int n, char *buff) {
  size_t tl = 0;  /* size already copied */
  do {
    size_t l = vslen(top - n);  /* length of string being copied */
    memcpy(buff + tl, svalue(top - n), l * sizeof(char));
    tl += l;
  } while (--n > 0);
}


/*
** Main operation for concatenation: concat 'total' values in the stack,
** from 'L->top - total' up to 'L->top - 1'.
*/
void luaV_concat (lua_State *L, int total) {
  lua_assert(total >= 2);
  do {
    StkId top = L->top;
    int n = 2;  /* number of elements handled in this pass (at least 2) */
    if (!(ttisstring(top-2) || cvt2str(top-2)) || !tostring(L, top-1))
      luaT_trybinTM(L, top-2, top-1, top-2, TM_CONCAT);
    else if (isemptystr(top - 1))  /* second operand is empty? */
      cast_void(tostring(L, top - 2));  /* result is first operand */
    else if (isemptystr(top - 2)) {  /* first operand is an empty string? */
      setobjs2s(L, top - 2, top - 1);  /* result is second op. */
    }
    else {
      /* at least two non-empty string values; get as many as possible */
      size_t tl = vslen(top - 1);
      TString *ts;
      /* collect total length and number of strings */
      for (n = 1; n < total && tostring(L, top - n - 1); n++) {
        size_t l = vslen(top - n - 1);
        if (l >= (MAX_SIZE/sizeof(char)) - tl)
          luaG_runerror(L, "string length overflow");
        tl += l;
      }
      if (tl <= LUAI_MAXSHORTLEN) {  /* is result a short string? */
        char buff[LUAI_MAXSHORTLEN];
        copy2buff(top, n, buff);  /* copy strings to buffer */
        ts = luaS_newlstr(L, buff, tl);
      }
      else {  /* long string; copy strings directly to final result */
        ts = luaS_createlngstrobj(L, tl);
        copy2buff(top, n, getstr(ts));
      }
      setsvalue2s(L, top - n, ts);  /* create result */
    }
    total -= n-1;  /* got 'n' strings to create 1 new */
    L->top -= n-1;  /* popped 'n' strings and pushed one */
  } while (total > 1);  /* repeat until only 1 result left */
}


/*
** Main operation 'ra' = #rb'.
*/
void luaV_objlen (lua_State *L, StkId ra, const TValue *rb) {
  const TValue *tm;
  switch (ttype(rb)) {
    case RAVI_TIARRAY:
    case RAVI_TFARRAY: {
      Table *h = hvalue(rb);
      setivalue(ra, raviH_getn(h));
      return;
    }
    case LUA_TTABLE: {
      Table *h = hvalue(rb);
      tm = fasttm(L, h->metatable, TM_LEN);
      if (tm) break;  /* metamethod? break switch to call it */
      setivalue(ra, luaH_getn(h));  /* else primitive len */
      return;
    }
    case LUA_TSHRSTR: {
      setivalue(ra, tsvalue(rb)->shrlen);
      return;
    }
    case LUA_TLNGSTR: {
      setivalue(ra, tsvalue(rb)->u.lnglen);
      return;
    }
    default: {  /* try metamethod */
      tm = luaT_gettmbyobj(L, rb, TM_LEN);
      if (ttisnil(tm))  /* no metamethod? */
        luaG_typeerror(L, rb, "get length of");
      break;
    }
  }
  luaT_callTM(L, tm, rb, rb, ra, 1);
}


/*
** Integer division; return 'm // n', that is, floor(m/n).
** C division truncates its result (rounds towards zero).
** 'floor(q) == trunc(q)' when 'q >= 0' or when 'q' is integer,
** otherwise 'floor(q) == trunc(q) - 1'.
*/
lua_Integer luaV_div (lua_State *L, lua_Integer m, lua_Integer n) {
  if (l_castS2U(n) + 1u <= 1u) {  /* special cases: -1 or 0 */
    if (n == 0)
      luaG_runerror(L, "attempt to divide by zero");
    return intop(-, 0, m);   /* n==-1; avoid overflow with 0x80000...//-1 */
  }
  else {
    lua_Integer q = m / n;  /* perform C division */
    if ((m ^ n) < 0 && m % n != 0)  /* 'm/n' would be negative non-integer? */
      q -= 1;  /* correct result for different rounding */
    return q;
  }
}


/*
** Integer modulus; return 'm % n'. (Assume that C '%' with
** negative operands follows C99 behavior. See previous comment
** about luaV_div.)
*/
lua_Integer luaV_mod (lua_State *L, lua_Integer m, lua_Integer n) {
  if (l_castS2U(n) + 1u <= 1u) {  /* special cases: -1 or 0 */
    if (n == 0)
      luaG_runerror(L, "attempt to perform 'n%%0'");
    return 0;   /* m % -1 == 0; avoid overflow with 0x80000...%-1 */
  }
  else {
    lua_Integer r = m % n;
    if (r != 0 && (m ^ n) < 0)  /* 'm/n' would be non-integer negative? */
      r += n;  /* correct result for different rounding */
    return r;
  }
}


/* number of bits in an integer */
#define NBITS	cast_int(sizeof(lua_Integer) * CHAR_BIT)

/*
** Shift left operation. (Shift right just negates 'y'.)
*/
lua_Integer luaV_shiftl (lua_Integer x, lua_Integer y) {
  if (y < 0) {  /* shift right? */
    if (y <= -NBITS) return 0;
    else return intop(>>, x, -y);
  }
  else {  /* shift left */
    if (y >= NBITS) return 0;
    else return intop(<<, x, y);
  }
}


/*
** check whether cached closure in prototype 'p' may be reused, that is,
** whether there is a cached closure with the same upvalues needed by
** new closure to be created.
*/
static LClosure *getcached (Proto *p, UpVal **encup, StkId base) {
  LClosure *c = p->cache;
  if (c != NULL) {  /* is there a cached closure? */
    int nup = p->sizeupvalues;
    Upvaldesc *uv = p->upvalues;
    int i;
    for (i = 0; i < nup; i++) {  /* check whether it has right upvalues */
      TValue *v = uv[i].instack ? base + uv[i].idx : encup[uv[i].idx]->v;
      if (c->upvals[i]->v != v)
        return NULL;  /* wrong upvalue; cannot reuse closure */
    }
  }
  return c;  /* return cached closure (or NULL if no cached closure) */
}


/*
** create a new Lua closure, push it in the stack, and initialize
** its upvalues. Note that the closure is not cached if prototype is
** already black (which means that 'cache' was already cleared by the
** GC).
*/
static void pushclosure (lua_State *L, Proto *p, UpVal **encup, StkId base,
                         StkId ra) {
  int nup = p->sizeupvalues;
  Upvaldesc *uv = p->upvalues;
  int i;
  LClosure *ncl = luaF_newLclosure(L, nup);
  ncl->p = p;
  setclLvalue(L, ra, ncl);  /* anchor new closure in stack */
  for (i = 0; i < nup; i++) {  /* fill in its upvalues */
    if (uv[i].instack)  /* upvalue refers to local variable? */
      ncl->upvals[i] = luaF_findupval(L, base + uv[i].idx);
    else  /* get upvalue from enclosing function */
      ncl->upvals[i] = encup[uv[i].idx];
    ncl->upvals[i]->refcount++;
    /* new closure is white, so we do not need a barrier here */
  }
  if (!isblack(p))  /* cache will not break GC invariant? */
    p->cache = ncl;  /* save it on cache for reuse */
}


/*
** finish execution of an opcode interrupted by an yield
*/
void luaV_finishOp (lua_State *L) {
  CallInfo *ci = L->ci;
  StkId base = ci->u.l.base;
  Instruction inst = *(ci->u.l.savedpc - 1);  /* interrupted instruction */
  OpCode op = GET_OPCODE(inst);
  switch (op) {  /* finish its execution */
    case OP_ADD: case OP_SUB: case OP_MUL: case OP_DIV: case OP_IDIV:
    case OP_BAND: case OP_BOR: case OP_BXOR: case OP_SHL: case OP_SHR:
    case OP_MOD: case OP_POW:
    case OP_UNM: case OP_BNOT: case OP_LEN:
    case OP_RAVI_GETTABUP_SK: 
    case OP_RAVI_GETFIELD: case OP_RAVI_GETI: case OP_RAVI_TABLE_GETFIELD: 
    case OP_RAVI_SELF_SK: case OP_RAVI_TABLE_SELF_SK:
    case OP_GETTABUP: case OP_GETTABLE: case OP_SELF: {
      setobjs2s(L, base + GETARG_A(inst), --L->top);
      break;
    }
    case OP_LE: case OP_LT: case OP_EQ: {
      int res = !l_isfalse(L->top - 1);
      L->top--;
      if (ci->callstatus & CIST_LEQ) {  /* "<=" using "<" instead? */
        lua_assert(op == OP_LE);
        ci->callstatus ^= CIST_LEQ;  /* clear mark */
        res = !res;  /* negate result */
      }
      lua_assert(GET_OPCODE(*ci->u.l.savedpc) == OP_JMP);
      if (res != GETARG_A(inst))  /* condition failed? */
        ci->u.l.savedpc++;  /* skip jump instruction */
      break;
    }
    case OP_CONCAT: {
      StkId top = L->top - 1;  /* top when 'luaT_trybinTM' was called */
      int b = GETARG_B(inst);      /* first element to concatenate */
      int total = cast_int(top - 1 - (base + b));  /* yet to concatenate */
      setobj2s(L, top - 2, top);  /* put TM result in proper position */
      if (total > 1) {  /* are there elements to concat? */
        L->top = top - 1;  /* top is one after last element (at top-2) */
        luaV_concat(L, total);  /* concat them (may yield again) */
      }
      /* move final result to final position */
      setobj2s(L, ci->u.l.base + GETARG_A(inst), L->top - 1);
      L->top = ci->top;  /* restore top */
      break;
    }
    case OP_TFORCALL: {
      lua_assert(GET_OPCODE(*ci->u.l.savedpc) == OP_TFORLOOP);
      L->top = ci->top;  /* correct top */
      break;
    }
    case OP_CALL: {
      if (GETARG_C(inst) - 1 >= 0)  /* nresults >= 0? */
        L->top = ci->top;  /* adjust results */
      break;
    }
    case OP_RAVI_SETI: case OP_RAVI_TABLE_SETFIELD: case OP_RAVI_SETFIELD:
    case OP_TAILCALL: case OP_SETTABUP: case OP_SETTABLE:
      break;
    default: lua_assert(0);
  }
}




/*
** {==================================================================
** Function 'luaV_execute': main interpreter loop
** ===================================================================
*/


/*
** some macros for common tasks in 'luaV_execute'
*/


#define RA(i)	(base+GETARG_A(i))
#define RB(i)	check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base+GETARG_B(i))
#define RC(i)	check_exp(getCMode(GET_OPCODE(i)) == OpArgR, base+GETARG_C(i))
#define RKB(i)	check_exp(getBMode(GET_OPCODE(i)) == OpArgK, \
	ISK(GETARG_B(i)) ? k+INDEXK(GETARG_B(i)) : base+GETARG_B(i))
#define RKC(i)	check_exp(getCMode(GET_OPCODE(i)) == OpArgK, \
	ISK(GETARG_C(i)) ? k+INDEXK(GETARG_C(i)) : base+GETARG_C(i))


#define updatemask(L)  (mask = L->hookmask & (LUA_MASKLINE | LUA_MASKCOUNT))
/*
** Execute a jump instruction. The 'updatemask' allows signals to stop
** tight loops. (Without it, the local copy of 'mask' could never change.)
*/
#define dojump(ci,i,e) \
  { int a = GETARG_A(i); \
    if (a != 0) luaF_close(L, ci->u.l.base + a - 1); \
    pc += GETARG_sBx(i) + e; updatemask(L); }

/* for test instructions, execute the jump instruction that follows it */
#define donextjump(ci)	{ i = *pc; dojump(ci, i, 1); }

/*
** Whenever code can raise errors (including memory errors), the global
** 'pc' must be correct to report occasional errors.
*/
#define savepc(L)	(ci->u.l.savedpc = pc)

/*
** Protect code that, in general, can raise errors, reallocate the
** stack, and change the hooks.
*/

#define Protect(code)  \
  { savepc(L); {code;}; base = ci->u.l.base; updatemask(L); }
#define Protect_base(code)  \
  { {code;}; base = ci->u.l.base; }

#define checkGC(L,c)  \
  { luaC_condGC(L, L->top = (c),  /* limit of live values */ \
                         Protect(L->top = ci->top));  /* restore top */ \
           luai_threadyield(L); }
#define checkGC_protectbase(L,c)  \
  { luaC_condGC(L, L->top = (c),  /* limit of live values */ \
                         Protect_base(L->top = ci->top));  /* restore top */ \
           luai_threadyield(L); }

#define checkGC_(L,c)  \
  { luaC_condGC(L, L->top = (c),  /* limit of live values */ \
                   L->top = ci->top);  /* restore top */ \
           luai_threadyield(L); }


#ifndef RAVI_USE_COMPUTED_GOTO

#if RAVI_BYTECODE_PROFILING_ENABLED
 /* fetch an instruction and prepare its execution */
#define vmfetch	{ \
  i = *(pc++); \
  if (RAVI_UNLIKELY(mask)) \
    Protect(luaG_traceexec(L)); \
  op = GET_OPCODE(i); \
  if (prevop != -1) raviV_add_profiledata(L, prevop); \
  prevop = op; \
  ra = RA(i); /* WARNING: any stack reallocation invalidates 'ra' */ \
}

#else
/* fetch an instruction and prepare its execution */
#define vmfetch	{ \
  i = *(pc++); \
  if (RAVI_UNLIKELY(mask)) \
    Protect(luaG_traceexec(L)); \
  op = GET_OPCODE(i); \
  ra = RA(i); /* WARNING: any stack reallocation invalidates 'ra' */ \
}

#endif

#define vmdispatch(o)	switch(o)
#define vmcase(l)	case l:
#define vmbreak		break

#else

#define vmlabel(l)   L_ ## l
#define vmcase(l)   vmlabel(l):

/* NOTE this is experimental. Maybe in this mode we should not allow hooks! */
#define vmfetch	{ \
  i = *(pc++); \
  if (RAVI_UNLIKELY(mask)) \
    Protect(luaG_traceexec(L)); \
  op = GET_OPCODE(i); \
  ra = RA(i); /* WARNING: any stack reallocation invalidates 'ra' */ \
  goto *dispatch_table[op]; \
}

#define vmdispatch(o) vmfetch
#define vmbreak vmfetch

#endif

/*
** copy of 'luaV_gettable', but protecting the call to potential
** metamethod (which can reallocate the stack)
*/
#define gettableProtected(L,t,k,v)  { const TValue *slot; \
  if (luaV_fastget(L,t,k,slot,luaH_get)) { setobj2s(L, v, slot); } \
  else Protect(luaV_finishget(L,t,k,v,slot)); }


/* same for 'luaV_settable' */
#define settableProtected(L,t,k,v) { const TValue *slot; \
  if (!luaV_fastset(L,t,k,slot,luaH_get,v)) \
    Protect(luaV_finishset(L,t,k,v,slot)); }


int raviV_check_usertype(lua_State *L, TString *name, const TValue *o)
{
  Table *mt;
  switch (ttnov(o)) {
  case LUA_TTABLE:
    mt = hvalue(o)->metatable;
    break;
  case LUA_TUSERDATA:
    mt = uvalue(o)->metatable;
    break;
  default:
    mt = NULL;
  }
  if (mt == NULL)
    return 0;
  /* get global table from registry */
  Table *reg = hvalue(&G(L)->l_registry);
  const TValue *metatab = luaH_getshortstr(reg, name);
  return (!ttisnil(metatab) && ttisLtable(metatab) && hvalue(metatab) == mt) || 0;
}

int luaV_execute (lua_State *L) {
  
#ifdef RAVI_USE_COMPUTED_GOTO
  static void* dispatch_table[] = {
    &&vmlabel(OP_MOVE),
    &&vmlabel(OP_LOADK),
    &&vmlabel(OP_LOADKX),
    &&vmlabel(OP_LOADBOOL),
    &&vmlabel(OP_LOADNIL),
    &&vmlabel(OP_GETUPVAL),
    &&vmlabel(OP_GETTABUP),
    &&vmlabel(OP_GETTABLE),
    &&vmlabel(OP_SETTABUP),
    &&vmlabel(OP_SETUPVAL),
    &&vmlabel(OP_SETTABLE),
    &&vmlabel(OP_NEWTABLE),
    &&vmlabel(OP_SELF),
    &&vmlabel(OP_ADD),
    &&vmlabel(OP_SUB),
    &&vmlabel(OP_MUL),
    &&vmlabel(OP_MOD),
    &&vmlabel(OP_POW),
    &&vmlabel(OP_DIV),
    &&vmlabel(OP_IDIV),
    &&vmlabel(OP_BAND),
    &&vmlabel(OP_BOR),
    &&vmlabel(OP_BXOR),
    &&vmlabel(OP_SHL),
    &&vmlabel(OP_SHR),
    &&vmlabel(OP_UNM),
    &&vmlabel(OP_BNOT),
    &&vmlabel(OP_NOT),
    &&vmlabel(OP_LEN),
    &&vmlabel(OP_CONCAT),
    &&vmlabel(OP_JMP),
    &&vmlabel(OP_EQ),
    &&vmlabel(OP_LT),
    &&vmlabel(OP_LE),
    &&vmlabel(OP_TEST),
    &&vmlabel(OP_TESTSET),
    &&vmlabel(OP_CALL),
    &&vmlabel(OP_TAILCALL),
    &&vmlabel(OP_RETURN),
    &&vmlabel(OP_FORLOOP),
    &&vmlabel(OP_FORPREP),
    &&vmlabel(OP_TFORCALL),
    &&vmlabel(OP_TFORLOOP),
    &&vmlabel(OP_SETLIST),
    &&vmlabel(OP_CLOSURE),
    &&vmlabel(OP_VARARG),
    &&vmlabel(OP_EXTRAARG),
    &&vmlabel(OP_RAVI_NEW_IARRAY),
    &&vmlabel(OP_RAVI_NEW_FARRAY),
    &&vmlabel(OP_RAVI_LOADIZ),
    &&vmlabel(OP_RAVI_LOADFZ),
    &&vmlabel(OP_RAVI_UNMF),
    &&vmlabel(OP_RAVI_UNMI),
    &&vmlabel(OP_RAVI_ADDFF),
    &&vmlabel(OP_RAVI_ADDFI),
    &&vmlabel(OP_RAVI_ADDII),
    &&vmlabel(OP_RAVI_SUBFF),
    &&vmlabel(OP_RAVI_SUBFI),
    &&vmlabel(OP_RAVI_SUBIF),
    &&vmlabel(OP_RAVI_SUBII),
    &&vmlabel(OP_RAVI_MULFF),
    &&vmlabel(OP_RAVI_MULFI),
    &&vmlabel(OP_RAVI_MULII),
    &&vmlabel(OP_RAVI_DIVFF),
    &&vmlabel(OP_RAVI_DIVFI),
    &&vmlabel(OP_RAVI_DIVIF),
    &&vmlabel(OP_RAVI_DIVII),
    &&vmlabel(OP_RAVI_TOINT),
    &&vmlabel(OP_RAVI_TOFLT),
    &&vmlabel(OP_RAVI_TOIARRAY),
    &&vmlabel(OP_RAVI_TOFARRAY),
    &&vmlabel(OP_RAVI_TOTAB),
    &&vmlabel(OP_RAVI_TOSTRING),
    &&vmlabel(OP_RAVI_TOCLOSURE),
    &&vmlabel(OP_RAVI_TOTYPE),
    &&vmlabel(OP_RAVI_MOVEI),
    &&vmlabel(OP_RAVI_MOVEF),
    &&vmlabel(OP_RAVI_MOVEIARRAY),
    &&vmlabel(OP_RAVI_MOVEFARRAY),
    &&vmlabel(OP_RAVI_MOVETAB),
    &&vmlabel(OP_RAVI_IARRAY_GET),
    &&vmlabel(OP_RAVI_FARRAY_GET),
    &&vmlabel(OP_RAVI_IARRAY_SET),
    &&vmlabel(OP_RAVI_FARRAY_SET),
    &&vmlabel(OP_RAVI_IARRAY_SETI),
    &&vmlabel(OP_RAVI_FARRAY_SETF),
    &&vmlabel(OP_RAVI_FORLOOP_IP),
    &&vmlabel(OP_RAVI_FORLOOP_I1),
    &&vmlabel(OP_RAVI_FORPREP_IP),
    &&vmlabel(OP_RAVI_FORPREP_I1),
    &&vmlabel(OP_RAVI_SETUPVALI),
    &&vmlabel(OP_RAVI_SETUPVALF),
    &&vmlabel(OP_RAVI_SETUPVAL_IARRAY),
    &&vmlabel(OP_RAVI_SETUPVAL_FARRAY),
    &&vmlabel(OP_RAVI_SETUPVALT),
    &&vmlabel(OP_RAVI_BAND_II),
    &&vmlabel(OP_RAVI_BOR_II),
    &&vmlabel(OP_RAVI_BXOR_II),
    &&vmlabel(OP_RAVI_SHL_II),
    &&vmlabel(OP_RAVI_SHR_II),
    &&vmlabel(OP_RAVI_BNOT_I),
    &&vmlabel(OP_RAVI_EQ_II),
    &&vmlabel(OP_RAVI_EQ_FF),
    &&vmlabel(OP_RAVI_LT_II),
    &&vmlabel(OP_RAVI_LT_FF),
    &&vmlabel(OP_RAVI_LE_II),
    &&vmlabel(OP_RAVI_LE_FF),
    &&vmlabel(OP_RAVI_TABLE_GETFIELD),
    &&vmlabel(OP_RAVI_TABLE_SETFIELD),
    &&vmlabel(OP_RAVI_TABLE_SELF_SK),
    &&vmlabel(OP_RAVI_GETI),
    &&vmlabel(OP_RAVI_SETI),
    &&vmlabel(OP_RAVI_GETFIELD),
    &&vmlabel(OP_RAVI_SELF_SK),
    &&vmlabel(OP_RAVI_SETFIELD),
    &&vmlabel(OP_RAVI_GETTABUP_SK)
  };
#endif
  
  CallInfo *ci = L->ci;
  LClosure *cl;
  TValue *k;
  StkId base;
  int mask;  /* local copy of 'L->hookmask & (LUA_MASKLINE | LUA_MASKCOUNT)' */
  const Instruction *pc;  /* local copy of 'ci->u.l.savedpc' */
  ci->callstatus |= CIST_FRESH;  /* fresh invocation of 'luaV_execute" */
 newframe:  /* reentry point when frame changes (call/return) */
  lua_assert(ci == L->ci);
  cl = clLvalue(ci->func);  /* local reference to function's closure */
  k = cl->p->k;  /* local reference to function's constant table */
  updatemask(L);
  base = ci->u.l.base;  /* local copy of function's base */
  pc = ci->u.l.savedpc;
  /* main loop of interpreter */
#if RAVI_BYTECODE_PROFILING_ENABLED
  OpCode prevop = -1;
#endif
  for (;;) {
    Instruction i;
    OpCode op;
    StkId ra;

    vmfetch;
    vmdispatch(op) {
      vmcase(OP_MOVE) {
        setobjs2s(L, ra, RB(i));
        vmbreak;
      }
      vmcase(OP_LOADK) {
        TValue *rb = k + GETARG_Bx(i);
        setobj2s(L, ra, rb);
        vmbreak;
      }
      vmcase(OP_LOADKX) {
        TValue *rb;
        lua_assert(GET_OPCODE(*pc) == OP_EXTRAARG);
        rb = k + GETARG_Ax(*pc++);
        setobj2s(L, ra, rb);
        vmbreak;
      }
      vmcase(OP_LOADBOOL) {
        setbvalue(ra, GETARG_B(i));
        if (GETARG_C(i)) pc++;  /* skip next instruction (if C) */
        vmbreak;
      }
      vmcase(OP_LOADNIL) {
        int b = GETARG_B(i);
        do {
          setnilvalue(ra++);
        } while (b--);
        vmbreak;
      }
      vmcase(OP_GETUPVAL) {
        int b = GETARG_B(i);
        setobj2s(L, ra, cl->upvals[b]->v);
        vmbreak;
      }
      vmcase(OP_SETUPVAL) {
        UpVal *uv = cl->upvals[GETARG_B(i)];
        setobj(L, uv->v, ra);
        luaC_upvalbarrier(L, uv);
        vmbreak;
      }
      vmcase(OP_GETTABUP) {
        TValue *upval = cl->upvals[GETARG_B(i)]->v;    /* table */
        TValue *rc = RKC(i);                           /* key */
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
        vmbreak;
      }
      vmcase(OP_GETTABLE) {
        StkId rb = RB(i);                              /* table */
        TValue *rc = RKC(i);                           /* key */
        GETTABLE_INLINE_PROTECTED(L, rb, rc, ra);
        vmbreak;
      }
      vmcase(OP_SETTABUP) {
        TValue *upval = cl->upvals[GETARG_A(i)]->v;
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
        vmbreak;
      }
      vmcase(OP_SETTABLE) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
        vmbreak;
      }
      vmcase(OP_RAVI_SETI) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
        vmbreak;
      }
      vmcase(OP_RAVI_SETFIELD)
      vmcase(OP_RAVI_TABLE_SETFIELD) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        SETTABLE_INLINE_SSKEY_PROTECTED(L, ra, rb, rc);
        vmbreak;
      }
      vmcase(OP_NEWTABLE) {
        int b = GETARG_B(i);
        int c = GETARG_C(i);
        Table *t;
        savepc(L);  /* in case of allocation errors */
        t = luaH_new(L);
        sethvalue(L, ra, t);
        if (b != 0 || c != 0)
          luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
        checkGC(L, ra + 1);
        vmbreak;
      }
      vmcase(OP_RAVI_SELF_SK) {
        StkId rb = RB(i); /* variable - may not be a table */
        /* we know that the key a short string constant */
        TValue *rc = RKC(i);
        setobjs2s(L, ra + 1, rb);
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
        vmbreak;
      }
      vmcase(OP_SELF) {
        const TValue *aux;
        StkId rb = RB(i);
        TValue *rc = RKC(i);
        TString *key = tsvalue(rc);  /* key must be a string */
        setobjs2s(L, ra + 1, rb);
        if (luaV_fastget(L, rb, key, aux, luaH_getstr)) {
          setobj2s(L, ra, aux);
        }
        else Protect(luaV_finishget(L, rb, rc, ra, aux));
        vmbreak;
      }
      vmcase(OP_ADD) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Number nb; lua_Number nc;
        if (ttisinteger(rb) && ttisinteger(rc)) {
          lua_Integer ib = ivalue(rb); lua_Integer ic = ivalue(rc);
          setivalue(ra, intop(+, ib, ic));
        }
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
          setfltvalue(ra, luai_numadd(L, nb, nc));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_ADD)); }
        vmbreak;
      }
      vmcase(OP_SUB) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Number nb; lua_Number nc;
        if (ttisinteger(rb) && ttisinteger(rc)) {
          lua_Integer ib = ivalue(rb); lua_Integer ic = ivalue(rc);
          setivalue(ra, intop(-, ib, ic));
        }
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
          setfltvalue(ra, luai_numsub(L, nb, nc));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_SUB)); }
        vmbreak;
      }
      vmcase(OP_MUL) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Number nb; lua_Number nc;
        if (ttisinteger(rb) && ttisinteger(rc)) {
          lua_Integer ib = ivalue(rb); lua_Integer ic = ivalue(rc);
          setivalue(ra, intop(*, ib, ic));
        }
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
          setfltvalue(ra, luai_nummul(L, nb, nc));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_MUL)); }
        vmbreak;
      }
      vmcase(OP_DIV) {  /* float division (always with floats) */
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Number nb; lua_Number nc;
        if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
          setfltvalue(ra, luai_numdiv(L, nb, nc));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_DIV)); }
        vmbreak;
      }
      vmcase(OP_BAND) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Integer ib; lua_Integer ic;
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
          setivalue(ra, intop(&, ib, ic));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_BAND)); }
        vmbreak;
      }
      vmcase(OP_BOR) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Integer ib; lua_Integer ic;
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
          setivalue(ra, intop(|, ib, ic));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_BOR)); }
        vmbreak;
      }
      vmcase(OP_BXOR) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Integer ib; lua_Integer ic;
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
          setivalue(ra, intop(^, ib, ic));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_BXOR)); }
        vmbreak;
      }
      vmcase(OP_SHL) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Integer ib; lua_Integer ic;
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
          setivalue(ra, luaV_shiftl(ib, ic));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_SHL)); }
        vmbreak;
      }
      vmcase(OP_SHR) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Integer ib; lua_Integer ic;
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
          setivalue(ra, luaV_shiftl(ib, -ic));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_SHR)); }
        vmbreak;
      }
      vmcase(OP_MOD) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Number nb; lua_Number nc;
        if (ttisinteger(rb) && ttisinteger(rc)) {
          lua_Integer ib = ivalue(rb); lua_Integer ic = ivalue(rc);
          setivalue(ra, luaV_mod(L, ib, ic));
        }
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
          lua_Number m;
          luai_nummod(L, nb, nc, m);
          setfltvalue(ra, m);
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_MOD)); }
        vmbreak;
      }
      vmcase(OP_IDIV) {  /* floor division */
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Number nb; lua_Number nc;
        if (ttisinteger(rb) && ttisinteger(rc)) {
          lua_Integer ib = ivalue(rb); lua_Integer ic = ivalue(rc);
          setivalue(ra, luaV_div(L, ib, ic));
        }
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
          setfltvalue(ra, luai_numidiv(L, nb, nc));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_IDIV)); }
        vmbreak;
      }
      vmcase(OP_POW) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Number nb; lua_Number nc;
        if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
          setfltvalue(ra, luai_numpow(L, nb, nc));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_POW)); }
        vmbreak;
      }
      vmcase(OP_UNM) {
        TValue *rb = RB(i);
        lua_Number nb;
        if (ttisinteger(rb)) {
          lua_Integer ib = ivalue(rb);
          setivalue(ra, intop(-, 0, ib));
        }
        else if (tonumberns(rb, nb)) {
          setfltvalue(ra, luai_numunm(L, nb));
        }
        else {
          Protect(luaT_trybinTM(L, rb, rb, ra, TM_UNM));
        }
        vmbreak;
      }
      vmcase(OP_BNOT) {
        TValue *rb = RB(i);
        lua_Integer ib;
        if (tointegerns(rb, &ib)) {
          setivalue(ra, intop(^, ~l_castS2U(0), ib));
        }
        else {
          Protect(luaT_trybinTM(L, rb, rb, ra, TM_BNOT));
        }
        vmbreak;
      }
      vmcase(OP_NOT) {
        TValue *rb = RB(i);
        int res = l_isfalse(rb);  /* next assignment may change this value */
        setbvalue(ra, res);
        vmbreak;
      }
      vmcase(OP_LEN) {
        Protect(luaV_objlen(L, ra, RB(i)));
        vmbreak;
      }
      vmcase(OP_CONCAT) {
        int b = GETARG_B(i);
        int c = GETARG_C(i);
        StkId rb;
        L->top = base + c + 1;  /* mark the end of concat operands */
        Protect(luaV_concat(L, c - b + 1));
        ra = RA(i);  /* 'luaV_concat' may invoke TMs and move the stack */
        rb = base + b;
        setobjs2s(L, ra, rb);
        checkGC(L, (ra >= rb ? ra + 1 : rb));
        L->top = ci->top;  /* restore top */
        vmbreak;
      }
      vmcase(OP_JMP) {
        dojump(ci, i, 0);
        vmbreak;
      }
      vmcase(OP_EQ) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        int res;
        Protect(
          res = luaV_equalobj(L, rb, rc);
        )
        if (res != GETARG_A(i))
          pc++;
        else
          donextjump(ci);
        vmbreak;
      }
      vmcase(OP_LT) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        int res;
        if (ttisinteger(rb) && ttisinteger(rc))
          res = (ivalue(rb) < ivalue(rc));
        else if (ttisnumber(rb) && ttisnumber(rc))
          res = LTnum(rb, rc);
        else Protect(
          res = luaV_lessthan(L, rb, rc);
        )
        if (res != GETARG_A(i))
          pc++;
        else
          donextjump(ci);
        vmbreak;
      }
      vmcase(OP_LE) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        int res;
        if (ttisinteger(rb) && ttisinteger(rc))
          res = (ivalue(rb) <= ivalue(rc));
        else if (ttisnumber(rb) && ttisnumber(rc))
          res = LEnum(rb, rc);
        else Protect(
          res = luaV_lessequal(L, rb, rc);
        )
        if (res != GETARG_A(i))
          pc++;
        else
          donextjump(ci);
        vmbreak;
      }
      vmcase(OP_TEST) {
        if (GETARG_C(i) ? l_isfalse(ra) : !l_isfalse(ra))
          pc++;
        else
          donextjump(ci);
        vmbreak;
      }
      vmcase(OP_TESTSET) {
        TValue *rb = RB(i);
        if (GETARG_C(i) ? l_isfalse(rb) : !l_isfalse(rb))
          pc++;
        else {
          setobjs2s(L, ra, rb);
          donextjump(ci);
        }
        vmbreak;
      }
      vmcase(OP_CALL) {
        int b = GETARG_B(i);
        int nresults = GETARG_C(i) - 1;
        if (b != 0)  /* fixed number of arguments? */
          L->top = ra+b;  /* top signals number of arguments */
        /* else previous instruction set top */ 
        /*
        See note below under OP_RETURN for why we pass the extra
        argument to luaD_precall() - it is basicaly to tell it that it
        was called from OP_CALL instruction
        */
        int c_or_compiled = 0;
        Protect(c_or_compiled = luaD_precall(L, ra, nresults, 1 /* OP_CALL */));
        if (c_or_compiled) { /* C or Lua JITed function? */
          /* RAVI change - if the Lua function was JIT compiled then
           * luaD_precall() returns 2
           * A return value of 1 indicates non Lua C function
           */
          if (c_or_compiled == 1 && nresults >= 0) {
            lua_assert(ci == L->ci);
            L->top = ci->top; /* adjust results */
          }
          // Protect((void)0);  /* update 'base' */
        }
        else { /* Lua function */
          ci = L->ci;
          lua_assert(!ci->jitstatus);
          goto newframe; /* restart luaV_execute over new Lua function */
        }
        vmbreak;
      }
      vmcase(OP_TAILCALL) {
        int b = GETARG_B(i);
        if (b != 0) L->top = ra+b;  /* else previous instruction set top */
        lua_assert(GETARG_C(i) - 1 == LUA_MULTRET);
        savepc(L);
        /* See note below under OP_RETURN for why we pass the extra
           argument to luaD_precall() - it is basicaly to tell it that it
           was called from OP_CALL instruction
           */
        if (luaD_precall(L, ra, LUA_MULTRET,
                         1 /* OP_CALL */)) { /* C function? or JIT call */
          Protect((void)0);  /* update 'base' */
        }
        else {
          /* tail call: put called frame (n) in place of caller one (o) */
          CallInfo *nci = L->ci;  /* called frame */
          CallInfo *oci = nci->previous;  /* caller frame */
          StkId nfunc = nci->func;  /* called function */
          StkId ofunc = oci->func;  /* caller function */
          /* last stack slot filled by 'precall' */
          StkId lim = nci->u.l.base + getproto(nfunc)->numparams;
          int aux;
          /* close all upvalues from previous call */
          if (cl->p->sizep > 0) luaF_close(L, oci->u.l.base);
          /* move new frame into old one */
          for (aux = 0; nfunc + aux < lim; aux++)
            setobjs2s(L, ofunc + aux, nfunc + aux);
          oci->u.l.base = ofunc + (nci->u.l.base - nfunc);  /* correct base */
          oci->top = L->top = ofunc + (L->top - nfunc);  /* correct top */
          oci->u.l.savedpc = nci->u.l.savedpc;
          oci->callstatus |= CIST_TAIL;  /* function was tail called */
          oci->jitstatus = 0;
          ci = L->ci = oci;  /* remove new frame */
          lua_assert(L->top == oci->u.l.base + getproto(ofunc)->maxstacksize);
          goto newframe;  /* restart luaV_execute over new Lua function */
        }
        vmbreak;
      }
      vmcase(OP_RETURN) {
        int b = GETARG_B(i);
        if (cl->p->sizep > 0) luaF_close(L, base);
        savepc(L);
        int nres = (b != 0 ? b - 1 : cast_int(L->top - ra));
        b = luaD_poscall(L, ci, ra, nres);
        if (ci->callstatus & CIST_FRESH) /* 'ci' still the called one */ {
          /* Lua VM assumes that this case is only
             executed when luaV_execute() is called externally
             i.e. not via OP_CALL, but in JIT mode this is not true
             as luaV_execute() will be called even from OP_CALL when
             a particular function cannot be compiled. So we need
             to somehow trigger the reset of L->top in this case.
             Since luaV_execute is called from various places it is
             more convenient to let the caller decide what to do -
             so we simply return b here. The caller is either OP_CALL
             in JIT mode (see how b is handled in OP_CALL JIT implementation)
             or via luaD_precall() if a JITed function is invoked (see
             ldo.c for how luaD_precall() handles this */
#if RAVI_BYTECODE_PROFILING_ENABLED
          raviV_add_profiledata(L, op);
#endif
          return b; /* external invocation: return */
        }
        else {  /* invocation via reentry: continue execution */
          ci = L->ci;
          if (b) L->top = ci->top;
          lua_assert(isLua(ci));
          lua_assert(GET_OPCODE(*((ci)->u.l.savedpc - 1)) == OP_CALL);
#if RAVI_BYTECODE_PROFILING_ENABLED
          raviV_add_profiledata(L, op);
#endif
          goto newframe;  /* restart luaV_execute over new Lua function */
        }
      }
      vmcase(OP_FORLOOP) {
        if (RAVI_LIKELY(ttisinteger(ra))) {  /* integer loop? */
          lua_Integer step = ivalue(ra + 2);
          lua_Integer idx = intop(+, ivalue(ra), step); /* increment index */
          lua_Integer limit = ivalue(ra + 1);
          if (RAVI_LIKELY((0 < step)) ? (idx <= limit) : (limit <= idx)) {
            pc += GETARG_sBx(i);  /* jump back */
            chgivalue(ra, idx);  /* update internal index... */
            setivalue(ra + 3, idx);  /* ...and external index */
          }
        }
        else {  /* floating loop */
          lua_Number step = fltvalue(ra + 2);
          lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
          lua_Number limit = fltvalue(ra + 1);
          if (luai_numlt(0, step) ? luai_numle(idx, limit)
                                  : luai_numle(limit, idx)) {
            pc += GETARG_sBx(i);  /* jump back */
            chgfltvalue(ra, idx);  /* update internal index... */
            setfltvalue(ra + 3, idx);  /* ...and external index */
          }
        }
        vmbreak;
      }
      vmcase(OP_FORPREP) {
        TValue *init = ra;
        TValue *plimit = ra + 1;
        TValue *pstep = ra + 2;
        lua_Integer ilimit;
        int stopnow;
        if (ttisinteger(init) && ttisinteger(pstep) &&
            luaV_forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {
          /* all values are integer */
          lua_Integer initv = (stopnow ? 0 : ivalue(init));
          setivalue(plimit, ilimit);
          setivalue(init, intop(-, initv, ivalue(pstep)));
        }
        else {  /* try making all values floats */
          lua_Number ninit; lua_Number nlimit; lua_Number nstep;
          savepc(L);  /* in case of errors */
          if (!tonumber(plimit, &nlimit))
            luaG_runerror(L, "'for' limit must be a number");
          setfltvalue(plimit, nlimit);
          if (!tonumber(pstep, &nstep))
            luaG_runerror(L, "'for' step must be a number");
          setfltvalue(pstep, nstep);
          if (!tonumber(init, &ninit))
            luaG_runerror(L, "'for' initial value must be a number");
          setfltvalue(init, luai_numsub(L, ninit, nstep));
        }
        pc += GETARG_sBx(i);
        vmbreak;
      }
      vmcase(OP_TFORCALL) {
        StkId cb = ra + 3;  /* call base */
        setobjs2s(L, cb+2, ra+2);
        setobjs2s(L, cb+1, ra+1);
        setobjs2s(L, cb, ra);
        L->top = cb + 3;  /* func. + 2 args (state and index) */
        Protect(luaD_call(L, cb, GETARG_C(i)));
        L->top = ci->top;
        i = *(pc++);  /* go to next instruction */
        ra = RA(i);
        lua_assert(GET_OPCODE(i) == OP_TFORLOOP);
        goto l_tforloop;
      }
      vmcase(OP_TFORLOOP) {
        l_tforloop:
        if (!ttisnil(ra + 1)) {  /* continue loop? */
          setobjs2s(L, ra, ra + 1);  /* save control variable */
          pc += GETARG_sBx(i);  /* jump back */
        }
        vmbreak;
      }
      vmcase(OP_SETLIST) {
        int n = GETARG_B(i);
        int c = GETARG_C(i);
        unsigned int last;
        Table *h;
        if (n == 0) n = cast_int(L->top - ra) - 1;
        if (c == 0) {
          lua_assert(GET_OPCODE(*pc) == OP_EXTRAARG);
          c = GETARG_Ax(*pc++);
        }
        h = hvalue(ra);
        last = ((c-1)*LFIELDS_PER_FLUSH) + n;
        savepc(L);  /* in case of allocation errors */
        if (h->ravi_array.array_type == RAVI_TTABLE) {
          if (last > h->sizearray)  /* needs more space? */
            luaH_resizearray(L, h, last);  /* pre-allocate it at once */
          for (; n > 0; n--) {
            TValue *val = ra + n;
            luaH_setint(L, h, last--, val);
            luaC_barrierback(L, h, val);
          }
        }
        else {
          int i = last - n + 1;
          for (; i <= (int)last; i++) {
            TValue *val = ra + i;
            unsigned int u = (unsigned int)(i);
            switch (h->ravi_array.array_type) {
              case RAVI_TARRAYINT: {
                if (ttisinteger(val)) {
                  raviH_set_int_inline(L, h, u, ivalue(val));
                }
                else {
                  lua_Integer i = 0;
                  if (luaV_tointeger_(val, &i)) {
                    raviH_set_int_inline(L, h, u, i);
                  }
                  else
                    luaG_runerror(L, "value cannot be converted to integer");
                }
              } break;
              case RAVI_TARRAYFLT: {
                if (ttisfloat(val)) {
                  raviH_set_float_inline(L, h, u, fltvalue(val));
                }
                else if (ttisinteger(val)) {
                  raviH_set_float_inline(L, h, u, (lua_Number)(ivalue(val)));
                }
                else {
                  lua_Number d = 0.0;
                  if (luaV_tonumber_(val, &d)) {
                    raviH_set_float_inline(L, h, u, d);
                  }
                  else
                    luaG_runerror(L, "value cannot be converted to number");
                }
              } break;
              default: lua_assert(0);
            }
          }
        }
        L->top = ci->top; /* correct top (in case of previous open call) */
        vmbreak;
      }
      vmcase(OP_CLOSURE) {
        Proto *p = cl->p->p[GETARG_Bx(i)];
        LClosure *ncl = getcached(p, cl->upvals, base);  /* cached closure */
        if (ncl == NULL) { /* no match? */
          savepc(L);  /* in case of allocation errors */
          pushclosure(L, p, cl->upvals, base, ra);  /* create a new one */
        }
        else
          setclLvalue(L, ra, ncl);  /* push cashed closure */
        checkGC(L, ra + 1);
        vmbreak;
      }
      vmcase(OP_VARARG) {
        int b = GETARG_B(i) - 1;  /* required results */
        int j;
        int n = cast_int(base - ci->func) - cl->p->numparams - 1;
        if (n < 0)  /* less arguments than parameters? */
          n = 0;  /* no vararg arguments */
        if (b < 0) {  /* B == 0? */
          b = n;  /* get all var. arguments */
          Protect(luaD_checkstack(L, n));
          ra = RA(i);  /* previous call may change the stack */
          L->top = ra + n;
        }
        for (j = 0; j < b && j < n; j++)
          setobjs2s(L, ra + j, base - n + j);
        for (; j < b; j++)  /* complete required results with nil */
          setnilvalue(ra + j);
        vmbreak;
      }
      vmcase(OP_EXTRAARG) {
        lua_assert(0);
        vmbreak;
      }
      vmcase(OP_RAVI_BAND_II) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setivalue(ra, intop(&, ivalue(rb), ivalue(rc)));
        vmbreak;
      }
      vmcase(OP_RAVI_BOR_II) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setivalue(ra, intop(| , ivalue(rb), ivalue(rc)));
        vmbreak;
      }
      vmcase(OP_RAVI_BXOR_II) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setivalue(ra, intop(^, ivalue(rb), ivalue(rc)));
        vmbreak;
      }
      vmcase(OP_RAVI_SHL_II) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setivalue(ra, luaV_shiftl(ivalue(rb), ivalue(rc)));
        vmbreak;
      }
      vmcase(OP_RAVI_SHR_II) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Integer ic = ivalue(rc);
        setivalue(ra, luaV_shiftl(ivalue(rb), -ic));
        vmbreak;
      }
      vmcase(OP_RAVI_BNOT_I) {
        /* On Win32 the following code generates a test failure
        * at line 29 of bitwise.lua test. Specifically following fails:
        * function x()
        *   local a= 0xF0000000
        *   local b=~a
        *   local c=~b
        *   local d=~~a
        *   print(a,b,c,d)
        *   print(~~a)
        * end
        * Inserting a prinf statement following the assignment to ib appears
        * to cause the problem to go away so this is a case of incorrect
        * optimization / code generation?
        * To work around this issue, for now we can disable the type
        * inference for OP_BNOT in line 1129 of lcode.c (codeexpval function)
        */
        TValue *rb = RB(i);
        lua_Integer ib = ivalue(rb);
        setivalue(ra, intop(^, ~l_castS2U(0), ib));
        vmbreak;
      }
      vmcase(OP_RAVI_EQ_II) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        int equals = (ivalue(rb) == ivalue(rc));
        if (equals != GETARG_A(i))
          pc++;
        else
          donextjump(ci);
        vmbreak;
      }
      vmcase(OP_RAVI_EQ_FF) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        int equals = (fltvalue(rb) == fltvalue(rc));
        if (equals != GETARG_A(i))
          pc++;
        else
          donextjump(ci);
        vmbreak;
      }
      vmcase(OP_RAVI_LT_II) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        int lessthan = (ivalue(rb) < ivalue(rc));
        if (lessthan != GETARG_A(i))
          pc++;
        else
          donextjump(ci);
        vmbreak;
      }
      vmcase(OP_RAVI_LT_FF) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        int lessthan = (fltvalue(rb) < fltvalue(rc));
        if (lessthan != GETARG_A(i))
          pc++;
        else
          donextjump(ci);
        vmbreak;
      }
      vmcase(OP_RAVI_LE_II) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        int lessequals = (ivalue(rb) <= ivalue(rc));
        if (lessequals != GETARG_A(i))
          pc++;
        else
          donextjump(ci);
        vmbreak;
      }
      vmcase(OP_RAVI_LE_FF) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        int lessequals = (fltvalue(rb) <= fltvalue(rc));
        if (lessequals != GETARG_A(i))
          pc++;
        else
          donextjump(ci);
        vmbreak;
      }
      vmcase(OP_RAVI_FORLOOP_IP)
      vmcase(OP_RAVI_FORLOOP_I1) {
        lua_Integer step = op == OP_RAVI_FORLOOP_I1 ? 1 : ivalue(ra + 2);
        lua_Integer idx = ivalue(ra) + step; /* increment index */
        lua_Integer limit = ivalue(ra + 1);
        if (idx <= limit) {
          pc += GETARG_sBx(i); /* jump back */
          chgivalue(ra, idx);               /* update internal index... */
          setivalue(ra + 3, idx);           /* ...and external index */
        }
        vmbreak;
      }
      vmcase(OP_RAVI_FORPREP_IP)
      vmcase(OP_RAVI_FORPREP_I1) {
        TValue *pinit = ra;
        //TValue *plimit = ra + 1;
        TValue *pstep = RAVI_LIKELY((op == OP_RAVI_FORPREP_I1)) ? NULL : ra + 2;
        //lua_Integer ilimit = ivalue(plimit);
        lua_Integer initv = ivalue(pinit);
        lua_Integer istep = RAVI_LIKELY((op == OP_RAVI_FORPREP_I1)) ? 1 : ivalue(pstep);
        //setivalue(plimit, ilimit);
        setivalue(pinit, initv - istep);
        pc += GETARG_sBx(i);
        vmbreak;
      }
      vmcase(OP_RAVI_NEW_IARRAY) {
        Table *t;
        savepc(L);  /* in case of allocation errors */
        t = raviH_new(L, RAVI_TARRAYINT, 0);
        setiarrayvalue(L, ra, t);
        checkGC(L, ra + 1);
        vmbreak;
      }
      vmcase(OP_RAVI_NEW_FARRAY) {
        Table *t;
        savepc(L);  /* in case of allocation errors */
        t = raviH_new(L, RAVI_TARRAYFLT, 0);
        setfarrayvalue(L, ra, t);
        checkGC(L, ra + 1);
        vmbreak;
      }
      vmcase(OP_RAVI_GETI) {
        TValue *rb = RB(i);
        TValue *rc = RKC(i);
        GETTABLE_INLINE_PROTECTED_I(L, rb, rc, ra);
        vmbreak;
      }
      /* This opcode is used when the key is known to be
         short string but the variable may or may not be
         a table
      */
      vmcase(OP_RAVI_GETTABUP_SK) {
        StkId rb = cl->upvals[GETARG_B(i)]->v; /* variable - may not be a table */
        lua_assert(ISK(GETARG_C(i)));
        /* we know that the key a short string constant */
        TValue *rc = k + INDEXK(GETARG_C(i));
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
        vmbreak;
      }
      vmcase(OP_RAVI_GETFIELD) {
        StkId rb = RB(i); /* variable - may not be a table */
        lua_assert(ISK(GETARG_C(i)));
        /* we know that the key a short string constant */
        TValue *rc = k + INDEXK(GETARG_C(i));
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
        vmbreak;
      }
      vmcase(OP_RAVI_TABLE_SELF_SK)
      vmcase(OP_RAVI_TABLE_GETFIELD) {
        /* This opcode is used when the key is known to be
           short string and the variable is known to be
           a table
        */
        StkId rb = RB(i);
        if (op == OP_RAVI_TABLE_SELF_SK) { setobjs2s(L, ra + 1, rb); }
        {
          lua_assert(ISK(GETARG_C(i)));
          TValue *rc = k + INDEXK(GETARG_C(i));
          TString *key = tsvalue(rc);
          lua_assert(key->tt == LUA_TSHRSTR);
          Table *h = hvalue(rb);
          const TValue *v = luaH_getshortstr(h, key);
          if (RAVI_LIKELY(!ttisnil(v))) {
            setobj2s(L, ra, v);
          }
          else {
            Protect(luaV_finishget(L, rb, rc, ra, v));
          }
        }
        vmbreak;
      }
      vmcase(OP_RAVI_IARRAY_GET) {
        TValue *rb = RB(i);
        TValue *rc = RKC(i);
        lua_Integer idx = ivalue(rc);
        Table *t = hvalue(rb);
        raviH_get_int_inline(L, t, idx, ra);
        vmbreak;
      }
      vmcase(OP_RAVI_FARRAY_GET) {
        TValue *rb = RB(i);
        TValue *rc = RKC(i);
        lua_Integer idx = ivalue(rc);
        Table *t = hvalue(rb);
        raviH_get_float_inline(L, t, idx, ra);
        vmbreak;
      }
      vmcase(OP_RAVI_IARRAY_SET) {
        Table *t = hvalue(ra);
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Integer idx = ivalue(rb);
        if (ttisinteger(rc)) { raviH_set_int_inline(L, t, idx, ivalue(rc)); }
        else if (ttisfloat(rc)) {
          raviH_set_int_inline(L, t, idx, (lua_Integer)fltvalue(rc));
        }
        else {
          lua_Integer j;
          if (tointeger(rc, &j)) { raviH_set_int_inline(L, t, idx, j); }
          else
            luaG_runerror(L, "integer expected");
        }
        vmbreak;
      }
      vmcase(OP_RAVI_IARRAY_SETI) {
        Table *t = hvalue(ra);
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Integer idx = ivalue(rb);
        raviH_set_int_inline(L, t, idx, ivalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_FARRAY_SET) {
        Table *t = hvalue(ra);
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Integer idx = ivalue(rb);
        if (ttisfloat(rc)) { raviH_set_float_inline(L, t, idx, fltvalue(rc)); }
        else if (ttisinteger(rc)) {
          raviH_set_float_inline(L, t, idx, ((lua_Number)ivalue(rc)));
        }
        else {
          lua_Number j;
          if (tonumber(rc, &j)) { raviH_set_float_inline(L, t, idx, j); }
          else
            luaG_runerror(L, "number expected");
        }
        vmbreak;
      }
      vmcase(OP_RAVI_FARRAY_SETF) {
        Table *t = hvalue(ra);
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Integer idx = ivalue(rb);
        raviH_set_float_inline(L, t, idx, fltvalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_SETUPVALI) {
        lua_Integer ia;
        if (tointeger(ra, &ia)) {
          UpVal *uv = cl->upvals[GETARG_B(i)];
          setivalue(uv->v, ia);
          luaC_upvalbarrier(L, uv);
        }
        else
          luaG_runerror(
            L, "upvalue of integer type, cannot be set to non integer value");
        vmbreak;
      }
      vmcase(OP_RAVI_SETUPVALF) {
        lua_Number na;
        if (tonumber(ra, &na)) {
          UpVal *uv = cl->upvals[GETARG_B(i)];
          setfltvalue(uv->v, na);
          luaC_upvalbarrier(L, uv);
        }
        else
          luaG_runerror(
            L, "upvalue of number type, cannot be set to non number value");
        vmbreak;
      }
      vmcase(OP_RAVI_SETUPVAL_IARRAY) {
        if (!ttisiarray(ra))
          luaG_runerror(L,
            "upvalue of integer[] type, cannot be set to non "
            "integer[] value");
        UpVal *uv = cl->upvals[GETARG_B(i)];
        setobj(L, uv->v, ra);
        luaC_upvalbarrier(L, uv);
        vmbreak;
      }
      vmcase(OP_RAVI_SETUPVAL_FARRAY) {
        if (!ttisfarray(ra))
          luaG_runerror(
            L,
            "upvalue of number[] type, cannot be set to non number[] value");
        UpVal *uv = cl->upvals[GETARG_B(i)];
        setobj(L, uv->v, ra);
        luaC_upvalbarrier(L, uv);
        vmbreak;
      }
      vmcase(OP_RAVI_SETUPVALT) {
        if (!ttisLtable(ra))
          luaG_runerror(
            L, "upvalue of table type, cannot be set to non table value");
        UpVal *uv = cl->upvals[GETARG_B(i)];
        setobj(L, uv->v, ra);
        luaC_upvalbarrier(L, uv);
        vmbreak;
      }
      vmcase(OP_RAVI_LOADIZ) {
        setivalue(ra, 0);
        vmbreak;
      }
      vmcase(OP_RAVI_LOADFZ) {
        setfltvalue(ra, 0.0);
        vmbreak;
      }
      vmcase(OP_RAVI_UNMF) {
        lua_assert(0);
        TValue *rb = RB(i);
        setfltvalue(ra, -fltvalue(rb));
        vmbreak;
      }
      vmcase(OP_RAVI_UNMI) {
        lua_assert(0);
        TValue *rb = RB(i);
        setivalue(ra, -ivalue(rb));
        vmbreak;
      }
      vmcase(OP_RAVI_ADDFF) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, fltvalue(rb) + fltvalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_ADDFI) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, fltvalue(rb) + ivalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_ADDII) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setivalue(ra, ivalue(rb) + ivalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_SUBFF) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, fltvalue(rb) - fltvalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_SUBFI) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, fltvalue(rb) - ivalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_SUBIF) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, ivalue(rb) - fltvalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_SUBII) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setivalue(ra, ivalue(rb) - ivalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_MULFF) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, fltvalue(rb) * fltvalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_MULFI) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, fltvalue(rb) * ivalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_MULII) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setivalue(ra, ivalue(rb) * ivalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_DIVFF) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, fltvalue(rb) / fltvalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_DIVFI) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, fltvalue(rb) / ivalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_DIVIF) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, ivalue(rb) / fltvalue(rc));
        vmbreak;
      }
      vmcase(OP_RAVI_DIVII) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        setfltvalue(ra, (lua_Number)(ivalue(rb)) / (lua_Number)(ivalue(rc)));
        vmbreak;
      }
      vmcase(OP_RAVI_MOVEI) {
        TValue *rb = RB(i);
        lua_Integer j;
        if (RAVI_LIKELY(tointeger(rb, &j))) { setivalue(ra, j); }
        else
          luaG_runerror(L, "MOVEI: integer expected");
        vmbreak;
      }
      vmcase(OP_RAVI_MOVEF) {
        TValue *rb = RB(i);
        lua_Number j;
        if (RAVI_LIKELY(tonumber(rb, &j))) { setfltvalue(ra, j); }
        else
          luaG_runerror(L, "MOVEF: number expected");
        vmbreak;
      }
      vmcase(OP_RAVI_MOVEIARRAY) {
        TValue *rb = RB(i);
        if (RAVI_LIKELY(ttisiarray(rb))) {
          setobjs2s(L, ra, rb);
        }
        else
          luaG_runerror(L, "integer[] expected");
        vmbreak;
      }
      vmcase(OP_RAVI_MOVEFARRAY) {
        TValue *rb = RB(i);
        if (RAVI_LIKELY(ttisfarray(rb))) {
          setobjs2s(L, ra, rb);
        }
        else
          luaG_runerror(L, "number[] expected");
        vmbreak;
      }
      vmcase(OP_RAVI_MOVETAB) {
        TValue *rb = RB(i);
        if (RAVI_LIKELY(ttisLtable(rb))) {
          setobjs2s(L, ra, rb);
        }
        else
          luaG_runerror(L, "table expected");
        vmbreak;
      }
      vmcase(OP_RAVI_TOINT) {
        lua_Integer j;
        if (RAVI_LIKELY(tointeger(ra, &j))) { setivalue(ra, j); }
        else
          luaG_runerror(L, "TOINT: integer expected");
        vmbreak;
      }
      vmcase(OP_RAVI_TOFLT) {
        lua_Number j;
        if (RAVI_LIKELY(tonumber(ra, &j))) { setfltvalue(ra, j); }
        else
          luaG_runerror(L, "TOFLT: number expected");
        vmbreak;
      }
      vmcase(OP_RAVI_TOTAB) {
        if (RAVI_UNLIKELY(!ttisLtable(ra)))
          luaG_runerror(L, "table expected");
        vmbreak;
      }
      vmcase(OP_RAVI_TOIARRAY) {
        if (RAVI_UNLIKELY(!ttisiarray(ra)))
          luaG_runerror(L, "integer[] expected");
        vmbreak;
      }
      vmcase(OP_RAVI_TOFARRAY) {
        if (RAVI_UNLIKELY(!ttisfarray(ra)))
          luaG_runerror(L, "number[] expected");
        vmbreak;
      }
      vmcase(OP_RAVI_TOSTRING) {
        if (!ttisnil(ra) && RAVI_UNLIKELY(!ttisstring(ra)))
          luaG_runerror(L, "string expected");        
        vmbreak;
      }
      vmcase(OP_RAVI_TOCLOSURE) {
        if (!ttisnil(ra) && RAVI_UNLIKELY(!ttisclosure(ra)))
          luaG_runerror(L, "closure expected");
        vmbreak;
      }
      vmcase(OP_RAVI_TOTYPE) {
        if (!ttisnil(ra)) {
          TValue *rb = k + GETARG_Bx(i);
          if  (!ttisshrstring(rb))
            luaG_runerror(L, "type name must be string");
          TString *key = tsvalue(rb);
          if (!raviV_check_usertype(L, key, ra))
            luaG_runerror(L, "type mismatch: expected %s", getstr(key));
        }
        vmbreak;
      }
    }
  }
}

void ravi_dump_value(lua_State *L, const TValue *stack_ptr) {
  (void)L;
  if (ttisCclosure(stack_ptr))
    printf("C closure\n");
  else if (ttislcf(stack_ptr))
    printf("light C function\n");
  else if (ttisfcf(stack_ptr))
	printf("fast C function\n");
  else if (ttisLclosure(stack_ptr))
    printf("Lua closure\n");
  else if (ttisfunction(stack_ptr))
    printf("function\n");
  else if (ttislngstring(stack_ptr) || ttisshrstring(stack_ptr) ||
    ttisstring(stack_ptr))
    printf("'%s'\n", svalue(stack_ptr));
  else if (ttistable(stack_ptr))
    printf("table\n");
  else if (ttisnil(stack_ptr))
    printf("nil\n");
  else if (ttisfloat(stack_ptr))
    printf("%.6f\n", fltvalue(stack_ptr));
  else if (ttisinteger(stack_ptr))
    printf("%lld\n", (long long)ivalue(stack_ptr));
  else if (ttislightuserdata(stack_ptr))
    printf("light user data\n");
  else if (ttisfulluserdata(stack_ptr))
    printf("full user data\n");
  else if (ttisboolean(stack_ptr))
    printf("boolean\n");
  else if (ttisthread(stack_ptr))
    printf("thread\n");
  else
    printf("other\n");
}

static void ravi_dump_ci(lua_State *L, CallInfo *ci) {
  StkId func = ci->func;
  int func_type = ttype(func);
  StkId base = NULL;
  Proto *p = NULL;
  int funcpos = ci->func - L->stack;
  StkId stack_ptr = ci->top - 1;
  int i;
  switch (func_type) {
  case RAVI_TFCF:
	  printf("stack[%d] = Fast C function\n", funcpos);
	  printf("---> called from \n");
	  return;
  case LUA_TLCF:
    printf("stack[%d] = Light C function\n", funcpos);
    printf("---> called from \n");
    return;
  case LUA_TCCL:
    printf("stack[%d] = C closure\n", funcpos);
    printf("---> called from \n");
    return;
  case LUA_TFUNCTION:
    p = clLvalue(func)->p;
    base = ci->u.l.base;
    i = ci->top - L->stack - 1;
    break;
  default:
    return;
  }
  for (; stack_ptr >= base; stack_ptr--, i--) {
    printf("stack[%d] reg[%d] = %s %s", i, (int)(stack_ptr-base), (stack_ptr == base ? "(base) " : ""), (stack_ptr == L->top ? "(L->top) " : ""));
    if (ttisCclosure(stack_ptr))
      printf("C closure\n");
    else if (ttislcf(stack_ptr))
      printf("light C function\n");
    else if (ttisfcf(stack_ptr))
      printf("fast C function\n");
    else if (ttisLclosure(stack_ptr))
      printf("Lua closure\n");
    else if (ttisfunction(stack_ptr))
      printf("function\n");
    else if (ttislngstring(stack_ptr) || ttisshrstring(stack_ptr) ||
             ttisstring(stack_ptr))
      printf("'%s'\n", svalue(stack_ptr));
    else if (ttistable(stack_ptr))
      printf("table\n");
    else if (ttisnil(stack_ptr))
      printf("nil\n");
    else if (ttisfloat(stack_ptr))
      printf("%.6f\n", fltvalue(stack_ptr));
    else if (ttisinteger(stack_ptr))
      printf("%lld\n", (long long)ivalue(stack_ptr));
    else if (ttislightuserdata(stack_ptr))
      printf("light user data\n");
    else if (ttisfulluserdata(stack_ptr))
      printf("full user data\n");
    else if (ttisboolean(stack_ptr))
      printf("boolean\n");
    else if (ttisthread(stack_ptr))
      printf("thread\n");
    else
      printf("other\n");
  }
  printf(
      "stack[%d] = Lua function (registers = %d, params = %d, locals = %d)\n",
      funcpos, (int)(p->maxstacksize), (int)(p->numparams), p->sizelocvars);
  printf("---> called from \n");
}

void ravi_dump_stack(lua_State *L, const char *s) {
  if (!s)
    return;
  CallInfo *ci = L->ci;
  printf("=======================\n");
  printf("Stack dump %s\n", s);
  printf("=======================\n");
  printf("L->top = %d\n", (int)(L->top - L->stack));
  while (ci) {
    ravi_dump_ci(L, ci);
    ci = ci->previous;
  }
  printf("\n");
}

void ravi_dump_stacktop(lua_State *L, const char *s) {
  CallInfo *ci = L->ci;
  int funcpos = (int)(ci->func - L->stack);
  int top = (int)(L->top - L->stack);
  int ci_top = (int)(ci->top - L->stack);
  printf("Stack dump %s function %d L->top = %d, ci->top = %d\n", s, funcpos,
         top, ci_top);
}

/*
** This function is called from JIT compiled code when JIT trace is
** enabled; the function needs to update the savedpc and
** call luaG_traceexec() if necessary
*/
void ravi_debug_trace(lua_State *L, int opCode, int pc) {
  RAVI_DEBUG_STACK(
      char buf[100]; CallInfo *ci = L->ci;
      int funcpos = (int)(ci->func - L->stack);
      int top = (int)(L->top - L->stack);
      int ci_top = (int)(ci->top - L->stack);
      int base = (int)(ci->u.l.base - L->stack); raviP_instruction_to_str(
          buf, sizeof buf, clvalue(L->ci->func)->l.p->code[pc]);
      printf(
          "Stack dump %s (%s) function %d, pc=%d, L->top = %d, ci->top = %d\n",
          luaP_opnames[opCode], buf, funcpos, pc, (top - base),
          (ci_top - base));
      lua_assert(L->ci->u.l.base <= L->top &&
                 L->top < L->stack + L->stacksize);)

  // Updates the savedpc pointer in the call frame
  // The savedpc is unimportant for the JIT but it is relied upon
  // by the debug interface. So we need to set this in order for the
  // debug api to work. Rather than setting it on every bytecode instruction
  // we have a dual approach. By default the JIT code only sets this prior to
  // function calls - this enables better stack traces for example, and ad-hoc
  // calls to debug api.
  // See void RaviCodeGenerator::emit_update_savedpc(RaviFunctionDef * def,
  // int pc) which is used for this purpose.
  // An optional setting in the JIT compiler also
  // enables this to be updated per bytecode instruction - this is only
  // required if someone wishes to set a line hook. The second option
  // is very expensive and will inhibit optimizations, hence it is optional.
  // This is the setting that is done below - and then the hook is invoked
  // See issue #15
  LClosure *closure = clLvalue(L->ci->func);
  L->ci->u.l.savedpc = &closure->p->code[pc + 1];
  if (L->hookmask & (LUA_MASKLINE | LUA_MASKCOUNT)) luaG_traceexec(L);
}

void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra) {
  Table *t = raviH_new(L, RAVI_TARRAYINT, 0);
  setiarrayvalue(L, ra, t);
  checkGC_(L, ra + 1);
}

void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra) {
  Table *t = raviH_new(L, RAVI_TARRAYFLT, 0);
  setfarrayvalue(L, ra, t);
  checkGC_(L, ra + 1);
}

void raviV_op_newtable(lua_State *L, CallInfo *ci, TValue *ra, int b, int c) {
  Table *t = luaH_new(L);
  sethvalue(L, ra, t);
  if (b != 0 || c != 0) luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
  checkGC_(L, ra + 1);
}

void raviV_op_setlist(lua_State *L, CallInfo *ci, TValue *ra, int b, int c) {
  int n = b;
  unsigned int last;
  Table *h;
  if (n == 0) n = cast_int(L->top - ra) - 1;
  h = hvalue(ra);
  last = ((c - 1) * LFIELDS_PER_FLUSH) + n;
  if (h->ravi_array.array_type == RAVI_TTABLE) {
    if (last > h->sizearray)        /* needs more space? */
      luaH_resizearray(L, h, last); /* pre-allocate it at once */
    for (; n > 0; n--) {
      TValue *val = ra + n;
      luaH_setint(L, h, last--, val);
      luaC_barrierback(L, h, val);
    }
  }
  else {
    int i = last - n + 1;
    for (; i <= (int)last; i++) {
      TValue *val = ra + i;
      unsigned int u = (unsigned int)(i);
      switch (h->ravi_array.array_type) {
        case RAVI_TARRAYINT: {
          if (ttisinteger(val)) { raviH_set_int_inline(L, h, u, ivalue(val)); }
          else {
            lua_Integer i = 0;
            if (luaV_tointeger_(val, &i)) { raviH_set_int_inline(L, h, u, i); }
            else
              luaG_runerror(L, "value cannot be converted to integer");
          }
        } break;
        case RAVI_TARRAYFLT: {
          if (ttisfloat(val)) {
            raviH_set_float_inline(L, h, u, fltvalue(val));
          }
          else if (ttisinteger(val)) {
            raviH_set_float_inline(L, h, u, (lua_Number)(ivalue(val)));
          }
          else {
            lua_Number d = 0.0;
            if (luaV_tonumber_(val, &d)) { raviH_set_float_inline(L, h, u, d); }
            else
              luaG_runerror(L, "value cannot be converted to number");
          }
        } break;
        default: lua_assert(0);
      }
    }
  }
  L->top = ci->top; /* correct top (in case of previous open call) */
}

void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c) {
  StkId rb, ra;
  StkId base = ci->u.l.base;
  L->top = base + c + 1; /* mark the end of concat operands */
  Protect_base(luaV_concat(L, c - b + 1));
  ra = base + a; /* 'luav_concat' may invoke TMs and move the stack */
  rb = base + b;
  setobjs2s(L, ra, rb);
  checkGC_protectbase(L, (ra >= rb ? ra + 1 : rb));
  L->top = ci->top; /* restore top */
}

void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int Bx) {
  StkId base = ci->u.l.base;
  Proto *p = cl->p->p[Bx];
  LClosure *ncl = getcached(p, cl->upvals, base); /* cached closure */
  StkId ra = base + a;
  if (ncl == NULL) /* no match? */ {
    pushclosure(L, p, cl->upvals, base, ra); /* create a new one */
  }
  else {
    setclLvalue(L, ra, ncl); /* push cashed closure */
  }
  checkGC_protectbase(L, ra + 1);
}

void raviV_op_vararg(lua_State *L, CallInfo *ci, LClosure *cl, int a, int b) {
  StkId base = ci->u.l.base;
  StkId ra;
  int j;
  int n = cast_int(base - ci->func) - cl->p->numparams - 1;
  if (n < 0) /* less arguments than parameters? */
    n = 0;   /* no vararg arguments */
  b = b - 1;
  if (b < 0) { /* B == 0? */
    b = n;     /* get all var. arguments */
    Protect_base(luaD_checkstack(L, n));
    ra = base + a; /* previous call may change the stack */
    L->top = ra + n;
  }
  else {
    ra = base + a;
  }
  for (j = 0; j < b && j < n; j++) 
    setobjs2s(L, ra + j, base - n + j);
  for (; j < b; j++) /* complete required results with nil */
    setnilvalue(ra + j);
}

// This is a cheat for a boring opcode
void raviV_op_loadnil(CallInfo *ci, int a, int b) {
  StkId base;
  base = ci->u.l.base;
  TValue *ra = base + a;
  do { setnilvalue(ra++); } while (b--);
}

void raviV_op_setupvali(lua_State *L, LClosure *cl, TValue *ra, int b) {
  lua_Integer ia;
  if (tointeger(ra, &ia)) {
    UpVal *uv = cl->upvals[b];
    setivalue(uv->v, ia);
    luaC_upvalbarrier(L, uv);
  }
  else
    luaG_runerror(
        L, "upvalue of integer type, cannot be set to non integer value");
}

void raviV_op_setupvalf(lua_State *L, LClosure *cl, TValue *ra, int b) {
  lua_Number na;
  if (tonumber(ra, &na)) {
    UpVal *uv = cl->upvals[b];
    setfltvalue(uv->v, na);
    luaC_upvalbarrier(L, uv);
  }
  else
    luaG_runerror(L,
                  "upvalue of number type, cannot be set to non number value");
}

void raviV_op_setupvalai(lua_State *L, LClosure *cl, TValue *ra, int b) {
  if (!ttisiarray(ra))
    luaG_runerror(
        L, "upvalue of integer[] type, cannot be set to non integer[] value");
  UpVal *uv = cl->upvals[b];
  setobj(L, uv->v, ra);
  luaC_upvalbarrier(L, uv);
}

void raviV_op_setupvalaf(lua_State *L, LClosure *cl, TValue *ra, int b) {
  if (!ttisfarray(ra))
    luaG_runerror(
        L, "upvalue of number[] type, cannot be set to non number[] value");
  UpVal *uv = cl->upvals[b];
  setobj(L, uv->v, ra);
  luaC_upvalbarrier(L, uv);
}

void raviV_op_setupvalt(lua_State *L, LClosure *cl, TValue *ra, int b) {
  if (!ttisLtable(ra))
    luaG_runerror(L, "upvalue of table type, cannot be set to non table value");
  UpVal *uv = cl->upvals[b];
  setobj(L, uv->v, ra);
  luaC_upvalbarrier(L, uv);
}

void raviV_op_setupval(lua_State *L, LClosure *cl, TValue *ra, int b) {
  UpVal *uv = cl->upvals[b];
  setobj(L, uv->v, ra);
  luaC_upvalbarrier(L, uv);
}

void raviV_op_add(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
  lua_Number nb;
  lua_Number nc;
  if (ttisinteger(rb) && ttisinteger(rc)) {
    lua_Integer ib = ivalue(rb);
    lua_Integer ic = ivalue(rc);
    setivalue(ra, intop(+, ib, ic));
  }
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    setfltvalue(ra, luai_numadd(L, nb, nc));
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_ADD);
  }
}

void raviV_op_sub(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
  lua_Number nb;
  lua_Number nc;
  if (ttisinteger(rb) && ttisinteger(rc)) {
    lua_Integer ib = ivalue(rb);
    lua_Integer ic = ivalue(rc);
    setivalue(ra, intop(-, ib, ic));
  }
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    setfltvalue(ra, luai_numsub(L, nb, nc));
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_SUB);
  }
}

void raviV_op_mul(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
  lua_Number nb;
  lua_Number nc;
  if (ttisinteger(rb) && ttisinteger(rc)) {
    lua_Integer ib = ivalue(rb);
    lua_Integer ic = ivalue(rc);
    setivalue(ra, intop(*, ib, ic));
  }
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    setfltvalue(ra, luai_nummul(L, nb, nc));
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_MUL);
  }
}

void raviV_op_div(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
  lua_Number nb;
  lua_Number nc;
  if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    setfltvalue(ra, luai_numdiv(L, nb, nc));
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_DIV);
  }
}

void raviV_op_shl(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
  lua_Integer ib;
  lua_Integer ic;
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    setivalue(ra, luaV_shiftl(ib, ic));
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_SHL);
  }
}

void raviV_op_shr(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
  lua_Integer ib;
  lua_Integer ic;
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    setivalue(ra, luaV_shiftl(ib, -ic));
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_SHR);
  }
}

void raviV_op_band(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
  lua_Integer ib;
  lua_Integer ic;
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    setivalue(ra, intop(&, ib, ic));
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_BAND);
  }
}

void raviV_op_bor(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
  lua_Integer ib;
  lua_Integer ic;
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    setivalue(ra, intop(|, ib, ic));
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_BOR);
  }
}

void raviV_op_bxor(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
  lua_Integer ib;
  lua_Integer ic;
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    setivalue(ra, intop (^, ib, ic));
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_BXOR);
  }
}

void raviV_op_bnot(lua_State *L, TValue *ra, TValue *rb) {
  lua_Integer ib;
  if (tointeger(rb, &ib)) { setivalue(ra, intop (^, ~l_castS2U(0), ib)); }
  else {
    luaT_trybinTM(L, rb, rb, ra, TM_BNOT);
  }
}

/*
** Main function for table access (invoking metamethods if needed).
** Compute 'val = t[key]'
** In Lua 5.3.2 this function is a macro but we need it to be a function
** so that JIT code can invoke it
*/
void raviV_gettable_sskey(lua_State *L, const TValue *t, TValue *key, StkId val) {
  GETTABLE_INLINE_SSKEY(L, t, key, val);
}


/*
** Main function for table assignment (invoking metamethods if needed).
** Compute 't[key] = val'
** In Lua 5.3.2 this function is a macro but we need it to be a function
** so that JIT code can invoke it
*/
void raviV_settable_sskey(lua_State *L, const TValue *t, TValue *key, StkId val) {
  SETTABLE_INLINE_SSKEY(L, t, key, val);
}

/*
** Main function for table access (invoking metamethods if needed).
** Compute 'val = t[key]'
** In Lua 5.3.2 this function is a macro but we need it to be a function
** so that JIT code can invoke it
*/
void raviV_gettable_i(lua_State *L, const TValue *t, TValue *key, StkId val) {
  GETTABLE_INLINE_I(L, t, key, val);
}


/*
** Main function for table assignment (invoking metamethods if needed).
** Compute 't[key] = val'
** In Lua 5.3.2 this function is a macro but we need it to be a function
** so that JIT code can invoke it
*/
void raviV_settable_i(lua_State *L, const TValue *t, TValue *key, StkId val) {
  SETTABLE_INLINE_I(L, t, key, val);
}

/**
** Opcode TOTYPE validates that the register A contains a
** type whose metatable is registered by name in constant Bx
*/
void raviV_op_totype(lua_State *L, TValue *ra, TValue *rb) {
  if (ttisnil(ra))
    return;
  if (!ttisshrstring(rb)) luaG_runerror(L, "type name must be string");
  TString *key = tsvalue(rb);
  if (!raviV_check_usertype(L, key, ra))
    luaG_runerror(L, "type mismatch: expected %s", getstr(key));
}

/* }================================================================== */


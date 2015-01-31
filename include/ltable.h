/*
** $Id: ltable.h,v 2.20 2014/09/04 18:15:29 roberto Exp $
** Lua tables (hash)
** See Copyright Notice in lua.h
*/

#ifndef ltable_h
#define ltable_h

#include "lobject.h"


#define gnode(t,i)	(&(t)->node[i])
#define gval(n)		(&(n)->i_val)
#define gnext(n)	((n)->i_key.nk.next)


/* 'const' to avoid wrong writings that can mess up field 'next' */ 
#define gkey(n)		cast(const TValue*, (&(n)->i_key.tvk))

#define wgkey(n)		(&(n)->i_key.nk)

#define invalidateTMcache(t)	((t)->flags = 0)


/* returns the key, given the value of a table entry */
#define keyfromval(v) \
  (gkey(cast(Node *, cast(char *, (v)) - offsetof(Node, i_val))))


LUAI_FUNC const TValue *luaH_getint (Table *t, lua_Integer key);
LUAI_FUNC void luaH_setint (lua_State *L, Table *t, lua_Integer key,
                                                    TValue *value);
LUAI_FUNC const TValue *luaH_getstr (Table *t, TString *key);
LUAI_FUNC const TValue *luaH_get (Table *t, const TValue *key);
LUAI_FUNC TValue *luaH_newkey (lua_State *L, Table *t, const TValue *key);
LUAI_FUNC TValue *luaH_set (lua_State *L, Table *t, const TValue *key);
LUAI_FUNC Table *luaH_new (lua_State *L);
LUAI_FUNC void luaH_resize (lua_State *L, Table *t, unsigned int nasize,
                                                    unsigned int nhsize);
LUAI_FUNC void luaH_resizearray (lua_State *L, Table *t, unsigned int nasize);
LUAI_FUNC void luaH_free (lua_State *L, Table *t);
LUAI_FUNC int luaH_next (lua_State *L, Table *t, StkId key);
LUAI_FUNC int luaH_getn (Table *t);

LUAI_FUNC Table *raviH_new(lua_State *L, ravitype_t tt); /* RAVI array specialization */
LUAI_FUNC int raviH_getn(Table *t); /* RAVI array specialization */
LUAI_FUNC void raviH_setint(lua_State *L, Table *t, lua_Integer key, TValue *value); /* RAVI array specialization */
LUAI_FUNC const TValue *raviH_getint(lua_State *L, Table *t, lua_Integer key); /* RAVI array specialization */

LUAI_FUNC void raviH_setint_int(lua_State *L, Table *t, lua_Integer key, lua_Integer value); /* RAVI array specialization */
LUAI_FUNC void raviH_setint_float(lua_State *L, Table *t, lua_Integer key, lua_Number value); /* RAVI array specialization */

#define raviH_getint_inline(L, t, key, v) \
  { unsigned ukey = (unsigned)(key-1); \
    if (ukey < t->ravi_array_len) \
      v = &t->array[ukey]; \
    else \
      luaG_runerror(L, "array out of bounds"); \
  }

#define raviH_setint_int_inline(L, t, key, value) \
  { unsigned u = (unsigned)(key-1); \
    if (u < t->ravi_array_len) { \
      setivalue(&t->array[u], value); \
    } else \
      raviH_setint_int(L, t, key, value); \
  }

#if defined(LUA_DEBUG)
LUAI_FUNC Node *luaH_mainposition (const Table *t, const TValue *key);
LUAI_FUNC int luaH_isdummy (Node *n);
#endif


#endif

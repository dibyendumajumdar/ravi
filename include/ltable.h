/*
** $Id: ltable.h,v 2.23.1.2 2018/05/24 19:39:05 roberto Exp $
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

/*
** writable version of 'gkey'; allows updates to individual fields,
** but not to the whole (which has incompatible type)
*/
#define wgkey(n)		(&(n)->i_key.nk)

#define invalidateTMcache(t)	((t)->flags = 0)


/* true when 't' is using 'dummynode' as its hash part */
#define isdummy(t)		((t)->lastfree == NULL)


/* allocated size for hash nodes */
#define allocsizenode(t)	(isdummy(t) ? 0 : sizenode(t))


/* returns the key, given the value of a table entry */
#define keyfromval(v) \
  (gkey(cast(Node *, cast(char *, (v)) - offsetof(Node, i_val))))


LUAI_FUNC const TValue *luaH_getint (Table *t, lua_Integer key);
LUAI_FUNC void luaH_setint (lua_State *L, Table *t, lua_Integer key,
                                                    TValue *value);

#if RAVI_USE_NEWHASH

/* 
Like LuaJIT we use a pre-computed hmask to minimise the number of steps
required to get to a node in the hash table
*/

#define hashpow2(t, h)	(gnode(t, (h & (t)->hmask)))
#define hashstr(t, str) hashpow2(t, (str)->hash)
#define hashboolean(t, p) hashpow2(t, p)
#define hashint(t, i) hashpow2(t, i)
/*
** for some types, it is better to avoid modulus by power of 2, as
** they tend to have many 2 factors.
*/
#define hashmod(t, n) (gnode(t, ((n) % ((t)->hmask|1))))
#define hashpointer(t, p) hashmod(t, point2uint(p))

#else 

#define hashpow2(t,n)		(gnode(t, lmod((n), sizenode(t))))
#define hashstr(t,str)		hashpow2(t, (str)->hash)
#define hashboolean(t,p)	hashpow2(t, p)
#define hashint(t,i)		hashpow2(t, i)
/*
** for some types, it is better to avoid modulus by power of 2, as
** they tend to have many 2 factors.
*/
#define hashmod(t,n)	(gnode(t, ((n) % ((sizenode(t)-1)|1))))
#define hashpointer(t,p)	hashmod(t, point2uint(p))

#endif

#if defined(RAVI_ENABLED)
/*
** search function for short strings
*/
static RAVI_ALWAYS_INLINE const TValue *luaH_getshortstr(Table *t, TString *key) {
  Node *n = hashstr(t, key);
  lua_assert(key->tt == LUA_TSHRSTR);
  for (;;) {  /* check whether 'key' is somewhere in the chain */
    const TValue *k = gkey(n);
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
      return gval(n);  /* that's it */
    else {
      int nx = gnext(n);
      if (nx == 0)
        return luaO_nilobject;  /* not found */
      n += nx;
    }
  }
}
#else
LUAI_FUNC const TValue *luaH_getshortstr (Table *t, TString *key);
#endif
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

/* RAVI array specialization */

/* Creates a specialized version of Lua Table to support Ravi's
 * integer[] and number[] arrays.
 */
LUAI_FUNC Table *raviH_new(lua_State *L, ravitype_t array_type, int is_slice);

LUAI_FUNC Table *raviH_new_integer_array(lua_State *L, unsigned int len,
                                         lua_Integer init_value);

LUAI_FUNC Table *raviH_new_number_array(lua_State *L, unsigned int len,
                                        lua_Number init_value);

/* Returns the array length - note that this function will
 * ignore any elements outside of the Ravi Array structure
 */
LUAI_FUNC int raviH_getn(Table *t);

/* Type specific array set operation */
LUAI_FUNC void raviH_set_int(lua_State *L, Table *t, lua_Unsigned key,
                             lua_Integer value);

/* Type specific array set operation */
LUAI_FUNC void raviH_set_float(lua_State *L, Table *t, lua_Unsigned key,
                               lua_Number value);

/* Create a slice of an existing array
* The original table containing the array is inserted into the
* the slice as a value against special key pointer('key_orig_table') so that
* the parent table is not garbage collected while this array contains a
* reference to it
* The array slice starts at start but start-1 is also accessible because of the
* implementation having array values starting at 0.
* A slice must not attempt to release the data array as this is not owned by it,
* and in fact may point to garbage from a memory allocater's point of view.
*/
LUAI_FUNC Table *raviH_new_slice(lua_State *L, TValue *parent,
                                 unsigned int start, unsigned int len);

/* Obtain parent array of the slice */
LUAI_FUNC const TValue *raviH_slice_parent(lua_State *L, TValue *slice);

/* Type specific array get operation */
#define raviH_get_int_inline(L, t, key, v)                                     \
  {                                                                            \
    unsigned int ukey = (unsigned int)((key));                                 \
    lua_Integer *data = (lua_Integer *)t->ravi_array.data;                     \
    if (ukey < t->ravi_array.len) {                                            \
      setivalue(v, data[ukey]);                                                \
    } else                                                                     \
      luaG_runerror(L, "array out of bounds");                                 \
  }

/* Type specific array get operation */
#define raviH_get_float_inline(L, t, key, v)                                   \
  {                                                                            \
    unsigned int ukey = (unsigned int)((key));                                 \
    lua_Number *data = (lua_Number *)t->ravi_array.data;                       \
    if (ukey < t->ravi_array.len) {                                            \
      setfltvalue(v, data[ukey]);                                              \
    } else                                                                     \
      luaG_runerror(L, "array out of bounds");                                 \
  }

/* Type specific array set operation */
#define raviH_set_int_inline(L, t, key, value)                                 \
  {                                                                            \
    unsigned int ukey = (unsigned int)((key));                                 \
    lua_Integer *data = (lua_Integer *)t->ravi_array.data;                     \
    if (ukey < t->ravi_array.len) {                                            \
      data[ukey] = (value);                                                    \
    } else                                                                     \
      raviH_set_int(L, t, ukey, (value));                                      \
  }

/* Type specific array set operation */
#define raviH_set_float_inline(L, t, key, value)                               \
  {                                                                            \
    unsigned int ukey = (unsigned int)((key));                                 \
    lua_Number *data = (lua_Number *)t->ravi_array.data;                       \
    if (ukey < t->ravi_array.len) {                                            \
      data[ukey] = (value);                                                    \
    } else                                                                     \
      raviH_set_float(L, t, ukey, (value));                                    \
  }

LUAI_FUNC void raviH_get_number_array_rawdata(lua_State *L, Table *t, lua_Number **startp, lua_Number **endp);
LUAI_FUNC void raviH_get_integer_array_rawdata(lua_State *L, Table *t, lua_Integer **startp, lua_Integer **endp);

#if defined(LUA_DEBUG)
LUAI_FUNC Node *luaH_mainposition (const Table *t, const TValue *key);
LUAI_FUNC int luaH_isdummy (const Table *t);
#endif


#endif

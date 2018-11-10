/*
** $Id: ltable.c,v 2.118.1.4 2018/06/08 16:22:51 roberto Exp $
** Lua tables (hash)
** See Copyright Notice in lua.h
*/

#define ltable_c
#define LUA_CORE

#include "lprefix.h"


/*
** Implementation of tables (aka arrays, objects, or hash tables).
** Tables keep its elements in two parts: an array part and a hash part.
** Non-negative integer keys are all candidates to be kept in the array
** part. The actual size of the array is the largest 'n' such that
** more than half the slots between 1 and n are in use.
** Hash uses a mix of chained scatter table with Brent's variation.
** A main invariant of these tables is that, if an element is not
** in its main position (i.e. the 'original' position that its hash gives
** to it), then the colliding element is in its own main position.
** Hence even when the load factor reaches 100%, performance remains good.
*/

#include <float.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"


/*
** Maximum size of array part (MAXASIZE) is 2^MAXABITS. MAXABITS is
** the largest integer such that MAXASIZE fits in an unsigned int.
*/
#define MAXABITS	cast_int(sizeof(int) * CHAR_BIT - 1)
#define MAXASIZE	(1u << MAXABITS)

/*
** Maximum size of hash part is 2^MAXHBITS. MAXHBITS is the largest
** integer such that 2^MAXHBITS fits in a signed int. (Note that the
** maximum number of elements in a table, 2^MAXABITS + 2^MAXHBITS, still
** fits comfortably in an unsigned int.)
*/
#define MAXHBITS	(MAXABITS - 1)

#define dummynode		(&dummynode_)

static const Node dummynode_ = {
  {NILCONSTANT},  /* value */
  {{NILCONSTANT, 0}}  /* key */
};


/*
** Hash for floating-point numbers.
** The main computation should be just
**     n = frexp(n, &i); return (n * INT_MAX) + i
** but there are some numerical subtleties.
** In a two-complement representation, INT_MAX does not has an exact
** representation as a float, but INT_MIN does; because the absolute
** value of 'frexp' is smaller than 1 (unless 'n' is inf/NaN), the
** absolute value of the product 'frexp * -INT_MIN' is smaller or equal
** to INT_MAX. Next, the use of 'unsigned int' avoids overflows when
** adding 'i'; the use of '~u' (instead of '-u') avoids problems with
** INT_MIN.
*/
#if !defined(l_hashfloat)
static int l_hashfloat (lua_Number n) {
  int i;
  lua_Integer ni;
  n = l_mathop(frexp)(n, &i) * -cast_num(INT_MIN);
  if (!lua_numbertointeger(n, &ni)) {  /* is 'n' inf/-inf/NaN? */
    lua_assert(luai_numisnan(n) || l_mathop(fabs)(n) == cast_num(HUGE_VAL));
    return 0;
  }
  else {  /* normal case */
    unsigned int u = cast(unsigned int, i) + cast(unsigned int, ni);
    return cast_int(u <= cast(unsigned int, INT_MAX) ? u : ~u);
  }
}
#endif


/*
** returns the 'main' position of an element in a table (that is, the index
** of its hash value)
*/
static Node *mainposition (const Table *t, const TValue *key) {
  switch (ttype(key)) {
    case LUA_TNUMINT:
      return hashint(t, ivalue(key));
    case LUA_TNUMFLT:
      return hashmod(t, l_hashfloat(fltvalue(key)));
    case LUA_TSHRSTR:
      return hashstr(t, tsvalue(key));
    case LUA_TLNGSTR:
      return hashpow2(t, luaS_hashlongstr(tsvalue(key)));
    case LUA_TBOOLEAN:
      return hashboolean(t, bvalue(key));
    case LUA_TLIGHTUSERDATA:
      return hashpointer(t, pvalue(key));
    case LUA_TLCF:
      return hashpointer(t, fvalue(key));
    case RAVI_TFCF:
      return hashpointer(t, fcfvalue(key));
    default:
      lua_assert(!ttisdeadkey(key));
      return hashpointer(t, gcvalue(key));
  }
}


/*
** returns the index for 'key' if 'key' is an appropriate key to live in
** the array part of the table, 0 otherwise.
*/
static unsigned int arrayindex (const TValue *key) {
  if (ttisinteger(key)) {
    lua_Integer k = ivalue(key);
    if (0 < k && (lua_Unsigned)k <= MAXASIZE)
      return cast(unsigned int, k);  /* 'key' is an appropriate array index */
  }
  return 0;  /* 'key' did not match some condition */
}


/*
** returns the index of a 'key' for table traversals. First goes all
** elements in the array part, then elements in the hash part. The
** beginning of a traversal is signaled by 0.
*/
static unsigned int findindex (lua_State *L, Table *t, StkId key) {
  unsigned int i;
  if (ttisnil(key)) return 0;  /* first iteration */
  i = arrayindex(key);
  if (i != 0 && i <= t->sizearray)  /* is 'key' inside array part? */
    return i;  /* yes; that's the index */
  else {
    int nx;
    Node *n = mainposition(t, key);
    for (;;) {  /* check whether 'key' is somewhere in the chain */
      /* key may be dead already, but it is ok to use it in 'next' */
      if (luaV_rawequalobj(gkey(n), key) ||
            (ttisdeadkey(gkey(n)) && iscollectable(key) &&
             deadvalue(gkey(n)) == gcvalue(key))) {
        i = cast_int(n - gnode(t, 0));  /* key index in hash table */
        /* hash elements are numbered after array ones */
        return (i + 1) + t->sizearray;
      }
      nx = gnext(n);
      if (nx == 0)
        luaG_runerror(L, "invalid key to 'next'");  /* key not found */
      else n += nx;
    }
  }
}

/* RAVI's implementation of luaH_next() equivalent 
 * if no more keys return 0
 * else return 1
 * If key is nil then start the iterator 
 * set value to key+1
 * increment *key 
 */
int raviH_next(lua_State *L, Table *t, StkId key) { 
  lua_Integer i;
  if (ttisnil(key))
    /* Lua keys start at 1 so this is just before that 
     * (although 0 is valid Ravi index it cannot be 
     * accessed using this method) 
     */
    i = 0; 
  else if (!tointeger(key, &i)) {
    return 0;
  }
  i = i + 1;
  if (i >= t->ravi_array.len)
    /* no more keys */
    return 0;
  setivalue(key, i);
  if (t->ravi_array.array_type == RAVI_TARRAYFLT) {
    raviH_get_float_inline(L, t, i, (key + 1));
  }
  else {
    raviH_get_int_inline(L, t, i, (key + 1));
  }
  return 1;
}

int luaH_next (lua_State *L, Table *t, StkId key) {
  if (t->ravi_array.array_type != RAVI_TTABLE)
    return raviH_next(L, t, key);
  else {
    unsigned int i = findindex(L, t, key);  /* find original element */
    for (; i < t->sizearray; i++) {  /* try first array part */
      if (!ttisnil(&t->array[i])) {  /* a non-nil value? */
        setivalue(key, i + 1);
        setobj2s(L, key + 1, &t->array[i]);
        return 1;
      }
    }
    for (i -= t->sizearray; cast_int(i) < sizenode(t); i++) {  /* hash part */
      if (!ttisnil(gval(gnode(t, i)))) {  /* a non-nil value? */
        setobj2s(L, key, gkey(gnode(t, i)));
        setobj2s(L, key + 1, gval(gnode(t, i)));
        return 1;
      }
    }
    return 0;  /* no more elements */
  }
}

/*
** {=============================================================
** Rehash
** ==============================================================
*/

/*
** Compute the optimal size for the array part of table 't'. 'nums' is a
** "count array" where 'nums[i]' is the number of integers in the table
** between 2^(i - 1) + 1 and 2^i. 'pna' enters with the total number of
** integer keys in the table and leaves with the number of keys that
** will go to the array part; return the optimal size.
*/
static unsigned int computesizes (unsigned int nums[], unsigned int *pna) {
  int i;
  unsigned int twotoi;  /* 2^i (candidate for optimal size) */
  unsigned int a = 0;  /* number of elements smaller than 2^i */
  unsigned int na = 0;  /* number of elements to go to array part */
  unsigned int optimal = 0;  /* optimal size for array part */
  /* loop while keys can fill more than half of total size */
  for (i = 0, twotoi = 1;
       twotoi > 0 && *pna > twotoi / 2;
       i++, twotoi *= 2) {
    if (nums[i] > 0) {
      a += nums[i];
      if (a > twotoi/2) {  /* more than half elements present? */
        optimal = twotoi;  /* optimal size (till now) */
        na = a;  /* all elements up to 'optimal' will go to array part */
      }
    }
  }
  lua_assert((optimal == 0 || optimal / 2 < na) && na <= optimal);
  *pna = na;
  return optimal;
}


static int countint (const TValue *key, unsigned int *nums) {
  unsigned int k = arrayindex(key);
  if (k != 0) {  /* is 'key' an appropriate array index? */
    nums[luaO_ceillog2(k)]++;  /* count as such */
    return 1;
  }
  else
    return 0;
}


/*
** Count keys in array part of table 't': Fill 'nums[i]' with
** number of keys that will go into corresponding slice and return
** total number of non-nil keys.
*/
static unsigned int numusearray (const Table *t, unsigned int *nums) {
  int lg;
  unsigned int ttlg;  /* 2^lg */
  unsigned int ause = 0;  /* summation of 'nums' */
  unsigned int i = 1;  /* count to traverse all array keys */
  /* traverse each slice */
  for (lg = 0, ttlg = 1; lg <= MAXABITS; lg++, ttlg *= 2) {
    unsigned int lc = 0;  /* counter */
    unsigned int lim = ttlg;
    if (lim > t->sizearray) {
      lim = t->sizearray;  /* adjust upper limit */
      if (i > lim)
        break;  /* no more elements to count */
    }
    /* count elements in range (2^(lg - 1), 2^lg] */
    for (; i <= lim; i++) {
      if (!ttisnil(&t->array[i-1]))
        lc++;
    }
    nums[lg] += lc;
    ause += lc;
  }
  return ause;
}


static int numusehash (const Table *t, unsigned int *nums, unsigned int *pna) {
  int totaluse = 0;  /* total number of elements */
  int ause = 0;  /* elements added to 'nums' (can go to array part) */
  int i = sizenode(t);
  while (i--) {
    Node *n = &t->node[i];
    if (!ttisnil(gval(n))) {
      ause += countint(gkey(n), nums);
      totaluse++;
    }
  }
  *pna += ause;
  return totaluse;
}


static void setarrayvector (lua_State *L, Table *t, unsigned int size) {
  unsigned int i;
  luaM_reallocvector(L, t->array, t->sizearray, size, TValue);
  for (i=t->sizearray; i<size; i++)
     setnilvalue(&t->array[i]);
  t->sizearray = size;
}


static void setnodevector (lua_State *L, Table *t, unsigned int size) {
  if (size == 0) {  /* no elements to hash part? */
    t->node = cast(Node *, dummynode);  /* use common 'dummynode' */
    t->lsizenode = 0;
#if RAVI_USE_NEWHASH
    t->hmask = 0;
#endif
    t->lastfree = NULL;  /* signal that it is using dummy node */
  }
  else {
    int i;
    int lsize = luaO_ceillog2(size);
    if (lsize > MAXHBITS)
      luaG_runerror(L, "table overflow");
    size = twoto(lsize);
    t->node = luaM_newvector(L, size, Node);
    for (i = 0; i < (int)size; i++) {
      Node *n = gnode(t, i);
      gnext(n) = 0;
      setnilvalue(wgkey(n));
      setnilvalue(gval(n));
    }
    t->lsizenode = cast_byte(lsize);
#if RAVI_USE_NEWHASH
    t->hmask = size - 1;
#endif
    t->lastfree = gnode(t, size);  /* all positions are free */
  }
}


typedef struct {
  Table *t;
  unsigned int nhsize;
} AuxsetnodeT;


static void auxsetnode (lua_State *L, void *ud) {
  AuxsetnodeT *asn = cast(AuxsetnodeT *, ud);
  setnodevector(L, asn->t, asn->nhsize);
}


void luaH_resize (lua_State *L, Table *t, unsigned int nasize,
                                          unsigned int nhsize) {
  unsigned int i;
  int j;
  AuxsetnodeT asn;
  unsigned int oldasize = t->sizearray;
  int oldhsize = allocsizenode(t);
  Node *nold = t->node;  /* save old hash ... */
  if (nasize > oldasize)  /* array part must grow? */
    setarrayvector(L, t, nasize);
  /* create new hash part with appropriate size */
  asn.t = t; asn.nhsize = nhsize;
  if (luaD_rawrunprotected(L, auxsetnode, &asn) != LUA_OK) {  /* mem. error? */
    setarrayvector(L, t, oldasize);  /* array back to its original size */
    luaD_throw(L, LUA_ERRMEM);  /* rethrow memory error */
  }
  if (nasize < oldasize) {  /* array part must shrink? */
    t->sizearray = nasize;
    /* re-insert elements from vanishing slice */
    for (i=nasize; i<oldasize; i++) {
      if (!ttisnil(&t->array[i]))
        luaH_setint(L, t, i + 1, &t->array[i]);
    }
    /* shrink array */
    luaM_reallocvector(L, t->array, oldasize, nasize, TValue);
  }
  /* re-insert elements from hash part */
  for (j = oldhsize - 1; j >= 0; j--) {
    Node *old = nold + j;
    if (!ttisnil(gval(old))) {
      /* doesn't need barrier/invalidate cache, as entry was
         already present in the table */
      setobjt2t(L, luaH_set(L, t, gkey(old)), gval(old));
    }
  }
  if (oldhsize > 0)  /* not the dummy node? */
    luaM_freearray(L, nold, cast(size_t, oldhsize)); /* free old hash */
}


void luaH_resizearray (lua_State *L, Table *t, unsigned int nasize) {
  int nsize = allocsizenode(t);
  luaH_resize(L, t, nasize, nsize);
}

/*
** nums[i] = number of keys 'k' where 2^(i - 1) < k <= 2^i
*/
static void rehash (lua_State *L, Table *t, const TValue *ek) {
  unsigned int asize;  /* optimal size for array part */
  unsigned int na;  /* number of keys in the array part */
  unsigned int nums[MAXABITS + 1];
  int i;
  int totaluse;
  for (i = 0; i <= MAXABITS; i++) nums[i] = 0;  /* reset counts */
  na = numusearray(t, nums);  /* count keys in array part */
  totaluse = na;  /* all those keys are integer keys */
  totaluse += numusehash(t, nums, &na);  /* count keys in hash part */
  /* count extra key */
  na += countint(ek, nums);
  totaluse++;
  /* compute new size for array part */
  asize = computesizes(nums, &na);
  /* resize the table to new computed sizes */
  luaH_resize(L, t, asize, totaluse - na);
}



/*
** }=============================================================
*/


Table *luaH_new (lua_State *L) {
  GCObject *o = luaC_newobj(L, LUA_TTABLE, sizeof(Table));
  Table *t = gco2t(o);
  t->metatable = NULL;
  t->flags = cast_byte(~0);
  t->array = NULL;
  t->sizearray = 0;
  t->ravi_array.len = 0; /* RAVI */
  t->ravi_array.array_type = RAVI_TTABLE; /* default is a Lua table */
  t->ravi_array.array_modifier = 0;
  t->ravi_array.data = NULL; /* data */
  t->ravi_array.size = 0;
  setnodevector(L, t, 0);
  return t;
}

Table *raviH_new(lua_State *L, ravitype_t tt, int is_slice) {
  lua_assert(tt == RAVI_TARRAYFLT || tt == RAVI_TARRAYINT);
  GCObject *o = luaC_newobj(L, tt == RAVI_TARRAYFLT ? RAVI_TFARRAY : RAVI_TIARRAY, sizeof(Table));
  Table *t = gco2t(o);
  t->metatable = NULL;
  t->flags = cast_byte(~0);
  t->array = NULL;
  t->sizearray = 0;
  t->ravi_array.len = 0; /* RAVI */
  t->ravi_array.array_modifier = 0;
  t->ravi_array.data = NULL; /* data */
  t->ravi_array.size = 0;
  setnodevector(L, t, 0);
  t->ravi_array.array_type = tt;
  if (!is_slice) {
    if (tt == RAVI_TARRAYFLT) {
      raviH_set_float_inline(L, t, 0, 0.0);
    }
    else {
      raviH_set_int_inline(L, t, 0, 0);
    }
  }
  return t;
}




void luaH_free (lua_State *L, Table *t) {
  if (t->ravi_array.array_modifier != RAVI_ARRAY_SLICE && t->ravi_array.data) {
    if (t->ravi_array.array_type == RAVI_TARRAYFLT)
      luaM_freemem(L, t->ravi_array.data, (t->ravi_array.size*sizeof(lua_Number)));
    else {
      lua_assert(t->ravi_array.array_type == RAVI_TARRAYINT);
      luaM_freemem(L, t->ravi_array.data, (t->ravi_array.size*sizeof(lua_Integer)));
    }
  }
  if (!isdummy(t))
    luaM_freearray(L, t->node, cast(size_t, sizenode(t)));
  luaM_freearray(L, t->array, t->sizearray);
  luaM_free(L, t);
}


static Node *getfreepos (Table *t) {
  if (!isdummy(t)) {
    while (t->lastfree > t->node) {
      t->lastfree--;
      if (ttisnil(gkey(t->lastfree)))
        return t->lastfree;
    }
  }
  return NULL;  /* could not find a free place */
}



/*
** inserts a new key into a hash table; first, check whether key's main
** position is free. If not, check whether colliding node is in its main
** position or not: if it is not, move colliding node to an empty place and
** put new key in its main position; otherwise (colliding node is in its main
** position), new key goes to an empty position.
*/
TValue *luaH_newkey (lua_State *L, Table *t, const TValue *key) {
  Node *mp;
  TValue aux;
  if (ttisnil(key)) luaG_runerror(L, "table index is nil");
  else if (ttisfloat(key)) {
    lua_Integer k;
    if (luaV_tointeger(key, &k, 0)) {  /* does index fit in an integer? */
      setivalue(&aux, k);
      key = &aux;  /* insert it as an integer */
    }
    else if (luai_numisnan(fltvalue(key)))
      luaG_runerror(L, "table index is NaN");
  }
  mp = mainposition(t, key);
  if (!ttisnil(gval(mp)) || isdummy(t)) {  /* main position is taken? */
    Node *othern;
    Node *f = getfreepos(t);  /* get a free place */
    if (f == NULL) {  /* cannot find a free place? */
      rehash(L, t, key);  /* grow table */
      /* whatever called 'newkey' takes care of TM cache */
      return luaH_set(L, t, key);  /* insert key into grown table */
    }
    lua_assert(!isdummy(t));
    othern = mainposition(t, gkey(mp));
    if (othern != mp) {  /* is colliding node out of its main position? */
      /* yes; move colliding node into free position */
      while (othern + gnext(othern) != mp)  /* find previous */
        othern += gnext(othern);
      gnext(othern) = cast_int(f - othern);  /* rechain to point to 'f' */
      *f = *mp;  /* copy colliding node into free pos. (mp->next also goes) */
      if (gnext(mp) != 0) {
        gnext(f) += cast_int(mp - f);  /* correct 'next' */
        gnext(mp) = 0;  /* now 'mp' is free */
      }
      setnilvalue(gval(mp));
    }
    else {  /* colliding node is in its own main position */
      /* new node will go into free position */
      if (gnext(mp) != 0)
        gnext(f) = cast_int((mp + gnext(mp)) - f);  /* chain new position */
      else lua_assert(gnext(f) == 0);
      gnext(mp) = cast_int(f - mp);
      mp = f;
    }
  }
  setnodekey(L, &mp->i_key, key);
  luaC_barrierback(L, t, key);
  lua_assert(ttisnil(gval(mp)));
  return gval(mp);
}


/*
** search function for integers
*/
const TValue *luaH_getint (Table *t, lua_Integer key) {
  /* (1 <= key && key <= t->sizearray) */
  if (l_castS2U(key) - 1 < t->sizearray)
    return &t->array[key - 1];
  else {
    Node *n = hashint(t, key);
    for (;;) {  /* check whether 'key' is somewhere in the chain */
      if (ttisinteger(gkey(n)) && ivalue(gkey(n)) == key)
        return gval(n);  /* that's it */
      else {
        int nx = gnext(n);
        if (nx == 0) break;
        n += nx;
      }
    }
    return luaO_nilobject;
  }
}

#if !defined(RAVI_ENABLED)
/*
** search function for short strings
*/
const TValue *luaH_getshortstr (Table *t, TString *key) {
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
#endif

/*
** "Generic" get version. (Not that generic: not valid for integers,
** which may be in array part, nor for floats with integral values.)
*/
static const TValue *getgeneric (Table *t, const TValue *key) {
  Node *n = mainposition(t, key);
  for (;;) {  /* check whether 'key' is somewhere in the chain */
    if (luaV_rawequalobj(gkey(n), key))
      return gval(n);  /* that's it */
    else {
      int nx = gnext(n);
      if (nx == 0)
        return luaO_nilobject;  /* not found */
      n += nx;
    }
  }
}


const TValue *luaH_getstr (Table *t, TString *key) {
  if (key->tt == LUA_TSHRSTR)
    return luaH_getshortstr(t, key);
  else {  /* for long strings, use generic case */
    TValue ko;
    setsvalue(cast(lua_State *, NULL), &ko, key);
    return getgeneric(t, &ko);
  }
}


/*
** main search function
*/
const TValue *luaH_get(Table *t, const TValue *key) {
#if 1
  switch (ttype(key)) {
    case LUA_TSHRSTR: return luaH_getshortstr(t, tsvalue(key));
    case LUA_TNUMINT: return luaH_getint(t, ivalue(key));
    case LUA_TNIL: return luaO_nilobject;
    case LUA_TNUMFLT: {
      lua_Integer k;
      if (luaV_tointeger(key, &k, 0)) /* index is int? */
        return luaH_getint(t, k);  /* use specialized version */
      /* else... */
    }  /* FALLTHROUGH */
    default:
      return getgeneric(t, key);
  }
#else
  int tt = ttype(key);
  if (RAVI_LIKELY(tt == LUA_TNUMINT))
    return luaH_getint(t, ivalue(key));
  else if (tt == LUA_TSHRSTR)
    return luaH_getshortstr(t, tsvalue(key));
  else if (tt == LUA_TNIL)
    return luaO_nilobject;
  else if (tt == LUA_TNUMFLT) {
    lua_Integer k;
    if (luaV_tointeger(key, &k, 0)) /* index is int? */
      return luaH_getint(t, k);     /* use specialized version */
    /* else... */
    return getgeneric(t, key);
  } /* FALLTHROUGH */
  else {
    return getgeneric(t, key);
  }
#endif
}

/*
** beware: when using this function you probably need to check a GC
** barrier and invalidate the TM cache.
*/
TValue *luaH_set (lua_State *L, Table *t, const TValue *key) {
  const TValue *p = luaH_get(t, key);
  if (p != luaO_nilobject)
    return cast(TValue *, p);
  else return luaH_newkey(L, t, key);
}


void luaH_setint (lua_State *L, Table *t, lua_Integer key, TValue *value) {
  const TValue *p = luaH_getint(t, key);
  TValue *cell;
  if (p != luaO_nilobject)
    cell = cast(TValue *, p);
  else {
    TValue k;
    setivalue(&k, key);
    cell = luaH_newkey(L, t, &k);
  }
  setobj2t(L, cell, value);
}


static lua_Unsigned unbound_search (Table *t, lua_Unsigned j) {
  lua_Unsigned i = j;  /* i is zero or a present index */
  j++;
  /* find 'i' and 'j' such that i is present and j is not */
  while (!ttisnil(luaH_getint(t, j))) {
    i = j;
    if (j > l_castS2U(LUA_MAXINTEGER) / 2) {  /* overflow? */
      /* table was built with bad purposes: resort to linear search */
      i = 1;
      while (!ttisnil(luaH_getint(t, i))) i++;
      return i - 1;
    }
    j *= 2;
  }
  /* now do a binary search between them */
  while (j - i > 1) {
    lua_Unsigned m = (i+j)/2;
    if (ttisnil(luaH_getint(t, m))) j = m;
    else i = m;
  }
  return i;
}


/*
** Try to find a boundary in table 't'. A 'boundary' is an integer index
** such that t[i] is non-nil and t[i+1] is nil (and 0 if t[1] is nil).
*/
int luaH_getn (Table *t) {
  unsigned int j;
  /* if this is a RAVI array then use specialized function */
  if (t->ravi_array.array_type != RAVI_TTABLE) {
    lua_assert(t->ravi_array.array_type == RAVI_TARRAYFLT ||
               t->ravi_array.array_type == RAVI_TARRAYINT);
    return t->ravi_array.len;
  }
  j = t->sizearray;
  if (j > 0 && ttisnil(&t->array[j - 1])) {
    /* there is a boundary in the array part: (binary) search for it */
    unsigned int i = 0;
    while (j - i > 1) {
      unsigned int m = (i+j)/2;
      if (ttisnil(&t->array[m - 1])) j = m;
      else i = m;
    }
    return i;
  }
  /* else must find a boundary in hash part */
  else if (isdummy(t))  /* hash part is empty? */
    return j;  /* that is easy... */
  else return unbound_search(t, j);
}

/* RAVI array specialization */
int raviH_getn(Table *t) {
  lua_assert(t->ravi_array.array_type != RAVI_TTABLE);
  return t->ravi_array.len - 1;
}

/* resize array and initialize new elements if requested */
static int ravi_resize_array(lua_State *L, Table *t, unsigned int new_size,
                             int initialize) {
  if (t->ravi_array.array_modifier) {
    /* cannot resize */
    return 0;
  }
  int number_array = RAVI_TARRAYFLT == t->ravi_array.array_type;
  unsigned int size =
      new_size < t->ravi_array.size + 10 ? t->ravi_array.size + 10 : new_size;
  if (number_array) {
    t->ravi_array.data = (char *)luaM_reallocv(
      L, t->ravi_array.data, t->ravi_array.size, size, sizeof(lua_Number));
    if (initialize) {
      lua_Number *data = (lua_Number *)t->ravi_array.data;
      memset(&data[t->ravi_array.len], 0, (size - t->ravi_array.size) * sizeof(lua_Number));
    }
  }
  else {
    t->ravi_array.data = (char *)luaM_reallocv(
      L, t->ravi_array.data, t->ravi_array.size, size, sizeof(lua_Integer));
    if (initialize) {
      lua_Integer *data = (lua_Integer *)t->ravi_array.data;
      memset(&data[t->ravi_array.len], 0, (size - t->ravi_array.size) * sizeof(lua_Integer));
    }
  }
  t->ravi_array.size = size;
  return 1;
}

void raviH_set_int(lua_State *L, Table *t, lua_Unsigned u1, lua_Integer value) {
  unsigned int u = (unsigned int)u1;
  lua_assert(t->ravi_array.array_type == RAVI_TARRAYINT);
  lua_Integer *data;
  if (u < t->ravi_array.len) {
  setval2:
    data = (lua_Integer *)t->ravi_array.data;
    data[u] = value;
  } else if (u == t->ravi_array.len) {
    if (u < t->ravi_array.size) {
    setval:
      t->ravi_array.len++;
      goto setval2;
    } else {
      if (ravi_resize_array(L, t, 0, 1))
        goto setval;
      else
        luaG_runerror(L, "array cannot be resized");
    }
  } else
    luaG_runerror(L, "array out of bounds");
}

void raviH_set_float(lua_State *L, Table *t, lua_Unsigned u1, lua_Number value) {
  unsigned int u = (unsigned int)u1;
  lua_assert(t->ravi_array.array_type == RAVI_TARRAYFLT);
  lua_Number *data;
  if (u < t->ravi_array.len) {
  setval2:
    data = (lua_Number *)t->ravi_array.data;
    data[u] = value;
  } else if (u == t->ravi_array.len) {
    if (u < t->ravi_array.size) {
    setval:
      t->ravi_array.len++;
      goto setval2;
    } else {
      if (ravi_resize_array(L, t, 0, 1))
        goto setval;
      else
        luaG_runerror(L, "array cannot be resized");
    }
  } else
    luaG_runerror(L, "array out of bounds");
}

Table *raviH_new_integer_array(lua_State *L, unsigned int len,
                               lua_Integer init_value) {
  Table *t = raviH_new(L, RAVI_TARRAYINT, 0);
  ravi_resize_array(L, t, len + 1, 0);
  lua_Integer *data = (lua_Integer *)t->ravi_array.data;
  data[0] = 0;
  for (unsigned int i = 1; i <= len; i++) {
    data[i] = init_value;
  }
  t->ravi_array.len = len + 1;
  t->ravi_array.array_modifier = RAVI_ARRAY_FIXEDSIZE;
  return t;
}

Table *raviH_new_number_array(lua_State *L, unsigned int len,
                              lua_Number init_value) {
  Table *t = raviH_new(L, RAVI_TARRAYFLT, 0);
  ravi_resize_array(L, t, len + 1, 0);
  lua_Number *data = (lua_Number *)t->ravi_array.data;
  data[0] = 0;
  for (unsigned int i = 1; i <= len; i++) {
    data[i] = init_value;
  }
  t->ravi_array.len = len + 1;
  t->ravi_array.array_modifier = RAVI_ARRAY_FIXEDSIZE;
  return t;
}

void raviH_get_number_array_rawdata(lua_State *L, Table *t, lua_Number **startp, lua_Number **endp) {
  (void)L;
  lua_assert(t->ravi_array.array_type == RAVI_TARRAYFLT);
  lua_Number *data = (lua_Number *)t->ravi_array.data;
  *startp = data;
  *endp = data + t->ravi_array.len;
}

void raviH_get_integer_array_rawdata(lua_State *L, Table *t, lua_Integer **startp, lua_Integer **endp) {
  (void)L;
  lua_assert(t->ravi_array.array_type == RAVI_TARRAYINT);
  lua_Integer *data = (lua_Integer *)t->ravi_array.data;
  *startp = data;
  *endp = data + t->ravi_array.len;
}

static const char *key_orig_table = "Originaltable";

/* Create a slice of an existing array
 * The original table containing the array is inserted into the
 * the slice as a value against special key pointer('key_orig_table') so that
 * the parent table is not garbage collected while this array contains a
 * reference to it
 * The array slice starts at start but start-1 is also accessible because of the
 * implementation having array values starting at 0.
 * A slice must not attempt to release the data array as this is not owned by
 * it,
 * and in fact may point to garbage from a memory allocater's point of view.
 */
Table *raviH_new_slice(lua_State *L, TValue *parent, unsigned int start,
                       unsigned int len) {
  if (!ttistable(parent) || ttisLtable(parent))
    luaG_runerror(L, "integer[] or number[] expected");
  Table *orig = hvalue(parent);
  if (!orig->ravi_array.array_modifier)
    luaG_runerror(
        L, "cannot create slice from dynamic integer[] or number[] array");
  /* Create the slice table */
  Table *t = raviH_new(L, orig->ravi_array.array_type, 1);
  lua_assert(t->ravi_array.data == NULL);
  /* Add a reference to the parent table */
  TValue k;
  setpvalue(&k, (void *)key_orig_table);
  TValue *cell = luaH_newkey(L, t, &k);
  setobj2t(L, cell, parent);
  /* Initialize */
  t->ravi_array.array_type = orig->ravi_array.array_type;
  t->ravi_array.array_modifier = RAVI_ARRAY_SLICE;
  if (ttisfarray(parent)) {
    lua_Number *data = (lua_Number *)orig->ravi_array.data;
    t->ravi_array.data = (char *)(data + start - 1);
  }
  else {
    lua_Integer *data = (lua_Integer *)orig->ravi_array.data;
    t->ravi_array.data = (char *)(data + start - 1);
  }
  t->ravi_array.len = len + 1;
  t->ravi_array.size = len + 1;
  return t;
}

/* Obtain parent array of the slice */
const TValue *raviH_slice_parent(lua_State *L, TValue *slice) {
  if (!ttistable(slice) || ttisLtable(slice))
    luaG_runerror(L, "slice of integer[] or number[] expected");
  Table *orig = hvalue(slice);
  if (orig->ravi_array.array_modifier != RAVI_ARRAY_SLICE)
    luaG_runerror(L, "slice of integer[] or number[] expected");
  /* Get reference to the parent table */
  TValue k;
  setpvalue(&k, (void *)key_orig_table);
  const TValue *cell = luaH_get(orig, &k);
  lua_assert(ttistable(cell));
  return cell;
}

#if defined(LUA_DEBUG)

Node *luaH_mainposition (const Table *t, const TValue *key) {
  return mainposition(t, key);
}

int luaH_isdummy (const Table *t) { return isdummy(t); }

#endif

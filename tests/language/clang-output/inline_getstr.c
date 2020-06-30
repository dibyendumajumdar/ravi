#include "lua_hdr.h"

#define LUA_TSTRING             4
#define LUA_TSHRSTR     (LUA_TSTRING | (0 << 4))

#define checktag(o,t)           (rttype(o) == (t))
#define ctb(t)                  ((t) | BIT_ISCOLLECTABLE)

#define cast(t, exp)    ((t)(exp))
#define lmod(s,size) \
        (cast(int, (s) & ((size)-1)))


#define ttisshrstring(o)        checktag((o), ctb(LUA_TSHRSTR))
#define gco2ts(o)       cast(struct TString *, (o))
#define val_(o)         ((o)->value_)
#define tsvalue(o)      gco2ts(val_(o).gc)

#define gnode(t,i)      (&(t)->node[i])
#define gval(n)         (&(n)->i_val)
#define gkey(n)         cast(const struct TValue*, (&(n)->i_key.tvk))

#define twoto(x)        (1<<(x))
#define sizenode(t)     (twoto((t)->lsizenode))

//#define hashpow2(t,n)           (gnode(t, lmod((n), sizenode(t))))
// Following uses cached hmask which is basically precomputed lmod()
#define hashpow2(t, h)	(gnode(t, (h & (t)->hmask)))
#define hashstr(t,str)          hashpow2(t, (str)->hash)
#define eqshrstr(a,b)           ((a) == (b))

extern const struct TValue *luaH_getstr (struct Table *t, struct TString *key);

const struct TValue *ravi_getstr (struct Table *t, struct TString *key) {
    struct Node *n = hashstr(t, key);
    const struct TValue *k = gkey(n);
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
        return gval(n);  /* that's it */
    return luaH_getstr(t, key);
}

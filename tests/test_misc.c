#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <assert.h>

#define check_exp(c,e)		(assert(c), (e))

/*
** basic types
*/
#define LUA_TNONE   (-1)

#define LUA_TNIL    0
#define LUA_TBOOLEAN    1
#define LUA_TLIGHTUSERDATA  2
#define LUA_TNUMBER   3
#define LUA_TSTRING   4
#define LUA_TTABLE    5
#define LUA_TFUNCTION   6
#define LUA_TUSERDATA   7
#define LUA_TTHREAD   8

#define LUA_NUMTAGS   9

/*
** Extra tags for non-values
*/
#define LUA_TPROTO  LUA_NUMTAGS
#define LUA_TDEADKEY  (LUA_NUMTAGS+1)

/*
** number of all possible tags (including LUA_TNONE but excluding DEADKEY)
*/
#define LUA_TOTALTAGS (LUA_TPROTO + 2)


/*
** tags for Tagged Values have the following use of bits:
** bits 0-3: actual tag (a LUA_T* value)
** bits 4-5: variant bits
** bit 6: whether value is collectable
*/

#define VARBITS   (3 << 4)


/*
** LUA_TFUNCTION variants:
** 0 - Lua function
** 1 - light C function
** 2 - regular C function (closure)
*/

/* Variant tags for functions */
#define LUA_TLCL  (LUA_TFUNCTION | (0 << 4))  /* Lua closure */
#define LUA_TLCF  (LUA_TFUNCTION | (1 << 4))  /* light C function */
#define LUA_TCCL  (LUA_TFUNCTION | (2 << 4))  /* C closure */


/* Variant tags for strings */
#define LUA_TSHRSTR (LUA_TSTRING | (0 << 4))  /* short strings */
#define LUA_TLNGSTR (LUA_TSTRING | (1 << 4))  /* long strings */


/* Variant tags for numbers */
#define LUA_TNUMFLT (LUA_TNUMBER | (0 << 4))  /* float numbers */
#define LUA_TNUMINT (LUA_TNUMBER | (1 << 4))  /* integer numbers */


/* Bit mark for collectable types */
#define BIT_ISCOLLECTABLE (1 << 6)

/* mark a tag as collectable */
#define ctb(t)      ((t) | BIT_ISCOLLECTABLE)


typedef struct lua_State lua_State;
typedef long long lua_Integer;
typedef double lua_Number;
typedef int (*lua_CFunction)(lua_State *);
typedef unsigned char lu_byte;

/*
** Common type for all collectable objects
*/
typedef struct GCObject GCObject;


/*
** Common Header for all collectable objects (in macro form, to be
** included in other objects)
*/
#define CommonHeader  GCObject *next; lu_byte tt; lu_byte marked


/*
** Common type has only the common header
*/
struct GCObject {
  CommonHeader;
};



/*
** Union of all Lua values
*/
typedef union Value Value;

#define RAVI_NAN_TAGGING 1
#if RAVI_NAN_TAGGING

#define RAVI_NAN_TAG   0x7ffc0000
#define RAVI_TYPE_MASK 0x0000ffff

/* 
TODO This is little endian - need to switch lo/hi for big
endian
*/
typedef struct ravi_HiLo {
  int lo;
  int hi;
} HiLo;

typedef union ravi_TypeOrDouble {
  HiLo hilo;
  double n;
  /* for debugging */
  unsigned char bytes[8];
} TypeOrDouble;

/*
** Tagged Values. This is the basic representation of values in Lua,
** an actual value plus a tag with its type.
*/

#define TValuefields  Value value_; TypeOrDouble tt_

typedef struct lua_TValue TValue;


/* macro defining a nil value */
#define NILCONSTANT {NULL}, { {0, (RAVI_NAN_TAG|LUA_TNIL)} }

/* Note that numbers are not stored in value_ when NaN tagging is ON */
#define val_(o)   ((o)->value_)
#define numval_(o) ((o)->tt_)

#define extracttype(o) ((o)->tt_.hilo.hi & RAVI_NAN_TAG)

/* raw type tag of a TValue */
#define rttype(o) (extracttype(o)==RAVI_NAN_TAG ? ((o)->tt_.hilo.hi & RAVI_TYPE_MASK) : LUA_TNUMFLT)

/* tag with no variants (bits 0-3) */
#define novariant(x)  ((x) & 0x0F)

/* type tag of a TValue (bits 0-3 for tags + variant bits 4-5) */
#define ttype(o)  (rttype(o) & 0x3F)

/* type tag of a TValue with no variants (bits 0-3) */
#define ttnov(o)  (novariant(rttype(o)))


/* Macros to test type */
#define checktag(o,t)   (rttype(o) == (t))
#define checktype(o,t)    (ttnov(o) == (t))
#define ttisfloat(o)    (extracttype(o) != RAVI_NAN_TAG)
#define ttisinteger(o)    checktag((o), LUA_TNUMINT)
#define ttisnumber(o)   (ttisfloat(o) || ttisinteger(o))
#define ttisnil(o)    checktag((o), LUA_TNIL)
#define ttisboolean(o)    checktag((o), LUA_TBOOLEAN)
#define ttislightuserdata(o)  checktag((o), LUA_TLIGHTUSERDATA)
#define ttisstring(o)   checktype((o), LUA_TSTRING)
#define ttisshrstring(o)  checktag((o), ctb(LUA_TSHRSTR))
#define ttislngstring(o)  checktag((o), ctb(LUA_TLNGSTR))
#define ttistable(o)    checktag((o), ctb(LUA_TTABLE))
#define ttisfunction(o)   checktype(o, LUA_TFUNCTION)
#define ttisclosure(o)    ((rttype(o) & 0x1F) == LUA_TFUNCTION)
#define ttisCclosure(o)   checktag((o), ctb(LUA_TCCL))
#define ttisLclosure(o)   checktag((o), ctb(LUA_TLCL))
#define ttislcf(o)    checktag((o), LUA_TLCF)
#define ttisfulluserdata(o) checktag((o), ctb(LUA_TUSERDATA))
#define ttisthread(o)   checktag((o), ctb(LUA_TTHREAD))
#define ttisdeadkey(o)    checktag((o), LUA_TDEADKEY)


/* Macros to access values */
#define ivalue(o) check_exp(ttisinteger(o), val_(o).i)
#define fltvalue(o) check_exp(ttisfloat(o), numval_(o).n)
#define nvalue(o) check_exp(ttisnumber(o), \
  (ttisinteger(o) ? cast_num(ivalue(o)) : fltvalue(o)))
#define gcvalue(o)  check_exp(iscollectable(o), val_(o).gc)
#define pvalue(o) check_exp(ttislightuserdata(o), val_(o).p)
#define tsvalue(o)  check_exp(ttisstring(o), gco2ts(val_(o).gc))
#define uvalue(o) check_exp(ttisfulluserdata(o), gco2u(val_(o).gc))
#define clvalue(o)  check_exp(ttisclosure(o), gco2cl(val_(o).gc))
#define clLvalue(o) check_exp(ttisLclosure(o), gco2lcl(val_(o).gc))
#define clCvalue(o) check_exp(ttisCclosure(o), gco2ccl(val_(o).gc))
#define fvalue(o) check_exp(ttislcf(o), val_(o).f)
#define hvalue(o) check_exp(ttistable(o), gco2t(val_(o).gc))
#define bvalue(o) check_exp(ttisboolean(o), val_(o).b)
#define thvalue(o)  check_exp(ttisthread(o), gco2th(val_(o).gc))
/* a dead value may get the 'gc' field, but cannot access its contents */
#define deadvalue(o)  check_exp(ttisdeadkey(o), cast(void *, val_(o).gc))

#define l_isfalse(o)  (ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))


#define iscollectable(o)  (rttype(o) & BIT_ISCOLLECTABLE)


/* Macros for internal tests */
#define righttt(obj)    (ttype(obj) == gcvalue(obj)->tt)

#define checkliveness(g,obj) \
  lua_longassert(!iscollectable(obj) || \
      (righttt(obj) && !isdead(g,gcvalue(obj))))


/* Macros to set values */
#define settt_(o,t) ((o)->tt_.hilo.hi=(t|RAVI_NAN_TAG))

#define setfltvalue(obj,x) \
  { TValue *io=(obj); numval_(io).n=(x); }

#define setivalue(obj,x) \
  { TValue *io=(obj); val_(io).i=(x); settt_(io, LUA_TNUMINT); }

#define setnilvalue(obj) settt_(obj, LUA_TNIL)

#define setfvalue(obj,x) \
  { TValue *io=(obj); val_(io).f=(x); settt_(io, LUA_TLCF); }

#define setpvalue(obj,x) \
  { TValue *io=(obj); val_(io).p=(x); settt_(io, LUA_TLIGHTUSERDATA); }

#define setbvalue(obj,x) \
  { TValue *io=(obj); val_(io).b=(x); settt_(io, LUA_TBOOLEAN); }

#define setgcovalue(L,obj,x) \
  { TValue *io = (obj); GCObject *i_g=(x); \
    val_(io).gc = i_g; settt_(io, ctb(i_g->tt)); }

#define setsvalue(L,obj,x) \
  { TValue *io = (obj); TString *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(x_->tt)); \
    checkliveness(G(L),io); }

#define setuvalue(L,obj,x) \
  { TValue *io = (obj); Udata *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TUSERDATA)); \
    checkliveness(G(L),io); }

#define setthvalue(L,obj,x) \
  { TValue *io = (obj); lua_State *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTHREAD)); \
    checkliveness(G(L),io); }

#define setclLvalue(L,obj,x) \
  { TValue *io = (obj); LClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TLCL)); \
    checkliveness(G(L),io); }

#define setclCvalue(L,obj,x) \
  { TValue *io = (obj); CClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TCCL)); \
    checkliveness(G(L),io); }

#define sethvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTABLE)); \
    checkliveness(G(L),io); }

#define setdeadvalue(obj) settt_(obj, LUA_TDEADKEY)



#define setobj(L,obj1,obj2) \
  { TValue *io1=(obj1); *io1 = *(obj2); \
    (void)L; checkliveness(G(L),io1); }


/*
** different types of assignments, according to destination
*/

/* from stack to (same) stack */
#define setobjs2s setobj
/* to stack (not from same stack) */
#define setobj2s  setobj
#define setsvalue2s setsvalue
#define sethvalue2s sethvalue
#define setptvalue2s  setptvalue
/* from table to same table */
#define setobjt2t setobj
/* to table */
#define setobj2t  setobj
/* to new object */
#define setobj2n  setobj
#define setsvalue2n setsvalue

#else
/*
** Tagged Values. This is the basic representation of values in Lua,
** an actual value plus a tag with its type.
*/

#define TValuefields  Value value_; int tt_

typedef struct lua_TValue TValue;


/* macro defining a nil value */
#define NILCONSTANT {NULL}, LUA_TNIL


#define val_(o)   ((o)->value_)


/* raw type tag of a TValue */
#define rttype(o) ((o)->tt_)

/* tag with no variants (bits 0-3) */
#define novariant(x)  ((x) & 0x0F)

/* type tag of a TValue (bits 0-3 for tags + variant bits 4-5) */
#define ttype(o)  (rttype(o) & 0x3F)

/* type tag of a TValue with no variants (bits 0-3) */
#define ttnov(o)  (novariant(rttype(o)))


/* Macros to test type */
#define checktag(o,t)   (rttype(o) == (t))
#define checktype(o,t)    (ttnov(o) == (t))
#define ttisnumber(o)   checktype((o), LUA_TNUMBER)
#define ttisfloat(o)    checktag((o), LUA_TNUMFLT)
#define ttisinteger(o)    checktag((o), LUA_TNUMINT)
#define ttisnil(o)    checktag((o), LUA_TNIL)
#define ttisboolean(o)    checktag((o), LUA_TBOOLEAN)
#define ttislightuserdata(o)  checktag((o), LUA_TLIGHTUSERDATA)
#define ttisstring(o)   checktype((o), LUA_TSTRING)
#define ttisshrstring(o)  checktag((o), ctb(LUA_TSHRSTR))
#define ttislngstring(o)  checktag((o), ctb(LUA_TLNGSTR))
#define ttistable(o)    checktag((o), ctb(LUA_TTABLE))
#define ttisfunction(o)   checktype(o, LUA_TFUNCTION)
#define ttisclosure(o)    ((rttype(o) & 0x1F) == LUA_TFUNCTION)
#define ttisCclosure(o)   checktag((o), ctb(LUA_TCCL))
#define ttisLclosure(o)   checktag((o), ctb(LUA_TLCL))
#define ttislcf(o)    checktag((o), LUA_TLCF)
#define ttisfulluserdata(o) checktag((o), ctb(LUA_TUSERDATA))
#define ttisthread(o)   checktag((o), ctb(LUA_TTHREAD))
#define ttisdeadkey(o)    checktag((o), LUA_TDEADKEY)


/* Macros to access values */
#define ivalue(o) check_exp(ttisinteger(o), val_(o).i)
#define fltvalue(o) check_exp(ttisfloat(o), val_(o).n)
#define nvalue(o) check_exp(ttisnumber(o), \
  (ttisinteger(o) ? cast_num(ivalue(o)) : fltvalue(o)))
#define gcvalue(o)  check_exp(iscollectable(o), val_(o).gc)
#define pvalue(o) check_exp(ttislightuserdata(o), val_(o).p)
#define tsvalue(o)  check_exp(ttisstring(o), gco2ts(val_(o).gc))
#define uvalue(o) check_exp(ttisfulluserdata(o), gco2u(val_(o).gc))
#define clvalue(o)  check_exp(ttisclosure(o), gco2cl(val_(o).gc))
#define clLvalue(o) check_exp(ttisLclosure(o), gco2lcl(val_(o).gc))
#define clCvalue(o) check_exp(ttisCclosure(o), gco2ccl(val_(o).gc))
#define fvalue(o) check_exp(ttislcf(o), val_(o).f)
#define hvalue(o) check_exp(ttistable(o), gco2t(val_(o).gc))
#define bvalue(o) check_exp(ttisboolean(o), val_(o).b)
#define thvalue(o)  check_exp(ttisthread(o), gco2th(val_(o).gc))
/* a dead value may get the 'gc' field, but cannot access its contents */
#define deadvalue(o)  check_exp(ttisdeadkey(o), cast(void *, val_(o).gc))

#define l_isfalse(o)  (ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))


#define iscollectable(o)  (rttype(o) & BIT_ISCOLLECTABLE)


/* Macros for internal tests */
#define righttt(obj)    (ttype(obj) == gcvalue(obj)->tt)

#define checkliveness(g,obj) \
  lua_longassert(!iscollectable(obj) || \
      (righttt(obj) && !isdead(g,gcvalue(obj))))


/* Macros to set values */
#define settt_(o,t) ((o)->tt_=(t))

#define setfltvalue(obj,x) \
  { TValue *io=(obj); val_(io).n=(x); settt_(io, LUA_TNUMFLT); }

#define setivalue(obj,x) \
  { TValue *io=(obj); val_(io).i=(x); settt_(io, LUA_TNUMINT); }

#define setnilvalue(obj) settt_(obj, LUA_TNIL)

#define setfvalue(obj,x) \
  { TValue *io=(obj); val_(io).f=(x); settt_(io, LUA_TLCF); }

#define setpvalue(obj,x) \
  { TValue *io=(obj); val_(io).p=(x); settt_(io, LUA_TLIGHTUSERDATA); }

#define setbvalue(obj,x) \
  { TValue *io=(obj); val_(io).b=(x); settt_(io, LUA_TBOOLEAN); }

#define setgcovalue(L,obj,x) \
  { TValue *io = (obj); GCObject *i_g=(x); \
    val_(io).gc = i_g; settt_(io, ctb(i_g->tt)); }

#define setsvalue(L,obj,x) \
  { TValue *io = (obj); TString *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(x_->tt)); \
    checkliveness(G(L),io); }

#define setuvalue(L,obj,x) \
  { TValue *io = (obj); Udata *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TUSERDATA)); \
    checkliveness(G(L),io); }

#define setthvalue(L,obj,x) \
  { TValue *io = (obj); lua_State *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTHREAD)); \
    checkliveness(G(L),io); }

#define setclLvalue(L,obj,x) \
  { TValue *io = (obj); LClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TLCL)); \
    checkliveness(G(L),io); }

#define setclCvalue(L,obj,x) \
  { TValue *io = (obj); CClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TCCL)); \
    checkliveness(G(L),io); }

#define sethvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTABLE)); \
    checkliveness(G(L),io); }

#define setdeadvalue(obj) settt_(obj, LUA_TDEADKEY)



#define setobj(L,obj1,obj2) \
  { TValue *io1=(obj1); *io1 = *(obj2); \
    (void)L; checkliveness(G(L),io1); }


/*
** different types of assignments, according to destination
*/

/* from stack to (same) stack */
#define setobjs2s setobj
/* to stack (not from same stack) */
#define setobj2s  setobj
#define setsvalue2s setsvalue
#define sethvalue2s sethvalue
#define setptvalue2s  setptvalue
/* from table to same table */
#define setobjt2t setobj
/* to table */
#define setobj2t  setobj
/* to new object */
#define setobj2n  setobj
#define setsvalue2n setsvalue


#endif


/*
** {======================================================
** types and prototypes
** =======================================================
*/


union Value {
  GCObject *gc;    /* collectable objects */
  void *p;         /* light userdata */
  int b;           /* booleans */
  lua_CFunction f; /* light C functions */
  lua_Integer i;   /* integer numbers */
  lua_Number n;    /* float numbers */
};


struct lua_TValue {
  TValuefields;
};

static void dumpvalue(TValue* v)
{
  // Litle endian assumed
  for (int i = 7; i >= 0; i--) {
    printf("%02x", v->tt_.bytes[i]);
  }
  printf("\n");
}


int main(int argc, const char **argv) {

  (void)argc;
  (void)argv;

  TValue v = { NILCONSTANT };
  dumpvalue(&v);
  assert(ttisnil(&v));

  setivalue(&v, 55);
  assert(ttisinteger(&v));
  assert(ttisnumber(&v));
  assert(ivalue(&v) == 55);

  setfltvalue(&v, 152.76);
  assert(ttisnumber(&v));
  assert(ttisfloat(&v));
  assert(fltvalue(&v) == 152.76);

  return 0;
}
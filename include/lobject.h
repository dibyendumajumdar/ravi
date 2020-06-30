/*
** $Id: lobject.h $
** Type definitions for Lua objects
** See Copyright Notice in lua.h
*/


#ifndef lobject_h
#define lobject_h


#include <stdarg.h>
#include <stdint.h>

#include "llimits.h"
#include "lua.h"


/*
** Extra tags for non-values
*/
#define LUA_TPROTO	LUA_NUMTAGS		/* function prototypes */
#define LUA_TDEADKEY	(LUA_NUMTAGS+1)		/* removed keys in tables */

/*
** number of all possible tags (including LUA_TNONE but excluding DEADKEY)
*/
#define LUA_TOTALTAGS	(LUA_TPROTO + 2)

/* 
** In Ravi, value type is extended to 16-bits so that we can hold more info.
** The actual type code is still 1 byte (least significant byte)
** and in particular all GC-able type codes must fit into 1 byte because
** the GC CommonHeader only allows 1 byte for the type code.
** The extra byte is for use by the type FCF (fast C function) to 
** encode the C function's parameter and return types.
*/
typedef uint16_t LuaType;

/*
** tags for Tagged Values have the following use of bits:
** bits 0-3: actual tag (a LUA_T* value)
** bits 4-5: variant bits
** bit 15: whether value is collectable
*/

/* add variant bits to a type */
#define makevariant(t,v)	((t) | ((v) << 4))


/* RAVI: Following are the types we will use
** use in parsing. The rationale for types is
** performance - as of now these are the only types that
** we care about from a performance point of view - if any
** other types appear then they are all treated as ANY
**/
typedef enum {
  RAVI_TANY = 0,      /* Lua dynamic type */
  RAVI_TNUMINT = 1,    /* integer number */
  RAVI_TNUMFLT,        /* floating point number */
  RAVI_TARRAYINT,      /* array of ints */
  RAVI_TARRAYFLT,      /* array of doubles */
  RAVI_TFUNCTION,      /* Lua or C Function */
  RAVI_TTABLE,         /* Lua table */
  RAVI_TSTRING,        /* string */
  RAVI_TNIL,           /* NIL */
  RAVI_TBOOLEAN,       /* boolean */
  RAVI_TUSERDATA       /* userdata or lightuserdata */
} ravitype_t;

/*
** Tagged Values. This is the basic representation of values in Lua,
** an actual value plus a tag with its type.
*/

/*
** Union of all Lua values
*/
typedef union Value {
  struct GCObject *gc;    /* collectable objects */
  void *p;         /* light userdata */
  int b;           /* booleans */
  lua_CFunction f; /* light C functions */
  lua_Integer i;   /* integer numbers */
  lua_Number n;    /* float numbers */
} Value;


/*
** Tagged Values. This is the basic representation of values in Lua:
** an actual value plus a tag with its type.
*/

#define TValuefields	Value value_; LuaType tt_

typedef struct lua_TValue {
  TValuefields;
} TValue;

#define val_(o)		((o)->value_)

/* raw type tag of a TValue */
#define rttype(o)	((o)->tt_)

/* tag with no variants (bits 0-3) */
#define novariant(x)	((x) & 0x0F)

/* type tag of a TValue (bits 0-3 for tags + variant bits 4-6) */
/* 7F is 0111 1111 */
#define ttype(o)	(rttype(o) & 0x7F)

/* type tag of a TValue with no variants (bits 0-3) */
#define ttnov(o)	(novariant(rttype(o)))


/* Macros to test type */
#define checktag(o,t)		(rttype(o) == (t))
#define checktype(o,t)		(ttnov(o) == (t))


/* Macros for internal tests */

/* collectable object has the same tag as the original value */
#define righttt(obj)		(ttype(obj) == gcvalue(obj)->tt)

/*
** Any value being manipulated by the program either is non
** collectable, or the collectable object has the right tag
** and it is not dead.
*/
#define checkliveness(L,obj) \
	lua_longassert(!iscollectable(obj) || \
		(righttt(obj) && (L == NULL || !isdead(G(L),gcvalue(obj)))))


/* Macros to set values */

/* set a value's tag */
#define settt_(o,t)	((o)->tt_=(t))


/* main macro to copy values (from 'obj1' to 'obj2') */
#define setobj(L,obj1,obj2) \
	{ TValue *io1=(obj1); const TValue *io2=(obj2); \
          io1->value_ = io2->value_; settt_(io1, io2->tt_); \
	  (void)L; checkliveness(L,io1); }


/*
** different types of assignments, according to destination
*/

/* from stack to (same) stack */
#define setobjs2s	setobj
/* to stack (not from same stack) */
#define setobj2s	setobj
/* from table to same table */
#define setobjt2t	setobj
/* to new object */
#define setobj2n	setobj
/* to table */
#define setobj2t	setobj

typedef TValue *StkId;  /* index to stack elements */
/*
** {==================================================================
** Nil
** ===================================================================
*/

/* macro defining a nil value */
#define NILCONSTANT	{NULL}, LUA_TNIL
/* macro to test for (any kind of) nil */
#define ttisnil(o)		checktag((o), LUA_TNIL)

#define setnilvalue(obj) settt_(obj, LUA_TNIL)
/* }================================================================== */


/*
** {==================================================================
** Booleans
** ===================================================================
*/


#define ttisboolean(o)		checktag((o), LUA_TBOOLEAN)
#define bvalue(o)	check_exp(ttisboolean(o), val_(o).b)
#define l_isfalse(o)	(ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))

#define setbvalue(obj,x) \
  { TValue *io=(obj); val_(io).b=(x); settt_(io, LUA_TBOOLEAN); }
/* }================================================================== */


/*
** {==================================================================
** Threads
** ===================================================================
*/

#define ttisthread(o)		checktag((o), ctb(LUA_TTHREAD))
#define thvalue(o)	check_exp(ttisthread(o), gco2th(val_(o).gc))

#define setthvalue(L,obj,x) \
  { TValue *io = (obj); lua_State *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTHREAD)); \
    checkliveness(L,io); }


/* }================================================================== */


/*
** {==================================================================
** Collectable Objects
** ===================================================================
*/

/*
** Common Header for all collectable objects (in macro form, to be
** included in other objects)
** Note that tt field is a byte.
*/
#define CommonHeader	struct GCObject *next; lu_byte tt; lu_byte marked


/* Common type for all collectable objects */
typedef struct GCObject {
  CommonHeader;
} GCObject;


/* Bit mark for collectable types */
#define BIT_ISCOLLECTABLE	(1 << 15)

#define iscollectable(o)	(rttype(o) & BIT_ISCOLLECTABLE)

/* mark a tag as collectable */
#define ctb(t)			((t) | BIT_ISCOLLECTABLE)

#define gcvalue(o)	check_exp(iscollectable(o), val_(o).gc)

#define setgcovalue(L,obj,x) \
  { TValue *io = (obj); GCObject *i_g=(x); \
    val_(io).gc = i_g; settt_(io, ctb(i_g->tt)); }

/* }================================================================== */


/*
** {==================================================================
** Numbers
** ===================================================================
*/

/* Variant tags for numbers */
#define LUA_TNUMFLT	makevariant(LUA_TNUMBER, 0)  /* float numbers */
#define LUA_TNUMINT	makevariant(LUA_TNUMBER, 1)  /* integer numbers */
#define ttisnumber(o)		checktype((o), LUA_TNUMBER)
#define ttisfloat(o)		checktag((o), LUA_TNUMFLT)
#define ttisinteger(o)		checktag((o), LUA_TNUMINT)

#define nvalue(o)	check_exp(ttisnumber(o), \
	(ttisinteger(o) ? cast_num(ivalue(o)) : fltvalue(o)))
#define fltvalue(o)	check_exp(ttisfloat(o), val_(o).n)
#define ivalue(o)	check_exp(ttisinteger(o), val_(o).i)

#define setfltvalue(obj,x) \
  { TValue *io=(obj); val_(io).n=(x); settt_(io, LUA_TNUMFLT); }

#define chgfltvalue(obj,x) \
  { TValue *io=(obj); lua_assert(ttisfloat(io)); val_(io).n=(x); }

#define setivalue(obj,x) \
  { TValue *io=(obj); val_(io).i=(x); settt_(io, LUA_TNUMINT); }

#define chgivalue(obj,x) \
  { TValue *io=(obj); lua_assert(ttisinteger(io)); val_(io).i=(x); }

/* }================================================================== */


/*
** {==================================================================
** Strings
** ===================================================================
*/

/* Variant tags for strings */
#define LUA_TSHRSTR	makevariant(LUA_TSTRING, 0)  /* short strings */
#define LUA_TLNGSTR	makevariant(LUA_TSTRING, 1)  /* long strings */

#define ttisstring(o)		checktype((o), LUA_TSTRING)
#define ttisshrstring(o)	checktag((o), ctb(LUA_TSHRSTR))
#define ttislngstring(o)	checktag((o), ctb(LUA_TLNGSTR))

#define tsvalue(o)	check_exp(ttisstring(o), gco2ts(val_(o).gc))

#define setsvalue(L,obj,x) \
  { TValue *io = (obj); TString *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(x_->tt)); \
    checkliveness(L,io); }

/* set a string to the stack */
#define setsvalue2s	setsvalue
/* set a string to a new object */
#define setsvalue2n	setsvalue

/*
** Header for string value; string bytes follow the end of this structure
** (aligned according to 'UTString'; see next).
*/
typedef struct TString {
  CommonHeader;
  lu_byte extra;  /* reserved words for short strings; "has hash" for longs */
  lu_byte shrlen;  /* length for short strings */
  unsigned int hash;
  union {
    size_t lnglen;  /* length for long strings */
    struct TString *hnext;  /* linked list for hash table */
  } u;
} TString;


/*
** Ensures that address after this type is always fully aligned.
*/
typedef union UTString {
  L_Umaxalign dummy;  /* ensures maximum alignment for strings */
  TString tsv;
} UTString;

/*
** Get the actual string (array of bytes) from a 'TString'.
** (Access to 'extra' ensures that value is really a 'TString'.)
*/
#define getstr(ts)  \
  check_exp(sizeof((ts)->extra), cast(char *, (ts)) + sizeof(UTString))


/* get the actual string (array of bytes) from a Lua value */
#define svalue(o)       getstr(tsvalue(o))

/* get string length from 'TString *s' */
#define tsslen(s)	((s)->tt == LUA_TSHRSTR ? (s)->shrlen : (s)->u.lnglen)

/* get string length from 'TValue *o' */
#define vslen(o)	tsslen(tsvalue(o))

/* }================================================================== */


/*
** {==================================================================
** Userdata
** ===================================================================
*/


/*
** Light userdata should be a variant of userdata, but for compatibility
** reasons they are also different types.
*/

#define ttislightuserdata(o)	checktag((o), LUA_TLIGHTUSERDATA)
#define ttisfulluserdata(o)	checktag((o), ctb(LUA_TUSERDATA))

#define pvalue(o)	check_exp(ttislightuserdata(o), val_(o).p)
#define uvalue(o)	check_exp(ttisfulluserdata(o), gco2u(val_(o).gc))

#define setpvalue(obj,x) \
  { TValue *io=(obj); val_(io).p=(x); settt_(io, LUA_TLIGHTUSERDATA); }

#define setuvalue(L,obj,x) \
  { TValue *io = (obj); Udata *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TUSERDATA)); \
    checkliveness(L,io); }

/*
** Header for userdata; memory area follows the end of this structure
** (aligned according to 'UUdata'; see next).
*/
typedef struct Udata {
  CommonHeader;
  LuaType ttuv_;  /* user value's tag */
  struct Table *metatable;
  size_t len;  /* number of bytes */
  union Value user_;  /* user value */
} Udata;


/*
** Ensures that address after this type is always fully aligned.
*/
typedef union UUdata {
  L_Umaxalign dummy;  /* ensures maximum alignment for 'local' udata */
  Udata uv;
} UUdata;


/*
**  Get the address of memory block inside 'Udata'.
** (Access to 'ttuv_' ensures that value is really a 'Udata'.)
*/
#define getudatamem(u)  \
  check_exp(sizeof((u)->ttuv_), (cast(char*, (u)) + sizeof(UUdata)))

#define setuservalue(L,u,o) \
	{ const TValue *io=(o); Udata *iu = (u); \
	  iu->user_ = io->value_; iu->ttuv_ = rttype(io); \
	  checkliveness(L,io); }


#define getuservalue(L,u,o) \
	{ TValue *io=(o); const Udata *iu = (u); \
	  io->value_ = iu->user_; settt_(io, iu->ttuv_); \
	  checkliveness(L,io); }

/* }================================================================== */


/*
** {==================================================================
** Prototypes
** ===================================================================
*/

/*
** Description of an upvalue for function prototypes
*/
typedef struct Upvaldesc {
  TString *name;  /* upvalue name (for debug information) */
  TString *usertype; /* RAVI extension: name of user type */
  lu_byte ravi_type; /* RAVI type of upvalue */
  lu_byte instack;  /* whether it is in stack (register) */
  lu_byte idx;  /* index of upvalue (in stack or in outer function's list) */
} Upvaldesc;


/*
** Description of a local variable for function prototypes
** (used for debug information)
*/
typedef struct LocVar {
  TString *varname;
  TString *usertype; /* RAVI extension: name of user type */
  int startpc;  /* first point where variable is active */
  int endpc;    /* first point where variable is dead */
  lu_byte ravi_type; /* RAVI type of the variable - RAVI_TANY if unknown */
} LocVar;

/** RAVI changes start */
typedef enum {
  RAVI_JIT_NOT_COMPILED = 0,
  RAVI_JIT_CANT_COMPILE = 1,
  RAVI_JIT_COMPILED = 2 /* But actual function pointer must be checked */
} ravi_jit_status_t;

typedef enum {
  RAVI_JIT_FLAG_NONE = 0,
  RAVI_JIT_FLAG_HASFORLOOP = 1
} ravi_jit_flag_t;

typedef struct RaviJITProto {
  lu_byte jit_status; /* 0=not compiled, 1=can't compile */
  lu_byte jit_flags; 
  unsigned short execution_count;   /* how many times has function been executed */
  void *jit_data;
  lua_CFunction jit_function;
} RaviJITProto;
/** RAVI changes end */

/*
** Function Prototypes
*/
typedef struct Proto {
  CommonHeader;
  lu_byte numparams;  /* number of fixed (named) parameters */
  lu_byte is_vararg;
  lu_byte maxstacksize;  /* number of registers needed by this function */
  int sizeupvalues;  /* size of 'upvalues' */
  int sizek;  /* size of 'k' */
  int sizecode;
  int sizelineinfo;
  int sizep;  /* size of 'p' */
  int sizelocvars;
  int linedefined;  /* debug information  */
  int lastlinedefined;  /* debug information  */
  TValue *k;  /* constants used by the function */
  Instruction *code;  /* opcodes */
  struct Proto **p;  /* functions defined inside the function */
  int *lineinfo;  /* map from opcodes to source lines (debug information) */
  LocVar *locvars;  /* information about local variables (debug information) */
  Upvaldesc *upvalues;  /* upvalue information */
  struct LClosure *cache;  /* last-created closure with this prototype */
  TString  *source;  /* used for debug information */
  GCObject *gclist;
  /* RAVI extension */
  RaviJITProto ravi_jit;
} Proto;

/*
** {==================================================================
** Closures
** ===================================================================
*/


/*
** LUA_TFUNCTION variants:
** 0 - Lua function
** 1 - light C function
** 2 - regular C function (closure)
** 4 - fast light C dunction (Ravi extension)
*/

/* Variant tags for functions */
#define LUA_TLCL	makevariant(LUA_TFUNCTION, 0)  /* Lua closure */
#define LUA_TLCF	makevariant(LUA_TFUNCTION, 1)  /* light C function */
#define LUA_TCCL	makevariant(LUA_TFUNCTION, 2)  /* C closure */
#define RAVI_TFCF	makevariant(LUA_TFUNCTION, 4)  /* Ravi extension: fast light C function */

#define ttisfunction(o)		checktype(o, LUA_TFUNCTION)
#define ttisclosure(o)		((rttype(o) & 0x1F) == LUA_TFUNCTION)
#define ttisLclosure(o)		checktag((o), ctb(LUA_TLCL))
#define ttislcf(o)		checktag((o), LUA_TLCF)

#define ttisCclosure(o)		checktag((o), ctb(LUA_TCCL))
#define ttisfcf(o)      (ttype(o) == RAVI_TFCF)
#define isLfunction(o)	ttisLclosure(o)

#define clvalue(o)	check_exp(ttisclosure(o), gco2cl(val_(o).gc))
#define clLvalue(o)	check_exp(ttisLclosure(o), gco2lcl(val_(o).gc))
#define fvalue(o)	check_exp(ttislcf(o), val_(o).f)
#define clCvalue(o)	check_exp(ttisCclosure(o), gco2ccl(val_(o).gc))
#define fcfvalue(o) check_exp(ttisfcf(o), val_(o).p)

#define setclLvalue(L,obj,x) \
  { TValue *io = (obj); LClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TLCL)); \
    checkliveness(L,io); }

#define setfvalue(obj,x) \
  { TValue *io=(obj); val_(io).f=(x); settt_(io, LUA_TLCF); }

#define setclCvalue(L,obj,x) \
  { TValue *io = (obj); CClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TCCL)); \
    checkliveness(L,io); }
	

/* The Fast C function call type is encoded as two
   bytes. The Hi Byte holds a function tag. The Lo Byte
   holds the Lua typecode */
#define setfvalue_fastcall(obj, x, tag) \
  {                       \
    TValue *io = (obj);   \
    lua_assert(tag >= 1 && tag < 0x80); \
    val_(io).p = (x);     \
    settt_(io, ((tag << 8) | RAVI_TFCF)); \
  }
#define getfcf_tag(typecode) (typecode >> 8)

/*
** Upvalues for Lua closures
*/

typedef struct UpVal {
  TValue *v;  /* points to stack or to its own value */
#ifdef RAVI_DEFER_STATEMENT
  unsigned int refcount;  /* reference counter */
  unsigned int flags; /* Used to mark deferred values */
#else
  lu_mem refcount;  /* reference counter */
#endif
  union {
    struct {  /* (when open) */
      struct UpVal *next;  /* linked list */
      int touched;  /* mark to avoid cycles with dead threads */
    } open;
    TValue value;  /* the value (when closed) */
  } u;
} UpVal;



#define ClosureHeader \
	CommonHeader; lu_byte nupvalues; GCObject *gclist

typedef struct CClosure {
  ClosureHeader;
  lua_CFunction f;
  TValue upvalue[1];  /* list of upvalues */
} CClosure;


typedef struct LClosure {
  ClosureHeader;
  struct Proto *p;
  UpVal *upvals[1];  /* list of upvalues */
} LClosure;


typedef union Closure {
  CClosure c;
  LClosure l;
} Closure;


#define getproto(o)	(clLvalue(o)->p)

/* }================================================================== */


/*
** {==================================================================
** Tables
** ===================================================================
*/

/* RAVI change: we support two sub types of table type
   and hence need to distinguish between the types.
   ttistable() returns true for all table types
   ttisLtable() only returns true if the value is a Lua table 
   ttisiarray() only returns true if the value is a Ravi subtype integer[]
   ttisfarray() only returns true if the value is a Ravi subtype number[]
 */
/** RAVI table subtypes **/
#define RAVI_TIARRAY makevariant(LUA_TTABLE, 1)  /* Ravi int array */
#define RAVI_TFARRAY makevariant(LUA_TTABLE, 2)  /* Ravi float array */

#define ttistable(o)     checktype((o), LUA_TTABLE)
#define ttisiarray(o)    checktag((o), ctb(RAVI_TIARRAY))
#define ttisfarray(o)    checktag((o), ctb(RAVI_TFARRAY))
#define ttisarray(o)     (ttisiarray(o) || ttisfarray(o))
#define ttisLtable(o)    checktag((o), ctb(LUA_TTABLE))

#define ttisdeadkey(o)		checktag((o), LUA_TDEADKEY)

/* Macros to access values */

#define hvalue(o)	check_exp(ttisLtable(o), gco2t(val_(o).gc))
#define arrvalue(o) check_exp(ttisarray(o), gco2array(val_(o).gc))
/* a dead value may get the 'gc' field, but cannot access its contents */
#define deadvalue(o)	check_exp(ttisdeadkey(o), cast(void *, val_(o).gc))

#define sethvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTABLE)); \
    checkliveness(L,io); }

/** RAVI extension **/
#define setiarrayvalue(L,obj,x) \
  { TValue *io = (obj); RaviArray *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(RAVI_TIARRAY)); \
    checkliveness(L,io); }

/** RAVI extension **/
#define setfarrayvalue(L,obj,x) \
  { TValue *io = (obj); RaviArray *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(RAVI_TFARRAY)); \
    checkliveness(L,io); }

#define sethvalue2s	sethvalue
#define setptvalue2s	setptvalue


typedef union TKey {
  struct {
    TValuefields;
    int next;  /* for chaining (offset for next node) */
  } nk;
  TValue tvk;
} TKey;




typedef struct Node {
  TValue i_val;
  TKey i_key;
} Node;

/* copy a value into a key without messing up field 'next' */
#define setnodekey(L,key,obj) \
	{ TKey *k_=(key); const TValue *io_=(obj); \
	  k_->nk.value_ = io_->value_; k_->nk.tt_ = io_->tt_; \
	  (void)L; checkliveness(L,io_); }

typedef struct Table {
  CommonHeader;
  lu_byte flags;  /* 1<<p means tagmethod(p) is not present */
  lu_byte lsizenode;  /* log2 of size of 'node' array */
  unsigned int sizearray;  /* size of 'array' array */
  TValue *array;  /* array part */
  Node *node;
  Node *lastfree;  /* any free position is before this position */
  struct Table *metatable;
  GCObject *gclist;
#if RAVI_USE_NEWHASH
  // TODO we should reorganize this structure
  unsigned int hmask; /* Hash part mask (size of hash part - 1) - borrowed from LuaJIT */
#endif
} Table;

/*
** Macros to manipulate keys inserted in nodes
*/
#define keytt(node)         ((node)->i_key.nk.tt_)
#define keyval(node)        ((node)->i_key.nk.value_)

#define keyisnil(node)      (keytt(node) == LUA_TNIL)
#define keyisinteger(node)  (keytt(node) == LUA_TNUMINT)
#define keyival(node)       (keyval(node).i)
#define keyfltval(node)     (keyval(node).n)
#define keyfval(node)       (keyval(node).f)
#define keypval(node)       (keyval(node).p)
#define keybval(node)       (keyval(node).b)
#define keygcval(node)      (keyval(node).gc)
#define keyisshrstr(node)   (keytt(node) == ctb(LUA_TSHRSTR))
#define keystrval(node)     (gco2ts(keyval(node).gc))

#define setnilkey(node)		(keytt(node) = LUA_TNIL)

#define keyiscollectable(n)	(keytt(n) & BIT_ISCOLLECTABLE)

#define gckey(n)	(keyval(n).gc)
#define gckeyN(n)	(keyiscollectable(n) ? gckey(n) : NULL)

#define setdeadvalue(obj)	settt_(obj, LUA_TDEADKEY)

/* }================================================================== */

/** RAVI extension */
typedef enum RaviArrayModifer {
  RAVI_ARRAY_SLICE = 1, /* Array is a slice - implies fixed size */
  RAVI_ARRAY_FIXEDSIZE = 2, /* Fixed size array */
  RAVI_ARRAY_ALLOCATED = 4, /* Array has memory allocated - cannot be true for slices */
  RAVI_ARRAY_ISFLOAT = 8 /* A number array */
} RaviArrayModifier;

enum {
  RAVI_ARRAY_MAX_INLINE = 3 /* By default we allow for inline storage of 3 elements */,
};

/** RAVI extension */
/* Array types look like Userdata from GC point of view, but
 * have the same base type as Lua tables.
 */
typedef struct RaviArray {
  CommonHeader;
  lu_byte flags;
  unsigned int len;  /* array length, holds real length which is 1+Lua length */
  unsigned int size; /* size of data, in arrays (not slices) if == RAVI_ARRAY_MAX_INLINE then it means we are using inline storage */
  union {
    lua_Number numarray[RAVI_ARRAY_MAX_INLINE];
    lua_Integer intarray[RAVI_ARRAY_MAX_INLINE];
    struct RaviArray* parent; /* Only set if this is a slice, parent must be a slice or a fixed length array */
  };
  char *data;    /* Pointer to data. In case of slices points in parent->data. In case of arrays this may point to heap or internal data */
  struct Table *metatable;
} RaviArray;


/*
** 'module' operation for hashing (size is always a power of 2)
*/
#define lmod(s,size) \
	(check_exp((size&(size-1))==0, (cast(int, (s) & ((size)-1)))))


#define twoto(x)	(1<<(x))
#define sizenode(t)	(twoto((t)->lsizenode))


/*
** (address of) a fixed nil value
*/
#define luaO_nilobject		(&luaO_nilobject_)

LUAI_DDEC const TValue luaO_nilobject_;

/* size of buffer for 'luaO_utf8esc' function */
#define UTF8BUFFSZ	8

LUAI_FUNC int luaO_int2fb (unsigned int x);
LUAI_FUNC int luaO_fb2int (int x);
LUAI_FUNC int luaO_utf8esc (char *buff, unsigned long x);
LUAI_FUNC int luaO_ceillog2 (unsigned int x);
LUAI_FUNC void luaO_arith (lua_State *L, int op, const TValue *p1,
                           const TValue *p2, TValue *res);
LUAI_FUNC size_t luaO_str2num (const char *s, TValue *o);
LUAI_FUNC int luaO_hexavalue (int c);
LUAI_FUNC void luaO_tostring (lua_State *L, StkId obj);
LUAI_FUNC const char *luaO_pushvfstring (lua_State *L, const char *fmt,
                                                       va_list argp);
LUAI_FUNC const char *luaO_pushfstring (lua_State *L, const char *fmt, ...);
LUAI_FUNC void luaO_chunkid (char *out, const char *source, size_t srclen);


#endif


#ifndef HEADER
#define HEADER

struct lua_State;
typedef unsigned char lu_byte;
typedef signed char ls_byte;
typedef int (*lua_CFunction) (struct lua_State *L);

#if 1 /* 64bit */
typedef long long lua_Integer;
typedef unsigned long long lua_Unsigned;
typedef double lua_Number;
typedef unsigned long long size_t;
typedef unsigned int Instruction;
typedef long long ptrdiff_t;
typedef size_t lu_mem;
typedef ptrdiff_t l_mem;
typedef ptrdiff_t intptr_t;
#else

#endif

struct GCObject {
  struct GCObject *next;
  lu_byte tt;
  lu_byte marked;
};

union Value {
  struct GCObject *gc; /* collectable objects */
  void *p;             /* light userdata */
  int b;               /* booleans */
  lua_CFunction f;     /* light C functions */
  lua_Integer i;       /* integer numbers */
  lua_Number n;        /* float numbers */
};

struct TValue {
  union Value value_;
  lu_byte tt_;
};

struct TString {
  struct GCObject *next;
  lu_byte tt;
  lu_byte marked;
  lu_byte extra;       /* reserved words for short strings; "has hash" for longs */
  lu_byte shrlen;      /* length for short strings */
  unsigned int hash;
  union {
    size_t lnglen;     /* length for long strings */
    struct TString *hnext;  /* linked list for hash table */
  } u;
};

union MaxAlign {
  lua_Number n;
  double u;
  void *s;
  lua_Integer i;
  long l;
};

union UTString {
  union MaxAlign dummy;  /* ensures maximum alignment for strings */
  struct TString tsv;
};

struct Udata {
  struct GCObject *next;
  lu_byte tt;
  lu_byte marked;
  lu_byte ttuv_; /* user value's tag */
  struct Table *metatable;
  size_t len; /* number of bytes */
  union Value user_;      /* user value */
};

union UUdata {
  union MaxAlign dummy; /* ensures maximum alignment for 'local' udata */
  struct Udata uv;
};

enum ravitype_t {
  RAVI_TANY,      /* Lua dynamic type */
  RAVI_TNUMINT,   /* integer number */
  RAVI_TNUMFLT,   /* floating point number */
  RAVI_TARRAYINT, /* array of ints */
  RAVI_TARRAYFLT, /* array of doubles */
  RAVI_TFUNCTION,
  RAVI_TTABLE,
  RAVI_TSTRING,
  RAVI_TNIL,
  RAVI_TBOOLEAN
};

struct Upvaldesc {
  struct TString *name;  /* upvalue name (for debug information) */
  struct TString *usertype; /* RAVI extension: name of user type */
  lu_byte ravi_type; /* RAVI type of upvalue */
  lu_byte instack; /* whether it is in stack */
  lu_byte idx; /* index of upvalue (in stack or in outer function's list) */
};

struct LocVar {
  struct TString *varname;
  struct TString *usertype; /* RAVI extension: name of user type */
  int startpc; /* first point where variable is active */
  int endpc;   /* first point where variable is dead */
  lu_byte ravi_type; /* RAVI type of the variable - RAVI_TANY if unknown */
};

struct RaviJITProto {
  lu_byte jit_status; // 0=not compiled, 1=can't compile, 2=compiled, 3=freed
  void *jit_data;
  int (*jit_function) (struct lua_State *);
};


struct Proto {
  struct GCObject *next;
  lu_byte tt;
  lu_byte marked;
  lu_byte numparams; /* number of fixed parameters */
  lu_byte is_vararg; /* 2: declared vararg; 1: uses vararg */
  lu_byte maxstacksize; /* maximum stack used by this function */
  int sizeupvalues;           /* size of 'upvalues' */
  int sizek;                  /* size of 'k' */
  int sizecode;
  int sizelineinfo;
  int sizep; /* size of 'p' */
  int sizelocvars;
  int linedefined;  /* debug information  */
  int lastlinedefined;  /* debug information  */
  struct TValue *k; /* constants used by the function */
  Instruction *code;  /* opcodes */
  struct Proto **p; /* functions defined inside the function */
  int *lineinfo;    /* map from opcodes to source lines (debug information) */
  struct LocVar *
      locvars; /* information about local variables (debug information) */
  struct Upvaldesc *upvalues; /* upvalue information */
  struct LClosure *cache;     /* last created closure with this prototype */
  struct TString *source;     /* used for debug information */
  struct GCObject *gclist;
  lu_byte cachemiss;
  struct RaviJITProto ravi_jit;
};

struct UpVal;

struct CClosure {
  struct GCObject *next;
  lu_byte tt;
  lu_byte marked;
  lu_byte nupvalues;
  struct GCObject *gclist;
  lua_CFunction f;
  struct TValue upvalue[1]; /* list of upvalues */
};

struct LClosure {
  struct GCObject *next;
  lu_byte tt;
  lu_byte marked;
  lu_byte nupvalues;
  struct GCObject *gclist;
  struct Proto *p;
  struct UpVal *upvals[1]; /* list of upvalues */
};

union Closure {
  struct CClosure c;
  struct LClosure l;
};

union Node {
  struct NodeKey {
    union Value value_;
    lu_byte tt_;
    lu_byte key_tt;  /* key type */
    int next;  /* for chaining */
    union Value key_val;  /* key value */
  } u;
  struct TValue i_val;  /* direct access to node's value as a proper 'TValue' */
};

struct RaviArray {
  char *data;
  enum ravitype_t type; /* RAVI specialization */
  unsigned int len; /* RAVI len specialization */
  unsigned int size; /* amount of memory allocated */
};

struct Table {
  struct GCObject *next;
  lu_byte tt;
  lu_byte marked;
  lu_byte flags;     /* 1<<p means tagmethod(p) is not present */
  lu_byte lsizenode; /* log2 of size of 'node' array */
  unsigned int sizearray;  /* size of 'array' array */
  struct TValue *array;    /* array part */
  union Node *node;
  union Node *lastfree; /* any free position is before this position */
  struct Table *metatable;
  struct GCObject *gclist;
  struct RaviArray ravi_array;
  unsigned int hmask;
};

/* lzio.h */
struct Mbuffer {
  char *buffer;
  size_t n;
  size_t buffsize;
};

/* lstate.h */
struct stringtable {
  struct TString **hash;
  int nuse; /* number of elements */
  int size;
};

struct lua_Debug;
typedef intptr_t lua_KContext;
typedef int (*lua_KFunction)(struct lua_State *L, int status, lua_KContext ctx);
typedef void *(*lua_Alloc)(void *ud, void *ptr, size_t osize,
                           size_t nsize);
typedef void (*lua_Hook)(struct lua_State *L, struct lua_Debug *ar);

struct CallInfoL {     /* only for Lua functions */
  struct TValue *base; /* base for this function */
  const Instruction *savedpc;
  ptrdiff_t dummy;     /* not present but for padding to match CallInfoC */
};

struct CallInfoC { /* only for C functions */
  lua_KFunction k; /* continuation in case of yields */
  ptrdiff_t old_errfunc;
  lua_KContext ctx; /* context info. in case of yields */
};

struct CallInfo {
  struct TValue *func;              /* function index in the stack */
  struct TValue *top;               /* top for this function */
  struct CallInfo *previous, *next; /* dynamic call link */
  union {
    struct CallInfoL l;
    struct CallInfoC c;
  } u;
  ptrdiff_t extra;
  short nresults; /* expected number of results from this function */
  lu_byte callstatus;
  lu_byte jitstatus;
};

struct CallInfoLua {
  struct TValue *func;              /* function index in the stack */
  struct TValue *top;               /* top for this function */
  struct CallInfo *previous, *next; /* dynamic call link */
  struct CallInfoL l;
  ptrdiff_t extra;
  short nresults; /* expected number of results from this function */
  lu_byte callstatus;
  lu_byte jitstatus;  /* Only valid if Lua function - if 1 means JITed - RAVI extension */
  short stacklevel;   /* Ravi extension - stack level, bootom level is 0 */
};

struct global_State;
struct lua_longjmp;

/*
** 'per thread' state
*/
struct lua_State {
  struct GCObject *next;
  lu_byte tt;
  lu_byte marked;
  lu_byte status;
  struct TValue *top; /* first free slot in the stack */
  struct global_State *l_G;
  struct CallInfoLua *ci;    /* call info for current function */
  const Instruction *oldpc; /* last pc traced */
  struct TValue *stack_last; /* last free slot in the stack */
  struct TValue *stack;      /* stack base */
  struct UpVal *openupval;   /* list of open upvalues in this stack */
  struct GCObject *gclist;
  struct lua_State *twups;      /* list of threads with open upvalues */
  struct lua_longjmp *errorJmp; /* current error recover point */
  struct CallInfo base_ci;      /* CallInfo for first level (C calling Lua) */
  volatile lua_Hook hook;
  ptrdiff_t errfunc; /* current error handling function (stack index) */
  int stacksize;
  int basehookcount;
  int hookcount;
  unsigned short nny;     /* number of non-yieldable calls in stack */
  unsigned short nCcalls; /* number of nested C calls */
  lu_byte hookmask;       /* Lua 5.3 uses l_signalT */
  lu_byte allowhook;
  unsigned short nci;     /* number of items in 'ci' list  (different position than Lua 5.3) */
};

/* lfunc.h */
struct UpVal {
  struct GCObject *next;
  lu_byte tt;
  lu_byte marked;
  struct TValue *v;  /* points to stack or to its own value */
  union {
    struct {  /* (when open) */
      struct UpVal *next;  /* linked list */
      struct UpVal **previous;
    } open;
    struct TValue value;  /* the value (when closed) */
  } u;
};

/*
** Union of all collectable objects (only for conversions)
*/
union GCUnion {
  struct GCObject gc;  /* common header */
  struct TString ts;
  struct Udata u;
  union Closure cl;
  struct Table h;
  struct Proto p;
  struct lua_State th;  /* thread */
};


#define rttype(o) ((o)->tt_)
#define BIT_ISCOLLECTABLE (1 << 6)
#define iscollectable(o)  (rttype(o) & BIT_ISCOLLECTABLE)

#define bitmask(b)    (1<<(b))
#define bit2mask(b1,b2)   (bitmask(b1) | bitmask(b2))
#define testbits(x,m)   ((x) & (m))
#define testbit(x,b)    testbits(x, bitmask(b))

#define WHITE0BIT 3  /* object is white (type 0) */
#define WHITE1BIT 4  /* object is white (type 1) */
#define BLACKBIT  5  /* object is black */
#define FINALIZEDBIT  6  /* object has been marked for finalization */
#define TESTGRAYBIT 7  /* used by tests (luaL_checkmemory) */
#define WHITEBITS bit2mask(WHITE0BIT, WHITE1BIT)

#define isblack(x)      testbit((x)->marked, BLACKBIT)
#define iswhite(x)      testbits((x)->marked, WHITEBITS)

#define val_(o)   ((o)->value_)
#define cast(t, exp)  ((t)(exp))
#define cast_u(o) cast(union GCUnion *, (o))
#define gco2t(o)  &((cast_u(o))->h)
#define hvalue(o) gco2t(val_(o).gc)
#define gcvalue(o)  (val_(o).gc)
#endif
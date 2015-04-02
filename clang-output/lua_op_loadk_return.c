extern int printf(const char *, ...);

struct GCObject {
  struct GCObject *next;
  unsigned char tt;
  unsigned char marked;
};

struct lua_State;

union Value {
  struct GCObject *gc;
  void *p;
  int b;
  int (*f)(struct lua_State *);
  long long i;
  double n;
};

struct TValue {
  union Value value_;
  int tt_;
};

struct TString {
  struct GCObject *next;
  unsigned char tt;
  unsigned char marked;
  unsigned char extra;
  unsigned int hash;
  unsigned long long len;
  struct TString *hnext;
};

union MaxAlign {
  double u;
  void *s;
  long long i;
  long l;
};

union UTString {
  union MaxAlign dummy;
  struct TString tsv;
};

struct Udata {
  struct GCObject *next;
  unsigned char tt;
  unsigned char marked;
  unsigned char ttuv_; /* user value's tag */
  struct Table *metatable;
  unsigned long long len; /* number of bytes */
  union Value user_;      /* user value */
};

union UUdata {
  union MaxAlign dummy; /* ensures maximum alignment for 'local' udata */
  struct Udata uv;
};

struct Upvaldesc {
  struct TString *name;  /* upvalue name (for debug information) */
  unsigned char instack; /* whether it is in stack */
  unsigned char
      idx; /* index of upvalue (in stack or in outer function's list) */
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

struct LocVar {
  struct TString *varname;
  int startpc; /* first point where variable is active */
  int endpc;   /* first point where variable is dead */
  enum ravitype_t
      ravi_type; /* RAVI type of the variable - RAVI_TANY if unknown */
};

struct Proto {
  struct GCObject *next;
  unsigned char tt;
  unsigned char marked;
  unsigned char numparams; /* number of fixed parameters */
  unsigned char is_vararg;
  unsigned char maxstacksize; /* maximum stack used by this function */
  int sizeupvalues;           /* size of 'upvalues' */
  int sizek;                  /* size of 'k' */
  int sizecode;
  int sizelineinfo;
  int sizep; /* size of 'p' */
  int sizelocvars;
  int linedefined;
  int lastlinedefined;
  struct TValue *k; /* constants used by the function */
  unsigned int *code;
  struct Proto **p; /* functions defined inside the function */
  int *lineinfo;    /* map from opcodes to source lines (debug information) */
  struct LocVar *
      locvars; /* information about local variables (debug information) */
  struct Upvaldesc *upvalues; /* upvalue information */
  struct LClosure *cache;     /* last created closure with this prototype */
  struct TString *source;     /* used for debug information */
  struct GCObject *gclist;
} Proto;

struct UpVal;

struct CClosure {
  struct GCObject *next;
  unsigned char tt;
  unsigned char marked;
  unsigned char nupvalues;
  struct GCObject *gclist;
  int (*f)(struct lua_State *);
  struct TValue upvalue[1]; /* list of upvalues */
};

struct LClosure {
  struct GCObject *next;
  unsigned char tt;
  unsigned char marked;
  unsigned char nupvalues;
  struct GCObject *gclist;
  struct Proto *p;
  struct UpVal *upvals[1]; /* list of upvalues */
};

union Closure {
  struct CClosure c;
  struct LClosure l;
};

union TKey {
  struct {
    union Value value_;
    int tt_;
    int next; /* for chaining (offset for next node) */
  } nk;
  struct TValue tvk;
};

struct Node {
  struct TValue i_val;
  union TKey i_key;
};

struct Table {
  struct GCObject *next;
  unsigned char tt;
  unsigned char marked;
  unsigned char flags;     /* 1<<p means tagmethod(p) is not present */
  unsigned char lsizenode; /* log2 of size of 'node' array */
  unsigned int sizearray;  /* size of 'array' array */
  struct TValue *array;    /* array part */
  struct Node *node;
  struct Node *lastfree; /* any free position is before this position */
  struct Table *metatable;
  struct GCObject *gclist;
  char *ravi_data;
  enum ravitype_t ravi_array_type; /* RAVI specialization */
  unsigned int ravi_array_len;     /* RAVI len specialization */
  unsigned int ravi_array_size;     /* RAVI len specialization */  
};

struct Mbuffer {
  char *buffer;
  unsigned long long n;
  unsigned long long buffsize;
};

struct stringtable {
  struct TString **hash;
  int nuse; /* number of elements */
  int size;
};

struct lua_Debug;
typedef long long ptrdiff_t;
typedef ptrdiff_t lua_KContext;
typedef int (*lua_KFunction)(struct lua_State *L, int status, lua_KContext ctx);
typedef void *(*lua_Alloc)(void *ud, void *ptr, unsigned long long osize,
                           unsigned long long nsize);
typedef void (*lua_Hook)(struct lua_State *L, struct lua_Debug *ar);

struct CallInfoL {     /* only for Lua functions */
  struct TValue *base; /* base for this function */
  const unsigned int *savedpc;
  ptrdiff_t dummy;
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
  unsigned char callstatus;
  unsigned char jitstatus;
};

struct CallInfoLua {
  struct TValue *func;              /* function index in the stack */
  struct TValue *top;               /* top for this function */
  struct CallInfo *previous, *next; /* dynamic call link */
  struct CallInfoL l;
  ptrdiff_t extra;
  short nresults; /* expected number of results from this function */
  unsigned char callstatus;
  unsigned char jitstatus;
};

struct global_State;
struct lua_longjmp;

/*
** 'per thread' state
*/
struct lua_State {
  struct GCObject *next;
  unsigned char tt;
  unsigned char marked;
  unsigned char status;
  struct TValue *top; /* first free slot in the stack */
  struct global_State *l_G;
  struct CallInfoLua *ci;    /* call info for current function */
  const unsigned int *oldpc; /* last pc traced */
  struct TValue *stack_last; /* last free slot in the stack */
  struct TValue *stack;      /* stack base */
  struct UpVal *openupval;   /* list of open upvalues in this stack */
  struct GCObject *gclist;
  struct lua_State *twups;      /* list of threads with open upvalues */
  struct lua_longjmp *errorJmp; /* current error recover point */
  struct CallInfo base_ci;      /* CallInfo for first level (C calling Lua) */
  lua_Hook hook;
  ptrdiff_t errfunc; /* current error handling function (stack index) */
  int stacksize;
  int basehookcount;
  int hookcount;
  unsigned short nny;     /* number of non-yieldable calls in stack */
  unsigned short nCcalls; /* number of nested C calls */
  unsigned char hookmask;
  unsigned char allowhook;
};

void testfunc(struct GCObject *obj) { printf("value = %d\n", obj->tt); }

extern int luaD_poscall(struct lua_State *L, struct TValue *ra);
extern void luaF_close(struct lua_State *L, struct TValue *base);

/*
 The following represents a C version of the Lua function:
 
 local function x()
   return 1004,2
 end

 The Lua byte codes are:

 function <(string):1,1> (4 instructions at 00000033C3366630)
 0 params, 2 slots, 0 upvalues, 0 locals, 2 constants, 0 functions
 1       [1]     LOADK           0 -1    ; 1004
 2       [1]     LOADK           1 -2    ; 2
 3       [1]     RETURN          0 3
 4       [1]     RETURN          0 1
 constants (2) for 00000033C3366630:
 1       1004
 2       2
 locals (0) for 00000033C3366630:
 upvalues (0) for 00000033C3366630:

 */
void test1(struct lua_State *L) {

  /* This is the function prologue */
  struct CallInfoLua *ci;
  struct LClosure *cl;
  struct TValue *k;
  struct TValue *base;
  struct CallInfoL *cil;

  ci = L->ci;
  base = ci->l.base;
  cl = (struct LClosure *)(ci->func->value_.gc);
  k = cl->p->k;

  /* First LOADK instruction */
  struct TValue *ra = base + 0;
  struct TValue *rb = k + 0;
  *ra = *rb;

  /* Second LOADK instruction */
  struct TValue *ra2 = base + 1;
  struct TValue *rb2 = k + 1;
  *ra2 = *rb2;

  /* OP_RETURN instruction */
  int b = 3;
  struct TValue *ra3 = base + 0;
  // if (b)
  L->top = ra3 + b - 1;
  if (cl->p->sizep > 0) 
    luaF_close(L, base);
  b = luaD_poscall(L, ra3);
  if (b)
    L->top = ci->top;
}
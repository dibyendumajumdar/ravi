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

/*
 The following represents a C version of the Lua function:

 local function x()
   local j = 0
   for i=1,5 do 
     j = i
   end
   return j
 end

 The Lua byte codes are:

 function <(string):1,1> (9 instructions at 00000014E5C8E3
 0 params, 5 slots, 0 upvalues, 5 locals, 3 constants, 0 f
 1       [1]     LOADK           0 -1    ; 0
 2       [1]     LOADK           1 -2    ; 1
 3       [1]     LOADK           2 -3    ; 5
 4       [1]     LOADK           3 -2    ; 1
 5       [1]     FORPREP         1 1     ; to 7
 6       [1]     MOVE            0 4
 7       [1]     FORLOOP         1 -2    ; to 6
 8       [1]     RETURN          0 2
 9       [1]     RETURN          0 1
 constants (3) for 00000014E5C8E380:
 1       0
 2       1
 3       5
 locals (5) for 00000014E5C8E380:
 0       j       2       10
 1       (for index)     5       8
 2       (for limit)     5       8
 3       (for step)      5       8
 4       i       6       7
 upvalues (0) for 00000014E5C8E380: 
*/

extern int forlimit(const struct TValue *obj, long long int *p, long long int step,
  int *stopnow);
extern int luaV_tonumber_(const struct TValue *obj, double *n);

void test1(struct lua_State *L) {

  /* This is the function prologue */
  struct CallInfoLua *ci;
  struct LClosure *cl;
  struct TValue *k;
  struct TValue *base;
  struct CallInfoL *cil;
  struct TValue *ra, *rb, *rc, *rd;
  int A;

  ci = L->ci;
  base = ci->l.base;
  cl = (struct LClosure *)(ci->func->value_.gc);
  k = cl->p->k;

label_loopbody:
  printf("dummy");

  #define LUA_NUMINT (3 | 1 << 4)
  #define LUA_NUMFLT (3)

label_forloop:
  // 7[1]     FORLOOP         1 - 2; to 6
  //if (ttisinteger(ra)) {  /* integer loop? */
  //  lua_Integer step = ivalue(ra + 2);
  //  lua_Integer idx = ivalue(ra) + step; /* increment index */
  //  lua_Integer limit = ivalue(ra + 1);
  //  if ((0 < step) ? (idx <= limit) : (limit <= idx)) {
  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    setivalue(ra, idx);  /* update internal index... */
  //    setivalue(ra + 3, idx);  /* ...and external index */
  //  }
  //}
  //else {  /* floating loop */
  //  lua_Number step = fltvalue(ra + 2);
  //  lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
  //  lua_Number limit = fltvalue(ra + 1);
  //  if (luai_numlt(0, step) ? luai_numle(idx, limit)
  //    : luai_numle(limit, idx)) {
  //    ci->u.l.savedpc += GETARG_sBx(i);  /* jump back */
  //    setfltvalue(ra, idx);  /* update internal index... */
  //    setfltvalue(ra + 3, idx);  /* ...and external index */
  //  }
  //}
  A = 1;
  ra = base + A;
  if (ra->tt_ == LUA_NUMINT) {
    rb = base + (A + 2);
    long long int step = rb->value_.i;
    long long int idx = ra->value_.i + step;
    rc = base + (A + 1);
    long long int limit = rc->value_.i;
    if ((0 < step) ? (idx <= limit) : (limit <= idx)) {
      ra->value_.i = idx;
      ra->tt_ = LUA_NUMINT;
      rd = base + (A + 3);
      rd->value_.i = idx;
      rd->tt_ = LUA_NUMINT;
      goto label_loopbody;
    }
  }
  else {
    rb = base + (A + 2);
    double step = rb->value_.n;
    double idx = ra->value_.n + step;
    rc = base + (A + 1);
    double limit = rc->value_.n;
    if ((0 < step) ? (idx <= limit) : (limit <= idx)) {
      ra->value_.n = idx;
      ra->tt_ = LUA_NUMFLT;
      rd = base + (A + 3);
      rd->value_.n = idx;
      rd->tt_ = LUA_NUMFLT;
      goto label_loopbody;
    }
  }

}
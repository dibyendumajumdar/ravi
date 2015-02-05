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
  enum ravitype_t ravi_array_type; /* RAVI specialization */
  unsigned int ravi_array_len;     /* RAVI len specialization */
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
};

struct CallInfoLua {
  struct TValue *func;              /* function index in the stack */
  struct TValue *top;               /* top for this function */
  struct CallInfo *previous, *next; /* dynamic call link */
  struct CallInfoL l;
  ptrdiff_t extra;
  short nresults; /* expected number of results from this function */
  unsigned char callstatus;
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

 local function x(y)
   if y == 1 then
     return 1.0
   elseif y == 5 then
     return 2.0
   else
     return 3.0
   end
 end

 The Lua byte codes are:

 function <(string):1,1> (13 instructions at 000000D6D5D26630)
 1 param, 2 slots, 0 upvalues, 1 local, 5 constants, 0 functions
 1       [1]     EQ              0 0 -1  ; - 1, if true to 3
 2       [1]     JMP             0 3     ; to 6
 3       [1]     LOADK           1 -2    ; 1.0
 4       [1]     RETURN          1 2
 5       [1]     JMP             0 7     ; to 13, never reached
 6       [1]     EQ              0 0 -3  ; - 5, if true to 8
 7       [1]     JMP             0 3     ; to 11
 8       [1]     LOADK           1 -4    ; 2.0
 9       [1]     RETURN          1 2
 10      [1]     JMP             0 2     ; to 13, never reached
 11      [1]     LOADK           1 -5    ; 3.0
 12      [1]     RETURN          1 2
 13      [1]     RETURN          0 1
 constants (5) for 000000D6D5D26630:
 1       1
 2       1.0
 3       5
 4       2.0
 5       3.0
 locals (1) for 000000D6D5D26630:
 0       y       1       14
 upvalues (0) for 000000D6D5D26630:
 */
void test1(struct lua_State *L) {

  /* This is the function prologue */
  struct CallInfoLua *ci;
  struct LClosure *cl;
  struct TValue *k;
  struct TValue *base;
  struct CallInfoL *cil;
  struct TValue *ra2, *rb2, *ra3;
  int b;

  ci = L->ci;
  base = ci->l.base;
  cl = (struct LClosure *)(ci->func->value_.gc);
  k = cl->p->k;

  /*1 [1] EQ 0 0 -1 ; - 1 */
  struct TValue *ra = base + 0;
  struct TValue *rb = base + 0;
  struct TValue *rc = k + 0;
  int eq = luaV_equalobj(L, rb, rc);
  base = ci->l.base;
  // A=0, if eq == A skip instruction 2
  if (eq == 0) {
    // i = *ci->l.savedpc;
    //  2       [1]     JMP             0 3     ; to 6
    int a = 0; //  GETARG_A(i);
    if (a > 0)
      luaF_close(L, ci->l.base + a - 1);
    // ci->u.l.savedpc += GETARG_sBx(i) + e;
    // jmp taken care below.
    goto label6;
  }

label3:
  // 3[1]     LOADK           1 - 2; 1.0
  ra2 = base + 1;
  rb2 = k + 1;
  *ra2 = *rb2;

  // 4[1]     RETURN          1 2
  b = 2;
  ra3 = base + 1;
  // if (b)
  L->top = ra3 + b - 1;
  if (cl->p->sizep > 0)
    luaF_close(L, base);
  b = luaD_poscall(L, ra3);
  if (b)
    L->top = ci->top;
  return;

  goto label_return;

label6:
  // 6[1]     EQ              0 0 -3; -5
  ra = base + 0;
  rb = base + 0;
  rc = k + 2;
  eq = luaV_equalobj(L, rb, rc);
  base = ci->l.base;
  // true; skip instruction 8
  if (eq == 0) {
    // i = *ci->l.savedpc;
    // 7[1]     JMP             0 3; to 11
    int a = 0; //  GETARG_A(i);
    if (a > 0)
      luaF_close(L, ci->l.base + a - 1);
    // ci->u.l.savedpc += GETARG_sBx(i) + e;
    // jmp taken care below.
    goto label11;
  }

label8:
  /* Second LOADK instruction */
  // 8[1]     LOADK           1 -4; 2.0
  ra2 = base + 1;
  rb2 = k + 3;
  *ra2 = *rb2;

  // 9[1]     RETURN          1 2
  b = 2;
  ra3 = base + 1;
  // if (b)
  L->top = ra3 + b - 1;
  if (cl->p->sizep > 0)
    luaF_close(L, base);
  b = luaD_poscall(L, ra3);
  if (b)
    L->top = ci->top;
  return;

  goto label_return;

label11:
  // 11[1]     LOADK           1 -5; 3.0
  // 12[1]     RETURN          1 2
  ra2 = base + 1;
  rb2 = k + 4;
  *ra2 = *rb2;

  // 9[1]     RETURN          1 2
  b = 2;
  ra3 = base + 1;
  // if (b)
  L->top = ra3 + b - 1;
  if (cl->p->sizep > 0)
    luaF_close(L, base);
  b = luaD_poscall(L, ra3);
  if (b)
    L->top = ci->top;
  return;

label_return:
  return;
}
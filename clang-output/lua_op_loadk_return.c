#include "lua_hdr.h"

extern int printf(const char *, ...);
void testfunc(struct GCObject *obj) { printf("value = %d\n", obj->tt); }
extern int luaD_poscall(struct lua_State *L, struct TValue *ra, int n);
extern void luaF_close(struct lua_State *L, struct TValue *base);
extern void test1(struct lua_State *L, int b, unsigned int x);

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
void test1(struct lua_State *L, int b, unsigned int x) {

  /* This is the function prologue */
  struct CallInfoLua *ci;
  struct LClosure *cl;
  struct TValue *k;
  struct TValue *base;
  struct CallInfoL *cil;

  ci = L->ci;
  base = ci->l.base;
  ci->l.savedpc = &x;
#if 0
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
  struct TValue *ra3 = base + 0;
  // if (b)
  //L->top = ra3 + b - 1;
  if (cl->p->sizep > 0) 
    luaF_close(L, base);
  int n = (b != 0) ? b-1 : (int)(L->top - ra); 
  b = luaD_poscall(L, ra3, n);
  if (b)
    L->top = ci->top;
#endif
}
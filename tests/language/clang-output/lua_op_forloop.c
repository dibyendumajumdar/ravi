#include "lua_hdr.h"

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

extern int printf(const char *, ...);
void testfunc(struct GCObject *obj) { printf("value = %d\n", obj->tt); }
extern int luaD_poscall(struct lua_State *L, struct TValue *ra);
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
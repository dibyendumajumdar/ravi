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
  struct TValue *ra, *rb, *rc;
  int b;

  ci = L->ci;
  base = ci->l.base;
  cl = (struct LClosure *)(ci->func->value_.gc);
  k = cl->p->k;

  // 1[1]     LOADK           0 -1; 0
  ra = base + 0;
  rb = k + 0;
  *ra = *rb;

  // 2[1]     LOADK           1 -2; 1
  ra = base + 1;
  rb = k + 1;
  *ra = *rb;

  // 3[1]     LOADK           2 -3; 5
  ra = base + 2;
  rb = k + 2;
  *ra = *rb;

  // 4[1]     LOADK           3 -2; 1
  ra = base + 3;
  rb = k + 1;
  *ra = *rb;

  // 5[1]     FORPREP         1 1; to 7
  //TValue *init = ra;
  //TValue *plimit = ra + 1;
  //TValue *pstep = ra + 2;
  //lua_Integer ilimit;
  //int stopnow;
  //if (ttisinteger(init) && ttisinteger(pstep) &&
  //  forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {
  //  /* all values are integer */
  //  lua_Integer initv = (stopnow ? 0 : ivalue(init));
  //  setivalue(plimit, ilimit);
  //  setivalue(init, initv - ivalue(pstep));
  //}
  //else {  /* try making all values floats */
  //  lua_Number ninit; lua_Number nlimit; lua_Number nstep;
  //  if (!tonumber(plimit, &nlimit))
  //    luaG_runerror(L, "'for' limit must be a number");
  //  setfltvalue(plimit, nlimit);
  //  if (!tonumber(pstep, &nstep))
  //    luaG_runerror(L, "'for' step must be a number");
  //  setfltvalue(pstep, nstep);
  //  if (!tonumber(init, &ninit))
  //    luaG_runerror(L, "'for' initial value must be a number");
  //  setfltvalue(init, luai_numsub(L, ninit, nstep));
  //}
  //ci->u.l.savedpc += GETARG_sBx(i);
label_forprep:
  ra = base + 1;
  struct TValue *init = ra;
  struct TValue *plimit = ra + 1;
  struct TValue *pstep = ra + 2;
  long long int ilimit;
  int stopnow;

  #define LUA_NUMINT (3 | 1 << 4)
  #define LUA_NUMFLT (3)
  int init_is_integer = init->tt_ == LUA_NUMINT;
  int pstep_is_integer = pstep->tt_ == LUA_NUMINT;
  int fl = forlimit(plimit, &ilimit, pstep->value_.i, &stopnow);
  if (init_is_integer && pstep_is_integer && fl) {
    long long int initv = (stopnow ? 0 : init->value_.i);
    plimit->value_.i = ilimit; plimit->tt_ = LUA_NUMINT;
    init->value_.i = initv - pstep->value_.i; init->tt_ = LUA_NUMINT;
  }
  else {
    double ninit;
    double nlimit;
    double nstep;
    int plimit_is_float = 0;
    if (plimit->tt_ == LUA_NUMFLT) {
      plimit_is_float = 1;
      nlimit = plimit->value_.n;
    }
    else {
      plimit_is_float = luaV_tonumber_(plimit, &nlimit);
    }
    if (!plimit_is_float)
      luaG_runerror(L, "'for' limit must be a number");
    plimit->value_.n = nlimit;
    plimit->tt_ = LUA_NUMFLT;

    int pstep_is_float = 0;
    if (pstep->tt_ == LUA_NUMFLT) {
      pstep_is_float = 1;
      nstep = pstep->value_.n;
    }
    else {
      pstep_is_float = luaV_tonumber_(pstep, &nstep);
    }
    if (!pstep_is_float)
      luaG_runerror(L, "'for' step must be a number");
    pstep->value_.n = nstep;
    pstep->tt_ = LUA_NUMFLT;

    int init_is_float = 0;
    if (init->tt_ == LUA_NUMFLT) {
      init_is_float = 1;
      ninit = init->value_.n;
    }
    else {
      init_is_float = luaV_tonumber_(init, &ninit);
    }
    if (!init_is_float)
      luaG_runerror(L, "'for' initial value must be a number");
    init->value_.n = ninit - nstep;
    init->tt_ = LUA_NUMFLT;
  }
  goto label_forloop;

label_loopbody:
  // 6[1]     MOVE            0 4
  ra = base + 0;
  rb = base + 4;
  *ra = *rb;

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
  ra = base + 1;
  if (ra->tt_ == LUA_NUMINT) {
    rb = ra + 2;
    long long int step = rb->value_.i;
    long long int idx = ra->value_.i + step;
    rc = ra + 1;
    long long int limit = rc->value_.i;
    if ((0 < step) ? (idx <= limit) : (limit <= idx)) {
      ra->value_.i = idx;
      ra->tt_ = LUA_NUMINT;
      rc = ra + 3;
      rc->value_.i = idx;
      rc->tt_ = LUA_NUMINT;
      goto label_loopbody;
    }
  }
  else {
    rb = ra + 2;
    double step = rb->value_.n;
    double idx = ra->value_.n + step;
    rc = ra + 1;
    double limit = rc->value_.n;
    if ((0 < step) ? (idx <= limit) : (limit <= idx)) {
      ra->value_.n = idx;
      ra->tt_ = LUA_NUMFLT;
      rc = ra + 3;
      rc->value_.n = idx;
      rc->tt_ = LUA_NUMFLT;
      goto label_loopbody;
    }
  }

  // 8[1]     RETURN          0 2
  b = 2;
  ra = base + 0;
  // if (b)
  L->top = ra + b - 1;

  printf("ptr diff %d\n", (int)(ra-L->top));

  if (cl->p->sizep > 0)
    luaF_close(L, base);
  b = luaD_poscall(L, ra);
  if (b)
    L->top = ci->top;
  return;
}
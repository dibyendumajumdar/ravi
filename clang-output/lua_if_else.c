#include "lua_hdr.h"

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

extern int printf(const char *, ...);
void testfunc(struct GCObject *obj) { printf("value = %d\n", obj->tt); }
extern int luaD_poscall(struct lua_State *L, struct TValue *ra);

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
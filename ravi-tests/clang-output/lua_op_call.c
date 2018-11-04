#include "lua_hdr.h"

extern int printf(const char *, ...);
extern int luaD_precall (struct lua_State *L, struct TValue *func, int nresults, int compile);
extern void luaV_execute(struct lua_State *L);

void luaV_op_call(struct lua_State *L, struct CallInfo *ci, struct TValue *ra, int b, int c) {
  int nresults = c - 1;
  if (b != 0) L->top = ra + b;  /* else previous instruction set top */
  int c_or_compiled = luaD_precall(L, ra, nresults, 1);
  if (c_or_compiled) {  /* C function? */
    if (c_or_compiled == 1 && nresults >= 0) 
      L->top = ci->top;  /* adjust results */
  }
  else {  /* Lua function */
    luaV_execute(L);
  }
}

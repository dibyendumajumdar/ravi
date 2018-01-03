#include "lua_hdr.h"

extern int printf(const char *, ...);
extern void luaC_upvalbarrier_ (struct lua_State *L, struct GCObject *o, struct GCObject *v);

void luaV_op_call(struct lua_State *L, struct LClosure *cl, struct TValue *ra, int b, int c) {
  struct UpVal *uv = cl->upvals[b];
  
  int ra_iscollectable = iscollectable(ra);
  int uv_isblack = isblack(uv);
  int rav_iswhite = iswhite(gcvalue(ra));
  if (ra_iscollectable && uv_isblack && rav_iswhite)
    luaC_upvalbarrier_(L,(struct GCObject *)uv, ra->value_.gc);
}

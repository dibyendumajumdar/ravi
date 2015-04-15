#include "lua_hdr.h"

extern int printf(const char *, ...);
extern void luaC_upvalbarrier_ (struct lua_State *L, struct UpVal *uv);

void luaV_op_call(struct lua_State *L, struct LClosure *cl, struct TValue *ra, int b, int c) {
  struct UpVal *uv = cl->upvals[b];
  
  uv->v->tt_ = ra->tt_;
  uv->v->value_.n = ra->value_.n;
  int b1 = iscollectable(uv->v);
  struct TValue *value = &uv->u.value;
  int b2 = uv->v != value; 
  if (b1 && !b2)
    luaC_upvalbarrier_(L,uv);
}

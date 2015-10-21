#include "lua_hdr.h"

void test1(struct lua_State *L, int b) {

  /* This is the function prologue */
  struct CallInfoLua *ci;
  struct LClosure *cl;
  struct Proto *p;

  ci = L->ci;
  cl = (struct LClosure *)(ci->func->value_.gc);
  p = cl->p;

  ci->l.savedpc = &p->code[b];
}
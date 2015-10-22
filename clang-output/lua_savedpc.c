#include "lua_hdr.h"

/*
** Event codes
*/
#define LUA_HOOKCALL    0
#define LUA_HOOKRET     1
#define LUA_HOOKLINE    2
#define LUA_HOOKCOUNT   3
#define LUA_HOOKTAILCALL 4

/*
** Event masks
*/
#define LUA_MASKCALL    (1 << LUA_HOOKCALL)
#define LUA_MASKRET     (1 << LUA_HOOKRET)
#define LUA_MASKLINE    (1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT   (1 << LUA_HOOKCOUNT)

extern void luaG_traceexec(struct lua_State *L);

void test1(struct lua_State *L, int b) {

  /* This is the function prologue */
  struct CallInfoLua *ci;
  struct LClosure *cl;
  struct Proto *p;

  ci = L->ci;
  cl = (struct LClosure *)(ci->func->value_.gc);
  p = cl->p;

  ci->l.savedpc = &p->code[b];
  if ((L->hookmask & (LUA_MASKLINE | LUA_MASKCOUNT)) &&
        (--L->hookcount == 0 || L->hookmask & LUA_MASKLINE)) {
    luaG_traceexec(L);
  }
}
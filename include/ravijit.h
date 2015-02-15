#ifndef RAVIJIT_H
#define RAVIJIT_H

#ifdef __cplusplus
extern "C" {
#endif

struct lua_State;
struct Proto;

int raviV_initjit(struct lua_State *L);
void raviV_close(struct lua_State *L);
int raviV_compile(struct lua_State *L, struct Proto *p);
void raviV_freeproto(struct lua_State *L, struct Proto *p);

#ifdef __cplusplus
}
#endif

#endif
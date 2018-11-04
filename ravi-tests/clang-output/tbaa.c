#include "lua_hdr.h"

extern int test(struct lua_State *L, int y);
extern int i;

int test(struct lua_State *L, int y) {

	int x = L->stacksize;
	y = x+i;

	return y;
}


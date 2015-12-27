#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "ravi_ast.h"

static int test_buildast(const char *code)
{
    int rc = 0;
    lua_State *L;
    L = luaL_newstate();
    if (raviL_loadbufferx(L, code, strlen(code), "testcode", NULL) != 0) {
      rc = 1;
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
      lua_pop(L, 1);  /* pop error message from the stack */
    }
    else
      raviL_dumpast(L);
    lua_close(L);
    return rc;
}

int main(void) 
{
    int failures = 0;
    //
    failures += test_buildast("return");
    return failures ? 1 : 0;
}

#define lvm_c
#define LUA_CORE

#include "lprefix.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"

#include <stdlib.h>

int test_opcodes1()
{
    lua_State *L;
    CallInfo *ci;
    
    L = (lua_State *)calloc(1, sizeof(lua_State));
    ci = (CallInfo *)calloc(1, sizeof(CallInfo));


    return 0;
}

int main(const char *argv[]) 
{
    int failures = 0;
    failures += test_opcodes1();

    return failures ? 1 : 0;
}
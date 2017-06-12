#ifndef RAVI_PROFILE_H
#define RAVI_PROFILE_H

#include "lua.h"
#include "lopcodes.h"

#if RAVI_BYTECODE_PROFILING_ENABLED

LUAI_FUNC void raviV_init_profiledata(lua_State *L);
LUAI_FUNC void raviV_add_profiledata(lua_State *L, OpCode opcode);
LUAI_FUNC void raviV_destroy_profiledata(lua_State *L);

#endif

#endif
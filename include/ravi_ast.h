/*
** $Id: lparser.h,v 1.74 2014/10/25 11:50:46 roberto Exp $
** Lua Parser
** See Copyright Notice in lua.h
*/

#ifndef ravi_ast_h
#define ravi_ast_h

#include "llimits.h"
#include "lobject.h"
#include "lzio.h"

/* 
** Experimental (wip) implementation of new
** parser and code generator 
*/
LUA_API int (ravi_load) (lua_State *L, lua_Reader reader, void *dt,
                          const char *chunkname, const char *mode);

LUALIB_API int (raviL_loadbufferx) (lua_State *L, const char *buff, size_t size,
                                 const char *name, const char *mode);

LUALIB_API int (raviL_dumpast) (lua_State *L);

#endif


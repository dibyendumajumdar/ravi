#include <ravi_omrjit.h>
#include <ravijit.h>
#include <stddef.h>
#include <assert.h>

#define LUA_CORE

#include "lauxlib.h"
#include "lobject.h"
#include "lstate.h"
#include "lua.h"


static const char Lua_header[] = ""
"typedef __SIZE_TYPE__ size_t;\n"
"typedef long long ptrdiff_t;\n"
"typedef long long intptr_t;\n"
"typedef long long int64_t;\n"
"typedef unsigned long long uint64_t;\n"
"typedef int int32_t;\n"
"typedef unsigned int uint32_t;\n"
"typedef short int16_t;\n"
"typedef unsigned short uint16_t;\n"
"typedef char int8_t;\n"
"typedef unsigned char uint8_t;\n"
"typedef size_t lu_mem;\n"
"typedef ptrdiff_t l_mem;\n"
"typedef unsigned char lu_byte;\n"
"typedef uint16_t LuaType;\n"
"#define NULL ((void *)0)\n"
"typedef struct lua_State lua_State;\n"
"#define LUA_TNONE		(-1)\n"
"#define LUA_TNIL		0\n"
"#define LUA_TBOOLEAN		1\n"
"#define LUA_TLIGHTUSERDATA	2\n"
"#define LUA_TNUMBER		3\n"
"#define LUA_TSTRING		4\n"
"#define LUA_TTABLE		5\n"
"#define LUA_TFUNCTION		6\n"
"#define LUA_TUSERDATA		7\n"
"#define LUA_TTHREAD		8\n"
"typedef double lua_Number;\n"
"typedef int64_t lua_Integer;\n"
"typedef uint64_t lua_Unsigned;\n"
"typedef int (*lua_CFunction) (lua_State *L);\n"
"extern int   (lua_absindex)(lua_State *L, int idx);\n"
"extern int   (lua_gettop)(lua_State *L);\n"
"extern void  (lua_pushvalue)(lua_State *L, int idx);\n"
"extern int   (lua_isnumber)(lua_State *L, int idx);\n"
"extern int   (lua_isstring)(lua_State *L, int idx);\n"
"extern int   (lua_iscfunction)(lua_State *L, int idx);\n"
"extern int   (lua_isinteger)(lua_State *L, int idx);\n"
"extern int   (lua_isuserdata)(lua_State *L, int idx);\n"
"extern int   (lua_type)(lua_State *L, int idx);\n"
"extern const char     *(lua_typename)(lua_State *L, int tp);\n"
"extern const char *    (ravi_typename)(lua_State *L, int idx);\n"
"extern lua_Number(lua_tonumberx) (lua_State *L, int idx, int *isnum);\n"
"extern lua_Integer(lua_tointegerx) (lua_State *L, int idx, int *isnum);\n"
"extern int   (lua_toboolean)(lua_State *L, int idx);\n"
"extern const char     *(lua_tolstring)(lua_State *L, int idx, size_t *len);\n"
"extern size_t(lua_rawlen) (lua_State *L, int idx);\n"
"extern lua_CFunction(lua_tocfunction) (lua_State *L, int idx);\n"
"extern void	       *(lua_touserdata)(lua_State *L, int idx);\n"
"extern lua_State      *(lua_tothread)(lua_State *L, int idx);\n"
"extern const void     *(lua_topointer)(lua_State *L, int idx);\n"
"#define LUA_OPADD	0\n"
"#define LUA_OPSUB	1\n"
"#define LUA_OPMUL	2\n"
"#define LUA_OPMOD	3\n"
"#define LUA_OPPOW	4\n"
"#define LUA_OPDIV	5\n"
"#define LUA_OPIDIV	6\n"
"#define LUA_OPBAND	7\n"
"#define LUA_OPBOR	8\n"
"#define LUA_OPBXOR	9\n"
"#define LUA_OPSHL	10\n"
"#define LUA_OPSHR	11\n"
"#define LUA_OPUNM	12\n"
"#define LUA_OPBNOT	13\n"
"extern void  (lua_arith)(lua_State *L, int op);\n"
"#define LUA_OPEQ	0\n"
"#define LUA_OPLT	1\n"
"#define LUA_OPLE	2\n"
"extern int   (lua_rawequal)(lua_State *L, int idx1, int idx2);\n"
"extern int   (lua_compare)(lua_State *L, int idx1, int idx2, int op);\n"
"extern void  (lua_pushnil)(lua_State *L);\n"
"extern void  (lua_pushnumber)(lua_State *L, lua_Number n);\n"
"extern void  (lua_pushinteger)(lua_State *L, lua_Integer n);\n"
"extern const char *(lua_pushlstring)(lua_State *L, const char *s, size_t len);\n"
"extern const char *(lua_pushstring)(lua_State *L, const char *s);\n"
"extern void  (lua_pushcclosure)(lua_State *L, lua_CFunction fn, int n);\n"
"extern void  (lua_pushboolean)(lua_State *L, int b);\n"
"extern void  (lua_pushlightuserdata)(lua_State *L, void *p);\n"
"extern int   (lua_pushthread)(lua_State *L);\n"
"extern int   (lua_getglobal)(lua_State *L, const char *name);\n"
"extern int   (lua_gettable)(lua_State *L, int idx);\n"
"extern int   (lua_getfield)(lua_State *L, int idx, const char *k);\n"
"extern int   (lua_geti)(lua_State *L, int idx, lua_Integer n);\n"
"extern int   (lua_rawget)(lua_State *L, int idx);\n"
"extern int   (lua_rawgeti)(lua_State *L, int idx, lua_Integer n);\n"
"extern int   (lua_rawgetp)(lua_State *L, int idx, const void *p);\n"
"extern void  (lua_createtable)(lua_State *L, int narr, int nrec);\n"
"extern void *(lua_newuserdata)(lua_State *L, size_t sz);\n"
"extern int   (lua_getmetatable)(lua_State *L, int objindex);\n"
"extern int   (lua_getuservalue)(lua_State *L, int idx);\n"
"extern void  (lua_setglobal)(lua_State *L, const char *name);\n"
"extern void  (lua_settable)(lua_State *L, int idx);\n"
"extern void  (lua_setfield)(lua_State *L, int idx, const char *k);\n"
"extern void  (lua_seti)(lua_State *L, int idx, lua_Integer n);\n"
"extern void  (lua_rawset)(lua_State *L, int idx);\n"
"extern void  (lua_rawseti)(lua_State *L, int idx, lua_Integer n);\n"
"extern void  (lua_rawsetp)(lua_State *L, int idx, const void *p);\n"
"extern int   (lua_setmetatable)(lua_State *L, int objindex);\n"
"extern void  (lua_setuservalue)(lua_State *L, int idx);\n"
;

enum {
    MAX_ARGS = 50,
    MAX_BUFFER = 4096
};

static int collect_args(lua_State *L, int tabindex, char *argv[], int maxargs,
                        char *buf, int buflen) {
  char errormessage[128];
  int len = lua_rawlen(L, tabindex);
  if (len > maxargs) {
    snprintf(errormessage, sizeof errormessage,
             "Arguments exceed total count %d elements", maxargs);
    luaL_argerror(L, 2, errormessage);
    len = maxargs;
  }
  char *p = buf;
  char *endp = buf + buflen;
  int n = 0;
  for (int i = 0; i < len; i++) {
    lua_rawgeti(L, tabindex, i + 1);
    size_t size = 0;
    const char *argument = luaL_checklstring(L, -1, &size);
    if (argument && size > 0) {
      if (p + size + 1 >= endp) {
        snprintf(errormessage, sizeof errormessage,
                 "Arguments exceed combined size of %d bytes", buflen);
        luaL_argerror(L, 2, errormessage);
        break;
      }
      strncpy(p, argument, size + 1);
      argv[n] = p;
      p += (size + 1);
      n++;
    }
  }
  assert(p <= endp);
  return n;
}

int ravi_compile_C(lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL) return 0;
  JIT_ContextRef context = G->ravi_state->jit;
  if (context == NULL) return 0;

  const char *codebuffer = NULL; 
  char *argv[MAX_ARGS + 1] = {NULL};
  int argc = 0;
  char buf[MAX_BUFFER + 1] = {0};
  int guard = 0xfefefefe;
  int codearg = 1;
  if (lua_istable(L, 1)) {
    argc = collect_args(L, 1, argv, MAX_ARGS, buf, sizeof buf);
    assert(argc >= 0 && argc <= MAX_ARGS);
    assert(argv[MAX_ARGS] == NULL);
    assert(guard == 0xfefefefe);
	codearg++;
  }
  codebuffer = luaL_checkstring(L, codearg);
  membuff_t mbuf;
  membuff_init(&mbuf, strlen(Lua_header) + 4096);
  membuff_add_string(&mbuf, Lua_header);
  membuff_add_string(&mbuf, codebuffer);  
  if (!dmrC_omrcompile(argc, argv, context, mbuf.buf)) {
    lua_pushboolean(L, false);
  }
  else {
    lua_pushboolean(L, true);
  }
  membuff_free(&mbuf);
  return 1;
}

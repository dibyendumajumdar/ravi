#include "ravi_api.h"

#define LUA_CORE

#include "lua.h"
#include "lapi.h"
#include "lauxlib.h"
#include "lfunc.h"
#include "lmem.h"
#include "lstring.h"
#include "ltable.h"

struct CompilerContext {
	lua_State* L;
	Table* h;  /* to avoid collection/reuse strings */
};

static Proto* lua_newProto(void* context, Proto* parent) {
	struct CompilerContext* ccontext = (struct CompilerContext*)context;
	lua_State* L = ccontext->L;
	Proto* p = luaF_newproto(L);
	if (parent) {
		int old_size = parent->sizep;
		int new_size = parent->sizep + 1;
		luaM_growvector(L, parent->p, old_size, new_size, Proto*, MAXARG_Bx, "functions");
		parent->p[parent->sizep++] = p;
		// luaC_objbarrier(L, f, clp);
	}
	return p;
}

/*
** creates a new string and anchors it in scanner's table so that
** it will not be collected until the end of the compilation
** (by that time it should be anchored somewhere)
*/
//TString* create_newstring(lua_State* L, Table *h, const char* str, size_t l) {
//	TValue* o;  /* entry for 'str' */
//	TString* ts = luaS_newlstr(L, str, l);  /* create new string */
//	setsvalue2s(L, L->top++, ts);  /* temporarily anchor it in stack */
//	o = luaH_set(L, h, L->top - 1);
//	if (ttisnil(o)) {  /* not in use yet? */
//	  /* boolean value does not need GC barrier;
//		 table has no metatable, so it does not need to invalidate cache */
//		setbvalue(o, 1);  /* t[string] = true */
//		luaC_checkGC(L);
//	}
//	else {  /* string already present */
//		ts = tsvalue(keyfromval(o));  /* re-use value previously stored */
//	}
//	L->top--;  /* remove string from stack */
//	return ts;
//}

/* Add a string constant to Proto and return its index */
static int lua_newStringConstant(void *context, Proto* proto, const char* s, unsigned len) {
	struct CompilerContext* ccontext = (struct CompilerContext*)context;
	lua_State* L = ccontext->L;
	Table* h = ccontext->h;
	//TString* ts = create_newstring(L, h, s, len);

}

/* Compile the C code for the given proto, and C source */
void lua_compileProto(lua_State* L, Proto* proto, const char* C_src, unsigned len) {

}

static const luaL_Reg ravilib[] = { 
                                   {NULL, NULL} };

int (raviopen_compiler)(lua_State *L) {
	luaL_newlib(L, ravilib);
	return 1;
}
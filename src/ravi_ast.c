#include <ravi_ast.h>

typedef enum {

	TNil

} TreeType;




typedef struct RaviTree {

	TreeType type; /* The type of value in the tree */


} RaviTree;


typedef struct LoadS {
  const char *s;
  size_t size;
} LoadS;


static const char *getS (lua_State *L, void *ud, size_t *size) {
  LoadS *ls = (LoadS *)ud;
  (void)L;  /* not used */
  if (ls->size == 0) return NULL;
  *size = ls->size;
  ls->size = 0;
  return ls->s;
}


LUALIB_API int raviL_loadbufferx (lua_State *L, const char *buff, size_t size,
                                 const char *name, const char *mode) {
  LoadS ls;
  ls.s = buff;
  ls.size = size;
  return ravi_load(L, getS, &ls, name, mode);
}

/* 
** Experimental (wip) implementation of new
** parser and code generator 
*/
LUA_API int (ravi_load) (lua_State *L, lua_Reader reader, void *dt,
                          const char *chunkname, const char *mode) {
  (void) reader;
  (void) dt;
  (void) chunkname;
  (void) mode;
  lua_pushnil(L);
  return 0;
}

LUALIB_API int (raviL_dumpast) (lua_State *L) {
  (void) L;
  return 0;
}



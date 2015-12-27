#include <ravi_ast.h>

typedef enum {

	/* Literals */
	TNil,
	TDots,
	TTrue,
	TFalse,
	TNumber,
	TStringLiteral,

	/* */
	TName,
	TNameList,
	TFunction,
	TLocalFunction,
	TReturn,

	/* */
	TExpList,
	TUnaryExp,
	TBinaryExp

} TreeType;

typedef struct RaviTree RaviTree;
#define RaviTree_CommonFields \
	TreeType type; /* The type of value in the tree */ \
	RaviTree *parent; /* Parent node */ \
	RaviTree *first; /* First child */ \
	RaviTree *last; /* Last child */ \
	RaviTree *next /* Next sibling */ 


struct RaviTree {
	RaviTree_CommonFields;
};

/* funcbody ::= ‘(’ [parlist] ‘)’ block end */
typedef struct RaviFunctionTree {
	RaviTree_CommonFields;

	/* parlist ::= namelist [‘,’ ‘...’] | ‘...’ */
	RaviTree *namelist;
	RaviTree *dots;

	RaviTree *block;

} RaviFunctionTree;

/* retstat ::= return [explist] [‘;’] */
typedef struct RaviReturnStatementTree {
	RaviTree_CommonFields;

	RaviTree *explist;

} RaviReturnStatementTree;


typedef struct LoadS {
  const char *s;
  size_t size;
} LoadS;

/*
** Execute a protected parser.
*/
#if 0
struct SParser {  /* data to 'f_parser' */
  ZIO *z;
  Mbuffer buff;  /* dynamic structure used by the scanner */
  Dyndata dyd;  /* dynamic structures used by the parser */
  const char *mode;
  const char *name;
};
#endif

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
LUA_API int (ravi_load) (lua_State *L, lua_Reader reader, void *data,
                          const char *chunkname, const char *mode) {
  (void) mode;
  ZIO z;
//  int status;
  lua_lock(L);
  if (!chunkname) chunkname = "?";
  luaZ_init(L, &z, reader, data);

  lua_pushnil(L);
  return 0;
}

LUALIB_API int (raviL_dumpast) (lua_State *L) {
  (void) L;
  return 0;
}




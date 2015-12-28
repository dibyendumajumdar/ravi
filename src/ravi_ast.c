#define LUA_CORE

#include "lprefix.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "lua.h"

#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "llex.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"

#include "ravi_ast.h"

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

LClosure *raviY_parser (lua_State *L, ZIO *z, Mbuffer *buff,
                       Dyndata *dyd, const char *name, int firstchar) {
  LexState lexstate;
  //Mbuffer mbuff;
  //FuncState funcstate;
  //LClosure *cl = luaF_newLclosure(L, 1);  /* create main closure */
  //setclLvalue(L, L->top, cl);  /* anchor it (to avoid being collected) */
  //luaD_inctop(L);
  lexstate.h = luaH_new(L);  /* create table for scanner */
  sethvalue(L, L->top, lexstate.h);  /* anchor it */
  luaD_inctop(L);
  TString *src = luaS_new(L, name);  /* create and anchor TString */
  setsvalue(L, L->top, src);
  luaD_inctop(L);  
  //funcstate.f = cl->p = luaF_newproto(L);
  //funcstate.f->source = luaS_new(L, name);  /* create and anchor TString */
  //lua_assert(iswhite(funcstate.f));  /* do not need barrier here */
  lexstate.buff = buff;
  lexstate.dyd = dyd;
  dyd->actvar.n = dyd->gt.n = dyd->label.n = 0;
  luaX_setinput(L, &lexstate, z, src, firstchar);
  //mainfunc(&lexstate, &funcstate);
  //lua_assert(!funcstate.prev && funcstate.nups == 1 && !lexstate.fs);
  /* all scopes should be correctly finished */
  lua_assert(dyd->actvar.n == 0 && dyd->gt.n == 0 && dyd->label.n == 0);
  L->top--;  /* remove source name */
  L->top--;  /* remove scanner's table */
  //return cl;  /* closure is on the stack, too */
  return luaY_parser(L, z, buff, dyd, name, firstchar);
}


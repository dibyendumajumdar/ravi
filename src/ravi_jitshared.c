/******************************************************************************
* Copyright (C) 2015-2017 Dibyendu Majumdar
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
#include <ravi_jitshared.h>
#include <ravi_membuf.h>

#include <stddef.h>
#include <assert.h>

#define GOTO_ON_ERROR 1

/*
 * Only 64-bits supported right now
 * Following must be kept in sync with changes in the actual header files
 */

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
"typedef enum {TM_INDEX,TM_NEWINDEX,TM_GC,\n"
"	TM_MODE,TM_LEN,TM_EQ,TM_ADD,TM_SUB,TM_MUL,\n"
"	TM_MOD,TM_POW,TM_DIV,TM_IDIV,TM_BAND,TM_BOR,\n"
"	TM_BXOR,TM_SHL,TM_SHR,TM_UNM,TM_BNOT,TM_LT,\n"
"	TM_LE,TM_CONCAT,TM_CALL,TM_N\n"
"} TMS;\n"
"typedef double lua_Number;\n"
"typedef int64_t lua_Integer;\n"
"typedef uint64_t lua_Unsigned;\n"
"typedef int (*lua_CFunction) (lua_State *L);\n"
"typedef union {\n"
"	lua_Number n;\n"
"	double u;\n"
"	void *s;\n"
"	lua_Integer i;\n"
"	long l;\n"
"} L_Umaxalign;\n"
"#define lua_assert(c)		((void)0)\n"
"#define check_exp(c,e)		(e)\n"
"#define lua_longassert(c)	((void)0)\n"
"#define luai_apicheck(l,e)	lua_assert(e)\n"
"#define api_check(l,e,msg)	luai_apicheck(l,(e) && msg)\n"
"#define UNUSED(x)	((void)(x))\n"
"#define cast(t, exp)	((t)(exp))\n"
"#define cast_void(i)	cast(void, (i))\n"
"#define cast_byte(i)	cast(lu_byte, (i))\n"
"#define cast_num(i)	cast(lua_Number, (i))\n"
"#define cast_int(i)	cast(int, (i))\n"
"#define cast_uchar(i)	cast(unsigned char, (i))\n"
"#define l_castS2U(i)	((lua_Unsigned)(i))\n"
"#define l_castU2S(i)	((lua_Integer)(i))\n"
"#define l_noret		void\n"
"typedef unsigned int Instruction;\n"
"#define luai_numidiv(L,a,b)     ((void)L, l_floor(luai_numdiv(L,a,b)))\n"
"#define luai_numdiv(L,a,b)      ((a)/(b))\n"
"#define luai_nummod(L,a,b,m)  \\\n"
"  { (m) = l_mathop(fmod)(a,b); if ((m)*(b) < 0) (m) += (b); }\n"
"#define LUA_TLCL	(LUA_TFUNCTION | (0 << 4))\n"
"#define LUA_TLCF	(LUA_TFUNCTION | (1 << 4))\n"
"#define LUA_TCCL	(LUA_TFUNCTION | (2 << 4))\n"
"#define RAVI_TFCF	(LUA_TFUNCTION | (4 << 4))\n"
"#define LUA_TSHRSTR	(LUA_TSTRING | (0 << 4))\n"
"#define LUA_TLNGSTR	(LUA_TSTRING | (1 << 4))\n"
"#define LUA_TNUMFLT	(LUA_TNUMBER | (0 << 4))\n"
"#define LUA_TNUMINT	(LUA_TNUMBER | (1 << 4))\n"
"#define RAVI_TIARRAY (LUA_TTABLE | (1 << 4))\n"
"#define RAVI_TFARRAY (LUA_TTABLE | (2 << 4))\n"
"#define BIT_ISCOLLECTABLE	(1 << 15)\n"
"#define ctb(t)			((t) | BIT_ISCOLLECTABLE)\n"
"typedef struct GCObject GCObject;\n"
"#define CommonHeader	GCObject *next; lu_byte tt; lu_byte marked\n"
"struct GCObject {\n"
"  CommonHeader;\n"
"};\n"
"typedef union Value {\n"
"  GCObject *gc;\n"
"  void *p;\n"
"  int b;\n"
"  lua_CFunction f;\n"
"  lua_Integer i;\n"
"  lua_Number n;\n"
"} Value;\n"
"#define TValuefields	Value value_; LuaType tt_\n"
"typedef struct lua_TValue {\n"
"  TValuefields;\n"
"} TValue;\n"
"#define NILCONSTANT	{NULL}, LUA_TNIL\n"
"#define val_(o)		((o)->value_)\n"
"#define rttype(o)	((o)->tt_)\n"
"#define novariant(x)	((x) & 0x0F)\n"
"#define ttype(o)	(rttype(o) & 0x7F)\n"
"#define ttnov(o)	(novariant(rttype(o)))\n"
"#define checktag(o,t)		(rttype(o) == (t))\n"
"#define checktype(o,t)		(ttnov(o) == (t))\n"
"#define ttisnumber(o)		checktype((o), LUA_TNUMBER)\n"
"#define ttisfloat(o)		checktag((o), LUA_TNUMFLT)\n"
"#define ttisinteger(o)		checktag((o), LUA_TNUMINT)\n"
"#define ttisnil(o)		checktag((o), LUA_TNIL)\n"
"#define ttisboolean(o)		checktag((o), LUA_TBOOLEAN)\n"
"#define ttislightuserdata(o)	checktag((o), LUA_TLIGHTUSERDATA)\n"
"#define ttisstring(o)		checktype((o), LUA_TSTRING)\n"
"#define ttisshrstring(o)	checktag((o), ctb(LUA_TSHRSTR))\n"
"#define ttislngstring(o)	checktag((o), ctb(LUA_TLNGSTR))\n"
"#define ttistable(o)		checktype((o), LUA_TTABLE)\n"
"#define ttisiarray(o)    checktag((o), ctb(RAVI_TIARRAY))\n"
"#define ttisfarray(o)    checktag((o), ctb(RAVI_TFARRAY))\n"
"#define ttisLtable(o)    checktag((o), ctb(LUA_TTABLE))\n"
"#define ttisfunction(o)		checktype(o, LUA_TFUNCTION)\n"
"#define ttisclosure(o)		((rttype(o) & 0x1F) == LUA_TFUNCTION)\n"
"#define ttisCclosure(o)		checktag((o), ctb(LUA_TCCL))\n"
"#define ttisLclosure(o)		checktag((o), ctb(LUA_TLCL))\n"
"#define ttislcf(o)		checktag((o), LUA_TLCF)\n"
"#define ttisfcf(o) (ttype(o) == RAVI_TFCF)\n"
"#define ttisfulluserdata(o)	checktag((o), ctb(LUA_TUSERDATA))\n"
"#define ttisthread(o)		checktag((o), ctb(LUA_TTHREAD))\n"
"#define ttisdeadkey(o)		checktag((o), LUA_TDEADKEY)\n"
"#define ivalue(o)	check_exp(ttisinteger(o), val_(o).i)\n"
"#define fltvalue(o)	check_exp(ttisfloat(o), val_(o).n)\n"
"#define nvalue(o)	check_exp(ttisnumber(o), \\\n"
"	(ttisinteger(o) ? cast_num(ivalue(o)) : fltvalue(o)))\n"
"#define gcvalue(o)	check_exp(iscollectable(o), val_(o).gc)\n"
"#define pvalue(o)	check_exp(ttislightuserdata(o), val_(o).p)\n"
"#define tsvalue(o)	check_exp(ttisstring(o), gco2ts(val_(o).gc))\n"
"#define uvalue(o)	check_exp(ttisfulluserdata(o), gco2u(val_(o).gc))\n"
"#define clvalue(o)	check_exp(ttisclosure(o), gco2cl(val_(o).gc))\n"
"#define clLvalue(o)	check_exp(ttisLclosure(o), gco2lcl(val_(o).gc))\n"
"#define clCvalue(o)	check_exp(ttisCclosure(o), gco2ccl(val_(o).gc))\n"
"#define fvalue(o)	check_exp(ttislcf(o), val_(o).f)\n"
"#define fcfvalue(o) check_exp(ttisfcf(o), val_(o).p)\n"
"#define hvalue(o)	check_exp(ttistable(o), gco2t(val_(o).gc))\n"
"#define bvalue(o)	check_exp(ttisboolean(o), val_(o).b)\n"
"#define thvalue(o)	check_exp(ttisthread(o), gco2th(val_(o).gc))\n"
"#define deadvalue(o)	check_exp(ttisdeadkey(o), cast(void *, val_(o).gc))\n"
"#define l_isfalse(o)	(ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))\n"
"#define iscollectable(o)	(rttype(o) & BIT_ISCOLLECTABLE)\n"
"#define righttt(obj)		(ttype(obj) == gcvalue(obj)->tt)\n"
"#define checkliveness(L,obj) \\\n"
"	lua_longassert(!iscollectable(obj) || \\\n"
"		(righttt(obj) && (L == NULL || !isdead(G(L),gcvalue(obj)))))\n"
"#define settt_(o,t)	((o)->tt_=(t))\n"
"#define setfltvalue(obj,x) \\\n"
"  { TValue *io=(obj); val_(io).n=(x); settt_(io, LUA_TNUMFLT); }\n"
"#define chgfltvalue(obj,x) \\\n"
"  { TValue *io=(obj); lua_assert(ttisfloat(io)); val_(io).n=(x); }\n"
"#define setivalue(obj,x) \\\n"
"  { TValue *io=(obj); val_(io).i=(x); settt_(io, LUA_TNUMINT); }\n"
"#define chgivalue(obj,x) \\\n"
"  { TValue *io=(obj); lua_assert(ttisinteger(io)); val_(io).i=(x); }\n"
"#define setnilvalue(obj) settt_(obj, LUA_TNIL)\n"
"#define setfvalue(obj,x) \\\n"
"  { TValue *io=(obj); val_(io).f=(x); settt_(io, LUA_TLCF); }\n"
"#define setfvalue_fastcall(obj, x, tag) \\\n"
"{ \\\n"
"    TValue *io = (obj);   \\\n"
"    lua_assert(tag >= 1 && tag < 0x80); \\\n"
"    val_(io).p = (x);     \\\n"
"    settt_(io, ((tag << 8) | RAVI_TFCF)); \\\n"
"}\n"
"#define setpvalue(obj,x) \\\n"
"  { TValue *io=(obj); val_(io).p=(x); settt_(io, LUA_TLIGHTUSERDATA); }\n"
"#define setbvalue(obj,x) \\\n"
"  { TValue *io=(obj); val_(io).b=(x); settt_(io, LUA_TBOOLEAN); }\n"
"#define setgcovalue(L,obj,x) \\\n"
"  { TValue *io = (obj); GCObject *i_g=(x); \\\n"
"    val_(io).gc = i_g; settt_(io, ctb(i_g->tt)); }\n"
"#define setsvalue(L,obj,x) \\\n"
"  { TValue *io = (obj); TString *x_ = (x); \\\n"
"    val_(io).gc = obj2gco(x_); settt_(io, ctb(x_->tt)); \\\n"
"    checkliveness(L,io); }\n"
"#define setuvalue(L,obj,x) \\\n"
"  { TValue *io = (obj); Udata *x_ = (x); \\\n"
"    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TUSERDATA)); \\\n"
"    checkliveness(L,io); }\n"
"#define setthvalue(L,obj,x) \\\n"
"  { TValue *io = (obj); lua_State *x_ = (x); \\\n"
"    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTHREAD)); \\\n"
"    checkliveness(L,io); }\n"
"#define setclLvalue(L,obj,x) \\\n"
"  { TValue *io = (obj); LClosure *x_ = (x); \\\n"
"    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TLCL)); \\\n"
"    checkliveness(L,io); }\n"
"#define setclCvalue(L,obj,x) \\\n"
"  { TValue *io = (obj); CClosure *x_ = (x); \\\n"
"    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TCCL)); \\\n"
"    checkliveness(L,io); }\n"
"#define sethvalue(L,obj,x) \\\n"
"  { TValue *io = (obj); Table *x_ = (x); \\\n"
"    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTABLE)); \\\n"
"    checkliveness(L,io); }\n"
"#define setiarrayvalue(L,obj,x) \\\n"
"  { TValue *io = (obj); Table *x_ = (x); \\\n"
"    val_(io).gc = obj2gco(x_); settt_(io, ctb(RAVI_TIARRAY)); \\\n"
"    checkliveness(L,io); }\n"
"#define setfarrayvalue(L,obj,x) \\\n"
"  { TValue *io = (obj); Table *x_ = (x); \\\n"
"    val_(io).gc = obj2gco(x_); settt_(io, ctb(RAVI_TFARRAY)); \\\n"
"    checkliveness(L,io); }\n"
"#define setdeadvalue(obj)	settt_(obj, LUA_TDEADKEY)\n"
"#define setobj(L,obj1,obj2) \\\n"
// NOTE we cannot use aggregate assign so following assigns by field but assumes
// n covers all value types
"	{ TValue *io1=(obj1); const TValue *io2=(obj2); io1->tt_ = io2->tt_; val_(io1).n = val_(io2).n; \\\n"
"	  (void)L; checkliveness(L,io1); }\n"
"#define setobjs2s	setobj\n"
"#define setobj2s	setobj\n"
"#define setsvalue2s	setsvalue\n"
"#define sethvalue2s	sethvalue\n"
"#define setptvalue2s	setptvalue\n"
"#define setobjt2t	setobj\n"
"#define setobj2n	setobj\n"
"#define setsvalue2n	setsvalue\n"
"#define setobj2t	setobj\n"
"typedef TValue *StkId;\n"
"typedef struct TString {\n"
"	CommonHeader;\n"
"	lu_byte extra;\n"
"	lu_byte shrlen;\n"
"	unsigned int hash;\n"
"	union {\n"
"		size_t lnglen;\n"
"		struct TString *hnext;\n"
"	} u;\n"
"} TString;\n"
"typedef union UTString {\n"
"	L_Umaxalign dummy;\n"
"	TString tsv;\n"
"} UTString;\n"
"#define getstr(ts)  \\\n"
"  check_exp(sizeof((ts)->extra), cast(char *, (ts)) + sizeof(UTString))\n"
"#define svalue(o)       getstr(tsvalue(o))\n"
"#define tsslen(s)	((s)->tt == LUA_TSHRSTR ? (s)->shrlen : (s)->u.lnglen)\n"
"#define vslen(o)	tsslen(tsvalue(o))\n"
"typedef struct Udata {\n"
"	CommonHeader;\n"
"	LuaType ttuv_;\n"
"	struct Table *metatable;\n"
"	size_t len;\n"
"	union Value user_;\n"
"} Udata;\n"
"typedef union UUdata {\n"
"	L_Umaxalign dummy;\n"
"	Udata uv;\n"
"} UUdata;\n"
"#define getudatamem(u)  \\\n"
"  check_exp(sizeof((u)->ttuv_), (cast(char*, (u)) + sizeof(UUdata)))\n"
"#define setuservalue(L,u,o) \\\n"
"	{ const TValue *io=(o); Udata *iu = (u); \\\n"
"	  iu->user_ = io->value_; iu->ttuv_ = rttype(io); \\\n"
"	  checkliveness(L,io); }\n"
"#define getuservalue(L,u,o) \\\n"
"	{ TValue *io=(o); const Udata *iu = (u); \\\n"
"	  io->value_ = iu->user_; settt_(io, iu->ttuv_); \\\n"
"	  checkliveness(L,io); }\n"
"typedef enum {\n"
"	RAVI_TANY = 0,\n"
"	RAVI_TNUMINT = 1,\n"
"	RAVI_TNUMFLT,\n"
"	RAVI_TARRAYINT,\n"
"	RAVI_TARRAYFLT,\n"
"	RAVI_TFUNCTION,\n"
"	RAVI_TTABLE,\n"
"	RAVI_TSTRING,\n"
"	RAVI_TNIL,\n"
"	RAVI_TBOOLEAN,\n"
"	RAVI_TUSERDATA\n"
"} ravitype_t;\n"
"typedef struct Upvaldesc {\n"
"	TString *name;\n"
"	TString *usertype;\n"
"	lu_byte ravi_type;\n"
"	lu_byte instack;\n"
"	lu_byte idx;\n"
"} Upvaldesc;\n"
"typedef struct LocVar {\n"
"	TString *varname;\n"
"	TString *usertype;\n"
"	int startpc;\n"
"	int endpc;\n"
"	lu_byte ravi_type;\n"
"} LocVar;\n"
"typedef enum {\n"
"	RAVI_JIT_NOT_COMPILED = 0,\n"
"	RAVI_JIT_CANT_COMPILE = 1,\n"
"	RAVI_JIT_COMPILED = 2\n"
"} ravi_jit_status_t;\n"
"typedef enum {\n"
"	RAVI_JIT_FLAG_NONE = 0,\n"
"	RAVI_JIT_FLAG_HASFORLOOP = 1\n"
"} ravi_jit_flag_t;\n"
"typedef struct RaviJITProto {\n"
"	lu_byte jit_status;\n"
"	lu_byte jit_flags;\n"
"	unsigned short execution_count;\n"
"	void *jit_data;\n"
"	lua_CFunction jit_function;\n"
"} RaviJITProto;\n"
"typedef struct Proto {\n"
"	CommonHeader;\n"
"	lu_byte numparams;\n"
"	lu_byte is_vararg;\n"
"	lu_byte maxstacksize;\n"
"	int sizeupvalues;\n"
"	int sizek;\n"
"	int sizecode;\n"
"	int sizelineinfo;\n"
"	int sizep;\n"
"	int sizelocvars;\n"
"	int linedefined;\n"
"	int lastlinedefined;\n"
"	TValue *k;\n"
"	Instruction *code;\n"
"	struct Proto **p;\n"
"	int *lineinfo;\n"
"	LocVar *locvars;\n"
"	Upvaldesc *upvalues;\n"
"	struct LClosure *cache;\n"
"	TString  *source;\n"
"	GCObject *gclist;\n"
"	RaviJITProto ravi_jit;\n"
"} Proto;\n"
"typedef struct UpVal UpVal;\n"
"#define ClosureHeader \\\n"
"	CommonHeader; lu_byte nupvalues; GCObject *gclist\n"
"typedef struct CClosure {\n"
"	ClosureHeader;\n"
"	lua_CFunction f;\n"
"	TValue upvalue[1];\n"
"} CClosure;\n"
"typedef struct LClosure {\n"
"	ClosureHeader;\n"
"	struct Proto *p;\n"
"	UpVal *upvals[1];\n"
"} LClosure;\n"
"typedef union Closure {\n"
"	CClosure c;\n"
"	LClosure l;\n"
"} Closure;\n"
"#define isLfunction(o)	ttisLclosure(o)\n"
"#define getproto(o)	(clLvalue(o)->p)\n"
"typedef union TKey {\n"
"	struct {\n"
"		TValuefields;\n"
"		int next;\n"
"	} nk;\n"
"	TValue tvk;\n"
"} TKey;\n"
"#define setnodekey(L,key,obj) \\\n"
"	{ TKey *k_=(key); const TValue *io_=(obj); \\\n"
"	  k_->nk.value_ = io_->value_; k_->nk.tt_ = io_->tt_; \\\n"
"	  (void)L; checkliveness(L,io_); }\n"
"typedef struct Node {\n"
"	TValue i_val;\n"
"	TKey i_key;\n"
"} Node;\n"
"typedef enum RaviArrayModifer {\n"
"	RAVI_ARRAY_SLICE = 1,\n"
"	RAVI_ARRAY_FIXEDSIZE = 2\n"
"} RaviArrayModifier;\n"
"typedef struct RaviArray {\n"
"	char *data;\n"
"	unsigned int len;\n"
"	unsigned int size;\n"
"	lu_byte array_type;\n"
"	lu_byte array_modifier;\n"
"} RaviArray;\n"
"typedef struct Table {\n"
"	CommonHeader;\n"
"	lu_byte flags;\n"
"	lu_byte lsizenode;\n"
"	unsigned int sizearray;\n"
"	TValue *array;\n"
"	Node *node;\n"
"	Node *lastfree;\n"
"	struct Table *metatable;\n"
"	GCObject *gclist;\n"
"	RaviArray ravi_array;\n"
#if RAVI_USE_NEWHASH
"	unsigned int hmask;\n"
#endif
"} Table;\n"
"typedef struct Mbuffer {\n"
"	char *buffer;\n"
"	size_t n;\n"
"	size_t buffsize;\n"
"} Mbuffer;\n"
"typedef struct stringtable {\n"
"	TString **hash;\n"
"	int nuse;\n"
"	int size;\n"
"} stringtable;\n"
"struct lua_Debug;\n"
"typedef intptr_t lua_KContext;\n"
"typedef int(*lua_KFunction)(struct lua_State *L, int status, lua_KContext ctx);\n"
"typedef void *(*lua_Alloc)(void *ud, void *ptr, size_t osize,\n"
"	size_t nsize);\n"
"typedef void(*lua_Hook)(struct lua_State *L, struct lua_Debug *ar);\n"
"typedef struct CallInfo {\n"
"	StkId func;\n"
"	StkId	top;\n"
"	struct CallInfo *previous, *next;\n"
"	union {\n"
"		struct {\n"
"			StkId base;\n"
"			const Instruction *savedpc;\n"
"		} l;\n"
"		struct {\n"
"			lua_KFunction k;\n"
"			ptrdiff_t old_errfunc;\n"
"			lua_KContext ctx;\n"
"		} c;\n"
"	} u;\n"
"	ptrdiff_t extra;\n"
"	short nresults;\n"
"	unsigned short callstatus;\n"
"	unsigned short stacklevel;\n"
"	lu_byte jitstatus;\n"
"   lu_byte magic;\n"
"} CallInfo;\n"
"#define CIST_OAH	(1<<0)\n"
"#define CIST_LUA	(1<<1)\n"
"#define CIST_HOOKED	(1<<2)\n"
"#define CIST_FRESH	(1<<3)\n"
"#define CIST_YPCALL	(1<<4)\n"
"#define CIST_TAIL	(1<<5)\n"
"#define CIST_HOOKYIELD	(1<<6)\n"
"#define CIST_LEQ	(1<<7)\n"
"#define CIST_FIN	(1<<8)\n"
"#define isLua(ci)	((ci)->callstatus & CIST_LUA)\n"
"#define isJITed(ci) ((ci)->jitstatus)\n"
"#define setoah(st,v)	((st) = ((st) & ~CIST_OAH) | (v))\n"
"#define getoah(st)	((st) & CIST_OAH)\n"
"typedef struct global_State global_State;\n"
"struct lua_State {\n"
"	CommonHeader;\n"
"	lu_byte status;\n"
"	StkId top;\n"
"	global_State *l_G;\n"
"	CallInfo *ci;\n"
"	const Instruction *oldpc;\n"
"	StkId stack_last;\n"
"	StkId stack;\n"
"	UpVal *openupval;\n"
"	GCObject *gclist;\n"
"	struct lua_State *twups;\n"
"	struct lua_longjmp *errorJmp;\n"
"	CallInfo base_ci;\n"
"	volatile lua_Hook hook;\n"
"	ptrdiff_t errfunc;\n"
"	int stacksize;\n"
"	int basehookcount;\n"
"	int hookcount;\n"
"	unsigned short nny;\n"
"	unsigned short nCcalls;\n"
"	lu_byte hookmask;\n"
"	lu_byte allowhook;\n"
"	unsigned short nci;\n"
"   lu_byte magic;\n"
"};\n"
"#define G(L)	(L->l_G)\n"
"union GCUnion {\n"
"	GCObject gc;\n"
"	struct TString ts;\n"
"	struct Udata u;\n"
"	union Closure cl;\n"
"	struct Table h;\n"
"	struct Proto p;\n"
"	struct lua_State th;\n"
"};\n"
"struct UpVal {\n"
"	TValue *v;\n"
"	lu_mem refcount;\n"
"	union {\n"
"		struct {\n"
"			UpVal *next;\n"
"			int touched;\n"
"		} open;\n"
"		TValue value;\n"
"	} u;\n"
"};\n"
"#define cast_u(o)	cast(union GCUnion *, (o))\n"
"#define gco2ts(o)  \\\n"
"	check_exp(novariant((o)->tt) == LUA_TSTRING, &((cast_u(o))->ts))\n"
"#define gco2u(o)  check_exp((o)->tt == LUA_TUSERDATA, &((cast_u(o))->u))\n"
"#define gco2lcl(o)  check_exp((o)->tt == LUA_TLCL, &((cast_u(o))->cl.l))\n"
"#define gco2ccl(o)  check_exp((o)->tt == LUA_TCCL, &((cast_u(o))->cl.c))\n"
"#define gco2cl(o)  \\\n"
"	check_exp(novariant((o)->tt) == LUA_TFUNCTION, &((cast_u(o))->cl))\n"
"#define gco2t(o)  check_exp(novariant((o)->tt) == LUA_TTABLE, &((cast_u(o))->h))\n"
"#define gco2p(o)  check_exp((o)->tt == LUA_TPROTO, &((cast_u(o))->p))\n"
"#define gco2th(o)  check_exp((o)->tt == LUA_TTHREAD, &((cast_u(o))->th))\n"
"#define obj2gco(v) \\\n"
"	check_exp(novariant((v)->tt) < LUA_TDEADKEY, (&(cast_u(v)->gc)))\n"
"#define LUA_FLOORN2I		0\n"
"#define tonumber(o,n) \\\n"
"  (ttisfloat(o) ? (*(n) = fltvalue(o), 1) : luaV_tonumber_(o,n))\n"
"#define tointeger(o,i) \\\n"
"  (ttisinteger(o) ? (*(i) = ivalue(o), 1) : luaV_tointeger(o,i,LUA_FLOORN2I))\n"
"extern int luaV_tonumber_(const TValue *obj, lua_Number *n);\n"
"extern int luaV_tointeger(const TValue *obj, lua_Integer *p, int mode);\n"
"extern void luaF_close (lua_State *L, StkId level);\n"
"extern int luaD_poscall (lua_State *L, CallInfo *ci, StkId firstResult, int nres);\n"
"extern int luaV_equalobj(lua_State *L, const TValue *t1, const TValue *t2);\n"
"extern int luaV_lessthan(lua_State *L, const TValue *l, const TValue *r);\n"
"extern int luaV_lessequal(lua_State *L, const TValue *l, const TValue *r);\n"
"extern void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val);\n"
"extern void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val);\n"
"extern int luaV_execute(lua_State *L);\n"
"extern int luaD_precall (lua_State *L, StkId func, int nresults, int op_call);\n"
"extern void raviV_op_newtable(lua_State *L, CallInfo *ci, TValue *ra, int b, int c);\n"
"extern void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra);\n"
"extern void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra);\n"
"extern void luaO_arith (lua_State *L, int op, const TValue *p1, const TValue *p2, TValue *res);\n"
"extern void raviV_op_setlist(lua_State *L, CallInfo *ci, TValue *ra, int b, int c);\n"
"extern void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c);\n"
"extern void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int Bx);\n"
"extern void raviV_op_vararg(lua_State *L, CallInfo *ci, LClosure *cl, int a, int b);\n"
"extern void luaV_objlen (lua_State *L, StkId ra, const TValue *rb);\n"
"extern int luaV_forlimit(const TValue *obj, lua_Integer *p, lua_Integer step, int *stopnow);\n"
"extern void raviV_op_setupval(lua_State *L, LClosure *cl, TValue *ra, int b);\n"
"extern void raviV_op_setupvali(lua_State *L, LClosure *cl, TValue *ra, int b);\n"
"extern void raviV_op_setupvalf(lua_State *L, LClosure *cl, TValue *ra, int b);\n"
"extern void raviV_op_setupvalai(lua_State *L, LClosure *cl, TValue *ra, int b);\n"
"extern void raviV_op_setupvalaf(lua_State *L, LClosure *cl, TValue *ra, int b);\n"
"extern void raviV_op_setupvalt(lua_State *L, LClosure *cl, TValue *ra, int b);\n"
"extern void raise_error(lua_State *L, int errorcode);\n"
"extern void luaD_call (lua_State *L, StkId func, int nResults);\n"
"extern void raviH_set_int(lua_State *L, Table *t, lua_Unsigned key, lua_Integer value);\n"
"extern void raviH_set_float(lua_State *L, Table *t, lua_Unsigned key, lua_Number value);\n"
"extern int raviV_check_usertype(lua_State *L, TString *name, const TValue *o);\n"
"extern void luaT_trybinTM (lua_State *L, const TValue *p1, const TValue *p2, TValue *res, TMS event);\n"
"extern void raviV_gettable_sskey(lua_State *L, const TValue *t, TValue *key, TValue *val);\n"
"extern void raviV_settable_sskey(lua_State *L, const TValue *t, TValue *key, TValue *val);\n"
"extern void raviV_gettable_i(lua_State *L, const TValue *t, TValue *key, TValue *val);\n"
"extern void raviV_settable_i(lua_State *L, const TValue *t, TValue *key, TValue *val);\n"
"#define R(i) (base + i)\n"
"#define K(i) (k + i)\n"
"#define tonumberns(o,n) \\\n"
"	(ttisfloat(o) ? ((n) = fltvalue(o), 1) : \\\n"
"	(ttisinteger(o) ? ((n) = cast_num(ivalue(o)), 1) : 0))\n"
;


// We can only compile a subset of op codes
// and not all features are supported
bool raviJ_cancompile(Proto *p) {
	const Instruction *code = p->code;
	int pc, n = p->sizecode;
	// Loop over the byte codes; as Lua compiler inserts
	// an extra RETURN op we need to ignore the last op
	for (pc = 0; pc < n; pc++) {
		Instruction i = code[pc];
		OpCode o = GET_OPCODE(i);
		switch (o) {
		case OP_RETURN:
		case OP_LOADK:
		case OP_RAVI_FORLOOP_IP:
		case OP_RAVI_FORLOOP_I1:
		case OP_FORLOOP:
		case OP_RAVI_FORPREP_IP:
		case OP_RAVI_FORPREP_I1:
		case OP_TFORCALL:
		case OP_TFORLOOP:
		case OP_FORPREP:
		case OP_MOVE:
		case OP_LOADNIL:
		case OP_JMP:
		case OP_LOADBOOL:
		case OP_EQ:
		case OP_RAVI_EQ_II:
		case OP_RAVI_EQ_FF:
		case OP_LT:
		case OP_RAVI_LT_II:
		case OP_RAVI_LT_FF:
		case OP_LE:
		case OP_RAVI_LE_II:
		case OP_RAVI_LE_FF:
		case OP_TEST:
		case OP_TESTSET:
		case OP_CALL:
		case OP_TAILCALL:
		case OP_GETTABUP:
		case OP_RAVI_GETTABUP_SK:
		case OP_GETUPVAL:
		case OP_SETUPVAL:
		case OP_RAVI_SETUPVALI:
		case OP_RAVI_SETUPVALF:
		case OP_RAVI_SETUPVAL_IARRAY:
		case OP_RAVI_SETUPVAL_FARRAY:
		case OP_RAVI_SETUPVALT:
		case OP_RAVI_TABLE_GETFIELD:
		case OP_RAVI_IARRAY_GET:
		case OP_RAVI_FARRAY_GET:
		case OP_RAVI_GETFIELD:
		case OP_RAVI_GETI:
		case OP_GETTABLE:
		case OP_SELF:
		case OP_RAVI_TABLE_SELF_SK:
		case OP_RAVI_SELF_SK:
		case OP_RAVI_SETFIELD:
		case OP_RAVI_TABLE_SETFIELD:
		case OP_RAVI_SETI:
		case OP_RAVI_IARRAY_SETI:
		case OP_RAVI_IARRAY_SET:
		case OP_RAVI_FARRAY_SETF:
		case OP_RAVI_FARRAY_SET:
		case OP_SETTABLE:
		case OP_SETTABUP:
		case OP_NEWTABLE:
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_MOD:
		case OP_POW:
		case OP_DIV:
		case OP_IDIV:
		case OP_BAND:
		case OP_BOR:
		case OP_BXOR:
		case OP_SHL:
		case OP_SHR:
		case OP_RAVI_ADDFF:
		case OP_RAVI_ADDFI:
		case OP_RAVI_ADDII:
		case OP_RAVI_SUBFF:
		case OP_RAVI_SUBFI:
		case OP_RAVI_SUBIF:
		case OP_RAVI_SUBII:
		case OP_RAVI_MULFF:
		case OP_RAVI_MULFI:
		case OP_RAVI_MULII:
		case OP_RAVI_DIVFF:
		case OP_RAVI_DIVFI:
		case OP_RAVI_DIVIF:
		case OP_RAVI_DIVII:
		case OP_RAVI_LOADIZ:
		case OP_RAVI_LOADFZ:
		case OP_RAVI_MOVEI:
		case OP_RAVI_MOVEF:
		case OP_RAVI_MOVEIARRAY:
		case OP_RAVI_MOVEFARRAY:
		case OP_RAVI_MOVETAB:
		case OP_RAVI_TOINT:
		case OP_RAVI_TOFLT:
		case OP_RAVI_TOIARRAY:
		case OP_RAVI_TOFARRAY:
		case OP_RAVI_TOTAB:
		case OP_RAVI_NEW_IARRAY:
		case OP_RAVI_NEW_FARRAY:
		case OP_VARARG:
		case OP_CONCAT:
		case OP_CLOSURE:
		case OP_LEN:
		case OP_NOT:
		case OP_SETLIST:
		case OP_RAVI_TOCLOSURE:
		case OP_RAVI_TOSTRING:
		case OP_RAVI_TOTYPE:
		case OP_LOADKX: break;
#if 0
		case OP_UNM:
		case OP_BNOT:
#endif
		default: {
			return false;
		}
		}
	}
	return true;
}

struct function {
	struct lua_State *L;
	struct Proto *p;
	char fname[30];
	int var;
	// flags to mark instructions
	unsigned char *jmps;
	// Flags related to locals
	unsigned char *locals;
	membuff_t prologue;
	membuff_t body;
	struct ravi_compile_options_t *options;
};

// Identify Ravi bytecode instructions that are jump
// targets - we need this so that when generating code
// we can emit labels for gotos
static void scan_jump_targets(struct function *fn) {
	const Instruction *code = fn->p->code;
	int pc, n = fn->p->sizecode;
	assert(fn->jmps != NULL);
	for (pc = 0; pc < n; pc++) {
		Instruction i = code[pc];
		OpCode op = GET_OPCODE(i);
		switch (op) {
		case OP_LOADBOOL: {
			int C = GETARG_C(i);
			if (C) {
				int j = pc + 2;  // jump target
				assert(j < n);
				fn->jmps[j] = true;
			}
		} break;
		case OP_JMP:
		case OP_RAVI_FORPREP_IP:
		case OP_RAVI_FORPREP_I1:
		case OP_RAVI_FORLOOP_IP:
		case OP_RAVI_FORLOOP_I1:
		case OP_FORLOOP:
		case OP_FORPREP:
		case OP_TFORLOOP: {
			int sbx = GETARG_sBx(i);
			int j = sbx + pc + 1;
			assert(j < n);
			fn->jmps[j] = true;
		} break;
		default: break;
		}
	}
}

static void emit_reg(struct function *fn, const char *name, int regnum) {
	assert(!ISK(regnum));
	membuff_add_fstring(&fn->body, "%s = R(%d);\n", name, regnum);
}

static void emit_reg_or_k(struct function *fn, const char *name, int regnum) {
	if (ISK(regnum)) {
		membuff_add_fstring(&fn->body, "%s = K(%d);\n", name, INDEXK(regnum));
	}
	else {
		membuff_add_fstring(&fn->body, "%s = R(%d);\n", name, regnum);
	}
}

// Updates the savedpc pointer in the call frame
// The savedpc is unimportant for the JIT but it is relied upon
// by the debug interface. So we need to set this in order for the
// debug api to work. Rather than setting it on every bytecode instruction
// we only set this prior to
// function calls - this enables better stack traces for example, and ad-hoc
// calls to debug api.
static void emit_update_savedpc(struct function *fn, int pc) {
  // Need to set savedpc to the next instruction (pc+1) rather than current as
  // that is
  // what the VM does in interpreted mode
  membuff_add_fstring(&fn->body, "ci->u.l.savedpc = &cl->p->code[%d+1];\n", pc);
}

static void emit_IARRAY_GET(struct function *fn, int A, int B, int C, bool omitArrayGetRangeCheck, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_string(&fn->body, "ukey = (lua_Unsigned)(ivalue(rc));\n");
  membuff_add_string(&fn->body, "t = hvalue(rb);");
  membuff_add_string(&fn->body, "iptr = (lua_Integer *)t->ravi_array.data;\n");
  if (!omitArrayGetRangeCheck) { membuff_add_string(&fn->body, "if (ukey < (lua_Unsigned)(t->ravi_array.len)) {\n"); }
  membuff_add_string(&fn->body, " setivalue(ra, iptr[ukey]);\n");
  if (!omitArrayGetRangeCheck) {
    membuff_add_string(&fn->body, "} else {\n");
#if GOTO_ON_ERROR
    membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_array_out_of_bounds);
    membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
    membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_array_out_of_bounds);
#endif
    membuff_add_string(&fn->body, "}\n");
  }
}

static void emit_FARRAY_GET(struct function *fn, int A, int B, int C, bool omitArrayGetRangeCheck, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_string(&fn->body, "ukey = (lua_Unsigned)(ivalue(rc));\n");
  membuff_add_string(&fn->body, "t = hvalue(rb);");
  membuff_add_string(&fn->body, "nptr = (lua_Number *)t->ravi_array.data;\n");
  if (!omitArrayGetRangeCheck) { membuff_add_string(&fn->body, "if (ukey < (lua_Unsigned)(t->ravi_array.len)) {\n"); }
  membuff_add_string(&fn->body, " setfltvalue(ra, nptr[ukey]);\n");
  if (!omitArrayGetRangeCheck) {
    membuff_add_string(&fn->body, "} else {\n");
#if GOTO_ON_ERROR
    membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_array_out_of_bounds);
    membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
    membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_array_out_of_bounds);
#endif
    membuff_add_string(&fn->body, "}\n");
  }
}

static void emit_IARRAY_SETI(struct function *fn, int A, int B, int C, bool known_int, int pc) {
  (void)pc;
  (void)known_int;
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_string(&fn->body, "t = hvalue(ra);\n");
  membuff_add_string(&fn->body, "ukey = (lua_Unsigned)(ivalue(rb));\n");
  membuff_add_string(&fn->body, "iptr = (lua_Integer *)t->ravi_array.data;\n");
  membuff_add_string(&fn->body, "if (ukey < (lua_Unsigned)(t->ravi_array.len)) {\n");
  membuff_add_string(&fn->body, " iptr[ukey] = ivalue(rc);\n");
  membuff_add_string(&fn->body, "} else {\n");
  membuff_add_fstring(&fn->body, " raviH_set_int(L, t, ukey, ivalue(rc));\n");
  membuff_add_string(&fn->body, "}\n");
}

static void emit_IARRAY_SET(struct function *fn, int A, int B, int C, bool known_int, int pc) {
  (void)pc;
  (void)known_int;
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_string(&fn->body, "t = hvalue(ra);\n");
  membuff_add_string(&fn->body, "ukey = (lua_Unsigned)(ivalue(rb));\n");
  membuff_add_string(&fn->body, "iptr = (lua_Integer *)t->ravi_array.data;\n");
  membuff_add_string(&fn->body, "if (!ttisinteger(rc)) {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_integer_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_integer_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
  membuff_add_string(&fn->body, "i = ivalue(rc);\n");
  membuff_add_string(&fn->body, "if (ukey < (lua_Unsigned)(t->ravi_array.len)) {\n");
  membuff_add_string(&fn->body, " iptr[ukey] = i;\n");
  membuff_add_string(&fn->body, "} else {\n");
  membuff_add_fstring(&fn->body, " raviH_set_int(L, t, ukey, i);\n");
  membuff_add_string(&fn->body, "}\n");
}

static void emit_FARRAY_SETF(struct function *fn, int A, int B, int C, bool known_int, int pc) {
  (void)pc;
  (void)known_int;
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_string(&fn->body, "t = hvalue(ra);\n");
  membuff_add_string(&fn->body, "ukey = (lua_Unsigned)(ivalue(rb));\n");
  membuff_add_string(&fn->body, "nptr = (lua_Number *)t->ravi_array.data;\n");
  membuff_add_string(&fn->body, "if (ukey < (lua_Unsigned)(t->ravi_array.len)) {\n");
  membuff_add_string(&fn->body, " nptr[ukey] = fltvalue(rc);\n");
  membuff_add_string(&fn->body, "} else {\n");
  membuff_add_fstring(&fn->body, " raviH_set_float(L, t, ukey, fltvalue(rc));\n");
  membuff_add_string(&fn->body, "}\n");
}

static void emit_FARRAY_SET(struct function *fn, int A, int B, int C, bool known_int, int pc) {
  (void)pc;
  (void)known_int;
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_string(&fn->body, "t = hvalue(ra);\n");
  membuff_add_string(&fn->body, "ukey = (lua_Unsigned)(ivalue(rb));\n");
  membuff_add_string(&fn->body, "nptr = (lua_Number *)t->ravi_array.data;\n");
  membuff_add_string(&fn->body, "if (!ttisnumber(rc)) {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_integer_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_integer_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
  membuff_add_string(&fn->body, "n = (ttisinteger(rc) ? (double)ivalue(rc) : fltvalue(rc));\n");
  membuff_add_string(&fn->body, "if (ukey < (lua_Unsigned)(t->ravi_array.len)) {\n");
  membuff_add_string(&fn->body, " nptr[ukey] = n;\n");
  membuff_add_string(&fn->body, "} else {\n");
  membuff_add_fstring(&fn->body, " raviH_set_float(L, t, ukey, n);\n");
  membuff_add_string(&fn->body, "}\n");
}

/* Handle OP_ADD, OP_SUB, OP_MUL */
static void emit_op_arithslow(struct function *fn, int A, int B, int C, OpCode op, int pc, const char *opchar,
                              const char *tm) {
  (void)op;
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_string(&fn->body, "if (ttisinteger(rb) && ttisinteger(rc)) {\n");
  membuff_add_string(&fn->body, " i = ivalue(rb);\n");
  membuff_add_string(&fn->body, " ic = ivalue(rc);\n");
  membuff_add_fstring(&fn->body, " setivalue(ra, (i %s ic));\n", opchar);
  membuff_add_string(&fn->body, "} else if (tonumberns(rb, n) && tonumberns(rc, nc)) {\n");
  membuff_add_fstring(&fn->body, " setfltvalue(ra, (n %s nc));\n", opchar);
  membuff_add_string(&fn->body, "} else {\n");
  emit_update_savedpc(fn, pc);
  membuff_add_fstring(&fn->body, " luaT_trybinTM(L, rb, rc, ra, %s);\n", tm);
  membuff_add_string(&fn->body, " base = ci->u.l.base;\n");
  membuff_add_string(&fn->body, "}\n");
}

static void emit_comparison(struct function *fn, int A, int B, int C, int j, int jA, const char *compfunc,
                            OpCode opCode, int pc) {
  const char *oper = "==";
  switch (opCode) {
    case OP_RAVI_LT_II: oper = "<"; goto Lemitint;
    case OP_RAVI_LE_II: oper = "<=";
    /* no break */
    case OP_RAVI_EQ_II:
    Lemitint:
      if (ISK(B) && ISK(C)) {
        TValue *Konst1 = &fn->p->k[INDEXK(B)];
        TValue *Konst2 = &fn->p->k[INDEXK(C)];
        membuff_add_fstring(&fn->body, "result = (%lld %s %lld);\n", Konst1->value_.i, oper, Konst2->value_.i);
      }
      else if (ISK(B)) {
        TValue *Konst1 = &fn->p->k[INDEXK(B)];
        emit_reg_or_k(fn, "rc", C);
        membuff_add_fstring(&fn->body, "result = (%lld %s ivalue(rc));\n", Konst1->value_.i, oper);
      }
      else if (ISK(C)) {
        TValue *Konst2 = &fn->p->k[INDEXK(C)];
        emit_reg_or_k(fn, "rb", B);
        membuff_add_fstring(&fn->body, "result = (ivalue(rb) %s %lld);\n", oper, Konst2->value_.i);
      }
      else {
        emit_reg_or_k(fn, "rb", B);
        emit_reg_or_k(fn, "rc", C);
        membuff_add_fstring(&fn->body, "result = (ivalue(rb) %s ivalue(rc));\n", oper);
      }
      break;
    case OP_RAVI_LT_FF: oper = "<"; goto Lemitflt;
    case OP_RAVI_LE_FF: oper = "<=";
    /* no break */
    case OP_RAVI_EQ_FF:
    Lemitflt:
      if (ISK(B) && ISK(C)) {
        TValue *Konst1 = &fn->p->k[INDEXK(B)];
        TValue *Konst2 = &fn->p->k[INDEXK(C)];
        membuff_add_fstring(&fn->body, "result = (%.17g %s %.17g);\n", Konst1->value_.n, oper, Konst2->value_.n);
      }
      else if (ISK(B)) {
        TValue *Konst1 = &fn->p->k[INDEXK(B)];
        emit_reg_or_k(fn, "rc", C);
        membuff_add_fstring(&fn->body, "result = (%.17g %s fltvalue(rc));\n", Konst1->value_.n, oper);
      }
      else if (ISK(C)) {
        TValue *Konst2 = &fn->p->k[INDEXK(C)];
        emit_reg_or_k(fn, "rb", B);
        membuff_add_fstring(&fn->body, "result = (fltvalue(rb) %s %.17g);\n", oper, Konst2->value_.n);
      }
      else {
        emit_reg_or_k(fn, "rb", B);
        emit_reg_or_k(fn, "rc", C);
        membuff_add_fstring(&fn->body, "result = (fltvalue(rb) %s fltvalue(rc));\n", oper);
      }
      break;
    default:
      emit_reg_or_k(fn, "rb", B);
      emit_reg_or_k(fn, "rc", C);
      emit_update_savedpc(fn, pc);
      membuff_add_fstring(&fn->body, "result = %s(L, rb, rc);\n", compfunc);
      // Reload pointer to base as the call to luaV_equalobj() may
      // have invoked a Lua function and as a result the stack may have
      // been reallocated - so the previous base pointer could be stale
      membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
      break;
  }
  membuff_add_fstring(&fn->body, "if (result == %d) {\n", A);
  if (jA > 0) {
    membuff_add_fstring(&fn->body, " ra = R(%d);\n", jA - 1);
    membuff_add_string(&fn->body, " luaF_close(L, ra);\n");
  }
  membuff_add_fstring(&fn->body, "  goto Lbc_%d;\n", j);
  membuff_add_string(&fn->body, "}\n");
}

static void emit_jump_label(struct function *fn, int pc) {
  if (fn->jmps[pc]) {
    char labelstmt[80];
    snprintf(labelstmt, sizeof labelstmt, "Lbc_%d:\n", pc);
    membuff_add_string(&fn->body, labelstmt);
  }
}

static void emit_op_loadk(struct function *fn, int A, int Bx, int pc) {
  emit_reg(fn, "ra", A);
  (void)pc;
  TValue *Konst = &fn->p->k[Bx];
  switch (Konst->tt_) {
    case LUA_TNUMINT: membuff_add_fstring(&fn->body, "setivalue(ra, %lld);\n", Konst->value_.i); break;
    case LUA_TNUMFLT: membuff_add_fstring(&fn->body, "setfltvalue(ra, %.17g);\n", Konst->value_.n); break;
    case LUA_TBOOLEAN: membuff_add_fstring(&fn->body, "setbvalue(ra, %d);\n", Konst->value_.b); break;
    default: {
      membuff_add_fstring(&fn->body, "rb = K(%d);\n", Bx);
      membuff_add_fstring(&fn->body, "setobj2s(L, ra, rb);\n");
      break;
    }
  }
}

static void emit_op_return(struct function *fn, int A, int B, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "if (cl->p->sizep > 0) luaF_close(L, base);\n");
  membuff_add_fstring(&fn->body, "result = (%d != 0 ? %d - 1 : cast_int(L->top - ra));\n", B, B);
  membuff_add_string(&fn->body, "return luaD_poscall(L, ci, ra, result);\n");
}

static void emit_op_move(struct function *fn, int A, int B, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_fstring(&fn->body, "rb = R(%d);\n", B);
  membuff_add_fstring(&fn->body, "setobj2s(L, ra, rb);\n");
}

static void emit_op_loadnil(struct function *fn, int A, int B, int pc) {
  (void)pc;
  int b = B;
  emit_reg(fn, "ra", A);
  do { membuff_add_fstring(&fn->body, "setnilvalue(ra++);\n"); } while (b--);
}

static void emit_op_jmp(struct function *fn, int A, int sBx, int pc) {
  (void)pc;
  if (A > 0) {
    membuff_add_fstring(&fn->body, "ra = R(%d);\n", A - 1);
    membuff_add_string(&fn->body, "luaF_close(L, ra);\n");
  }
  membuff_add_fstring(&fn->body, "goto Lbc_%d;\n", sBx);
}

static void emit_op_loadbool(struct function *fn, int A, int B, int C, int j, int pc) {
  (void)pc;
  membuff_add_fstring(&fn->body, "ra = R(%d);\n", A);
  membuff_add_fstring(&fn->body, "setbvalue(ra, %d);\n", B);

  if (C) { membuff_add_fstring(&fn->body, "goto Lbc_%d;\n", j); }
}

static void emit_op_test(struct function *fn, int A, int B, int C, int j, int jA, int pc) {
  (void)pc;
  (void)B;
  emit_reg(fn, "ra", A);
  if (C) { membuff_add_string(&fn->body, "result = l_isfalse(ra);\n"); }
  else {
    membuff_add_string(&fn->body, "result = !l_isfalse(ra);\n");
  }
  membuff_add_fstring(&fn->body, "if (!result) {\n", A);
  if (jA > 0) {
    membuff_add_fstring(&fn->body, " ra = R(%d);\n", jA - 1);
    membuff_add_string(&fn->body, " luaF_close(L, ra);\n");
  }
  membuff_add_fstring(&fn->body, "  goto Lbc_%d;\n", j);
  membuff_add_string(&fn->body, " }\n");
}

static void emit_op_testset(struct function *fn, int A, int B, int C, int j, int jA, int pc) {
  (void)pc;
  membuff_add_fstring(&fn->body, "rb = R(%d);\n", B);
  if (C) { membuff_add_string(&fn->body, "result = l_isfalse(rb);\n"); }
  else {
    membuff_add_string(&fn->body, "result = !l_isfalse(rb);\n");
  }
  membuff_add_fstring(&fn->body, "if (!result) {\n", A);
  membuff_add_fstring(&fn->body, " ra = R(%d);\n", A);
  membuff_add_string(&fn->body, "  setobjs2s(L, ra, rb);");
  if (jA > 0) {
    membuff_add_fstring(&fn->body, " ra = R(%d);\n", jA - 1);
    membuff_add_string(&fn->body, " luaF_close(L, ra);\n");
  }
  membuff_add_fstring(&fn->body, "  goto Lbc_%d;\n", j);
  membuff_add_string(&fn->body, " }\n");
}

static void emit_endf(struct function *fn) {
  membuff_add_string(&fn->body, "Lraise_error:\n");
  membuff_add_string(&fn->body, "raise_error(L, error_code); /* does not return */\n");
  membuff_add_string(&fn->body, "return 0;\n");
  membuff_add_string(&fn->body, "}\n");
}

static void initfn(struct function *fn, struct lua_State *L, struct Proto *p, const char *fname, struct ravi_compile_options_t *options) {
  fn->L = L;
  fn->p = p;
  fn->var = 0;
  fn->options = options;
  snprintf(fn->fname, sizeof fn->fname, "%s", fname);
  fn->jmps = calloc(p->sizecode, sizeof fn->jmps[0]);
  if (p->sizelocvars)
    fn->locals = calloc(p->sizelocvars, sizeof fn->locals[0]);
  else
    fn->locals = NULL;
  membuff_init(&fn->prologue, strlen(Lua_header) + 4096);
  membuff_init(&fn->body, 4096);
  if (!(options->codegen_type == RAVI_CODEGEN_FUNCTION_ONLY)) 
	  membuff_add_string(&fn->prologue, Lua_header);
  membuff_add_fstring(&fn->prologue, "extern int %s(lua_State *L);\n", fn->fname);
  membuff_add_fstring(&fn->prologue, "int %s(lua_State *L) {\n", fn->fname);
  membuff_add_string(&fn->prologue, "int error_code = 0;\n");
  membuff_add_string(&fn->prologue, "lua_Integer i = 0;\n");
  membuff_add_string(&fn->prologue, "lua_Integer ic = 0;\n");
  membuff_add_string(&fn->prologue, "lua_Number n = 0.0;\n");
  membuff_add_string(&fn->prologue, "lua_Number nc = 0.0;\n");
  membuff_add_string(&fn->prologue, "int result = 0;\n");
  membuff_add_string(&fn->prologue, "StkId ra = NULL;\n");
  membuff_add_string(&fn->prologue, "StkId rb = NULL;\n");
  membuff_add_string(&fn->prologue, "StkId rc = NULL;\n");
  membuff_add_string(&fn->prologue, "lua_Unsigned ukey = 0;\n");
  membuff_add_string(&fn->prologue, "lua_Integer *iptr = NULL;\n");
  membuff_add_string(&fn->prologue, "lua_Number *nptr = NULL;\n");
  membuff_add_string(&fn->prologue, "Table *t = NULL;\n");
  // TODO we never set this???
  // ci->callstatus |= CIST_FRESH;  /* fresh invocation of 'luaV_execute" */
  // lua_assert(ci == L->ci);
  membuff_add_string(&fn->prologue, "CallInfo *ci = L->ci;\n");
  membuff_add_string(&fn->prologue, "LClosure *cl = clLvalue(ci->func);\n");
  membuff_add_string(&fn->prologue, "TValue *k = cl->p->k;\n");
  membuff_add_string(&fn->prologue, "StkId base = ci->u.l.base;\n");
}

// Handle OP_CALL
// Note that Lua assumes that functions called via OP_CALL
// are Lua functions and secondly that once OP_CALL completes the
// current function will continue within the same luaV_execute()
// call. However in a JIT case each JIT function is a different call
// so we need to take care of the behaviour differences between
// OP_CALL and external calls
static void emit_op_call(struct function *fn, int A, int B, int C, int pc) {
  int nresults = C - 1;
  if (B != 0) { membuff_add_fstring(&fn->body, "L->top = R(%d);\n", A + B); }
  emit_reg(fn, "ra", A);
  emit_update_savedpc(fn, pc);
  membuff_add_fstring(&fn->body, "result = luaD_precall(L, ra, %d, 1);\n", nresults);
  membuff_add_string(&fn->body, "if (result) {\n");
  membuff_add_fstring(&fn->body, " if (result == 1 && %d >= 0)\n", nresults);
  membuff_add_string(&fn->body, "  L->top = ci->top;\n");
  membuff_add_string(&fn->body, "}\n");
  membuff_add_string(&fn->body, "else {  /* Lua function */\n");
  membuff_add_string(&fn->body, " result = luaV_execute(L);\n");
  membuff_add_string(&fn->body, " if (result) L->top = ci->top;\n");
  membuff_add_string(&fn->body, "}\n");
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

// R(A) := UpValue[B][RK(C)]
static void emit_op_gettabup(struct function *fn, int A, int B, int C, int pc, OpCode op) {
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rc", C);
  emit_update_savedpc(fn, pc);
  if (op == OP_RAVI_GETTABUP_SK) {
	membuff_add_fstring(&fn->body, "raviV_gettable_sskey(L, cl->upvals[%d]->v, rc, ra);\n", B);
  }
  else {
    membuff_add_fstring(&fn->body, "luaV_gettable(L, cl->upvals[%d]->v, rc, ra);\n", B);
  }
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

// R(A) := UpValue[B][RK(C)]
static void emit_op_settabup(struct function *fn, int A, int B, int C, int pc) {
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  emit_update_savedpc(fn, pc);
  membuff_add_fstring(&fn->body, "luaV_settable(L, cl->upvals[%d]->v, rb, rc);\n", A);
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

// R(A) := R(B)[RK(C)]
static void emit_op_gettable(struct function *fn, int A, int B, int C, int pc, OpCode op) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  emit_update_savedpc(fn, pc);
  if (op == OP_RAVI_GETFIELD) { 
    membuff_add_string(&fn->body, "raviV_gettable_sskey(L, rb, rc, ra);\n"); }
  else if (op == OP_RAVI_GETI) {
    membuff_add_string(&fn->body, "raviV_gettable_i(L, rb, rc, ra);\n");
  }
  else {
    membuff_add_string(&fn->body, "luaV_gettable(L, rb, rc, ra);\n");
  }
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

static void emit_op_self(struct function *fn, int A, int B, int C, int pc, OpCode op) {
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  membuff_add_string(&fn->body, "setobjs2s(L, ra + 1, rb);\n");
  emit_reg_or_k(fn, "rc", C);
  emit_update_savedpc(fn, pc);
  if (op == OP_RAVI_SELF_SK) {
    membuff_add_string(&fn->body, "raviV_gettable_sskey(L, rb, rc, ra);\n");
  } else {
    membuff_add_string(&fn->body, "luaV_gettable(L, rb, rc, ra);\n");
  }
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

static void emit_op_len(struct function *fn, int A, int B, int pc) {
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  emit_update_savedpc(fn, pc);
  membuff_add_string(&fn->body, "luaV_objlen(L, ra, rb);\n");
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

// R(A)[RK(B)] := RK(C)
static void emit_op_settable(struct function *fn, int A, int B, int C, int pc, OpCode op) {
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  emit_update_savedpc(fn, pc);
  if (op == OP_RAVI_SETFIELD) { 
    membuff_add_string(&fn->body, "raviV_settable_sskey(L, ra, rb, rc);\n"); }
  else if (op == OP_RAVI_SETI) {
    membuff_add_string(&fn->body, "raviV_settable_i(L, ra, rb, rc);\n");
  }
  else {
    membuff_add_fstring(&fn->body, "luaV_settable(L, ra, rb, rc);\n", B);
  }
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

// R(A) := UpValue[B]
static void emit_op_getupval(struct function *fn, int A, int B, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_fstring(&fn->body, "setobjs2s(L, ra, cl->upvals[%d]->v);\n", B);
}

static void emit_op_newtable(struct function *fn, int A, int B, int C, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_fstring(&fn->body, "raviV_op_newtable(L, ci, ra, %d, %d);\n", B, C);
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

static void emit_op_newarrayint(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "raviV_op_newarrayint(L, ci, ra);\n");
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

static void emit_op_newarrayfloat(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "raviV_op_newarrayfloat(L, ci, ra);\n");
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

// Default implementation for binary ops
static void emit_binary_op(struct function *fn, int A, int B, int C, OpCode op, int pc) {
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  emit_update_savedpc(fn, pc);
  membuff_add_fstring(&fn->body, "luaO_arith(L, %d, rb, rc, ra);\n", cast_int(op - OP_ADD));
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

void emit_ff_op(struct function *fn, int A, int B, int C, int pc, const char *op) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_fstring(&fn->body, "setfltvalue(ra, fltvalue(rb) %s fltvalue(rc));\n", op);
}

static void emit_fi_op(struct function *fn, int A, int B, int C, int pc, const char *op) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_fstring(&fn->body, "setfltvalue(ra, fltvalue(rb) %s ivalue(rc));\n", op);
}

static void emit_if_op(struct function *fn, int A, int B, int C, int pc, const char *op) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_fstring(&fn->body, "setfltvalue(ra, ivalue(rb) %s fltvalue(rc));\n", op);
}

static void emit_ii_op(struct function *fn, int A, int B, int C, int pc, const char *op) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_fstring(&fn->body, "setivalue(ra, ivalue(rb) %s ivalue(rc));\n", op);
}

static void emit_op_divii(struct function *fn, int A, int B, int C, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg_or_k(fn, "rb", B);
  emit_reg_or_k(fn, "rc", C);
  membuff_add_string(&fn->body,
                     "setfltvalue(ra, (lua_Number)(ivalue(rb)) / "
                     "(lua_Number)(ivalue(rc)));\n");
}

static void emit_op_loadfz(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "setfltvalue(ra, 0.0);\n");
}
static void emit_op_loadiz(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "setivalue(ra, 0);\n");
}

static void emit_op_movei(struct function *fn, int A, int B, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  membuff_add_string(&fn->body, "i = 0;\n");
  membuff_add_string(&fn->body, "if (ttisinteger(rb)) { i = ivalue(rb); setivalue(ra, i); }\n");
  membuff_add_string(&fn->body, "else {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_integer_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_integer_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_movef(struct function *fn, int A, int B, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  membuff_add_string(&fn->body, "n = 0.0;\n");
  membuff_add_string(
      &fn->body,
      "if (ttisnumber(rb)) { n = (ttisfloat(rb) ? fltvalue(rb) : (double)ivalue(rb)); setfltvalue(ra, n); }\n");
  membuff_add_string(&fn->body, "else {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_number_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_number_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_MOVEIARRAY(struct function *fn, int A, int B, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  membuff_add_string(&fn->body, "if (ttisiarray(rb)) { setobjs2s(L, ra, rb); }\n");
  membuff_add_string(&fn->body, "else {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_integer_array_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_integer_array_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_MOVEFARRAY(struct function *fn, int A, int B, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  membuff_add_string(&fn->body, "if (ttisfarray(rb)) { setobjs2s(L, ra, rb); }\n");
  membuff_add_string(&fn->body, "else {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_number_array_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_number_array_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_movetab(struct function *fn, int A, int B, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  membuff_add_string(&fn->body, "if (ttisLtable(rb)) { setobjs2s(L, ra, rb); }\n");
  membuff_add_string(&fn->body, "else {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_table_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_table_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_toint(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "i = 0;\n");
  membuff_add_string(&fn->body, "if (ttisinteger(ra)) {i = ivalue(ra); setivalue(ra, i); }\n");
  membuff_add_string(&fn->body, "else {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_integer_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_integer_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_toflt(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "n = 0.0;\n");
  membuff_add_string(
      &fn->body,
      "if (ttisnumber(ra)) { n = (ttisinteger(ra) ? (double) ivalue(ra) : fltvalue(ra)); setfltvalue(ra, n); }\n");
  membuff_add_string(&fn->body, "else {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_number_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_number_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_toai(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "if (!ttisiarray(ra)) {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_integer_array_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_integer_array_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_toaf(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "if (!ttisfarray(ra)) {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_number_array_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_number_array_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_totab(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "if (!ttisLtable(ra)) {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_table_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_table_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_toclosure(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "if (!ttisclosure(ra)) {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_closure_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_closure_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_tostring(struct function *fn, int A, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "if (!ttisstring(ra)) {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, " error_code = %d;\n", Error_string_expected);
  membuff_add_string(&fn->body, " goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_string_expected);
#endif
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_totype(struct function *fn, int A, int Bx, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "if (!ttisnil(ra)) {\n");
  membuff_add_fstring(&fn->body, " rb = K(%d);\n", Bx);
  membuff_add_string(&fn->body, "  if (!ttisshrstring(rb) || !raviV_check_usertype(L, tsvalue(rb), ra)) {\n");
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, "   error_code = %d;\n", Error_type_mismatch);
  membuff_add_string(&fn->body, "   goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, "   raise_error(L, %d);\n", Error_type_mismatch);
#endif
  membuff_add_string(&fn->body, "  }\n");
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_setlist(struct function *fn, int A, int B, int C, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_fstring(&fn->body, "raviV_op_setlist(L, ci, ra, %d, %d);\n", B, C);
}

static void emit_op_concat(struct function *fn, int A, int B, int C, int pc) {
  emit_update_savedpc(fn, pc);
  membuff_add_fstring(&fn->body, "raviV_op_concat(L, ci, %d, %d, %d);\n", A, B, C);
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

static void emit_op_closure(struct function *fn, int A, int Bx, int pc) {
  (void)pc;
  membuff_add_fstring(&fn->body, "raviV_op_closure(L, ci, cl, %d, %d);\n", A, Bx);
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

static void emit_op_vararg(struct function *fn, int A, int B, int pc) {
  (void)pc;
  membuff_add_fstring(&fn->body, "raviV_op_vararg(L, ci, cl, %d, %d);\n", A, B);
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
}

static void emit_op_not(struct function *fn, int A, int B, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  emit_reg(fn, "rb", B);
  membuff_add_string(&fn->body, "result = l_isfalse(rb);\n");
  membuff_add_string(&fn->body, "setbvalue(ra, result);\n");
}

static void emit_op_setupval(struct function *fn, int A, int B, int pc, const char *suffix) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_fstring(&fn->body, "raviV_op_setupval%s(L, cl, ra, %d);\n", suffix, B);
}

static void emit_op_iforprep(struct function *fn, int A, int pc, int step_one, int pc1) {
  (void)pc1;
  if (!fn->locals[A]) {
    fn->locals[A] = 1;  // Lua can reuse the same forloop vars if loop isn't nested
    // Although in IFOR instructions we do not need all the vars below
    // it can happen that the same slot is used by normal FOR loop
    // The optimizer should get rid of unused vars anyway
    membuff_add_fstring(&fn->prologue, "lua_Integer i_%d = 0;\n", A);
    membuff_add_fstring(&fn->prologue, "lua_Integer limit_%d = 0;\n", A);
    membuff_add_fstring(&fn->prologue, "lua_Integer step_%d = 0;\n", A);
    membuff_add_fstring(&fn->prologue, "lua_Number ninit_%d = 0.0;\n", A);
    membuff_add_fstring(&fn->prologue, "lua_Number nlimit_%d = 0.0;\n", A);
    membuff_add_fstring(&fn->prologue, "lua_Number nstep_%d = 0.0;\n", A);
    membuff_add_fstring(&fn->prologue, "int intloop_%d = 0;\n", A);
  }
  emit_reg(fn, "ra", A);
  membuff_add_fstring(&fn->body, "i_%d = ivalue(ra);\n", A);
  membuff_add_fstring(&fn->body, "ra = R(%d);\n", A + 1);
  membuff_add_fstring(&fn->body, "limit_%d = ivalue(ra);\n", A);
  if (!step_one) {
    membuff_add_fstring(&fn->body, "ra = R(%d);\n", A + 2);
    membuff_add_fstring(&fn->body, "step_%d = ivalue(ra);\n", A);
    membuff_add_fstring(&fn->body, "i_%d -= step_%d;\n", A, A);
  }
  else {
    membuff_add_fstring(&fn->body, "i_%d -= 1;\n", A);
  }
  membuff_add_fstring(&fn->body, "goto Lbc_%d;\n", pc);
}

static void emit_op_forprep(struct function *fn, int A, int pc, int pc1) {
  (void)pc1;
  if (!fn->locals[A]) {
    fn->locals[A] = 1;  // Lua can reuse the same forloop vars if loop isn't nested
    membuff_add_fstring(&fn->prologue, "lua_Integer i_%d = 0;\n", A);
    membuff_add_fstring(&fn->prologue, "lua_Integer limit_%d = 0;\n", A);
    membuff_add_fstring(&fn->prologue, "lua_Integer step_%d = 0;\n", A);
    membuff_add_fstring(&fn->prologue, "lua_Number ninit_%d = 0.0;\n", A);
    membuff_add_fstring(&fn->prologue, "lua_Number nlimit_%d = 0.0;\n", A);
    membuff_add_fstring(&fn->prologue, "lua_Number nstep_%d = 0.0;\n", A);
    membuff_add_fstring(&fn->prologue, "int intloop_%d = 0;\n", A);
  }
  emit_reg(fn, "ra", A);  // init
  membuff_add_string(&fn->body, "rb = ra+1; /*limit*/\n");
  membuff_add_string(&fn->body, "rc = ra+2; /*step*/\n");
  membuff_add_fstring(
      &fn->body, "if (ttisinteger(ra) && ttisinteger(rc) && ttisinteger(rb)) {\n", A);
  membuff_add_fstring(&fn->body, " limit_%d = ivalue(rb);\n", A);
  membuff_add_fstring(&fn->body, " i_%d = ivalue(ra);\n", A);
  membuff_add_fstring(&fn->body, " step_%d = ivalue(rc);\n", A);
  membuff_add_fstring(&fn->body, " i_%d -= step_%d;\n", A, A);
  membuff_add_fstring(&fn->body, " intloop_%d = 1;\n", A);
  membuff_add_string(&fn->body, "}\n");
  membuff_add_string(&fn->body, "else {\n");
  membuff_add_fstring(&fn->body, " if (!ttisnumber(rb)) {\n", A);
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, "  error_code = %d;\n", Error_for_limit_must_be_number);
  membuff_add_string(&fn->body, "  goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_for_limit_must_be_number);
#endif
  membuff_add_string(&fn->body, " }\n");
  membuff_add_fstring(&fn->body, " if (!ttisnumber(rc)) {\n", A);
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, "  error_code = %d;\n", Error_for_step_must_be_number);
  membuff_add_string(&fn->body, "  goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_for_step_must_be_number);
#endif
  membuff_add_string(&fn->body, " }\n");
  membuff_add_fstring(&fn->body, " if (!ttisnumber(ra)) {\n", A);
#if GOTO_ON_ERROR
  membuff_add_fstring(&fn->body, "  error_code = %d;\n", Error_for_initial_value_must_be_number);
  membuff_add_string(&fn->body, "  goto Lraise_error;\n");
#else
  membuff_add_fstring(&fn->body, " raise_error(L, %d);\n", Error_for_initial_value_must_be_number);
#endif
  membuff_add_string(&fn->body, " }\n");
  membuff_add_fstring(&fn->body, " tonumberns(rb, nlimit_%d);\n", A);
  membuff_add_fstring(&fn->body, " tonumberns(ra, ninit_%d);\n", A);
  membuff_add_fstring(&fn->body, " tonumberns(rc, nstep_%d);\n", A);
  membuff_add_fstring(&fn->body, " ninit_%d -= nstep_%d;\n", A, A);
  membuff_add_fstring(&fn->body, " intloop_%d = 0;\n", A);
  membuff_add_string(&fn->body, "}\n");
  membuff_add_fstring(&fn->body, "goto Lbc_%d;\n", pc);
}

static void emit_op_iforloop(struct function *fn, int A, int pc, int step_one, int pc1) {
  (void)pc1;
  if (!step_one) { membuff_add_fstring(&fn->body, "i_%d += step_%d;\n", A, A); }
  else {
    membuff_add_fstring(&fn->body, "i_%d += 1;\n", A);
  }
  membuff_add_fstring(&fn->body,
                      "if (i_%d <= limit_%d) {\n  ra = R(%d);\n  setivalue(ra, "
                      "i_%d);\n  goto Lbc_%d;\n}\n",
                      A, A, A + 3, A, pc);
}

static void emit_op_forloop(struct function *fn, int A, int pc, int pc1) {
  (void)pc1;
  membuff_add_fstring(&fn->body, "if (intloop_%d) {\n", A);
  membuff_add_fstring(&fn->body, " i_%d += step_%d;\n", A, A);
  membuff_add_fstring(&fn->body, "  if ((0 < step_%d) ? (i_%d <= limit_%d) : (limit_%d <= i_%d)) {\n", A, A, A, A, A);
  membuff_add_fstring(&fn->body, "   ra = R(%d);\n   setivalue(ra, i_%d);\n   goto Lbc_%d;\n", A + 3, A, pc);
  membuff_add_string(&fn->body, "  }\n");
  membuff_add_string(&fn->body, "}\n");
  membuff_add_string(&fn->body, "else {\n");
  membuff_add_fstring(&fn->body, " ninit_%d += nstep_%d;\n", A, A);
  membuff_add_fstring(&fn->body, "  if ((0.0 < nstep_%d) ? (ninit_%d <= nlimit_%d) : (nlimit_%d <= ninit_%d)) {\n", A,
                      A, A, A, A);
  membuff_add_fstring(&fn->body, "   ra = R(%d);\n   setfltvalue(ra, ninit_%d);\n   goto Lbc_%d;\n", A + 3, A, pc);
  membuff_add_string(&fn->body, "  }\n");
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_tforcall(struct function *fn, int A, int B, int C, int j, int jA, int pc) {
  (void)B;
  emit_update_savedpc(fn, pc);
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "rb = ra + 3 + 2;\n"); /*rb = cb*/
  membuff_add_string(&fn->body, "rc = ra + 2;\n");     /*rb = cb*/
  membuff_add_string(&fn->body, "setobjs2s(L, rb, rc);\n");
  membuff_add_string(&fn->body, "rb = ra + 3 + 1;\n"); /*rb = cb*/
  membuff_add_string(&fn->body, "rc = ra + 1;\n");     /*rb = cb*/
  membuff_add_string(&fn->body, "setobjs2s(L, rb, rc);\n");
  membuff_add_string(&fn->body, "rb = ra + 3;\n"); /*rb = cb*/
  membuff_add_string(&fn->body, "setobjs2s(L, rb, ra);\n");
  membuff_add_string(&fn->body, "L->top = rb + 3;\n");
  membuff_add_fstring(&fn->body, "luaD_call(L, rb, %d);\n", C);
  membuff_add_string(&fn->body, "base = ci->u.l.base;\n");
  membuff_add_string(&fn->body, "L->top = ci->top;\n");
  emit_reg(fn, "ra", jA);
  membuff_add_string(&fn->body, "if (!ttisnil(ra + 1)) {\n");
  membuff_add_string(&fn->body, " rb = ra + 1;\n");
  membuff_add_string(&fn->body, " setobjs2s(L, ra, rb);\n");
  membuff_add_fstring(&fn->body, " goto Lbc_%d;\n", j);
  membuff_add_string(&fn->body, "}\n");
}

static void emit_op_tforloop(struct function *fn, int A, int j, int pc) {
  (void)pc;
  emit_reg(fn, "ra", A);
  membuff_add_string(&fn->body, "if (!ttisnil(ra + 1)) {\n");
  membuff_add_string(&fn->body, " rb = ra + 1;\n");
  membuff_add_string(&fn->body, " setobjs2s(L, ra, rb);\n");
  membuff_add_fstring(&fn->body, " goto Lbc_%d;\n", j);
  membuff_add_string(&fn->body, "}\n");
}

static void cleanup(struct function *fn) {
  free(fn->jmps);
  if (fn->locals) free(fn->locals);
  membuff_free(&fn->prologue);
  membuff_free(&fn->body);
}

// Compile a Lua function
// Returns true if compilation was successful
bool raviJ_codegen(struct lua_State *L, struct Proto *p,
	struct ravi_compile_options_t *options, const char *fname, membuff_t *buf) {

	if (options->codegen_type == RAVI_CODEGEN_HEADER_ONLY) {
		membuff_rewindpos(buf);
		membuff_add_string(buf, Lua_header);
		return true;
	}

	if (!raviJ_cancompile(p))
		return false;

	struct function fn;
	initfn(&fn, L, p, fname, options);
	scan_jump_targets(&fn);

	const Instruction *code = p->code;
	int pc, n = p->sizecode;
	for (pc = 0; pc < n; pc++) {
		emit_jump_label(&fn, pc);
		Instruction i = code[pc];
		OpCode op = GET_OPCODE(i);
		int A = GETARG_A(i);
		switch (op) {
		case OP_LOADK: {
			int Bx = GETARG_Bx(i);
			emit_op_loadk(&fn, A, Bx, pc);
		} break;
		case OP_LOADKX: {
			Instruction inst = code[++pc];
			int Ax = GETARG_Ax(inst);
			lua_assert(GET_OPCODE(inst) == OP_EXTRAARG);
			emit_op_loadk(&fn, A, Ax, pc - 1);
		} break;
		case OP_LOADNIL: {
			int B = GETARG_B(i);
			emit_op_loadnil(&fn, A, B, pc);
		} break;
		case OP_RETURN: {
			int B = GETARG_B(i);
			emit_op_return(&fn, A, B, pc);
		} break;
		case OP_RAVI_FORPREP_I1:
		case OP_RAVI_FORPREP_IP: {
			int sbx = GETARG_sBx(i);
			int j = sbx + pc + 1;
			emit_op_iforprep(&fn, A, j, op == OP_RAVI_FORPREP_I1, pc);
		} break;
		case OP_RAVI_FORLOOP_I1:
		case OP_RAVI_FORLOOP_IP: {
			int sbx = GETARG_sBx(i);
			int j = sbx + pc + 1;
			emit_op_iforloop(&fn, A, j, op == OP_RAVI_FORLOOP_I1, pc);
		} break;
		case OP_FORPREP: {
			int sbx = GETARG_sBx(i);
			int j = sbx + pc + 1;
			emit_op_forprep(&fn, A, j, pc);
		} break;
		case OP_FORLOOP: {
			int sbx = GETARG_sBx(i);
			int j = sbx + pc + 1;
			emit_op_forloop(&fn, A, j, pc);
		} break;
		case OP_TFORCALL: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			// OP_TFORCALL is followed by OP_TFORLOOP - we process this
			// along with OP_TFORCALL
			pc++;
			i = code[pc];
			op = GET_OPCODE(i);
			lua_assert(op == OP_TFORLOOP);
			int sbx = GETARG_sBx(i);
			// j below is the jump target
			int j = sbx + pc + 1;
			emit_op_tforcall(&fn, A, B, C, j, GETARG_A(i), pc - 1);
		} break;
		case OP_TFORLOOP: {
			int sbx = GETARG_sBx(i);
			int j = sbx + pc + 1;
			emit_op_tforloop(&fn, A, j, pc);
		} break;
		case OP_MOVE: {
			int B = GETARG_B(i);
			emit_op_move(&fn, A, B, pc);
		} break;
		case OP_JMP: {
			int sbx = GETARG_sBx(i);
			int j = sbx + pc + 1;
			emit_op_jmp(&fn, A, j, pc);
		} break;
		case OP_LOADBOOL: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_op_loadbool(&fn, A, B, C, pc + 2, pc);
		} break;
		case OP_RAVI_EQ_II:
		case OP_RAVI_EQ_FF:
		case OP_RAVI_LT_II:
		case OP_RAVI_LT_FF:
		case OP_RAVI_LE_II:
		case OP_RAVI_LE_FF:
		case OP_LT:
		case OP_LE:
		case OP_EQ: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			OpCode compOperator = op;
			const char *comparison_function =
				((op == OP_EQ || op == OP_RAVI_EQ_II || op == OP_RAVI_EQ_FF)
					? "luaV_equalobj"
					: ((op == OP_LT || op == OP_RAVI_LT_II || op == OP_RAVI_LT_FF)
						? "luaV_lessthan"
						: "luaV_lessequal"));
			// OP_EQ etc is followed by OP_JMP - we process this
			// along with OP_EQ etc
			pc++;
			i = code[pc];
			op = GET_OPCODE(i);
			lua_assert(op == OP_JMP);
			int sbx = GETARG_sBx(i);
			// j below is the jump target
			int j = sbx + pc + 1;
			emit_comparison(&fn, A, B, C, j, GETARG_A(i), comparison_function,
				compOperator, pc - 1);
		} break;
		case OP_NOT: {
			int B = GETARG_B(i);
			emit_op_not(&fn, A, B, pc);
		} break;
		case OP_TEST: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			// OP_TEST is followed by OP_JMP - we process this
			// along with OP_TEST
			pc++;
			i = code[pc];
			op = GET_OPCODE(i);
			lua_assert(op == OP_JMP);
			int sbx = GETARG_sBx(i);
			// j below is the jump target
			int j = sbx + pc + 1;
			emit_op_test(&fn, A, B, C, j, GETARG_A(i), pc - 1);
		} break;
		case OP_TESTSET: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			// OP_TESTSET is followed by OP_JMP - we process this
			// along with OP_TESTSET
			pc++;
			i = code[pc];
			op = GET_OPCODE(i);
			lua_assert(op == OP_JMP);
			int sbx = GETARG_sBx(i);
			// j below is the jump target
			int j = sbx + pc + 1;
			emit_op_testset(&fn, A, B, C, j, GETARG_A(i), pc - 1);
		} break;
		case OP_RAVI_TABLE_GETFIELD:
		case OP_RAVI_GETFIELD:
		case OP_RAVI_GETI:
		case OP_GETTABLE: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_op_gettable(&fn, A, B, C, pc, op);
		} break;
		case OP_RAVI_IARRAY_GET: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_IARRAY_GET(&fn, A, B, C, options->omit_array_get_range_check, pc);
		} break;
		case OP_RAVI_FARRAY_GET: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_FARRAY_GET(&fn, A, B, C, options->omit_array_get_range_check, pc);
		} break;
		case OP_RAVI_SETFIELD:
		case OP_RAVI_TABLE_SETFIELD:
		case OP_RAVI_SETI:
		case OP_SETTABLE: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_op_settable(&fn, A, B, C, pc, op);
		} break;
		case OP_RAVI_IARRAY_SETI: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_IARRAY_SETI(&fn, A, B, C, true, pc);
		} break;
		case OP_RAVI_IARRAY_SET: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_IARRAY_SET(&fn, A, B, C, true, pc);
		} break;
		case OP_RAVI_FARRAY_SETF: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_FARRAY_SETF(&fn, A, B, C, true, pc);
		} break;
		case OP_RAVI_FARRAY_SET: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_FARRAY_SET(&fn, A, B, C, true, pc);
		} break;
		case OP_RAVI_GETTABUP_SK:
		case OP_GETTABUP: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_op_gettabup(&fn, A, B, C, pc, op);
		} break;
		case OP_SETTABUP: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_op_settabup(&fn, A, B, C, pc);
		} break;
		case OP_TAILCALL:
		case OP_CALL: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_op_call(&fn, A, B, C, pc);
		} break;
		case OP_GETUPVAL: {
			int B = GETARG_B(i);
			emit_op_getupval(&fn, A, B, pc);
		} break;
		case OP_SETUPVAL: {
			int B = GETARG_B(i);
			emit_op_setupval(&fn, A, B, pc, "");
		} break;
		case OP_RAVI_SETUPVALI: {
			int B = GETARG_B(i);
			emit_op_setupval(&fn, A, B, pc, "i");
		} break;
		case OP_RAVI_SETUPVALF: {
			int B = GETARG_B(i);
			emit_op_setupval(&fn, A, B, pc, "f");
		} break;
		case OP_RAVI_SETUPVAL_IARRAY: {
			int B = GETARG_B(i);
			emit_op_setupval(&fn, A, B, pc, "ai");
		} break;
		case OP_RAVI_SETUPVAL_FARRAY: {
			int B = GETARG_B(i);
			emit_op_setupval(&fn, A, B, pc, "af");
		} break;
		case OP_RAVI_SETUPVALT: {
			int B = GETARG_B(i);
			emit_op_setupval(&fn, A, B, pc, "t");
		} break;
		case OP_NEWTABLE: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_op_newtable(&fn, A, B, C, pc);
		} break;
#if 0
    case OP_BNOT: {
      int B = GETARG_B(i);
      emit_BNOT(&fn, A, B, pc);
    } break;
    case OP_UNM: {
      int B = GETARG_B(i);
      emit_UNM(&fn, A, B, pc);
    } break;
#endif
		case OP_ADD: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			if (options->inline_lua_arithmetic_operators) {
				emit_op_arithslow(&fn, A, B, C, op, pc, "+",
					"TM_ADD");
			}
			else {
				emit_binary_op(&fn, A, B, C, op, pc);
			}
		} break;
		case OP_SUB: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			if (options->inline_lua_arithmetic_operators) {
				emit_op_arithslow(&fn, A, B, C, op, pc, "-",
					"TM_SUB");
			}
			else {
				emit_binary_op(&fn, A, B, C, op, pc);
			}
		} break;
		case OP_MUL: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			if (options->inline_lua_arithmetic_operators) {
				emit_op_arithslow(&fn, A, B, C, op, pc, "*",
					"TM_MUL");
			}
			else {
				emit_binary_op(&fn, A, B, C, op, pc);
			}
		} break;
		case OP_MOD:
		case OP_POW:
		case OP_DIV:
		case OP_IDIV:
		case OP_BAND:
		case OP_BOR:
		case OP_BXOR:
		case OP_SHL:
		case OP_SHR: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_binary_op(&fn, A, B, C, op, pc);
		} break;
		case OP_RAVI_ADDFF: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_ff_op(&fn, A, B, C, pc, "+");
		} break;
		case OP_RAVI_ADDFI: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_fi_op(&fn, A, B, C, pc, "+");
		} break;
		case OP_RAVI_ADDII: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_ii_op(&fn, A, B, C, pc, "+");
		} break;
		case OP_RAVI_SUBFF: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_ff_op(&fn, A, B, C, pc, "-");
		} break;
		case OP_RAVI_SUBFI: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_fi_op(&fn, A, B, C, pc, "-");
		} break;
		case OP_RAVI_SUBIF: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_if_op(&fn, A, B, C, pc, "-");
		} break;
		case OP_RAVI_SUBII: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_ii_op(&fn, A, B, C, pc, "-");
		} break;
		case OP_RAVI_MULFF: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_ff_op(&fn, A, B, C, pc, "*");
		} break;
		case OP_RAVI_MULFI: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_fi_op(&fn, A, B, C, pc, "*");
		} break;
		case OP_RAVI_MULII: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_ii_op(&fn, A, B, C, pc, "*");
		} break;
		case OP_RAVI_DIVFF: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_ff_op(&fn, A, B, C, pc, "/");
		} break;
		case OP_RAVI_DIVFI: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_fi_op(&fn, A, B, C, pc, "/");
		} break;
		case OP_RAVI_DIVIF: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_if_op(&fn, A, B, C, pc, "/");
		} break;
		case OP_RAVI_DIVII: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_op_divii(&fn, A, B, C, pc);
		} break;
		case OP_RAVI_LOADFZ: {
			emit_op_loadfz(&fn, A, pc);
		} break;
		case OP_RAVI_LOADIZ: {
			emit_op_loadiz(&fn, A, pc);
		} break;
		case OP_RAVI_TOINT: {
			emit_op_toint(&fn, A, pc);
		} break;
		case OP_RAVI_TOFLT: {
			emit_op_toflt(&fn, A, pc);
		} break;
		case OP_RAVI_TOTAB: {
			emit_op_totab(&fn, A, pc);
		} break;
		case OP_RAVI_TOIARRAY: {
			emit_op_toai(&fn, A, pc);
		} break;
		case OP_RAVI_TOFARRAY: {
			emit_op_toaf(&fn, A, pc);
		} break;
		case OP_RAVI_TOSTRING: {
			emit_op_tostring(&fn, A, pc);
		} break;
		case OP_RAVI_TOCLOSURE: {
			emit_op_toclosure(&fn, A, pc);
		} break;
		case OP_RAVI_TOTYPE: {
			emit_op_totype(&fn, A, GETARG_Bx(i), pc);
		} break;  
		case OP_RAVI_MOVEI: {
			int B = GETARG_B(i);
			emit_op_movei(&fn, A, B, pc);
		} break;
		case OP_RAVI_MOVEF: {
			int B = GETARG_B(i);
			emit_op_movef(&fn, A, B, pc);
		} break;
		case OP_RAVI_MOVEIARRAY: {
			int B = GETARG_B(i);
			emit_op_MOVEIARRAY(&fn, A, B, pc);
		} break;
		case OP_RAVI_MOVEFARRAY: {
			int B = GETARG_B(i);
			emit_op_MOVEFARRAY(&fn, A, B, pc);
		} break;
		case OP_RAVI_MOVETAB: {
			int B = GETARG_B(i);
			emit_op_movetab(&fn, A, B, pc);
		} break;
		case OP_RAVI_NEW_IARRAY: {
			emit_op_newarrayint(&fn, A, pc);
		} break;
		case OP_RAVI_NEW_FARRAY: {
			emit_op_newarrayfloat(&fn, A, pc);
		} break;
		case OP_CONCAT: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_op_concat(&fn, A, B, C, pc);
		} break;
		case OP_CLOSURE: {
			int Bx = GETARG_Bx(i);
			emit_op_closure(&fn, A, Bx, pc);
		} break;
		case OP_VARARG: {
			int B = GETARG_B(i);
			emit_op_vararg(&fn, A, B, pc);
		} break;
		case OP_SETLIST: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			if (C == 0) {
				// OP_SETLIST is followed by OP_EXTRAARG
				Instruction inst = code[++pc];
				C = GETARG_Ax(inst);
				lua_assert(GET_OPCODE(inst) == OP_EXTRAARG);
			}
			emit_op_setlist(&fn, A, B, C, pc);
		} break;
		case OP_SELF:
		case OP_RAVI_TABLE_SELF_SK:
		case OP_RAVI_SELF_SK: {
			int B = GETARG_B(i);
			int C = GETARG_C(i);
			emit_op_self(&fn, A, B, C, pc, op);
		} break;
		case OP_LEN: {
			int B = GETARG_B(i);
			emit_op_len(&fn, A, B, pc);
		} break;
		default: abort();
		}
	}
	emit_endf(&fn);
	membuff_rewindpos(buf);
	membuff_add_string(buf, fn.prologue.buf);
	membuff_add_string(buf, fn.body.buf);
	cleanup(&fn);
	return true;
}

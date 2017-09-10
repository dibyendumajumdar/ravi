/******************************************************************************
 * Copyright (C) 2015 Dibyendu Majumdar
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

#include <ravi_nanojit.h>
#include <ravi_membuf.h>
#include <ravijit.h>
#include <stddef.h>

// FIXME should be in ravi_State
static int id = 0;

#define LUA_CORE

#include "lauxlib.h"
#include "lobject.h"
#include "lstate.h"
#include "lua.h"

static bool register_builtin_arg1(NJXContextRef module, const char *name,
	void *fp, enum NJXValueKind return_type,
	enum NJXValueKind arg1)
{
	enum NJXValueKind args[1];
	args[0] = arg1;
	return NJX_register_C_function(module, name, fp, return_type, args, 1);
}
static bool register_builtin_arg2(NJXContextRef module, const char *name,
	void *fp, enum NJXValueKind return_type,
	enum NJXValueKind arg1,
	enum NJXValueKind arg2)
{
	enum NJXValueKind args[2];
	args[0] = arg1;
	args[1] = arg2;
	return NJX_register_C_function(module, name, fp, return_type, args, 2);
}
static bool register_builtin_arg3(NJXContextRef module, const char *name,
	void *fp, enum NJXValueKind return_type,
	enum NJXValueKind arg1,
	enum NJXValueKind arg2,
	enum NJXValueKind arg3)
{
	enum NJXValueKind args[3];
	args[0] = arg1;
	args[1] = arg2;
	args[2] = arg3;
	return NJX_register_C_function(module, name, fp, return_type, args, 3);
}
static bool register_builtin_arg4(NJXContextRef module, const char *name,
	void *fp, enum NJXValueKind return_type,
	enum NJXValueKind arg1,
	enum NJXValueKind arg2,
	enum NJXValueKind arg3,
	enum NJXValueKind arg4)
{
	enum NJXValueKind args[4];
	args[0] = arg1;
	args[1] = arg2;
	args[2] = arg3;
	args[3] = arg4;
	return NJX_register_C_function(module, name, fp, return_type, args, 4);
}

// Initialize the JIT State and attach it to the
// Global Lua State
// If a JIT State already exists then this function
// will return -1
int raviV_initjit(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state != NULL) return -1;
  ravi_State *jit = (ravi_State *)calloc(1, sizeof(ravi_State));
  // The parameter true means we will be dumping stuff as we compile
  jit->jit = NJX_create_context(true);
  //extern void luaF_close (lua_State *L, StkId level);
  register_builtin_arg2(jit->jit, "luaF_close", luaF_close, NJXValueKind_V, NJXValueKind_P, NJXValueKind_P);
  //extern int luaD_poscall (lua_State *L, CallInfo *ci, StkId firstResult, int nres);
  register_builtin_arg4(jit->jit, "luaD_poscall", luaD_poscall, NJXValueKind_I, NJXValueKind_P, NJXValueKind_P, NJXValueKind_P, NJXValueKind_I);
  G->ravi_state = jit;
  return 0;
}

// Free up the JIT State
void raviV_close(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL) return;
  // All compiled functions will be deleted at this stage
  NJX_destroy_context(G->ravi_state->jit);
  free(G->ravi_state);
}

// Dump the LLVM IR
void raviV_dumpIR(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}

// Dump the LLVM ASM
void raviV_dumpASM(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}

void raviV_setminexeccount(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  // G->ravi_state->jit->min_exec_count_ = value;
}
int raviV_getminexeccount(lua_State *L) {
  global_State *G = G(L);
  // if (!G->ravi_state)
  return 0;
  // return G->ravi_state->jit->min_exec_count_;
}

void raviV_setmincodesize(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  // G->ravi_state->jit->min_code_size_ = value;
}
int raviV_getmincodesize(lua_State *L) {
  global_State *G = G(L);
  // if (!G->ravi_state)
  return 0;
  // return G->ravi_state->jit->min_code_size_;
}

void raviV_setauto(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  // G->ravi_state->jit->auto_ = value;
}
int raviV_getauto(lua_State *L) {
  global_State *G = G(L);
  // if (!G->ravi_state)
  return 0;
  // return G->ravi_state->jit->auto_;
}

// Turn on/off the JIT compiler
void raviV_setjitenabled(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  // G->ravi_state->jit->enabled_ = value;
}
int raviV_getjitenabled(lua_State *L) {
  global_State *G = G(L);
  // if (!G->ravi_state)
  return 0;
  // return G->ravi_state->jit->enabled_;
}

void raviV_setoptlevel(lua_State *L, int value) {
  global_State *G = G(L);
  if (!G->ravi_state) return;
  // G->ravi_state->jit->opt_level_ = value;
}
int raviV_getoptlevel(lua_State *L) {
  global_State *G = G(L);
  // if (!G->ravi_state)
  return 0;
  // return G->ravi_state->jit->opt_level_;
}

void raviV_setsizelevel(lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getsizelevel(lua_State *L) {
  (void)L;
  return 0;
}

void raviV_setgcstep(lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getgcstep(lua_State *L) {
  (void)L;
  return 0;
}

// Turn on/off the JIT compiler
void raviV_settraceenabled(lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_gettraceenabled(lua_State *L) {
  (void)L;
  return 0;
}

int raviV_compile_n(struct lua_State *L, struct Proto *p[], int n,
                    ravi_compile_options_t *options) {
  int count = 0;
  for (int i = 0; i < n; i++) {
    if (raviV_compile(L, p[i], options)) count++;
  }
  return count > 0;
}

static const char Lua_header[] = ""
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
"typedef long long lua_Integer;\n"
"typedef unsigned long long lua_Unsigned;\n"
"typedef int (*lua_CFunction) (lua_State *L);\n"
"typedef __SIZE_TYPE__ size_t;\n"
"typedef long long ptrdiff_t;\n"
"typedef long long intptr_t;\n"
"typedef size_t lu_mem;\n"
"typedef ptrdiff_t l_mem;\n"
"typedef unsigned char lu_byte;\n"
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
"#define LUA_TSHRSTR	(LUA_TSTRING | (0 << 4))\n"
"#define LUA_TLNGSTR	(LUA_TSTRING | (1 << 4))\n"
"#define LUA_TNUMFLT	(LUA_TNUMBER | (0 << 4))\n"
"#define LUA_TNUMINT	(LUA_TNUMBER | (1 << 4))\n"
"#define RAVI_TIARRAY (LUA_TTABLE | (1 << 4))\n"
"#define RAVI_TFARRAY (LUA_TTABLE | (2 << 4))\n"
"#define BIT_ISCOLLECTABLE	(1 << 6)\n"
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
"#define TValuefields	Value value_; int tt_\n"
"typedef struct lua_TValue {\n"
"  TValuefields;\n"
"} TValue;\n"
"#define NILCONSTANT	{NULL}, LUA_TNIL\n"
"#define val_(o)		((o)->value_)\n"
"#define rttype(o)	((o)->tt_)\n"
"#define novariant(x)	((x) & 0x0F)\n"
"#define ttype(o)	(rttype(o) & 0x3F)\n"
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
"	{ TValue *io1=(obj1); *io1 = *(obj2); \\\n"
"	  (void)L; checkliveness(L,io1); }\n"
"#define setobjs2s	setobj\n"
"#define setobj2s	setobj\n"
"#define setsvalue2s	setsvalue\n"
"#define sethvalue2s	sethvalue\n"
"#define setptvalue2s	setptvalue\n"
"#define setobjt2t	setobj\n"
"#define setobj2n	setobj\n"
"#define setsvalue2n	setsvalue\n"
"#define setobj2t(L,o1,o2)  ((void)L, *(o1)=*(o2), checkliveness(L,(o1)))\n"
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
"	lu_byte ttuv_;\n"
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
"	RAVI_TANY = -1,\n"
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
"	ravitype_t type;\n"
"	lu_byte instack;\n"
"	lu_byte idx;\n"
"} Upvaldesc;\n"
"typedef struct LocVar {\n"
"	TString *varname;\n"
"	int startpc;\n"
"	int endpc;\n"
"	ravitype_t ravi_type;\n"
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
"extern void luaF_close (lua_State *L, StkId level);\n"
"extern int luaD_poscall (lua_State *L, CallInfo *ci, StkId firstResult, int nres);\n"
"#define RB(i) (base + i)\n"
;

static int showparsetree(const char *buffer);

// We can only compile a subset of op codes
// and not all features are supported
static bool can_compile(Proto *p) {
  if (p->ravi_jit.jit_status == 1) return false;
  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  // Loop over the byte codes; as Lua compiler inserts
  // an extra RETURN op we need to ignore the last op
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode o = GET_OPCODE(i);
    switch (o) {
      case OP_RETURN: break;
#if 0
		case OP_LOADK:
		case OP_LOADKX:
		case OP_RAVI_FORLOOP_IP:
		case OP_RAVI_FORLOOP_I1:
		case OP_RAVI_FORPREP_IP:
		case OP_RAVI_FORPREP_I1:
		case OP_MOVE:
		case OP_LOADNIL:
		case OP_RAVI_LOADIZ:
		case OP_RAVI_LOADFZ:
		case OP_CALL:
		case OP_TAILCALL:
		case OP_JMP:
		case OP_EQ:
		case OP_RAVI_EQ_II:
		case OP_RAVI_EQ_FF:
		case OP_LT:
		case OP_RAVI_LT_II:
		case OP_RAVI_LT_FF:
		case OP_LE:
		case OP_RAVI_LE_II:
		case OP_RAVI_LE_FF:
		case OP_GETTABUP:
		case OP_LOADBOOL:
		case OP_NOT:
		case OP_TEST:
		case OP_TESTSET:
		case OP_RAVI_MOVEI:
		case OP_RAVI_MOVEF:
		case OP_RAVI_TOINT:
		case OP_RAVI_TOFLT:
		case OP_VARARG:
		case OP_CONCAT:
		case OP_CLOSURE:
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
		case OP_SELF:
		case OP_LEN:
		case OP_SETTABLE:
		case OP_GETTABLE:
		case OP_NEWTABLE:
		case OP_SETLIST:
		case OP_TFORCALL:
		case OP_TFORLOOP:
		case OP_RAVI_NEWARRAYI:
		case OP_RAVI_NEWARRAYF:
		case OP_RAVI_GETTABLE_AI:
		case OP_RAVI_GETTABLE_AF:
		case OP_RAVI_TOARRAYI:
		case OP_RAVI_TOARRAYF:
		case OP_RAVI_MOVEAI:
		case OP_RAVI_MOVEAF:
		case OP_RAVI_SETTABLE_AI:
		case OP_RAVI_SETTABLE_AII:
		case OP_RAVI_SETTABLE_AF:
		case OP_RAVI_SETTABLE_AFF:
		case OP_SETTABUP:
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
		case OP_GETUPVAL:
		case OP_SETUPVAL:
		case OP_FORPREP:
		case OP_FORLOOP:
		case OP_MOD:
		case OP_IDIV:
		case OP_UNM:
		case OP_POW:
#endif
      default: {
        p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
        return false;
      }
    }
  }
  return true;
}

struct function {
  struct lua_State *L;
  struct Proto *p;
  int id;
  char fname[30];
  int var;
  // should be bitmap
  // flags to mark instructions
  unsigned char *jmps;
  membuff_t prologue;
  membuff_t body;
};

static int add_local_var(struct function *fn) { return fn->var++; }

// Identify Ravi bytecode instructions that are jump
// targets - we need this so that when generating code
// we can emit labels for gotos
static void scan_jump_targets(struct function *fn) {
  const Instruction *code = fn->p->code;
  int pc, n = fn->p->sizecode;
  lua_assert(fn->jmps != NULL);
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    switch (op) {
      case OP_LOADBOOL: {
        int C = GETARG_C(i);
        int j = pc + 2;  // jump target
        fn->jmps[j] = 1;
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
        fn->jmps[j] = true;
      } break;
      default: break;
    }
  }
}

static void emit_jump_label(struct function *fn, int pc) {
  if (fn->jmps[pc]) {
    char labelstmt[80];
    snprintf(labelstmt, sizeof labelstmt, "Lbc_%d:\n", pc);
    membuff_add_string(&fn->body, labelstmt);
  }
}

static void emit_op_return(struct function *fn, int A, int B, int pc) {
  membuff_add_fstring(&fn->body, "ra = RB(%d);\n", A);
  membuff_add_string(&fn->body, "if (cl->p->sizep > 0) luaF_close(L, base);\n");
  int var = add_local_var(fn);
  membuff_add_fstring(&fn->prologue, "int nres_%d = 0;\n", var);
  membuff_add_fstring(&fn->body,
                      "nres_%d = (%d != 0 ? %d - 1 : cast_int(L->top - ra));\n",
                      var, B, B);
  membuff_add_fstring(&fn->body, "return luaD_poscall(L, ci, ra, nres_%d);\n",
                      var);
}

static void emit_endf(struct function *fn) {
  membuff_add_string(&fn->body, "}\n");
}

static void initfn(struct function *fn, struct lua_State *L, struct Proto *p) {
  fn->L = L;
  fn->p = p;
  fn->id = id++;
  fn->var = 0;
  snprintf(fn->fname, sizeof fn->fname, "jitf%d", fn->id);
  fn->jmps = calloc(p->sizecode, sizeof fn->jmps[0]);
  membuff_init(&fn->prologue, strlen(Lua_header) + 4096);
  membuff_init(&fn->body, 4096);
  membuff_add_string(&fn->prologue, Lua_header);
  membuff_add_fstring(&fn->prologue, "int %s(lua_State *L) {\n", fn->fname);
  membuff_add_string(&fn->prologue, "CallInfo *ci = L->ci;\n");
  membuff_add_string(&fn->prologue, "LClosure *cl;\n");
  membuff_add_string(&fn->prologue, "TValue *k;\n");
  membuff_add_string(&fn->prologue, "StkId base;\n");
  membuff_add_string(&fn->prologue, "StkId ra;\n");
  // TODO we never set this???
  // ci->callstatus |= CIST_FRESH;  /* fresh invocation of 'luaV_execute" */
  // lua_assert(ci == L->ci);
  membuff_add_string(&fn->prologue, "cl = clLvalue(ci->func);\n");
  membuff_add_string(&fn->prologue, "k = cl->p->k;\n");
  membuff_add_string(&fn->prologue, "base = ci->u.l.base;\n");
}

static void cleanup(struct function *fn) {
  free(fn->jmps);
  membuff_free(&fn->prologue);
  membuff_free(&fn->body);
}

#define RA(i) (base + GETARG_A(i))
/* to be used after possible stack reallocation */
#define RB(i) check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i))
#define RC(i) check_exp(getCMode(GET_OPCODE(i)) == OpArgR, base + GETARG_C(i))
#define RKB(i)                                 \
  check_exp(getBMode(GET_OPCODE(i)) == OpArgK, \
            ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i))
#define RKC(i)                                 \
  check_exp(getCMode(GET_OPCODE(i)) == OpArgK, \
            ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i))
#define KBx(i) \
  (k + (GETARG_Bx(i) != 0 ? GETARG_Bx(i) - 1 : GETARG_Ax(*ci->u.l.savedpc++)))
/* RAVI */
#define KB(i) \
  check_exp(getBMode(GET_OPCODE(i)) == OpArgK, k + INDEXK(GETARG_B(i)))
#define KC(i) \
  check_exp(getCMode(GET_OPCODE(i)) == OpArgK, k + INDEXK(GETARG_C(i)))

// Compile a Lua function
// If JIT is turned off then compilation is skipped
// Compilation occurs if either auto compilation is ON (subject to some
// thresholds)
// or if a manual compilation request was made
// Returns true if compilation was successful
int raviV_compile(struct lua_State *L, struct Proto *p,
                  ravi_compile_options_t *options) {
  if (p->ravi_jit.jit_status == RAVI_JIT_COMPILED) return true;

  if (p->ravi_jit.jit_status != RAVI_JIT_NOT_COMPILED || !can_compile(p))
    return false;

  global_State *G = G(L);
  if (G->ravi_state == NULL) return false;
  NJXContextRef context = G->ravi_state->jit;
  if (context == NULL) return false;

  struct function fn;
  initfn(&fn, L, p);

  scan_jump_targets(&fn);

  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  for (pc = 0; pc < n; pc++) {
    emit_jump_label(&fn, pc);
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    int A = GETARG_A(i);
    switch (op) {
      case OP_RETURN: {
        int B = GETARG_B(i);
        emit_op_return(&fn, A, B, pc);
      } break;

      default: abort();
    }
  }
  emit_endf(&fn);

  membuff_add_string(&fn.prologue, fn.body.buf);
  printf(fn.prologue.buf);

  showparsetree(fn.prologue.buf);

  int (*fp)(lua_State * L) = NULL;
  char *argv[] = {NULL};
  if (!dmrC_nanocompile(0, argv, context, fn.prologue.buf)) {
    p->ravi_jit.jit_status = RAVI_JIT_CANT_COMPILE;
  }
  else {
    fp = NJX_get_function_by_name(context, fn.fname);
    if (fp != NULL) {
      p->ravi_jit.jit_data = NULL;
      p->ravi_jit.jit_function = fp;
      p->ravi_jit.jit_status = RAVI_JIT_COMPILED;
    }
  }

  cleanup(&fn);
  return fp != NULL;
}

// Free the JIT compiled function
// Note that this is called by the garbage collector
void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}

#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <walksymbol.h>

struct tree_visitor {
	struct dmr_C *C;
	int nesting_level;
};

static const char spaces[] = "                                           ";
static void nestedprint(struct tree_visitor *visitor, const char *fmt,
	va_list args)
{
	printf("%.*s", visitor->nesting_level, spaces);
	vfprintf(stdout, fmt, args);
}

static void output(struct tree_visitor *visitor, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	nestedprint(visitor, fmt, args);
	va_end(args);
}

static void begin_symbol_impl(void *data, struct symbol_info *syminfo)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;

	if (syminfo->symbol_type == SYM_NODE) {
		if (syminfo->name && syminfo->name[0])
			output(treevisitor, "%s id:%llu\t\t\t\tSYM_NODE\n",
				syminfo->name, syminfo->id);
		else
			output(treevisitor, "id:%llu\t\t\t\tSYM_NODE\n",
				syminfo->id);
	}
	else if (syminfo->symbol_type == SYM_BASETYPE ||
		syminfo->symbol_type == SYM_LABEL) {
		output(treevisitor, "%s\n", syminfo->name);
	}
	else if (syminfo->symbol_type == SYM_STRUCT ||
		syminfo->symbol_type == SYM_UNION ||
		syminfo->symbol_type == SYM_ENUM) {
		output(treevisitor, "%s %s\n",
			dmrC_get_type_name(syminfo->symbol_type), syminfo->name);
	}
	else {
		output(treevisitor, "%s\n",
			dmrC_get_type_name(syminfo->symbol_type));
	}
	treevisitor->nesting_level++;
}

static void decrement_nesting(void *data)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	treevisitor->nesting_level--;
}

static void increment_nesting_openbrace(void *data, const char *tag)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "%s {\n", tag);
	treevisitor->nesting_level++;
}

static void decrement_nesting_closebrace(void *data)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	treevisitor->nesting_level--;
	output(treevisitor, "}\n");
}

static void end_symbol_impl(void *data) { decrement_nesting(data); }

static void begin_members_impl(void *data, struct symbol_info *syminfo)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "{\n");
}
static void end_members_impl(void *data)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "}\n");
}
static void begin_arguments_impl(void *data, struct symbol_info *syminfo)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "(\n");
}
static void end_arguments_impl(void *data)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, ")\n");
}

static const char *statement_type_name(enum statement_type type)
{
	static const char *stmt_names[] = {
		[STMT_NONE] = "",[STMT_DECLARATION] = "decl",
		[STMT_EXPRESSION] = "expr",[STMT_COMPOUND] = "",
		[STMT_IF] = "if",[STMT_RETURN] = "ret",
		[STMT_CASE] = "case",[STMT_SWITCH] = "switch",
		[STMT_ITERATOR] = "iter",[STMT_LABEL] = "",
		[STMT_GOTO] = "goto",[STMT_ASM] = "asm",
		[STMT_CONTEXT] = "context",[STMT_RANGE] = "range",
	};
	return stmt_names[type];
}

static void begin_statement_impl(void *data, enum statement_type stmt_type)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	const char *s = statement_type_name(stmt_type);
	if (s && *s)
		output(treevisitor, "%s {\n", s);
	else
		output(treevisitor, "{\n");
	treevisitor->nesting_level++;
}
static void end_statement_impl(void *data)
{
	decrement_nesting_closebrace(data);
}

static void reference_symbol_impl(void *data, uint64_t sym, const char *name)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	if (*name)
		output(treevisitor, "%s sym(%llu)\n", name, sym);
	else
		output(treevisitor, "sym(%llu)\n", sym);
}
static void int_literal_impl(void *data, long long value, int bit_size,
	bool is_unsigned)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "(%d)%lld\n", bit_size, value);
}
static void float_literal_impl(void *data, long double fvalue, int bit_size)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "(%d)%f\n", bit_size, fvalue);
}
static void string_literal_impl(void *data, const char *str)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "%s\n", str);
}
static void begin_assignment_expression_impl(void *data,
	enum expression_type expr_type,
	int op)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	char oper[4] = { 0 };
	const char *opname = dmrC_show_special(treevisitor->C, op);
	if (op < 125)
		oper[0] = (char)op;
	else {
		strcpy(oper, opname);
	}
	output(treevisitor, "%s\t\t\t\t(EXPR_ASSIGNMENT)\n", oper);
	treevisitor->nesting_level++;
}

static void end_assignment_expression_impl(void *data)
{
	decrement_nesting(data);
}

static void begin_binop_expression_impl(void *data,
	enum expression_type expr_type, int op)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	char oper[4] = { 0 };
	const char *opname = dmrC_show_special(treevisitor->C, op);
	if (op < 125)
		oper[0] = (char)op;
	else {
		strcpy(oper, opname);
	}
	output(treevisitor, "%s\t\t\t\t(EXPR_BINOP)\n", oper);
	treevisitor->nesting_level++;
}

static void end_binop_expression_impl(void *data) { decrement_nesting(data); }

static void begin_preop_expression_impl(void *data,
	enum expression_type expr_type, int op)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	char oper[6] = { 0 };
	const char *opname = dmrC_show_special(treevisitor->C, op);
	if (op == '*')
		strcpy(oper, "deref");
	else if (op < 125)
		oper[0] = (char)op;
	else {
		strcpy(oper, opname);
	}
	output(treevisitor, "%s\t\t\t\t(EXPR_PREOP)\n", oper);
	treevisitor->nesting_level++;
}

static void end_preop_expression_impl(void *data) { decrement_nesting(data); }

static void begin_postop_expression_impl(void *data,
	enum expression_type expr_type, int op)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	char oper[6] = { 0 };
	const char *opname = dmrC_show_special(treevisitor->C, op);
	strcpy(oper, opname);
	output(treevisitor, "%s\t\t\t\t(EXPR_POSTOP)\n", oper);
	treevisitor->nesting_level++;
}

static void end_postop_expression_impl(void *data) { decrement_nesting(data); }

static void begin_direct_call_expression_impl(void *data,
	enum expression_type expr_type,
	const char *name)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "direct call %s\n", name);
	treevisitor->nesting_level++;
}
static void begin_indirect_call_expression_impl(void *data,
	enum expression_type expr_type)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "indirect call\n");
	treevisitor->nesting_level++;
}
static void end_call_expression_impl(void *data) { decrement_nesting(data); }

static void begin_callarg_expression_impl(void *data,
	enum expression_type expr_type,
	int argpos)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "arg[%d]\n", argpos);
	treevisitor->nesting_level++;
}
static void end_callarg_expression_impl(void *data) { decrement_nesting(data); }
static void begin_initializer_impl(void *data, struct symbol_info *syminfo)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "=\n");
}
static void begin_cast_expression_impl(void *data,
	enum expression_type expr_type,
	int oldbits, int newbits,
	bool is_unsigned)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "cast %s\n", (is_unsigned ? "[unsigned]" : ""));
	treevisitor->nesting_level++;
}
static void end_cast_expression_impl(void *data) { decrement_nesting(data); }
static void begin_conditional_expression_impl(void *data,
	enum expression_type expr_type)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "select\n");
	treevisitor->nesting_level++;
}
static void end_conditional_expression_impl(void *data)
{
	decrement_nesting(data);
}
static void begin_label_expression_impl(void *data,
	enum expression_type expr_type)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "label\n");
	treevisitor->nesting_level++;
}
static void end_label_expression_impl(void *data) { decrement_nesting(data); }
static void do_expression_identifier_impl(void *data,
	enum expression_type expr_type,
	const char *ident)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "set %s\n", ident);
}
static void do_expression_index_impl(void *data, enum expression_type expr_type,
	unsigned from, unsigned to)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "set %u..%u\n", from, to);
}
static void begin_expression_position_impl(void *data,
	enum expression_type expr_type,
	unsigned init_offset, int bit_offset,
	const char *ident)
{
	struct tree_visitor *treevisitor = (struct tree_visitor *)data;
	output(treevisitor, "set [%d:%d] of %s\n", init_offset, bit_offset,
		ident);
	treevisitor->nesting_level++;
}
static void end_expression_position_impl(void *data)
{
	decrement_nesting(data);
}
static void begin_label_impl(void *data, const char *name)
{
	printf("%s:", name);
}

static void end_label_impl(void *data) {}

static void begin_iterator_prestatement_impl(void *data)
{
	increment_nesting_openbrace(data, "prestatement");
}
static void end_iterator_prestatement_impl(void *data)
{
	decrement_nesting_closebrace(data);
}
static void begin_iterator_precondition_impl(void *data)
{
	increment_nesting_openbrace(data, "precondition");
}
static void end_iterator_precondition_impl(void *data)
{
	decrement_nesting_closebrace(data);
}
static void begin_iterator_statement_impl(void *data)
{
	increment_nesting_openbrace(data, "statement");
}
static void end_iterator_statement_impl(void *data)
{
	decrement_nesting_closebrace(data);
}
static void begin_iterator_postcondition_impl(void *data)
{
	increment_nesting_openbrace(data, "postcondition");
}
static void end_iterator_postcondition_impl(void *data)
{
	decrement_nesting_closebrace(data);
}
static void begin_iterator_poststatement_impl(void *data)
{
	increment_nesting_openbrace(data, "poststatement");
}
static void end_iterator_poststatement_impl(void *data)
{
	decrement_nesting_closebrace(data);
}

static void begin_case_value_impl(void *data, long long value)
{
	char temp[100];
	snprintf(temp, sizeof temp, "on %lld goto", value);
	increment_nesting_openbrace(data, temp);
}
static void begin_case_range_impl(void *data, long long from, long long to)
{
	char temp[100];
	snprintf(temp, sizeof temp, "on %lld ... %lld goto", from, to);
	increment_nesting_openbrace(data, temp);
}
static void begin_default_case_impl(void *data)
{
	increment_nesting_openbrace(data, "by default goto");
}
static void end_case_impl(void *data) { decrement_nesting_closebrace(data); }

static void begin_if_then_impl(void *data)
{
	increment_nesting_openbrace(data, "then");
}
static void end_if_then_impl(void *data) { decrement_nesting_closebrace(data); }
static void begin_if_else_impl(void *data)
{
	increment_nesting_openbrace(data, "else");
}
static void end_if_else_impl(void *data) { decrement_nesting_closebrace(data); }

static void clean_up_symbols(struct dmr_C *C, struct symbol_list *list)
{
	struct symbol *sym;

	FOR_EACH_PTR(list, sym) { dmrC_expand_symbol(C, sym); }
	END_FOR_EACH_PTR(sym);
}

int showparsetree(const char *inputbuffer)
{
	struct symbol_list *list;
	struct string_list *filelist = NULL;
	char *file;

	struct dmr_C *C = new_dmr_C();

	struct tree_visitor treevisitor = { .C = C,.nesting_level = 0 };

	struct symbol_visitor visitor;
	dmrC_init_symbol_visitor(&visitor);

	visitor.data = &treevisitor;
	visitor.begin_symbol = begin_symbol_impl;
	visitor.end_symbol = end_symbol_impl;
	visitor.reference_symbol = reference_symbol_impl;
	visitor.begin_struct_members = begin_members_impl;
	visitor.end_struct_members = end_members_impl;
	visitor.begin_func_arguments = begin_arguments_impl;
	visitor.end_func_arguments = end_arguments_impl;
	visitor.string_literal = string_literal_impl;
	visitor.int_literal = int_literal_impl;
	visitor.float_literal = float_literal_impl;
	visitor.string_literal = string_literal_impl;
	visitor.begin_statement = begin_statement_impl;
	visitor.end_statement = end_statement_impl;
	visitor.begin_assignment_expression = begin_assignment_expression_impl;
	visitor.end_assignment_expression = end_assignment_expression_impl;
	visitor.begin_binop_expression = begin_binop_expression_impl;
	visitor.end_binop_expression = end_binop_expression_impl;
	visitor.begin_preop_expression = begin_preop_expression_impl;
	visitor.end_preop_expression = end_preop_expression_impl;
	visitor.begin_postop_expression = begin_postop_expression_impl;
	visitor.end_postop_expression = end_postop_expression_impl;
	visitor.begin_direct_call_expression =
		begin_direct_call_expression_impl;
	visitor.begin_indirect_call_expression =
		begin_indirect_call_expression_impl;
	visitor.end_call_expression = end_call_expression_impl;
	visitor.begin_callarg_expression = begin_callarg_expression_impl;
	visitor.end_callarg_expression = end_callarg_expression_impl;
	visitor.begin_initializer = begin_initializer_impl;
	visitor.begin_cast_expression = begin_cast_expression_impl;
	visitor.end_cast_expression = end_cast_expression_impl;
	visitor.begin_conditional_expression =
		begin_conditional_expression_impl;
	visitor.end_conditional_expression = end_conditional_expression_impl;
	visitor.begin_label_expression = begin_label_expression_impl;
	visitor.end_label_expression = end_label_expression_impl;
	visitor.do_expression_identifier = do_expression_identifier_impl;
	visitor.do_expression_index = do_expression_index_impl;
	visitor.begin_expression_position = begin_expression_position_impl;
	visitor.end_expression_position = end_expression_position_impl;
	visitor.begin_label = begin_label_impl;
	visitor.end_label = end_label_impl;
	visitor.begin_iterator_prestatement = begin_iterator_prestatement_impl;
	visitor.end_iterator_prestatement = end_iterator_prestatement_impl;
	visitor.begin_iterator_precondition = begin_iterator_precondition_impl;
	visitor.end_iterator_precondition = end_iterator_precondition_impl;
	visitor.begin_iterator_statement = begin_iterator_statement_impl;
	visitor.end_iterator_statement = end_iterator_statement_impl;
	visitor.begin_iterator_postcondition =
		begin_iterator_postcondition_impl;
	visitor.end_iterator_postcondition = end_iterator_postcondition_impl;
	visitor.begin_iterator_poststatement =
		begin_iterator_poststatement_impl;
	visitor.end_iterator_poststatement = end_iterator_poststatement_impl;
	visitor.begin_case_value = begin_case_value_impl;
	visitor.begin_case_range = begin_case_range_impl;
	visitor.begin_default_case = begin_default_case_impl;
	visitor.end_case = end_case_impl;
	visitor.begin_if_then = begin_if_then_impl;
	visitor.end_if_then = end_if_then_impl;
	visitor.begin_if_else = begin_if_else_impl;
	visitor.end_if_else = end_if_else_impl;

	int argc = 0;
	char *argv[] = { NULL };
	char *buffer = strdup(inputbuffer);
	list = dmrC_sparse_initialize(C, argc, argv, &filelist);

	// Simplification
	clean_up_symbols(C, list);
	dmrC_walk_symbol_list(C, list, &visitor);

	list = dmrC_sparse_buffer(C, "buffer", buffer, 0);

	// Simplification
	clean_up_symbols(C, list);

	// Show the end result.
	dmrC_walk_symbol_list(C, list, &visitor);

	destroy_dmr_C(C);
	free(buffer);

	return 0;
}
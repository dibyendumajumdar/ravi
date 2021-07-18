/* Following is an IR Dump from the compiler
define Proc%1
L0 (entry)
	NEWTABLE {T(0)}
	MOV {T(0)} {local(matrix, 0)}
	CLOSURE {Proc%2} {T(1)}
	PUTsk {T(1)} {local(matrix, 0), 'T' Ks(0)}
	CLOSURE {Proc%3} {T(2)}
	PUTsk {T(2)} {local(matrix, 0), 'mul' Ks(1)}
	CLOSURE {Proc%4} {T(3)}
	PUTsk {T(3)} {local(matrix, 0), 'gen' Ks(2)}
	RET {local(matrix, 0)} {L1}
L1 (exit)
define Proc%2
L0 (entry)
	TOTABLE {local(a, 0)}
	LEN {local(a, 0)} {T(0)}
	TGETik {local(a, 0), 1 Kint(0)} {T(1)}
	LEN {T(1)} {T(2)}
	NEWTABLE {T(3)}
	TOINT {T(0)}
	MOVi {T(0)} {Tint(0)}
	TOINT {T(2)}
	MOVi {T(2)} {Tint(1)}
	MOV {T(3)} {local(x, 1)}
	MOV {1 Kint(0)} {Tint(3)}
	MOV {Tint(1)} {Tint(4)}
	MOV {1 Kint(0)} {Tint(5)}
	SUBii {Tint(3), Tint(5)} {Tint(3)}
	BR {L2}
L1 (exit)
L2
	ADDii {Tint(3), Tint(5)} {Tint(3)}
	BR {L3}
L3
	LIii {Tint(4), Tint(3)} {Tbool(6)}
	CBR {Tbool(6)} {L5, L4}
L4
	MOV {Tint(3)} {Tint(2)}
	LOADGLOBAL {Upval(_ENV), 'table' Ks(0)} {T(3)}
	GETsk {T(3), 'numarray' Ks(1)} {T(2)}
	MOV {T(2)} {T(0)}
	CALL {T(0), Tint(1), 0.000000000000 Kflt(0)} {T(0..), 1 Kint(0)}
	TOFARRAY {T(0[0..])}
	MOV {T(0[0..])} {local(xi, 2)}
	MOV {local(xi, 2)} {T(5)}
	TPUTik {T(5)} {local(x, 1), Tint(2)}
	MOV {1 Kint(0)} {Tint(8)}
	MOV {Tint(0)} {Tint(9)}
	MOV {1 Kint(0)} {Tint(10)}
	SUBii {Tint(8), Tint(10)} {Tint(8)}
	BR {L6}
L5
	RET {local(x, 1)} {L1}
L6
	ADDii {Tint(8), Tint(10)} {Tint(8)}
	BR {L7}
L7
	LIii {Tint(9), Tint(8)} {Tbool(11)}
	CBR {Tbool(11)} {L9, L8}
L8
	MOV {Tint(8)} {Tint(7)}
	TGETik {local(a, 0), Tint(7)} {T(5)}
	GETik {T(5), Tint(2)} {T(4)}
	TOFLT {T(4)}
	MOV {T(4)} {Tflt(1)}
	FAPUTfv {Tflt(1)} {local(xi, 2), Tint(7)}
	BR {L6}
L9
	BR {L2}
define Proc%3
L0 (entry)
	TOTABLE {local(a, 0)}
	TOTABLE {local(b, 1)}
	LOADGLOBAL {Upval(_ENV), 'assert' Ks(0)} {T(0)}
	MOV {T(0)} {T(1)}
	TGETik {local(a, 0), 1 Kint(0)} {T(2)}
	LEN {T(2)} {T(3)}
	LEN {local(b, 1)} {T(4)}
	EQ {T(3), T(4)} {T(5)}
	CALL {T(1), T(5)} {T(1..), 1 Kint(0)}
	LEN {local(a, 0)} {T(5)}
	TGETik {local(a, 0), 1 Kint(0)} {T(4)}
	LEN {T(4)} {T(3)}
	TGETik {local(b, 1), 1 Kint(0)} {T(6)}
	LEN {T(6)} {T(7)}
	NEWTABLE {T(8)}
	TOINT {T(5)}
	MOVi {T(5)} {Tint(0)}
	TOINT {T(3)}
	MOVi {T(3)} {Tint(1)}
	TOINT {T(7)}
	MOVi {T(7)} {Tint(2)}
	MOV {T(8)} {local(x, 2)}
	GETsk {Upval(1, Proc%1, matrix), 'T' Ks(1)} {T(8)}
	MOV {T(8)} {T(7)}
	CALL {T(7), local(b, 1)} {T(7..), 1 Kint(0)}
	TOTABLE {T(7[7..])}
	MOV {T(7[7..])} {local(c, 3)}
	MOV {1 Kint(0)} {Tint(4)}
	MOV {Tint(0)} {Tint(5)}
	MOV {1 Kint(0)} {Tint(6)}
	SUBii {Tint(4), Tint(6)} {Tint(4)}
	BR {L2}
L1 (exit)
L2
	ADDii {Tint(4), Tint(6)} {Tint(4)}
	BR {L3}
L3
	LIii {Tint(5), Tint(4)} {Tbool(7)}
	CBR {Tbool(7)} {L5, L4}
L4
	MOV {Tint(4)} {Tint(3)}
	LOADGLOBAL {Upval(_ENV), 'table' Ks(2)} {T(3)}
	GETsk {T(3), 'numarray' Ks(3)} {T(5)}
	MOV {T(5)} {T(9)}
	CALL {T(9), Tint(2), 0.000000000000 Kflt(0)} {T(9..), 1 Kint(0)}
	TOFARRAY {T(9[9..])}
	MOV {T(9[9..])} {local(xi, 4)}
	MOV {local(xi, 4)} {T(11)}
	TPUTik {T(11)} {local(x, 2), Tint(3)}
	MOV {1 Kint(0)} {Tint(9)}
	MOV {Tint(2)} {Tint(10)}
	MOV {1 Kint(0)} {Tint(11)}
	SUBii {Tint(9), Tint(11)} {Tint(9)}
	BR {L6}
L5
	RET {local(x, 2)} {L1}
L6
	ADDii {Tint(9), Tint(11)} {Tint(9)}
	BR {L7}
L7
	LIii {Tint(10), Tint(9)} {Tbool(12)}
	CBR {Tbool(12)} {L9, L8}
L8
	MOV {Tint(9)} {Tint(8)}
	TGETik {local(a, 0), Tint(3)} {T(11)}
	TGETik {local(c, 3), Tint(8)} {T(10)}
	MOVf {0.000000000000 Kflt(0)} {Tflt(0)}
	TOFARRAY {T(11)}
	MOV {T(11)} {local(ai, 5)}
	TOFARRAY {T(10)}
	MOV {T(10)} {local(cj, 6)}
	MOV {1 Kint(0)} {Tint(14)}
	MOV {Tint(1)} {Tint(15)}
	MOV {1 Kint(0)} {Tint(16)}
	SUBii {Tint(14), Tint(16)} {Tint(14)}
	BR {L10}
L9
	BR {L2}
L10
	ADDii {Tint(14), Tint(16)} {Tint(14)}
	BR {L11}
L11
	LIii {Tint(15), Tint(14)} {Tbool(17)}
	CBR {Tbool(17)} {L13, L12}
L12
	MOV {Tint(14)} {Tint(13)}
	FAGETik {local(ai, 5), Tint(13)} {Tflt(1)}
	FAGETik {local(cj, 6), Tint(13)} {Tflt(2)}
	MULff {Tflt(1), Tflt(2)} {Tflt(3)}
	ADDff {Tflt(0), Tflt(3)} {Tflt(2)}
	MOVf {Tflt(2)} {Tflt(0)}
	BR {L10}
L13
	FAPUTfv {Tflt(0)} {local(xi, 4), Tint(8)}
	BR {L6}
define Proc%4
L0 (entry)
	TOINT {local(n, 0)}
	NEWTABLE {T(0)}
	DIVfi {1.000000000000 Kflt(0), local(n, 0)} {Tflt(1)}
	DIVfi {Tflt(1), local(n, 0)} {Tflt(2)}
	MOV {T(0)} {local(a, 1)}
	MOVf {Tflt(2)} {Tflt(0)}
	MOV {1 Kint(0)} {Tint(1)}
	MOV {local(n, 0)} {Tint(2)}
	MOV {1 Kint(0)} {Tint(3)}
	SUBii {Tint(1), Tint(3)} {Tint(1)}
	BR {L2}
L1 (exit)
L2
	ADDii {Tint(1), Tint(3)} {Tint(1)}
	BR {L3}
L3
	LIii {Tint(2), Tint(1)} {Tbool(4)}
	CBR {Tbool(4)} {L5, L4}
L4
	MOV {Tint(1)} {Tint(0)}
	LOADGLOBAL {Upval(_ENV), 'table' Ks(0)} {T(0)}
	GETsk {T(0), 'numarray' Ks(1)} {T(1)}
	MOV {T(1)} {T(2)}
	CALL {T(2), local(n, 0), 0.000000000000 Kflt(1)} {T(2..), 1 Kint(0)}
	TOFARRAY {T(2[2..])}
	MOV {T(2[2..])} {local(ai, 2)}
	MOV {local(ai, 2)} {T(4)}
	TPUTik {T(4)} {local(a, 1), Tint(0)}
	MOV {1 Kint(0)} {Tint(6)}
	MOV {local(n, 0)} {Tint(7)}
	MOV {1 Kint(0)} {Tint(8)}
	SUBii {Tint(6), Tint(8)} {Tint(6)}
	BR {L6}
L5
	RET {local(a, 1)} {L1}
L6
	ADDii {Tint(6), Tint(8)} {Tint(6)}
	BR {L7}
L7
	LIii {Tint(7), Tint(6)} {Tbool(9)}
	CBR {Tbool(9)} {L9, L8}
L8
	MOV {Tint(6)} {Tint(5)}
	SUBii {Tint(0), Tint(5)} {Tint(10)}
	MULfi {Tflt(0), Tint(10)} {Tflt(1)}
	ADDii {Tint(0), Tint(5)} {Tint(10)}
	SUBii {Tint(10), 2 Kint(1)} {Tint(11)}
	MULfi {Tflt(1), Tint(11)} {Tflt(3)}
	FAPUTfv {Tflt(3)} {local(ai, 2), Tint(5)}
	BR {L6}
L9
	BR {L2}

End of IR dump*/
#ifdef __MIRC__
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __INTPTR_TYPE__ intptr_t;
typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __INT32_TYPE__ int32_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT16_TYPE__ int16_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __INT8_TYPE__ int8_t;
typedef __UINT8_TYPE__ uint8_t;
#define NULL ((void *)0)
#define EXPORT
#else
#include <stddef.h>
#include <stdint.h>
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#endif
typedef size_t lu_mem;
typedef unsigned char lu_byte;
typedef uint16_t LuaType;
typedef struct lua_State lua_State;
#define LUA_TNONE		(-1)
#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8
#define LUA_OK  0
typedef enum {TM_INDEX,TM_NEWINDEX,TM_GC,
	TM_MODE,TM_LEN,TM_EQ,TM_ADD,TM_SUB,TM_MUL,
	TM_MOD,TM_POW,TM_DIV,TM_IDIV,TM_BAND,TM_BOR,
	TM_BXOR,TM_SHL,TM_SHR,TM_UNM,TM_BNOT,TM_LT,
	TM_LE,TM_CONCAT,TM_CALL,TM_N
} TMS;
typedef double lua_Number;
typedef int64_t lua_Integer;
typedef uint64_t lua_Unsigned;
typedef int (*lua_CFunction) (lua_State *L);
typedef union {
	lua_Number n;
	double u;
	void *s;
	lua_Integer i;
	long l;
} L_Umaxalign;
#define lua_assert(c)		((void)0)
#define check_exp(c,e)		(e)
#define lua_longassert(c)	((void)0)
#define luai_apicheck(l,e)	lua_assert(e)
#define api_check(l,e,msg)	luai_apicheck(l,(e) && msg)
#define UNUSED(x)	((void)(x))
#define cast(t, exp)	((t)(exp))
#define cast_void(i)	cast(void, (i))
#define cast_byte(i)	cast(lu_byte, (i))
#define cast_num(i)	cast(lua_Number, (i))
#define cast_int(i)	cast(int, (i))
#define cast_uchar(i)	cast(unsigned char, (i))
#define l_castS2U(i)	((lua_Unsigned)(i))
#define l_castU2S(i)	((lua_Integer)(i))
#define l_noret		void
typedef unsigned int Instruction;
#define luai_numidiv(L,a,b)     ((void)L, l_floor(luai_numdiv(L,a,b)))
#define luai_numdiv(L,a,b)      ((a)/(b))
#define luai_nummod(L,a,b,m)  \
  { (m) = l_mathop(fmod)(a,b); if ((m)*(b) < 0) (m) += (b); }
#define LUA_TLCL	(LUA_TFUNCTION | (0 << 4))
#define LUA_TLCF	(LUA_TFUNCTION | (1 << 4))
#define LUA_TCCL	(LUA_TFUNCTION | (2 << 4))
#define RAVI_TFCF	(LUA_TFUNCTION | (4 << 4))
#define LUA_TSHRSTR	(LUA_TSTRING | (0 << 4))
#define LUA_TLNGSTR	(LUA_TSTRING | (1 << 4))
#define LUA_TNUMFLT	(LUA_TNUMBER | (0 << 4))
#define LUA_TNUMINT	(LUA_TNUMBER | (1 << 4))
#define RAVI_TIARRAY (LUA_TTABLE | (1 << 4))
#define RAVI_TFARRAY (LUA_TTABLE | (2 << 4))
#define BIT_ISCOLLECTABLE	(1 << 15)
#define ctb(t)			((t) | BIT_ISCOLLECTABLE)
typedef struct GCObject GCObject;
#define CommonHeader	GCObject *next; lu_byte tt; lu_byte marked
struct GCObject {
  CommonHeader;
};
typedef union Value {
  GCObject *gc;
  void *p;
  int b;
  lua_CFunction f;
  lua_Integer i;
  lua_Number n;
} Value;
#define TValuefields	Value value_; LuaType tt_
typedef struct lua_TValue {
  TValuefields;
} TValue;
#define NILCONSTANT	{NULL}, LUA_TNIL
#define val_(o)		((o)->value_)
#define rttype(o)	((o)->tt_)
#define novariant(x)	((x) & 0x0F)
#define ttype(o)	(rttype(o) & 0x7F)
#define ttnov(o)	(novariant(rttype(o)))
#define checktag(o,t)		(rttype(o) == (t))
#define checktype(o,t)		(ttnov(o) == (t))
#define ttisnumber(o)		checktype((o), LUA_TNUMBER)
#define ttisfloat(o)		checktag((o), LUA_TNUMFLT)
#define ttisinteger(o)		checktag((o), LUA_TNUMINT)
#define ttisnil(o)		checktag((o), LUA_TNIL)
#define ttisboolean(o)		checktag((o), LUA_TBOOLEAN)
#define ttislightuserdata(o)	checktag((o), LUA_TLIGHTUSERDATA)
#define ttisstring(o)		checktype((o), LUA_TSTRING)
#define ttisshrstring(o)	checktag((o), ctb(LUA_TSHRSTR))
#define ttislngstring(o)	checktag((o), ctb(LUA_TLNGSTR))
#define ttistable(o)		checktype((o), LUA_TTABLE)
#define ttisiarray(o)    checktag((o), ctb(RAVI_TIARRAY))
#define ttisfarray(o)    checktag((o), ctb(RAVI_TFARRAY))
#define ttisarray(o)     (ttisiarray(o) || ttisfarray(o))
#define ttisLtable(o)    checktag((o), ctb(LUA_TTABLE))
#define ttisfunction(o)		checktype(o, LUA_TFUNCTION)
#define ttisclosure(o)		((rttype(o) & 0x1F) == LUA_TFUNCTION)
#define ttisCclosure(o)		checktag((o), ctb(LUA_TCCL))
#define ttisLclosure(o)		checktag((o), ctb(LUA_TLCL))
#define ttislcf(o)		checktag((o), LUA_TLCF)
#define ttisfcf(o) (ttype(o) == RAVI_TFCF)
#define ttisfulluserdata(o)	checktag((o), ctb(LUA_TUSERDATA))
#define ttisthread(o)		checktag((o), ctb(LUA_TTHREAD))
#define ttisdeadkey(o)		checktag((o), LUA_TDEADKEY)
#define ivalue(o)	check_exp(ttisinteger(o), val_(o).i)
#define fltvalue(o)	check_exp(ttisfloat(o), val_(o).n)
#define nvalue(o)	check_exp(ttisnumber(o), \
	(ttisinteger(o) ? cast_num(ivalue(o)) : fltvalue(o)))
#define gcvalue(o)	check_exp(iscollectable(o), val_(o).gc)
#define pvalue(o)	check_exp(ttislightuserdata(o), val_(o).p)
#define tsvalue(o)	check_exp(ttisstring(o), gco2ts(val_(o).gc))
#define uvalue(o)	check_exp(ttisfulluserdata(o), gco2u(val_(o).gc))
#define clvalue(o)	check_exp(ttisclosure(o), gco2cl(val_(o).gc))
#define clLvalue(o)	check_exp(ttisLclosure(o), gco2lcl(val_(o).gc))
#define clCvalue(o)	check_exp(ttisCclosure(o), gco2ccl(val_(o).gc))
#define fvalue(o)	check_exp(ttislcf(o), val_(o).f)
#define fcfvalue(o) check_exp(ttisfcf(o), val_(o).p)
#define hvalue(o)	check_exp(ttistable(o), gco2t(val_(o).gc))
#define arrvalue(o) check_exp(ttisarray(o), gco2array(val_(o).gc))
#define arrvalue(o) check_exp(ttisarray(o), gco2array(val_(o).gc))
#define bvalue(o)	check_exp(ttisboolean(o), val_(o).b)
#define thvalue(o)	check_exp(ttisthread(o), gco2th(val_(o).gc))
#define deadvalue(o)	check_exp(ttisdeadkey(o), cast(void *, val_(o).gc))
#define l_isfalse(o)	(ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))
#define iscollectable(o)	(rttype(o) & BIT_ISCOLLECTABLE)
#define righttt(obj)		(ttype(obj) == gcvalue(obj)->tt)
#define checkliveness(L,obj) \
	lua_longassert(!iscollectable(obj) || \
		(righttt(obj) && (L == NULL || !isdead(G(L),gcvalue(obj)))))
#define settt_(o,t)	((o)->tt_=(t))
#define setfltvalue(obj,x) \
  { TValue *io=(obj); val_(io).n=(x); settt_(io, LUA_TNUMFLT); }
#define chgfltvalue(obj,x) \
  { TValue *io=(obj); lua_assert(ttisfloat(io)); val_(io).n=(x); }
#define setivalue(obj,x) \
  { TValue *io=(obj); val_(io).i=(x); settt_(io, LUA_TNUMINT); }
#define chgivalue(obj,x) \
  { TValue *io=(obj); lua_assert(ttisinteger(io)); val_(io).i=(x); }
#define setnilvalue(obj) settt_(obj, LUA_TNIL)
#define setfvalue(obj,x) \
  { TValue *io=(obj); val_(io).f=(x); settt_(io, LUA_TLCF); }
#define setfvalue_fastcall(obj, x, tag) \
{ \
    TValue *io = (obj);   \
    lua_assert(tag >= 1 && tag < 0x80); \
    val_(io).p = (x);     \
    settt_(io, ((tag << 8) | RAVI_TFCF)); \
}
#define setpvalue(obj,x) \
  { TValue *io=(obj); val_(io).p=(x); settt_(io, LUA_TLIGHTUSERDATA); }
#define setbvalue(obj,x) \
  { TValue *io=(obj); val_(io).b=(x); settt_(io, LUA_TBOOLEAN); }
#define setgcovalue(L,obj,x) \
  { TValue *io = (obj); GCObject *i_g=(x); \
    val_(io).gc = i_g; settt_(io, ctb(i_g->tt)); }
#define setsvalue(L,obj,x) \
  { TValue *io = (obj); TString *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(x_->tt)); \
    checkliveness(L,io); }
#define setuvalue(L,obj,x) \
  { TValue *io = (obj); Udata *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TUSERDATA)); \
    checkliveness(L,io); }
#define setthvalue(L,obj,x) \
  { TValue *io = (obj); lua_State *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTHREAD)); \
    checkliveness(L,io); }
#define setclLvalue(L,obj,x) \
  { TValue *io = (obj); LClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TLCL)); \
    checkliveness(L,io); }
#define setclCvalue(L,obj,x) \
  { TValue *io = (obj); CClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TCCL)); \
    checkliveness(L,io); }
#define sethvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTABLE)); \
    checkliveness(L,io); }
#define setiarrayvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(RAVI_TIARRAY)); \
    checkliveness(L,io); }
#define setfarrayvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(RAVI_TFARRAY)); \
    checkliveness(L,io); }
#define setdeadvalue(obj)	settt_(obj, LUA_TDEADKEY)
#define setobj(L,obj1,obj2) \
	{ TValue *io1=(obj1); const TValue *io2=(obj2); io1->tt_ = io2->tt_; val_(io1).n = val_(io2).n; \
	  (void)L; checkliveness(L,io1); }
#define setobjs2s	setobj
#define setobj2s	setobj
#define setsvalue2s	setsvalue
#define sethvalue2s	sethvalue
#define setptvalue2s	setptvalue
#define setobjt2t	setobj
#define setobj2n	setobj
#define setsvalue2n	setsvalue
#define setobj2t	setobj
typedef TValue *StkId;
typedef struct TString {
	CommonHeader;
	lu_byte extra;
	lu_byte shrlen;
	unsigned int hash;
	union {
		size_t lnglen;
		struct TString *hnext;
	} u;
} TString;
typedef union UTString {
	L_Umaxalign dummy;
	TString tsv;
} UTString;
#define getstr(ts)  \
  check_exp(sizeof((ts)->extra), cast(char *, (ts)) + sizeof(UTString))
#define svalue(o)       getstr(tsvalue(o))
#define tsslen(s)	((s)->tt == LUA_TSHRSTR ? (s)->shrlen : (s)->u.lnglen)
#define vslen(o)	tsslen(tsvalue(o))
typedef struct Udata {
	CommonHeader;
	LuaType ttuv_;
	struct Table *metatable;
	size_t len;
	union Value user_;
} Udata;
typedef union UUdata {
	L_Umaxalign dummy;
	Udata uv;
} UUdata;
#define getudatamem(u)  \
  check_exp(sizeof((u)->ttuv_), (cast(char*, (u)) + sizeof(UUdata)))
#define setuservalue(L,u,o) \
	{ const TValue *io=(o); Udata *iu = (u); \
	  iu->user_ = io->value_; iu->ttuv_ = rttype(io); \
	  checkliveness(L,io); }
#define getuservalue(L,u,o) \
	{ TValue *io=(o); const Udata *iu = (u); \
	  io->value_ = iu->user_; settt_(io, iu->ttuv_); \
	  checkliveness(L,io); }
typedef enum {
RAVI_TI_NIL,
RAVI_TI_FALSE,
RAVI_TI_TRUE,
RAVI_TI_INTEGER,
RAVI_TI_FLOAT,
RAVI_TI_INTEGER_ARRAY,
RAVI_TI_FLOAT_ARRAY,
RAVI_TI_TABLE,
RAVI_TI_STRING,
RAVI_TI_FUNCTION,
RAVI_TI_USERDATA,
RAVI_TI_OTHER
} ravi_type_index;
typedef uint32_t ravi_type_map;
#define RAVI_TM_NIL (((ravi_type_map)1)<<RAVI_TI_NIL)
#define RAVI_TM_FALSE (((ravi_type_map)1)<<RAVI_TI_FALSE)
#define RAVI_TM_TRUE (((ravi_type_map)1)<<RAVI_TI_TRUE)
#define RAVI_TM_INTEGER (((ravi_type_map)1)<<RAVI_TI_INTEGER)
#define RAVI_TM_FLOAT (((ravi_type_map)1)<<RAVI_TI_FLOAT)
#define RAVI_TM_INTEGER_ARRAY (((ravi_type_map)1)<<RAVI_TI_INTEGER_ARRAY)
#define RAVI_TM_FLOAT_ARRAY (((ravi_type_map)1)<<RAVI_TI_FLOAT_ARRAY)
#define RAVI_TM_TABLE (((ravi_type_map)1)<<RAVI_TI_TABLE)
#define RAVI_TM_STRING (((ravi_type_map)1)<<RAVI_TI_STRING)
#define RAVI_TM_FUNCTION (((ravi_type_map)1)<<RAVI_TI_FUNCTION)
#define RAVI_TM_USERDATA (((ravi_type_map)1)<<RAVI_TI_USERDATA)
#define RAVI_TM_OTHER (((ravi_type_map)1)<<RAVI_TI_OTHER)
#define RAVI_TM_FALSISH (RAVI_TM_NIL | RAVI_TM_FALSE)
#define RAVI_TM_TRUISH (~RAVI_TM_FALSISH)
#define RAVI_TM_BOOLEAN (RAVI_TM_FALSE | RAVI_TM_TRUE)
#define RAVI_TM_NUMBER (RAVI_TM_INTEGER | RAVI_TM_FLOAT)
#define RAVI_TM_INDEXABLE (RAVI_TM_INTEGER_ARRAY | RAVI_TM_FLOAT_ARRAY | RAVI_TM_TABLE)
#define RAVI_TM_STRING_OR_NIL (RAVI_TM_STRING | RAVI_TM_NIL)
#define RAVI_TM_FUNCTION_OR_NIL (RAVI_TM_FUNCTION | RAVI_TM_NIL)
#define RAVI_TM_BOOLEAN_OR_NIL (RAVI_TM_BOOLEAN | RAVI_TM_NIL)
#define RAVI_TM_USERDATA_OR_NIL (RAVI_TM_USERDATA | RAVI_TM_NIL)
#define RAVI_TM_ANY (~0)
typedef enum {
RAVI_TNIL = RAVI_TM_NIL,           /* NIL */
RAVI_TNUMINT = RAVI_TM_INTEGER,    /* integer number */
RAVI_TNUMFLT = RAVI_TM_FLOAT,        /* floating point number */
RAVI_TNUMBER = RAVI_TM_NUMBER,
RAVI_TARRAYINT = RAVI_TM_INTEGER_ARRAY,      /* array of ints */
RAVI_TARRAYFLT = RAVI_TM_FLOAT_ARRAY,      /* array of doubles */
RAVI_TTABLE = RAVI_TM_TABLE,         /* Lua table */
RAVI_TSTRING = RAVI_TM_STRING_OR_NIL,        /* string */
RAVI_TFUNCTION = RAVI_TM_FUNCTION_OR_NIL,      /* Lua or C Function */
RAVI_TBOOLEAN = RAVI_TM_BOOLEAN_OR_NIL,       /* boolean */
RAVI_TTRUE = RAVI_TM_TRUE,
RAVI_TFALSE = RAVI_TM_FALSE,
RAVI_TUSERDATA = RAVI_TM_USERDATA_OR_NIL,      /* userdata or lightuserdata */
RAVI_TANY = RAVI_TM_ANY,      /* Lua dynamic type */
} ravitype_t;
typedef struct Upvaldesc {
	TString *name;
	TString *usertype;
	ravi_type_map ravi_type;
	lu_byte instack;
	lu_byte idx;
} Upvaldesc;
typedef struct LocVar {
	TString *varname;
	TString *usertype;
	int startpc;
	int endpc;
	ravi_type_map ravi_type;
} LocVar;
typedef enum {
	RAVI_JIT_NOT_COMPILED = 0,
	RAVI_JIT_CANT_COMPILE = 1,
	RAVI_JIT_COMPILED = 2
} ravi_jit_status_t;
typedef enum {
	RAVI_JIT_FLAG_NONE = 0,
	RAVI_JIT_FLAG_HASFORLOOP = 1
} ravi_jit_flag_t;
typedef struct RaviJITProto {
	lu_byte jit_status;
	lu_byte jit_flags;
	unsigned short execution_count;
	void *jit_data;
	lua_CFunction jit_function;
} RaviJITProto;
typedef struct Proto {
	CommonHeader;
	lu_byte numparams;
	lu_byte is_vararg;
	lu_byte maxstacksize;
	int sizeupvalues;
	int sizek;
	int sizecode;
	int sizelineinfo;
	int sizep;
	int sizelocvars;
	int linedefined;
	int lastlinedefined;
	TValue *k;
	Instruction *code;
	struct Proto **p;
	int *lineinfo;
	LocVar *locvars;
	Upvaldesc *upvalues;
	struct LClosure *cache;
	TString  *source;
	GCObject *gclist;
	RaviJITProto ravi_jit;
} Proto;
typedef struct UpVal UpVal;
#define ClosureHeader \
	CommonHeader; lu_byte nupvalues; GCObject *gclist
typedef struct CClosure {
	ClosureHeader;
	lua_CFunction f;
	TValue upvalue[1];
} CClosure;
typedef struct LClosure {
	ClosureHeader;
	struct Proto *p;
	UpVal *upvals[1];
} LClosure;
typedef union Closure {
	CClosure c;
	LClosure l;
} Closure;
#define isLfunction(o)	ttisLclosure(o)
#define getproto(o)	(clLvalue(o)->p)
typedef union TKey {
	struct {
		TValuefields;
		int next;
	} nk;
	TValue tvk;
} TKey;
#define setnodekey(L,key,obj) \
	{ TKey *k_=(key); const TValue *io_=(obj); \
	  k_->nk.value_ = io_->value_; k_->nk.tt_ = io_->tt_; \
	  (void)L; checkliveness(L,io_); }
typedef struct Node {
	TValue i_val;
	TKey i_key;
} Node;
typedef enum RaviArrayModifer {
 RAVI_ARRAY_SLICE = 1,
 RAVI_ARRAY_FIXEDSIZE = 2,
 RAVI_ARRAY_ALLOCATED = 4,
 RAVI_ARRAY_ISFLOAT = 8
} RaviArrayModifier;
enum {
 RAVI_ARRAY_MAX_INLINE = 3,
};
typedef struct RaviArray {
 CommonHeader;
 lu_byte flags;
 unsigned int len;
 unsigned int size;
 union {
  lua_Number numarray[RAVI_ARRAY_MAX_INLINE];
  lua_Integer intarray[RAVI_ARRAY_MAX_INLINE];
  struct RaviArray* parent;
 };
 char *data;
 struct Table *metatable;
} RaviArray;
typedef struct Table {
 CommonHeader;
 lu_byte flags;
 lu_byte lsizenode;
 unsigned int sizearray;
 TValue *array;
 Node *node;
 Node *lastfree;
 struct Table *metatable;
 GCObject *gclist;
 unsigned int hmask;
} Table;
typedef struct Mbuffer {
	char *buffer;
	size_t n;
	size_t buffsize;
} Mbuffer;
typedef struct stringtable {
	TString **hash;
	int nuse;
	int size;
} stringtable;
struct lua_Debug;
typedef intptr_t lua_KContext;
typedef int(*lua_KFunction)(struct lua_State *L, int status, lua_KContext ctx);
typedef void *(*lua_Alloc)(void *ud, void *ptr, size_t osize,
	size_t nsize);
typedef void(*lua_Hook)(struct lua_State *L, struct lua_Debug *ar);
typedef struct CallInfo {
	StkId func;
	StkId	top;
	struct CallInfo *previous, *next;
	union {
		struct {
			StkId base;
			const Instruction *savedpc;
		} l;
		struct {
			lua_KFunction k;
			ptrdiff_t old_errfunc;
			lua_KContext ctx;
		} c;
	} u;
	ptrdiff_t extra;
	short nresults;
	unsigned short callstatus;
	unsigned short stacklevel;
	lu_byte jitstatus;
   lu_byte magic;
} CallInfo;
#define CIST_OAH	(1<<0)
#define CIST_LUA	(1<<1)
#define CIST_HOOKED	(1<<2)
#define CIST_FRESH	(1<<3)
#define CIST_YPCALL	(1<<4)
#define CIST_TAIL	(1<<5)
#define CIST_HOOKYIELD	(1<<6)
#define CIST_LEQ	(1<<7)
#define CIST_FIN	(1<<8)
#define isLua(ci)	((ci)->callstatus & CIST_LUA)
#define isJITed(ci) ((ci)->jitstatus)
#define setoah(st,v)	((st) = ((st) & ~CIST_OAH) | (v))
#define getoah(st)	((st) & CIST_OAH)
typedef struct global_State global_State;
struct lua_State {
	CommonHeader;
	lu_byte status;
	StkId top;
	global_State *l_G;
	CallInfo *ci;
	const Instruction *oldpc;
	StkId stack_last;
	StkId stack;
	UpVal *openupval;
	GCObject *gclist;
	struct lua_State *twups;
	struct lua_longjmp *errorJmp;
	CallInfo base_ci;
	volatile lua_Hook hook;
	ptrdiff_t errfunc;
	int stacksize;
	int basehookcount;
	int hookcount;
	unsigned short nny;
	unsigned short nCcalls;
	lu_byte hookmask;
	lu_byte allowhook;
	unsigned short nci;
   lu_byte magic;
};
#define G(L)	(L->l_G)
union GCUnion {
	GCObject gc;
	struct TString ts;
	struct Udata u;
	union Closure cl;
	struct Table h;
   struct RaviArray arr;
	struct Proto p;
	struct lua_State th;
};
struct UpVal {
	TValue *v;
	lu_mem refcount;
	union {
		struct {
			UpVal *next;
			int touched;
		} open;
		TValue value;
	} u;
};
#define cast_u(o)	cast(union GCUnion *, (o))
#define gco2ts(o)  \
	check_exp(novariant((o)->tt) == LUA_TSTRING, &((cast_u(o))->ts))
#define gco2u(o)  check_exp((o)->tt == LUA_TUSERDATA, &((cast_u(o))->u))
#define gco2lcl(o)  check_exp((o)->tt == LUA_TLCL, &((cast_u(o))->cl.l))
#define gco2ccl(o)  check_exp((o)->tt == LUA_TCCL, &((cast_u(o))->cl.c))
#define gco2cl(o)  \
	check_exp(novariant((o)->tt) == LUA_TFUNCTION, &((cast_u(o))->cl))
#define gco2t(o)  check_exp((o)->tt == LUA_TTABLE, &((cast_u(o))->h))
#define gco2array(o)  check_exp(((o)->tt == RAVI_TIARRAY || (o)->tt == RAVI_TFARRAY), &((cast_u(o))->arr))
#define gco2p(o)  check_exp((o)->tt == LUA_TPROTO, &((cast_u(o))->p))
#define gco2th(o)  check_exp((o)->tt == LUA_TTHREAD, &((cast_u(o))->th))
#define obj2gco(v) \
	check_exp(novariant((v)->tt) < LUA_TDEADKEY, (&(cast_u(v)->gc)))
#define LUA_FLOORN2I		0
#define tonumber(o,n) \
  (ttisfloat(o) ? (*(n) = fltvalue(o), 1) : luaV_tonumber_(o,n))
#define tointeger(o,i) \
  (ttisinteger(o) ? (*(i) = ivalue(o), 1) : luaV_tointeger(o,i,LUA_FLOORN2I))
#define tointegerns(o, i) (ttisinteger(o) ? (*(i) = ivalue(o), 1) : luaV_tointegerns(o, i, LUA_FLOORN2I))
extern int luaV_tonumber_(const TValue *obj, lua_Number *n);
extern int luaV_tointeger(const TValue *obj, lua_Integer *p, int mode);
extern int luaV_tointegerns(const TValue *obj, lua_Integer *p, int mode);
extern void luaF_close (lua_State *L, StkId level);
extern int luaD_poscall (lua_State *L, CallInfo *ci, StkId firstResult, int nres);
extern void luaD_growstack (lua_State *L, int n);
extern int luaV_equalobj(lua_State *L, const TValue *t1, const TValue *t2);
extern int luaV_lessthan(lua_State *L, const TValue *l, const TValue *r);
extern int luaV_lessequal(lua_State *L, const TValue *l, const TValue *r);
extern void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val);
extern void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val);
extern int luaV_execute(lua_State *L);
extern int luaD_precall (lua_State *L, StkId func, int nresults, int op_call);
extern void raviV_op_newtable(lua_State *L, CallInfo *ci, TValue *ra, int b, int c);
extern void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra);
extern void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra);
extern void luaO_arith (lua_State *L, int op, const TValue *p1, const TValue *p2, TValue *res);
extern void raviV_op_setlist(lua_State *L, CallInfo *ci, TValue *ra, int b, int c);
extern void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c);
extern void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int Bx);
extern void raviV_op_vararg(lua_State *L, CallInfo *ci, LClosure *cl, int a, int b);
extern void luaV_objlen (lua_State *L, StkId ra, const TValue *rb);
extern int luaV_forlimit(const TValue *obj, lua_Integer *p, lua_Integer step, int *stopnow);
extern void raviV_op_setupval(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_op_setupvali(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_op_setupvalf(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_op_setupvalai(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_op_setupvalaf(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_op_setupvalt(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_raise_error(lua_State *L, int errorcode);
extern void raviV_raise_error_with_info(lua_State *L, int errorcode, const char *info);
extern void luaD_call (lua_State *L, StkId func, int nResults);
extern void raviH_set_int(lua_State *L, RaviArray *t, lua_Unsigned key, lua_Integer value);
extern void raviH_set_float(lua_State *L, RaviArray *t, lua_Unsigned key, lua_Number value);
extern int raviV_check_usertype(lua_State *L, TString *name, const TValue *o);
extern void luaT_trybinTM (lua_State *L, const TValue *p1, const TValue *p2, TValue *res, TMS event);
extern void raviV_gettable_sskey(lua_State *L, const TValue *t, TValue *key, TValue *val);
extern void raviV_settable_sskey(lua_State *L, const TValue *t, TValue *key, TValue *val);
extern void raviV_gettable_i(lua_State *L, const TValue *t, TValue *key, TValue *val);
extern void raviV_settable_i(lua_State *L, const TValue *t, TValue *key, TValue *val);
extern lua_Integer luaV_shiftl(lua_Integer x, lua_Integer y);
extern void ravi_dump_value(lua_State *L, const struct lua_TValue *v);
extern void raviV_op_bnot(lua_State *L, TValue *ra, TValue *rb);
extern void luaV_concat (lua_State *L, int total);
extern void *luaM_realloc_ (lua_State *L, void *block, size_t osize, size_t nsize);
extern LClosure *luaF_newLclosure (lua_State *L, int n);
extern TString *luaS_newlstr (lua_State *L, const char *str, size_t l);
extern Proto *luaF_newproto (lua_State *L);
extern void luaD_inctop (lua_State *L);
#define luaM_reallocv(L,b,on,n,e) luaM_realloc_(L, (b), (on)*(e), (n)*(e))
#define luaM_newvector(L,n,t) cast(t *, luaM_reallocv(L, NULL, 0, n, sizeof(t)))
#define R(i) (base + i)
#define K(i) (k + i)
#define S(i) (stackbase + i)
#define stackoverflow(L, n) (((int)(L->top - L->stack) + (n) + 5) >= L->stacksize)
#define savestack(L,p)		((char *)(p) - (char *)L->stack)
#define restorestack(L,n)	((TValue *)((char *)L->stack + (n)))
#define tonumberns(o,n) \
	(ttisfloat(o) ? ((n) = fltvalue(o), 1) : \
	(ttisinteger(o) ? ((n) = cast_num(ivalue(o)), 1) : 0))
#define intop(op,v1,v2) l_castU2S(l_castS2U(v1) op l_castS2U(v2))
#define nan (0./0.)
#define inf (1./0.)
#define luai_numunm(L,a)        (-(a))
static int __ravifunc_1(lua_State *L) {
int error_code = 0;
int result = 0;
CallInfo *ci = L->ci;
LClosure *cl = clLvalue(ci->func);
TValue *k = cl->p->k;
StkId base = ci->u.l.base;
TValue ival0; settt_(&ival0, LUA_TNUMINT);
TValue fval0; settt_(&fval0, LUA_TNUMFLT);
TValue bval0; settt_(&bval0, LUA_TBOOLEAN);
TValue ival1; settt_(&ival1, LUA_TNUMINT);
TValue fval1; settt_(&fval1, LUA_TNUMFLT);
TValue bval1; settt_(&bval1, LUA_TBOOLEAN);
TValue ival2; settt_(&ival2, LUA_TNUMINT);
TValue fval2; settt_(&fval2, LUA_TNUMFLT);
TValue bval2; settt_(&bval2, LUA_TBOOLEAN);
TValue nilval; setnilvalue(&nilval);
L0:
// NEWTABLE {T(0)}
{
 TValue *ra = R(1);
 raviV_op_newtable(L, ci, ra, 0, 0);
base = ci->u.l.base;
}
// MOV {T(0)} {local(matrix, 0)}
{
 const TValue *src_reg = R(1);
 TValue *dst_reg = R(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CLOSURE {Proc%2} {T(1)}
raviV_op_closure(L, ci, cl, 2, 0);
base = ci->u.l.base;
// PUTsk {T(1)} {local(matrix, 0), 'T' Ks(0)}
{
 TValue *tab = R(0);
 TValue *key = K(0);
 TValue *src = R(2);
 raviV_settable_sskey(L, tab, key, src);
 base = ci->u.l.base;
}
// CLOSURE {Proc%3} {T(2)}
raviV_op_closure(L, ci, cl, 3, 1);
base = ci->u.l.base;
// PUTsk {T(2)} {local(matrix, 0), 'mul' Ks(1)}
{
 TValue *tab = R(0);
 TValue *key = K(1);
 TValue *src = R(3);
 raviV_settable_sskey(L, tab, key, src);
 base = ci->u.l.base;
}
// CLOSURE {Proc%4} {T(3)}
raviV_op_closure(L, ci, cl, 4, 2);
base = ci->u.l.base;
// PUTsk {T(3)} {local(matrix, 0), 'gen' Ks(2)}
{
 TValue *tab = R(0);
 TValue *key = K(2);
 TValue *src = R(4);
 raviV_settable_sskey(L, tab, key, src);
 base = ci->u.l.base;
}
// RET {local(matrix, 0)} {L1}
luaF_close(L, base);
{
 TValue *stackbase = ci->func;
 int wanted = ci->nresults;
 result = wanted == -1 ? 0 : 1;
 if (wanted == -1) wanted = 1;
 int j = 0;
 if (0 < wanted) {
{
 const TValue *src_reg = R(0);
 TValue *dst_reg = S(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
 }
 j++;
 while (j < wanted) {
  setnilvalue(S(j));
  j++;
 }
 L->top = S(0) + wanted;
 L->ci = ci->previous;
}
goto L1;
L1:
 return result;
Lraise_error:
 raviV_raise_error(L, error_code); /* does not return */
 return result;
}
static int __ravifunc_2(lua_State *L) {
int error_code = 0;
int result = 0;
CallInfo *ci = L->ci;
LClosure *cl = clLvalue(ci->func);
TValue *k = cl->p->k;
StkId base = ci->u.l.base;
lua_Integer i_0 = 0, i_1 = 0, i_2 = 0, i_3 = 0, i_4 = 0, i_5 = 0, i_6 = 0, i_7 = 0, i_8 = 0, i_9 = 0, i_10 = 0, i_11 = 0;
lua_Number f_0 = 0, f_1 = 0;
TValue ival0; settt_(&ival0, LUA_TNUMINT);
TValue fval0; settt_(&fval0, LUA_TNUMFLT);
TValue bval0; settt_(&bval0, LUA_TBOOLEAN);
TValue ival1; settt_(&ival1, LUA_TNUMINT);
TValue fval1; settt_(&fval1, LUA_TNUMFLT);
TValue bval1; settt_(&bval1, LUA_TBOOLEAN);
TValue ival2; settt_(&ival2, LUA_TNUMINT);
TValue fval2; settt_(&fval2, LUA_TNUMFLT);
TValue bval2; settt_(&bval2, LUA_TBOOLEAN);
TValue nilval; setnilvalue(&nilval);
L0:
// TOTABLE {local(a, 0)}
{
 TValue *ra = R(0);
 if (!ttisLtable(ra)) {
  error_code = 4;
  goto Lraise_error;
 }
}
// LEN {local(a, 0)} {T(0)}
{
 TValue *len = R(3);
 TValue *obj = R(0);
 luaV_objlen(L, len, obj);
base = ci->u.l.base;
}
// TGETik {local(a, 0), 1 Kint(0)} {T(1)}
{
 TValue *tab = R(0);
 TValue *key = &ival0; ival0.value_.i = 1;
 TValue *dst = R(4);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// LEN {T(1)} {T(2)}
{
 TValue *len = R(5);
 TValue *obj = R(4);
 luaV_objlen(L, len, obj);
base = ci->u.l.base;
}
// NEWTABLE {T(3)}
{
 TValue *ra = R(6);
 raviV_op_newtable(L, ci, ra, 0, 0);
base = ci->u.l.base;
}
// TOINT {T(0)}
{
 TValue *ra = R(3);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// MOVi {T(0)} {Tint(0)}
{
TValue *reg = R(3);
i_0 = ivalue(reg);
}
// TOINT {T(2)}
{
 TValue *ra = R(5);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// MOVi {T(2)} {Tint(1)}
{
TValue *reg = R(5);
i_1 = ivalue(reg);
}
// MOV {T(3)} {local(x, 1)}
{
 const TValue *src_reg = R(6);
 TValue *dst_reg = R(1);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {1 Kint(0)} {Tint(3)}
i_3 = 1;
// MOV {Tint(1)} {Tint(4)}
i_4 = i_1;
// MOV {1 Kint(0)} {Tint(5)}
i_5 = 1;
// SUBii {Tint(3), Tint(5)} {Tint(3)}
{ i_3 = i_3 - i_5; }
// BR {L2}
goto L2;
L1:
 return result;
Lraise_error:
 raviV_raise_error(L, error_code); /* does not return */
 return result;
L2:
// ADDii {Tint(3), Tint(5)} {Tint(3)}
{ i_3 = i_3 + i_5; }
// BR {L3}
goto L3;
L3:
// LIii {Tint(4), Tint(3)} {Tbool(6)}
{ i_6 = i_4 < i_3; }
// CBR {Tbool(6)} {L5, L4}
{ if (i_6 != 0) goto L5; else goto L4; }
L4:
// MOV {Tint(3)} {Tint(2)}
i_2 = i_3;
// LOADGLOBAL {Upval(_ENV), 'table' Ks(0)} {T(3)}
{
 TValue *tab = cl->upvals[0]->v;
 TValue *key = K(0);
 TValue *dst = R(6);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// GETsk {T(3), 'numarray' Ks(1)} {T(2)}
{
 TValue *tab = R(6);
 TValue *key = K(1);
 TValue *dst = R(5);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOV {T(2)} {T(0)}
{
 const TValue *src_reg = R(5);
 TValue *dst_reg = R(3);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(0), Tint(1), 0.000000000000 Kflt(0)} {T(0..), 1 Kint(0)}
 if (stackoverflow(L,4)) { luaD_growstack(L, 4); base = ci->u.l.base; }
 L->top = R(3) + 3;
{
 TValue *dst_reg = R(5);
 setfltvalue(dst_reg, 0);
}
{
 TValue *dst_reg = R(4);
 setivalue(dst_reg, i_1);
}
{
 TValue *ra = R(3);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TOFARRAY {T(0[0..])}
{
 TValue *ra = R(3);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(0[0..])} {local(xi, 2)}
{
 const TValue *src_reg = R(3);
 TValue *dst_reg = R(2);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {local(xi, 2)} {T(5)}
{
 const TValue *src_reg = R(2);
 TValue *dst_reg = R(8);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// TPUTik {T(5)} {local(x, 1), Tint(2)}
{
 TValue *tab = R(1);
 TValue *key = &ival0; ival0.value_.i = i_2;
 TValue *src = R(8);
 raviV_settable_i(L, tab, key, src);
 base = ci->u.l.base;
}
// MOV {1 Kint(0)} {Tint(8)}
i_8 = 1;
// MOV {Tint(0)} {Tint(9)}
i_9 = i_0;
// MOV {1 Kint(0)} {Tint(10)}
i_10 = 1;
// SUBii {Tint(8), Tint(10)} {Tint(8)}
{ i_8 = i_8 - i_10; }
// BR {L6}
goto L6;
L5:
// RET {local(x, 1)} {L1}
{
 TValue *stackbase = ci->func;
 int wanted = ci->nresults;
 result = wanted == -1 ? 0 : 1;
 if (wanted == -1) wanted = 1;
 int j = 0;
 if (0 < wanted) {
{
 const TValue *src_reg = R(1);
 TValue *dst_reg = S(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
 }
 j++;
 while (j < wanted) {
  setnilvalue(S(j));
  j++;
 }
 L->top = S(0) + wanted;
 L->ci = ci->previous;
}
goto L1;
L6:
// ADDii {Tint(8), Tint(10)} {Tint(8)}
{ i_8 = i_8 + i_10; }
// BR {L7}
goto L7;
L7:
// LIii {Tint(9), Tint(8)} {Tbool(11)}
{ i_11 = i_9 < i_8; }
// CBR {Tbool(11)} {L9, L8}
{ if (i_11 != 0) goto L9; else goto L8; }
L8:
// MOV {Tint(8)} {Tint(7)}
i_7 = i_8;
// TGETik {local(a, 0), Tint(7)} {T(5)}
{
 TValue *tab = R(0);
 TValue *key = &ival0; ival0.value_.i = i_7;
 TValue *dst = R(8);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// GETik {T(5), Tint(2)} {T(4)}
{
 TValue *tab = R(8);
 TValue *key = &ival0; ival0.value_.i = i_2;
 TValue *dst = R(7);
 luaV_gettable(L, tab, key, dst);
 base = ci->u.l.base;
}
// TOFLT {T(4)}
{
 TValue *ra = R(7);
 lua_Number n = 0;
 if (ttisnumber(ra)) { n = (ttisinteger(ra) ? (double) ivalue(ra) : fltvalue(ra)); setfltvalue(ra, n); }
 else {
  error_code = 1;
  goto Lraise_error;
 }
}
// MOV {T(4)} {Tflt(1)}
{
TValue *reg = R(7);
f_1 = fltvalue(reg);
}
// FAPUTfv {Tflt(1)} {local(xi, 2), Tint(7)}
{
 RaviArray *arr = arrvalue(R(2));
 lua_Unsigned ukey = (lua_Unsigned) i_7;
 lua_Number *iptr = (lua_Number *)arr->data;
 if (ukey < (lua_Unsigned)(arr->len)) {
 iptr[ukey] = f_1;
} else {
 raviH_set_float(L, arr, ukey, f_1);
}
}
// BR {L6}
goto L6;
L9:
// BR {L2}
goto L2;
}
static int __ravifunc_3(lua_State *L) {
int error_code = 0;
int result = 0;
CallInfo *ci = L->ci;
LClosure *cl = clLvalue(ci->func);
TValue *k = cl->p->k;
StkId base = ci->u.l.base;
lua_Integer i_0 = 0, i_1 = 0, i_2 = 0, i_3 = 0, i_4 = 0, i_5 = 0, i_6 = 0, i_7 = 0, i_8 = 0, i_9 = 0, i_10 = 0, i_11 = 0, i_12 = 0, i_13 = 0, i_14 = 0, i_15 = 0, i_16 = 0, i_17 = 0;
lua_Number f_0 = 0, f_1 = 0, f_2 = 0, f_3 = 0;
TValue ival0; settt_(&ival0, LUA_TNUMINT);
TValue fval0; settt_(&fval0, LUA_TNUMFLT);
TValue bval0; settt_(&bval0, LUA_TBOOLEAN);
TValue ival1; settt_(&ival1, LUA_TNUMINT);
TValue fval1; settt_(&fval1, LUA_TNUMFLT);
TValue bval1; settt_(&bval1, LUA_TBOOLEAN);
TValue ival2; settt_(&ival2, LUA_TNUMINT);
TValue fval2; settt_(&fval2, LUA_TNUMFLT);
TValue bval2; settt_(&bval2, LUA_TBOOLEAN);
TValue nilval; setnilvalue(&nilval);
L0:
// TOTABLE {local(a, 0)}
{
 TValue *ra = R(0);
 if (!ttisLtable(ra)) {
  error_code = 4;
  goto Lraise_error;
 }
}
// TOTABLE {local(b, 1)}
{
 TValue *ra = R(1);
 if (!ttisLtable(ra)) {
  error_code = 4;
  goto Lraise_error;
 }
}
// LOADGLOBAL {Upval(_ENV), 'assert' Ks(0)} {T(0)}
{
 TValue *tab = cl->upvals[0]->v;
 TValue *key = K(0);
 TValue *dst = R(7);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOV {T(0)} {T(1)}
{
 const TValue *src_reg = R(7);
 TValue *dst_reg = R(8);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// TGETik {local(a, 0), 1 Kint(0)} {T(2)}
{
 TValue *tab = R(0);
 TValue *key = &ival0; ival0.value_.i = 1;
 TValue *dst = R(9);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// LEN {T(2)} {T(3)}
{
 TValue *len = R(10);
 TValue *obj = R(9);
 luaV_objlen(L, len, obj);
base = ci->u.l.base;
}
// LEN {local(b, 1)} {T(4)}
{
 TValue *len = R(11);
 TValue *obj = R(1);
 luaV_objlen(L, len, obj);
base = ci->u.l.base;
}
// EQ {T(3), T(4)} {T(5)}
{
 int result = 0;
 TValue *rb = R(10);
 TValue *rc = R(11);
 if (ttisinteger(rb) && ttisinteger(rc))
  result = (ivalue(rb) == ivalue(rc));
 else {
  result = luaV_equalobj(L, rb, rc);
  base = ci->u.l.base;
 }
 setbvalue(R(12), result != 0);
}
// CALL {T(1), T(5)} {T(1..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(8) + 2;
{
 const TValue *src_reg = R(12);
 TValue *dst_reg = R(9);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(8);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// LEN {local(a, 0)} {T(5)}
{
 TValue *len = R(12);
 TValue *obj = R(0);
 luaV_objlen(L, len, obj);
base = ci->u.l.base;
}
// TGETik {local(a, 0), 1 Kint(0)} {T(4)}
{
 TValue *tab = R(0);
 TValue *key = &ival0; ival0.value_.i = 1;
 TValue *dst = R(11);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// LEN {T(4)} {T(3)}
{
 TValue *len = R(10);
 TValue *obj = R(11);
 luaV_objlen(L, len, obj);
base = ci->u.l.base;
}
// TGETik {local(b, 1), 1 Kint(0)} {T(6)}
{
 TValue *tab = R(1);
 TValue *key = &ival0; ival0.value_.i = 1;
 TValue *dst = R(13);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// LEN {T(6)} {T(7)}
{
 TValue *len = R(14);
 TValue *obj = R(13);
 luaV_objlen(L, len, obj);
base = ci->u.l.base;
}
// NEWTABLE {T(8)}
{
 TValue *ra = R(15);
 raviV_op_newtable(L, ci, ra, 0, 0);
base = ci->u.l.base;
}
// TOINT {T(5)}
{
 TValue *ra = R(12);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// MOVi {T(5)} {Tint(0)}
{
TValue *reg = R(12);
i_0 = ivalue(reg);
}
// TOINT {T(3)}
{
 TValue *ra = R(10);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// MOVi {T(3)} {Tint(1)}
{
TValue *reg = R(10);
i_1 = ivalue(reg);
}
// TOINT {T(7)}
{
 TValue *ra = R(14);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// MOVi {T(7)} {Tint(2)}
{
TValue *reg = R(14);
i_2 = ivalue(reg);
}
// MOV {T(8)} {local(x, 2)}
{
 const TValue *src_reg = R(15);
 TValue *dst_reg = R(2);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// GETsk {Upval(1, Proc%1, matrix), 'T' Ks(1)} {T(8)}
{
 TValue *tab = cl->upvals[1]->v;
 TValue *key = K(1);
 TValue *dst = R(15);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOV {T(8)} {T(7)}
{
 const TValue *src_reg = R(15);
 TValue *dst_reg = R(14);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(7), local(b, 1)} {T(7..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(14) + 2;
{
 const TValue *src_reg = R(1);
 TValue *dst_reg = R(15);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(14);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TOTABLE {T(7[7..])}
{
 TValue *ra = R(14);
 if (!ttisLtable(ra)) {
  error_code = 4;
  goto Lraise_error;
 }
}
// MOV {T(7[7..])} {local(c, 3)}
{
 const TValue *src_reg = R(14);
 TValue *dst_reg = R(3);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {1 Kint(0)} {Tint(4)}
i_4 = 1;
// MOV {Tint(0)} {Tint(5)}
i_5 = i_0;
// MOV {1 Kint(0)} {Tint(6)}
i_6 = 1;
// SUBii {Tint(4), Tint(6)} {Tint(4)}
{ i_4 = i_4 - i_6; }
// BR {L2}
goto L2;
L1:
 return result;
Lraise_error:
 raviV_raise_error(L, error_code); /* does not return */
 return result;
L2:
// ADDii {Tint(4), Tint(6)} {Tint(4)}
{ i_4 = i_4 + i_6; }
// BR {L3}
goto L3;
L3:
// LIii {Tint(5), Tint(4)} {Tbool(7)}
{ i_7 = i_5 < i_4; }
// CBR {Tbool(7)} {L5, L4}
{ if (i_7 != 0) goto L5; else goto L4; }
L4:
// MOV {Tint(4)} {Tint(3)}
i_3 = i_4;
// LOADGLOBAL {Upval(_ENV), 'table' Ks(2)} {T(3)}
{
 TValue *tab = cl->upvals[0]->v;
 TValue *key = K(2);
 TValue *dst = R(10);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// GETsk {T(3), 'numarray' Ks(3)} {T(5)}
{
 TValue *tab = R(10);
 TValue *key = K(3);
 TValue *dst = R(12);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOV {T(5)} {T(9)}
{
 const TValue *src_reg = R(12);
 TValue *dst_reg = R(16);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(9), Tint(2), 0.000000000000 Kflt(0)} {T(9..), 1 Kint(0)}
 if (stackoverflow(L,4)) { luaD_growstack(L, 4); base = ci->u.l.base; }
 L->top = R(16) + 3;
{
 TValue *dst_reg = R(18);
 setfltvalue(dst_reg, 0);
}
{
 TValue *dst_reg = R(17);
 setivalue(dst_reg, i_2);
}
{
 TValue *ra = R(16);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TOFARRAY {T(9[9..])}
{
 TValue *ra = R(16);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(9[9..])} {local(xi, 4)}
{
 const TValue *src_reg = R(16);
 TValue *dst_reg = R(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {local(xi, 4)} {T(11)}
{
 const TValue *src_reg = R(4);
 TValue *dst_reg = R(18);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// TPUTik {T(11)} {local(x, 2), Tint(3)}
{
 TValue *tab = R(2);
 TValue *key = &ival0; ival0.value_.i = i_3;
 TValue *src = R(18);
 raviV_settable_i(L, tab, key, src);
 base = ci->u.l.base;
}
// MOV {1 Kint(0)} {Tint(9)}
i_9 = 1;
// MOV {Tint(2)} {Tint(10)}
i_10 = i_2;
// MOV {1 Kint(0)} {Tint(11)}
i_11 = 1;
// SUBii {Tint(9), Tint(11)} {Tint(9)}
{ i_9 = i_9 - i_11; }
// BR {L6}
goto L6;
L5:
// RET {local(x, 2)} {L1}
{
 TValue *stackbase = ci->func;
 int wanted = ci->nresults;
 result = wanted == -1 ? 0 : 1;
 if (wanted == -1) wanted = 1;
 int j = 0;
 if (0 < wanted) {
{
 const TValue *src_reg = R(2);
 TValue *dst_reg = S(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
 }
 j++;
 while (j < wanted) {
  setnilvalue(S(j));
  j++;
 }
 L->top = S(0) + wanted;
 L->ci = ci->previous;
}
goto L1;
L6:
// ADDii {Tint(9), Tint(11)} {Tint(9)}
{ i_9 = i_9 + i_11; }
// BR {L7}
goto L7;
L7:
// LIii {Tint(10), Tint(9)} {Tbool(12)}
{ i_12 = i_10 < i_9; }
// CBR {Tbool(12)} {L9, L8}
{ if (i_12 != 0) goto L9; else goto L8; }
L8:
// MOV {Tint(9)} {Tint(8)}
i_8 = i_9;
// TGETik {local(a, 0), Tint(3)} {T(11)}
{
 TValue *tab = R(0);
 TValue *key = &ival0; ival0.value_.i = i_3;
 TValue *dst = R(18);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// TGETik {local(c, 3), Tint(8)} {T(10)}
{
 TValue *tab = R(3);
 TValue *key = &ival0; ival0.value_.i = i_8;
 TValue *dst = R(17);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOVf {0.000000000000 Kflt(0)} {Tflt(0)}
f_0 = 0;
// TOFARRAY {T(11)}
{
 TValue *ra = R(18);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(11)} {local(ai, 5)}
{
 const TValue *src_reg = R(18);
 TValue *dst_reg = R(5);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// TOFARRAY {T(10)}
{
 TValue *ra = R(17);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(10)} {local(cj, 6)}
{
 const TValue *src_reg = R(17);
 TValue *dst_reg = R(6);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {1 Kint(0)} {Tint(14)}
i_14 = 1;
// MOV {Tint(1)} {Tint(15)}
i_15 = i_1;
// MOV {1 Kint(0)} {Tint(16)}
i_16 = 1;
// SUBii {Tint(14), Tint(16)} {Tint(14)}
{ i_14 = i_14 - i_16; }
// BR {L10}
goto L10;
L9:
// BR {L2}
goto L2;
L10:
// ADDii {Tint(14), Tint(16)} {Tint(14)}
{ i_14 = i_14 + i_16; }
// BR {L11}
goto L11;
L11:
// LIii {Tint(15), Tint(14)} {Tbool(17)}
{ i_17 = i_15 < i_14; }
// CBR {Tbool(17)} {L13, L12}
{ if (i_17 != 0) goto L13; else goto L12; }
L12:
// MOV {Tint(14)} {Tint(13)}
i_13 = i_14;
// FAGETik {local(ai, 5), Tint(13)} {Tflt(1)}
{
 RaviArray *arr = arrvalue(R(5));
 lua_Unsigned ukey = (lua_Unsigned) i_13;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_1 = iptr[ukey];
}
// FAGETik {local(cj, 6), Tint(13)} {Tflt(2)}
{
 RaviArray *arr = arrvalue(R(6));
 lua_Unsigned ukey = (lua_Unsigned) i_13;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_2 = iptr[ukey];
}
// MULff {Tflt(1), Tflt(2)} {Tflt(3)}
{ f_3 = f_1 * f_2; }
// ADDff {Tflt(0), Tflt(3)} {Tflt(2)}
{ f_2 = f_0 + f_3; }
// MOVf {Tflt(2)} {Tflt(0)}
f_0 = f_2;
// BR {L10}
goto L10;
L13:
// FAPUTfv {Tflt(0)} {local(xi, 4), Tint(8)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_8;
 lua_Number *iptr = (lua_Number *)arr->data;
 if (ukey < (lua_Unsigned)(arr->len)) {
 iptr[ukey] = f_0;
} else {
 raviH_set_float(L, arr, ukey, f_0);
}
}
// BR {L6}
goto L6;
}
static int __ravifunc_4(lua_State *L) {
int error_code = 0;
int result = 0;
CallInfo *ci = L->ci;
LClosure *cl = clLvalue(ci->func);
TValue *k = cl->p->k;
StkId base = ci->u.l.base;
lua_Integer i_0 = 0, i_1 = 0, i_2 = 0, i_3 = 0, i_4 = 0, i_5 = 0, i_6 = 0, i_7 = 0, i_8 = 0, i_9 = 0, i_10 = 0, i_11 = 0;
lua_Number f_0 = 0, f_1 = 0, f_2 = 0, f_3 = 0;
TValue ival0; settt_(&ival0, LUA_TNUMINT);
TValue fval0; settt_(&fval0, LUA_TNUMFLT);
TValue bval0; settt_(&bval0, LUA_TBOOLEAN);
TValue ival1; settt_(&ival1, LUA_TNUMINT);
TValue fval1; settt_(&fval1, LUA_TNUMFLT);
TValue bval1; settt_(&bval1, LUA_TBOOLEAN);
TValue ival2; settt_(&ival2, LUA_TNUMINT);
TValue fval2; settt_(&fval2, LUA_TNUMFLT);
TValue bval2; settt_(&bval2, LUA_TBOOLEAN);
TValue nilval; setnilvalue(&nilval);
L0:
// TOINT {local(n, 0)}
{
 TValue *ra = R(0);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// NEWTABLE {T(0)}
{
 TValue *ra = R(3);
 raviV_op_newtable(L, ci, ra, 0, 0);
base = ci->u.l.base;
}
// DIVfi {1.000000000000 Kflt(0), local(n, 0)} {Tflt(1)}
{ f_1 = 1 / ((lua_Number)(ivalue(R(0)))); }
// DIVfi {Tflt(1), local(n, 0)} {Tflt(2)}
{ f_2 = f_1 / ((lua_Number)(ivalue(R(0)))); }
// MOV {T(0)} {local(a, 1)}
{
 const TValue *src_reg = R(3);
 TValue *dst_reg = R(1);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOVf {Tflt(2)} {Tflt(0)}
f_0 = f_2;
// MOV {1 Kint(0)} {Tint(1)}
i_1 = 1;
// MOV {local(n, 0)} {Tint(2)}
{
TValue *reg = R(0);
i_2 = ivalue(reg);
}
// MOV {1 Kint(0)} {Tint(3)}
i_3 = 1;
// SUBii {Tint(1), Tint(3)} {Tint(1)}
{ i_1 = i_1 - i_3; }
// BR {L2}
goto L2;
L1:
 return result;
Lraise_error:
 raviV_raise_error(L, error_code); /* does not return */
 return result;
L2:
// ADDii {Tint(1), Tint(3)} {Tint(1)}
{ i_1 = i_1 + i_3; }
// BR {L3}
goto L3;
L3:
// LIii {Tint(2), Tint(1)} {Tbool(4)}
{ i_4 = i_2 < i_1; }
// CBR {Tbool(4)} {L5, L4}
{ if (i_4 != 0) goto L5; else goto L4; }
L4:
// MOV {Tint(1)} {Tint(0)}
i_0 = i_1;
// LOADGLOBAL {Upval(_ENV), 'table' Ks(0)} {T(0)}
{
 TValue *tab = cl->upvals[0]->v;
 TValue *key = K(0);
 TValue *dst = R(3);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// GETsk {T(0), 'numarray' Ks(1)} {T(1)}
{
 TValue *tab = R(3);
 TValue *key = K(1);
 TValue *dst = R(4);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOV {T(1)} {T(2)}
{
 const TValue *src_reg = R(4);
 TValue *dst_reg = R(5);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(2), local(n, 0), 0.000000000000 Kflt(1)} {T(2..), 1 Kint(0)}
 if (stackoverflow(L,4)) { luaD_growstack(L, 4); base = ci->u.l.base; }
 L->top = R(5) + 3;
{
 TValue *dst_reg = R(7);
 setfltvalue(dst_reg, 0);
}
{
 const TValue *src_reg = R(0);
 TValue *dst_reg = R(6);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(5);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TOFARRAY {T(2[2..])}
{
 TValue *ra = R(5);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(2[2..])} {local(ai, 2)}
{
 const TValue *src_reg = R(5);
 TValue *dst_reg = R(2);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {local(ai, 2)} {T(4)}
{
 const TValue *src_reg = R(2);
 TValue *dst_reg = R(7);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// TPUTik {T(4)} {local(a, 1), Tint(0)}
{
 TValue *tab = R(1);
 TValue *key = &ival0; ival0.value_.i = i_0;
 TValue *src = R(7);
 raviV_settable_i(L, tab, key, src);
 base = ci->u.l.base;
}
// MOV {1 Kint(0)} {Tint(6)}
i_6 = 1;
// MOV {local(n, 0)} {Tint(7)}
{
TValue *reg = R(0);
i_7 = ivalue(reg);
}
// MOV {1 Kint(0)} {Tint(8)}
i_8 = 1;
// SUBii {Tint(6), Tint(8)} {Tint(6)}
{ i_6 = i_6 - i_8; }
// BR {L6}
goto L6;
L5:
// RET {local(a, 1)} {L1}
{
 TValue *stackbase = ci->func;
 int wanted = ci->nresults;
 result = wanted == -1 ? 0 : 1;
 if (wanted == -1) wanted = 1;
 int j = 0;
 if (0 < wanted) {
{
 const TValue *src_reg = R(1);
 TValue *dst_reg = S(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
 }
 j++;
 while (j < wanted) {
  setnilvalue(S(j));
  j++;
 }
 L->top = S(0) + wanted;
 L->ci = ci->previous;
}
goto L1;
L6:
// ADDii {Tint(6), Tint(8)} {Tint(6)}
{ i_6 = i_6 + i_8; }
// BR {L7}
goto L7;
L7:
// LIii {Tint(7), Tint(6)} {Tbool(9)}
{ i_9 = i_7 < i_6; }
// CBR {Tbool(9)} {L9, L8}
{ if (i_9 != 0) goto L9; else goto L8; }
L8:
// MOV {Tint(6)} {Tint(5)}
i_5 = i_6;
// SUBii {Tint(0), Tint(5)} {Tint(10)}
{ i_10 = i_0 - i_5; }
// MULfi {Tflt(0), Tint(10)} {Tflt(1)}
{ f_1 = f_0 * ((lua_Number)(i_10)); }
// ADDii {Tint(0), Tint(5)} {Tint(10)}
{ i_10 = i_0 + i_5; }
// SUBii {Tint(10), 2 Kint(1)} {Tint(11)}
{ i_11 = i_10 - 2; }
// MULfi {Tflt(1), Tint(11)} {Tflt(3)}
{ f_3 = f_1 * ((lua_Number)(i_11)); }
// FAPUTfv {Tflt(3)} {local(ai, 2), Tint(5)}
{
 RaviArray *arr = arrvalue(R(2));
 lua_Unsigned ukey = (lua_Unsigned) i_5;
 lua_Number *iptr = (lua_Number *)arr->data;
 if (ukey < (lua_Unsigned)(arr->len)) {
 iptr[ukey] = f_3;
} else {
 raviH_set_float(L, arr, ukey, f_3);
}
}
// BR {L6}
goto L6;
L9:
// BR {L2}
goto L2;
}
EXPORT LClosure *mymain(lua_State *L) {
 LClosure *cl = luaF_newLclosure(L, 1);
 setclLvalue(L, L->top, cl);
 luaD_inctop(L);
 cl->p = luaF_newproto(L);
 Proto *f = cl->p;
 f->ravi_jit.jit_function = __ravifunc_1;
 f->ravi_jit.jit_status = RAVI_JIT_COMPILED;
 f->numparams = 0;
 f->is_vararg = 0;
 f->maxstacksize = 5;
 f->k = luaM_newvector(L, 3, TValue);
 f->sizek = 3;
 for (int i = 0; i < 3; i++)
  setnilvalue(&f->k[i]);
 {
  TValue *o = &f->k[1];
  setsvalue2n(L, o, luaS_newlstr(L, "mul", 3));
 }
 {
  TValue *o = &f->k[2];
  setsvalue2n(L, o, luaS_newlstr(L, "gen", 3));
 }
 {
  TValue *o = &f->k[0];
  setsvalue2n(L, o, luaS_newlstr(L, "T", 1));
 }
 f->upvalues = luaM_newvector(L, 1, Upvaldesc);
 f->sizeupvalues = 1;
 f->upvalues[0].instack = 1;
 f->upvalues[0].idx = 0;
 f->upvalues[0].name = NULL; // _ENV
 f->upvalues[0].usertype = NULL;
 f->upvalues[0].ravi_type = 128;
 f->p = luaM_newvector(L, 3, Proto *);
 f->sizep = 3;
 for (int i = 0; i < 3; i++)
   f->p[i] = NULL;
 f->p[0] = luaF_newproto(L);
{ 
 Proto *parent = f; f = f->p[0];
 f->ravi_jit.jit_function = __ravifunc_2;
 f->ravi_jit.jit_status = RAVI_JIT_COMPILED;
 f->numparams = 1;
 f->is_vararg = 0;
 f->maxstacksize = 9;
 f->k = luaM_newvector(L, 2, TValue);
 f->sizek = 2;
 for (int i = 0; i < 2; i++)
  setnilvalue(&f->k[i]);
 {
  TValue *o = &f->k[1];
  setsvalue2n(L, o, luaS_newlstr(L, "numarray", 8));
 }
 {
  TValue *o = &f->k[0];
  setsvalue2n(L, o, luaS_newlstr(L, "table", 5));
 }
 f->upvalues = luaM_newvector(L, 1, Upvaldesc);
 f->sizeupvalues = 1;
 f->upvalues[0].instack = 0;
 f->upvalues[0].idx = 0;
 f->upvalues[0].name = NULL; // _ENV
 f->upvalues[0].usertype = NULL;
 f->upvalues[0].ravi_type = 128;
 f = parent;
}
 f->p[1] = luaF_newproto(L);
{ 
 Proto *parent = f; f = f->p[1];
 f->ravi_jit.jit_function = __ravifunc_3;
 f->ravi_jit.jit_status = RAVI_JIT_COMPILED;
 f->numparams = 2;
 f->is_vararg = 0;
 f->maxstacksize = 19;
 f->k = luaM_newvector(L, 4, TValue);
 f->sizek = 4;
 for (int i = 0; i < 4; i++)
  setnilvalue(&f->k[i]);
 {
  TValue *o = &f->k[0];
  setsvalue2n(L, o, luaS_newlstr(L, "assert", 6));
 }
 {
  TValue *o = &f->k[3];
  setsvalue2n(L, o, luaS_newlstr(L, "numarray", 8));
 }
 {
  TValue *o = &f->k[1];
  setsvalue2n(L, o, luaS_newlstr(L, "T", 1));
 }
 {
  TValue *o = &f->k[2];
  setsvalue2n(L, o, luaS_newlstr(L, "table", 5));
 }
 f->upvalues = luaM_newvector(L, 2, Upvaldesc);
 f->sizeupvalues = 2;
 f->upvalues[0].instack = 0;
 f->upvalues[0].idx = 0;
 f->upvalues[0].name = NULL; // _ENV
 f->upvalues[0].usertype = NULL;
 f->upvalues[0].ravi_type = 128;
 f->upvalues[1].instack = 1;
 f->upvalues[1].idx = 0;
 f->upvalues[1].name = NULL; // matrix
 f->upvalues[1].usertype = NULL;
 f->upvalues[1].ravi_type = -1;
 f = parent;
}
 f->p[2] = luaF_newproto(L);
{ 
 Proto *parent = f; f = f->p[2];
 f->ravi_jit.jit_function = __ravifunc_4;
 f->ravi_jit.jit_status = RAVI_JIT_COMPILED;
 f->numparams = 1;
 f->is_vararg = 0;
 f->maxstacksize = 8;
 f->k = luaM_newvector(L, 2, TValue);
 f->sizek = 2;
 for (int i = 0; i < 2; i++)
  setnilvalue(&f->k[i]);
 {
  TValue *o = &f->k[0];
  setsvalue2n(L, o, luaS_newlstr(L, "table", 5));
 }
 {
  TValue *o = &f->k[1];
  setsvalue2n(L, o, luaS_newlstr(L, "numarray", 8));
 }
 f->upvalues = luaM_newvector(L, 1, Upvaldesc);
 f->sizeupvalues = 1;
 f->upvalues[0].instack = 0;
 f->upvalues[0].idx = 0;
 f->upvalues[0].name = NULL; // _ENV
 f->upvalues[0].usertype = NULL;
 f->upvalues[0].ravi_type = 128;
 f = parent;
}
 return cl;
}

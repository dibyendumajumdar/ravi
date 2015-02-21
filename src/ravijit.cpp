/**
* Copyright (c) Dibyendu Majumdar
* 2015
*/

#include "ravijit.h"
#include "ravillvm.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO we probably do not need all the headers
// below

#define lvm_c
#define LUA_CORE

#include "lprefix.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"

#ifdef __cplusplus
}
#endif

#include <array>
#include <iterator>
#include <type_traits>

namespace ravi {

// TODO - should probably be an atomic int
static volatile int init = 0;

// All Lua types are gathered here
struct LuaLLVMTypes {

  LuaLLVMTypes(llvm::LLVMContext &context);
  void dump();

  llvm::Type *C_intptr_t;
  llvm::Type *C_size_t;
  llvm::Type *C_ptrdiff_t;

  llvm::Type *lua_NumberT;
  llvm::Type *lua_IntegerT;
  llvm::Type *lua_UnsignedT;
  llvm::Type *lua_KContextT;

  llvm::FunctionType *lua_CFunctionT;
  llvm::PointerType *plua_CFunctionT;

  llvm::FunctionType *lua_KFunctionT;
  llvm::PointerType *plua_KFunctionT;

  llvm::FunctionType *lua_HookT;
  llvm::PointerType *plua_HookT;

  llvm::FunctionType *lua_AllocT;
  llvm::PointerType *plua_AllocT;

  llvm::Type *l_memT;
  llvm::Type *lu_memT;

  llvm::Type *lu_byteT;
  llvm::Type *L_UmaxalignT;
  llvm::Type *C_pcharT;

  llvm::Type *C_intT;

  llvm::StructType *lua_StateT;
  llvm::PointerType *plua_StateT;

  llvm::StructType *global_StateT;
  llvm::PointerType *pglobal_StateT;

  llvm::StructType *ravi_StateT;
  llvm::PointerType *pravi_StateT;

  llvm::StructType *GCObjectT;
  llvm::PointerType *pGCObjectT;

  llvm::StructType *ValueT;
  llvm::StructType *TValueT;
  llvm::PointerType *pTValueT;

  llvm::StructType *TStringT;
  llvm::PointerType *pTStringT;
  llvm::PointerType *ppTStringT;

  llvm::StructType *UdataT;
  llvm::StructType *TableT;
  llvm::PointerType *pTableT;

  llvm::StructType *UpvaldescT;
  llvm::PointerType *pUpvaldescT;

  llvm::Type *ravitype_tT;
  llvm::StructType *LocVarT;
  llvm::PointerType *pLocVarT;

  llvm::Type *InstructionT;
  llvm::PointerType *pInstructionT;
  llvm::StructType *LClosureT;
  llvm::PointerType *pLClosureT;
  llvm::PointerType *ppLClosureT;
  llvm::PointerType *pppLClosureT;

  llvm::StructType *RaviJITProtoT;
  llvm::PointerType *pRaviJITProtoT;

  llvm::StructType *ProtoT;
  llvm::PointerType *pProtoT;
  llvm::PointerType *ppProtoT;

  llvm::StructType *UpValT;
  llvm::PointerType *pUpValT;

  llvm::StructType *CClosureT;
  llvm::PointerType *pCClosureT;

  llvm::StructType *TKeyT;
  llvm::PointerType *pTKeyT;

  llvm::StructType *NodeT;
  llvm::PointerType *pNodeT;

  llvm::StructType *lua_DebugT;
  llvm::PointerType *plua_DebugT;

  llvm::StructType *lua_longjumpT;
  llvm::PointerType *plua_longjumpT;

  llvm::StructType *MbufferT;
  llvm::StructType *stringtableT;

  llvm::PointerType *StkIdT;

  llvm::StructType *CallInfoT;
  llvm::StructType *CallInfo_cT;
  llvm::StructType *CallInfo_lT;
  llvm::PointerType *pCallInfoT;

  llvm::FunctionType *jitFunctionT;

  llvm::FunctionType *luaD_poscallT;
  llvm::FunctionType *luaF_closeT;
  llvm::FunctionType *luaV_equalobjT;

  std::array<llvm::Constant *, 21> kInt;

  llvm::Constant *kFalse;
};

LuaLLVMTypes::LuaLLVMTypes(llvm::LLVMContext &context) {

  static_assert(std::is_floating_point<lua_Number>::value &&
                    sizeof(lua_Number) == sizeof(double),
                "lua_Number is not a double");
  lua_NumberT = llvm::Type::getDoubleTy(context);

  static_assert(std::is_integral<lua_Integer>::value,
                "lua_Integer is not an integer type");
  lua_IntegerT = llvm::Type::getIntNTy(context, sizeof(lua_Integer) * 8);

  static_assert(sizeof(lua_Integer) == sizeof(lua_Unsigned),
                "lua_Integer and lua_Unsigned are of different size");
  lua_UnsignedT = lua_IntegerT;

  C_intptr_t = llvm::Type::getIntNTy(context, sizeof(intptr_t) * 8);
  C_size_t = llvm::Type::getIntNTy(context, sizeof(size_t) * 8);
  C_ptrdiff_t = llvm::Type::getIntNTy(context, sizeof(ptrdiff_t) * 8);
  C_intT = llvm::Type::getIntNTy(context, sizeof(int) * 8);

  static_assert(sizeof(size_t) == sizeof(lu_mem),
                "lu_mem size is not same as size_t");
  lu_memT = C_size_t;

  static_assert(sizeof(ptrdiff_t) == sizeof(l_mem),
                "l_mem size is not same as ptrdiff_t");
  l_memT = C_ptrdiff_t;

  static_assert(sizeof(L_Umaxalign) == sizeof(double),
                "L_Umaxalign is not same size as double");
  L_UmaxalignT = llvm::Type::getDoubleTy(context);

  lu_byteT = llvm::Type::getInt8Ty(context);
  C_pcharT = llvm::Type::getInt8PtrTy(context);

  InstructionT = C_intT;
  pInstructionT = llvm::PointerType::get(InstructionT, 0);

  lua_StateT = llvm::StructType::create(context, "ravi.lua_State");
  plua_StateT = llvm::PointerType::get(lua_StateT, 0);

  lua_KContextT = C_ptrdiff_t;

  std::vector<llvm::Type *> elements;
  elements.push_back(plua_StateT);
  lua_CFunctionT = llvm::FunctionType::get(C_intT, elements, false);
  plua_CFunctionT = llvm::PointerType::get(lua_CFunctionT, 0);

  jitFunctionT = lua_CFunctionT;

  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(C_intT);
  elements.push_back(lua_KContextT);
  lua_KFunctionT = llvm::FunctionType::get(C_intT, elements, false);
  plua_KFunctionT = llvm::PointerType::get(lua_KFunctionT, 0);

  elements.clear();
  elements.push_back(llvm::Type::getInt8PtrTy(context));
  elements.push_back(llvm::Type::getInt8PtrTy(context));
  elements.push_back(C_size_t);
  elements.push_back(C_size_t);
  lua_AllocT = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context),
                                       elements, false);
  plua_AllocT = llvm::PointerType::get(lua_AllocT, 0);

  lua_DebugT = llvm::StructType::create(context, "ravi.lua_Debug");
  plua_DebugT = llvm::PointerType::get(lua_DebugT, 0);

  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(plua_DebugT);
  lua_HookT = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context),
                                      elements, false);
  plua_HookT = llvm::PointerType::get(lua_HookT, 0);

  // struct GCObject {
  //   GCObject *next;
  //   lu_byte tt;
  //   lu_byte marked
  // };
  GCObjectT = llvm::StructType::create(context, "ravi.GCObject");
  pGCObjectT = llvm::PointerType::get(GCObjectT, 0);
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  GCObjectT->setBody(elements);

  static_assert(sizeof(Value) == sizeof(lua_Number),
                "Value type is larger than lua_Number");
  // In LLVM unions should be set to the largest member
  // So in the case of a Value this is the double type
  // union Value {
  //   GCObject *gc;    /* collectable objects */
  //   void *p;         /* light userdata */
  //   int b;           /* booleans */
  //   lua_CFunction f; /* light C functions */
  //   lua_Integer i;   /* integer numbers */
  //   lua_Number n;    /* float numbers */
  // };
  ValueT = llvm::StructType::create(context, "ravi.Value");
  elements.clear();
  elements.push_back(lua_NumberT);
  ValueT->setBody(elements);

  // struct TValue {
  //   union Value value_;
  //   int tt_;
  // };
  TValueT = llvm::StructType::create(context, "ravi.TValue");
  elements.clear();
  elements.push_back(ValueT);
  elements.push_back(C_intT);
  TValueT->setBody(elements);
  pTValueT = llvm::PointerType::get(TValueT, 0);

  StkIdT = pTValueT;

  ///*
  //** Header for string value; string bytes follow the end of this structure
  //** (aligned according to 'UTString'; see next).
  //*/
  // typedef struct TString {
  //   GCObject *next;
  //   lu_byte tt;
  //   lu_byte marked
  //   lu_byte extra;  /* reserved words for short strings; "has hash" for longs
  //   */
  //   unsigned int hash;
  //   size_t len;  /* number of characters in string */
  //   struct TString *hnext;  /* linked list for hash table */
  // } TString;

  ///*
  //** Ensures that address after this type is always fully aligned.
  //*/
  // typedef union UTString {
  //  L_Umaxalign dummy;  /* ensures maximum alignment for strings */
  //  TString tsv;
  //} UTString;
  TStringT = llvm::StructType::create(context, "ravi.TString");
  pTStringT = llvm::PointerType::get(TStringT, 0);
  ppTStringT = llvm::PointerType::get(pTStringT, 0);
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);  /* extra */
  elements.push_back(C_intT);    /* hash */
  elements.push_back(C_size_t);  /* len */
  elements.push_back(pTStringT); /* hnext */
  TStringT->setBody(elements);

  // Table
  TableT = llvm::StructType::create(context, "ravi.Table");
  pTableT = llvm::PointerType::get(TableT, 0);

  ///*
  //** Header for userdata; memory area follows the end of this structure
  //** (aligned according to 'UUdata'; see next).
  //*/
  // typedef struct Udata {
  //  GCObject *next;
  //  lu_byte tt;
  //  lu_byte marked
  //  lu_byte ttuv_;  /* user value's tag */
  //  struct Table *metatable;
  //  size_t len;  /* number of bytes */
  //  union Value user_;  /* user value */
  //} Udata;
  UdataT = llvm::StructType::create(context, "ravi.Udata");
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT); /* ttuv_ */
  elements.push_back(pTableT);  /* metatable */
  elements.push_back(C_size_t); /* len */
  elements.push_back(ValueT);   /* user_ */
  UdataT->setBody(elements);

  ///*
  //** Description of an upvalue for function prototypes
  //*/
  // typedef struct Upvaldesc {
  //  TString *name;  /* upvalue name (for debug information) */
  //  lu_byte instack;  /* whether it is in stack */
  //  lu_byte idx;  /* index of upvalue (in stack or in outer function's list)
  //  */
  //}Upvaldesc;
  UpvaldescT = llvm::StructType::create(context, "ravi.Upvaldesc");
  elements.clear();
  elements.push_back(pTStringT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  UpvaldescT->setBody(elements);
  pUpvaldescT = llvm::PointerType::get(UpvaldescT, 0);

  ///*
  //** Description of a local variable for function prototypes
  //** (used for debug information)
  //*/
  // typedef struct LocVar {
  //  TString *varname;
  //  int startpc;  /* first point where variable is active */
  //  int endpc;    /* first point where variable is dead */
  //  ravitype_t ravi_type; /* RAVI type of the variable - RAVI_TANY if unknown
  //  */
  //} LocVar;
  ravitype_tT = llvm::Type::getIntNTy(context, sizeof(ravitype_t) * 8);
  LocVarT = llvm::StructType::create(context, "ravi.LocVar");
  elements.clear();
  elements.push_back(pTStringT);   /* varname */
  elements.push_back(C_intT);      /* startpc */
  elements.push_back(C_intT);      /* endpc */
  elements.push_back(ravitype_tT); /* ravi_type */
  LocVarT->setBody(elements);
  pLocVarT = llvm::PointerType::get(LocVarT, 0);

  LClosureT = llvm::StructType::create(context, "ravi.LClosure");
  pLClosureT = llvm::PointerType::get(LClosureT, 0);
  ppLClosureT = llvm::PointerType::get(pLClosureT, 0);
  pppLClosureT = llvm::PointerType::get(ppLClosureT, 0);

  RaviJITProtoT = llvm::StructType::create(context, "ravi.RaviJITProto");
  pRaviJITProtoT = llvm::PointerType::get(RaviJITProtoT, 0);

  ///*
  //** Function Prototypes
  //*/
  // typedef struct Proto {
  //  CommonHeader;
  //  lu_byte numparams;  /* number of fixed parameters */
  //  lu_byte is_vararg;
  //  lu_byte maxstacksize;  /* maximum stack used by this function */
  //  int sizeupvalues;  /* size of 'upvalues' */
  //  int sizek;  /* size of 'k' */
  //  int sizecode;
  //  int sizelineinfo;
  //  int sizep;  /* size of 'p' */
  //  int sizelocvars;
  //  int linedefined;
  //  int lastlinedefined;
  //  TValue *k;  /* constants used by the function */
  //  Instruction *code;
  //  struct Proto **p;  /* functions defined inside the function */
  //  int *lineinfo;  /* map from opcodes to source lines (debug information) */
  //  LocVar *locvars;  /* information about local variables (debug information)
  //  */
  //  Upvaldesc *upvalues;  /* upvalue information */
  //  struct LClosure *cache;  /* last created closure with this prototype */
  //  TString  *source;  /* used for debug information */
  //  GCObject *gclist;
  //  /* RAVI */
  //  RaviJITProto *ravi_jit;
  //} Proto;

  ProtoT = llvm::StructType::create(context, "ravi.Proto");
  pProtoT = llvm::PointerType::get(ProtoT, 0);
  ppProtoT = llvm::PointerType::get(pProtoT, 0);
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);                          /* numparams */
  elements.push_back(lu_byteT);                          /* is_vararg */
  elements.push_back(lu_byteT);                          /* maxstacksize */
  elements.push_back(C_intT);                            /* sizeupvalues */
  elements.push_back(C_intT);                            /* sizek */
  elements.push_back(C_intT);                            /* sizecode */
  elements.push_back(C_intT);                            /* sizelineinfo */
  elements.push_back(C_intT);                            /* sizep */
  elements.push_back(C_intT);                            /* sizelocvars */
  elements.push_back(C_intT);                            /* linedefined */
  elements.push_back(C_intT);                            /* lastlinedefined */
  elements.push_back(pTValueT);                          /* k */
  elements.push_back(pInstructionT);                     /* code */
  elements.push_back(ppProtoT);                          /* p */
  elements.push_back(llvm::PointerType::get(C_intT, 0)); /* lineinfo */
  elements.push_back(pLocVarT);                          /* locvars */
  elements.push_back(pUpvaldescT);                       /* upvalues */
  elements.push_back(pLClosureT);                        /* cache */
  elements.push_back(pTStringT);                         /* source */
  elements.push_back(pGCObjectT);                        /* gclist */
  elements.push_back(pRaviJITProtoT);                    /* ravi_jit */
  ProtoT->setBody(elements);

  ///*
  //** Lua Upvalues
  //*/
  // typedef struct UpVal UpVal;
  UpValT = llvm::StructType::create(context, "ravi.UpVal");
  pUpValT = llvm::PointerType::get(UpValT, 0);

  ///*
  //** Closures
  //*/

  //#define ClosureHeader \
	//CommonHeader; lu_byte nupvalues; GCObject *gclist

  // typedef struct CClosure {
  //  ClosureHeader;
  //  lua_CFunction f;
  //  TValue upvalue[1];  /* list of upvalues */
  //} CClosure;

  CClosureT = llvm::StructType::create(context, "ravi.CClosure");
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);        /* nupvalues */
  elements.push_back(pGCObjectT);      /* gclist */
  elements.push_back(plua_CFunctionT); /* f */
  elements.push_back(llvm::ArrayType::get(TValueT, 1));
  CClosureT->setBody(elements);
  pCClosureT = llvm::PointerType::get(CClosureT, 0);

  // typedef struct LClosure {
  //  ClosureHeader;
  //  struct Proto *p;
  //  UpVal *upvals[1];  /* list of upvalues */
  //} LClosure;
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);   /* nupvalues */
  elements.push_back(pGCObjectT); /* gclist */
  elements.push_back(pProtoT);    /* p */
  elements.push_back(llvm::ArrayType::get(pUpValT, 1));
  LClosureT->setBody(elements);

  ///*
  //** Tables
  //*/

  // typedef union TKey {
  //  struct {
  //    TValuefields;
  //    int next;  /* for chaining (offset for next node) */
  //  } nk;
  //  TValue tvk;
  //} TKey;
  TKeyT = llvm::StructType::create(context, "ravi.TKey");
  elements.clear();
  elements.push_back(ValueT);
  elements.push_back(C_intT);
  elements.push_back(C_intT); /* next */
  TKeyT->setBody(elements);
  pTKeyT = llvm::PointerType::get(TKeyT, 0);

  // typedef struct Node {
  // TValue i_val;
  // TKey i_key;
  //} Node;
  NodeT = llvm::StructType::create(context, "ravi.Node");
  elements.clear();
  elements.push_back(TValueT); /* i_val */
  elements.push_back(TKeyT);   /* i_key */
  NodeT->setBody(elements);
  pNodeT = llvm::PointerType::get(NodeT, 0);

  // typedef struct Table {
  //  CommonHeader;
  //  lu_byte flags;  /* 1<<p means tagmethod(p) is not present */
  //  lu_byte lsizenode;  /* log2 of size of 'node' array */
  //  unsigned int sizearray;  /* size of 'array' array */
  //  TValue *array;  /* array part */
  //  Node *node;
  //  Node *lastfree;  /* any free position is before this position */
  //  struct Table *metatable;
  //  GCObject *gclist;
  //  ravitype_t ravi_array_type; /* RAVI specialization */
  //  unsigned int ravi_array_len; /* RAVI len specialization */
  //} Table;
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);    /* flags */
  elements.push_back(lu_byteT);    /* lsizenode */
  elements.push_back(C_intT);      /* sizearray */
  elements.push_back(pTValueT);    /* array part */
  elements.push_back(pNodeT);      /* node */
  elements.push_back(pNodeT);      /* lastfree */
  elements.push_back(pTableT);     /* metatable */
  elements.push_back(pGCObjectT);  /* gclist */
  elements.push_back(ravitype_tT); /* ravi_array_type */
  elements.push_back(C_intT);      /* ravi_array_len */
  TableT->setBody(elements);

  // struct lua_longjmp;  /* defined in ldo.c */
  lua_longjumpT = llvm::StructType::create(context, "ravi.lua_longjmp");
  plua_longjumpT = llvm::PointerType::get(lua_longjumpT, 0);

  // lzio.h
  // typedef struct Mbuffer {
  //  char *buffer;
  //  size_t n;
  //  size_t buffsize;
  //} Mbuffer;
  MbufferT = llvm::StructType::create(context, "ravi.Mbuffer");
  elements.clear();
  elements.push_back(llvm::Type::getInt8PtrTy(context)); /* buffer */
  elements.push_back(C_size_t);                          /* n */
  elements.push_back(C_size_t);                          /* buffsize */
  MbufferT->setBody(elements);

  // typedef struct stringtable {
  //  TString **hash;
  //  int nuse;  /* number of elements */
  //  int size;
  //} stringtable;
  stringtableT = llvm::StructType::create(context, "ravi.stringtable");
  elements.clear();
  elements.push_back(ppTStringT); /* hash */
  elements.push_back(C_intT);     /* nuse */
  elements.push_back(C_intT);     /* size */
  stringtableT->setBody(elements);

  ///*
  //** Information about a call.
  //** When a thread yields, 'func' is adjusted to pretend that the
  //** top function has only the yielded values in its stack; in that
  //** case, the actual 'func' value is saved in field 'extra'.
  //** When a function calls another with a continuation, 'extra' keeps
  //** the function index so that, in case of errors, the continuation
  //** function can be called with the correct top.
  //*/
  // typedef struct CallInfo {
  //  StkId func;  /* function index in the stack */
  //  StkId	top;  /* top for this function */
  //  struct CallInfo *previous, *next;  /* dynamic call link */
  //  union {
  //    struct {  /* only for Lua functions */
  //      StkId base;  /* base for this function */
  //      const Instruction *savedpc;
  //    } l;
  //    struct {  /* only for C functions */
  //      lua_KFunction k;  /* continuation in case of yields */
  //      ptrdiff_t old_errfunc;
  //      lua_KContext ctx;  /* context info. in case of yields */
  //    } c;
  //  } u;
  //  ptrdiff_t extra;
  //  short nresults;  /* expected number of results from this function */
  //  lu_byte callstatus;
  //} CallInfo;

  elements.clear();
  elements.push_back(StkIdT);        /* base */
  elements.push_back(pInstructionT); /* savedpc */
  elements.push_back(
      C_ptrdiff_t); /* dummy to make this same size as the other member */
  CallInfo_lT = llvm::StructType::create(elements);

  elements.clear();
  elements.push_back(plua_KFunctionT); /* k */
  elements.push_back(C_ptrdiff_t);     /* old_errfunc */
  elements.push_back(lua_KContextT);   /* ctx */
  CallInfo_cT = llvm::StructType::create(elements);

  CallInfoT = llvm::StructType::create(context, "ravi.CallInfo");
  pCallInfoT = llvm::PointerType::get(CallInfoT, 0);
  elements.clear();
  elements.push_back(StkIdT);     /* func */
  elements.push_back(StkIdT);     /* top */
  elements.push_back(pCallInfoT); /* previous */
  elements.push_back(pCallInfoT); /* next */
  elements.push_back(
      CallInfo_lT); /* u.l  - as we will typically access the lua call details
                       */
  elements.push_back(C_ptrdiff_t);                     /* extra */
  elements.push_back(llvm::Type::getInt16Ty(context)); /* nresults */
  elements.push_back(lu_byteT);                        /* callstatus */
  CallInfoT->setBody(elements);

  // typedef struct ravi_State ravi_State;

  ravi_StateT = llvm::StructType::create(context, "ravi.ravi_State");
  pravi_StateT = llvm::PointerType::get(ravi_StateT, 0);

  ///*
  //** 'global state', shared by all threads of this state
  //*/
  // typedef struct global_State {
  //  lua_Alloc frealloc;  /* function to reallocate memory */
  //  void *ud;         /* auxiliary data to 'frealloc' */
  //  lu_mem totalbytes;  /* number of bytes currently allocated - GCdebt */
  //  l_mem GCdebt;  /* bytes allocated not yet compensated by the collector */
  //  lu_mem GCmemtrav;  /* memory traversed by the GC */
  //  lu_mem GCestimate;  /* an estimate of the non-garbage memory in use */
  //  stringtable strt;  /* hash table for strings */
  //  TValue l_registry;
  //  unsigned int seed;  /* randomized seed for hashes */
  //  lu_byte currentwhite;
  //  lu_byte gcstate;  /* state of garbage collector */
  //  lu_byte gckind;  /* kind of GC running */
  //  lu_byte gcrunning;  /* true if GC is running */
  //  GCObject *allgc;  /* list of all collectable objects */
  //  GCObject **sweepgc;  /* current position of sweep in list */
  //  GCObject *finobj;  /* list of collectable objects with finalizers */
  //  GCObject *gray;  /* list of gray objects */
  //  GCObject *grayagain;  /* list of objects to be traversed atomically */
  //  GCObject *weak;  /* list of tables with weak values */
  //  GCObject *ephemeron;  /* list of ephemeron tables (weak keys) */
  //  GCObject *allweak;  /* list of all-weak tables */
  //  GCObject *tobefnz;  /* list of userdata to be GC */
  //  GCObject *fixedgc;  /* list of objects not to be collected */
  //  struct lua_State *twups;  /* list of threads with open upvalues */
  //  Mbuffer buff;  /* temporary buffer for string concatenation */
  //  unsigned int gcfinnum;  /* number of finalizers to call in each GC step */
  //  int gcpause;  /* size of pause between successive GCs */
  //  int gcstepmul;  /* GC 'granularity' */
  //  lua_CFunction panic;  /* to be called in unprotected errors */
  //  struct lua_State *mainthread;
  //  const lua_Number *version;  /* pointer to version number */
  //  TString *memerrmsg;  /* memory-error message */
  //  TString *tmname[TM_N];  /* array with tag-method names */
  //  struct Table *mt[LUA_NUMTAGS];  /* metatables for basic types */
  //  /* RAVI */
  //  ravi_State *ravi_state;
  //} global_State;

  global_StateT = llvm::StructType::create(context, "ravi.global_State");
  pglobal_StateT = llvm::PointerType::get(global_StateT, 0);

  ///*
  //** 'per thread' state
  //*/
  // struct lua_State {
  //  CommonHeader;
  //  lu_byte status;
  //  StkId top;  /* first free slot in the stack */
  //  global_State *l_G;
  //  CallInfo *ci;  /* call info for current function */
  //  const Instruction *oldpc;  /* last pc traced */
  //  StkId stack_last;  /* last free slot in the stack */
  //  StkId stack;  /* stack base */
  //  UpVal *openupval;  /* list of open upvalues in this stack */
  //  GCObject *gclist;
  //  struct lua_State *twups;  /* list of threads with open upvalues */
  //  struct lua_longjmp *errorJmp;  /* current error recover point */
  //  CallInfo base_ci;  /* CallInfo for first level (C calling Lua) */
  //  lua_Hook hook;
  //  ptrdiff_t errfunc;  /* current error handling function (stack index) */
  //  int stacksize;
  //  int basehookcount;
  //  int hookcount;
  //  unsigned short nny;  /* number of non-yieldable calls in stack */
  //  unsigned short nCcalls;  /* number of nested C calls */
  //  lu_byte hookmask;
  //  lu_byte allowhook;
  //};
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);                        /* status */
  elements.push_back(StkIdT);                          /* top */
  elements.push_back(pglobal_StateT);                  /* l_G */
  elements.push_back(pCallInfoT);                      /* ci */
  elements.push_back(pInstructionT);                   /* oldpc */
  elements.push_back(StkIdT);                          /* stack_last */
  elements.push_back(StkIdT);                          /* stack */
  elements.push_back(pUpValT);                         /* openupval */
  elements.push_back(pGCObjectT);                      /* gclist */
  elements.push_back(plua_StateT);                     /* twups */
  elements.push_back(plua_longjumpT);                  /* errorJmp */
  elements.push_back(CallInfoT);                       /* base_ci */
  elements.push_back(plua_HookT);                      /* hook */
  elements.push_back(C_ptrdiff_t);                     /* errfunc */
  elements.push_back(C_intT);                          /* stacksize */
  elements.push_back(C_intT);                          /* basehookcount */
  elements.push_back(C_intT);                          /* hookcount */
  elements.push_back(llvm::Type::getInt16Ty(context)); /* nny */
  elements.push_back(llvm::Type::getInt16Ty(context)); /* nCcalls */
  elements.push_back(lu_byteT);                        /* hookmask */
  elements.push_back(lu_byteT);                        /* allowhook */
  lua_StateT->setBody(elements);

  // int luaD_poscall (lua_State *L, StkId firstResult)
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(StkIdT);
  luaD_poscallT = llvm::FunctionType::get(C_intT, elements, false);

  // void luaF_close (lua_State *L, StkId level)
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(StkIdT);
  luaF_closeT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // int luaV_equalobj (lua_State *L, const TValue *t1, const TValue *t2)
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pTValueT);
  elements.push_back(pTValueT);
  luaV_equalobjT = llvm::FunctionType::get(C_intT, elements, false);

  for (int j = 0; j < kInt.size(); j++)
    kInt[j] = llvm::ConstantInt::get(C_intT, j);

  kFalse = llvm::ConstantInt::getFalse(llvm::Type::getInt1Ty(context));
}

void LuaLLVMTypes::dump() {
  GCObjectT->dump();
  fputs("\n", stdout);
  TValueT->dump();
  fputs("\n", stdout);
  TStringT->dump();
  fputs("\n", stdout);
  UdataT->dump();
  fputs("\n", stdout);
  UpvaldescT->dump();
  fputs("\n", stdout);
  LocVarT->dump();
  fputs("\n", stdout);
  ProtoT->dump();
  fputs("\n", stdout);
  CClosureT->dump();
  fputs("\n", stdout);
  LClosureT->dump();
  fputs("\n", stdout);
  TKeyT->dump();
  fputs("\n", stdout);
  NodeT->dump();
  fputs("\n", stdout);
  TableT->dump();
  fputs("\n", stdout);
  MbufferT->dump();
  fputs("\n", stdout);
  stringtableT->dump();
  fputs("\n", stdout);
  CallInfoT->dump();
  fputs("\n", stdout);
  lua_StateT->dump();
  fputs("\n", stdout);
}

class RAVI_API RaviJITStateImpl;

// Represents a JITed or JITable function
// Each function gets its own module and execution engine - this
// may change in future
// The advantage is that we can drop the function when the corresponding
// Lua object is garbage collected - with MCJIT this is not possible
// to do at function level
class RAVI_API RaviJITFunctionImpl : public RaviJITFunction {

  // The function is tracked by RaviJITState so we need to
  // tell RaviJITState when this function dies
  RaviJITStateImpl *owner_;

  // Module within which the function will be defined
  llvm::Module *module_;

  // Unique name for the function
  std::string name_;

  // The execution engine responsible for compiling the
  // module
  llvm::ExecutionEngine *engine_;

  // The llvm Function definition
  llvm::Function *function_;

  // Pointer to compiled function - this is only set when
  // the function
  void *ptr_;

public:
  RaviJITFunctionImpl(RaviJITStateImpl *owner, llvm::Module *module,
                      llvm::FunctionType *type,
                      llvm::GlobalValue::LinkageTypes linkage,
                      const std::string &name);
  virtual ~RaviJITFunctionImpl();

  // Compile the function if not already compiled and
  // return pointer to function
  virtual void *compile();

  // Add declaration for an extern function that is not
  // loaded dynamically - i.e., is part of the the executable
  // and therefore not visible at runtime by name
  virtual llvm::Constant *addExternFunction(llvm::FunctionType *type,
                                            void *address,
                                            const std::string &name);

  virtual const std::string &name() const { return name_; }
  virtual llvm::Function *function() const { return function_; }
  virtual llvm::Module *module() const { return module_; }
  virtual llvm::ExecutionEngine *engine() const { return engine_; }
  virtual RaviJITState *owner() const;
  virtual void dump();
};

// Ravi's JIT State
// All of the JIT information is held here
class RAVI_API RaviJITStateImpl : public RaviJITState {

  // map of names to functions
  std::map<std::string, RaviJITFunction *> functions_;

  llvm::LLVMContext &context_;

  // The triple represents the host target
  std::string triple_;

  // Lua type definitions
  LuaLLVMTypes *types_;

public:
  RaviJITStateImpl();
  virtual ~RaviJITStateImpl();

  // Create a function of specified type and linkage
  virtual RaviJITFunction *
  createFunction(llvm::FunctionType *type,
                 llvm::GlobalValue::LinkageTypes linkage,
                 const std::string &name);

  // Stop tracking the named function - note that
  // the function is assumed to be destroyed by the user
  void deleteFunction(const std::string &name);

  void addGlobalSymbol(const std::string &name, void *address);

  virtual void dump();
  virtual llvm::LLVMContext &context() { return context_; }
  LuaLLVMTypes *types() const { return types_; }
};

RaviJITState *RaviJITFunctionImpl::owner() const { return owner_; }

RaviJITStateImpl::RaviJITStateImpl() : context_(llvm::getGlobalContext()) {
  // Unless following three lines are not executed then
  // ExecutionEngine cannot be created
  // This should ideally be an atomic check but because LLVM docs
  // say that it is okay to call these functions more than once we
  // do not bother
  if (init == 0) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    init++;
  }
  triple_ = llvm::sys::getProcessTriple();
#ifdef _WIN32
  // On Windows we get compilation error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set -elf as the object
  // format
  triple_ += "-elf";
#endif
  types_ = new LuaLLVMTypes(context_);
}

RaviJITStateImpl::~RaviJITStateImpl() {
  std::vector<RaviJITFunction *> todelete;
  for (auto &f = std::begin(functions_); f != std::end(functions_); f++) {
    todelete.push_back(f->second);
  }
  // delete all the compiled objects
  for (int i = 0; i < todelete.size(); i++) {
    delete todelete[i];
  }
  delete types_;
}

void RaviJITStateImpl::addGlobalSymbol(const std::string &name, void *address) {
  llvm::sys::DynamicLibrary::AddSymbol(name, address);
}

void RaviJITStateImpl::dump() {
  types_->dump();
  for (auto f : functions_) {
    f.second->dump();
  }
}

RaviJITFunction *
RaviJITStateImpl::createFunction(llvm::FunctionType *type,
                                 llvm::GlobalValue::LinkageTypes linkage,
                                 const std::string &name) {
  // MCJIT treats each module as a compilation unit
  // To enable function level life cycle we create a
  // module per function
  std::string moduleName = "ravi_module_" + name;
  llvm::Module *module = new llvm::Module(moduleName, context_);
#if defined(_WIN32)
  // On Windows we get error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set
  // -elf as the object format
  module->setTargetTriple(triple_);
#endif
  RaviJITFunction *f =
      new RaviJITFunctionImpl(this, module, type, linkage, name);
  functions_[name] = f;
  return f;
}

void RaviJITStateImpl::deleteFunction(const std::string &name) {
  functions_.erase(name);
  // This is called when RaviJITFunction is deleted
}

RaviJITFunctionImpl::RaviJITFunctionImpl(
    RaviJITStateImpl *owner, llvm::Module *module, llvm::FunctionType *type,
    llvm::GlobalValue::LinkageTypes linkage, const std::string &name)
    : owner_(owner), module_(module), name_(name), engine_(nullptr),
      function_(nullptr), ptr_(nullptr) {
  function_ = llvm::Function::Create(type, linkage, name, module);
  std::string errStr;
  engine_ = llvm::EngineBuilder(module)
                .setEngineKind(llvm::EngineKind::JIT)
                .setUseMCJIT(true)
                .setErrorStr(&errStr)
                .create();
  if (!engine_) {
    fprintf(stderr, "Could not create ExecutionEngine: %s\n", errStr.c_str());
    return;
  }
}

RaviJITFunctionImpl::~RaviJITFunctionImpl() {
  // Remove this function from parent
  owner_->deleteFunction(name_);
  if (engine_)
    delete engine_;
  else if (module_)
    delete module_;
  // Check - we assume here that deleting engine deletes the module
  // and function, and deleting module deletes function
}

void *RaviJITFunctionImpl::compile() {

// TODO add optimize steps
#if 0
  // Create a function pass manager for this engine
  llvm::FunctionPassManager *FPM = new llvm::FunctionPassManager(OpenModule);

  // Set up the optimizer pipeline.  Start with registering info about how the
  // target lays out data structures.
  OpenModule->setDataLayout(NewEngine->getDataLayout());
  FPM->add(new llvm::DataLayoutPass());
  // Provide basic AliasAnalysis support for GVN.
  FPM->add(llvm::createBasicAliasAnalysisPass());
  // Promote allocas to registers.
  FPM->add(llvm::createPromoteMemoryToRegisterPass());
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  FPM->add(llvm::createInstructionCombiningPass());
  // Reassociate expressions.
  FPM->add(llvm::createReassociatePass());
  // Eliminate Common SubExpressions.
  FPM->add(llvm::createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  FPM->add(llvm::createCFGSimplificationPass());
  FPM->doInitialization();

  // For each function in the module
  llvm::Module::iterator it;
  llvm::Module::iterator end = OpenModule->end();
  for (it = OpenModule->begin(); it != end; ++it) {
    // Run the FPM on this function
    FPM->run(*it);
  }

  // We don't need this anymore
  delete FPM;
#endif

  if (ptr_)
    return ptr_;
  if (!function_ || !engine_)
    return NULL;

  // Upon creation, MCJIT holds a pointer to the Module object
  // that it received from EngineBuilder but it does not immediately
  // generate code for this module. Code generation is deferred
  // until either the MCJIT::finalizeObject method is called
  // explicitly or a function such as MCJIT::getPointerToFunction
  // is called which requires the code to have been generated.
  engine_->finalizeObject();
  ptr_ = engine_->getPointerToFunction(function_);
  return ptr_;
}

llvm::Constant *
RaviJITFunctionImpl::addExternFunction(llvm::FunctionType *type, void *address,
                                       const std::string &name) {
  llvm::Function *f = llvm::Function::Create(
      type, llvm::Function::ExternalLinkage, name, module_);
  // We should have been able to call
  // engine_->addGlobalMapping() but this doesn't work
  // See http://lists.cs.uiuc.edu/pipermail/llvmdev/2014-April/071856.html
  // See bug report http://llvm.org/bugs/show_bug.cgi?id=20656
  // following will call DynamicLibrary::AddSymbol
  owner_->addGlobalSymbol(name, address);
  return f;
}

void RaviJITFunctionImpl::dump() { module_->dump(); }

std::unique_ptr<RaviJITState> RaviJITStateFactory::newJITState() {
  return std::unique_ptr<RaviJITState>(new RaviJITStateImpl());
}

// This structure holds stuff we need when compiling a single
// function
struct RaviFunctionDef {
  RaviJITStateImpl *jitState;
  RaviJITFunctionImpl *raviF;
  llvm::Function *f;
  llvm::BasicBlock *entry;
  llvm::Value *L;
  LuaLLVMTypes *types;
  llvm::IRBuilder<> *builder;
  llvm::Constant *luaD_poscallF;
  llvm::Constant *luaF_closeF;
  llvm::Constant *luaV_equalobjF;
  std::vector<llvm::BasicBlock *>jmp_targets;
};

// This class is responsible for compiling Lua byte code
// to LLVM IR
class RaviCodeGenerator {
public:
  RaviCodeGenerator(RaviJITStateImpl *jitState);

  // Compile given function if possible
  // The p->ravi_jit structure will be updated
  // Note that if a function fails to compile then
  // a flag is set so that it doesn't get compiled again
  void compile(lua_State *L, Proto *p);

  // We can only compile a subset of op codes
  // and not all features are supported
  bool canCompile(Proto *p);

  // Create a unique function name in the context
  // of this generator
  const char *unique_function_name();

  // Create the prologue of the JIT function
  // Argument will be named L
  // Initial BasicBlock will be created
  // int func(lua_State *L) {
  std::unique_ptr<RaviJITFunctionImpl>
  create_function(llvm::IRBuilder<> &builder, RaviFunctionDef *def);

  // Add extern declarations for Lua functions we need to call
  void emit_extern_declarations(RaviFunctionDef *def);

  llvm::Value *emit_gep_ci_func_value_gc_asLClosure(RaviFunctionDef *def,
                                                    llvm::Value *ci);

  llvm::Value *emit_gep(RaviFunctionDef *def, llvm::Value *s, int arg1,
                        int arg2);

  llvm::Value *emit_gep(RaviFunctionDef *def, llvm::Value *s, int arg1,
                        int arg2, int arg3);

  void emit_LOADK(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto, int A, int Bx);

  llvm::Value *emit_array_get(RaviFunctionDef *def, llvm::Value *ptr,
                              int offset);

  void emit_RETURN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                   int A, int B);

  void emit_JMP(RaviFunctionDef *def, int j);

  void scan_jump_targets(RaviFunctionDef *def, Proto *p);

  void emit_EQ(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto, int A, int B, int C, int j, int jA);

  void link_block(RaviFunctionDef *def, int pc);

private:
  RaviJITStateImpl *jitState_;
  char temp_[31]; // for name
  int id_;        // for name
};

RaviCodeGenerator::RaviCodeGenerator(RaviJITStateImpl *jitState)
    : jitState_(jitState), id_(1) {
  temp_[0] = 0;
}

const char *RaviCodeGenerator::unique_function_name() {
  snprintf(temp_, sizeof temp_, "ravif%d", id_++);
  return temp_;
}

llvm::Value *RaviCodeGenerator::emit_gep(RaviFunctionDef *def, llvm::Value *s,
                                         int arg1, int arg2) {
  llvm::SmallVector<llvm::Value *, 2> values;
  values.push_back(def->types->kInt[arg1]);
  values.push_back(def->types->kInt[arg2]);
  return def->builder->CreateInBoundsGEP(s, values);
}

llvm::Value *RaviCodeGenerator::emit_gep(RaviFunctionDef *def, llvm::Value *s,
                                         int arg1, int arg2, int arg3) {
  llvm::SmallVector<llvm::Value *, 3> values;
  values.push_back(def->types->kInt[arg1]);
  values.push_back(def->types->kInt[arg2]);
  values.push_back(def->types->kInt[arg3]);
  return def->builder->CreateInBoundsGEP(s, values);
}

llvm::Value *
RaviCodeGenerator::emit_gep_ci_func_value_gc_asLClosure(RaviFunctionDef *def,
                                                        llvm::Value *ci) {
  // emit code for (LClosure *)ci->func->value_.gc
  // fortunately as func is at the beginning of the ci
  // structure we can just cast ci to LClosure*
  llvm::Value *pppLuaClosure =
      def->builder->CreateBitCast(ci, def->types->pppLClosureT);
  llvm::Value *ppLuaClosure = def->builder->CreateLoad(pppLuaClosure);
  return ppLuaClosure;
}

llvm::Value *RaviCodeGenerator::emit_array_get(RaviFunctionDef *def,
                                               llvm::Value *ptr, int offset) {
  // emit code for &ptr[offset]
  return def->builder->CreateInBoundsGEP(
      ptr, llvm::ConstantInt::get(def->types->C_intT, offset));
}

void RaviCodeGenerator::emit_JMP(RaviFunctionDef *def, int j) {
  assert(def->jmp_targets[j]);
  if (def->builder->GetInsertBlock()->getTerminator()) {
    llvm::BasicBlock *jmp_block =
      llvm::BasicBlock::Create(def->jitState->context(), "jmp", def->f);
    def->builder->SetInsertPoint(jmp_block);
  }
  def->builder->CreateBr(def->jmp_targets[j]);
}

void RaviCodeGenerator::emit_LOADK(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                                   int A, int Bx) {
  //    case OP_LOADK: {
  //      TValue *rb = k + GETARG_Bx(i);
  //      setobj2s(L, ra, rb);
  //    } break;

 
  // Load L->ci
  llvm::Value *ci_val = def->builder->CreateLoad(L_ci);

  // Get pointer to base
  llvm::Value *Ci_base = emit_gep(def, ci_val, 0, 4, 0);

  // Load pointer to base
  llvm::Value *base_ptr = def->builder->CreateLoad(Ci_base);

  // Load pointer to proto
  llvm::Value *proto_ptr = def->builder->CreateLoad(proto);

  // Obtain pointer to Proto->k
  llvm::Value *proto_k = emit_gep(def, proto_ptr, 0, 14);

  // Load pointer to k
  llvm::Value *k_ptr = def->builder->CreateLoad(proto_k);

  // LOADK requires a structure assignment
  // in LLVM as far as I can tell this requires a call to
  // an intrinsic memcpy
  llvm::Value *src;
  llvm::Value *dest;

  if (A == 0) {
    // If A is 0 we can use the base pointer which is &base[0]
    dest = base_ptr;
  } else {
    // emit &base[A]
    dest = emit_array_get(def, base_ptr, A);
  }
  if (Bx == 0) {
    // If Bx is 0 we can use the base pointer which is &k[0]
    src = k_ptr;
  } else {
    // emit &k[Bx]
    src = emit_array_get(def, k_ptr, Bx);
  }

  // First get the declaration for the inttrinsic memcpy
  llvm::SmallVector<llvm::Type *, 3> vec;
  vec.push_back(def->types->C_pcharT); /* i8 */
  vec.push_back(def->types->C_pcharT); /* i8 */
  vec.push_back(def->types->C_intT);
  llvm::Function *f = llvm::Intrinsic::getDeclaration(
      def->raviF->module(), llvm::Intrinsic::memcpy, vec);
  lua_assert(f);

  // Cast src and dest to i8*
  llvm::Value *dest_ptr =
      def->builder->CreateBitCast(dest, def->types->C_pcharT);
  llvm::Value *src_ptr = def->builder->CreateBitCast(src, def->types->C_pcharT);

  // Create call to intrinsic memcpy
  llvm::SmallVector<llvm::Value *, 5> values;
  values.push_back(dest_ptr);
  values.push_back(src_ptr);
  values.push_back(llvm::ConstantInt::get(def->types->C_intT, sizeof(TValue)));
  values.push_back(
      llvm::ConstantInt::get(def->types->C_intT, sizeof(L_Umaxalign)));
  values.push_back(def->types->kFalse);
  def->builder->CreateCall(f, values);
}

void RaviCodeGenerator::emit_RETURN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
  int A, int B) {

  // Here is what OP_RETURN looks like. We only compile steps
  // marked with //*.
  // TODO that means we cannot handle functions that have sub
  // functions (closures) as do not handle the luaF_close() call

  // case OP_RETURN: {
  //  int b = GETARG_B(i);
  //*  if (b != 0) L->top = ra + b - 1;
  //*  if (cl->p->sizep > 0) luaF_close(L, base);
  //*  b = luaD_poscall(L, ra);
  //    if (!(ci->callstatus & CIST_REENTRY))  /* 'ci' still the called one */
  //      return;  /* external invocation: return */
  //    else {  /* invocation via reentry: continue execution */
  //*      ci = L->ci;
  //*      if (b) L->top = ci->top;
  //      goto newframe;  /* restart luaV_execute over new Lua function */
  //    }
  // }

  // As Lua inserts redundant OP_RETURN instructions it is
  // possible that this is one of them. If this is the case then the
  // current block may already be terminated - so we have to insert
  // a new block
  if (def->builder->GetInsertBlock()->getTerminator()) {
    llvm::BasicBlock *return_block =
        llvm::BasicBlock::Create(def->jitState->context(), "return", def->f);
    def->builder->SetInsertPoint(return_block);
  }

  // Load L->ci
  llvm::Value *ci_val = def->builder->CreateLoad(L_ci);

  // Get pointer to base
  llvm::Value *Ci_base = emit_gep(def, ci_val, 0, 4, 0);

  // Load pointer to base
  llvm::Value *base_ptr = def->builder->CreateLoad(Ci_base);

  // Load pointer to proto
  llvm::Value *proto_ptr = def->builder->CreateLoad(proto);

  // Obtain pointer to Proto->k
  llvm::Value *proto_k = emit_gep(def, proto_ptr, 0, 14);

  // Load pointer to k
  llvm::Value *k_ptr = def->builder->CreateLoad(proto_k);

  llvm::Value *top = nullptr;

  // Get pointer to register A
  llvm::Value *ra_ptr = A == 0 ? base_ptr : emit_array_get(def, base_ptr, A);

  //*  if (b != 0) L->top = ra + b - 1;
  if (B != 0) {
    // Get pointer to register at ra + b - 1
    llvm::Value *ptr = emit_array_get(def, base_ptr, A + B - 1);
    // Get pointer to L->top
    top = emit_gep(def, def->L, 0, 4);
    // Assign to L->top
    def->builder->CreateStore(ptr, top);
  }

  // if (cl->p->sizep > 0) luaF_close(L, base);
  // Get pointer to Proto->sizep
  llvm::Value *psize_ptr = emit_gep(def, proto_ptr, 0, 10);
  // Load psize
  llvm::Value *psize = def->builder->CreateLoad(psize_ptr);
  // Test if psize > 0
  llvm::Value *psize_gt_0 =
      def->builder->CreateICmpSGT(psize, def->types->kInt[0]);
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(psize_gt_0, then_block, else_block);
  def->builder->SetInsertPoint(then_block);
  // Call luaF_close
  def->builder->CreateCall2(def->luaF_closeF, def->L, base_ptr);
  def->builder->CreateBr(else_block);
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);

  //*  b = luaD_poscall(L, ra);
  llvm::Value *result =
      def->builder->CreateCall2(def->luaD_poscallF, def->L, ra_ptr);

  //*      if (b) L->top = ci->top;
  // Test if b is != 0
  llvm::Value *result_is_notzero =
      def->builder->CreateICmpNE(result, def->types->kInt[0]);
  llvm::BasicBlock *ThenBB =
      llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *ElseBB =
      llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(result_is_notzero, ThenBB, ElseBB);
  def->builder->SetInsertPoint(ThenBB);
  // Get pointer to ci->top
  llvm::Value *citop = emit_gep(def, ci_val, 0, 1);
  // Load ci->top
  llvm::Value *citop_val = def->builder->CreateLoad(citop);
  if (!top)
    // Get L->top
    top = emit_gep(def, def->L, 0, 4);
  // Assign ci>top to L->top
  def->builder->CreateStore(citop_val, top);
  def->builder->CreateBr(ElseBB);
  def->f->getBasicBlockList().push_back(ElseBB);
  def->builder->SetInsertPoint(ElseBB);

  // as our prototype is lua_Cfunction we need
  // to return a value
  def->builder->CreateRet(def->types->kInt[1]);
}

void RaviCodeGenerator::emit_EQ(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto, int A, int B, int C, int j, int jA) {
  //  case OP_EQ: {
  //    TValue *rb = RKB(i);
  //    TValue *rc = RKC(i);
  //    Protect(
  //      if (cast_int(luaV_equalobj(L, rb, rc)) != GETARG_A(i))
  //        ci->u.l.savedpc++;
  //      else
  //        donextjump(ci);
  //    )
  //  } break;

  // Load L->ci
  llvm::Value *ci_val = def->builder->CreateLoad(L_ci);

  // Get pointer to base
  llvm::Value *Ci_base = emit_gep(def, ci_val, 0, 4, 0);

  // Load pointer to base
  llvm::Value *base_ptr = def->builder->CreateLoad(Ci_base);

  // Load pointer to proto
  llvm::Value *proto_ptr = def->builder->CreateLoad(proto);

  // Obtain pointer to Proto->k
  llvm::Value *proto_k = emit_gep(def, proto_ptr, 0, 14);

  // Load pointer to k
  llvm::Value *k_ptr = def->builder->CreateLoad(proto_k);

  llvm::Value *lhs_ptr;
  llvm::Value *rhs_ptr;

  // Get pointer to register B
  llvm::Value *base_or_k = ISK(B) ? k_ptr : base_ptr;
  int b = ISK(B) ? INDEXK(B) : B;
  if (b == 0) {
    lhs_ptr = base_or_k;
  }
  else {
    lhs_ptr = emit_array_get(def, base_or_k, b);
  }

  // Get pointer to register C
  base_or_k = ISK(C) ? k_ptr : base_ptr;
  int c = ISK(C) ? INDEXK(C) : C;
  if (c == 0) {
    rhs_ptr = base_or_k;
  }
  else {
    rhs_ptr = emit_array_get(def, base_or_k, c);
  }
  
  // Call luaV_equalobj with register B and C
  llvm::Value *result = def->builder->CreateCall3(def->luaV_equalobjF, def->L, lhs_ptr, rhs_ptr);
  // Test if result is equal to operand A
  llvm::Value *result_eq_A = def->builder->CreateICmpEQ(result, llvm::ConstantInt::get(def->types->C_intT, A));
  // If result == A then we need to execute the next statement which is a jump
  llvm::BasicBlock *then_block =
    llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *else_block =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(result_eq_A, then_block, else_block);
  def->builder->SetInsertPoint(then_block);

  // if (a > 0) luaF_close(L, ci->u.l.base + a - 1);
  if (jA > 0) {
    // jA is the A operand of the Jump instruction
    // Get pointer to base
    llvm::Value *Ci_base2 = emit_gep(def, ci_val, 0, 4, 0);

    // Load pointer to base
    llvm::Value *base2_ptr = def->builder->CreateLoad(Ci_base2);

    // base + a - 1
    llvm::Value *val = jA == 1 ? base2_ptr : emit_array_get(def, base2_ptr, jA - 1);

    // Call 
    def->builder->CreateCall2(def->luaF_closeF, def->L, val);
  }
  // Do the jump
  def->builder->CreateBr(def->jmp_targets[j]);
  // Add the else block and make it current so that the next instruction flows here
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
}

// Check if we can compile
// The cases we can compile will increase over time
bool RaviCodeGenerator::canCompile(Proto *p) {
  if (jitState_ == nullptr) {
    p->ravi_jit.jit_status = 1;
    return false;
  }
  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  // TODO we cannot handle variable arguments or
  // if the function has sub functions (closures)
  if (p->sizep > 0 || p->is_vararg) {
    p->ravi_jit.jit_status = 1;
    return false;
  }
  // Loop over the byte codes; as Lua compiler inserts
  // an extra RETURN op we need to ignore the last op
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode o = GET_OPCODE(i);
    switch (o) {
    case OP_LOADK:
    case OP_RETURN:
    case OP_JMP:
    case OP_EQ:
      break;
    default:
      return false;
    }
  }
  return true;
}

std::unique_ptr<RaviJITFunctionImpl>
RaviCodeGenerator::create_function(llvm::IRBuilder<> &builder,
                                   RaviFunctionDef *def) {
  LuaLLVMTypes *types = jitState_->types();

  std::unique_ptr<ravi::RaviJITFunctionImpl> func =
      std::unique_ptr<RaviJITFunctionImpl>(
          (RaviJITFunctionImpl *)jitState_->createFunction(
              types->jitFunctionT, llvm::Function::ExternalLinkage,
              unique_function_name()));
  if (!func)
    return func;

  llvm::Function *mainFunc = func->function();
  llvm::BasicBlock *entry =
      llvm::BasicBlock::Create(jitState_->context(), "entry", mainFunc);
  builder.SetInsertPoint(entry);

  auto argiter = mainFunc->arg_begin();
  llvm::Value *arg1 = argiter++;
  arg1->setName("L");

  def->jitState = jitState_;
  def->f = mainFunc;
  def->entry = entry;
  def->L = arg1;
  def->raviF = func.get();
  def->types = types;
  def->builder = &builder;
  def->luaD_poscallF = nullptr;
  def->luaF_closeF = nullptr;
  def->luaV_equalobjF = nullptr;

  return func;
}

void RaviCodeGenerator::emit_extern_declarations(RaviFunctionDef *def) {
  // Add extern declarations for Lua functions that we need to call
  def->luaD_poscallF = def->raviF->addExternFunction(
      def->types->luaD_poscallT, &luaD_poscall, "luaD_poscall");
  def->luaF_closeF = def->raviF->addExternFunction(def->types->luaF_closeT,
                                                   &luaF_close, "luaF_close");
  def->luaV_equalobjF = def->raviF->addExternFunction(
      def->types->luaV_equalobjT, &luaV_equalobj, "luaV_equalobj");
}

#define RA(i) (base + GETARG_A(i))
/* to be used after possible stack reallocation */
#define RB(i) check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base + GETARG_B(i))
#define RC(i) check_exp(getCMode(GET_OPCODE(i)) == OpArgR, base + GETARG_C(i))
#define RKB(i)                                                                 \
  check_exp(getBMode(GET_OPCODE(i)) == OpArgK,                                 \
            ISK(GETARG_B(i)) ? k + INDEXK(GETARG_B(i)) : base + GETARG_B(i))
#define RKC(i)                                                                 \
  check_exp(getCMode(GET_OPCODE(i)) == OpArgK,                                 \
            ISK(GETARG_C(i)) ? k + INDEXK(GETARG_C(i)) : base + GETARG_C(i))
#define KBx(i)                                                                 \
  (k + (GETARG_Bx(i) != 0 ? GETARG_Bx(i) - 1 : GETARG_Ax(*ci->u.l.savedpc++)))
/* RAVI */
#define KB(i)                                                                  \
  check_exp(getBMode(GET_OPCODE(i)) == OpArgK, k + INDEXK(GETARG_B(i)))
#define KC(i)                                                                  \
  check_exp(getCMode(GET_OPCODE(i)) == OpArgK, k + INDEXK(GETARG_C(i)))

void RaviCodeGenerator::link_block(RaviFunctionDef *def, int pc) {
  // If the current bytecode offset pc is on a jump target
  // then we need to insert the block we previously created in scan_jump_targets()
  // and make it the current insert block; also if the previous block
  // is unterminated then we simply provide a branch from previous block to the
  // new block
  if (def->jmp_targets[pc]) {
    // We are on a jump target
    // Get the block we previously created scan_jump_targets
    llvm::BasicBlock *block = def->jmp_targets[pc];
    if (!def->builder->GetInsertBlock()->getTerminator()) {
      // Previous block not terminated so branch to the 
      // new block
      def->builder->CreateBr(block);
    }
    // Now add the new block and make it current
    def->f->getBasicBlockList().push_back(block);
    def->builder->SetInsertPoint(block);
  }
}

void RaviCodeGenerator::compile(lua_State *L, Proto *p) {
  if (p->ravi_jit.jit_status != 0 || !canCompile(p))
    return;
#if 1
  RaviFunctionDef def = { 0 };

  llvm::LLVMContext &context = jitState_->context();
  llvm::IRBuilder<> builder(context);

  auto f = create_function(builder, &def);
  if (!f) {
    p->ravi_jit.jit_status = 1; // can't compile
    return;
  }
  // Add extern declarations for Lua functions we need to call
  emit_extern_declarations(&def);

  // Create BasicBlocks for all the jump targets in the Lua bytecode
  scan_jump_targets(&def, p);

  // Get pointer to L->ci
  llvm::Value *L_ci = emit_gep(&def, def.L, 0, 6);

  // Load pointer value
  llvm::Value *ci_val = builder.CreateLoad(L_ci);

  // Get pointer to base
  //llvm::Value *Ci_base = emit_gep(&def, ci_val, 0, 4, 0);

  // Load pointer to base
  //llvm::Value *base_ptr = builder.CreateLoad(Ci_base);

  // We need to get hold of the constants table
  // which is located in ci->func->value_.gc
  // and is a value of type LClosure*
  // fortunately as func is at the beginning of the ci
  // structure we can just cast ci to LClosure*
  llvm::Value *L_cl = emit_gep_ci_func_value_gc_asLClosure(&def, ci_val);

  // Load pointer to LClosure
  llvm::Value *cl_ptr = builder.CreateLoad(L_cl);

  // Get pointer to the Proto* which is cl->p
  llvm::Value *proto = emit_gep(&def, cl_ptr, 0, 5);

  // Load pointer to proto
  //llvm::Value *proto_ptr = builder.CreateLoad(proto);

  // Obtain pointer to Proto->k
  //llvm::Value *proto_k = emit_gep(&def, proto_ptr, 0, 14);

  // Load pointer to k
  //llvm::Value *k_ptr = builder.CreateLoad(proto_k);

  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  for (pc = 0; pc < n; pc++) {
    link_block(&def, pc);
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    int A = GETARG_A(i);
    switch (op) {
    case OP_LOADK: {
      int Bx = GETARG_Bx(i);
      emit_LOADK(&def, L_ci, proto, A, Bx);
    } break;
    case OP_RETURN: {
      int B = GETARG_B(i);
      emit_RETURN(&def, L_ci, proto, A, B);
    } break;
    case OP_EQ: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      pc++;
      i = code[pc];
      op = GET_OPCODE(i);
      lua_assert(op == OP_JMP);
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      emit_EQ(&def, L_ci, proto, A, B, C, j, GETARG_A(i));
    } break;
    case OP_JMP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      emit_JMP(&def, j);
    } break;
    default:
      break;
    }
  }

  f->dump();
  llvm::verifyFunction(*f->function());
  ravi::RaviJITFunctionImpl *llvm_func = f.release();
  p->ravi_jit.jit_data = reinterpret_cast<void *>(llvm_func);
  p->ravi_jit.jit_function = (lua_CFunction)llvm_func->compile();
  if (p->ravi_jit.jit_function == nullptr) {
    p->ravi_jit.jit_status = 1; // can't compile
    delete llvm_func;
    p->ravi_jit.jit_data = NULL;
  } else {
    p->ravi_jit.jit_status = 2;
  }
#else
  // For now
  p->ravi_jit.jit_status = 1; // can't compile
#endif
}

void RaviCodeGenerator::scan_jump_targets(RaviFunctionDef *def, Proto *p)  {
  def->jmp_targets.clear();
  const Instruction *code = p->code;
  int pc, n = p->sizecode;
  for (pc = 0; pc < n; pc++)
    def->jmp_targets.push_back(nullptr);

  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode op = GET_OPCODE(i);
    int A = GETARG_A(i);
    switch (op) {
    case OP_JMP:
    case OP_FORLOOP:
    case OP_FORPREP:
    case OP_TFORLOOP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      def->jmp_targets[j] = llvm::BasicBlock::Create(def->jitState->context(), "jmptarget");
    } break;
    default:
      break;
    }
  }
}


}

#ifdef __cplusplus
extern "C" {
#endif

struct ravi_State {
  ravi::RaviJITState *jit;
  ravi::RaviCodeGenerator *code_generator;
};

int raviV_initjit(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state != NULL)
    return -1;
  ravi_State *jit = (ravi_State *)calloc(1, sizeof(ravi_State));
  jit->jit = new ravi::RaviJITStateImpl();
  jit->code_generator =
      new ravi::RaviCodeGenerator((ravi::RaviJITStateImpl *)jit->jit);
  G->ravi_state = jit;
  return 0;
}

void raviV_close(struct lua_State *L) {
  global_State *G = G(L);
  if (G->ravi_state == NULL)
    return;
  delete G->ravi_state->code_generator;
  delete G->ravi_state->jit;
  free(G->ravi_state);
}

int raviV_compile(struct lua_State *L, struct Proto *p) {
  global_State *G = G(L);
  if (G->ravi_state == NULL)
    return 0;
  G->ravi_state->code_generator->compile(L, p);
  return 0;
}

void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  if (p->ravi_jit.jit_status == 2) /* compiled */ {
    ravi::RaviJITFunction *f =
        reinterpret_cast<ravi::RaviJITFunction *>(p->ravi_jit.jit_data);
    if (f)
      delete f;
    p->ravi_jit.jit_status = 3;
    p->ravi_jit.jit_function = NULL;
    p->ravi_jit.jit_data = NULL;
  }
}

#ifdef __cplusplus
}
#endif

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
//#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
//#include "ltm.h"
#include "lvm.h"

#ifdef __cplusplus
}
#endif

#include <array>
#include <iterator>
#include <type_traits>
#include <atomic>

namespace ravi {

// TODO - should probably be an atomic int
static std::atomic_int init;

// All Lua types are gathered here
struct LuaLLVMTypes {

  LuaLLVMTypes(llvm::LLVMContext &context);
  void dump();

  llvm::Type *C_intptr_t;
  llvm::Type *C_size_t;
  llvm::Type *C_ptrdiff_t;
  llvm::Type *C_int64_t;

  llvm::Type *lua_NumberT;
  llvm::Type *plua_NumberT;

  llvm::Type *lua_IntegerT;
  llvm::PointerType *plua_IntegerT;

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
  llvm::Type *C_pintT;

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
  llvm::FunctionType *luaV_lessthanT;
  llvm::FunctionType *luaV_lessequalT;
  llvm::FunctionType *luaG_runerrorT;
  llvm::FunctionType *luaV_forlimitT;
  llvm::FunctionType *luaV_tonumberT;

  std::array<llvm::Constant *, 21> kInt;
  std::array<llvm::Constant *, 21> kluaInteger;

  llvm::Constant *kFalse;

  // To allow better optimization we need to decorate the
  // LLVM Load/Store instructions with type information.
  // For this we need to construct tbaa metadata 
  llvm::MDBuilder mdbuilder;
  llvm::MDNode *tbaa_root;
  llvm::MDNode *tbaa_charT;
  llvm::MDNode *tbaa_shortT;
  llvm::MDNode *tbaa_intT;
  llvm::MDNode *tbaa_longlongT;
  llvm::MDNode *tbaa_pointerT;
  llvm::MDNode *tbaa_CallInfo_lT;
  llvm::MDNode *tbaa_CallInfoT;
  llvm::MDNode *tbaa_luaStateT;
  llvm::MDNode *tbaa_luaState_ciT;
  llvm::MDNode *tbaa_luaState_ci_baseT;
  llvm::MDNode *tbaa_CallInfo_funcT;
  llvm::MDNode *tbaa_CallInfo_func_LClosureT;
  llvm::MDNode *tbaa_LClosureT;
  llvm::MDNode *tbaa_LClosure_pT;
  llvm::MDNode *tbaa_ProtoT;
  llvm::MDNode *tbaa_Proto_kT;
  llvm::MDNode *tbaa_TValueT;
  llvm::MDNode *tbaa_TValue_nT;
  llvm::MDNode *tbaa_TValue_ttT;
  llvm::MDNode *tbaa_luaState_topT;
};

LuaLLVMTypes::LuaLLVMTypes(llvm::LLVMContext &context) : mdbuilder(context) {

  static_assert(std::is_floating_point<lua_Number>::value &&
                    sizeof(lua_Number) == sizeof(double),
                "lua_Number is not a double");
  lua_NumberT = llvm::Type::getDoubleTy(context);
  plua_NumberT = llvm::PointerType::get(lua_NumberT, 0);

  static_assert(sizeof(lua_Integer) == sizeof(lua_Number), "Only 64-bit int supported");
  static_assert(std::is_integral<lua_Integer>::value,
                "lua_Integer is not an integer type");
  lua_IntegerT = llvm::Type::getIntNTy(context, sizeof(lua_Integer) * 8);
  plua_IntegerT = llvm::PointerType::get(lua_IntegerT, 0);

  static_assert(sizeof(lua_Integer) == sizeof(lua_Unsigned),
                "lua_Integer and lua_Unsigned are of different size");
  lua_UnsignedT = lua_IntegerT;

  C_intptr_t = llvm::Type::getIntNTy(context, sizeof(intptr_t) * 8);
  C_size_t = llvm::Type::getIntNTy(context, sizeof(size_t) * 8);
  C_ptrdiff_t = llvm::Type::getIntNTy(context, sizeof(ptrdiff_t) * 8);
  C_int64_t = llvm::Type::getIntNTy(context, sizeof(int64_t) * 8);
  C_intT = llvm::Type::getIntNTy(context, sizeof(int) * 8);
  C_pintT = llvm::PointerType::get(C_intT, 0);

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

  static_assert(sizeof(Value) == sizeof(lua_Number) && sizeof(Value) == sizeof(lua_Integer),
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

  // int luaV_lessthan (lua_State *L, const TValue *l, const TValue *r)
  luaV_lessthanT = llvm::FunctionType::get(C_intT, elements, false);

  // int luaV_lessequal (lua_State *L, const TValue *l, const TValue *r)
  luaV_lessequalT = llvm::FunctionType::get(C_intT, elements, false);

  // l_noret luaG_runerror (lua_State *L, const char *fmt, ...)
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(C_pcharT);
  luaG_runerrorT = llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  elements.clear();
  elements.push_back(pTValueT);
  elements.push_back(plua_IntegerT);
  elements.push_back(lua_IntegerT);
  elements.push_back(C_pintT);
  luaV_forlimitT = llvm::FunctionType::get(C_intT, elements, false);

  elements.clear();
  elements.push_back(pTValueT);
  elements.push_back(plua_NumberT);
  luaV_tonumberT = llvm::FunctionType::get(C_intT, elements, false);

  for (int j = 0; j < kInt.size(); j++)
    kInt[j] = llvm::ConstantInt::get(C_intT, j);
  for (int j = 0; j < kluaInteger.size(); j++)
    kluaInteger[j] = llvm::ConstantInt::get(lua_IntegerT, j);

  kFalse = llvm::ConstantInt::getFalse(llvm::Type::getInt1Ty(context));

  // Do what Clang does
  //!5 = metadata !{metadata !"Simple C/C++ TBAA"}
  tbaa_root = mdbuilder.createTBAARoot("Simple C / C++ TBAA"); 
  //!4 = metadata !{metadata !"omnipotent char", metadata !5, i64 0}
  tbaa_charT = mdbuilder.createTBAAScalarTypeNode("omnipotent char", tbaa_root, 0); 
  //!3 = metadata !{metadata !"any pointer", metadata !4, i64 0}
  tbaa_pointerT = mdbuilder.createTBAAScalarTypeNode("any pointer", tbaa_charT, 0);
  //!10 = metadata !{metadata !"short", metadata !4, i64 0}
  tbaa_shortT = mdbuilder.createTBAAScalarTypeNode("short", tbaa_charT, 0);
  //!11 = metadata !{metadata !"int", metadata !4, i64 0}
  tbaa_intT = mdbuilder.createTBAAScalarTypeNode("int", tbaa_charT, 0);
  //!9 = metadata !{metadata !"long long", metadata !4, i64 0}
  tbaa_longlongT = mdbuilder.createTBAAScalarTypeNode("long long", tbaa_charT, 0);

  //!14 = metadata !{metadata !"CallInfoL", metadata !3, i64 0, metadata !3, i64 4, metadata !9, i64 8}
  std::vector<std::pair<llvm::MDNode *, uint64_t> > nodes;
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 4));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_longlongT, 8));
  tbaa_CallInfo_lT = mdbuilder.createTBAAStructTypeNode("CallInfo_l", nodes);

  //!13 = metadata !{metadata !"CallInfoLua", 
  //                 metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8, 
  //                 metadata !3, i64 12, metadata !14, i64 16, metadata !9, i64 32, 
  //                 metadata !10, i64 40, metadata !4, i64 42}
  nodes.clear();
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 4));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 8));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 12));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_CallInfo_lT, 16));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_longlongT, 32));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_shortT, 40));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 42));
  tbaa_CallInfoT = mdbuilder.createTBAAStructTypeNode("CallInfo", nodes);

  //!7 = metadata !{metadata !"lua_State", 
  //                metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, 
  //                metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64 12, 
  //                metadata !3, i64 16, metadata !3, i64 20, metadata !3, i64 24, 
  //                metadata !3, i64 28, metadata !3, i64 32, metadata !3, i64 36, 
  //                metadata !3, i64 40, metadata !3, i64 44, metadata !8, i64 48, 
  //                metadata !3, i64 104, metadata !9, i64 112, metadata !11, i64 120, 
  //                metadata !11, i64 124, metadata !11, i64 128, metadata !10, i64 132, 
  //                metadata !10, i64 134, metadata !4, i64 136, metadata !4, i64 137}
  nodes.clear();
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 4));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 5));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 6));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 8));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 12));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 16));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 20));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 24));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 28));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 32));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 36));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 40));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 44));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_CallInfoT, 48));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 92));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_longlongT, 96));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 104));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 108));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 112));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_shortT, 114));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_shortT, 116));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 118));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 119));
  tbaa_luaStateT = mdbuilder.createTBAAStructTypeNode("lua_State", nodes);

  tbaa_luaState_ciT = mdbuilder.createTBAAStructTagNode(tbaa_luaStateT, tbaa_CallInfoT, 16);
  tbaa_luaState_ci_baseT = mdbuilder.createTBAAStructTagNode(tbaa_CallInfoT, tbaa_pointerT, 16);
  tbaa_CallInfo_funcT = mdbuilder.createTBAAStructTagNode(tbaa_CallInfoT, tbaa_pointerT, 0);
  tbaa_CallInfo_func_LClosureT = mdbuilder.createTBAAStructTagNode(tbaa_pointerT, tbaa_pointerT, 0);

  //!20 = metadata !{metadata !"Proto", 
  //                 metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, 
  //                 metadata !4, i64 6, metadata !4, i64 7, metadata !4, i64 8, 
  //                 metadata !11, i64 12, metadata !11, i64 16, metadata !11, i64 20, 
  //                 metadata !11, i64 24, metadata !11, i64 28, metadata !11, i64 32, 
  //                 metadata !11, i64 36, metadata !11, i64 40, metadata !3, i64 44, 
  //                 metadata !3, i64 48, metadata !3, i64 52, metadata !3, i64 56, 
  //                 metadata !3, i64 60, metadata !3, i64 64, metadata !3, i64 68, 
  //                 metadata !3, i64 72, metadata !3, i64 76}
  nodes.clear();
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 4));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 5));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 6));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 7));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 8));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 12));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 16));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 20));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 24));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 28));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 32));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 36));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 40));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 44));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 48));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 52));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 56));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 60));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 64));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 68));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 72));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 76));
  tbaa_ProtoT = mdbuilder.createTBAAStructTypeNode("Proto", nodes);

  //!18 = metadata !{metadata !"LClosure", 
  //                 metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, 
  //                 metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64 12, 
  //                 metadata !4, i64 16}
  nodes.clear();
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 4));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 5));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 6));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 8));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 12));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 16));
  tbaa_LClosureT = mdbuilder.createTBAAStructTypeNode("LClosure", nodes);

  tbaa_LClosure_pT = mdbuilder.createTBAAStructTagNode(tbaa_LClosureT, tbaa_pointerT, 12);

  //!19 = metadata !{metadata !20, metadata !3, i64 44}
  tbaa_Proto_kT = mdbuilder.createTBAAStructTagNode(tbaa_ProtoT, tbaa_pointerT, 44);

  nodes.clear();
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_longlongT, 0));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_intT, 8));
  tbaa_TValueT = mdbuilder.createTBAAStructTypeNode("TValue", nodes);

  tbaa_TValue_nT = mdbuilder.createTBAAStructTagNode(tbaa_TValueT, tbaa_longlongT, 0);
  tbaa_TValue_ttT = mdbuilder.createTBAAStructTagNode(tbaa_TValueT, tbaa_intT, 8);
  
  tbaa_luaState_topT = mdbuilder.createTBAAStructTagNode(tbaa_luaStateT, tbaa_pointerT, 8);
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
  RaviJITFunctionImpl(RaviJITStateImpl *owner, 
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
  const std::string& triple() const {
    return triple_;
  }
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
  for (auto f = std::begin(functions_); f != std::end(functions_); f++) {
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
  RaviJITFunction *f =
      new RaviJITFunctionImpl(this, type, linkage, name);
  functions_[name] = f;
  return f;
}

void RaviJITStateImpl::deleteFunction(const std::string &name) {
  functions_.erase(name);
  // This is called when RaviJITFunction is deleted
}

RaviJITFunctionImpl::RaviJITFunctionImpl(
    RaviJITStateImpl *owner, llvm::FunctionType *type,
    llvm::GlobalValue::LinkageTypes linkage, const std::string &name)
    : owner_(owner), name_(name), engine_(nullptr), module_(nullptr),
      function_(nullptr), ptr_(nullptr) {
  // MCJIT treats each module as a compilation unit
  // To enable function level life cycle we create a
  // module per function
  std::string moduleName = "ravi_module_" + name;
  module_ = new llvm::Module(moduleName, owner->context());
#if defined(_WIN32)
  // On Windows we get error saying incompatible object format
  // Reading posts on mailining lists I found that the issue is that COEFF
  // format is not supported and therefore we need to set
  // -elf as the object format
  module_->setTargetTriple(owner->triple());
#endif

  function_ = llvm::Function::Create(type, linkage, name, module_);
  std::string errStr;
#if LLVM_VERSION_MINOR > 5
  // LLVM 3.6.0 change
  std::unique_ptr<llvm::Module> module(module_);
  engine_ = llvm::EngineBuilder(std::move(module))
                .setEngineKind(llvm::EngineKind::JIT)
                .setErrorStr(&errStr)
                .create();
#else
  engine_ = llvm::EngineBuilder(module_)
    .setEngineKind(llvm::EngineKind::JIT)
    .setUseMCJIT(true)
    .setErrorStr(&errStr)
    .create();
#endif
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
    // if engine was created then we don't need to delete the
    // module as it would have been deleted by the engine
    delete module_;
}

void *RaviJITFunctionImpl::compile() {

  //module_->dump();

  // Create a function pass manager for this engine
  llvm::FunctionPassManager *FPM = new llvm::FunctionPassManager(module_);

  // Set up the optimizer pipeline.  Start with registering info about how the
  // target lays out data structures.
#if LLVM_VERSION_MINOR > 5
  // LLVM 3.6.0 change
  module_->setDataLayout(engine_->getDataLayout());
  FPM->add(new llvm::DataLayoutPass());
#else
  auto target_layout = engine_->getTargetMachine()->getDataLayout();
  module_->setDataLayout(target_layout);
  FPM->add(new llvm::DataLayoutPass(*engine_->getDataLayout()));
#endif
  // Provide basic AliasAnalysis support for GVN.
  FPM->add(llvm::createBasicAliasAnalysisPass());
  // Promote allocas to registers.
  FPM->add(llvm::createPromoteMemoryToRegisterPass());
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  FPM->add(llvm::createInstructionCombiningPass());
  // Reassociate expressions.
  FPM->add(llvm::createReassociatePass());
  FPM->add(llvm::createTypeBasedAliasAnalysisPass());
  // Eliminate Common SubExpressions.
  FPM->add(llvm::createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  FPM->add(llvm::createCFGSimplificationPass());
  FPM->doInitialization();

  // For each function in the module
  // Run the FPM on this function
  FPM->run(*function_);

  //module_->dump();

  // We don't need this anymore
  delete FPM;

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

  // Lua function declarations
  llvm::Constant *luaD_poscallF;
  llvm::Constant *luaF_closeF;
  llvm::Constant *luaV_equalobjF;
  llvm::Constant *luaV_lessthanF;
  llvm::Constant *luaV_lessequalF;
  llvm::Constant *luaG_runerrorF;
  llvm::Constant *luaV_forlimitF;
  llvm::Constant *luaV_tonumberF;

  // Jump targets in the function
  std::vector<llvm::BasicBlock *> jmp_targets;

  // Load pointer to proto
  llvm::Instruction *proto_ptr;

  // Obtain pointer to Proto->k
  llvm::Value *proto_k;

  // Load pointer to k
  llvm::Instruction *k_ptr;

  // Load L->ci
  llvm::Instruction *ci_val;

  // Get pointer to base
  llvm::Value *Ci_base;
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

  // emit code for (LClosure *)ci->func->value_.gc
  llvm::Value *emit_gep_ci_func_value_gc_asLClosure(RaviFunctionDef *def,
                                                    llvm::Value *ci);

  llvm::Value *emit_gep(RaviFunctionDef *def, const char *name, llvm::Value *s,
                        int arg1, int arg2);

  llvm::Value *emit_gep(RaviFunctionDef *def, const char *name, llvm::Value *s,
                        int arg1, int arg2, int arg3);

  // emit code for &ptr[offset]
  llvm::Value *emit_array_get(RaviFunctionDef *def, llvm::Value *ptr,
                              int offset);

  // Look for Lua bytecodes that are jump targets and allocate
  // a BasicBlock for each such target in def->jump_targets.
  // The BasicBlocks are not inserted into the function until later
  // but having them created allows rest of the code to insert
  // branch instructions
  void scan_jump_targets(RaviFunctionDef *def, Proto *p);

  void link_block(RaviFunctionDef *def, int pc);

  void emit_LOADK(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                  int A, int Bx);

  void emit_RETURN(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
                   int A, int B);

  void emit_JMP(RaviFunctionDef *def, int j);

  void emit_FORPREP(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
    int A, int sBx);

  // Emit code for OP_EQ, OP_LT and OP_LE
  // The callee parameter should be luaV_equalobj, luaV_lessthan and
  // luaV_lessequal
  // respectively
  // A, B, C must be operands of the OP_EQ/OP_LT/OP_LE instructions
  // j must be the jump target (offset of the code to which we need to jump to)
  // jA must be the A operand of the jump instruction
  void emit_EQ(RaviFunctionDef *def, llvm::Value *L_ci, llvm::Value *proto,
               int A, int B, int C, int j, int jA, llvm::Constant *callee);

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

llvm::Value *RaviCodeGenerator::emit_gep(RaviFunctionDef *def, const char *name,
                                         llvm::Value *s, int arg1, int arg2) {
  llvm::SmallVector<llvm::Value *, 2> values;
  values.push_back(def->types->kInt[arg1]);
  values.push_back(def->types->kInt[arg2]);
  return def->builder->CreateInBoundsGEP(s, values, name);
}

llvm::Value *RaviCodeGenerator::emit_gep(RaviFunctionDef *def, const char *name,
                                         llvm::Value *s, int arg1, int arg2,
                                         int arg3) {
  llvm::SmallVector<llvm::Value *, 3> values;
  values.push_back(def->types->kInt[arg1]);
  values.push_back(def->types->kInt[arg2]);
  values.push_back(def->types->kInt[arg3]);
  return def->builder->CreateInBoundsGEP(s, values, name);
}

llvm::Value *
RaviCodeGenerator::emit_gep_ci_func_value_gc_asLClosure(RaviFunctionDef *def,
                                                        llvm::Value *ci) {
  // emit code for (LClosure *)ci->func->value_.gc
  // fortunately as func is at the beginning of the ci
  // structure we can just cast ci to LClosure*
  llvm::Value *pppLuaClosure =
      def->builder->CreateBitCast(ci, def->types->pppLClosureT);
  llvm::Instruction *ppLuaClosure = def->builder->CreateLoad(pppLuaClosure);
  ppLuaClosure->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_CallInfo_funcT);
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
        llvm::BasicBlock::Create(def->jitState->context(), "jump", def->f);
    def->builder->SetInsertPoint(jmp_block);
  }
  def->builder->CreateBr(def->jmp_targets[j]);
}

void RaviCodeGenerator::emit_FORPREP(RaviFunctionDef *def, llvm::Value *L_ci,
  llvm::Value *proto, int A, int pc) {

    //case OP_FORPREP: {
    //  if (ttisinteger(init) && ttisinteger(pstep) &&
    //    forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {
    //    /* all values are integer */
    //    lua_Integer initv = (stopnow ? 0 : ivalue(init));
    //    setivalue(plimit, ilimit);
    //    setivalue(init, initv - ivalue(pstep));
    //  }
    //  else {  /* try making all values floats */
    //    lua_Number ninit; lua_Number nlimit; lua_Number nstep;
    //    if (!tonumber(plimit, &nlimit))
    //      luaG_runerror(L, "'for' limit must be a number");
    //    setfltvalue(plimit, nlimit);
    //    if (!tonumber(pstep, &nstep))
    //      luaG_runerror(L, "'for' step must be a number");
    //    setfltvalue(pstep, nstep);
    //    if (!tonumber(init, &ninit))
    //      luaG_runerror(L, "'for' initial value must be a number");
    //    setfltvalue(init, luai_numsub(L, ninit, nstep));
    //  }
    //  ci->u.l.savedpc += GETARG_sBx(i);
    //} break;

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_ci_baseT);

  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr;

  //  lua_Integer ilimit;
  //  int stopnow;
  llvm::Value *ilimit = def->builder->CreateAlloca(def->types->lua_IntegerT, nullptr, "ilimit");
  llvm::Value *stopnow = def->builder->CreateAlloca(def->types->C_intT, nullptr, "stopnow");
  llvm::Value *nlimit = def->builder->CreateAlloca(def->types->lua_NumberT, nullptr, "nlimit");
  llvm::Value *ninit = def->builder->CreateAlloca(def->types->lua_NumberT, nullptr, "ninit");
  llvm::Value *nstep = def->builder->CreateAlloca(def->types->lua_NumberT, nullptr, "nstep");

  //  TValue *init = ra;
  //  TValue *plimit = ra + 1;
  //  TValue *pstep = ra + 2;
  llvm::Value *init = A == 0 ? base_ptr : emit_array_get(def, base_ptr, A);
  llvm::Value *plimit = emit_array_get(def, base_ptr, A + 1);
  llvm::Value *pstep = emit_array_get(def, base_ptr, A + 2);
  //  if (ttisinteger(init) && ttisinteger(pstep) &&
  //    forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {
  llvm::Value *tt = emit_gep(def, "tt_", init, 0, 1);
  llvm::Instruction *tt_i = def->builder->CreateLoad(tt);
  tt_i->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  llvm::Value *cmp1 = def->builder->CreateICmpEQ(tt_i, def->types->kInt[LUA_TNUMINT]);
  llvm::Value *tt2 = emit_gep(def, "tt_", pstep, 0, 1);
  llvm::Instruction *tt_j = def->builder->CreateLoad(tt2);
  tt_j->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  llvm::Value *icmp2 = def->builder->CreateICmpEQ(tt_j, def->types->kInt[LUA_TNUMINT]);
  llvm::Value *pstep_ivalue_ptr = def->builder->CreateBitCast(pstep, def->types->plua_IntegerT);
  llvm::Instruction *pstep_ivalue = def->builder->CreateLoad(pstep_ivalue_ptr, "pstep_ivalue");
  pstep_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Value *forlimit_ret = def->builder->CreateCall4(def->luaV_forlimitF, plimit, ilimit, pstep_ivalue, stopnow);
  llvm::Value *and1 = def->builder->CreateAnd(cmp1, icmp2);
  llvm::Value *tobool = def->builder->CreateICmpNE(forlimit_ret, def->types->kInt[0]);
  llvm::Value *and2 = def->builder->CreateAnd(and1, tobool);
  llvm::BasicBlock *then1 =
    llvm::BasicBlock::Create(def->jitState->context(), "if.then", def->f);
  llvm::BasicBlock *else1 =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else");
  def->builder->CreateCondBr(and2, then1, else1);
  def->builder->SetInsertPoint(then1);

  //    /* all values are integer */
  //    lua_Integer initv = (stopnow ? 0 : ivalue(init));
  llvm::Instruction *stopnow_val = def->builder->CreateLoad(stopnow, "stopnow_val");
  stopnow_val->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_intT);
  llvm::Value *stopnow_is_zero = def->builder->CreateICmpEQ(stopnow_val, def->types->kInt[0]);
  llvm::Value *init_value_ptr = def->builder->CreateBitCast(init, def->types->plua_IntegerT, "init_ivalue_ptr");

  llvm::BasicBlock *then1_iffalse =
    llvm::BasicBlock::Create(def->jitState->context(), "if.then.iffalse", def->f);
  llvm::BasicBlock *then1_iftrue =
    llvm::BasicBlock::Create(def->jitState->context(), "if.then.iftrue");
  def->builder->CreateCondBr(stopnow_is_zero, then1_iffalse, then1_iftrue);
  def->builder->SetInsertPoint(then1_iffalse);
  llvm::Instruction *init_ivalue = def->builder->CreateLoad(init_value_ptr, "init_ivalue");
  init_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  def->builder->CreateBr(then1_iftrue);
  def->f->getBasicBlockList().push_back(then1_iftrue);
  def->builder->SetInsertPoint(then1_iftrue);

  auto phi1 = def->builder->CreatePHI(def->types->lua_IntegerT, 2);
  phi1->addIncoming(init_ivalue, then1_iffalse);
  phi1->addIncoming(def->types->kluaInteger[0], then1);
  //    setivalue(plimit, ilimit);
  llvm::Instruction *ilimit_val = def->builder->CreateLoad(ilimit, "ilimit");
  ilimit_val->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
  llvm::Value *plimit_ivalue_ptr = def->builder->CreateBitCast(init, def->types->plua_IntegerT, "plimit_ivalue_ptr");
  llvm::Instruction *plimit_value = def->builder->CreateStore(ilimit_val, plimit_ivalue_ptr);
  plimit_value->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Value *plimit_tt_ptr = emit_gep(def, "plimit.tt_", plimit, 0, 1);
  llvm::Instruction *plimit_tt = def->builder->CreateStore(def->types->kInt[LUA_TNUMINT], plimit_tt_ptr);
  plimit_tt->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  //    setivalue(init, initv - ivalue(pstep));
  // we aleady know init is LUA_TNUMINT
  pstep_ivalue = def->builder->CreateLoad(pstep_ivalue_ptr, "pstep_ivalue");
  pstep_ivalue->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Value *sub = def->builder->CreateSub(phi1, pstep_ivalue, "sub", false, true);
  llvm::Instruction *init_ivalue_store = def->builder->CreateStore(sub, init_value_ptr);
  init_ivalue_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  lua_assert(def->jmp_targets[pc]);
  def->builder->CreateBr(def->jmp_targets[pc]);

  def->f->getBasicBlockList().push_back(else1);
  def->builder->SetInsertPoint(else1);

  // PLIMIT

  plimit_tt_ptr = emit_gep(def, "plimit.tt_", plimit, 0, 1);
  plimit_tt = def->builder->CreateLoad(plimit_tt_ptr);
  plimit_tt->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  cmp1 = def->builder->CreateICmpEQ(plimit_tt, def->types->kInt[LUA_TNUMFLT]);
  llvm::BasicBlock *else1_plimit_ifnum =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.plimit.ifnum", def->f);
  llvm::BasicBlock *else1_plimit_elsenum =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.plimit.elsenum");
  def->builder->CreateCondBr(cmp1, else1_plimit_ifnum, else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_ifnum);

  llvm::Value *plimit_nvalue_ptr = def->builder->CreateBitCast(init, def->types->plua_NumberT, "plimit.n.ptr");
  llvm::Instruction *plimit_nvalue_load = def->builder->CreateLoad(plimit_nvalue_ptr);
  plimit_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Instruction *nlimit_store = def->builder->CreateStore(plimit_nvalue_load, nlimit);
  nlimit_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

  llvm::BasicBlock *else1_pstep =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.pstep");
  def->builder->CreateBr(else1_pstep);

  def->f->getBasicBlockList().push_back(else1_plimit_elsenum);
  def->builder->SetInsertPoint(else1_plimit_elsenum);

  llvm::Value *plimit_isnum = def->builder->CreateCall2(def->luaV_tonumberF, plimit, nlimit);
  llvm::Value *plimit_isnum_bool = def->builder->CreateICmpEQ(plimit_isnum, def->types->kInt[0], "plimit.isnum");
  llvm::BasicBlock *else1_plimit_tonum_elsenum =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.plimit.tonum.ifnum", def->f);
  def->builder->CreateCondBr(plimit_isnum_bool, else1_plimit_tonum_elsenum, else1_pstep);

  def->builder->SetInsertPoint(else1_plimit_tonum_elsenum);
  llvm::Value *errmsg1 = def->builder->CreateGlobalString("'for' limit must be a number");
  def->builder->CreateCall2(def->luaG_runerrorF, def->L, emit_gep(def, "", errmsg1, 0, 0));
  def->builder->CreateBr(else1_pstep);

  def->f->getBasicBlockList().push_back(else1_pstep);
  def->builder->SetInsertPoint(else1_pstep);
  llvm::Instruction *nlimit_load = def->builder->CreateLoad(nlimit);
  nlimit_load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
  llvm::Value *plimit_n = def->builder->CreateBitCast(plimit, def->types->plua_NumberT, "plimit.n.ptr");
  llvm::Instruction *plimit_store = def->builder->CreateStore(nlimit_load, plimit_n);
  plimit_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Instruction *plimit_tt_store = def->builder->CreateStore(def->types->kInt[LUA_TNUMFLT], plimit_tt_ptr);
  plimit_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

  ///   PSTEP

  llvm::Instruction *pstep_tt = def->builder->CreateLoad(tt2);
  pstep_tt->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  cmp1 = def->builder->CreateICmpEQ(pstep_tt, def->types->kInt[LUA_TNUMFLT]);
  llvm::BasicBlock *else1_pstep_ifnum =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.pstep.ifnum", def->f);
  llvm::BasicBlock *else1_pstep_elsenum =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.pstep.elsenum");
  def->builder->CreateCondBr(cmp1, else1_pstep_ifnum, else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_ifnum);

  llvm::Value *pstep_nvalue_ptr = def->builder->CreateBitCast(pstep, def->types->plua_NumberT, "pstep.n.ptr");
  llvm::Instruction *pstep_nvalue_load = def->builder->CreateLoad(pstep_nvalue_ptr);
  pstep_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Instruction *nstep_store = def->builder->CreateStore(pstep_nvalue_load, nstep);
  nstep_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

  llvm::BasicBlock *else1_pinit =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.pinit");
  def->builder->CreateBr(else1_pinit);

  def->f->getBasicBlockList().push_back(else1_pstep_elsenum);
  def->builder->SetInsertPoint(else1_pstep_elsenum);

  llvm::Value *pstep_isnum = def->builder->CreateCall2(def->luaV_tonumberF, pstep, nstep);
  llvm::Value *pstep_isnum_bool = def->builder->CreateICmpEQ(pstep_isnum, def->types->kInt[0], "pstep.isnum");
  llvm::BasicBlock *else1_pstep_tonum_elsenum =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.pstep.tonum.ifnum", def->f);
  def->builder->CreateCondBr(pstep_isnum_bool, else1_pstep_tonum_elsenum, else1_pinit);

  def->builder->SetInsertPoint(else1_pstep_tonum_elsenum);
  errmsg1 = def->builder->CreateGlobalString("'for' step must be a number");
  def->builder->CreateCall2(def->luaG_runerrorF, def->L, emit_gep(def, "", errmsg1, 0, 0));
  def->builder->CreateBr(else1_pinit);

  def->f->getBasicBlockList().push_back(else1_pinit);
  def->builder->SetInsertPoint(else1_pinit);
  llvm::Instruction *nstep_load = def->builder->CreateLoad(nstep);
  nstep_load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
  llvm::Value *pstep_n = def->builder->CreateBitCast(pstep, def->types->plua_NumberT, "pstep.n.ptr");
  llvm::Instruction *pstep_store = def->builder->CreateStore(nstep_load, pstep_n);
  pstep_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Instruction *pstep_tt_store = def->builder->CreateStore(def->types->kInt[LUA_TNUMFLT], tt2);
  pstep_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

  ///   PINIT

  llvm::Instruction *pinit_tt = def->builder->CreateLoad(tt);
  pinit_tt->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  cmp1 = def->builder->CreateICmpEQ(pinit_tt, def->types->kInt[LUA_TNUMFLT]);
  llvm::BasicBlock *else1_pinit_ifnum =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.pinit.ifnum", def->f);
  llvm::BasicBlock *else1_pinit_elsenum =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.pinit.elsenum");
  def->builder->CreateCondBr(cmp1, else1_pinit_ifnum, else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_ifnum);

  llvm::Value *pinit_nvalue_ptr = def->builder->CreateBitCast(init, def->types->plua_NumberT, "pinit.n.ptr");
  llvm::Instruction *pinit_nvalue_load = def->builder->CreateLoad(pinit_nvalue_ptr);
  pinit_nvalue_load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Instruction *ninit_store = def->builder->CreateStore(pinit_nvalue_load, ninit);
  ninit_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);

  llvm::BasicBlock *else1_pdone =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.pdone");
  def->builder->CreateBr(else1_pdone);

  def->f->getBasicBlockList().push_back(else1_pinit_elsenum);
  def->builder->SetInsertPoint(else1_pinit_elsenum);

  llvm::Value *pinit_isnum = def->builder->CreateCall2(def->luaV_tonumberF, init, ninit);
  llvm::Value *pinit_isnum_bool = def->builder->CreateICmpEQ(pinit_isnum, def->types->kInt[0], "pinit.isnum");
  llvm::BasicBlock *else1_pinit_tonum_elsenum =
    llvm::BasicBlock::Create(def->jitState->context(), "if.else.pinit.tonum.ifnum", def->f);
  def->builder->CreateCondBr(pinit_isnum_bool, else1_pinit_tonum_elsenum, else1_pdone);

  def->builder->SetInsertPoint(else1_pinit_tonum_elsenum);
  errmsg1 = def->builder->CreateGlobalString("'for' initial value must be a number");
  def->builder->CreateCall2(def->luaG_runerrorF, def->L, emit_gep(def, "", errmsg1, 0, 0));
  def->builder->CreateBr(else1_pdone);

  def->f->getBasicBlockList().push_back(else1_pdone);
  def->builder->SetInsertPoint(else1_pdone);
  llvm::Instruction *ninit_load = def->builder->CreateLoad(ninit);
  ninit_load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
  nstep_load = def->builder->CreateLoad(nstep);
  nstep_load->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_longlongT);
  llvm::Value *init_n = def->builder->CreateFSub(ninit_load, nstep_load);
  llvm::Value *pinit_n = def->builder->CreateBitCast(init, def->types->plua_NumberT, "pinit.n.ptr");
  llvm::Instruction *pinit_store = def->builder->CreateStore(init_n, pinit_n);
  pinit_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);
  llvm::Instruction *pinit_tt_store = def->builder->CreateStore(def->types->kInt[LUA_TNUMFLT], tt);
  pinit_tt_store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);
  def->builder->CreateBr(def->jmp_targets[pc]);

  def->f->dump();
  assert(false);
}

void RaviCodeGenerator::emit_LOADK(RaviFunctionDef *def, llvm::Value *L_ci,
                                   llvm::Value *proto, int A, int Bx) {
  //    case OP_LOADK: {
  //      TValue *rb = k + GETARG_Bx(i);
  //      setobj2s(L, ra, rb);
  //    } break;

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_ci_baseT);

  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr; 

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

#if 1
  // destvalue->value->i = srcvalue->value->i;
  llvm::Value *srcvalue = emit_gep(def, "srcvalue", src, 0, 0, 0);
  llvm::Value *destvalue = emit_gep(def, "destvalue", dest, 0, 0, 0);
  llvm::Instruction *store = def->builder->CreateStore(def->builder->CreateLoad(srcvalue), destvalue);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_nT);

  // destvalue->type = srcvalue->type
  llvm::Value *srctype = emit_gep(def, "srctype", src, 0, 1);
  llvm::Value *desttype = emit_gep(def, "desttype", dest, 0, 1);
  store = def->builder->CreateStore(def->builder->CreateLoad(srctype), desttype);
  store->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_TValue_ttT);

#else
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
#endif
}

void RaviCodeGenerator::emit_RETURN(RaviFunctionDef *def, llvm::Value *L_ci,
                                    llvm::Value *proto, int A, int B) {

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

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_ci_baseT);

  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr; 

  llvm::Value *top = nullptr;

  // Get pointer to register A
  llvm::Value *ra_ptr = A == 0 ? base_ptr : emit_array_get(def, base_ptr, A);

  //*  if (b != 0) L->top = ra + b - 1;
  if (B != 0) {
    // Get pointer to register at ra + b - 1
    llvm::Value *ptr = emit_array_get(def, base_ptr, A + B - 1);
    // Get pointer to L->top
    top = emit_gep(def, "L_top", def->L, 0, 4);
    // Assign to L->top
    llvm::Instruction *ins = def->builder->CreateStore(ptr, top);
    ins->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_topT);
  }

  // if (cl->p->sizep > 0) luaF_close(L, base);
  // Get pointer to Proto->sizep
  llvm::Value *psize_ptr = emit_gep(def, "sizep", def->proto_ptr, 0, 10);
  // Load psize
  llvm::Instruction *psize = def->builder->CreateLoad(psize_ptr);
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
  llvm::Value *citop = emit_gep(def, "ci_top", def->ci_val, 0, 1);
  // Load ci->top
  llvm::Instruction *citop_val = def->builder->CreateLoad(citop);
  if (!top)
    // Get L->top
    top = emit_gep(def, "L_top", def->L, 0, 4);
  // Assign ci>top to L->top
  llvm::Instruction *ins2 = def->builder->CreateStore(citop_val, top);
  def->builder->CreateBr(ElseBB);
  def->f->getBasicBlockList().push_back(ElseBB);
  def->builder->SetInsertPoint(ElseBB);

  // as our prototype is lua_Cfunction we need
  // to return a value
  def->builder->CreateRet(def->types->kInt[1]);
}

void RaviCodeGenerator::emit_EQ(RaviFunctionDef *def, llvm::Value *L_ci,
                                llvm::Value *proto, int A, int B, int C, int j,
                                int jA, llvm::Constant *callee) {
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

  // Load pointer to base
  llvm::Instruction *base_ptr = def->builder->CreateLoad(def->Ci_base);
  base_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_ci_baseT);

  // Load pointer to k
  llvm::Value *k_ptr = def->k_ptr;

  llvm::Value *lhs_ptr;
  llvm::Value *rhs_ptr;

  // Get pointer to register B
  llvm::Value *base_or_k = ISK(B) ? k_ptr : base_ptr;
  int b = ISK(B) ? INDEXK(B) : B;
  if (b == 0) {
    lhs_ptr = base_or_k;
  } else {
    lhs_ptr = emit_array_get(def, base_or_k, b);
  }

  // Get pointer to register C
  base_or_k = ISK(C) ? k_ptr : base_ptr;
  int c = ISK(C) ? INDEXK(C) : C;
  if (c == 0) {
    rhs_ptr = base_or_k;
  } else {
    rhs_ptr = emit_array_get(def, base_or_k, c);
  }

  // Call luaV_equalobj with register B and C
  llvm::Value *result =
      def->builder->CreateCall3(callee, def->L, lhs_ptr, rhs_ptr);
  // Test if result is equal to operand A
  llvm::Value *result_eq_A = def->builder->CreateICmpEQ(
      result, llvm::ConstantInt::get(def->types->C_intT, A));
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

    // Load pointer to base
    llvm::Instruction *base2_ptr = def->builder->CreateLoad(def->Ci_base);
    base2_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def->types->tbaa_luaState_ci_baseT);

    // base + a - 1
    llvm::Value *val =
        jA == 1 ? base2_ptr : emit_array_get(def, base2_ptr, jA - 1);

    // Call
    def->builder->CreateCall2(def->luaF_closeF, def->L, val);
  }
  // Do the jump
  def->builder->CreateBr(def->jmp_targets[j]);
  // Add the else block and make it current so that the next instruction flows
  // here
  def->f->getBasicBlockList().push_back(else_block);
  def->builder->SetInsertPoint(else_block);
}

// Check if we can compile
// The cases we can compile will increase over time
bool RaviCodeGenerator::canCompile(Proto *p) {
  if (p->ravi_jit.jit_status == 1)
    return false;
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
    case OP_LT:
    case OP_LE:
#if 0
    case OP_FORPREP:
    case OP_FORLOOP:
    case OP_MOVE:
#endif
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

  return func;
}

void RaviCodeGenerator::emit_extern_declarations(RaviFunctionDef *def) {
  // Add extern declarations for Lua functions that we need to call
  def->luaD_poscallF = def->raviF->addExternFunction(
      def->types->luaD_poscallT, reinterpret_cast<void *>(&luaD_poscall), "luaD_poscall");
  def->luaF_closeF = def->raviF->addExternFunction(def->types->luaF_closeT,
                                                   reinterpret_cast<void *>(&luaF_close), "luaF_close");
  def->luaV_equalobjF = def->raviF->addExternFunction(
      def->types->luaV_equalobjT, reinterpret_cast<void *>(&luaV_equalobj), "luaV_equalobj");
  def->luaV_lessthanF = def->raviF->addExternFunction(
      def->types->luaV_lessthanT, reinterpret_cast<void *>(&luaV_lessthan), "luaV_lessthan");
  def->luaV_lessequalF = def->raviF->addExternFunction(
      def->types->luaV_lessequalT, reinterpret_cast<void *>(&luaV_lessequal), "luaV_lessequal");
  def->luaG_runerrorF = def->raviF->addExternFunction(
    def->types->luaG_runerrorT, reinterpret_cast<void *>(&luaG_runerror), "luaG_runerror");
  def->luaV_forlimitF = def->raviF->addExternFunction(
    def->types->luaV_forlimitT, reinterpret_cast<void *>(&luaV_forlimit), "luaV_forlimit");
  def->luaV_tonumberF = def->raviF->addExternFunction(
    def->types->luaV_tonumberT, reinterpret_cast<void *>(&luaV_tonumber_), "luaV_tonumber_");
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
  // then we need to insert the block we previously created in
  // scan_jump_targets()
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
  RaviFunctionDef def = {0};

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
  llvm::Value *L_ci = emit_gep(&def, "L_ci", def.L, 0, 6);

  // Load L->ci
  def.ci_val = builder.CreateLoad(L_ci);
  def.ci_val->setMetadata(llvm::LLVMContext::MD_tbaa, def.types->tbaa_CallInfoT); 

  // Get pointer to base
  def.Ci_base = emit_gep(&def, "base", def.ci_val, 0, 4, 0);

  // We need to get hold of the constants table
  // which is located in ci->func->value_.gc
  // and is a value of type LClosure*
  // fortunately as func is at the beginning of the ci
  // structure we can just cast ci to LClosure*
  llvm::Value *L_cl = emit_gep_ci_func_value_gc_asLClosure(&def, def.ci_val);

  // Load pointer to LClosure
  llvm::Instruction *cl_ptr = builder.CreateLoad(L_cl);
  cl_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def.types->tbaa_CallInfo_func_LClosureT);

  // Get pointer to the Proto* which is cl->p
  llvm::Value *proto = emit_gep(&def, "Proto", cl_ptr, 0, 5);

  // Load pointer to proto
  def.proto_ptr = builder.CreateLoad(proto);
  def.proto_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def.types->tbaa_LClosure_pT);

  // Obtain pointer to Proto->k
  def.proto_k = emit_gep(&def, "k", def.proto_ptr, 0, 14);

  // Load pointer to k
  def.k_ptr = builder.CreateLoad(def.proto_k);
  def.k_ptr->setMetadata(llvm::LLVMContext::MD_tbaa, def.types->tbaa_Proto_kT);
  
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
    case OP_LT:
    case OP_LE:
    case OP_EQ: {
      int B = GETARG_B(i);
      int C = GETARG_C(i);
      llvm::Constant *comparison_function =
          (op == OP_EQ
               ? def.luaV_equalobjF
               : (op == OP_LT ? def.luaV_lessthanF : def.luaV_lessequalF));
      // OP_EQ is followed by OP_JMP - we process this
      // along with OP_EQ
      pc++;
      i = code[pc];
      op = GET_OPCODE(i);
      lua_assert(op == OP_JMP);
      int sbx = GETARG_sBx(i);
      // j below is the jump target
      int j = sbx + pc + 1;
      emit_EQ(&def, L_ci, proto, A, B, C, j, GETARG_A(i), comparison_function);
    } break;
    case OP_JMP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      emit_JMP(&def, j);
    } break;
    case OP_FORPREP: {
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      emit_FORPREP(&def, L_ci, proto, A, j);
    } break;
    case OP_FORLOOP: {
      assert(false);
    } break;
    default:
      break;
    }
  }

  llvm::verifyFunction(*f->function());
  ravi::RaviJITFunctionImpl *llvm_func = f.release();
  p->ravi_jit.jit_data = reinterpret_cast<void *>(llvm_func);
  p->ravi_jit.jit_function = (lua_CFunction)llvm_func->compile();
  lua_assert(p->ravi_jit.jit_function);
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

void RaviCodeGenerator::scan_jump_targets(RaviFunctionDef *def, Proto *p) {
  // We need to precreate blocks for jump targets so that we
  // can generate branch instructions in the code
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
      const char *targetname = nullptr;
      char temp[80];
      if (op == OP_JMP)
        targetname = "jmp";
      else if (op == OP_FORLOOP)
        targetname = "forbody";
      else if (op == OP_FORPREP)
        targetname = "forloop";
      else
        targetname = "tforbody";
      int sbx = GETARG_sBx(i);
      int j = sbx + pc + 1;
      snprintf(temp, sizeof temp, "%s%d_", targetname, j+1);
      def->jmp_targets[j] =
          llvm::BasicBlock::Create(def->jitState->context(), temp);
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

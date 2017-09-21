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
#include "ravi_llvmcodegen.h"

namespace ravi {

LuaLLVMTypes::LuaLLVMTypes(llvm::LLVMContext &context) : mdbuilder(context) {
  C_voidT = llvm::Type::getVoidTy(context);

  C_floatT = llvm::Type::getFloatTy(context);
  C_doubleT = llvm::Type::getDoubleTy(context);

#ifdef LUA_32BITS
  // issue #93 add support for 32-bit Lua
  lua_NumberT = C_floatT;
  static_assert(std::is_floating_point<lua_Number>::value &&
                    sizeof(lua_Number) == sizeof(float),
                "lua_Number is not a float");
  static_assert(sizeof(lua_Integer) == sizeof(lua_Number) &&
                    sizeof(lua_Integer) == sizeof(int32_t),
                "lua_Integer size (32-bit) does not match lua_Number");
#else
  lua_NumberT = C_doubleT;
  static_assert(std::is_floating_point<lua_Number>::value &&
                    sizeof(lua_Number) == sizeof(double),
                "lua_Number is not a double");
  static_assert(sizeof(lua_Integer) == sizeof(lua_Number) &&
                    sizeof(lua_Integer) == sizeof(int64_t),
                "lua_Integer size (64-bit) does not match lua_Number");
#endif

  plua_NumberT = llvm::PointerType::get(lua_NumberT, 0);
  pplua_NumberT = llvm::PointerType::get(plua_NumberT, 0);

  static_assert(std::is_integral<lua_Integer>::value,
                "lua_Integer is not an integer type");
  lua_IntegerT = llvm::Type::getIntNTy(context, sizeof(lua_Integer) * 8);
  plua_IntegerT = llvm::PointerType::get(lua_IntegerT, 0);
  pplua_IntegerT = llvm::PointerType::get(plua_IntegerT, 0);

  static_assert(sizeof(lua_Integer) == sizeof(lua_Unsigned),
                "lua_Integer and lua_Unsigned are of different size");
  lua_UnsignedT = lua_IntegerT;

  C_intptr_t = llvm::Type::getIntNTy(context, sizeof(intptr_t) * 8);
  C_size_t = llvm::Type::getIntNTy(context, sizeof(size_t) * 8);
  C_psize_t = llvm::PointerType::get(C_size_t, 0);
  C_ptrdiff_t = llvm::Type::getIntNTy(context, sizeof(ptrdiff_t) * 8);
  C_int64_t = llvm::Type::getIntNTy(context, sizeof(int64_t) * 8);
  C_intT = llvm::Type::getIntNTy(context, sizeof(int) * 8);
  C_pintT = llvm::PointerType::get(C_intT, 0);
  C_shortT = llvm::Type::getIntNTy(context, sizeof(short) * 8);

  static_assert(sizeof(size_t) == sizeof(lu_mem),
                "lu_mem size is not same as size_t");
  lu_memT = C_size_t;

  static_assert(sizeof(ptrdiff_t) == sizeof(l_mem),
                "l_mem size is not same as ptrdiff_t");
  l_memT = C_ptrdiff_t;

  tmsT = C_intT;

  static_assert(sizeof(L_Umaxalign) == sizeof(double),
                "L_Umaxalign is not same size as double");
  L_UmaxalignT = C_doubleT;

  static_assert(sizeof(lu_byte) == sizeof(char),
                "lu_byte is not same size as char");
  lu_byteT = llvm::Type::getInt8Ty(context);
  C_pcharT = llvm::Type::getInt8PtrTy(context);

  static_assert(sizeof(Instruction) == sizeof(int),
                "Instruction is not same size as int");
  InstructionT = C_intT;
  pInstructionT = llvm::PointerType::get(InstructionT, 0);

  ravitype_tT = llvm::Type::getIntNTy(context, sizeof(ravitype_t) * 8);

  lua_StateT = llvm::StructType::create(context, "struct.lua_State");
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

  lua_DebugT = llvm::StructType::create(context, "struct.lua_Debug");
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
  GCObjectT = llvm::StructType::create(context, "struct.GCObject");
  pGCObjectT = llvm::PointerType::get(GCObjectT, 0);
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  GCObjectT->setBody(elements);

  static_assert(sizeof(Value) == sizeof(lua_Number) &&
                    sizeof(Value) == sizeof(lua_Integer),
                "Value type is larger than lua_Number");
  static_assert(sizeof(TValue) == sizeof(lua_Number) * 2,
                "TValue type is not 2*sizeof(lua_Number)");
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
  ValueT = llvm::StructType::create(context, "union.Value");
  elements.clear();
  elements.push_back(lua_NumberT);
  ValueT->setBody(elements);

  // NOTE: Following structure changes when NaN tagging is enabled
  // struct TValue {
  //   union Value value_;
  //   int tt_;
  // };
  TValueT = llvm::StructType::create(context, "struct.TValue");
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
  //   lu_byte shrlen;  /* length for short strings */
  //   unsigned int hash;
  //   union {
  //     size_t lnglen;  /* length for long strings */
  //     struct TString *hnext;  /* linked list for hash table */
  //   } u;
  // } TString;

  ///*
  //** Ensures that address after this type is always fully aligned.
  //*/
  // typedef union UTString {
  //  L_Umaxalign dummy;  /* ensures maximum alignment for strings */
  //  TString tsv;
  //} UTString;
  TStringT = llvm::StructType::create(context, "struct.TString");
  pTStringT = llvm::PointerType::get(TStringT, 0);
  ppTStringT = llvm::PointerType::get(pTStringT, 0);
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT); /* extra */
  elements.push_back(lu_byteT); /* shrlen */
  elements.push_back(C_intT);   /* hash */
  elements.push_back(C_size_t); /* len (as this is the larger member) */
  TStringT->setBody(elements);

  // Table
  TableT = llvm::StructType::create(context, "struct.Table");
  pTableT = llvm::PointerType::get(TableT, 0);
  ppTableT = llvm::PointerType::get(pTableT, 0);

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
  UdataT = llvm::StructType::create(context, "struct.Udata");
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
  //  ravitype_t type;
  //  lu_byte instack;  /* whether it is in stack */
  //  lu_byte idx;  /* index of upvalue (in stack or in outer function's list)
  //  */
  //}Upvaldesc;
  UpvaldescT = llvm::StructType::create(context, "struct.Upvaldesc");
  elements.clear();
  elements.push_back(pTStringT);   /* name */
  elements.push_back(ravitype_tT); /* type */
  elements.push_back(lu_byteT);    /* instack */
  elements.push_back(lu_byteT);    /* idx */
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
  LocVarT = llvm::StructType::create(context, "struct.LocVar");
  elements.clear();
  elements.push_back(pTStringT);   /* varname */
  elements.push_back(C_intT);      /* startpc */
  elements.push_back(C_intT);      /* endpc */
  elements.push_back(ravitype_tT); /* ravi_type */
  LocVarT->setBody(elements);
  pLocVarT = llvm::PointerType::get(LocVarT, 0);

  LClosureT = llvm::StructType::create(context, "struct.LClosure");
  pLClosureT = llvm::PointerType::get(LClosureT, 0);
  ppLClosureT = llvm::PointerType::get(pLClosureT, 0);
  pppLClosureT = llvm::PointerType::get(ppLClosureT, 0);

  RaviJITProtoT = llvm::StructType::create(context, "struct.RaviJITProto");
  elements.clear();
  elements.push_back(lu_byteT);        /* jit_status*/
  elements.push_back(lu_byteT);        /* jit_flags */
  elements.push_back(C_shortT);        /* execution_count */
  elements.push_back(C_pcharT);        /* jit_data */
  elements.push_back(plua_CFunctionT); /* jit_function */
  RaviJITProtoT->setBody(elements);

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
  //  RaviJITProto ravi_jit;
  //} Proto;

  ProtoT = llvm::StructType::create(context, "struct.Proto");
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
  elements.push_back(RaviJITProtoT);                     /* ravi_jit */
  ProtoT->setBody(elements);

  ///*
  //** Lua Upvalues
  //*/
  // typedef struct UpVal UpVal;
  UpValT = llvm::StructType::create(context, "struct.UpVal");
  pUpValT = llvm::PointerType::get(UpValT, 0);

  ///*
  //** Closures
  //*/

  //#define ClosureHeader CommonHeader; lu_byte nupvalues; GCObject *gclist

  // typedef struct CClosure {
  //  ClosureHeader;
  //  lua_CFunction f;
  //  TValue upvalue[1];  /* list of upvalues */
  //} CClosure;

  CClosureT = llvm::StructType::create(context, "struct.CClosure");
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

  // NOTE following structure changes when NaN Tagging is enabled

  // typedef union TKey {
  //  struct {
  //    TValuefields;
  //    int next;  /* for chaining (offset for next node) */
  //  } nk;
  //  TValue tvk;
  //} TKey;
  TKeyT = llvm::StructType::create(context, "struct.TKey");
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
  NodeT = llvm::StructType::create(context, "struct.Node");
  elements.clear();
  elements.push_back(TValueT); /* i_val */
  elements.push_back(TKeyT);   /* i_key */
  NodeT->setBody(elements);
  pNodeT = llvm::PointerType::get(NodeT, 0);

  // typedef struct RaviArray {
  //  char *data;
  //  unsigned int len; /* RAVI len specialization */
  //  unsigned int size; /* amount of memory allocated */
  //  lu_byte array_type; /* RAVI specialization */
  //  lu_byte array_modifier; /* Flags that affect how the array is handled */
  //} RaviArray;

  RaviArrayT = llvm::StructType::create(context, "struct.RaviArray");
  elements.clear();
  elements.push_back(C_pcharT);
  elements.push_back(C_intT);
  elements.push_back(C_intT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  RaviArrayT->setBody(elements);

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
  //  RaviArray ravi_array;
  //  unsigned int hmask; /* Hash part mask (size of hash part - 1) */
  //} Table;
  elements.clear();
  elements.push_back(pGCObjectT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);
  elements.push_back(lu_byteT);   /* flags */
  elements.push_back(lu_byteT);   /* lsizenode */
  elements.push_back(C_intT);     /* sizearray */
  elements.push_back(pTValueT);   /* array part */
  elements.push_back(pNodeT);     /* node */
  elements.push_back(pNodeT);     /* lastfree */
  elements.push_back(pTableT);    /* metatable */
  elements.push_back(pGCObjectT); /* gclist */
  elements.push_back(RaviArrayT); /* RaviArray */
#if RAVI_USE_NEWHASH
  elements.push_back(C_intT);     /* hmask  */
#endif
  TableT->setBody(elements);

  // struct lua_longjmp;  /* defined in ldo.c */
  lua_longjumpT = llvm::StructType::create(context, "struct.lua_longjmp");
  plua_longjumpT = llvm::PointerType::get(lua_longjumpT, 0);

  // lzio.h
  // typedef struct Mbuffer {
  //  char *buffer;
  //  size_t n;
  //  size_t buffsize;
  //} Mbuffer;
  MbufferT = llvm::StructType::create(context, "struct.Mbuffer");
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
  stringtableT = llvm::StructType::create(context, "struct.stringtable");
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
  //  unsigned short callstatus;
  //  unsigned short stacklevel; /* Ravi extension - stack level, bottom level
  //  is 0 */
  //  lu_byte jitstatus; /* Only valid if Lua function - if 1 means JITed - RAVI
  //  extension */
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

  CallInfoT = llvm::StructType::create(context, "struct.CallInfo");
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
  elements.push_back(C_shortT);                        /* callstatus */
  elements.push_back(C_shortT); /* stacklevel RAVI extension */
  elements.push_back(lu_byteT); /* jitstatus RAVI extension*/
  CallInfoT->setBody(elements);

  // typedef struct ravi_State ravi_State;

  ravi_StateT = llvm::StructType::create(context, "struct.ravi_State");
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
  //  TString *strcache[STRCACHE_SIZE][1];  /* cache for strings in API */
  //  /* RAVI */
  //  ravi_State *ravi_state;
  //} global_State;

  global_StateT = llvm::StructType::create(context, "struct.global_State");
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
  //  unsigned short nci;  /* number of items in 'ci' list */
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
  elements.push_back(C_shortT);
  lua_StateT->setBody(elements);

  // struct UpVal {
  //  struct TValue *v;  /* points to stack or to its own value */
  //  unsigned long long refcount;  /* reference counter */
  //  union {
  //    struct {  /* (when open) */
  //      struct UpVal *next;  /* linked list */
  //      int touched;  /* mark to avoid cycles with dead threads */
  //    } open;
  //    struct TValue value;  /* the value (when closed) */
  //  } u;
  //};
  elements.clear();
  elements.push_back(pTValueT);
  elements.push_back(C_size_t);
  elements.push_back(TValueT);
  UpValT->setBody(elements);

  // int luaD_poscall (lua_State *L, CallInfo *ci, StkId firstResult, int nres);
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pCallInfoT);
  elements.push_back(StkIdT);
  elements.push_back(C_intT);
  luaD_poscallT = llvm::FunctionType::get(C_intT, elements, false);

  // void luaC_upvalbarrier_ (lua_State *L, UpVal *uv)
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pUpValT);
  luaC_upvalbarrierT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // int luaD_precall (lua_State *L, StkId func, int nresults, int op_call);
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(StkIdT);
  elements.push_back(C_intT);
  elements.push_back(C_intT);
  luaD_precallT = llvm::FunctionType::get(C_intT, elements, false);

  // void luaD_call (lua_State *L, StkId func, int nResults);
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(StkIdT);
  elements.push_back(C_intT);
  luaD_callT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // int luaV_execute(lua_State L);
  elements.clear();
  elements.push_back(plua_StateT);
  luaV_executeT = llvm::FunctionType::get(C_intT, elements, false);

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
  luaG_runerrorT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, true);

  // int luaV_forlimit (const TValue *obj, lua_Integer *p, lua_Integer step,
  //                    int *stopnow)
  elements.clear();
  elements.push_back(pTValueT);
  elements.push_back(plua_IntegerT);
  elements.push_back(lua_IntegerT);
  elements.push_back(C_pintT);
  luaV_forlimitT = llvm::FunctionType::get(C_intT, elements, false);

  // int luaV_tonumber_ (const TValue *obj, lua_Number *n)
  elements.clear();
  elements.push_back(pTValueT);
  elements.push_back(plua_NumberT);
  luaV_tonumberT = llvm::FunctionType::get(C_intT, elements, false);

  // int luaV_tointeger_ (const TValue *obj, lua_Integer *p)
  elements.clear();
  elements.push_back(pTValueT);
  elements.push_back(plua_IntegerT);
  luaV_tointegerT = llvm::FunctionType::get(C_intT, elements, false);

  // void luaV_objlen (lua_State *L, StkId ra, const TValue *rb)
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pTValueT);
  elements.push_back(pTValueT);
  luaV_objlenT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val)
  elements.push_back(pTValueT);
  luaV_gettableT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  // void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val)
  luaV_settableT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_gettable_sskeyT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_settable_sskeyT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_gettable_iT =
	  llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_settable_iT =
	  llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // void luaV_finishget (lua_State *L, const TValue *t, TValue *key,
  //                      StkId val, const TValue *slot);
  elements.push_back(pTValueT);
  luaV_finishgetT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // void luaT_trybinTM (lua_State *L, const TValue *p1, const TValue *p2,
  //                     StkId res, TMS event);
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pTValueT);
  elements.push_back(pTValueT);
  elements.push_back(pTValueT);
  elements.push_back(tmsT);
  luaT_trybinTMT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // void raviV_op_loadnil(CallInfo *ci, int a, int b)
  elements.clear();
  elements.push_back(pCallInfoT);
  elements.push_back(C_intT);
  elements.push_back(C_intT);
  raviV_op_loadnilT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra)
  // void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra)
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pCallInfoT);
  elements.push_back(pTValueT);
  raviV_op_newarrayintT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_newarrayfloatT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // void raviV_op_newtable(lua_State *L, CallInfo *ci, TValue *ra, int b, int
  // c)
  // void raviV_op_setlist(lua_State *L, CallInfo *ci, TValue *ra, int b, int c)
  elements.push_back(C_intT);
  elements.push_back(C_intT);
  raviV_op_newtableT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_setlistT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // lua_Integer luaV_div (lua_State *L, lua_Integer m, lua_Integer n)
  // lua_Integer luaV_mod (lua_State *L, lua_Integer m, lua_Integer n)
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(lua_IntegerT);
  elements.push_back(lua_IntegerT);
  luaV_modT = llvm::FunctionType::get(lua_IntegerT, elements, false);
  luaV_divT = llvm::FunctionType::get(lua_IntegerT, elements, false);

  // void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c)
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pCallInfoT);
  elements.push_back(C_intT);
  elements.push_back(C_intT);
  elements.push_back(C_intT);
  raviV_op_concatT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int
  // Bx)
  // void raviV_op_vararg(lua_State *L, CallInfo *ci, LClosure *cl, int a, int
  // b)
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pCallInfoT);
  elements.push_back(pLClosureT);
  elements.push_back(C_intT);
  elements.push_back(C_intT);
  raviV_op_closureT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_varargT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // void raviV_op_setupvali(lua_State *L, LClosure *cl, TValue *ra, int b);
  // void raviV_op_setupvalf(lua_State *L, LClosure *cl, TValue *ra, int b);
  // void raviV_op_setupvalai(lua_State *L, LClosure *cl, TValue *ra, int b);
  // void raviV_op_setupvalaf(lua_State *L, LClosure *cl, TValue *ra, int b);
  // void raviV_op_setupvalt(lua_State *L, LClosure *cl, TValue *ra, int b);
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pLClosureT);
  elements.push_back(pTValueT);
  elements.push_back(C_intT);
  raviV_op_setupvaliT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_setupvalfT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_setupvalaiT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_setupvalafT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_setupvaltT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // void raviV_op_bor(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
  // void raviV_op_bxor(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
  // void raviV_op_band(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
  // void raviV_op_bnot(lua_State *L, TValue *ra, TValue *rb);
  // void raviV_op_shl(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
  // void raviV_op_shr(lua_State *L, TValue *ra, TValue *rb, TValue *rc);
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pTValueT);
  elements.push_back(pTValueT);
  raviV_op_bnotT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  elements.push_back(pTValueT);
  raviV_op_shlT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_shrT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_borT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_bxorT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_bandT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_addT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_subT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_mulT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  raviV_op_divT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // const TValue *luaH_getint(Table *t, lua_Integer key);
  elements.clear();
  elements.push_back(pTableT);
  elements.push_back(lua_IntegerT);
  luaH_getintT = llvm::FunctionType::get(pTValueT, elements, false);

  // void luaH_setint(lua_State *L, Table *t, lua_Integer key, TValue *value);
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pTableT);
  elements.push_back(lua_IntegerT);
  elements.push_back(pTValueT);
  luaH_setintT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // const TValue *luaH_getstr(Table *t, TString *key);
  elements.clear();
  elements.push_back(pTableT);
  elements.push_back(pTStringT);
  luaH_getstrT = llvm::FunctionType::get(pTValueT, elements, false);

  // void raviH_set_int(lua_State *L, Table *t, lua_Unsigned key, lua_Integer
  // value);
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pTableT);
  elements.push_back(lua_UnsignedT);
  elements.push_back(lua_IntegerT);
  raviH_set_intT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  // void raviH_set_float(lua_State *L, Table *t, lua_Unsigned key, lua_Number
  // value);
  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pTableT);
  elements.push_back(lua_UnsignedT);
  elements.push_back(lua_NumberT);
  raviH_set_floatT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(pTValueT);
  ravi_dump_valueT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(C_pcharT);
  ravi_dump_stackT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);
  ravi_dump_stacktopT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  elements.clear();
  elements.push_back(plua_StateT);
  elements.push_back(C_intT);
  elements.push_back(C_intT);
  ravi_debug_traceT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), elements, false);

  for (int j = 0; j < kInt.size(); j++)
    kInt[j] = llvm::ConstantInt::get(C_intT, j);
  for (int j = 0; j < kluaInteger.size(); j++)
    kluaInteger[j] = llvm::ConstantInt::get(lua_IntegerT, j);
  for (int j = 0; j < kByte.size(); j++)
    kByte[j] = llvm::ConstantInt::get(lu_byteT, j);
  kFalse = llvm::ConstantInt::getFalse(llvm::Type::getInt1Ty(context));

  // Do what Clang does
  //! 5 = metadata !{metadata !"Simple C/C++ TBAA"}
  tbaa_root = mdbuilder.createTBAARoot("Simple C / C++ TBAA");

  //! 4 = metadata !{metadata !"omnipotent char", metadata !5, i64 0}
  tbaa_charT =
      mdbuilder.createTBAAScalarTypeNode("omnipotent char", tbaa_root, 0);
  tbaa_pcharT = mdbuilder.createTBAAStructTagNode(tbaa_charT, tbaa_charT, 0);

  //! 3 = metadata !{metadata !"any pointer", metadata !4, i64 0}
  tbaa_pointerT =
      mdbuilder.createTBAAScalarTypeNode("any pointer", tbaa_charT, 0);
  tbaa_ppointerT =
      mdbuilder.createTBAAStructTagNode(tbaa_pointerT, tbaa_pointerT, 0);

  //! 10 = metadata !{metadata !"short", metadata !4, i64 0}
  tbaa_shortT = mdbuilder.createTBAAScalarTypeNode("short", tbaa_charT, 0);
  tbaa_pshortT = mdbuilder.createTBAAStructTagNode(tbaa_shortT, tbaa_shortT, 0);

  //! 11 = metadata !{metadata !"int", metadata !4, i64 0}
  tbaa_intT = mdbuilder.createTBAAScalarTypeNode("int", tbaa_charT, 0);
  tbaa_pintT = mdbuilder.createTBAAStructTagNode(tbaa_intT, tbaa_intT, 0);

  //! 9 = metadata !{metadata !"long long", metadata !4, i64 0}
  tbaa_longlongT =
      mdbuilder.createTBAAScalarTypeNode("long long", tbaa_charT, 0);
  tbaa_plonglongT =
      mdbuilder.createTBAAStructTagNode(tbaa_longlongT, tbaa_longlongT, 0);

  tbaa_doubleT = mdbuilder.createTBAAScalarTypeNode("double", tbaa_charT, 0);
  tbaa_pdoubleT =
      mdbuilder.createTBAAStructTagNode(tbaa_doubleT, tbaa_doubleT, 0);

  //! 14 = metadata !{metadata !"CallInfoL", metadata !3, i64 0, metadata !3,
  //! i64
  // 4, metadata !9, i64 8}
  std::vector<std::pair<llvm::MDNode *, uint64_t>> nodes;
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 4));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_longlongT, 8));
  tbaa_CallInfo_lT = mdbuilder.createTBAAStructTypeNode("CallInfo_l", nodes);

  //! 13 = metadata !{metadata !"CallInfoLua",
  //                 metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8,
  //                 metadata !3, i64 12, metadata !14, i64 16, metadata !9, i64
  //                 32,
  //                 metadata !10, i64 40, metadata !4, i64 42}
  nodes.clear();
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 0));  // func
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 4));  // top
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 8));  // previous
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 12));  // next
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_CallInfo_lT, 16));  // l
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_longlongT, 32));  // extra
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_shortT, 40));  // nresults
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_shortT, 42));  // callstatus
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_shortT, 44));  // stacklevel
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 46));  // jitstatus
  tbaa_CallInfoT = mdbuilder.createTBAAStructTypeNode("CallInfo", nodes);

  //! 7 = metadata !{metadata !"lua_State",
  //                metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5,
  //                metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64 12,
  //                metadata !3, i64 16, metadata !3, i64 20, metadata !3, i64
  //                24,
  //                metadata !3, i64 28, metadata !3, i64 32, metadata !3, i64
  //                36,
  //                metadata !3, i64 40, metadata !3, i64 44, metadata !8, i64
  //                48,
  //                metadata !3, i64 104, metadata !9, i64 112, metadata !11,
  //                i64 120,
  //                metadata !11, i64 124, metadata !11, i64 128, metadata !10,
  //                i64 132,
  //                metadata !10, i64 134, metadata !4, i64 136, metadata !4,
  //                i64 137}
  nodes.clear();
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 4));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 5));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 6));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 8));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 12));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 16));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 20));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 24));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 28));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 32));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 36));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 40));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 44));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_CallInfoT, 48));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 92));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_longlongT, 96));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 104));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 108));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 112));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_shortT, 114));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_shortT, 116));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 118));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 119));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_shortT, 120));
  tbaa_luaStateT = mdbuilder.createTBAAStructTypeNode("lua_State", nodes);

  tbaa_luaState_ciT =
      mdbuilder.createTBAAStructTagNode(tbaa_luaStateT, tbaa_pointerT, 16);
  tbaa_luaState_ci_baseT =
      mdbuilder.createTBAAStructTagNode(tbaa_CallInfoT, tbaa_pointerT, 16);
  tbaa_CallInfo_funcT =
      mdbuilder.createTBAAStructTagNode(tbaa_CallInfoT, tbaa_pointerT, 0);
  tbaa_CallInfo_func_LClosureT =
      mdbuilder.createTBAAStructTagNode(tbaa_pointerT, tbaa_pointerT, 0);
  tbaa_CallInfo_topT =
      mdbuilder.createTBAAStructTagNode(tbaa_CallInfoT, tbaa_pointerT, 4);
  tbaa_CallInfo_savedpcT =
      mdbuilder.createTBAAStructTagNode(tbaa_CallInfoT, tbaa_pointerT, 20);
  tbaa_CallInfo_callstatusT =
      mdbuilder.createTBAAStructTagNode(tbaa_CallInfoT, tbaa_shortT, 42);
  tbaa_CallInfo_jitstatusT =
      mdbuilder.createTBAAStructTagNode(tbaa_CallInfoT, tbaa_charT, 46);

  //! 20 = metadata !{metadata !"Proto",
  //                 metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5,
  //                 metadata !4, i64 6, metadata !4, i64 7, metadata !4, i64 8,
  //                 metadata !11, i64 12, metadata !11, i64 16, metadata !11,
  //                 i64 20,
  //                 metadata !11, i64 24, metadata !11, i64 28, metadata !11,
  //                 i64 32,
  //                 metadata !11, i64 36, metadata !11, i64 40, metadata !3,
  //                 i64 44,
  //                 metadata !3, i64 48, metadata !3, i64 52, metadata !3, i64
  //                 56,
  //                 metadata !3, i64 60, metadata !3, i64 64, metadata !3, i64
  //                 68,
  //                 metadata !3, i64 72, metadata !3, i64 76}
  nodes.clear();
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 0));           // next
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 4));  // tt
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 5));  // marked
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 6));  // numparams
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 7));  // is_vararg
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 8));  // maxstacksize
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 12));  // sizeupvalues
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 16));  // sizek
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 20));  // sizecode
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 24));  // sizelineinfo
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 28));  // sizep
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 32));  // sizelocvars
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 36));  // linedefined
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 40));  // lastlinedefined
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 44));  // k
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 48));  // code
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 52));  // p
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 56));  // lineinfo
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 60));  // locvars
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 64));  // upvalues
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 68));  // cache
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 72));  // source
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 76));  // gclist
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 80));  // ravi_jit
  tbaa_ProtoT = mdbuilder.createTBAAStructTypeNode("Proto", nodes);

  //! 18 = metadata !{metadata !"LClosure",
  //                 metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5,
  //                 metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64
  //                 12,
  //                 metadata !4, i64 16}
  nodes.clear();
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 4));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 5));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 6));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 8));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 12));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 16));
  tbaa_LClosureT = mdbuilder.createTBAAStructTypeNode("LClosure", nodes);

  tbaa_LClosure_pT =
      mdbuilder.createTBAAStructTagNode(tbaa_LClosureT, tbaa_pointerT, 12);
  tbaa_LClosure_upvalsT =
      mdbuilder.createTBAAStructTagNode(tbaa_LClosureT, tbaa_pointerT, 16);

  tbaa_Proto_kT =
      mdbuilder.createTBAAStructTagNode(tbaa_ProtoT, tbaa_pointerT, 44);
  tbaa_Proto_sizepT =
      mdbuilder.createTBAAStructTagNode(tbaa_ProtoT, tbaa_intT, 28);
  tbaa_Proto_codeT =
      mdbuilder.createTBAAStructTagNode(tbaa_ProtoT, tbaa_pointerT, 48);

  nodes.clear();
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_longlongT, 0));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 8));
  tbaa_TValueT = mdbuilder.createTBAAStructTypeNode("TValue", nodes);

  tbaa_TValue_nT =
      mdbuilder.createTBAAStructTagNode(tbaa_TValueT, tbaa_longlongT, 0);
  tbaa_TValue_hT =
      mdbuilder.createTBAAStructTagNode(tbaa_pointerT, tbaa_pointerT, 0);
  tbaa_TValue_ttT =
      mdbuilder.createTBAAStructTagNode(tbaa_TValueT, tbaa_intT, 8);

  tbaa_luaState_topT =
      mdbuilder.createTBAAStructTagNode(tbaa_luaStateT, tbaa_pointerT, 8);

  nodes.clear();
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_longlongT, 8));
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_TValueT, 16));
  tbaa_UpValT = mdbuilder.createTBAAStructTypeNode("UpVal", nodes);
  tbaa_UpVal_vT =
      mdbuilder.createTBAAStructTagNode(tbaa_UpValT, tbaa_pointerT, 0);

  // RaviArray
  nodes.clear();
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 0));         /* data */
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 4)); /* len */
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 8)); /* size */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 12)); /* type */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 13)); /* modifiers */
  tbaa_RaviArrayT = mdbuilder.createTBAAStructTypeNode("RaviArray", nodes);

  // Table TBAA struct type
  nodes.clear();
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 0)); /* next */
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 4)); /* tt */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 5)); /* marked */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 6)); /* flags */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_charT, 7)); /* lsizenode */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 8)); /* size array */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 12)); /* array */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 16)); /* node */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 20)); /* lastfree */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 24)); /* metatable */
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_pointerT, 28)); /* gclist */
  nodes.push_back(std::pair<llvm::MDNode *, uint64_t>(tbaa_RaviArrayT,
                                                      32)); /* ravi_array */
#if RAVI_USE_NEWHASH
  nodes.push_back(
      std::pair<llvm::MDNode *, uint64_t>(tbaa_intT, 48)); /* hmask */
#endif
  tbaa_TableT = mdbuilder.createTBAAStructTypeNode("Table", nodes);

  tbaa_Table_flags =
      mdbuilder.createTBAAStructTagNode(tbaa_TableT, tbaa_charT, 6);
  tbaa_Table_lsizenode =
      mdbuilder.createTBAAStructTagNode(tbaa_TableT, tbaa_charT, 7);
  tbaa_Table_sizearray =
      mdbuilder.createTBAAStructTagNode(tbaa_TableT, tbaa_intT, 8);
  tbaa_Table_array =
      mdbuilder.createTBAAStructTagNode(tbaa_TableT, tbaa_pointerT, 12);
  tbaa_Table_node =
      mdbuilder.createTBAAStructTagNode(tbaa_TableT, tbaa_pointerT, 16);
  tbaa_Table_metatable =
      mdbuilder.createTBAAStructTagNode(tbaa_TableT, tbaa_pointerT, 24);
  tbaa_RaviArray_dataT =
      mdbuilder.createTBAAStructTagNode(tbaa_TableT, tbaa_pointerT, 32);
  tbaa_RaviArray_lenT =
      mdbuilder.createTBAAStructTagNode(tbaa_TableT, tbaa_intT, 36);
  tbaa_RaviArray_typeT =
      mdbuilder.createTBAAStructTagNode(tbaa_TableT, tbaa_charT, 44);
#if RAVI_USE_NEWHASH
  tbaa_Table_hmask =
      mdbuilder.createTBAAStructTagNode(tbaa_TableT, tbaa_intT, 48);
#endif
}

void LuaLLVMTypes::dump() {
#if defined(LLVM_ENABLE_DUMP)
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
  RaviJITProtoT->dump();
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
#endif
}
}
Lua Types in LLVM
=================

We need to map Lua types to equivalent type definitions in LLVM. In Ravi we do hold all the type definitions in a struct as shown below::

  struct LuaLLVMTypes {
  
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
  
  };

The actual definition of the types above is shown below::

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



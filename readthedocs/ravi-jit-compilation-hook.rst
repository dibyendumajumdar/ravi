LLVM Compilation hooks in Ravi
==============================

The current approach is Ravi is that a Lua function can be compiled at the function level. (Note that this is the plan - I am working on the implementation).

In terms of changes to support this - we essentially have following. First we have a bunch of C functions - think of these are the compiler API::

  #ifdef __cplusplus
  extern "C" {
  #endif
  
  struct lua_State;
  struct Proto;
  
  /* Initialise the JIT engine */
  int raviV_initjit(struct lua_State *L);
  
  /* Shutdown the JIT engine */
  void raviV_close(struct lua_State *L);
  
  /* Compile the given function if possible */
  int raviV_compile(struct lua_State *L, struct Proto *p);
  
  /* Free the JIT structures associated with the prototype */
  void raviV_freeproto(struct lua_State *L, struct Proto *p);
  
  #ifdef __cplusplus
  }
  #endif

Next the ``Proto`` struct definition has some extra fields::

  typedef struct RaviJITProto {
    lu_byte jit_status; // 0=not compiled, 1=can't compile, 2=compiled, 3=freed
    void *jit_data;
    lua_CFunction jit_function;
  } RaviJITProto;

  /*
  ** Function Prototypes
  */
  typedef struct Proto {
    CommonHeader;
    lu_byte numparams;  /* number of fixed parameters */
    lu_byte is_vararg;
    lu_byte maxstacksize;  /* maximum stack used by this function */
    int sizeupvalues;  /* size of 'upvalues' */
    int sizek;  /* size of 'k' */
    int sizecode;
    int sizelineinfo;
    int sizep;  /* size of 'p' */
    int sizelocvars;
    int linedefined;
    int lastlinedefined;
    TValue *k;  /* constants used by the function */
    Instruction *code;
    struct Proto **p;  /* functions defined inside the function */
    int *lineinfo;  /* map from opcodes to source lines (debug information) */
    LocVar *locvars;  /* information about local variables (debug information) */
    Upvaldesc *upvalues;  /* upvalue information */
    struct LClosure *cache;  /* last created closure with this prototype */
    TString  *source;  /* used for debug information */
    GCObject *gclist;
    /* RAVI */
    RaviJITProto ravi_jit;
  } Proto;

The ``ravi_jit`` member is initialized in ``lfunc.c``::

  Proto *luaF_newproto (lua_State *L) {
    GCObject *o = luaC_newobj(L, LUA_TPROTO, sizeof(Proto));
    Proto *f = gco2p(o);
    f->k = NULL;
    /* code ommitted */
    f->ravi_jit.jit_data = NULL;
    f->ravi_jit.jit_function = NULL;
    f->ravi_jit.jit_status = 0; /* not compiled */
    return f;
  }

The corresponding function to free is::

  void luaF_freeproto (lua_State *L, Proto *f) {
    raviV_freeproto(L, f);
    luaM_freearray(L, f->code, f->sizecode);
    luaM_freearray(L, f->p, f->sizep);
    luaM_freearray(L, f->k, f->sizek);
    luaM_freearray(L, f->lineinfo, f->sizelineinfo);
    luaM_freearray(L, f->locvars, f->sizelocvars);
    luaM_freearray(L, f->upvalues, f->sizeupvalues);
    luaM_free(L, f);
  }

  
When a Lua Function is called it goes through ``luaD_precall()`` in ``ldo.c``. This has been modified to invoke the compiler / use compiled version::

  /*
  ** returns true if function has been executed (C function)
  */
  int luaD_precall (lua_State *L, StkId func, int nresults) {
    lua_CFunction f;
    CallInfo *ci;
    int n;  /* number of arguments (Lua) or returns (C) */
    ptrdiff_t funcr = savestack(L, func);
    switch (ttype(func)) {

	/* omitted */

    case LUA_TLCL: {  /* Lua function: prepare its call */
      CallInfo *prevci = L->ci; /* RAVI - for validation */
      StkId base;
      Proto *p = clLvalue(func)->p;
      n = cast_int(L->top - func) - 1;  /* number of real arguments */
      luaD_checkstack(L, p->maxstacksize);
      for (; n < p->numparams; n++)
        setnilvalue(L->top++);  /* complete missing arguments */
      if (!p->is_vararg) {
        func = restorestack(L, funcr);
        base = func + 1;
      }
      else {
        base = adjust_varargs(L, p, n);
        func = restorestack(L, funcr);  /* previous call can change stack */
      }
      ci = next_ci(L);  /* now 'enter' new function */
      ci->nresults = nresults;
      ci->func = func;
      ci->u.l.base = base;
      ci->top = base + p->maxstacksize;
      lua_assert(ci->top <= L->stack_last);
      ci->u.l.savedpc = p->code;  /* starting point */
      ci->callstatus = CIST_LUA;
      ci->jitstatus = 0;
      L->top = ci->top;
      luaC_checkGC(L);  /* stack grow uses memory */
      if (L->hookmask & LUA_MASKCALL)
        callhook(L, ci);
      if (compile) {
        if (p->ravi_jit.jit_status == 0) {
          /* not compiled */
          raviV_compile(L, p, 0);
        }
        if (p->ravi_jit.jit_status == 2) {
          /* compiled */
          lua_assert(p->ravi_jit.jit_function != NULL);
          ci->jitstatus = 1;
          /* As JITed function is like a C function 
           * employ the same restrictions on recursive
           * calls as for C functions
           */
          if (++L->nCcalls >= LUAI_MAXCCALLS) {
            if (L->nCcalls == LUAI_MAXCCALLS)
              luaG_runerror(L, "C stack overflow");
            else if (L->nCcalls >= (LUAI_MAXCCALLS + (LUAI_MAXCCALLS >> 3)))
              luaD_throw(L, LUA_ERRERR);  /* error while handing stack error */
          }
          /* Disable YIELDs - so JITed functions cannot
           * yield
           */
          L->nny++;
          (*p->ravi_jit.jit_function)(L);
          L->nny--;
          L->nCcalls--;
          lua_assert(L->ci == prevci);
          /* Return a different value from 1 to 
           * allow luaV_execute() to distinguish between 
           * JITed function and true C function
           */
          return 2;
        }
      }
      return 0;
    }
    default: {  /* not a function */

	  /* omitted */
    }
    }
  }


Note that the above returns 2 if compiled Lua function is called. The behaviour in ``lvm.c`` is similar to that when a C function is called.


================================================
Ravi Parsing and ByteCode Implementation Details
================================================

This document covers the enhancements to the Lua parser and byte-code generator.
The Ravi JIT implementation is described elsewhere.

Introduction
============
Since the reason for introducing optional static typing is to enhance performance primarily - not all types benefit from this capability. In fact it is quite hard to extend this to generic recursive structures such as tables without encurring significant overhead. For instance - even to represent a recursive type in the parser will require dynamic memory allocation and add great overhead to the parser.

From a performance point of view the only types that seem worth specializing are:

* integer (64-bit int)
* number (double)
* array of integers
* array of numbers
* table

Implementation Strategy
=======================
I want to build on existing Lua types rather than introducing completely new types to the Lua system. I quite like the minimalist nature of Lua. However, to make the execution efficient I am adding new type specific opcodes and enhancing the Lua parser/code generator to encode these opcodes only when types are known. The new opcodes will execute more efficiently as they will not need to perform type checks. Morever, type specific instructions will lend themselves to more efficient JIT compilation.

I am adding new opcodes that cover arithmetic operations, array operations, variable assignments, etc..

Modifications to Lua Bytecode structure
=======================================
An immediate issue is that the Lua bytecode structure has a 6-bit opcode which is insufficient to hold the various opcodes that I will need. Simply extending the size of this is problematic as then it reduces the space available to the operands A B and C. Furthermore the way Lua bytecodes work means that B and C operands must be 1-bit larger than A - as the extra bit is used to flag whether the operand refers to a constant or a register. (Thanks to Dirk Laurie for pointing this out). 

I am amending the bit mapping in the 32-bit instruction to allow 9-bits for the byte-code, 7-bits for operand A, and 8-bits for operands B and C. This means that some of the Lua limits (maximum number of variables in a function, etc.) have to be revised to be lower than the default.

New OpCodes
===========
The new instructions are specialised for types, and also for register/versus constant. So for example ``OP_RAVI_ADDFI`` means add ``number`` and ``integer``. And ``OP_RAVI_ADDFF`` means add ``number`` and ``number``. The existing Lua opcodes that these are based on define which operands are used.

Example::

  local i=0; i=i+1

Above standard Lua code compiles to::

  [0] LOADK A=0 Bx=-1
  [1] ADD A=0 B=0 C=-2
  [2] RETURN A=0 B=1

We add type info using Ravi extensions::

  local i:integer=0; i=i+1

Now the code compiles to::

  [0] LOADK A=0 Bx=-1
  [1] ADDII A=0 B=0 C=-2
  [2] RETURN A=0 B=1

Above uses type specialised opcode ``OP_RAVI_ADDII``. 

Type Information
================
The basic first step is to add type information to Lua. 

As the parser progresses it creates a vector of ``LocVar`` for each function containing a list of local variables. I have enhanced ``LocVar`` structure in ``lobject.h`` to hold type information.

::

  /* Following are the types we will use
  ** use in parsing. The rationale for types is
  ** performance - as of now these are the only types that
  ** we care about from a performance point of view - if any
  ** other types appear then they are all treated as ANY
  **/
  typedef enum {
    RAVI_TANY = -1,      /* Lua dynamic type */
    RAVI_TNUMINT,   /* integer number */
    RAVI_TNUMFLT,   /* floating point number */
    RAVI_TARRAYINT, /* array of ints */
    RAVI_TARRAYFLT,  /* array of doubles */
    RAVI_TFUNCTION,
    RAVI_TTABLE,
    RAVI_TSTRING,
    RAVI_TNIL,
    RAVI_TBOOLEAN
  } ravitype_t;

  /*
  ** Description of a local variable for function prototypes
  ** (used for debug information)
  */
  typedef struct LocVar {
    TString *varname;
    int startpc;  /* first point where variable is active */
    int endpc;    /* first point where variable is dead */
    ravitype_t ravi_type; /* RAVI type of the variable - RAVI_TANY if unknown */
  } LocVar;

The ``expdesc`` structure is used by the parser to hold nodes in the expression tree. I have enhanced the ``expdesc`` structure to hold the type of an expression.  

::

   typedef struct expdesc {
     expkind k;
     union {
       struct {  /* for indexed variables (VINDEXED) */
         short idx;  /* index (R/K) */
         lu_byte t;  /* table (register or upvalue) */
         lu_byte vt;  /* whether 't' is register (VLOCAL) or upvalue (VUPVAL) */
         ravitype_t key_type; /* key type */
       } ind;
       int info;  /* for generic use */
       lua_Number nval;  /* for VKFLT */
       lua_Integer ival;    /* for VKINT */
     } u;
     int t;  /* patch list of 'exit when true' */
     int f;  /* patch list of 'exit when false' */
     ravitype_t ravi_type; /* RAVI change: type of the expression if known, else RAVI_TANY */
   } expdesc;

Note the addition of type information in two places. Firstly at the ``expdesc`` level which identifies the type of the ``expdesc``. Secondly in the `ind` structure - the ``key_type`` is used to track the type of the key that will be used to index into a table. 

The table structure has been enhanced to hold additional information for array usage.

::

  typedef enum RaviArrayModifer {
    RAVI_ARRAY_SLICE = 1,
    RAVI_ARRAY_FIXEDSIZE = 2
  } RaviArrayModifier;

  typedef struct RaviArray {
    char *data;
    unsigned int len; /* RAVI len specialization */
    unsigned int size; /* amount of memory allocated */
    lu_byte array_type; /* RAVI specialization */
    lu_byte array_modifier; /* Flags that affect how the array is handled */
  } RaviArray;

  typedef struct Table {
    CommonHeader;
    lu_byte flags;  /* 1<<p means tagmethod(p) is not present */
    lu_byte lsizenode;  /* log2 of size of 'node' array */
    unsigned int sizearray;  /* size of 'array' array */
    TValue *array;  /* array part */
    Node *node;
    Node *lastfree;  /* any free position is before this position */
    struct Table *metatable;
    GCObject *gclist;
    RaviArray ravi_array;
  } Table;


Parser Enhancements
===================
The parser needs to be enhanced to generate type specific instructions at various points. 

Local Variable Declarations
---------------------------
First enhancement needed is when local variable declarations are parsed. We need to allow the type to be defined for each variable and ensure that any assignments are type-checked. This is somewhat complex process, due to the fact that assignments can be expressions involving function calls. The last function call is treated as a variable assignment - i.e. all trailing variables are assumed to be assigned values from the function call - if not the variables are set to nil by default. 

The entry point for parsing a local statement is ``localstat()`` in ``lparser.c``. This function has been enhanced to parse the type annotations supported by Ravi. The modified function is shown below.

::

  /* Parse
   *   name : type
   *   where type is 'integer', 'integer[]',
   *                 'number', 'number[]'
   */
  static ravitype_t declare_localvar(LexState *ls) {
    /* RAVI change - add type */
    TString *name = str_checkname(ls);
    /* assume a dynamic type */
    ravitype_t tt = RAVI_TANY;
    /* if the variable name is followed by a colon then we have a type
     * specifier 
     */
    if (testnext(ls, ':')) {
      TString *typename = str_checkname(ls); /* we expect a type name */
      const char *str = getaddrstr(typename);
      /* following is not very nice but easy as 
       * the lexer doesn't need to be changed
       */
      if (strcmp(str, "integer") == 0)
        tt = RAVI_TNUMINT;
      else if (strcmp(str, "number") == 0)
        tt = RAVI_TNUMFLT;
      if (tt == RAVI_TNUMFLT || tt == RAVI_TNUMINT) {
        /* if we see [] then it is an array type */
        if (testnext(ls, '[')) {
          checknext(ls, ']');
          tt = (tt == RAVI_TNUMFLT) ? RAVI_TARRAYFLT : RAVI_TARRAYINT;
        }
      }
    }
    new_localvar(ls, name, tt);
    return tt;
  }
   
  /* parse a local variable declaration statement - called from statement() */
  static void localstat (LexState *ls) {
    /* stat -> LOCAL NAME {',' NAME} ['=' explist] */
    int nvars = 0;
    int nexps;
    expdesc e;
    e.ravi_type = RAVI_TANY;
    /* RAVI while declaring locals we need to gather the types
     * so that we can check any assignments later on.
     * TODO we may be able to use register_typeinfo() here
     * instead.
     */
    int vars[MAXVARS] = { 0 };
    do {
      /* RAVI changes start */
      /* local name : type = value */
      vars[nvars] = declare_localvar(ls);
      /* RAVI changes end */
      nvars++;
    } while (testnext(ls, ','));
    if (testnext(ls, '='))
      nexps = localvar_explist(ls, &e, vars, nvars);
    else {
      e.k = VVOID;
      nexps = 0;
    }
    localvar_adjust_assign(ls, nvars, nexps, &e);
    adjustlocalvars(ls, nvars);
  }

The do-while loop is responsible for parsing the variable names and the type annotations. As each variable name is parsed we detect if there is a type annotation, if and if present the type is recorded in the array ``vars``. 

Parameter lists may have static type annotations as well, so when parsing parameters we again need to invoke ``declare_localvar()``.

::

  static void parlist (LexState *ls) {
    /* parlist -> [ param { ',' param } ] */
    FuncState *fs = ls->fs;
    Proto *f = fs->f;
    int nparams = 0;
    f->is_vararg = 0;
    if (ls->t.token != ')') {  /* is 'parlist' not empty? */
      do {
        switch (ls->t.token) {
          case TK_NAME: {  /* param -> NAME */
            /* RAVI change - add type */
            declare_localvar(ls);
            nparams++;
            break;
          }
          case TK_DOTS: {  /* param -> '...' */
            luaX_next(ls);
            f->is_vararg = 1;
            break;
          }
          default: luaX_syntaxerror(ls, "<name> or '...' expected");
        }
      } while (!f->is_vararg && testnext(ls, ','));
    }
    adjustlocalvars(ls, nparams);
    f->numparams = cast_byte(fs->nactvar);
    luaK_reserveregs(fs, fs->nactvar);  /* reserve register for parameters */
    for (int i = 0; i < f->numparams; i++) {
      ravitype_t tt = raviY_get_register_typeinfo(fs, i);
      DEBUG_VARS(raviY_printf(fs, "Parameter [%d] = %v\n", i + 1, getlocvar(fs, i)));
      /* do we need to convert ? */
      if (tt == RAVI_TNUMFLT || tt == RAVI_TNUMINT) {
        /* code an instruction to convert in place */
        luaK_codeABC(ls->fs, tt == RAVI_TNUMFLT ? OP_RAVI_TOFLT : OP_RAVI_TOINT, i, 0, 0);
      }
      else if (tt == RAVI_TARRAYFLT || tt == RAVI_TARRAYINT) {
        /* code an instruction to convert in place */
        luaK_codeABC(ls->fs, tt == RAVI_TARRAYFLT ? OP_RAVI_TOARRAYF : OP_RAVI_TOARRAYI, i, 0, 0);
      }
    }
  }

Additionally for parameters that are decorated with static types we need to introduce new instructions to coerce the types at run time. That is what is happening in the for loop at the end.

The ``declare_localvar()`` function passes the type of the variable to ``new_localvar()`` which records this in the ``LocVar`` structure associated with the variable.

::
  
  static int registerlocalvar (LexState *ls, TString *varname, int ravi_type) {
    FuncState *fs = ls->fs;
    Proto *f = fs->f;
    int oldsize = f->sizelocvars;
    luaM_growvector(ls->L, f->locvars, fs->nlocvars, f->sizelocvars,
                  LocVar, SHRT_MAX, "local variables");
    while (oldsize < f->sizelocvars) {
      /* RAVI change initialize */
      f->locvars[oldsize].startpc = -1;
      f->locvars[oldsize].endpc = -1;
      f->locvars[oldsize].ravi_type = RAVI_TANY;
      f->locvars[oldsize++].varname = NULL;
    }
    f->locvars[fs->nlocvars].varname = varname;
    f->locvars[fs->nlocvars].ravi_type = ravi_type;
    luaC_objbarrier(ls->L, f, varname);
    return fs->nlocvars++;
  }

  /* create a new local variable in function scope, and set the
   * variable type (RAVI - added type tt) */
  static void new_localvar (LexState *ls, TString *name, ravitype_t tt) {
    FuncState *fs = ls->fs;
    Dyndata *dyd = ls->dyd;
    /* register variable and get its index */
    /* RAVI change - record type info for local variable */
    int i = registerlocalvar(ls, name, tt);
    checklimit(fs, dyd->actvar.n + 1 - fs->firstlocal,
                  MAXVARS, "local variables");
    luaM_growvector(ls->L, dyd->actvar.arr, dyd->actvar.n + 1,
                  dyd->actvar.size, Vardesc, MAX_INT, "local variables");
    /* variable will be placed at stack position dyd->actvar.n */
    dyd->actvar.arr[dyd->actvar.n].idx = cast(short, i);
    DEBUG_VARS(raviY_printf(fs, "new_localvar -> registering %v fs->f->locvars[%d] at ls->dyd->actvar.arr[%d]\n", &fs->f->locvars[i], i, dyd->actvar.n));
    dyd->actvar.n++;
    DEBUG_VARS(raviY_printf(fs, "new_localvar -> ls->dyd->actvar.n set to %d\n", dyd->actvar.n));
  }

The next bit of change is how the expressions are handled following the ``=`` symbol. The previously built ``vars`` array is passed to a modified version of ``explist()`` called ``localvar_explist()``. This handles the parsing of expressions and then ensuring that each expression matches the type of the variable where known. The ``localvar_explist()`` function is shown next.

::

  static int localvar_explist(LexState *ls, expdesc *v, int *vars, int nvars) {
    /* explist -> expr { ',' expr } */
    int n = 1;  /* at least one expression */
    expr(ls, v);
  #if RAVI_ENABLED
    ravi_typecheck(ls, v, vars, nvars, 0);
  #endif
    while (testnext(ls, ',')) {
      luaK_exp2nextreg(ls->fs, v);
      expr(ls, v);
  #if RAVI_ENABLED
      ravi_typecheck(ls, v, vars, nvars, n);
  #endif
      n++;
    }
    return n;
  }

The main changes compared to ``explist()`` are the calls to ``ravi_typecheck()``. Note that the array ``vars`` is passed to the ``ravi_typecheck()`` function along with the current variable index in ``n``. The ``ravi_typecheck()`` function is reproduced below.

::

  static void ravi_typecheck(LexState *ls, expdesc *v, int *vars, int nvars, int n)
  {
    if (n < nvars && vars[n] != RAVI_TANY && v->ravi_type != vars[n]) {
      if (v->ravi_type != vars[n] && 
          (vars[n] == RAVI_TARRAYFLT || vars[n] == RAVI_TARRAYINT) && 
          v->k == VNONRELOC) {
        /* as the bytecode for generating a table is already 
         * emitted by this stage we have to amend the generated byte code 
         * - not sure if there is a better approach.
         * We look for the last bytecode that is OP_NEWTABLE 
         * and that has the same destination
         * register as v->u.info which is our variable
         * local a:integer[] = { 1 }
         *                     ^ We are just past this and
         *                       about to assign to a
         */
        int i = ls->fs->pc - 1;
        for (; i >= 0; i--) {
          Instruction *pc = &ls->fs->f->code[i];
          OpCode op = GET_OPCODE(*pc);
          int reg;
          if (op != OP_NEWTABLE)
            continue;
          reg = GETARG_A(*pc);
          if (reg != v->u.info)
            continue;
          op = (vars[n] == RAVI_TARRAYINT) ? OP_RAVI_NEWARRAYI : OP_RAVI_NEWARRAYF;
          SET_OPCODE(*pc, op); /* modify opcode */
          DEBUG_CODEGEN(raviY_printf(ls->fs, "[%d]* %o ; modify opcode\n", i, *pc));
          break;
        }
        if (i < 0)
          luaX_syntaxerror(ls, "expecting array initializer");
      }
      /* if we are calling a function then convert return types */
      else if (v->ravi_type != vars[n] && 
              (vars[n] == RAVI_TNUMFLT || vars[n] == RAVI_TNUMINT) && 
              v->k == VCALL) {
        /* For local variable declarations that call functions e.g.
         * local i = func()
         * Lua ensures that the function returns values 
         * to register assigned to variable i and above so that no 
         * separate OP_MOVE instruction is necessary. So that means that
         * we need to coerce the return values in situ.
         */
        /* Obtain the instruction for OP_CALL */
        Instruction *pc = &getcode(ls->fs, v); 
        lua_assert(GET_OPCODE(*pc) == OP_CALL);
        int a = GETARG_A(*pc); /* function return values 
                                  will be placed from register pointed 
                                  by A and upwards */
        int nrets = GETARG_C(*pc) - 1; /* operand C contains 
                                          number of return values expected  */
        /* Note that at this stage nrets is always 1 
         * - as Lua patches in the this value for the last 
         * function call in a variable declaration statement 
         * in adjust_assign and localvar_adjust_assign */
        /* all return values that are going to be assigned 
           to typed local vars must be converted to the correct type */
        int i;
        for (i = n; i < (n+nrets); i++)
          /* do we need to convert ? */
          if ((vars[i] == RAVI_TNUMFLT || vars[i] == RAVI_TNUMINT))
            /* code an instruction to convert in place */
            luaK_codeABC(ls->fs, 
                         vars[i] == RAVI_TNUMFLT ? 
                                    OP_RAVI_TOFLT : OP_RAVI_TOINT, 
                         a+(i-n), 0, 0);
          else if ((vars[i] == RAVI_TARRAYFLT || vars[i] == RAVI_TARRAYINT))
            /* code an instruction to convert in place */
            luaK_codeABC(ls->fs, 
                         vars[i] == RAVI_TARRAYFLT ? 
                                    OP_RAVI_TOARRAYF : OP_RAVI_TOARRAYI, 
                         a + (i - n), 0, 0);
      }
      else if ((vars[n] == RAVI_TNUMFLT || vars[n] == RAVI_TNUMINT) && 
               v->k == VINDEXED) {
        if (vars[n] == RAVI_TNUMFLT && v->ravi_type != RAVI_TARRAYFLT || 
          vars[n] == RAVI_TNUMINT && v->ravi_type != RAVI_TARRAYINT) 
          luaX_syntaxerror(ls, "Invalid local assignment");
      }
      else
        luaX_syntaxerror(ls, "Invalid local assignment");
    }
  }

There are several parts to this function.

The simple case is when the type of the expression matches the variable.

Secondly if the expression is a table initializer then we need to generate specialized opcodes if the target variable is supposed to be ``integer[]`` or ``number[]``. The specialized opcode sets up some information in the ``Table`` structure. The problem is that this requires us to modify ``OP_NEWTABLE`` instruction which has already been emitted. So we scan the generated instructions to find the last ``OP_NEWTABLE`` instruction that assigns to the register associated with the target variable.  

Next bit of special handling is for function calls. If the assignment makes a function call then we perform type coercion on return values where these values are being assigned to variables with defined types. This means that if the target variable is ``integer`` or ``number`` we issue opcodes ``TOINT`` and ``TOFLT`` respectively. If the target variable is ``integer[]`` or ``number[]`` then we issue ``TOARRAYI`` and ``TOARRAYF`` respectively. These opcodes ensure that the values are of required type or can be cast to the required type.

Note that any left over variables that are not assigned values, are set to 0 if they are of integer or number type, else they are set to nil as per Lua's default behavior. This is handled in ``localvar_adjust_assign()`` which is described later on.

Finally the last case is when the target variable is ``integer`` or ``number`` and the expression is a table / array access. In this case we check that the table is of required type.

The ``localvar_adjust_assign()`` function referred to above is shown below.

::

  static void localvar_adjust_assign(LexState *ls, int nvars, int nexps, expdesc *e) {
    FuncState *fs = ls->fs;
    int extra = nvars - nexps;
    if (hasmultret(e->k)) {
      extra++;  /* includes call itself */
      if (extra < 0) extra = 0;
      /* following adjusts the C operand in the OP_CALL instruction */
      luaK_setreturns(fs, e, extra);  /* last exp. provides the difference */
  #if RAVI_ENABLED
      /* Since we did not know how many return values to process in localvar_explist() we
      * need to add instructions for type coercions at this stage for any remaining
      * variables
      */
      ravi_coercetype(ls, e, extra);
  #endif
      if (extra > 1) luaK_reserveregs(fs, extra - 1);
    }
    else {
      if (e->k != VVOID) luaK_exp2nextreg(fs, e);  /* close last expression */
      if (extra > 0) {
        int reg = fs->freereg;
        luaK_reserveregs(fs, extra);
        /* RAVI TODO for typed variables we should not set to nil? */
        luaK_nil(fs, reg, extra);
  #if RAVI_ENABLED
        /* typed variables that are primitives cannot be set to nil so 
         * we need to emit instructions to initialise them to default values 
         */
        ravi_setzero(fs, reg, extra);
  #endif
      }
    }
  }

As mentioned before any variables left over in a local declaration that have not been assigned values must be set to default values appropriate for the type. In the case of trailing values returned by a function call we need to coerce the values to the required types. All this is done in the ``localvar_adjust_assign()`` function above.

Note that local declarations have a complication that until the declaration is complete the variable does not come in scope. So we have to be careful when we wish to map from a register to the local variable declaration as this mapping is only available after the variable is activated. Couple of helper routines are shown below. 

::

  /* translate from local register to local variable index
   */
  static int register_to_locvar_index(FuncState *fs, int reg) {
    int idx;
    lua_assert(reg >= 0 && (fs->firstlocal + reg) < fs->ls->dyd->actvar.n);
    /* Get the LocVar associated with the register */
    idx = fs->ls->dyd->actvar.arr[fs->firstlocal + reg].idx;
    lua_assert(idx < fs->nlocvars);
    return idx;
  }

  /* get type of a register - if the register is not allocated
   * to an active local variable, then return RAVI_TANY else
   * return the type associated with the variable.
   * This is a RAVI function
   */
  ravitype_t raviY_get_register_typeinfo(FuncState *fs, int reg) {
    int idx; 
    LocVar *v;
    if (reg < 0 || reg >= fs->nactvar || (fs->firstlocal + reg) >= fs->ls->dyd->actvar.n)
      return RAVI_TANY;
    /* Get the LocVar associated with the register */
    idx = fs->ls->dyd->actvar.arr[fs->firstlocal + reg].idx;
    lua_assert(idx < fs->nlocvars);
    v = &fs->f->locvars[idx];
    /* Variable in scope so return the type if we know it */
    return v->ravi_type;
  }

Note the use of ``register_to_localvar_index()`` in functions below.

::

  /* Generate instructions for converting types 
   * This is needed post a function call to handle
   * variable number of return values
   * n = number of return values to adjust 
   */
  static void ravi_coercetype(LexState *ls, expdesc *v, int n)
  {
    if (v->k != VCALL || n <= 0) return;
    /* For local variable declarations that call functions e.g.
    * local i = func()
    * Lua ensures that the function returns values to register 
    * assigned to variable and above so that no separate 
    * OP_MOVE instruction is necessary. So that means that
    * we need to coerce the return values in situ.
    */
    /* Obtain the instruction for OP_CALL */
    Instruction *pc = &getcode(ls->fs, v); 
    lua_assert(GET_OPCODE(*pc) == OP_CALL);
    int a = GETARG_A(*pc); /* function return values will be placed 
                              from register pointed by A and upwards */
    /* all return values that are going to be assigned 
     to typed local vars must be converted to the correct type */
    int i;
    for (i = a + 1; i < a + n; i++) {
      /* Since this is called when parsing local statements the 
       * variable may not yet have a register assigned to it 
       * so we can't use raviY_get_register_typeinfo()
       * here. Instead we need to check the variable definition - so we 
       * first convert from local register to variable index.
       */
      int idx = register_to_locvar_index(ls->fs, i);
      /* get variable's type */
      ravitype_t ravi_type = ls->fs->f->locvars[idx].ravi_type;  
      /* do we need to convert ? */
      if (ravi_type == RAVI_TNUMFLT || ravi_type == RAVI_TNUMINT)
        /* code an instruction to convert in place */
        luaK_codeABC(ls->fs, ravi_type == RAVI_TNUMFLT ? 
                     OP_RAVI_TOFLT : OP_RAVI_TOINT, i, 0, 0);
      else if (ravi_type == RAVI_TARRAYINT || ravi_type == RAVI_TARRAYFLT)
        luaK_codeABC(ls->fs, ravi_type == RAVI_TARRAYINT ? 
                     OP_RAVI_TOARRAYI : OP_RAVI_TOARRAYF, i, 0, 0);
    }
  }

  static void ravi_setzero(FuncState *fs, int from, int n) {
    int last = from + n - 1;  /* last register to set nil */
    int i;
    for (i = from; i <= last; i++) {
      /* Since this is called when parsing local statements 
       * the variable may not yet have a register assigned to 
       * it so we can't use raviY_get_register_typeinfo()
       * here. Instead we need to check the variable definition - so we
       * first convert from local register to variable index.
       */
      int idx = register_to_locvar_index(fs, i);
      /* get variable's type */   
      ravitype_t ravi_type = fs->f->locvars[idx].ravi_type;  
      /* do we need to convert ? */
      if (ravi_type == RAVI_TNUMFLT || ravi_type == RAVI_TNUMINT)
        /* code an instruction to convert in place */
        luaK_codeABC(fs, ravi_type == RAVI_TNUMFLT ? 
           OP_RAVI_LOADFZ : OP_RAVI_LOADIZ, i, 0, 0);
    }
  }

Assignments
-----------

Assignment statements have to be enhanced to perform similar type checks as for local declarations. Fortunately he assignment goes through the function ``luaK_storevar()`` in ``lcode.c``. A modified version of this is shown below.

::

  void luaK_storevar (FuncState *fs, expdesc *var, expdesc *ex) {
    switch (var->k) {
      case VLOCAL: {
        check_valid_store(fs, var, ex);
        freeexp(fs, ex);
        exp2reg(fs, ex, var->u.info);
        return;
      }
      case VUPVAL: {
        int e = luaK_exp2anyreg(fs, ex);
        luaK_codeABC(fs, OP_SETUPVAL, e, var->u.info, 0);
        break;
      }
      case VINDEXED: {
        OpCode op = (var->u.ind.vt == VLOCAL) ? 
                       OP_SETTABLE : OP_SETTABUP;
        if (op == OP_SETTABLE) {
          /* table value set - if array access then use specialized versions */
          if (var->ravi_type == RAVI_TARRAYFLT && 
              var->u.ind.key_type == RAVI_TNUMINT)
            op = OP_RAVI_SETTABLE_AF;
          else if (var->ravi_type == RAVI_TARRAYINT && 
                   var->u.ind.key_type == RAVI_TNUMINT)
            op = OP_RAVI_SETTABLE_AI;
        }
        int e = luaK_exp2RK(fs, ex);
        luaK_codeABC(fs, op, var->u.ind.t, var->u.ind.idx, e);
        break;
      }
      default: {
        lua_assert(0);  /* invalid var kind to store */
        break;
      }
    }
    freeexp(fs, ex);
  }

Firstly note the call to ``check_valid_store()`` for a local variable assignment. The ``check_valid_store()`` function validates that the assignment is compatible.

Secondly if the assignment is to an indexed variable, i.e., table, then we need to generate special opcodes for arrays.

MOVE opcodes
------------
Any ``MOVE`` instructions must be modified so that if the target is register that hosts a variable of known type then we need to generate special instructions that do a type conversion during the move. This is handled in ``discharge2reg()`` function which is reproduced below.

::

  static void discharge2reg (FuncState *fs, expdesc *e, int reg) {
    luaK_dischargevars(fs, e);
    switch (e->k) {
      case VNIL: {
        luaK_nil(fs, reg, 1);
        break;
      }
      case VFALSE: case VTRUE: {
        luaK_codeABC(fs, OP_LOADBOOL, reg, e->k == VTRUE, 0);
        break;
      }
      case VK: {
        luaK_codek(fs, reg, e->u.info);
        break;
      }
      case VKFLT: {
        luaK_codek(fs, reg, luaK_numberK(fs, e->u.nval));
        break;
      }
      case VKINT: {
        luaK_codek(fs, reg, luaK_intK(fs, e->u.ival));
        break;
      }
      case VRELOCABLE: {
        Instruction *pc = &getcode(fs, e);
        SETARG_A(*pc, reg);
        DEBUG_EXPR(raviY_printf(fs, "discharge2reg (VRELOCABLE set arg A) %e\n", e));
        DEBUG_CODEGEN(raviY_printf(fs, "[%d]* %o ; set A to %d\n", e->u.info, *pc, reg));
        break;
      }
      case VNONRELOC: {
        if (reg != e->u.info) {
          /* code a MOVEI or MOVEF if the target register is a local typed variable */
          int ravi_type = raviY_get_register_typeinfo(fs, reg);
          switch (ravi_type) {
          case RAVI_TNUMINT:
            luaK_codeABC(fs, OP_RAVI_MOVEI, reg, e->u.info, 0);
            break;
          case RAVI_TNUMFLT:
            luaK_codeABC(fs, OP_RAVI_MOVEF, reg, e->u.info, 0);
            break;
          case RAVI_TARRAYINT:
            luaK_codeABC(fs, OP_RAVI_MOVEAI, reg, e->u.info, 0);
            break;
          case RAVI_TARRAYFLT:
            luaK_codeABC(fs, OP_RAVI_MOVEAF, reg, e->u.info, 0);
            break;
          default:
            luaK_codeABC(fs, OP_MOVE, reg, e->u.info, 0);
            break;
          }
        }
        break;
      }
      default: {
        lua_assert(e->k == VVOID || e->k == VJMP);
        return;  /* nothing to do... */
      }
    }
    e->u.info = reg;
    e->k = VNONRELOC; 
  }

Note the handling of ``VNONRELOC`` case.

Expression Parsing
------------------
The expression evaluation process must be modified so that type information is retained and flows through as the parser evaluates the expression. This involves ensuring that the type information is passed through as the parser modifies, reuses, creates new ``expdesc`` objects. Essentially this means keeping the ``ravi_type`` correct.

Additionally when arithmetic operations take place two things need to happen: a) specialized opcodes need to be emitted and b) the type of the resulting expression needs to be set.

::

  static void codeexpval (FuncState *fs, OpCode op,
                        expdesc *e1, expdesc *e2, int line) {
    lua_assert(op >= OP_ADD);
    if (op <= OP_BNOT && constfolding(fs, getarithop(op), e1, e2))
      return;  /* result has been folded */
    else {
      int o1, o2;
      int isbinary = 1;
      /* move operands to registers (if needed) */
      if (op == OP_UNM || op == OP_BNOT || op == OP_LEN) {  /* unary op? */
        o2 = 0;  /* no second expression */
        o1 = luaK_exp2anyreg(fs, e1);  /* cannot operate on constants */
        isbinary = 0;
      }
      else {  /* regular case (binary operators) */
        o2 = luaK_exp2RK(fs, e2);  /* both operands are "RK" */
        o1 = luaK_exp2RK(fs, e1);
      }
      if (o1 > o2) {  /* free registers in proper order */
        freeexp(fs, e1);
        freeexp(fs, e2);
      }
      else {
        freeexp(fs, e2);
        freeexp(fs, e1);
      }
  #if RAVI_ENABLED
      if (op == OP_ADD && 
        (e1->ravi_type == RAVI_TNUMFLT || e1->ravi_type == RAVI_TNUMINT) &&
        (e2->ravi_type == RAVI_TNUMFLT || e2->ravi_type == RAVI_TNUMINT))
        generate_binarithop(fs, e1, e2, o1, o2, 0);
      else if (op == OP_MUL &&
        (e1->ravi_type == RAVI_TNUMFLT || e1->ravi_type == RAVI_TNUMINT) &&
        (e2->ravi_type == RAVI_TNUMFLT || e2->ravi_type == RAVI_TNUMINT))
        generate_binarithop(fs, e1, e2, o1, o2, OP_RAVI_MULFF - OP_RAVI_ADDFF);

      /* todo optimize the SUB opcodes when constant is small */
      else if (op == OP_SUB && 
               e1->ravi_type == RAVI_TNUMFLT && 
               e2->ravi_type == RAVI_TNUMFLT) {
        e1->u.info = luaK_codeABC(fs, OP_RAVI_SUBFF, 0, o1, o2); 
      }
      else if (op == OP_SUB && 
               e1->ravi_type == RAVI_TNUMFLT && 
               e2->ravi_type == RAVI_TNUMINT) {
        e1->u.info = luaK_codeABC(fs, OP_RAVI_SUBFI, 0, o1, o2); 
      }
      /* code omitted here  .... */
      else {
  #endif
        e1->u.info = luaK_codeABC(fs, op, 0, o1, o2);  /* generate opcode */
  #if RAVI_ENABLED
      }
  #endif
      e1->k = VRELOCABLE;  /* all those operations are relocable */
      if (isbinary) {
        if ((op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV) 
          && e1->ravi_type == RAVI_TNUMFLT && e2->ravi_type == RAVI_TNUMFLT)
          e1->ravi_type = RAVI_TNUMFLT;
        else if ((op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV) 
          && e1->ravi_type == RAVI_TNUMFLT && e2->ravi_type == RAVI_TNUMINT)
          e1->ravi_type = RAVI_TNUMFLT;
        else if ((op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV) 
          && e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMFLT)
          e1->ravi_type = RAVI_TNUMFLT;
        else if ((op == OP_ADD || op == OP_SUB || op == OP_MUL) 
          && e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMINT)
          e1->ravi_type = RAVI_TNUMINT;
        else if ((op == OP_DIV) 
          && e1->ravi_type == RAVI_TNUMINT && e2->ravi_type == RAVI_TNUMINT)
          e1->ravi_type = RAVI_TNUMFLT;
        else
          e1->ravi_type = RAVI_TANY;
      }
      else {
        if (op == OP_LEN || op == OP_BNOT)
          e1->ravi_type = RAVI_TNUMINT;
      }
      luaK_fixline(fs, line);
    }
  } 

When expression reference indexed variables, i.e., tables, we need to emit specialized opcodes if the table is an array. This is done in ``luaK_dischargevars()``.

::

  void luaK_dischargevars (FuncState *fs, expdesc *e) {
    switch (e->k) {
      case VLOCAL: {
        e->k = VNONRELOC;
        DEBUG_EXPR(raviY_printf(fs, "luaK_dischargevars (VLOCAL->VNONRELOC) %e\n", e));
        break;
      }
      case VUPVAL: {
        e->u.info = luaK_codeABC(fs, OP_GETUPVAL, 0, e->u.info, 0);
        e->k = VRELOCABLE;
        DEBUG_EXPR(raviY_printf(fs, "luaK_dischargevars (VUPVAL->VRELOCABLE) %e\n", e));
        break;
      }
      case VINDEXED: {
        OpCode op = OP_GETTABUP;  /* assume 't' is in an upvalue */
        freereg(fs, e->u.ind.idx);
        if (e->u.ind.vt == VLOCAL) {  /* 't' is in a register? */
          freereg(fs, e->u.ind.t);
          /* table access - set specialized op codes if array types are detected */
          if (e->ravi_type == RAVI_TARRAYFLT && 
              e->u.ind.key_type == RAVI_TNUMINT)
            op = OP_RAVI_GETTABLE_AF;
          else if (e->ravi_type == RAVI_TARRAYINT && 
                   e->u.ind.key_type == RAVI_TNUMINT)
            op = OP_RAVI_GETTABLE_AI;
          else
            op = OP_GETTABLE;
          if (e->ravi_type == RAVI_TARRAYFLT || e->ravi_type == RAVI_TARRAYINT)
            /* set the type of resulting expression */
            e->ravi_type = e->ravi_type == RAVI_TARRAYFLT ? 
                             RAVI_TNUMFLT : RAVI_TNUMINT;
        }
        e->u.info = luaK_codeABC(fs, op, 0, e->u.ind.t, e->u.ind.idx);
        e->k = VRELOCABLE;
        DEBUG_EXPR(raviY_printf(fs, "luaK_dischargevars (VINDEXED->VRELOCABLE) %e\n", e));
        break;
      }
      case VVARARG:
      case VCALL: {
        luaK_setoneret(fs, e);
        break;
      }
      default: break;  /* there is one value available (somewhere) */
    }
  }

fornum statements
-----------------

The Lua fornum statements create special variables. In order to allows the loop variable to be used in expressions within the loop body we need to set the types of these variables. This is handled in ``fornum()`` as shown below. Additional complexity is due to the fact that Ravi tries to detect when fornum loops use positive integer step and if this step is ``1``; specialized bytecodes are generated for these scenarios.

::

  typedef struct Fornuminfo {
    ravitype_t type;
    int is_constant;
    int int_value;
  } Fornuminfo;

  /* parse the single expressions needed in numerical for loops
   * called by fornum()
   */
  static int exp1 (LexState *ls, Fornuminfo *info) {
    /* Since the local variable in a fornum loop is local to the loop and does
     * not use any variable in outer scope we don't need to check its
     * type - also the loop is already optimised so no point trying to
     * optimise the iteration variable
     */
    expdesc e;
    int reg;
    e.ravi_type = RAVI_TANY;
    expr(ls, &e);
    DEBUG_EXPR(raviY_printf(ls->fs, "fornum exp -> %e\n", &e));
    info->is_constant = (e.k == VKINT);
    info->int_value = info->is_constant ? e.u.ival : 0;
    luaK_exp2nextreg(ls->fs, &e);
    lua_assert(e.k == VNONRELOC);
    reg = e.u.info;
    info->type = e.ravi_type;
    return reg;
  }

  /* parse a for loop body for both versions of the for loop
   * called by fornum(), forlist()
   */
  static void forbody (LexState *ls, int base, int line, int nvars, int isnum, Fornuminfo *info) {
    /* forbody -> DO block */
    BlockCnt bl;
    OpCode forprep_inst = OP_FORPREP, forloop_inst = OP_FORLOOP;
    FuncState *fs = ls->fs;
    int prep, endfor;
    adjustlocalvars(ls, 3);  /* control variables */
    checknext(ls, TK_DO);
    if (isnum) {
      ls->fs->f->ravi_jit.jit_flags = 1;
      if (info && info->is_constant && info->int_value > 1) {
        forprep_inst = OP_RAVI_FORPREP_IP;
        forloop_inst = OP_RAVI_FORLOOP_IP;
      }
      else if (info && info->is_constant && info->int_value == 1) {
        forprep_inst = OP_RAVI_FORPREP_I1;
        forloop_inst = OP_RAVI_FORLOOP_I1;
      }
    }
    prep = isnum ? luaK_codeAsBx(fs, forprep_inst, base, NO_JUMP) : luaK_jump(fs);
    enterblock(fs, &bl, 0);  /* scope for declared variables */
    adjustlocalvars(ls, nvars);
    luaK_reserveregs(fs, nvars);
    block(ls);
    leaveblock(fs);  /* end of scope for declared variables */
    luaK_patchtohere(fs, prep);
    if (isnum)  /* numeric for? */
      endfor = luaK_codeAsBx(fs, forloop_inst, base, NO_JUMP);
    else {  /* generic for */
      luaK_codeABC(fs, OP_TFORCALL, base, 0, nvars);
      luaK_fixline(fs, line);
      endfor = luaK_codeAsBx(fs, OP_TFORLOOP, base + 2, NO_JUMP);
    }
    luaK_patchlist(fs, endfor, prep + 1);
    luaK_fixline(fs, line);
  }

  /* parse a numerical for loop, calls forbody()
   * called from forstat()
   */
  static void fornum (LexState *ls, TString *varname, int line) {
    /* fornum -> NAME = exp1,exp1[,exp1] forbody */
    FuncState *fs = ls->fs;
    int base = fs->freereg;
    LocVar *vidx, *vlimit, *vstep, *vvar;
    new_localvarliteral(ls, "(for index)");
    new_localvarliteral(ls, "(for limit)");
    new_localvarliteral(ls, "(for step)");
    new_localvar(ls, varname, RAVI_TANY);
    /* The fornum sets up its own variables as above.
       These are expected to hold numeric values - but from Ravi's
       point of view we need to know if the variable is an integer or
       double. So we need to check if this can be determined from the
       fornum expressions. If we can then we will set the 
       fornum variables to the type we discover.
    */
    vidx = &fs->f->locvars[fs->nlocvars - 4]; /* index variable - not yet active so get it from locvars*/
    vlimit = &fs->f->locvars[fs->nlocvars - 3]; /* index variable - not yet active so get it from locvars*/
    vstep = &fs->f->locvars[fs->nlocvars - 2]; /* index variable - not yet active so get it from locvars*/
    vvar = &fs->f->locvars[fs->nlocvars - 1]; /* index variable - not yet active so get it from locvars*/
    checknext(ls, '=');
    /* get the type of each expression */
    Fornuminfo tidx = { RAVI_TANY,0,0 }, tlimit = { RAVI_TANY,0,0 }, tstep = { RAVI_TNUMINT,0,0 };
    Fornuminfo *info = NULL;
    exp1(ls, &tidx);  /* initial value */
    checknext(ls, ',');
    exp1(ls, &tlimit);  /* limit */
    if (testnext(ls, ','))
      exp1(ls, &tstep);  /* optional step */
    else {  /* default step = 1 */
      tstep.is_constant = 1;
      tstep.int_value = 1;
      luaK_codek(fs, fs->freereg, luaK_intK(fs, 1));
      luaK_reserveregs(fs, 1);
    }
    if (tidx.type == tlimit.type && tlimit.type == tstep.type && (tidx.type == RAVI_TNUMFLT || tidx.type == RAVI_TNUMINT)) {
      if (tidx.type == RAVI_TNUMINT && tstep.is_constant)
        info = &tstep;
      /* Ok so we have an integer or double */
      vidx->ravi_type = vlimit->ravi_type = vstep->ravi_type = vvar->ravi_type = tidx.type;
      DEBUG_VARS(raviY_printf(fs, "fornum -> setting type for index %v\n", vidx));
      DEBUG_VARS(raviY_printf(fs, "fornum -> setting type for limit %v\n", vlimit));
      DEBUG_VARS(raviY_printf(fs, "fornum -> setting type for step %v\n", vstep));
      DEBUG_VARS(raviY_printf(fs, "fornum -> setting type for variable %v\n", vvar));
    }
    forbody(ls, base, line, 1, 1, info);
  }
   

Handling of Upvalues
====================
Upvalues can be used to update local variables that have static typing specified. So this means that upvalues need to be annotated with types as well and any operation that updates an upvalue must be type checked. To support this the Lua parser has been enhanced to record the type of an upvalue in ``Upvaldesc``::

  /*
  ** Description of an upvalue for function prototypes
  */
  typedef struct Upvaldesc {
    TString *name;  /* upvalue name (for debug information) */
    ravitype_t type; /* RAVI type of upvalue */
    lu_byte instack;  /* whether it is in stack */
    lu_byte idx;  /* index of upvalue (in stack or in outer function's list) */
  } Upvaldesc;


Whenever a new upvalue is referenced, we assign the type of the the upvalue to the expression in function ``singlevaraux()`` - relevant code is shown below::

  static int singlevaraux (FuncState *fs, TString *n, expdesc *var, int base) {
    /* ... omitted code ... */  
      int idx = searchupvalue(fs, n);  /* try existing upvalues */
      if (idx < 0) {  /* not found? */
        if (singlevaraux(fs->prev, n, var, 0) == VVOID) /* try upper levels */
          return VVOID;  /* not found; is a global */
        /* else was LOCAL or UPVAL */
        idx  = newupvalue(fs, n, var);  /* will be a new upvalue */
      }
      init_exp(var, VUPVAL, idx, fs->f->upvalues[idx].type); /* RAVI : set upvalue type */
      return VUPVAL;
      /* ... omitted code ... */
  }

The function ``newupvalue()`` sets the type of a new upvalue::

  /* create a new upvalue */
  static int newupvalue (FuncState *fs, TString *name, expdesc *v) {
    Proto *f = fs->f;
    int oldsize = f->sizeupvalues;
    checklimit(fs, fs->nups + 1, MAXUPVAL, "upvalues");
    luaM_growvector(fs->ls->L, f->upvalues, fs->nups, f->sizeupvalues,
                  Upvaldesc, MAXUPVAL, "upvalues");
    while (oldsize < f->sizeupvalues) f->upvalues[oldsize++].name = NULL;

    f->upvalues[fs->nups].instack = (v->k == VLOCAL);
    f->upvalues[fs->nups].idx = cast_byte(v->u.info);
    f->upvalues[fs->nups].name = name;
    f->upvalues[fs->nups].type = v->ravi_type;
    luaC_objbarrier(fs->ls->L, f, name);
    return fs->nups++;
  }

When we need to generate assignments to an upvalue (OP_SETUPVAL) we need to use more specialized opcodes that do the necessary conversion at runtime. This is handled in ``luaK_storevar()`` in ``lcode.c``::


  /* Emit store for LHS expression. */
  void luaK_storevar (FuncState *fs, expdesc *var, expdesc *ex) {
    switch (var->k) {
      /* ... omitted code .. */
      case VUPVAL: {
        OpCode op = check_valid_setupval(fs, var, ex);
        int e = luaK_exp2anyreg(fs, ex);
        luaK_codeABC(fs, op, e, var->u.info, 0);
        break;
      }
      /* ... omitted code ... */
    }
  }

  static OpCode check_valid_setupval(FuncState *fs, expdesc *var, expdesc *ex) {
    OpCode op = OP_SETUPVAL;
    if (var->ravi_type != RAVI_TANY && var->ravi_type != ex->ravi_type) {
      if (var->ravi_type == RAVI_TNUMINT)
        op = OP_RAVI_SETUPVALI;
      else if (var->ravi_type == RAVI_TNUMFLT)
        op = OP_RAVI_SETUPVALF;
      else if (var->ravi_type == RAVI_TARRAYINT)
        op = OP_RAVI_SETUPVALAI;
      else if (var->ravi_type == RAVI_TARRAYFLT)
        op = OP_RAVI_SETUPVALAF;
      else
        luaX_syntaxerror(fs->ls,
                      luaO_pushfstring(fs->ls->L, "Invalid assignment of "
                                                   "upvalue: upvalue type "
                                                   "%d, expression type %d",
                                        var->ravi_type, ex->ravi_type));
    }
    return op;
  }

VM Enhancements
===============
A number of new opcodes are introduced to allow type specific operations.

Currently there are specialized versions of ``ADD``, ``SUB``, ``MUL`` and ``DIV`` operations. This will be extended to cover additional operators such as ``IDIV``.
The ``ADD`` and ``MUL`` operations are implemented in a similar way. Both allow a second operand to be encoded directly in the ``C`` operand - when the value is a constant in the range [0,127]. 

One thing to note is that apart from division if an operation involves constants it is folded by Lua. Divisions are treated specially - an expression involving the ``0`` constant is not folded, even when the ``0`` is a numerator. Also worth noting is that DIV operator results in a float even when two integers are divided; you have to use ``IDIV`` to get an integer result - this opcode triggered in Lua 5.3 when the ``//`` operator is used. 

A divide by zero when using integers causes a run time error, whereas for floating point operation the result is NaN. 

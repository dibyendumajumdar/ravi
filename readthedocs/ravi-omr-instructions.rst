===============================
Build Ravi with Eclipse OMR JIT 
===============================

.. contents:: Table of Contents
   :depth: 2
   :backlinks: top

Overview
========
.. note:: The Eclipse OMR JIT backend is work in progress. The code generation is not yet optimal.

Recently support has been added in Ravi to use the Eclipse OMR JIT backend. 
A `trimmed down version of the Eclipse OMR JIT <https://github.com/dibyendumajumdar/nj>`_ is used to ensure that the resulting 
binaries are smaller in size. 

The main advantages / disadvantages of the OMR JIT backend over LLVM are:

* The OMR JIT backend is much smaller compared to LLVM. On my iMac it takes less than 3 minutes to compile and build the library.
* The OMR JIT engine contains an optimizing compiler, therefore the generated code is much better than say `NanoJIT <https://github.com/dibyendumajumdar/nanojit>`_, 
  although not as good as LLVM.

The approach taken with the OMR JIT backend is somewhat different compared with the LLVM backend.

* An intermediate C compiler is used; this is based on the `dmr_C <https://github.com/dibyendumajumdar/dmr_c>`_ project. Using a C intermediate layer makes
  development of the JIT backend easier to evolve. In comparison the LLVM backed was written by hand, using the LLVM api.
* Users can view the intermediate C code for a Lua function by simply invoking ``ravi.dumpir(function)`` on any function:

::

  Ravi 5.3.4
  Copyright (C) 1994-2017 Lua.org, PUC-Rio
  Portions Copyright (C) 2015-2017 Dibyendu Majumdar
  Options assertions ltests omrjit
  > x = function() print 'hello world' end
  > ravi.dumpir(x)

Above results in (note that only the function code is shown below)::

  int jit_function(lua_State *L) {
    int error_code = 0;
    lua_Integer i = 0;
    lua_Integer ic = 0;
    lua_Number n = 0.0;
    lua_Number nc = 0.0;
    int result = 0;
    StkId ra = NULL;
    StkId rb = NULL;
    StkId rc = NULL;
    lua_Unsigned ukey = 0;
    lua_Integer *iptr = NULL;
    lua_Number *nptr = NULL;
    Table *t = NULL;
    CallInfo *ci = L->ci;
    LClosure *cl = clLvalue(ci->func);
    TValue *k = cl->p->k;
    StkId base = ci->u.l.base;
    ra = R(0);
    rc = K(0);
    raviV_gettable_sskey(L, cl->upvals[0]->v, rc, ra);
    base = ci->u.l.base;
    ra = R(1);
    rb = K(1);
    setobj2s(L, ra, rb);
    L->top = R(2);
    ra = R(0);
    result = luaD_precall(L, ra, 0, 1);
    if (result) {
      if (result == 1 && 0 >= 0)
        L->top = ci->top;
      }
      else {  /* Lua function */
        result = luaV_execute(L);
      if (result) L->top = ci->top;
    }
    base = ci->u.l.base;
    ra = R(0);
    if (cl->p->sizep > 0) luaF_close(L, base);
    result = (1 != 0 ? 1 - 1 : cast_int(L->top - ra));
    return luaD_poscall(L, ci, ra, result);
    Lraise_error:
    raise_error(L, error_code); /* does not return */
    return 0;
  }

Build Dependencies
==================

* `CMake <https://cmake.org/>`_ is required for more advanced builds
* On Windows you will need Visual Studio 2017 Community edition

Build Instructions
==================
* Ravi uses a cut-down version of the `Eclipse OMR JIT engine <https://github.com/dibyendumajumdar/nj>`_. First build this library and install it.
* The Ravi CMake build assumes you have installed the OMR JIT library under ``\Software\omr`` on Windows and ``$HOME/Software/omr`` on Linux or Mac OSX.
* Now you can build Ravi as follows on Linux or Mac OSX::

  cd build
  cmake -DOMR_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
  make

If you did not use the default locations above to install OMR, then you will need to amend the file ``cmake/FindOMRJIT.cmake``.

JIT API for OMR backend
=======================
auto mode
  in this mode the compiler decides when to compile a Lua function. The current implementation is very simple - 
  any Lua function call is checked to see if the bytecodes contained in it can be compiled. If this is true then 
  the function is compiled provided either a) function has a fornum loop, or b) it is largish (greater than 150 bytecodes) 
  or c) it is being executed many times (> 50). Because of the simplistic behaviour performance the benefit of JIT
  compilation is only available if the JIT compiled functions will be executed many times so that the cost of JIT 
  compilation can be amortized.   
manual mode
  in this mode user must explicitly request compilation. This is the default mode. This mode is suitable for library 
  developers who can pre compile the functions in library module table.

A JIT api is available with following functions:

``ravi.jit([b])``
  returns enabled setting of JIT compiler; also enables/disables the JIT compiler; defaults to true
``ravi.auto([b [, min_size [, min_executions]]])``
  returns setting of auto compilation and compilation thresholds; also sets the new settings if values are supplied; defaults are false, 150, 50.
``ravi.compile(func_or_table[, options])``
  compiles a Lua function (or functions if a table is supplied) if possible, returns ``true`` if compilation was 
  successful for at least one function. 
  ``options`` is an optional table with compilation options - in particular, 
  ``omitArrayGetRangeCheck`` if set true disables range checks in array get operations to improve performance in some cases.
  ``inlineLuaArithmeticOperators`` if set to true enables generation of inline code for Lua arithemtic op codes such as
  ``OP_ADD``, ``OP_MUL`` and ``OP_SUB``. 
``ravi.iscompiled(func)``
  returns the JIT status of a function
``ravi.dumplua(func)``
  dumps the Lua bytecode of the function
``ravi.dumpir(func)``
  dumps the C intermediate code for a Lua function
``ravi.optlevel([n])``
  sets optimization level (0, 1, 2); defaults to 1. 
``ravi.verbosity([b])``
  If set to 1 then everytime a Lua function is compiled the C intermediate code will be dumped.

Compiler Trace Output from OMR
==============================
The OMR JIT backend can generate detailed compilation traces if you define following environment variable::

  export TR_Options=traceIlGen,traceFull,log=trtrace.log

Note that the generated traces can be huge!


Ravi's JIT Implementation
=========================
There are two JIT backends available for Ravi.

* The LLVM JIT backend is described in `JIT Status <http://the-ravi-programming-language.readthedocs.org/en/latest/ravi-jit-status.html>`_ page.
* The Eclipse OMR JIT backend is in development. This backend uses a C intermediate layer.

JIT API
-------
auto mode
  in this mode the compiler decides when to compile a Lua function. The current implementation is very simple - 
  any Lua function call is checked to see if the bytecodes contained in it can be compiled. If this is true then 
  the function is compiled provided either a) function has a fornum loop, or b) it is largish (greater than 150 bytecodes) 
  or c) it is being executed many times (> 50). Because of the simplistic behaviour performance the benefit of JIT
  compilation is only available if the JIT compiled functions will be executed many times so that the cost of JIT compilation can be amortized.   
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
  successful for at least one function. ``options`` is an optional table with compilation options - in particular 
  ``omitArrayGetRangeCheck`` - which disables range checks in array get operations to improve performance in some cases. 
  Note that at present if the first argument is a table of functions and has more than 100 functions then only the
  first 100 will be compiled. You can invoke compile() repeatedly on the table until it returns false. Each 
  invocation leads to a new module being created; any functions already compiled are skipped.
``ravi.iscompiled(func)``
  returns the JIT status of a function
``ravi.dumplua(func)``
  dumps the Lua bytecode of the function
``ravi.dumpir(func)``
  (deprecated) dumps the IR of the compiled function (only if function was compiled; only available in LLVM 4.0 and earlier)
``ravi.dumpasm(func)``
  (deprecated) dumps the machine code using the currently set optimization level (only if function was compiled; only available in LLVM version 4.0 and earlier)
``ravi.optlevel([n])``
  sets LLVM optimization level (0, 1, 2, 3); defaults to 2. These levels are handled by reusing LLVMs default pass definitions which are geared towards C/C++ programs, but appear to work well here. If level is set to 0, then an attempt is made to use fast instruction selection to further speed up compilation.
``ravi.sizelevel([n])``
  sets LLVM size level (0, 1, 2); defaults to 0
``ravi.tracehook([b])``
  Enables support for line hooks via the debug api. Note that enabling this option will result in inefficient JIT as a call to a C function will be inserted at beginning of every Lua bytecode boundary; use this option only when you want to use the debug api to step through code line by line
``ravi.verbosity([b])``
  Controls the amount of verbose messages generated during compilation. Currently only available for LLVM.

Performance
===========
For performance benchmarks please visit the `Ravi Performance Benchmarks <http://the-ravi-programming-language.readthedocs.org/en/latest/ravi-benchmarks.html>`_ page.

To obtain the best possible performance, types must be annotated so that Ravi's JIT compiler can generate efficient code. 
Additionally function calls are expensive - as the JIT compiler cannot inline function calls, all function calls go via the Lua call protocol which has a large overhead. This is true for both Lua functions and C functions. For best performance avoid function calls inside loops.

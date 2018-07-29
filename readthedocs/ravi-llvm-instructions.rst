===================================
Building Ravi with LLVM JIT backend
===================================

.. contents:: Table of Contents
   :depth: 2
   :backlinks: top

Quick build without JIT
=======================
A Makefile is supplied for a simple build without the JIT on Unix platforms. Just run ``make`` and follow instructions. You may need to customize the Makefiles. 

For building Ravi with JIT options please read on.

Build Dependencies
==================

* `CMake <https://cmake.org/>`_ is required for more advanced builds
* On Windows you will need Visual Studio 2017 Community edition
* LLVM versions >= 3.5

LLVM JIT Backend
================
Following versions of LLVM work with Ravi.

* LLVM 3.7, 3.8, 3.9, 4.0, 5.0, 6.0
* LLVM 3.5 and 3.6 should also work but have not been recently tested

Unless otherwise noted the instructions below should work for LLVM 3.9 and later.

Since LLVM 5.0 Ravi has begun to use the new ORC JIT apis. These apis are more memory efficient 
compared to the MCJIT apis because they release the Module IR as early as possible, whereas with 
MCJIT the Module IR hangs around as long as the compiled code is held. Because of this significant
improvement, I recommend using LLVM 5.0 and above.

Building LLVM on Windows
------------------------
I built LLVM from source. I used the following sequence from the VS2017 command window::

  cd \github\llvm
  mkdir build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=c:\LLVM -DLLVM_TARGETS_TO_BUILD="X86" -G "Visual Studio 15 2017 Win64" ..  

I then opened the generated solution in VS2017 and performed a INSTALL build from there. Above will build the 64-bit version of LLVM libraries. To build a 32-bit version omit the ``Win64`` parameter. 

.. note:: Note that if you perform a Release build of LLVM then you will also need to do a Release build of Ravi otherwise you will get link errors.

Building LLVM on Ubuntu
-----------------------
On Ubuntu I found that the official LLVM distributions don't work with CMake. The CMake config files appear to be broken.
So I ended up downloading and building LLVM from source and that worked. The approach is similar to that described for MAC OS X below.

Building LLVM on MAC OS X
-------------------------
I am using Max OSX El Capitan. Pre-requisites are XCode 7.x and CMake.
Ensure cmake is on the path.
Assuming that LLVM source has been extracted to ``$HOME/llvm-3.7.0.src`` I follow these steps::

  cd llvm-3.7.0.src
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/LLVM -DLLVM_TARGETS_TO_BUILD="X86" ..
  make install

Building Ravi with LLVM JIT backend enabled
-------------------------------------------
I am developing Ravi using Visual Studio 2017 Community Edition on Windows 10 64bit, gcc on Unbuntu 64-bit, and clang/Xcode on MAC OS X. I was also able to successfully build a Ubuntu version on Windows 10 using the newly released Ubuntu/Linux sub-system for Windows 10.

.. note:: Location of cmake files prior to LLVM 3.9 was ``$LLVM_INSTALL_DIR/share/llvm/cmake``.

Assuming that LLVM has been installed as described above, then on Windows I invoke the cmake config as follows::

  cd build
  cmake -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=c:\ravi -DLLVM_DIR=c:\LLVM\lib\cmake\llvm -G "Visual Studio 15 2017 Win64" ..

I then open the solution in VS2017 and do a build from there.

On Ubuntu I use::

  cd build
  cmake -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
  make

Note that on a clean install of Ubuntu 15.10 I had to install following packages:

* cmake
* git
* libreadline-dev

On MAC OS X I use::

  cd build
  cmake -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Release -G "Xcode" ..

I open the generated project in Xcode and do a build from there. You can also use the command line build tools if you wish - generate the make files in the same way as for Linux.

Building without JIT
====================
You can omit ``-DLLVM_JIT=ON`` and ``OMR_JIT=ON`` options to build Ravi with a null JIT implementation.

Building Static Libraries
=========================
By default the build generates a shared library for Ravi. You can choose to create a static library and statically linked executables by supplying the argument ``-DSTATIC_BUILD=ON`` to CMake.

JIT API
-------
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
  dumps the IR of the compiled function (only if function was compiled; only available in LLVM 4.0 and earlier)
``ravi.dumpasm(func)``
  (deprecated) dumps the machine code using the currently set optimization level (only if function was compiled; only available in LLVM version 4.0 and earlier)
``ravi.optlevel([n])``
  sets LLVM optimization level (0, 1, 2, 3); defaults to 2. These levels are handled by reusing LLVMs default pass definitions which are geared towards C/C++ programs, but appear to work well here. If level is set to 0, then an attempt is made to use fast instruction selection to further speed up compilation.
``ravi.sizelevel([n])``
  sets LLVM size level (0, 1, 2); defaults to 0
``ravi.tracehook([b])``
  Enables support for line hooks via the debug api. Note that enabling this option will result in inefficient JIT as a call to a C function will be inserted at beginning of every Lua bytecode boundary; use this option only when you want to use the debug api to step through code line by line
``ravi.verbosity([b])``
  Controls the amount of verbose messages generated during compilation.

Performance
===========
For performance benchmarks please visit the `Ravi Performance Benchmarks <http://the-ravi-programming-language.readthedocs.org/en/latest/ravi-benchmarks.html>`_ page.

To obtain the best possible performance, types must be annotated so that Ravi's JIT compiler can generate efficient code. 
Additionally function calls are expensive - as the JIT compiler cannot inline function calls, all function calls go via the Lua call protocol which has a large overhead. This is true for both Lua functions and C functions. For best performance avoid function calls inside loops.

Testing
=======
I test the build by running a modified version of Lua 5.3.3 test suite. These tests are located in the ``lua-tests`` folder. Additionally I have ravi specific tests in the ``ravi-tests`` folder. There is a also a travis build that occurs upon commits - this build runs the tests as well.

.. note:: To thoroughly test changes, you need to invoke CMake with ``-DCMAKE_BUILD_TYPE=Debug`` option. This turns on assertions, memory checking, and also enables an internal module used by Lua tests.


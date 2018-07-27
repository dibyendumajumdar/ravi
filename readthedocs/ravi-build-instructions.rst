=============
Building Ravi
=============

.. contents:: Table of Contents
   :depth: 1
   :backlinks: top

Quick build without JIT
=======================
A Makefile is supplied for a simple build without the JIT. Just run ``make`` and follow instructions. You may need to customize the Makefiles. 

For building Ravi with JIT options please read on.

Build Dependencies
==================

* CMake is required for more advanced builds
* On Windows you will need Visual Studio 2017 Community edition

LLVM JIT Backend
================
Ravi can be built with or without LLVM. Following versions of LLVM work with Ravi.

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


Eclipse OMR JIT Backend
=======================
* Ravi uses a cut-down version of the `Eclipse OMR JIT engine <https://github.com/dibyendumajumdar/nj>`_. First build this library and install it.
* The Ravi CMake build assumes you have installed the OMR JIT library under ``\Software\omr`` on Windows and ``$HOME/Software/omr`` on Linux or Mac OSX.
* Now you can build Ravi as follows on Linux or Mac OSX:

  cd build
  cmake -DOMR_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
  make

Building without JIT
====================
You can omit ``-DLLVM_JIT=ON`` and ``OMR_JIT=ON`` options to build Ravi with a null JIT implementation.

Building Static Libraries
=========================
By default the build generates a shared library for Ravi. You can choose to create a static library and statically linked executables by supplying the argument ``-DSTATIC_BUILD=ON`` to CMake.

Build Artifacts
===============
The Ravi build creates a shared or static depending upon options supplied to CMake, the Ravi executable and some test programs. Additionally when JIT compilation is switched off, the ``ravidebug`` executable is generated which is the `debug adapter for use by Visual Studio Code <https://github.com/dibyendumajumdar/ravi/tree/master/vscode-debugger>`_. 

The ``ravi`` command recognizes following environment variables. Note that these are only for internal debugging purposes.

``RAVI_DEBUG_EXPR``
  if set to a value this triggers debug output of expression parsing
``RAVI_DEBUG_CODEGEN``
  if set to a value this triggers a dump of the code being generated
``RAVI_DEBUG_VARS``
  if set this triggers a dump of local variables construction and destruction

Also see section above on available API for dumping either Lua bytecode or LLVM IR for compiled code.

Testing
=======
I test the build by running a modified version of Lua 5.3.3 test suite. These tests are located in the ``lua-tests`` folder. Additionally I have ravi specific tests in the ``ravi-tests`` folder. There is a also a travis build that occurs upon commits - this build runs the tests as well.

.. note:: To thoroughly test changes, you need to invoke CMake with ``-DCMAKE_BUILD_TYPE=Debug`` option. This turns on assertions, memory checking, and also enables an internal module used by Lua tests.


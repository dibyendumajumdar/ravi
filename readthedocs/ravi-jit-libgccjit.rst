========================================
JIT Compilation for Ravi using ``libgccjit``
========================================

Introduction
------------
The latest `gcc 5.2 release <http://gcc.gnu.org/>`_ contains a new component called ``libgccjit``. This basically exposes an API via a shared library to the compilation functions within gcc. 

I am keen to provide support for this in Ravi. From initial look it seems to contain all the features I need to implement a JIT compiler for Ravi. Obviously having implemented the LLVM version it is going to be a little easier as I can mostly do a port of the LLVM version.

License
-------
Ravi itself is licensed under MIT license (including the code that implements the JIT compiler) - however I think that when linked to ``libgccjit`` the effective license will be GPLv3. 

Why another JIT engine?
-----------------------
Well partly as I feel I have a moral obligation to support gcc, given it has been instrumental in bringing about the OpenSource / Free Software ecosystem. 

Secondly I am always looking for alternatives that will let me reduce the footprint of Ravi. The ``libgccjit`` is offered as a shared library - this is a great thing. I hate to have to statically link LLVM. 

LLVM implementation and ``libgccjit`` implementation will both be kept in sync so that user can choose either option. Right now the LLVM implementation is more advanced and new features are implemented there first and then ported to the ``libgccjit`` implementation.

Building GCC
------------
I am running Ubuntu 14.04 LTS on VMWare virtual machine.

I built gcc 5.2 from source as follows.

1. Extracted gcc-5.2 source to ``~/gcc-5.2.0``.
2. Created a build folder ``~/buildgcc``.
3. Installed various pre-requisites for gcc.
4. Then ran following from inside the build folder::

     ../gcc-5.2.0/configure --prefix=~/local --enable-host-shared --enable-languages=jit,c++ --disable-bootstrap --disable-multilib

5. Next performed the build as follows::

     make
     make install

On Mac OSX Yosemite
-------------------
It appears that the `HomeBrew <http://brew.sh/>`_ project supports creating the ``libgccjit 5.2`` library. However the default formula doesn't quite work and needs to be patched for libgccjit to work properly. A patched formula can be found at `here <https://github.com/dibyendumajumdar/ravi/blob/master/patches/gcc.rb>`_. To use the patched version edit the gcc formula and copy the patched version. After that following should build and install gcc 5.2 including the JIT library::

   brew install gcc --with-jit --without-multilib
   
Current Status
--------------
Many bytecodes are now compiled - see below for detailed status. The current version of Ravi passes the Lua test cases using ``libgccjit``.

Building Ravi with ``libgccjit`` on Linux
-------------------------------------
.. warning:: Note that right now the Ravi's ``libgccjit`` based JIT implementation is work in progress - please expect bugs. 

You can build Ravi with ``libgccjit`` linked in as follows::

  mkdir build
  cd build
  export LD_LIBRARY_PATH=~/local/lib64:$LD_LIBRARY_PATH
  cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=~/local/bin/gcc -DCMAKE_CXX_COMPILER=~/local/bin/g++ -DLLVM_DIR=~/LLVM/share/llvm/cmake -DLLVM_JIT=ON -DGCC_JIT=OFF ..
  make

Above assumes that gccjit is installed under ``~/local`` as described in Building GCC section above.

A helloworld test program is built. To run it though you need to ensure that your ``PATH`` and ``LD_LIBRARY_PATH`` variables include ``~/local/bin`` and ``~/local/lib`` respectively.

Initial Observations 
--------------------
In terms of packaging ``libgccjit`` consists of a C header file, a C++ header file and one shared library. That is pretty neat as it simplifies the usage.

Setting up of the Lua types is proving easier in ``libgccjit`` due to the fact that Lua uses unions extensively and ``libgccjit`` supports defining union types. This means that most of the Lua types can be translated more naturally. LLVM on the other hand does not support unions so I had to carefully define structs that would match the size of the union, and in the JIT compilation use casts where needed.

JIT Status of Lua/Ravi Bytecodes
---------------------------------
Following is the status as of 4 July 2015.

+-------------------------+----------+--------------------------------------------------+
| name                    | JITed?   | description                                      |
+=========================+==========+==================================================+
| OP_MOVE                 | YES      | R(A) := R(B)                                     |
+-------------------------+----------+--------------------------------------------------+
| OP_LOADK                | YES      | R(A) := Kst(Bx)                                  |
+-------------------------+----------+--------------------------------------------------+
| OP_LOADKX               | NO       | R(A) := Kst(extra arg)                           |
+-------------------------+----------+--------------------------------------------------+
| OP_LOADBOOL             | YES      | R(A) := (Bool)B; if (C) pc++                     |
+-------------------------+----------+--------------------------------------------------+
| OP_LOADNIL              | YES      | R(A), R(A+1), ..., R(A+B) := nil                 |
+-------------------------+----------+--------------------------------------------------+
| OP_GETUPVAL             | YES      | R(A) := UpValue[B]                               |
+-------------------------+----------+--------------------------------------------------+
| OP_GETTABUP             | YES      | R(A) := UpValue[B][RK(C)]                        |
+-------------------------+----------+--------------------------------------------------+
| OP_GETTABLE             | YES      | R(A) := R(B)[RK(C)]                              |
+-------------------------+----------+--------------------------------------------------+
| OP_SETTABUP             | YES      | UpValue[A][RK(B)] := RK(C)                       |
+-------------------------+----------+--------------------------------------------------+
| OP_SETUPVAL             | YES      | UpValue[B] := R(A)                               |
+-------------------------+----------+--------------------------------------------------+
| OP_SETTABLE             | YES      | R(A)[RK(B)] := RK(C)                             |
+-------------------------+----------+--------------------------------------------------+
| OP_NEWTABLE             | YES      | R(A) := {} (size = B,C)                          |
+-------------------------+----------+--------------------------------------------------+
| OP_SELF                 | YES      | R(A+1) := R(B); R(A) := R(B)[RK(C)]              |
+-------------------------+----------+--------------------------------------------------+
| OP_ADD                  | YES      | R(A) := RK(B) + RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_SUB                  | YES      | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_MUL                  | YES      | R(A) := RK(B) * RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_MOD                  | NO       | R(A) := RK(B) % RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_POW                  | NO       | R(A) := RK(B) ^ RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_DIV                  | YES      | R(A) := RK(B) / RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_IDIV                 | NO       | R(A) := RK(B) // RK(C)                           |
+-------------------------+----------+--------------------------------------------------+
| OP_BAND                 | NO       | R(A) := RK(B) & RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_BOR                  | NO       | R(A) := RK(B) | RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_BXOR                 | NO       | R(A) := RK(B) ~ RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_SHL                  | NO       | R(A) := RK(B) << RK(C)                           |
+-------------------------+----------+--------------------------------------------------+
| OP_SHR                  | NO       | R(A) := RK(B) >> RK(C)                           |
+-------------------------+----------+--------------------------------------------------+
| OP_UNM                  | NO       | R(A) := -R(B)                                    |
+-------------------------+----------+--------------------------------------------------+
| OP_BNOT                 | NO       | R(A) := ~R(B)                                    |
+-------------------------+----------+--------------------------------------------------+
| OP_NOT                  | YES      | R(A) := not R(B)                                 |
+-------------------------+----------+--------------------------------------------------+
| OP_LEN                  | YES      | R(A) := length of R(B)                           |
+-------------------------+----------+--------------------------------------------------+
| OP_CONCAT               | YES      | R(A) := R(B).. ... ..R(C)                        |
+-------------------------+----------+--------------------------------------------------+
| OP_JMP                  | YES      | c+=sBx; if (A) close all upvalues >= R(A - 1)    |
+-------------------------+----------+--------------------------------------------------+
| OP_EQ                   | YES      | if ((RK(B) == RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_LT                   | YES      | if ((RK(B) <  RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_LE                   | YES      | if ((RK(B) <= RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_TEST                 | YES      | if not (R(A) <=> C) then pc++                    |
+-------------------------+----------+--------------------------------------------------+
| OP_TESTSET              | YES      | if (R(B) <=> C) then R(A) := R(B) else pc++      |
+-------------------------+----------+--------------------------------------------------+
| OP_CALL                 | YES      | R(A), .. ,R(A+C-2) := R(A)(R(A+1), .. ,R(A+B-1)) |
+-------------------------+----------+--------------------------------------------------+
| OP_TAILCALL             | YES      | return R(A)(R(A+1), ... ,R(A+B-1))               |
|                         |          | Compiled as OP_CALL so no tail call optimization |
+-------------------------+----------+--------------------------------------------------+
| OP_RETURN               | YES      | return R(A), ... ,R(A+B-2) (see note)            |
+-------------------------+----------+--------------------------------------------------+
| OP_FORLOOP              | NO       | R(A)+=R(A+2);                                    |
|                         |          | if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) } |
+-------------------------+----------+--------------------------------------------------+
| OP_FORPREP              | NO       | R(A)-=R(A+2); pc+=sBx                            |
+-------------------------+----------+--------------------------------------------------+
| OP_TFORCALL             | YES      | R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2));   |
+-------------------------+----------+--------------------------------------------------+
| OP_TFORLOOP             | YES      | if R(A+1) ~= nil then { R(A)=R(A+1); pc += sBx } |
+-------------------------+----------+--------------------------------------------------+
| OP_SETLIST              | YES      | R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B	        |
+-------------------------+----------+--------------------------------------------------+
| OP_CLOSURE              | YES      | R(A) := closure(KPROTO[Bx])                      |
+-------------------------+----------+--------------------------------------------------+
| OP_VARARG               | YES      | R(A), R(A+1), ..., R(A+B-2) = vararg             |
+-------------------------+----------+--------------------------------------------------+
| OP_EXTRAARG             | N/A      | extra (larger) argument for previous opcode      |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_NEWARRAYI       | YES      | R(A) := array of int                             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_NEWARRAYF       | YES      | R(A) := array of float                           |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_LOADIZ          | YES      | R(A) := tointeger(0)                             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_LOADFZ          | YES      | R(A) := tonumber(0)                              |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_ADDFF           | YES      | R(A) := RK(B) + RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_ADDFI           | YES      | R(A) := RK(B) + RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_ADDII           | YES      | R(A) := RK(B) + RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBFF           | YES      | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBFI           | YES      | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBIF           | YES      | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBII           | YES      | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MULFF           | YES      | R(A) := RK(B) * RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MULFI           | YES      | R(A) := RK(B) * RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MULII           | YES      | R(A) := RK(B) * RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_DIVFF           | YES      | R(A) := RK(B) / RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_DIVFI           | YES      | R(A) := RK(B) / RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_DIVIF           | YES      | R(A) := RK(B) / RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_DIVII           | YES      | R(A) := RK(B) / RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_TOINT           | YES      | R(A) := toint(R(A))                              |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_TOFLT           | YES      | R(A) := tofloat(R(A))                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_TOARRAYI        | YES      | R(A) := to_arrayi(R(A))                          |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_TOARRAYF        | YES      | R(A) := to_arrayf(R(A))                          |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MOVEI           | YES      | R(A) := R(B), check R(B) is integer              |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MOVEF           | YES      | R(A) := R(B), check R(B) is number               |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MOVEAI          | YES      | R(A) := R(B), check R(B) is array of integer     |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MOVEAF          | YES      | R(A) := R(B), check R(B) is array of numbers     |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_GETTABLE_AI     | YES      | R(A) := R(B)[RK(C)] where R(B) is array of       |
|                         |          | integers and RK(C) is integer                    |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_GETTABLE_AF     | YES      | R(A) := R(B)[RK(C)] where R(B) is array of       |
|                         |          | numbers and RK(C) is integer                     |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETTABLE_AI     | YES      | R(A)[RK(B)] := RK(C) where RK(B) is an integer   |
|                         |          | R(A) is array of integers, and RK(C) is an int   |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETTABLE_AF     | YES      | R(A)[RK(B)] := RK(C) where RK(B) is an integer   |
|                         |          | R(A) is array of numbers, and RK(C) is a number  |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_FORLOOP_IP      | YES      | R(A)+=R(A+2);                                    |
|                         |          | if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) } |
|                         |          | Specialization for integer step > 1              |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_FORPREP_IP      | YES      | R(A)-=R(A+2); pc+=sBx                            |
|                         |          | Specialization for integer step > 1              |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_FORLOOP_I1      | YES      | R(A)+=R(A+2);                                    |
|                         |          | if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) } |
|                         |          | Specialization for integer step == 1             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_FORPREP_I1      | YES      | R(A)-=R(A+2); pc+=sBx                            |
|                         |          | Specialization for integer step == 1             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETUPVALI       | NO       | UpValue[B] := tointeger(R(A))                    |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETUPVALF       | NO       | UpValue[B] := tonumber(R(A))                     |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETUPVALAI      | NO       | UpValue[B] := toarrayint(R(A))                   |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETUPVALAF      | NO       | UpValue[B] := toarrayflt(R(A))                   |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETTABLE_AII    | YES      | R(A)[RK(B)] := RK(C) where RK(B) is an integer   |
|                         |          | R(A) is array of integers, and RK(C) is an int   |
|                         |          | No conversion as input is known to be int        |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETTABLE_AFF    | YES      | R(A)[RK(B)] := RK(C) where RK(B) is an integer   |
|                         |          | R(A) is array of numbers, and RK(C) is a number  |
|                         |          | No conversion as input is known to be float      |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_BAND_II         | NO       | R(A) := RK(B) & RK(C), operands are int          |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_BOR_II          | NO       | R(A) := RK(B) | RK(C), operands are int          |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_BXOR_II         | NO       | R(A) := RK(B) ~ RK(C), operands are int          |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SHL_II          | NO       | R(A) := RK(B) << RK(C), operands are int         |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SHR_II          | NO       | R(A) := RK(B) >> RK(C), operands are int         |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_BNOT_I          | NO       | R(A) := ~R(B), int operand                       |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_EQ_II           | YES      | if ((RK(B) == RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_EQ_FF           | YES      | if ((RK(B) == RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_LT_II           | YES      | if ((RK(B) <  RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_LT_FF           | YES      | if ((RK(B) <  RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_LE_II           | YES      | if ((RK(B) <= RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_LE_FF           | YES      | if ((RK(B) <= RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+

Ravi's libgccjit JIT compiler source
------------------------------------
The libgccjit JIT implementation is in following sources:

* ravijit.h - defines the JIT API
* ravi_gccjit.h - defines the types used by the code generator, and declares prototypes

* ravijit.cpp - basic JIT infrastructure and Ravi API definition
* ravi_gcctypes.c - contains JIT type definitions for Lua objects 
* ravi_gcccodegen.c - JIT compiler - main driver for compiling Lua bytecodes 
* ravi_gccload.c - implements OP_LOADK and OP_MOVE, and related operations, also OP_LOADBOOL
* ravi_gcccomp.c - implements OP_EQ, OP_LT, OP_LE, OP_TEST and OP_TESTSET.
* ravi_gccreturn.c - implements OP_RETURN
* ravi_gccforprep.c - implements OP_RAVI_FORPREP_I1 and OP_RAVI_FORPREP_IP
* ravi_gccforloop.c - implements OP_RAVI_FORLOOP_I1 and OP_RAVI_FORLOOP_IP
* ravi_gcctforcall.c - implements OP_TFORCALL and OP_TFORLOOP
* ravi_gccarith1.c - implements various type specialized arithmetic operations - these are Ravi extensions
* ravi_gccarith2.c - implements Lua opcodes such as OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_UNM
* ravi_gcccall.c - implements OP_CALL, OP_JMP
* ravi_gcctable.c - implements OP_GETTABLE, OP_SETTABLE and various other table operations, OP_SELF, and also upvalue operations
* ravi_gccrest.c - OP_CLOSURE, OP_VARARG, OP_CONCAT

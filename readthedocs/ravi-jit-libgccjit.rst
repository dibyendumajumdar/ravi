========================================
JIT Compilation for Ravi using libgccjit
========================================

Introduction
------------
The latest `gcc 5.1 release <http://gcc.gnu.org/>`_ contains a new component called ``libgccjit``. This basically exposes an API via a shared library to the compilation functions within gcc. 

I am keen to provide support for this in Ravi. From initial look it seems to contain all the features I need to implement a JIT compiler for Ravi. Obviously having implemented the LLVM version it is going to be a little easier as I can mostly do a port of the LLVM version.

License
-------
Ravi with this component will be licensed as GPLv3. Without this component the license will be MIT license.

Why another JIT engine?
-----------------------
Well partly as I feel I have a moral obligation to support gcc, given it has been instrumental in bringing about the OpenSource / Free Software ecosystem. 

Secondly I am always looking for alternatives that will let me reduce the footprint of Ravi. The libgccjit is offered as a shared library - this is a great thing. I hate to have to statically link LLVM. 

LLVM implementation and libgccjit implementation will both be kept in sync so that user can choose either option. 

Building GCC
------------
I am running Ubuntu 14.04 LTS on VMWare virtual machine.

I built gcc 5.1 from source as follows.

1. Unpacked source to ``~/gcc-5.1.0``
2. Created a build folder ``~/buildgcc``
3. Installed various pre-requisites for gcc.
4. Then ran following from inside the build folder::

     ../gcc-5.1.0/configure --prefix=~/local --enable-host-shared --enable-languages=jit,c++ --disable-bootstrap --disable-multilib

5. Next performed the build as follows::

     make
     make install

Current Status
--------------
Work on this started only recently (8 June 2015) so not much to show yet. But expectation is that there will be a working implementation by end June - the strategy is to port the existing LLVM implementation to equivalent libgccjit implementation.

Building Ravi with libgccjit on Linux
-------------------------------------
Note that right now the libgccjit implementation is not yet functional. However you can build Ravi with libgccjit linked in as follows::

  mkdir build
  cd build
  cmake -DLLVM_JIT=OFF -DGCC_JIT=ON ..
  make

Above assumes that gccjit is installed under ``~/local`` as described in Building GCC section above.

A helloworld test program is built. To run it though you need to ensure that your ``PATH`` and ``LD_LIBRARY_PATH`` variables include ``~/local/bin`` and ``~/local/lib`` respectively.

Initial Observations 
--------------------
In terms of packaging ``libgccjit`` consists of a C header file, a C++ header file and one shared library. That is pretty neat as it simplifies the usage.

Setting up of the Lua types is proving easier in ``libgccjit`` due to the fact that Lua uses unions extensively and ``libgccjit`` supports defining union types. This means that most of the Lua types can be translated more naturally. LLVM on the other hand does not support unions so I had to carefully define structs that would match the size of the union, and in the JIT compilation use casts where needed.


JIT Status of Lua/Ravi Bytecodes
---------------------------------
Following is the status as of 28 June 2015.

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
| OP_GETUPVAL             | NO       | R(A) := UpValue[B]                               |
+-------------------------+----------+--------------------------------------------------+
| OP_GETTABUP             | YES      | R(A) := UpValue[B][RK(C)]                        |
+-------------------------+----------+--------------------------------------------------+
| OP_GETTABLE             | NO       | R(A) := R(B)[RK(C)]                              |
+-------------------------+----------+--------------------------------------------------+
| OP_SETTABUP             | NO       | UpValue[A][RK(B)] := RK(C)                       |
+-------------------------+----------+--------------------------------------------------+
| OP_SETUPVAL             | NO       | UpValue[B] := R(A)                               |
+-------------------------+----------+--------------------------------------------------+
| OP_SETTABLE             | NO       | R(A)[RK(B)] := RK(C)                             |
+-------------------------+----------+--------------------------------------------------+
| OP_NEWTABLE             | NO       | R(A) := {} (size = B,C)                          |
+-------------------------+----------+--------------------------------------------------+
| OP_SELF                 | NO       | R(A+1) := R(B); R(A) := R(B)[RK(C)]              |
+-------------------------+----------+--------------------------------------------------+
| OP_ADD                  | NO       | R(A) := RK(B) + RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_SUB                  | NO       | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_MUL                  | NO       | R(A) := RK(B) * RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_MOD                  | NO       | R(A) := RK(B) % RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_POW                  | NO       | R(A) := RK(B) ^ RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_DIV                  | NO       | R(A) := RK(B) / RK(C)                            |
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
| OP_NOT                  | NO       | R(A) := not R(B)                                 |
+-------------------------+----------+--------------------------------------------------+
| OP_LEN                  | NO       | R(A) := length of R(B)                           |
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
| OP_TFORCALL             | NO       | R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2));   |
+-------------------------+----------+--------------------------------------------------+
| OP_TFORLOOP             | NO       | if R(A+1) ~= nil then { R(A)=R(A+1); pc += sBx } |
+-------------------------+----------+--------------------------------------------------+
| OP_SETLIST              | NO       | R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B	        |
+-------------------------+----------+--------------------------------------------------+
| OP_CLOSURE              | YES      | R(A) := closure(KPROTO[Bx])                      |
+-------------------------+----------+--------------------------------------------------+
| OP_VARARG               | YES      | R(A), R(A+1), ..., R(A+B-2) = vararg             |
+-------------------------+----------+--------------------------------------------------+
| OP_EXTRAARG             | N/A      | extra (larger) argument for previous opcode      |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_NEWARRAYI       | NO       | R(A) := array of int                             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_NEWARRAYF       | NO       | R(A) := array of float                           |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_LOADIZ          | YES      | R(A) := tointeger(0)                             |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_LOADFZ          | YES      | R(A) := tonumber(0)                              |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_UNMF            | NO       | R(A) := -R(B) floating point                     |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_UNMI            | NO       | R(A) := -R(B) integer                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_ADDFF           | YES      | R(A) := RK(B) + RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_ADDFI           | YES      | R(A) := RK(B) + RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_ADDII           | YES      | R(A) := RK(B) + RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_ADDFN           | YES      | R(A) := RK(B) + C                                |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_ADDIN           | YES      | R(A) := RK(B) + C                                |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBFF           | YES      | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBFI           | YES      | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBIF           | YES      | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBII           | YES      | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBFN           | YES      | R(A) := RK(B) - C                                |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBNF           | YES      | R(A) := B - RK(C)                                |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBIN           | YES      | R(A) := RK(B) - C                                |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SUBNI           | YES      | R(A) := B - RK(C)                                |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MULFF           | YES      | R(A) := RK(B) * RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MULFI           | YES      | R(A) := RK(B) * RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MULII           | YES      | R(A) := RK(B) * RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MULFN           | YES      | R(A) := RK(B) * C                                |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MULIN           | YES      | R(A) := RK(B) * C                                |
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
| OP_RAVI_TOARRAYI        | NO       | R(A) := to_arrayi(R(A))                          |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_TOARRAYF        | NO       | R(A) := to_arrayf(R(A))                          |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MOVEI           | YES      | R(A) := R(B), check R(B) is integer              |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MOVEF           | YES      | R(A) := R(B), check R(B) is number               |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MOVEAI          | NO       | R(A) := R(B), check R(B) is array of integer     |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MOVEAF          | NO       | R(A) := R(B), check R(B) is array of numbers     |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_GETTABLE_AI     | NO       | R(A) := R(B)[RK(C)] where R(B) is array of       |
|                         |          | integers and RK(C) is integer                    |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_GETTABLE_AF     | NO       | R(A) := R(B)[RK(C)] where R(B) is array of       |
|                         |          | numbers and RK(C) is integer                     |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETTABLE_AI     | NO       | R(A)[RK(B)] := RK(C) where RK(B) is an integer   |
|                         |          | R(A) is array of integers, and RK(C) is an int   |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETTABLE_AF     | NO       | R(A)[RK(B)] := RK(C) where RK(B) is an integer   |
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





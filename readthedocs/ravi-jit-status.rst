Ravi JIT Compilation Status
===========================

Introduction
------------
Ravi uses LLVM for JIT compilation. 

Benefits of using LLVM
----------------------
* LLVM has a well documented intermediate representation called LLVM IR.
* The LLVM ``IRBuilder`` implements type checks so that when LLVM code is being generated, basic type errors are caught by the builder. 
* LLVM provides a verifier to check that the generated IR is valid. This allows the IR to be validated prior to machine code generation.
* All of the LLVM optimization passes can be used.
* The Clang compiler supports generating LLVM IR so that if you want to know what the LLVM IR should look like for a particular piece of code, you can write a small C snippet and have Clang generate the IR for you.
* There is great momentum behind LLVM.
* The LLVM license is not based on GPL, so it is not viral.
* LLVM is much better documented than other products that aim to cover similar ground.
* LLVM's API is well designed and has a layered architecture.

Drawbacks of LLVM
-----------------
* LLVM is huge in size. Lua on its own is tiny - but when linked to LLVM the resulting binary is a monster.
* There is a cost to compiling in LLVM so the benefit of compilation accrues only when a Lua function will be used again and again.
* LLVM cannot be linked as a shared library on Windows and a shared library configuration is not recommended on other platforms as well.
* LLVM's API keeps changing so that with every release of LLVM one has to revise the way it is used.

The Architecture of Ravi's JIT Compilation
------------------------------------------
* The unit of compilation is a Lua function
* Each Lua function is compiled to a Module/Function in LLVM parlance
* The compiled code is attached to the Lua function prototype
* The compiled code is garbage collected as normal by Lua
* The Lua runtime coordinates function calls - so anytime a Lua function is called it goes via the Lua infrastructure. 
* The decision to call a JIT compiled version is made in the Lua Infrastructure (specifically in ``luaD_precall()`` function in ``ldo.c``)
* The JIT compiler translates Lua/Ravi bytecode to LLVM IR - i.e. it does not translate Lua source code.
* There is no inlining of Lua functions.
* Generally the JIT compiler implements the same instructions as in ``lvm.c`` - however for some bytecodes the code calls a C function rather than generating inline IR. These opcodes are OP_LOADNIL, OP_NEWTABLE, OP_RAVI_NEWARRAYINT, OP_RAVI_NEWARRAYFLT, OP_SETLIST, OP_CONCAT, OP_CLOSURE, OP_VARARG, OP_RAVI_SHL_II, OP_RAVI_SHR_II. 
* Ravi represents Lua values as done by Lua 5.3 - i.e. in a 16 byte structure. 
* Ravi compiler generates type specifc opcodes which result in simpler and higher performance LLVM IR.

Limitations of JIT compilation
------------------------------
* Coroutines are not supported - JITed functions cannot yield
* The Debug API relies upon a field called ``savedpc`` which tracks the current instruction being executed by Lua interpreter. As this is not updated by the JIT code the Debug API can only provide a subset of normal functionality. The Debug API is not yet fully tested.
* The Lua VM supports infinite tail recursion. The JIT compiler treats OP_TAILCALL as normal OP_CALL so that recursion is limited to about 110 levels.
* The Lua C API has not yet been tested against the Ravi extensions - especially static typing and array types. Do not use the C API for now - as you could break the type system of Ravi.
* Bit-wise operators are JIT compiled only when the variables are known to be integers (specialized byte codes are used).

JIT Status of Lua/Ravi Bytecodes
---------------------------------
The JIT compilation status of the Lua and Ravi bytecodes are given below.

This information was last updated on 25th July 2015. As new bytecodes are being added to the JIT compiler on a regular basis
the status information below may be slightly out of date.

Note that if a Lua functions contains a bytecode that cannot be be JITed then the function cannot be JITed.

+-------------------------+----------+--------------------------------------------------+
| name                    | JITed?   | description                                      |
+=========================+==========+==================================================+
| OP_MOVE                 | YES      | R(A) := R(B)                                     |
+-------------------------+----------+--------------------------------------------------+
| OP_LOADK                | YES      | R(A) := Kst(Bx)                                  |
+-------------------------+----------+--------------------------------------------------+
| OP_LOADKX               | YES      | R(A) := Kst(extra arg)                           |
+-------------------------+----------+--------------------------------------------------+
| OP_LOADBOOL             | YES      | R(A) := (Bool)B; if (C) pc++                     |
+-------------------------+----------+--------------------------------------------------+
| OP_LOADNIL              | YES (1)  | R(A), R(A+1), ..., R(A+B) := nil                 |
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
| OP_NEWTABLE             | YES (1)  | R(A) := {} (size = B,C)                          |
+-------------------------+----------+--------------------------------------------------+
| OP_SELF                 | YES (1)  | R(A+1) := R(B); R(A) := R(B)[RK(C)]              |
+-------------------------+----------+--------------------------------------------------+
| OP_ADD                  | YES      | R(A) := RK(B) + RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_SUB                  | YES      | R(A) := RK(B) - RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_MUL                  | YES      | R(A) := RK(B) * RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_MOD                  | YES      | R(A) := RK(B) % RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_POW                  | YES      | R(A) := RK(B) ^ RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_DIV                  | YES      | R(A) := RK(B) / RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_IDIV                 | YES      | R(A) := RK(B) // RK(C)                           |
+-------------------------+----------+--------------------------------------------------+
| OP_BAND                 | YES (1)  | R(A) := RK(B) & RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_BOR                  | YES (1)  | R(A) := RK(B) | RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_BXOR                 | YES (1)  | R(A) := RK(B) ~ RK(C)                            |
+-------------------------+----------+--------------------------------------------------+
| OP_SHL                  | YES (1)  | R(A) := RK(B) << RK(C)                           |
+-------------------------+----------+--------------------------------------------------+
| OP_SHR                  | YES (1)  | R(A) := RK(B) >> RK(C)                           |
+-------------------------+----------+--------------------------------------------------+
| OP_UNM                  | YES      | R(A) := -R(B)                                    |
+-------------------------+----------+--------------------------------------------------+
| OP_BNOT                 | YES (1)  | R(A) := ~R(B)                                    |
+-------------------------+----------+--------------------------------------------------+
| OP_NOT                  | YES      | R(A) := not R(B)                                 |
+-------------------------+----------+--------------------------------------------------+
| OP_LEN                  | YES (1)  | R(A) := length of R(B)                           |
+-------------------------+----------+--------------------------------------------------+
| OP_CONCAT               | YES (1)  | R(A) := R(B).. ... ..R(C)                        |
+-------------------------+----------+--------------------------------------------------+
| OP_JMP                  | YES      | c+=sBx; if (A) close all upvalues >= R(A - 1)    |
+-------------------------+----------+--------------------------------------------------+
| OP_EQ                   | YES (1)  | if ((RK(B) == RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_LT                   | YES (1)  | if ((RK(B) <  RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_LE                   | YES (1)  | if ((RK(B) <= RK(C)) ~= A) then pc++             |
+-------------------------+----------+--------------------------------------------------+
| OP_TEST                 | YES      | if not (R(A) <=> C) then pc++                    |
+-------------------------+----------+--------------------------------------------------+
| OP_TESTSET              | YES      | if (R(B) <=> C) then R(A) := R(B) else pc++      |
+-------------------------+----------+--------------------------------------------------+
| OP_CALL                 | YES      | R(A), .. ,R(A+C-2) := R(A)(R(A+1), .. ,R(A+B-1)) |
+-------------------------+----------+--------------------------------------------------+
| OP_TAILCALL             | YES (2)  | return R(A)(R(A+1), ... ,R(A+B-1))               |
|                         |          | Compiled as OP_CALL so no tail call optimization |
+-------------------------+----------+--------------------------------------------------+
| OP_RETURN               | YES      | return R(A), ... ,R(A+B-2) (see note)            |
+-------------------------+----------+--------------------------------------------------+
| OP_FORLOOP              | YES      | R(A)+=R(A+2);                                    |
|                         |          | if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) } |
+-------------------------+----------+--------------------------------------------------+
| OP_FORPREP              | YES      | R(A)-=R(A+2); pc+=sBx                            |
+-------------------------+----------+--------------------------------------------------+
| OP_TFORCALL             | YES      | R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2));   |
+-------------------------+----------+--------------------------------------------------+
| OP_TFORLOOP             | YES      | if R(A+1) ~= nil then { R(A)=R(A+1); pc += sBx } |
+-------------------------+----------+--------------------------------------------------+
| OP_SETLIST              | YES (1)  | R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B	        |
+-------------------------+----------+--------------------------------------------------+
| OP_CLOSURE              | YES (1)  | R(A) := closure(KPROTO[Bx])                      |
+-------------------------+----------+--------------------------------------------------+
| OP_VARARG               | YES (1)  | R(A), R(A+1), ..., R(A+B-2) = vararg             |
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
| OP_RAVI_SETUPVALI       | YES (1)  | UpValue[B] := tointeger(R(A))                    |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETUPVALF       | YES (1)  | UpValue[B] := tonumber(R(A))                     |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETUPVALAI      | YES (1)  | UpValue[B] := toarrayint(R(A))                   |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETUPVALAF      | YES (1)  | UpValue[B] := toarrayflt(R(A))                   |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETTABLE_AII    | YES      | R(A)[RK(B)] := RK(C) where RK(B) is an integer   |
|                         |          | R(A) is array of integers, and RK(C) is an int   |
|                         |          | No conversion as input is known to be int        |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETTABLE_AFF    | YES      | R(A)[RK(B)] := RK(C) where RK(B) is an integer   |
|                         |          | R(A) is array of numbers, and RK(C) is a number  |
|                         |          | No conversion as input is known to be float      |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_BAND_II         | YES      | R(A) := RK(B) & RK(C), operands are int          |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_BOR_II          | YES      | R(A) := RK(B) | RK(C), operands are int          |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_BXOR_II         | YES      | R(A) := RK(B) ~ RK(C), operands are int          |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SHL_II          | YES (5)  | R(A) := RK(B) << RK(C), operands are int         |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SHR_II          | YES (5)  | R(A) := RK(B) >> RK(C), operands are int         |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_BNOT_I          | YES      | R(A) := ~R(B), int operand                       |
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
| OP_RAVI_GETTABLE_I      | YES      | R(A) := R(B)[RK(C)], integer key	                |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_GETTABLE_S      | YES      | R(A) := R(B)[RK(C)], string key                  |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_GETFIELD        | YES      | R(A) := R(B)[RK(C)], string key                  |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETTABLE_I      | YES (4)  | R(A)[RK(B)] := RK(C), integer key                |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETTABLE_S      | YES (3)  | R(A)[RK(B)] := RK(C), string key                 |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETFIELD        | YES      | R(A)[RK(B)] := RK(C), string key                 |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_TOTAB           | YES      | R(A) := to_table(R(A))                           |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_MOVETAB         | YES      | R(A) := R(B), check R(B) is a table              |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SETUPVALT       | YES (1)  | UpValue[B] := to_table(R(A))                     |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SELF_SK         | YES      | R(A+1) := R(B); R(A) := R(B)[RK(C)]              |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_SELF_S          | YES      | R(A+1) := R(B); R(A) := R(B)[RK(C)]              |
+-------------------------+----------+--------------------------------------------------+
| OP_RAVI_GETTABUP_SK     | YES      | R(A) := UpValue[B][RK(C)]                        |
+-------------------------+----------+--------------------------------------------------+

1. These bytecoes are handled via function calls rather than inline code generation
2. Tail calls are the same as ordinary calls.
3. The _SK variant is generated
4. Generates generic SETTABLE 
5. Inline code is generated only when operand is a constant integer

Ravi's LLVM JIT compiler source
-------------------------------
The LLVM JIT implementation is in following sources:

* ravillvm.h - includes LLVM headers and defines the generic JIT State and Function interfaces
* ravijit.h - defines the JIT API
* ravi_llvmcodegen.h - defines the types used by the code generator
* ravijit.cpp - Non implementation specific JIT API functions

* ravi_llvmjit.cpp - basic LLVM infrastructure and Ravi API definition
* ravi_llvmtypes.cpp - contains LLVM type definitions for Lua objects 
* ravi_llvmcodegen.cpp - LLVM JIT compiler - main driver for compiling Lua bytecodes into LLVM IR
* ravi_llvmload.cpp - implements OP_LOADK and OP_MOVE, and related operations, also OP_LOADBOOL
* ravi_llvmcomp.cpp - implements OP_EQ, OP_LT, OP_LE, OP_TEST and OP_TESTSET.
* ravi_llvmreturn.cpp - implements OP_RETURN
* ravi_llvmforprep.cpp - implements OP_FORPREP
* ravi_llvmforloop.cpp - implements OP_FORLOOP
* ravi_llvmtforcall.cpp - implements OP_TFORCALL and OP_TFORLOOP
* ravi_llvmarith1.cpp - implements various type specialized arithmetic operations - these are Ravi extensions
* ravi_llvmarith2.cpp - implements Lua opcodes such as OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_POW, OP_IDIV, OP_MOD, OP_UNM
* ravi_llvmcall.cpp - implements OP_CALL, OP_JMP
* ravi_llvmtable.cpp - implements OP_GETTABLE, OP_SETTABLE and various other table operations, OP_SELF, and also upvalue operations
* ravi_llvmrest.cpp - OP_CLOSURE, OP_VARARG, OP_CONCAT


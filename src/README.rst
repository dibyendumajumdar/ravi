This folder has all the Lua and Ravi source files.

The Lua sources begin with the letter ``l``.
The following Lua sources have been enhanced for Ravi.

* lvm.c - this has the Ravi bytecode implementations
* lopcodes.c - definitions of Ravi opcodes
* ldo.c - changes to enable calling JITed function
* lparser.c - parser changes for optional typing
* lcode.c - code generator changes for optional typing
* lfunc.c - changes to support JIT compilation

The LLVM JIT implementation is in following sources:

* ravijit.cpp - basic LLVM infrastructure and Ravi API definition
* ravi_llvmtypes.cpp - contains LLVM type definitions for Lua objects 
* ravi_llvmcodegen.cpp - LLVM JIT compiler - main driver for compiling Lua bytecodes into LLVM IR, also contains implementations of opcodes like OP_JMP
* ravi_llvmload.cpp - implements OP_LOADK and OP_MOVE
* ravi_llvmcomp.cpp - implements OP_EQ, OP_LT and OP_LE.
* ravi_llvmreturn.cpp - implements OP_RETURN
* ravi_llvmforprep.cpp - implements OP_FORPREP
* ravi_llvmforloop.cpp - implements OP_FORLOOP
* ravi_llvmarith1.cpp - implements various arithmetic operations
* ravi_llvmcall.cpp - implements OP_CALL
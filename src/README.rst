This folder has all the Lua and Ravi source files.

The Lua sources begin with the letter ``l``.
The following Lua sources have been enhanced for Ravi.

* lvm.c - this has the Ravi bytecode implementations
* lopcodes.c - definitions of Ravi opcodes
* ldo.c - changes to enable calling JITed function
* lparser.c - parser changes for optional typing
* lcode.c - code generator changes for optional typing
* lfunc.c - changes to support JIT compilation
* linit.c - the Ravi api functions get loaded here
* ltable.c - enhancements for Ravi array types
* lapi.c - enhancements for Ravi array types

The LLVM JIT implementation is in following sources:

* ravijit.cpp - basic LLVM infrastructure and Ravi API definition
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

Apart from LLVM backend, we also have MIR and OMR JIT backends.
These backends use C as the intermediate language. The common C code generator is in:

* ravi_jitshared.c - this is the C code generator for a given Ravi / Lua function

The MIR JIT implementation is in:

* ravi_mirjit.c - defines the driver functions. The MIR backend has its own C preprocessor, parser and code generator. 

The OMR JIT driver is in:

* ravi_omrjit.c - defines the driver functions. The OMR JIT backend uses the dmrC project as the C preprocessor, parser and code generator.

The new Ravi Parser and Code Generator implementation is in:

* ravi_ast_parse.c - contains the parser that builds AST
* ravi_ast_print.c - contains utilities for printing out the AST
* ravi_ast_typecheck.c - contains the type checking phase of the parser

Ravi also uses Doug Lea's malloc implementation. The implementation is in:

* ravi_alloc.c - Doug Lea's malloc implementation, adapted for Ravi.

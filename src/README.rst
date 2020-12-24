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

Ravi uses Doug Lea's malloc implementation. The implementation is in:

* ravi_alloc.c - Doug Lea's malloc implementation, adapted for Ravi.

## JIT Implementations

* ravi_jit.c - Ravi JIT API definition

Some backends use C as the intermediate language. The common C code generator is in:

* ravi_jitshared.c - this is the C code generator for a given Ravi / Lua function

The MIR JIT implementation is in:

* ravi_mirjit.c - defines the driver functions. The MIR backend has its own C preprocessor, parser and code generator.


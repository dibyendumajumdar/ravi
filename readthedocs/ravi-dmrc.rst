====================================
dmr_C embedded C parser and compiler
====================================

Ravi includes dmr_C, an embedded C parser and compiler. The C compiler supports LLVM and Eclipse OMR backends.
The following api is under development, but is not yet fully functional, and is subject to change.

``drmc.getsymbols(source)``
    Parses the input and returns a table of symbols found. 
``dmrc.compileC(source)``
    Compiles the input source.

Examples
========
An example use of the C parser is `ravi-tests/dmrc_getsymbols.lua <https://github.com/dibyendumajumdar/ravi/blob/master/ravi-tests/dmrc_getsymbols.lua>`_.
For an example of invoking the C compiler with LLVM backend see `ravi-tests/dmrc_testllvm.lua <https://github.com/dibyendumajumdar/ravi/blob/master/ravi-tests/dmrc_testllvm.lua>`_.

Outstanding issues
==================
* The Eclipse OMR backend cannot automatically access C functions externally defined; these have to be pre-registered. A solution might be to expose the resolution of symbols from dynamic libraries.
* We need to validate that the compiled C function is callable from Lua. This is not as easy to do with Eclipse OMR backend as with the LLVM backend.

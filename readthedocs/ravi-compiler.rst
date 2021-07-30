New Compiler Framework in Ravi
==============================

A new compiler framework in Ravi is now available as a preview feature.

The new framework allows both JIT and AOT compilation of Lua/Ravi code.
A large subset of the language is supported.

The new compiler differs from the standard Lua/Ravi compiler in following ways:

* Unlike the standard compiler, the new compiler generates Abstract Syntax Trees (ASTs) in the parsing phase.
* A new linear Intermediate Representation (IR) is produced from the AST.
* Finally the IR is translated to C code, which can be JITed using the MIR backend or compiled ahead-of-time using a standard C compiler such as gcc, clang or MSVC.

The new compiler can be invoked in the following ways.

``Compiler.load(code)``

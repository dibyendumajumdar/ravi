CLANG output
============

To help with LLVM IR code generation I am using clang's ability to dump the IR code. I construct small C programs to represent certain Lua op codes and the use clang to generate the IR. This then forms the basis for the code generation in Ravi.

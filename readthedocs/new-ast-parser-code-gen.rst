New Parser and Code Generator
=============================

This is to capture design / implementation details for the new parser and code generator.
Changes are in ``ast`` branch.

What is Where?
--------------

Quick reminders:

1. lapi.c has new function ravi_load()
2. lauxlib.c has new functions raviL_loadbufferx() and raviL_dumpast()
3. ldo.c has new functions ravi_f_parser() and raviD_protectedparser()
4. Implementation of raviY_parser() is in ravi_ast.c
5. There is a new Lua api raviload() that calls ravi_load() - this can be used to start developing/testing the new parser.
6. There is a also a test driver test_ast.c.

The new parser does nothing yet - it simply ends up calling the standard Lua parser. The idea is that we will have a passthrough
that is only used to test/build the new module without actually interfering with the existing flow. 
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

Its been a while since I started this ... so cannot recollect now what the entry point was. I think the aim was to 
allow the new parser to be called by using a new API function. The new parser is a pass through - i.e. it throws away everything
and eventually calls the existing parser so this way I can experiment with it without actually needing to generate code
just yet.

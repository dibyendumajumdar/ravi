Ravi Debug Adapter for VSCode
=============================

The aim is to provide a debug adapter that allows Microsoft's Visual Studio Code to step through Ravi or 
Lua code. 

Implementation Notes
--------------------
The approach is to create a standalone executable that can be invoked by VSCode. VSCode communicates 
with the adapter via stdin/stdout. This means that Lua cannot use stdin/stdout - but for now nothing is done
to prevent Lua access to these. 

VSCode communicates with the adapter using a JSON wire protocol.


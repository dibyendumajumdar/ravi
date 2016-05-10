Ravi Debug Adapter for VSCode
=============================

The aim is to provide a debug adapter that allows Microsoft's Visual Studio Code to step through Ravi or 
Lua code. 

Implementation Notes
--------------------
The approach is to create a standalone executable that can be invoked by VSCode. VSCode communicates 
with the adapter via stdin/stdout. This means that Lua cannot use stdin/stdout therefore Lua output 
to stdout/stderr is captured and sent to the debugger front-end.

VSCode communicates with the adapter using a JSON wire protocol. For a sample session look at
`Example JSON Protocol Messages <https://github.com/dibyendumajumdar/ravi/blob/master/vscode-debugger/docs/example-protocol-messages.txt>`_.

Status
------
This is work in progress. The basic debugger is working with following features and limitations.

* Launch a Ravi/Lua program and stop on entry
* Step through code (stepin, stepout, next all behave as stepin)
* Continue works, but pause doesn't. Note that the execution is very slow under the debugger.
* Set breakpoints at line/source level
* Only local variables are shown in the Variables window right now; number of variables displayed is limited to 120.
* Tables are expanded to one level only - expansion limited to 120 elements
* Lua stdout and stderr are redirected to the debugger
* The debugger can step into dynamically generated Lua code
* No recognition of Ravi specific type information yet
* Has been tested briefly on Windows 10 and OSX so far
* Various hard coded limits - e.g. number of breakpoints limited to 20
* Expect lots of bugs!

Note: This is very early days and the debugger not yet ready for real use so try at your own risk!

Installation
------------
Currently a Windows 10 64-version is available from the Visual Studio Code marketplace - just search for extension 'Ravi Debug'.

Screenshots
-----------

.. figure:: ../readthedocs/debugger-screenshot1.jpg
   :alt: Ravi Debugger screenshot


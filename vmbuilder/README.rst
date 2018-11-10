This is a new initiative to build a VM for Ravi/Lua 5.3 using techniques similar to LuaJIT.

Note: This work is temporarily on hold due to changes in bytecode encoding.

Goals
=====
* Create a new Lua/Ravi interpreter that is coded mostly in assembly
* Initial work will focus on X86-64 architecture only
* The VM will support the extended bytecode set of Ravi
* An equally important goal is to document the effort so that it is easier for others to understand how the VM is implemented

Design Notes
============
* `Notes on LuaJIT <https://github.com/dibyendumajumdar/ravi/blob/master/vmbuilder/docs/luajit_buildvm.rst>`_
* `VM Design and Implementation Notes <https://github.com/dibyendumajumdar/ravi/blob/master/vmbuilder/docs/vm-design.rst>`_

Timescales
==========
This project will proceed slowly as this is my first foray into assembly language programming. Also I can only spend time on this project in my free time (i.e. weekends and holidays). 

Acknowledgements
================
I plan to reuse parts of LuaJIT / dynasm for this project.


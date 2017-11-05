Notes on LuaJIT 2.1 Interpreter VM
==================================

These notes are only for X86-64 architecture.

* The VM code is generated using a number of steps:

	1. First the `vm_x86.dasc <https://github.com/LuaJIT/LuaJIT/blob/master/src/vm_x86.dasc>`_ file is processed by dynasm to create the ``buildvm_arch.h`` file. 
	2. Next the generated ``buildvm_arch.h`` is combined with the ``buildvm`` files to create a custom VM code generator.
	3. The generated ``buildvm`` tool is then run to produce the VM code. This is actually an object file in the case of Windows, but on UNIX machines the output is a assembly code file. However this file does not contain human readable assembly code - instead the code is output as a sequence of binary values. An example of the output file is available `at the RaptorJIT project <https://github.com/raptorjit/raptorjit/blob/master/src/reusevm/lj_vm.S>`_.

* The generated VM code contains an assembler routine for each LuaJIT bytecode, and also a number of other library and utility functions.
* The offsets of the assembler routines for the bytecode instructions are gathered and used to create the computed goto ``DISPATCH`` table. This table is stored alongside `the LuaJIT global_State/lua_State structure  in GG_State <https://github.com/LuaJIT/LuaJIT/blob/master/src/lj_dispatch.h>`_. At runtime this dispatch table is accessed via a register.
* Each generated assembly function for bytecodes does a fetch of the next bytecode instruction and then jumps to the implementing assembly function using the ``DISPATCH`` table. 
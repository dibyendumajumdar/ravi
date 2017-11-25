Design and Implementation Notes
===============================
The overall approach is:

* For each bytecode create an assembler routine
* Create a dispatch table with pointers to the assembler routines; the dispatch table is stored in the Lua global_State structure
  but this could change in future
* Each assembler routine will (after completing its action) fetch the next bytecode instruction and jump to the next 
  assembler routine using the dispatch table
* The assembler routines are not full fledged C functions - they are part of one whole program. Hence they make assumptions about
  register usage which will be documented below. The VM as a whole will have a fixed set of register allocations so that most 
  important information is held in registers. 
  
Implementation Considerations
-----------------------------
* The dispatch table is stored in global_State - it is not clear yet whether it is worth making a local stack copy of it when the
  VM starts executing.
  
Why dynasm
----------
The implementation requires following key abilities that dynasm has:

* Obtain the offets of the bytecode labels so that these can be gathered into a dispatch table.
* Use C code to calculate various structure offsets etc.
* Macros to create aliases for registers, and generate common sequences.

I am not sure whether this combination of features is available in other approaches such as using inline assembler in C code. I have briefly looked at:

* Inline assembly in gcc / clang - too ugly syntactically to work with
* Inline assembly in D - almost okay - no macros however so simple stuff like creating aliases for registers requires ugly mixin templates. Also the generated code appears to lack enough controls (such as disabling the frame pointer register, and ensuring correct unwind data on Win64).

Using an assembler like yasm has the problem of computing offsets of C structures.

Issue with dynasm
-----------------
On Windows 64-bit the generated code requires UNWIND information however the mechanism for this is in LuaJIT specific files (buildbm_peobj) and not fully reusable. I have modified this to decouple from LuaJIT. This took some effort because LuaJIT's code
has numerous magic numbers with no explanation of what the code is doing. Not very helpful for anyone trying to work out what
the code is doing unless you already know what needs doing.

Register Allocations
--------------------
The VM will use a fixed set of registers mostly with some register usage varying across routines. The following table shows the
planned usage. 

Nomenclature

* cs - callee saved, if we call a C function then after it returns we can rely on these registers
* v - volatile, these registers may be overridden by a called function so do not rely on them after function call
* `(n)` - used to pass arg n to function

+--------------------+------------------+------------------------------+------------------------------------------+
| Windows X64 reg    | Linux X64 reg    | Assignment                   | Notes                                    |
+====================+==================+==============================+==========================================+
| rbx (cs)           | rbx (cs)         | PC                           | Pointer to next bytecode                 |
+--------------------+------------------+------------------------------+------------------------------------------+
| rbp (cs)           | rbp (cs)         | L                            | Pointer to lua_State                     |
+--------------------+------------------+------------------------------+------------------------------------------+
| rdi (cs)           | rdi (v) (1)      | TMP1                         | Scratch                                  |
+--------------------+------------------+------------------------------+------------------------------------------+
| rsi (cs)           | rsi (v) (2)      | TMP2                         | Scratch                                  |
+--------------------+------------------+------------------------------+------------------------------------------+
| rsp (cs)           | rsp              |                              | Stack pointer                            |
+--------------------+------------------+------------------------------+------------------------------------------+
| r12 (cs)           | r12 (cs)         | CI                           | CallInfo (Lua frame)                     |
+--------------------+------------------+------------------------------+------------------------------------------+
| r13 (cs)           | r13 (cs)         | LCL                          | Current function's LClosure              |
+--------------------+------------------+------------------------------+------------------------------------------+
| r14 (cs)           | r14 (cs)         | DISPATCH                     | Ptr to Dispatch table                    |
+--------------------+------------------+------------------------------+------------------------------------------+
| r15 (cs)           | r15 (cs)         | KBASE                        | Ptr to constants table in Proto          |
+--------------------+------------------+------------------------------+------------------------------------------+
| rax (v)            | rax (v)          | RCa                          | Scratch - upon entry to subroutine eax   |
|                    |                  |                              | will have the B,C portion of bytecode    |
+--------------------+------------------+------------------------------+------------------------------------------+
| rcx (v) (1)        | rcx (v) (4)      | RAa                          | Scratch - upon entry to subroutine ecx   |
|                    |                  |                              | will have the value of A in bytecode     |
+--------------------+------------------+------------------------------+------------------------------------------+
| rdx (v) (2)        | rdx (v) (3)      | RBa                          | Scratch - upon entry to subroutine edx   |
|                    |                  |                              | will have the OpCode                     |
+--------------------+------------------+------------------------------+------------------------------------------+
| r8 (v) (3)         | r8 (v) (5)       | BASE                         | Pointer to Lua stack base                |
+--------------------+------------------+------------------------------+------------------------------------------+
| r9 (v) (4)         | r9 (v) (6)       | TMP3                         | Scratch                                  |
+--------------------+------------------+------------------------------+------------------------------------------+

Stack space 
-----------
On Win64 every function gets a 32-byte shadow space for the 4 register arguments, which we can use. But we also need
to provide a shadow space for function calls inside the VM. Basically these 4 stack positions cancel out if we make use 
of the slots provided by the caller.

VMBuilder
---------
The `VMBuilder <https://github.com/dibyendumajumdar/ravi/tree/master/vmbuilder/src>`_ is a tool that generates the code for the VM. It is a sub-project and the only component that needs to use dynasm. 

To build the tool, you have to use CMake. Following steps are for Windows::

  cd vmbuilder\src
  mkdir build
  cd build
  cmake -G "Visual Studio 15 Win64" ..
  
Above will generate the Visual Studio project that you can open in VS2017. Do a BUILD, followed by INSTALL. The resulting buildvm.exe 
will be installed under ``vmbuilber/bin``.

On Linux, the process is as follows::

  cd vmbuilder/src
  mkdir build
  cd build
  cmake ..
  make
  make install
  
Running VMBuilder on Windows
----------------------------
VMBuilder tool is run as follows on Windows::

  cd vmbuilder\bin
  buildvm -m peobj -o vm.obj
  buildvm -m bcdef > ravi_bcdef.h

If you have LLVM installed you can obtain the assembly source as follows::

  llvm-objdump -unwind-info -d -t vm.obj > vm.asm

Running VMBuilder on UNIX systems
---------------------------------
On Linux, the steps for unning ``buildvm`` are::

  cd vmbuilder/bin
  ./buildvm -m elfasm -o vm.s
  ./buildvm -m bcdef > ravi_bcdef.h
  
Note that you should not have to run VMBuilder manually this way as the Ravi CMake build will do these steps anyway. 

Files generated by VMBuilder
----------------------------
On Windows VMBuilder generates an object file. On Linux and Mac OSX it generates assembly source file.
The generated file is then linked with the main Ravi library.

Additionally a header file is generated named 'ravi_bcdef.h' - containing an array of offsets that can be used to obtain the
address of each assembly routine by adding the offset to the 'ravi_vm_asm_begin' symbol.

Current Issues
--------------
* Some additional work may be necessary to link the ASM routines when shared library builds are on - at least on Windows where
  I noticed that the ASM functions were not properly being invoked. Have switch to static builds for now.

Setup of dispatch table
-----------------------
Currently this occurs in `lstate.c <https://github.com/dibyendumajumdar/ravi/blob/master/src/lstate.c>`_ in function ``dispatch_init()`` which is shown below::

  #ifdef RAVI_USE_ASMVM
  /* Initialize dispatch table used by the ASM VM */
  static void dispatch_init(global_State *G) {
    ASMFunction *disp = G->dispatch;
    for (uint32_t i = 0; i < NUM_OPCODES; i++) {
      /*
      Following computes an offset for the assembly routine for the given OpCode.
      The offset is relative to the global symbol ravi_vm_asm_begin that is
      generated as part of the VMBuilder code generation. All the bytecode
      routines are at some offset to this global symbol.
      */
      /* NOTE: enabling ltests.h modifies the global_State and breaks the assumptions abou the location of the dispatch table */
      disp[i] = makeasmfunc(ravi_bytecode_offsets[i]);
    }
  }
  #endif
 
Progress
--------
The only op codes implemented so far are:

* OP_RETURN

Here is a `link to the generated assembly code on Windows X64 <https://github.com/dibyendumajumdar/ravi/blob/master/vmbuilder/asm/vm-win64.asm>`_.

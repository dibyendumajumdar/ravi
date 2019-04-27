Design and Implementation Notes
===============================
The overall approach is:

* For each bytecode create an assembler routine
* Create a dispatch table with pointers to the assembler routines; the dispatch table is stored in the Lua global_State structure
  but this could change in future
* Each assembler routine will (after completing its action) fetch the next bytecode instruction and jump to the next 
  assembler routine using the dispatch table
* The assembler routines are C functions - they are part of one whole program. Hence they make assumptions about
  register usage which will be documented below. The VM as a whole will have a fixed set of register allocations so that most 
  important information is held in registers. 
  
Implementation Considerations
-----------------------------
* The dispatch table is stored in global_State - it is not clear yet whether it is worth making a local stack copy of it when the
  VM starts executing.
  
Why dynasm
----------
The implementation requires following key abilities that dynasm has:

* Obtain the offsets of the bytecode labels so that these can be gathered into a dispatch table.
* Use C code to calculate various structure offsets etc.
* Macros to create aliases for registers, and generate common sequences.

I am not sure whether this combination of features is available in other approaches such as using inline assembler in C code. I have briefly looked at:

* Inline assembly in gcc / clang - too ugly syntactically to work with
* Inline assembly in D - almost okay - no macros however so simple stuff like creating aliases for registers requires ugly mixin templates. The generated code appears to lack enough controls (such as disabling the frame pointer register, and ensuring correct unwind data on Win64). Also do not know how to get the offsets from generated code.

Using an assembler like yasm has the problem of computing offsets of C structures.

Issues with dynasm
------------------
On Windows 64-bit the generated code requires UNWIND information however the mechanism for this was in LuaJIT specific files (buildvm_peobj) and not fully reusable. I have modified this to decouple from LuaJIT. This took some effort because LuaJIT's code
has numerous magic numbers with no explanation of what the code is doing. Not very helpful for anyone trying to work out what
the code is doing unless you already know what needs doing.

I wish dynasm could generate annotated assembly source file that is human readable. This would allow debuggers to display the
source code and would make debugging the assembly instructions much easier.

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
| rdi (cs)           | rdi (v) (1)      |                              |                                          |
+--------------------+------------------+------------------------------+------------------------------------------+
| rsi (cs)           | rsi (v) (2)      |                              |                                          |
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
| rax (v)            | rax (v)          | RCa = rax, RC = eax          | Scratch - also eax used for              |
|                    |                  |                              | the B,C portion of bytecode              |
+--------------------+------------------+------------------------------+------------------------------------------+
| rcx (v) (1)        | rcx (v) (4)      | RAa = rcx, RA = ecx          | Scratch - also ecx used for              |
|                    |                  |                              | the value of A in bytecode               |
+--------------------+------------------+------------------------------+------------------------------------------+
| rdx (v) (2)        | rdx (v) (3)      | RBa = rdx, OP = edx          | Scratch - also edx used for              |
|                    |                  |                              | the OpCode                               |
+--------------------+------------------+------------------------------+------------------------------------------+
| r8 (v) (3)         | r8 (v) (5)       | BASE                         | Pointer to Lua stack base                |
+--------------------+------------------+------------------------------+------------------------------------------+
| r9 (v) (4)         | r9 (v) (6)       | TMP3                         | Scratch                                  |
+--------------------+------------------+------------------------------+------------------------------------------+
| r10 (v)            | r10 (v           | TMP1                         | Scratch                                  |
+--------------------+------------------+------------------------------+------------------------------------------+
| r11 (v)            | r11 (v)          | TMP2                         | Scratch                                  |
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
  I noticed that the ASM functions were not properly being invoked. Have switched to static builds for now.

Exported Symbols
----------------
The main public symbols that are accessed externally are:

* ravi_vm_asm_begin - this is the start of the generated code, and all the assembly routines are at offets relative to the address of this symbol.
* ravi_luaV_interp - this is the entry VM point, equivalent to luaV_execute().

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
      /* NOTE: enabling ltests.h modifies the global_State and breaks the assumptions about
         the location of the dispatch table */
      disp[i] = makeasmfunc(ravi_bytecode_offsets[i]);
    }
  }
  #endif
 
OpCode structure
----------------
The OpCode is stored in LuaJIT inspired format when the ASM VM is enabled. This is showns below::

  +---+---+---+----+
  | B | C | A | Op |
  +---+---+---+----+
  | Bx    | A | Op |
  +-------+---+----+
  | Ax        | Op |
  +-----------+----+

Note that the bytecode decoding assumes above and will break if Lua style encoding is used. 

Progress
--------
The only op codes implemented so far are:

* OP_RETURN (some scenarios to be tested, e.g. upvalues)
* OP_LOADK 
* OP_MOVE
* OP_RAVI_FORPREP_IP and OP_RAVI_FORPREP_I1
* OP_RAVI_FORLOOP_I1 and OP_RAVI_FORLOOP_I1

Here is a `link to the generated assembly code on Windows X64 <https://github.com/dibyendumajumdar/ravi/blob/master/vmbuilder/asm/vm-win64.asm>`_.

The equivalent `Linux assembly code is <https://github.com/dibyendumajumdar/ravi/blob/master/vmbuilder/asm/vm-linux64.s>`_.

It seems hard to test and validate the assembly code. I have to step through the code instruction by instruction.

On Windows I debug in Visual Studio 2017. 

On Linux I use Eclipse CDT to debug the code. I use the instruction step mode.

On Mac OSX I use Xcode. 

I think it might be useful to create a test harness that mocks the Lua structures
and functions so that each bytecode instruction can be tested in isolation. This will need some work however, so not yet sure.
The other alternative is to check by running Lua scripts which is how we test Ravi normally.

Windows X64 Specifics
---------------------
On Windows the VMBuilder tool generates object code rather than an assembly source file. Win64 also requires some special data 
for stack unwinding in case of exceptions. I believe even longjmps trigger this functionality. 

The way we handle this now is by generating following in the object file::

  Unwind info:

  Function Table:
    Start Address: ravi_vm_asm_begin
    End Address: ravi_vm_asm_begin + 0x06a2
    Unwind Info Address: .xdata
      Version: 1
      Flags: 0
      Size of prolog: 0
      Number of Codes: 9
      No frame pointer used
      Unwind Codes:
        0x00: UOP_AllocSmall 40
        0x00: UOP_PushNonVol R15
        0x00: UOP_PushNonVol R14
        0x00: UOP_PushNonVol R13
        0x00: UOP_PushNonVol R12
        0x00: UOP_PushNonVol RBX
        0x00: UOP_PushNonVol RSI
        0x00: UOP_PushNonVol RDI
        0x00: UOP_PushNonVol RBP
        
Basically above tells Windows what the function epilogue (stack) looks like so that Windows can correctly restore the registers when 
unwinding the stack. Note that the unwind information applies to the entire generated code and not a specific function. In particular
the assumption is that there any entry point in the code needs to have a prologue that is the exact inverse of the
epilogue described above.

Here is the function prologue::

  ravi_luaV_interp:
     651:	55 	pushq	%rbp
     652:	57 	pushq	%rdi
     653:	56 	pushq	%rsi
     654:	53 	pushq	%rbx
     655:	41 54 	pushq	%r12
     657:	41 55 	pushq	%r13
     659:	41 56 	pushq	%r14
     65b:	41 57 	pushq	%r15
     65d:	48 83 ec 28 	subq	$40, %rsp
     
And the epilogue::

  ravi_vm_return:
     691:	48 83 c4 28 	addq	$40, %rsp
     695:	41 5f 	popq	%r15
     697:	41 5e 	popq	%r14
     699:	41 5d 	popq	%r13
     69b:	41 5c 	popq	%r12
     69d:	5b 	popq	%rbx
     69e:	5e 	popq	%rsi
     69f:	5f 	popq	%rdi
     6a0:	5d 	popq	%rbp
     6a1:	c3 	retq
     
As you can see the unwind information basically tells Windows what the epilogue is supposed to be, and where to find the saved
values of the registers.

Building Ravi With New ASM VM
-----------------------------
This is only for the brave who want to hack with the code.

To enable the new VM first build and install VMBuilder as described above.
Then build Ravi using the cmake flags ``-DSTATIC_BUILD=ON`` and ``-DASM_VM=ON`` enabled. Don't enable JIT.

Right now the ASM VM is exercised via the ``test_vm`` sub project. The ASM VM is only invoked in special cases, i.e. a function has small number of instructions and only contains supported instructions, and additionally as OP_CALL is not yet implemented, you can only call the new VM via the Lua C api (see `test_asmvm() in test_vm.c <https://github.com/dibyendumajumdar/ravi/blob/master/tests/test_vm.c>`_).

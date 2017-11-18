Design and Implementation Notes
===============================
The overall approach is:

* For each bytecode create an assembler routine
* Create a dispatch table with pointers to the assembler routines
* Each assembler routine will (after completing its action) fetch the next bytecode instruction and jump to the next 
  assembler routine using the dispatch table
* The assembler routines are not full fledged C functions - they are part of one whole program. Hence they make assumptions about
  register usage which will be documented below. The VM as a whole will have a fixed set of register allocations so that most 
  important information is held in registers. 
  
Design Considerations
---------------------
* We will use 64-bit pointers and operations mostly - and not try to squeeze pointers into 32-bit registers as LuaJIT appears to do.
* The dispatch table will be stored in global_State - it is not clear yet whether it is worth making a local stack copy of it when the
  VM starts executing.
  
Why dynasm
----------
The implementation requires following key abilities that dynasm has:

* Obtain the offets of the bytecode labels so that these can be gathered into a dispatch table.
* Use C code to calculate various structure offets etc.
* Macros 

I am not sure whether this combination of features is available in other approaches such as using inline assembler in C code.

Issue with dynasm
-----------------
On Windows 64-bit the generated code requires UNWIND information however the mechanism for this is in LuaJIT specific files (buildbm_peobj) and not fully reusable. I have modified this to decouple from LuaJIT. This took some effort because LuaJIT's code
has numrous magic numbers and stuff with no explanation of what they are doing. Not very helpful for anyone trying to work out what
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
to provide a shadow space for function calls inside the VM. 

We can size the stack such that we pre-allocate the 32 byte shadow space on Win64, so that we don't need to adjust the 
stack every time we make a call to a C function. But instead of using this we use the shadow space provided by the caller.

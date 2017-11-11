Design and Implementation Notes
===============================
The overall approach is:

* For each bytecode create an assembler routine
* Create a dispatch table with pointers to the assembler routines
* Each assembler routine will fetch the next bytecode instruction and jump to the next assembler routine using the dispatch table
* The assembler routines are not full fledged C functions - they are part of one whole program. Hence they make assumptions about
  register usage which will be documented below. The VM as a whole will have a fixed set of register allocations so that most 
  important information is held in registers. 
  
Design Considerations
---------------------
* We will use 64-bit pointers and operations mostly - and not try to squeeze pointers into 32-bit regisers as LuaJIT appears to do.
* The dispatch table will be stored in global_State - it is not clear yet whether it is worth making a local stack copy of it when the
  VM starts executing.
  
Why dynasm
----------
The implementation requires following key abilities that dynasm has:

* Define dynamic labels using integer counter (e.g. opCode). I suppose this isn't critical as long as below is possible. 
* Obtain the offets of the labels so that these can be gathered into a dispatch table.
* Use C code to calculate various structure offets etc.
* Macros 

I am not sure whether this combination of features is available in other approaches such as using inline assembler in C code. 

Register Allocations
--------------------
The VM will use a fixed set of registers mostly with some register usage varying across routines. The following table shows the
planned usage. 

Nomenclature

* cs - callee saved, if we call a C function then after it returns we can rely on these registers
* v - volatile, these registers may be overridden by a called function so do not rely on them after function call
* (n) - used to pass arg n to function

+--------------------+------------------+------------------------------+------------------------------------------+
| Windows X64 reg    | Linux X64 reg    | Assignment                   | Notes                                    |
+====================+==================+==============================+==========================================+
| rbx (cs)           | rbx (cs)         | PC (ebx)                     | Lua byte code offset                     |
+--------------------+------------------+------------------------------+------------------------------------------+
| rbp (cs)           | rbp (cs)         | L                            |                                          |
+--------------------+------------------+------------------------------+------------------------------------------+
| rdi (cs)           | rdi (v) (1)      | TMP1                         | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| rsi (cs)           | rsi (v) (2)      | TMP2                         | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| rsp (cs)           | rsp              |                              | Stack pointer                            |
+--------------------+------------------+------------------------------+------------------------------------------+
| r12 (cs)           | r12 (cs)         | CI                           | CallInfo (Lua frame)                     |
+--------------------+------------------+------------------------------+------------------------------------------+
| r13 (cs)           | r13 (cs)         | LCL                          | LuaClosure                               |
+--------------------+------------------+------------------------------+------------------------------------------+
| r14 (cs)           | r14 (cs)         | DISPATCH                     | Ptr to Dispatch table                    |
+--------------------+------------------+------------------------------+------------------------------------------+
| r15 (cs)           | r15 (cs)         | KBASE                        | Ptr to constants table in Proto          |
+--------------------+------------------+------------------------------+------------------------------------------+
| rax (v)            | rax (v)          | RCa                          | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| rcx (v) (1)        | rcx (v) (4)      | RAa                          | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| rdx (v) (2)        | rdx (v) (3)      | RBa                          |                                          |
+--------------------+------------------+------------------------------+------------------------------------------+
| r8 (v) (3)         | r8 (v) (5)       | BASE                         | Lua stack base                           |
+--------------------+------------------+------------------------------+------------------------------------------+
| r9 (v) (4)         | r9 (v) (6)       | TMP3                         |                                          |
+--------------------+------------------+------------------------------+------------------------------------------+

Stack space 
-----------
On Win64 every function gets a 32-byte shadow space for the 4 register arguments, which we can use. But we also need
to provide a shadow space for function calls inside the VM. 

We can size the stack such that we pre-allocate the 32 byte shadow space on Win64, so that we don't need to adjust the 
stack every time we make a call to a C function. But instead of using this we use the shadow space provided by the caller.

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

* Define dynamic labels using integer counter (e.e. opCode). 
* Obtain the offets of the labels so that these can be gathered into a dispatch table.
* Use C code to calculate various structure offets etc. 

I am not sure whether this combination of features is available in other approaches such as using inline assembler in C code. 

Register Allocations
--------------------
The VM will use a fixed set of registers mostly with some register usage varying across routines. The following table shows the
planned usage.

+--------------------+------------------+------------------------------+------------------------------------------+
| Windows X64 reg    | Linux X64 reg    | Assignment                   | Notes                                    |
+====================+==================+==============================+==========================================+
| rbx                | z                | z                            | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| rbp                | z                | z                            | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| rdi                | z                | z                            | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| rsi                | z                | z                            | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| rsp                | z                | z                            | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| r12                | z                | z                            | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| r13                | z                | z                            | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| r14                | z                | z                            | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| r15                | z                | z                            | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+
| z                  | z                | z                            | z                                        |
+--------------------+------------------+------------------------------+------------------------------------------+


RBX, RBP, RDI, RSI, RSP, R12, R13, R14, and R15

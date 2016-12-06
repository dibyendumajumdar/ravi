==========================
Lua 5.3 Bytecode Reference
==========================

This is my attempt to bring up date the Lua bytecode reference.
Following copyrights are acknowledged:

:: 

  A No-Frills Introduction to Lua 5.1 VM Instructions
    by Kein-Hong Man, esq. <khman AT users.sf.net>
    Version 0.1, 2006-03-13

`A No-Frills Introduction to Lua 5.1 VM Instructions <http://luaforge.net/docman/83/98/ANoFrillsIntroToLua51VMInstructions.pdf>`_ is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike License 2.0. You are free to copy, distribute and display the work, and make derivative works as long as you give the original author credit, you do not use this work for commercial purposes, and if you alter, transform, or build upon this work, you distribute the resulting work only under a license identical to this one. See the following URLs for more information::

    http://creativecommons.org/licenses/by-nc-sa/2.0/
    http://creativecommons.org/licenses/by-nc-sa/2.0/legalcode


Lua Stack and Registers
=======================
Lua employs two stacks.
The ``Callinfo`` stack tracks activation frames. 
There is the secondary stack ``L->stack`` that is an array of ``TValue`` objects. 
The ``Callinfo`` objects index into this array. Registers are basically slots in 
the ``L->stack`` array.

When a function is called - the stack is setup as follows::

  stack
  |            function reference
  |            var arg 1
  |            ... 
  |            var arg n
  | base->     fixed arg 1
  |            ...
  |            fixed arg n
  |            local 1
  |            ...
  |            local n
  |            temporaries 
  |            ...
  |  top->     
  |  
  V

So ``top`` is just past the registers needed by the function. 
The number of registers is determined based on parameters, locals and temporaries.

For each Lua function, the ``base`` of the stack is set to the first fixed parameter or local.
All register addressing is done as offset from ``base`` - so ``R(0)`` is at ``base+0`` on the stack. 

.. figure:: Drawing_Lua_Stack.jpg
   :alt: Drawing of Lua Stack

   The figure above shows how the stack is related to other Lua objects.


Instruction Notation
====================

R(A)
  Register A (specified in instruction field A)
R(B)
  Register B (specified in instruction field B)
R(C)
  Register C (specified in instruction field C)
PC
  Program Counter
Kst(n)
  Element n in the constant list
Upvalue[n]
  Name of upvalue with index n
Gbl[sym]
  Global variable indexed by symbol sym
RK(B)
  Register B or a constant index
RK(C)
  Register C or a constant index
sBx
  Signed displacement (in field sBx) for all kinds of jumps

Instruction Summary
===================

Lua bytecode instructions are 32-bits in size. 
All instructions have an opcode in the first 6 bits.
Instructions can have the following fields::

  'A' : 8 bits
  'B' : 9 bits
  'C' : 9 bits
  'Ax' : 26 bits ('A', 'B', and 'C' together)
  'Bx' : 18 bits ('B' and 'C' together)
  'sBx' : signed Bx

A signed argument is represented in excess K; that is, the number
value is the unsigned value minus K. K is exactly the maximum value
for that argument (so that -max is represented by 0, and +max is
represented by 2*max), which is half the maximum for the corresponding  
unsigned argument.

Note that B and C operands need to have an extra bit compared to A.
This is because B and A can reference registers or constants, and the
extra bit is used to decide which one. But A always references registers
so it doesn't need the extra bit.

+------------+-------------------------------------------------------------+
| Opcode     | Description                                                 |
+============+=============================================================+
| MOVE       | Copy a value between registers                              |
+------------+-------------------------------------------------------------+
| LOADK      | Load a constant into a register                             |
+------------+-------------------------------------------------------------+
| LOADKX     | Load constant into register                                 |
+------------+-------------------------------------------------------------+
| LOADBOOL   | Load a boolean into a register                              |
+------------+-------------------------------------------------------------+
| LOADNIL    | Load nil values into a range of registers                   |
+------------+-------------------------------------------------------------+
| GETUPVAL   | Read an upvalue into a register                             |
+------------+-------------------------------------------------------------+
| GETTABUP   | Read a value from table in up-value into a register         |
+------------+-------------------------------------------------------------+
| GETTABLE   | Read a table element into a register                        |
+------------+-------------------------------------------------------------+
| SETTABUP   | Write a register value into table in up-value               |
+------------+-------------------------------------------------------------+
| SETUPVAL   | Write a register value into an upvalue                      |
+------------+-------------------------------------------------------------+
| SETTABLE   | Write a register value into a table element                 |
+------------+-------------------------------------------------------------+
| NEWTABLE   | Create a new table                                          |
+------------+-------------------------------------------------------------+
| SELF       | Prepare an object method for calling                        |
+------------+-------------------------------------------------------------+
| ADD        | Addition operator                                           |
+------------+-------------------------------------------------------------+
| SUB        | Subtraction operator                                        |
+------------+-------------------------------------------------------------+
| MUL        | Multiplication operator                                     |
+------------+-------------------------------------------------------------+
| MOD        | Modulus (remainder) operator                                |
+------------+-------------------------------------------------------------+
| POW        | Exponentation operator                                      |
+------------+-------------------------------------------------------------+
| DIV        | Division operator                                           |
+------------+-------------------------------------------------------------+
| IDIV       | Integer division operator                                   |
+------------+-------------------------------------------------------------+
| BAND       | Bit-wise AND operator                                       |
+------------+-------------------------------------------------------------+
| BOR        | Bit-wise OR operator                                        |
+------------+-------------------------------------------------------------+
| BXOR       | Bit-wise Exclusive OR operator                              |
+------------+-------------------------------------------------------------+
| SHL        | Shift bits left                                             |
+------------+-------------------------------------------------------------+
| SHR        | Shift bits right                                            |
+------------+-------------------------------------------------------------+
| UNM        | Unary minus                                                 |
+------------+-------------------------------------------------------------+
| BNOT       | Bit-wise NOT operator                                       |
+------------+-------------------------------------------------------------+
| NOT        | Logical NOT operator                                        |
+------------+-------------------------------------------------------------+
| LEN        | Length operator                                             |
+------------+-------------------------------------------------------------+
| CONCAT     | Concatenate a range of registers                            |
+------------+-------------------------------------------------------------+
| JMP        | Unconditional jump                                          |
+------------+-------------------------------------------------------------+
| EQ         | Equality test                                               |
+------------+-------------------------------------------------------------+
| LT         | Less than test                                              |
+------------+-------------------------------------------------------------+
| LE         | Less than or equal to test                                  |
+------------+-------------------------------------------------------------+
| TEST       | Boolean test, with conditional jump                         |
+------------+-------------------------------------------------------------+
| TESTSET    | Boolean test, with conditional jump and assignment          |
+------------+-------------------------------------------------------------+
| CALL       | Call a closure                                              |
+------------+-------------------------------------------------------------+
| TAILCALL   | Perform a tail call                                         |
+------------+-------------------------------------------------------------+
| RETURN     | Return from function call                                   |
+------------+-------------------------------------------------------------+
| FORLOOP    | Iterate a numeric for loop                                  |
+------------+-------------------------------------------------------------+
| FORPREP    | Initialization for a numeric for loop                       |
+------------+-------------------------------------------------------------+
| TFORLOOP   | Iterate a generic for loop                                  |
+------------+-------------------------------------------------------------+
| TFORCALL   | Initialization for a generic for loop                       |
+------------+-------------------------------------------------------------+
| SETLIST    | Set a range of array elements for a table                   |
+------------+-------------------------------------------------------------+
| CLOSURE    | Create a closure of a function prototype                    |
+------------+-------------------------------------------------------------+
| VARARG     | Assign vararg function arguments to registers               |
+------------+-------------------------------------------------------------+


'``OP_CALL``' instruction
=========================

Syntax
------

::

  CALL A B C    R(A), ... ,R(A+C-2) := R(A)(R(A+1), ... ,R(A+B-1))

Description
-----------

Performs a function call, with register R(A) holding the reference to the function object to be called. Parameters to the function are placed in the registers following R(A). If B is 1, the function has no parameters. If B is 2 or more, there are (B-1) parameters. If B >= 2, then upon entry to the called function, R(A+1) will become the ``base``. 

If B is 0, the function parameters range from R(A+1) to the top of the stack. This form is used when the 
number of parameters to pass is set by the previous VM instruction, which has to be one of '``OP_CALL``' or
'``OP_VARARG``'. 

Examples
--------

Example of '``OP_VARARG``' followed by '``OP_CALL``'::

  function y(...) print(...) end

  1 [1] GETTABUP  0 0 -1  ; _ENV "print"
  2 [1] VARARG    1 0     ; VARARG will set L->top
  3 [1] CALL      0 0 1   ; B=0 so L->top set by previous instruction
  4 [1] RETURN    0 1

Example of '``OP_CALL``' followed by '``OP_CALL``'::

  function z1() y(x()) end

  1 [1] GETTABUP  0 0 -1  ; _ENV "y"
  2 [1] GETTABUP  1 0 -2  ; _ENV "x"
  3 [1] CALL      1 1 0   ; C=0 so return values indicated by L->top
  4 [1] CALL      0 0 1   ; B=0 so L->top set by previous instruction
  5 [1] RETURN    0 1

Thus upon entry to a function ``base`` is always the location of the first fixed parameter if any or else ``local`` if any. The three possibilities are shown below.

::

                                       Two variable args and 1     Two variable args and no 
  Caller   One fixed arg               fixed arg                   fixed args
  R(A)     CI->func  [ function    ]   CI->func  [ function    ]   CI->func [ function   ]
  R(A+1)   CI->base  [ fixed arg 1 ]             [ var arg 1   ]            [ var arg 1  ]
  R(A+2)             [ local 1     ]             [ var arg 2   ]            [ var arg 2  ]
  R(A+3)                               CI->base  [ fixed arg 1 ]   CI->base [ local 1    ]
  R(A+4)                                         [ local 1     ]
                                        
Results returned by the function call are placed in a range of registers starting from R(A). If C is 1, no return results are saved. If C is 2 or more, (C-1) return values are saved. If C is 0, then multiple return results are saved. In this case the number of values to save is determined by one of following ways:

* A C function returns an integer value indicating number of results returned so for C function calls
  this is used (see the value of ``n`` passed to `luaD_poscall() <http://www.lua.org/source/5.3/ldo.c.html#luaD_poscall>`_ in `luaD_precall() <http://www.lua.org/source/5.3/ldo.c.html#luaD_precall>`_)
* For Lua functions, the the results are saved by the called function's '``OP_RETURN``' instruction.

More examples
-------------

::

  x=function() y() end

Produces::

  function <stdin:1,1> (3 instructions at 000000CECB2BE040)
  0 params, 2 slots, 1 upvalue, 0 locals, 1 constant, 0 functions
    1       [1]     GETTABUP        0 0 -1  ; _ENV "y"
    2       [1]     CALL            0 1 1
    3       [1]     RETURN          0 1
  constants (1) for 000000CECB2BE040:
    1       "y"
  locals (0) for 000000CECB2BE040:
  upvalues (1) for 000000CECB2BE040:
    0       _ENV    0       0

In line [2], the call has zero parameters (field B is 1), zero results are retained (field C is 1), while register 0 temporarily holds the reference to the function object from global y. Next we see a function call with multiple parameters or arguments::

  x=function() z(1,2,3) end

Generates::

  function <stdin:1,1> (6 instructions at 000000CECB2D7BC0)
  0 params, 4 slots, 1 upvalue, 0 locals, 4 constants, 0 functions
    1       [1]     GETTABUP        0 0 -1  ; _ENV "z"
    2       [1]     LOADK           1 -2    ; 1
    3       [1]     LOADK           2 -3    ; 2
    4       [1]     LOADK           3 -4    ; 3
    5       [1]     CALL            0 4 1
    6       [1]     RETURN          0 1
  constants (4) for 000000CECB2D7BC0:
    1       "z"
    2       1
    3       2
    4       3
  locals (0) for 000000CECB2D7BC0:
  upvalues (1) for 000000CECB2D7BC0:
    0       _ENV    0       0


Lines [1] to [4] loads the function reference and the arguments in order, then line [5] makes the call with an operand B value of 4, which means there are 3 parameters. Since the call statement is not assigned to anything, no return results need to be retained, hence field C is 1. Here is an example that uses multiple parameters and multiple return values::


  x=function() local p,q,r,s = z(y()) end

Produces::

  function <stdin:1,1> (5 instructions at 000000CECB2D6CC0)
  0 params, 4 slots, 1 upvalue, 4 locals, 2 constants, 0 functions
    1       [1]     GETTABUP        0 0 -1  ; _ENV "z"
    2       [1]     GETTABUP        1 0 -2  ; _ENV "y"
    3       [1]     CALL            1 1 0
    4       [1]     CALL            0 0 5
    5       [1]     RETURN          0 1
  constants (2) for 000000CECB2D6CC0:
    1       "z"
    2       "y"
  locals (4) for 000000CECB2D6CC0:
    0       p       5       6
    1       q       5       6
    2       r       5       6
    3       s       5       6
  upvalues (1) for 000000CECB2D6CC0:
    0       _ENV    0       0

First, the function references are retrieved (lines [1] and [2]), then function y is called first (temporary register 1). The CALL
has a field C of 0, meaning multiple return values are accepted. These return values become the parameters to function z, and so in line [4], field B of the CALL instruction is 0, signifying multiple parameters. After the call to function z, 4 results are retained, so field C in line [4] is 5. Finally, here is an example with calls to standard library functions::

  x=function() print(string.char(64)) end

Leads to::

  function <stdin:1,1> (7 instructions at 000000CECB2D6220)
  0 params, 3 slots, 1 upvalue, 0 locals, 4 constants, 0 functions
    1       [1]     GETTABUP        0 0 -1  ; _ENV "print"
    2       [1]     GETTABUP        1 0 -2  ; _ENV "string"
    3       [1]     GETTABLE        1 1 -3  ; "char"
    4       [1]     LOADK           2 -4    ; 64
    5       [1]     CALL            1 2 0
    6       [1]     CALL            0 0 1
    7       [1]     RETURN          0 1
  constants (4) for 000000CECB2D6220:
    1       "print"
    2       "string"
    3       "char"
    4       64
  locals (0) for 000000CECB2D6220:
  upvalues (1) for 000000CECB2D6220:
    0       _ENV    0       0

When a function call is the last parameter to another function call, the former can pass multiple return values, while the latter can accept multiple parameters.

'``OP_TAILCALL``' instruction
=============================

Syntax
------

::

  TAILCALL  A B C return R(A)(R(A+1), ... ,R(A+B-1))

Description
-----------

Performs a tail call, which happens when a return statement has a single function call as the expression, e.g. return foo(bar). A tail call results in the function being interpreted within the same call frame as the caller - the stack is replaced and then a 'goto' executed to start at the entry point in the VM. Only Lua functions can be tailcalled. Tailcalls allow infinite recursion without growing the stack.

Like '``OP_CALL``', register R(A) holds the reference to the function object to be called. B encodes the number of parameters in the same manner as a '``OP_CALL``' instruction.

C isn’t used by TAILCALL, since all return results are significant. In any case, Lua always generates a 0 for C, to denote multiple return results.

Examples
--------
An '``OP_TAILCALL``' is used only for one specific return style, described above. Multiple return results are always produced by a tail call. Here is an example:


::

  function y() return x('foo', 'bar') end

Generates::

  function <stdin:1,1> (6 instructions at 000000C3C24DE4A0)
  0 params, 3 slots, 1 upvalue, 0 locals, 3 constants, 0 functions
    1       [1]     GETTABUP        0 0 -1  ; _ENV "x"
    2       [1]     LOADK           1 -2    ; "foo"
    3       [1]     LOADK           2 -3    ; "bar"
    4       [1]     TAILCALL        0 3 0
    5       [1]     RETURN          0 0
    6       [1]     RETURN          0 1
  constants (3) for 000000C3C24DE4A0:
    1       "x"
    2       "foo"
    3       "bar"
  locals (0) for 000000C3C24DE4A0:
  upvalues (1) for 000000C3C24DE4A0:
    0       _ENV    0       0


Arguments for a tail call are handled in exactly the same way as arguments for a normal call, so in line [4], the tail call has a field B value of 3, signifying 2 parameters. Field C is 0, for multiple returns; this due to the constant LUA_MULTRET in lua.h. In practice, field C is not used by the virtual machine (except as an assert) since the syntax guarantees multiple return results.
Line [5] is a '``OP_RETURN``' instruction specifying multiple return results. This is required when the function called by '``OP_TAILCALL`` is a C function. In the case of a C function, execution continues to line [5] upon return, thus the RETURN is necessary. Line [6] is redundant. When Lua functions are tailcalled, the virtual machine does not return to line [5] at all.

'``OP_RETURN``' instruction
===========================

Syntax
------

::

  RETURN  A B return R(A), ... ,R(A+B-2)

Description
-----------

Returns to the calling function, with optional return values. 

First '``OP_RETURN``'' closes any open upvalues by calling `luaF_close() <http://www.lua.org/source/5.3/lfunc.c.html#luaF_close>`_.

If B is 1, there are no return values. If B is 2 or more, there are (B-1) return values, located in consecutive registers from R(A) onwards. If B is 0, the set of values range from R(A) to the top of the stack. 

It is assumed that if the VM is returning to a Lua function then it is within the same invocation of the ``luaV_execute()``. Else it is assumed that ``luaV_execute()`` is being invoked from a C function.

If B is 0 then the previous instruction (which must be either '``OP_CALL``' or '``OP_VARARG``' ) would have set ``L->top`` to indicate how many values to return. The number of values to be returned in this case is R(A) to L->top. 

If B > 0 then the number of values to be returned is simply B-1.

'``OP_RETURN``' calls `luaD_poscall() <http://www.lua.org/source/5.3/ldo.c.html#luaD_poscall>`_ which is responsible for copying return values to the caller - the first result is placed at the current ``closure``'s address. ``luaD_poscall()`` leaves ``L->top`` just past the last result that was copied.

If '``OP_RETURN``' is returning to a Lua function and if the number of return values expected was indeterminate - i.e. '``OP_CALL``' had operand C = 0, then ``L->top`` is left where ``luaD_poscall()`` placed it - just beyond the top of the result list. This allows the '``OP_CALL``' instruction to figure out how many results were returned. If however '``OP_CALL``' had invoked with a value of C > 0 then the expected number of results is known, and in that case, ``L->top`` is reset to  the calling function's ``C->top``.

If ``luaV_execute()`` was called externally then '``OP_RETURN``' leaves ``L->top`` unchanged - so it will continue to be just past the top of the results list. This is because luaV_execute() does not have a way of informing callers how many values were returned; so the caller can determine the number of results by inspecting ``L->top``.

Examples
--------

Example of '``OP_VARARG``' followed by '``OP_RETURN``'::

  function x(...) return ... end

  1 [1]  VARARG          0 0
  2 [1]  RETURN          0 0

Suppose we call ``x(1,2,3)``; then, observe the setting of ``L->top`` when '``OP_RETURN``' executes::

  (LOADK A=1 Bx=-2)      L->top = 4, ci->top = 4
  (LOADK A=2 Bx=-3)      L->top = 4, ci->top = 4
  (LOADK A=3 Bx=-4)      L->top = 4, ci->top = 4
  (TAILCALL A=0 B=4 C=0) L->top = 4, ci->top = 4
  (VARARG A=0 B=0)       L->top = 2, ci->top = 2  ; we are in x()
  (RETURN A=0 B=0)       L->top = 3, ci->top = 2

Observe that '``OP_VARARG``' set ``L->top`` to ``base+3``.

But if we call ``x(1)`` instead::

  (LOADK A=1 Bx=-2)      L->top = 4, ci->top = 4
  (LOADK A=2 Bx=-3)      L->top = 4, ci->top = 4
  (LOADK A=3 Bx=-4)      L->top = 4, ci->top = 4
  (TAILCALL A=0 B=4 C=0) L->top = 4, ci->top = 4
  (VARARG A=0 B=0)       L->top = 2, ci->top = 2 ; we are in x()
  (RETURN A=0 B=0)       L->top = 1, ci->top = 2

Notice that this time '``OP_VARARG``' set ``L->top`` to ``base+1``.

'``OP_JMP``' instruction
========================

Syntax
------

::

  JMP sBx PC += sBx

Description
-----------

Performs an unconditional jump, with sBx as a signed displacement. sBx is added to the program counter (PC), which points to the next instruction to be executed. 
E.g., if sBx is 0, the VM will proceed to the next instruction.

'``OP_JMP``' is used in loops, conditional statements, and in expressions when a boolean true/false need to be generated.

Examples
--------

For example, since a relational test instruction makes conditional jumps rather than generate a boolean result, a JMP is used in the code sequence for loading either a true or a false::

  function x() local m, n; return m >= n end

Generates::

  function <stdin:1,1> (7 instructions at 00000034D2ABE340)
  0 params, 3 slots, 0 upvalues, 2 locals, 0 constants, 0 functions
    1       [1]     LOADNIL         0 1
    2       [1]     LE              1 1 0   ; to 4 if false    (n <= m)
    3       [1]     JMP             0 1     ; to 5
    4       [1]     LOADBOOL        2 0 1
    5       [1]     LOADBOOL        2 1 0
    6       [1]     RETURN          2 2
    7       [1]     RETURN          0 1
  constants (0) for 00000034D2ABE340:
  locals (2) for 00000034D2ABE340:
    0       m       2       8
    1       n       2       8
  upvalues (0) for 00000034D2ABE340:

Line[2] performs the relational test. In line [3], the JMP skips over the false path (line [4]) to the true path (line [5]). The result is placed into temporary local 2, and returned to the caller by RETURN in line [6].

'``OP_VARARG``' instruction
===========================

Syntax
------

::

  VARARG  A B R(A), R(A+1), ..., R(A+B-1) = vararg

Description
-----------

``VARARG`` implements the vararg operator ``...`` in expressions. ``VARARG`` copies B-1 parameters into a number of registers starting from R(A), padding with nils if there aren’t enough values. If B is 0, ``VARARG`` copies as many values as it can based on the number of parameters passed. If a fixed number of values is required, B is a value greater than 1. If any number of values is required, B is 0.


Examples
--------

The use of VARARG will become clear with the help of a few examples::

  local a,b,c = ...

Generates::

  main <(string):0,0> (2 instructions at 00000029D9FA8310)
  0+ params, 3 slots, 1 upvalue, 3 locals, 0 constants, 0 functions
        1       [1]     VARARG          0 4
        2       [1]     RETURN          0 1
  constants (0) for 00000029D9FA8310:
  locals (3) for 00000029D9FA8310:
        0       a       2       3
        1       b       2       3
        2       c       2       3
  upvalues (1) for 00000029D9FA8310:
        0       _ENV    1       0  

Note that the main or top-level chunk is a vararg function. In this example, the left hand side of the assignment statement needs three values (or objects.) So in instruction [1], the operand B of the ``VARARG`` instruction is (3+1), or 4. ``VARARG`` will copy three values into a, b and c. If there are less than three values available, nils will be used to fill up the empty places.

::

  local a = function(...) local a,b,c = ... end

This gives::

  main <(string):0,0> (2 instructions at 00000029D9FA72D0)
  0+ params, 2 slots, 1 upvalue, 1 local, 0 constants, 1 function
        1       [1]     CLOSURE         0 0     ; 00000029D9FA86D0
        2       [1]     RETURN          0 1
  constants (0) for 00000029D9FA72D0:
  locals (1) for 00000029D9FA72D0:
        0       a       2       3
  upvalues (1) for 00000029D9FA72D0:
        0       _ENV    1       0

  function <(string):1,1> (2 instructions at 00000029D9FA86D0)
  0+ params, 3 slots, 0 upvalues, 3 locals, 0 constants, 0 functions
        1       [1]     VARARG          0 4
        2       [1]     RETURN          0 1
  constants (0) for 00000029D9FA86D0:
  locals (3) for 00000029D9FA86D0:
        0       a       2       3
        1       b       2       3
        2       c       2       3
  upvalues (0) for 00000029D9FA86D0:


Here is an alternate version where a function is instantiated and assigned to local a. The old-style arg is retained for compatibility purposes, but is unused in the above example.

::

  local a; a(...)

Leads to::

  main <(string):0,0> (5 instructions at 00000029D9FA6D30)
  0+ params, 3 slots, 1 upvalue, 1 local, 0 constants, 0 functions
        1       [1]     LOADNIL         0 0
        2       [1]     MOVE            1 0
        3       [1]     VARARG          2 0
        4       [1]     CALL            1 0 1
        5       [1]     RETURN          0 1
  constants (0) for 00000029D9FA6D30:
  locals (1) for 00000029D9FA6D30:
        0       a       2       6
  upvalues (1) for 00000029D9FA6D30:
        0       _ENV    1       0

When a function is called with ``...`` as the argument, the function will accept a variable number of parameters or arguments. On instruction [3], a ``VARARG`` with a B field of 0 is used. The ``VARARG`` will copy all the parameters passed on to the main chunk to register 2 onwards, so that the ``CALL`` in the next line can utilize them as parameters of function ``a``. The function call is set to accept a multiple number of parameters and returns zero results.

::

  local a = {...}

Produces::

  main <(string):0,0> (4 instructions at 00000029D9FA8130)
  0+ params, 2 slots, 1 upvalue, 1 local, 0 constants, 0 functions
        1       [1]     NEWTABLE        0 0 0
        2       [1]     VARARG          1 0
        3       [1]     SETLIST         0 0 1   ; 1
        4       [1]     RETURN          0 1
  constants (0) for 00000029D9FA8130:
  locals (1) for 00000029D9FA8130:
        0       a       4       5
  upvalues (1) for 00000029D9FA8130:
        0       _ENV    1       0

And::

  return ...

Produces::

  main <(string):0,0> (3 instructions at 00000029D9FA8270)
  0+ params, 2 slots, 1 upvalue, 0 locals, 0 constants, 0 functions
        1       [1]     VARARG          0 0
        2       [1]     RETURN          0 0
        3       [1]     RETURN          0 1
  constants (0) for 00000029D9FA8270:
  locals (0) for 00000029D9FA8270:
  upvalues (1) for 00000029D9FA8270:
        0       _ENV    1       0

Above are two other cases where ``VARARG`` needs to copy all passed parameters 
over to a set of registers in order for the next operation to proceed. Both the above forms of 
table creation and return accepts a variable number of values or objects.

'``OP_LOADBOOL``' instruction
=========================

Syntax
------

::

  LOADBOOL A B C    R(A) := (Bool)B; if (C) pc++      

Description
-----------

Loads a boolean value (true or false) into register R(A). true is usually encoded as an integer 1, false is always 0. If C is non-zero, then the next instruction is skipped (this is used when you have an assignment statement where the expression uses relational operators, e.g. M = K>5.)
You can use any non-zero value for the boolean true in field B, but since you cannot use booleans as numbers in Lua, it’s best to stick to 1 for true.

``LOADBOOL`` is used for loading a boolean value into a register. It’s also used where a boolean result is supposed to be generated, because relational test instructions, for example, do not generate boolean results – they perform conditional jumps instead. The operand C is used to optionally skip the next instruction (by incrementing PC by 1) in order to support such code. For simple assignments of boolean values, C is always 0.

Examples
--------

The following line of code::

  f=load('local a,b = true,false')

generates::

  main <(string):0,0> (3 instructions at 0000020F274C2610)
  0+ params, 2 slots, 1 upvalue, 2 locals, 0 constants, 0 functions
        1       [1]     LOADBOOL        0 1 0
        2       [1]     LOADBOOL        1 0 0
        3       [1]     RETURN          0 1
  constants (0) for 0000020F274C2610:
  locals (2) for 0000020F274C2610:
        0       a       3       4
        1       b       3       4
  upvalues (1) for 0000020F274C2610:
        0       _ENV    1       0

This example is straightforward: Line [1] assigns true to local a (register 0) while line [2] assigns false to local b (register 1). In both cases, field C is 0, so PC is not incremented and the next instruction is not skipped.

Next, look at this line::

  f=load('local a = 5 > 2')

This leads to following bytecode::

  main <(string):0,0> (5 instructions at 0000020F274BAE00)
  0+ params, 2 slots, 1 upvalue, 1 local, 2 constants, 0 functions
        1       [1]     LT              1 -2 -1 ; 2 5
        2       [1]     JMP             0 1     ; to 4
        3       [1]     LOADBOOL        0 0 1
        4       [1]     LOADBOOL        0 1 0
        5       [1]     RETURN          0 1
  constants (2) for 0000020F274BAE00:
        1       5
        2       2
  locals (1) for 0000020F274BAE00:
        0       a       5       6
  upvalues (1) for 0000020F274BAE00:
        0       _ENV    1       0

This is an example of an expression that gives a boolean result and is assigned to a variable. Notice that Lua does not optimize the expression into a true value; Lua does not perform compile-time constant evaluation for relational operations, but it can perform simple constant evaluation for arithmetic operations.

Since the relational operator ``LT``  does not give a boolean result but performs a conditional jump, ``LOADBOOL`` uses its C operand to perform an unconditional jump in line [3] – this saves one instruction and makes things a little tidier. The reason for all this is that the instruction set is simply optimized for if...then blocks. Essentially, ``local a = 5 > 2`` is executed in the following way::

  local a 
  if 2 < 5 then  
    a = true 
  else  
    a = false 
  end

In the disassembly listing, when ``LT`` tests 2 < 5, it evaluates to true and doesn’t perform a conditional jump. Line [2] jumps over the false result path, and in line [4], the local a (register 0) is assigned the boolean true by the instruction ``LOADBOOL``. If 2 and 5 were reversed, line [3] will be followed instead, setting a false, and then the true result path (line [4]) will be skipped, since ``LOADBOOL`` has its field C set to non-zero.

So the true result path goes like this (additional comments in parentheses)::

        1       [1]     LT              1 -2 -1 ; 2 5       (if 2 < 5)
        2       [1]     JMP             0 1     ; to 4     
        4       [1]     LOADBOOL        0 1 0   ;           (a = true)           
        5       [1]     RETURN          0 1

and the false result path (which never executes in this example) goes like this::

        1       [1]     LT              1 -2 -1 ; 2 5       (if 2 < 5)
        3       [1]     LOADBOOL        0 0 1               (a = false)
        5       [1]     RETURN          0 1

The true result path looks longer, but it isn’t, due to the way the virtual machine is implemented. This will be discussed further in the section on relational and logic instructions.



'``OP_EQ``', '``OP_LT``' and '``OP_LE``' Instructions
=====================================================

Relational and logic instructions are used in conjunction with other instructions to implement control 
structures or expressions. Instead of generating boolean results, these instructions conditionally perform 
a jump over the next instruction; the emphasis is on implementing control blocks. Instructions are arranged 
so that there are two paths to follow based on the relational test.

::

  EQ  A B C if ((RK(B) == RK(C)) ~= A) then PC++
  LT  A B C if ((RK(B) <  RK(C)) ~= A) then PC++
  LE  A B C if ((RK(B) <= RK(C)) ~= A) then PC++

Description
-----------

Compares RK(B) and RK(C), which may be registers or constants. If the boolean result is not A, 
then skip the next instruction. Conversely, if the boolean result equals A, continue with the 
next instruction.

``EQ`` is for equality. ``LT`` is for “less than” comparison. ``LE`` is for “less than or equal to” 
comparison. The boolean A field allows the full set of relational comparison operations to be 
synthesized from these three instructions. The Lua code generator produces either 0 or 1 for the boolean A.

For the fall-through case, a ``JMP`` is always expected, in order to optimize execution in the 
virtual machine. In effect, ``EQ``, ``LT`` and ``LE`` must always be paired with a following ``JMP`` 
instruction.

Examples
--------
By comparing the result of the relational operation with A, the sense of the comparison can 
be reversed. Obviously the alternative is to reverse the paths taken by the instruction, but that 
will probably complicate code generation some more. The conditional jump is performed if the comparison 
result is not A, whereas execution continues normally if the comparison result matches A. 
Due to the way code is generated and the way the virtual machine works, a ``JMP`` instruction is 
always expected to follow an ``EQ``, ``LT`` or ``LE``. The following ``JMP`` is optimized by 
executing it in conjunction with ``EQ``, ``LT`` or ``LE``.

::

  local x,y; return x ~= y

Generates::

  main <(string):0,0> (7 instructions at 0000001BC48FD390)
  0+ params, 3 slots, 1 upvalue, 2 locals, 0 constants, 0 functions
        1       [1]     LOADNIL         0 1
        2       [1]     EQ              0 0 1
        3       [1]     JMP             0 1     ; to 5
        4       [1]     LOADBOOL        2 0 1
        5       [1]     LOADBOOL        2 1 0
        6       [1]     RETURN          2 2
        7       [1]     RETURN          0 1
  constants (0) for 0000001BC48FD390:
  locals (2) for 0000001BC48FD390:
        0       x       2       8
        1       y       2       8
  upvalues (1) for 0000001BC48FD390:
        0       _ENV    1       0

In the above example, the equality test is performed in instruction [2]. However, since the 
comparison need to be returned as a result, ``LOADBOOL`` instructions are used to set a 
register with the correct boolean value. This is the usual code pattern generated if the expression 
requires a boolean value to be generated and stored in a register as an intermediate value or 
a final result.

It is easier to visualize the disassembled code as::

  if x ~= y then
    return true
  else
    return false
  end

The true result path (when the comparison result matches A) goes like this::

  1  [1] LOADNIL    0   1      
  2  [1] EQ         0   0   1    ; to 4 if true    (x ~= y)
  3  [1] JMP        1            ; to 5
  5  [1] LOADBOOL   2   1   0    ; true            (true path)
  6  [1] RETURN     2   2      

While the false result path (when the comparison result does not match A) goes like this::

  1  [1] LOADNIL    0   1      
  2  [1] EQ         0   0   1    ; to 4 if true    (x ~= y)
  4  [1] LOADBOOL   2   0   1    ; false, to 6     (false path)
  6  [1] RETURN     2   2      

Comments following the ``EQ`` in line [2] lets the user know when the conditional jump 
is taken. The jump is taken when “the value in register 0 equals to the value in register 1” 
(the comparison) is not false (the value of operand A). If the comparison is x == y, 
everything will be the same except that the A operand in the ``EQ`` instruction will be 1, 
thus reversing the sense of the comparison. Anyway, these are just the Lua code generator’s 
conventions; there are other ways to code x ~= y in terms of Lua virtual machine instructions.

For conditional statements, there is no need to set boolean results. Lua is optimized for 
coding the more common conditional statements rather than conditional expressions.

::

  local x,y; if x ~= y then return "foo" else return "bar" end

Results in::

  main <(string):0,0> (9 instructions at 0000001BC4914D50)
  0+ params, 3 slots, 1 upvalue, 2 locals, 2 constants, 0 functions
        1       [1]     LOADNIL         0 1
        2       [1]     EQ              1 0 1   ; to 4 if false    (x ~= y)
        3       [1]     JMP             0 3     ; to 7
        4       [1]     LOADK           2 -1    ; "foo"            (true block)
        5       [1]     RETURN          2 2
        6       [1]     JMP             0 2     ; to 9
        7       [1]     LOADK           2 -2    ; "bar"            (false block)
        8       [1]     RETURN          2 2
        9       [1]     RETURN          0 1
  constants (2) for 0000001BC4914D50:
        1       "foo"
        2       "bar"
  locals (2) for 0000001BC4914D50:
        0       x       2       10
        1       y       2       10
  upvalues (1) for 0000001BC4914D50:
        0       _ENV    1       0

In the above conditional statement, the same inequality operator is used in the source, 
but the sense of the ``EQ`` instruction in line [2] is now reversed. Since the ``EQ`` 
conditional jump can only skip the next instruction, additional ``JMP`` instructions 
are needed to allow large blocks of code to be placed in both true and false paths. 
In contrast, in the previous example, only a single instruction is needed to set a 
boolean value. For ``if`` statements, the true block comes first followed by the false 
block in code generated by the code generator. To reverse the positions of the true and 
false paths, the value of operand A is changed.

The true path (when ``x ~= y`` is true) goes from [2] to [4]–[6] and on to [9]. Since 
there is a ``RETURN`` in line [5], the ``JMP`` in line [6] and the ``RETURN`` in [9] 
are never executed at all; they are redundant but does not adversely affect performance 
in any way. The false path is from [2] to [3] to [7]–[9] onwards. So in a disassembly 
listing, you should see the true and false code blocks in the same order as in the 
Lua source.

The following is another example, this time with an ``elseif``::

  if 8 > 9 then return 8 elseif 5 >= 4 then return 5 else return 9 end

Generates::

  main <(string):0,0> (13 instructions at 0000001BC4913770)
  0+ params, 2 slots, 1 upvalue, 0 locals, 4 constants, 0 functions
        1       [1]     LT              0 -2 -1 ; 9 8
        2       [1]     JMP             0 3     ; to 6
        3       [1]     LOADK           0 -1    ; 8
        4       [1]     RETURN          0 2
        5       [1]     JMP             0 7     ; to 13
        6       [1]     LE              0 -4 -3 ; 4 5
        7       [1]     JMP             0 3     ; to 11
        8       [1]     LOADK           0 -3    ; 5
        9       [1]     RETURN          0 2
        10      [1]     JMP             0 2     ; to 13
        11      [1]     LOADK           0 -2    ; 9
        12      [1]     RETURN          0 2
        13      [1]     RETURN          0 1
  constants (4) for 0000001BC4913770:
        1       8
        2       9
        3       5
        4       4
  locals (0) for 0000001BC4913770:
  upvalues (1) for 0000001BC4913770:
        0       _ENV    1       0

This example is a little more complex, but the blocks are structured in the same order 
as the Lua source, so interpreting the disassembled code should not be too hard.

'``OP_TEST``' and '``OP_TESTSET``' instructions
===============================================

Syntax
------

::

  TEST        A C     if not (R(A) <=> C) then pc++     
  TESTSET     A B C   if (R(B) <=> C) then R(A) := R(B) else pc++ 

Description
-----------
These two instructions used for performing boolean tests and implementing Lua’s logic operators.

Used to implement and and or logical operators, or for testing a single register in a conditional statement.

For ``TESTSET``, register R(B) is coerced into a boolean and compared to the boolean field C. If R(B) matches C, the next instruction is skipped, otherwise R(B) is assigned to R(A) and the VM continues with the next instruction. The and operator uses a C of 0 (false) while or uses a C value of 1 (true).

``TEST`` is a more primitive version of ``TESTSET``. ``TEST`` is used when the assignment operation is not needed, otherwise it is the same as ``TESTSET`` except that the operand slots are different.

For the fall-through case, a ``JMP`` is always expected, in order to optimize execution in the virtual machine. In effect, ``TEST`` and ``TESTSET`` must always be paired with a following ``JMP`` instruction.

Examples
--------

``TEST`` and ``TESTSET`` are used in conjunction with a following ``JMP`` instruction, while ``TESTSET`` has an addditional conditional assignment. Like ``EQ``, ``LT`` and ``LE``, the following ``JMP`` instruction is compulsory, as the virtual machine will execute the ``JMP`` together with ``TEST`` or ``TESTSET``. The two instructions are used to implement short-circuit LISP-style logical operators that retains and propagates operand values instead of booleans. First, we’ll look at how and and or behaves::

  f=load('local a,b,c; c = a and b')

Generates::

  main <(string):0,0> (5 instructions at 0000020F274CF1A0)
  0+ params, 3 slots, 1 upvalue, 3 locals, 0 constants, 0 functions
        1       [1]     LOADNIL         0 2
        2       [1]     TESTSET         2 0 0   ; to 4 if true 
        3       [1]     JMP             0 1     ; to 5
        4       [1]     MOVE            2 1
        5       [1]     RETURN          0 1
  constants (0) for 0000020F274CF1A0:
  locals (3) for 0000020F274CF1A0:
        0       a       2       6
        1       b       2       6
        2       c       2       6
  upvalues (1) for 0000020F274CF1A0:
        0       _ENV    1       0

An ``and`` sequence exits on ``false`` operands (which can be ``false`` or ``nil``) because any ``false`` operands in a string of and operations will make the whole boolean expression ``false``. If operands evaluates to ``true``, evaluation continues. When a string of ``and`` operations evaluates to ``true``, the result is the last operand value.

In line [2], the first operand (the local a) is set to local c when the test is false (with a field C of 0), while the jump to [4] is made when the test is true, and then in line [4], the expression result is set to the second operand (the local b). This is equivalent to::

  if a then  
    c = b      -- executed by MOVE on line [4] 
  else  
    c = a      -- executed by TESTSET on line [2] 
  end

The ``c = a`` portion is done by ``TESTSET`` itself, while ``MOVE`` performs ``c = b``. Now, if the result is already set with one of the possible values, a ``TEST`` instruction is used instead::

  f=load('local a,b; a = a and b')

Generates::

  main <(string):0,0> (5 instructions at 0000020F274D0A70)
  0+ params, 2 slots, 1 upvalue, 2 locals, 0 constants, 0 functions
        1       [1]     LOADNIL         0 1
        2       [1]     TEST            0 0     ; to 4 if true 
        3       [1]     JMP             0 1     ; to 5
        4       [1]     MOVE            0 1
        5       [1]     RETURN          0 1
  constants (0) for 0000020F274D0A70:
  locals (2) for 0000020F274D0A70:
        0       a       2       6
        1       b       2       6
  upvalues (1) for 0000020F274D0A70:
        0       _ENV    1       0

The ``TEST`` instruction does not perform an assignment operation, since ``a = a`` is redundant. This makes ``TEST`` a little faster. This is equivalent to::

  if a then  
    a = b 
  end

Next, we will look at the or operator::

  f=load('local a,b,c; c = a or b')

Generates::

  main <(string):0,0> (5 instructions at 0000020F274D1AB0)
  0+ params, 3 slots, 1 upvalue, 3 locals, 0 constants, 0 functions
        1       [1]     LOADNIL         0 2
        2       [1]     TESTSET         2 0 1   ; to 4 if false 
        3       [1]     JMP             0 1     ; to 5
        4       [1]     MOVE            2 1
        5       [1]     RETURN          0 1
  constants (0) for 0000020F274D1AB0:
  locals (3) for 0000020F274D1AB0:
        0       a       2       6
        1       b       2       6
        2       c       2       6
  upvalues (1) for 0000020F274D1AB0:
        0       _ENV    1       0

An ``or`` sequence exits on ``true`` operands, because any operands evaluating to ``true`` in a string of or operations will make the whole boolean expression ``true``. If operands evaluates to ``false``, evaluation continues. When a string of or operations evaluates to ``false``, all operands must have evaluated to ``false``.

In line [2], the local ``a`` value is set to local c if it is ``true``, while the jump is made if it is ``false`` (the field C is 1). Thus in line [4], the local ``b`` value is the result of the expression if local ``a`` evaluates to ``false``. This is equivalent to::

  if a then  
    c = a      -- executed by TESTSET on line [2] 
  else  
    c = b      -- executed by MOVE on line [4] 
  end

Like the case of and, TEST is used when the result already has one of the possible values, saving an assignment operation::

  f=load('local a,b; a = a or b')

Generates::

  main <(string):0,0> (5 instructions at 0000020F274D1010)
  0+ params, 2 slots, 1 upvalue, 2 locals, 0 constants, 0 functions
        1       [1]     LOADNIL         0 1
        2       [1]     TEST            0 1     ; to 4 if false
        3       [1]     JMP             0 1     ; to 5
        4       [1]     MOVE            0 1
        5       [1]     RETURN          0 1
  constants (0) for 0000020F274D1010:
  locals (2) for 0000020F274D1010:
        0       a       2       6
        1       b       2       6
  upvalues (1) for 0000020F274D1010:
        0       _ENV    1       0

Short-circuit logical operators also means that the following Lua code does not require the use of a boolean operation::

  f=load('local a,b,c; if a > b and a > c then return a end')

Leads to::

  main <(string):0,0> (7 instructions at 0000020F274D1150)
  0+ params, 3 slots, 1 upvalue, 3 locals, 0 constants, 0 functions
        1       [1]     LOADNIL         0 2
        2       [1]     LT              0 1 0   ; to 4 if true
        3       [1]     JMP             0 3     ; to 7
        4       [1]     LT              0 2 0   ; to 6 if true
        5       [1]     JMP             0 1     ; to 7
        6       [1]     RETURN          0 2
        7       [1]     RETURN          0 1
  constants (0) for 0000020F274D1150:
  locals (3) for 0000020F274D1150:
        0       a       2       8
        1       b       2       8
        2       c       2       8
  upvalues (1) for 0000020F274D1150:
        0       _ENV    1       0

With short-circuit evaluation, ``a > c`` is never executed if ``a > b`` is false, so the logic of the Lua statement can be readily implemented using the normal conditional structure. If both ``a > b`` and ``a > c`` are true, the path followed is [2] (the ``a > b`` test) to [4] (the ``a > c`` test) and finally to [6], returning the value of ``a``. A ``TEST`` instruction is not required. This is equivalent to::

  if a > b then  
    if a > c then    
      return a  
    end 
  end

For a single variable used in the expression part of a conditional statement, ``TEST`` is used to boolean-test the variable::

  f=load('if Done then return end')

Generates::

  main <(string):0,0> (5 instructions at 0000020F274D13D0)
  0+ params, 2 slots, 1 upvalue, 0 locals, 1 constant, 0 functions
        1       [1]     GETTABUP        0 0 -1  ; _ENV "Done"
        2       [1]     TEST            0 0     ; to 4 if true
        3       [1]     JMP             0 1     ; to 5
        4       [1]     RETURN          0 1
        5       [1]     RETURN          0 1
  constants (1) for 0000020F274D13D0:
        1       "Done"
  locals (0) for 0000020F274D13D0:
  upvalues (1) for 0000020F274D13D0:
        0       _ENV    1       0

In line [2], the ``TEST`` instruction jumps to the ``true`` block if the value in temporary register 0 (from the global ``Done``) is ``true``. The ``JMP`` at line [3] jumps over the ``true`` block, which is the code inside the if block (line [4]).

If the test expression of a conditional statement consist of purely boolean operators, then a number of TEST instructions will be used in the usual short-circuit evaluation style::

  f=load('if Found and Match then return end')

Generates::

  main <(string):0,0> (8 instructions at 0000020F274D1C90)
  0+ params, 2 slots, 1 upvalue, 0 locals, 2 constants, 0 functions
        1       [1]     GETTABUP        0 0 -1  ; _ENV "Found"
        2       [1]     TEST            0 0     ; to 4 if true
        3       [1]     JMP             0 4     ; to 8
        4       [1]     GETTABUP        0 0 -2  ; _ENV "Match"
        5       [1]     TEST            0 0     ; to 7 if true
        6       [1]     JMP             0 1     ; to 8
        7       [1]     RETURN          0 1
        8       [1]     RETURN          0 1
  constants (2) for 0000020F274D1C90:
        1       "Found"
        2       "Match"
  locals (0) for 0000020F274D1C90:
  upvalues (1) for 0000020F274D1C90:
        0       _ENV    1       0

In the last example, the true block of the conditional statement is executed only if both ``Found`` and ``Match`` evaluate to ``true``. The path is from [2] (test for ``Found``) to [4] to [5] (test for ``Match``) to [7] (the true block, which is an explicit ``return`` statement.)

If the statement has an ``else`` section, then the ``JMP`` on line [6] will jump to the false block (the ``else`` block) while an additional ``JMP`` will be added to the true block to jump over this new block of code. If ``or`` is used instead of ``and``, the appropriate C operand will be adjusted accordingly.

Finally, here is how Lua’s ternary operator (:? in C) equivalent works::

  f=load('local a,b,c; a = a and b or c')

Generates::

  main <(string):0,0> (7 instructions at 0000020F274D1A10)
  0+ params, 3 slots, 1 upvalue, 3 locals, 0 constants, 0 functions
        1       [1]     LOADNIL         0 2
        2       [1]     TEST            0 0     ; to 4 if true
        3       [1]     JMP             0 2     ; to 6
        4       [1]     TESTSET         0 1 1   ; to 6 if false
        5       [1]     JMP             0 1     ; to 7
        6       [1]     MOVE            0 2
        7       [1]     RETURN          0 1
  constants (0) for 0000020F274D1A10:
  locals (3) for 0000020F274D1A10:
        0       a       2       8
        1       b       2       8
        2       c       2       8
  upvalues (1) for 0000020F274D1A10:
        0       _ENV    1       0

The ``TEST`` in line [2] is for the ``and`` operator. First, local ``a`` is tested in line [2]. If it is false, then execution continues in [3], jumping to line [6]. Line [6] assigns local ``c`` to the end result because since if ``a`` is false, then ``a and b`` is ``false``, and ``false or c`` is ``c``.

If local ``a`` is ``true`` in line [2], the ``TEST`` instruction makes a jump to line [4], where there is a ``TESTSET``, for the ``or`` operator. If ``b`` evaluates to ``true``, then the end result is assigned the value of ``b``, because ``b or c`` is ``b`` if ``b`` is ``not false``. If ``b`` is also ``false``, the end result will be ``c``.

For the instructions in line [2], [4] and [6], the target (in field A) is register 0, or the local ``a``, which is the location where the result of the boolean expression is assigned. The equivalent Lua code is::

  if a then  
    if b then    
      a = b  
    else    
      a = c  
    end 
  else  
    a = c 
  end

The two ``a = c`` assignments are actually the same piece of code, but are repeated here to avoid using a ``goto`` and a label. Normally, if we assume ``b`` is ``not false`` and ``not nil``, we end up with the more recognizable form::

  if a then  
    a = b     -- assuming b ~= false 
  else  
    a = c 
  end


'``OP_FORPREP``' and '``OP_FORLOOP``' instructions
==================================================

Syntax
------
::

  FORPREP    A sBx   R(A)-=R(A+2); pc+=sBx
  FORLOOP    A sBx   R(A)+=R(A+2);
                     if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) }


Description
-----------
Lua has dedicated instructions to implement the two types of ``for`` loops, while the other two types of loops uses traditional test-and-jump.

``FORPREP`` initializes a numeric for loop, while ``FORLOOP`` performs an iteration of a numeric for loop.

A numeric for loop requires 4 registers on the stack, and each register must be a number. R(A) holds the initial value and doubles as the internal loop variable (the internal index); R(A+1) is the limit; R(A+2) is the stepping value; R(A+3) is the actual loop variable (the external index) that is local to the for block.

``FORPREP`` sets up a for loop. Since ``FORLOOP`` is used for initial testing of the loop condition as well as conditional testing during the loop itself, ``FORPREP`` performs a negative step and jumps unconditionally to ``FORLOOP`` so that ``FORLOOP`` is able to correctly make the initial loop test. After this initial test, ``FORLOOP`` performs a loop step as usual, restoring the initial value of the loop index so that the first iteration can start.

In ``FORLOOP``, a jump is made back to the start of the loop body if the limit has not been reached or exceeded. The sense of the comparison depends on whether the stepping is negative or positive, hence the “<?=” operator. Jumps for both instructions are encoded as signed displacements in the ``sBx`` field. An empty loop has a ``FORLOOP`` ``sBx`` value of -1.

``FORLOOP`` also sets R(A+3), the external loop index that is local to the loop block. This is significant if the loop index is used as an upvalue (see below.) R(A), R(A+1) and R(A+2) are not visible to the programmer.

The loop variable ends with the last value before the limit is reached (unlike C) because it is not updated unless the jump is made. However, since loop variables are local to the loop itself, you should not be able to use it unless you cook up an implementation-specific hack.

Examples
--------
For the sake of efficiency, ``FORLOOP`` contains a lot of functionality, so when a loop iterates, only one instruction, ``FORLOOP``, is needed. Here is a simple example::

  f=load('local a = 0; for i = 1,100,5 do a = a + i end')

Generates::

  main <(string):0,0> (8 instructions at 000001E9F0DF52F0)
  0+ params, 5 slots, 1 upvalue, 5 locals, 4 constants, 0 functions
        1       [1]     LOADK           0 -1    ; 0
        2       [1]     LOADK           1 -2    ; 1
        3       [1]     LOADK           2 -3    ; 100
        4       [1]     LOADK           3 -4    ; 5
        5       [1]     FORPREP         1 1     ; to 7
        6       [1]     ADD             0 0 4
        7       [1]     FORLOOP         1 -2    ; to 6
        8       [1]     RETURN          0 1
  constants (4) for 000001E9F0DF52F0:
        1       0
        2       1
        3       100
        4       5
  locals (5) for 000001E9F0DF52F0:
        0       a       2       9
        1       (for index)     5       8
        2       (for limit)     5       8
        3       (for step)      5       8
        4       i       6       7
  upvalues (1) for 000001E9F0DF52F0:
        0       _ENV    1       0

In the above example, notice that the ``for`` loop causes three additional local pseudo-variables (or internal variables) to be defined, apart from the external loop index, ``i``. The three pseudovariables, named ``(for index)``, ``(for limit)`` and ``(for step)`` are required to completely specify the state of the loop, and are not visible to Lua source code. They are arranged in consecutive registers, with the external loop index given by R(A+3) or register 4 in the example.

The loop body is in line [6] while line [7] is the ``FORLOOP`` instruction that steps through the loop state. The ``sBx`` field of ``FORLOOP`` is negative, as it always jumps back to the beginning of the loop body.

Lines [2]–[4] initialize the three register locations where the loop state will be stored. If the loop step is not specified in the Lua source, a constant 1 is added to the constant pool and a ``LOADK`` instruction is used to initialize the pseudo-variable ``(for step)`` with the loop step.

``FORPREP`` in lines [5] makes a negative loop step and jumps to line [7] for the initial test. In the example, at line [5], the internal loop index (at register 1) will be (1-5) or -4. When the virtual machine arrives at the ``FORLOOP`` in line [7] for the first time, one loop step is made prior to the first test, so the initial value that is actually tested against the limit is (-4+5) or 1. Since 1 < 100, an iteration will be performed. The external loop index ``i`` is then set to 1 and a jump is made to line [6], thus starting the first iteration of the loop.

The loop at line [6]–[7] repeats until the internal loop index exceeds the loop limit of 100. The conditional jump is not taken when that occurs and the loop ends. Beyond the scope of the loop body, the loop state (``(for index)``, ``(for limit)``, ``(for step)`` and ``i``) is not valid. This is determined by the parser and code generator. The range of PC values for which the loop state variables are valid is located in the locals list. 

Here is another example::

  f=load('for i = 10,1,-1 do if i == 5 then break end end')

This leads to::

  main <(string):0,0> (8 instructions at 000001E9F0DEC110)
  0+ params, 4 slots, 1 upvalue, 4 locals, 4 constants, 0 functions
        1       [1]     LOADK           0 -1    ; 10
        2       [1]     LOADK           1 -2    ; 1
        3       [1]     LOADK           2 -3    ; -1
        4       [1]     FORPREP         0 2     ; to 7
        5       [1]     EQ              1 3 -4  ; - 5
        6       [1]     JMP             0 1     ; to 8
        7       [1]     FORLOOP         0 -3    ; to 5
        8       [1]     RETURN          0 1
  constants (4) for 000001E9F0DEC110:
        1       10
        2       1
        3       -1
        4       5
  locals (4) for 000001E9F0DEC110:
        0       (for index)     4       8
        1       (for limit)     4       8
        2       (for step)      4       8
        3       i       5       7
  upvalues (1) for 000001E9F0DEC110:
        0       _ENV    1       0

In the second loop example above, except for a negative loop step size, the structure of the loop is identical. The body of the loop is from line [5] to line [7]. Since no additional stacks or states are used, a break translates simply to a ``JMP`` instruction (line [6]). There is nothing to clean up after a ``FORLOOP`` ends or after a ``JMP`` to exit a loop.


'``OP_TFORCALL``' and '``OP_TFORLOOP``' instructions
====================================================

Syntax
------
::

  TFORCALL    A C        R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2))
  TFORLOOP    A sBx      if R(A+1) ~= nil then { R(A)=R(A+1); pc += sBx }

Description
-----------
Apart from a numeric ``for`` loop (implemented by ``FORPREP`` and ``FORLOOP``), Lua has a generic ``for`` loop, implemented by ``TFORCALL`` and ``TFORLOOP``.

The generic ``for`` loop keeps 3 items in consecutive register locations to keep track of things. R(A) is the iterator function, which is called once per loop. R(A+1) is the state, and R(A+2) is the control variable. At the start, R(A+2) has an initial value. R(A), R(A+1) and R(A+2) are internal to the loop and cannot be accessed by the programmer.

In addition to these internal loop variables, the programmer specifies one or more loop variables that are external and visible to the programmer. These loop variables reside at locations R(A+3) onwards, and their count is specified in operand C. Operand C must be at least 1. They are also local to the loop body, like the external loop index in a numerical for loop.

Each time ``TFORCALL`` executes, the iterator function referenced by R(A) is called with two arguments: the state and the control variable (R(A+1) and R(A+2)). The results are returned in the local loop variables, from R(A+3) onwards, up to R(A+2+C).

Next, the ``TFORLOOP`` instruction tests the first return value, R(A+3). If it is nil, the iterator loop is at an end, and the ``for`` loop block ends by simply moving to the next instruction.

If R(A+3) is not nil, there is another iteration, and R(A+3) is assigned as the new value of the control variable, R(A+2). Then the ``TFORLOOP`` instruction sends execution back to the beginning of the loop (the ``sBx`` operand specifies how many instructions to move to get to the start of the loop body). 


Examples
--------
This example has a loop with one additional result (``v``) in addition to the loop enumerator (``i``)::

  f=load('for i,v in pairs(t) do print(i,v) end')

This produces::

  main <(string):0,0> (11 instructions at 0000014DB7FD2610)
  0+ params, 8 slots, 1 upvalue, 5 locals, 3 constants, 0 functions
        1       [1]     GETTABUP        0 0 -1  ; _ENV "pairs"
        2       [1]     GETTABUP        1 0 -2  ; _ENV "t"
        3       [1]     CALL            0 2 4
        4       [1]     JMP             0 4     ; to 9
        5       [1]     GETTABUP        5 0 -3  ; _ENV "print"
        6       [1]     MOVE            6 3
        7       [1]     MOVE            7 4
        8       [1]     CALL            5 3 1
        9       [1]     TFORCALL        0 2
        10      [1]     TFORLOOP        2 -6    ; to 5
        11      [1]     RETURN          0 1
  constants (3) for 0000014DB7FD2610:
        1       "pairs"
        2       "t"
        3       "print"
  locals (5) for 0000014DB7FD2610:
        0       (for generator) 4       11
        1       (for state)     4       11
        2       (for control)   4       11
        3       i       5       9
        4       v       5       9
  upvalues (1) for 0000014DB7FD2610:
        0       _ENV    1       0


The iterator function is located in register 0, and is named ``(for generator)`` for debugging purposes. The state is in register 1, and has the name ``(for state)``. The control variable, ``(for control)``, is contained in register 2. These correspond to locals R(A), R(A+1) and R(A+2) in the ``TFORCALL`` description. Results from the iterator function call is placed into register 3 and 4, which are locals ``i`` and ``v``, respectively. On line [9], the operand C of ``TFORCALL`` is 2, corresponding to two iterator variables (``i`` and ``v``).

Lines [1]–[3] prepares the iterator state. Note that the call to the ``pairs()`` standard library function has 1 parameter and 3 results. After the call in line [3], register 0 is the iterator function (which by default is the Lua function ``next()`` unless ``__pairs`` meta method has been overriden), register 1 is the loop state, register 2 is the initial value of the control variable (which is ``nil`` in the default case). The iterator variables ``i`` and ``v`` are both invalid at the moment, because we have not entered the loop yet.

Line [4] is a ``JMP`` to ``TFORCALL`` on line [9]. The ``TFORCALL`` instruction calls the iterator function, generating the first set of enumeration results in locals ``i`` and ``v``. 

The ``TFORLOOP`` insruction executes and checks whether ``i`` is ``nil``. If it is not ``nil``, then the internal control variable (register 2) is set to the value in ``i`` and control goes back to to the start of the loop body (lines [5]–[8]).

The body of the generic ``for`` loop executes (``print(i,v)``) and then ``TFORCALL`` is encountered again, calling the iterator function to get the next iteration state. Finally, when the ``TFORLOOP`` finds that the first result from the iterator is ``nil``, the loop ends, and execution continues on line [11].

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

  > function y() return x('foo', 'bar') end
  > ravi.dumplua(y)

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
==========================
Lua 5.3 Bytecode Reference
==========================

This is my attempt to bring up date the Lua bytecode reference.
Following copyrights are acknowledged:

:: 

  A No-Frills Introduction to Lua 5.1 VM Instructions
    by Kein-Hong Man, esq. <khman AT users.sf.net>
    Version 0.1, 2006-03-13


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


'``OP_CALL``' instruction
=========================

Syntax
------

::

  CALL A B C    R(A), ... ,R(A+C-2) := R(A)(R(A+1), ... ,R(A+B-1))

Performs a function call, with register R(A) holding the reference to the function object to be called. Parameters to the function are placed in the registers following R(A). If B is 1, the function has no parameters. If B is 2 or more, there are (B-1) parameters. If B >= 2, then upon entry to the called function, R(A+1) will become the ``base``. 

If B is 0, the function parameters range from R(A+1) to the top of the stack. This form is used when the 
number of parameters to pass is set by the previous VM instruction, which has to be one of '``OP_CALL``' or
'``OP_VARARG``'. 

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


'``OP_RETURN``' instruction
===========================

Syntax
------

::

  RETURN  A B return R(A), ... ,R(A+B-2)

Returns to the calling function, with optional return values. 

First RETURN closes any open upvalues by calling `luaF_close() <http://www.lua.org/source/5.3/lfunc.c.html#luaF_close>`_.

If B is 1, there are no return values. If B is 2 or more, there are (B-1) return values, located in consecutive registers from R(A) onwards. If B is 0, the set of values range from R(A) to the top of the stack. 

It is assumed that if the VM is returning to a Lua function then it is within the same invocation of the ``luaV_execute()``. Else it is assumed that ``luaV_execute()`` is being invoked from a C function.

If B is 0 then the previous instruction (which must be either '``OP_CALL``' or '``OP_VARARG``' ) would have set ``L->top`` to indicate how many values to return. The number of values to be returned in this case is R(A) to L->top. 

If B > 0 then the number of values to be returned is simply B-1.

'``OP_RETURN``' calls `luaD_poscall() <http://www.lua.org/source/5.3/ldo.c.html#luaD_poscall>`_ which is responsible for copying return values to the caller - the first result is placed at the current ``closure``'s address. ``luaD_poscall()`` leaves ``L->top`` just past the last result that was copied.

If '``OP_RETURN``' is returning to a Lua function and if the number of return values expected was indeterminate - i.e. '``OP_CALL``' had operand C = 0, then ``L->top`` is left where ``luaD_poscall()`` placed it - just beyond the top of the result list. This allows the '``OP_CALL``' instruction to figure out how many results were returned. If however '``OP_CALL``' had invoked with a value of C > 0 then the expected number of results is known, and in that case, ``L->top`` is reset to  the calling function's ``C->top``.

If ``luaV_execute()`` was called externally then '``OP_RETURN``' leaves ``L->top`` unchanged - so it will continue to be just past the top of the results list. This is because luaV_execute() does not have a way of informing callers how many values were returned; so the caller can determine the number of results by inspecting ``L->top``.

Example of 'VARARG' followed by 'RETURN'::

  function x(...) return ... end

  1 [1]  VARARG          0 0
  2 [1]  RETURN          0 0

Now suppose we call ``x(1,2,3)`` then observe the setting of ``L->top`` when '``RETURN``' executes::

  (LOADK A=1 Bx=-2)      L->top = 4, ci->top = 4
  (LOADK A=2 Bx=-3)      L->top = 4, ci->top = 4
  (LOADK A=3 Bx=-4)      L->top = 4, ci->top = 4
  (TAILCALL A=0 B=4 C=0) L->top = 4, ci->top = 4
  (VARARG A=0 B=0)       L->top = 2, ci->top = 2  ; we are in x()
  (RETURN A=0 B=0)       L->top = 3, ci->top = 2

Observe that '``VARARG``' set ``L->top`` to ``base+3``.

But if we call ``x(1)`` instead::

  (LOADK A=1 Bx=-2)      L->top = 4, ci->top = 4
  (LOADK A=2 Bx=-3)      L->top = 4, ci->top = 4
  (LOADK A=3 Bx=-4)      L->top = 4, ci->top = 4
  (TAILCALL A=0 B=4 C=0) L->top = 4, ci->top = 4
  (VARARG A=0 B=0)       L->top = 2, ci->top = 2 ; we are in x()
  (RETURN A=0 B=0)       L->top = 1, ci->top = 2

Notice that this time '``VARARG``' set ``L->top`` to ``base+1``.
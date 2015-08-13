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
                                        
Results returned by the function call is placed in a range of registers starting from R(A). If C is 1, no return results are saved. If C is 2 or more, (C-1) return values are saved. If C is 0, then multiple return results are saved. In this case the number of values to save is determined by one of following ways:

* A C function returns an integer value indicating number of results returned so for C function calls
  this is used (see the value of ``n`` passed to `luaD_poscall() <http://www.lua.org/source/5.3/ldo.c.html#luaD_poscall>`_ in `luaD_precall() <http://www.lua.org/source/5.3/ldo.c.html#luaD_precall>`_)
* For Lua functions, the the results are saved by the called function's '``OP_RETURN``' instruction.





==========================
Lua 5.3 Bytecode Reference
==========================

This is my attempt to bring up date the Lua bytecode reference.
Following copyrights are acknowledged:

:: 

  A No-Frills Introduction to Lua 5.1 VM Instructions
    by Kein-Hong Man, esq. <khman AT users.sf.net>
    Version 0.1, 2006-03-13


'``OP_CALL``' instruction
=========================

Syntax
------

::

  CALL A B C    R(A), ... ,R(A+C-2) := R(A)(R(A+1), ... ,R(A+B-1))

Performs a function call, with register R(A) holding the reference to the function object to be called. Parameters to the function are placed in the registers following R(A). If B is 1, the function has no parameters. If B is 2 or more, there are (B-1) parameters. If B > 0 then upon entry to the function, R(A+1) will be the ``base``. 

If B is 0, the function parameters range from R(A+1) to the top of the stack. This form is used when the last expression in the parameter list is a function call, so the number of actual parameters is indeterminate. In this case, upon entry to the function, ``base`` is set to the register beyond the vararg section, which means that the varargs sit between the R(A) and ``base``.

Thus upon entry to a function ``base`` is always the location of the first fixed parameter if any or else ``local`` if any.

Results returned by the function call is placed in a range of registers starting from R(A). If C is 1, no return results are saved. If C is 2 or more, (C-1) return values are saved. If C is 0, then multiple return results are saved, depending on the called function.

CALL always updates the top of stack value. CALL, RETURN, VARARG and SETLIST can use multiple values (up to the top of the stack.)



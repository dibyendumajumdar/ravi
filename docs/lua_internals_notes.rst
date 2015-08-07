===================
Lua Internals Notes
===================


These are rough notes - once something useful is identified it will be documented elsewhere.

``L->top`` and ``L->ci->top``
-----------------------------

``L->top``
  Tracks the top of the stack but has another purpose as explained by Roberto:

    It is not only L->top that has a dual purpose; the stack has a dual
    usage, due to the way that Lua uses a register-based VM. While inside
    a Lua function, Lua preallocates in the stack all "registers" that the
    function will eventually need. So, while running VM instructions in
    one function, L->top points to the end of its activation record (stack
    frame). When Lua enters or re-enters a Lua function, L->top must be
    corrected to this frame, except when multiple returns are involved.
    In that case, it is true that L->top is used to pass information
    strictly between two consecutive VM instructions. (From either
    OP_CALL/OP_VARARG to one of OP_CALL/OP_RETURN/OP_SETLIST.)


OP_CALL
-------------------------
OP_CALL is assumed to only be used for Lua calling Lua - when a function
is called via OP_CALL, then OP_RETURN must reset L->top to L->ci->top 
if posD_call() returned non-zero value.



===================
Lua Internals Notes
===================


These are rough notes - once something useful is identified it will be documented elsewhere.

``L->top`` and ``L->ci->top``
-----------------------------

``L->top``
  Appears to track the stack position where the next argument will be pushed (i.e. stack top).


How is ``L->top`` updated
-------------------------

``api_incr_top``
  Macro defined in ``lapi.h`` - increments L->top, and checks that it doesn't go beyond ``L->ci->top`` (stack overflow). So then this indicates that ``L->top`` cannot go beyond ``L->ci->top``.

  Primarily used in ``lapi.c`` but also in ``ldo.c``, ``ldebug.c`` and ``lstate.c``.



Lua calls Lua via OP_CALL
  When this happens, the OP_RETURN must reset L->top to L->ci->top if posD_call() returned non-zero value.

  This can only happen when OP_CALL is involved and the target is a Lua function. 
  Since an interpreted Lua function will be executed via luaV_execute() which doesn't reset L->top for external calls, we need to handle this.
  If the Lua function was JITed then we also need to reset.

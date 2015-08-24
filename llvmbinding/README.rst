LLVM Bindings for Lua/Ravi
==========================

As part of the Ravi Programming Language, it is my intention to provide a Lua 5.3 compatible LLVM binding.
This will allow Lua programmers to write their own JIT compilers in Lua!

Right now this is in early development so there is no documentation. But the Lua programs here
demonstrate the features available to date.

LLVM Modules and Execution Engines
----------------------------------
One of the complexities of LLVM is the handling of modules and execution engines in a JIT environment. In Ravi I made the simple decision that each
Lua function would get its own module and EE. This allows the function to be
garbage collected as normal and release the associated module and EE. One of 
the things that is possible but not yet implemented is releasing the module 
and EE early; this requires implementing a custom memory manager (issue #48).

To mimic the Ravi model, the LLVM bindings provide a shortcut to setup 
an LLVM module and execution engine for a Lua C function. The following example
illustrates::

  -- Get the LLVM context - right now this is the
  -- global context
  local context = llvm.context()

  -- Create a lua_CFunction instance
  -- At this stage the function will get a module and 
  -- execution engine but no body
  local mainfunc = context:lua_CFunction("demo")

Above creates an ``lvvm::Function`` instance within a new module. An EE is 
automatically attached. You can get hold of the module as shown below::

  -- Get hold of the module
  -- as we will create the fib function as an 
  -- iternal function
  local module = mainfunc:module()

Other native functions may be created within the same module as normal. However
note that once the Lua function is compiled then no further updates to the 
module are possible.

The model I recommend when using this feature is to create one exported
Lua C function in the module, with several private 'internal' supporting functions within the module.


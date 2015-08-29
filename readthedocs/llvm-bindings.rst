LLVM Bindings for Lua/Ravi
==========================

As part of the Ravi Programming Language, it is my intention to provide a Lua 5.3 compatible LLVM binding.
This will allow Lua programmers to write their own JIT compilers in Lua!

Right now this is in early development so there is no documentation. But the Lua programs here
demonstrate the features available to date.

LLVM Modules and Execution Engines
----------------------------------
One of the complexities of LLVM is the handling of modules and execution engines in a JIT environment. In Ravi I made the simple decision that each Lua function would get its own module and EE. This allows the function to be
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
  local module = mainfunc:module()

Other native functions may be created within the same module as normal. However
note that once the Lua function is compiled then no further updates to the 
module are possible.

The model I recommend when using this feature is to create one exported
Lua C function in the module, with several private 'internal' supporting functions within the module.

Creating Modules and Execution Engines
--------------------------------------
The LLVM api for these functions are not exposed yet. 

Type Hierarchy
--------------
The bindings provide a number of Lua types::

  + LLVMcontext
  + LLVMfunction
    + LLVMmainfunction
  + LLVMmodule
  + LLVMtype
    + LLVMstructtype
    + LLVMpointertype
    + LLVMfunctiontype
  + LLVMvalue
    + LLVMinstruction
    + LLVMconstant
  + LLVMirbuilder
  + LLVMbasicblock  


Available Bindings
------------------
The following table lists the Lua LLVM api functions available.

+----------------------------------------------------------------------------------------------+
| Lua LLVM API                                                                                 |
+==============================================================================================+
| ``llvm.context() -> LLVMcontext``                                                            |
|   Returns global llvm::Context                                                               |
+----------------------------------------------------------------------------------------------+
| **LLVMcontext methods**                                                                      |
+----------------------------------------------------------------------------------------------+
| ``lua_CFunction(name) -> LLVMmainfunction``                                                  |
|   Creates an llvm::Function within a new llvm::Module; and associates an                     |
|   llvm::ExecutionEngine with the module                                                      |
| ``types() -> table of predefined type bindings``                                             |
|   Returns a table of predefined LLVM type bindings                                           |
| ``structtype(name) -> LLVMstructtype``                                                       |
|   Opaque struct type; body can be added                                                      |
| ``pointertype(type) -> LLVMpointertype``                                                     | 
|   Given a type returns a pointertype                                                         |
| ``functiontype(return_type, {argtypes}, {options}) -> LLVMfunctiontype``                     |
|   Creates a function type with specified return type, argument types. Takes the option       |
|   'vararg' which is false by default.                                                        |
| ``basicblock(name) -> LLVMbasicblock``                                                       | 
|   Create a basic block                                                                       |
| ``intconstant(intgervalue) -> LLVMvalue``                                                    | 
|   Returns an integer constant value                                                          |
| ``nullconstant(pointertype) -> LLVMvalue``                                                   | 
|   Returns a NULL constant of specified pointertype                                           |
+----------------------------------------------------------------------------------------------+
| **LLVMstructtype methods**                                                                   |
+----------------------------------------------------------------------------------------------+
| ``setbody({types})``                                                                         | 
|   Adds members to the struct type                                                            |
+----------------------------------------------------------------------------------------------+
| **LLVMmainfunction methods**                                                                 |
+----------------------------------------------------------------------------------------------+
| ``appendblock(LLVMbasicblock)``                                                              | 
|   Adds a basic block to the end                                                              |
| ``compile()``                                                                                | 
|   Compiles the module and returns a reference to the C Closure                               |
| ``arg(position) -> LLVMvalue``                                                               | 
|   Returns the argument at position; position >= 1; returns ``nil`` if argument not available |
| ``module() -> LLVMmodule``                                                                   | 
|   Returns the module associated with the function                                            |
| ``extern(name[, functiontype]) -> LLVMconstant``                                             | 
|   Returns an extern declaration; A number of Lua Api functions are predefined.               |
+----------------------------------------------------------------------------------------------+
| **LLVMmodule methods**                                                                       |
+----------------------------------------------------------------------------------------------+
| ``newfunction(name, functiontype) -> LLVMfunction``                                          | 
|   Returns an internal linkage function within the module                                     |
| ``dump()``                                                                                   | 
|   Dumps the module                                                                           |
+----------------------------------------------------------------------------------------------+
| **LLVMfunction methods**                                                                     |
+----------------------------------------------------------------------------------------------+
| ``appendblock(LLVMbasicblock)``                                                              | 
|   Adds a basic block to the end                                                              |
| ``arg(position) -> LLVMvalue``                                                               | 
|   Returns the argument at position; position >= 1; returns ``nil`` if argument not available |
+----------------------------------------------------------------------------------------------+
| **LLVMirbuilder methods**                                                                    |
+----------------------------------------------------------------------------------------------+
| ``setinsertpoint(basicblock)``                                                               |
|   Set current basicblock                                                                     |
| ``ret([value])``                                                                             |
|   Emit return instruction                                                                    |
| ``stringconstant(string) -> LLVMvalue``                                                      |
|   Create a global string constant                                                            |
| ``call({args}, {options}) -> LLVMinstruction``                                               |
|   Emit call instruction; 'tailcall' option is false by default                               | 
| ``br(basicblock) -> LLVMinstruction``                                                        |
|   Emit a branch instruction                                                                  |
| ``condbr(vavlue, true_block, false_block) -> LLVMinstruction``                               |
|   Emit a conditional branch                                                                  |
|                                                                                              |
| Binary Operators                                                                             |
| * ``icmpeq``                                                                                 |
| * ``icmpne``                                                                                 |
| * ``icmpugt``                                                                                |
| * ``icmpuge``                                                                                |
| * ``icmpult``                                                                                |
| * ``icmpule``                                                                                |
| * ``icmpsgt``                                                                                |
| * ``icmpsge``                                                                                |
| * ``icmpslt``                                                                                |
| * ``icmpsle``                                                                                |
| * ``fcmpoeq``                                                                                |
| * ``fcmpogt``                                                                                |
| * ``fcmpoge``                                                                                |
| * ``fcmpolt``                                                                                |
| * ``fcmpole``                                                                                |
| * ``fcmpone``                                                                                |
| * ``fcmpord``                                                                                |
| * ``fcmpun``                                                                                 |
| * ``fcmpueq``                                                                                |
| * ``fcmpugt``                                                                                |
| * ``fcmpuge``                                                                                |
| * ``fcmpult``                                                                                |
| * ``fcmpule``                                                                                |
| * ``fcmpune``                                                                                |
| * ``nswadd``                                                                                 |
| * ``nuwadd``                                                                                 |
| * ``nswsub``                                                                                 |
| * ``nuwsub``                                                                                 |
| * ``udiv``                                                                                   |
| * ``exactudiv``                                                                              |
| * ``sdiv``                                                                                   |
| * ``exactsdiv``                                                                              |
| * ``neg``                                                                                    |
| * ``urem``                                                                                   |
| * ``srem``                                                                                   |
| * ``and``                                                                                    |
| * ``or``                                                                                     |
| * ``xor``                                                                                    |
| * ``fadd``                                                                                   |
| * ``fsub``                                                                                   |
| * ``fmul``                                                                                   |
| * ``fdiv``                                                                                   |
| * ``frem``                                                                                   |
|                                                                                              |
| Unary Operators                                                                              |
| * ``not``                                                                                    |
| * ``fneg``                                                                                   |
|                                                                                              |
| GEP Operators                                                                                |
| * ``gep``                                                                                    |
| * ``inboundsgep``                                                                            |
|                                                                                              |
| Conversion Operators                                                                         |
| * ``trunc``                                                                                  |
| * ``zext``                                                                                   |
| * ``sext``                                                                                   |
| * ``zextortrunc``                                                                            |
| * ``sextortrunc``                                                                            |
| * ``fptoui``                                                                                 |
| * ``fptosi``                                                                                 |
| * ``uitofp``                                                                                 |
| * ``sitofp``                                                                                 |
| * ``fptrunc``                                                                                |
| * ``fpext``                                                                                  |
| * ``ptrtoint``                                                                               |
| * ``inttoptr``                                                                               |
| * ``bitcast``                                                                                |
| * ``sextorbitcast``                                                                          |
| * ``zextorbitcast``                                                                          |
| * ``truncorbitcast``                                                                         |
| * ``pointercast``                                                                            |
| * ``fpcast``                                                                                 |
|                                                                                              |
| Memory Operators                                                                             |
| * ``alloca``                                                                                 |
| * ``load``                                                                                   |
| * ``store``                                                                                  |
+----------------------------------------------------------------------------------------------+

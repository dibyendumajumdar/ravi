Ravi Programming Language
=========================

Experimental derivative/dialect of Lua. Ravi is a Sanskrit word that means the Sun.

Lua is perfect as a small embeddable dynamic language. So why a derivative? The reason is primarily to extend Lua with static typing for greater performance. However, at the same time maintain full compatibility with standard Lua.

There are other attempts to add static typing to Lua (e.g. [Typed Lua](https://github.com/andremm/typedlua>)) but these efforts are mostly about adding static type checks in the language while leaving the VM unmodified. So the static typing is to aid programming in the large - the code is eventually translated to standard Lua and executed in the unmodified Lua VM.

My motivation is somewhat different - I want to enhance the VM to support more efficient operations when types are known. 

Goals
-----
* Optional static typing for Lua 
* No new types
* Type specific bytecodes to improve performance
* Full backward compatibility with Lua 5.3

Status
------
The project was kicked off in January 2015. My intention is start small and grow incrementally.

As of now (end Jan 2015) you can declare local variables as `int` or `double`. This triggers following behaviour:

* `int` and `double` variables are initialized to 0
* arithmetic operations trigger type specific bytecodes
* values assigned to these variables are checked - statically unless the values are results from a function call in which case the there is an attempt to convert values at runtime.

Obviously this is early days so expect bugs.

Example of code that works - you can copy this to the command line input:
```lua
local function tryme(); local i,j = 5,6; return i,j; end; local i:int, j:int = tryme(); return i+j
```
Another:
```lua
local j:double; for i=1,1000000000 do; j = j+1; end; return j
```

The build is CMake based. I am testing this using Visual Studio 2013 on Windows 8.1 64bit and gcc on Unbuntu 64-bit.

To build on Windows I use:
```
cd build
cmake -G "Visual Studio 12 Win64" ..
```
I then open the solution in VS2013 and do a build from there.

On Ubuntu I use:
```
cd build
cmake -G "Unix Makefiles" ..
make
```

The `lua` command recognizes following environment variables.

* `RAVI_DEBUG_EXPR` - if set to a value this triggers debug output of expression parsing
* `RAVI_DEBUG_CODEGEN` - if set to a value this triggers a dump of the code being generated
* `RAVI_DEBUG_VARS` - if set this triggers a dump of local variables construction and destruction

Work Plan
---------
* Feb 2015 - implement type specialisation for arrays 
* Mar 2015 - implement function parameter / return type specialisation

License
-------
Same as Lua.

Language Syntax
---------------
I hope to enhance the language to variables to be optionally decorated with types. As the reason for doing so is performance primarily - not all types benefit from this capability. In fact it is quite hard to extend this to generic recursive structures such as tables without encurring significant overhead. For instance - even to represent a recursive type in the parser will require dynamic memory allocation and add great overhead to the parser.

So as of now the only types that seem worth specializing are:

* int (64-bit)
* double
* array of ints
* array of doubles

Everything else will just be dynamic type as in Lua. However we can recognise following types to make the language more user friendly:

* string
* table 
* function
* nil
* boolean

And we may end up allowing additionally following types depending on whether they help our goals:

* array of booleans
* array of strings
* array of functions

The syntax for introducing the type will probably be as below:
```
function foo(s: string) : string
  return s
end
```

Local variables may be given types as shown below:
```
function foo() : string
  local s: string = "hello world!"
  return s
end
```

If no type is specified then then type will be dynamic - exactly what the Lua default is.

When a typed function is called the inputs and return value can be validated. Consider the function below:

```
local function foo(a, b: int, c: string)
  return
end
```
When this function is called the compiler can validate that `b` is an int and `c` is a string. `a` on the other hand is dynamic so will behave as regular Lua value. The compiler can also ensure that the types of `b` and `c` are respected within the function. 

Return statements in typed functions can also be validated.

Array Types
-----------

When it comes to complex types such as arrays, tables and functions, at this point in time, I think that Ravi only needs to support explicit specialization for arrays of integers and doubles. 

```
function foo(p1: {}, p2: int[])
  -- p1 is a table
  -- p2 is an array of integers
  local t1 = {} -- t1 is a table
  local a1 : int[] = {} -- a1 is an array of integers, specialization of table
  local d1 : double[] = {} -- d1 is an array of doubles, specialization of table
end
```

To support array types we need a mix of runtime and compile time type checking. The Lua table type will be enhanced to have type information so that when an array type is created the type of the array will be recorded. This will allow the runtime to detect incorrect usage of array type and raise errors if necessary. However, on the other hand, it will be possible to pass the array type to an existing Lua function as a regular table - and as long as the Lua function does not attempt to subvert the array type it should work as normal.

The array types will have some special behaviour:

* indices must be >= 1
* array will grow automatically if user sets the element just past the array length
* it will be an error to attempt to set an element that is beyond len+1 
* the current used length of the array will be recorded and returned by len operations
* the array will only permit the right type of value to be assigned (this will be checked at runtime to allow full compatibility with Lua)
* accessing out of bounds elements will cause an error, except for setting the len+1 element
* it will be possible to pass arrays to functions and return arrays from functions - the array types will be checked at runtime
* it should be possible to store an array type in a table - however any operations on array type can only be optimised to special bytecode if the array type is a local variable. Otherwise regular table access will be used subject to runtime checks. 
* array types may not have meta methods - this will be enforced at runtime

All type checks are at runtime
------------------------------
To keep with Lua's dynamic nature I plan a mix of compile type checking and runtime type checks. However due to the dynamic nature of Lua, compilation happens at runtime anyway so effectually all checks are at runtime.

Implementation Strategy
-----------------------
I do not want to introduce any new types to the Lua system as the types I need already exist and I quite like the minimalist nature of Lua. However, to make the execution efficient I want to approach this by adding new type specific opcodes, and by enhancing the Lua parser/code generator to encode these opcodes only when types are known. The new opcodes will execute more efficiently as they will not need to perform type checks. In reality the performance gain may be offset by the increase in the instruction decoding / branching - so it remains to be seen whether this approach is beneficial. However, I am hoping that type specific instructions will lend themselves to more efficient JIT at a later stage.

My plan is to add new opcodes that cover arithmetic operations, array operations, variable assignments, etc..

I will probably need to augment some existing types such as functions and tables to add the type signature.

I intend to first add the opcodes to the VM before starting work on the parser and code generator.

Modifications to Lua Bytecode structure
---------------------------------------
An immediate issue is that the Lua bytecode structure has a 6-bit opcode which is insufficient to hold the various opcodes that I will need. Simply extending the size of this is problematic as then it reduces the space available to the operands A B and C. Furthermore the way Lua bytecodes work means that B and C operands must be 1-bit larger than A - as the extra bit is used to flag whether the operand refers to a constant or a register. (Thanks to Dirk Laurie for pointing this out). 

If I change the sizes of the components it will make the new bytecode incompatible with Lua. Although this doesn't matter so much as long as source level compatibility is retained - I would like a solution that allows me to maintain full compatibility at bytecode level. An obvious solution is to allow extended 64-bit instructions - while retaining the existing 32-bit instructions.  

For now however I am just amending the bit mapping in the 32-bit instruction to allow 9-bits for the byte-code, 7-bits for operand A, and 8-bits for operands B and C. This means that some of the Lua limits (maximum number of variables in a function, etc.) have to be revised to be lower than the default.

New OpCodes
-----------
The new instructions are specialised for types, and also for register/versus constant. So for example `OP_RAVI_ADDFIKK` means add `float` and `int` where both values are constants. And `OP_RAVI_ADDFFRR` means add `float` and `float` - both obtained from registers. The existing Lua opcodes that these are based on define which operands are used.

Example:
--------
```
> local i=0; i=i+1
```
Above standard Lua code compiles to:
```
[0] LOADK A=0 Bx=-1
[1] ADD A=0 B=0 C=-2
[2] RETURN A=0 B=1
```
We add type info using Ravi extensions:
```
> local i:int=0; i=i+1
```
Now the code compiles to:
```
[0] LOADK A=0 Bx=-1
[1] ADDIIRK A=0 B=0 C=-2
[2] RETURN A=0 B=1
```
Above uses type specialised opcode `OP_RAVI_ADDIIRK`. 

Documentation
-------------
As I progress I will add documentation in the Wiki.

* [Ravi Internals](https://github.com/dibyendumajumdar/ravi/wiki/RaviInternals)
* [Lua Internals](https://github.com/dibyendumajumdar/ravi/wiki/Lua-Internals)
* [Change Log](https://github.com/dibyendumajumdar/ravi/wiki/Changes)


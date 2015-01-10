Ravi Programming Language
=========================

Experimental derivative of Lua. Ravi is a Sanskrit word that means the Sun.

Lua is perfect as a small embeddable dynamic language. So why a derivative? The reason is primarily to extend Lua with static typing for greater performance. However, at the same time I would like to retain compatibility with standard Lua.

There are various attempts to add static typing to Lua but these efforts are mostly about adding static type checks in the language while leaving the VM unmodified. So the static typing is to aid programming - the code is eventually translated to standard Lua and executed in the unmodified Lua VM.

My motivation is somewhat different - I want to enhance the VM to support more efficient operations when types are known. 

Status
------
The project was kicked off in January 2015. I expect it will be a while before there is any code that runs. However my intention is start small and grow incrementally.

For latest status see the Changes page in the Wiki.

License
-------
Will be same as Lua.

Language Syntax
---------------
I hope to enhance the language to enable static typing of following:
* int (64-bit)
* double
* string
* table 
* array (see below)
* bool 
* functions and closures

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

Tables and arrays need special syntax to denote the element / key types. The syntax might use the angle brackets similar to C++ template aruguments.

```
function foo() 
  local t1 = {} -- table<any,any>
  local t2 : table<string,string> = {} -- table with string keys and values
  local t3 : table<string,double> = {} -- table with string keys and double values
  local a1 : array<int> = {} -- array of integers
end
```

With regards to function types, full static typing at all times is difficult as then all function types have to be known in advance (either via forward declarations or by access to bytecodes). It seems to me that a pragmatic approach will be to perform run-time checking of function argument types. So for example:

```
-- array of functions
local func_table : array<function> = {
  function (s: string) : string 
    return s 
  end,
  function (i, j) 
    return i+j 
  end
}
```
Above the array of functions allows various function types - all it cares is that the element must be a functon.

When a typed function begins to execute the first step will be validate the input parameters against any explicit type specifications. Consider the function below:

```
local function foo(a, b: int, c: string)
  return
end
```
When this function starts executing it will validate that `b` is an int and `c` is a string. `a` on the other hand is dynamic so will behave as regular Lua value. The compiler will ensure that the types of `b` and `c` are respected within the function. So by a combination of runtime checking and compiler static typing a solution can be implemented that is not too disruptive.

Implementation Strategy
-----------------------
I do not want to any new types to the Lua system as the required types already exist. However, to make the execution efficient I want to approach this by adding new type specific opcodes, and by enhancing the Lua parser/code generator to encode these opcodes when types are known. The new opcodes will execute more efficiently as they will not need to perform type checks.

My plan is to add new opcodes that cover arithmetic operations, array operations and table operations (latter is lower priority).

I will probably need to augment some existing types such as functions and tables to add the type signature so that at runtime when a function is called it can perform typechecks if a function signature is available.

I intend to first add the opcodes to the VM before starting work on the parser and code generator.

Challenges with Lua Bytecode structure
--------------------------------------
An immediate issue is that the Lua bytecode structure has a 6-bit opcode which is insufficient to hold the various opcodes that I will need. Simply extending the size of this is problematic as then it reduces the space available to the operands A B and C. Furthermore the way Lua bytecodes work means that B and C operands must be 1-bit larger than A - as the extra bit is used to flag whether the operand refers to a constant or a register. (Thanks to Dirk Laurie for pointing this out). 

If I change the sizes of the components it will make the new bytecode incompatible with Lua. Although this doesn't matter so much as long as source level compatibility is retained - I would rather have a solution that allows me to maintain full compatibility at bytecode level. An obvious solution is to allow 64-bit instructions - while retaining the existing 32-bit instructions. So I investigated whether I could use a technique similar to OP_LOADKX. 

Unfortunately the way the Lua parser / byte-code generator works, it turns out that while expressions are being parsed, values may be held as bytecode instructions. So this makes it much more complex to implement the two instruction approach.

For now therefore I am just amending the bit mapping in the 32-bit instruction to allow 9-bits for the byte-code, 7-bits for operand A, and 8-bits for operands B and C.  

New OpCodes
-----------
The new instructions are specialised for types, and also for register/versus constant. So for example `OP_RAVI_ADDFIKK` means add `float` and `int` where both values are constants. And `OP_RAVI_ADDFFRR` means add `float` and `float` - both obtained from registers. The existing Lua opcodes that these are based on define which operands are used.





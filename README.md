Ravi
====

Experimental derivative of Lua. Ravi is a Sanskrit word that means the Sun.

Lua is perfect as a small embeddable dynamic language. So why a derivative? The reason is primarily to extend Lua with static typing for greater efficiency in performance. However, at the same time I would like to retain compatibility with Lua to the degree possible.

There are other attempts to add static typing to Lua but these efforts are mostly about adding static type checks in the language while leaving the VM unmodified. So the static typing is merely to aid programming - the code is eventually translated to Lua and executed in the VM.

My motivation is somewhat different - I want to enhance the VM to support more efficient operations when types are known. 

Status
------
The project is being kicked off in January 2015. I expect it will be a while before there is any code that runs. However my intention is start small and grow incrementally.

License
-------
Will be same as Lua.

Language Syntax
---------------
I hope to enhance the language to support following types:
* int (64-bit)
* double
* string
* table (see below)
* array (see below)
* any - this will be a dynamic type similar to existing Lua
* bool 

The syntax for introducting the type will probably be as below:
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

If no type is specified then then type will be `any` - however user cannot specify this - i.e. the lack of a type will imply this.

Tables and arrays need special syntax to denote the element / key types. The syntax might use the angle brackets similar to C++ template aruguments.

```
function foo() 
  local t1 = {} -- table<any,any>
  local t2 : table<string,string> = {} -- table with string keys and values
  local t3 : table<string,double> = {} -- table with string keys and double values
  local a1 : array<int> = {} -- array of integers
end
```

With regards to function types, full static typing at all times is difficult as then all function types have to be known in advance. It seems to me that a pragmatic approach will be to perform run-time checking of function argument types. So for example:

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

Implementation Strategy
-----------------------
I do not want to add actual types to the system as the required types already exist. However, to make the execution efficient I want to approach this by adding new type specific opcodes, and by enhancing the Lua parser/code generator to encode these opcodes when types are known. The new opcodes will execute more efficiently as they will not need to perform type checks.

My plan is to add new opcodes that cover arithmetic operations, array operations and table operations.

I will probably need to augment some existing types such a functions and tables to add the type signature so that at runtime when a function is called it can perform typechecks if a function signature is available.

I intend to first add the opcodes to the VM before starting work on the parser and code generator.

New OpCodes
-----------




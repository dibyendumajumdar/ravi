===================
Crash Course in Lua
===================

Introduction
============
Lua is a small, but powerful language that is implemented as a library in C. This crash course in meant to help you quickly become familiar with Lua. This guide assumes you know C or C++, or Java, and perhaps a scripting language like Python - it is not
a beginner's guide. This is also an opinionated introduction to Lua, and WIP and incomplete.

Key Features
============
* Lua is dynamically typed like Python
* By default variables in Lua are global unless declared local
* There is a single complex / aggregate type called a Table, which combines hash table/map and array features
* Functions in Lua are values stored in variables; in particular functions do not have names
* Globals in Lua are just values stored in a special Lua table 
* Functions in Lua are closures - they can capture variables from outer scope and such variables live on even though the surrounding scope is no longer alive
* Lua functions can return multiple values
* Lua has integer (since 5.3) and double types that map to native C types
* A Lua script is called a chunk - and is the unit of compilation in Lua
* Lua functions can be yielded from and resumed later on, i.e., Lua supports coroutines
* A special ``nil`` value represents non-existent value
* Lua's error handling is based on C setjmp/longjmp, and errors are caught via a special function call mechanism
* Lua has some nice syntactic sugar for tables and functions 
* Lua has a meta mechanism that enables a DIY class / object system with some syntactic sugar to make it look nice
* You can create user defined types in C and make them available in Lua
* Lua supports operator overloading via 'meta' methods
* Although Lua has a boolean type any value that is not 0 and not ``nil`` is considered true
* The result of logical ``and`` and logical ``or`` is not true or false; these operators select one of the values 
* The Lua stack is a heap allocated structure - and you can think of Lua as a library that manipulates this stack
* Lua compiles code to bytecode before execution
* Lua's compiler is designed to be fast and frugal - it generates code as it parses, there is no intermediate AST construction
* Like C, Lua comes with a very small standard library - in fact Lua's standard library is just a wrapper for C standard library
  plus some basic utilities for Lua
* Lua provides a debug API that can be used to manipulate Lua's internals to a degree - and can be used to implement a debugger
* Lua has an incremental garbage collector
* Lua is single threaded but its VM is small and encapsulated in a single data structure - hence each OS thread can be given its own 
  Lua VM
* Lua is Open Source but has a closed development model - external contributions are not possible
* Major Lua versions are not backward compatible
* LuaJIT is a JIT compiler for Lua but features an optional high performance C interface mechanism that makes it incompatible with Lua

In the rest of this document I will expand on each of the features above.

Lua is dynamically typed
========================
This means that values have types but variables do not. Example::

  x = 1 -- x holds an integer
  x = 5.0 -- x now holds a double
  x = {} -- x now holds an empty table
  x = function() end -- x now holds a function with empty body
  
Variables are global unless declared local
==========================================
In the example above, ``x`` is global. 
But saying::

  local x = 1 
  
makes ``x`` local, i.e. its scope and visiability is constrained to the enclosing block of code, and any nested blocks. Note that
local variables avoid a lookup in the 'global' table and hence are more efficient. Thus it is common practice to cache values in
local variable. For example, ``math.abs()`` is a function - and following creates a local variable that caches it::

  local abs = math.abs
  abs(5.5) -- invoked same function as math.abs
  
  local print = print -- caches global print() function
  print('hello world!') -- calls the same function as global print()

There is following exceptions to the rule:

* the iterator variables declared in a ``for`` loop are implicitly local.
* function parameters are implicitly local 

The Table type
==============
Lua's only complex / aggregate data type is a table. Tables are used for many things in Lua, even internally within Lua.
Here are some examples::

  local a = {} -- creates an empty table
  local b = {10,20,30} -- creates a table with three array elements at positions 1,2,3
                       -- short cut for local b = {}
                       --               b[1] = 10
                       --               b[2] = 20
                       --               b[3] = 30
  local c = { name='Ravi' } -- creates a table with one hash map entry
                            -- short cut for local c = {}
                            --                     c['name'] = 'Ravi'
                            

Internally the table is a composite hash table / array structure. Consecutive values starting at integer index 1 are inserted into the array, else the values go into the hash table. Hence, in the example below::

  local t = {}
  t[1] = 20 -- goes into array
  t[2] = 10 -- goes into array
  t[100] = 1 -- goes into hash table as not consecutive
  t.name = 'Ravi' -- goes into hash tabe

To iterate over array values you can write::

  for i = 1,#t do
    print(t[i])
  end
  
Note that above will only print 20,10.

To iterate over all values write::

  for k,v in pairs(t) do
    print(k,v)
  end
  
Unfortunately you need to get a good understanding of when values will go into the array part of a table, because some Lua library functions work only on the array part. Example::

  table.sort(t)
  
You will see that only values at indices 1 and 2 were sorted.
Another frequent problem is that the only way to reliably know the total number of elements in a table is to count the values. 
The ``#`` operator returns the length of the consecutive array elements starting at index 1.

Functions are values stored in variables
========================================
You already saw that we can write::

  local x = function() 
            end
  
This creates a function and stores in in local variable ``x``. Ths is the same as::

  local function x() 
  end
  
Omitting the ``local`` keyword would create ``x`` in global scope.

Functions can be defined within functions - in fact all Lua functions are defined within a 'chunk' of code, which gets wrapped into a Lua function.

Internally a function has a 'prototype' that holds the compiled code and other meta data regarding the function. An instance of the
function in created when the code executes. You can think of the 'prototype' as the 'class' of the function, and the function instance is akin to an object created from this class. 

Globals are just values in a special table
==========================================
Globals are handled in an interesting way. Whenever a name is used that is not found in any of the enclosing scopes and is not declared ``local``, then Lua will access/create a variable in a table called ``_ENV``. Actually this is just a captured value that points to a special table in Lua state by default. This table access becomes evident when you look at the bytecode generated for some Lua code::

  function hello()
    print('hello world')
  end

Generates::

  function <stdin:1,3> (4 instructions at 00000151C0AA9530)
  0 params, 2 slots, 1 upvalue, 0 locals, 2 constants, 0 functions
        1       [2]     GETTABUP        0 0 -1  ; _ENV "print"
        2       [2]     LOADK           1 -2    ; "hello world"
        3       [2]     CALL            0 2 1
        4       [3]     RETURN          0 1
  constants (2) for 00000151C0AA9530:
        1       "print"
        2       "hello world"
  locals (0) for 00000151C0AA9530:
  upvalues (1) for 00000151C0AA9530:
        0       _ENV    0       0

The ``GETTABUP`` instruction looks up the name 'print' in the captured variable ``_ENV``. Lua uses the term 'upvalue' for captured variables.

Functions in Lua are closures
=============================
Lua functions can reference variables in outer scopes - and such references can be captured by the function so that even if the outer scope does not exist anymore the variable still lives on::

  -- x() returns two anonymous functions
  x = function()
    local a = 1
    return  function(b)
              a = a+b
              return a
            end,
            function(b)
              a = a+b
              return a
            end
  end
    
  -- call x
  m,n = x()
  m(1) -- returns 2
  n(1) -- returns 3

In the example above, the local variable ``a`` in function ``x()`` is captured inside the two anonymous functions that reference it. You can see this if you dump the bytecode for ``m``::

  function <stdin:1,1> (6 instructions at 00000151C0AD3AB0)
  1 param, 2 slots, 1 upvalue, 1 local, 0 constants, 0 functions
        1       [1]     GETUPVAL        1 0     ; a
        2       [1]     ADD             1 1 0
        3       [1]     SETUPVAL        1 0     ; a
        4       [1]     GETUPVAL        1 0     ; a
        5       [1]     RETURN          1 2
        6       [1]     RETURN          0 1
  constants (0) for 00000151C0AD3AB0:
  locals (1) for 00000151C0AD3AB0:
        0       b       1       7
  upvalues (1) for 00000151C0AD3AB0:
        0       a       1       0
        
The ``GETUPVAL`` and ``SETUPVAL`` instructions access captured variables or upvalues as they are known in Lua.


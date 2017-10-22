===================
Introduction to Lua
===================

Introduction
============
`Lua <https://www.lua.org>`_ is a small but powerful interpreted language that is implemented as a C library. This guide is meant to help you quickly become familiar with the main features of Lua. This guide assumes you know C, C++, or Java, and perhaps a scripting language like Python - it is not a beginner's guide. Nor is it a tutorial for Lua. 

Key Features of Lua
===================
* Lua versions
* Lua is dynamically typed like Python
* By default variables in Lua are global unless declared local
* There is a single complex / aggregate type called a 'table', which combines hash table/map and array features
* Functions in Lua are values stored in variables; in particular functions do not have names
* Globals in Lua are just values stored in a special Lua table 
* Functions in Lua are closures - they can capture variables from outer scope and such variables live on even though the surrounding scope is no longer alive
* Lua functions can return multiple values
* Lua has integer (since 5.3) and floating point types that map to native C types
* A special ``nil`` value represents non-existent value
* Any value that is not ``false`` or ``nil`` is true
* The result of logical ``and`` and logical ``or`` is not true or false; these operators select one of the values 
* Lua has some nice syntactic sugar for tables and functions 
* A Lua script is called a chunk - and is the unit of compilation in Lua
* Lua functions can be yielded from and resumed later on, i.e., Lua supports coroutines
* Lua's error handling is based on C setjmp/longjmp, and errors are caught via a special function call mechanism
* Lua has a meta mechanism that enables a DIY class / object system with some syntactic sugar to make it look nice
* You can create user defined types in C and make them available in Lua
* Lua supports operator overloading via 'meta' methods
* The Lua stack is a heap allocated structure - and you can think of Lua as a library that manipulates this stack
* Lua compiles code to bytecode before execution
* Lua's compiler is designed to be fast and frugal - it generates code as it parses, there is no intermediate AST construction
* Like C, Lua comes with a very small standard library - in fact Lua's standard library is just a wrapper for C standard library
  plus some basic utilities for Lua
* Lua's standard library includes pattern matching for strings in which the patterns themselves are strings, rather like regular expressions in Python or Perl, but simpler.
* Lua provides a debug API that can be used to manipulate Lua's internals to a degree - and can be used to implement a debugger
* Lua has an incremental garbage collector
* Lua is single threaded but its VM is small and encapsulated in a single data structure - hence each OS thread can be given its own 
  Lua VM
* Lua is Open Source but has a closed development model - external contributions are not possible
* Major Lua versions are not backward compatible
* LuaJIT is a JIT compiler for Lua but features an optional high performance C interface mechanism that makes it incompatible with Lua

In the rest of this document I will expand on each of the features above.

Lua versions
============
For all practical purposes only Lua version 5.1, 5.2 and 5.3 matter. Note however that each of these is considered a major version and therefore is not backward compatible (e.g. Lua 5.3 cannot necessarily run Lua 5.1 code). 

* Lua 5.2 has a new mechanism for resolving undeclared variables compared to 5.1
* Lua 5.3 has integer subtypes and bitwise operators that did not exist in 5.1 or 5.2. 

Mostly what this document covers should be applicable to all these versions, except as otherwise noted.

Lua is dynamically typed
========================
This means that values have types but variables do not. Example::

  x = 1 -- x holds an integer
  x = 5.0 -- x now holds a floating pont value
  x = {} -- x now holds an empty table
  x = function() end -- x now holds a function with empty body
  
Variables are global unless declared local
==========================================
In the example above, ``x`` is global. 
But saying::

  local x = 1 
  
makes ``x`` local, i.e. its scope and visibility is constrained to the enclosing block of code, and any nested blocks. Note that
local variables avoid a lookup in the 'global' table and hence are more efficient. Thus it is common practice to cache values in
local variables. For example, ``print`` is a global function - and following creates a local variable that caches it::

  local print = print -- caches global print() function
  print('hello world!') -- calls the same function as global print()

There are some exceptions to the rule:

* the iterator variables declared in a ``for`` loop are implicitly local.
* function parameters are local to the function 

The 'table' type
================
Lua's only complex / aggregate data type is a table. Tables are used for many things in Lua, even internally within Lua.
Here are some examples::

  local a = {} -- creates an empty table
  local b = {10,20,30} -- creates a table with three array elements at positions 1,2,3
                       -- this is short cut for:
                       -- local b = {}
                       -- b[1] = 10
                       -- b[2] = 20
                       -- b[3] = 30
  local c = { name='Ravi' } -- creates a table with one hash map entry
                            -- this is short cut for:
                            -- local c = {}
                            -- c['name'] = 'Ravi'
                            
Internally the table is a composite hash table / array structure. Consecutive values starting at integer index 1 are inserted into the array, else the values go into the hash table. Hence, in the example below::

  local t = {}
  t[1] = 20 -- goes into array
  t[2] = 10 -- goes into array
  t[100] = 1 -- goes into hash table as not consecutive
  t.name = 'Ravi' -- goes into hash table
                  -- t.name is syntactic sugar for t['name']

To iterate over array values you can write::

  for i = 1,#t do
    print(t[i])
  end
  
Note that above will only print 20,10.

To iterate over all values write::

  for k,v in pairs(t) do
    print(k,v)
  end
  
Unfortunately, you need to get a good understanding of when values will go into the array part of a table, because some Lua library functions work only on the array part. Example::

  table.sort(t)
  
You will see that only values at indices 1 and 2 were sorted.
Another frequent problem is that the only way to reliably know the total number of elements in a table is to count the values. 
The ``#`` operator returns the length of the consecutive array elements starting at index 1.

Functions are values stored in variables
========================================
You already saw that we can write::

  local x = function() 
            end
  
This creates a function and stores in in local variable ``x``. This is the same as::

  local function x() 
  end
  
Omitting the ``local`` keyword would create ``x`` in global scope.

Functions can be defined within functions - in fact all Lua functions are defined within a 'chunk' of code, which gets wrapped inside a Lua function.

Internally a function has a 'prototype' that holds the compiled code and other meta data regarding the function. An instance of the
function in created when the code executes. You can think of the 'prototype' as the 'class' of the function, and the function instance is akin to an object created from this class. 

Globals are just values in a special table
==========================================
Globals are handled in an interesting way. Whenever a name is used that is not found in any of the enclosing scopes and is not declared ``local``, then Lua will access/create a variable in a table accessed by the name ``_ENV`` (this applies to Lua 5.2 and above - Lua 5.1 had a different mechanism). Actually ``_ENV`` is just a captured value that points to a special table in Lua by default. This table access becomes evident when you look at the bytecode generated for some Lua code::

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

The ``GETTABUP`` instruction looks up the name 'print' in the captured table variable ``_ENV``. Lua uses the term 'upvalue' for captured variables.

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

Lua functions can return multiple values
========================================
An example of this already appeared above. Here is another::

  function foo()
    return 1, 'text'
  end
  
  x,y = foo()
  
Lua has integer and floating point numeric types
================================================
Since Lua 5.3 Lua's number type has integer and floating point representations. This is automatically managed; however a library function is provided to tell you what Lua thinks the number type is.

::

  x = 1  -- integer 
  y = 4.2 -- double 
  
  print(math.type(x)) -- says 'integer'
  print(math.type(y)) -- says 'float'
  
On 64-bit architecture by default an integer is represented as C ``int64_t`` and floating point as ``double``. The representation of the numeric type as native C types is one of the secrets of Lua's performance, as the numeric types do not require 'boxing'.
  
In Lua 5.3, there is a special division operator ``//`` that does integer division if the operands are both integer. Example::

  x = 4
  y = 3
  
  print(x//y) -- integer division results in 0
  print(x/y) -- floating division results in 1.3333333333333
  
Note that officially the ``//`` operator does floor division, hence if one or both of its operands is floating point then the result is also a floating point representing the floor of the division of its operands.

Having integer types has also made it natural to have support for bitwise operators in Lua 5.3.

A special ``nil`` value represents non-existent value
=====================================================
Lua has special value ``nil`` that represents no value, and evaluates to false in boolean expressions.

Any value that is not ``false`` or ``nil`` is true
==================================================
As mentioned above ``nil`` evaluates to false. 

Logical ``and`` and logical ``or`` select one of the values
===========================================================
When you perform a logical ``and`` or ``or`` the result is not boolean; these operators select one of the values. This is best
illustrated via examples::

  false or 'hello' -- selects 'hello'
  'hello' and 'world' -- selects 'world'
  false and 'hello' -- selects false
  nil or false -- selects false
  nil and false -- selects nil
  
* ``and`` selects the first value if it evaluates to false else the second value.
* ``or`` selects the first value if it evaluates to true else the second value.

Lua has some nice syntactic sugar for tables and functions
==========================================================
If you are calling a Lua function with a single string or table argument then the parenthesis can be omitted::

  print 'hello world' -- syntactic sugar for print('hello world')
  options { verbose=true, debug=true } -- syntactic sugar for options( { ... } )

Above is often used to create a DSL. For instance, see:

* `Lua's bug list <https://github.com/lua/lua/blob/master/bugs>`_
* `Premake <https://github.com/premake/premake-core/wiki/Your-First-Script>`_ - a tool similar to CMake

You have already seen also that::

  t = { surname = 'majumdar' }      -- t.surname is sugar for t['surname']
  t.name = 'dibyendu'               -- syntactic sugar for t['name'] = 'dibyendu'
  
A useful use case for tables is as modules. Thus a standard library module like ``math`` is simply a table of functions. Here is an example::

  module = { print, type } 
  module.print('hello')
  module.print 'hello'
  module.type('hello')
  
Finally, you can emulate an object oriented syntax using the ``:`` operator::

  x:foo('hello')                   -- syntactic sugar for foo(x, 'hello')
  
As we shall see, this feature enables Lua to support object orientation.

A Lua script is called a chunk - and is the unit of compilation in Lua
======================================================================
When you present a script to Lua, it is compiled. The script can be a file or a string. Internally the content of the script is wrapped inside a Lua function. So that means that a scipt can have ``local`` variables, as these live in the wrapping function. 

It is common practice for scripts to return a table of functions - as then the script can be treated as a module. There is a library function 'require' which loads a script as a module.

Suppose you have following script saved in a file ``sample.lua``::

  -- sample script
  local function foo() end
  local function bar() end
  
  return { foo=foo, bar=bar }      -- i.e. ['foo'] = foo, ['bar'] = bar
  
Above script returns a table containing two functions.

Now another script can load this as follows::

  local sample = require 'sample' -- Will call sample.lua script and save its table of functions

The library function ``require()`` does more than what is described above, of course. For instance it ensures that the module is only loaded once, and it uses various search paths to locate the script. It can even load C modules. Anyway, now the table returned from 
the sample script is stored in the local variable 'sample' and we can write::

  sample.foo()
  sample.bar()
  
Lua functions can be yielded from and resumed later
===================================================
Lua allows functions to be suspended and resumed. The function suspends itself by calling a library function to yield. Sometime later
the function may be resumed by the caller or something else - when resumed, the Lua function continues from the point of suspension.

When yielding you can pass values back to the caller. Similarly when resuming the caller can pass values to the function.

This is perhaps the most advanced feature in Lua, and not one that can be easily demonstrated in a simple way. Following is the simplest example I could think of. 

::

  function test()
    local message = coroutine.yield('hello')
    print(message)
  end
  
  -- create a new Lua stack (thread)
  thread = coroutine.create(test)
  
  -- start the coroutine 
  status,message = coroutine.resume(thread) -- initial start
  
  -- coroutine suspended so we have got control back
  -- the coroutine yielded message to us - lets print it
  print(message) -- says 'hello', the value returned by yield
  
  -- Resume the coroutine / send it the message 'world'
  status,message = coroutine.resume(thread, 'world') 
 
  -- above will print 'world' 
  -- status above will be true
  -- but now the coroutine has ended so further calls to resume will return status as false
 
By the fact that 'hello' is printed before 'world' we can tell that the coroutine was suspended and then resumed. 
 
In the Lua documentation, the return value from ``coroutine.create()`` is called a ``thread``. However don't confuse this with threads as in C++ or Java. You can think of a Lua ``thread`` as just another Lua stack. Basically whenever Lua executes any code - the code operates on a Lua stack. Initially there is only one stack (main thread). When you create a coroutine, a new stack is allocated, and the all functions called from the coroutine will operate on this new stack. Since the Lua stack is a heap allocated structure - suspending the coroutine is equivalent to returning back to the caller using a ``longjmp()``. The stack is preserved, so that the function that yielded can be resumed later from wherever it suspended itself.

There is no automatic scheduling of Lua coroutines, a coroutine has to be explicitly resumed by the program. 

Lua's error handling is based on C setjmp/longjmp
=================================================
You raise an error in Lua by calling library functions ``error()`` or ``assert()``. Lua library functions can also raise errors. When an error is raised Lua does a C ``longjmp`` to the nearest location in the call stack where the caller used a 'protected call'. A 'protected call' is a function calling mechanism that does a C ``setjmp``.

Here is how a protected call is done::

  function foo(message) 
    -- raise error if message is nil
    if not message then
      error('message expected') 
    else
      print(message)
      return 4.2
    end
  end
  
  -- call foo('hello') in protected mode
  -- this is done using the Lua library function pcall()
  status,returnvalue = pcall(foo, 'hello') 
  
  -- since this call should succeed, status will be true
  -- returnvalue should contain 4.2
  assert(returnvalue == 4.2)
  
  -- call foo() without arguments in protected mode
  status, returnvalue = pcall(foo) 
  -- above will fail and status will be false
  -- But returnvalue will now have the error message
  
  assert(not status)
  print(returnvalue)
  -- above prints 'message expected'

The Lua error handling mechanism has following issues:

* The code that can raise errors must be encapsulated in a function as ``pcall()`` can only call functions
* The return values from ``pcall()`` depend upon whether the call terminated normally or due to an error - so caller needs to check the status of the call and only then proceed
* On raising an error the ``longjmp`` unwinds the stack - there is no mechanism for any intermediate objects to perform cleanup as is possible in C++ using destructors, or in Java, C++, Python using ``finally`` blocks, or as done by the ``defer`` statement in Go
* You can setup a finalizer on Lua user types that will eventually execute when the value is garbage collected - this is typically used to free up memory used by the value - but you have no control over when the finalizer will run, hence relying upon finalizers for cleanup is problematic

Lua has a meta mechanism that enables a DIY class / object system
=================================================================
Firstly simple object oriented method calls can be emulated in Lua by relying upon the ``:`` operator described earlier. Recollect that::

  object:method(arg) -- is shortcut for method(object, arg)
  
The next bit of syntactic sugar is shown below::

  object = {}
  function object:method(arg) 
    print('method called with ', self, arg) -- self is automatic parameter and is really object
  end

Above is syntactic sugar for following equivalent code::

  object = {}
  object.method = function(self, arg)
    print('method called with ', self, arg)
  end
  
As the object is passed as the ``self`` argument, the method can access other properties and methods contained in the object, which is just a normal table.

::
    object:method('hello')                  -- calls method(object, 'hello')

This mechanism is fine for Lua code but doesn't work for user defined values created in C. Lua supports another more sophisticated approach that makes use of a facility in Lua called metatables. A ``metatable`` is simply an ordinary table that you can associate with any table or user defined type created in C code. The advantage of using the ``metatable`` approach is that it also works for user defined types created in C code. Here we will look at how it can be applied to Lua code.

Keeping to the same example above, this approach requires us to populate a metatable with the methods. We can think of the metatable as the class of the object.::

  Class = {}                  -- our metatable
  Class.__index = Class       -- This is a meta property (see description below)
  
  function Class:method(arg)
    print('method called with ', self, arg)
  end
  
  function Class:new() 
    -- factory for creating objects
    local object = {}
    setmetatable(object, Class)
    return object
  end
  
Notice that we set the field ``__index`` in the ``Class`` table to point to itself. This is a special field that Lua recognizes and whenever you access a field in an object (table), if the field is not found in the object and if the object has a metatable with ``__index`` field set, the Lua will lookup the field you want in the metatable. Thus in the example below::

  object = Class:new()
  object:method('hello')

Lua notices that there is no ``method`` field in object. But object has a metatable assigned to it, and this has ``__index`` set, so Lua looks up ``Class.__index['method']`` and finds the method.
  
    
    


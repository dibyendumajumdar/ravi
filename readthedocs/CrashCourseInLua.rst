===================
Crash Course in Lua
===================

Introduction
============
Lua is a small, but powerful language that is implemented as a library in C. This crash course in meant to help you quickly become familiar with Lua. This is also an opinionated introduction to Lua. This is also WIP and incomplete.

Key Features
============
* Lua is dynamically typed like Python
* By default variables in Lua are global unless declared local
* There is a single complex / aggregate type called a Table, which combines hash table/map and array features
* Functions in Lua are values stored in variables; in particular functions do not have names
* Globals in Lua are just values stored in a special Lua table 
* Lua has an incremental garbage collector
* Lua is single threaded but its VM is small and encapsulated in a single data structure - hence each OS thread can be given its own 
  Lua VM
* Lua has integer (since 5.3) and double types that map to native C types
* A Lua script is called a chunk - and is the unit of compilation in Lua
* Lua functions can be yielded from and resumed later on, i.e., Lua supports coroutines
* A special ``nil`` value represents non-existent value
* Functions in Lua are closures - they can capture variables from outer scope and such variables live on even though the surrounding scope
  is no longer alive
* Lua's error handling is based on C setjmp/longjmp, and errors are caught via a special function call mechanism
* Lua has some nice syntactic sugar for tables and functions 
* Lua has a meta mechanism that enables a DIY class / object system with some syntactic sugar to make it look nice
* Lua functions can return multiple values
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
                            



  

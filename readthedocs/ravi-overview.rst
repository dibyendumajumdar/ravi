=========================
Ravi Programming Language
=========================

Ravi is a derivative/dialect of `Lua 5.3 <http://www.lua.org/>`_ with limited optional static typing and an `LLVM <http://www.llvm.org/>`_ powered JIT compiler. The name Ravi comes from the Sanskrit word for the Sun. Interestingly a precursor to Lua was `Sol <http://www.lua.org/history.html>`_ which had support for static types; Sol means the Sun in Portugese.

Lua is perfect as a small embeddable dynamic language so why a derivative? Ravi extends Lua with static typing for greater performance under JIT compilation. However, the static typing is optional and therefore Lua programs are also valid Ravi programs.

There are other attempts to add static typing to Lua - e.g. `Typed Lua <https://github.com/andremm/typedlua>`_ but these efforts are mostly about adding static type checks in the language while leaving the VM unmodified. The Typed Lua effort is very similar to the approach taken by Typescript in the JavaScript world. The static typing is to aid programming in the large - the code is eventually translated to standard Lua and executed in the unmodified Lua VM.

My motivation is somewhat different - I want to enhance the VM to support more efficient operations when types are known. Type information can be exploited by JIT compilation technology to improve performance. At the same time, I want to keep the language safe and therefore usable by non-expert programmers. 

Of course there is also the fantastic `LuaJIT <http://luajit.org>`_ implementation. Ravi has a different goal compared to 
LuaJIT. Ravi prioritizes ease of maintenance and support, language safety, and compatibility with Lua 5.3, over maximum performance. For more detailed comparison please refer to the documentation links below.


.. contents:: Table of Contents
   :depth: 1
   :backlinks: top

Features
========
* Optional static typing 
* Type specific bytecodes to improve performance
* Compatibility with Lua 5.3 (see Compatibility section below)
* `LLVM <http://www.llvm.org/>`_ powered JIT compiler
* Additionally a `libgccjit <https://gcc.gnu.org/wiki/JIT>`_ based alternative JIT compiler is also available (on a branch), although this is not currently being worked on
* LLVM bindings exposed in Lua

Recent Work
===========
* `Experimental Type Annotations`_ for user defined types was implemented in Oct 2017.
* A new `X86-64 VM written in assembler <https://github.com/dibyendumajumdar/ravi/tree/master/vmbuilder>`_ using `dynasm <https://luajit.org/dynasm.html>`_ tool is under development currently.

Documentation
=============
See `Ravi Documentation <http://the-ravi-programming-language.readthedocs.org/en/latest/index.html>`_.
As more stuff is built I will keep updating the documentation so please revisit for latest information.

Also see the slides I presented at the `Lua 2015 Workshop <http://www.lua.org/wshop15.html>`_.

Lua Goodies
===========
* `An Introduction to Lua <http://the-ravi-programming-language.readthedocs.io/en/latest/lua-introduction.html>`_ attempts to provide a quick overview of Lua for folks coming from other languages.
* `Lua 5.3 Bytecode Reference <http://the-ravi-programming-language.readthedocs.io/en/latest/lua_bytecode_reference.html>`_ is my attempt to bring up to date the `Lua 5.1 Bytecode Reference <http://luaforge.net/docman/83/98/ANoFrillsIntroToLua51VMInstructions.pdf>`_. 

JIT Implementation
==================
The LLVM JIT compiler is functional. The Lua and Ravi bytecodes currently implemented in LLVM are described in `JIT Status <http://the-ravi-programming-language.readthedocs.org/en/latest/ravi-jit-status.html>`_ page.

Ravi also provides an `LLVM binding <http://the-ravi-programming-language.readthedocs.org/en/latest/llvm-bindings.html>`_; this is still work in progress so please check documentation for the latest status.

Ravi Extensions to Lua 5.3
==========================

Optional Static Typing
----------------------
Ravi allows you to annotate ``local`` variables and function parameters with static types. The supported types and the resulting behaviour are as follows:

``integer``
  denotes an integral value of 64-bits.
``number``
  denotes a double (floating point) value of 8 bytes.
``integer[]``
  denotes an array of integers
``number[]``
  denotes an array of numbers
``table``
  a Lua table

Additionally there are some `Experimental Type Annotations`_.

Declaring the types of ``local`` variables and function parameters has following advantages.

* ``integer`` and ``number`` types are automatically initialized to zero
* Arithmetic operations on numeric types make use of type specific bytecodes which leads to more efficient JIT compilation
* Specialised operators to get/set from array types are implemented; this makes array access more efficient in JIT mode as the access can be inlined
* Declared tables allow specialized opcodes for table gets involving integer and short literal string keys; these opcodes result in more efficient JIT code
* Values assigned to typed variables are checked statically when possible; if the values are results from a function call then runtime type checking is performed
* The standard table operations on arrays are checked to ensure that the type is not subverted
* Even if a typed variable is captured in a closure its type must be respected
* When function parameters are decorated with types, Ravi performs an implicit coersion of those parameters to the required types. If the coersion fails a runtime error occurs.

The array types (``number[]`` and ``integer[]``) are specializations of Lua table with some additional special behaviour:

* Array types are not compatible with declared table variables, i.e. following is not allowed::
  
    local t: table = {}
    local t2: number[] = t  -- error!

    local t3: number[] = {}
    local t4: table = t3    -- error!

  But following is okay::

    local t5: number[] = {}
    local t6 = t5           -- t6 treated as table

  The reason for these restrictions is that declared table types generate optimized JIT code which assumes that the keys are integers
  or literal short strings. Similarly declared array types result in specialized JIT code that assume integer keys and numeric values. 
  The generated JIT code would be incorrect if the types were not as expected.

* Indices >= 1 should be used when accessing array elements. Ravi arrays (and slices) have a hidden slot at index 0 for performance reasons, but this is not visible in ``pairs()`` or ``ipairs()``, or when initializing an array using a literal initializer; only direct access via the ``[]`` operator can see this slot.   
* Arrays must always be initialized:: 

    local t: number[] = {} -- okay
    local t2: number[]     -- error!

  This restriction is placed as otherwise the JIT code would need to insert tests to validate that the variable is not nil.

* An array will grow automatically (unless the array was created as fixed length using ``table.intarray()`` or ``table.numarray()``) if the user sets the element just past the array length::

    local t: number[] = {} -- dynamic array
    t[1] = 4.2             -- okay, array grows by 1
    t[5] = 2.4             -- error! as attempt to set value 

* It is an error to attempt to set an element that is beyond len+1 on dynamic arrays; for fixed length arrays attempting to set elements at positions greater than len will cause an error.
* The current used length of the array is recorded and returned by len operations
* The array only permits the right type of value to be assigned (this is also checked at runtime to allow compatibility with Lua)
* Accessing out of bounds elements will cause an error, except for setting the len+1 element on dynamic arrays
* It is possible to pass arrays to functions and return arrays from functions. Arrays passed to functions appear as Lua tables inside 
  those functions if the parameters are untyped - however the tables will still be subject to restrictions as above. If the parameters are typed then the arrays will be recognized at compile time::

    local function f(a, b: integer[], c)
      -- Here a is dynamic type
      -- b is declared as integer[]
      -- c is also a dynamic type
      b[1] = a[1] -- Okay only if a is actually also integer[]
      b[1] = c[1] -- Will fail if c[1] cannot be converted to an integer
    end

    local a : integer[] = {1}
    local b : integer[] = {}
    local c = {1}

    f(a,b,c)        -- ok as c[1] is integer
    f(a,b, {'hi'})  -- error!

* Arrays returned from functions can be stored into appropriately typed local variables - there is validation that the types match::

    local t: number[] = f() -- type will be checked at runtime

* Operations on array types can be optimised to special bytecode and JIT only when the array type is statically known. Otherwise regular table access will be used subject to runtime checks.
* Array types ignore ``__index``, ``__newindex`` and ``__len`` metamethods.
* Array types cannot be set as metatables for other values. 
* ``pairs()`` and ``ipairs()`` work on arrays as normal
* There is no way to delete an array element.
* The array data is stored in contiguous memory just like native C arrays; morever the garbage collector does not scan the array data

A declared table (as shown below) has some additional nuances::

    local t: table = {}

* Like array types, a variable of ``table`` type must be initialized
* Array types are not compatible with declared table variables, i.e. following is not allowed::
   
    local t: table = {}
    local t2: number[] = t -- error!

* When short string literals are used to access a table element, specialized bytecodes are generated that are more efficiently JIT compiled::

    local t: table = { name='dibyendu'}
    print(t.name) -- The GETTABLE opcode is specialized in this case

* As with array types, specialized bytecodes are generated when integer keys are used

Following library functions allow creation of array types of defined length.

``table.intarray(num_elements, initial_value)``
  creates an integer array of specified size, and initializes with initial value. The return type is integer[]. The size of the array cannot be changed dynamically, i.e. it is fixed to the initial specified size. This allows slices to be created on such arrays.

``table.numarray(num_elements, initial_value)``
  creates an number array of specified size, and initializes with initial value. The return type is number[]. The size of the array cannot be changed dynamically, i.e. it is fixed to the initial specified size. This allows slices to be created on such arrays.

Type Assertions
---------------
Ravi does not support defining new types, or structured types based on tables. This creates some practical issues when dynamic types are mixed with static types. For example::

  local t = { 1,2,3 }
  local i: integer = t[1] -- generates an error

Above code generates an error as the compiler does not know that the value in ``t[1]`` is an integer. However often we as programmers know the type that is expected and it would be nice to be able to tell the compiler what the expected type of ``t[1]`` is above. To enable this Ravi supports type assertion operators. A type assertion is introduced by the '``@``' symbol, which must be followed by the type name. So we can rewrite the above example as::

  local t = { 1,2,3 }
  local i: integer = @integer( t[1] )

The type assertion operator is a unary operator and binds to the expression following the operator. We use the parenthesis above to enure that the type assertion is applied to ``t[1]`` rather than ``t``. More examples are shown below::

  local a: number[] = @number[] { 1,2,3 }
  local t = { @number[] { 4,5,6 }, @integer[] { 6,7,8 } }
  local a1: number[] = @number[]( t[1] )
  local a2: integer[] = @integer[]( t[2] )

For a real example of how type assertions can be used, please have a look at the test program `gaussian2.lua <https://github.com/dibyendumajumdar/ravi/blob/master/ravi-tests/gaussian2.lua>`_ 

Experimental Type Annotations
-----------------------------
Following type annotations have experimental support. These type annotations are not always statically enforced. Furthermore using these types does not affect the JIT code generation, i.e. variables annotated using these types are still treated as dynamic types. 

The scenarios where these type annotations have an impact are:

* Function parameters containing these annotations lead to type assertions at runtime.
* The type assertion operator @ can be applied to these types - leading to runtime assertions.
* Annotating ``local`` declarations results in type assertions.

``string``
  denotes a string
``closure``
  denotes a function
Name
  Denotes a value that has a `metatable registered under Name <https://www.lua.org/pil/28.2.html>`_ in the Lua registry. The Name must be a valid Lua name - hence periods in the name are not allowed. 

The main use case for these annotations is to help with type checking of larger Ravi programs. These type checks, particularly the one for user defined types, are executed directly by the VM and hence are more efficient than performing the checks in other ways. 

All three types above allow ``nil`` assignment.

Examples::

  -- Create a metatable
  local mt = { __name='MyType'}

  -- Register the metatable in Lua registry
  debug.getregistry().MyType = mt

  -- Create an object and assign the metatable as its type
  local t = {}
  setmetatable(t, mt)

  -- Use the metatable name as the object's type
  function x(s: MyType) 
    local assert = assert
    assert(@MyType(s) == @MyType(t))
    assert(@MyType(t) == t)
  end

  -- Here we use the string type
  function x(s1: string, s2: string)
    return @string( s1 .. s2 )
  end
  
  -- Following demonstrates an error caused by the type checking
  -- Note that this error is raised at runtime
  function x() 
    local s: string
    -- call a function that returns integer value
    -- and try to assign to s
    s = (function() return 1 end)() 
  end
  x() -- will fail at runtime

Array Slices
------------
Since release 0.6 Ravi supports array slices. An array slice allows a portion of a Ravi array to be treated as if it is an array - this allows efficient access to the underlying array elements. Following new functions are available:

``table.slice(array, start_index, num_elements)``
  creates a slice from an existing *fixed size* array - allowing efficient access to the underlying array elements.

Slices access the memory of the underlying array; hence a slice can only be created on fixed size arrays (constructed by ``table.numarray()`` or ``table.intarray()``). This ensures that the array memory cannot be reallocated while a slice is referring to it. Ravi does not track the slices that refer to arrays - slices get garbage collected as normal. 

Slices cannot extend the array size for the same reasons above.

The type of a slice is the same as that of the underlying array - hence slices get the same optimized JIT operations for array access.

Each slice holds an internal reference to the underlying array to ensure that the garbage collector does not reclaim the array while there are slices pointing to it.

For an example use of slices please see the `matmul1.ravi <https://github.com/dibyendumajumdar/ravi/blob/master/ravi-tests/matmul1.ravi>`_ benchmark program in the repository. Note that this feature is highly experimental and not very well tested.
  
Examples
--------
Example of code that works - you can copy this to the command line input::

  function tryme()
    local i,j = 5,6
    return i,j
  end
  local i:integer, j:integer = tryme(); print(i+j)

When values from a function call are assigned to a typed variable, an implicit type coersion takes place. In above example an error would occur if the function returned values that could not converted to integers.

In the following example, the parameter ``j`` is defined as a ``number``, hence it is an error to pass a value that cannot be converted to a ``number``::

  function tryme(j: number)
    for i=1,1000000000 do
      j = j+1
    end
    return j
  end
  print(tryme(0.0))

An example with arrays::

  function tryme()
    local a : number[], j:number = {}
    for i=1,10 do
      a[i] = i
      j = j + a[i]
    end
    return j
  end
  print(tryme())

Another example using arrays. Here the function receives a parameter ``arr`` of type ``number[]`` - it would be an error to pass any other type to the function because only ``number[]`` types can be converted to ``number[]`` types::

  function sum(arr: number[]) 
    local n: number = 0.0
    for i = 1,#arr do
      n = n + arr[i]
    end
    return n
  end

  print(sum(table.numarray(10, 2.0)))

The ``table.numarray(n, initial_value)`` creates a ``number[]`` of specified size and initializes the array with the given initial value.

All type checks are at runtime
------------------------------
To keep with Lua's dynamic nature Ravi uses a mix of compile type checking and runtime type checks. However due to the dynamic nature of Lua, compilation happens at runtime anyway so effectually all checks are at runtime. 

JIT API
-------
The LLVM based JIT compiler is functional. 
There are two modes of JIT compilation.

auto mode
  in this mode the compiler decides when to compile a Lua function. The current implementation is very simple - any Lua function call is checked to see if the bytecodes contained in it can be compiled. If this is true then the function is compiled provided either a) function has a fornum loop, or b) it is largish (greater than 150 bytecodes) or c) it is being executed many times (> 50). Because of the simplistic behaviour performance the benefit of JIT compilation is only available if the JIT compiled functions will be executed many times so that the cost of JIT compilation can be amortized.
manual mode
  in this mode user must explicitly request compilation. This is the default mode. This mode is suitable for library developers who can pre compile the functions in library module table.

A JIT api is available with following functions:

``ravi.jit([b])``
  returns enabled setting of JIT compiler; also enables/disables the JIT compiler; defaults to true
``ravi.auto([b [, min_size [, min_executions]]])``
  returns setting of auto compilation and compilation thresholds; also sets the new settings if values are supplied; defaults are false, 150, 50.
``ravi.compile(func_or_table[, options])``
  compiles a Lua function (or functions if a table is supplied) if possible, returns ``true`` if compilation was successful for at least one function. ``options`` is an optional table with compilation options - in particular ``omitArrayGetRangeCheck`` - which disables range checks in array get operations to improve performance in some cases. Note that at present if the first argument is a table of functions and has more than 100 functions then only the first 100 will be compiled. You can invoke compile() repeatedly on the table until it returns false. Each invocation leads to a new module being created; any functions already compiled are skipped.
``ravi.iscompiled(func)``
  returns the JIT status of a function
``ravi.dumplua(func)``
  dumps the Lua bytecode of the function
``ravi.dumpir(func)``
  (deprecated) dumps the IR of the compiled function (only if function was compiled; only available in LLVM 4.0 and earlier)
``ravi.dumpasm(func)``
  (deprecated) dumps the machine code using the currently set optimization level (only if function was compiled; only available in LLVM version 4.0 and earlier)
``ravi.optlevel([n])``
  sets LLVM optimization level (0, 1, 2, 3); defaults to 2. These levels are handled by reusing LLVMs default pass definitions which are geared towards C/C++ programs, but appear to work well here. If level is set to 0, then an attempt is made to use fast instruction selection to further speed up compilation.
``ravi.sizelevel([n])``
  sets LLVM size level (0, 1, 2); defaults to 0
``ravi.tracehook([b])``
  Enables support for line hooks via the debug api. Note that enabling this option will result in inefficient JIT as a call to a C function will be inserted at beginning of every Lua bytecode boundary; use this option only when you want to use the debug api to step through code line by line
``ravi.verbosity([b])``
  Controls the amount of verbose messages generated during compilation. Currently only available for LLVM.

Performance
===========
For performance benchmarks please visit the `Ravi Performance Benchmarks <http://the-ravi-programming-language.readthedocs.org/en/latest/ravi-benchmarks.html>`_ page.

To obtain the best possible performance, types must be annotated so that Ravi's JIT compiler can generate efficient code. 
Additionally function calls are expensive - as the JIT compiler cannot inline function calls, all function calls go via the Lua call protocol which has a large overhead. This is true for both Lua functions and C functions. For best performance avoid function calls inside loops.

Compatibility with Lua
======================
Ravi should be able to run all Lua 5.3 programs in interpreted mode, but following should be noted: 

* Ravi supports optional typing and enhanced types such as arrays (described above). Programs using these features cannot be run by standard Lua. However all types in Ravi can be passed to Lua functions; operations on Ravi arrays within Lua code will be subject to restrictions as described in the section above on arrays. 
* Values crossing from Lua to Ravi will be subjected to typechecks should these values be assigned to typed variables.
* Upvalues cannot subvert the static typing of local variables (issue #26) when types are annotated.
* Certain Lua limits are reduced due to changed byte code structure. These are described below.

+-----------------+-------------+-------------+
| Limit name      | Lua value   | Ravi value  |
+=================+=============+=============+
| MAXUPVAL        | 255         | 125         |
+-----------------+-------------+-------------+
| LUAI_MAXCCALLS  | 200         | 125         |
+-----------------+-------------+-------------+
| MAXREGS         | 255         | 125         |
+-----------------+-------------+-------------+
| MAXVARS         | 200         | 125         |
+-----------------+-------------+-------------+
| MAXARGLINE      | 250         | 120         |
+-----------------+-------------+-------------+

When JIT compilation is enabled there are following additional constraints:

* Ravi will only execute JITed code from the main Lua thread; any secondary threads (coroutines) execute in interpreter mode.
* In JITed code tailcalls are implemented as regular calls so unlike the interpreter VM which supports infinite tail recursion JIT compiled code only supports tail recursion to a depth of about 110 (issue #17)

Building Ravi
=============

Quick build without JIT
-----------------------
A Makefile is supplied for a simple build without the JIT. Just run ``make`` and follow instructions. You may need to customize the Makefiles. 

For building Ravi with JIT options please read on.

Build Dependencies
------------------

* CMake

Ravi can be built with or without LLVM. Following versions of LLVM work with Ravi.

* LLVM 3.7 or 3.8 or 3.9 or 4.0 or 5.0
* LLVM 3.5 and 3.6 should also work but have not been recently tested

Unless otherwise noted the instructions below should work for LLVM 3.9 and later.

Since LLVM 5.0 Ravi has begun to use the new ORC JIT apis. These apis are more memory efficient compared to the MCJIT apis because they release the Module IR as early as possible, whereas with MCJIT the Module IR hangs around as long as the compiled code is held. Because of this significant improvement, I recommend using LLVM 5.0 and above.

Building LLVM on Windows
------------------------
I built LLVM from source. I used the following sequence from the VS2017 command window::

  cd \github\llvm
  mkdir build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=c:\LLVM -DLLVM_TARGETS_TO_BUILD="X86" -G "Visual Studio 15 2017 Win64" ..  

I then opened the generated solution in VS2017 and performed a INSTALL build from there. Above will build the 64-bit version of LLVM libraries. To build a 32-bit version omit the ``Win64`` parameter. 

.. note:: Note that if you perform a Release build of LLVM then you will also need to do a Release build of Ravi otherwise you will get link errors.

Building LLVM on Ubuntu
-----------------------
On Ubuntu I found that the official LLVM distributions don't work with CMake. The CMake config files appear to be broken.
So I ended up downloading and building LLVM from source and that worked. The approach is similar to that described for MAC OS X below.

Building LLVM on MAC OS X
-------------------------
I am using Max OSX El Capitan. Pre-requisites are XCode 7.x and CMake.
Ensure cmake is on the path.
Assuming that LLVM source has been extracted to ``$HOME/llvm-3.7.0.src`` I follow these steps::

  cd llvm-3.7.0.src
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/LLVM -DLLVM_TARGETS_TO_BUILD="X86" ..
  make install

Building Ravi with JIT enabled
------------------------------
I am developing Ravi using Visual Studio 2017 Community Edition on Windows 10 64bit, gcc on Unbuntu 64-bit, and clang/Xcode on MAC OS X. I was also able to successfully build a Ubuntu version on Windows 10 using the newly released Ubuntu/Linux sub-system for Windows 10.

.. note:: Location of cmake files prior to LLVM 3.9 was ``$LLVM_INSTALL_DIR/share/llvm/cmake``.

Assuming that LLVM has been installed as described above, then on Windows I invoke the cmake config as follows::

  cd build
  cmake -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=c:\ravi -DLLVM_DIR=c:\LLVM\lib\cmake\llvm -G "Visual Studio 15 2017 Win64" ..

I then open the solution in VS2017 and do a build from there.

On Ubuntu I use::

  cd build
  cmake -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
  make

Note that on a clean install of Ubuntu 15.10 I had to install following packages:

* cmake
* git
* libreadline-dev

On MAC OS X I use::

  cd build
  cmake -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Release -G "Xcode" ..

I open the generated project in Xcode and do a build from there. You can also use the command line build tools if you wish - generate the make files in the same way as for Linux.

Building without JIT
--------------------
You can omit ``-DLLVM_JIT=ON`` option above to build Ravi with a null JIT implementation.

Building Static Libraries
-------------------------
By default the build generates a shared library for Ravi. You can choose to create a static library and statically linked executables by supplying the argument ``-DSTATIC_BUILD=ON`` to CMake.

Build Artifacts
---------------
The Ravi build creates a shared or static depending upon options supplied to CMake, the Ravi executable and some test programs. Additionally when JIT compilation is switched off, the ``ravidebug`` executable is generated which is the `debug adapter for use by Visual Studio Code <https://github.com/dibyendumajumdar/ravi/tree/master/vscode-debugger>`_. 

The ``ravi`` command recognizes following environment variables. Note that these are only for internal debugging purposes.

``RAVI_DEBUG_EXPR``
  if set to a value this triggers debug output of expression parsing
``RAVI_DEBUG_CODEGEN``
  if set to a value this triggers a dump of the code being generated
``RAVI_DEBUG_VARS``
  if set this triggers a dump of local variables construction and destruction

Also see section above on available API for dumping either Lua bytecode or LLVM IR for compiled code.

Testing
-------
I test the build by running a modified version of Lua 5.3.3 test suite. These tests are located in the ``lua-tests`` folder. Additionally I have ravi specific tests in the ``ravi-tests`` folder. There is a also a travis build that occurs upon commits - this build runs the tests as well.

.. note:: To thoroughly test changes, you need to invoke CMake with ``-DCMAKE_BUILD_TYPE=Debug`` option. This turns on assertions, memory checking, and also enables an internal module used by Lua tests.

Roadmap
=======
* 2015 
       - Implemented JIT compilation using LLVM
* 2015 
       - Implemented libgccjit based alternative JIT (now discontinued)
* 2016 
       - Implemented debugger for Ravi and Lua 5.3 for `Visual Studio Code <https://github.com/dibyendumajumdar/ravi/tree/master/vscode-debugger>`_ 
* 2017 
       - Embedded C compiler using dmrC project (C JIT compiler) 
       - Additional type annotations
* 2018 
       - 1.0 release of Ravi
       - More testing and test cases
       - ASM VM for X86-64 platform 
       - Better support for earlier Lua versions (5.1 especially)

License
=======
MIT License for LLVM version.


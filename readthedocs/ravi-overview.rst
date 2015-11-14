Ravi Programming Language
=========================

Ravi is a derivative/dialect of `Lua 5.3 <http://www.lua.org/>`_ with limited optional static typing and an `LLVM <http://www.llvm.org/>`_ powered JIT compiler. The name Ravi comes from the Sanskrit word for the Sun.

Lua is perfect as a small embeddable dynamic language so why a derivative? Ravi extends Lua with static typing for greater performance under JIT compilation. However, the static typing is optional and therefore Lua programs are also valid Ravi programs.

There are other attempts to add static typing to Lua (e.g. `Typed Lua <https://github.com/andremm/typedlua>`_ but these efforts are mostly about adding static type checks in the language while leaving the VM unmodified. So the static typing is to aid programming in the large - the code is eventually translated to standard Lua and executed in the unmodified Lua VM.

My motivation is somewhat different - I want to enhance the VM to support more efficient operations when types are known. Type information can be exploited by JIT compilation technology to improve performance.

Goals
-----
* Optional static typing for Lua 
* Type specific bytecodes to improve performance
* Compatibility with Lua 5.3 (see Compatibility section below)
* `LLVM <http://www.llvm.org/>`_ powered JIT compiler
* Additionally a `libgccjit <https://gcc.gnu.org/wiki/JIT>`_ based alternative JIT compiler is also available.

Documentation
--------------
See `Ravi Documentation <http://the-ravi-programming-language.readthedocs.org/en/latest/index.html>`_.
As more stuff is built I will keep updating the documentation so please revisit for latest information.

Also see the slides I presented at the `Lua 2015 Workshop <http://www.lua.org/wshop15.html>`_.

Status
------
The project was kicked off in January 2015. 

JIT Implementation
++++++++++++++++++
The LLVM JIT compiler is functional. The Lua and Ravi bytecodes currently implemented in LLVM are described in `JIT Status <http://the-ravi-programming-language.readthedocs.org/en/latest/ravi-jit-status.html>`_ page.

Ravi also provides an `LLVM binding <http://the-ravi-programming-language.readthedocs.org/en/latest/llvm-bindings.html>`_; this is still work in progress so please check documentation for the latest status.

As of July 2015 the `libgccjit <http://the-ravi-programming-language.readthedocs.org/en/latest/ravi-jit-libgccjit.html>`_ based JIT implementation is also functional but some byte codes are not yet compiled, and featurewise this implementation is somewhat lagging behind the LLVM based implementation. 

Performance Benchmarks
++++++++++++++++++++++
For performance benchmarks please visit the `Ravi Performance Benchmarks <http://the-ravi-programming-language.readthedocs.org/en/latest/ravi-benchmarks.html>`_ page.

Ravi Extensions to Lua 5.3
--------------------------

Optional Static Typing
++++++++++++++++++++++
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

Declaring the types of ``local`` variables and function parameters has following advantages.

* ``integer`` and ``number`` types are automatically initialized to zero
* Arithmetic operations on numeric types make use of type specific bytecodes which leads to more efficient JIT compilation
* Specialised operators to get/set from array types are implemented; this makes array access more efficient in JIT mode as the access can be inlined
* Declared tables allow specialized opcodes for usages involving integer and short literal string keys; these opcodes result in more efficient JIT code
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

  The reason for this discrepancy is that declared table types generate optimized JIT code which assumes that the keys are integers
  or literal short strings. The generated code would be incorrect if this expectation was not met.

* Indices >= 1 should be used when accessing array elements. Ravi arrays (and slices) have a hidden slot at index 0 for performance reasons, but this is not visible under ``pairs()`` or ``ipairs()``, or when initializing an array using a literal initializer; only direct access via the ``[]`` operator can see this slot.   
* Arrays must always be initialized:: 

    local t: number[] = {} -- okay
    local t2: number[]     -- error!

  This restriction is placed as otherwise the JIT code would need to insert tests to validate that the variable is not nil.

* An array will grow automatically if user sets the element just past the array length::

    local t: number[] = {}
    t[1] = 4.2             -- okay, array grows by 1
    t[5] = 2.4             -- error! as attempt to set value 

* It is an error to attempt to set an element that is beyond len+1 
* The current used length of the array is recorded and returned by len operations
* The array only permits the right type of value to be assigned (this is also checked at runtime to allow compatibility with Lua)
* Accessing out of bounds elements will cause an error, except for setting the len+1 element
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
* Array types may not have meta methods - this will be enforced at runtime (TODO). 
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
  creates an integer array of specified size, and initializes with initial value. The return type is integer[]

``table.numarray(num_elements, initial_value)``
  creates an number array of specified size, and initializes with initial value. The return type is number[]

Array Slices
++++++++++++
Since release 0.6 Ravi supports array slices. An array slice allows a portion of a Ravi array to be treated as if it is an array - this allows efficient access to the underlying array elements. Following new functions are available:

``table.slice(array, start_index, num_elements)``
  creates a slice from an existing array - allowing efficient access to the underlying array elements.

Slices access the memory of the underlying array; hence as soon as a slice is created, the underlying array becomes fixed in size. This ensures that the array memory cannot be reallocated while a slice is referring to it. Note that once the array becomes fixed size it cannot go back to being dynamic as Ravi does not track the slices that refer to arrays. 

Slices cannot extend the array size for the same reasons above.

The type of a slice is the same as that of the underlying array - hence slices get the same optimized JIT operations for array access.

Each slice holds an internal reference to the underlying array to ensure that the garbage collector does not reclaim the array while there are slices pointing to it.

For an example use of slices please see the `matmul1.ravi <https://github.com/dibyendumajumdar/ravi/blob/master/ravi-tests/matmul1.ravi>`_ benchmark program in the repository. Note that this feature is highly experimental and not very well tested.
  
Examples
++++++++
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
++++++++++++++++++++++++++++++
To keep with Lua's dynamic nature Ravi uses a mix of compile type checking and runtime type checks. However due to the dynamic nature of Lua, compilation happens at runtime anyway so effectually all checks are at runtime. 

JIT Compilation
---------------
The LLVM based JIT compiler is functional. Most bytecodes other than bit-wise operators are JIT compiled when using LLVM, but there are restrictions as described in compatibility section below. Everything described below relates to using LLVM as the JIT compiler.
 
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
``ravi.compile(func[, options])``
  compiles a Lua function if possible, returns ``true`` if compilation was successful. ``options`` is an optional table with compilation options - in particular ``omitArrayGetRangeCheck`` - which disables range checks in array get operations to improve performance in some cases. 
``ravi.iscompiled(func)``
  returns the JIT status of a function
``ravi.dumplua(func)``
  dumps the Lua bytecode of the function
``ravi.dumpir(func)``
  dumps the IR of the compiled function (only if function was compiled; only LLVM version)
``ravi.dumpasm(func)``
  dumps the machine code using the currently set optimization level (only if function was compiled; only LLVM)
``ravi.optlevel([n])``
  sets LLVM optimization level (0, 1, 2, 3); defaults to 2
``ravi.sizelevel([n])``
  sets LLVM size level (0, 1, 2); defaults to 0
``ravi.tracehook([b])``
  Enables support for line hooks via the debug api. Note that enabling this option will result in inefficient JIT as a call to a C function will be inserted at beginning of every Lua bytecode 
  boundary; use this option only when you want to use the debug api to step through code line by line

Compatibility with Lua
----------------------
Ravi should be able to run all Lua 5.3 programs in interpreted mode, but there are some differences: 

* Ravi supports optional typing and enhanced types such as arrays (described above). Programs using these features cannot be run by standard Lua. However all types in Ravi can be passed to Lua functions; operations on Ravi arrays within Lua code will be subject to restrictions as described in the section above on arrays. 
* Values crossing from Lua to Ravi will be subjected to typechecks should these values be assigned to typed variables.
* Upvalues cannot subvert the static typing of local variables (issue #26)
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

When JIT compilation is enabled some things will not work:

* You cannot yield from a compiled function as compiled code does not support coroutines (issue 14); as a workaround Ravi will only execute JITed code from the main Lua thread; any secondary threads (coroutines) execute in interpreter mode.
* In JITed code tailcalls are implemented as regular calls so unlike Lua VM which supports infinite tail recursion JIT compiled code only supports tail recursion to a depth of about 110 (issue #17)

Build Dependencies - LLVM version
---------------------------------

* CMake
* LLVM 3.7 

The build is CMake based.

Building LLVM on Windows
------------------------
I built LLVM 3.7.0 from source. I used the following sequence from the VS2015 command window::

  cd \github\llvm
  mkdir build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=c:\LLVM37 -DLLVM_TARGETS_TO_BUILD="X86" -G "Visual Studio 14 Win64" ..  

I then opened the generated solution in VS2015 and performed a INSTALL build from there. 
Note that if you perform a Release build of LLVM then you will also need to do a Release build of Ravi otherwise you will get link errors.

Building LLVM on Ubuntu
-----------------------
On Ubuntu I found that the official LLVM distributions don't work with CMake. The CMake config files appear to be broken.
So I ended up downloading and building LLVM 3.7.0 from source and that worked. The approach is similar to that described for MAC OS X below.

Building LLVM on MAC OS X
-------------------------
I am using Max OSX Yosemite. Pre-requisites are XCode 6.1 and CMake.
Ensure cmake is on the path.
Assuming that LLVM source has been extracted to ``$HOME/llvm-3.7.0.src`` I follow these steps::

  cd llvm-3.7.0.src
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/LLVM -DLLVM_TARGETS_TO_BUILD="X86" ..
  make install

Building Ravi
-------------
I am developing Ravi using Visual Studio 2015 Community Edition on Windows 8.1 64bit, gcc on Unbuntu 64-bit, and clang/Xcode on MAC OS X.

Assuming that LLVM has been installed as described above, then on Windows I invoke the cmake config as follows::

  cd build
  cmake -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=c:\ravi -DLLVM_DIR=c:\LLVM37\share\llvm\cmake -G "Visual Studio 14 Win64" ..

I then open the solution in VS2015 and do a build from there.

On Ubuntu I use::

  cd build
  cmake -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/share/llvm/cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
  make

On MAC OS X I use::

  cd build
  cmake -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/share/llvm/cmake -DCMAKE_BUILD_TYPE=Release -G "Xcode" ..

I open the generated project in Xcode and do a build from there.

Build Artifacts
---------------
The Ravi build creates a shared library, the Lua executable and some test programs.

The ``lua`` command recognizes following environment variables. Note that these are only for internal debugging purposes.

``RAVI_DEBUG_EXPR``
  if set to a value this triggers debug output of expression parsing
``RAVI_DEBUG_CODEGEN``
  if set to a value this triggers a dump of the code being generated
``RAVI_DEBUG_VARS``
  if set this triggers a dump of local variables construction and destruction

Also see section above on available API for dumping either Lua bytecode or LLVM IR for compiled code.

Work Plan
---------
* Feb-Jun 2015 - implement JIT compilation using LLVM
* Jun-Jul 2015 - libgccjit based alternative JIT
* Jun-Nov 2015 - testing  
* Dec 2015 - beta release
* 2016 - Focus on creating numeric library bindings - in particular.

    * BLAS and LAPACK
    * GNU Scientific library
    * symengine

License
-------
MIT License for LLVM version.

Language Syntax - Future work
-----------------------------
Since the reason for introducing optional static typing is to enhance performance primarily - not all types benefit from this capability. In fact it is quite hard to extend this to generic recursive structures such as tables without encurring significant overhead. For instance - even to represent a recursive type in the parser will require dynamic memory allocation and add great overhead to the parser.

From a performance point of view the only types that seem worth specializing are:

* integer (64-bit int)
* number (double)
* array of integers
* array of numbers

Implementation Strategy
-----------------------
I want to build on existing Lua types rather than introducing completely new types to the Lua system. I quite like the minimalist nature of Lua. However, to make the execution efficient I am adding new type specific opcodes and enhancing the Lua parser/code generator to encode these opcodes only when types are known. The new opcodes will execute more efficiently as they will not need to perform type checks. Morever, type specific instructions will lend themselves to more efficient JIT compilation.

I am adding new opcodes that cover arithmetic operations, array operations, variable assignments, etc..

Modifications to Lua Bytecode structure
---------------------------------------
An immediate issue is that the Lua bytecode structure has a 6-bit opcode which is insufficient to hold the various opcodes that I will need. Simply extending the size of this is problematic as then it reduces the space available to the operands A B and C. Furthermore the way Lua bytecodes work means that B and C operands must be 1-bit larger than A - as the extra bit is used to flag whether the operand refers to a constant or a register. (Thanks to Dirk Laurie for pointing this out). 

I am amending the bit mapping in the 32-bit instruction to allow 9-bits for the byte-code, 7-bits for operand A, and 8-bits for operands B and C. This means that some of the Lua limits (maximum number of variables in a function, etc.) have to be revised to be lower than the default.

New OpCodes
-----------
The new instructions are specialised for types, and also for register/versus constant. So for example ``OP_RAVI_ADDFI`` means add ``number`` and ``integer``. And ``OP_RAVI_ADDFF`` means add ``number`` and ``number``. The existing Lua opcodes that these are based on define which operands are used.

Example::

  local i=0; i=i+1

Above standard Lua code compiles to::

  [0] LOADK A=0 Bx=-1
  [1] ADD A=0 B=0 C=-2
  [2] RETURN A=0 B=1

We add type info using Ravi extensions::

  local i:integer=0; i=i+1

Now the code compiles to::

  [0] LOADK A=0 Bx=-1
  [1] ADDII A=0 B=0 C=-2
  [2] RETURN A=0 B=1

Above uses type specialised opcode ``OP_RAVI_ADDII``. 


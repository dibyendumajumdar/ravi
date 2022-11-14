Ravi User Manual
================

.. contents:: Table of Contents
   :depth: 2
   :backlinks: top

------------
Introduction
------------

Ravi is based on Lua 5.3. Additionally some features of Lua 5.4 have been back-ported to Ravi. 
This manual describes the enhancements available in Ravi compared to the Lua 5.3 baseline. 
Before continuing with this manual, please read the `Lua 5.3 manual <https://www.lua.org/manual/5.3/>`_ .

------------
Organization
------------

This manual covers following topics:

* Ravi Language/ Virtual Machine Extensions
* JIT and AOT compilation capabilities
* How to obtain and build Ravi

--------------------------
Ravi Extensions to Lua 5.3
--------------------------

Ravi provides following enhancements to Lua 5.3.

* Optional static typing
* ``defer`` statement (some compliation modes)
* Generational Garbage Collector from Lua 5.4
* JIT and AOT compilation support
* A set of batteries, i.e., curated set of libraries.

----------------------
Optional Static Typing
----------------------
Ravi allows you optionally to annotate ``local`` variables and function parameters with types. 

Function return types cannot be annotated because in Lua, functions are un-named values and there is no reliable way for a static analysis of a function call's return value.

The supported type-annotations are as follows:

``integer``
  denotes an integral value of 64-bits.
``number``
  denotes a double (floating point) value of 8 bytes.
``integer[]``
  denotes an array of integers
``number[]``
  denotes an array of numbers
``table``
  denotes a Lua table
``string``
  denotes a string
``closure``
  denotes a function
``Name [. Name]*``
  Denotes a string that has a `metatable registered <https://www.lua.org/pil/28.2.html>`_ in the Lua registry. This allows userdata
  types to be asserted by their registered names.

General Notes
-------------
* Assignments to type-annotated variables are checked at compile time if possible; when the assignments occur due to a function call,  runtime type-checking is performed
* If function parameters are decorated with types, Ravi performs implicit type assertion checks on those parameters upon function entry. If the assertions fail then runtime errors are raised.
* Ravi performs type-checking of up-values that reference variables that are annotated with types
* To keep with Lua's dynamic nature Ravi uses a mix of compile type-checking and runtime type checks. However, in Lua, compilation happens at runtime anyway so effectively all checks are at runtime. 

Caveats
-------
The Lua C api allows C programmers to manipulate values on the Lua stack. This is incompatible with Ravi's type-checking because the compiler doesn't know about these operations; hence if you need to do such operations from C code, please ensure that values retain their types, or else just write plain Lua code.

Ravi does its best to validate operations performed via the Lua debug api; however, in general, the same caveats apply.

``integer`` and ``number`` types
--------------------------------
* ``integer`` and ``number`` types are automatically initialized to zero rather than ``nil``
* Arithmetic operations on numeric types make use of type-specialized bytecodes that lead to better code-generation
  
``integer[]`` and ``number[]`` array types
------------------------------------------
The array types (``number[]`` and ``integer[]``) are specializations of Lua table with some additional behaviour:

* Arrays must always be initialized:: 

    local t: number[] = {} -- okay
    local t2: number[]     -- error!

  This restriction is placed as otherwise the JIT code would need to insert tests to validate that the variable is not ``nil``.
* Specialised operators to get/set from array types are implemented; these makes array-element access more efficient in JIT mode as the access can be inlined
* Operations on array types can be optimised to specialized bytecode only when the array type is known at compile time. Otherwise regular table access will be used, subject to runtime checks.
* The standard table operations on arrays are checked to ensure that the array type is not subverted
* Array types are not compatible with declared table variables, i.e. the following is not allowed::
  
    local t: table = {}
    local t2: number[] = t  -- error!

    local t3: number[] = {}
    local t4: table = t3    -- error!

  But the following is okay::

    local t5: number[] = {}
    local t6 = t5           -- t6 treated as table

  These restrictions are applied because declared table and array types generate optimized code that makes assumptions about keys and values. The generated code would be incorrect if the types were not as expected.
* Indices >= 1 should be used when accessing array-elements. Ravi arrays (and slices) have a hidden slot at index 0 for performance reasons, but this is not visible in ``pairs()`` or ``ipairs()``, or when initializing an array using a literal initializer; only direct access via the ``[]`` operator can see this slot.   
* An array will grow automatically (unless the array was created as fixed length using ``table.intarray()`` or ``table.numarray()``) if the user sets the element just past the array length::

    local t: number[] = {} -- dynamic array
    t[1] = 4.2             -- okay, array grows by 1
    t[5] = 2.4             -- error! as attempt to set value 

* It is an error to attempt to set an element that is beyond ``len+1`` on dynamic arrays; for fixed length arrays attempting to set elements at positions greater than ``len`` will cause an error.
* The current used length of the array is recorded and returned by the ``len`` operation
* The array only permits the right type of value to be assigned (this is also checked at runtime to allow compatibility with Lua)
* Accessing out of bounds elements will cause an error, except for setting the ``len+1`` element on dynamic arrays. There is a compiler option to omit bounds checking on reads.
* It is possible to pass arrays to functions and return arrays from functions. Arrays passed to functions appear as Lua tables inside those functions if the parameters are untyped - however the tables will still be subject to restrictions as above. If the parameters are typed then the arrays will be recognized at compile time::

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

* Array types ignore ``__index``, ``__newindex`` and ``__len`` metamethods.
* Array types cannot be set as metatables for other values. 
* ``pairs()`` and ``ipairs()`` work on arrays as normal
* There is no way to delete an array element.
* The array data is stored in contiguous memory just like native C arrays; morever the garbage collector does not scan the array data

The following library functions allow creation of array types of defined length.

``table.intarray(num_elements, initial_value)``
  creates an integer array of specified size, and initializes with initial value. The return type is integer[]. The size of the array cannot be changed dynamically, i.e. it is fixed to the initial specified size. This allows slices to be created on such arrays.

``table.numarray(num_elements, initial_value)``
  creates an number array of specified size, and initializes with initial value. The return type is number[]. The size of the array cannot be changed dynamically, i.e. it is fixed to the initial specified size. This allows slices to be created on such arrays.

``table`` type
--------------
A declared table (as shown below) has the following nuances.

* Like array types, a variable of ``table`` type must be initialized::

    local t: table = {}

* Declared tables allow specialized opcodes for table gets involving integer and short literal string keys; these opcodes result in more efficient JIT code
* Array types are not compatible with declared table variables, i.e. the following is not allowed::
   
    local t: table = {}
    local t2: number[] = t -- error!

* When short string literals are used to access a table element, specialized bytecodes are generated that may be more efficiently JIT compiled::

    local t: table = { name='dibyendu'}
    print(t.name) -- The GETTABLE opcode is specialized in this case

* As with array types, specialized bytecodes are generated when integer keys are used

``string``, ``closure`` and user-defined types
----------------------------------------------
These type-annotations have experimental support. They are not always statically enforced. Furthermore using these types does not affect the JIT code-generation, i.e. variables annotated using these types are still treated as dynamic types. 

The scenarios where these type-annotations have an impact are:

* Function parameters containing these annotations lead to type assertions at runtime.
* The type assertion operator @ can be applied to these types - leading to runtime assertions.
* Annotating ``local`` declarations results in type assertions.
* All three types above allow ``nil`` assignment.

The main use case for these annotations is to help with type-checking of larger Ravi programs. These type checks, particularly the one for user defined types, are executed directly by the VM and hence are more efficient than performing the checks in other ways. 

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
  
  -- The following demonstrates an error caused by the type-checking
  -- Note that this error is raised at runtime
  function x() 
    local s: string
    -- call a function that returns integer value
    -- and try to assign to s
    s = (function() return 1 end)() 
  end
  x() -- will fail at runtime

Type Assertions
---------------
Ravi does not support defining new types, or structured types based on tables. This creates some practical issues when dynamic types are mixed with static types. For example::

  local t = { 1,2,3 }
  local i: integer = t[1] -- generates an error

The above code generates an error as the compiler does not know that the value in ``t[1]`` is an integer. However often we as programmers know the type that is expected, it would be nice to be able to tell the compiler what the expected type of ``t[1]`` is above. To enable this Ravi supports type assertion operators. A type assertion is introduced by the '``@``' symbol, which must be followed by the type name. So we can rewrite the above example as::

  local t = { 1,2,3 }
  local i: integer = @integer( t[1] )

The type assertion operator is a unary operator and binds to the expression following the operator. We use the parenthesis above to ensure that the type assertion is applied to ``t[1]`` rather than ``t``. More examples are shown below::

  local a: number[] = @number[] { 1,2,3 }
  local t = { @number[] { 4,5,6 }, @integer[] { 6,7,8 } }
  local a1: number[] = @number[]( t[1] )
  local a2: integer[] = @integer[]( t[2] )

For a real example of how type assertions can be used, please have a look at the test program `gaussian2.lua <https://github.com/dibyendumajumdar/ravi/blob/master/ravi-tests/gaussian2.lua>`_ 

Array Slices
------------
Since release 0.6 Ravi supports array slices. An array slice allows a portion of a Ravi array to be treated as if it is an array - this allows efficient access to the underlying array-elements. The following new functions are available:

``table.slice(array, start_index, num_elements)``
  creates a slice from an existing *fixed size* array - allowing efficient access to the underlying array-elements.

Slices access the memory of the underlying array; hence a slice can only be created on fixed size arrays (constructed by ``table.numarray()`` or ``table.intarray()``). This ensures that the array memory cannot be reallocated while a slice is referring to it. Ravi does not track the slices that refer to arrays - slices get garbage collected as normal. 

Slices cannot extend the array size for the same reasons above.

The type of a slice is the same as that of the underlying array - hence slices get the same optimized JIT operations for array access.

Each slice holds an internal reference to the underlying array to ensure that the garbage collector does not reclaim the array while there are slices pointing to it.

For an example use of slices please see the `matmul1_ravi.lua <https://github.com/dibyendumajumdar/ravi/blob/master/ravi-tests/matmul1_ravi.lua>`_ benchmark program in the repository. Note that this feature is highly experimental and not very well tested.
  
Type Annotation Examples
------------------------
Example of code that works - you can copy this to the command line input::

  function tryme()
    local i,j = 5,6
    return i,j
  end
  local i:integer, j:integer = tryme(); print(i+j)

When values from a function call are assigned to a typed variable, an implicit type coercion takes place. In the above example an error would occur if the function returned values that could not converted to integers.

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

-----------------------
The ``defer`` statement
-----------------------

A new addition to Ravi is the ``defer`` statement. The statement has the form::

   defer
     block
   end

Where ``block`` is a set of Lua statements.

The ``defer`` statement creates an anonymous ``closure`` that will be invoked when the enclosing scope is exited, whether
normally or because of an error. 

Example::

   y = 0
   function x()
     defer y = y + 1 end
     defer y = y + 1 end
   end
   x()
   assert(y == 2)
   
``defer`` statements are meant to be used for releasing resources in a deterministic manner. The syntax and functionality is
inspired by the similar statement in the Go language. The implementation is based upon Lua 5.4.

Note that the ``defer`` statement should be considered a beta feature not yet ready for production use as it is undergoing testing.

----------
Embedded C
----------

This feature is only available when using the new Compiler framework JIT described later in this manual.
It is not available in the interpreter or in the ByteCode JIT compiler.

New keywords
------------

New keywords ``C__decl``, ``C__unsafe``, ``C__new`` have been added to the language.

``C__decl`` 
  allows C type declarations via a string argument. A restriction is imposed that the declared types contain no pointers or unions. 
  All type declarations in a chunk of Ravi code are amalgamated in the generated code, hence duplicate declarations will result in errors.
  Struct declarations can have a flexible array member. The size of this flexible array member is determined when creating a new object using ``C__new()``.
``C__unsafe`` 
  takes a list of symbols and a C code in string argument. The C code may not make function calls or attempt to return or goto.
``C__new`` 
  allows a userdata type of given struct type to be created. The struct type should have been declared before. A size argument is required; 
  when the struct type has a flexible array member, the size specifies the dimension of this array member, otherwise the size defines whether 
  a single object will be created or an array.

Example Usage
-------------

Following example illustrates several features of the new syntax::

  C__decl [[
    typedef struct {
      int m,n;
      double data[];
    } Matrix;
  ]] 
  
  MatrixFunctions = {}
  function MatrixFunctions.new(m: integer, n: integer)
    local M = C__new('Matrix', m*n)
    C__unsafe(m,n,M) [[
        Matrix *matrix = (Matrix *)M.ptr;
        matrix->m = m;
        matrix->n = n;
        for (int i = 0; i < m*n; i++)
            matrix->data[i] = 0.0;
    ]]
    return M
  end
  
  function MatrixFunctions.dim(M)
    local m: integer
    local n: integer
    C__unsafe(m,n,M) [[
        Matrix *matrix = (Matrix *)M.ptr;
        m = matrix->m;
        n = matrix->n;
    ]]
    return m, n
  end
  
  local M = MatrixFunctions.new(10,11)
  local m, n = MatrixFunctions.dim(M)
  assert(m == 10)
  assert(n == 11)

Type Mapping
------------

When accessing userdata, string or Ravi array types, following implicit types are used::

  // For userdata and string types
  typedef struct {
     char *ptr;
    unsigned int len;
  } Ravi_StringOrUserData;
  
  // For integer[]
  typedef struct {
    lua_Integer *ptr;
    unsigned int len;
  } Ravi_IntegerArray;
  
  // For number[]  
  typedef struct {
    lua_Number *ptr;
    unsigned int len;
  } Ravi_NumberArray;

Each symbol argument to ``C__unsafe`` is made available in the C code.

* Primitive integer or floating point values have the types ``lua_Integer`` and ``lua_Number`` respectively.
* Userdata or string types are made available as ``Ravi_StringOrUserData`` structure.
* ``integer[]`` and ``number[]`` arrays are made available as ``Ravi_IntegerArray`` and ``Ravi_NumberArray`` respectively.

Values assigned to primitive types are made visible in Ravi code. The other supported types are reference types, hence any updates become 
visible in Ravi code, however, making changes to Lua strings is not permitted (although this is not yet enforced).

For string, full userdata, ``integer[]`` and ``number[]`` array types, a len attribute is populated. For lightweight userdata, the len attribute 
will be set to 0.

C Parser
--------

This feature uses a custom version of the `chibicc <https://github.com/rui314/chibicc>`_ project to parse and validate the C code snippets. 
The goal is to perform a sanity check of the C code snippet before merging it into the generated code.

The parser enforces some constraints:

* There is no pre-processor support; this is deliberate
* The parser prevents function calls; this rule may be relaxed in future to allow calls to simple C standard library functions
* The parser enforces that the interaction between Ravi and C is limited to userdata types, primitive types (number and integer), 
  primitive arrays (integer[] and number[]) and strings. Access to Lua tables is not supported.

-------------------------------
JIT and AOT Compilation Support
-------------------------------

Ravi uses MIR as the JIT engine, and has two different JIT pipelines. The features and capabilities differ across the Interpreter stack and the JIT pipelines,
and are summarized below.

ByteCode JIT
  This is the original JIT pipeline available since 2015. It works by translating Ravi Interpreter bytecodes to machine code.

Compiler framework JIT
  This is a work-in-progress alpha quality JIT pipeline that translates Ravi source code to an AST followed by Intermediate Code and finally to machine code.

+---------------------------------------------------------------+----------------------+----------------------+--------------------------+
|  Feature                                                      |  Interpreter         |  ByteCode JIT        |  Compiler framework JIT  |
+===============================================================+======================+======================+==========================+
|  Support for Lua debug api                                    |  Yes                 |  No                  |  No                      |
+---------------------------------------------------------------+----------------------+----------------------+--------------------------+
|  Support for coroutines                                       |  Yes                 |  No                  |  No                      |
+---------------------------------------------------------------+----------------------+----------------------+--------------------------+
|  Support for ``defer`` statement                              |  Yes                 |  Yes                 |  No                      |
+---------------------------------------------------------------+----------------------+----------------------+--------------------------+
|  Support for embedded C code                                  |  No                  |  No                  |  Yes                     |
+---------------------------------------------------------------+----------------------+----------------------+--------------------------+
|  Ravi Bytecodes                                               |  Yes                 |  Yes                 |  No                      |
+---------------------------------------------------------------+----------------------+----------------------+--------------------------+
|  AOT Compilation to native shared libraries                   |  No                  |  No                  |  Yes                     |
+---------------------------------------------------------------+----------------------+----------------------+--------------------------+
|  JIT Compilation to native code                               |  No                  |  Yes                 |  Yes                     |
+---------------------------------------------------------------+----------------------+----------------------+--------------------------+
|  JIT engine for generating machine code                       |  N/a                 |  MIR                 |  MIR                     |
+---------------------------------------------------------------+----------------------+----------------------+--------------------------+

----------------
ByteCode JIT API
----------------
This JIT pipelines works by translating Ravi Interpreter bytecodes to machine code via intermediate C code compiled using MIR.
The JIT can be used in two modes:

auto mode
  in this mode the compiler decides when to compile a Lua function. The current implementation is very simple - 
  any Lua function call is checked to see if the bytecodes contained in it can be compiled. If this is true then 
  the function is compiled provided either a) function has a fornum loop, or b) it is largish (greater than 150 bytecodes) 
  or c) it is being executed many times (> 50). Because of the simplistic behaviour performance the benefit of JIT
  compilation is only available if the JIT compiled functions will be executed many times so that the cost of JIT 
  compilation can be amortized.   
manual mode
  in this mode user must explicitly request compilation. This is the default mode. This mode is suitable for library 
  developers who can pre compile the functions in library module table.

A ByteCode JIT api is described below:

``ravi.jit([b])``
  returns enabled setting of JIT compiler; also enables/disables the JIT compiler; defaults to true
``ravi.jitname()``
  returns an identifier for the JIT
``ravi.options()``
  returns a string with compiled options
``ravi.auto([b [, min_size [, min_executions]]])``
  returns setting of auto compilation and compilation thresholds; also sets the new settings if values are supplied; defaults are false, 150, 50.
``ravi.compile(func_or_table[, options])``
  compiles a Lua function (or functions if a table is supplied) if possible, returns ``true`` if compilation was 
  successful for at least one function. ``options`` is an optional table with compilation options - in particular 
  ``omitArrayGetRangeCheck`` - which disables range checks in array get operations to improve performance in some cases. 
  Note that at present if the first argument is a table of functions and has more than 100 functions then only the
  first 100 will be compiled. You can invoke compile() repeatedly on the table until it returns false. Each 
  invocation leads to a new module being created; any functions already compiled are skipped.
``ravi.iscompiled(func)``
  returns the JIT status of a function
``ravi.dumplua(func)``
  dumps the Lua bytecode of the function
``ravi.dumpir(func)``
  dumps the intermediate code of the compiled function; interpretation up to the JIT backend.
``ravi.optlevel([n])``
  sets optimization level (0, 1, 2, 3); the interpretation of this is up to the JIT backend.
``ravi.verbosity([b])``
  Controls the amount of verbose messages generated during compilation.

----------------------
Compiler framework JIT
----------------------

This compiler framework translates Ravi source code to machine code as follows:

* The compiler generates Abstract Syntax Trees (ASTs) in the parsing phase.
* A `new linear Intermediate Representation (IR) <https://github.com/dibyendumajumdar/ravi-compiler/blob/master/docs/linear-ir.md>`_ is produced from the AST.
* Finally the IR is translated to C code, which can be JITed using the MIR backend or compiled ahead-of-time using a standard C compiler such as gcc, clang or MSVC.
* Since the compiler generates C intermediate code, it is possible to embed snippets of C code via special language extensions. For details see `Embedding C <https://github.com/dibyendumajumdar/ravi-compiler/wiki/Embedding-C>`_.

Additional details regarding the new implementation can be found at the `compiler project site <https://github.com/dibyendumajumdar/ravi-compiler>`_.

The new compiler can be invoked in the following ways.

Generate Parse Tree
-------------------

``compiler.parse(code: string)``
  Generates a parse tree for the input Ravi source code. The parse tree is formatted such that it can be executed in Lua/Ravi code by providing suitable function 
  definitions.

JIT Compilation
---------------

``compiler.load(code: string)``
  JIT compiles a chunk of code and returns a closure on the stack representing the compiled output.
``compiler.loadfile(filename: string)``
  Opens the named file and JIT compiles its content as a Lua chunk. Returns a closure on the stack representing the compiled output.
 
AOT Compilation
---------------
  
``compiler.compile(filename: string, mainfunc: string)``
  Compiles the contents of the given filename and generates C code. The function returns the generated code as a string. The supplied ``mainfunc`` will be the name of the main function in the generated code. The generated C code can be compiled using a C compiler to produce a shared library. Note that on Windows, you will need to provide the Ravi Import library as an argument to the linker when generating the shared library. See examples below.
``package.load_ravi_lib(filename: string, mainfunc: string)``
  Loads the shared library with the given filename, and returns the function identified as ``mainfunc``. The returned function can be executed to effectively run the contents of the original input file.

Limitations
-----------

There are some limitations in this approach that you need to be aware of.

* The generated code does not have Lua bytecode. Hence Lua interpreter / debugger cannot do anything with these functions.
* The generated code is dependent upon the VM specifics - in particular it relies upon the VM stack setup, the call stack setup, etc. There are additional support functions needed by the compiled code, including data types supported by Ravi. For these reasons, only Ravi can execute the AOT compiled code.
* There is no support for Lua hooks in the generated code, as there are no Lua bytecodes.
* Coroutines are not supported; the generated code can only be executed on the main thread, and moreover cannot yield either.
* Var args are not yet supported. 
* The 'defer' statement is not supported since the compiler does not always preserve values on Lua stack / optimizes to use C stack.
* The Lua debug interface cannot be used to manipulate objects inside the compiled code.


JIT Examples
------------

Here is an example of a simple JIT compile session::

  f = compiler.load("print 'hello'")
  assert(f and type(f) == 'function')
  f()
  
We can inspect the function ``f``::

  > ravi.dumplua(f)

  main <?:0,0> (0 instructions at 000001300E6C9180)
  0 params, 2 slots, 1 upvalue, 0 locals, 2 constants, 0 functions
  constants (2) for 000001300E6C9180:
        1       "print"
        2       "hello"
  locals (0) for 000001300E6C9180:
  upvalues (1) for 000001300E6C9180:
        0       -       1       0
  
For more JIT examples, please have a look at `compiler test cases <https://github.com/dibyendumajumdar/ravi/tree/master/tests/comptests/inputs>`_.

AOT Examples
------------

For AOT compilation, you will need a C compiler, such as ``clang``, ``gcc`` or ``MSVC``.

Here is an example session on Windows::

  -- The session must be started inside a command prompt with 64-bit MSVC enabled
  -- We assume Ravi is compiled and installed at /Software/ravi on the local drive.

  -- Helper functions
  function readall(file)
    local f = assert(io.open(file, "rb"))
    local content = f:read("*all")
    f:close()
    return content
  end

  function writestring(file,str)
    local f = io.open(file,'w')
    f:write(str)
    f:close()
  end

  function comptoC(inputfile, outputfile)
    local chunk = readall(inputfile)
    local compiled = compiler.compile(chunk, 'mymain')
    if not compiled then
        error ('Failed to compile')
    end
    writestring(outputfile, compiled)
  end

  comptoC('gaussian2_lib.lua', 'gaussian2_lib.c')
  -- Note compiler options may need to change for Release vs Debug builds
  assert(os.execute("cl /c /Os /D WIN32 /DMD gaussian2_lib.c"))
  assert(os.execute("link /LIBPATH:/Software/ravi/lib libravi.lib /DLL /MACHINE:X64 /OUT:gaussian2_lib.dll gaussian2_lib.obj"))
  local f = package.load_ravi_lib('gaussian2_lib.dll', 'mymain') -- load shared library and obtain reference to mymain
  assert(f and type(f) == 'function')
  local glib = f() -- execute mymain
  assert(glib and type(glib) == 'table')

For the same example in a Linux environment, have a look at the `AOT Examples <https://github.com/dibyendumajumdar/ravi/tree/master/aot-examples>`_.


-------------------------------
Generational Garbage Collection
-------------------------------
Ravi incorporates the generational garbage collector from Lua 5.4.
Please refer to the Lua 5.4 manual regarding the api changes to support generational collection.

Note that by default, Ravi uses the incremental garbage collector. The generational collector is new and may have bugs in its implementation
(a few bugs have been reported on Lua mailing lists, fixes are being applied to Ravi when applicable).

To switch to generational GC::

   collectgarbage("generational")

To switch to incremental GC::

   collectgarbage("incremental")


----------------
C API Extensions
----------------
Ravi provides following C API extensions:

::

   LUA_API void  (ravi_pushcfastcall)(lua_State *L, void *ptr, int tag);

   /* Allowed tags - subject to change. Max value is 128. Note that
      each tag requires special handling in ldo.c */
   enum {
     RAVI_TFCF_EXP = 1,
     RAVI_TFCF_LOG = 2,
     RAVI_TFCF_D_D = 3,
   };

   /* Create an integer array (specialization of Lua table)
    * of given size and initialize array with supplied initial value
    */
   LUA_API void ravi_create_integer_array(lua_State *L, int narray,
                                          lua_Integer initial_value);

   /* Create an number array (specialization of Lua table)
    * of given size and initialize array with supplied initial value
    */
   LUA_API void ravi_create_number_array(lua_State *L, int narray,
                                         lua_Number initial_value);

   /* Create a slice of an existing array
    * The original table containing the array is inserted into the
    * the slice as a value against special key so that
    * the parent table is not garbage collected while this array contains a
    * reference to it
    * The array slice starts at start but start-1 is also accessible because of the
    * implementation having array values starting at 0.
    * A slice must not attempt to release the data array as this is not owned by
    * it,
    * and in fact may point to garbage from a memory allocater's point of view.
    */
   LUA_API void ravi_create_slice(lua_State *L, int idx, unsigned int start,
                                  unsigned int len);

   /* Tests if the argument is a number array
    */
   LUA_API int ravi_is_number_array(lua_State *L, int idx);

   /* Tests if the argument is a integer array
   */
   LUA_API int ravi_is_integer_array(lua_State *L, int idx);

   /* Get the raw data associated with the number array at idx.
    * Note that Ravi arrays have an extra element at offset 0 - this
    * function returns a pointer to &data[0]. The number of
    * array elements is returned in length.
    */
   typedef struct {
     lua_Number *data;
     unsigned int length;
   } Ravi_NumberArray;
   LUA_API void ravi_get_number_array_rawdata(lua_State *L, int idx, Ravi_NumberArray *array_data);

   /* Get the raw data associated with the integer array at idx.
    * Note that Ravi arrays have an extra element at offset 0 - this
    * function returns a pointer to &data[0]. The number of
    * array elements is returned in length.
    */
   typedef struct {
     lua_Integer *data;
     unsigned int length;
   } Ravi_IntegerArray;
   LUA_API void ravi_get_integer_array_rawdata(lua_State *L, int idx, Ravi_IntegerArray *array_data);

   /* API to set the output functions used by Lua / Ravi
    * This allows the default implementations to be overridden
    */
   LUA_API void ravi_set_writefuncs(lua_State *L, ravi_Writestring writestr, ravi_Writeline writeln, ravi_Writestringerror writestringerr);

   /* Following are the default implementations */
   LUA_API void ravi_writestring(lua_State *L, const char *s, size_t len);
   LUA_API void ravi_writeline(lua_State *L);
   LUA_API void ravi_writestringerror(lua_State *L, const char *fmt, const char *p);

   /* The debugger can set some data - but only once */
   LUA_API void ravi_set_debugger_data(lua_State *L, void *data);
   LUA_API void *ravi_get_debugger_data(lua_State *L);

   /* Takes a function parameter and outputs the bytecodes to stdout */
   LUA_API void ravi_dump_function(lua_State *L);
   /* Takes a function parameter and returns a table of lines containing bytecodes for the function */
   LUA_API int ravi_list_code(lua_State *L);
   /* Returns a table with various system limits */
   LUA_API int ravi_get_limits(lua_State *L);



Ravi Extensions to Lua 5.3
==========================

.. contents:: Table of Contents
   :depth: 2
   :backlinks: top

Optional Static Typing
----------------------
Ravi allows you to optionally annotate ``local`` variables and function parameters with static types. 

Function return types cannot be annotated because in Lua, functions are un-named values and there is no reliable way for a static analysis of a function call's return value.

The supported type annotations are as follows:

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
``Name [. Name]+``
  Denotes a string that has a `metatable registered under Name <https://www.lua.org/pil/28.2.html>`_ in the Lua registry. 
  The Name must be a valid Lua name with the exception that periods are allowed in the name. 

General Notes
-------------
* Assignments to typed variables are statically checked if possible; when the assignments occur due to a function call then runtime type checking is performed
* If function parameters are decorated with types, Ravi performs implicit type assertion checks against those parameters upon function entry. If the assertions fail then runtime errors are raised.
* Even if a typed variable is captured in a closure its type must be respected
* To keep with Lua's dynamic nature Ravi uses a mix of compile type checking and runtime type checks. However due to the dynamic nature of Lua, compilation happens at runtime anyway so effectively all checks are at runtime. 

Caveats
-------
The Lua C api allows C programmers to manipulate values on the Lua stack. This is incompatible with Ravi's type checking because the compiler doesn't know about these operations; hence if you need to do such operations from C code, please ensure that values retain their types, or else just write plain Lua code.

Ravi does its best to validate operations performed via the Lua debug api; however in general, the same caveats apply.

``integer`` and ``number`` types
--------------------------------
* ``integer`` and ``number`` types are automatically initialized to zero rather than ``nil``
* Arithmetic operations on numeric types make use of type specific bytecodes that lead to more efficient JIT compilation
  
``integer[]`` and ``number[]`` array types
------------------------------------------
The array types (``number[]`` and ``integer[]``) are specializations of Lua table with some additional behaviour:

* Arrays must always be initialized:: 

    local t: number[] = {} -- okay
    local t2: number[]     -- error!

  This restriction is placed as otherwise the JIT code would need to insert tests to validate that the variable is not ``nil``.
* Specialised operators to get/set from array types are implemented; these makes array element access more efficient in JIT mode as the access can be inlined
* Operations on array types can be optimised to special bytecode and JIT only when the array type is statically known. Otherwise regular table access will be used subject to runtime checks.
* The standard table operations on arrays are checked to ensure that the type is not subverted
* Array types are not compatible with declared table variables, i.e. following is not allowed::
  
    local t: table = {}
    local t2: number[] = t  -- error!

    local t3: number[] = {}
    local t4: table = t3    -- error!

  But following is okay::

    local t5: number[] = {}
    local t6 = t5           -- t6 treated as table

  These restrictions are applied because declared table and array types generate optimized JIT code which make assumptions about the keys and values. The generated JIT code would be incorrect if the types were not as expected.
* Indices >= 1 should be used when accessing array elements. Ravi arrays (and slices) have a hidden slot at index 0 for performance reasons, but this is not visible in ``pairs()`` or ``ipairs()``, or when initializing an array using a literal initializer; only direct access via the ``[]`` operator can see this slot.   
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

* Array types ignore ``__index``, ``__newindex`` and ``__len`` metamethods.
* Array types cannot be set as metatables for other values. 
* ``pairs()`` and ``ipairs()`` work on arrays as normal
* There is no way to delete an array element.
* The array data is stored in contiguous memory just like native C arrays; morever the garbage collector does not scan the array data

Following library functions allow creation of array types of defined length.

``table.intarray(num_elements, initial_value)``
  creates an integer array of specified size, and initializes with initial value. The return type is integer[]. The size of the array cannot be changed dynamically, i.e. it is fixed to the initial specified size. This allows slices to be created on such arrays.

``table.numarray(num_elements, initial_value)``
  creates an number array of specified size, and initializes with initial value. The return type is number[]. The size of the array cannot be changed dynamically, i.e. it is fixed to the initial specified size. This allows slices to be created on such arrays.

``table`` type
--------------
A declared table (as shown below) has following nuances.

* Like array types, a variable of ``table`` type must be initialized::

    local t: table = {}

* Declared tables allow specialized opcodes for table gets involving integer and short literal string keys; these opcodes result in more efficient JIT code
* Array types are not compatible with declared table variables, i.e. following is not allowed::
   
    local t: table = {}
    local t2: number[] = t -- error!

* When short string literals are used to access a table element, specialized bytecodes are generated that may be more efficiently JIT compiled::

    local t: table = { name='dibyendu'}
    print(t.name) -- The GETTABLE opcode is specialized in this case

* As with array types, specialized bytecodes are generated when integer keys are used

``string``, ``closure`` and user-defined types
----------------------------------------------
These type annotations have experimental support. They are not always statically enforced. Furthermore using these types does not affect the JIT code generation, i.e. variables annotated using these types are still treated as dynamic types. 

The scenarios where these type annotations have an impact are:

* Function parameters containing these annotations lead to type assertions at runtime.
* The type assertion operator @ can be applied to these types - leading to runtime assertions.
* Annotating ``local`` declarations results in type assertions.
* All three types above allow ``nil`` assignment.

The main use case for these annotations is to help with type checking of larger Ravi programs. These type checks, particularly the one for user defined types, are executed directly by the VM and hence are more efficient than performing the checks in other ways. 

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

Array Slices
------------
Since release 0.6 Ravi supports array slices. An array slice allows a portion of a Ravi array to be treated as if it is an array - this allows efficient access to the underlying array elements. Following new functions are available:

``table.slice(array, start_index, num_elements)``
  creates a slice from an existing *fixed size* array - allowing efficient access to the underlying array elements.

Slices access the memory of the underlying array; hence a slice can only be created on fixed size arrays (constructed by ``table.numarray()`` or ``table.intarray()``). This ensures that the array memory cannot be reallocated while a slice is referring to it. Ravi does not track the slices that refer to arrays - slices get garbage collected as normal. 

Slices cannot extend the array size for the same reasons above.

The type of a slice is the same as that of the underlying array - hence slices get the same optimized JIT operations for array access.

Each slice holds an internal reference to the underlying array to ensure that the garbage collector does not reclaim the array while there are slices pointing to it.

For an example use of slices please see the `matmul1_ravi.lua <https://github.com/dibyendumajumdar/ravi/blob/master/ravi-tests/matmul1_ravi.lua>`_ benchmark program in the repository. Note that this feature is highly experimental and not very well tested.
  
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

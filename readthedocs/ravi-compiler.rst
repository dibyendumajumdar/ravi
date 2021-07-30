New Compiler Framework in Ravi
==============================

A new compiler framework in Ravi is now available as a preview feature.

The new framework allows both JIT and AOT compilation of Lua/Ravi code.
A large subset of the language is supported.

The new compiler differs from the standard Lua/Ravi compiler in following ways:

* Unlike the standard compiler, the new compiler generates Abstract Syntax Trees (ASTs) in the parsing phase.
* A new linear Intermediate Representation (IR) is produced from the AST.
* Finally the IR is translated to C code, which can be JITed using the MIR backend or compiled ahead-of-time using a standard C compiler such as gcc, clang or MSVC.

The new compiler can be invoked in the following ways.

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
* The generated code knows about the internals of the Ravi VM - in particular it relies upon the VM stack, GC, etc. Hence the current version of the output is not compatible with Lua 5.4 as the Lua authors changed the stack layout in this version. It is possible to amend the generated code to support Lua 5.3 or Lua 5.4 but without additional support functions there will be no way to execute these in standard Lua. For these reasons, only Ravi can execute the AOT compiled code.
* There is no support for Lua hooks in the generated code, as there are no Lua bytecodes.
* Coroutines are not supported; the generated code can only be executed on the main thread, and moreover cannot yield either.
* Var args are not yet supported. 
* The Lua debugger cannot be used to debug the compiled code.
* The Lua debug interface cannot be used to manipulate objects inside the compiled code.
* The compiler is still in early stages of development, hence bugs can be expected.


JIT Examples
------------

Here is an example of a simple JIT compile session::

  f = compiler.load("print 'hello'")
  assert(f and type(f) == 'function')
  f()
  
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



  

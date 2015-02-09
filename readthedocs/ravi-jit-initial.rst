LLVM First Steps
================

Although there appears to be a lot of documentation in the LLVM site surprisingly some basic information is hard to find.
The main source of guidance for creating a JIT is in the example toy language 
`Kaleidoscope <https://github.com/llvm-mirror/llvm/blob/master/examples/Kaleidoscope/MCJIT/complete/toy.cpp>`_. But here too
there are several versions - so you have to pick the right version that is compatible with the LLVM version you
are using.

A Lua JITed function will execute in the context of Lua. So it needs to be able to access the ``lua_State`` and its various 
structures. So I wanted a sample that demonstrates passing a pointer to a structure and accessing it within the JITed 
function. 

The initial test program I created is meant to be a "hello world" type test but covering the functionility described above.
The test I want to run is::

  // Creating a function that takes pointer to struct as argument
  // The function gets value from one of the fields in the struct
  // And returns it
  // The equivalent C program is:
  //
  // extern int printf(const char *, ...);
  //
  // struct GCObject {
  //   struct GCObject *next;
  //   unsigned char a;
  //   unsigned char b;
  // };
  //
  // int testfunc(struct GCObject *obj) {
  //   printf("value = %d\n", obj->a);
  //   return obj->a;
  // }

You can view the test program at `test_llvm.cpp <https://github.com/dibyendumajumdar/ravi/blob/master/tests/test_llvm.cpp>`_. 
Here is some commentary around it.

I used the new ``MCJIT`` engine in my test. It seems that this engine compiles modules rather than individual
functions - and once compiled a module cannot be modified. So in the Lua context we need to create a new module
everytime we JIT compile a function - or alternatively we JIT compile a whole Lua source file including all its 
functions into a single module. 

I found the blog post `Using MCJIT with Kaleidoscope 
<http://blog.llvm.org/2013/07/using-mcjit-with-kaleidoscope-tutorial.html>`_ useful in understanding some 
finer points about using ``MCJIT``. 

Links
-----
* `Resolving extern functions <http://lists.cs.uiuc.edu/pipermail/llvmdev/2009-July/023559.html>`_
* `Object format issue on Windows <http://lists.cs.uiuc.edu/pipermail/llvmdev/2013-December/068407.html>`_

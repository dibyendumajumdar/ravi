========================
JIT Compilation for Ravi
========================

Introduction
------------
Lua's performance is pretty good as an interpreter. To get real performance improvements we need to switch paradigms and get into JIT compilation. One of the main goals of the optional typing in Ravi is to allow better JIT code generation as type information can be exploited. It is also to allow a more conventional JIT compiler rather than the high-tech trace compilers used in Luajit.

Approach
--------
Primary reason for Ravi's existence is that it needs to be implemented using technology that does not require us writing assembly language code for each platform. The other goal is to allow long term support and maintainability of the JIT compiler. Given these considerations my current plan is to use LLVM as the JIT compilation framework.

Status
------
Project kicked off Feb 2015. I am new to this so progress will be slow and painful. I would like to get this compiler built and working by first half of 2015. As I make progress I will document the results here.

Preparing for LLVM
------------------
I am using Windows as my primary development platform but I also test on Linux using a VM. The first step appears to be to build LLVM from source as the only binary distribution is for CLANG. 

I am using LLVM 3.5.1 release at present. I downloaded the source archive, uncompressed and using CMake GUI created the build configuration. Only item I changed ``CMAKE_INSTALL_PREFIX`` which I set to ``c:\LLVM``.

Note: I had to build the Release version for Ravi to link properly.

After modifying Ravi's ``CMakeLists.txt`` I invoked the cmake config as follows::

  C:\github\ravi\build>cmake -DLLVM_DIR=c:\LLVM\share\llvm\cmake -G "Visual Studio 12 Win64" ..

On Ubuntu I found that the official LLVM distributions don't work with CMake. The CMake config files appear to be broken.

So I ended up downloading and building LLVM 3.5.1 from source and that worked. I used the same approach as on Windows - i.e., set ``CMAKE_INSTALL_PREFIX`` using ``cmake-gui`` and that was about it.

The command to create makefiles was as follows::

  cmake -DLLVM_DIR=/home/user/LLVM/share/llvm/cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..

Creating Lua Structure Mappings
-------------------------------
One of the first tasks appears to be to map Lua structures to LLVM type definitions. Here is an example of how the ``GCObject`` structure may be mapped::

  /* create a GCObject structure as defined in lobject.h */
  // create struct type
  llvm::StructType *myt = llvm::StructType::create(context, "RaviGCObject");
  // we nee pointer to struct as this is the first field
  llvm::PointerType *pmyt = llvm::PointerType::get(myt, 0);
  std::vector<llvm::Type *> elements;
  elements.push_back(pmyt); 
  elements.push_back(llvm::Type::getInt8Ty(context));
  elements.push_back(llvm::Type::getInt8Ty(context));
  // define structure body
  myt->setBody(elements);

Memory Management
-----------------
Curiously LLVM docs do not say much about how memory should be managed. I am still trying to figure this out.


Links
-----
* `Mapping High Level Constructs to LLVM IR <http://llvm.lyngvig.org/Articles/Mapping-High-Level-Constructs-to-LLVM-IR>`_
* `IRBuilder Sample <https://github.com/eliben/llvm-clang-samples/blob/master/src_llvm/experimental/build_llvm_ir.cpp>`_
* `Using MCJIT with Kaleidoscope <http://blog.llvm.org/2013/07/using-mcjit-with-kaleidoscope-tutorial.html>`_

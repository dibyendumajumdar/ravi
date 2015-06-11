========================================
JIT Compilation for Ravi using libgccjit
========================================

Introduction
------------
The latest `gcc 5.1 release <http://gcc.gnu.org/>`_ contains a new component called ``libgccjit``. This basically exposes an API via a shared library to the compilation functions within gcc. 

I am keen to provide support for this in Ravi. From initial look it seems to contain all the features I need to implement a JIT compiler for Ravi. Obviously having implemented the LLVM version it is going to be a little easier as I can mostly do a port of the LLVM version.

License
-------
Ravi with this component will be licensed as GPLv3. Without this component the license will be MIT license.

Why another JIT engine?
-----------------------
Well partly as I feel I have a moral obligation to support gcc, given it has been instrumental in bringing about the OpenSource / Free Software ecosystem. 

Secondly I am always looking for alternatives that will let me reduce the footprint of Ravi. The libgccjit is offered as a shared library - this is a great thing. I hate to have to statically link LLVM. 

LLVM implementation and libgccjit implementation will both be kept in sync so that user can choose either option. 

Building GCC
------------
I am running Ubuntu 14.04 LTS on VMWare virtual machine.

I built gcc 5.1 from source as follows.

1. Unpacked source to ``~/gcc-5.1.0``
2. Created a build folder ``~/buildgcc``
3. Installed various pre-requisites for gcc.
4. Then ran following from inside the build folder::

     ../gcc-5.1.0/configure --prefix=~/local --enable-host-shared --enable-languages=jit,c++ --disable-bootstrap --disable-multilib

5. Next performed the build as follows::

     make
     make install

Current Status
--------------
Work on this started only recently (8 June 2015) so not much to show yet. But expectation is that there will be a working implementation by end June - the strategy is to port the existing LLVM implementation to equivalent libgccjit implementation.

Building Ravi with libgccjit on Linux
-------------------------------------
Note that right now the libgccjit implementation is not yet functional. However you can build Ravi with libgccjit linked in as follows::

  mkdir build
  cd build
  cmake -DLLVM_JIT=OFF -DGCC_JIT=ON ..
  make

Above assumes that gccjit is installed under ``~/local`` as described in Building GCC section above.

A helloworld test program is built. To run it though you need to set your ``PATH`` and ``LD_LIBRARY_PATH`` variables to ``~/local/bin`` and ``~/local/lib`` respectively.

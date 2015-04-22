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

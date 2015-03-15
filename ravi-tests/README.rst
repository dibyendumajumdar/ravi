This folder contains various performance and unit tests.

Unit tests
----------
* ravi_tests1.ravi - contains some basic tests

Performance tests
-----------------
* fornum_test1.lua
* fornum_test2.lua 
* fornum_test2.ravi - with optional types
* fornum_test3.lua 

Following performance tests were obtained from the `The Computer Programming Language Benchmarks Game <http://benchmarksgame.alioth.debian.org/>`_. Original author is `Mike Pall <http://luajit.org/>`_.

* fannkuchen.lua
* fannkuchen.ravi - with optional types

* mandel.lua
* mandel.ravi - with optional types
* mandel1.ravi - modified so that it can be JIT compiled
* mandel1.lua - can be JIT compiled but also compatible with Lua

Example LLVM and Ravi bytecode listings
---------------------------------------
The files ending in ``.ll`` extension are LLVM IR dumps of some of the samples.
In the listings sub-folder you will also find some Ravi bytecode listings.
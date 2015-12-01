This folder contains various performance and unit tests.

Language tests
--------------
* basics.lua - some simple tests
* ravi_tests1.ravi - contains some basic tests
* ravi_errors.ravi - contains tests for error conditions
* bitwise_tests.lua - modified Lua 5.3 tests to exercise JIT on bitwise operations

Performance tests
-----------------
Following are simple loop tests:

* fornum_test1.lua
* fornum_test2.lua 
* fornum_test2.ravi - with optional types
* fornum_test3.lua 

Matrix multiplication test:

* matmul1.lua - matrix multiplication (Lua compatible)
* matmul1.ravi - matrix multiplication (ravi version with static typing)

Following performance tests were obtained from the `The Computer Programming Language Benchmarks Game <http://benchmarksgame.alioth.debian.org/>`_. Original author is `Mike Pall <http://luajit.org/>`_.

* fannkuchen.lua
* fannkuchen.ravi - with optional types

* mandel1.ravi - modified so that it can be JIT compiled
* mandel1.lua - can be JIT compiled but also compatible with Lua

See ``run_tests.sh`` for how to run these tests.
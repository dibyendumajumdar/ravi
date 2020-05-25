This folder contains various performance and unit tests.

Language tests
--------------
* language/basics.lua - some simple tests
* language/ravi_tests1.ravi - contains most of the Ravi specific tests
* language/ravi_tests3.ravi - additional tests
* language/ravi_errors.ravi - contains tests for error conditions
* language/bitwise_tests.lua - modified Lua 5.3 tests to exercise JIT on bitwise operations

Performance tests
-----------------
Following are simple loop tests:

* performance/fornum_test1.lua
* performance/fornum_test2.lua
* performance/fornum_test2.ravi - with optional types
* performance/fornum_test3.lua

Matrix multiplication test:

* performance/matmul1.lua - matrix multiplication (Lua compatible)
* performance/matmul1.ravi - matrix multiplication (ravi version with static typing)
* performance/matmul1_ravi.lua - matrix multiplication (ravi version with static typing)

Following performance tests were obtained from the `The Computer Programming Language Benchmarks Game <http://benchmarksgame.alioth.debian.org/>`_. Original author is `Mike Pall <http://luajit.org/>`_.

* performance/fannkuchen.lua
* performance/fannkuchen.ravi - with optional types

* performance/mandel1.ravi - modified so that it can be JIT compiled
* performance/mandel1.lua - can be JIT compiled but also compatible with Lua

See ``run_tests.sh`` for how to run these tests.
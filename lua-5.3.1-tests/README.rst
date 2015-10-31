Lua 5.3.x Tests
===============

This folder contains a copy of the Lua 5.3 tests, amended slightly for Ravi. The changes are to skip some
tests that fail in Ravi due to lack of support for certain features of Lua - e.g. when the tests rely
upon the debug interface.

Notes
-----
* The 'ltests' library is included in Ravi in Debug builds.
* The tests crash on Windows 64-bit environment when JIT mode is enabled; this appears to be due to lack of 
  support for Windows stack unwinding in LLVM. See issue #30.
* In JIT mode the tests take much longer to run, especially if full JIT is switched on. This is because the tests
  generate a lot of small Lua functions dynamically - in thousands - and all the time is spent in JIT compiling
  these functions.


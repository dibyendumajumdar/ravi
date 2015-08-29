Lua 5.3.x Tests
===============

This folder contains a copy of the Lua 5.3 tests, amended slightly for Ravi. The changes are to skip some
tests that fail in Ravi due to lack of support for certain features of Lua - e.g. when the tests rely
upon the debug interface.
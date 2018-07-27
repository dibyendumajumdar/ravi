=========================
Ravi Programming Language
=========================

Ravi is a derivative/dialect of `Lua 5.3 <http://www.lua.org/>`_ with limited optional static typing and 
an `LLVM <http://www.llvm.org/>`_ powered JIT compiler. The name Ravi comes from the Sanskrit word for the Sun. 
Interestingly a precursor to Lua was `Sol <http://www.lua.org/history.html>`_ which had support for 
static types; Sol means the Sun in Portugese.

Lua is perfect as a small embeddable dynamic language so why a derivative? Ravi extends Lua with 
static typing for greater performance under JIT compilation. However, the static typing is 
optional and therefore Lua programs are also valid Ravi programs.

There are other attempts to add static typing to Lua - e.g. `Typed Lua <https://github.com/andremm/typedlua>`_ but 
these efforts are mostly about adding static type checks in the language while leaving the VM unmodified. 
The Typed Lua effort is very similar to the approach taken by Typescript in the JavaScript world. 
The static typing is to aid programming in the large - the code is eventually translated to standard Lua 
and executed in the unmodified Lua VM.

My motivation is somewhat different - I want to enhance the VM to support more efficient operations when types are 
known. Type information can be exploited by JIT compilation technology to improve performance. At the same time, 
I want to keep the language safe and therefore usable by non-expert programmers. 

Of course there is also the fantastic `LuaJIT <http://luajit.org>`_ implementation. Ravi has a different goal compared to 
LuaJIT. Ravi prioritizes ease of maintenance and support, language safety, and compatibility with Lua 5.3, 
over maximum performance. For more detailed comparison please refer to the documentation links below.

.. contents:: Table of Contents
   :depth: 1
   :backlinks: top

Features
========
* Optional static typing - for details `see the reference manual <ravi-reference.rst>`_.
* Type specific bytecodes to improve performance
* Compatibility with Lua 5.3 (see Compatibility section below)
* `LLVM <http://www.llvm.org/>`_ powered JIT compiler
* `Eclipse OMR <https://github.com/eclipse/omr>`_ powered JIT compiler
* Built-in C pre-processor, parser and JIT compiler
* A `distribution with batteries <https://github.com/dibyendumajumdar/ravi-distro>`_ is in the works - 
  this will provide ready made binary downloads of Ravi/Lua with select high quality libraries.

Documentation
=============
* For the Lua extensions in Ravi see the `Reference Manual <ravi-reference.rst>`_.
* `Build instructions <ravi-build-instructions.rst>`_.
* Also see `Ravi Documentation <http://the-ravi-programming-language.readthedocs.org/en/latest/index.html>`_.

Also see the slides I presented at the `Lua 2015 Workshop <http://www.lua.org/wshop15.html>`_.

Lua Goodies
===========
* `An Introduction to Lua <http://the-ravi-programming-language.readthedocs.io/en/latest/lua-introduction.html>`_ attempts to provide a quick overview of Lua for folks coming from other languages.
* `Lua 5.3 Bytecode Reference <http://the-ravi-programming-language.readthedocs.io/en/latest/lua_bytecode_reference.html>`_ is my attempt to bring up to date the `Lua 5.1 Bytecode Reference <http://luaforge.net/docman/83/98/ANoFrillsIntroToLua51VMInstructions.pdf>`_. 

Compatibility with Lua
======================
Ravi should be able to run all Lua 5.3 programs in interpreted mode, but following should be noted: 

* Ravi supports optional typing and enhanced types such as arrays (described above). Programs using these features cannot be run by standard Lua. However all types in Ravi can be passed to Lua functions; operations on Ravi arrays within Lua code will be subject to restrictions as described in the section above on arrays. 
* Values crossing from Lua to Ravi will be subjected to typechecks should these values be assigned to typed variables.
* Upvalues cannot subvert the static typing of local variables (issue #26) when types are annotated.
* Certain Lua limits are reduced due to changed byte code structure. These are described below.

+-----------------+-------------+-------------+
| Limit name      | Lua value   | Ravi value  |
+=================+=============+=============+
| MAXUPVAL        | 255         | 125         |
+-----------------+-------------+-------------+
| LUAI_MAXCCALLS  | 200         | 125         |
+-----------------+-------------+-------------+
| MAXREGS         | 255         | 125         |
+-----------------+-------------+-------------+
| MAXVARS         | 200         | 125         |
+-----------------+-------------+-------------+
| MAXARGLINE      | 250         | 120         |
+-----------------+-------------+-------------+

When JIT compilation is enabled there are following additional constraints:

* Ravi will only execute JITed code from the main Lua thread; any secondary threads (coroutines) execute in interpreter mode.
* In JITed code tailcalls are implemented as regular calls so unlike the interpreter VM which supports infinite tail recursion JIT compiled code only supports tail recursion to a depth of about 110 (issue #17)

History
=======
* 2015 
       - Implemented JIT compilation using LLVM
       - Implemented libgccjit based alternative JIT (now discontinued)
* 2016 
       - Implemented debugger for Ravi and Lua 5.3 for `Visual Studio Code <https://github.com/dibyendumajumdar/ravi/tree/master/vscode-debugger>`_ 
* 2017 
       - Embedded C compiler using dmrC project (C JIT compiler) 
       - Additional type annotations
* 2018 
       - Added Eclipse OMR JIT backend

License
=======
MIT License for LLVM version.


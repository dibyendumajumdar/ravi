=========================
Ravi Programming Language
=========================
.. image:: https://travis-ci.org/dibyendumajumdar/ravi.svg?branch=master
    :target: https://travis-ci.org/dibyendumajumdar/ravi

Ravi is a dialect of `Lua <http://www.lua.org/>`_ with limited optional static typing and 
features a JIT compiler powered by `MIR <https://github.com/vnmakarov/mir>`_ as well as experimental support for AOT compilation to native code.
The name Ravi comes from the Sanskrit word for the Sun. 
Interestingly a precursor to Lua was `Sol <http://www.lua.org/history.html>`_ which had support for 
static types; Sol means the Sun in Portugese.

Lua is perfect as a small embeddable dynamic language so why a derivative? Ravi extends Lua with 
static typing for improved performance when JIT compilation is enabled. However, the static typing is 
optional and therefore Lua programs are also valid Ravi programs.

There are other attempts to add static typing to Lua - e.g. `Typed Lua <https://github.com/andremm/typedlua>`_ but 
these efforts are mostly about adding static type checks in the language while leaving the VM unmodified. 
The Typed Lua effort is very similar to the approach taken by Typescript in the JavaScript world. 
The static typing is to aid programming in the large - the code is eventually translated to standard Lua 
and executed in the unmodified Lua VM.

My motivation is somewhat different - I want to enhance the VM to support more efficient operations when types are 
known. Type information can be exploited by JIT compilation technology to improve performance. At the same time, 
I want to keep the language safe and therefore usable by non-expert programmers. 

Of course there is the fantastic `LuaJIT <http://luajit.org>`_ implementation. Ravi has a different goal compared to 
LuaJIT. Ravi prioritizes ease of maintenance and support, language safety, and compatibility with Lua 5.3, 
over maximum performance. For more detailed comparison please refer to the documentation links below.

Features
========
* Optional static typing - for details `see the reference manual <https://the-ravi-programming-language.readthedocs.io/en/latest/ravi-reference.html>`_.
* Type specific bytecodes to improve performance
* Compatibility with Lua 5.3 (see Compatibility section below)
* Generational GC from Lua 5.4
* ``defer`` statement for releasing resources
* Compact JIT backend `MIR <https://github.com/vnmakarov/mir>`_.
* A `distribution with batteries <https://github.com/dibyendumajumdar/Suravi>`_.
* A `Visual Studio Code debugger extension <https://marketplace.visualstudio.com/items?itemName=ravilang.ravi-debug>`_ - interpreted mode debugger.
* A new compiler framework for `JIT and AOT compilation <https://the-ravi-programming-language.readthedocs.io/en/latest/ravi-compiler.html>`_.
* `AOT Compilation to shared library <https://github.com/dibyendumajumdar/ravi/tree/master/aot-examples>`_.
* Preview feature: Ability to `embed C code snippets <https://github.com/dibyendumajumdar/ravi-compiler/wiki/Embedding-C>`_.

Documentation
=============
* For the Lua extensions in Ravi see the `Reference Manual <https://the-ravi-programming-language.readthedocs.io/en/latest/ravi-reference.html>`_.
* `MIR JIT Build instructions <https://the-ravi-programming-language.readthedocs.io/en/latest/ravi-mir-instructions.html>`_.
* Also see `Ravi Documentation <http://the-ravi-programming-language.readthedocs.org/en/latest/index.html>`_.
* and the slides I presented at the `Lua 2015 Workshop <http://www.lua.org/wshop15.html>`_.

Lua Goodies
===========
* `An Introduction to Lua <http://the-ravi-programming-language.readthedocs.io/en/latest/lua-introduction.html>`_ attempts to provide a quick overview of Lua for folks coming from other languages.
* `Lua 5.3 Bytecode Reference <http://the-ravi-programming-language.readthedocs.io/en/latest/lua_bytecode_reference.html>`_ is my attempt to bring up to date the `Lua 5.1 Bytecode Reference <http://luaforge.net/docman/83/98/ANoFrillsIntroToLua51VMInstructions.pdf>`_.
* A `patch for Lua 5.3 <https://github.com/dibyendumajumdar/ravi/blob/master/patches/defer_statement_for_Lua_5_3.patch>`_ implements the 'defer' statement.
* A `patch for Lua 5.4.[0-2] <https://github.com/dibyendumajumdar/ravi/blob/master/patches/defer_statement_for_Lua_5_4.patch>`_ implements the 'defer' statement.
* Updated `patch for Lua 5.4.[3-4] <https://github.com/dibyendumajumdar/ravi/blob/master/patches/defer_statement_patch_for_Lua_5_4_3.patch>`_ implements the 'defer' statement.

Lua 5.4 Position Statement
==========================
Lua 5.4 relationship to Ravi is as follows:

* Generational GC - back-ported to Ravi.
* New random number generator - back-ported to Ravi.
* Multiple user values can be associated with userdata - under consideration.
* ``<const>`` variables - not planned. 
* ``<close>`` variables - Ravi has ``'defer'`` statement which is the better option in my opinion, hence no plans to support ``<close>`` variables. 
* Interpreter performance improvements - these are beneficial to Lua interpreter but not to the JIT backends, hence not much point in back-porting.
* Table implementation changes - under consideration. 
* String to number coercion is now part of string library metamethods - back-ported to Ravi.
* utf8 library accepts codepoints up to 2^31 - back-ported to Ravi.
* Removal of compatibility layers for 5.1, and 5.2 - not implemented as Ravi continues to provide these layers as per Lua 5.3.

Compatibility with Lua 5.3
==========================
Ravi should be able to run all Lua 5.3 programs in interpreted mode, but following should be noted:

* Ravi supports optional typing and enhanced types such as arrays (see the documentation). Programs using these features cannot be run by standard Lua. However all types in Ravi can be passed to Lua functions; operations on Ravi arrays within Lua code will be subject to restrictions as described in the section above on arrays.
* Values crossing from Lua to Ravi will be subjected to typechecks should these values be assigned to typed variables.
* Upvalues cannot subvert the static typing of local variables (issue #26) when types are annotated.
* Certain Lua limits are reduced due to changed byte code structure. These are described below.
* Ravi uses an extended bytecode which means it is not compatible with Lua 5.x bytecode.
* Ravi incorporates the new Generational GC from Lua 5.4, hence the GC interface has changed. 

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
* Debug api and hooks are not supported in JIT mode

History
=======
* 2015
       - Implemented JIT compilation using LLVM
       - Implemented `libgccjit based alternative JIT <https://github.com/dibyendumajumdar/ravi/tree/gccjit-ravi534>`_ (now discontinued)
* 2016
       - Implemented debugger for Ravi and Lua 5.3 for `Visual Studio Code <https://github.com/dibyendumajumdar/ravi/tree/master/vscode-debugger>`_
* 2017
       - Embedded C compiler using dmrC project (C JIT compiler) (now discontinued)
       - Additional type-annotations
* 2018
       - Implemented `Eclipse OMR JIT backend <https://github.com/dibyendumajumdar/ravi/tree/omrjit>`_ (now discontinued)
       - Created `Ravi with batteries <https://github.com/dibyendumajumdar/Suravi>`_.
* 2019 
       - New language feature - `defer` statement
       - New JIT backend `MIR <https://github.com/vnmakarov/mir>`_.
* 2020
       - `New parser / type checker / compiler <https://github.com/dibyendumajumdar/ravi-compiler>`_
       - Generational GC back-ported from Lua 5.4
       - Support for `LLVM backend <https://github.com/dibyendumajumdar/ravi/tree/llvm>`_ archived
* 2021
       - Integrated AOT and JIT compilation support 
       - `Embedded C syntax <https://github.com/dibyendumajumdar/ravi-compiler/wiki/Embedding-C>`_   

License
=======
MIT License

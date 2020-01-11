New Parser and Code Generator for Ravi
======================================

Status
------
The basic parser and type checker have been implemented. However the ``defer`` statement is not yet done.
Progress is very slow but things are moving every now and then when I get time.

Usage
-----
The parse tree can be generated as follows::

   tree, errmessage = ast.parse 'some lua code'
   if not tree then
      print(errmessage)
   else
      print(tree:tostring())
   end

Examples::

   tree, errmessage = ast.parse 'print "hello"'
   if not tree then
      print(errmessage)
   else
      print(tree:tostring())
   end
   
This generates output::

  function()
    --[expression statement start]
     --[expression list start]
       --[suffixed expr start] any
        --[primary start] any
          print --global symbol ?
        --[primary end]
        --[suffix list start]
          --[function call start] any
           (
             'hello'
           )
          --[function call end]
        --[suffix list end]
       --[suffixed expr end]
     --[expression list end]
    --[expression statement end]
  end

For more examples please see:

* `ravi-tests/ravi_test_ast.lua <https://github.com/dibyendumajumdar/ravi/blob/master/ravi-tests/ravi_test_ast.lua>`_ 
* `ravi-tests/ravi_test_ast2.lua <https://github.com/dibyendumajumdar/ravi/blob/master/ravi-tests/ravi_test_ast2.lua>`_. 

If you run these scripts the parse AST will be dumped to stdout.

Design Notes
------------

These are some design notes on the new parser / code generator.

There will be several phases:

1. Convert input into syntax tree - this will look very much like the input source, i.e. there will be fornum loops, if statements,
   while and repeat loops etc. During this phase only the types for literals and local decalarations will be known. MOSTLY DONE.
2. The next phase will be a type checking phase. In this phase types will be derive for expressions, and also type assertions will be
   added where required such as unpon function entry, and after function calls. But the overall syntax tree will still resemble the input 
   source except for the additional instructions. MOSTLY DONE. 
3. Third phase will be to assign virtual registers; first to locals, and then to temporaries. For simplicity I will probably keep the
   temporaries and locals in separate ranges.
4. Next phase will be linearize the instructions - during this phase the loops, if statements etc will get translated to equivalent of 
   conditional and unconditional jumps. We will probably create intermediate structures that can easily map to Ravi bytecodes.
5. In the first phase we will stop here, generate byte code and finish. 
6. The next phase will be translate into basic blocks.
7. Following that we will construct a CFG, perform dominator analysis and convert to SSA form.
8. Hopefully as a result of above we can do some simple optimizations and then emit the bytecode at the end.

Some other things
-----------------
1. Locals that are used as up-values or passed or overwritten by function calls should be marked as having 'escaped'.
   Having this knowledge will enable backend JIT to use the stack for values that cannot escape.
2. During code generation it will be good to know which registers are type constant - i.e. their types do not change. register allocation
   should be designed / implemented so that we try to avoid over-writing type data where possible. This will allow backend JIT 
   to generate more optimized code.


Implementation
--------------
The new Ravi Parser and Code Generator implementation is in:

* `ravi_ast_parse.c <https://github.com/dibyendumajumdar/ravi/blob/master/src/ravi_ast_parse.c>`_ - contains the parser that builds AST
* `ravi_ast_print.c <https://github.com/dibyendumajumdar/ravi/blob/master/src/ravi_ast_print.c>`_ - contains utilities for printing out the AST
* `ravi_ast_typecheck.c <https://github.com/dibyendumajumdar/ravi/blob/master/src/ravi_ast_typecheck.c>`_ - contains the type checking phase of the parser


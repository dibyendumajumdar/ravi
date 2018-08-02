New Parser and Code Generator for Ravi
======================================

These are some design notes on the new parser / code generator.

There will be several phases:

1. Convert input into syntax tree - this will look very much like the input source, i.e. there will be fornum loops, if statements,
   while and repeat loops etc. During this phase only the types for literals and local decalarations will be known.
2. The next phase will be a type checking phase. In this phase types will be derive for expressions, and also type assertions will be
   added where required such as unpon function entry, and after function calls. But the overall syntax tree will still resemble the input 
   source except for the additional instructions.
3. Third phase will be to assign virtual registers; first to locals, and then to temporaries. For simplicity I will probably keep the
   temporaries and locals in separate ranges.
4. Next phase will be linearize the instructions - during this phase the loops, if statements etc will get translated to equivalent of 
   conditional and unconditional jumps. 
5. We could at this stage generate byte code and finish. Initially maybe we will stop here.
6. The next phase will be translate into basic blocks.
7. Following that we will construct a CFG, perform dominator analysis and convert to SSA form.

Some other things
-----------------
1. locals that are used as up-values or passed or overwritten by function calls should be marked as having 'escaped'.
   Having this knowledge will enable backend JIT to use the stack for values that cannot escape.
2. during code generation it will be good to know which registers are type constant - i.e. their types do not change. register allocation
   should be designed / implemented so that we try to avoid over-writing type data where possible. This will allow backend JIT 
   to generate more optimized code.

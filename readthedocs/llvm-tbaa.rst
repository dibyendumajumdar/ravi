LLVM Type Based Alias Analysis
==============================
When a Lua opcode involves a call to a Lua function, the Lua stack may be reallocated. So then the base pointer which points to the function's
base stack position must be refreshed.

To keep compilation simple I coded the compiler so that at the beginning of each opcode the base pointer is reloaded. My assumption was that
the LLVM optimizer will realise that the base pointer hasn't changed and so the loads are redundant and can be removed. However to my surprise I found that this
is not the case. 

The main difference between the IR I was generating and that produced by Clang was that Clang generated IR appeared to be decorated 
by tbaa metadata. Example::

  %base2 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 4, i32 0
  %1 = load %struct.TValue** %base2, align 4, !tbaa !12

Here the ``!tbaa !12`` refers to a tbaa metadata entry.

I won't show the Clang generated tbaa metadata here, but here is how I added similar support in Ravi. The required steps are:

1. Create tbaa metadata mappings for the types in the system. 
2. Annotate Load and Store instructions with tbaa references.

Creating TBAA Metadata
----------------------
Firstly you need an MDBuilder instance. So you need to include following headers::

  #include "llvm/IR/MDBuilder.h"
  #include "llvm/IR/Metadata.h"

We can create an MDBuilder instance like this::

  llvm::MDBuilder mdbuilder(llvm::getGlobalContext());

The TBAA nodes hang off a root node. So we create that next::

  llvm::MDNode *tbaa_root;
  // Do what Clang does
  tbaa_root = mdbuilder.createTBAARoot("Simple C / C++ TBAA"); 

Next we need to create some simple scalar types. We only need one type per size, so that means we don't need 
``long long`` and ``double`` - either one will do. We create these scalar types as follows::

  llvm::MDNode *tbaa_charT;
  llvm::MDNode *tbaa_shortT;
  llvm::MDNode *tbaa_intT;
  llvm::MDNode *tbaa_longlongT;
  llvm::MDNode *tbaa_pointerT;


  //!4 = metadata !{metadata !"omnipotent char", metadata !5, i64 0}
  tbaa_charT = mdbuilder.createTBAAScalarTypeNode("omnipotent char", tbaa_root, 0); 
  //!3 = metadata !{metadata !"any pointer", metadata !4, i64 0}
  tbaa_pointerT = mdbuilder.createTBAAScalarTypeNode("any pointer", tbaa_charT, 0);
  //!10 = metadata !{metadata !"short", metadata !4, i64 0}
  tbaa_shortT = mdbuilder.createTBAAScalarTypeNode("short", tbaa_charT, 0);
  //!11 = metadata !{metadata !"int", metadata !4, i64 0}
  tbaa_intT = mdbuilder.createTBAAScalarTypeNode("int", tbaa_charT, 0);
  //!9 = metadata !{metadata !"long long", metadata !4, i64 0}
  tbaa_longlongT = mdbuilder.createTBAAScalarTypeNode("long long", tbaa_charT, 0);


The second argument to ``createTBAAScalarTypeNode()`` is the parent node. Note the hierarchy here::

  + root
  |
  +--+ char
     |
     +--+-- any pointer
        |
        +-- short
        |
        +-- int
        |
        +-- long long 

This is how Clang has it defined. 

Next we need to define aggregate (struct) types. The API we need for this is ``createTBAAStructTypeNode()``.
This method accepts a vector of ``std::pair<llvm::MDNode *, uint64_t>`` objects - each element in the vector defines
a field in the struct. The integer parameter needs to be the offset of the field within the struct. Interestingly
Clang generates offsets that indicate pointers are being treated as 32-bit quantities - even though I ran this on
a 64-bit machine. So I guess that as long as we consistently use the size then this doesn't matter. The sizes used
by Clang are:

* char - 1 byte
* short - 2 bytes
* int - 4 bytes
* pointer - 4 bytes
* long long - 8 bytes

Another interesting thing is that padding needs to be accounted for.

So now lets look at how to map following struct::

  struct CallInfoL {     /* only for Lua functions */
    struct TValue *base; /* base for this function */
    const unsigned int *savedpc;
    ptrdiff_t dummy;
  };

We map this as::

  llvm::MDNode *tbaa_CallInfo_lT;

  //!14 = metadata !{metadata !"CallInfoL", metadata !3, i64 0, metadata !3, i64 4, metadata !9, i64 8}
  std::vector<std::pair<llvm::MDNode *, uint64_t> > nodes;
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 4));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_longlongT, 8));
  tbaa_CallInfo_lT = mdbuilder.createTBAAStructTypeNode("CallInfo_l", nodes);

To illustrate how a structure is referenced as a field in another lets also look at::  


  struct CallInfo {
    struct TValue *func;              /* function index in the stack */
    struct TValue *top;               /* top for this function */
    struct CallInfo *previous, *next; /* dynamic call link */
    struct CallInfoL l;
    ptrdiff_t extra;
    short nresults; /* expected number of results from this function */
    unsigned char callstatus;
  };

We have a ``CallInfoL`` as the type of a field within the struct. Therefore:: 

  llvm::MDNode *tbaa_CallInfoT;

  //!13 = metadata !{metadata !"CallInfo", 
  //                 metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8, 
  //                 metadata !3, i64 12, metadata !14, i64 16, metadata !9, i64 32, 
  //                 metadata !10, i64 40, metadata !4, i64 42}
  nodes.clear();
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 0));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 4));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 8));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_pointerT, 12));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_CallInfo_lT, 16));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_longlongT, 32));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_shortT, 40));
  nodes.push_back(std::pair<llvm::MDNode*, uint64_t>(tbaa_charT, 42));
  tbaa_CallInfoT = mdbuilder.createTBAAStructTypeNode("CallInfo", nodes);

Decorating Load and Store instructions
--------------------------------------

So now we have created TBAA metadata for two struct types.
Next we need to see how we use these in Load and Store instructions. Lets assume we need to load the pointer 
stored in ``Callinfo.top``. In order to decorate the Load instruction with tbaa we need to create
a Struct Tag Node - which is like a path node. Here it is:: 

  llvm::MDNode *tbaa_CallInfo_topT;
  tbaa_CallInfo_topT = mdbuilder.createTBAAStructTagNode(tbaa_CallInfoT, tbaa_pointerT, 4);
 
Above is saying that the field ``top`` in struct ``CallInfo`` is a pointer at offset 4.

Armed with this we can code::

  llvm::Value *callinfo_top = /* GEP instruction */
  llvm::Instruction *top = Builder.CreateLoad(callinfo_top);
  top->setMetadata(llvm::LLVMContext::MD_tbaa, tbaa_CallInfo_topT);

Links
-----
* `TypeBasedAliasAnalysis code <http://llvm.org/docs/doxygen/html/TypeBasedAliasAnalysis_8cpp_source.html>`_.
* `IR documentation on tbaa metadata <http://llvm.org/docs/LangRef.html#tbaa-metadata>`_.
* `Embedded metadata <http://nondot.org/sabre/LLVMNotes/EmbeddedMetadata.txt>`_.
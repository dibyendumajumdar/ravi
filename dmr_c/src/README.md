The code is organised as follows:


## Pointer Linked List

This is implemented in:

* ptrlist.h
* ptrlist.c

## Memory allocation

* allocate.h
* allocate.c

## Tokenizer

This is implemented in:

* token.h
* tokenize.c

The tokenizer depends on declarations in symbol.h and target.h

## Parser

The C parser parses code as well as evaluates constant expressions and as far as I can tell it will also perform 
inlining of functions. The evaluation code is reused by the pre-processor so the C parser is lower level than the 
pre-processor.

The parser implementation is in following:

* symbol.h
* symbol.c
* scope.h
* scope.c
* parse.h
* parse.c
* char.c
* char.h
* expression.h
* expression.c
* expand.c
* evaluate.c
* inline.c
* ident-list.h
* show-parse.c

## Pre-processor

The pre-processor depends upon the parser functions to evaluate expressions. It is implemented in:

* pre-process.c

## Linearizer

This component transforms the parsed representation to a linear form (SSA I believe). The implementation is in:

* linearize.h
* linearize.c
* flow.h
* flow.c
* cse.c
* liveness.c
* memops.c 
* simplify.c
* unssa.c

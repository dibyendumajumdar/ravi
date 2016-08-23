/******************************************************************************
* Copyright (C) 2015 Dibyendu Majumdar
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#include <ravijit.h>

int raviV_compile(struct lua_State *L, struct Proto *p,
                  ravi_compile_options_t *options) {
  (void)L;
  (void)p;
  (void)options;
  return 0;
}

int raviV_compile_n(struct lua_State *L, struct Proto *p[], int n,
  ravi_compile_options_t *options) {
  (void)L;
  (void)p;
  (void)n;
  (void)options;
  return 0;
}

void raviV_freeproto(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}

// Initialize the JIT State and attach it to the
// Global Lua State
// If a JIT State already exists then this function
// will return -1
int raviV_initjit(struct lua_State *L) {
  (void)L;
  return 0;
}

// Free up the JIT State
void raviV_close(struct lua_State *L) {
  (void)L;
}

// Dump the LLVM IR
void raviV_dumpIR(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}

// Dump the LLVM ASM
void raviV_dumpASM(struct lua_State *L, struct Proto *p) {
  (void)L;
  (void)p;
}

void raviV_setminexeccount(struct lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getminexeccount(struct lua_State *L) {
  (void)L;
  return 0;
}

void raviV_setmincodesize(struct lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getmincodesize(struct lua_State *L) {
  (void)L;
  return 0;
}

void raviV_setauto(struct lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getauto(struct lua_State *L) {
  (void)L;
  return 0;
}

// Turn on/off the JIT compiler
void raviV_setjitenabled(struct lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getjitenabled(struct lua_State *L) {
  (void)L;
  return 0;
}

void raviV_setoptlevel(struct lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getoptlevel(struct lua_State *L) {
  (void)L;
  return 0;
}

void raviV_setsizelevel(struct lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getsizelevel(struct lua_State *L) {
  (void)L;
  return 0;
}

void raviV_setgcstep(struct lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_getgcstep(struct lua_State *L) {
  (void)L;
  return 0;
}

// Turn on/off the JIT compiler
void raviV_settraceenabled(struct lua_State *L, int value) {
  (void)L;
  (void)value;
}
int raviV_gettraceenabled(struct lua_State *L) {
  (void)L;
  return 0;
}

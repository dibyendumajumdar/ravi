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
#ifndef RAVIJIT_H
#define RAVIJIT_H

#ifdef __cplusplus
extern "C" {
#endif

struct lua_State;
struct Proto;

/* Initialise the JIT engine */
int raviV_initjit(struct lua_State *L);

/* Shutdown the JIT engine */
void raviV_close(struct lua_State *L);

/* Compile the given function if possible */
int raviV_compile(struct lua_State *L, struct Proto *p, int manual_request, int dump);

/* Free the JIT structures associated with the prototype */
void raviV_freeproto(struct lua_State *L, struct Proto *p);

#ifdef __cplusplus
}
#endif

#endif
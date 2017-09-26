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
typedef struct ravi_compile_options_t ravi_compile_options_t;

/* Initialise the JIT engine */
int raviV_initjit(struct lua_State *L);

/* Shutdown the JIT engine */
void raviV_close(struct lua_State *L);

/* Compile the given function if possible */
int raviV_compile(struct lua_State *L, struct Proto *p,
                  ravi_compile_options_t *options);
/* Compile an array of functions */
int raviV_compile_n(struct lua_State *L, struct Proto *p[], int n,
                    ravi_compile_options_t *options);
int raviV_iscompiled(struct lua_State *L, struct Proto *p);

/* Free the JIT structures associated with the prototype */
void raviV_freeproto(struct lua_State *L, struct Proto *p);

/* Set the code size preference */
void raviV_setsizelevel(struct lua_State *L, int sizelevel);
int raviV_getsizelevel(struct lua_State *L);

/* Set optimizer level */
void raviV_setoptlevel(struct lua_State *L, int optlevel);
int raviV_getoptlevel(struct lua_State *L);
  
/* Set verbosity */
void raviV_setverbosity(struct lua_State *L, int verbosity);
int raviV_getverbosity(struct lua_State *L);

/* Enable or disable JIT */
void raviV_setjitenabled(struct lua_State *L, int enabled);
int raviV_getjitenabled(struct lua_State *L);

/* Sets auto compilation flag */
void raviV_setauto(struct lua_State *L, int value);
int raviV_getauto(struct lua_State *L);

/* Sets the minimum code size for auto compilation */
void raviV_setmincodesize(struct lua_State *L, int mincodesize);
int raviV_getmincodesize(struct lua_State *L);

/* Sets the minimum execution count */
void raviV_setminexeccount(struct lua_State *L, int minexecccount);
int raviV_getminexeccount(struct lua_State *L);

/* Enable IR / codegen validations */
void raviV_setvalidation(struct lua_State *L, int enabled);
int raviV_getvalidation(struct lua_State *L);
  
/* Enable calls to GCSTEP  */
void raviV_setgcstep(struct lua_State *L, int value);
int raviV_getgcstep(struct lua_State *L);

/* Enable or disable trace hook */
void raviV_settraceenabled(struct lua_State *L, int enabled);
int raviV_gettraceenabled(struct lua_State *L);

/* Dumps the IR if available */
void raviV_dumpIR(struct lua_State *L, struct Proto *p);

/* Dump the compiled assembly code if available */
void raviV_dumpASM(struct lua_State *L, struct Proto *p);

#ifdef __cplusplus
}
#endif

#endif
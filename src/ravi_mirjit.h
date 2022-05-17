/******************************************************************************
* Copyright (C) 2019-2022 Dibyendu Majumdar
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
#ifndef RAVI_MIRJIT_H
#define RAVI_MIRJIT_H

#include "ravi_jitshared.h"

#ifdef USE_MIRJIT
#include "c2mir.h"
#include "mir-gen.h"

struct ravi_State {
	MIR_context_t jit;
	unsigned long long id;	// counter to generate function names
	unsigned int verbosity_ : 3;
	unsigned int auto_ : 1; /* Should we auto compile what we can? */
	unsigned int enabled_ : 1; /* is JIT enabled */
	unsigned int opt_level_ : 3; /* optimization level */
	unsigned int tracehook_enabled_ : 1; /* enable calls to luaG_traceexec() at every bytecode, this is expensive ! */
	unsigned int validation_ : 1; /* Enable extra validation such as IL verification */
	unsigned int compiling_; /* flag to help avoid recursion */
	int min_code_size_; /* min code size for compilation */
	int min_exec_count_; /* min execution count for compilation */
	struct c2mir_options options; /* MIR options */
};

extern void mir_prepare(MIR_context_t ctx, int optlevel);
extern void mir_cleanup(MIR_context_t ctx);
extern MIR_item_t mir_find_function(MIR_module_t module, const char *func_name);
extern MIR_module_t mir_compile_C_module(
    struct c2mir_options *options,
    MIR_context_t ctx,
    const char *inputbuffer, /* Code to be compiled */
    const char *source_name /* Name of the function, must be unique */
    );
extern void *mir_get_func(MIR_context_t ctx, MIR_module_t module, const char *func_name);

#ifdef __cplusplus
};
#endif

#endif /* USE_MIRJIT */

#endif /* RAVI_MIRJIT_H */

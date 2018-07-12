/******************************************************************************
* Copyright (C) 2015-2017 Dibyendu Majumdar
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
/******************************************************************************
* Copyright (C) 2015-2018 Dibyendu Majumdar
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
#ifndef RAVI_OMRJIT_H
#define RAVI_OMRJIT_H

#include <ravi_jitshared.h>

#ifdef USE_OMRJIT
#include "dmr_c.h"

struct ravi_State {
	JIT_ContextRef jit;
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
};

#ifdef __cplusplus
};
#endif

#endif /* USE_OMRJIT */

#endif /* RAVI_OMRJIT_H */

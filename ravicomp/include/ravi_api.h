/******************************************************************************
 * Copyright (C) 2020-2021 Dibyendu Majumdar
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
#ifndef ravicomp_RAVIAPI_H
#define ravicomp_RAVIAPI_H

#include "ravicomp_export.h"
#include "ravi_compiler.h"

#include <stdlib.h>

struct Ravi_CompilerInterface {
	/* ------------------------ Inputs ------------------------------ */
	void *context; /* Ravi supplied context */

	const char *source; /* Source code to be compiled - managed by Ravi */
	size_t source_len; /* Size of source code */
	const char *source_name; /* Name of the source */
	const char *compiler_options; /* flags to be passed to compiler */

	char main_func_name[31]; /* Name of the generated function that when called will set up the Lua closure */

	/* ------------------------- Outputs ------------------------------ */
	const char* generated_code;  /* Output of the compiler, must be freed by caller. */

	/* ------------------------ Debugging and error handling ----------------------------------------- */
	void (*debug_message)(void *context, const char *filename, long long line, const char *message);
	void (*error_message)(void *context, const char *message);
};

/**
 * This is the API exposed by the Compiler itself. This function is invoked by
 * Ravi when it is necessary to compile some Ravi code.
 * @param compiler_interface The interface expected by the compiler must be setup
 * @return 0 for success, non-zero for failure
 */
RAVICOMP_EXPORT int raviX_compile(struct Ravi_CompilerInterface *compiler_interface);

#endif

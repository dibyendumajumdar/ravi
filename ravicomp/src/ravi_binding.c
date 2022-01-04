/******************************************************************************
 * Copyright (C) 2020-2022 Dibyendu Majumdar
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

/* This will contain Lua bindings */

#include "ravi_api.h"
#include "ravi_compiler.h"

#include "cfg.h"
#include "codegen.h"
#include "optimizer.h"

int raviX_compile(struct Ravi_CompilerInterface *compiler_interface)
{
	int rc = 0;
	int dump_ir = 0;
	if (compiler_interface->compiler_options != NULL) {
		dump_ir = strstr(compiler_interface->compiler_options, "--dump-ir") != NULL;
	}
	compiler_interface->generated_code = NULL;
	CompilerState *compiler_state = raviX_init_compiler(compiler_interface->memory_allocator);
	LinearizerState *linearizer = raviX_init_linearizer(compiler_state);
	rc = raviX_parse(compiler_state, compiler_interface->source, compiler_interface->source_len,
			 compiler_interface->source_name);
	if (rc != 0) {
		compiler_interface->error_message(compiler_interface->context, raviX_get_last_error(compiler_state));
		goto L_exit;
	}
	rc = raviX_ast_lower(compiler_state);
	if (rc != 0) {
		compiler_interface->error_message(compiler_interface->context, raviX_get_last_error(compiler_state));
		goto L_exit;
	}
	rc = raviX_ast_typecheck(compiler_state);
	if (rc != 0) {
		compiler_interface->error_message(compiler_interface->context, raviX_get_last_error(compiler_state));
		goto L_exit;
	}
	rc = raviX_ast_simplify(compiler_state);
	if (rc != 0) {
		compiler_interface->error_message(compiler_interface->context, raviX_get_last_error(compiler_state));
		goto L_exit;
	}
	rc = raviX_ast_linearize(linearizer);
	if (rc != 0) {
		compiler_interface->error_message(compiler_interface->context, raviX_get_last_error(compiler_state));
		goto L_exit;
	}
	raviX_construct_cfg(linearizer->main_proc);
	raviX_remove_unreachable_blocks(linearizer);

	TextBuffer buf;
	raviX_buffer_init(&buf, 4096);
	if (dump_ir) {
		raviX_buffer_add_string(&buf, "#if 0\n");
		raviX_buffer_add_string(&buf, "// Following is an IR Dump from the compiler\n");
		raviX_show_linearizer(linearizer, &buf);
		raviX_buffer_add_string(&buf, "\n// End of IR dump\n");
		raviX_buffer_add_string(&buf, "#endif\n");
	}
	rc = raviX_generate_C(linearizer, &buf, compiler_interface);
	compiler_interface->generated_code = buf.buf;

L_exit:
	raviX_destroy_linearizer(linearizer);
	raviX_destroy_compiler(compiler_state);

	return rc;
}

void raviX_release(struct Ravi_CompilerInterface *compiler_interface)
{
	if (compiler_interface->generated_code != NULL) {
		raviX_free((void *)compiler_interface->generated_code);
		compiler_interface->generated_code = NULL;
	}
}
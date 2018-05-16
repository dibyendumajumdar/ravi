#ifndef DMR_C_SCOPE_H
#define DMR_C_SCOPE_H
/*
* Symbol scoping is pretty simple.
*
* Copyright (C) 2003 Transmeta Corp.
*               2003 Linus Torvalds
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
/*
* This version is part of the dmr_c project.
* Copyright (C) 2017 Dibyendu Majumdar
*/

#include <lib.h>

#ifdef __cplusplus
extern "C" {
#endif


struct scope {
	struct token *token;    /* Scope start information */
	struct symbol_list *symbols; /* List of symbols in this scope */
	struct scope *next;
};

static inline int dmrC_toplevel(struct dmr_C *C, struct scope *scope)
{
	return scope == C->file_scope || scope == C->global_scope;
}

extern void dmrC_start_file_scope(struct dmr_C *C);
extern void dmrC_end_file_scope(struct dmr_C *C);
extern void dmrC_new_file_scope(struct dmr_C *C);

extern void dmrC_start_symbol_scope(struct dmr_C *C);
extern void dmrC_end_symbol_scope(struct dmr_C *C);

extern void dmrC_start_function_scope(struct dmr_C *C);
extern void dmrC_end_function_scope(struct dmr_C *C);

extern void dmrC_bind_scope(struct dmr_C *C, struct symbol *, struct scope *);
extern void dmrC_rebind_scope(struct dmr_C *C, struct symbol *sym, struct scope *news);

extern int dmrC_is_outer_scope(struct dmr_C *C, struct scope *);

extern void dmrC_init_scope(struct dmr_C *C);
extern void dmrC_destroy_all_scopes(struct dmr_C *C);

#ifdef __cplusplus
}
#endif


#endif

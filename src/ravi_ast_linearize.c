/*
Copyright (C) 2018-2020 Dibyendu Majumdar
*/

#include <ravi_ast.h>
#include <ptrlist.h>

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

/* Linearizer - WIP  */
void raviA_init_linearizer(struct linearizer *linearizer, struct ast_container *container) {
	memset(linearizer, 0, sizeof * linearizer);
	linearizer->ast_container = container;
	dmrC_allocator_init(&linearizer->edge_allocator, "edge_allocator", sizeof(struct edge), sizeof(double), CHUNK);
	dmrC_allocator_init(&linearizer->instruction_allocator, "instruction_allocator", sizeof(struct instruction), sizeof(double), CHUNK);
	dmrC_allocator_init(&linearizer->ptrlist_allocator, "ptrlist_allocator", sizeof(struct ptr_list), sizeof(double), CHUNK);
	dmrC_allocator_init(&linearizer->pseudo_allocator, "pseudo_allocator", sizeof(struct pseudo), sizeof(double), CHUNK);
}

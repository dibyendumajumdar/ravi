/******************************************************************************
 * Copyright (C) 2020-2023 Dibyendu Majumdar
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
#ifndef ravicomp_ALLOCATE_H
#define ravicomp_ALLOCATE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#ifdef __cplusplus
//extern "C" {
//#endif

extern void *raviX_malloc(size_t size);
extern void *raviX_calloc(size_t nmemb, size_t size);
extern void *raviX_realloc(void *ptr, size_t size);
extern void raviX_free(void *ptr);

#ifndef C_MEMORYALLOCATOR_DEFINED
#define C_MEMORYALLOCATOR_DEFINED
/* Note that this struct below is also defined in ravi_compiler.h/chibicc.h and all
 * definitions must be kept in sync.
 */
typedef struct C_MemoryAllocator {
	void *arena;
	void *(*realloc)(void *arena, void* mem, size_t newsize);
	void *(*calloc)(void *arena,  size_t n_elements, size_t elem_size);
	void (*free)(void *arena, void *p);
	void *(*create_arena)(size_t, int);
	size_t (*destroy_arena)(void *arena);
} C_MemoryAllocator;
#endif

/*
Reallocate array from old_n to new_n. If new_n is 0 then array memory is freed.
If new_n is greater than old_n then old data is copied across and the
additional allocated space is zeroed out so caller can rely on the extra space being
initialized to zeros.
*/
extern void *raviX_realloc_array(void *oldp, size_t element_size, size_t old_n, size_t new_n);
/*
Delete num_to_delete elements starting at starting_index from array of size array_size, where sizeof(each element) is
element_size. The freed up space will be zero initialized. Returns the new array_size.
*/
extern size_t raviX_del_array_element(void *p, size_t element_size, size_t array_size, size_t starting_index,
				      size_t num_to_delete);

/* We often want an array of some type with dynamic memory management. The following macros let us
 * create such array types and provide simple ways of pushing an element to the array.
 */
#define DECLARE_ARRAY(array_type, TYPE)                                                                                \
	typedef struct array_type {                                                                                    \
		unsigned capacity;                                                                                     \
		unsigned count;                                                                                        \
		TYPE *data;                                                                                            \
	} array_type
#define array_push(A, type, value)                                                                                     \
	{                                                                                                              \
		if ((A)->count == (A)->capacity) {                                                                     \
			unsigned newsize = (A)->capacity += 10;                                                        \
			(A)->data =                                                                                    \
			    (type *)raviX_realloc_array((A)->data, sizeof((A)->data[0]), (A)->capacity, newsize);      \
			(A)->capacity = newsize;                                                                       \
		}                                                                                                      \
		(A)->data[(A)->count++] = value;                                                                       \
	}
#define array_clearmem(A)                                                                                              \
	{                                                                                                              \
		raviX_realloc_array((A)->data, sizeof((A)->data[0]), (A)->capacity, 0);                                \
		(A)->data = NULL;                                                                                      \
		(A)->capacity = 0;                                                                                     \
		(A)->count = 0;                                                                                        \
	}

//#ifdef __cplusplus
//}
//#endif

#endif


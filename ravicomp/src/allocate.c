/*
 * This version is part of the Ravi Compiler project.
 * Copyright (C) 2017-2023 Dibyendu Majumdar
 */

#include "allocate.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *raviX_malloc(size_t size)
{
	void *ptr;
	ptr = malloc(size);
        if (!ptr) {
                fprintf(stderr, "out of memory\n");
                exit(1);
        }
	return ptr;
}

void *raviX_calloc(size_t nmemb, size_t size)
{
	void *ptr;
	ptr = calloc(nmemb, size);
        if (!ptr) {
                fprintf(stderr, "out of memory\n");
                exit(1);
        }
	return ptr;
}

void *raviX_realloc(void *p, size_t size)
{
	void *ptr;
	ptr = realloc(p, size);
        if (!ptr) {
                fprintf(stderr, "out of memory\n");
                exit(1);
        }
	return ptr;
}

void raviX_free(void *ptr)
{
    free(ptr);
}


/*
Reallocate array from old_n to new_n. If new_n is 0 then array memeory is freed.
If new_n is greater than old_n then old data is copied across and the
additional allocated space is zeroed out so caller can rely on the extra space being
initialized to zeros.
*/
void *raviX_realloc_array(void *oldp, size_t element_size, size_t old_n, size_t new_n)
{
	if (new_n == 0) {
		raviX_free(oldp);
		return NULL;
	}
	assert(new_n > old_n);
	size_t newsize = element_size * new_n;
	void *newp = realloc(oldp, newsize);
	if (!newp) {
		fprintf(stderr, "out of memory\n");
		abort();
	}
	size_t oldsize = old_n * element_size;
	char *p = (char *)newp;
	memset(p + oldsize, 0, newsize - oldsize);
	return newp;
}

/*
Delete n elements starting at i from array a of size array_size, where sizeof(each element) is element_size.
The freed up space will be zero initialized.
*/
size_t raviX_del_array_element(void *a, size_t element_size, size_t array_size, size_t i, size_t n)
{
	assert(i + n <= array_size);
	char *p = (char *)a;
	char *dest = p + i * element_size;
	char const *src = p + (i + n) * element_size;
	size_t count = element_size * (array_size - n - i);
	memmove(dest, src, count);
	size_t new_array_size = array_size - n;
	size_t newsize = element_size * new_array_size;
	size_t oldsize = element_size * array_size;
	memset(p + newsize, 0, oldsize - newsize);
	return new_array_size;
}

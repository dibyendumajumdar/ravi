/*
 * allocate.c - simple space-efficient blob allocator.
 *
 * Copyright (C) 2003 Transmeta Corp.
 *               2003-2004 Linus Torvalds
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
 *
 * Simple allocator for data that doesn't get partially free'd.
 * The tokenizer and parser allocate a _lot_ of small data structures
 * (often just two-three bytes for things like small integers),
 * and since they all depend on each other you can't free them
 * individually _anyway_. So do something that is very space-
 * efficient: allocate larger "blobs", and give out individual
 * small bits and pieces of it with no maintenance overhead.
 */
/*
 * This version is part of the Ravi Compiler project.
 * Copyright (C) 2017-2021 Dibyendu Majumdar
 */

#include <allocate.h>

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

static void *blob_alloc(size_t size)
{
	void *ptr = raviX_malloc(size);
	memset(ptr, 0, size);
	return ptr;
}

static void blob_free(void *addr, size_t size)
{
	(void)size;
	raviX_free(addr);
}

void raviX_allocator_init(Allocator *A, const char *name, size_t size, unsigned int alignment, unsigned int chunking)
{
	A->name = name;
	A->blobs = NULL;
	A->size = size;
	A->alignment = alignment;
	A->chunking = chunking;
	A->freelist = NULL;
	A->allocations = 0;
	A->total_bytes = 0;
	A->useful_bytes = 0;
}

void *raviX_allocator_allocate(Allocator *A, size_t extra)
{
	size_t size = extra + A->size;
	size_t alignment = A->alignment;
	AllocationBlob *blob = A->blobs;
	void *retval;

	if (size > A->chunking) {
		fprintf(stderr, "allocation failure: requested size %lld is larger than maximum chunk size %lld\n",
			(long long)size, (long long)A->chunking);
		exit(1);
	}
	/*
	 * NOTE! The freelist only works with things that are
	 *  (a) sufficiently aligned
	 *  (b) use a constant size
	 * Don't try to free allocators that don't follow
	 * these rules.
	 */
	if (A->freelist) {
		void **p = (void **)A->freelist;
		retval = p;
		A->freelist = *p;
		memset(retval, 0, size);
		return retval;
	}

	A->allocations++;
	A->useful_bytes += size;
	size = (size + alignment - 1) & ~(alignment - 1);
	if (!blob || blob->left < size) {
		size_t offset, chunking = A->chunking;
		AllocationBlob *newblob = (AllocationBlob *)blob_alloc(chunking);
		A->total_bytes += chunking;
		newblob->next = blob;
		blob = newblob;
		A->blobs = newblob;
		offset = offsetof(AllocationBlob, data);
		offset = (offset + alignment - 1) & ~(alignment - 1);
		blob->left = chunking - offset;
		blob->offset = offset - offsetof(AllocationBlob, data);
	}
	retval = blob->data + blob->offset;
	blob->offset += size;
	blob->left -= size;
	return retval;
}

void raviX_allocator_free(Allocator *A, void *entry)
{
	void **p = (void **)entry;
	*p = A->freelist;
	A->freelist = p;
}
void raviX_allocator_show_allocations(Allocator *A)
{
	fprintf(stderr,
		"%s: %d allocations, %d bytes (%d total bytes, "
		"%6.2f%% usage, %6.2f average size)\n",
		A->name, (int)A->allocations, (int)A->useful_bytes, (int)A->total_bytes,
		100 * (double)A->useful_bytes / A->total_bytes, (double)A->useful_bytes / A->allocations);
}
void raviX_allocator_drop_all_allocations(Allocator *A)
{
	AllocationBlob *blob = A->blobs;
	A->blobs = NULL;
	A->allocations = 0;
	A->total_bytes = 0;
	A->useful_bytes = 0;
	A->freelist = NULL;
	while (blob) {
		AllocationBlob *next = blob->next;
		blob_free(blob, A->chunking);
		blob = next;
	}
}
void raviX_allocator_destroy(Allocator *A)
{
	raviX_allocator_drop_all_allocations(A);
	A->blobs = NULL;
	A->allocations = 0;
	A->total_bytes = 0;
	A->useful_bytes = 0;
	A->freelist = NULL;
}
void raviX_allocator_transfer(Allocator *A, Allocator *transfer_to)
{
	assert(transfer_to->blobs == NULL);
	assert(transfer_to->freelist == NULL);
	transfer_to->blobs = A->blobs;
	transfer_to->allocations = A->allocations;
	transfer_to->total_bytes = A->total_bytes;
	transfer_to->useful_bytes = A->useful_bytes;
	transfer_to->freelist = A->freelist;
	transfer_to->alignment = A->alignment;
	transfer_to->chunking = A->chunking;
	transfer_to->size = A->size;
	A->blobs = NULL;
	A->allocations = 0;
	A->total_bytes = 0;
	A->useful_bytes = 0;
	A->freelist = NULL;
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
	char *src = p + (i + n) * element_size;
	size_t count = element_size * (array_size - n - i);
	memmove(dest, src, count);
	size_t new_array_size = array_size - n;
	size_t newsize = element_size * new_array_size;
	size_t oldsize = element_size * array_size;
	memset(p + newsize, 0, oldsize - newsize);
	return new_array_size;
}

#if 0

struct foo {
  int a, b;
};

int raviX_test_allocator() {
  Allocator alloc;
  raviX_allocator_init(&alloc, "foo", sizeof(struct foo), __alignof__(struct foo),
                 sizeof(AllocationBlob) + sizeof(struct foo) * 2);
  struct foo *t1 = (struct foo *)raviX_allocator_allocate(&alloc, 0);
  if (t1 == NULL)
    return 1;
  if (alloc.alignment_ != __alignof__(struct foo))
    return 1;
  if (alloc.allocations != 1)
    return 1;
  if (alloc.freelist_ != NULL)
    return 1;
  struct foo *t2 = (struct foo *)raviX_allocator_allocate(&alloc, 0);
  if (t2 != t1 + 1)
    return 1;
  //dmrC_allocator_show_allocations(&alloc);
  raviX_allocator_free(&alloc, t1);
  raviX_allocator_free(&alloc, t2);
  struct foo *t3 = (struct foo *)raviX_allocator_allocate(&alloc, 0);
  if (t3 != t2)
    return 1;
  struct foo *t4 = (struct foo *)raviX_allocator_allocate(&alloc, 0);
  if (t4 != t1)
    return 1;
  struct foo *t5 = (struct foo *)raviX_allocator_allocate(&alloc, 0);
  (void)t5;
  if (alloc.total_bytes !=
      (sizeof(AllocationBlob) + sizeof(struct foo) * 2) * 2)
    return 1;
  Allocator alloc2;
  memset(&alloc2, 0, sizeof alloc2);
  AllocationBlob *saved = alloc.blobs_;
  raviX_allocator_transfer(&alloc, &alloc2);
  if (alloc.blobs_ != NULL)
	  return 1;
  if (alloc2.blobs_ != saved)
	  return 1;
  raviX_allocator_destroy(&alloc2);
  printf("allocator tests okay\n");
  return 0;
}

#endif

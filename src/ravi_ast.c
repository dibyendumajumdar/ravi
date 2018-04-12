#define LUA_CORE

#include "lprefix.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "lua.h"

#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "llex.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lzio.h"
#include "ravi_ast.h"
#include "ravi_membuf.h"

#include "lauxlib.h"

#define MAXVARS		125 


// README
// THIS IS NOT WORKING - IT IS WORK IN PROGRESS
// Set test_ast.c for a way to exercise this.
// Also we enable an API raviload() to get to the entry point here

// We need a simple bump allocator
// Following is based on code from Linus Torvalds sparse project

/*
* Our "blob" allocator works on chunks that are multiples
* of this size (the underlying allocator may be a mmap that
* cannot handle smaller chunks, for example, so trying to
* allocate blobs that aren't aligned is not going to work).
*/
#define CHUNK 32768

static size_t alignment = sizeof(double);

struct allocation_blob {
	struct allocation_blob *next;
	size_t left, offset;
	unsigned char data[];
};

struct allocator {
	const char *name_;
	struct allocation_blob *blobs_;
	size_t size_;
	unsigned int alignment_;
	unsigned int chunking_;
	void *freelist_;
	size_t allocations, total_bytes, useful_bytes;
};


static void *blob_alloc(size_t size) {
	void *ptr;
	ptr = malloc(size);
	if (ptr != NULL) memset(ptr, 0, size);
	return ptr;
}

static void blob_free(void *addr, size_t size) {
	(void)size;
	free(addr);
}

static void allocator_init(struct allocator *A, const char *name, size_t size,
	unsigned int alignment, unsigned int chunking) {
	A->name_ = name;
	A->blobs_ = NULL;
	A->size_ = size;
	A->alignment_ = alignment;
	A->chunking_ = chunking;
	A->freelist_ = NULL;
	A->allocations = 0;
	A->total_bytes = 0;
	A->useful_bytes = 0;
}

static void drop_all_allocations(struct allocator *A) {
	struct allocation_blob *blob = A->blobs_;
	A->blobs_ = NULL;
	A->allocations = 0;
	A->total_bytes = 0;
	A->useful_bytes = 0;
	A->freelist_ = NULL;
	while (blob) {
		struct allocation_blob *next = blob->next;
		blob_free(blob, A->chunking_);
		blob = next;
	}
}

static void *allocator_allocate(struct allocator *A, size_t extra) {
	size_t size = extra + A->size_;
	size_t alignment = A->alignment_;
	struct allocation_blob *blob = A->blobs_;
	void *retval;

	assert(size <= A->chunking_);
	/*
	* NOTE! The freelist only works with things that are
	*  (a) sufficiently aligned
	*  (b) use a constant size
	* Don't try to free allocators that don't follow
	* these rules.
	*/
	if (A->freelist_) {
		void **p = (void **)A->freelist_;
		retval = p;
		A->freelist_ = *p;
		memset(retval, 0, size);
		return retval;
	}

	A->allocations++;
	A->useful_bytes += size;
	size = (size + alignment - 1) & ~(alignment - 1);
	if (!blob || blob->left < size) {
		size_t offset, chunking = A->chunking_;
		struct allocation_blob *newblob =
			(struct allocation_blob *)blob_alloc(chunking);
		if (!newblob) {
			fprintf(stderr, "out of memory\n");
			abort();
		}
		A->total_bytes += chunking;
		newblob->next = blob;
		blob = newblob;
		A->blobs_ = newblob;
		offset = offsetof(struct allocation_blob, data);
		offset = (offset + alignment - 1) & ~(alignment - 1);
		blob->left = chunking - offset;
		blob->offset = offset - offsetof(struct allocation_blob, data);
	}
	retval = blob->data + blob->offset;
	blob->offset += size;
	blob->left -= size;
	return retval;
}

static void allocator_free(struct allocator *A, void *entry) {
	void **p = (void **)entry;
	*p = A->freelist_;
	A->freelist_ = p;
}

static void show_allocations(struct allocator *x) {
	fprintf(stderr,
		"allocator %s: %d allocations, %d bytes (%d total bytes, "
		"%6.2f%% usage, %6.2f average size)\n", x->name_,
		(int)x->allocations, (int)x->useful_bytes, (int)x->total_bytes,
		100 * (double)x->useful_bytes / x->total_bytes,
		(double)x->useful_bytes / x->allocations);
}

static void allocator_destroy(struct allocator *A) {
	show_allocations(A);
	drop_all_allocations(A);
	A->blobs_ = NULL;
	A->allocations = 0;
	A->total_bytes = 0;
	A->useful_bytes = 0;
	A->freelist_ = NULL;
}


//////////////////////// PTRLIST /////////////////////////////

/*
* The ptr list data structure is like a train - with cars linked to each other.
* Just as in a train each car has many seats, so in ptr list each "node" has
* several entries. Unlike a train however, the ptr list is arranged as a ring,
* i.e. the the front and back nodes are linked to each other. Hence there is no
* such thing as a 'head' of the list - i.e. any node can be the head!
*/

#ifndef LIST_NODE_NR
#define LIST_NODE_NR (29)
#endif

#define DECLARE_PTR_LIST(listname, type)                                       \
	struct listname {                                                      \
		int nr_ : 8;                                                   \
		int rm_ : 8;                                                   \
		struct listname *prev_;                                        \
		struct listname *next_;                                        \
		struct allocator *allocator_;                                  \
		type *list_[LIST_NODE_NR];                                     \
	}

/* Each node in the list */
DECLARE_PTR_LIST(ptr_list, void);

struct ptr_list_iter {
	struct ptr_list *__head;
	struct ptr_list *__list;
	int __nr;
};

/* The ptr list */
static int ptrlist_size(const struct ptr_list *self);
static void **ptrlist_add(struct ptr_list **self, void *ptr, struct allocator *alloc);
static void *ptrlist_nth_entry(struct ptr_list *list, unsigned int idx);
static void *ptrlist_first(struct ptr_list *list);
static void *ptrlist_last(struct ptr_list *list);
static int ptrlist_linearize(struct ptr_list *head, void **arr, int max);
static void ptrlist_split_node(struct ptr_list *head);
static void ptrlist_pack(struct ptr_list **self);
static void ptrlist_remove_all(struct ptr_list **self);
static int ptrlist_remove(struct ptr_list **self, void *entry, int count);
static int ptrlist_replace(struct ptr_list **self, void *old_ptr, void *new_ptr,
	int count);
static void *ptrlist_undo_last(struct ptr_list **self);
static void *ptrlist_delete_last(struct ptr_list **self);
static void ptrlist_concat(struct ptr_list *a, struct ptr_list **self);
static void ptrlist_sort(struct ptr_list **self, void *,
	int(*cmp)(void *, const void *, const void *));

/* iterator functions */
static struct ptr_list_iter ptrlist_forward_iterator(struct ptr_list *self);
static struct ptr_list_iter ptrlist_reverse_iterator(struct ptr_list *self);
static void *ptrlist_iter_next(struct ptr_list_iter *self);
static void *ptrlist_iter_prev(struct ptr_list_iter *self);
static void ptrlist_iter_split_current(struct ptr_list_iter *self);
static void ptrlist_iter_insert(struct ptr_list_iter *self, void *newitem);
static void ptrlist_iter_remove(struct ptr_list_iter *self);
static void ptrlist_iter_set(struct ptr_list_iter *self, void *ptr);
static void ptrlist_iter_mark_deleted(struct ptr_list_iter *self);

static inline void **ptrlist_iter_this_address(struct ptr_list_iter *self) {
	return &self->__list->list_[self->__nr];
}
#define ptr_list_empty(x) ((x) == NULL)
#define PTR_ENTRY_NOTAG(h,i)	((h)->list_[i])
#define PTR_ENTRY(h,i)	(void *)(PTR_ENTRY_NOTAG(h,i))

#define FOR_EACH_PTR(list, var) \
	{ struct ptr_list_iter var##iter__ = ptrlist_forward_iterator((struct ptr_list *)list); \
	for (var = ptrlist_iter_next(&var##iter__); var != NULL; var = ptrlist_iter_next(&var##iter__))
#define END_FOR_EACH_PTR(var) }

#define FOR_EACH_PTR_REVERSE(list, var) \
	{ struct ptr_list_iter var##iter__ = ptrlist_reverse_iterator((struct ptr_list *)list); \
	for (var = ptrlist_iter_prev(&var##iter__); var != NULL; var = ptrlist_iter_prev(&var##iter__))
#define END_FOR_EACH_PTR_REVERSE(var) }

#define RECURSE_PTR_REVERSE(list, var) \
	{ struct ptr_list_iter var##iter__ = list##iter__; \
	for (var = ptrlist_iter_prev(&var##iter__); var != NULL; var = ptrlist_iter_prev(&var##iter__))

#define PREPARE_PTR_LIST(list, var)	\
	struct ptr_list_iter var##iter__ = ptrlist_forward_iterator((struct ptr_list *)list); \
	var = ptrlist_iter_next(&var##iter__)

#define NEXT_PTR_LIST(var) \
	var = ptrlist_iter_next(&var##iter__)
#define FINISH_PTR_LIST(var) 

#define THIS_ADDRESS(type, var) \
	(type *)ptrlist_iter_this_address(&var##iter__)

#define DELETE_CURRENT_PTR(var) \
	ptrlist_iter_remove(&var##iter__)	

#define REPLACE_CURRENT_PTR(type, var, replacement) \
	ptrlist_iter_set(&var##iter__, replacement)

#define INSERT_CURRENT(newval, var) \
	ptrlist_iter_insert(&var##iter__, newval)	

#define MARK_CURRENT_DELETED(PTR_TYPE, var) \
	ptrlist_iter_mark_deleted(&var##iter__)

/*
* ptrlist.c
*
* Pointer ptrlist_t manipulation
*
* (C) Copyright Linus Torvalds 2003-2005
*/
/*
* This version is part of the dmr_c project.
* Copyright (C) 2017 Dibyendu Majumdar
*/

#define PARANOIA 1

/* For testing we change this */
static int N_ = LIST_NODE_NR;

void ptrlist_split_node(struct ptr_list *head)
{
	int old = head->nr_, nr = old / 2;
	struct allocator *alloc = head->allocator_;
	assert(alloc);
	struct ptr_list *newlist =
		(struct ptr_list *)allocator_allocate(alloc, 0);
	struct ptr_list *next = head->next_;
	newlist->allocator_ = alloc;

	old -= nr;
	head->nr_ = old;
	newlist->next_ = next;
	next->prev_ = newlist;
	newlist->prev_ = head;
	head->next_ = newlist;
	newlist->nr_ = nr;
	memcpy(newlist->list_, head->list_ + old, nr * sizeof(void *));
	memset(head->list_ + old, 0xf0, nr * sizeof(void *));
}

struct ptr_list_iter ptrlist_forward_iterator(struct ptr_list *head)
{
	struct ptr_list_iter iter;
	iter.__head = iter.__list = head;
	iter.__nr = -1;
	return iter;
}

// Reverse iterator has to start from previous node not previous entry
// in the given head
struct ptr_list_iter ptrlist_reverse_iterator(struct ptr_list *head)
{
	struct ptr_list_iter iter;
	iter.__head = iter.__list = head ? head->prev_ : NULL;
	iter.__nr = iter.__head ? iter.__head->nr_ : 0;
	return iter;
}

void *ptrlist_iter_next(struct ptr_list_iter *self)
{
	if (self->__head == NULL)
		return NULL;
	self->__nr++;
Lretry:
	if (self->__nr < self->__list->nr_) {
		void *ptr = self->__list->list_[self->__nr];
		if (self->__list->rm_ && !ptr) {
			self->__nr++;
			goto Lretry;
		}
		return ptr;
	}
	else if (self->__list->next_ != self->__head) {
		self->__list = self->__list->next_;
		self->__nr = 0;
		goto Lretry;
	}
	return NULL;
}

void *ptrlist_nth_entry(struct ptr_list *list, unsigned int idx)
{
	struct ptr_list *head = list;
	if (!head)
		return NULL;
	do {
		unsigned int nr = list->nr_;
		if (idx < nr)
			return list->list_[idx];
		else
			idx -= nr;
	} while ((list = list->next_) != head);
	return NULL;
}

void *ptrlist_iter_prev(struct ptr_list_iter *self)
{
	if (self->__head == NULL)
		return NULL;
	self->__nr--;
Lretry:
	if (self->__nr >= 0 && self->__nr < self->__list->nr_) {
		void *ptr = self->__list->list_[self->__nr];
		if (self->__list->rm_ && !ptr) {
			self->__nr--;
			goto Lretry;
		}
		return ptr;
	}
	else if (self->__list->prev_ != self->__head) {
		self->__list = self->__list->prev_;
		self->__nr = self->__list->nr_ - 1;
		goto Lretry;
	}
	return NULL;
}

void ptrlist_iter_split_current(struct ptr_list_iter *self)
{
	if (self->__list->nr_ == N_) {
		/* full so split */
		ptrlist_split_node(self->__list);
		if (self->__nr >= self->__list->nr_) {
			self->__nr -= self->__list->nr_;
			self->__list = self->__list->next_;
		}
	}
}

void ptrlist_iter_insert(struct ptr_list_iter *self, void *newitem)
{
	assert(self->__nr >= 0);
	ptrlist_iter_split_current(self);
	void **__this = self->__list->list_ + self->__nr;
	void **__last = self->__list->list_ + self->__list->nr_ - 1;
	while (__last >= __this) {
		__last[1] = __last[0];
		__last--;
	}
	*__this = newitem;
	self->__list->nr_++;
}

void ptrlist_iter_remove(struct ptr_list_iter *self)
{
	assert(self->__nr >= 0);
	void **__this = self->__list->list_ + self->__nr;
	void **__last = self->__list->list_ + self->__list->nr_ - 1;
	while (__this < __last) {
		__this[0] = __this[1];
		__this++;
	}
	*__this = (void *)((uintptr_t)0xf0f0f0f0);
	self->__list->nr_--;
	self->__nr--;
}

void ptrlist_iter_set(struct ptr_list_iter *self, void *ptr)
{
	assert(self->__list && self->__nr >= 0 &&
		self->__nr < self->__list->nr_);
	self->__list->list_[self->__nr] = ptr;
}

void ptrlist_iter_mark_deleted(struct ptr_list_iter *self)
{
	ptrlist_iter_set(self, NULL);
	self->__list->rm_++;
}

int ptrlist_size(const struct ptr_list *head) {
	int nr = 0;
	if (head) {
		const struct ptr_list *list = head;
		do {
			nr += list->nr_ - list->rm_;
		} while ((list = list->next_) != head);
	}
	return nr;
}

void **ptrlist_add(struct ptr_list **listp, void *ptr, struct allocator *alloc)
{
	struct ptr_list *list = *listp;
	struct ptr_list *last = NULL;
	void **ret;
	int nr;

	if (!list || (nr = (last = list->prev_)->nr_) >= N_) {
		struct ptr_list *newlist =
			(struct ptr_list *)allocator_allocate(alloc, 0);
		newlist->allocator_ = alloc;
		if (!list) {
			newlist->next_ = newlist;
			newlist->prev_ = newlist;
			*listp = newlist;
		}
		else {
			newlist->prev_ = last;
			newlist->next_ = list;
			list->prev_ = newlist;
			last->next_ = newlist;
		}
		last = newlist;
		nr = 0;
	}
	ret = last->list_ + nr;
	*ret = ptr;
	nr++;
	last->nr_ = nr;
	return ret;
}

void *ptrlist_first(struct ptr_list *list)
{
	if (!list)
		return NULL;
	return list->list_[0];
}

void *ptrlist_last(struct ptr_list *list)
{
	if (!list)
		return NULL;
	list = list->prev_;
	return list->list_[list->nr_ - 1];
}

/*
* Linearize the entries of a list up to a total of 'max',
* and return the nr of entries linearized.
*
* The array to linearize into (second argument) should really
* be "void *x[]", but we want to let people fill in any kind
* of pointer array, so let's just call it "void **".
*/
int ptrlist_linearize(struct ptr_list *head, void **arr, int max) {
	int nr = 0;
	if (head && max > 0) {
		struct ptr_list *list = head;

		do {
			int i = list->nr_;
			if (i > max)
				i = max;
			memcpy(arr, list->list_, i * sizeof(void *));
			arr += i;
			nr += i;
			max -= i;
			if (!max)
				break;
		} while ((list = list->next_) != head);
	}
	return nr;
}

/*
* When we've walked the list and deleted entries,
* we may need to re-pack it so that we don't have
* any empty blocks left (empty blocks upset the
* walking code
*/
void ptrlist_pack(struct ptr_list **listp)
{
	struct ptr_list *head = *listp;

	if (head) {
		struct ptr_list *entry = head;
		do {
			struct ptr_list *next;
		restart:
			next = entry->next_;
			if (!entry->nr_) {
				struct ptr_list *prev;
				if (next == entry) {
					allocator_free(entry->allocator_, entry);
					*listp = NULL;
					return;
				}
				prev = entry->prev_;
				prev->next_ = next;
				next->prev_ = prev;
				allocator_free(entry->allocator_, entry);
				if (entry == head) {
					*listp = next;
					head = next;
					entry = next;
					goto restart;
				}
			}
			entry = next;
		} while (entry != head);
	}
}

void ptrlist_remove_all(struct ptr_list **listp) {
	struct ptr_list *tmp, *list = *listp;
	if (!list)
		return;
	list->prev_->next_ = NULL;
	while (list) {
		tmp = list;
		list = list->next_;
		allocator_free(tmp->allocator_, tmp);
	}
	*listp = NULL;
}


int ptrlist_remove(struct ptr_list **self, void *entry, int count) {
	struct ptr_list_iter iter = ptrlist_forward_iterator(*self);
	for (void *ptr = ptrlist_iter_next(&iter); ptr != NULL;
		ptr = ptrlist_iter_next(&iter)) {
		if (ptr == entry) {
			ptrlist_iter_remove(&iter);
			if (!--count)
				goto out;
		}
	}
	assert(count <= 0);
out:
	ptrlist_pack(self);
	return count;
}

int ptrlist_replace(struct ptr_list **self, void *old_ptr, void *new_ptr,
	int count) {
	struct ptr_list_iter iter = ptrlist_forward_iterator(*self);
	for (void *ptr = ptrlist_iter_next(&iter); ptr != NULL;
		ptr = ptrlist_iter_next(&iter)) {
		if (ptr == old_ptr) {
			ptrlist_iter_set(&iter, new_ptr);
			if (!--count)
				goto out;
		}
	}
	assert(count <= 0);
out:
	return count;
}

/* This removes the last entry, but doesn't pack the ptr list */
void *ptrlist_undo_last(struct ptr_list **head)
{
	struct ptr_list *last, *first = *head;

	if (!first)
		return NULL;
	last = first;
	do {
		last = last->prev_;
		if (last->nr_) {
			void *ptr;
			int nr = --last->nr_;
			ptr = last->list_[nr];
			last->list_[nr] = (void *)((intptr_t)0xf1f1f1f1);
			return ptr;
		}
	} while (last != first);
	return NULL;
}


void *ptrlist_delete_last(struct ptr_list **head)
{
	void *ptr = NULL;
	struct ptr_list *last, *first = *head;

	if (!first)
		return NULL;
	last = first->prev_;
	if (last->nr_)
		ptr = last->list_[--last->nr_];
	if (last->nr_ <= 0) {
		first->prev_ = last->prev_;
		last->prev_->next_ = first;
		if (last == first)
			*head = NULL;
		allocator_free(last->allocator_, last);
	}
	return ptr;
}

void ptrlist_concat(struct ptr_list *a, struct ptr_list **b) {
	struct allocator *alloc = NULL;
	struct ptr_list_iter iter = ptrlist_forward_iterator(a);
	if (a)
		alloc = a->allocator_;
	else if (*b)
		alloc = (*b)->allocator_;
	else
		return;
	for (void *ptr = ptrlist_iter_next(&iter); ptr != NULL;
		ptr = ptrlist_iter_next(&iter)) {
		ptrlist_add(b, ptr, alloc);
	}
}

/*
* sort_list: a stable sort for lists.
*
* Time complexity: O(n*log n)
*   [assuming limited zero-element fragments]
*
* Space complexity: O(1).
*
* Stable: yes.
*/

static void array_sort(void **ptr, int nr, void *userdata,
	int(*cmp)(void *, const void *, const void *)) {
	int i;
	for (i = 1; i < nr; i++) {
		void *p = ptr[i];
		if (cmp(userdata, ptr[i - 1], p) > 0) {
			int j = i;
			do {
				ptr[j] = ptr[j - 1];
				if (!--j)
					break;
			} while (cmp(userdata, ptr[j - 1], p) > 0);
			ptr[j] = p;
		}
	}
}

static void verify_sorted(struct ptr_list *l, int n, void *userdata,
	int(*cmp)(void *, const void *, const void *)) {
	int i = 0;
	const void *a;
	struct ptr_list *head = l;

	while (l->nr_ == 0) {
		l = l->next_;
		if (--n == 0)
			return;
		assert(l != head);
	}

	a = l->list_[0];
	while (n > 0) {
		const void *b;
		if (++i >= l->nr_) {
			i = 0;
			l = l->next_;
			n--;
			assert(l != head || n == 0);
			continue;
		}
		b = l->list_[i];
		assert(cmp(userdata, a, b) <= 0);
		a = b;
	}
}

static void flush_to(struct ptr_list *b, void **buffer, int *nbuf) {
	int nr = b->nr_;
	assert(*nbuf >= nr);
	memcpy(b->list_, buffer, nr * sizeof(void *));
	*nbuf = *nbuf - nr;
	memmove(buffer, buffer + nr, *nbuf * sizeof(void *));
}

static void dump_to(struct ptr_list *b, void **buffer, int nbuf) {
	assert(nbuf <= b->nr_);
	memcpy(b->list_, buffer, nbuf * sizeof(void *));
}

// Merge two already-sorted sequences of blocks:
//   (b1_1, ..., b1_n)  and  (b2_1, ..., b2_m)
// Since we may be moving blocks around, we return the new head
// of the merged list.
static struct ptr_list *
merge_block_seqs(struct ptr_list *b1, int n, struct ptr_list *b2,
	int m, void *userdata, int(*cmp)(void *, const void *, const void *)) {
	int i1 = 0, i2 = 0;
	void *buffer[2 * LIST_NODE_NR];
	int nbuf = 0;
	struct ptr_list *newhead = b1;

	// printf ("Merging %d blocks at %p with %d blocks at %p\n", n, b1, m, b2);

	// Skip empty blocks in b2.
	while (b2->nr_ == 0) {
		// BEEN_THERE('F');
		b2 = b2->next_;
		if (--m == 0) {
			// BEEN_THERE('G');
			return newhead;
		}
	}

	// Do a quick skip in case entire blocks from b1 are
	// already less than smallest element in b2.
	while (b1->nr_ == 0 ||
		cmp(userdata, PTR_ENTRY(b1, b1->nr_ - 1), PTR_ENTRY(b2, 0)) < 0) {
		// printf ("Skipping whole block.\n");
		// BEEN_THERE('H');
		b1 = b1->next_;
		if (--n == 0) {
			// BEEN_THERE('I');
			return newhead;
		}
	}

	while (1) {
		void *d1 = PTR_ENTRY(b1, i1);
		void *d2 = PTR_ENTRY(b2, i2);

		assert(i1 >= 0 && i1 < b1->nr_);
		assert(i2 >= 0 && i2 < b2->nr_);
		assert(b1 != b2);
		assert(n > 0);
		assert(m > 0);

		if (cmp(userdata, d1, d2) <= 0) {
			// BEEN_THERE('J');
			buffer[nbuf++] = d1;
			// Element from b1 is smaller
			if (++i1 >= b1->nr_) {
				// BEEN_THERE('L');
				flush_to(b1, buffer, &nbuf);
				do {
					b1 = b1->next_;
					if (--n == 0) {
						// BEEN_THERE('O');
						while (b1 != b2) {
							// BEEN_THERE('P');
							flush_to(b1, buffer, &nbuf);
							b1 = b1->next_;
						}
						assert(nbuf == i2);
						dump_to(b2, buffer, nbuf);
						return newhead;
					}
				} while (b1->nr_ == 0);
				i1 = 0;
			}
		}
		else {
			// BEEN_THERE('K');
			// Element from b2 is smaller
			buffer[nbuf++] = d2;
			if (++i2 >= b2->nr_) {
				struct ptr_list *l = b2;
				// BEEN_THERE('M');
				// OK, we finished with b2.  Pull it out
				// and plug it in before b1.

				b2 = b2->next_;
				b2->prev_ = l->prev_;
				b2->prev_->next_ = b2;
				l->next_ = b1;
				l->prev_ = b1->prev_;
				l->next_->prev_ = l;
				l->prev_->next_ = l;

				if (b1 == newhead) {
					// BEEN_THERE('N');
					newhead = l;
				}

				flush_to(l, buffer, &nbuf);
				b2 = b2->prev_;
				do {
					b2 = b2->next_;
					if (--m == 0) {
						// BEEN_THERE('Q');
						assert(nbuf == i1);
						dump_to(b1, buffer, nbuf);
						return newhead;
					}
				} while (b2->nr_ == 0);
				i2 = 0;
			}
		}
	}
}

void ptrlist_sort(struct ptr_list **plist, void *userdata,
	int(*cmp)(void *, const void *, const void *)) {
	struct ptr_list *head = *plist, *list = head;
	int blocks = 1;

	assert(N_ == LIST_NODE_NR);
	if (!head)
		return;

	// Sort all the sub-lists
	do {
		array_sort(list->list_, list->nr_, userdata, cmp);
#ifdef PARANOIA
		verify_sorted(list, 1, userdata, cmp);
#endif
		list = list->next_;
	} while (list != head);

	// Merge the damn things together
	while (1) {
		struct ptr_list *block1 = head;

		do {
			struct ptr_list *block2 = block1;
			struct ptr_list *next, *newhead;
			int i;

			for (i = 0; i < blocks; i++) {
				block2 = block2->next_;
				if (block2 == head) {
					if (block1 == head) {
						// BEEN_THERE('A');
						*plist = head;
						return;
					}
					// BEEN_THERE('B');
					goto next_pass;
				}
			}

			next = block2;
			for (i = 0; i < blocks;) {
				next = next->next_;
				i++;
				if (next == head) {
					// BEEN_THERE('C');
					break;
				}
				// BEEN_THERE('D');
			}

			newhead = merge_block_seqs(block1, blocks, block2, i, userdata, cmp);
#ifdef PARANOIA
			verify_sorted(newhead, blocks + i, userdata, cmp);
#endif
			if (block1 == head) {
				// BEEN_THERE('E');
				head = newhead;
			}
			block1 = next;
		} while (block1 != head);
	next_pass:
		blocks <<= 1;
	}
}

////////////////////////// AST 

static const char *AST_type = "Ravi.AST";

#define test_Ravi_AST(L, idx) \
  ((struct ast_container *)raviL_testudata(L, idx, AST_type))
#define check_Ravi_AST(L, idx) \
  ((struct ast_container *)raviL_checkudata(L, idx, AST_type))

struct symbol_list;

/*
* Userdata object to hold the abstract syntax tree
*/
struct ast_container {
	struct allocator ast_node_allocator;
	struct allocator string_allocator;
	struct allocator ptrlist_allocator;
	struct allocator block_scope_allocator;
	struct allocator symbol_allocator;
	struct ast_node *main_function;
	struct symbol_list *external_symbols; /* symbols not defined in this chunk */
};

struct ast_node;
DECLARE_PTR_LIST(ast_node_list, struct ast_node);

struct var_type;
DECLARE_PTR_LIST(var_type_list, struct var_type);

DECLARE_PTR_LIST(string_list, char);

struct var_type {
	ravitype_t type;
	const TString *type_name;	/* type name */
};

struct literal {
	ravitype_t type;
	union {
		lua_Integer i;
		lua_Number n;
		const TString *s;
	} u;
};

struct symbol;
DECLARE_PTR_LIST(symbol_list, struct symbol);

struct block_scope;

enum symbol_type {
	SYM_GLOBAL,
	SYM_LOCAL,
	SYM_UPVALUE,
	SYM_LABEL
};

struct symbol {
	enum symbol_type symbol_type;
	ravitype_t value_type;
	union {
		struct {
			const TString *var_name; /* name of the variable */
			struct block_scope *block; /* NULL if global */
		} var;
		struct {
			const TString *label_name;
			struct block_scope *block;
		} label;
		struct {
			struct symbol *var; /* variable reference */
		} upvalue;
	};
};

struct block_scope {
	struct ast_node *function; /* function owning this block - FUNCTION_EXPR */
	struct block_scope *parent; /* parent block */
	struct symbol_list *symbol_list; /* symbols defined in this block */
	struct ast_node_list *statement_list; /* statements in this block */
};

enum ast_node_type {
	AST_NONE, // Used when the node doesn't represent an AST such as test_then_block.
	AST_RETURN_STMT,
	AST_ASSIGN_STMT,
	AST_BREAK_STMT,
	AST_GOTO_STMT,
	AST_LABEL_STMT,
	AST_DO_STMT,
	AST_LOCAL_STMT,
	AST_FUNCTION_STMT,
	AST_IF_STMT,
	AST_WHILE_STMT,
	AST_FORIN_STMT,
	AST_FORNUM_STMT,
	AST_REPEAT_STMT,
	AST_EXPR_STMT, // Also handles assignment statements
	AST_BLOCK_STMT, // Not independent - part of another statement
	AST_LITERAL_EXPR,
	AST_SYMBOL_EXPR,
	AST_Y_INDEX_EXPR, // [] op
	AST_FIELD_SELECTOR_EXPR, // table field
	AST_INDEXED_ASSIGN_EXPR, // table value assign in table constructor
	AST_SUFFIXED_EXPR,
	AST_UNARY_EXPR,
	AST_BINARY_EXPR,
	AST_FUNCTION_EXPR, // function literal
	AST_TABLE_EXPR, // table constructor
	AST_FUNCTION_CALL_EXPR
};

struct ast_node {
	enum ast_node_type type;
	union {
		struct {
			struct ast_node_list *exprlist;
		} return_stmt;
		struct {
			struct symbol *symbol;
		} label_stmt;
		struct {
			const TString *name; /* target label, used to resolve the goto destination */
			struct ast_node *label_stmt; /* Initially this may be NULL for unresolved goto statements */
		} goto_stmt;
		struct {
			struct symbol_list *vars;
			struct ast_node_list *exprlist;
		} local_stmt;
		struct {
			struct ast_node_list *var_expr_list; // Optional var expressions, comma separated
			struct ast_node_list *exr_list; // Comma separated expressions
		} expression_stmt; /* Also covers assignments*/
		struct {
			struct ast_node *name; // base symbol to be looked up
			struct ast_node_list *selectors; // Optional
			struct ast_node *methodname; // Optional 
			struct ast_node *function_expr; // Function's AST
		} function_stmt;
		struct {
			struct block_scope *scope; // The do statement only creates a new scope, used by AST_DO_STMT and AST_BLOCK_STMT
		} do_stmt;
		struct {
			struct ast_node *condition;
			struct block_scope *scope;
		} test_then_block;
		struct {
			struct ast_node_list *if_condition_list; // Actually a list of test_then_blocks
			struct block_scope *else_block;
		} if_stmt;
		struct {
			struct ast_node *condition;
			struct block_scope *loop_body;
			struct block_scope *loop_scope; // This has the label "break"
		} while_or_repeat_stmt;
		struct {
			struct symbol_list *symbols;
			struct ast_node_list *expressions;
			struct block_scope *loop_scope; // This scope contains the symbols and break label
			struct block_scope *loop_body;
		} for_stmt;
		struct {
			struct literal literal;
		} literal_expr;
		struct {
			ravitype_t type;
		} common_expr; // To access the type field common to all expr objects
		struct {
			ravitype_t type;
			struct symbol *var;
		} symbol_expr;
		struct {
			ravitype_t type;
			struct ast_node *expr; // '[' expr ']'
		} index_expr;
		struct {
			ravitype_t type;
			UnOpr unary_op;
			struct ast_node *expr;
		} unary_expr;
		struct {
			ravitype_t type;
			BinOpr binary_op;
			struct ast_node *exprleft;
			struct ast_node *exprright;
		} binary_expr;
		struct {
			ravitype_t type;
			unsigned int is_vararg : 1;
			unsigned int is_method : 1;
			struct ast_node *parent_function; /* parent function or NULL if main chunk */
			struct block_scope *main_block; /* the function's main block */
			struct symbol_list *args; /* arguments, also must be part of the function block's symbol list */
			struct ast_node_list *child_functions; /* child functions declared in this function */
		} function_expr; /* a literal expression whose result is a value of type function */
		struct {
			ravitype_t type;
			struct ast_node *index_expr; /* If NULL means this is a list field with next available index, else specfies index expression */
			struct ast_node *value_expr;
		} indexed_assign_expr; /* Assign values in table constructor */
		struct {
			ravitype_t type;
			struct ast_node_list *expr_list;
		} table_expr; /* table constructor expression */
		struct {
			ravitype_t type;
			struct ast_node *primary_expr;
			struct ast_node_list *suffix_list;
		} suffixed_expr;
		struct {
			ravitype_t type;
			TString *methodname; // Optional methodname
			struct ast_node_list *arg_list; // Call arguments
		} function_call_expr;
	};
};

struct parser_state {
	LexState *ls;
	struct ast_container *container;
	struct ast_node *current_function;
	struct ast_node *current_node;
	struct block_scope *current_scope;
};

static void add_symbol(struct ast_container *container, struct symbol_list **list, struct symbol *sym) {
	ptrlist_add((struct ptr_list **)list, sym, &container->ptrlist_allocator);
}

static void add_ast_node(struct ast_container *container, struct ast_node_list **list, struct ast_node *node) {
	ptrlist_add((struct ptr_list **)list, node, &container->ptrlist_allocator);
}

static struct ast_container * new_ast_container(lua_State *L) {
	struct ast_container *container =
		(struct ast_container *)lua_newuserdata(L, sizeof(struct ast_container));
	allocator_init(&container->ast_node_allocator, "ast nodes", sizeof(struct ast_node), sizeof(double), CHUNK);
	allocator_init(&container->string_allocator, "strings", 0, 0, CHUNK);
	allocator_init(&container->ptrlist_allocator, "ptrlists", sizeof(struct ptr_list), sizeof(double), CHUNK);
	allocator_init(&container->block_scope_allocator, "block scopes", sizeof(struct block_scope), sizeof(double), CHUNK);
	allocator_init(&container->symbol_allocator, "symbols", sizeof(struct symbol), sizeof(double), CHUNK);
	container->main_function = NULL;
	container->external_symbols = NULL;
	raviL_getmetatable(L, AST_type);
	lua_setmetatable(L, -2);
	return container;
}

/* __gc function for IRBuilderHolder */
static int collect_ast_container(lua_State *L) {
	struct ast_container *container = check_Ravi_AST(L, 1);
	allocator_destroy(&container->symbol_allocator);
	allocator_destroy(&container->block_scope_allocator);
	allocator_destroy(&container->ast_node_allocator);
	allocator_destroy(&container->string_allocator);
	allocator_destroy(&container->ptrlist_allocator);
	return 0;
}

/* forward declarations */
static struct ast_node *parse_expression(struct parser_state *);
static void parse_statement_list(struct parser_state *, struct ast_node_list **list);
static struct ast_node *parse_statement(struct parser_state *);
static struct ast_node *new_function(struct parser_state *parser);
static struct block_scope *new_scope(struct parser_state *parser);
static void end_scope(struct parser_state *parser);
static struct ast_node *new_literal_expression(struct parser_state *parser, ravitype_t type);
static struct ast_node *generate_label(struct parser_state *parser, TString *label);

static l_noret error_expected(LexState *ls, int token) {
	luaX_syntaxerror(ls,
		luaO_pushfstring(ls->L, "%s expected", luaX_token2str(ls, token)));
}

static int testnext(LexState *ls, int c) {
	if (ls->t.token == c) {
		luaX_next(ls);
		return 1;
	}
	else return 0;
}

static void check(LexState *ls, int c) {
	if (ls->t.token != c)
		error_expected(ls, c);
}

static void checknext(LexState *ls, int c) {
	check(ls, c);
	luaX_next(ls);
}


/*============================================================*/
/* GRAMMAR RULES */
/*============================================================*/


/*
** check whether current token is in the follow set of a block.
** 'until' closes syntactical blocks, but do not close scope,
** so it is handled in separate.
*/
static int block_follow(LexState *ls, int withuntil) {
	switch (ls->t.token) {
	case TK_ELSE: case TK_ELSEIF:
	case TK_END: case TK_EOS:
		return 1;
	case TK_UNTIL: return withuntil;
	default: return 0;
	}
}

static void enterlevel(LexState *ls) {
	lua_State *L = ls->L;
	++L->nCcalls;
	// checklimit(ls->fs, L->nCcalls, LUAI_MAXCCALLS, "C levels");
}

static inline void leavelevel(LexState *ls) {
	ls->L->nCcalls--;
}


static void check_match(LexState *ls, int what, int who, int where) {
	if (!testnext(ls, what)) {
		if (where == ls->linenumber)
			error_expected(ls, what);
		else {
			luaX_syntaxerror(ls, luaO_pushfstring(ls->L,
				"%s expected (to close %s at line %d)",
				luaX_token2str(ls, what), luaX_token2str(ls, who), where));
		}
	}
}

/* Check that current token is a name, and advance */
static TString *check_name_and_next(LexState *ls) {
	TString *ts;
	check(ls, TK_NAME);
	ts = ls->t.seminfo.ts;
	luaX_next(ls);
	return ts;
}

/* Check that current token is a name, and advance */
static TString *str_checkname(LexState *ls) {
	TString *ts;
	check(ls, TK_NAME);
	ts = ls->t.seminfo.ts;
	luaX_next(ls);
	return ts;
}

/* create a new local variable in function scope, and set the
* variable type (RAVI - added type tt) */
static struct symbol* new_local_symbol(struct parser_state *parser, TString *name, ravitype_t tt, TString *usertype) {
	struct block_scope *scope = parser->current_scope;
	struct symbol *symbol = allocator_allocate(&parser->container->symbol_allocator, 0);
	symbol->value_type = tt;
	symbol->symbol_type = SYM_LOCAL;
	symbol->var.block = scope;
	symbol->var.var_name = name;
	add_symbol(parser->container, &scope->symbol_list, symbol); // Add to the end of the symbol list
	// Note that Lua allows multiple local declarations of the same name
	// so a new instance just gets added to the end
	return symbol;
}

/* create a new label */
static struct symbol* new_label(struct parser_state *parser, TString *name) {
	struct block_scope *scope = parser->current_scope;
	assert(scope);
	struct symbol *symbol = allocator_allocate(&parser->container->symbol_allocator, 0);
	symbol->value_type = RAVI_TANY;
	symbol->symbol_type = SYM_LABEL;
	symbol->label.block = scope;
	symbol->label.label_name = name;
	add_symbol(parser->container, &scope->symbol_list, symbol); // Add to the end of the symbol list
																// Note that Lua allows multiple local declarations of the same name
																// so a new instance just gets added to the end
	return symbol;
}


/* create a new local variable
*/
static struct symbol* new_localvarliteral_(struct parser_state *parser, const char *name, size_t sz) {
	return new_local_symbol(parser, luaX_newstring(parser->ls, name, sz), RAVI_TANY, NULL);
}

/* create a new local variable
*/
#define new_localvarliteral(parser,name) \
	new_localvarliteral_(parser, "" name, (sizeof(name)/sizeof(char))-1)

/* moves the active variable watermark (nactvar) to cover the
* local variables in the current declaration. Also
* sets the starting code location (set to current instruction)
* for nvars new local variables
*/
static void adjustlocalvars(struct parser_state *parser, int nvars) {
}

struct symbol *search_for_variable_in_block(struct block_scope *scope, const TString *varname) {
	struct symbol *symbol;
	// Lookup in reverse order so that we discover the 
	// most recently added local symbol - as Lua allows same
	// symbol to be declared local more than once in a scope
	// Should also work with nesting as the function when parsed
	// will only know about vars declared in parent function until
	// now.
	FOR_EACH_PTR_REVERSE(scope->symbol_list, symbol) {
		switch (symbol->symbol_type) {
		case SYM_LOCAL: {
			if (varname == symbol->var.var_name) {
				return symbol;
			}
			break;
		}
		case SYM_UPVALUE: {
			assert(symbol->upvalue.var->symbol_type == SYM_LOCAL);
			if (varname == symbol->upvalue.var->var.var_name) {
				return symbol;
			}
		}
		default:
			break;
		}
	} END_FOR_EACH_PTR_REVERSE(symbol);
	return NULL;
}

struct symbol *search_for_label_in_block(struct block_scope *scope, const TString *name) {
	struct symbol *symbol;
	// Lookup in reverse order so that we discover the 
	// most recently added local symbol - as Lua allows same
	// symbol to be declared local more than once in a scope
	// Should also work with nesting as the function when parsed
	// will only know about vars declared in parent function until
	// now.
	FOR_EACH_PTR_REVERSE(scope->symbol_list, symbol) {
		switch (symbol->symbol_type) {
		case SYM_LABEL: {
			if (name == symbol->var.var_name) {
				return symbol;
			}
			break;
		}
		default:
			break;
		}
	} END_FOR_EACH_PTR_REVERSE(symbol);
	return NULL;
}

struct symbol *search_globals(struct parser_state *parser, const TString *varname) {
	struct symbol *symbol;
	FOR_EACH_PTR(parser->container->external_symbols, symbol) {
		switch (symbol->symbol_type) {
		case SYM_GLOBAL: {
			if (varname == symbol->var.var_name) {
				return symbol;
			}
			break;
		}
		default:
			assert(0); // Not expecting any other type of global symbol
			break;
		}
	} END_FOR_EACH_PTR(symbol);
	return NULL;
}

/* Searches for a variable starting from current scope, and going up the
* scope chain. If not found and search_globals_too is requested then
* also searches the external symbols
*/
struct symbol *search_for_variable(struct parser_state *parser, const TString *varname, bool search_globals_too) {
	struct block_scope *current_scope = parser->current_scope;
	assert(current_scope && current_scope->function == parser->current_function);
	while (current_scope) {
		struct symbol *symbol = search_for_variable_in_block(current_scope, varname);
		if (symbol)
			return symbol;
		current_scope = current_scope->parent;
	}
	if (search_globals_too) {
		return search_globals(parser, varname);
	}
	return NULL;
}

/* Searches for a label in current function
*/
struct symbol *search_for_label(struct parser_state *parser, const TString *name) {
	struct block_scope *current_scope = parser->current_scope;
	while (current_scope && current_scope->function == parser->current_function) {
		struct symbol *symbol = search_for_label_in_block(current_scope, name);
		if (symbol)
			return symbol;
		current_scope = current_scope->parent;
	}
	return NULL;
}

/* intialize var with the variable name - may be local,
* global or upvalue - note that var->k will be set to
* VLOCAL (local var), or VINDEXED or VUPVAL? TODO check
*/
static struct ast_node *singlevar(struct parser_state *parser) {
	TString *varname = check_name_and_next(parser->ls);
	struct symbol *symbol = search_for_variable(parser, varname, false);
	if (symbol) {
		if (symbol->symbol_type == SYM_LOCAL) {
			// If the symbol occurred in a parent function then we
			// need to construct an upvalue
			if (symbol->var.block->function != parser->current_function) {
				// construct upvalue
				struct symbol *upvalue = allocator_allocate(&parser->container->symbol_allocator, 0);
				upvalue->symbol_type = SYM_UPVALUE;
				upvalue->upvalue.var = symbol;
				upvalue->value_type = symbol->value_type;
				add_symbol(parser->container, &parser->current_scope->symbol_list, upvalue);
				symbol = upvalue;
			}
		}
	}
	else {
		// Return global symbol
		struct symbol *global = allocator_allocate(&parser->container->symbol_allocator, 0);
		global->symbol_type = SYM_GLOBAL;
		global->var.var_name = varname;
		global->var.block = NULL;
		global->value_type = RAVI_TANY;
		// We don't add globals to any scope so that they are 
		// always looked up
		symbol = global;
	}
	struct ast_node *symbol_expr = allocator_allocate(&parser->container->ast_node_allocator, 0);
	symbol_expr->type = AST_SYMBOL_EXPR;
	symbol_expr->symbol_expr.type = symbol->value_type;
	symbol_expr->symbol_expr.var = symbol;
	return symbol_expr;
}

static void adjust_assign(struct parser_state *parser, int nvars, int nexps, expdesc *e) {
	LexState *ls = parser->ls;
	FuncState *fs = ls->fs;
	int extra = nvars - nexps;
	if (hasmultret(e->k)) {
		extra++;  /* includes call itself */
		if (extra < 0) extra = 0;
		/* following adjusts the C operand in the OP_CALL instruction */
		luaK_setreturns(fs, e, extra);  /* last exp. provides the difference */
		if (extra > 1) luaK_reserveregs(fs, extra - 1);
	}
	else {
		if (e->k != VVOID) luaK_exp2nextreg(fs, e);  /* close last expression */
		if (extra > 0) {
			int reg = fs->freereg;
			luaK_reserveregs(fs, extra);
			/* RAVI TODO for typed variables we should not set to nil? */
			luaK_nil(fs, reg, extra);
		}
	}
	if (nexps > nvars)
		ls->fs->freereg -= nexps - nvars;  /* remove extra values */
}

/*
** create a label named 'break' to resolve break statements
*/
static void breaklabel(struct parser_state *parser) {
}

/*
** generates an error for an undefined 'goto'; choose appropriate
** message when label name is a reserved word (which can only be 'break')
*/
static l_noret undefgoto(struct parser_state *parser, Labeldesc *gt) {
	LexState *ls = parser->ls;
	const char *msg = isreserved(gt->name)
		? "<%s> at line %d not inside a loop"
		: "no visible label '%s' for <goto> at line %d";
	msg = luaO_pushfstring(ls->L, msg, getstr(gt->name), gt->line);
	semerror(ls, msg);
}

/*============================================================*/
/* GRAMMAR RULES */
/*============================================================*/

static struct ast_node *new_string_literal(struct parser_state *parser, const TString *ts) {
	struct ast_node *node = allocator_allocate(&parser->container->ast_node_allocator, 0);
	node->type = AST_LITERAL_EXPR;
	node->literal_expr.literal.type = RAVI_TSTRING;
	node->literal_expr.literal.u.s = ts;
	return node;
}

static struct ast_node *new_field_selector(struct parser_state *parser, const TString *ts) {
	struct ast_node *index = allocator_allocate(&parser->container->ast_node_allocator, 0);
	index->type = AST_FIELD_SELECTOR_EXPR;
	index->index_expr.expr = new_string_literal(parser, ts);
	index->index_expr.type = RAVI_TANY;
	return index;
}

/*
* Parse ['.' | ':'] NAME
*/
static struct ast_node *parse_field_selector(struct parser_state *parser) {
	LexState *ls = parser->ls;
	/* fieldsel -> ['.' | ':'] NAME */
	luaX_next(ls);  /* skip the dot or colon */
	TString *ts = check_name_and_next(ls);
	return new_field_selector(parser, ts);
}

/*
* Parse '[' expr ']
*/
static struct ast_node *parse_yindex(struct parser_state *parser) {
	LexState *ls = parser->ls;
	/* index -> '[' expr ']' */
	luaX_next(ls);  /* skip the '[' */
	struct ast_node *expr = parse_expression(parser);
	checknext(ls, ']');

	struct ast_node *index = allocator_allocate(&parser->container->ast_node_allocator, 0);
	index->type = AST_Y_INDEX_EXPR;
	index->index_expr.expr = expr;
	index->index_expr.type = RAVI_TANY;
	return index;
}


/*
** {======================================================================
** Rules for Constructors
** =======================================================================
*/

static struct ast_node *new_indexed_assign_expr(struct parser_state *parser, struct ast_node *index_expr, struct ast_node *value_expr) {
	struct ast_node *set = allocator_allocate(&parser->container->ast_node_allocator, 0);
	set->type = AST_INDEXED_ASSIGN_EXPR;
	set->indexed_assign_expr.index_expr = index_expr;
	set->indexed_assign_expr.value_expr = value_expr;
	set->indexed_assign_expr.type = value_expr->common_expr.type; /* type of indexed assignment is same as the value*/
	return set;
}

static struct ast_node *parse_recfield(struct parser_state *parser) {
	LexState *ls = parser->ls;
	/* recfield -> (NAME | '['exp1']') = exp1 */
	struct ast_node *index_expr;
	if (ls->t.token == TK_NAME) {
		const TString *ts = check_name_and_next(ls);
		index_expr = new_field_selector(parser, ts);
	}
	else  /* ls->t.token == '[' */
		index_expr = parse_yindex(parser);
	checknext(ls, '=');
	struct ast_node *value_expr = parse_expression(parser);
	return new_indexed_assign_expr(parser, index_expr, value_expr);
}

static struct ast_node *parse_listfield(struct parser_state *parser) {
	/* listfield -> exp */
	struct ast_node *value_expr = parse_expression(parser);
	return new_indexed_assign_expr(parser, NULL, value_expr);
}

static struct ast_node *parse_field(struct parser_state *parser) {
	LexState *ls = parser->ls;
	/* field -> listfield | recfield */
	switch (ls->t.token) {
	case TK_NAME: {  /* may be 'listfield' or 'recfield' */
		if (luaX_lookahead(ls) != '=')  /* expression? */
			return parse_listfield(parser);
		else
			return parse_recfield(parser);
		break;
	}
	case '[': {
		return parse_recfield(parser);
		break;
	}
	default: {
		return parse_listfield(parser);
		break;
	}
	}
	return NULL;
}


static struct ast_node *parse_table_constructor(struct parser_state *parser) {
	LexState *ls = parser->ls;
	/* constructor -> '{' [ field { sep field } [sep] ] '}'
	sep -> ',' | ';' */
	int line = ls->linenumber;
	checknext(ls, '{');
	struct ast_node *table_expr = allocator_allocate(&parser->container->ast_node_allocator, 0);
	table_expr->table_expr.type = RAVI_TTABLE;
	table_expr->table_expr.expr_list = NULL;
	table_expr->type = AST_TABLE_EXPR;
	do {
		if (ls->t.token == '}') break;
		struct ast_node *field_expr = parse_field(parser);
		add_ast_node(parser->container, &table_expr->table_expr.expr_list, field_expr);
	} while (testnext(ls, ',') || testnext(ls, ';'));
	check_match(ls, '}', '{', line);
	return table_expr;
}

/* }====================================================================== */

/*
* We would like to allow user defined types to contain the sequence
* NAME [. NAME]+
* The initial NAME is supplied.
* Returns extended name.
* Note that the returned string will be anchored in the Lexer and must
* be anchored somewhere else by the time parsing finishes
*/
static TString *user_defined_type_name(LexState *ls, TString *typename) {
	char buffer[128];
	size_t len = 0;
	if (testnext(ls, '.')) {
		char buffer[128] = { 0 };
		const char *str = getstr(typename);
		len = strlen(str);
		if (len >= sizeof buffer) {
			luaX_syntaxerror(ls, "User defined type name is too long");
			return typename;
		}
		snprintf(buffer, sizeof buffer, "%s", str);
		do {
			typename = str_checkname(ls);
			str = getstr(typename);
			size_t newlen = len + strlen(str) + 1;
			if (newlen >= sizeof buffer) {
				luaX_syntaxerror(ls, "User defined type name is too long");
				return typename;
			}
			snprintf(buffer + len, sizeof buffer - len, ".%s", str);
			len = newlen;
		} while (testnext(ls, '.'));
		typename = luaX_newstring(ls, buffer, strlen(buffer));
	}
	return typename;
}

/* RAVI Parse
*   name : type
*   where type is 'integer', 'integer[]',
*                 'number', 'number[]'
*/
static struct symbol * declare_local_variable(struct parser_state *parser, TString **pusertype) {
	LexState *ls = parser->ls;
	/* assume a dynamic type */
	ravitype_t tt = RAVI_TANY;
	TString *name = check_name_and_next(ls);
	if (testnext(ls, ':')) {
		TString *typename = str_checkname(ls); /* we expect a type name */
		const char *str = getstr(typename);
		/* following is not very nice but easy as
		* the lexer doesn't need to be changed
		*/
		if (strcmp(str, "integer") == 0)
			tt = RAVI_TNUMINT;
		else if (strcmp(str, "number") == 0)
			tt = RAVI_TNUMFLT;
		else if (strcmp(str, "closure") == 0)
			tt = RAVI_TFUNCTION;
		else if (strcmp(str, "table") == 0)
			tt = RAVI_TTABLE;
		else if (strcmp(str, "string") == 0)
			tt = RAVI_TSTRING;
		else if (strcmp(str, "boolean") == 0)
			tt = RAVI_TBOOLEAN;
		else if (strcmp(str, "any") == 0)
			tt = RAVI_TANY;
		else {
			/* default is a userdata type */
			tt = RAVI_TUSERDATA;
			typename = user_defined_type_name(ls, typename);
			str = getstr(typename);
			*pusertype = typename;
		}
		if (tt == RAVI_TNUMFLT || tt == RAVI_TNUMINT) {
			/* if we see [] then it is an array type */
			if (testnext(ls, '[')) {
				checknext(ls, ']');
				tt = (tt == RAVI_TNUMFLT) ? RAVI_TARRAYFLT : RAVI_TARRAYINT;
			}
		}
	}
	return new_local_symbol(parser, name, tt, *pusertype);
}

static bool parse_parameter_list(struct parser_state *parser, struct symbol_list **list) {
	LexState *ls = parser->ls;
	/* parlist -> [ param { ',' param } ] */
	int nparams = 0;
	bool is_vararg = false;
	enum { N = MAXVARS + 10 };
	int vars[N] = { 0 };
	TString *typenames[N] = { NULL };
	if (ls->t.token != ')') {  /* is 'parlist' not empty? */
		do {
			switch (ls->t.token) {
			case TK_NAME: {  /* param -> NAME */
							 /* RAVI change - add type */
				struct symbol *symbol = declare_local_variable(parser, &typenames[nparams]);
				add_symbol(parser->container, list, symbol);
				nparams++;
				break;
			}
			case TK_DOTS: {  /* param -> '...' */
				luaX_next(ls);
				is_vararg = true;  /* declared vararg */
				break;
			}
			default: luaX_syntaxerror(ls, "<name> or '...' expected");
			}
		} while (!is_vararg && testnext(ls, ','));
	}
	adjustlocalvars(parser, nparams);
	return is_vararg;
}


static void parse_function_body(struct parser_state *parser, struct ast_node *func_ast, int ismethod, int line) {
	LexState *ls = parser->ls;
	/* body ->  '(' parlist ')' block END */
	checknext(ls, '(');
	if (ismethod) {
		struct symbol *symbol = new_localvarliteral(parser, "self");  /* create 'self' parameter */
		add_symbol(parser->container, &func_ast->function_expr.args, symbol);
		adjustlocalvars(parser, 1);
	}
	bool is_vararg = parse_parameter_list(parser, &func_ast->function_expr.args);
	func_ast->function_expr.is_vararg = is_vararg;
	func_ast->function_expr.is_method = ismethod;
	checknext(ls, ')');
	parse_statement_list(parser, &parser->current_scope->statement_list);
	check_match(ls, TK_END, TK_FUNCTION, line);
}

/* parse expression list */
static int parse_expression_list(struct parser_state *parser, struct ast_node_list **list) {
	LexState *ls = parser->ls;
	/* explist -> expr { ',' expr } */
	int n = 1;  /* at least one expression */
	struct ast_node *expr = parse_expression(parser);
	add_ast_node(parser->container, list, expr);
	while (testnext(ls, ',')) {
		expr = parse_expression(parser);
		add_ast_node(parser->container, list, expr);
		n++;
	}
	return n;
}

/* parse function arguments */
static struct ast_node *parse_function_call(struct parser_state *parser, TString *methodname, int line) {
	LexState *ls = parser->ls;
	int base, nparams;
	struct ast_node *call_expr = allocator_allocate(&parser->container->ast_node_allocator, 0);
	call_expr->type = AST_FUNCTION_CALL_EXPR;
	call_expr->function_call_expr.methodname = methodname;
	call_expr->function_call_expr.arg_list = NULL;
	call_expr->function_call_expr.type = RAVI_TANY;
	switch (ls->t.token) {
	case '(': {  /* funcargs -> '(' [ explist ] ')' */
		luaX_next(ls);
		if (ls->t.token == ')')  /* arg list is empty? */
			// args.k = VVOID;
			;
		else {
			parse_expression_list(parser, &call_expr->function_call_expr.arg_list);
		}
		check_match(ls, ')', '(', line);
		break;
	}
	case '{': {  /* funcargs -> constructor */
		struct ast_node *table_expr = parse_table_constructor(parser);
		add_ast_node(parser->container, &call_expr->function_call_expr.arg_list, table_expr);
		break;
	}
	case TK_STRING: {  /* funcargs -> STRING */
		struct ast_node *string_expr = new_literal_expression(parser, RAVI_TSTRING);
		string_expr->literal_expr.literal.u.s = ls->t.seminfo.ts;
		add_ast_node(parser->container, &call_expr->function_call_expr.arg_list, string_expr);
		luaX_next(ls); 
		break;
	}
	default: {
		luaX_syntaxerror(ls, "function arguments expected");
	}
	}
	return call_expr;
}




/*
** {======================================================================
** Expression parsing
** =======================================================================
*/

/* primary expression - name or subexpression */
static struct ast_node *parse_primary_expression(struct parser_state *parser) {
	LexState *ls = parser->ls;
	struct ast_node *primary_expr = NULL;
	/* primaryexp -> NAME | '(' expr ')' */
	switch (ls->t.token) {
	case '(': {
		int line = ls->linenumber;
		luaX_next(ls);
		primary_expr = parse_expression(parser);
		check_match(ls, ')', '(', line);
		break;
	}
	case TK_NAME: {
		primary_expr = singlevar(parser);
		break;
	}
	default: {
		luaX_syntaxerror(ls, "unexpected symbol");
	}
	}
	assert(primary_expr);
	return primary_expr;
}

/* variable or field access or function call */
static struct ast_node *parse_suffixed_expression(struct parser_state *parser) {
	LexState *ls = parser->ls;
	/* suffixedexp ->
	primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs } */
	int line = ls->linenumber;
	struct ast_node *suffixed_expr = allocator_allocate(&parser->container->ast_node_allocator, 0);
	suffixed_expr->suffixed_expr.primary_expr = parse_primary_expression(parser);
	suffixed_expr->type = AST_SUFFIXED_EXPR;
	suffixed_expr->suffixed_expr.type = suffixed_expr->suffixed_expr.primary_expr->common_expr.type;
	suffixed_expr->suffixed_expr.suffix_list = NULL;
	for (;;) {
		switch (ls->t.token) {
		case '.': {  /* fieldsel */
			struct ast_node *suffix = parse_field_selector(parser);
			add_ast_node(parser->container, &suffixed_expr->suffixed_expr.suffix_list, suffix);
			suffixed_expr->suffixed_expr.type = RAVI_TANY;
			break;
		}
		case '[': {  /* '[' exp1 ']' */
			struct ast_node *suffix = parse_yindex(parser);
			add_ast_node(parser->container, &suffixed_expr->suffixed_expr.suffix_list, suffix);
			suffixed_expr->suffixed_expr.type = RAVI_TANY;
			break;
		}
		case ':': {  /* ':' NAME funcargs */
			luaX_next(ls);
			TString *methodname = check_name_and_next(ls);
			struct ast_node *suffix = parse_function_call(parser, methodname, line);
			add_ast_node(parser->container, &suffixed_expr->suffixed_expr.suffix_list, suffix);
			break;
		}
		case '(': case TK_STRING: case '{': {  /* funcargs */
			struct ast_node *suffix = parse_function_call(parser, NULL, line);
			add_ast_node(parser->container, &suffixed_expr->suffixed_expr.suffix_list, suffix);
			break;
		}
		default: return suffixed_expr;
		}
	}
}

static struct ast_node *new_literal_expression(struct parser_state *parser, ravitype_t type) {
	struct ast_node *expr = allocator_allocate(&parser->container->ast_node_allocator, 0);
	expr->type = AST_LITERAL_EXPR;
	expr->literal_expr.literal.type = type;
	return expr;
}

static struct ast_node *parse_simple_expression(struct parser_state *parser) {
	LexState *ls = parser->ls;
	/* simpleexp -> FLT | INT | STRING | NIL | TRUE | FALSE | ... |
	constructor | FUNCTION body | suffixedexp */
	struct ast_node *expr = NULL;
	switch (ls->t.token) {
	case TK_FLT: {
		expr = new_literal_expression(parser, RAVI_TNUMFLT);
		expr->literal_expr.literal.u.n = ls->t.seminfo.r;
		break;
	}
	case TK_INT: {
		expr = new_literal_expression(parser, RAVI_TNUMINT);
		expr->literal_expr.literal.u.i = ls->t.seminfo.i;
		break;
	}
	case TK_STRING: {
		expr = new_literal_expression(parser, RAVI_TSTRING);
		expr->literal_expr.literal.u.s = ls->t.seminfo.ts;
		break;
	}
	case TK_NIL: {
		expr = new_literal_expression(parser, RAVI_TNIL);
		expr->literal_expr.literal.u.i = -1;
		break;
	}
	case TK_TRUE: {
		expr = new_literal_expression(parser, RAVI_TBOOLEAN);
		expr->literal_expr.literal.u.i = 1;
		break;
	}
	case TK_FALSE: {
		expr = new_literal_expression(parser, RAVI_TBOOLEAN);
		expr->literal_expr.literal.u.i = 0;
		break;
	}
	case TK_DOTS: {  /* vararg */
		// Not handled yet
		expr = NULL;
		break;
	}
	case '{': {  /* constructor */
		return parse_table_constructor(parser);
	}
	case TK_FUNCTION: {
		luaX_next(ls);
		struct ast_node *function_ast = new_function(parser);
		parse_function_body(parser, function_ast, 0, ls->linenumber);
		return function_ast;
	}
	default: {
		return parse_suffixed_expression(parser);
	}
	}
	luaX_next(ls);
	return expr;
}


static UnOpr get_unary_opr(int op) {
	switch (op) {
	case TK_NOT: return OPR_NOT;
	case '-': return OPR_MINUS;
	case '~': return OPR_BNOT;
	case '#': return OPR_LEN;
	case TK_TO_INTEGER: return OPR_TO_INTEGER;
	case TK_TO_NUMBER: return OPR_TO_NUMBER;
	case TK_TO_INTARRAY: return OPR_TO_INTARRAY;
	case TK_TO_NUMARRAY: return OPR_TO_NUMARRAY;
	case TK_TO_TABLE: return OPR_TO_TABLE;
	case TK_TO_STRING: return OPR_TO_STRING;
	case TK_TO_CLOSURE: return OPR_TO_CLOSURE;
	case '@': return OPR_TO_TYPE;
	default: return OPR_NOUNOPR;
	}
}


static BinOpr get_binary_opr(int op) {
	switch (op) {
	case '+': return OPR_ADD;
	case '-': return OPR_SUB;
	case '*': return OPR_MUL;
	case '%': return OPR_MOD;
	case '^': return OPR_POW;
	case '/': return OPR_DIV;
	case TK_IDIV: return OPR_IDIV;
	case '&': return OPR_BAND;
	case '|': return OPR_BOR;
	case '~': return OPR_BXOR;
	case TK_SHL: return OPR_SHL;
	case TK_SHR: return OPR_SHR;
	case TK_CONCAT: return OPR_CONCAT;
	case TK_NE: return OPR_NE;
	case TK_EQ: return OPR_EQ;
	case '<': return OPR_LT;
	case TK_LE: return OPR_LE;
	case '>': return OPR_GT;
	case TK_GE: return OPR_GE;
	case TK_AND: return OPR_AND;
	case TK_OR: return OPR_OR;
	default: return OPR_NOBINOPR;
	}
}


static const struct {
	lu_byte left;  /* left priority for each binary operator */
	lu_byte right; /* right priority */
} priority[] = {  /* ORDER OPR */
	{ 10, 10 },{ 10, 10 },           /* '+' '-' */
	{ 11, 11 },{ 11, 11 },           /* '*' '%' */
	{ 14, 13 },                  /* '^' (right associative) */
	{ 11, 11 },{ 11, 11 },           /* '/' '//' */
	{ 6, 6 },{ 4, 4 },{ 5, 5 },   /* '&' '|' '~' */
	{ 7, 7 },{ 7, 7 },           /* '<<' '>>' */
	{ 9, 8 },                   /* '..' (right associative) */
	{ 3, 3 },{ 3, 3 },{ 3, 3 },   /* ==, <, <= */
	{ 3, 3 },{ 3, 3 },{ 3, 3 },   /* ~=, >, >= */
	{ 2, 2 },{ 1, 1 }            /* and, or */
};

#define UNARY_PRIORITY	12  /* priority for unary operators */


/*
** subexpr -> (simpleexp | unop subexpr) { binop subexpr }
** where 'binop' is any binary operator with a priority higher than 'limit'
*/
static struct ast_node *parse_sub_expression(struct parser_state *parser, int limit, BinOpr *untreated_op) {
	LexState *ls = parser->ls;
	BinOpr op;
	UnOpr uop;
	enterlevel(ls);
	struct ast_node *expr = NULL;
	uop = get_unary_opr(ls->t.token);
	if (uop != OPR_NOUNOPR) {
		int line = ls->linenumber;
		// RAVI change - get usertype if @<name>
		TString *usertype = NULL;
		if (uop == OPR_TO_TYPE) {
			usertype = ls->t.seminfo.ts;
			luaX_next(ls);
			// Check and expand to extended name if necessary
			usertype = user_defined_type_name(ls, usertype);
		}
		else {
			luaX_next(ls);
		}
		BinOpr ignored;
		struct ast_node *subexpr = parse_sub_expression(parser, UNARY_PRIORITY, &ignored);
		expr = allocator_allocate(&parser->container->ast_node_allocator, 0);
		expr->type = AST_UNARY_EXPR;
		expr->unary_expr.expr = subexpr;
		expr->unary_expr.type = subexpr->common_expr.type;
		expr->unary_expr.unary_op = uop;
	}
	else {
		expr = parse_simple_expression(parser);
	}
	/* expand while operators have priorities higher than 'limit' */
	op = get_binary_opr(ls->t.token);
	while (op != OPR_NOBINOPR && priority[op].left > limit) {
		BinOpr nextop;
		int line = ls->linenumber;
		luaX_next(ls);
		/* read sub-expression with higher priority */
		struct ast_node *exprright = parse_sub_expression(parser, priority[op].right, &nextop);

		struct ast_node *binexpr = allocator_allocate(&parser->container->ast_node_allocator, 0);
		binexpr->type = AST_BINARY_EXPR;
		binexpr->binary_expr.exprleft = expr;
		binexpr->binary_expr.exprright = exprright;
		binexpr->binary_expr.binary_op = op;
		binexpr->binary_expr.type = expr->common_expr.type; // FIXME - needs to be worked out
		expr = binexpr; // Becomes the left expr for next iteration
		op = nextop;
	}
	leavelevel(ls);
	*untreated_op = op;  /* return first untreated operator */
	return expr;
}


static struct ast_node *parse_expression(struct parser_state *parser) {
	BinOpr ignored;
	return parse_sub_expression(parser, 0, &ignored);
}

/* }==================================================================== */



/*
** {======================================================================
** Rules for Statements
** =======================================================================
*/


static struct block_scope *parse_block(struct parser_state *parser) {
	/* block -> statlist */
	struct block_scope *scope = new_scope(parser);
	parse_statement_list(parser, &parser->current_scope->statement_list);
	end_scope(parser);
	return scope;
}



/* parse condition in a repeat statement or an if control structure
* called by repeatstat(), test_then_block()
*/
static struct ast_node *parse_condition(struct parser_state *parser) {
	/* cond -> exp */
	return parse_expression(parser);                   /* read condition */
}

static struct ast_node *parse_goto_statment(struct parser_state *parser) {
	LexState *ls = parser->ls;
	int line = ls->linenumber;
	TString *label;
	int g;
	if (testnext(ls, TK_GOTO))
		label = check_name_and_next(ls);
	else {
		luaX_next(ls);  /* skip break */
		label = luaX_newstring(ls, "break", sizeof "break");
	}
	// Resolve labels in the end?
	struct ast_node *goto_stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	goto_stmt->type = AST_GOTO_STMT;
	goto_stmt->goto_stmt.name = label;
	goto_stmt->goto_stmt.label_stmt = NULL; // unresolved
	return goto_stmt;
}

/* skip no-op statements */
static void skip_noop_statements(struct parser_state *parser) {
	LexState *ls = parser->ls;
	while (ls->t.token == ';' || ls->t.token == TK_DBCOLON)
		parse_statement(parser);
}

static struct ast_node *generate_label(struct parser_state *parser, TString *label) {
	struct symbol *symbol = new_label(parser, label);
	struct ast_node *label_stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	label_stmt->type = AST_LABEL_STMT;
	label_stmt->label_stmt.symbol = symbol;
	return label_stmt;
}

static struct ast_node *parse_label_statement(struct parser_state *parser, TString *label, int line) {
	LexState *ls = parser->ls;
	/* label -> '::' NAME '::' */
	checknext(ls, TK_DBCOLON);  /* skip double colon */
	/* create new entry for this label */
	struct ast_node *label_stmt = generate_label(parser, label);
	skip_noop_statements(parser);  /* skip other no-op statements */
	return label_stmt;
}

/* parse a while-do control structure, body processed by block()
* called by statement()
*/
static struct ast_node *parse_while_statement(struct parser_state *parser, int line) {
	LexState *ls = parser->ls;
	/* whilestat -> WHILE cond DO block END */
	luaX_next(ls);  /* skip WHILE */
	struct ast_node *stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	stmt->type = AST_WHILE_STMT;
	stmt->while_or_repeat_stmt.loop_scope = NULL;
	stmt->while_or_repeat_stmt.loop_body = NULL;
	stmt->while_or_repeat_stmt.condition = parse_condition(parser);
	stmt->while_or_repeat_stmt.loop_scope = new_scope(parser);
	checknext(ls, TK_DO);
	stmt->while_or_repeat_stmt.loop_body = parse_block(parser);
	check_match(ls, TK_END, TK_WHILE, line);
	struct ast_node *breaklabel = generate_label(parser, luaX_newstring(ls, "break", sizeof "break"));
	add_ast_node(parser->container, &parser->current_scope->statement_list, breaklabel);
	end_scope(parser);
	return stmt;
}

/* parse a repeat-until control structure, body parsed by statlist()
* called by statement()
*/
static struct ast_node *parse_repeat_statement(struct parser_state *parser, int line) {
	LexState *ls = parser->ls;
	/* repeatstat -> REPEAT block UNTIL cond */
	luaX_next(ls);  /* skip REPEAT */
	struct ast_node *stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	stmt->type = AST_REPEAT_STMT;
	stmt->while_or_repeat_stmt.condition = NULL;
	stmt->while_or_repeat_stmt.loop_body = NULL;
	stmt->while_or_repeat_stmt.loop_scope = new_scope(parser); /* loop block */
	stmt->while_or_repeat_stmt.loop_body = new_scope(parser); /* scope block */
	parse_statement_list(parser, &parser->current_scope->statement_list); // added to loop block
	check_match(ls, TK_UNTIL, TK_REPEAT, line);
	stmt->while_or_repeat_stmt.condition = parse_condition(parser);  /* read condition (inside scope block) */
	end_scope(parser);
	struct ast_node *breaklabel = generate_label(parser, luaX_newstring(ls, "break", sizeof "break"));
	add_ast_node(parser->container, &parser->current_scope->statement_list, breaklabel);
	end_scope(parser);
	return stmt;
}

/* parse a for loop body for both versions of the for loop
* called by fornum(), forlist()
*/
static void parse_forbody(struct parser_state *parser, struct ast_node* stmt, int line, int nvars, int isnum) {
	LexState *ls = parser->ls;
	/* forbody -> DO block */
	checknext(ls, TK_DO);	
	struct ast_node *blockstmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	blockstmt->type = AST_BLOCK_STMT;
	blockstmt->do_stmt.scope = stmt->for_stmt.loop_body = parse_block(parser);
	add_ast_node(parser->container, &parser->current_scope->statement_list, blockstmt);
}

/* parse a numerical for loop, calls forbody()
* called from forstat()
*/
static void parse_fornum_statement(struct parser_state *parser, struct ast_node *stmt, TString *varname, int line) {
	LexState *ls = parser->ls;
	/* fornum -> NAME = exp1,exp1[,exp1] forbody */
	add_symbol(parser->container, &stmt->for_stmt.symbols, new_localvarliteral(parser, "(for index)"));
	add_symbol(parser->container, &stmt->for_stmt.symbols, new_localvarliteral(parser, "(for limit)"));
	add_symbol(parser->container, &stmt->for_stmt.symbols, new_localvarliteral(parser, "(for step)"));
	add_symbol(parser->container, &stmt->for_stmt.symbols, new_local_symbol(parser, varname, RAVI_TANY, NULL));
	/* The fornum sets up its own variables as above.
	These are expected to hold numeric values - but from Ravi's
	point of view we need to know if the variable is an integer or
	double. So we need to check if this can be determined from the
	fornum expressions. If we can then we will set the
	fornum variables to the type we discover.
	*/
	checknext(ls, '=');
	/* get the type of each expression */
	add_ast_node(parser->container, &stmt->for_stmt.expressions, parse_expression(parser));  /* initial value */
	checknext(ls, ',');
	add_ast_node(parser->container, &stmt->for_stmt.expressions, parse_expression(parser));  /* limit */
	if (testnext(ls, ',')) {
		add_ast_node(parser->container, &stmt->for_stmt.expressions, parse_expression(parser));  /* optional step */
	}
	parse_forbody(parser, stmt, line, 1, 1);
}

/* parse a generic for loop, calls forbody()
* called from forstat()
*/
static void parse_for_list(struct parser_state *parser, struct ast_node *stmt, TString *indexname) {
	LexState *ls = parser->ls;
	/* forlist -> NAME {,NAME} IN explist forbody */
	int nvars = 4; /* gen, state, control, plus at least one declared var */
	int line;
	/* create control variables */
	add_symbol(parser->container, &stmt->for_stmt.symbols, new_localvarliteral(parser, "(for generator)"));
	add_symbol(parser->container, &stmt->for_stmt.symbols, new_localvarliteral(parser, "(for state)"));
	add_symbol(parser->container, &stmt->for_stmt.symbols, new_localvarliteral(parser, "(for control)"));
	/* create declared variables */
	add_symbol(parser->container, &stmt->for_stmt.symbols, new_local_symbol(parser, indexname, RAVI_TANY, NULL)); /* RAVI TODO for name:type syntax? */
	while (testnext(ls, ',')) {
		add_symbol(parser->container, &stmt->for_stmt.symbols, new_local_symbol(parser, check_name_and_next(ls), RAVI_TANY, NULL)); /* RAVI change - add type */
		nvars++;
	}
	checknext(ls, TK_IN);
	parse_expression_list(parser, &stmt->for_stmt.expressions);
	line = ls->linenumber;
	parse_forbody(parser, stmt, line, nvars - 3, 0);
}

/* initial parsing of a for loop - calls fornum() or forlist()
* called from statement()
*/
static struct ast_node *parse_for_statement(struct parser_state *parser, int line) {
	LexState *ls = parser->ls;
	/* forstat -> FOR (fornum | forlist) END */
	TString *varname;
	struct ast_node *stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	stmt->type = AST_NONE;
	stmt->for_stmt.symbols = NULL;
	stmt->for_stmt.expressions = NULL;
	stmt->for_stmt.loop_scope = new_scope(parser);
	stmt->for_stmt.loop_body = NULL;
	luaX_next(ls);  /* skip 'for' */
	varname = check_name_and_next(ls);  /* first variable name */
	switch (ls->t.token) {
	case '=': 
		stmt->type = AST_FORNUM_STMT;
		parse_fornum_statement(parser, stmt, varname, line); 
		break;
	case ',': case TK_IN: 
		stmt->type = AST_FORIN_STMT;
		parse_for_list(parser, stmt, varname); 
		break;
	default: 
		luaX_syntaxerror(ls, "'=' or 'in' expected");
	}
	check_match(ls, TK_END, TK_FOR, line);
	/* add a break label */
	struct ast_node *breaklabel = generate_label(parser, luaX_newstring(ls, "break", sizeof "break"));
	add_ast_node(parser->container, &parser->current_scope->statement_list, breaklabel);
	end_scope(parser);
	return stmt;
}

/* parse if cond then block - called from ifstat() */
static struct ast_node *parse_if_cond_then_block(struct parser_state *parser, int *escapelist) {
	LexState *ls = parser->ls;
	/* test_then_block -> [IF | ELSEIF] cond THEN block */
	int jf;         /* instruction to skip 'then' code (if condition is false) */
	luaX_next(ls);  /* skip IF or ELSEIF */
	struct ast_node *test_then_block = allocator_allocate(&parser->container->ast_node_allocator, 0);
	test_then_block->type = AST_NONE; // This is not an AST node on its own
	test_then_block->test_then_block.condition = parse_expression(parser);  /* read condition */
	test_then_block->test_then_block.scope = NULL;
	checknext(ls, TK_THEN);
	if (ls->t.token == TK_GOTO || ls->t.token == TK_BREAK) {
		test_then_block->test_then_block.scope = new_scope(parser);
		struct ast_node *stmt = parse_goto_statment(parser);  /* handle goto/break */
		add_ast_node(parser->container, &parser->current_scope->statement_list, stmt);
		skip_noop_statements(parser);  /* skip other no-op statements */
		if (block_follow(ls, 0)) {  /* 'goto' is the entire block? */
			end_scope(parser);
			return test_then_block;  /* and that is it */
		}
		else  /* must skip over 'then' part if condition is false */
			;
	}
	else {  /* regular case (not goto/break) */
		test_then_block->test_then_block.scope = new_scope(parser);
	}
	parse_statement_list(parser, &parser->current_scope->statement_list);  /* 'then' part */
	end_scope(parser);
	return test_then_block;
}

/* parse an if control structure - called from statement() */
static struct ast_node *parse_if_statement(struct parser_state *parser, int line) {
	LexState *ls = parser->ls;
	/* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
	int escapelist = NO_JUMP;  /* exit list for finished parts */
	struct ast_node *stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	stmt->type = AST_IF_STMT;
	stmt->if_stmt.if_condition_list = NULL;
	stmt->if_stmt.else_block = NULL;
	struct ast_node *test_then_block = parse_if_cond_then_block(parser, &escapelist);  /* IF cond THEN block */
	add_ast_node(parser->container, &stmt->if_stmt.if_condition_list, test_then_block);
	while (ls->t.token == TK_ELSEIF) {
		test_then_block = parse_if_cond_then_block(parser, &escapelist);  /* ELSEIF cond THEN block */
		add_ast_node(parser->container, &stmt->if_stmt.if_condition_list, test_then_block);
	}
	if (testnext(ls, TK_ELSE))
		stmt->if_stmt.else_block = parse_block(parser);  /* 'else' part */
	check_match(ls, TK_END, TK_IF, line);
	return stmt;
}

/* parse a local function statement - called from statement() */
static struct ast_node *parse_local_function(struct parser_state *parser) {
	LexState *ls = parser->ls;
	struct symbol *symbol = new_local_symbol(parser, check_name_and_next(ls), RAVI_TFUNCTION, NULL);  /* new local variable */
	struct ast_node *function_ast = new_function(parser);
	adjustlocalvars(parser, 1);  /* enter its scope */
	parse_function_body(parser, function_ast, 0, ls->linenumber);  /* function created in next register */
	struct ast_node *stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	stmt->type = AST_LOCAL_STMT;
	stmt->local_stmt.vars = NULL;
	stmt->local_stmt.exprlist = NULL;
	add_symbol(parser->container, &stmt->local_stmt.vars, symbol);
	add_ast_node(parser->container, &stmt->local_stmt.exprlist, function_ast);
	return stmt;
}

/* parse a local variable declaration statement - called from statement() */
static struct ast_node *parse_local_statement(struct parser_state *parser) {
	LexState *ls = parser->ls;
	/* stat -> LOCAL NAME {',' NAME} ['=' explist] */
	int nvars = 0;
	int nexps;
	/* RAVI while declaring locals we need to gather the types
	* so that we can check any assignments later on.
	* TODO we may be able to use register_typeinfo() here
	* instead.
	*/
	enum { N = MAXVARS + 10 };
	int vars[N] = { 0 };
	TString *usertypes[N] = { NULL };
	struct ast_node *node = allocator_allocate(&parser->container->ast_node_allocator, 0);
	node->type = AST_LOCAL_STMT;
	node->local_stmt.vars = NULL;
	node->local_stmt.exprlist = NULL;
	do {
		/* local name : type = value */
		struct symbol *symbol = declare_local_variable(parser, &usertypes[nvars]);
		add_symbol(parser->container, &node->local_stmt.vars, symbol);
		nvars++;
		if (nvars >= MAXVARS)
			luaX_syntaxerror(ls, "too many local variables");
	} while (testnext(ls, ','));
	if (testnext(ls, '='))
		nexps = parse_expression_list(parser, &node->local_stmt.exprlist);
	else {
		nexps = 0;
	}
	adjustlocalvars(parser, nvars);
	return node;
}

/* parse a function name specification - called from funcstat()
* returns boolean value - true if function is a method
*/
static struct ast_node *parse_function_name(struct parser_state *parser) {
	LexState *ls = parser->ls;
	/* funcname -> NAME {fieldsel} [':' NAME] */
	struct ast_node *function_stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	function_stmt->type = AST_FUNCTION_STMT;
	function_stmt->function_stmt.function_expr = NULL;
	function_stmt->function_stmt.methodname = NULL;
	function_stmt->function_stmt.selectors = NULL;
	function_stmt->function_stmt.name = singlevar(parser);
	while (ls->t.token == '.') {
		add_ast_node(parser->container, &function_stmt->function_stmt.selectors, parse_field_selector(parser));
	}
	if (ls->t.token == ':') {
		function_stmt->function_stmt.methodname = parse_field_selector(parser);
	}
	return function_stmt;
}

/* parse a function statement - called from statement() */
static struct ast_node *parse_function_statement(struct parser_state *parser, int line) {
	LexState *ls = parser->ls;
	/* funcstat -> FUNCTION funcname body */
	luaX_next(ls); /* skip FUNCTION */
	struct ast_node *function_stmt = parse_function_name(parser);
	int ismethod = function_stmt->function_stmt.methodname != NULL;
	struct ast_node *function_ast = new_function(parser);
	parse_function_body(parser, function_ast, ismethod, line);
	function_stmt->function_stmt.function_expr = function_ast;
	return function_stmt;
}

/* parse function call with no returns or assignment statement - called from statement() */
static struct ast_node * parse_expr_statement(struct parser_state *parser) {
	struct ast_node *stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	stmt->type = AST_EXPR_STMT;
	stmt->expression_stmt.var_expr_list = NULL;
	stmt->expression_stmt.exr_list = NULL;
	LexState *ls = parser->ls;
	/* stat -> func | assignment */
	/* Until we see '=' we do not know if this is an assignment or expr list*/
	struct ast_node_list *current_list = NULL;
	add_ast_node(parser->container, &current_list, parse_suffixed_expression(parser));
	while (testnext(ls, ',')) {  /* assignment -> ',' suffixedexp assignment */
		add_ast_node(parser->container, &current_list, parse_suffixed_expression(parser));
	}
	if (ls->t.token == '=') { /* stat -> assignment ? */
		checknext(ls, '=');
		stmt->expression_stmt.var_expr_list = current_list;
		current_list = NULL;
		parse_expression_list(parser, &current_list);
	}
	stmt->expression_stmt.exr_list = current_list;
	// TODO Check that if not assignment then it is a function call
	return stmt;
}

/* parse return statement - called from statement() */
static struct ast_node *parse_return_statement(struct parser_state *parser) {
	LexState *ls = parser->ls;
	/* stat -> RETURN [explist] [';'] */
	struct ast_node *return_stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	return_stmt->type = AST_RETURN_STMT;
	return_stmt->return_stmt.exprlist = NULL;
	int first, nret; /* registers with returned values */
	if (block_follow(ls, 1) || ls->t.token == ';')
		first = nret = 0;  /* return no values */
	else {
		nret = parse_expression_list(parser, &return_stmt->return_stmt.exprlist);  /* optional return values */
	}
	testnext(ls, ';');  /* skip optional semicolon */
	return return_stmt;
}

static struct ast_node *parse_do_statement(struct parser_state *parser, int line) {
	luaX_next(parser->ls);  /* skip DO */
	struct ast_node *stmt = allocator_allocate(&parser->container->ast_node_allocator, 0);
	stmt->type = AST_DO_STMT;
	stmt->do_stmt.scope = parse_block(parser);
	check_match(parser->ls, TK_END, TK_DO, line);
	return stmt;
}

/* parse a statement */
static struct ast_node *parse_statement(struct parser_state *parser) {
	LexState *ls = parser->ls;
	int line = ls->linenumber;  /* may be needed for error messages */
	enterlevel(ls);
	struct ast_node *stmt = NULL;
	switch (ls->t.token) {
	case ';': {  /* stat -> ';' (empty statement) */
		luaX_next(ls);  /* skip ';' */
		break;
	}
	case TK_IF: {  /* stat -> ifstat */
		stmt = parse_if_statement(parser, line);
		break;
	}
	case TK_WHILE: {  /* stat -> whilestat */
		stmt = parse_while_statement(parser, line);
		break;
	}
	case TK_DO: {  /* stat -> DO block END */
		stmt = parse_do_statement(parser, line);
		break;
	}
	case TK_FOR: {  /* stat -> forstat */
		stmt = parse_for_statement(parser, line);
		break;
	}
	case TK_REPEAT: {  /* stat -> repeatstat */
		stmt = parse_repeat_statement(parser, line);
		break;
	}
	case TK_FUNCTION: {  /* stat -> funcstat */
		stmt = parse_function_statement(parser, line);
		break;
	}
	case TK_LOCAL: {  /* stat -> localstat */
		luaX_next(ls);  /* skip LOCAL */
		if (testnext(ls, TK_FUNCTION))  /* local function? */
			stmt = parse_local_function(parser);
		else
			stmt = parse_local_statement(parser);
		break;
	}
	case TK_DBCOLON: {  /* stat -> label */
		luaX_next(ls);  /* skip double colon */
		stmt = parse_label_statement(parser, check_name_and_next(ls), line);
		break;
	}
	case TK_RETURN: {  /* stat -> retstat */
		luaX_next(ls);  /* skip RETURN */
		stmt = parse_return_statement(parser);
		break;
	}
	case TK_BREAK:   /* stat -> breakstat */
	case TK_GOTO: {  /* stat -> 'goto' NAME */
		stmt = parse_goto_statment(parser);
		break;
	}
	default: {  /* stat -> func | assignment */
		stmt = parse_expr_statement(parser);
		break;
	}
	}
	leavelevel(ls);
	return stmt;
}

/* Parses a sequence of statements */
/* statlist -> { stat [';'] } */
static void parse_statement_list(struct parser_state *parser, struct ast_node_list **list) {
	LexState *ls = parser->ls;
	while (!block_follow(ls, 1)) {
		if (ls->t.token == TK_RETURN) {
			struct ast_node *stmt = parse_statement(parser);
			add_ast_node(parser->container, list, stmt);
			return;  /* 'return' must be last statement */
		}
		struct ast_node *stmt = parse_statement(parser);
		if (stmt) {
			// This check is temporary - FIXME
			add_ast_node(parser->container, list, stmt);
		}
	}
}

/* Starts a new scope. If the current function has no main block
* defined then the new scoe becomes its main block. The new scope
* gets existing scope as parent even if that belongs to parent
* function.
*/
static struct block_scope *new_scope(struct parser_state *parser) {
	struct ast_container *container = parser->container;
	struct block_scope *scope = allocator_allocate(&container->block_scope_allocator, 0);
	scope->symbol_list = NULL;
	scope->statement_list = NULL;
	scope->function = parser->current_function;
	assert(scope->function && scope->function->type == AST_FUNCTION_EXPR);
	scope->parent = parser->current_scope;
	parser->current_scope = scope;
	if (!parser->current_function->function_expr.main_block)
		parser->current_function->function_expr.main_block = scope;
	return scope;
}

static void end_scope(struct parser_state *parser) {
	assert(parser->current_scope);
	struct block_scope *scope = parser->current_scope;
	parser->current_scope = scope->parent;
	assert(parser->current_scope != NULL || scope == parser->current_function->function_expr.main_block);
}

/* Creates a new function AST node and starts the function scope.
New function becomes child of current function if any, and scope is linked
to previous scope which may be of parent function.
*/
static struct ast_node *new_function(struct parser_state *parser) {
	struct ast_container *container = parser->container;
	struct ast_node *node = allocator_allocate(&container->ast_node_allocator, 0);
	node->type = AST_FUNCTION_EXPR;
	node->function_expr.type = RAVI_TFUNCTION;
	node->function_expr.is_method = false;
	node->function_expr.is_vararg = false;
	node->function_expr.args = NULL;
	node->function_expr.child_functions = NULL;
	node->function_expr.main_block = NULL;
	node->function_expr.parent_function = parser->current_function;
	if (parser->current_function) {
		// Make this function a child of current function
		add_ast_node(parser->container, &parser->current_function->function_expr.child_functions, node);
	}
	parser->current_function = node;
	parser->current_node = node;
	new_scope(parser); /* Start function scope */
	return node;
}

/* Ends the function node and closes the scope for the function. The
* function being closed becomes the current AST node, while parent function/scope
* become current function/scope.
*/
static struct ast_node *end_function(struct parser_state *parser) {
	assert(parser->current_function);
	end_scope(parser);
	struct ast_node *function = parser->current_function;
	parser->current_node = function;
	parser->current_function = function->function_expr.parent_function;
	return function;
}

/* mainfunc() equivalent - parses a Lua script, also known as chunk.
The code is wrapped in a vararg function */
static void parse_chunk(struct parser_state *parser) {
	luaX_next(parser->ls); /* read first token */
	parser->container->main_function = new_function(parser); /* vararg function wrapper */
	parser->container->main_function->function_expr.is_vararg = true;
	parse_statement_list(parser, &parser->current_scope->statement_list);
	end_function(parser);
	check(parser->ls, TK_EOS);
}

static void parser_state_init(struct parser_state *parser, LexState *ls, struct ast_container *container) {
	parser->ls = ls;
	parser->container = container;
	parser->current_function = NULL;
	parser->current_node = NULL;
	parser->current_scope = NULL;
}

static void printf_buf(membuff_t *buf, const char *format, ...) {
	static const char *PADDING = "                                                                                ";
	char tbuf[80] = { 0 };
	va_list ap;
	const char *cp;
	va_start(ap, format);
	for (cp = format; *cp; cp++) {
		if (cp[0] == '%' && cp[1] == 'p') { /* padding */
			int level = va_arg(ap, int);
			snprintf(tbuf, sizeof tbuf, "%.*s", level, PADDING);
			membuff_add_string(buf, tbuf);
			cp++;
		}
		else if (cp[0] == '%' && cp[1] == 't') { /* TString */
			const TString *ts;
			ts = va_arg(ap, const TString *);
			const char *s = getstr(ts);
			membuff_add_string(buf, s);
			cp++;
		}
		else if (cp[0] == '%' && cp[1] == 's') { /* const char * */
			const char *s;
			s = va_arg(ap, const char *);
			membuff_add_string(buf, s);
			cp++;
		}
		else if (cp[0] == '%' && cp[1] == 'c') { /* comment */
			const char *s;
			s = va_arg(ap, const char *);
			membuff_add_fstring(buf, "--[%s]", s);
			cp++;
		}
		else if (cp[0] == '%' && cp[1] == 'i') { /* integer */
			lua_Integer i;
			i = va_arg(ap, lua_Integer);
			membuff_add_longlong(buf, i);
			cp++;
		}
		else if (cp[0] == '%' && cp[1] == 'f') { /* float */
			double d;
			d = va_arg(ap, double);
			membuff_add_fstring(buf, "%.16f", d);
			cp++;
		}
		else if (cp[0] == '%' && cp[1] == 'b') { /* boolean */
			lua_Integer i;
			i = va_arg(ap, lua_Integer);
			membuff_add_bool(buf, i != 0);
			cp++;
		}
		else {
			membuff_add_char(buf, *cp);
		}
	}
	va_end(ap);
}

static void print_ast_node(membuff_t *buf, struct ast_node *node, int level);

static void print_ast_node_list(membuff_t *buf, struct ast_node_list *list, int level, const char *delimiter) {
	struct ast_node *node;
	bool is_first = true;
	FOR_EACH_PTR(list, node) {
		if (is_first)
			is_first = false;
		else if (delimiter)
			printf_buf(buf, "%p%s\n", level, delimiter);
		print_ast_node(buf, node, level + 1);
	} END_FOR_EACH_PTR(node);
}

static void print_block_scope(membuff_t *buf, struct block_scope *scope, int level) {
	print_ast_node_list(buf, scope->statement_list, level + 1, NULL);
}

static void print_symbol(membuff_t *buf, struct symbol *sym, int level) {
	switch (sym->symbol_type) {
	case SYM_GLOBAL: {
		printf_buf(buf, "%p%t %c\n", level, sym->var.var_name, "global symbol");
		break;
	}
	case SYM_LOCAL: {
		printf_buf(buf, "%p%t %c\n", level, sym->var.var_name, "local symbol");
		break;
	}
	case SYM_UPVALUE: {
		printf_buf(buf, "%p%s %c\n", level, sym->upvalue.var->var.var_name, "upvalue");
		break;
	}
	default:
		assert(0);
	}
}

static void print_symbol_list(membuff_t *buf, struct symbol_list *list, int level, const char *delimiter) {
	struct symbol *node;
	bool is_first = true;
	FOR_EACH_PTR(list, node) {
		if (is_first)
			is_first = false;
		else if (delimiter)
			printf_buf(buf, "%p%s\n", level, delimiter);
		print_symbol(buf, node, level + 1);
	} END_FOR_EACH_PTR(node);
}

static const char* get_unary_opr_str(UnOpr op) {
	switch (op) {
	case OPR_NOT: return "not";
	case OPR_MINUS: return "-";
	case OPR_BNOT: return "~";
	case OPR_LEN: return "#";
	case OPR_TO_INTEGER: return "@integer";
	case OPR_TO_NUMBER: return "@number";
	case OPR_TO_INTARRAY: return "@integer[]";
	case OPR_TO_NUMARRAY: return "@number[]";
	case OPR_TO_TABLE: return "@table";
	case OPR_TO_CLOSURE: return "@closure";
	case OPR_TO_STRING: return "@string";
	case OPR_TO_TYPE: return "@<usertype>";
	default: return "";
	}
}

static const char *get_binary_opr_str(BinOpr op) {
	switch (op) {
	case OPR_ADD: return "+";
	case OPR_SUB: return "-";
	case OPR_MUL: return "*";
	case OPR_MOD: return "%";
	case OPR_POW: return "^";
	case OPR_DIV: return "/";
	case OPR_IDIV: return "//";
	case OPR_BAND: return "&";
	case OPR_BOR: return "|";
	case OPR_BXOR: return "~";
	case OPR_SHL: return "<<";
	case OPR_SHR: return ">>";
	case OPR_CONCAT: return "..";
	case OPR_NE: return "~=";
	case OPR_EQ: return "==";
	case OPR_LT: return "<";
	case OPR_LE: return "<=";
	case OPR_GT: return ">";
	case OPR_GE: return ">=";
	case OPR_AND: return "and";
	case OPR_OR: return "or";
	default: return "";
	}
}

static void print_ast_node(membuff_t *buf, struct ast_node *node, int level)
{
	switch (node->type) {
	case AST_FUNCTION_EXPR: {
		if (node->function_expr.args) {
			printf_buf(buf, "%pfunction(\n", level);
			print_symbol_list(buf, node->function_expr.args, level + 1, ",");
			printf_buf(buf, "%p)\n", level);
		}
		else {
			printf_buf(buf, "%pfunction()\n", level);
		}
		print_block_scope(buf, node->function_expr.main_block, level);
		printf_buf(buf, "%pend\n", level);
		break;
	}
	case AST_NONE:
		break;
	case AST_RETURN_STMT: {
		printf_buf(buf, "%preturn\n", level);
		print_ast_node_list(buf, node->return_stmt.exprlist, level + 1, ",");
		break;
	}
	case AST_LOCAL_STMT: {
		printf_buf(buf, "%plocal\n", level);
		printf_buf(buf, "%p%c\n", level, "[symbols]");
		print_symbol_list(buf, node->local_stmt.vars, level + 1, ",");
		if (node->local_stmt.exprlist) {
			printf_buf(buf, "%p%c\n", level, "[expressions]");
			print_ast_node_list(buf, node->local_stmt.exprlist, level + 1, ",");
		}
		break;
	}
	case AST_FUNCTION_STMT: {
		print_ast_node(buf, node->function_stmt.name, level);
		if (node->function_stmt.selectors) {
			printf_buf(buf, "%p%c\n", level + 1, "[selectors]");
			print_ast_node_list(buf, node->function_stmt.selectors, level + 2, NULL);
		}
		if (node->function_stmt.methodname) {
			printf_buf(buf, "%p%c\n", level + 1, "[method name]");
			print_ast_node(buf, node->function_stmt.methodname, level + 2);
		}
		printf_buf(buf, "%p=\n", level + 1);
		print_ast_node(buf, node->function_stmt.function_expr, level + 2);
		break;
	}
	case AST_LABEL_STMT: {
		printf_buf(buf, "%p::%t::\n", level, node->label_stmt.symbol->label.label_name);
		break;
	}
	case AST_GOTO_STMT: {
		printf_buf(buf, "%pgoto %t\n", level, node->goto_stmt.name);
		break;
	}
	case AST_DO_STMT: {
		printf_buf(buf, "%pdo\n", level);
		print_ast_node_list(buf, node->do_stmt.scope->statement_list, level + 1, NULL);
		printf_buf(buf, "%pend\n", level);
		break;
	}
	case AST_BLOCK_STMT: {
		print_ast_node_list(buf, node->do_stmt.scope->statement_list, level, NULL);
		break;
	}
	case AST_EXPR_STMT: {
		printf_buf(buf, "%p%c\n", level, "[expression statement start]");
		if (node->expression_stmt.var_expr_list) {
			printf_buf(buf, "%p%c\n", level+1, "[var list start]");
			print_ast_node_list(buf, node->expression_stmt.var_expr_list, level + 2, ",");
			printf_buf(buf, "%p= %c\n", level+1, "[var list end]");
		}
		printf_buf(buf, "%p%c\n", level + 1, "[expression list start]");
		print_ast_node_list(buf, node->expression_stmt.exr_list, level + 2, ",");
		printf_buf(buf, "%p%c\n", level + 1, "[expression list end]");
		printf_buf(buf, "%p%c\n", level, "[expression statement end]");
		break;
	}
	case AST_IF_STMT: {
		struct ast_node *test_then_block;
		bool is_first = true;
		FOR_EACH_PTR(node->if_stmt.if_condition_list, test_then_block) {
			if (is_first) {
				is_first = false;
				printf_buf(buf, "%pif\n", level);
			}
			else
				printf_buf(buf, "%pelseif\n", level);
			print_ast_node(buf, test_then_block->test_then_block.condition, level + 1);
			printf_buf(buf, "%pthen\n", level);
			print_ast_node_list(buf, test_then_block->test_then_block.scope->statement_list, level + 1, NULL);
		} END_FOR_EACH_PTR(node);
		if (node->if_stmt.else_block) {
			printf_buf(buf, "%pelse\n", level);
			print_ast_node_list(buf, node->if_stmt.else_block->statement_list, level + 1, NULL);
		}
		printf_buf(buf, "%pend\n", level);
		break;
	}
	case AST_WHILE_STMT: {
		printf_buf(buf, "%pwhile\n", level);
		print_ast_node(buf, node->while_or_repeat_stmt.condition, level + 1);
		printf_buf(buf, "%pdo\n", level);
		print_ast_node_list(buf, node->while_or_repeat_stmt.loop_body->statement_list, level + 1, NULL);
		printf_buf(buf, "%pend\n", level);
		break;
	}
	case AST_REPEAT_STMT: {
		printf_buf(buf, "%prepeat\n", level);
		print_ast_node_list(buf, node->while_or_repeat_stmt.loop_body->statement_list, level + 1, NULL);
		printf_buf(buf, "%puntil\n", level);
		print_ast_node(buf, node->while_or_repeat_stmt.condition, level + 1);
		printf_buf(buf, "%p%c\n", level, "[repeat end]");
		break;
	}
	case AST_FORIN_STMT: {
		printf_buf(buf, "%pfor\n", level);
		print_block_scope(buf, node->for_stmt.loop_scope, level + 1);
		printf_buf(buf, "%pend\n", level);
		break;
	}
	case AST_FORNUM_STMT: {
		printf_buf(buf, "%pfor\n", level);
		print_block_scope(buf, node->for_stmt.loop_scope, level + 1);
		printf_buf(buf, "%pend\n", level);
		break;
	}
	case AST_SUFFIXED_EXPR: {
		printf_buf(buf, "%p%c\n", level, "[primary start]");
		print_ast_node(buf, node->suffixed_expr.primary_expr, level + 1);
		printf_buf(buf, "%p%c\n", level, "[primary end]");
		if (node->suffixed_expr.suffix_list) {
			printf_buf(buf, "%p%c\n", level, "[suffix list start]");
			print_ast_node_list(buf, node->suffixed_expr.suffix_list, level + 1, NULL);
			printf_buf(buf, "%p%c\n", level, "[suffix list end]");
		}
		break;
	}
	case AST_FUNCTION_CALL_EXPR: {
		printf_buf(buf, "%p%c\n", level, "[function call start]");
		if (node->function_call_expr.methodname) {
			printf_buf(buf, "%p: %t (\n", level+1, node->function_call_expr.methodname);
		}
		else {
			printf_buf(buf, "%p(\n", level+1);
		}
		print_ast_node_list(buf, node->function_call_expr.arg_list, level + 2, ",");
		printf_buf(buf, "%p)\n", level + 1);
		printf_buf(buf, "%p%c\n", level, "[function call end]");
		break;
	}
	case AST_SYMBOL_EXPR: {
		print_symbol(buf, node->symbol_expr.var, level + 1);
		break;
	}
	case AST_BINARY_EXPR: {
		printf_buf(buf, "%p%c\n", level, "[binary expr start]");
		print_ast_node(buf, node->binary_expr.exprleft, level + 1);
		printf_buf(buf, "%p%s\n", level, get_binary_opr_str(node->binary_expr.binary_op));
		print_ast_node(buf, node->binary_expr.exprright, level + 1);
		printf_buf(buf, "%p%c\n", level, "[binary expr end]");
		break;
	}
	case AST_UNARY_EXPR: {
		printf_buf(buf, "%p%c\n", level, "[unary expr start]");
		printf_buf(buf, "%p%s\n", level, get_unary_opr_str(node->unary_expr.unary_op));
		print_ast_node(buf, node->unary_expr.expr, level + 1);
		printf_buf(buf, "%p%c\n", level, "[unary expr end]");
		break;
	}
	case AST_LITERAL_EXPR: {
		printf_buf(buf, "%p", level);
		switch (node->literal_expr.literal.type) {
		case RAVI_TNIL: printf_buf(buf, "nil"); break;
		case RAVI_TBOOLEAN: printf_buf(buf, "%b", node->literal_expr.literal.u.i); break;
		case RAVI_TNUMINT: printf_buf(buf, "%i", node->literal_expr.literal.u.i); break;
		case RAVI_TNUMFLT: printf_buf(buf, "%f", node->literal_expr.literal.u.n); break;
		case RAVI_TSTRING: printf_buf(buf, "'%t'", node->literal_expr.literal.u.s); break;
		default: assert(0);
		}
		printf_buf(buf, "\n");
		break;
	}
	case AST_FIELD_SELECTOR_EXPR: {
		printf_buf(buf, "%p%c\n", level, "[field selector start]");
		printf_buf(buf, "%p.\n", level + 1);
		print_ast_node(buf, node->index_expr.expr, level + 2);
		printf_buf(buf, "%p%c\n", level, "[field selector end]");
		break;
	}
	case AST_Y_INDEX_EXPR: {
		printf_buf(buf, "%p%c\n", level, "[Y index start]");
		printf_buf(buf, "%p[\n", level + 1);
		print_ast_node(buf, node->index_expr.expr, level + 2);
		printf_buf(buf, "%p]\n", level + 1);
		printf_buf(buf, "%p%c\n", level, "[Y index end]");
		break;
	}
	case AST_INDEXED_ASSIGN_EXPR: {
		printf_buf(buf, "%p%c\n", level, "[indexed assign start]");
		if (node->indexed_assign_expr.index_expr) {
			printf_buf(buf, "%p%c\n", level, "[index start]");
			print_ast_node(buf, node->indexed_assign_expr.index_expr, level + 1);
			printf_buf(buf, "%p%c\n", level, "[index end]");
		}
		printf_buf(buf, "%p%c\n", level, "[value start]");
		print_ast_node(buf, node->indexed_assign_expr.value_expr, level + 1);
		printf_buf(buf, "%p%c\n", level, "[value end]");
		printf_buf(buf, "%p%c\n", level, "[indexed assign end]");
		break;
	}
	case AST_TABLE_EXPR: {
		printf_buf(buf, "%p{ %c\n", level, "[table constructor start]");
		print_ast_node_list(buf, node->table_expr.expr_list, level + 1, ",");
		printf_buf(buf, "%p} %c\n", level, "[table constructor end]");
		break;
	}
	default:
		printf_buf(buf, "%pUnsupported node type %d\n", level, node->type);
		assert(0);
	}
}

/* Converts the AST to a string representation */
static int ast_container_to_string(lua_State *L) {
	struct ast_container *container = check_Ravi_AST(L, 1);
	membuff_t mbuf;
	membuff_init(&mbuf, 1024);
	print_ast_node(&mbuf, container->main_function, 0);
	lua_pushstring(L, mbuf.buf);
	membuff_free(&mbuf);
	return 1;
}

/*
** Parse the given source 'chunk' and build an abstract
** syntax tree; return 0 on success / non-zero return code on
** failure
** On success will push a userdata object containing the abstract
** syntax tree.
** On failure push an error message.
*/
static int parse_to_ast(lua_State *L, ZIO *z, Mbuffer *buff,
	const char *name, int firstchar) {
	struct ast_container *container = new_ast_container(L);
	LexState lexstate;
	lexstate.h = luaH_new(L);         /* create table for scanner */
	sethvalue(L, L->top, lexstate.h);
	setuservalue(L,
		uvalue(L->top - 1), L->top); /* set the table as container's uservalue */
	luaD_inctop(L);
	TString *src = luaS_new(L, name); /* create and anchor TString */
	setsvalue(L, L->top, src);
	luaD_inctop(L);
	lexstate.buff = buff;
	lexstate.dyd = NULL; /* Unlike standard Lua parser / code generator we do not use this */
	luaX_setinput(L, &lexstate, z, src, firstchar);
	struct parser_state parser_state;
	parser_state_init(&parser_state, &lexstate, container);
	lua_lock(L); // Workaround for ZIO (used by lexer) which assumes lua_lock()
	parse_chunk(&parser_state); // FIXME must be protected call
	lua_unlock(L);
	L->top--; /* remove source name */
	L->top--; /* remove scanner table */
	return 0; /* OK */
}

/*
** Execute a protected parser.
*/
struct SParser {  /* data to 'f_parser' */
	ZIO *z;
	Mbuffer buff;  /* dynamic structure used by the scanner */
	Dyndata dyd;  /* dynamic structures used by the parser */
	const char *mode;
	const char *name;
};

static void checkmode(lua_State *L, const char *mode, const char *x) {
	if (mode && strchr(mode, x[0]) == NULL) {
		luaO_pushfstring(L,
			"attempt to load a %s chunk (mode is '%s')", x, mode);
		luaD_throw(L, LUA_ERRSYNTAX);
	}
}

static int ravi_f_parser(lua_State *L, void *ud) {
	LClosure *cl;
	struct SParser *p = cast(struct SParser *, ud);
	lua_lock(L);  // Workaroud for zgetc() which assumes lua_lock()
	int c = zgetc(p->z);  /* read first character */
	lua_unlock(L);
	checkmode(L, p->mode, "text");
	return parse_to_ast(L, p->z, &p->buff, p->name, c);
}

static int protected_ast_builder(lua_State *L, ZIO *z, const char *name,
	const char *mode) {
	struct SParser p;
	int status;
	L->nny++;  /* cannot yield during parsing */
	p.z = z; p.name = name; p.mode = mode;
	p.dyd.actvar.arr = NULL; p.dyd.actvar.size = 0;
	p.dyd.gt.arr = NULL; p.dyd.gt.size = 0;
	p.dyd.label.arr = NULL; p.dyd.label.size = 0;
	luaZ_initbuffer(L, &p.buff);
	status = luaD_pcall(L, ravi_f_parser, &p, savestack(L, L->top), L->errfunc);
	luaZ_freebuffer(L, &p.buff);
	luaM_freearray(L, p.dyd.actvar.arr, p.dyd.actvar.size);
	luaM_freearray(L, p.dyd.gt.arr, p.dyd.gt.size);
	luaM_freearray(L, p.dyd.label.arr, p.dyd.label.size);
	L->nny--;
	return status;
}

/*
**
** Builds an Abstract Syntax Tree (encapsulated in userdata type) from the given
** input buffer. This function returns 0 if all OK
* - On success a userdata object representing the tree,
*   else an error message will be pushed on to the stack
**
** This is part of the new experimental (wip) implementation of new
** parser and code generator
*/
static int build_ast_from_reader(lua_State *L, lua_Reader reader, void *data,
	const char *chunkname, const char *mode) {
	ZIO z;
	int status;
	if (!chunkname) chunkname = "?";
	luaZ_init(L, &z, reader, data);
	status = protected_ast_builder(L, &z, chunkname, mode);
	return status;
}

/*
** reserved slot, above all arguments, to hold a copy of the returned
** string to avoid it being collected while parsed. 'load' has four
** optional arguments (chunk, source name, mode, and environment).
*/
#define RESERVEDSLOT	5

/*
** Reader for generic 'load' function: 'lua_load' uses the
** stack for internal stuff, so the reader cannot change the
** stack top. Instead, it keeps its resulting string in a
** reserved slot inside the stack.
*/
static const char *generic_reader(lua_State *L, void *ud, size_t *size) {
	(void)(ud);  /* not used */
	luaL_checkstack(L, 2, "too many nested functions");
	lua_pushvalue(L, 1);  /* get function */
	lua_call(L, 0, 1);  /* call it */
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);  /* pop result */
		*size = 0;
		return NULL;
	}
	else if (!lua_isstring(L, -1))
		luaL_error(L, "reader function must return a string");
	lua_replace(L, RESERVEDSLOT);  /* save string in reserved slot */
	return lua_tolstring(L, RESERVEDSLOT, size);
}

typedef struct LoadS {
	const char *s;
	size_t size;
} LoadS;


static const char *getS(lua_State *L, void *ud, size_t *size) {
	LoadS *ls = (LoadS *)ud;
	(void)L;  /* not used */
	if (ls->size == 0) return NULL;
	*size = ls->size;
	ls->size = 0;
	return ls->s;
}

/*
* Builds an Abstract Syntax Tree (encapsulated in userdata type) from the given
* input buffer. This function returns 0 if all OK
* - On success a userdata object representing the tree,
*   else an error message will be pushed on to the stack
*/
static int build_ast_from_buffer(lua_State *L, const char *buff, size_t size,
	const char *name, const char *mode) {
	LoadS ls;
	ls.s = buff;
	ls.size = size;
	return build_ast_from_reader(L, getS, &ls, name, mode);
}

static int build_ast(lua_State *L) {
	int status;
	size_t l;
	const char *s = lua_tolstring(L, 1, &l);
	const char *mode = luaL_optstring(L, 3, "bt");
	int env = (!lua_isnone(L, 4) ? 4 : 0);  /* 'env' index or 0 if no 'env' */
	if (s != NULL) {  /* loading a string? */
		const char *chunkname = luaL_optstring(L, 2, s);
		status = build_ast_from_buffer(L, s, l, chunkname, mode);
	}
	else {  /* loading from a reader function */
		const char *chunkname = luaL_optstring(L, 2, "=(load)");
		luaL_checktype(L, 1, LUA_TFUNCTION);
		lua_settop(L, RESERVEDSLOT);  /* create reserved slot */
		status = build_ast_from_reader(L, generic_reader, NULL, chunkname, mode);
	}
	return status == 0 ? 1 : 0;
}


static const luaL_Reg container_methods[] = {
	{ "tostring", ast_container_to_string },
	{ NULL, NULL } };

static const luaL_Reg astlib[] = {
	/* Entrypoint for new AST */
	{ "parse", build_ast },
	{ NULL, NULL } };

LUAMOD_API int raviopen_ast_library(lua_State *L) {
	raviL_newmetatable(L, AST_type, AST_type);
	lua_pushcfunction(L, collect_ast_container);
	lua_setfield(L, -2, "__gc");
	lua_pushvalue(L, -1);           /* push metatable */
	lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
	luaL_setfuncs(L, container_methods, 0);
	lua_pop(L, 1);

	luaL_newlib(L, astlib);
	return 1;
}
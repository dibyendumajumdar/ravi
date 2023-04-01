/*
 * ptrlist.c
 *
 * Pointer ptrlist_t manipulation
 *
 * (C) Copyright Linus Torvalds 2003-2005
 */
/*
 * Copyright (C) 2017-2023 Dibyendu Majumdar
 */

#define PARANOIA 1

#include "ptrlist.h"

#include <stdio.h>
#include <stdlib.h>

/* The ptr list */

/* For testing we change this */
static int N_ = LIST_NODE_NR;

void raviX_ptrlist_split_node(PtrList *head)
{
	int old = head->nr_, nr = old / 2;
	C_MemoryAllocator *alloc = head->allocator_;
	assert(alloc);
	PtrList *newlist = (PtrList *)alloc->calloc(alloc->arena, 1, sizeof(PtrList));
	PtrList *next = head->next_;
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

PtrListIterator raviX_ptrlist_forward_iterator(PtrList *head)
{
	PtrListIterator iter;
	iter.__head = iter.__list = head;
	iter.__nr = -1;
	return iter;
}

// Reverse iterator has to start from previous node not previous entry
// in the given head
PtrListIterator raviX_ptrlist_reverse_iterator(PtrList *head)
{
	PtrListIterator iter;
	iter.__head = iter.__list = head ? head->prev_ : NULL;
	iter.__nr = iter.__head ? iter.__head->nr_ : 0;
	return iter;
}

void *raviX_ptrlist_iter_next(PtrListIterator *self)
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
	} else if (self->__list->next_ != self->__head) {
		self->__list = self->__list->next_;
		self->__nr = 0;
		goto Lretry;
	}
	return NULL;
}

void *raviX_ptrlist_nth_entry(PtrList *list, unsigned int idx)
{
	PtrList *head = list;
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

void *raviX_ptrlist_iter_prev(PtrListIterator *self)
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
	} else if (self->__list->prev_ != self->__head) {
		self->__list = self->__list->prev_;
		self->__nr = self->__list->nr_ - 1;
		goto Lretry;
	}
	return NULL;
}

void raviX_ptrlist_iter_split_current(PtrListIterator *self)
{
	if (self->__list->nr_ == N_) {
		/* full so split */
		raviX_ptrlist_split_node(self->__list);
		if (self->__nr >= self->__list->nr_) {
			self->__nr -= self->__list->nr_;
			self->__list = self->__list->next_;
		}
	}
}

void raviX_ptrlist_iter_insert(PtrListIterator *self, void *newitem)
{
	assert(self->__nr >= 0);
	raviX_ptrlist_iter_split_current(self);
	void **__this = self->__list->list_ + self->__nr;
	void **__last = self->__list->list_ + self->__list->nr_ - 1;
	while (__last >= __this) {
		__last[1] = __last[0];
		__last--;
	}
	*__this = newitem;
	self->__list->nr_++;
}

void raviX_ptrlist_iter_remove(PtrListIterator *self)
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

void raviX_ptrlist_iter_set(PtrListIterator *self, void *ptr)
{
	assert(self->__list && self->__nr >= 0 && self->__nr < self->__list->nr_);
	self->__list->list_[self->__nr] = ptr;
}

void raviX_ptrlist_iter_mark_deleted(PtrListIterator *self)
{
	raviX_ptrlist_iter_set(self, NULL);
	self->__list->rm_++;
}

int raviX_ptrlist_size(const PtrList *self)
{
	int nr = 0;
	if (self) {
		const PtrList *list = self;
		do {
			nr += list->nr_ - list->rm_;
		} while ((list = list->next_) != self);
	}
	return nr;
}

void **raviX_ptrlist_add(PtrList **self, void *ptr, C_MemoryAllocator *ptr_list_allocator)
{
	PtrList *list = *self;
	PtrList *last = NULL;
	void **ret;
	int nr;

	if (!list || (nr = (last = list->prev_)->nr_) >= N_) {
		PtrList *newlist = (PtrList *)ptr_list_allocator->calloc(ptr_list_allocator->arena, 1, sizeof(PtrList));
		newlist->allocator_ = ptr_list_allocator;
		if (!list) {
			newlist->next_ = newlist;
			newlist->prev_ = newlist;
			*self = newlist;
		} else {
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

void *raviX_ptrlist_first(PtrList *list)
{
	if (!list)
		return NULL;
	return list->list_[0];
}

void *raviX_ptrlist_last(PtrList *list)
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
int raviX_ptrlist_linearize(PtrList *head, void **arr, int max)
{
	int nr = 0;
	if (head && max > 0) {
		PtrList *list = head;

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
void raviX_ptrlist_pack(PtrList **self)
{
	PtrList *head = *self;
	if (head) {
		PtrList *entry = head;
		do {
			PtrList *next;
		restart:
			next = entry->next_;
			if (!entry->nr_) {
				PtrList *prev;
				if (next == entry) {
					C_MemoryAllocator *alloc = entry->allocator_;
					alloc->free(alloc->arena, entry);
					*self = NULL;
					return;
				}
				prev = entry->prev_;
				prev->next_ = next;
				next->prev_ = prev;
				C_MemoryAllocator *alloc = entry->allocator_;
				alloc->free(alloc->arena, entry);
				if (entry == head) {
					*self = next;
					head = next;
					entry = next;
					goto restart;
				}
			}
			entry = next;
		} while (entry != head);
	}
}

void raviX_ptrlist_remove_all(PtrList **self)
{
	PtrList *tmp, *list = *self;
	if (!list)
		return;
	list->prev_->next_ = NULL;
	while (list) {
		tmp = list;
		list = list->next_;
		C_MemoryAllocator *alloc = tmp->allocator_;
		alloc->free(alloc->arena, tmp);
	}
	*self = NULL;
}

int raviX_ptrlist_remove(PtrList **self, void *entry, int count)
{
	PtrListIterator iter = raviX_ptrlist_forward_iterator(*self);
	for (void *ptr = raviX_ptrlist_iter_next(&iter); ptr != NULL; ptr = raviX_ptrlist_iter_next(&iter)) {
		if (ptr == entry) {
			raviX_ptrlist_iter_remove(&iter);
			if (!--count)
				goto out;
		}
	}
	assert(count <= 0);
out:
	raviX_ptrlist_pack(self);
	return count;
}

int raviX_ptrlist_replace(PtrList **self, void *old_ptr, void *new_ptr, int count)
{
	PtrListIterator iter = raviX_ptrlist_forward_iterator(*self);
	for (void *ptr = raviX_ptrlist_iter_next(&iter); ptr != NULL; ptr = raviX_ptrlist_iter_next(&iter)) {
		if (ptr == old_ptr) {
			raviX_ptrlist_iter_set(&iter, new_ptr);
			if (!--count)
				goto out;
		}
	}
	assert(count <= 0);
out:
	return count;
}

/* This removes the last entry, but doesn't pack the ptr list */
void *raviX_ptrlist_undo_last(PtrList **self)
{
	PtrList *last, *first = *self;

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

void *raviX_ptrlist_delete_last(PtrList **self)
{
	void *ptr = NULL;
	PtrList *last, *first = *self;

	if (!first)
		return NULL;
	last = first->prev_;
	if (last->nr_)
		ptr = last->list_[--last->nr_];
	if (last->nr_ <= 0) {
		first->prev_ = last->prev_;
		last->prev_->next_ = first;
		if (last == first)
			*self = NULL;
		C_MemoryAllocator *alloc = last->allocator_;
		alloc->free(alloc->arena, last);
	}
	return ptr;
}

void raviX_ptrlist_concat(PtrList *a, PtrList **self)
{
	C_MemoryAllocator *alloc = NULL;
	PtrListIterator iter = raviX_ptrlist_forward_iterator(a);
	if (a)
		alloc = a->allocator_;
	else if (*self)
		alloc = (*self)->allocator_;
	else
		return;
	for (void *ptr = raviX_ptrlist_iter_next(&iter); ptr != NULL; ptr = raviX_ptrlist_iter_next(&iter)) {
		raviX_ptrlist_add(self, ptr, alloc);
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

static void array_sort(void **ptr, int nr, void *userdata, int (*cmp)(void *, const void *, const void *))
{
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

static void verify_sorted(PtrList *l, int n, void *userdata, int (*cmp)(void *, const void *, const void *))
{
	int i = 0;
	const void *a;
	PtrList *head = l;

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

static void flush_to(PtrList *b, void **buffer, int *nbuf)
{
	int nr = b->nr_;
	assert(*nbuf >= nr);
	memcpy(b->list_, buffer, nr * sizeof(void *));
	*nbuf = *nbuf - nr;
	memmove(buffer, buffer + nr, *nbuf * sizeof(void *));
}

static void dump_to(PtrList *b, void **buffer, int nbuf)
{
	assert(nbuf <= b->nr_);
	memcpy(b->list_, buffer, nbuf * sizeof(void *));
}

// Merge two already-sorted sequences of blocks:
//   (b1_1, ..., b1_n)  and  (b2_1, ..., b2_m)
// Since we may be moving blocks around, we return the new head
// of the merged list.
static PtrList *merge_block_seqs(PtrList *b1, int n, PtrList *b2, int m, void *userdata,
					 int (*cmp)(void *, const void *, const void *))
{
	int i1 = 0, i2 = 0;
	void *buffer[2 * LIST_NODE_NR];
	int nbuf = 0;
	PtrList *newhead = b1;

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
	while (b1->nr_ == 0 || cmp(userdata, PTR_ENTRY(b1, b1->nr_ - 1), PTR_ENTRY(b2, 0)) < 0) {
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
		} else {
			// BEEN_THERE('K');
			// Element from b2 is smaller
			buffer[nbuf++] = d2;
			if (++i2 >= b2->nr_) {
				PtrList *l = b2;
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

void raviX_ptrlist_sort(PtrList **plist, void *userdata, int (*cmp)(void *, const void *, const void *))
{
	PtrList *head = *plist, *list = head;
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
		PtrList *block1 = head;

		do {
			PtrList *block2 = block1;
			PtrList *next, *newhead;
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

#if 0

static int int_cmp(void *ud, const void *_a, const void *_b)
{
	(void)ud;
	const int *a = (const int *)_a;
	const int *b = (const int *)_b;
	return *a - *b;
}

#define MIN(_x, _y) ((_x) < (_y) ? (_x) : (_y))

static int test_sort()
{
	int i, *e;
	const int N = 10000;

	srand(N);
	for (i = 0; i < 1000; i++)
		(void)rand();

	Allocator ptrlist_allocator;
	raviX_allocator_init(&ptrlist_allocator, "ptrlist_nodes", sizeof(PtrList), __alignof__(PtrList),
			     CHUNK);
	Allocator int_allocator;
	raviX_allocator_init(&int_allocator, "ints", sizeof(int), __alignof__(int), CHUNK);
	PtrList *int_list = NULL;

	for (i = 0; i < N; i++) {
		e = (int *)raviX_allocator_allocate(&int_allocator, 0);
		*e = rand();
		raviX_ptrlist_add(&int_list, e, &ptrlist_allocator);
	}
	if (raviX_ptrlist_size(int_list) != N)
		return 1;
	raviX_ptrlist_sort(&int_list, NULL, int_cmp);
	// Sort already sorted stuff.
	raviX_ptrlist_sort(&int_list, NULL, int_cmp);

	int *p = NULL;
	PtrListIterator iter = raviX_ptrlist_forward_iterator(int_list);
	int count = 0;
	for (int *k = (int *)raviX_ptrlist_iter_next(&iter); k != NULL; k = (int *)raviX_ptrlist_iter_next(&iter)) {
		if (p != NULL) {
			if (*k < *p)
				return 1;
		}
		p = k;
		count++;
	}
	if (count != N)
		return 1;

	PtrList *l = int_list, *l2;
	l2 = l;
	int expected_count = 0;
	do {
		l2->nr_ = MIN(l2->nr_, rand() % 3);
		for (i = 0; i < l2->nr_; i++) {
			*((int *)(l2->list_[i])) = rand();
			expected_count++;
		}
		l2 = l2->next_;
	} while (l2 != l);
	raviX_ptrlist_sort(&int_list, NULL, int_cmp);

	p = NULL;
	iter = raviX_ptrlist_forward_iterator(int_list);
	count = 0;
	for (int *k = (int *)raviX_ptrlist_iter_next(&iter); k != NULL; k = (int *)raviX_ptrlist_iter_next(&iter)) {
		if (p != NULL) {
			if (*k < *p)
				return 1;
		}
		p = k;
		count++;
	}
	if (count != expected_count)
		return 1;
	raviX_ptrlist_remove_all(&int_list);
	raviX_allocator_destroy(&int_allocator);
	raviX_allocator_destroy(&ptrlist_allocator);
	return 0;
}

struct mystruct {
	int i;
};

struct mytoken {
	const char *a;
};

static int test_ptrlist_basics()
{
	Allocator ptrlist_allocator;
	raviX_allocator_init(&ptrlist_allocator, "ptrlist_nodes", sizeof(PtrList), __alignof__(PtrList),
			     CHUNK);
	Allocator token_allocator;
	raviX_allocator_init(&token_allocator, "ptr_list_tokens", sizeof(struct mytoken), __alignof__(struct mytoken),
			     CHUNK);
	PtrList *token_list = NULL;
	if (raviX_ptrlist_size(token_list) != 0)
		return 1;
	struct mytoken *tok1 = (struct mytoken *)raviX_allocator_allocate(&token_allocator, 0);
	struct mytoken **tok1p = (struct mytoken **)raviX_ptrlist_add(&token_list, tok1, &ptrlist_allocator);
	if (raviX_ptrlist_size(token_list) != 1)
		return 1;
	if (tok1 != *tok1p)
		return 1;
	if (raviX_ptrlist_first(token_list) != tok1)
		return 1;
	if (raviX_ptrlist_last(token_list) != tok1)
		return 1;
	struct mytoken *tok2 = (struct mytoken *)raviX_allocator_allocate(&token_allocator, 0);
	struct mytoken **tok2p = (struct mytoken **)raviX_ptrlist_add(&token_list, tok2, &ptrlist_allocator);
	if (raviX_ptrlist_size(token_list) != 2)
		return 1;
	struct mytoken *tok3 = (struct mytoken *)raviX_allocator_allocate(&token_allocator, 0);
	raviX_ptrlist_add(&token_list, tok3, &ptrlist_allocator);
	if (raviX_ptrlist_size(token_list) != 3)
		return 1;
	struct mytoken *tok4 = (struct mytoken *)raviX_allocator_allocate(&token_allocator, 0);
	raviX_ptrlist_add(&token_list, tok4, &ptrlist_allocator);
	if (raviX_ptrlist_size(token_list) != 4)
		return 1;
	struct mytoken *tok5 = (struct mytoken *)raviX_allocator_allocate(&token_allocator, 0);
	struct mytoken **tok5p = (struct mytoken **)raviX_ptrlist_add(&token_list, tok5, &ptrlist_allocator);
	if (raviX_ptrlist_size(token_list) != 5)
		return 1;

	if (tok2 != *tok2p)
		return 1;
	if (tok5 != *tok5p)
		return 1;
	if (raviX_ptrlist_first(token_list) != tok1)
		return 1;
	if (raviX_ptrlist_last(token_list) != tok5)
		return 1;
	struct mytoken *toks[5];
	int lin1 = raviX_ptrlist_linearize(token_list, (void **)toks, 5);
	if (lin1 != 5)
		return 1;
	if (toks[0] != tok1)
		return 1;
	if (toks[1] != tok2)
		return 1;
	if (toks[2] != tok3)
		return 1;
	if (toks[3] != tok4)
		return 1;
	if (toks[4] != tok5)
		return 1;
	if (raviX_ptrlist_size(token_list) != 5)
		return 1;
	raviX_ptrlist_pack(&token_list);
	if (raviX_ptrlist_size(token_list) != 5)
		return 1;

	if (raviX_ptrlist_first(token_list) != tok1)
		return 1;
	if (raviX_ptrlist_last(token_list) != tok5)
		return 1;

	const int X = 5 + 1;
	const int Y = X - 1;
	const int Z = Y - 1;
	PtrListIterator iter1 = raviX_ptrlist_forward_iterator(token_list);
	for (int i = 0; i < X; i++) {
		struct mytoken *tk = (struct mytoken *)raviX_ptrlist_iter_next(&iter1);
		if (tk == NULL) {
			if (i == Y)
				break;
			return 1;
		}
		if (tk != toks[i])
			return 1;
	}
	PtrListIterator iter2 = raviX_ptrlist_reverse_iterator(token_list);
	for (int i = 0; i < X; i++) {
		struct mytoken *tk = (struct mytoken *)raviX_ptrlist_iter_prev(&iter2);
		if (tk == NULL) {
			if (i == Y)
				break;
			return 1;
		}
		if (tk != toks[Z - i])
			return 1;
	}
	struct mytoken *tok0 = (struct mytoken *)raviX_allocator_allocate(&token_allocator, 0);
	PtrListIterator iter3 = raviX_ptrlist_forward_iterator(token_list);
	if (!raviX_ptrlist_iter_next(&iter3))
		return 1;
	raviX_ptrlist_iter_insert(&iter3, tok0);
	if (raviX_ptrlist_size(token_list) != 6)
		return 1;
	if (raviX_ptrlist_first(token_list) != tok0)
		return 1;
	if (raviX_ptrlist_last(token_list) != tok5)
		return 1;

	Allocator mystruct_allocator;
	raviX_allocator_init(&mystruct_allocator, "mystructs", sizeof(struct mystruct), __alignof__(struct mystruct),
			     CHUNK);
	PtrList *mystruct_list = NULL;

	struct mystruct *s1 = (struct mystruct *)raviX_allocator_allocate(&mystruct_allocator, 0);
	s1->i = 1;
	struct mystruct *s2 = (struct mystruct *)raviX_allocator_allocate(&mystruct_allocator, 0);
	s2->i = 2;
	struct mystruct *s3 = (struct mystruct *)raviX_allocator_allocate(&mystruct_allocator, 0);
	s3->i = 3;
	struct mystruct *s4 = (struct mystruct *)raviX_allocator_allocate(&mystruct_allocator, 0);
	s4->i = 4;
	struct mystruct *s5 = (struct mystruct *)raviX_allocator_allocate(&mystruct_allocator, 0);
	s5->i = 5;
	struct mystruct *s6 = (struct mystruct *)raviX_allocator_allocate(&mystruct_allocator, 0);
	s6->i = 6;

	raviX_ptrlist_add(&mystruct_list, s1, &ptrlist_allocator);
	raviX_ptrlist_add(&mystruct_list, s2, &ptrlist_allocator);
	raviX_ptrlist_add(&mystruct_list, s3, &ptrlist_allocator);
	raviX_ptrlist_add(&mystruct_list, s4, &ptrlist_allocator);
	raviX_ptrlist_add(&mystruct_list, s5, &ptrlist_allocator);
	raviX_ptrlist_add(&mystruct_list, s6, &ptrlist_allocator);

	struct mystruct *serial1_expected[6] = {s1, s2, s3, s4, s5, s6};
	struct mystruct *serial1_got[6];
	raviX_ptrlist_linearize(mystruct_list, (void **)serial1_got, 6);
	for (int i = 0; i < 6; i++) {
		if (serial1_expected[i] != serial1_got[i])
			return 1;
	}

	if (raviX_ptrlist_remove(&mystruct_list, s3, 1) != 0)
		return 1;
	PtrListIterator iter4 = raviX_ptrlist_forward_iterator(mystruct_list);
	for (struct mystruct *p = (struct mystruct *)raviX_ptrlist_iter_next(&iter4); p != NULL;
	     p = (struct mystruct *)raviX_ptrlist_iter_next(&iter4)) {
		if (p->i == 4)
			raviX_ptrlist_iter_remove(&iter4);
	}
	if (raviX_ptrlist_size(mystruct_list) != 4)
		return 1;

	struct mystruct *serial3_expected[4] = {s1, s2, s5, s6};
	struct mystruct *serial3_got[4];
	int reverse_expected[2] = {2, 1};

	int i = 0;
	struct mystruct *p;
	FOR_EACH_PTR(mystruct_list, p)
	{
		if (i == 4)
			return 1;
		serial3_got[i++] = p;
		if (i == 3) {
			struct mystruct *p2;
			int j = 0;
			RECURSE_PTR_REVERSE(p, p2)
			{
				if (j >= 2 || reverse_expected[j] != p2->i)
					return 1;
				j++;
			}
			END_FOR_EACH_PTR_REVERSE(p2);
		}
	}
	END_FOR_EACH_PTR(p);
	if (i != 4)
		return 1;
	for (i = 0; i < 4; i++) {
		if (serial3_expected[i] != serial3_got[i])
			return 1;
	}

	i = 0;
	PREPARE_PTR_LIST(mystruct_list, p);
	while (p != NULL) {
		if (i == 4)
			return 1;
		serial3_got[i++] = p;
		NEXT_PTR_LIST(p);
	}
	FINISH_PTR_LIST(p);
	if (i != 4)
		return 1;
	for (i = 0; i < 4; i++) {
		if (serial3_expected[i] != serial3_got[i])
			return 1;
	}

	i = 0;
	FOR_EACH_PTR_REVERSE(mystruct_list, p)
	{
		if (i == 4)
			return 1;
		serial3_got[i++] = p;
		if (i == 2) {
			struct mystruct *p3;
			int j = 0;
			RECURSE_PTR_REVERSE(p, p3)
			{
				if (j >= 2 || reverse_expected[j] != p3->i)
					return 1;
				j++;
			}
			END_FOR_EACH_PTR_REVERSE(p3);
		}
	}
	END_FOR_EACH_PTR_REVERSE(p);
	if (i != 4)
		return 1;
	for (int i = 0; i < 4; i++) {
		if (serial3_expected[3 - i] != serial3_got[i])
			return 1;
	}

	raviX_ptrlist_remove_all(&token_list);
	raviX_ptrlist_remove_all(&mystruct_list);

	raviX_allocator_destroy(&token_allocator);
	raviX_allocator_destroy(&mystruct_allocator);
	raviX_allocator_destroy(&ptrlist_allocator);
	return 0;
}

int test_ptrlist()
{
	if (test_sort() != 0)
		return 1;

	/* For testing we set N_ temporarily */
	N_ = 2;
	int failure_count = test_ptrlist_basics();
	N_ = LIST_NODE_NR;

	if (failure_count == 0)
		printf("ptrlist test okay\n");
	return failure_count;
}

#endif
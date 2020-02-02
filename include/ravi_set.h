/*
 * Copyright © 2009 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *
 */

#ifndef RAVI_SET_H
#define RAVI_SET_H

#include <inttypes.h>
#include <stdbool.h>

struct set_entry {
	uint32_t hash;
	const void *key;
};

struct set {
	struct set_entry *table;
	uint32_t (*hash_function)(const void *key);
	int (*key_equals_function)(const void *a, const void *b);
	uint32_t size;
	uint32_t rehash;
	uint32_t max_entries;
	uint32_t size_index;
	uint32_t entries;
	uint32_t deleted_entries;
};

struct set *
set_create(uint32_t (*hash_function)(const void *key),
	   int (*key_equals_function)(const void *a,
				      const void *b));
void
set_destroy(struct set *set,
	    void (*delete_function)(struct set_entry *entry));

struct set_entry *
set_add(struct set *set, const void *key);

bool
set_contains(struct set *set, const void *key);

void
set_remove(struct set *set, const void *key);

struct set_entry *
set_search(struct set *set, const void *key);

void
set_remove_entry(struct set *set, struct set_entry *entry);

struct set_entry *
set_next_entry(struct set *set, struct set_entry *entry);

struct set_entry *
set_random_entry(struct set *set,
		 int (*predicate)(struct set_entry *entry));

/**
 * This foreach function is safe against deletion (which just replaces
 * an entry's data with the deleted marker), but not against insertion
 * (which may rehash the table, making entry a dangling pointer).
 */
#define set_foreach(ht, entry)					\
	for (entry = set_next_entry(ht, NULL);			\
	     entry != NULL;					\
	     entry = set_next_entry(ht, entry))

/* Alternate interfaces to reduce repeated calls to hash function. */
struct set_entry *
set_search_pre_hashed(struct set *set, uint32_t hash, const void *key);

struct set_entry *
set_add_pre_hashed(struct set *set, uint32_t hash, const void *key);

#endif

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

#ifndef RAVI_HASH_TABLE_H
#define RAVI_HASH_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

struct hash_entry {
	uint32_t hash;
	const void *key;
	void *data;
};

struct hash_table {
	struct hash_entry *table;
	uint32_t (*hash_function)(const void *key);
	int (*key_equals_function)(const void *a, const void *b);
	uint32_t size;
	uint32_t rehash;
	uint32_t max_entries;
	uint32_t size_index;
	uint32_t entries;
	uint32_t deleted_entries;
};

struct hash_table *
hash_table_create(uint32_t (*hash_function)(const void *key),
		  int (*key_equals_function)(const void *a,
					     const void *b));
void
hash_table_destroy(struct hash_table *ht,
		   void (*delete_function)(struct hash_entry *entry));

struct hash_entry *
hash_table_insert(struct hash_table *ht, const void *key, void *data);

struct hash_entry *
hash_table_search(struct hash_table *ht, const void *key);

void
hash_table_remove(struct hash_table *ht, const void *key);

void
hash_table_remove_entry(struct hash_table *ht, struct hash_entry *entry);

struct hash_entry *
hash_table_next_entry(struct hash_table *ht,
		      struct hash_entry *entry);

struct hash_entry *
hash_table_random_entry(struct hash_table *ht,
			int (*predicate)(struct hash_entry *entry));

/**
 * This foreach function is safe against deletion (which just replaces
 * an entry's data with the deleted marker), but not against insertion
 * (which may rehash the table, making entry a dangling pointer).
 */
#define hash_table_foreach(ht, entry)				\
	for (entry = hash_table_next_entry(ht, NULL);		\
	     entry != NULL;					\
	     entry = hash_table_next_entry(ht, entry))

/* Alternate interfaces to reduce repeated calls to hash function. */
struct hash_entry *
hash_table_search_pre_hashed(struct hash_table *ht,
			     uint32_t hash,
			     const void *key);

struct hash_entry *
hash_table_insert_pre_hashed(struct hash_table *ht,
			     uint32_t hash,
			     const void *key, void *data);


#ifdef __cplusplus
} /* extern C */
#endif

#endif

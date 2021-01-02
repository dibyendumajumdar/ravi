/*
 * Copyright © 2009 Intel Corporation
 * Copyright © 1988-2004 Keith Packard and Bart Massey.
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
 * Except as contained in this notice, the names of the authors
 * or their institutions shall not be used in advertising or
 * otherwise to promote the sale, use or other dealings in this
 * Software without prior written authorization from the
 * authors.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *    Keith Packard <keithp@keithp.com>
 */

#include "set.h"

#include <assert.h>
#include <stdlib.h>


#define ARRAY_SIZE(array) ((int)(sizeof(array) / sizeof(array[0])))

/*
 * From Knuth -- a good choice for hash/rehash values is p, p-2 where
 * p and p-2 are both prime.  These tables are sized to have an extra 10%
 * free to avoid exponential performance degradation as the hash table fills
 */

static const uint32_t deleted_key_value;
static const void *deleted_key = &deleted_key_value;

static const struct {
   uint32_t max_entries, size, rehash;
} hash_sizes[] = {
    { 2,		5,		3	  },
    { 4,		7,		5	  },
    { 8,		13,		11	  },
    { 16,		19,		17	  },
    { 32,		43,		41        },
    { 64,		73,		71        },
    { 128,		151,		149       },
    { 256,		283,		281       },
    { 512,		571,		569       },
    { 1024,		1153,		1151      },
    { 2048,		2269,		2267      },
    { 4096,		4519,		4517      },
    { 8192,		9013,		9011      },
    { 16384,		18043,		18041     },
    { 32768,		36109,		36107     },
    { 65536,		72091,		72089     },
    { 131072,		144409,		144407    },
    { 262144,		288361,		288359    },
    { 524288,		576883,		576881    },
    { 1048576,		1153459,	1153457   },
    { 2097152,		2307163,	2307161   },
    { 4194304,		4613893,	4613891   },
    { 8388608,		9227641,	9227639   },
    { 16777216,		18455029,	18455027  },
    { 33554432,		36911011,	36911009  },
    { 67108864,		73819861,	73819859  },
    { 134217728,	147639589,	147639587 },
    { 268435456,	295279081,	295279079 },
    { 536870912,	590559793,	590559791 },
    { 1073741824,	1181116273,	1181116271},
    { 2147483648ul,	2362232233ul,	2362232231ul}
};

static int
entry_is_free(const SetEntry *entry)
{
	return entry->key == NULL;
}

static int
entry_is_deleted(const SetEntry *entry)
{
	return entry->key == deleted_key;
}

static int
entry_is_present(const SetEntry *entry)
{
	return entry->key != NULL && entry->key != deleted_key;
}

Set *raviX_set_create(uint32_t (*hash_function)(const void *key),
	   int (*key_equals_function)(const void *a,
				   const void *b))
{
	Set *set;

	set = malloc(sizeof(*set));
	if (set == NULL)
		return NULL;

	set->size_index = 0;
	set->size = hash_sizes[set->size_index].size;
	set->rehash = hash_sizes[set->size_index].rehash;
	set->max_entries = hash_sizes[set->size_index].max_entries;
	set->hash_function = hash_function;
	set->key_equals_function = key_equals_function;
	set->table = calloc(set->size, sizeof(*set->table));
	set->entries = 0;
	set->deleted_entries = 0;

	if (set->table == NULL) {
		free(set);
		return NULL;
	}

	return set;
}

/**
 * Frees the given set.
 *
 * If delete_function is passed, it gets called on each entry present before
 * freeing.
 */
void raviX_set_destroy(Set *set, void (*delete_function)(SetEntry *entry))
{
	if (!set)
		return;

	if (delete_function) {
		SetEntry *entry;

		set_foreach(set, entry) {
			delete_function(entry);
		}
	}
	free(set->table);
	free(set);
}

/* Does the set contain an entry with the given key.
 */
bool raviX_set_contains(Set *set, const void *key)
{
	SetEntry *entry;

	entry = raviX_set_search(set, key);

	return entry != NULL;
}

/**
 * Finds a set entry with the given key.
 *
 * Returns NULL if no entry is found.
 */
SetEntry *raviX_set_search(Set *set, const void *key)
{
	uint32_t hash = set->hash_function(key);

	return raviX_set_search_pre_hashed(set, hash, key);
}

/**
 * Finds a set entry with the given key and hash of that key.
 *
 * Returns NULL if no entry is found.
 */
SetEntry *raviX_set_search_pre_hashed(Set *set, uint32_t hash, const void *key)
{
	uint32_t hash_address;

	hash_address = hash % set->size;
	do {
		uint32_t double_hash;

		SetEntry *entry = set->table + hash_address;

		if (entry_is_free(entry)) {
			return NULL;
		} else if (entry_is_present(entry) && entry->hash == hash) {
			if (set->key_equals_function(key, entry->key)) {
				return entry;
			}
		}

		double_hash = 1 + hash % set->rehash;

		hash_address = (hash_address + double_hash) % set->size;
	} while (hash_address != hash % set->size);

	return NULL;
}

static void
set_rehash(Set *set, int new_size_index)
{
	Set old_set;
	SetEntry *table, *entry;

	if (new_size_index >= ARRAY_SIZE(hash_sizes))
		return;

	table = calloc(hash_sizes[new_size_index].size, sizeof(*set->table));
	if (table == NULL)
		return;

	old_set = *set;

	set->table = table;
	set->size_index = new_size_index;
	set->size = hash_sizes[set->size_index].size;
	set->rehash = hash_sizes[set->size_index].rehash;
	set->max_entries = hash_sizes[set->size_index].max_entries;
	set->entries = 0;
	set->deleted_entries = 0;

	set_foreach(&old_set, entry) { raviX_set_add_pre_hashed(set, entry->hash, entry->key);
	}

	free(old_set.table);
}

/**
 * Inserts the key into the set.
 *
 * Note that insertion may rearrange the set on a resize or rehash, so
 * previously found set_entry pointers are no longer valid after this
 * function.
 */
SetEntry *raviX_set_add(Set *set, const void *key)
{
	uint32_t hash = set->hash_function(key);

	/* Make sure nobody tries to add one of the magic values as a
	 * key. If you need to do so, either do so in a wrapper, or
	 * store keys with the magic values separately in the struct
	 * set.
	 */
	assert(key != NULL);

	return raviX_set_add_pre_hashed(set, hash, key);
}

/**
 * Inserts the key with the given hash into the set.
 *
 * Note that insertion may rearrange the set on a resize or rehash, so
 * previously found set_entry pointers are no longer valid after this
 * function.
 */
SetEntry *raviX_set_add_pre_hashed(Set *set, uint32_t hash, const void *key)
{
	uint32_t hash_address;
	SetEntry *available_entry = NULL;

	if (set->entries >= set->max_entries) {
		set_rehash(set, set->size_index + 1);
	} else if (set->deleted_entries + set->entries >= set->max_entries) {
		set_rehash(set, set->size_index);
	}

	hash_address = hash % set->size;
	do {
		SetEntry *entry = set->table + hash_address;
		uint32_t double_hash;

		if (!entry_is_present(entry)) {
			/* Stash the first available entry we find */
			if (available_entry == NULL)
				available_entry = entry;
			if (entry_is_free(entry))
				break;
		}

		/* Implement replacement when another insert happens
		 * with a matching key.  This is a relatively common
		 * feature of hash tables, with the alternative
		 * generally being "insert the new value as well, and
		 * return it first when the key is searched for".
		 *
		 * Note that the set doesn't have a delete callback.
		 * If freeing of old keys is required to avoid memory leaks,
		 * perform a search before inserting.
		 */
		if (!entry_is_deleted(entry) &&
		    entry->hash == hash &&
		    set->key_equals_function(key, entry->key)) {
			entry->key = key;
			return entry;
		}

		double_hash = 1 + hash % set->rehash;

		hash_address = (hash_address + double_hash) % set->size;
	} while (hash_address != hash % set->size);

	if (available_entry) {
		if (entry_is_deleted(available_entry))
			set->deleted_entries--;
		available_entry->hash = hash;
		available_entry->key = key;
		set->entries++;
		return available_entry;
	}

	/* We could hit here if a required resize failed. An unchecked-malloc
	 * application could ignore this result.
	 */
	return NULL;
}

/**
 * This function searches for, and removes an entry from the set.
 *
 * If the caller has previously found a SetEntry pointer,
 * (from calling raviX_set_search or remembering it from raviX_set_add), then
 * raviX_set_remove_entry can be called instead to avoid an extra search.
 */
void raviX_set_remove(Set *set, const void *key)
{
	SetEntry *entry;

	entry = raviX_set_search(set, key);

	raviX_set_remove_entry(set, entry);
}

/**
 * This function deletes the set given set entry.
 *
 * Note that deletion doesn't otherwise modify the set, so an
 * iteration over the set deleting entries is safe.
 */
void raviX_set_remove_entry(Set *set, SetEntry *entry)
{
	if (!entry)
		return;

	entry->key = deleted_key;
	set->entries--;
	set->deleted_entries++;
}

/**
 * This function is an iterator over the set.
 *
 * Pass in NULL for the first entry, as in the start of a for loop.
 * Note that an iteration over the set is O(table_size) not
 * O(entries).
 */
SetEntry *raviX_set_next_entry(Set *set, SetEntry *entry)
{
	if (entry == NULL)
		entry = set->table;
	else
		entry = entry + 1;

	for (; entry != set->table + set->size; entry++) {
		if (entry_is_present(entry)) {
			return entry;
		}
	}

	return NULL;
}

#ifndef _WIN32
SetEntry *raviX_set_random_entry(Set *set,
		 int (*predicate)(SetEntry *entry))
{
	SetEntry *entry;
	uint32_t i = random() % set->size;

	if (set->entries == 0)
		return NULL;

	for (entry = set->table + i; entry != set->table + set->size; entry++) {
		if (entry_is_present(entry) &&
		    (!predicate || predicate(entry))) {
			return entry;
		}
	}

	for (entry = set->table; entry != set->table + i; entry++) {
		if (entry_is_present(entry) &&
		    (!predicate || predicate(entry))) {
			return entry;
		}
	}

	return NULL;
}
#endif

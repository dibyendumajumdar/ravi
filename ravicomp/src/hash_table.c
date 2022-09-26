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


#include "hash_table.h"
#include "allocate.h"

#include <assert.h>
#include <stdlib.h>


#define ARRAY_SIZE(array) ((int)(sizeof(array) / sizeof(array[0])))

/*
 * From Knuth -- a good choice for hash/rehash values is p, p-2 where
 * p and p-2 are both prime.  These tables are sized to have an extra 10%
 * free to avoid exponential performance degradation as the hash table fills
 */

static const uint32_t deleted_key_value = 0;
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
entry_is_free(const HashEntry *entry)
{
	return entry->key == NULL;
}

static int
entry_is_deleted(const HashEntry *entry)
{
	return entry->key == deleted_key;
}

static int
entry_is_present(const HashEntry *entry)
{
	return entry->key != NULL && entry->key != deleted_key;
}

HashTable *
raviX_hash_table_create(uint32_t (*hash_function)(const void *key),
		  int (*key_equals_function)(const void *a,
					     const void *b))
{
	HashTable *ht;

	ht = (HashTable *) raviX_malloc(sizeof(*ht));

	ht->size_index = 0;
	ht->size = hash_sizes[ht->size_index].size;
	ht->rehash = hash_sizes[ht->size_index].rehash;
	ht->max_entries = hash_sizes[ht->size_index].max_entries;
	ht->hash_function = hash_function;
	ht->key_equals_function = key_equals_function;
	ht->table = (HashEntry *) raviX_calloc(ht->size, sizeof(*ht->table));
	ht->entries = 0;
	ht->deleted_entries = 0;

	if (ht->table == NULL) {
		raviX_free(ht);
		return NULL;
	}

	return ht;
}

/**
 * Frees the given hash table.
 *
 * If delete_function is passed, it gets called on each entry present before
 * freeing.
 */
void
raviX_hash_table_destroy(HashTable *ht,
		   void (*delete_function)(HashEntry *entry))
{
	if (!ht)
		return;

	if (delete_function) {
		HashEntry *entry;

		hash_table_foreach(ht, entry) {
			delete_function(entry);
		}
	}
	raviX_free(ht->table);
	raviX_free(ht);
}

/**
 * Finds a hash table entry with the given key.
 *
 * Returns NULL if no entry is found.  Note that the data pointer may be
 * modified by the user.
 */
HashEntry *
raviX_hash_table_search(HashTable *ht, const void *key)
{
	uint32_t hash = ht->hash_function(key);

	return raviX_hash_table_search_pre_hashed(ht, hash, key);
}

/**
 * Finds a hash table entry with the given key and hash of that key.
 *
 * Returns NULL if no entry is found.  Note that the data pointer may be
 * modified by the user.
 */
HashEntry *
raviX_hash_table_search_pre_hashed(HashTable *ht, uint32_t hash,
			     const void *key)
{
	uint32_t start_hash_address = hash % ht->size;
	uint32_t hash_address = start_hash_address;

	do {
		uint32_t double_hash;

		HashEntry *entry = ht->table + hash_address;

		if (entry_is_free(entry)) {
			return NULL;
		} else if (entry_is_present(entry) && entry->hash == hash) {
			if (ht->key_equals_function(key, entry->key)) {
				return entry;
			}
		}

		double_hash = 1 + hash % ht->rehash;

		hash_address = (hash_address + double_hash) % ht->size;
	} while (hash_address != start_hash_address);

	return NULL;
}

static void
hash_table_rehash(HashTable *ht, int new_size_index)
{
	HashTable old_ht;
	HashEntry *table, *entry;

	if (new_size_index >= ARRAY_SIZE(hash_sizes))
		return;

	table = (HashEntry *) raviX_calloc(hash_sizes[new_size_index].size, sizeof(*ht->table));

	old_ht = *ht;

	ht->table = table;
	ht->size_index = new_size_index;
	ht->size = hash_sizes[ht->size_index].size;
	ht->rehash = hash_sizes[ht->size_index].rehash;
	ht->max_entries = hash_sizes[ht->size_index].max_entries;
	ht->entries = 0;
	ht->deleted_entries = 0;

	hash_table_foreach(&old_ht, entry) {
		raviX_hash_table_insert_pre_hashed(ht, entry->hash,
					     entry->key, entry->data);
	}

	raviX_free(old_ht.table);
}

/**
 * Inserts the key into the table.
 *
 * Note that insertion may rearrange the table on a resize or rehash,
 * so previously found hash_entries are no longer valid after this function.
 */
HashEntry *
raviX_hash_table_insert(HashTable *ht, const void *key, void *data)
{
	uint32_t hash = ht->hash_function(key);

	/* Make sure nobody tries to add one of the magic values as a
	 * key. If you need to do so, either do so in a wrapper, or
	 * store keys with the magic values separately in the struct
	 * hash_table.
	 */
	assert(key != NULL);

	return raviX_hash_table_insert_pre_hashed(ht, hash, key, data);
}

/**
 * Inserts the key with the given hash into the table.
 *
 * Note that insertion may rearrange the table on a resize or rehash,
 * so previously found hash_entries are no longer valid after this function.
 */
HashEntry *
raviX_hash_table_insert_pre_hashed(HashTable *ht, uint32_t hash,
			     const void *key, void *data)
{
	uint32_t start_hash_address, hash_address;
	HashEntry *available_entry = NULL;

	if (ht->entries >= ht->max_entries) {
		hash_table_rehash(ht, ht->size_index + 1);
	} else if (ht->deleted_entries + ht->entries >= ht->max_entries) {
		hash_table_rehash(ht, ht->size_index);
	}

	start_hash_address = hash % ht->size;
	hash_address = start_hash_address;
	do {
		HashEntry *entry = ht->table + hash_address;
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
		 * Note that the hash table doesn't have a delete
		 * callback.  If freeing of old data pointers is
		 * required to avoid memory leaks, perform a search
		 * before inserting.
		 */
		if (!entry_is_deleted(entry) &&
		    entry->hash == hash &&
		    ht->key_equals_function(key, entry->key)) {
			entry->key = key;
			entry->data = data;
			return entry;
		}


		double_hash = 1 + hash % ht->rehash;

		hash_address = (hash_address + double_hash) % ht->size;
	} while (hash_address != start_hash_address);

	if (available_entry) {
		if (entry_is_deleted(available_entry))
			ht->deleted_entries--;
		available_entry->hash = hash;
		available_entry->key = key;
		available_entry->data = data;
		ht->entries++;
		return available_entry;
	}

	/* We could hit here if a required resize failed. An unchecked-malloc
	 * application could ignore this result.
	 */
	return NULL;
}

/**
 * This function searches for, and removes an entry from the hash table.
 *
 * If the caller has previously found a HashEntry pointer,
 * (from calling hash_table_search or remembering it from
 * hash_table_insert), then hash_table_remove_entry can be called
 * instead to avoid an extra search.
 */
void
raviX_hash_table_remove(HashTable *ht, const void *key)
{
	HashEntry *entry;

	entry = raviX_hash_table_search(ht, key);

	raviX_hash_table_remove_entry(ht, entry);
}

/**
 * This function deletes the given hash table entry.
 *
 * Note that deletion doesn't otherwise modify the table, so an iteration over
 * the table deleting entries is safe.
 */
void
raviX_hash_table_remove_entry(HashTable *ht, HashEntry *entry)
{
	if (!entry)
		return;

	entry->key = deleted_key;
	ht->entries--;
	ht->deleted_entries++;
}

/**
 * This function is an iterator over the hash table.
 *
 * Pass in NULL for the first entry, as in the start of a for loop.  Note that
 * an iteration over the table is O(table_size) not O(entries).
 */
HashEntry *
raviX_hash_table_next_entry(HashTable *ht, HashEntry *entry)
{
	if (entry == NULL)
		entry = ht->table;
	else
		entry = entry + 1;

	for (; entry != ht->table + ht->size; entry++) {
		if (entry_is_present(entry)) {
			return entry;
		}
	}

	return NULL;
}

#if 0
#ifndef _WIN32
/**
 * Returns a random entry from the hash table.
 *
 * This may be useful in implementing random replacement (as opposed
 * to just removing everything) in caches based on this hash table
 * implementation.  @predicate may be used to filter entries, or may
 * be set to NULL for no filtering.
 */
HashEntry *
hash_table_random_entry(HashTable *ht,
			int (*predicate)(HashEntry *entry))
{
	HashEntry *entry;
	uint32_t i = random() % ht->size;

	if (ht->entries == 0)
		return NULL;

	for (entry = ht->table + i; entry != ht->table + ht->size; entry++) {
		if (entry_is_present(entry) &&
		    (!predicate || predicate(entry))) {
			return entry;
		}
	}

	for (entry = ht->table; entry != ht->table + i; entry++) {
		if (entry_is_present(entry) &&
		    (!predicate || predicate(entry))) {
			return entry;
		}
	}

	return NULL;
}
#endif
#endif

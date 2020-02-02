/*
 * Copyright © 2009,2013 Intel Corporation
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

#ifndef RAVI_INT_SET_H
#define RAVI_INT_SET_H

#include <inttypes.h>
#include <stdbool.h>

struct int_set_entry {
	uint32_t value;
	unsigned int occupied : 1;
	unsigned int deleted  : 1;
};

struct int_set {
	struct int_set_entry *table;
	uint32_t size;
	uint32_t rehash;
	uint32_t max_entries;
	uint32_t size_index;
	uint32_t entries;
	uint32_t deleted_entries;
};

struct int_set *
int_set_create(void);

void
int_set_destroy(struct int_set *set);

struct int_set_entry *
int_set_add(struct int_set *set, uint32_t value);

bool
int_set_contains(struct int_set *set, uint32_t value);

void
int_set_remove(struct int_set *set, uint32_t value);

struct int_set_entry *
int_set_search(struct int_set *set, uint32_t value);

void
int_set_remove_entry(struct int_set *set, struct int_set_entry *entry);

struct int_set_entry *
int_set_next_entry(struct int_set *set, struct int_set_entry *entry);

/* Return a random entry in the set that satisfies predicate.
 *
 * The 'predicate' function pointer may be NULL in which any random
 * entry will be returned. */
struct int_set_entry *
int_set_random_entry(struct int_set *set,
		     int (*predicate)(struct int_set_entry *entry));

#endif

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
 */

/* Quick FNV-1a hash implementation based on:
 * http://www.isthe.com/chongo/tech/comp/fnv/
 *
 * FNV-1a may not be the best hash out there -- Jenkins's lookup3 is supposed
 * to be quite good, and it may beat FNV.  But FNV has the advantage that it
 * involves almost no code.
 */
#include <fnv_hash.h>

#include <string.h>

uint32_t
fnv1_hash_string(const char *key)
{
	uint32_t hash = 2166136261ul;
	const uint8_t *bytes = (uint8_t *)key;

	while (*bytes != 0) {
		hash ^= *bytes;
		hash = hash * 0x01000193;
		bytes++;
	}

	return hash;
}

uint32_t
fnv1_hash_data(const void *data, size_t size)
{
	uint32_t hash = 2166136261ul;
	const uint8_t *bytes = (uint8_t *)data;

	while (size-- != 0) {
		hash ^= *bytes;
		hash = hash * 0x01000193;
		bytes++;
	}

	return hash;
}

int
string_key_equals(const void *a, const void *b)
{
	return strcmp((const char *)a, (const char *)b) == 0;
}

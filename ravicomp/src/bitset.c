/* This file is a part of MIR project.
   Copyright (C) 2018-2020 Vladimir Makarov <vmakarov.gcc@gmail.com>.
*/
/******************************************************************************
 * Copyright (C) 2020-2021 Dibyendu Majumdar
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

/*
 * Adapted for Ravi Compiler project
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#include <allocate.h>
#include <bitset.h>

#if !defined(BITMAP_ENABLE_CHECKING) && !defined(NDEBUG)
#define BITMAP_ENABLE_CHECKING
#endif

#ifndef BITMAP_ENABLE_CHECKING
#define BITMAP_ASSERT(EXPR, OP) ((void) (EXPR))

#else
static inline void mir_bitset_assert_fail (const char *op) {
	fprintf (stderr, "wrong %s for a bitset", op);
	assert (0);
}

#define BITMAP_ASSERT(EXPR, OP) (void) ((EXPR) ? 0 : (mir_bitset_assert_fail (#OP), 0))

#endif

#define BITMAP_WORD_BITS 64

void raviX_bitset_create2(BitSet *bm, size_t init_bits_num) {
	bm->els_num = 0;
	bm->size = (init_bits_num + BITMAP_WORD_BITS - 1) / BITMAP_WORD_BITS;
	bm->varr = calloc(bm->size, sizeof(bitset_el_t));
}

void raviX_bitset_destroy(BitSet * bm)
{
	free(bm->varr);
}

static void bitset_expand (BitSet * bm, size_t nb) {
	size_t new_len = (nb + BITMAP_WORD_BITS - 1) / BITMAP_WORD_BITS;
	if (new_len > bm->els_num) {
		if (new_len > bm->size) {
			bm->varr = raviX_realloc_array(bm->varr, sizeof(bitset_el_t), bm->size, new_len);
			bm->size = new_len;
		}
		bm->els_num = new_len;
	}
}

int raviX_bitset_bit_p(const BitSet * bm, size_t nb) {
	size_t nw, sh, len = bm->els_num;
	bitset_el_t *addr = bm->varr;

	if (nb >= BITMAP_WORD_BITS * len) return 0;
	nw = nb / BITMAP_WORD_BITS;
	sh = nb % BITMAP_WORD_BITS;
	return (addr[nw] >> sh) & 1;
}

/* Set the given bit to 1, and return true if the bit was previously unset, i.e.
 * this set caused bit to change from 0 to 1
 */
int raviX_bitset_set_bit_p(BitSet * bm, size_t bit) {
	size_t nw, sh;
	bitset_el_t *addr;
	int res;

	bitset_expand (bm, bit + 1);
	addr = bm->varr;
	nw = bit / BITMAP_WORD_BITS;
	sh = bit % BITMAP_WORD_BITS;
	res = ((addr[nw] >> sh) & 1) == 0; /* Was this bit previously unset? */
	assert(nw < bm->els_num);
	addr[nw] |= (bitset_el_t) 1 << sh;
	return res;
}

int raviX_bitset_clear_bit_p(BitSet * bm, size_t nb) {
	size_t nw, sh, len = bm->els_num;
	bitset_el_t *addr = bm->varr;
	int res;

	if (nb >= BITMAP_WORD_BITS * len) return 0;
	nw = nb / BITMAP_WORD_BITS;
	sh = nb % BITMAP_WORD_BITS;
	res = (addr[nw] >> sh) & 1;
	addr[nw] &= ~((bitset_el_t) 1 << sh);
	return res;
}

int raviX_bitset_set_or_clear_bit_range_p(BitSet * bm, size_t nb, size_t len, int set_p) {
	size_t nw, lsh, rsh, range_len;
	bitset_el_t mask, *addr;
	int res = 0;

	bitset_expand (bm, nb + len);
	addr = bm->varr;
	while (len > 0) {
		nw = nb / BITMAP_WORD_BITS;
		lsh = nb % BITMAP_WORD_BITS;
		rsh = len >= BITMAP_WORD_BITS - lsh ? 0 : BITMAP_WORD_BITS - (nb + len) % BITMAP_WORD_BITS;
		mask = ((~(bitset_el_t) 0) >> (rsh + lsh)) << lsh;
		if (set_p) {
			res |= (~addr[nw] & mask) != 0;
			addr[nw] |= mask;
		} else {
			res |= (addr[nw] & mask) != 0;
			addr[nw] &= ~mask;
		}
		range_len = BITMAP_WORD_BITS - rsh - lsh;
		len -= range_len;
		nb += range_len;
	}
	return res;
}

void raviX_bitset_copy(BitSet * dst, const BitSet * src) {

	size_t dst_len = dst->els_num;
	size_t src_len = src->els_num;

	if (dst_len >= src_len)
		dst->els_num = src_len;
	else
		bitset_expand (dst, src_len * BITMAP_WORD_BITS);
	memcpy (dst->varr, src->varr,
		src_len * sizeof (bitset_el_t));
}

int raviX_bitset_equal_p(const BitSet * bm1, const BitSet * bm2) {
	const BitSet * temp_bm;
	size_t i, temp_len, bm1_len = bm1->els_num;
	size_t bm2_len = bm2->els_num;
	bitset_el_t *addr1, *addr2;

	if (bm1_len > bm2_len) {
		temp_bm = bm1;
		bm1 = bm2;
		bm2 = temp_bm;
		temp_len = bm1_len;
		bm1_len = bm2_len;
		bm2_len = temp_len;
	}
	addr1 = bm1->varr;
	addr2 = bm2->varr;
	if (memcmp (addr1, addr2, bm1_len * sizeof (bitset_el_t)) != 0) return false;
	for (i = bm1_len; i < bm2_len; i++)
		if (addr2[i] != 0) return false;
	return true;
}

int raviX_bitset_intersect_p(const BitSet * bm1, const BitSet * bm2) {
	size_t i, min_len, bm1_len = bm1->els_num;
	size_t bm2_len = bm2->els_num;
	bitset_el_t *addr1 = bm1->varr;
	bitset_el_t *addr2 = bm2->varr;

	min_len = bm1_len <= bm2_len ? bm1_len : bm2_len;
	for (i = 0; i < min_len; i++)
		if ((addr1[i] & addr2[i]) != 0) return true;
	return false;
}

int raviX_bitset_empty_p(const BitSet * bm) {
	size_t i, len = bm->els_num;
	bitset_el_t *addr = bm->varr;

	for (i = 0; i < len; i++)
		if (addr[i] != 0) return false;
	return true;
}

static bitset_el_t bitset_el_max2 (bitset_el_t el1, bitset_el_t el2) {
	return el1 < el2 ? el2 : el1;
}

static bitset_el_t bitset_el_max3 (bitset_el_t el1, bitset_el_t el2, bitset_el_t el3) {
	if (el1 <= el2) return el2 < el3 ? el3 : el2;
	return el1 < el3 ? el3 : el1;
}

/* Return the number of bits set in BM.  */
size_t raviX_bitset_bit_count(const BitSet * bm) {
	size_t i, len = bm->els_num;
	bitset_el_t el, *addr = bm->varr;
	size_t count = 0;

	for (i = 0; i < len; i++) {
		if ((el = addr[i]) != 0) {
			for (; el != 0; el >>= 1)
				if (el & 1) count++;
		}
	}
	return count;
}

int raviX_bitset_op2(BitSet * dst, const BitSet * src1, const BitSet * src2,
		     bitset_el_t (*op) (bitset_el_t, bitset_el_t)) {
	size_t i, len, bound, src1_len, src2_len;
	bitset_el_t old, *dst_addr, *src1_addr, *src2_addr;
	int change_p = false;

	src1_len = src1->els_num;
	src2_len = src2->els_num;
	len = bitset_el_max2 (src1_len, src2_len);
	bitset_expand (dst, len * BITMAP_WORD_BITS);
	dst_addr = dst->varr;
	src1_addr = src1->varr;
	src2_addr = src2->varr;
	for (bound = i = 0; i < len; i++) {
		old = dst_addr[i];
		if ((dst_addr[i] = op (i >= src1_len ? 0 : src1_addr[i], i >= src2_len ? 0 : src2_addr[i]))
		    != 0)
			bound = i + 1;
		if (old != dst_addr[i]) change_p = true;
	}
	dst->els_num = bound;
	return change_p;
}

int raviX_bitset_op3(BitSet * dst, const BitSet * src1, const BitSet * src2,
			      const BitSet * src3, bitset_el_t (*op) (bitset_el_t, bitset_el_t, bitset_el_t)) {
	size_t i, len, bound, src1_len, src2_len, src3_len;
	bitset_el_t old, *dst_addr, *src1_addr, *src2_addr, *src3_addr;
	int change_p = false;

	src1_len = src1->els_num;
	src2_len = src2->els_num;
	src3_len = src3->els_num;
	len = bitset_el_max3 (src1_len, src2_len, src3_len);
	bitset_expand (dst, len * BITMAP_WORD_BITS);
	dst_addr = dst->varr;
	src1_addr = src1->varr;
	src2_addr = src2->varr;
	src3_addr = src3->varr;
	for (bound = i = 0; i < len; i++) {
		old = dst_addr[i];
		if ((dst_addr[i] = op (i >= src1_len ? 0 : src1_addr[i], i >= src2_len ? 0 : src2_addr[i],
				       i >= src3_len ? 0 : src3_addr[i]))
		    != 0)
			bound = i + 1;
		if (old != dst_addr[i]) change_p = true;
	}
	dst->els_num = bound;
	return change_p;
}

int raviX_bitset_iterator_next(BitSetIterator *iter, size_t *nbit) {
	const size_t el_bits_num = sizeof (bitset_el_t) * CHAR_BIT;
	size_t curr_nel = iter->nbit / el_bits_num, len = iter->bitset->els_num;
	bitset_el_t el, *addr = iter->bitset->varr;

	for (; curr_nel < len; curr_nel++, iter->nbit = curr_nel * el_bits_num)
		if ((el = addr[curr_nel]) != 0)
			for (el >>= iter->nbit % el_bits_num; el != 0; el >>= 1, iter->nbit++)
				if (el & 1) {
					*nbit = iter->nbit++;
					return true;
				}
	return false;
}




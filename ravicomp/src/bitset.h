/* This file is a part of MIR project.
   Copyright (C) 2018-2020 Vladimir Makarov <vmakarov.gcc@gmail.com>.
*/
/*
 * Adapted for Ravi Compiler project
 */

#ifndef ravicomp_BITSET_H
#define ravicomp_BITSET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t bitset_el_t;

typedef struct BitSet {
	size_t els_num;
	size_t size;
	bitset_el_t *varr;
} BitSet;

extern void raviX_bitset_create2(BitSet *, size_t init_bits_num);
static inline void raviX_bitset_create(BitSet *bm)
{
	raviX_bitset_create2(bm, 0);
}
extern void raviX_bitset_destroy(BitSet * bm);
static inline void raviX_bitset_clear(BitSet * bm)
{
	bm->els_num = 0;
}
extern int raviX_bitset_bit_p(const BitSet * bm, size_t nb);
/* Sets a bit ON and returns true if previously bit was not set */
extern int raviX_bitset_set_bit_p(BitSet * bm, size_t bit);
extern int raviX_bitset_clear_bit_p(BitSet * bm, size_t nb);
extern int raviX_bitset_set_or_clear_bit_range_p(BitSet * bm, size_t nb, size_t len, int set_p);
static inline int raviX_bitset_set_bit_range_p(BitSet * bm, size_t nb, size_t len) {
	return raviX_bitset_set_or_clear_bit_range_p(bm, nb, len, true);
}
static inline int raviX_bitset_clear_bit_range_p(BitSet * bm, size_t nb, size_t len) {
	return raviX_bitset_set_or_clear_bit_range_p(bm, nb, len, false);
}
extern void raviX_bitset_copy(BitSet * dst, const BitSet * src);
extern int raviX_bitset_equal_p(const BitSet * bm1, const BitSet * bm2);
extern int raviX_bitset_intersect_p(const BitSet * bm1, const BitSet * bm2);
extern int raviX_bitset_empty_p(const BitSet * bm);
/* Return the number of bits set in BM.  */
extern size_t raviX_bitset_bit_count(const BitSet * bm);
extern int raviX_bitset_op2(BitSet * dst, const BitSet * src1, const BitSet * src2,
			    bitset_el_t (*op) (bitset_el_t, bitset_el_t));
static inline bitset_el_t raviX_bitset_el_and(bitset_el_t el1, bitset_el_t el2) { return el1 & el2; }
static inline int raviX_bitset_and(BitSet * dst, BitSet * src1, BitSet * src2) {
	return raviX_bitset_op2(dst, src1, src2, raviX_bitset_el_and);
}
static inline bitset_el_t raviX_bitset_el_and_compl(bitset_el_t el1, bitset_el_t el2) {
	return el1 & ~el2;
}
static inline int raviX_bitset_and_compl(BitSet * dst, BitSet * src1, BitSet * src2) {
	return raviX_bitset_op2(dst, src1, src2, raviX_bitset_el_and_compl);
}
static inline bitset_el_t raviX_bitset_el_ior(bitset_el_t el1, bitset_el_t el2) { return el1 | el2; }
static inline int raviX_bitset_ior(BitSet * dst, BitSet * src1, BitSet * src2) {
	return raviX_bitset_op2(dst, src1, src2, raviX_bitset_el_ior);
}
int raviX_bitset_op3(BitSet * dst, const BitSet * src1, const BitSet * src2,
		const BitSet * src3, bitset_el_t (*op) (bitset_el_t, bitset_el_t, bitset_el_t));
static inline bitset_el_t raviX_bitset_el_ior_and(bitset_el_t el1, bitset_el_t el2, bitset_el_t el3) {
	return el1 | (el2 & el3);
}
/* DST = SRC1 | (SRC2 & SRC3).  Return true if DST changed.  */
static inline int raviX_bitset_ior_and(BitSet * dst, BitSet * src1, BitSet * src2, BitSet * src3) {
	return raviX_bitset_op3(dst, src1, src2, src3, raviX_bitset_el_ior_and);
}
static inline bitset_el_t raviX_bitset_el_ior_and_compl(bitset_el_t el1, bitset_el_t el2, bitset_el_t el3) {
	return el1 | (el2 & ~el3);
}
/* DST = SRC1 | (SRC2 & ~SRC3).  Return true if DST changed.  */
static inline int raviX_bitset_ior_and_compl(BitSet * dst, BitSet * src1, BitSet * src2, BitSet * src3) {
	return raviX_bitset_op3(dst, src1, src2, src3, raviX_bitset_el_ior_and_compl);
}

typedef struct {
	BitSet * bitset;
	size_t nbit;
} BitSetIterator;
static inline void raviX_bitset_iterator_init(BitSetIterator *iter, BitSet * bitset) {
	iter->bitset = bitset;
	iter->nbit = 0;
}
extern int raviX_bitset_iterator_next(BitSetIterator *iter, size_t *nbit);
#define FOREACH_BITSET_BIT(iter, bitset, nbit) \
  for (raviX_bitset_iterator_init (&iter, bitset); raviX_bitset_iterator_next (&iter, &nbit);)




#ifdef __cplusplus
} /* extern C */
#endif

#endif


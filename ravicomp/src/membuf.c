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

#include "membuf.h"

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allocate.h>

static int d2s_buffered_n(double f, char* result);

void raviX_string_copy(char *buf, const char *src, size_t buflen)
{
	if (buflen == 0)
		return;
	strncpy(buf, src, buflen);
	buf[buflen - 1] = 0;
}

void raviX_buffer_init(TextBuffer *mb, size_t initial_size)
{
	if (initial_size > 0) {
		mb->buf = (char *)raviX_calloc(1, initial_size);
	} else
		mb->buf = NULL;
	mb->pos = 0;
	mb->capacity = initial_size;
}
void raviX_buffer_resize(TextBuffer *mb, size_t new_size)
{
	if (new_size <= mb->capacity)
		return;
	char *newmem = (char *)realloc(mb->buf, new_size);
	if (newmem == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	mb->buf = newmem;
	mb->capacity = new_size;
}
void raviX_buffer_reserve(TextBuffer *mb, size_t n)
{
	if (mb->capacity < mb->pos + n) {
		size_t new_size = (((mb->pos + n) * 3 + 30) / 2) & ~15;
		raviX_buffer_resize(mb, new_size);
		assert(mb->capacity > mb->pos + n);
	}
}
void raviX_buffer_free(TextBuffer *mb) { raviX_free(mb->buf); }
void raviX_buffer_add_bytes(TextBuffer *mb, const char *str, size_t len)
{
	size_t required_size = mb->pos + len + 1; /* extra byte for NULL terminator */
	raviX_buffer_resize(mb, required_size);
	assert(mb->capacity - mb->pos > len);
	raviX_string_copy(&mb->buf[mb->pos], str, mb->capacity - mb->pos);
	mb->pos += len;
}
void raviX_buffer_add_string(TextBuffer *mb, const char *str)
{
	size_t len = strlen(str);
	raviX_buffer_add_bytes(mb, str, len);
}

void raviX_buffer_add_fstring(TextBuffer *mb, const char *fmt, ...)
{
	va_list args;
	int estimated_size = 128;

	for (int i = 0; i < 2; i++) {
		raviX_buffer_reserve(mb, estimated_size); // ensure we have at least estimated_size free space
		va_start(args, fmt);
		int n = vsnprintf(mb->buf + mb->pos, estimated_size, fmt, args);
		va_end(args);
		if (n >= estimated_size) {
			estimated_size = n + 1; // allow for 0 byte
		} else if (n < 0) {
			fprintf(stderr, "Buffer conversion error\n");
			assert(false);
			break;
		} else {
			mb->pos += n;
			break;
		}
	}
}

void raviX_buffer_add_bool(TextBuffer *mb, bool value)
{
	if (value)
		raviX_buffer_add_string(mb, "true");
	else
		raviX_buffer_add_string(mb, "false");
}
void raviX_buffer_add_int(TextBuffer *mb, int value)
{
	char temp[100];
	snprintf(temp, sizeof temp, "%d", value);
	raviX_buffer_add_string(mb, temp);
}
void raviX_buffer_add_longlong(TextBuffer *mb, int64_t value)
{
	char temp[100];
	snprintf(temp, sizeof temp, "%" PRId64 "", value);
	raviX_buffer_add_string(mb, temp);
}
void raviX_buffer_add_char(TextBuffer *mb, char c)
{
	char temp[2] = {c, '\0'};
	raviX_buffer_add_string(mb, temp);
}
void raviX_buffer_add_double(TextBuffer *mb, double d) {
	// Needs 25 bytes actually
	raviX_buffer_reserve(mb, 32);
	int n = d2s_buffered_n(d, &mb->buf[mb->pos]);
	mb->pos += n;
	mb->buf[mb->pos] = 0;
}



//////////////////////////////////////////////////
#define RYU_OPTIMIZE_SIZE

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef RYU_DEBUG
#include <inttypes.h>
#include <stdio.h>
#endif

// Copyright 2018 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.
#ifndef RYU_COMMON_H
#define RYU_COMMON_H

#include <assert.h>
#include <stdint.h>
#include <string.h>

#if defined(_M_IX86) || defined(_M_ARM)
#define RYU_32_BIT_PLATFORM
#endif

// Returns the number of decimal digits in v, which must not contain more than 9 digits.
static inline uint32_t decimalLength9(const uint32_t v) {
	// Function precondition: v is not a 10-digit number.
	// (f2s: 9 digits are sufficient for round-tripping.)
	// (d2fixed: We print 9-digit blocks.)
	assert(v < 1000000000);
	if (v >= 100000000) { return 9; }
	if (v >= 10000000) { return 8; }
	if (v >= 1000000) { return 7; }
	if (v >= 100000) { return 6; }
	if (v >= 10000) { return 5; }
	if (v >= 1000) { return 4; }
	if (v >= 100) { return 3; }
	if (v >= 10) { return 2; }
	return 1;
}

// Returns e == 0 ? 1 : [log_2(5^e)]; requires 0 <= e <= 3528.
static inline int32_t log2pow5(const int32_t e) {
	// This approximation works up to the point that the multiplication overflows at e = 3529.
	// If the multiplication were done in 64 bits, it would fail at 5^4004 which is just greater
	// than 2^9297.
	assert(e >= 0);
	assert(e <= 3528);
	return (int32_t) ((((uint32_t) e) * 1217359) >> 19);
}

// Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
static inline int32_t pow5bits(const int32_t e) {
	// This approximation works up to the point that the multiplication overflows at e = 3529.
	// If the multiplication were done in 64 bits, it would fail at 5^4004 which is just greater
	// than 2^9297.
	assert(e >= 0);
	assert(e <= 3528);
	return (int32_t) (((((uint32_t) e) * 1217359) >> 19) + 1);
}

// Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
static inline int32_t ceil_log2pow5(const int32_t e) {
	return log2pow5(e) + 1;
}

// Returns floor(log_10(2^e)); requires 0 <= e <= 1650.
static inline uint32_t log10Pow2(const int32_t e) {
	// The first value this approximation fails for is 2^1651 which is just greater than 10^297.
	assert(e >= 0);
	assert(e <= 1650);
	return (((uint32_t) e) * 78913) >> 18;
}

// Returns floor(log_10(5^e)); requires 0 <= e <= 2620.
static inline uint32_t log10Pow5(const int32_t e) {
	// The first value this approximation fails for is 5^2621 which is just greater than 10^1832.
	assert(e >= 0);
	assert(e <= 2620);
	return (((uint32_t) e) * 732923) >> 20;
}

static inline int copy_special_str(char * const result, const bool sign, const bool exponent, const bool mantissa) {
	if (mantissa) {
		memcpy(result, "NaN", 3);
		return 3;
	}
	if (sign) {
		result[0] = '-';
	}
	if (exponent) {
		memcpy(result + sign, "Infinity", 8);
		return sign + 8;
	}
	memcpy(result + sign, "0E0", 3);
	return sign + 3;
}

static inline uint32_t float_to_bits(const float f) {
	uint32_t bits = 0;
	memcpy(&bits, &f, sizeof(float));
	return bits;
}

static inline uint64_t double_to_bits(const double d) {
	uint64_t bits = 0;
	memcpy(&bits, &d, sizeof(double));
	return bits;
}

#endif // RYU_COMMON_H

// Copyright 2018 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.
#ifndef RYU_DIGIT_TABLE_H
#define RYU_DIGIT_TABLE_H

// A table of all two-digit numbers. This is used to speed up decimal digit
// generation by copying pairs of digits into the final output.
static const char DIGIT_TABLE[200] = {
    '0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9',
    '1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9',
    '2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9',
    '3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9',
    '4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9',
    '5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9',
    '6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9',
    '7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9',
    '8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9',
    '9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9'
};

#endif // RYU_DIGIT_TABLE_H


// Copyright 2018 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.
#ifndef RYU_D2S_INTRINSICS_H
#define RYU_D2S_INTRINSICS_H

#include <assert.h>
#include <stdint.h>

// Defines RYU_32_BIT_PLATFORM if applicable.
//#include "ryu/common.h"

// ABSL avoids uint128_t on Win32 even if __SIZEOF_INT128__ is defined.
// Let's do the same for now.
#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER) && !defined(RYU_ONLY_64_BIT_OPS)
#define HAS_UINT128
#elif defined(_MSC_VER) && !defined(RYU_ONLY_64_BIT_OPS) && defined(_M_X64)
#define HAS_64_BIT_INTRINSICS
#endif

#if defined(HAS_UINT128)
typedef __uint128_t uint128_t;
#endif

#if defined(HAS_64_BIT_INTRINSICS)

#include <intrin.h>

static inline uint64_t umul128(const uint64_t a, const uint64_t b, uint64_t* const productHi) {
  return _umul128(a, b, productHi);
}

// Returns the lower 64 bits of (hi*2^64 + lo) >> dist, with 0 < dist < 64.
static inline uint64_t shiftright128(const uint64_t lo, const uint64_t hi, const uint32_t dist) {
  // For the __shiftright128 intrinsic, the shift value is always
  // modulo 64.
  // In the current implementation of the double-precision version
  // of Ryu, the shift value is always < 64. (In the case
  // RYU_OPTIMIZE_SIZE == 0, the shift value is in the range [49, 58].
  // Otherwise in the range [2, 59].)
  // However, this function is now also called by s2d, which requires supporting
  // the larger shift range (TODO: what is the actual range?).
  // Check this here in case a future change requires larger shift
  // values. In this case this function needs to be adjusted.
  assert(dist < 64);
  return __shiftright128(lo, hi, (unsigned char) dist);
}

#else // defined(HAS_64_BIT_INTRINSICS)

static inline uint64_t umul128(const uint64_t a, const uint64_t b, uint64_t* const productHi) {
	// The casts here help MSVC to avoid calls to the __allmul library function.
	const uint32_t aLo = (uint32_t)a;
	const uint32_t aHi = (uint32_t)(a >> 32);
	const uint32_t bLo = (uint32_t)b;
	const uint32_t bHi = (uint32_t)(b >> 32);

	const uint64_t b00 = (uint64_t)aLo * bLo;
	const uint64_t b01 = (uint64_t)aLo * bHi;
	const uint64_t b10 = (uint64_t)aHi * bLo;
	const uint64_t b11 = (uint64_t)aHi * bHi;

	const uint32_t b00Lo = (uint32_t)b00;
	const uint32_t b00Hi = (uint32_t)(b00 >> 32);

	const uint64_t mid1 = b10 + b00Hi;
	const uint32_t mid1Lo = (uint32_t)(mid1);
	const uint32_t mid1Hi = (uint32_t)(mid1 >> 32);

	const uint64_t mid2 = b01 + mid1Lo;
	const uint32_t mid2Lo = (uint32_t)(mid2);
	const uint32_t mid2Hi = (uint32_t)(mid2 >> 32);

	const uint64_t pHi = b11 + mid1Hi + mid2Hi;
	const uint64_t pLo = ((uint64_t)mid2Lo << 32) | b00Lo;

	*productHi = pHi;
	return pLo;
}

static inline uint64_t shiftright128(const uint64_t lo, const uint64_t hi, const uint32_t dist) {
	// We don't need to handle the case dist >= 64 here (see above).
	assert(dist < 64);
	assert(dist > 0);
	return (hi << (64 - dist)) | (lo >> dist);
}

#endif // defined(HAS_64_BIT_INTRINSICS)

#if defined(RYU_32_BIT_PLATFORM)

// Returns the high 64 bits of the 128-bit product of a and b.
static inline uint64_t umulh(const uint64_t a, const uint64_t b) {
  // Reuse the umul128 implementation.
  // Optimizers will likely eliminate the instructions used to compute the
  // low part of the product.
  uint64_t hi;
  umul128(a, b, &hi);
  return hi;
}

// On 32-bit platforms, compilers typically generate calls to library
// functions for 64-bit divisions, even if the divisor is a constant.
//
// E.g.:
// https://bugs.llvm.org/show_bug.cgi?id=37932
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=17958
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=37443
//
// The functions here perform division-by-constant using multiplications
// in the same way as 64-bit compilers would do.
//
// NB:
// The multipliers and shift values are the ones generated by clang x64
// for expressions like x/5, x/10, etc.

static inline uint64_t div5(const uint64_t x) {
  return umulh(x, 0xCCCCCCCCCCCCCCCDu) >> 2;
}

static inline uint64_t div10(const uint64_t x) {
  return umulh(x, 0xCCCCCCCCCCCCCCCDu) >> 3;
}

static inline uint64_t div100(const uint64_t x) {
  return umulh(x >> 2, 0x28F5C28F5C28F5C3u) >> 2;
}

static inline uint64_t div1e8(const uint64_t x) {
  return umulh(x, 0xABCC77118461CEFDu) >> 26;
}

static inline uint64_t div1e9(const uint64_t x) {
  return umulh(x >> 9, 0x44B82FA09B5A53u) >> 11;
}

static inline uint32_t mod1e9(const uint64_t x) {
  // Avoid 64-bit math as much as possible.
  // Returning (uint32_t) (x - 1000000000 * div1e9(x)) would
  // perform 32x64-bit multiplication and 64-bit subtraction.
  // x and 1000000000 * div1e9(x) are guaranteed to differ by
  // less than 10^9, so their highest 32 bits must be identical,
  // so we can truncate both sides to uint32_t before subtracting.
  // We can also simplify (uint32_t) (1000000000 * div1e9(x)).
  // We can truncate before multiplying instead of after, as multiplying
  // the highest 32 bits of div1e9(x) can't affect the lowest 32 bits.
  return ((uint32_t) x) - 1000000000 * ((uint32_t) div1e9(x));
}

#else // defined(RYU_32_BIT_PLATFORM)

static inline uint64_t div5(const uint64_t x) {
	return x / 5;
}

static inline uint64_t div10(const uint64_t x) {
	return x / 10;
}

static inline uint64_t div100(const uint64_t x) {
	return x / 100;
}

static inline uint64_t div1e8(const uint64_t x) {
	return x / 100000000;
}

static inline uint64_t div1e9(const uint64_t x) {
	return x / 1000000000;
}

static inline uint32_t mod1e9(const uint64_t x) {
	return (uint32_t) (x - 1000000000 * div1e9(x));
}

#endif // defined(RYU_32_BIT_PLATFORM)

static inline uint32_t pow5Factor(uint64_t value) {
	const uint64_t m_inv_5 = 14757395258967641293u; // 5 * m_inv_5 = 1 (mod 2^64)
	const uint64_t n_div_5 = 3689348814741910323u;  // #{ n | n = 0 (mod 2^64) } = 2^64 / 5
	uint32_t count = 0;
	for (;;) {
		assert(value != 0);
		value *= m_inv_5;
		if (value > n_div_5)
			break;
		++count;
	}
	return count;
}

// Returns true if value is divisible by 5^p.
static inline bool multipleOfPowerOf5(const uint64_t value, const uint32_t p) {
	// I tried a case distinction on p, but there was no performance difference.
	return pow5Factor(value) >= p;
}

// Returns true if value is divisible by 2^p.
static inline bool multipleOfPowerOf2(const uint64_t value, const uint32_t p) {
	assert(value != 0);
	assert(p < 64);
	// __builtin_ctzll doesn't appear to be faster here.
	return (value & ((1ull << p) - 1)) == 0;
}

// We need a 64x128-bit multiplication and a subsequent 128-bit shift.
// Multiplication:
//   The 64-bit factor is variable and passed in, the 128-bit factor comes
//   from a lookup table. We know that the 64-bit factor only has 55
//   significant bits (i.e., the 9 topmost bits are zeros). The 128-bit
//   factor only has 124 significant bits (i.e., the 4 topmost bits are
//   zeros).
// Shift:
//   In principle, the multiplication result requires 55 + 124 = 179 bits to
//   represent. However, we then shift this value to the right by j, which is
//   at least j >= 115, so the result is guaranteed to fit into 179 - 115 = 64
//   bits. This means that we only need the topmost 64 significant bits of
//   the 64x128-bit multiplication.
//
// There are several ways to do this:
// 1. Best case: the compiler exposes a 128-bit type.
//    We perform two 64x64-bit multiplications, add the higher 64 bits of the
//    lower result to the higher result, and shift by j - 64 bits.
//
//    We explicitly cast from 64-bit to 128-bit, so the compiler can tell
//    that these are only 64-bit inputs, and can map these to the best
//    possible sequence of assembly instructions.
//    x64 machines happen to have matching assembly instructions for
//    64x64-bit multiplications and 128-bit shifts.
//
// 2. Second best case: the compiler exposes intrinsics for the x64 assembly
//    instructions mentioned in 1.
//
// 3. We only have 64x64 bit instructions that return the lower 64 bits of
//    the result, i.e., we have to use plain C.
//    Our inputs are less than the full width, so we have three options:
//    a. Ignore this fact and just implement the intrinsics manually.
//    b. Split both into 31-bit pieces, which guarantees no internal overflow,
//       but requires extra work upfront (unless we change the lookup table).
//    c. Split only the first factor into 31-bit pieces, which also guarantees
//       no internal overflow, but requires extra work since the intermediate
//       results are not perfectly aligned.
#if defined(HAS_UINT128)

// Best case: use 128-bit type.
static inline uint64_t mulShift64(const uint64_t m, const uint64_t* const mul, const int32_t j) {
	const uint128_t b0 = ((uint128_t) m) * mul[0];
	const uint128_t b2 = ((uint128_t) m) * mul[1];
	return (uint64_t) (((b0 >> 64) + b2) >> (j - 64));
}

static inline uint64_t mulShiftAll64(const uint64_t m, const uint64_t* const mul, const int32_t j,
				     uint64_t* const vp, uint64_t* const vm, const uint32_t mmShift) {
//  m <<= 2;
//  uint128_t b0 = ((uint128_t) m) * mul[0]; // 0
//  uint128_t b2 = ((uint128_t) m) * mul[1]; // 64
//
//  uint128_t hi = (b0 >> 64) + b2;
//  uint128_t lo = b0 & 0xffffffffffffffffull;
//  uint128_t factor = (((uint128_t) mul[1]) << 64) + mul[0];
//  uint128_t vpLo = lo + (factor << 1);
//  *vp = (uint64_t) ((hi + (vpLo >> 64)) >> (j - 64));
//  uint128_t vmLo = lo - (factor << mmShift);
//  *vm = (uint64_t) ((hi + (vmLo >> 64) - (((uint128_t) 1ull) << 64)) >> (j - 64));
//  return (uint64_t) (hi >> (j - 64));
	*vp = mulShift64(4 * m + 2, mul, j);
	*vm = mulShift64(4 * m - 1 - mmShift, mul, j);
	return mulShift64(4 * m, mul, j);
}

#elif defined(HAS_64_BIT_INTRINSICS)

static inline uint64_t mulShift64(const uint64_t m, const uint64_t* const mul, const int32_t j) {
  // m is maximum 55 bits
  uint64_t high1;                                   // 128
  const uint64_t low1 = umul128(m, mul[1], &high1); // 64
  uint64_t high0;                                   // 64
  umul128(m, mul[0], &high0);                       // 0
  const uint64_t sum = high0 + low1;
  if (sum < high0) {
    ++high1; // overflow into high1
  }
  return shiftright128(sum, high1, j - 64);
}

static inline uint64_t mulShiftAll64(const uint64_t m, const uint64_t* const mul, const int32_t j,
  uint64_t* const vp, uint64_t* const vm, const uint32_t mmShift) {
  *vp = mulShift64(4 * m + 2, mul, j);
  *vm = mulShift64(4 * m - 1 - mmShift, mul, j);
  return mulShift64(4 * m, mul, j);
}

#else // !defined(HAS_UINT128) && !defined(HAS_64_BIT_INTRINSICS)

static inline uint64_t mulShift64(const uint64_t m, const uint64_t* const mul, const int32_t j) {
  // m is maximum 55 bits
  uint64_t high1;                                   // 128
  const uint64_t low1 = umul128(m, mul[1], &high1); // 64
  uint64_t high0;                                   // 64
  umul128(m, mul[0], &high0);                       // 0
  const uint64_t sum = high0 + low1;
  if (sum < high0) {
    ++high1; // overflow into high1
  }
  return shiftright128(sum, high1, j - 64);
}

// This is faster if we don't have a 64x64->128-bit multiplication.
static inline uint64_t mulShiftAll64(uint64_t m, const uint64_t* const mul, const int32_t j,
  uint64_t* const vp, uint64_t* const vm, const uint32_t mmShift) {
  m <<= 1;
  // m is maximum 55 bits
  uint64_t tmp;
  const uint64_t lo = umul128(m, mul[0], &tmp);
  uint64_t hi;
  const uint64_t mid = tmp + umul128(m, mul[1], &hi);
  hi += mid < tmp; // overflow into hi

  const uint64_t lo2 = lo + mul[0];
  const uint64_t mid2 = mid + mul[1] + (lo2 < lo);
  const uint64_t hi2 = hi + (mid2 < mid);
  *vp = shiftright128(mid2, hi2, (uint32_t) (j - 64 - 1));

  if (mmShift == 1) {
    const uint64_t lo3 = lo - mul[0];
    const uint64_t mid3 = mid - mul[1] - (lo3 > lo);
    const uint64_t hi3 = hi - (mid3 > mid);
    *vm = shiftright128(mid3, hi3, (uint32_t) (j - 64 - 1));
  } else {
    const uint64_t lo3 = lo + lo;
    const uint64_t mid3 = mid + mid + (lo3 < lo);
    const uint64_t hi3 = hi + hi + (mid3 < mid);
    const uint64_t lo4 = lo3 - mul[0];
    const uint64_t mid4 = mid3 - mul[1] - (lo4 > lo3);
    const uint64_t hi4 = hi3 - (mid4 > mid3);
    *vm = shiftright128(mid4, hi4, (uint32_t) (j - 64));
  }

  return shiftright128(mid, hi, (uint32_t) (j - 64 - 1));
}

#endif // HAS_64_BIT_INTRINSICS

#endif // RYU_D2S_INTRINSICS_H

// Copyright 2018 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.
#ifndef RYU_D2S_SMALL_TABLE_H
#define RYU_D2S_SMALL_TABLE_H

// Defines HAS_UINT128 and uint128_t if applicable.
//#include "ryu/d2s_intrinsics.h"

// These tables are generated by PrintDoubleLookupTable.
#define DOUBLE_POW5_INV_BITCOUNT 125
#define DOUBLE_POW5_BITCOUNT 125

static const uint64_t DOUBLE_POW5_INV_SPLIT2[15][2] = {
    {                    1u, 2305843009213693952u },
    {  5955668970331000884u, 1784059615882449851u },
    {  8982663654677661702u, 1380349269358112757u },
    {  7286864317269821294u, 2135987035920910082u },
    {  7005857020398200553u, 1652639921975621497u },
    { 17965325103354776697u, 1278668206209430417u },
    {  8928596168509315048u, 1978643211784836272u },
    { 10075671573058298858u, 1530901034580419511u },
    {   597001226353042382u, 1184477304306571148u },
    {  1527430471115325346u, 1832889850782397517u },
    { 12533209867169019542u, 1418129833677084982u },
    {  5577825024675947042u, 2194449627517475473u },
    { 11006974540203867551u, 1697873161311732311u },
    { 10313493231639821582u, 1313665730009899186u },
    { 12701016819766672773u, 2032799256770390445u }
};
static const uint32_t POW5_INV_OFFSETS[19] = {
    0x54544554, 0x04055545, 0x10041000, 0x00400414, 0x40010000, 0x41155555,
    0x00000454, 0x00010044, 0x40000000, 0x44000041, 0x50454450, 0x55550054,
    0x51655554, 0x40004000, 0x01000001, 0x00010500, 0x51515411, 0x05555554,
    0x00000000
};

static const uint64_t DOUBLE_POW5_SPLIT2[13][2] = {
    {                    0u, 1152921504606846976u },
    {                    0u, 1490116119384765625u },
    {  1032610780636961552u, 1925929944387235853u },
    {  7910200175544436838u, 1244603055572228341u },
    { 16941905809032713930u, 1608611746708759036u },
    { 13024893955298202172u, 2079081953128979843u },
    {  6607496772837067824u, 1343575221513417750u },
    { 17332926989895652603u, 1736530273035216783u },
    { 13037379183483547984u, 2244412773384604712u },
    {  1605989338741628675u, 1450417759929778918u },
    {  9630225068416591280u, 1874621017369538693u },
    {   665883850346957067u, 1211445438634777304u },
    { 14931890668723713708u, 1565756531257009982u }
};
static const uint32_t POW5_OFFSETS[21] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x40000000, 0x59695995,
    0x55545555, 0x56555515, 0x41150504, 0x40555410, 0x44555145, 0x44504540,
    0x45555550, 0x40004000, 0x96440440, 0x55565565, 0x54454045, 0x40154151,
    0x55559155, 0x51405555, 0x00000105
};

#define POW5_TABLE_SIZE 26
static const uint64_t DOUBLE_POW5_TABLE[POW5_TABLE_SIZE] = {
    1ull, 5ull, 25ull, 125ull, 625ull, 3125ull, 15625ull, 78125ull, 390625ull,
    1953125ull, 9765625ull, 48828125ull, 244140625ull, 1220703125ull, 6103515625ull,
    30517578125ull, 152587890625ull, 762939453125ull, 3814697265625ull,
    19073486328125ull, 95367431640625ull, 476837158203125ull,
    2384185791015625ull, 11920928955078125ull, 59604644775390625ull,
    298023223876953125ull //, 1490116119384765625ull
};

#if defined(HAS_UINT128)

// Computes 5^i in the form required by Ryu, and stores it in the given pointer.
static inline void double_computePow5(const uint32_t i, uint64_t* const result) {
	const uint32_t base = i / POW5_TABLE_SIZE;
	const uint32_t base2 = base * POW5_TABLE_SIZE;
	const uint32_t offset = i - base2;
	const uint64_t* const mul = DOUBLE_POW5_SPLIT2[base];
	if (offset == 0) {
		result[0] = mul[0];
		result[1] = mul[1];
		return;
	}
	const uint64_t m = DOUBLE_POW5_TABLE[offset];
	const uint128_t b0 = ((uint128_t) m) * mul[0];
	const uint128_t b2 = ((uint128_t) m) * mul[1];
	const uint32_t delta = pow5bits(i) - pow5bits(base2);
	const uint128_t shiftedSum = (b0 >> delta) + (b2 << (64 - delta)) + ((POW5_OFFSETS[i / 16] >> ((i % 16) << 1)) & 3);
	result[0] = (uint64_t) shiftedSum;
	result[1] = (uint64_t) (shiftedSum >> 64);
}

// Computes 5^-i in the form required by Ryu, and stores it in the given pointer.
static inline void double_computeInvPow5(const uint32_t i, uint64_t* const result) {
	const uint32_t base = (i + POW5_TABLE_SIZE - 1) / POW5_TABLE_SIZE;
	const uint32_t base2 = base * POW5_TABLE_SIZE;
	const uint32_t offset = base2 - i;
	const uint64_t* const mul = DOUBLE_POW5_INV_SPLIT2[base]; // 1/5^base2
	if (offset == 0) {
		result[0] = mul[0];
		result[1] = mul[1];
		return;
	}
	const uint64_t m = DOUBLE_POW5_TABLE[offset]; // 5^offset
	const uint128_t b0 = ((uint128_t) m) * (mul[0] - 1);
	const uint128_t b2 = ((uint128_t) m) * mul[1]; // 1/5^base2 * 5^offset = 1/5^(base2-offset) = 1/5^i
	const uint32_t delta = pow5bits(base2) - pow5bits(i);
	const uint128_t shiftedSum =
	    ((b0 >> delta) + (b2 << (64 - delta))) + 1 + ((POW5_INV_OFFSETS[i / 16] >> ((i % 16) << 1)) & 3);
	result[0] = (uint64_t) shiftedSum;
	result[1] = (uint64_t) (shiftedSum >> 64);
}

#else // defined(HAS_UINT128)

// Computes 5^i in the form required by Ryu, and stores it in the given pointer.
static inline void double_computePow5(const uint32_t i, uint64_t* const result) {
  const uint32_t base = i / POW5_TABLE_SIZE;
  const uint32_t base2 = base * POW5_TABLE_SIZE;
  const uint32_t offset = i - base2;
  const uint64_t* const mul = DOUBLE_POW5_SPLIT2[base];
  if (offset == 0) {
    result[0] = mul[0];
    result[1] = mul[1];
    return;
  }
  const uint64_t m = DOUBLE_POW5_TABLE[offset];
  uint64_t high1;
  const uint64_t low1 = umul128(m, mul[1], &high1);
  uint64_t high0;
  const uint64_t low0 = umul128(m, mul[0], &high0);
  const uint64_t sum = high0 + low1;
  if (sum < high0) {
    ++high1; // overflow into high1
  }
  // high1 | sum | low0
  const uint32_t delta = pow5bits(i) - pow5bits(base2);
  result[0] = shiftright128(low0, sum, delta) + ((POW5_OFFSETS[i / 16] >> ((i % 16) << 1)) & 3);
  result[1] = shiftright128(sum, high1, delta);
}

// Computes 5^-i in the form required by Ryu, and stores it in the given pointer.
static inline void double_computeInvPow5(const uint32_t i, uint64_t* const result) {
  const uint32_t base = (i + POW5_TABLE_SIZE - 1) / POW5_TABLE_SIZE;
  const uint32_t base2 = base * POW5_TABLE_SIZE;
  const uint32_t offset = base2 - i;
  const uint64_t* const mul = DOUBLE_POW5_INV_SPLIT2[base]; // 1/5^base2
  if (offset == 0) {
    result[0] = mul[0];
    result[1] = mul[1];
    return;
  }
  const uint64_t m = DOUBLE_POW5_TABLE[offset];
  uint64_t high1;
  const uint64_t low1 = umul128(m, mul[1], &high1);
  uint64_t high0;
  const uint64_t low0 = umul128(m, mul[0] - 1, &high0);
  const uint64_t sum = high0 + low1;
  if (sum < high0) {
    ++high1; // overflow into high1
  }
  // high1 | sum | low0
  const uint32_t delta = pow5bits(base2) - pow5bits(i);
  result[0] = shiftright128(low0, sum, delta) + 1 + ((POW5_INV_OFFSETS[i / 16] >> ((i % 16) << 1)) & 3);
  result[1] = shiftright128(sum, high1, delta);
}

#endif // defined(HAS_UINT128)

#endif // RYU_D2S_SMALL_TABLE_H

#define DOUBLE_MANTISSA_BITS 52
#define DOUBLE_EXPONENT_BITS 11
#define DOUBLE_BIAS 1023

static inline uint32_t decimalLength17(const uint64_t v) {
	// This is slightly faster than a loop.
	// The average output length is 16.38 digits, so we check high-to-low.
	// Function precondition: v is not an 18, 19, or 20-digit number.
	// (17 digits are sufficient for round-tripping.)
	assert(v < 100000000000000000L);
	if (v >= 10000000000000000L) { return 17; }
	if (v >= 1000000000000000L) { return 16; }
	if (v >= 100000000000000L) { return 15; }
	if (v >= 10000000000000L) { return 14; }
	if (v >= 1000000000000L) { return 13; }
	if (v >= 100000000000L) { return 12; }
	if (v >= 10000000000L) { return 11; }
	if (v >= 1000000000L) { return 10; }
	if (v >= 100000000L) { return 9; }
	if (v >= 10000000L) { return 8; }
	if (v >= 1000000L) { return 7; }
	if (v >= 100000L) { return 6; }
	if (v >= 10000L) { return 5; }
	if (v >= 1000L) { return 4; }
	if (v >= 100L) { return 3; }
	if (v >= 10L) { return 2; }
	return 1;
}

// A floating decimal representing m * 10^e.
typedef struct floating_decimal_64 {
	uint64_t mantissa;
	// Decimal exponent's range is -324 to 308
	// inclusive, and can fit in a short if needed.
	int32_t exponent;
} floating_decimal_64;

static inline floating_decimal_64 d2d(const uint64_t ieeeMantissa, const uint32_t ieeeExponent) {
	int32_t e2;
	uint64_t m2;
	if (ieeeExponent == 0) {
		// We subtract 2 so that the bounds computation has 2 additional bits.
		e2 = 1 - DOUBLE_BIAS - DOUBLE_MANTISSA_BITS - 2;
		m2 = ieeeMantissa;
	} else {
		e2 = (int32_t) ieeeExponent - DOUBLE_BIAS - DOUBLE_MANTISSA_BITS - 2;
		m2 = (1ull << DOUBLE_MANTISSA_BITS) | ieeeMantissa;
	}
	const bool even = (m2 & 1) == 0;
	const bool acceptBounds = even;

#ifdef RYU_DEBUG
	printf("-> %" PRIu64 " * 2^%d\n", m2, e2 + 2);
#endif

	// Step 2: Determine the interval of valid decimal representations.
	const uint64_t mv = 4 * m2;
	// Implicit bool -> int conversion. True is 1, false is 0.
	const uint32_t mmShift = ieeeMantissa != 0 || ieeeExponent <= 1;
	// We would compute mp and mm like this:
	// uint64_t mp = 4 * m2 + 2;
	// uint64_t mm = mv - 1 - mmShift;

	// Step 3: Convert to a decimal power base using 128-bit arithmetic.
	uint64_t vr, vp, vm;
	int32_t e10;
	bool vmIsTrailingZeros = false;
	bool vrIsTrailingZeros = false;
	if (e2 >= 0) {
		// I tried special-casing q == 0, but there was no effect on performance.
		// This expression is slightly faster than max(0, log10Pow2(e2) - 1).
		const uint32_t q = log10Pow2(e2) - (e2 > 3);
		e10 = (int32_t) q;
		const int32_t k = DOUBLE_POW5_INV_BITCOUNT + pow5bits((int32_t) q) - 1;
		const int32_t i = -e2 + (int32_t) q + k;
#if defined(RYU_OPTIMIZE_SIZE)
		uint64_t pow5[2];
    double_computeInvPow5(q, pow5);
    vr = mulShiftAll64(m2, pow5, i, &vp, &vm, mmShift);
#else
		vr = mulShiftAll64(m2, DOUBLE_POW5_INV_SPLIT[q], i, &vp, &vm, mmShift);
#endif
#ifdef RYU_DEBUG
		printf("%" PRIu64 " * 2^%d / 10^%u\n", mv, e2, q);
    printf("V+=%" PRIu64 "\nV =%" PRIu64 "\nV-=%" PRIu64 "\n", vp, vr, vm);
#endif
		if (q <= 21) {
			// This should use q <= 22, but I think 21 is also safe. Smaller values
			// may still be safe, but it's more difficult to reason about them.
			// Only one of mp, mv, and mm can be a multiple of 5, if any.
			const uint32_t mvMod5 = ((uint32_t) mv) - 5 * ((uint32_t) div5(mv));
			if (mvMod5 == 0) {
				vrIsTrailingZeros = multipleOfPowerOf5(mv, q);
			} else if (acceptBounds) {
				// Same as min(e2 + (~mm & 1), pow5Factor(mm)) >= q
				// <=> e2 + (~mm & 1) >= q && pow5Factor(mm) >= q
				// <=> true && pow5Factor(mm) >= q, since e2 >= q.
				vmIsTrailingZeros = multipleOfPowerOf5(mv - 1 - mmShift, q);
			} else {
				// Same as min(e2 + 1, pow5Factor(mp)) >= q.
				vp -= multipleOfPowerOf5(mv + 2, q);
			}
		}
	} else {
		// This expression is slightly faster than max(0, log10Pow5(-e2) - 1).
		const uint32_t q = log10Pow5(-e2) - (-e2 > 1);
		e10 = (int32_t) q + e2;
		const int32_t i = -e2 - (int32_t) q;
		const int32_t k = pow5bits(i) - DOUBLE_POW5_BITCOUNT;
		const int32_t j = (int32_t) q - k;
#if defined(RYU_OPTIMIZE_SIZE)
		uint64_t pow5[2];
    double_computePow5(i, pow5);
    vr = mulShiftAll64(m2, pow5, j, &vp, &vm, mmShift);
#else
		vr = mulShiftAll64(m2, DOUBLE_POW5_SPLIT[i], j, &vp, &vm, mmShift);
#endif
#ifdef RYU_DEBUG
		printf("%" PRIu64 " * 5^%d / 10^%u\n", mv, -e2, q);
    printf("%u %d %d %d\n", q, i, k, j);
    printf("V+=%" PRIu64 "\nV =%" PRIu64 "\nV-=%" PRIu64 "\n", vp, vr, vm);
#endif
		if (q <= 1) {
			// {vr,vp,vm} is trailing zeros if {mv,mp,mm} has at least q trailing 0 bits.
			// mv = 4 * m2, so it always has at least two trailing 0 bits.
			vrIsTrailingZeros = true;
			if (acceptBounds) {
				// mm = mv - 1 - mmShift, so it has 1 trailing 0 bit iff mmShift == 1.
				vmIsTrailingZeros = mmShift == 1;
			} else {
				// mp = mv + 2, so it always has at least one trailing 0 bit.
				--vp;
			}
		} else if (q < 63) { // TODO(ulfjack): Use a tighter bound here.
			// We want to know if the full product has at least q trailing zeros.
			// We need to compute min(p2(mv), p5(mv) - e2) >= q
			// <=> p2(mv) >= q && p5(mv) - e2 >= q
			// <=> p2(mv) >= q (because -e2 >= q)
			vrIsTrailingZeros = multipleOfPowerOf2(mv, q);
#ifdef RYU_DEBUG
			printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif
		}
	}
#ifdef RYU_DEBUG
	printf("e10=%d\n", e10);
  printf("V+=%" PRIu64 "\nV =%" PRIu64 "\nV-=%" PRIu64 "\n", vp, vr, vm);
  printf("vm is trailing zeros=%s\n", vmIsTrailingZeros ? "true" : "false");
  printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif

	// Step 4: Find the shortest decimal representation in the interval of valid representations.
	int32_t removed = 0;
	uint8_t lastRemovedDigit = 0;
	uint64_t output;
	// On average, we remove ~2 digits.
	if (vmIsTrailingZeros || vrIsTrailingZeros) {
		// General case, which happens rarely (~0.7%).
		for (;;) {
			const uint64_t vpDiv10 = div10(vp);
			const uint64_t vmDiv10 = div10(vm);
			if (vpDiv10 <= vmDiv10) {
				break;
			}
			const uint32_t vmMod10 = ((uint32_t) vm) - 10 * ((uint32_t) vmDiv10);
			const uint64_t vrDiv10 = div10(vr);
			const uint32_t vrMod10 = ((uint32_t) vr) - 10 * ((uint32_t) vrDiv10);
			vmIsTrailingZeros &= vmMod10 == 0;
			vrIsTrailingZeros &= lastRemovedDigit == 0;
			lastRemovedDigit = (uint8_t) vrMod10;
			vr = vrDiv10;
			vp = vpDiv10;
			vm = vmDiv10;
			++removed;
		}
#ifdef RYU_DEBUG
		printf("V+=%" PRIu64 "\nV =%" PRIu64 "\nV-=%" PRIu64 "\n", vp, vr, vm);
    printf("d-10=%s\n", vmIsTrailingZeros ? "true" : "false");
#endif
		if (vmIsTrailingZeros) {
			for (;;) {
				const uint64_t vmDiv10 = div10(vm);
				const uint32_t vmMod10 = ((uint32_t) vm) - 10 * ((uint32_t) vmDiv10);
				if (vmMod10 != 0) {
					break;
				}
				const uint64_t vpDiv10 = div10(vp);
				const uint64_t vrDiv10 = div10(vr);
				const uint32_t vrMod10 = ((uint32_t) vr) - 10 * ((uint32_t) vrDiv10);
				vrIsTrailingZeros &= lastRemovedDigit == 0;
				lastRemovedDigit = (uint8_t) vrMod10;
				vr = vrDiv10;
				vp = vpDiv10;
				vm = vmDiv10;
				++removed;
			}
		}
#ifdef RYU_DEBUG
		printf("%" PRIu64 " %d\n", vr, lastRemovedDigit);
    printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif
		if (vrIsTrailingZeros && lastRemovedDigit == 5 && vr % 2 == 0) {
			// Round even if the exact number is .....50..0.
			lastRemovedDigit = 4;
		}
		// We need to take vr + 1 if vr is outside bounds or we need to round up.
		output = vr + ((vr == vm && (!acceptBounds || !vmIsTrailingZeros)) || lastRemovedDigit >= 5);
	} else {
		// Specialized for the common case (~99.3%). Percentages below are relative to this.
		bool roundUp = false;
		const uint64_t vpDiv100 = div100(vp);
		const uint64_t vmDiv100 = div100(vm);
		if (vpDiv100 > vmDiv100) { // Optimization: remove two digits at a time (~86.2%).
			const uint64_t vrDiv100 = div100(vr);
			const uint32_t vrMod100 = ((uint32_t) vr) - 100 * ((uint32_t) vrDiv100);
			roundUp = vrMod100 >= 50;
			vr = vrDiv100;
			vp = vpDiv100;
			vm = vmDiv100;
			removed += 2;
		}
		// Loop iterations below (approximately), without optimization above:
		// 0: 0.03%, 1: 13.8%, 2: 70.6%, 3: 14.0%, 4: 1.40%, 5: 0.14%, 6+: 0.02%
		// Loop iterations below (approximately), with optimization above:
		// 0: 70.6%, 1: 27.8%, 2: 1.40%, 3: 0.14%, 4+: 0.02%
		for (;;) {
			const uint64_t vpDiv10 = div10(vp);
			const uint64_t vmDiv10 = div10(vm);
			if (vpDiv10 <= vmDiv10) {
				break;
			}
			const uint64_t vrDiv10 = div10(vr);
			const uint32_t vrMod10 = ((uint32_t) vr) - 10 * ((uint32_t) vrDiv10);
			roundUp = vrMod10 >= 5;
			vr = vrDiv10;
			vp = vpDiv10;
			vm = vmDiv10;
			++removed;
		}
#ifdef RYU_DEBUG
		printf("%" PRIu64 " roundUp=%s\n", vr, roundUp ? "true" : "false");
    printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif
		// We need to take vr + 1 if vr is outside bounds or we need to round up.
		output = vr + (vr == vm || roundUp);
	}
	const int32_t exp = e10 + removed;

#ifdef RYU_DEBUG
	printf("V+=%" PRIu64 "\nV =%" PRIu64 "\nV-=%" PRIu64 "\n", vp, vr, vm);
  printf("O=%" PRIu64 "\n", output);
  printf("EXP=%d\n", exp);
#endif

	floating_decimal_64 fd;
	fd.exponent = exp;
	fd.mantissa = output;
	return fd;
}

static inline int to_chars(const floating_decimal_64 v, const bool sign, char* const result) {
	// Step 5: Print the decimal representation.
	int index = 0;
	if (sign) {
		result[index++] = '-';
	}

	uint64_t output = v.mantissa;
	const uint32_t olength = decimalLength17(output);

#ifdef RYU_DEBUG
	printf("DIGITS=%" PRIu64 "\n", v.mantissa);
  printf("OLEN=%u\n", olength);
  printf("EXP=%u\n", v.exponent + olength);
#endif

	// Print the decimal digits.
	// The following code is equivalent to:
	// for (uint32_t i = 0; i < olength - 1; ++i) {
	//   const uint32_t c = output % 10; output /= 10;
	//   result[index + olength - i] = (char) ('0' + c);
	// }
	// result[index] = '0' + output % 10;

	uint32_t i = 0;
	// We prefer 32-bit operations, even on 64-bit platforms.
	// We have at most 17 digits, and uint32_t can store 9 digits.
	// If output doesn't fit into uint32_t, we cut off 8 digits,
	// so the rest will fit into uint32_t.
	if ((output >> 32) != 0) {
		// Expensive 64-bit division.
		const uint64_t q = div1e8(output);
		uint32_t output2 = ((uint32_t) output) - 100000000 * ((uint32_t) q);
		output = q;

		const uint32_t c = output2 % 10000;
		output2 /= 10000;
		const uint32_t d = output2 % 10000;
		const uint32_t c0 = (c % 100) << 1;
		const uint32_t c1 = (c / 100) << 1;
		const uint32_t d0 = (d % 100) << 1;
		const uint32_t d1 = (d / 100) << 1;
		memcpy(result + index + olength - i - 1, DIGIT_TABLE + c0, 2);
		memcpy(result + index + olength - i - 3, DIGIT_TABLE + c1, 2);
		memcpy(result + index + olength - i - 5, DIGIT_TABLE + d0, 2);
		memcpy(result + index + olength - i - 7, DIGIT_TABLE + d1, 2);
		i += 8;
	}
	uint32_t output2 = (uint32_t) output;
	while (output2 >= 10000) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
		const uint32_t c = output2 - 10000 * (output2 / 10000);
#else
		const uint32_t c = output2 % 10000;
#endif
		output2 /= 10000;
		const uint32_t c0 = (c % 100) << 1;
		const uint32_t c1 = (c / 100) << 1;
		memcpy(result + index + olength - i - 1, DIGIT_TABLE + c0, 2);
		memcpy(result + index + olength - i - 3, DIGIT_TABLE + c1, 2);
		i += 4;
	}
	if (output2 >= 100) {
		const uint32_t c = (output2 % 100) << 1;
		output2 /= 100;
		memcpy(result + index + olength - i - 1, DIGIT_TABLE + c, 2);
		i += 2;
	}
	if (output2 >= 10) {
		const uint32_t c = output2 << 1;
		// We can't use memcpy here: the decimal dot goes between these two digits.
		result[index + olength - i] = DIGIT_TABLE[c + 1];
		result[index] = DIGIT_TABLE[c];
	} else {
		result[index] = (char) ('0' + output2);
	}

	// Print decimal point if needed.
	if (olength > 1) {
		result[index + 1] = '.';
		index += olength + 1;
	} else {
		++index;
	}

	// Print the exponent.
	result[index++] = 'E';
	int32_t exp = v.exponent + (int32_t) olength - 1;
	if (exp < 0) {
		result[index++] = '-';
		exp = -exp;
	}

	if (exp >= 100) {
		const int32_t c = exp % 10;
		memcpy(result + index, DIGIT_TABLE + 2 * (exp / 10), 2);
		result[index + 2] = (char) ('0' + c);
		index += 3;
	} else if (exp >= 10) {
		memcpy(result + index, DIGIT_TABLE + 2 * exp, 2);
		index += 2;
	} else {
		result[index++] = (char) ('0' + exp);
	}

	return index;
}

static inline bool d2d_small_int(const uint64_t ieeeMantissa, const uint32_t ieeeExponent,
				 floating_decimal_64* const v) {
	const uint64_t m2 = (1ull << DOUBLE_MANTISSA_BITS) | ieeeMantissa;
	const int32_t e2 = (int32_t) ieeeExponent - DOUBLE_BIAS - DOUBLE_MANTISSA_BITS;

	if (e2 > 0) {
		// f = m2 * 2^e2 >= 2^53 is an integer.
		// Ignore this case for now.
		return false;
	}

	if (e2 < -52) {
		// f < 1.
		return false;
	}

	// Since 2^52 <= m2 < 2^53 and 0 <= -e2 <= 52: 1 <= f = m2 / 2^-e2 < 2^53.
	// Test if the lower -e2 bits of the significand are 0, i.e. whether the fraction is 0.
	const uint64_t mask = (1ull << -e2) - 1;
	const uint64_t fraction = m2 & mask;
	if (fraction != 0) {
		return false;
	}

	// f is an integer in the range [1, 2^53).
	// Note: mantissa might contain trailing (decimal) 0's.
	// Note: since 2^53 < 10^16, there is no need to adjust decimalLength17().
	v->mantissa = m2 >> -e2;
	v->exponent = 0;
	return true;
}

static int d2s_buffered_n(double f, char* result) {
	// Step 1: Decode the floating-point number, and unify normalized and subnormal cases.
	const uint64_t bits = double_to_bits(f);

#ifdef RYU_DEBUG
	printf("IN=");
  for (int32_t bit = 63; bit >= 0; --bit) {
    printf("%d", (int) ((bits >> bit) & 1));
  }
  printf("\n");
#endif

	// Decode bits into sign, mantissa, and exponent.
	const bool ieeeSign = ((bits >> (DOUBLE_MANTISSA_BITS + DOUBLE_EXPONENT_BITS)) & 1) != 0;
	const uint64_t ieeeMantissa = bits & ((1ull << DOUBLE_MANTISSA_BITS) - 1);
	const uint32_t ieeeExponent = (uint32_t) ((bits >> DOUBLE_MANTISSA_BITS) & ((1u << DOUBLE_EXPONENT_BITS) - 1));
	// Case distinction; exit early for the easy cases.
	if (ieeeExponent == ((1u << DOUBLE_EXPONENT_BITS) - 1u) || (ieeeExponent == 0 && ieeeMantissa == 0)) {
		return copy_special_str(result, ieeeSign, ieeeExponent, ieeeMantissa);
	}

	floating_decimal_64 v;
	const bool isSmallInt = d2d_small_int(ieeeMantissa, ieeeExponent, &v);
	if (isSmallInt) {
		// For small integers in the range [1, 2^53), v.mantissa might contain trailing (decimal) zeros.
		// For scientific notation we need to move these zeros into the exponent.
		// (This is not needed for fixed-point notation, so it might be beneficial to trim
		// trailing zeros in to_chars only if needed - once fixed-point notation output is implemented.)
		for (;;) {
			const uint64_t q = div10(v.mantissa);
			const uint32_t r = ((uint32_t) v.mantissa) - 10 * ((uint32_t) q);
			if (r != 0) {
				break;
			}
			v.mantissa = q;
			++v.exponent;
		}
	} else {
		v = d2d(ieeeMantissa, ieeeExponent);
	}

	return to_chars(v, ieeeSign, result);
}

static void d2s_buffered(double f, char* result) {
	const int index = d2s_buffered_n(f, result);

	// Terminate the string.
	result[index] = '\0';
}

static char* d2s(double f) {
	char* const result = (char*) malloc(25);
	d2s_buffered(f, result);
	return result;
}

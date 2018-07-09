#ifndef DMR_C_TARGET_H
#define DMR_C_TARGET_H
/*
* sparse/target.h
*
* Copyright (C) 2003 Transmeta Corp.
*               2003 Linus Torvalds
*/
/*
* This version is part of the dmr_c project.
* Copyright (C) 2017 Dibyendu Majumdar
*/

#include <lib.h>

struct target_t {
  struct symbol *size_t_ctype;
  struct symbol *ssize_t_ctype;

  /*
  * For "__attribute__((aligned))"
  */
  int max_alignment;

  /*
  * Integer data types
  */
  int bits_in_bool;
  int bits_in_char;
  int bits_in_short;
  int bits_in_int;
  int bits_in_long;
  int bits_in_longlong;
  int bits_in_longlonglong;

  int bits_in_wchar;
  int max_int_alignment;

  /*
  * Floating point data types
  */
  int bits_in_float;
  int bits_in_double;
  int bits_in_longdouble;

  int max_fp_alignment;

  /*
  * Pointer data type
  */
  int bits_in_pointer;
  int pointer_alignment;

  /*
  * Enum data types
  */
  int bits_in_enum;
  int enum_alignment;
};

/*
* Helper functions for converting bits to bytes and vice versa.
*/

static inline int dmrC_bits_to_bytes(const struct target_t *target, int bits) {
  return bits >= 0 ? (bits + target->bits_in_char - 1) / target->bits_in_char : -1;
}

static inline int dmrC_bytes_to_bits(const struct target_t *target, int bytes) {
  return bytes * target->bits_in_char;
}

static inline unsigned long long dmrC_array_element_offset(const struct target_t *target, unsigned int base_bits, int idx)
{
	int fragment = base_bits % target->bits_in_char;
	if (fragment)
		base_bits += target->bits_in_char - fragment;
	return base_bits * idx;
}

extern void dmrC_init_target(struct dmr_C *C);
extern void dmrC_destroy_target(struct dmr_C *C);


#endif
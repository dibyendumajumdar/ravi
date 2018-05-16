/*
* Copyright (C) 2003 Transmeta Corp.
*               2003 Linus Torvalds
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
/*
* This version is part of the dmr_c project.
* Copyright (C) 2017 Dibyendu Majumdar
*/

#include <stdio.h>
#include <stdlib.h>

#include <port.h>
#include <symbol.h>
#include <target.h>

enum dummy { DUMMY };

void dmrC_init_target(struct dmr_C *C) {

  struct target_t *t = (struct target_t *)calloc(1, sizeof(struct target_t));

  /*
  * For "__attribute__((aligned))"
  */
  t->max_alignment = 16;

  /*
  * Integer data types
  */
  t->bits_in_bool = 1;
  t->bits_in_char = 8;
  t->bits_in_short = 16;
  t->bits_in_int = 32;
  t->bits_in_long = sizeof(long) * t->bits_in_char;
  t->bits_in_longlong = 64;
  t->bits_in_longlonglong = 128;

  t->bits_in_wchar = 32;
  t->max_int_alignment = __alignof__(long long); 

  /*
  * Floating point data types
  */
  t->bits_in_float = 32;
  t->bits_in_double = 64;
  t->bits_in_longdouble = sizeof(long double) * t->bits_in_char;

  t->max_fp_alignment = 8;

  /*
  * Pointer data type
  */
  t->bits_in_pointer = sizeof(void *) * t->bits_in_char;
  t->pointer_alignment = __alignof__(void *);

  /*
  * Enum data types
  */
  t->bits_in_enum = 32;
  t->enum_alignment = 4;

  C->target = t;
}

void dmrC_destroy_target(struct dmr_C *C) {
	free(C->target);
	C->target = NULL;
}


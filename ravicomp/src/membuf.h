/******************************************************************************
 * Copyright (C) 2020-2022 Dibyendu Majumdar
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

#ifndef ravicomp_MEMBUF_H
#define ravicomp_MEMBUF_H

#include <ravi_compiler.h>

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

extern void raviX_buffer_add_bool(TextBuffer *mb, bool value);
extern void raviX_buffer_add_int(TextBuffer *mb, int value);
extern void raviX_buffer_add_longlong(TextBuffer *mb, int64_t value);
extern void raviX_buffer_add_char(TextBuffer *mb, char c);
extern void raviX_buffer_add_double(TextBuffer *mb, double d); // Outputs the double as string using ryu

/* Following add and remove raw bytes */

/* Unchecked - user must first resize */
static inline void raviX_buffer_addc(TextBuffer *mb, int c)
{
	mb->buf[mb->pos++] = (char)c;
	assert(mb->pos < mb->capacity);
}
static inline void raviX_buffer_remove(TextBuffer *mb, int i) { mb->pos -= i; }

#endif

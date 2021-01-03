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
		mb->buf = (char *)calloc(1, initial_size);
		if (mb->buf == NULL) {
			fprintf(stderr, "out of memory\n");
			exit(1);
		}
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
void raviX_buffer_free(TextBuffer *mb) { free(mb->buf); }
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
		if (n > estimated_size) {
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

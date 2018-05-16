/*
* sparse/char.c
*
* Copyright (C) 2003 Transmeta Corp.
*               2003-2004 Linus Torvalds
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
#include <string.h>

#include <port.h>
#include <target.h>
#include <lib.h>
#include <allocate.h>
#include <token.h>
#include <expression.h>

static const char *parse_escape(struct dmr_C *C, const char *p, unsigned *val, const char *end, int bits, struct position pos)
{
	unsigned c = *p++;
	unsigned d;
	if (c != '\\') {
		*val = c;
		return p;
	}

	c = *p++;
	switch (c) {
	case 'a': c = '\a'; break;
	case 'b': c = '\b'; break;
	case 't': c = '\t'; break;
	case 'n': c = '\n'; break;
	case 'v': c = '\v'; break;
	case 'f': c = '\f'; break;
	case 'r': c = '\r'; break;
#ifndef _MSC_VER
	case 'e': c = '\e'; break;
#endif
	case 'x': {
		unsigned mask = -(1U << (bits - 4));
		for (c = 0; p < end; c = (c << 4) + d) {
			d = dmrC_hexval(*p);
			if (d > 16)
				break;
			p++;
			if (c & mask) {
				dmrC_warning(C, pos,
					"hex escape sequence out of range");
				mask = 0;
			}
		}
		break;
	}
	case '0':case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': {
		if (p + 2 < end)
			end = p + 2;
		c -= '0';
		while (p < end && (d = *p - '0') < 8) {
			c = (c << 3) + d;
			p++;
		}
		if ((c & 0400) && bits < 9)
			dmrC_warning(C, pos,
				"octal escape sequence out of range");
		break;
	}
	default:	/* everything else is left as is */
		dmrC_warning(C, pos, "unknown escape sequence: '\\%c'", c);
		break;
	case '\\':
	case '\'':
	case '"':
	case '?':
		break;	/* those are legal, so no warnings */
	}
	*val = c & ~((~0U << (bits - 1)) << 1);
	return p;
}

void dmrC_get_char_constant(struct dmr_C *C, struct token *token, unsigned long long *val)
{
	const char *p = token->embedded, *end;
	unsigned v;
	int type = dmrC_token_type(token);
	switch (type) {
	case TOKEN_CHAR:
	case TOKEN_WIDE_CHAR:
		p = token->string->data;
		end = p + token->string->length - 1;
		break;
	case TOKEN_CHAR_EMBEDDED_0:
	case TOKEN_CHAR_EMBEDDED_1:
	case TOKEN_CHAR_EMBEDDED_2:
	case TOKEN_CHAR_EMBEDDED_3:
		end = p + type - TOKEN_CHAR;
		break;
	default:
		end = p + type - TOKEN_WIDE_CHAR;
	}
	p = parse_escape(C, p, &v, end,
			type < TOKEN_WIDE_CHAR ? C->target->bits_in_char : C->target->bits_in_wchar, token->pos);
	if (p != end)
		dmrC_warning(C, token->pos,
			"multi-character character constant");
	*val = v;
}

struct token *dmrC_get_string_constant(struct dmr_C *C, struct token *token, struct expression *expr)
{
	struct string *string = token->string;
	struct token *next = token->next, *done = NULL;
	int stringtype = dmrC_token_type(token);
	int is_wide = stringtype == TOKEN_WIDE_STRING;
	char buffer[MAX_STRING];
	int len = 0;
	int bits;
	int esc_count = 0;

	while (!done) {
		switch (dmrC_token_type(next)) {
		case TOKEN_WIDE_STRING:
			is_wide = 1;
		case TOKEN_STRING:
			next = next->next;
			break;
		default:
			done = next;
		}
	}
	bits = is_wide ? C->target->bits_in_wchar : C->target->bits_in_char;
	while (token != done) {
		unsigned v;
		const char *p = token->string->data;
		const char *end = p + token->string->length - 1;
		while (p < end) {
			if (*p == '\\')
				esc_count++;
			p = parse_escape(C, p, &v, end, bits, token->pos);
			if (len < MAX_STRING)
				buffer[len] = v;
			len++;
		}
		token = token->next;
	}
	if (len > MAX_STRING) {
		dmrC_warning(C, token->pos, "trying to concatenate %d-character string (%d bytes max)", len, MAX_STRING);
		len = MAX_STRING;
	}

	if (esc_count || len >= (int)string->length) {
		if (string->immutable || len >= string->length)	/* can't cannibalize */
			string = (struct string *)dmrC_allocator_allocate(&C->string_allocator, len+1);
		string->length = len+1;
		memcpy(string->data, buffer, len);
		string->data[len] = '\0';
	}
	expr->string = string;
	expr->wide = is_wide;
	return token;
}

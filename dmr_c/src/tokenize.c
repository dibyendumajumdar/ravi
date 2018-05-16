/*
* This is a really stupid C tokenizer. It doesn't do any include
* files or anything complex at all. That's the preprocessor.
*
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
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allocate.h>
#include <lib.h>
#include <port.h>
#include <symbol.h>
#include <token.h>

#define EOF (-1)
#define BUFSIZE (8192)

typedef struct {
	int fd, offset, size;
	int pos, line, nr;
	int newline, whitespace;
	struct token **tokenlist;
	struct token *token;
	unsigned char *buffer;
} stream_t;

const char *dmrC_stream_name(struct dmr_C *C, int stream)
{
	if (stream == 0 && C->T->input_stream_nr == 0)
		return "<buffer>";
	if (stream < 0 || stream > C->T->input_stream_nr)
		return "<bad stream>";
	return C->T->input_streams[stream].name;
}

static struct position stream_pos(stream_t *stream)
{
	struct position pos;
	pos.type = 0;
	pos.stream = stream->nr;
	pos.newline = stream->newline;
	pos.whitespace = stream->whitespace;
	pos.pos = stream->pos;
	pos.line = stream->line;
	pos.noexpand = 0;
	return pos;
}

const char *dmrC_show_special(struct dmr_C *C, int val)
{
	C->T->special_buffer[0] = val;
	C->T->special_buffer[1] = 0;
	if (val >= SPECIAL_BASE)
		strcpy(C->T->special_buffer,
		       (char *)dmrC_combinations_[val - SPECIAL_BASE]);
	return C->T->special_buffer;
}

const char *dmrC_show_ident(struct dmr_C *C, const struct ident *ident)
{
	if (!ident)
		return "<noident>";
	sprintf(C->T->ident_buffer, "%.*s", ident->len, ident->name);
	return C->T->ident_buffer;
}

static char *charstr(char *ptr, unsigned char c, unsigned char escape, unsigned char next)
{
	if (isprint(c)) {
		if (c == escape || c == '\\')
			*ptr++ = '\\';
		*ptr++ = c;
		return ptr;
	}
	*ptr++ = '\\';
	switch (c) {
	case '\n':
		*ptr++ = 'n';
		return ptr;
	case '\t':
		*ptr++ = 't';
		return ptr;
	}
	if (!isdigit(next))
		return ptr + sprintf(ptr, "%o", c);
		
	return ptr + sprintf(ptr, "%03o", c);
}

const char *dmrC_show_string(struct dmr_C *C, const struct string *string)
{
	char *ptr;
	int i;

	if (!string->length)
		return "<bad_string>";
	ptr = C->T->string_buffer;
	*ptr++ = '"';
	for (i = 0; i < string->length - 1; i++) {
		const char *p = string->data + i;
		ptr = charstr(ptr, p[0], '"', p[1]);
	}
	*ptr++ = '"';
	*ptr = '\0';
	return C->T->string_buffer;
}

static const char *show_char(struct dmr_C *C, const char *s, size_t len, char prefix, char delim)
{
	char *p = C->T->char_buffer;
	if (prefix)
		*p++ = prefix;
	*p++ = delim;
	memcpy(p, s, len);
	p += len;
	*p++ = delim;
	*p++ = '\0';
	return C->T->char_buffer;
}

static const char *quote_char(struct dmr_C *C, const char *s, size_t len, char prefix, char delim)
{
	size_t i;
	char *p = C->T->quote_buffer;
	if (prefix)
		*p++ = prefix;
	if (delim == '"')
		*p++ = '\\';
	*p++ = delim;
	for (i = 0; i < len; i++) {
		if (s[i] == '"' || s[i] == '\\')
			*p++ = '\\';
		*p++ = s[i];
	}
	if (delim == '"')
		*p++ = '\\';
	*p++ = delim;
	*p++ = '\0';
	return C->T->quote_buffer;
}

const char *dmrC_show_token(struct dmr_C *C, const struct token *token)
{
	if (!token)
		return "<no token>";
	switch (dmrC_token_type(token)) {
	case TOKEN_ERROR:
		return "syntax error";

	case TOKEN_EOF:
		return "end-of-input";

	case TOKEN_IDENT:
		return dmrC_show_ident(C, token->ident);

	case TOKEN_NUMBER:
		return token->number;

	case TOKEN_SPECIAL:
		return dmrC_show_special(C, token->special);

	case TOKEN_CHAR:
		return show_char(C, token->string->data,
				 token->string->length - 1, 0, '\'');
	case TOKEN_CHAR_EMBEDDED_0:
	case TOKEN_CHAR_EMBEDDED_1:
	case TOKEN_CHAR_EMBEDDED_2:
	case TOKEN_CHAR_EMBEDDED_3:
		return show_char(C, token->embedded,
				 dmrC_token_type(token) - TOKEN_CHAR, 0, '\'');
	case TOKEN_WIDE_CHAR:
		return show_char(C, token->string->data,
				 token->string->length - 1, 'L', '\'');
	case TOKEN_WIDE_CHAR_EMBEDDED_0:
	case TOKEN_WIDE_CHAR_EMBEDDED_1:
	case TOKEN_WIDE_CHAR_EMBEDDED_2:
	case TOKEN_WIDE_CHAR_EMBEDDED_3:
		return show_char(C, token->embedded,
				 dmrC_token_type(token) - TOKEN_WIDE_CHAR, 'L', '\'');
	case TOKEN_STRING:
		return show_char(C, token->string->data,
				 token->string->length - 1, 0, '"');
	case TOKEN_WIDE_STRING:
		return show_char(C, token->string->data,
				 token->string->length - 1, 'L', '"');

	case TOKEN_STREAMBEGIN:
		snprintf(C->T->token_buffer, sizeof C->T->token_buffer,
			 "<beginning of '%s'>",
			 dmrC_stream_name(C, token->pos.stream));
		return C->T->token_buffer;

	case TOKEN_STREAMEND:
		snprintf(C->T->token_buffer, sizeof C->T->token_buffer,
			 "<end of '%s'>", dmrC_stream_name(C, token->pos.stream));
		return C->T->token_buffer;

	case TOKEN_UNTAINT:
		snprintf(C->T->token_buffer, sizeof C->T->token_buffer,
			 "<untaint>");
		return C->T->token_buffer;

	case TOKEN_ARG_COUNT:
		snprintf(C->T->token_buffer, sizeof C->T->token_buffer,
			 "<argcnt>");
		return C->T->token_buffer;

	default:
		snprintf(C->T->token_buffer, sizeof C->T->token_buffer,
			 "unhandled token type '%d' ", dmrC_token_type(token));
		return C->T->token_buffer;
	}
}

const char *dmrC_quote_token(struct dmr_C *C, const struct token *token)
{
	switch (dmrC_token_type(token)) {
	case TOKEN_ERROR:
		return "syntax error";

	case TOKEN_IDENT:
		return dmrC_show_ident(C, token->ident);

	case TOKEN_NUMBER:
		return token->number;

	case TOKEN_SPECIAL:
		return dmrC_show_special(C, token->special);

	case TOKEN_CHAR:
		return quote_char(C, token->string->data,
				  token->string->length - 1, 0, '\'');
	case TOKEN_CHAR_EMBEDDED_0:
	case TOKEN_CHAR_EMBEDDED_1:
	case TOKEN_CHAR_EMBEDDED_2:
	case TOKEN_CHAR_EMBEDDED_3:
		return quote_char(C, token->embedded,
				  dmrC_token_type(token) - TOKEN_CHAR, 0, '\'');
	case TOKEN_WIDE_CHAR:
		return quote_char(C, token->string->data,
				  token->string->length - 1, 'L', '\'');
	case TOKEN_WIDE_CHAR_EMBEDDED_0:
	case TOKEN_WIDE_CHAR_EMBEDDED_1:
	case TOKEN_WIDE_CHAR_EMBEDDED_2:
	case TOKEN_WIDE_CHAR_EMBEDDED_3:
		return quote_char(C, token->embedded,
				  dmrC_token_type(token) - TOKEN_WIDE_CHAR, 'L',
				  '\'');
	case TOKEN_STRING:
		return quote_char(C, token->string->data,
				  token->string->length - 1, 0, '"');
	case TOKEN_WIDE_STRING:
		return quote_char(C, token->string->data,
				  token->string->length - 1, 'L', '"');
	default:
		snprintf(C->T->quoted_token_buffer,
			 sizeof C->T->quoted_token_buffer,
			 "unhandled token type '%d' ", dmrC_token_type(token));
		return C->T->quoted_token_buffer;
	}
}

#define HASHED_INPUT_BITS (6)
#define HASHED_INPUT (1 << HASHED_INPUT_BITS)
#define HASH_PRIME 0x9e370001UL

#ifndef _MSC_VER
static int input_stream_hashes[HASHED_INPUT] = {[0 ... HASHED_INPUT - 1] = -1};
#else
static int input_stream_hashes[HASHED_INPUT] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
#endif

int *dmrC_hash_stream(const char *name)
{
	uint32_t hash = 0;
	unsigned char c;

	while ((c = *name++) != 0)
		hash = (hash + (c << 4) + (c >> 4)) * 11;

	hash *= HASH_PRIME;
	hash >>= 32 - HASHED_INPUT_BITS;
	return input_stream_hashes + hash;
}

int dmrC_init_stream(struct dmr_C *C, const char *name, int fd, const char **next_path)
{
	int stream = C->T->input_stream_nr, *hash;
	struct stream *current;

	if (stream >= C->T->input_streams_allocated) {
		int newalloc = stream * 4 / 3 + 10;
		C->T->input_streams = (struct stream *)realloc(C->T->input_streams, newalloc * sizeof(struct stream));
		if (!C->T->input_streams)
			dmrC_die(C, "Unable to allocate more streams space");
		C->T->input_streams_allocated = newalloc;
	}
	current = C->T->input_streams + stream;
	memset(current, 0, sizeof(*current));
	current->name = name;
	current->fd = fd;
	current->next_path = next_path;
	current->path = NULL;
	current->constant = CONSTANT_FILE_MAYBE;
	C->T->input_stream_nr = stream + 1;
	hash = dmrC_hash_stream(name);
	current->next_stream = *hash;
	*hash = stream;
	return stream;
}

static struct token *alloc_token(struct dmr_C *C, stream_t *stream)
{
	struct token *token = (struct token *)dmrC_allocator_allocate(&C->token_allocator, 0);
	token->pos = stream_pos(stream);
	return token;
}

/*
*  Argh...  That was surprisingly messy - handling '\r' complicates the
*  things a _lot_.
*/
static int nextchar_slow(struct dmr_C *C, stream_t *stream)
{
	int offset = stream->offset;
	int size = stream->size;
	int c;
	int spliced = 0, had_cr, had_backslash;

restart:
	had_cr = had_backslash = 0;

repeat:
	if (offset >= size) {
		if (stream->fd < 0)
			goto got_eof;
		size = read(stream->fd, stream->buffer, BUFSIZE);
		if (size <= 0)
			goto got_eof;
		stream->size = size;
		stream->offset = offset = 0;
	}

	c = stream->buffer[offset++];
	if (had_cr)
		goto check_lf;

	if (c == '\r') {
		had_cr = 1;
		goto repeat;
	}

norm:
	if (!had_backslash) {
		switch (c) {
		case '\t':
			stream->pos += C->T->tabstop - stream->pos % C->T->tabstop;
			break;
		case '\n':
			stream->line++;
			stream->pos = 0;
			stream->newline = 1;
			break;
		case '\\':
			had_backslash = 1;
			stream->pos++;
			goto repeat;
		default:
			stream->pos++;
		}
	} else {
		if (c == '\n') {
			stream->line++;
			stream->pos = 0;
			spliced = 1;
			goto restart;
		}
		offset--;
		c = '\\';
	}
out:
	stream->offset = offset;

	return c;

check_lf:
	if (c != '\n')
		offset--;
	c = '\n';
	goto norm;

got_eof:
	if (had_backslash) {
		c = '\\';
		goto out;
	}
	if (stream->pos)
		dmrC_warning(C, stream_pos(stream), "no newline at end of file");
	else if (spliced)
		dmrC_warning(C, stream_pos(stream), "backslash-newline at end of file");
	return EOF;
}

/*
*  We want that as light as possible while covering all normal cases.
*  Slow path (including the logics with line-splicing and EOF sanity
*  checks) is in nextchar_slow().
*/
static inline int nextchar(struct dmr_C *C, stream_t *stream)
{
	int offset = stream->offset;

	if (offset < stream->size) {
		int c = stream->buffer[offset++];
		if (!C->T->special[c]) {
			stream->offset = offset;
			stream->pos++;
			return c;
		}
	}
	return nextchar_slow(C, stream);
}

struct token dmrC_eof_token_entry_;

static struct token *mark_eof(struct dmr_C *C, stream_t *stream)
{
	struct token *end;

	end = alloc_token(C, stream);
	dmrC_token_type(end) = TOKEN_STREAMEND;
	end->pos.newline = 1;

	dmrC_eof_token_entry_.next = &dmrC_eof_token_entry_;
	dmrC_eof_token_entry_.pos.newline = 1;

	end->next = &dmrC_eof_token_entry_;
	*stream->tokenlist = end;
	stream->tokenlist = NULL;
	return end;
}

static void add_token(stream_t *stream)
{
	struct token *token = stream->token;

	stream->token = NULL;
	token->next = NULL;
	*stream->tokenlist = token;
	stream->tokenlist = &token->next;
}

static void drop_token(stream_t *stream)
{
	stream->newline |= stream->token->pos.newline;
	stream->whitespace |= stream->token->pos.whitespace;
	stream->token = NULL;
}

enum {
	Letter = 1,
	Digit = 2,
	Hex = 4,
	Exp = 8,
	Dot = 16,
	ValidSecond = 32,
	Quote = 64,
};

/*
* pp-number:
*	digit
*	. digit
*	pp-number digit
*	pp-number identifier-nodigit
*	pp-number e sign
*	pp-number E sign
*	pp-number p sign
*	pp-number P sign
*	pp-number .
*/
static int get_one_number(struct dmr_C *C, int c, int next, stream_t *stream)
{
	struct token *token;
	char *buffer = C->T->number_buffer;
	char *p = buffer, *buf, *buffer_end = buffer + sizeof C->T->number_buffer;
	size_t len;

	*p++ = c;
	for (;;) {
		long kclass = C->T->cclass[next + 1];
		if (!(kclass & (Dot | Digit | Letter)))
			break;
		if (p != buffer_end)
			*p++ = next;
		next = nextchar(C, stream);
		if (kclass & Exp) {
			if (next == '-' || next == '+') {
				if (p != buffer_end)
					*p++ = next;
				next = nextchar(C, stream);
			}
		}
	}

	if (p == buffer_end) {
		dmrC_sparse_error(C, stream_pos(stream),
			     "number token exceeds %td characters",
			     buffer_end - buffer);
		// Pretend we saw just "1".
		buffer[0] = '1';
		p = buffer + 1;
	}

	*p++ = 0;
	len = p - buffer;
	buf = (char *)dmrC_allocator_allocate(&C->byte_allocator, len);
	memcpy(buf, buffer, len);

	token = stream->token;
	dmrC_token_type(token) = TOKEN_NUMBER;
	token->number = buf;
	add_token(stream);

	return next;
}

static int eat_string(struct dmr_C *C, int next, stream_t *stream,
		      enum e_token_type type)
{
	char *buffer = C->T->string_buffer2;
	struct string *string;
	struct token *token = stream->token;
	int len = 0;
	int escape;
	int want_hex = 0;
	char delim = type < TOKEN_STRING ? '\'' : '"';

	for (escape = 0; escape || next != delim; next = nextchar(C, stream)) {
		if (len < MAX_STRING)
			buffer[len] = next;
		len++;
		if (next == '\n') {
			dmrC_warning(C, stream_pos(stream),
				"Newline in string or character constant");
			if (delim == '\'') /* assume it's lost ' */
				break;
		}
		if (next == EOF) {
			dmrC_warning(C, stream_pos(stream),
				"End of file in middle of string");
			return next;
		}
		if (!escape) {
			if (want_hex && !(C->T->cclass[next + 1] & Hex))
				dmrC_warning(
				    C, stream_pos(stream),
				    "\\x used with no following hex digits");
			want_hex = 0;
			escape = next == '\\';
		} else {
			escape = 0;
			want_hex = next == 'x';
		}
	}
	if (want_hex)
		dmrC_warning(C, stream_pos(stream),
			"\\x used with no following hex digits");
	if (len > MAX_STRING) {
		dmrC_warning(C, stream_pos(stream),
			"string too long (%d bytes, %d bytes max)", len, MAX_STRING);
		len = MAX_STRING;
	}
	if (delim == '\'' && len <= 4) {
		if (len == 0) {
			dmrC_sparse_error(C, stream_pos(stream),
				     "empty character constant");
			return nextchar(C, stream);
		}
		dmrC_token_type(token) = type + len;
		memset(buffer + len, '\0', 4 - len);
		memcpy(token->embedded, buffer, 4);
	} else {
		dmrC_token_type(token) = type;
		string = (struct string *)dmrC_allocator_allocate(&C->string_allocator, len + 1);
		memcpy(string->data, buffer, len);
		string->data[len] = '\0';
		string->length = len + 1;
		token->string = string;
	}

	/* Pass it on.. */
	token = stream->token;
	add_token(stream);
	return nextchar(C, stream);
}

static int drop_stream_eoln(struct dmr_C *C, stream_t *stream)
{
	drop_token(stream);
	for (;;) {
		switch (nextchar(C, stream)) {
		case EOF:
			return EOF;
		case '\n':
			return nextchar(C, stream);
		}
	}
}

static int drop_stream_comment(struct dmr_C *C, stream_t *stream)
{
	int newline;
	int next;
	drop_token(stream);
	newline = stream->newline;

	next = nextchar(C, stream);
	for (;;) {
		int curr = next;
		if (curr == EOF) {
			dmrC_warning(C, stream_pos(stream),
				"End of file in the middle of a comment");
			return curr;
		}
		next = nextchar(C, stream);
		if (curr == '*' && next == '/')
			break;
	}
	stream->newline = newline;
	return nextchar(C, stream);
}

unsigned char dmrC_combinations_[][4] = COMBINATION_STRINGS;

#define NR_COMBINATIONS (SPECIAL_ARG_SEPARATOR - SPECIAL_BASE)

/* hash function for two-character punctuators - all give unique values */
#define special_hash(c0, c1) (((c0*8+c1*2)+((c0*8+c1*2)>>5))&31)

static int get_one_special(struct dmr_C *C, int c, stream_t *stream)
{
	struct token *token;
	int next, value, i;

	next = nextchar(C, stream);

	/*
	* Check for numbers, strings, character constants, and comments
	*/
	switch (c) {
	case '.':
		if (next >= '0' && next <= '9')
			return get_one_number(C, c, next, stream);
		break;
	case '"':
		return eat_string(C, next, stream, TOKEN_STRING);
	case '\'':
		return eat_string(C, next, stream, TOKEN_CHAR);
	case '/':
		if (next == '/')
			return drop_stream_eoln(C, stream);
		if (next == '*')
			return drop_stream_comment(C, stream);
	}

	/*
	* Check for combinations
	*/
	value = c;
	if (C->T->cclass[next + 1] & ValidSecond) {
		i = special_hash(c, next);
		if (C->T->hash_results[i][0] == c && C->T->hash_results[i][1] == next) {
			value = C->T->code[i];
			next = nextchar(C, stream);
			if (value >= SPECIAL_LEFTSHIFT &&
			    next == "==."[value - SPECIAL_LEFTSHIFT]) {
				value += 3;
				next = nextchar(C, stream);
			}
		}
	}

	/* Pass it on.. */
	token = stream->token;
	dmrC_token_type(token) = TOKEN_SPECIAL;
	token->special = value;
	add_token(stream);
	return next;
}

#define IDENT_HASH_BITS (13)
#define IDENT_HASH_SIZE (1<<IDENT_HASH_BITS)
#define IDENT_HASH_MASK (IDENT_HASH_SIZE-1)

#define ident_hash_init(c)		(c)
#define ident_hash_add(oldhash,c)	((oldhash)*11 + (c))
#define ident_hash_end(hash)		((((hash) >> IDENT_HASH_BITS) + (hash)) & IDENT_HASH_MASK)

void dmrC_show_identifier_stats(struct dmr_C *C)
{
	int i;
	int distribution[100];

	fprintf(stderr, "identifiers: %d hits, %d misses\n", 
		C->T->ident_hit, C->T->ident_miss);

	for (i = 0; i < 100; i++)
		distribution[i] = 0;

	for (i = 0; i < IDENT_HASH_SIZE; i++) {
		struct ident *ident = C->T->hash_table[i];
		int count = 0;

		while (ident) {
			count++;
			ident = ident->next;
		}
		if (count > 99)
			count = 99;
		distribution[count]++;
	}

	for (i = 0; i < 100; i++) {
		if (distribution[i])
			fprintf(stderr, "%2d: %d buckets\n", i,
				distribution[i]);
	}
}

static struct ident *alloc_ident(struct dmr_C *C, const char *name, size_t len)
{
	struct ident *ident = (struct ident *)dmrC_allocator_allocate(&C->ident_allocator, len);
	ident->symbols = NULL;
	assert(len <= 256);
	ident->len = (unsigned char)len;
	ident->tainted = 0;
	memcpy(ident->name, name, len);
	return ident;
}

static struct ident *insert_hash(struct dmr_C *C, struct ident *ident,
				 unsigned long hash)
{
	ident->next = C->T->hash_table[hash];
	C->T->hash_table[hash] = ident;
	C->T->ident_miss++;
	return ident;
}

static struct ident *create_hashed_ident(struct dmr_C *C, const char *name,
					 size_t len, unsigned long hash)
{
	struct ident *ident;
	struct ident **p;

	p = &C->T->hash_table[hash];
	while ((ident = *p) != NULL) {
		if (ident->len == (unsigned char) len) {
			if (strncmp(name, ident->name, len) != 0)
				goto next;

			C->T->ident_hit++;
			return ident;
		}
next:
		//misses++;
		p = &ident->next;
	}
	ident = alloc_ident(C, name, len);
	*p = ident;
	ident->next = NULL;
	C->T->ident_miss++;
	C->T->idents++;
	return ident;
}

static unsigned long hash_name(const char *name, int len)
{
	unsigned long hash;
	const unsigned char *p = (const unsigned char *)name;

	hash = ident_hash_init(*p++);
	while (--len) {
		unsigned int i = *p++;
		hash = ident_hash_add(hash, i);
	}
	return ident_hash_end(hash);
}

struct ident *dmrC_hash_ident(struct dmr_C *C, struct ident *ident)
{
	return insert_hash(C, ident, hash_name(ident->name, (int)ident->len));
}

struct ident *dmrC_built_in_ident(struct dmr_C *C, const char *name)
{
	size_t len = strlen(name);
	return create_hashed_ident(C, name, len, hash_name(name, (int)len));
}

struct token *dmrC_built_in_token(struct dmr_C *C, int stream, struct ident *ident)
{
	struct token *token;

	token = (struct token *)dmrC_allocator_allocate(&C->token_allocator, 0);
	token->pos.stream = stream;
	dmrC_token_type(token) = TOKEN_IDENT;
	token->ident = ident;
	return token;
}

static int get_one_identifier(struct dmr_C *C, int c, stream_t *stream)
{
	struct token *token;
	struct ident *ident;
	unsigned long hash;
	char buf[256];
	int len = 1;
	int next;

	hash = ident_hash_init(c);
	buf[0] = c;
	for (;;) {
		next = nextchar(C, stream);
		if (!(C->T->cclass[next + 1] & (Letter | Digit)))
			break;
		if (len >= (int)sizeof(buf))
			break;
		hash = ident_hash_add(hash, next);
		buf[len] = next;
		len++;
	};
	if (C->T->cclass[next + 1] & Quote) {
		if (len == 1 && buf[0] == 'L') {
			if (next == '\'')
				return eat_string(C, nextchar(C, stream),
						  stream, TOKEN_WIDE_CHAR);
			else
				return eat_string(C, nextchar(C, stream),
						  stream, TOKEN_WIDE_STRING);
		}
	}
	hash = ident_hash_end(hash);
	ident = create_hashed_ident(C, buf, len, hash);

	/* Pass it on.. */
	token = stream->token;
	dmrC_token_type(token) = TOKEN_IDENT;
	token->ident = ident;
	add_token(stream);
	return next;
}

static int get_one_token(struct dmr_C *C, int c, stream_t *stream)
{
	long kclass = C->T->cclass[c + 1];
	if (kclass & Digit)
		return get_one_number(C, c, nextchar(C, stream), stream);
	if (kclass & Letter)
		return get_one_identifier(C, c, stream);
	return get_one_special(C, c, stream);
}

static struct token *setup_stream(struct dmr_C *C, stream_t *stream,
				  int idx, int fd, unsigned char *buf,
				  unsigned int buf_size)
{
	struct token *begin;

	stream->nr = idx;
	stream->line = 1;
	stream->newline = 1;
	stream->whitespace = 0;
	stream->pos = 0;

	stream->token = NULL;
	stream->fd = fd;
	stream->offset = 0;
	stream->size = buf_size;
	stream->buffer = buf;

	begin = alloc_token(C, stream);
	dmrC_token_type(begin) = TOKEN_STREAMBEGIN;
	stream->tokenlist = &begin->next;
	return begin;
}

static struct token *tokenize_stream(struct dmr_C *C, stream_t *stream)
{
	int c = nextchar(C, stream);
	while (c != EOF) {
		if (!isspace(c)) {
			struct token *token = alloc_token(C, stream);
			stream->token = token;
			stream->newline = 0;
			stream->whitespace = 0;
			c = get_one_token(C, c, stream);
			continue;
		}
		stream->whitespace = 1;
		c = nextchar(C, stream);
	}
	return mark_eof(C, stream);
}

struct token *dmrC_tokenize_buffer(struct dmr_C *C, unsigned char *buffer,
			      unsigned long size, struct token **endtoken)
{
	stream_t stream;
	struct token *begin;

	begin = setup_stream(C, &stream, 0, -1, buffer, size);
	*endtoken = tokenize_stream(C, &stream);
	return begin;
}

struct token *dmrC_tokenize_buffer_stream(struct dmr_C *C, const char *name,
					  unsigned char *buffer,
					  unsigned long size,
					  struct token **endtoken)
{
	stream_t stream;
	struct token *begin;
	int idx;

	idx = dmrC_init_stream(C, name, -1, C->includepath);
	if (idx < 0) {
		return NULL;
	}
	begin = setup_stream(C, &stream, idx, -1, buffer, size);
	*endtoken = tokenize_stream(C, &stream);
	return begin;
}

struct token *dmrC_tokenize(struct dmr_C *C, const char *name, int fd,
		       struct token *endtoken, const char **next_path)
{
	struct token *begin, *end;
	stream_t stream;
	unsigned char buffer[BUFSIZE];
	int idx;

	idx = dmrC_init_stream(C, name, fd, next_path);
	if (idx < 0) {
		// dmrC_info(endtoken->pos, "File %s is const", name);
		return endtoken;
	}

	begin = setup_stream(C, &stream, idx, fd, buffer, 0);
	end = tokenize_stream(C, &stream);
	if (endtoken)
		end->next = endtoken;
	return begin;
}

void dmrC_init_tokenizer(struct dmr_C *C)
{
	C->T = (struct tokenizer_state_t *)calloc(
	    1, sizeof(struct tokenizer_state_t));
	C->T->tabstop = 8;

	C->T->special['\t'] = 1;
	C->T->special['\r'] = 1;
	C->T->special['\n'] = 1;
	C->T->special['\\'] = 1;

	C->T->cclass['0' + 1] = Digit | Hex; /* \<octal> */
	C->T->cclass['1' + 1] = Digit | Hex; /* \<octal> */
	C->T->cclass['2' + 1] = Digit | Hex; /* \<octal> */
	C->T->cclass['3' + 1] = Digit | Hex; /* \<octal> */
	C->T->cclass['4' + 1] = Digit | Hex; /* \<octal> */
	C->T->cclass['5' + 1] = Digit | Hex; /* \<octal> */
	C->T->cclass['6' + 1] = Digit | Hex; /* \<octal> */
	C->T->cclass['7' + 1] = Digit | Hex; /* \<octal> */
	C->T->cclass['8' + 1] = Digit | Hex;
	C->T->cclass['9' + 1] = Digit | Hex;
	C->T->cclass['A' + 1] = Letter | Hex;
	C->T->cclass['B' + 1] = Letter | Hex;
	C->T->cclass['C' + 1] = Letter | Hex;
	C->T->cclass['D' + 1] = Letter | Hex;
	C->T->cclass['E' + 1] = Letter | Hex | Exp; /* E<exp> */
	C->T->cclass['F' + 1] = Letter | Hex;
	C->T->cclass['G' + 1] = Letter;
	C->T->cclass['H' + 1] = Letter;
	C->T->cclass['I' + 1] = Letter;
	C->T->cclass['J' + 1] = Letter;
	C->T->cclass['K' + 1] = Letter;
	C->T->cclass['L' + 1] = Letter;
	C->T->cclass['M' + 1] = Letter;
	C->T->cclass['N' + 1] = Letter;
	C->T->cclass['O' + 1] = Letter;
	C->T->cclass['P' + 1] = Letter | Exp; /* P<exp> */
	C->T->cclass['Q' + 1] = Letter;
	C->T->cclass['R' + 1] = Letter;
	C->T->cclass['S' + 1] = Letter;
	C->T->cclass['T' + 1] = Letter;
	C->T->cclass['U' + 1] = Letter;
	C->T->cclass['V' + 1] = Letter;
	C->T->cclass['W' + 1] = Letter;
	C->T->cclass['X' + 1] = Letter;
	C->T->cclass['Y' + 1] = Letter;
	C->T->cclass['Z' + 1] = Letter;
	C->T->cclass['a' + 1] = Letter | Hex; /* \a, \b */
	C->T->cclass['b' + 1] = Letter | Hex; /* \a, \b */
	C->T->cclass['c' + 1] = Letter | Hex;
	C->T->cclass['d' + 1] = Letter | Hex;
	C->T->cclass['e' + 1] = Letter | Hex | Exp; /* \e, e<exp> */
	C->T->cclass['f' + 1] = Letter | Hex;       /* \f */
	C->T->cclass['g' + 1] = Letter;
	C->T->cclass['h' + 1] = Letter;
	C->T->cclass['i' + 1] = Letter;
	C->T->cclass['j' + 1] = Letter;
	C->T->cclass['k' + 1] = Letter;
	C->T->cclass['l' + 1] = Letter;
	C->T->cclass['m' + 1] = Letter;
	C->T->cclass['n' + 1] = Letter; /* \n */
	C->T->cclass['o' + 1] = Letter;
	C->T->cclass['p' + 1] = Letter | Exp; /* p<exp> */
	C->T->cclass['q' + 1] = Letter;
	C->T->cclass['r' + 1] = Letter; /* \r */
	C->T->cclass['s' + 1] = Letter;
	C->T->cclass['t' + 1] = Letter; /* \t */
	C->T->cclass['u' + 1] = Letter;
	C->T->cclass['v' + 1] = Letter; /* \v */
	C->T->cclass['w' + 1] = Letter;
	C->T->cclass['x' + 1] = Letter; /* \x<hex> */
	C->T->cclass['y' + 1] = Letter;
	C->T->cclass['z' + 1] = Letter;
	C->T->cclass['_' + 1] = Letter;
	C->T->cclass['.' + 1] = Dot | ValidSecond;
	C->T->cclass['=' + 1] = ValidSecond;
	C->T->cclass['+' + 1] = ValidSecond;
	C->T->cclass['-' + 1] = ValidSecond;
	C->T->cclass['>' + 1] = ValidSecond;
	C->T->cclass['<' + 1] = ValidSecond;
	C->T->cclass['&' + 1] = ValidSecond;
	C->T->cclass['|' + 1] = ValidSecond;
	C->T->cclass['#' + 1] = ValidSecond;
	C->T->cclass['\'' + 1] = Quote;
	C->T->cclass['"' + 1] = Quote;

/*
* note that we won't get false positives - special_hash(0,0) is 0 and
* entry 0 is filled (by +=), so all the missing ones are OK.
*/
#define RES(c0, c1)                                                            \
	C->T->hash_results[special_hash(c0, c1)][0] = c0,                          \
		      C->T->hash_results[special_hash(c0, c1)][1] = c1
	RES('+', '='); /* 00 */
	RES('/', '='); /* 01 */
	RES('^', '='); /* 05 */
	RES('&', '&'); /* 07 */
	RES('#', '#'); /* 08 */
	RES('<', '<'); /* 0a */
	RES('<', '='); /* 0c */
	RES('!', '='); /* 0e */
	RES('%', '='); /* 0f */
	RES('-', '-'); /* 10 */
	RES('-', '='); /* 11 */
	RES('-', '>'); /* 13 */
	RES('=', '='); /* 15 */
	RES('&', '='); /* 17 */
	RES('*', '='); /* 18 */
	RES('.', '.'); /* 1a */
	RES('+', '+'); /* 1b */
	RES('|', '='); /* 1c */
	RES('>', '='); /* 1d */
	RES('|', '|'); /* 1e */
	RES('>', '>'); /* 1f */
#undef RES

#define CODE(c0, c1, value) C->T->code[special_hash(c0, c1)] = value
	CODE('+', '=', SPECIAL_ADD_ASSIGN);  /* 00 */
	CODE('/', '=', SPECIAL_DIV_ASSIGN);  /* 01 */
	CODE('^', '=', SPECIAL_XOR_ASSIGN);  /* 05 */
	CODE('&', '&', SPECIAL_LOGICAL_AND); /* 07 */
	CODE('#', '#', SPECIAL_HASHHASH);    /* 08 */
	CODE('<', '<', SPECIAL_LEFTSHIFT);   /* 0a */
	CODE('<', '=', SPECIAL_LTE);	 /* 0c */
	CODE('!', '=', SPECIAL_NOTEQUAL);    /* 0e */
	CODE('%', '=', SPECIAL_MOD_ASSIGN);  /* 0f */
	CODE('-', '-', SPECIAL_DECREMENT);   /* 10 */
	CODE('-', '=', SPECIAL_SUB_ASSIGN);  /* 11 */
	CODE('-', '>', SPECIAL_DEREFERENCE); /* 13 */
	CODE('=', '=', SPECIAL_EQUAL);       /* 15 */
	CODE('&', '=', SPECIAL_AND_ASSIGN);  /* 17 */
	CODE('*', '=', SPECIAL_MUL_ASSIGN);  /* 18 */
	CODE('.', '.', SPECIAL_DOTDOT);      /* 1a */
	CODE('+', '+', SPECIAL_INCREMENT);   /* 1b */
	CODE('|', '=', SPECIAL_OR_ASSIGN);   /* 1c */
	CODE('>', '=', SPECIAL_GTE);	 /* 1d */
	CODE('|', '|', SPECIAL_LOGICAL_OR);  /* 1e */
	CODE('>', '>', SPECIAL_RIGHTSHIFT);  /* 1f */
#undef CODE

	C->T->hash_table =
	    (struct ident **)calloc(IDENT_HASH_SIZE, sizeof(struct ident *));
}

void dmrC_destroy_tokenizer(struct dmr_C *C)
{
	free(C->T->input_streams);
	free(C->T->hash_table);
	free(C->T);
	C->T = NULL;
}

int dmrC_test_tokenizer()
{
	struct dmr_C *C = new_dmr_C();
	char test1[100] =
	    "int main() { printf(\"hello world!\\n\"); return 0; }";
	const char *expected[] = {"<beginning of 'builtin'>",
				  "int",
				  "main",
				  "(",
				  ")",
				  "{",
				  "printf",
				  "(",
				  "\"hello world!\\n\"",
				  ")",
				  ";",
				  "return",
				  "0",
				  ";",
				  "}",
				  NULL};

	struct token *start;
	struct token *end;
	start = dmrC_tokenize_buffer(C, (unsigned char *)test1,
				(unsigned long)strlen(test1), &end);
	int i = 0;
	int failure_count = 0;
	for (struct token *p = start; p != end; p = p->next) {
		if (expected[i] == NULL) {
			failure_count++;
			break;
		}
		if (strcmp(dmrC_show_token(C, p), expected[i]) != 0) {
			fprintf(stderr, "Expected '%s' got '%s'\n", expected[i],
				dmrC_show_token(C, p));
			failure_count++;
			break;
		}
		i++;
	}
	destroy_dmr_C(C);
	if (failure_count == 0) {
		printf("tokenizer hello world test okay\n");
	}
	return failure_count;
}

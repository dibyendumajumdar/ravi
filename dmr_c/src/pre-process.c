/*
 * Do C preprocessing, based on a token list gathered by
 * the tokenizer.
 *
 * This may not be the smartest preprocessor on the planet.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>

#include <port.h>
#include <lib.h>
#include <allocate.h>
#include <parse.h>
#include <token.h>
#include <symbol.h>
#include <expression.h>
#include <scope.h>

#define dirty_stream(stream)				\
	do {						\
		if (!stream->dirty) {			\
			stream->dirty = 1;		\
			if (!stream->ifndef)		\
				stream->protect = NULL;	\
		}					\
	} while(0)

#define end_group(stream)					\
	do {							\
		if (stream->ifndef == stream->top_if) {		\
			stream->ifndef = NULL;			\
			if (!stream->dirty)			\
				stream->protect = NULL;		\
			else if (stream->protect)		\
				stream->dirty = 0;		\
		}						\
	} while(0)

#define nesting_error(stream)		\
	do {				\
		stream->dirty = 1;	\
		stream->ifndef = NULL;	\
		stream->protect = NULL;	\
	} while(0)

static struct token *alloc_token(struct dmr_C *C, struct position *pos)
{
	struct token *token = (struct token *)dmrC_allocator_allocate(&C->token_allocator, 0);

	token->pos.stream = pos->stream;
	token->pos.line = pos->line;
	token->pos.pos = pos->pos;
	token->pos.whitespace = 1;
	return token;
}

/* Expand symbol 'sym' at '*list' */
static int expand(struct dmr_C *C, struct token **, struct symbol *);

static void replace_with_string(struct dmr_C *C, struct token *token, const char *str)
{
	int size = (int) strlen(str) + 1;
	struct string *s = (struct string *)dmrC_allocator_allocate(&C->string_allocator, size);

	s->length = size;
	memcpy(s->data, str, size);
	dmrC_token_type(token) = TOKEN_STRING;
	token->string = s;
}

static void replace_with_integer(struct dmr_C *C, struct token *token, unsigned int val)
{
	char *buf = (char *)dmrC_allocator_allocate(&C->byte_allocator, 11);
	sprintf(buf, "%u", val);
	dmrC_token_type(token) = TOKEN_NUMBER;
	token->number = buf;
}

static struct symbol *lookup_macro(struct dmr_C *C, struct ident *ident)
{
	(void) C;
	struct symbol *sym = dmrC_lookup_symbol(ident, (enum namespace_type) (NS_MACRO | NS_UNDEF));
	if (sym && sym->ns != NS_MACRO)
		sym = NULL;
	return sym;
}

static int token_defined(struct dmr_C *C, struct token *token)
{
	if (dmrC_token_type(token) == TOKEN_IDENT) {
		struct symbol *sym = lookup_macro(C, token->ident);
		if (sym) {
			sym->used_in = C->file_scope;
			return 1;
		}
		return 0;
	}

	dmrC_sparse_error(C, token->pos, "expected preprocessor identifier");
	return 0;
}

static void replace_with_defined(struct dmr_C *C, struct token *token)
{
	static const char *string[] = { "0", "1" };
	int defined = token_defined(C, token);

	dmrC_token_type(token) = TOKEN_NUMBER;
	token->number = string[defined];
}

static int expand_one_symbol(struct dmr_C *C, struct token **list)
{
	struct token *token = *list;
	struct symbol *sym;

	if (token->pos.noexpand)
		return 1;

	sym = lookup_macro(C, token->ident);
	if (sym) {
		sym->used_in = C->file_scope;
		return expand(C, list, sym);
	}
	if (token->ident == C->S->__LINE___ident) {
		replace_with_integer(C, token, token->pos.line);
	} else if (token->ident == C->S->__FILE___ident) {
		replace_with_string(C, token, dmrC_stream_name(C, token->pos.stream));
	} else if (token->ident == C->S->__DATE___ident) {
		if (!C->t)
			time(&C->t);
		strftime(C->date_buffer, 12, "%b %e %Y", localtime(&C->t));
		replace_with_string(C, token, C->date_buffer);
	} else if (token->ident == C->S->__TIME___ident) {
		if (!C->t)
			time(&C->t);
		strftime(C->date_buffer, 9, "%T", localtime(&C->t));
		replace_with_string(C, token, C->date_buffer);
	} else if (token->ident == C->S->__COUNTER___ident) {
		replace_with_integer(C, token, C->counter_macro++);
	}
	return 1;
}

static inline struct token *scan_next(struct dmr_C *C, struct token **where)
{
	struct token *token = *where;
	(void) C;
	if (dmrC_token_type(token) != TOKEN_UNTAINT)
		return token;
	do {
		token->ident->tainted = 0;
		token = token->next;
	} while (dmrC_token_type(token) == TOKEN_UNTAINT);
	*where = token;
	return token;
}

static void expand_list(struct dmr_C *C, struct token **list)
{
	struct token *next;
	while (!dmrC_eof_token(next = scan_next(C, list))) {
		if (dmrC_token_type(next) != TOKEN_IDENT || expand_one_symbol(C, list))
			list = &next->next;
	}
}

static void preprocessor_line(struct dmr_C *C, struct stream *stream, struct token **line);

static struct token *collect_arg(struct dmr_C *C, struct token *prev, int vararg, struct position *pos, int count)
{
	struct stream *stream = C->T->input_streams + prev->pos.stream;
	struct token **p = &prev->next;
	struct token *next;
	int nesting = 0;

	while (!dmrC_eof_token(next = scan_next(C, p))) {
		if (next->pos.newline && dmrC_match_op(next, '#')) {
			if (!next->pos.noexpand) {
				dmrC_sparse_error(C, next->pos,
					     "directive in argument list");
				preprocessor_line(C, stream, p);
				dmrC_allocator_free(&C->token_allocator, next);	/* Free the '#' token */
				continue;
			}
		}
		switch (dmrC_token_type(next)) {
		case TOKEN_STREAMEND:
		case TOKEN_STREAMBEGIN:
			*p = &dmrC_eof_token_entry_;
			return next;
		case TOKEN_STRING:
		case TOKEN_WIDE_STRING:
			if (count > 1)
				next->string->immutable = 1;
			break;
		}
		if (C->false_nesting) {
			*p = next->next;
			dmrC_allocator_free(&C->token_allocator, next);
			continue;
		}
		if (dmrC_match_op(next, '(')) {
			nesting++;
		} else if (dmrC_match_op(next, ')')) {
			if (!nesting--)
				break;
		} else if (dmrC_match_op(next, ',') && !nesting && !vararg) {
			break;
		}
		next->pos.stream = pos->stream;
		next->pos.line = pos->line;
		next->pos.pos = pos->pos;
		p = &next->next;
	}
	*p = &dmrC_eof_token_entry_;
	return next;
}

/*
 * We store arglist as <counter> [arg1] <number of uses for arg1> ... eof
 */

struct arg {
	struct token *argument;
	struct token *expanded;
	struct token *str;
	int n_normal;
	int n_quoted;
	int n_str;
};

static int collect_arguments(struct dmr_C *C, struct token *start, struct token *arglist, struct arg *args, struct token *what)
{
	int wanted = arglist->count.normal;
	struct token *next = NULL;
	int count = 0;

	arglist = arglist->next;	/* skip counter */

	if (!wanted) {
		next = collect_arg(C, start, 0, &what->pos, 0);
		if (dmrC_eof_token(next))
			goto Eclosing;
		if (!dmrC_eof_token(start->next) || !dmrC_match_op(next, ')')) {
			count++;
			goto Emany;
		}
	} else {
		for (count = 0; count < wanted; count++) {
			struct argcount *p = &arglist->next->count;
			next = collect_arg(C, start, p->vararg, &what->pos, p->normal);
			arglist = arglist->next->next;
			if (dmrC_eof_token(next))
				goto Eclosing;
			args[count].argument = start->next;
			args[count].n_normal = p->normal;
			args[count].n_quoted = p->quoted;
			args[count].n_str = p->str;
			if (dmrC_match_op(next, ')')) {
				count++;
				break;
			}
			start = next;
		}
		if (count == wanted && !dmrC_match_op(next, ')'))
			goto Emany;
		if (count == wanted - 1) {
			struct argcount *p = &arglist->next->count;
			if (!p->vararg)
				goto Efew;
			args[count].argument = NULL;
			args[count].n_normal = p->normal;
			args[count].n_quoted = p->quoted;
			args[count].n_str = p->str;
		}
		if (count < wanted - 1)
			goto Efew;
	}
	what->next = next->next;
	return 1;

Efew:
	dmrC_sparse_error(C, what->pos, "macro \"%s\" requires %d arguments, but only %d given",
		dmrC_show_token(C, what), wanted, count);
	goto out;
Emany:
	while (dmrC_match_op(next, ',')) {
		next = collect_arg(C, next, 0, &what->pos, 0);
		count++;
	}
	if (dmrC_eof_token(next))
		goto Eclosing;
	dmrC_sparse_error(C, what->pos, "macro \"%s\" passed %d arguments, but takes just %d",
		dmrC_show_token(C, what), count, wanted);
	goto out;
Eclosing:
	dmrC_sparse_error(C, what->pos, "unterminated argument list invoking macro \"%s\"",
		dmrC_show_token(C, what));
out:
	what->next = next->next;
	return 0;
}

static struct token *dup_list(struct dmr_C *C, struct token *list)
{
	struct token *res = NULL;
	struct token **p = &res;

	while (!dmrC_eof_token(list)) {
		struct token *newtok = (struct token *)dmrC_allocator_allocate(&C->token_allocator, 0);
		*newtok = *list;
		*p = newtok;
		p = &newtok->next;
		list = list->next;
	}
	return res;
}

static const char *show_token_sequence(struct dmr_C *C, struct token *token, int quote)
{
	char *ptr = C->preprocessor_buffer;
	int whitespace = 0;

	if (!token && !quote)
		return "<none>";
	while (!dmrC_eof_token(token)) {
		const char *val = quote ? dmrC_quote_token(C, token) : dmrC_show_token(C, token);
		int len = (int) strlen(val);

		if (ptr + whitespace + len >= C->preprocessor_buffer + sizeof C->preprocessor_buffer) {
			dmrC_sparse_error(C, token->pos, "too long token expansion");
			break;
		}

		if (whitespace)
			*ptr++ = ' ';
		memcpy(ptr, val, len);
		ptr += len;
		token = token->next;
		whitespace = token->pos.whitespace;
	}
	*ptr = 0;
	return C->preprocessor_buffer;
}

static struct token *stringify(struct dmr_C *C, struct token *arg)
{
	const char *s = show_token_sequence(C, arg, 1);
	int size = (int) strlen(s)+1;
	struct token *token = (struct token *)dmrC_allocator_allocate(&C->token_allocator, 0);
	struct string *string = (struct string *)dmrC_allocator_allocate(&C->string_allocator, size);

	memcpy(string->data, s, size);
	string->length = size;
	token->pos = arg->pos;
	dmrC_token_type(token) = TOKEN_STRING;
	token->string = string;
	token->next = &dmrC_eof_token_entry_;
	return token;
}

static void expand_arguments(struct dmr_C *C, int count, struct arg *args)
{
	int i;
	for (i = 0; i < count; i++) {
		struct token *arg = args[i].argument;
		if (!arg)
			arg = &dmrC_eof_token_entry_;
		if (args[i].n_str)
			args[i].str = stringify(C, arg);
		if (args[i].n_normal) {
			if (!args[i].n_quoted) {
				args[i].expanded = arg;
				args[i].argument = NULL;
			} else if (dmrC_eof_token(arg)) {
				args[i].expanded = arg;
			} else {
				args[i].expanded = dup_list(C, arg);
			}
			expand_list(C, &args[i].expanded);
		}
	}
}

/*
 * Possibly valid combinations:
 *  - ident + ident -> ident
 *  - ident + number -> ident unless number contains '.', '+' or '-'.
 *  - 'L' + char constant -> wide char constant
 *  - 'L' + string literal -> wide string literal
 *  - number + number -> number
 *  - number + ident -> number
 *  - number + '.' -> number
 *  - number + '+' or '-' -> number, if number used to end on [eEpP].
 *  - '.' + number -> number, if number used to start with a digit.
 *  - special + special -> either special or an error.
 */
static enum e_token_type combine(struct dmr_C *C, struct token *left, struct token *right, char *p)
{
	int len;
	enum e_token_type t1 = (enum e_token_type) dmrC_token_type(left), t2 = (enum e_token_type) dmrC_token_type(right);

	if (t1 != TOKEN_IDENT && t1 != TOKEN_NUMBER && t1 != TOKEN_SPECIAL)
		return TOKEN_ERROR;

	if (t1 == TOKEN_IDENT && left->ident == C->S->L_ident) {
		if (t2 >= TOKEN_CHAR && t2 < TOKEN_WIDE_CHAR)
			return (enum e_token_type) (t2 + TOKEN_WIDE_CHAR - TOKEN_CHAR);
		if (t2 == TOKEN_STRING)
			return TOKEN_WIDE_STRING;
	}

	if (t2 != TOKEN_IDENT && t2 != TOKEN_NUMBER && t2 != TOKEN_SPECIAL)
		return TOKEN_ERROR;

	strcpy(p, dmrC_show_token(C, left));
	strcat(p, dmrC_show_token(C, right));
	len = (int) strlen(p);

	if (len >= 256)
		return TOKEN_ERROR;

	if (t1 == TOKEN_IDENT) {
		if (t2 == TOKEN_SPECIAL)
			return TOKEN_ERROR;
		if (t2 == TOKEN_NUMBER && strpbrk(p, "+-."))
			return TOKEN_ERROR;
		return TOKEN_IDENT;
	}

	if (t1 == TOKEN_NUMBER) {
		if (t2 == TOKEN_SPECIAL) {
			switch (right->special) {
			case '.':
				break;
			case '+': case '-':
				if (strchr("eEpP", p[len - 2]))
					break;
			default:
				return TOKEN_ERROR;
			}
		}
		return TOKEN_NUMBER;
	}

	if (p[0] == '.' && isdigit((unsigned char)p[1]))
		return TOKEN_NUMBER;

	return TOKEN_SPECIAL;
}

static int merge(struct dmr_C *C, struct token *left, struct token *right)
{
	enum e_token_type res = combine(C, left, right, C->preprocessor_mergebuffer);
	int n;

	switch (res) {
	case TOKEN_IDENT:
		left->ident = dmrC_built_in_ident(C, C->preprocessor_mergebuffer);
		left->pos.noexpand = 0;
		return 1;

	case TOKEN_NUMBER: {
		char *number = (char *) dmrC_allocator_allocate(&C->byte_allocator, strlen(C->preprocessor_mergebuffer) + 1);
		memcpy(number, C->preprocessor_mergebuffer, strlen(C->preprocessor_mergebuffer) + 1);
		dmrC_token_type(left) = TOKEN_NUMBER;	/* could be . + num */
		left->number = number;
		return 1;
	}

	case TOKEN_SPECIAL:
		if (C->preprocessor_mergebuffer[2] && C->preprocessor_mergebuffer[3])
			break;
		for (n = SPECIAL_BASE; n < SPECIAL_ARG_SEPARATOR; n++) {
			if (!memcmp(C->preprocessor_mergebuffer, dmrC_combinations_[n-SPECIAL_BASE], 3)) {
				left->special = n;
				return 1;
			}
		}
		break;

	case TOKEN_WIDE_CHAR:
	case TOKEN_WIDE_STRING:
		dmrC_token_type(left) = res;
		left->pos.noexpand = 0;
		left->string = right->string;
		return 1;

	case TOKEN_WIDE_CHAR_EMBEDDED_0:
	case TOKEN_WIDE_CHAR_EMBEDDED_1:
	case TOKEN_WIDE_CHAR_EMBEDDED_2:
	case TOKEN_WIDE_CHAR_EMBEDDED_3:
		dmrC_token_type(left) = res;
		left->pos.noexpand = 0;
		memcpy(left->embedded, right->embedded, 4);
		return 1;

	default:
		;
	}
	dmrC_sparse_error(C, left->pos, "'##' failed: concatenation is not a valid token");
	return 0;
}

static struct token *dup_token(struct dmr_C *C, struct token *token, struct position *streampos)
{
	struct token *alloc = alloc_token(C, streampos);
	dmrC_token_type(alloc) = dmrC_token_type(token);
	alloc->pos.newline = token->pos.newline;
	alloc->pos.whitespace = token->pos.whitespace;
	alloc->number = token->number;
	alloc->pos.noexpand = token->pos.noexpand;
	return alloc;	
}

static struct token **copy(struct dmr_C *C, struct token **where, struct token *list, int *count)
{
	int need_copy = --*count;
	while (!dmrC_eof_token(list)) {
		struct token *token;
		if (need_copy)
			token = dup_token(C, list, &list->pos);
		else
			token = list;
		if (dmrC_token_type(token) == TOKEN_IDENT && token->ident->tainted)
			token->pos.noexpand = 1;
		*where = token;
		where = &token->next;
		list = list->next;
	}
	*where = &dmrC_eof_token_entry_;
	return where;
}

static int handle_kludge(struct dmr_C *C, struct token **p, struct arg *args)
{
	struct token *t = (*p)->next->next;
	(void) C;
	while (1) {
		struct arg *v = &args[t->argnum];
		if (dmrC_token_type(t->next) != TOKEN_CONCAT) {
			if (v->argument) {
				/* ignore the first ## */
				*p = (*p)->next;
				return 0;
			}
			/* skip the entire thing */
			*p = t;
			return 1;
		}
		if (v->argument && !dmrC_eof_token(v->argument))
			return 0; /* no magic */
		t = t->next->next;
	}
}

static struct token **substitute(struct dmr_C *C, struct token **list, struct token *body, struct arg *args)
{
	struct position *base_pos = &(*list)->pos;
	int *count;
	enum {Normal, Placeholder, Concat} state = Normal;

	for (; !dmrC_eof_token(body); body = body->next) {
		struct token *added, *arg;
		struct token **tail;
		struct token *t;

		switch (dmrC_token_type(body)) {
		case TOKEN_GNU_KLUDGE:
			/*
			 * GNU kludge: if we had <comma>##<vararg>, behaviour
			 * depends on whether we had enough arguments to have
			 * a vararg.  If we did, ## is just ignored.  Otherwise
			 * both , and ## are ignored.  Worse, there can be
			 * an arbitrary number of ##<arg> in between; if all of
			 * those are empty, we act as if they hadn't been there,
			 * otherwise we act as if the kludge didn't exist.
			 */
			t = body;
			if (handle_kludge(C, &body, args)) {
				if (state == Concat)
					state = Normal;
				else
					state = Placeholder;
				continue;
			}
			added = dup_token(C, t, base_pos);
			dmrC_token_type(added) = TOKEN_SPECIAL;
			tail = &added->next;
			break;

		case TOKEN_STR_ARGUMENT:
			arg = args[body->argnum].str;
			count = &args[body->argnum].n_str;
			goto copy_arg;

		case TOKEN_QUOTED_ARGUMENT:
			arg = args[body->argnum].argument;
			count = &args[body->argnum].n_quoted;
			if (!arg || dmrC_eof_token(arg)) {
				if (state == Concat)
					state = Normal;
				else
					state = Placeholder;
				continue;
			}
			goto copy_arg;

		case TOKEN_MACRO_ARGUMENT:
			arg = args[body->argnum].expanded;
			count = &args[body->argnum].n_normal;
			if (dmrC_eof_token(arg)) {
				state = Normal;
				continue;
			}
		copy_arg:
			tail = copy(C, &added, arg, count);
			added->pos.newline = body->pos.newline;
			added->pos.whitespace = body->pos.whitespace;
			break;

		case TOKEN_CONCAT:
			if (state == Placeholder)
				state = Normal;
			else
				state = Concat;
			continue;

		case TOKEN_IDENT:
			added = dup_token(C, body, base_pos);
			if (added->ident->tainted)
				added->pos.noexpand = 1;
			tail = &added->next;
			break;

		default:
			added = dup_token(C, body, base_pos);
			tail = &added->next;
			break;
		}

		/*
		 * if we got to doing real concatenation, we already have
		 * added something into the list, so dmrC_containing_token() is OK.
		 */
		if (state == Concat && merge(C, dmrC_containing_token(list), added)) {
			*list = added->next;
			if (tail != &added->next)
				list = tail;
		} else {
			*list = added;
			list = tail;
		}
		state = Normal;
	}
	*list = &dmrC_eof_token_entry_;
	return list;
}

static int expand(struct dmr_C *C, struct token **list, struct symbol *sym)
{
	struct token *last;
	struct token *token = *list;
	struct ident *expanding = token->ident;
	struct token **tail;
	int nargs = sym->arglist ? sym->arglist->count.normal : 0;
#ifndef _MSC_VER
	struct arg args[nargs];
#else
	struct arg *args = (struct arg *)alloca(sizeof(struct arg)*nargs);
#endif

	if (expanding->tainted) {
		token->pos.noexpand = 1;
		return 1;
	}

	if (sym->arglist) {
		if (!dmrC_match_op(scan_next(C, &token->next), '('))
			return 1;
		if (!collect_arguments(C, token->next, sym->arglist, args, token))
			return 1;
		expand_arguments(C, nargs, args);
	}

	expanding->tainted = 1;

	last = token->next;
	tail = substitute(C, list, sym->expansion, args);
	/*
	 * Note that it won't be eof - at least TOKEN_UNTAINT will be there.
	 * We still can lose the newline flag if the sucker expands to nothing,
	 * but the price of dealing with that is probably too high (we'd need
	 * to collect the flags during scan_next())
	 */
	(*list)->pos.newline = token->pos.newline;
	(*list)->pos.whitespace = token->pos.whitespace;
	*tail = last;

	return 0;
}

static const char *token_name_sequence(struct dmr_C *C, struct token *token, int endop, struct token *start)
{
	char *ptr = C->preprocessor_tokenseqbuffer;

	while (!dmrC_eof_token(token) && !dmrC_match_op(token, endop)) {
		int len;
		const char *val = token->string->data;
		if (dmrC_token_type(token) != TOKEN_STRING)
			val = dmrC_show_token(C, token);
		len = (int) strlen(val);
		memcpy(ptr, val, len);
		ptr += len;
		token = token->next;
	}
	*ptr = 0;
	if (endop && !dmrC_match_op(token, endop))
		dmrC_sparse_error(C, start->pos, "expected '>' at end of filename");
	return C->preprocessor_tokenseqbuffer;
}

static int already_tokenized(struct dmr_C *C, const char *path)
{
	int stream, next;

	for (stream = *dmrC_hash_stream(path); stream >= 0 ; stream = next) {
		struct stream *s = C->T->input_streams + stream;

		next = s->next_stream;
		if (s->once) {
			if (strcmp(path, s->name))
				continue;
			return 1;
		}
		if (s->constant != CONSTANT_FILE_YES)
			continue;
		if (strcmp(path, s->name))
			continue;
		if (s->protect && !lookup_macro(C, s->protect))
			continue;
		return 1;
	}
	return 0;
}

/* Handle include of header files.
 * The relevant options are made compatible with gcc. The only options that
 * are not supported is -withprefix and friends.
 *
 * Three set of include paths are known:
 * quote_includepath:	Path to search when using #include "file.h"
 * angle_includepath:	Paths to search when using #include <file.h>
 * isys_includepath:	Paths specified with -isystem, come before the
 *			built-in system include paths. Gcc would suppress
 *			warnings from system headers. Here we separate
 *			them from the angle_ ones to keep search ordering.
 *
 * sys_includepath:	Built-in include paths.
 * dirafter_includepath Paths added with -dirafter.
 *
 * The above is implemented as one array with pointers
 *                         +--------------+
 * quote_includepath --->  |              |
 *                         +--------------+
 *                         |              |
 *                         +--------------+
 * angle_includepath --->  |              |
 *                         +--------------+
 * isys_includepath  --->  |              |
 *                         +--------------+
 * sys_includepath   --->  |              |
 *                         +--------------+
 * dirafter_includepath -> |              |
 *                         +--------------+
 *
 * -I dir insert dir just before isys_includepath and move the rest
 * -I- makes all dirs specified with -I before to quote dirs only and
 *   angle_includepath is set equal to isys_includepath.
 * -nostdinc removes all sys dirs by storing NULL in entry pointed
 *   to by * sys_includepath. Note that this will reset all dirs built-in
 *   and added before -nostdinc by -isystem and -idirafter.
 * -isystem dir adds dir where isys_includepath points adding this dir as
 *   first systemdir
 * -idirafter dir adds dir to the end of the list
 */

static void set_stream_include_path(struct dmr_C *C, struct stream *stream)
{
	const char *path = stream->path;
	if (!path) {
		const char *p = strrchr(stream->name, '/');
		path = "";
		if (p) {
			int len = (int)(p - stream->name + 1);
			char *m = (char *)dmrC_allocator_allocate(&C->byte_allocator, len+1);
			/* This includes the final "/" */
			memcpy(m, stream->name, len);
			m[len] = 0;
			path = m;
		}
		stream->path = path;
	}
	C->includepath[0] = path;
}

static int try_include(struct dmr_C *C, const char *path, const char *filename, int flen, struct token **where, const char **next_path)
{
	int fd;
	int plen = (int) strlen(path);

	memcpy(C->fullname, path, plen);
	if (plen && path[plen-1] != '/') {
		C->fullname[plen] = '/';
		plen++;
	}
	/* flen includes extra 0 byte */
	memcpy(C->fullname+plen, filename, flen);
	if (already_tokenized(C, C->fullname))
		return 1;
	//printf("Opening %s\n", C->fullname);
	fd = open(C->fullname, O_RDONLY);
	if (fd >= 0) {
		char * streamname = (char *)dmrC_allocator_allocate(&C->byte_allocator, plen + flen);
		memcpy(streamname, C->fullname, plen + flen);
		*where = dmrC_tokenize(C, streamname, fd, *where, next_path);
		close(fd);
		return 1;
	}
	return 0;
}

static int do_include_path(struct dmr_C *C, const char **pptr, struct token **list, struct token *token, const char *filename, int flen)
{
	const char *path;
  
	(void) token;
	while ((path = *pptr++) != NULL) {
		if (!try_include(C, path, filename, flen, list, pptr))
			continue;
		return 1;
	}
	return 0;
}

static int free_preprocessor_line(struct dmr_C *C, struct token *token)
{
	while (dmrC_token_type(token) != TOKEN_EOF) {
		struct token *free = token;
		token = token->next;
		dmrC_allocator_free(&C->token_allocator, free);
	};
	return 1;
}

static int handle_include_path(struct dmr_C *C, struct stream *stream, struct token **list, struct token *token, int how)
{
	const char *filename;
	struct token *next;
	const char **path;
	int expect;
	int flen;

	next = token->next;
	expect = '>';
	if (!dmrC_match_op(next, '<')) {
		expand_list(C, &token->next);
		expect = 0;
		next = token;
		if (dmrC_match_op(token->next, '<')) {
			next = token->next;
			expect = '>';
		}
	}

	token = next->next;
	filename = token_name_sequence(C, token, expect, token);
	flen = (int)strlen(filename) + 1;

	/* Absolute path? */
	if (filename[0] == '/') {
		if (try_include(C, "", filename, flen, list, C->includepath))
			return 0;
		goto out;
	}

	switch (how) {
	case 1:
		path = stream->next_path;
		break;
	case 2:
		C->includepath[0] = "";
		path = C->includepath;
		break;
	default:
		/* Dir of input file is first dir to search for quoted includes */
		set_stream_include_path(C, stream);
		path = expect ? C->angle_includepath : C->quote_includepath;
		break;
	}
	/* Check the standard include paths.. */
	if (do_include_path(C, path, list, token, filename, flen))
		return 0;
out:
	dmrC_error_die(C, token->pos, "unable to open '%s'", filename);
	return 0;
}

static int handle_include(struct dmr_C *C, struct stream *stream, struct token **list, struct token *token)
{
	return handle_include_path(C, stream, list, token, 0);
}

static int handle_include_next(struct dmr_C *C, struct stream *stream, struct token **list, struct token *token)
{
	return handle_include_path(C, stream, list, token, 1);
}

static int handle_argv_include(struct dmr_C *C, struct stream *stream, struct token **list, struct token *token)
{
	return handle_include_path(C, stream, list, token, 2);
}

static int token_different(struct dmr_C *C, struct token *t1, struct token *t2)
{
	int different;

	(void) C;
	if (dmrC_token_type(t1) != dmrC_token_type(t2))
		return 1;

	switch (dmrC_token_type(t1)) {
	case TOKEN_IDENT:
		different = t1->ident != t2->ident;
		break;
	case TOKEN_ARG_COUNT:
	case TOKEN_UNTAINT:
	case TOKEN_CONCAT:
	case TOKEN_GNU_KLUDGE:
		different = 0;
		break;
	case TOKEN_NUMBER:
		different = strcmp(t1->number, t2->number);
		break;
	case TOKEN_SPECIAL:
		different = t1->special != t2->special;
		break;
	case TOKEN_MACRO_ARGUMENT:
	case TOKEN_QUOTED_ARGUMENT:
	case TOKEN_STR_ARGUMENT:
		different = t1->argnum != t2->argnum;
		break;
	case TOKEN_CHAR_EMBEDDED_0:
	case TOKEN_CHAR_EMBEDDED_1:
	case TOKEN_CHAR_EMBEDDED_2:
	case TOKEN_CHAR_EMBEDDED_3:
	case TOKEN_WIDE_CHAR_EMBEDDED_0:
	case TOKEN_WIDE_CHAR_EMBEDDED_1:
	case TOKEN_WIDE_CHAR_EMBEDDED_2:
	case TOKEN_WIDE_CHAR_EMBEDDED_3:
		different = memcmp(t1->embedded, t2->embedded, 4);
		break;
	case TOKEN_CHAR:
	case TOKEN_WIDE_CHAR:
	case TOKEN_STRING:
	case TOKEN_WIDE_STRING: {
		struct string *s1, *s2;

		s1 = t1->string;
		s2 = t2->string;
		different = 1;
		if (s1->length != s2->length)
			break;
		different = memcmp(s1->data, s2->data, s1->length);
		break;
	}
	default:
		different = 1;
		break;
	}
	return different;
}

static int token_list_different(struct dmr_C *C, struct token *list1, struct token *list2)
{
	for (;;) {
		if (list1 == list2)
			return 0;
		if (!list1 || !list2)
			return 1;
		if (token_different(C, list1, list2))
			return 1;
		list1 = list1->next;
		list2 = list2->next;
	}
}

static inline void set_arg_count(struct dmr_C *C, struct token *token)
{
        (void) C;
	dmrC_token_type(token) = TOKEN_ARG_COUNT;
	token->count.normal = token->count.quoted =
	token->count.str = token->count.vararg = 0;
}

static struct token *parse_arguments(struct dmr_C *C, struct token *list)
{
	struct token *arg = list->next, *next = list;
	struct argcount *count = &list->count;

	set_arg_count(C, list);

	if (dmrC_match_op(arg, ')')) {
		next = arg->next;
		list->next = &dmrC_eof_token_entry_;
		return next;
	}

	while (dmrC_token_type(arg) == TOKEN_IDENT) {
		if (arg->ident == C->S->__VA_ARGS___ident)
			goto Eva_args;
		if (!++count->normal)
			goto Eargs;
		next = arg->next;

		if (dmrC_match_op(next, ',')) {
			set_arg_count(C, next);
			arg = next->next;
			continue;
		}

		if (dmrC_match_op(next, ')')) {
			set_arg_count(C, next);
			next = next->next;
			arg->next->next = &dmrC_eof_token_entry_;
			return next;
		}

		/* normal cases are finished here */

		if (dmrC_match_op(next, SPECIAL_ELLIPSIS)) {
			if (dmrC_match_op(next->next, ')')) {
				set_arg_count(C, next);
				next->count.vararg = 1;
				next = next->next;
				arg->next->next = &dmrC_eof_token_entry_;
				return next->next;
			}

			arg = next;
			goto Enotclosed;
		}

		if (dmrC_eof_token(next)) {
			goto Enotclosed;
		} else {
			arg = next;
			goto Ebadstuff;
		}
	}

	if (dmrC_match_op(arg, SPECIAL_ELLIPSIS)) {
		next = arg->next;
		dmrC_token_type(arg) = TOKEN_IDENT;
		arg->ident = C->S->__VA_ARGS___ident;
		if (!dmrC_match_op(next, ')'))
			goto Enotclosed;
		if (!++count->normal)
			goto Eargs;
		set_arg_count(C, next);
		next->count.vararg = 1;
		next = next->next;
		arg->next->next = &dmrC_eof_token_entry_;
		return next;
	}

	if (dmrC_eof_token(arg)) {
		arg = next;
		goto Enotclosed;
	}
	if (dmrC_match_op(arg, ','))
		goto Emissing;
	else
		goto Ebadstuff;


Emissing:
	dmrC_sparse_error(C, arg->pos, "parameter name missing");
	return NULL;
Ebadstuff:
	dmrC_sparse_error(C, arg->pos, "\"%s\" may not appear in macro parameter list",
		dmrC_show_token(C, arg));
	return NULL;
Enotclosed:
	dmrC_sparse_error(C, arg->pos, "missing ')' in macro parameter list");
	return NULL;
Eva_args:
	dmrC_sparse_error(C, arg->pos, "__VA_ARGS__ can only appear in the expansion of a C99 variadic macro");
	return NULL;
Eargs:
	dmrC_sparse_error(C, arg->pos, "too many arguments in macro definition");
	return NULL;
}

static int try_arg(struct dmr_C *C, struct token *token, enum e_token_type type, struct token *arglist)
{
	struct ident *ident = token->ident;
	int nr;
        
        (void) C;

	if (!arglist || dmrC_token_type(token) != TOKEN_IDENT)
		return 0;

	arglist = arglist->next;

	for (nr = 0; !dmrC_eof_token(arglist); nr++, arglist = arglist->next->next) {
		if (arglist->ident == ident) {
			struct argcount *count = &arglist->next->count;
			int n;

			token->argnum = nr;
			dmrC_token_type(token) = type;
			switch (type) {
			case TOKEN_MACRO_ARGUMENT:
				n = ++count->normal;
				break;
			case TOKEN_QUOTED_ARGUMENT:
				n = ++count->quoted;
				break;
			default:
				n = ++count->str;
			}
			if (n)
				return count->vararg ? 2 : 1;
			/*
			 * XXX - need saner handling of that
			 * (>= 1024 instances of argument)
			 */
			dmrC_token_type(token) = TOKEN_ERROR;
			return -1;
		}
	}
	return 0;
}

static struct token *handle_hash(struct dmr_C *C, struct token **p, struct token *arglist)
{
	struct token *token = *p;
	if (arglist) {
		struct token *next = token->next;
		if (!try_arg(C, next, TOKEN_STR_ARGUMENT, arglist))
			goto Equote;
		next->pos.whitespace = token->pos.whitespace;
		dmrC_allocator_free(&C->token_allocator, token);
		token = *p = next;
	} else {
		token->pos.noexpand = 1;
	}
	return token;

Equote:
	dmrC_sparse_error(C, token->pos, "'#' is not followed by a macro parameter");
	return NULL;
}

/* token->next is ## */
static struct token *handle_hashhash(struct dmr_C *C, struct token *token, struct token *arglist)
{
	struct token *last = token;
	struct token *concat;
	int state = dmrC_match_op(token, ',');
	
	try_arg(C, token, TOKEN_QUOTED_ARGUMENT, arglist);

	while (1) {
		struct token *t;
		int is_arg;

		/* eat duplicate ## */
		concat = token->next;
		while (dmrC_match_op(t = concat->next, SPECIAL_HASHHASH)) {
			token->next = t;
			dmrC_allocator_free(&C->token_allocator, concat);
			concat = t;
		}
		dmrC_token_type(concat) = TOKEN_CONCAT;

		if (dmrC_eof_token(t))
			goto Econcat;

		if (dmrC_match_op(t, '#')) {
			t = handle_hash(C, &concat->next, arglist);
			if (!t)
				return NULL;
		}

		is_arg = try_arg(C, t, TOKEN_QUOTED_ARGUMENT, arglist);

		if (state == 1 && is_arg) {
			state = is_arg;
		} else {
			last = t;
			state = dmrC_match_op(t, ',');
		}

		token = t;
		if (!dmrC_match_op(token->next, SPECIAL_HASHHASH))
			break;
	}
	/* handle GNU ,##__VA_ARGS__ kludge, in all its weirdness */
	if (state == 2)
		dmrC_token_type(last) = TOKEN_GNU_KLUDGE;
	return token;

Econcat:
	dmrC_sparse_error(C, concat->pos, "'##' cannot appear at the ends of macro expansion");
	return NULL;
}

static struct token *parse_expansion(struct dmr_C *C, struct token *expansion, struct token *arglist, struct ident *name)
{
	struct token *token = expansion;
	struct token **p;

	if (dmrC_match_op(token, SPECIAL_HASHHASH))
		goto Econcat;

	for (p = &expansion; !dmrC_eof_token(token); p = &token->next, token = *p) {
		if (dmrC_match_op(token, '#')) {
			token = handle_hash(C, p, arglist);
			if (!token)
				return NULL;
		}
		if (dmrC_match_op(token->next, SPECIAL_HASHHASH)) {
			token = handle_hashhash(C, token, arglist);
			if (!token)
				return NULL;
		} else {
			try_arg(C, token, TOKEN_MACRO_ARGUMENT, arglist);
		}
		switch (dmrC_token_type(token)) {
		case TOKEN_ERROR:
			goto Earg;

		case TOKEN_STRING:
		case TOKEN_WIDE_STRING:
			token->string->immutable = 1;
			break;
		}
	}
	token = alloc_token(C, &expansion->pos);
	dmrC_token_type(token) = TOKEN_UNTAINT;
	token->ident = name;
	token->next = *p;
	*p = token;
	return expansion;

Econcat:
	dmrC_sparse_error(C, token->pos, "'##' cannot appear at the ends of macro expansion");
	return NULL;
Earg:
	dmrC_sparse_error(C, token->pos, "too many instances of argument in body");
	return NULL;
}

static int do_handle_define(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token, int attr)
{
	struct token *arglist, *expansion;
	struct token *left = token->next;
	struct symbol *sym;
	struct ident *name;
	int ret;

        (void) stream;
        (void) line;
        
	if (dmrC_token_type(left) != TOKEN_IDENT) {
		dmrC_sparse_error(C, token->pos, "expected identifier to 'define'");
		return 1;
	}

	name = left->ident;

	arglist = NULL;
	expansion = left->next;
	if (!expansion->pos.whitespace) {
		if (dmrC_match_op(expansion, '(')) {
			arglist = expansion;
			expansion = parse_arguments(C, expansion);
			if (!expansion)
				return 1;
		} else if (!dmrC_eof_token(expansion)) {
			dmrC_warning(C, expansion->pos,
				"no whitespace before object-like macro body");
		}
	}

	expansion = parse_expansion(C, expansion, arglist, name);
	if (!expansion)
		return 1;

	ret = 1;
	sym = dmrC_lookup_symbol(name, (enum namespace_type) (NS_MACRO | NS_UNDEF));
	if (sym) {
		int clean;

		if (attr < sym->attr)
			goto out;

		clean = (attr == sym->attr && sym->ns == NS_MACRO);

		if (token_list_different(C, sym->expansion, expansion) ||
		    token_list_different(C, sym->arglist, arglist)) {
			ret = 0;
			if ((clean && attr == SYM_ATTR_NORMAL)
					|| sym->used_in == C->file_scope) {
				dmrC_warning(C, left->pos, "preprocessor token %.*s redefined",
						name->len, name->name);
				dmrC_info(C, sym->pos, "this was the original definition");
			}
		} else if (clean)
			goto out;
	}

	if (!sym || sym->scope != C->file_scope) {
		sym = dmrC_alloc_symbol(C->S, left->pos, SYM_NODE);
		dmrC_bind_symbol(C->S, sym, name, NS_MACRO);
		dmrC_add_ident(C, &C->macros, name);
		ret = 0;
	}

	if (!ret) {
		sym->expansion = expansion;
		sym->arglist = arglist;
		dmrC_allocator_free(&C->token_allocator, token);	/* Free the "define" token, but not the rest of the line */
	}

	sym->ns = NS_MACRO;
	sym->used_in = NULL;
	sym->attr = attr;
out:
	return ret;
}

static int handle_define(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	return do_handle_define(C, stream, line, token, SYM_ATTR_NORMAL);
}

static int handle_weak_define(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	return do_handle_define(C, stream, line, token, SYM_ATTR_WEAK);
}

static int handle_strong_define(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	return do_handle_define(C, stream, line, token, SYM_ATTR_STRONG);
}

static int do_handle_undef(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token, int attr)
{
	struct token *left = token->next;
	struct symbol *sym;

        (void) stream;
        (void) line;
        
	if (dmrC_token_type(left) != TOKEN_IDENT) {
		dmrC_sparse_error(C, token->pos, "expected identifier to 'undef'");
		return 1;
	}

	sym = dmrC_lookup_symbol(left->ident, (enum namespace_type) (NS_MACRO | NS_UNDEF));
	if (sym) {
		if (attr < sym->attr)
			return 1;
		if (attr == sym->attr && sym->ns == NS_UNDEF)
			return 1;
	} else if (attr <= SYM_ATTR_NORMAL)
		return 1;

	if (!sym || sym->scope != C->file_scope) {
		sym = dmrC_alloc_symbol(C->S, left->pos, SYM_NODE);
		dmrC_bind_symbol(C->S, sym, left->ident, NS_MACRO);
	}

	sym->ns = NS_UNDEF;
	sym->used_in = NULL;
	sym->attr = attr;

	return 1;
}

static int handle_undef(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	return do_handle_undef(C, stream, line, token, SYM_ATTR_NORMAL);
}

static int handle_strong_undef(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	return do_handle_undef(C, stream, line, token, SYM_ATTR_STRONG);
}

static int preprocessor_if(struct dmr_C *C, struct stream *stream, struct token *token, int true_value)
{
	dmrC_token_type(token) = C->false_nesting ? TOKEN_SKIP_GROUPS : TOKEN_IF;
	free_preprocessor_line(C, token->next);
	token->next = stream->top_if;
	stream->top_if = token;
	if (C->false_nesting || true_value != 1)
		C->false_nesting++;
	return 0;
}

static int handle_ifdef(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	struct token *next = token->next;
	int arg;
        
        (void) line;
	if (dmrC_token_type(next) == TOKEN_IDENT) {
		arg = token_defined(C, next);
	} else {
		dirty_stream(stream);
		if (!C->false_nesting)
			dmrC_sparse_error(C, token->pos, "expected preprocessor identifier");
		arg = -1;
	}
	return preprocessor_if(C, stream, token, arg);
}

static int handle_ifndef(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	struct token *next = token->next;
	int arg;
        
        (void) line;
	if (dmrC_token_type(next) == TOKEN_IDENT) {
		if (!stream->dirty && !stream->ifndef) {
			if (!stream->protect) {
				stream->ifndef = token;
				stream->protect = next->ident;
			} else if (stream->protect == next->ident) {
				stream->ifndef = token;
				stream->dirty = 1;
			}
		}
		arg = !token_defined(C, next);
	} else {
		dirty_stream(stream);
		if (!C->false_nesting)
			dmrC_sparse_error(C, token->pos, "expected preprocessor identifier");
		arg = -1;
	}

	return preprocessor_if(C, stream, token, arg);
}

static const char *show_token_sequence(struct dmr_C *C, struct token *token, int quote);

/*
 * Expression handling for #if and #elif; it differs from normal expansion
 * due to special treatment of "defined".
 */
static int expression_value(struct dmr_C *C, struct token **where)
{
	struct expression *expr;
	struct token *p;
	struct token **list = where, **beginning = NULL;
	long long value;
	int state = 0;

	while (!dmrC_eof_token(p = scan_next(C, list))) {
		switch (state) {
		case 0:
			if (dmrC_token_type(p) != TOKEN_IDENT)
				break;
			if (p->ident == C->S->defined_ident) {
				state = 1;
				beginning = list;
				break;
			}
			if (!expand_one_symbol(C, list))
				continue;
			if (dmrC_token_type(p) != TOKEN_IDENT)
				break;
			dmrC_token_type(p) = TOKEN_ZERO_IDENT;
			break;
		case 1:
			if (dmrC_match_op(p, '(')) {
				state = 2;
			} else {
				state = 0;
				replace_with_defined(C, p);
				*beginning = p;
			}
			break;
		case 2:
			if (dmrC_token_type(p) == TOKEN_IDENT)
				state = 3;
			else
				state = 0;
			replace_with_defined(C, p);
			*beginning = p;
			break;
		case 3:
			state = 0;
			if (!dmrC_match_op(p, ')'))
				dmrC_sparse_error(C, p->pos, "missing ')' after \"defined\"");
			*list = p->next;
			continue;
		}
		list = &p->next;
	}

	p = dmrC_constant_expression(C, *where, &expr);
	if (!dmrC_eof_token(p))
		dmrC_sparse_error(C, p->pos, "garbage at end: %s", show_token_sequence(C, p, 0));
	value = dmrC_get_expression_value(C, expr);
	return value != 0;
}

static int handle_if(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	int value = 0;
        
        (void)line;
	if (!C->false_nesting)
		value = expression_value(C, &token->next);

	dirty_stream(stream);
	return preprocessor_if(C, stream, token, value);
}

static int handle_elif(struct dmr_C *C, struct stream * stream, struct token **line, struct token *token)
{
	struct token *top_if = stream->top_if;
        
        (void) line;
	end_group(stream);

	if (!top_if) {
		nesting_error(stream);
		dmrC_sparse_error(C, token->pos, "unmatched #elif within stream");
		return 1;
	}

	if (dmrC_token_type(top_if) == TOKEN_ELSE) {
		nesting_error(stream);
		dmrC_sparse_error(C, token->pos, "#elif after #else");
		if (!C->false_nesting)
			C->false_nesting = 1;
		return 1;
	}

	dirty_stream(stream);
	if (dmrC_token_type(top_if) != TOKEN_IF)
		return 1;
	if (C->false_nesting) {
		C->false_nesting = 0;
		if (!expression_value(C, &token->next))
			C->false_nesting = 1;
	} else {
		C->false_nesting = 1;
		dmrC_token_type(top_if) = TOKEN_SKIP_GROUPS;
	}
	return 1;
}

static int handle_else(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	struct token *top_if = stream->top_if;
        
        (void) line;
	end_group(stream);

	if (!top_if) {
		nesting_error(stream);
		dmrC_sparse_error(C, token->pos, "unmatched #else within stream");
		return 1;
	}

	if (dmrC_token_type(top_if) == TOKEN_ELSE) {
		nesting_error(stream);
		dmrC_sparse_error(C, token->pos, "#else after #else");
	}
	if (C->false_nesting) {
		if (dmrC_token_type(top_if) == TOKEN_IF)
			C->false_nesting = 0;
	} else {
		C->false_nesting = 1;
	}
	dmrC_token_type(top_if) = TOKEN_ELSE;
	return 1;
}

static int handle_endif(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	struct token *top_if = stream->top_if;
        
        (void) line;
	end_group(stream);
	if (!top_if) {
		nesting_error(stream);
		dmrC_sparse_error(C, token->pos, "unmatched #endif in stream");
		return 1;
	}
	if (C->false_nesting)
		C->false_nesting--;
	stream->top_if = top_if->next;
	dmrC_allocator_free(&C->token_allocator, top_if);
	return 1;
}

static int handle_warning(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
        (void) line;
        (void) stream;
	dmrC_warning(C, token->pos, "%s", show_token_sequence(C, token->next, 0));
	return 1;
}

static int handle_error(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
        (void) line;
        (void) stream;
        
	dmrC_sparse_error(C, token->pos, "%s", show_token_sequence(C, token->next, 0));
	return 1;
}

static int handle_nostdinc(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
        (void) token;
        (void) line;
        (void) stream;
	/*
	 * Do we have any non-system includes?
	 * Clear them out if so..
	 */
	*C->sys_includepath = NULL;
	return 1;
}

static inline void update_inc_ptrs(struct dmr_C *C, const char ***where)
{

	if (*where <= C->dirafter_includepath) {
		C->dirafter_includepath++;
		/* If this was the entry that we prepend, don't
		 * rise the lower entries, even if they are at
		 * the same level. */
		if (where == &C->dirafter_includepath)
			return;
	}
	if (*where <= C->sys_includepath) {
		C->sys_includepath++;
		if (where == &C->sys_includepath)
			return;
	}
	if (*where <= C->isys_includepath) {
		C->isys_includepath++;
		if (where == &C->isys_includepath)
			return;
	}

	/* angle_includepath is actually never updated, since we
	 * don't suppport -iquote rught now. May change some day. */
	if (*where <= C->angle_includepath) {
		C->angle_includepath++;
		if (where == &C->angle_includepath)
			return;
	}
}

/* Add a path before 'where' and update the pointers associated with the
 * includepath array */
static void add_path_entry(struct dmr_C *C, struct token *token, const char *path,
	const char ***where)
{
	const char **dst;
	const char *next;

	/* Need one free entry.. */
	if (C->includepath[INCLUDEPATHS-2])
		dmrC_error_die(C, token->pos, "too many include path entries");

	/* check that this is not a duplicate */
	dst = C->includepath;
	while (*dst) {
		if (strcmp(*dst, path) == 0)
			return;
		dst++;
	}
	next = path;
	dst = *where;

	update_inc_ptrs(C, where);

	/*
	 * Move them all up starting at dst,
	 * insert the new entry..
	 */
	do {
		const char *tmp = *dst;
		*dst = next;
		next = tmp;
		dst++;
	} while (next);
}

static int handle_add_include(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
        (void) stream;
        (void) line;
	for (;;) {
		token = token->next;
		if (dmrC_eof_token(token))
			return 1;
		if (dmrC_token_type(token) != TOKEN_STRING) {
			dmrC_warning(C, token->pos, "expected path string");
			return 1;
		}
		add_path_entry(C, token, token->string->data, &C->isys_includepath);
	}
}

static int handle_add_isystem(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
        (void)line;
        (void)stream;
	for (;;) {
		token = token->next;
		if (dmrC_eof_token(token))
			return 1;
		if (dmrC_token_type(token) != TOKEN_STRING) {
			dmrC_sparse_error(C, token->pos, "expected path string");
			return 1;
		}
		add_path_entry(C, token, token->string->data, &C->sys_includepath);
	}
}

static int handle_add_system(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
        (void) line;
        (void) stream;
	for (;;) {
		token = token->next;
		if (dmrC_eof_token(token))
			return 1;
		if (dmrC_token_type(token) != TOKEN_STRING) {
			dmrC_sparse_error(C, token->pos, "expected path string");
			return 1;
		}
		add_path_entry(C, token, token->string->data, &C->dirafter_includepath);
	}
}

/* Add to end on includepath list - no pointer updates */
static void add_dirafter_entry(struct dmr_C *C, struct token *token, const char *path)
{
	const char **dst = C->includepath;

	/* Need one free entry.. */
	if (C->includepath[INCLUDEPATHS-2])
		dmrC_error_die(C, token->pos, "too many include path entries");

	/* Add to the end */
	while (*dst)
		dst++;
	*dst = path;
	dst++;
	*dst = NULL;
}

static int handle_add_dirafter(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
        (void) line;
        (void) stream;
	for (;;) {
		token = token->next;
		if (dmrC_eof_token(token))
			return 1;
		if (dmrC_token_type(token) != TOKEN_STRING) {
			dmrC_sparse_error(C, token->pos, "expected path string");
			return 1;
		}
		add_dirafter_entry(C, token, token->string->data);
	}
}

static int handle_split_include(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
        (void) stream;
        (void) line;
        (void) token;
	/*
	 * -I-
	 *  From info gcc:
	 *  Split the include path.  Any directories specified with `-I'
	 *  options before `-I-' are searched only for headers requested with
	 *  `#include "FILE"'; they are not searched for `#include <FILE>'.
	 *  If additional directories are specified with `-I' options after
	 *  the `-I-', those directories are searched for all `#include'
	 *  directives.
	 *  In addition, `-I-' inhibits the use of the directory of the current
	 *  file directory as the first search directory for `#include "FILE"'.
	 */
	C->quote_includepath = C->includepath+1;
	C->angle_includepath = C->sys_includepath;
	return 1;
}

/*
 * We replace "#pragma xxx" with "__pragma__" in the token
 * stream. Just as an example.
 *
 * We'll just #define that away for now, but the theory here
 * is that we can use this to insert arbitrary token sequences
 * to turn the pragmas into internal front-end sequences for
 * when we actually start caring about them.
 *
 * So eventually this will turn into some kind of extended
 * __attribute__() like thing, except called __pragma__(xxx).
 */
static int handle_pragma(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
	struct token *next = *line;

	if (dmrC_match_ident(token->next, C->S->once_ident) && dmrC_eof_token(token->next->next)) {
		stream->once = 1;
		return 1;
	}
	token->ident = C->S->pragma_ident;
	token->pos.newline = 1;
	token->pos.whitespace = 1;
	token->pos.pos = 1;
	*line = token;
	token->next = next;
	return 0;
}

/*
 * We ignore #line for now.
 */
static int handle_line(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
        (void) stream;
        (void) token;
        (void)line;
        (void) C;
	return 1;
}

static int handle_nondirective(struct dmr_C *C, struct stream *stream, struct token **line, struct token *token)
{
        (void) stream;
        (void) line;
	dmrC_sparse_error(C, token->pos, "unrecognized preprocessor line '%s'", show_token_sequence(C, token, 0));
	return 1;
}


static void init_preprocessor(struct dmr_C *C)
{	
	int i;
	int stream = dmrC_init_stream(C, "preprocessor", -1, C->includepath);
	static struct {
		const char *name;
		int (*handler)(struct dmr_C *, struct stream *, struct token **, struct token *);
	} normal[] = {
		{ "define",		handle_define },
		{ "weak_define",	handle_weak_define },
		{ "strong_define",	handle_strong_define },
		{ "undef",		handle_undef },
		{ "strong_undef",	handle_strong_undef },
		{ "warning",		handle_warning },
		{ "error",		handle_error },
		{ "include",		handle_include },
		{ "include_next",	handle_include_next },
		{ "pragma",		handle_pragma },
		{ "line",		handle_line },

		// our internal preprocessor tokens
		{ "nostdinc",	   handle_nostdinc },
		{ "add_include",   handle_add_include },
		{ "add_isystem",   handle_add_isystem },
		{ "add_system",    handle_add_system },
		{ "add_dirafter",  handle_add_dirafter },
		{ "split_include", handle_split_include },
		{ "argv_include",  handle_argv_include },
	}, special[] = {
		{ "ifdef",	handle_ifdef },
		{ "ifndef",	handle_ifndef },
		{ "else",	handle_else },
		{ "endif",	handle_endif },
		{ "if",		handle_if },
		{ "elif",	handle_elif },
	};

	for (i = 0; i < (int)ARRAY_SIZE(normal); i++) {
		struct symbol *sym;
		sym = dmrC_create_symbol(C->S, stream, normal[i].name, SYM_PREPROCESSOR, NS_PREPROCESSOR);
		sym->handler = normal[i].handler;
		sym->normal = 1;
	}
	for (i = 0; i < (int)ARRAY_SIZE(special); i++) {
		struct symbol *sym;
		sym = dmrC_create_symbol(C->S, stream, special[i].name, SYM_PREPROCESSOR, NS_PREPROCESSOR);
		sym->handler = special[i].handler;
		sym->normal = 0;
	}

	C->counter_macro = 0;
}

static void handle_preprocessor_line(struct dmr_C *C, struct stream *stream, struct token **line, struct token *start)
{
	int (*handler)(struct dmr_C *, struct stream *, struct token **, struct token *);
	struct token *token = start->next;
	int is_normal = 1;

	if (dmrC_eof_token(token))
		return;

	if (dmrC_token_type(token) == TOKEN_IDENT) {
		struct symbol *sym = dmrC_lookup_symbol(token->ident, NS_PREPROCESSOR);
		if (sym) {
			handler = sym->handler;
			is_normal = sym->normal;
		} else {
			handler = handle_nondirective;
		}
	} else if (dmrC_token_type(token) == TOKEN_NUMBER) {
		handler = handle_line;
	} else {
		handler = handle_nondirective;
	}

	if (is_normal) {
		dirty_stream(stream);
		if (C->false_nesting)
			goto out;
	}
	if (!handler(C, stream, line, token))	/* all set */
		return;

out:
	free_preprocessor_line(C, token);
}

static void preprocessor_line(struct dmr_C *C, struct stream *stream, struct token **line)
{
	struct token *start = *line, *next;
	struct token **tp = &start->next;

	for (;;) {
		next = *tp;
		if (next->pos.newline)
			break;
		tp = &next->next;
	}
	*line = next;
	*tp = &dmrC_eof_token_entry_;
	handle_preprocessor_line(C, stream, line, start);
}

static void do_preprocess(struct dmr_C *C, struct token **list)
{
	struct token *next;

	while (!dmrC_eof_token(next = scan_next(C, list))) {
		struct stream *stream = C->T->input_streams + next->pos.stream;

		if (next->pos.newline && dmrC_match_op(next, '#')) {
			if (!next->pos.noexpand) {
				preprocessor_line(C, stream, list);
				dmrC_allocator_free(&C->token_allocator, next);	/* Free the '#' token */
				continue;
			}
		}

		switch (dmrC_token_type(next)) {
		case TOKEN_STREAMEND:
			if (stream->top_if) {
				nesting_error(stream);
				dmrC_sparse_error(C, stream->top_if->pos, "unterminated preprocessor conditional");
				stream->top_if = NULL;
				C->false_nesting = 0;
			}
			if (!stream->dirty)
				stream->constant = CONSTANT_FILE_YES;
			*list = next->next;
			continue;
		case TOKEN_STREAMBEGIN:
			*list = next->next;
			continue;

		default:
			dirty_stream(stream);
			if (C->false_nesting) {
				*list = next->next;
				dmrC_allocator_free(&C->token_allocator, next);
				continue;
			}

			if (dmrC_token_type(next) != TOKEN_IDENT ||
			    expand_one_symbol(C, list))
				list = &next->next;
		}
	}
}

struct token * dmrC_preprocess(struct dmr_C *C, struct token *token)
{
	C->preprocessing = 1;
	init_preprocessor(C);
	do_preprocess(C, &token);

	// Drop all expressions from preprocessing, they're not used any more.
	// This is not true when we have multiple files, though ;/
	// clear_expression_alloc();
	C->preprocessing = 0;

	return token;
}

void dmrC_init_preprocessor_state(struct dmr_C *C) {
	assert(C->includepath[0] == NULL);
	C->includepath[0] = "";
	C->includepath[1] = "/usr/include";
	C->includepath[2] = "/usr/local/include";
	C->includepath[3] = NULL;

	C->quote_includepath = C->includepath;
	C->angle_includepath = C->includepath + 1;
	C->isys_includepath = C->includepath + 1;
	C->sys_includepath = C->includepath + 1;
	C->dirafter_includepath = C->includepath + 3;
}

static void dump_macro(struct dmr_C *C, struct symbol *sym)
{
	int nargs = sym->arglist ? sym->arglist->count.normal : 0;
	struct token **args;
	struct token *token;

	args = alloca(sizeof(struct token *) * nargs);
	printf("#define %s", dmrC_show_ident(C, sym->ident));
	token = sym->arglist;
	if (token) {
		const char *sep = "";
		int narg = 0;
		putchar('(');
		for (; !dmrC_eof_token(token); token = token->next) {
			if (dmrC_token_type(token) == TOKEN_ARG_COUNT)
				continue;
			printf("%s%s", sep, dmrC_show_token(C, token));
			args[narg++] = token;
			sep = ", ";
		}
		putchar(')');
	}
	putchar(' ');

	token = sym->expansion;
	while (!dmrC_eof_token(token)) {
		struct token *next = token->next;
		switch (dmrC_token_type(token)) {
		case TOKEN_UNTAINT:
			break;
		case TOKEN_MACRO_ARGUMENT:
			token = args[token->argnum];
			/* fall-through */
		default:
			printf("%s", dmrC_show_token(C, token));
			if (next->pos.whitespace)
				putchar(' ');
		}
		token = next;
	}
	putchar('\n');
}

void dmrC_dump_macro_definitions(struct dmr_C *C)
{
	struct ident *name;

	FOR_EACH_PTR(C->macros, name) {
		struct symbol *sym = lookup_macro(C, name);
		if (sym)
			dump_macro(C, sym);
	} END_FOR_EACH_PTR(name);
}
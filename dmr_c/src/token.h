/*
* Basic tokenization structures. NOTE! Those tokens had better
* be pretty small, since we're going to keep them all in memory
* indefinitely.
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

#ifndef DMR_C_TOKENIZER_H
#define DMR_C_TOKENIZER_H

#include <lib.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This describes the pure lexical elements (tokens), with
 * no semantic meaning. In other words, an identifier doesn't
 * have a type or meaning, it is only a specific string in
 * the input stream.
 *
 * Semantic meaning is handled elsewhere.
 */

enum constantfile {
  CONSTANT_FILE_MAYBE,    // To be determined, not inside any #ifs in this file
  CONSTANT_FILE_IFNDEF,   // To be determined, currently inside #ifndef
  CONSTANT_FILE_NOPE,     // No
  CONSTANT_FILE_YES       // Yes
};

struct stream {
	int fd;
	const char *name;
	const char *path; // input-file path - see set_stream_include_path()
	const char **next_path;

	/* Use these to check for "already parsed" */
	enum constantfile constant;
	int dirty, next_stream, once;
	struct ident *protect;
	struct token *ifndef;
	struct token *top_if;
};

struct ident {
	struct ident *next;	/* Hash chain of identifiers */
	struct symbol *symbols;	/* Pointer to semantic meaning list */
	unsigned char len;	/* Length of identifier name */
	unsigned char tainted:1,
	              reserved:1,
		      keyword:1;
	char name[];		/* Actual identifier */
};
DECLARE_PTR_LIST(ident_list, struct ident);

enum e_token_type {
	TOKEN_EOF,
	TOKEN_ERROR,
	TOKEN_IDENT,
	TOKEN_ZERO_IDENT,
	TOKEN_NUMBER,
	TOKEN_CHAR,
	TOKEN_CHAR_EMBEDDED_0,
	TOKEN_CHAR_EMBEDDED_1,
	TOKEN_CHAR_EMBEDDED_2,
	TOKEN_CHAR_EMBEDDED_3,
	TOKEN_WIDE_CHAR,
	TOKEN_WIDE_CHAR_EMBEDDED_0,
	TOKEN_WIDE_CHAR_EMBEDDED_1,
	TOKEN_WIDE_CHAR_EMBEDDED_2,
	TOKEN_WIDE_CHAR_EMBEDDED_3,
	TOKEN_STRING,
	TOKEN_WIDE_STRING,
	TOKEN_SPECIAL,
	TOKEN_STREAMBEGIN,
	TOKEN_STREAMEND,
	TOKEN_MACRO_ARGUMENT,
	TOKEN_STR_ARGUMENT,
	TOKEN_QUOTED_ARGUMENT,
	TOKEN_CONCAT,
	TOKEN_GNU_KLUDGE,
	TOKEN_UNTAINT,
	TOKEN_ARG_COUNT,
	TOKEN_IF,
	TOKEN_SKIP_GROUPS,
	TOKEN_ELSE,
};

/* Combination tokens */
#define COMBINATION_STRINGS {	\
	"+=", "++",		\
	"-=", "--", "->",	\
	"*=",			\
	"/=",			\
	"%=",			\
	"<=", ">=",		\
	"==", "!=",		\
	"&&", "&=",		\
	"||", "|=",		\
	"^=", "##",		\
	"<<", ">>", "..",	\
	"<<=", ">>=", "...",	\
	"",			\
	"<", ">", "<=", ">="	\
}

extern unsigned char dmrC_combinations_[][4];

enum special_token {
	SPECIAL_BASE = 256,
	SPECIAL_ADD_ASSIGN = SPECIAL_BASE,
	SPECIAL_INCREMENT,
	SPECIAL_SUB_ASSIGN,
	SPECIAL_DECREMENT,
	SPECIAL_DEREFERENCE,
	SPECIAL_MUL_ASSIGN,
	SPECIAL_DIV_ASSIGN,
	SPECIAL_MOD_ASSIGN,
	SPECIAL_LTE,
	SPECIAL_GTE,
	SPECIAL_EQUAL,
	SPECIAL_NOTEQUAL,
	SPECIAL_LOGICAL_AND,
	SPECIAL_AND_ASSIGN,
	SPECIAL_LOGICAL_OR,
	SPECIAL_OR_ASSIGN,
	SPECIAL_XOR_ASSIGN,
	SPECIAL_HASHHASH,
	SPECIAL_LEFTSHIFT,
	SPECIAL_RIGHTSHIFT,
	SPECIAL_DOTDOT,
	SPECIAL_SHL_ASSIGN,
	SPECIAL_SHR_ASSIGN,
	SPECIAL_ELLIPSIS,
	SPECIAL_ARG_SEPARATOR,
	SPECIAL_UNSIGNED_LT,
	SPECIAL_UNSIGNED_GT,
	SPECIAL_UNSIGNED_LTE,
	SPECIAL_UNSIGNED_GTE,
};

struct string {
	unsigned int length:31;
	unsigned int immutable:1;
	char data[];
};

/* will fit into 32 bits */
struct argcount {
	unsigned normal:10;
	unsigned quoted:10;
	unsigned str:10;
	unsigned vararg:1;
};

/*
 * This is a very common data structure, it should be kept
 * as small as humanly possible. Big (rare) types go as
 * pointers.
 */
struct token {
	struct position pos;
	struct token *next;
	union {
		const char *number;
		struct ident *ident;
		unsigned int special;
		struct string *string;
		int argnum;
		struct argcount count;
		char embedded[4];
	};
};

static inline struct token *dmrC_containing_token(struct token **p)
{
	void *addr = (char *)p - ((char *)&((struct token *)0)->next - (char *)0);
	return (struct token *)addr;
}

struct tokenizer_state_t {
	unsigned int tabstop;
	int input_stream_nr;
	struct stream *input_streams;
	int input_streams_allocated;
	char special[256];		   // identifies CR LF TAB
	long cclass[257];		   // character class
	unsigned char hash_results[32][2]; // hashes compound operators
	int code[32]; // token values for compound operators
	char special_buffer[4];
	char ident_buffer[256];
	char string_buffer[4 * MAX_STRING + 3];
	char char_buffer[MAX_STRING + 4];
	char quote_buffer[2 * MAX_STRING + 6];
	char token_buffer[256];
	char quoted_token_buffer[256];
	char number_buffer[4095];
	char string_buffer2[MAX_STRING];
	struct ident **hash_table;
	int ident_hit, ident_miss, idents;
	const char **includepath;
};


#define dmrC_token_type(x) ((x)->pos.type)

/*
 * Last token in the stream - points to itself.
 * This allows us to not test for NULL pointers
 * when following the token->next chain..
 */
extern struct token dmrC_eof_token_entry_;
#define dmrC_eof_token(x) ((x) == &dmrC_eof_token_entry_)
extern void dmrC_init_tokenizer(struct dmr_C *C);
extern void dmrC_destroy_tokenizer(struct dmr_C *C);

extern int dmrC_init_stream(struct dmr_C *C, const char *name, int fd,
	const char **next_path);
extern const char *dmrC_stream_name(struct dmr_C *C, int stream);
extern struct ident *dmrC_hash_ident(struct dmr_C *C, struct ident *ident);
extern struct ident *dmrC_built_in_ident(struct dmr_C *C, const char *name);
extern struct token *dmrC_built_in_token(struct dmr_C *C, int stream, struct ident *ident);
extern const char *dmrC_show_special(struct dmr_C *C, int val);
extern const char *dmrC_show_ident(struct dmr_C *C, const struct ident *ident);
extern const char *dmrC_show_string(struct dmr_C *C, const struct string *string);
extern const char *dmrC_show_token(struct dmr_C *C, const struct token *token);
extern const char *dmrC_quote_token(struct dmr_C *C, const struct token *token);
extern int *dmrC_hash_stream(const char *name);
extern struct token *dmrC_tokenize(struct dmr_C *C, const char *name, int fd,
	struct token *endtoken, const char **next_path);
/* This function assumes that stream 0 is being used - so it is not suitable
   for general use */
extern struct token *dmrC_tokenize_buffer(struct dmr_C *C, unsigned char *buffer,
	unsigned long size,
	struct token **endtoken);
/* This version allows a named stream to be created */
extern struct token *dmrC_tokenize_buffer_stream(struct dmr_C *C,
						 const char *name,
						 unsigned char *buffer,
						 unsigned long size,
						 struct token **endtoken);
extern void dmrC_show_identifier_stats(struct dmr_C *C);
extern struct token *dmrC_preprocess(struct dmr_C *C, struct token *);
extern void dmrC_init_preprocessor_state(struct dmr_C *C);
static inline int dmrC_match_op(struct token *token, unsigned int op)
{
	return token->pos.type == TOKEN_SPECIAL && token->special == op;
}

static inline int dmrC_match_ident(struct token *token, struct ident *id)
{
	return token->pos.type == TOKEN_IDENT && token->ident == id;
}

static inline void dmrC_add_ident(struct dmr_C *C, struct ident_list **list, struct ident *ident)
{
	ptrlist_add((struct ptr_list **)list, ident, &C->ptrlist_allocator);
}

extern int dmrC_test_tokenizer();

#ifdef __cplusplus
}
#endif


#endif
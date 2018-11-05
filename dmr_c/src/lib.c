/*
* 'sparse' library helper routines.
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

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include <sys/types.h>

#include <allocate.h>
#include <lib.h>
#include <port.h>
#include <target.h>
#include <token.h>
#include <scope.h>
#include <expression.h>
#include <parse.h>
#include <linearize.h>

#define SPARSE_VERSION "0.5" 
#ifndef __GNUC__
# define __GNUC__ 2
# define __GNUC_MINOR__ 95
# define __GNUC_PATCHLEVEL__ 0
#endif

int gcc_major = __GNUC__;
int gcc_minor = __GNUC_MINOR__;
int gcc_patchlevel = __GNUC_PATCHLEVEL__;

long double dmrC_string_to_ld(const char *nptr, char **endptr)
{
	return strtod(nptr, endptr);
}

struct token *dmrC_skip_to_token(struct token *token, int op)
{
	while (!dmrC_match_op(token, op) && !dmrC_eof_token(token))
		token = token->next;
	return token;
}

struct token *dmrC_expect_token(struct dmr_C *C, struct token *token, int op, const char *where)
{
	if (!dmrC_match_op(token, op)) {
		static struct token bad_token;
		if (token != &bad_token) {
			bad_token.next = token;
			dmrC_sparse_error(C, token->pos, "Expected %s %s", dmrC_show_special(C, op), where);
			dmrC_sparse_error(C, token->pos, "got %s", dmrC_show_token(C, token));
		}
		if (op == ';')
			return dmrC_skip_to_token(token, op);
		return &bad_token;
	}
	return token->next;
}

unsigned int dmrC_hexval(unsigned int c)
{
	int retval = 256;
	switch (c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		retval = c - '0';
		break;
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		retval = c - 'a' + 10;
		break;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		retval = c - 'A' + 10;
		break;
	}
	return retval;
}

static void do_warn(struct dmr_C *C, const char *type, struct position pos,
		    const char *fmt, va_list args)
{
	static char buffer[512];
	const char *name;

	vsprintf(buffer, fmt, args);
	name = dmrC_stream_name(C, pos.stream);

	fprintf(stderr, "%s:%d:%d: %s%s\n", name, pos.line, pos.pos, type,
		buffer);
}

void dmrC_info(struct dmr_C *C, struct position pos, const char *fmt, ...)
{
	va_list args;

	if (!C->show_info)
		return;
	va_start(args, fmt);
	do_warn(C, "", pos, fmt, args);
	va_end(args);
}

static void do_error(struct dmr_C *C, struct position pos, const char *fmt,
		     va_list args)
{
	C->die_if_error = 1;
	C->show_info = 1;
	/* Shut up warnings after an error */
	C->has_error |= ERROR_CURR_PHASE;
	C->max_warnings = 0;
	if (C->errors > 100) {
		C->show_info = 0;
		if (C->once)
			return;
		fmt = "too many errors";
		C->once = 1;
	}

	do_warn(C, "error: ", pos, fmt, args);
	C->errors++;
}


void dmrC_warning(struct dmr_C *C, struct position pos, const char *fmt, ...)
{
	va_list args;

	if (C->Wsparse_error) {
		va_start(args, fmt);
		do_error(C, pos, fmt, args);
		va_end(args);
		return;
	}
	if (!C->max_warnings || C->has_error) {
		C->show_info = 0;
		return;
	}

	if (!--C->max_warnings) {
		C->show_info = 0;
		fmt = "too many warnings";
	}

	va_start(args, fmt);
	do_warn(C, "warning: ", pos, fmt, args);
	va_end(args);
}


void dmrC_sparse_error(struct dmr_C *C, struct position pos, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_error(C, pos, fmt, args);
	va_end(args);
}

void dmrC_expression_error(struct dmr_C *C, struct expression *expr, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_error(C, expr->pos, fmt, args);
	va_end(args);
	expr->ctype = &C->S->bad_ctype;
}

void dmrC_error_die(struct dmr_C *C, struct position pos, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_warn(C, "error: ", pos, fmt, args);
	va_end(args);
	longjmp(C->jmpbuf, 1);
}

void dmrC_die(struct dmr_C *C, const char *fmt, ...)
{
	va_list args;
	static char buffer[512];

	(void)C;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	fprintf(stderr, "%s\n", buffer);
	longjmp(C->jmpbuf, 1);
}

#define ARCH_LP32  0
#define ARCH_LP64  1
#define ARCH_LLP64 2

#if _WIN32 || _WIN64
#if _WIN64
#define ARCH_M64_DEFAULT ARCH_LLP64
#else
#define ARCH_M64_DEFAULT ARCH_LP32
#endif
#elif __GNUC__
#ifdef __x86_64__
#define ARCH_M64_DEFAULT ARCH_LP64 
#else
#define ARCH_M64_DEFAULT ARCH_LP32
#endif
#else
#error Unsupported compiler
#endif

#ifdef __BIG_ENDIAN__
#define ARCH_BIG_ENDIAN 1
#else
#define ARCH_BIG_ENDIAN 0
#endif

struct dmr_C *new_dmr_C()
{
	struct dmr_C *C = (struct dmr_C *)calloc(1, sizeof(struct dmr_C));
	C->standard = STANDARD_GNU89;
	C->arch_m64 = ARCH_M64_DEFAULT;
	C->arch_msize_long = 0;
	C->arch_big_endian = ARCH_BIG_ENDIAN;
#ifdef GCC_BASE
	C->gcc_base_dir = GCC_BASE;
#else
	C->gcc_base_dir = NULL;
#endif
#ifdef MULTIARCH_TRIPLET
	C->multiarch_dir = MULTIARCH_TRIPLET;
#else
	C->multiarch_dir = NULL;
#endif

	C->Waddress_space = 1;
	C->Wcast_truncate = 1;
	C->Wcontext = 1;
	C->Wdecl = 1;
	C->Wdeclarationafterstatement = -1;
	C->Wdesignated_init = 1;
	C->Wenum_mismatch = 1;
	C->Wnon_pointer_null = 1;
	C->Wold_initializer = 1;
	C->Wone_bit_signed_bitfield = 1;
	C->Wuninitialized = 1;
	C->Wunknown_attribute = 1;
	C->Wvla = 1;
	C->Woverride_init = 1;
	C->Wbitwise = 1;
	C->Wmemcpy_max_count = 1;

	C->max_warnings = 100;
	C->show_info = 1;
	C->fmemcpy_max_count = 100000;

	dmrC_allocator_init(&C->ptrlist_allocator, "ptrlist_nodes", sizeof(struct ptr_list),
		__alignof__(struct ptr_list), CHUNK);
	dmrC_allocator_init(&C->byte_allocator, "bytes", sizeof(char),
		       __alignof__(char), CHUNK);
	dmrC_allocator_init(&C->ident_allocator, "identifiers", sizeof(struct ident),
		       __alignof__(struct ident), CHUNK);
	dmrC_allocator_init(&C->string_allocator, "strings", sizeof(struct string),
		       __alignof__(struct string), CHUNK);
	dmrC_allocator_init(&C->token_allocator, "tokens", sizeof(struct token),
		       __alignof__(struct token), CHUNK);
	dmrC_allocator_init(&C->scope_allocator, "scopes", sizeof(struct scope),
		__alignof__(struct scope), CHUNK);
	dmrC_allocator_init(&C->expression_allocator, "expressions", sizeof(struct expression),
		__alignof__(struct expression), CHUNK);
	dmrC_allocator_init(&C->statement_allocator, "statements", sizeof(struct statement),
		__alignof__(struct statement), CHUNK);

	C->warnings[0].name = "address-space";		C->warnings[0].flag = &C->Waddress_space;
	C->warnings[1].name = "bitwise";			C->warnings[1].flag = &C->Wbitwise;
	C->warnings[2].name = "cast-to-as";			C->warnings[2].flag = &C->Wcast_to_as;
	C->warnings[3].name = "cast-truncate";		C->warnings[3].flag = &C->Wcast_truncate;
	C->warnings[4].name = "context";			C->warnings[4].flag = &C->Wcontext;
	C->warnings[5].name = "decl";				C->warnings[5].flag = &C->Wdecl;
	C->warnings[6].name = "declaration-after-statement"; C->warnings[6].flag = &C->Wdeclarationafterstatement;
	C->warnings[7].name = "default-bitfield-sign"; C->warnings[7].flag = &C->Wdefault_bitfield_sign;
	C->warnings[8].name = "designated-init";	C->warnings[8].flag = &C->Wdesignated_init;
	C->warnings[9].name = "do-while";			C->warnings[9].flag = &C->Wdo_while;
	C->warnings[10].name = "enum-mismatch";		C->warnings[10].flag = &C->Wenum_mismatch;
	C->warnings[11].name = "sparse-error";		C->warnings[11].flag = &C->Wsparse_error;
	C->warnings[12].name = "init-cstring";		C->warnings[12].flag = &C->Winit_cstring;
	C->warnings[13].name = "non-pointer-null";	C->warnings[13].flag = &C->Wnon_pointer_null;
	C->warnings[14].name = "old-initializer";	C->warnings[14].flag = &C->Wold_initializer;
	C->warnings[15].name = "one-bit-signed-bitfield"; C->warnings[15].flag = &C->Wone_bit_signed_bitfield;
	C->warnings[16].name = "paren-string";		C->warnings[16].flag = &C->Wparen_string;
	C->warnings[17].name = "ptr-subtraction-blows"; C->warnings[17].flag = &C->Wptr_subtraction_blows;
	C->warnings[18].name = "return-void";		C->warnings[18].flag = &C->Wreturn_void;
	C->warnings[19].name = "shadow";			C->warnings[19].flag = &C->Wshadow;
	C->warnings[20].name = "sizeof-bool";		C->warnings[20].flag = &C->Wsizeof_bool;
	C->warnings[21].name = "tautological-compare"; C->warnings[21].flag = &C->Wtautological_compare;
	C->warnings[22].name = "transparent-union"; C->warnings[22].flag = &C->Wtransparent_union;
	C->warnings[23].name = "typesign";			C->warnings[23].flag = &C->Wtypesign;
	C->warnings[24].name = "undef";				C->warnings[24].flag = &C->Wundef;
	C->warnings[25].name = "uninitialized";		C->warnings[25].flag = &C->Wuninitialized;
	C->warnings[26].name = "unknown-attribute"; C->warnings[26].flag = &C->Wunknown_attribute;
	C->warnings[27].name = "vla";				C->warnings[27].flag = &C->Wvla;
	C->warnings[28].name = "address";			C->warnings[28].flag = &C->Waddress;
	C->warnings[29].name = "memcpy-max-count";		C->warnings[29].flag = &C->Wmemcpy_max_count;
	C->warnings[30].name = "override-init";			C->warnings[30].flag = &C->Woverride_init;
	C->warnings[31].name = "override-init-all";		C->warnings[31].flag = &C->Woverride_init_all;

	C->debugs[0].name = "entry"; C->debugs[0].flag = &C->dbg_entry;
	C->debugs[1].name = "dead";  C->debugs[1].flag = &C->dbg_dead;

	C->dumps[0].name = "D"; C->dumps[0].flag = &C->dump_macro_defs;

	dmrC_init_target(C);
	dmrC_init_tokenizer(C);
	dmrC_init_preprocessor_state(C);
	dmrC_init_scope(C);
	dmrC_init_symbols(C);
	dmrC_init_ctype(C);
#if !defined(PARSER_ONLY)
	dmrC_init_linearizer(C);
#endif
	return C;
}

void destroy_dmr_C(struct dmr_C *C)
{
	dmrC_destroy_all_scopes(C);
	dmrC_destroy_tokenizer(C);
	dmrC_destroy_target(C);
	dmrC_destroy_symbols(C);
#if !defined(PARSER_ONLY)
	dmrC_destroy_linearizer(C);
#endif
	dmrC_allocator_destroy(&C->token_allocator);
	dmrC_allocator_destroy(&C->protected_token_allocator);
	dmrC_allocator_destroy(&C->string_allocator);
	dmrC_allocator_destroy(&C->ident_allocator);
	dmrC_allocator_destroy(&C->byte_allocator);
	dmrC_allocator_destroy(&C->scope_allocator);
	dmrC_allocator_destroy(&C->expression_allocator);
	dmrC_allocator_destroy(&C->statement_allocator);
	dmrC_allocator_destroy(&C->ptrlist_allocator);
	free(C);
}

void dmrC_add_pre_buffer(struct dmr_C *C, const char *fmt, ...)
{
	va_list args;
	unsigned int size;
	struct token *begin, *end;
	char buffer[4096];

	va_start(args, fmt);
	size = vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	begin = dmrC_tokenize_buffer(C, (unsigned char *)buffer, size, &end);
	if (!C->pre_buffer_begin)
		C->pre_buffer_begin = begin;
	if (C->pre_buffer_end)
		C->pre_buffer_end->next = begin;
	C->pre_buffer_end = end;
}

static char **handle_switch_D(struct dmr_C *C, char *arg, char **next)
{
	const char *name = arg + 1;
	const char *value = "1";

	if (!*name || isspace((unsigned char)*name))
		dmrC_die(C, "argument to `-D' is missing");

	for (;;) {
		char c;
		c = *++arg;
		if (!c)
			break;
		if (isspace((unsigned char)c) || c == '=') {
			*arg = '\0';
			value = arg + 1;
			break;
		}
	}
	dmrC_add_pre_buffer(C, "#define %s %s\n", name, value);
	return next;
}

static char **handle_switch_E(struct dmr_C *C, char *arg, char **next)
{
	if (arg[1] == '\0')
		C->preprocess_only = 1;
	return next;
}

static char **handle_switch_I(struct dmr_C *C, char *arg, char **next)
{
	char *path = arg + 1;

	switch (arg[1]) {
	case '-':
		dmrC_add_pre_buffer(C, "#split_include\n");
		break;

	case '\0':	/* Plain "-I" */
		path = *++next;
		if (!path)
			dmrC_die(C, "missing argument for -I option");
		/* Fall through */
	default:
		dmrC_add_pre_buffer(C, "#add_include \"%s/\"\n", path);
	}
	return next;
}

static void add_cmdline_include(struct dmr_C *C, char *filename)
{
	if (C->cmdline_include_nr >= CMDLINE_INCLUDE)
		dmrC_die(C, "too many include files for %s\n", filename);
	C->cmdline_include[C->cmdline_include_nr++] = filename;
}

static char **handle_switch_i(struct dmr_C *C, char *arg, char **next)
{
	if (*next && !strcmp(arg, "include"))
		add_cmdline_include(C, *++next);
	else if (*next && !strcmp(arg, "imacros"))
		add_cmdline_include(C, *++next);
	else if (*next && !strcmp(arg, "isystem")) {
		char *path = *++next;
		if (!path)
			dmrC_die(C, "missing argument for -isystem option");
		dmrC_add_pre_buffer(C, "#add_isystem \"%s/\"\n", path);
	}
	else if (*next && !strcmp(arg, "idirafter")) {
		char *path = *++next;
		if (!path)
			dmrC_die(C, "missing argument for -idirafter option");
		dmrC_add_pre_buffer(C, "#add_dirafter \"%s/\"\n", path);
	}
	return next;
}

static char **handle_switch_M(struct dmr_C *C, char *arg, char **next)
{
	if (!strcmp(arg, "MF") || !strcmp(arg, "MQ") || !strcmp(arg, "MT")) {
		if (!*next)
			dmrC_die(C, "missing argument for -%s option", arg);
		return next + 1;
	}
	return next;
}

static char **handle_multiarch_dir(struct dmr_C *C, char *arg, char **next)
{
        (void)arg;
	C->multiarch_dir = *++next;
	if (!C->multiarch_dir)
		dmrC_die(C, "missing argument for -multiarch-dir option");
	return next;
}
static char **handle_switch_m(struct dmr_C *C, char *arg, char **next)
{
	if (!strcmp(arg, "m64")) {
		C->arch_m64 = ARCH_LP64;
	} else if (!strcmp(arg, "m32")) {
		C->arch_m64 = ARCH_LP32;
	} else if (!strcmp(arg, "msize-llp64")) {
		C->arch_m64 = ARCH_LLP64;
	} else if (!strcmp(arg, "msize-long")) {
		C->arch_msize_long = 1;
	} else if (!strcmp(arg, "multiarch-dir")) {
		return handle_multiarch_dir(C, arg, next);
	} else if (!strcmp(arg, "mbig-endian")) {
		C->arch_big_endian = 1;
	} else if (!strcmp(arg, "mlittle-endian")) {
		C->arch_big_endian = 0;
	}
	return next;
}

static void handle_arch_m64_finalize(struct dmr_C *C)
{
	switch (C->arch_m64) {
	case ARCH_LP32:
		C->target->bits_in_long = 32;
		C->target->max_int_alignment = 4;
		C->target->bits_in_pointer = 32;
		C->target->pointer_alignment = 4;
		break;
	case ARCH_LP64:
		C->target->bits_in_long = 64;
		C->target->max_int_alignment = 8;
		C->target->size_t_ctype = &C->S->ulong_ctype;
		C->target->ssize_t_ctype = &C->S->long_ctype;
		dmrC_add_pre_buffer(C, "#weak_define __LP64__ 1\n");
		dmrC_add_pre_buffer(C, "#weak_define _LP64 1\n");
		goto case_64bit_common;
	case ARCH_LLP64:
		C->target->bits_in_long = 32;
		C->target->max_int_alignment = 4;
		C->target->size_t_ctype = &C->S->ullong_ctype;
		C->target->ssize_t_ctype = &C->S->llong_ctype;
		dmrC_add_pre_buffer(C, "#weak_define __LLP64__ 1\n");
		goto case_64bit_common;
	case_64bit_common:
		C->target->bits_in_pointer = 64;
		C->target->pointer_alignment = 8;
#ifdef __x86_64__
		dmrC_add_pre_buffer(C, "#weak_define __x86_64__ 1\n");
#endif
		break;
	}
}

static void handle_arch_msize_long_finalize(struct dmr_C *C)
{
	if (C->arch_msize_long) {
		C->target->size_t_ctype = &C->S->ulong_ctype;
		C->target->ssize_t_ctype = &C->S->long_ctype;
	}
}

static void handle_arch_finalize(struct dmr_C *C)
{
	handle_arch_m64_finalize(C);
	handle_arch_msize_long_finalize(C);
}


static int handle_simple_switch(const char *arg, const char *name, int *flag)
{
	int val = 1;

	// Prefixe "no-" mean to turn flag off.
	if (strncmp(arg, "no-", 3) == 0) {
		arg += 3;
		val = 0;
	}

	if (strcmp(arg, name) == 0) {
		*flag = val;
		return 1;
	}

	// not handled
	return 0;
}

static char **handle_switch_o(struct dmr_C *C, char *arg, char **next)
{
	if (!strcmp(arg, "o")) {       // "-o foo"
		if (!*++next)
			dmrC_die(C, "argument to '-o' is missing");
	}
	snprintf(C->output_file_name, sizeof C->output_file_name, "%s", *next);
	return next;
}


static char **handle_onoff_switch(struct dmr_C *C, char *arg, char **next, const struct warning warnings[], int n)
{
	int flag = WARNING_ON;
	char *p = arg + 1;
	int i;

	if (strcmp(p, "sparse-all") == 0) {
		for (i = 0; i < n; i++) {
			if (*warnings[i].flag != WARNING_FORCE_OFF && warnings[i].flag != &C->Wsparse_error)
				*warnings[i].flag = WARNING_ON;
		}
	}

	// Prefixes "no" and "no-" mean to turn warning off.
	if (p[0] == 'n' && p[1] == 'o') {
		p += 2;
		if (p[0] == '-')
			p++;
		flag = WARNING_FORCE_OFF;
	}

	for (i = 0; i < n; i++) {
		if (!strcmp(p, warnings[i].name)) {
			*warnings[i].flag = flag;
			return next;
		}
	}

	// Unknown.
	return NULL;
}

static char **handle_switch_W(struct dmr_C *C, char *arg, char **next)
{
	char ** ret = handle_onoff_switch(C, arg, next, C->warnings, 32);
	if (ret)
		return ret;

	// Unknown.
	return next;
}

static char **handle_switch_v(struct dmr_C *C, char *arg, char **next)
{
	char ** ret = handle_onoff_switch(C, arg, next, C->debugs, 2);
	if (ret)
		return ret;

	// Unknown.
	do {
		C->verbose++;
	} while (*++arg == 'v');
	return next;
}

static char **handle_switch_d(struct dmr_C *C, char *arg, char **next)
{
	char ** ret = handle_onoff_switch(C, arg, next, C->dumps, 1);
	if (ret)
		return ret;

	return next;
}


static void handle_onoff_switch_finalize(const struct warning warnings[], int n)
{
	int i;

	for (i = 0; i < n; i++) {
		if (*warnings[i].flag == WARNING_FORCE_OFF)
			*warnings[i].flag = WARNING_OFF;
	}
}

static void handle_switch_W_finalize(struct dmr_C *C)
{
	handle_onoff_switch_finalize(C->warnings, 32);

	/* default Wdeclarationafterstatement based on the C dialect */
	if (-1 == C->Wdeclarationafterstatement)
	{
		switch (C->standard)
		{
		case STANDARD_C89:
		case STANDARD_C94:
			C->Wdeclarationafterstatement = 1;
			break;

		case STANDARD_C99:
		case STANDARD_GNU89:
		case STANDARD_GNU99:
		case STANDARD_C11:
		case STANDARD_GNU11:
			C->Wdeclarationafterstatement = 0;
			break;

		default:
			assert(0);
		}

	}
}

static void handle_switch_v_finalize(struct dmr_C *C)
{
	handle_onoff_switch_finalize(C->debugs, 2);
}

static char **handle_switch_U(struct dmr_C *C, char *arg, char **next)
{
	const char *name = arg + 1;
	dmrC_add_pre_buffer(C, "#undef %s\n", name);
	return next;
}

static char **handle_switch_O(struct dmr_C *C, char *arg, char **next)
{
	int level = 1;
	if (arg[1] >= '0' && arg[1] <= '9')
		level = arg[1] - '0';
	C->optimize = level;
	C->optimize_size = arg[1] == 's';
	return next;
}

static char **handle_switch_fmemcpy_max_count(struct dmr_C *C, char *arg, char **next)
{
	unsigned long long val;
	char *end;

	val = strtoull(arg, &end, 0);
	if (*end != '\0' || end == arg)
		dmrC_die(C, "error: missing argument to \"-fmemcpy-max-count=\"");

	if (val == 0)
		val = ~0ULL;
	C->fmemcpy_max_count = val;
	return next;
}

static char **handle_switch_ftabstop(struct dmr_C *C, char *arg, char **next)
{
	char *end;
	unsigned long val;

	if (*arg == '\0')
		dmrC_die(C, "error: missing argument to \"-ftabstop=\"");

	/* we silently ignore silly values */
	val = strtoul(arg, &end, 10);
	if (*end == '\0' && 1 <= val && val <= 100)
		C->T->tabstop = val;

	return next;
}

static char **handle_switch_fdump(struct dmr_C *C, char *arg, char **next)
{
	if (!strncmp(arg, "linearize", 9)) {
		arg += 9;
		if (*arg == '\0')
			C->fdump_linearize = 1;
		else if (!strcmp(arg, "=only"))
			C->fdump_linearize = 2;
	}

	/* ignore others flags */
	return next;
}

static char **handle_switch_f(struct dmr_C *C, char *arg, char **next)
{
	arg++;

	if (!strncmp(arg, "tabstop=", 8))
		return handle_switch_ftabstop(C, arg + 8, next);
	if (!strncmp(arg, "dump-", 5))
		return handle_switch_fdump(C, arg + 5, next);
	if (!strncmp(arg, "memcpy-max-count=", 17))
		return handle_switch_fmemcpy_max_count(C, arg + 17, next);

	/* handle switches w/ arguments above, boolean and only boolean below */
	if (handle_simple_switch(arg, "mem-report", &C->fmem_report))
		return next;

	return next;
}

static char **handle_switch_G(struct dmr_C *C, char *arg, char **next)
{
  (void) C;
	if (!strcmp(arg, "G") && *next)
		return next + 1; // "-G 0"
	else
		return next;     // "-G0" or (bogus) terminal "-G"
}

static char **handle_switch_a(struct dmr_C *C, char *arg, char **next)
{
	if (!strcmp(arg, "ansi"))
		C->standard = STANDARD_C89;

	return next;
}

static char **handle_switch_s(struct dmr_C *C, char *arg, char **next)
{
	if (!strncmp(arg, "std=", 4))
	{
		arg += 4;

		if (!strcmp(arg, "c89") ||
			!strcmp(arg, "iso9899:1990"))
			C->standard = STANDARD_C89;

		else if (!strcmp(arg, "iso9899:199409"))
			C->standard = STANDARD_C94;

		else if (!strcmp(arg, "c99") ||
			!strcmp(arg, "c9x") ||
			!strcmp(arg, "iso9899:1999") ||
			!strcmp(arg, "iso9899:199x"))
			C->standard = STANDARD_C99;

		else if (!strcmp(arg, "gnu89"))
			C->standard = STANDARD_GNU89;

		else if (!strcmp(arg, "gnu99") || !strcmp(arg, "gnu9x"))
			C->standard = STANDARD_GNU99;

		else if (!strcmp(arg, "c11") ||
			 !strcmp(arg, "c1x") ||
			 !strcmp(arg, "iso9899:2011"))
			C->standard = STANDARD_C11;

		else if (!strcmp(arg, "gnu11"))
			C->standard = STANDARD_GNU11;

		else
			dmrC_die(C, "Unsupported C dialect");
	}

	return next;
}

static char **handle_nostdinc(struct dmr_C *C, char *arg, char **next)
{
        (void)arg;
	dmrC_add_pre_buffer(C, "#nostdinc\n");
	return next;
}

static char **handle_switch_n(struct dmr_C *C, char *arg, char **next)
{
	if (!strcmp (arg, "nostdinc"))
		return handle_nostdinc(C, arg, next);

	return next;
}
static char **handle_base_dir(struct dmr_C *C, char *arg, char **next)
{
        (void)arg;
	C->gcc_base_dir = *++next;
	if (!C->gcc_base_dir)
		dmrC_die(C, "missing argument for -gcc-base-dir option");
	return next;
}

static char **handle_switch_g(struct dmr_C *C, char *arg, char **next)
{
	if (!strcmp (arg, "gcc-base-dir"))
		return handle_base_dir(C, arg, next);

	return next;
}
static char **handle_version(struct dmr_C *C, char *arg, char **next)
{
        (void)C;
        (void)arg;
        (void)next;
	printf("%s\n", SPARSE_VERSION);
	exit(0);
}

static char **handle_param(struct dmr_C *C, char *arg, char **next)
{
	char *value = NULL;

	/* For now just skip any '--param=*' or '--param *' */
	if (*arg == '\0') {
		value = *++next;
	} else if (isspace((unsigned char)*arg) || *arg == '=') {
		value = ++arg;
	}

	if (!value)
		dmrC_die(C, "missing argument for --param option");

	return next;
}
struct switches {
	const char *name;
	char **(*fn)(struct dmr_C *, char *, char **);
	unsigned int prefix:1;
};

static char **handle_long_options(struct dmr_C *C, char *arg, char **next)
{
	static struct switches cmd[] = {
		{ "param", handle_param, 1 },
		{ "version", handle_version , 0},
		{ NULL, NULL, 0 }
	};
	struct switches *s = cmd;

	while (s->name) {
		int optlen = (int) strlen(s->name);
		if (!strncmp(s->name, arg, optlen + !s->prefix))
			return s->fn(C, arg + optlen, next);
		s++;
	}
	return next;

}

static char **handle_switch(struct dmr_C *C, char *arg, char **next)
{
	switch (*arg) {
	case 'a': return handle_switch_a(C, arg, next);
	case 'D': return handle_switch_D(C, arg, next);
	case 'd': return handle_switch_d(C, arg, next);
	case 'E': return handle_switch_E(C, arg, next);
	case 'f': return handle_switch_f(C, arg, next);
	case 'g': return handle_switch_g(C, arg, next);
	case 'G': return handle_switch_G(C, arg, next);
	case 'I': return handle_switch_I(C, arg, next);
	case 'i': return handle_switch_i(C, arg, next);
	case 'M': return handle_switch_M(C, arg, next);
	case 'm': return handle_switch_m(C, arg, next);
	case 'n': return handle_switch_n(C, arg, next);
	case 'o': return handle_switch_o(C, arg, next);
	case 'O': return handle_switch_O(C, arg, next);
	case 's': return handle_switch_s(C, arg, next);
	case 'U': return handle_switch_U(C, arg, next);
	case 'v': return handle_switch_v(C, arg, next);
	case 'W': return handle_switch_W(C, arg, next);
	case '-': return handle_long_options(C, arg + 1, next);

	default:
		break;
	}

	/*
	* Ignore unknown command line options:
	* they're probably gcc switches
	*/
	return next;
}

static void predefined_sizeof(struct dmr_C *C, const char *name, unsigned bits)
{
	dmrC_add_pre_buffer(C, "#weak_define __SIZEOF_%s__ %d\n", name, bits/8);
}

static void predefined_max(struct dmr_C *C, const char *name, const char *suffix, unsigned bits)
{
	unsigned long long max = (1ULL << (bits - 1)) - 1;

	dmrC_add_pre_buffer(C, "#weak_define __%s_MAX__ %#llx%s\n", name, max, suffix);
}

static void predefined_type_size(struct dmr_C *C, const char *name, const char *suffix, unsigned bits)
{
	predefined_max(C, name, suffix, bits);

	predefined_sizeof(C, name, bits);
}

static void predefined_macros(struct dmr_C *C)
{
#if LLVM_JIT
	dmrC_add_pre_buffer(C, "#define __LLVM_BACKEND__ 1\n");
#elif NANO_JIT
	dmrC_add_pre_buffer(C, "#define __NANOJIT_BACKEND__ 1\n");
#elif OMR_JIT
	dmrC_add_pre_buffer(C, "#define __OMR_BACKEND__ 1\n");
#endif
	dmrC_add_pre_buffer(C, "#define __CHECKER__ 1\n");

	predefined_sizeof(C, "SHORT", C->target->bits_in_short);
	predefined_max(C, "SHRT", "", C->target->bits_in_short);
	predefined_max(C, "SCHAR", "", C->target->bits_in_char);
	predefined_max(C, "WCHAR", "", C->target->bits_in_wchar);
	dmrC_add_pre_buffer(C, "#weak_define __CHAR_BIT__ %d\n", C->target->bits_in_char);

	predefined_type_size(C, "INT", "", C->target->bits_in_int);
	predefined_type_size(C, "LONG", "L", C->target->bits_in_long);
	predefined_type_size(C, "LONG_LONG", "LL", C->target->bits_in_longlong);

	predefined_sizeof(C, "INT128", 128);

	predefined_sizeof(C, "SIZE_T", C->target->bits_in_pointer);
	predefined_sizeof(C, "PTRDIFF_T", C->target->bits_in_pointer);
	predefined_sizeof(C, "POINTER", C->target->bits_in_pointer);

	predefined_sizeof(C, "FLOAT", C->target->bits_in_float);
	predefined_sizeof(C, "DOUBLE", C->target->bits_in_double);
	predefined_sizeof(C, "LONG_DOUBLE", C->target->bits_in_longdouble);
	dmrC_add_pre_buffer(C, "#weak_define __%s_ENDIAN__ 1\n",
		C->arch_big_endian ? "BIG" : "LITTLE");

	dmrC_add_pre_buffer(C, "#weak_define __ORDER_LITTLE_ENDIAN__ 1234\n");
	dmrC_add_pre_buffer(C, "#weak_define __ORDER_BIG_ENDIAN__ 4321\n");
	dmrC_add_pre_buffer(C, "#weak_define __ORDER_PDP_ENDIAN__ 3412\n");
	dmrC_add_pre_buffer(C, "#weak_define __BYTE_ORDER__ __ORDER_%s_ENDIAN__\n",
		C->arch_big_endian ? "BIG" : "LITTLE");
}

void dmrC_declare_builtin_functions(struct dmr_C *C)
{
	/* Gaah. gcc knows tons of builtin <string.h> functions */
	dmrC_add_pre_buffer(C, "extern void *__builtin_memchr(const void *, int, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void *__builtin_memcpy(void *, const void *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void *__builtin_mempcpy(void *, const void *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void *__builtin_memmove(void *, const void *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void *__builtin_memset(void *, int, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_memcmp(const void *, const void *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strcat(char *, const char *);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strncat(char *, const char *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_strcmp(const char *, const char *);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_strncmp(const char *, const char *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_strcasecmp(const char *, const char *);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_strncasecmp(const char *, const char *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strchr(const char *, int);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strrchr(const char *, int);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strcpy(char *, const char *);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strncpy(char *, const char *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strdup(const char *);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strndup(const char *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern __SIZE_TYPE__ __builtin_strspn(const char *, const char *);\n");
	dmrC_add_pre_buffer(C, "extern __SIZE_TYPE__ __builtin_strcspn(const char *, const char *);\n");
	dmrC_add_pre_buffer(C, "extern char * __builtin_strpbrk(const char *, const char *);\n");
	dmrC_add_pre_buffer(C, "extern char* __builtin_stpcpy(const char *, const char*);\n");
	dmrC_add_pre_buffer(C, "extern char* __builtin_stpncpy(const char *, const char*, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern __SIZE_TYPE__ __builtin_strlen(const char *);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strstr(const char *, const char *);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strcasestr(const char *, const char *);\n");
	dmrC_add_pre_buffer(C, "extern char *__builtin_strnstr(const char *, const char *, __SIZE_TYPE__);\n");

	/* And even some from <strings.h> */
	dmrC_add_pre_buffer(C, "extern int  __builtin_bcmp(const void *, const void *, __SIZE_TYPE_);\n");
	dmrC_add_pre_buffer(C, "extern void __builtin_bcopy(const void *, void *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void __builtin_bzero(void *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern char*__builtin_index(const char *, int);\n");
	dmrC_add_pre_buffer(C, "extern char*__builtin_rindex(const char *, int);\n");

	/* And bitwise operations.. */
	dmrC_add_pre_buffer(C, "extern int __builtin_clrsb(int);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_clrsbl(long);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_clrsbll(long long);\n");

	/* And bitwise operations.. */
	dmrC_add_pre_buffer(C, "extern int __builtin_clz(int);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_clzl(long);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_clzll(long long);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_ctz(int);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_ctzl(long);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_ctzll(long long);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_ffs(int);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_ffsl(long);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_ffsll(long long);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_parity(unsigned int);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_parityl(unsigned long);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_parityll(unsigned long long);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_popcount(unsigned int);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_popcountl(unsigned long);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_popcountll(unsigned long long);\n");

	/* And byte swaps.. */
	dmrC_add_pre_buffer(C, "extern unsigned short __builtin_bswap16(unsigned short);\n");
	dmrC_add_pre_buffer(C, "extern unsigned int __builtin_bswap32(unsigned int);\n");
	dmrC_add_pre_buffer(C, "extern unsigned long long __builtin_bswap64(unsigned long long);\n");

	/* And atomic memory access functions.. */
	dmrC_add_pre_buffer(C, "extern int __sync_fetch_and_add(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_fetch_and_sub(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_fetch_and_or(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_fetch_and_and(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_fetch_and_xor(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_fetch_and_nand(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_add_and_fetch(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_sub_and_fetch(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_or_and_fetch(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_and_and_fetch(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_xor_and_fetch(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_nand_and_fetch(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_bool_compare_and_swap(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __sync_val_compare_and_swap(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern void __sync_synchronize();\n");
	dmrC_add_pre_buffer(C, "extern int __sync_lock_test_and_set(void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern void __sync_lock_release(void *, ...);\n");

	/* And some random ones.. */
	dmrC_add_pre_buffer(C, "extern void *__builtin_return_address(unsigned int);\n");
	dmrC_add_pre_buffer(C, "extern void *__builtin_extract_return_addr(void *);\n");
	dmrC_add_pre_buffer(C, "extern void *__builtin_frame_address(unsigned int);\n");
	dmrC_add_pre_buffer(C, "extern void __builtin_trap(void);\n");
	dmrC_add_pre_buffer(C, "extern void *__builtin_alloca(__SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void __builtin_prefetch (const void *, ...);\n");
	dmrC_add_pre_buffer(C, "extern long __builtin_alpha_extbl(long, long);\n");
	dmrC_add_pre_buffer(C, "extern long __builtin_alpha_extwl(long, long);\n");
	dmrC_add_pre_buffer(C, "extern long __builtin_alpha_insbl(long, long);\n");
	dmrC_add_pre_buffer(C, "extern long __builtin_alpha_inswl(long, long);\n");
	dmrC_add_pre_buffer(C, "extern long __builtin_alpha_insql(long, long);\n");
	dmrC_add_pre_buffer(C, "extern long __builtin_alpha_inslh(long, long);\n");
	dmrC_add_pre_buffer(C, "extern long __builtin_alpha_cmpbge(long, long);\n");
	dmrC_add_pre_buffer(C, "extern int  __builtin_abs(int);\n");
	dmrC_add_pre_buffer(C, "extern long __builtin_labs(long);\n");
	dmrC_add_pre_buffer(C, "extern long long __builtin_llabs(long long);\n");
	dmrC_add_pre_buffer(C, "extern double __builtin_fabs(double);\n");
	dmrC_add_pre_buffer(C, "extern __SIZE_TYPE__ __builtin_va_arg_pack_len(void);\n");

	/* Add Blackfin-specific stuff */
	dmrC_add_pre_buffer(C, 
		"#ifdef __bfin__\n"
		"extern void __builtin_bfin_csync(void);\n"
		"extern void __builtin_bfin_ssync(void);\n"
		"extern int __builtin_bfin_norm_fr1x32(int);\n"
		"#endif\n"
	);

	/* And some floating point stuff.. */
	dmrC_add_pre_buffer(C, "extern int __builtin_isgreater(float, float);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_isgreaterequal(float, float);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_isless(float, float);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_islessequal(float, float);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_islessgreater(float, float);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_isunordered(float, float);\n");

	/* And some INFINITY / NAN stuff.. */
	dmrC_add_pre_buffer(C, "extern double __builtin_huge_val(void);\n");
	dmrC_add_pre_buffer(C, "extern float __builtin_huge_valf(void);\n");
	dmrC_add_pre_buffer(C, "extern long double __builtin_huge_vall(void);\n");
	dmrC_add_pre_buffer(C, "extern double __builtin_inf(void);\n");
	dmrC_add_pre_buffer(C, "extern float __builtin_inff(void);\n");
	dmrC_add_pre_buffer(C, "extern long double __builtin_infl(void);\n");
	dmrC_add_pre_buffer(C, "extern double __builtin_nan(const char *);\n");
	dmrC_add_pre_buffer(C, "extern float __builtin_nanf(const char *);\n");
	dmrC_add_pre_buffer(C, "extern long double __builtin_nanl(const char *);\n");

	/* And some __FORTIFY_SOURCE ones.. */
	dmrC_add_pre_buffer(C, "extern __SIZE_TYPE__ __builtin_object_size(void *, int);\n");
	dmrC_add_pre_buffer(C, "extern void * __builtin___memcpy_chk(void *, const void *, __SIZE_TYPE__, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void * __builtin___memmove_chk(void *, const void *, __SIZE_TYPE__, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void * __builtin___mempcpy_chk(void *, const void *, __SIZE_TYPE__, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void * __builtin___memset_chk(void *, int, __SIZE_TYPE__, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin___sprintf_chk(char *, int, __SIZE_TYPE__, const char *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin___snprintf_chk(char *, __SIZE_TYPE__, int , __SIZE_TYPE__, const char *, ...);\n");
	dmrC_add_pre_buffer(C, "extern char * __builtin___stpcpy_chk(char *, const char *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern char * __builtin___strcat_chk(char *, const char *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern char * __builtin___strcpy_chk(char *, const char *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern char * __builtin___strncat_chk(char *, const char *, __SIZE_TYPE__, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern char * __builtin___strncpy_chk(char *, const char *, __SIZE_TYPE__, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin___vsprintf_chk(char *, int, __SIZE_TYPE__, const char *, __builtin_va_list);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin___vsnprintf_chk(char *, __SIZE_TYPE__, int, __SIZE_TYPE__, const char *, __builtin_va_list ap);\n");
	dmrC_add_pre_buffer(C, "extern void __builtin_unreachable(void);\n");

	/* And some from <stdlib.h> */
	dmrC_add_pre_buffer(C, "extern void __builtin_abort(void);\n");
	dmrC_add_pre_buffer(C, "extern void *__builtin_calloc(__SIZE_TYPE__, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void __builtin_exit(int);\n");
	dmrC_add_pre_buffer(C, "extern void *__builtin_malloc(__SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void *__builtin_realloc(void *, __SIZE_TYPE__);\n");
	dmrC_add_pre_buffer(C, "extern void __builtin_free(void *);\n");

	/* And some from <stdio.h> */
	dmrC_add_pre_buffer(C, "extern int __builtin_printf(const char *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_sprintf(char *, const char *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_snprintf(char *, __SIZE_TYPE__, const char *, ...);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_puts(const char *);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_vprintf(const char *, __builtin_va_list);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_vsprintf(char *, const char *, __builtin_va_list);\n");
	dmrC_add_pre_buffer(C, "extern int __builtin_vsnprintf(char *, __SIZE_TYPE__, const char *, __builtin_va_list ap);\n");
}

void dmrC_create_builtin_stream(struct dmr_C *C)
{
	dmrC_add_pre_buffer(C, "#define __DMR_C__ 1\n");

	dmrC_add_pre_buffer(C, "#weak_define __GNUC__ %d\n", gcc_major);
	dmrC_add_pre_buffer(C, "#weak_define __GNUC_MINOR__ %d\n", gcc_minor);
	dmrC_add_pre_buffer(C, "#weak_define __GNUC_PATCHLEVEL__ %d\n", gcc_patchlevel);

	/* add the multiarch include directories, if any */
	if (C->multiarch_dir && *C->multiarch_dir) {
		dmrC_add_pre_buffer(C, "#add_system \"/usr/include/%s\"\n", C->multiarch_dir);
		dmrC_add_pre_buffer(C, "#add_system \"/usr/local/include/%s\"\n", C->multiarch_dir);
	}

	/* We add compiler headers path here because we have to parse
	* the arguments to get it, falling back to default. */
	if (C->gcc_base_dir && *C->gcc_base_dir) {
		dmrC_add_pre_buffer(C, "#add_system \"%s/include\"\n", C->gcc_base_dir);
		dmrC_add_pre_buffer(C, "#add_system \"%s/include-fixed\"\n", C->gcc_base_dir);
	}

	dmrC_add_pre_buffer(C, "#define __extension__\n");
	dmrC_add_pre_buffer(C, "#define __pragma__\n");
	dmrC_add_pre_buffer(C, "#define _Pragma(x)\n");


	// gcc defines __SIZE_TYPE__ to be size_t.  For linux/i86 and
	// solaris/sparc that is really "unsigned int" and for linux/x86_64
	// it is "long unsigned int".  In either case we can probably
	// get away with this.  We need the #weak_define as cgcc will define
	// the right __SIZE_TYPE__.
	if (C->target->size_t_ctype == &C->S->ullong_ctype)
		dmrC_add_pre_buffer(C, "#weak_define __SIZE_TYPE__ unsigned long long int\n");
	else if (C->target->size_t_ctype == &C->S->ulong_ctype)
		dmrC_add_pre_buffer(C, "#weak_define __SIZE_TYPE__ unsigned long int\n");
	else
		dmrC_add_pre_buffer(C, "#weak_define __SIZE_TYPE__ unsigned int\n");
	dmrC_add_pre_buffer(C, "#weak_define __STDC__ 1\n");

	switch (C->standard)
	{
	case STANDARD_C89:
		dmrC_add_pre_buffer(C, "#weak_define __STRICT_ANSI__\n");
		break;

	case STANDARD_C94:
		dmrC_add_pre_buffer(C, "#weak_define __STDC_VERSION__ 199409L\n");
		dmrC_add_pre_buffer(C, "#weak_define __STRICT_ANSI__\n");
		break;

	case STANDARD_C99:
		dmrC_add_pre_buffer(C, "#weak_define __STDC_VERSION__ 199901L\n");
		dmrC_add_pre_buffer(C, "#weak_define __STRICT_ANSI__\n");
		break;

	case STANDARD_GNU89:
		break;

	case STANDARD_GNU99:
		dmrC_add_pre_buffer(C, "#weak_define __STDC_VERSION__ 199901L\n");
		break;

	case STANDARD_C11:
		dmrC_add_pre_buffer(C, "#weak_define __STRICT_ANSI__ 1\n");

	case STANDARD_GNU11:
		dmrC_add_pre_buffer(C, "#weak_define __STDC_NO_ATOMICS__ 1\n");
		dmrC_add_pre_buffer(C, "#weak_define __STDC_NO_COMPLEX__ 1\n");
		dmrC_add_pre_buffer(C, "#weak_define __STDC_NO_THREADS__ 1\n");
		dmrC_add_pre_buffer(C, "#weak_define __STDC_VERSION__ 201112L\n");
		break;

	default:
		assert(0);
	}

	if (!C->codegen) {
		/* Do not output these macros when we are generating code e.g. via LLVM as these macros are not correct */
		dmrC_add_pre_buffer(C, "#define __builtin_stdarg_start(a,b) ((a) = (__builtin_va_list)(&(b)))\n");
		dmrC_add_pre_buffer(C, "#define __builtin_va_start(a,b) ((a) = (__builtin_va_list)(&(b)))\n");
		dmrC_add_pre_buffer(C, "#define __builtin_ms_va_start(a,b) ((a) = (__builtin_ms_va_list)(&(b)))\n");
		dmrC_add_pre_buffer(C, "#define __builtin_va_arg(arg,type)  ({ type __va_arg_ret = *(type *)(arg); arg += sizeof(type); __va_arg_ret; })\n");
		dmrC_add_pre_buffer(C, "#define __builtin_va_alist (*(void *)0)\n");
		dmrC_add_pre_buffer(C, "#define __builtin_va_arg_incr(x) ((x) + 1)\n");
		dmrC_add_pre_buffer(C, "#define __builtin_va_copy(dest, src) ({ dest = src; (void)0; })\n");
		dmrC_add_pre_buffer(C, "#define __builtin_ms_va_copy(dest, src) ({ dest = src; (void)0; })\n");
		dmrC_add_pre_buffer(C, "#define __builtin_va_end(arg)\n");
		dmrC_add_pre_buffer(C, "#define __builtin_ms_va_end(arg)\n");
		dmrC_add_pre_buffer(C, "#define __builtin_va_arg_pack()\n");
	}

	/* FIXME! We need to do these as special magic macros at expansion time! */
	dmrC_add_pre_buffer(C, "#define __BASE_FILE__ \"base_file.c\"\n");

	if (C->optimize)
		dmrC_add_pre_buffer(C, "#define __OPTIMIZE__ 1\n");
	if (C->optimize_size)
		dmrC_add_pre_buffer(C, "#define __OPTIMIZE_SIZE__ 1\n");

}

static struct symbol_list *sparse_tokenstream(struct dmr_C *C, struct token *token)
{
	int builtin = token && !token->pos.stream;

	// Preprocess the stream
	token = dmrC_preprocess(C, token);
	if (C->die_if_error)
		return NULL;

	if (C->dump_macro_defs && !builtin)
		dmrC_dump_macro_definitions(C);
	
	if (C->preprocess_only) {
		while (!dmrC_eof_token(token)) {
			int prec = 1;
			struct token *next = token->next;
			const char *separator = "";
			if (next->pos.whitespace)
				separator = " ";
			if (next->pos.newline) {
				separator = "\n\t\t\t\t\t";
				prec = next->pos.pos;
				if (prec > 4)
					prec = 4;
			}
			printf("%s%.*s", dmrC_show_token(C, token), prec, separator);
			token = next;
		}
		putchar('\n');

		return NULL;
	}

	// Parse the resulting C code
	while (!dmrC_eof_token(token))
		token = dmrC_external_declaration(C, token, &C->S->translation_unit_used_list, NULL);
	return C->S->translation_unit_used_list;
}

static struct symbol_list *sparse_file(struct dmr_C *C, const char *filename)
{
	int fd;
	struct token *token;

	if (strcmp(filename, "-") == 0) {
		fd = 0;
	} else {
		fd = open(filename, O_RDONLY);
		if (fd < 0)
			dmrC_die(C, "No such file: %s", filename);
	}

	// Tokenize the input stream
	token = dmrC_tokenize(C, filename, fd, NULL, C->T->includepath);
	close(fd);

	return sparse_tokenstream(C, token);
}

/*
* This handles the "-include" directive etc: we're in global
* scope, and all types/macros etc will affect all the following
* files.
*
* NOTE NOTE NOTE! "#undef" of anything in this stage will
* affect all subsequent files too, i.e. we can have non-local
* behaviour between files!
*/
static struct symbol_list *sparse_initial(struct dmr_C *C)
{
	int i;

	// Prepend any "include" file to the stream.
	// We're in global scope, it will affect all files!
	for (i = 0; i < C->cmdline_include_nr; i++)
		dmrC_add_pre_buffer(C, "#argv_include \"%s\"\n", C->cmdline_include[i]);

	return sparse_tokenstream(C, C->pre_buffer_begin);
}

static void protect_token_alloc(struct dmr_C *C) {
	/* prevent tokens from being deallocated ? */
	dmrC_allocator_transfer(&C->token_allocator, &C->protected_token_allocator);
}

static void clear_token_alloc(struct dmr_C *C) {
	/* prevent tokens from being deallocated ? */
	dmrC_allocator_drop_all_allocations(&C->token_allocator);
}

struct symbol_list *dmrC_sparse_initialize(struct dmr_C *C, int argc, char **argv, struct string_list **filelist)
{
	char **args;
	struct symbol_list *list;

	args = argv;
	for (; args < argv + argc;) {
		char *arg = *++args;
		if (!arg)
			break;

		if (arg[0] == '-' && arg[1]) {
			args = handle_switch(C, arg + 1, args);
			continue;
		}
		ptrlist_add((struct ptr_list **)filelist, arg, &C->ptrlist_allocator);
	}
	handle_switch_W_finalize(C);
	handle_switch_v_finalize(C);

	handle_arch_finalize(C);

	list = NULL;
	//if (!ptr_list_empty(filelist)) {

		dmrC_create_builtin_stream(C);
		predefined_macros(C);
		if (!C->preprocess_only)
			dmrC_declare_builtin_functions(C);

		list = sparse_initial(C);

		/*
		* Protect the initial token allocations, since
		* they need to survive all the others
		*/
		protect_token_alloc(C);
	//}
	return list;
}

struct symbol_list * dmrC_sparse_keep_tokens(struct dmr_C *C, char *filename)
{
	struct symbol_list *res;

	/* Clear previous symbol list */
	C->S->translation_unit_used_list = NULL;

	dmrC_new_file_scope(C);
	res = sparse_file(C, filename);

	/* And return it */
	return res;
}


struct symbol_list * dmrC__sparse(struct dmr_C *C, char *filename)
{
	struct symbol_list *res;

	res = dmrC_sparse_keep_tokens(C, filename);

	/* Drop the tokens for this file after parsing */
	clear_token_alloc(C);

	/* And return it */
	return res;
}

struct symbol_list * dmrC_sparse(struct dmr_C *C, char *filename)
{
	struct symbol_list *res = dmrC__sparse(C, filename);

	if (C->has_error & ERROR_CURR_PHASE)
		C->has_error = ERROR_PREV_PHASE;

	/* Evaluate the complete symbol list */
	if (res)
		dmrC_evaluate_symbol_list(C, res);

	return res;
}

struct symbol_list *dmrC_sparse_buffer(struct dmr_C *C, const char *name, char *buffer, int keep_tokens)
{
	struct symbol_list *res;

	/* Clear previous symbol list */
	C->S->translation_unit_used_list = NULL;
	dmrC_new_file_scope(C);

	struct token *start;
	struct token *end;
	start = dmrC_tokenize_buffer_stream(C, name, (unsigned char *)buffer,
				     (unsigned long)strlen(buffer), &end);

	res = sparse_tokenstream(C, start);

	if (!keep_tokens) {
		/* Drop the tokens for this file after parsing */
		clear_token_alloc(C);
	}

	/* Evaluate the complete symbol list */
	if (res)
		dmrC_evaluate_symbol_list(C, res);

	/* And return it */
	return res;
}

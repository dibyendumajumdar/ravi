/*
 * Symbol lookup and handling.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <port.h>
#include <lib.h>
#include <allocate.h>
#include <ptrlist.h>
#include <token.h>
#include <parse.h>
#include <symbol.h>
#include <scope.h>
#include <expression.h>

#include <target.h>

/*
 * If the symbol is an inline symbol, add it to the list of symbols to parse
 */
void dmrC_access_symbol(struct global_symbols_t *S, struct symbol *sym)
{
	if (sym->ctype.modifiers & MOD_INLINE) {
		if (!(sym->ctype.modifiers & MOD_ACCESSED)) {
			dmrC_add_symbol(S->C, &S->translation_unit_used_list, sym);
			sym->ctype.modifiers |= MOD_ACCESSED;
		}
	}
}

struct symbol *dmrC_lookup_symbol(struct ident *ident, enum namespace_type ns)
{
	struct symbol *sym;

	for (sym = ident->symbols; sym; sym = sym->next_id) {
		if (sym->ns & ns) {
			sym->used = 1;
			return sym;
		}
	}
	return NULL;
}

struct context *dmrC_alloc_context(struct global_symbols_t *S)
{
	return (struct context *)dmrC_allocator_allocate(&S->context_allocator, 0);
}

struct symbol *dmrC_alloc_symbol(struct global_symbols_t *S, struct position pos, int type)
{
	struct symbol *sym = (struct symbol *) dmrC_allocator_allocate(&S->symbol_allocator, 0);
	sym->type = type;
	sym->pos = pos;
	sym->endpos.type = 0;
	return sym;
}

struct struct_union_info {
	unsigned long max_align;
	unsigned long bit_size;
	int align_size;
};

/*
 * Unions are fairly easy to lay out ;)
 */
static void lay_out_union(struct global_symbols_t *S, struct symbol *sym, struct struct_union_info *info)
{
	dmrC_examine_symbol_type(S, sym);

	// Unnamed bitfields do not affect alignment.
	if (sym->ident || !dmrC_is_bitfield_type(sym)) {
		if (sym->ctype.alignment > info->max_align)
			info->max_align = sym->ctype.alignment;
	}

	if (sym->bit_size > (int) info->bit_size)
		info->bit_size = sym->bit_size;

	sym->offset = 0;
}

static int bitfield_base_size(struct symbol *sym)
{
	if (sym->type == SYM_NODE)
		sym = sym->ctype.base_type;
	if (sym->type == SYM_BITFIELD)
		sym = sym->ctype.base_type;
	return sym->bit_size;
}

/*
 * Structures are a bit more interesting to lay out
 */
static void lay_out_struct(struct global_symbols_t *S, struct symbol *sym, struct struct_union_info *info)
{
	unsigned long bit_size, align_bit_mask;
	int base_size;

	dmrC_examine_symbol_type(S, sym);

	// Unnamed bitfields do not affect alignment.
	if (sym->ident || !dmrC_is_bitfield_type(sym)) {
		if (sym->ctype.alignment > info->max_align)
			info->max_align = sym->ctype.alignment;
	}

	bit_size = info->bit_size;
	base_size = sym->bit_size; 

	/*
	 * Unsized arrays cause us to not align the resulting
	 * structure size
	 */
	if (base_size < 0) {
		info->align_size = 0;
		base_size = 0;
	}

	align_bit_mask = dmrC_bytes_to_bits(S->C->target, sym->ctype.alignment) - 1;

	/*
	 * Bitfields have some very special rules..
	 */
	if (dmrC_is_bitfield_type (sym)) {
		unsigned long bit_offset = bit_size & align_bit_mask;
		int room = bitfield_base_size(sym) - bit_offset;
		// Zero-width fields just fill up the unit.
		int width = base_size ? base_size : (bit_offset ? room : 0);

		if (width > room) {
			bit_size = (bit_size + align_bit_mask) & ~align_bit_mask;
			bit_offset = 0;
		}
		sym->offset = dmrC_bits_to_bytes(S->C->target, bit_size - bit_offset);
		sym->bit_offset = bit_offset;
		sym->ctype.base_type->bit_offset = bit_offset;
		info->bit_size = bit_size + width;
		// dmrC_warning (sym->pos, "bitfield: offset=%d:%d  size=:%d", sym->offset, sym->bit_offset, width);

		return;
	}

	/*
	 * Otherwise, just align it right and add it up..
	 */
	bit_size = (bit_size + align_bit_mask) & ~align_bit_mask;
	sym->offset = dmrC_bits_to_bytes(S->C->target, bit_size);

	info->bit_size = bit_size + base_size;
	// dmrC_warning (sym->pos, "regular: offset=%d", sym->offset);
}

static struct symbol * examine_struct_union_type(struct global_symbols_t *S, struct symbol *sym, int advance)
{
	struct struct_union_info info = {
		.max_align = 1,
		.bit_size = 0,
		.align_size = 1
	};
	unsigned long bit_size, bit_align;
	void (*fn)(struct global_symbols_t *S, struct symbol *, struct struct_union_info *);
	struct symbol *member;

	fn = advance ? lay_out_struct : lay_out_union;
	FOR_EACH_PTR(sym->symbol_list, member) {
		fn(S, member, &info);
	} END_FOR_EACH_PTR(member); 

	if (!sym->ctype.alignment)
		sym->ctype.alignment = info.max_align;
	bit_size = info.bit_size;
	if (info.align_size) {
		bit_align = dmrC_bytes_to_bits(S->C->target, sym->ctype.alignment)-1;
		bit_size = (bit_size + bit_align) & ~bit_align;
	}
	sym->bit_size = bit_size;
	return sym;
}

static struct symbol *examine_base_type(struct global_symbols_t *S, struct symbol *sym)
{
	struct symbol *base_type;

	/* Check the base type */
	base_type = dmrC_examine_symbol_type(S, sym->ctype.base_type);
	if (!base_type || base_type->type == SYM_PTR)
		return base_type;
	sym->ctype.as |= base_type->ctype.as;
	sym->ctype.modifiers |= base_type->ctype.modifiers & MOD_PTRINHERIT;
	dmrC_concat_context_list(base_type->ctype.contexts,
				 &sym->ctype.contexts);
	if (base_type->type == SYM_NODE) {
		base_type = base_type->ctype.base_type;
		sym->ctype.base_type = base_type;
	}
	return base_type;
}

static struct symbol * examine_array_type(struct global_symbols_t *S, struct symbol *sym)
{
	struct symbol *base_type = examine_base_type(S, sym);
	unsigned int bit_size = -1, alignment;
	struct expression *array_size = sym->array_size;

	if (!base_type)
		return sym;

	if (array_size) {	
		bit_size = (unsigned int) dmrC_array_element_offset(S->C->target, base_type->bit_size,
						(int) dmrC_get_expression_value_silent(S->C, array_size));
		if (array_size->type != EXPR_VALUE) {
			if (S->C->Wvla)
				dmrC_warning(S->C, array_size->pos, "Variable length array is used.");
			bit_size = -1;
		}
	}
	alignment = base_type->ctype.alignment;
	if (!sym->ctype.alignment)
		sym->ctype.alignment = alignment;
	sym->bit_size = bit_size;
	return sym;
}

static struct symbol *examine_bitfield_type(struct global_symbols_t *S, struct symbol *sym)
{
	struct symbol *base_type = examine_base_type(S, sym);
	unsigned long bit_size, alignment, modifiers;

	if (!base_type)
		return sym;
	bit_size = base_type->bit_size;
	if (sym->bit_size > (int) bit_size)
		dmrC_warning(S->C, sym->pos, "impossible field-width, %d, for this type",  sym->bit_size);

	alignment = base_type->ctype.alignment;
	if (!sym->ctype.alignment)
		sym->ctype.alignment = alignment;
	modifiers = base_type->ctype.modifiers;

	/* Bitfields are unsigned, unless the base type was explicitly signed */
	if (!(modifiers & MOD_EXPLICITLY_SIGNED))
		modifiers = (modifiers & ~MOD_SIGNED) | MOD_UNSIGNED;
	sym->ctype.modifiers |= modifiers & MOD_SIGNEDNESS;
	return sym;
}

/*
 * "typeof" will have to merge the types together
 */
void dmrC_merge_type(struct symbol *sym, struct symbol *base_type)
{
	sym->ctype.as |= base_type->ctype.as;
	sym->ctype.modifiers |= (base_type->ctype.modifiers & ~MOD_STORAGE);
	dmrC_concat_context_list(base_type->ctype.contexts,
				 &sym->ctype.contexts);
	sym->ctype.base_type = base_type->ctype.base_type;
	if (sym->ctype.base_type->type == SYM_NODE)
		dmrC_merge_type(sym, sym->ctype.base_type);
}

static int count_array_initializer(struct global_symbols_t *S, struct symbol *t, struct expression *expr)
{
	int nr = 0;
	int is_char = 0;

	/*
	 * Arrays of character types are special; they can be initialized by
	 * string literal _or_ by string literal in braces.  The latter means
	 * that with T x[] = {<string literal>} number of elements in x depends
	 * on T - if it's a character type, we get the length of string literal
	 * (including NUL), otherwise we have one element here.
	 */
	if (t->ctype.base_type == &S->int_type && t->ctype.modifiers & MOD_CHAR)
		is_char = 1;

	switch (expr->type) {
	case EXPR_INITIALIZER: {
		struct expression *entry;
		int count = 0;
		int str_len = 0;
		FOR_EACH_PTR(expr->expr_list, entry) {
			count++;
			switch (entry->type) {
			case EXPR_INDEX:
				if ((int)entry->idx_to >= nr)
					nr = entry->idx_to+1;
				break;
			case EXPR_PREOP: {
				struct expression *e = entry;
				if (is_char) {
					while (e && e->type == EXPR_PREOP && e->op == '(')
						e = e->unop;
					if (e && e->type == EXPR_STRING) {
						entry = e;
			case EXPR_STRING:
						if (is_char)
							str_len = entry->string->length;
					}


				}
			}
			default:
				nr++;
			}
		} END_FOR_EACH_PTR(entry);
		if (count == 1 && str_len)
			nr = str_len;
		break;
	}
	case EXPR_PREOP:
		if (is_char) { 
			struct expression *e = expr;
			while (e && e->type == EXPR_PREOP && e->op == '(')
				e = e->unop;
			if (e && e->type == EXPR_STRING) {
				expr = e;
	case EXPR_STRING:
				if (is_char)
					nr = expr->string->length;
			}
		}
		break;
	default:
		break;
	}
	return nr;
}

static struct expression *get_symbol_initializer(struct symbol *sym)
{
	do {
		if (sym->initializer)
			return sym->initializer;
	} while ((sym = sym->same_symbol) != NULL);
	return NULL;
}
static struct symbol * examine_node_type(struct global_symbols_t *S, struct symbol *sym)
{
	struct symbol *base_type = examine_base_type(S, sym);
	int bit_size;
	unsigned long alignment;

	/* SYM_NODE - figure out what the type of the node was.. */
	bit_size = 0;
	alignment = 0;
	if (!base_type)
		return sym;

	bit_size = base_type->bit_size;
	alignment = base_type->ctype.alignment;

	/* Pick up signedness information into the node */
	sym->ctype.modifiers |= (MOD_SIGNEDNESS & base_type->ctype.modifiers);

	if (!sym->ctype.alignment)
		sym->ctype.alignment = alignment;

	/* Unsized array? The size might come from the initializer.. */
	if (bit_size < 0 && base_type->type == SYM_ARRAY) {
		struct expression *initializer = get_symbol_initializer(sym);
		if (initializer) {
			struct symbol *node_type = base_type->ctype.base_type;
			int count = count_array_initializer(S, node_type, initializer);

			if (node_type && node_type->bit_size >= 0)
				bit_size = (int) dmrC_array_element_offset(S->C->target, node_type->bit_size, count);
			/* Note that the bit_size will be set on parent SYM_NODE rather than here */
			//base_type->bit_size = bit_size;
		}
	}
	
	sym->bit_size = bit_size;
	return sym;
}

static struct symbol *examine_enum_type(struct global_symbols_t *S, struct symbol *sym)
{
	struct symbol *base_type = examine_base_type(S, sym);

	sym->ctype.modifiers |= (base_type->ctype.modifiers & MOD_SIGNEDNESS);
	sym->bit_size = S->C->target->bits_in_enum;
	if (base_type->bit_size > sym->bit_size)
		sym->bit_size = base_type->bit_size;
	sym->ctype.alignment = S->C->target->enum_alignment;
	if (base_type->ctype.alignment > sym->ctype.alignment)
		sym->ctype.alignment = base_type->ctype.alignment;
	return sym;
}

static struct symbol *examine_pointer_type(struct global_symbols_t *S, struct symbol *sym)
{
	/*
	 * We need to set the pointer size first, and
	 * examine the thing we point to only afterwards.
	 * That's because this pointer type may end up
	 * being needed for the base type size evaluation.
	 */
	if (!sym->bit_size)
		sym->bit_size = S->C->target->bits_in_pointer;
	if (!sym->ctype.alignment)
		sym->ctype.alignment = S->C->target->pointer_alignment;
	return sym;
}

/*
 * Fill in type size and alignment information for
 * regular SYM_TYPE things.
 */
struct symbol *dmrC_examine_symbol_type(struct global_symbols_t *S, struct symbol * sym)
{
	if (!sym)
		return sym;

	/* Already done? */
	if (sym->examined)
		return sym;
	sym->examined = 1;

	switch (sym->type) {
	case SYM_FN:
	case SYM_NODE:
		return examine_node_type(S, sym);
	case SYM_ARRAY:
		return examine_array_type(S, sym);
	case SYM_STRUCT:
		return examine_struct_union_type(S, sym, 1);
	case SYM_UNION:
		return examine_struct_union_type(S, sym, 0);
	case SYM_PTR:
		return examine_pointer_type(S, sym);
	case SYM_ENUM:
		return examine_enum_type(S, sym);
	case SYM_BITFIELD:
		return examine_bitfield_type(S, sym);
	case SYM_BASETYPE:
		/* Size and alignment had better already be set up */
		return sym;
	case SYM_TYPEOF: {
		struct symbol *base = dmrC_evaluate_expression(S->C, sym->initializer);
		if (base) {
			unsigned long mod = 0;

			if (dmrC_is_bitfield_type(base))
				dmrC_warning(S->C, base->pos, "typeof applied to bitfield type");
			if (base->type == SYM_NODE) {
				mod |= base->ctype.modifiers & MOD_TYPEOF;
				base = base->ctype.base_type;
			}
			sym->type = SYM_NODE;
			sym->ctype.modifiers = mod;
			sym->ctype.base_type = base;
			return examine_node_type(S, sym);
		}
		break;
	}
	case SYM_PREPROCESSOR:
		dmrC_sparse_error(S->C, sym->pos, "ctype on preprocessor command? (%s)", dmrC_show_ident(S->C, sym->ident));
		return NULL;
	case SYM_UNINITIALIZED:
		dmrC_sparse_error(S->C, sym->pos, "ctype on uninitialized symbol %p", sym);
		return NULL;
	case SYM_RESTRICT:
		examine_base_type(S, sym);
		return sym;
	case SYM_FOULED:
		examine_base_type(S, sym);
		return sym;
	default:
		dmrC_sparse_error(S->C, sym->pos, "Examining unknown symbol type %d", sym->type);
		break;
	}
	return sym;
}

const char* dmrC_get_type_name(enum type type)
{
	const char *type_lookup[] = {
	[SYM_UNINITIALIZED] = "uninitialized",
	[SYM_PREPROCESSOR] = "preprocessor",
	[SYM_BASETYPE] = "basetype",
	[SYM_NODE] = "node",
	[SYM_PTR] = "pointer",
	[SYM_FN] = "function",
	[SYM_ARRAY] = "array",
	[SYM_STRUCT] = "struct",
	[SYM_UNION] = "union",
	[SYM_ENUM] = "enum",
	[SYM_TYPEDEF] = "typedef",
	[SYM_TYPEOF] = "typeof",
	[SYM_MEMBER] = "member",
	[SYM_BITFIELD] = "bitfield",
	[SYM_LABEL] = "label",
	[SYM_RESTRICT] = "restrict",
	[SYM_FOULED] = "fouled",
	[SYM_KEYWORD] = "keyword",
	[SYM_BAD] = "bad"};

	if (type <= SYM_BAD)
		return type_lookup[type];
	else
		return NULL;
}

struct symbol *dmrC_examine_pointer_target(struct global_symbols_t *S, struct symbol *sym)
{
	return examine_base_type(S, sym);
}

void dmrC_create_fouled(struct global_symbols_t *S, struct symbol *type)
{
	if (type->bit_size < S->C->target->bits_in_int) {
		struct symbol *news = dmrC_alloc_symbol(S, type->pos, type->type);
		*news = *type;
		news->bit_size = S->C->target->bits_in_int;
		news->type = SYM_FOULED;
		news->ctype.base_type = type;
		dmrC_add_symbol(S->C, &S->restr, type);
		dmrC_add_symbol(S->C, &S->fouled, news);
	}
}

struct symbol *dmrC_befoul(struct global_symbols_t *S, struct symbol *type)
{
	struct symbol *t1, *t2;
	while (type->type == SYM_NODE)
		type = type->ctype.base_type;
	PREPARE_PTR_LIST(S->restr, t1);
	PREPARE_PTR_LIST(S->fouled, t2);
	for (;;) {
		if (t1 == type)
			return t2;
		if (!t1)
			break;
		NEXT_PTR_LIST(t1);
		NEXT_PTR_LIST(t2);
	}
	FINISH_PTR_LIST(t2);
	FINISH_PTR_LIST(t1);
	return NULL;
}

void dmrC_check_declaration(struct global_symbols_t *S, struct symbol *sym)
{
	int warned = 0;
	struct symbol *next = sym;

	while ((next = next->next_id) != NULL) {
		if (next->ns != sym->ns)
			continue;
		if (sym->scope == next->scope) {
			sym->same_symbol = next;
			return;
		}
		/* Extern in block level matches a TOPLEVEL non-static symbol */
		if (sym->ctype.modifiers & MOD_EXTERN) {
			if ((next->ctype.modifiers & (MOD_TOPLEVEL|MOD_STATIC)) == MOD_TOPLEVEL) {
				sym->same_symbol = next;
				return;
			}
		}


		if (!S->C->Wshadow || warned)
			continue;
		if (dmrC_get_sym_type(next) == SYM_FN)
			continue;
		warned = 1;
		dmrC_warning(S->C, sym->pos, "symbol '%s' shadows an earlier one", dmrC_show_ident(S->C, sym->ident));
		dmrC_info(S->C, next->pos, "originally declared here");
	}
}

void dmrC_bind_symbol(struct global_symbols_t *S, struct symbol *sym, struct ident *ident, enum namespace_type ns)
{
	struct scope *scope;
	if (sym->bound) {
		dmrC_sparse_error(S->C, sym->pos, "internal error: symbol type already bound");
		return;
	}
	if (ident->reserved && (ns & (NS_TYPEDEF | NS_STRUCT | NS_LABEL | NS_SYMBOL))) {
		dmrC_sparse_error(S->C, sym->pos, "Trying to use reserved word '%s' as identifier", dmrC_show_ident(S->C, ident));
		return;
	}
	sym->ns = ns;
	sym->next_id = ident->symbols;
	ident->symbols = sym;
	if (sym->ident && sym->ident != ident)
		dmrC_warning(S->C, sym->pos, "Symbol '%s' already bound", dmrC_show_ident(S->C, sym->ident));
	sym->ident = ident;
	sym->bound = 1;

	scope = S->C->block_scope;
	if (ns == NS_SYMBOL && dmrC_toplevel(S->C, scope)) {
		unsigned mod = MOD_ADDRESSABLE | MOD_TOPLEVEL;

		scope = S->C->global_scope;
		if (sym->ctype.modifiers & MOD_STATIC ||
		    dmrC_is_extern_inline(sym)) {
			scope = S->C->file_scope;
			mod = MOD_TOPLEVEL;
		}
		sym->ctype.modifiers |= mod;
	}
	if (ns == NS_MACRO)
		scope = S->C->file_scope;
	if (ns == NS_LABEL)
		scope = S->C->function_scope;
	dmrC_bind_scope(S->C, sym, scope);
}

struct symbol *dmrC_create_symbol(struct global_symbols_t *S, int stream, const char *name, int type, int ns)
{
	struct ident *ident = dmrC_built_in_ident(S->C, name);
	struct symbol *sym = dmrC_lookup_symbol(ident, ns);

	if (sym && sym->type != type)
		dmrC_die(S->C, "symbol %s created with different types: %d old %d", name,
				type, sym->type);

	if (!sym) {
		struct token *token = dmrC_built_in_token(S->C, stream, ident);

		sym = dmrC_alloc_symbol(S, token->pos, type);
		dmrC_bind_symbol(S, sym, token->ident, ns);
	}
	return sym;
}

void dmrC_init_symbols(struct dmr_C *C)
{
	struct global_symbols_t *S = (struct global_symbols_t *) calloc(1, sizeof(struct global_symbols_t));
	C->S = S;
	S->C = C; 
	dmrC_allocator_init(&S->context_allocator, "contexts", sizeof(struct context), __alignof__(struct context),
		CHUNK);
	dmrC_allocator_init(&S->global_ident_allocator, "global_identifiers", sizeof(struct ident),
		__alignof__(struct ident), CHUNK);
	dmrC_allocator_init(&S->symbol_allocator, "symbols", sizeof(struct symbol),
		__alignof__(struct symbol), CHUNK);

	int stream = dmrC_init_stream(C, "builtin", -1, C->T->includepath);

#define __INIT_IDENT(n, str, res) (struct ident *) dmrC_allocator_allocate(&S->global_ident_allocator, sizeof(str)); S->n->len = sizeof(str)-1; memcpy(S->n->name, str, sizeof(str)); S->n->reserved = res;
#define __IDENT(n,str,res) \
	{S->n  = __INIT_IDENT(n, str, res)}
#include "ident-list.h"

#define __IDENT(n,str,res) \
	dmrC_hash_ident(C, S->n)
#include "ident-list.h"

	dmrC_init_parser(C, stream);
	dmrC_init_builtins(C, stream);
}

void dmrC_destroy_symbols(struct dmr_C *C) {
	/* tokenizer must be destroyed before this */
	assert(C->T == NULL);

	dmrC_destroy_parser(C);
	assert(C->P == NULL);

	struct global_symbols_t *S = C->S;

	dmrC_allocator_destroy(&S->context_allocator);
	dmrC_allocator_destroy(&S->global_ident_allocator);
	dmrC_allocator_destroy(&S->symbol_allocator);

	free(S);
	C->S = NULL;
}

void dmrC_init_ctype(struct dmr_C *C)
{
	assert(C);
	struct global_symbols_t *S = C->S;
	assert(S);
	struct target_t *T = C->target;
	assert(T);
	const struct ctype_declare *ctype;

#ifdef _MSC_VER
	if (sizeof(long long) == sizeof(size_t)) {
		T->size_t_ctype = &S->ullong_ctype;
		T->ssize_t_ctype = &S->llong_ctype;
	}
	else {
		assert(sizeof(int) == sizeof(size_t));
		T->size_t_ctype = &S->uint_ctype;
		T->ssize_t_ctype = &S->int_ctype;
	}
#else
	T->size_t_ctype = &S->uint_ctype;
	T->ssize_t_ctype = &S->int_ctype;
#endif

#define MOD_ESIGNED (MOD_SIGNED | MOD_EXPLICITLY_SIGNED)
#define MOD_LL (MOD_LONG | MOD_LONGLONG)
#define MOD_LLL MOD_LONGLONGLONG
	const struct ctype_declare {
		struct symbol *ptr;
		enum type type;
		unsigned long modifiers;
		int *bit_size;
		int *maxalign;
		struct symbol *base_type;
	} ctype_declaration[] = {
		{ &S->bool_ctype,	    SYM_BASETYPE, MOD_UNSIGNED,		    &T->bits_in_bool,	     &T->max_int_alignment, &S->int_type },
		{ &S->void_ctype,	    SYM_BASETYPE, 0,			    NULL,	     NULL,		 NULL },
		{ &S->type_ctype,	    SYM_BASETYPE, MOD_TYPE,		    NULL,		     NULL,		 NULL },
		{ &S->incomplete_ctype,SYM_BASETYPE, 0,			    NULL,		     NULL,		 NULL },
		{ &S->bad_ctype,	    SYM_BASETYPE, 0,			    NULL,		     NULL,		 NULL },

		{ &S->char_ctype,	    SYM_BASETYPE, MOD_SIGNED | MOD_CHAR,    &T->bits_in_char,	     &T->max_int_alignment, &S->int_type },
		{ &S->schar_ctype,	    SYM_BASETYPE, MOD_ESIGNED | MOD_CHAR,   &T->bits_in_char,	     &T->max_int_alignment, &S->int_type },
		{ &S->uchar_ctype,	    SYM_BASETYPE, MOD_UNSIGNED | MOD_CHAR,  &T->bits_in_char,	     &T->max_int_alignment, &S->int_type },
		{ &S->short_ctype,	    SYM_BASETYPE, MOD_SIGNED | MOD_SHORT,   &T->bits_in_short,	     &T->max_int_alignment, &S->int_type },
		{ &S->sshort_ctype,    SYM_BASETYPE, MOD_ESIGNED | MOD_SHORT,  &T->bits_in_short,	     &T->max_int_alignment, &S->int_type },
		{ &S->ushort_ctype,    SYM_BASETYPE, MOD_UNSIGNED | MOD_SHORT, &T->bits_in_short,	     &T->max_int_alignment, &S->int_type },
		{ &S->int_ctype,	    SYM_BASETYPE, MOD_SIGNED,		    &T->bits_in_int,	     &T->max_int_alignment, &S->int_type },
		{ &S->sint_ctype,	    SYM_BASETYPE, MOD_ESIGNED,		    &T->bits_in_int,	     &T->max_int_alignment, &S->int_type },
		{ &S->uint_ctype,	    SYM_BASETYPE, MOD_UNSIGNED,		    &T->bits_in_int,	     &T->max_int_alignment, &S->int_type },
		{ &S->long_ctype,	    SYM_BASETYPE, MOD_SIGNED | MOD_LONG,    &T->bits_in_long,	     &T->max_int_alignment, &S->int_type },
		{ &S->slong_ctype,	    SYM_BASETYPE, MOD_ESIGNED | MOD_LONG,   &T->bits_in_long,	     &T->max_int_alignment, &S->int_type },
		{ &S->ulong_ctype,	    SYM_BASETYPE, MOD_UNSIGNED | MOD_LONG,  &T->bits_in_long,	     &T->max_int_alignment, &S->int_type },
		{ &S->llong_ctype,	    SYM_BASETYPE, MOD_SIGNED | MOD_LL,	    &T->bits_in_longlong,       &T->max_int_alignment, &S->int_type },
		{ &S->sllong_ctype,    SYM_BASETYPE, MOD_ESIGNED | MOD_LL,	    &T->bits_in_longlong,       &T->max_int_alignment, &S->int_type },
		{ &S->ullong_ctype,    SYM_BASETYPE, MOD_UNSIGNED | MOD_LL,    &T->bits_in_longlong,       &T->max_int_alignment, &S->int_type },
		{ &S->lllong_ctype,    SYM_BASETYPE, MOD_SIGNED | MOD_LLL,	    &T->bits_in_longlonglong,   &T->max_int_alignment, &S->int_type },
		{ &S->slllong_ctype,   SYM_BASETYPE, MOD_ESIGNED | MOD_LLL,    &T->bits_in_longlonglong,   &T->max_int_alignment, &S->int_type },
		{ &S->ulllong_ctype,   SYM_BASETYPE, MOD_UNSIGNED | MOD_LLL,   &T->bits_in_longlonglong,   &T->max_int_alignment, &S->int_type },

		{ &S->float_ctype,	    SYM_BASETYPE,  0,			    &T->bits_in_float,          &T->max_fp_alignment,  &S->fp_type },
		{ &S->double_ctype,    SYM_BASETYPE, MOD_LONG,		    &T->bits_in_double,         &T->max_fp_alignment,  &S->fp_type },
		{ &S->ldouble_ctype,   SYM_BASETYPE, MOD_LONG | MOD_LONGLONG,  &T->bits_in_longdouble,     &T->max_fp_alignment,  &S->fp_type },

		{ &S->string_ctype,    SYM_PTR,	  0,			    &T->bits_in_pointer,        &T->pointer_alignment, &S->char_ctype },
		{ &S->ptr_ctype,	    SYM_PTR,	  0,			    &T->bits_in_pointer,        &T->pointer_alignment, &S->void_ctype },
		{ &S->null_ctype,	    SYM_PTR,	  0,			    &T->bits_in_pointer,        &T->pointer_alignment, &S->void_ctype },
		{ &S->label_ctype,	    SYM_PTR,	  0,			    &T->bits_in_pointer,        &T->pointer_alignment, &S->void_ctype },
		{ &S->lazy_ptr_ctype,  SYM_PTR,	  0,			    &T->bits_in_pointer,        &T->pointer_alignment, &S->void_ctype },
		{ NULL, SYM_UNINITIALIZED, 0, NULL, NULL, NULL }
	};
#undef MOD_LLL
#undef MOD_LL
#undef MOD_ESIGNED


	for (ctype = ctype_declaration ; ctype->ptr; ctype++) {
		struct symbol *sym = ctype->ptr;
		unsigned long bit_size = ctype->bit_size ? *ctype->bit_size : -1;
		unsigned long maxalign = ctype->maxalign ? *ctype->maxalign : 0;
		unsigned long alignment = dmrC_bits_to_bytes(T, bit_size);

		if (alignment > maxalign)
			alignment = maxalign;
		sym->type = ctype->type;
		sym->bit_size = bit_size;
		sym->ctype.alignment = alignment;
		sym->ctype.base_type = ctype->base_type;
		sym->ctype.modifiers = ctype->modifiers;
	}

	S->typenames[0].sym = &S->char_ctype; S->typenames[0].name = "char";
	S->typenames[1].sym = &S->schar_ctype; S->typenames[1].name = "signed char";
	S->typenames[2].sym = &S->uchar_ctype; S->typenames[2].name = "unsigned char";
	S->typenames[3].sym = &S->short_ctype; S->typenames[3].name = "short";
	S->typenames[4].sym = &S->sshort_ctype; S->typenames[4].name = "signed short";
	S->typenames[5].sym = &S->ushort_ctype; S->typenames[5].name = "unsigned short";
	S->typenames[6].sym = &S->int_ctype; S->typenames[6].name = "int";
	S->typenames[7].sym = &S->sint_ctype; S->typenames[7].name = "signed int";
	S->typenames[8].sym = &S->uint_ctype; S->typenames[8].name = "unsigned int";
	S->typenames[9].sym = &S->slong_ctype; S->typenames[9].name = "signed long";
	S->typenames[10].sym = &S->long_ctype; S->typenames[10].name = "long";
	S->typenames[11].sym = &S->ulong_ctype; S->typenames[11].name = "unsigned long";
	S->typenames[12].sym = &S->llong_ctype; S->typenames[12].name = "long long";
	S->typenames[13].sym = &S->sllong_ctype; S->typenames[13].name = "signed long long";
	S->typenames[14].sym = &S->ullong_ctype; S->typenames[14].name = "unsigned long long";
	S->typenames[15].sym = &S->lllong_ctype; S->typenames[15].name = "long long long";
	S->typenames[16].sym = &S->slllong_ctype; S->typenames[16].name = "signed long long long";
	S->typenames[17].sym = &S->ulllong_ctype; S->typenames[17].name = "unsigned long long long";

	S->typenames[18].sym = &S->void_ctype; S->typenames[18].name = "void";
	S->typenames[19].sym = &S->bool_ctype; S->typenames[19].name = "bool";
	S->typenames[20].sym = &S->string_ctype; S->typenames[20].name = "string";

	S->typenames[21].sym = &S->float_ctype; S->typenames[21].name = "float";
	S->typenames[22].sym = &S->double_ctype; S->typenames[22].name = "double";
	S->typenames[23].sym = &S->ldouble_ctype; S->typenames[23].name = "long double";
	S->typenames[24].sym = &S->incomplete_ctype; S->typenames[24].name = "incomplete type";
	S->typenames[25].sym = &S->int_type; S->typenames[25].name = "abstract int";
	S->typenames[26].sym = &S->fp_type; S->typenames[26].name = "abstract fp";
	S->typenames[27].sym = &S->label_ctype; S->typenames[27].name = "label type";
	S->typenames[28].sym = &S->bad_ctype; S->typenames[28].name = "bad type";
	S->typenames[29].sym = NULL, S->typenames[29].name = NULL;
}


/*
 * Sparse c2xml
 *
 * Dumps the parse tree as an xml document
 *
 * Copyright (C) 2007 Rob Taylor
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
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allocate.h>
#include <dmr_c.h>
#include <expression.h>
#include <parse.h>
#include <port.h>
#include <symbol.h>
#include <token.h>
#include <scope.h>
#include <lua.h>
#include <lauxlib.h>

struct parser_state {
    lua_State *L;
    struct dmr_C *C;
    struct allocator int_allocator;
    int idcount;
    int tabidx;
    int stack[30];
    int stackpos;
};

static void push_tabindex(struct parser_state *S) 
{
    assert(S->stackpos >= -1 && S->stackpos < 28);
    S->stackpos++;
    S->stack[S->stackpos] = S->tabidx;
    S->tabidx = 1;
}

static void pop_tabindex(struct parser_state *S)
{
    assert(S->stackpos >= 0);
    S->tabidx = S->stack[S->stackpos];
    S->stackpos--;
}

static void *visited(struct parser_state *S, int id) {
	int *p = (int *)dmrC_allocator_allocate(&S->int_allocator, 0);
	*p = id;
	return p;
}

static void examine_symbol(struct parser_state *S, struct symbol *sym);

static void newProp(struct parser_state *S, const char *name, const char *value)
{
    lua_pushstring(S->L, name);
    lua_pushstring(S->L, value);
    lua_settable(S->L, -3);
}

static void newNumProp(struct parser_state *S, const char *name, int value)
{
    lua_pushstring(S->L, name);
    lua_pushinteger(S->L, value);
    lua_settable(S->L, -3);
}

static void newIdProp(struct parser_state *S, const char *name, unsigned int id)
{
	char buf[256];
	snprintf(buf, 256, "_%d", id);
	newProp(S, name, buf);
}

static void popNamedTable(struct parser_state *S, const char *name) 
{
    lua_assert(lua_istable(S->L, -1));
    lua_assert(lua_istable(S->L, -2));
    pop_tabindex(S);
    lua_pushstring(S->L, name);
    lua_pushvalue(S->L, -2); /* top table */
    lua_settable(S->L, -4); /* bottom table */
    lua_pop(S->L, 1);   /* pop top table */
}

static void popTable(struct parser_state *S)
{
    lua_assert(lua_istable(S->L, -1));
    lua_assert(lua_istable(S->L, -2));
    pop_tabindex(S);
    int id = S->tabidx++;
    lua_seti(S->L, -2, id); /* bottom table */
}

static void new_sym_node(struct parser_state *S, struct symbol *sym, const char *name)
{
	const char *ident = dmrC_show_ident(S->C, sym->ident);

	assert(name != NULL);
	assert(sym != NULL);

    lua_newtable(S->L);
    push_tabindex(S);
    newNumProp(S, "id", S->idcount);
	newProp(S, "type", name);
	
	if (sym->ident && ident)
		newProp(S, "ident", ident);
	newProp(S, "file", dmrC_stream_name(S->C, sym->pos.stream));

	newNumProp(S, "startline", sym->pos.line);
	newNumProp(S, "startcol", sym->pos.pos);

	if (sym->endpos.type) {
		newNumProp(S, "endline", sym->endpos.line);
		newNumProp(S, "endcol", sym->endpos.pos);
		if (sym->pos.stream != sym->endpos.stream)
			newProp(S, "endfile", dmrC_stream_name(S->C, sym->endpos.stream));
        }
	sym->aux = visited(S, S->idcount);

	S->idcount++;
}

static void examine_members(struct parser_state *S, struct ptr_list *list)
{
	struct symbol *sym;

	FOR_EACH_PTR(list, sym) {
		examine_symbol(S, sym);
	} END_FOR_EACH_PTR(sym);
}

static void examine_modifiers(struct parser_state *S, struct symbol *sym)
{
	struct mod_name {
		unsigned long mod;
		const char *name;
	} * m;

	static struct mod_name mod_names[] = {
	    {MOD_AUTO, "auto"},
	    {MOD_REGISTER, "register"},
	    {MOD_STATIC, "static"},
	    {MOD_EXTERN, "extern"},
	    {MOD_CONST, "const"},
	    {MOD_VOLATILE, "volatile"},
	    {MOD_SIGNED, "signed"},
	    {MOD_UNSIGNED, "unsigned"},
	    {MOD_CHAR, "char"},
	    {MOD_SHORT, "short"},
	    {MOD_LONG, "long"},
	    {MOD_LONGLONG, "long long"},
	    {MOD_LONGLONGLONG, "long long long"},
	    {MOD_TYPEDEF, "typedef"},
	    {MOD_TLS, "tls"},
	    {MOD_INLINE, "inline"},
	    {MOD_ADDRESSABLE, "addressable"},
	    {MOD_NOCAST, "nocast"},
	    {MOD_NODEREF, "noderef"},
	    {MOD_ACCESSED, "accessed"},
	    {MOD_TOPLEVEL, "toplevel"},
	    {MOD_ASSIGNED, "assigned"},
	    {MOD_TYPE, "type"},
	    {MOD_SAFE, "safe"},
	    {MOD_USERTYPE, "usertype"},
	    {MOD_NORETURN, "noreturn"},
	    {MOD_EXPLICITLY_SIGNED, "explicitly-signed"},
	    {MOD_BITWISE, "bitwise"},
	    {MOD_PURE, "pure"},
	};

	if (sym->ns != NS_SYMBOL)
		return;

	for (int i = 0; i < ARRAY_SIZE(mod_names); i++) {
		m = mod_names + i;
		if (sym->ctype.modifiers & m->mod) {
			newNumProp(S, m->name, 1);
		}
	}

    if (dmrC_is_prototype(sym))
        newNumProp(S, "prototype", 1);
}

static void
examine_layout(struct parser_state *S, struct symbol *sym)
{
	dmrC_examine_symbol_type(S->C->S, sym);

	newNumProp(S, "bitsize", sym->bit_size);
	newNumProp(S, "alignment", sym->ctype.alignment);
	newNumProp(S, "offset", sym->offset);
	if (dmrC_is_bitfield_type(sym)) {
		newNumProp(S, "bitoffset", sym->bit_offset);
	}
}

static void examine_symbol(struct parser_state *S, struct symbol *sym)
{
	const char *base;
	int array_size;

	if (!sym)
		return;
	if (sym->aux)		/*already visited */
		return;

	if (sym->ident && sym->ident->reserved)
		return;

    int savedpos = lua_gettop(S->L);
	new_sym_node(S, sym, dmrC_get_type_name(sym->type));
	examine_modifiers(S, sym);
	examine_layout(S, sym);

	if (sym->ctype.base_type) {
		if ((base = dmrC_builtin_typename(S->C, sym->ctype.base_type)) == NULL) {
			if (!sym->ctype.base_type->aux) {
				examine_symbol(S, sym->ctype.base_type);
			}
			newNumProp(S, "basetype", *((int *)sym->ctype.base_type->aux));
		} else {
			newProp(S, "builtintype", base);
		}
	}
	if (sym->array_size) {
		/* TODO: modify dmrC_get_expression_value to give error return */
		array_size = dmrC_get_expression_value(S->C, sym->array_size);
		newNumProp(S, "arraysize", array_size);
	}


	switch (sym->type) {
	case SYM_STRUCT:
	case SYM_UNION:
		examine_members(S, sym->symbol_list);
		break;
    case SYM_FN: {
        int savedpos2 = lua_gettop(S->L);
        lua_newtable(S->L);
        push_tabindex(S);
        examine_members(S, sym->arguments);
        popNamedTable(S, "arguments");
        //examine_members(C, sym->symbol_list);
        lua_assert(savedpos2 == lua_gettop(S->L));
        break;
    }
	case SYM_UNINITIALIZED:
		newProp(S, "builtintype", dmrC_builtin_typename(S->C, sym));
		break;
	default:
		break;
	}
    popTable(S);

    lua_assert(savedpos == lua_gettop(S->L));
    return;
}

static struct position *get_expansion_end (struct token *token)
{
	struct token *p1, *p2;

	for (p1=NULL, p2=NULL;
	     !dmrC_eof_token(token);
	     p2 = p1, p1 = token, token = token->next);

	if (p2)
		return &(p2->pos);
	else
		return NULL;
}

static void examine_macro(struct parser_state *S, struct symbol *sym)
{
	struct position *pos;

	/* this should probably go in the main codebase*/
	pos = get_expansion_end(sym->expansion);
	if (pos)
		sym->endpos = *pos;
	else
		sym->endpos = sym->pos;

	new_sym_node(S, sym, "macro");
}

static void examine_namespace(struct parser_state *S, struct symbol *sym)
{
	if (sym->ident && sym->ident->reserved)
		return;

	switch(sym->ns) {
	case NS_MACRO:
		examine_macro(S, sym);
		break;
	case NS_TYPEDEF:
	case NS_STRUCT:
	case NS_SYMBOL:
		examine_symbol(S, sym);
		break;
	case NS_NONE:
	case NS_LABEL:
	case NS_ITERATOR:
	case NS_UNDEF:
	case NS_PREPROCESSOR:
	case NS_KEYWORD:
		break;
	default:
        break;
		//dmrC_die(S->C, "Unrecognised namespace type %d",sym->ns);
	}

}

static int get_stream_id (struct dmr_C *C, const char *name)
{
	int i;
	for (i=0; i<C->T->input_stream_nr; i++) {
		if (strcmp(name, dmrC_stream_name(C, i))==0)
			return i;
	}
	return -1;
}

static void examine_symbol_list(struct parser_state *S, int stream_id, struct ptr_list *list)
{
	struct symbol *sym;
	if (!list)
		return;
	FOR_EACH_PTR(list, sym) {
		if (sym->pos.stream == stream_id)
			examine_namespace(S, sym);
	} END_FOR_EACH_PTR(sym);
}

static int dmrC_getsymbols(lua_State *L)
{
	struct ptr_list *symlist;
	struct ptr_list *filelist = NULL;
	char *file;

	struct dmr_C *C = new_dmr_C();

    const char *codebuffer = luaL_checkstring(L, 1);
    char *argv[] = { NULL };
    int argc = 0;

    symlist = dmrC_sparse_initialize(C, argc, argv, &filelist);

    struct parser_state parser_state = {
        .L = L,
        .C = C,
        .idcount = 0,
        .stack = {0},
        .stackpos = -1,
        .tabidx = 1
    };
    dmrC_allocator_init(&parser_state.int_allocator, "integers", sizeof(int), __alignof__(int), CHUNK);

    lua_newtable(L);
    int luastack = lua_gettop(L);
    push_tabindex(&parser_state);
#if 0
	FOR_EACH_PTR(filelist, file) {
        int fd = get_stream_id(C, file);
		examine_symbol_list(C, fd, symlist);
		dmrC_sparse_keep_tokens(C, file);
		examine_symbol_list(C, fd, C->file_scope->symbols);
		examine_symbol_list(C, fd, C->global_scope->symbols);
	} END_FOR_EACH_PTR(file);
#endif
    char *buffer = strdup(codebuffer);
    if (dmrC_sparse_buffer(C, "buffer", buffer, 1)) {
        int fd = get_stream_id(C, "buffer");
        examine_symbol_list(&parser_state, fd, C->file_scope->symbols);
        examine_symbol_list(&parser_state, fd, C->global_scope->symbols);
    }
    free(buffer);

	destroy_dmr_C(C);
    dmrC_allocator_destroy(&parser_state.int_allocator);

    lua_assert(luastack == lua_gettop(L));

	return 1;
}

static const luaL_Reg dmrclib[] = {
    { "getsymbols", dmrC_getsymbols },{ NULL, NULL } };

LUAMOD_API int raviopen_dmrcluaapi(lua_State *L) {
    luaL_newlib(L, dmrclib);
    return 1;
}

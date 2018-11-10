/*
 * The goal of this module is to extract Symbols from C code and 
 * and present it to Ravi (Lua) code. The extracted symbols are saved into
 * tables.
 * 
 * This tool is based upon Sparse c2xml
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

#define LUA_LIB

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

struct parser_state {
  lua_State *L;
  struct dmr_C *C;
  int idcount;
  int tabidx;
  int stack[30];
  int stackpos;
};

static void push_tabindex(struct parser_state *S) {
  assert(S->stackpos >= -1 && S->stackpos < 28);
  S->stackpos++;
  S->stack[S->stackpos] = S->tabidx;
  S->tabidx = 1;
}

static void pop_tabindex(struct parser_state *S) {
  assert(S->stackpos >= 0);
  S->tabidx = S->stack[S->stackpos];
  S->stackpos--;
}

static void *visited(struct parser_state *S, int id) {
        (void) S;
	return (void *)(intptr_t)id;
}

static void examine_symbol(struct parser_state *S, struct symbol *sym, const char *name);

static void newProp(struct parser_state *S, const char *name,
                    const char *value) {
  lua_pushstring(S->L, name);
  lua_pushstring(S->L, value);
  lua_settable(S->L, -3);
}

static void newNumProp(struct parser_state *S, const char *name, intptr_t value) {
  lua_pushstring(S->L, name);
  lua_pushinteger(S->L, value);
  lua_settable(S->L, -3);
}

static void popNamedTable(struct parser_state *S, const char *name) {
  lua_assert(lua_istable(S->L, -1));
  lua_assert(lua_istable(S->L, -2));
  pop_tabindex(S);
  lua_pushstring(S->L, name);
  lua_pushvalue(S->L, -2); /* top table */
  lua_settable(S->L, -4);  /* bottom table */
  lua_pop(S->L, 1);        /* pop top table */
}

static void popTable(struct parser_state *S) {
  lua_assert(lua_istable(S->L, -1));
  lua_assert(lua_istable(S->L, -2));
  pop_tabindex(S);
  int id = S->tabidx++;
  lua_seti(S->L, -2, id); /* bottom table */
}

static void new_sym_node(struct parser_state *S, struct symbol *sym,
                         const char *name) {
  const char *ident = dmrC_show_ident(S->C, sym->ident);

  assert(name != NULL);
  assert(sym != NULL);

  lua_newtable(S->L);
  push_tabindex(S);
  newNumProp(S, "symbol_id", S->idcount);
  newProp(S, "type", name);

  if (sym->ident && ident) newProp(S, "ident", ident);
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

static void examine_members(struct parser_state *S, struct symbol_list *list) {
  struct symbol *sym;

  FOR_EACH_PTR(list, sym) { examine_symbol(S, sym, NULL); }
  END_FOR_EACH_PTR(sym);
}

static void examine_modifiers(struct parser_state *S, struct symbol *sym) {
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

  if (sym->ns != NS_SYMBOL) return;

  for (int i = 0; i < (int)ARRAY_SIZE(mod_names); i++) {
    m = mod_names + i;
    if (sym->ctype.modifiers & m->mod) { newNumProp(S, m->name, 1); }
  }

  if (dmrC_is_prototype(sym)) newNumProp(S, "prototype", 1);
}

static void examine_layout(struct parser_state *S, struct symbol *sym) {
  dmrC_examine_symbol_type(S->C->S, sym);

  newNumProp(S, "bitsize", sym->bit_size);
  newNumProp(S, "alignment", sym->ctype.alignment);
  newNumProp(S, "offset", sym->offset);
  if (dmrC_is_bitfield_type(sym)) {
    newNumProp(S, "bitoffset", sym->bit_offset);
  }
}

static void examine_symbol(struct parser_state *S, struct symbol *sym,
                           const char *name) {
  const char *base;
  int array_size;

  if (!sym) return;
  if (sym->aux) /*already visited */
    return;

  if (sym->ident && sym->ident->reserved) return;

  int savedpos = lua_gettop(S->L); // For Lua stack validation
  new_sym_node(S, sym, dmrC_get_type_name(sym->type));
  examine_modifiers(S, sym);
  examine_layout(S, sym);

  if (sym->ctype.base_type) {
    if ((base = dmrC_builtin_typename(S->C, sym->ctype.base_type)) == NULL) {
      if (!sym->ctype.base_type->aux) {
        examine_symbol(S, sym->ctype.base_type, "basetype");
      }
      else {
        newNumProp(S, "basetype", (intptr_t)sym->ctype.base_type->aux);
      }
    }
    else {
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
    case SYM_UNION: {
      int savedpos2 = lua_gettop(S->L); // For Lua stack validation
      lua_newtable(S->L);
      push_tabindex(S);
      examine_members(S, sym->symbol_list);
      popNamedTable(S, "members");
      lua_assert(savedpos2 == lua_gettop(S->L));
      break;
    }
    case SYM_FN: {
      int savedpos2 = lua_gettop(S->L); // For Lua stack validation
      lua_newtable(S->L);
      push_tabindex(S);
      examine_members(S, sym->arguments);
      popNamedTable(S, "arguments");
      lua_assert(savedpos2 == lua_gettop(S->L));
      break;
    }
    case SYM_UNINITIALIZED:
      newProp(S, "builtintype", dmrC_builtin_typename(S->C, sym));
      break;
    default: break;
  }
  if (name)
    popNamedTable(S, name);
  else
    popTable(S);

  lua_assert(savedpos == lua_gettop(S->L));
  return;
}

static struct position *get_expansion_end(struct token *token) {
  struct token *p1, *p2;

  for (p1 = NULL, p2 = NULL; !dmrC_eof_token(token);
       p2 = p1, p1 = token, token = token->next)
    ;

  if (p2)
    return &(p2->pos);
  else
    return NULL;
}

static void examine_macro(struct parser_state *S, struct symbol *sym) {
  struct position *pos;

  /* this should probably go in the main codebase*/
  pos = get_expansion_end(sym->expansion);
  if (pos)
    sym->endpos = *pos;
  else
    sym->endpos = sym->pos;

  new_sym_node(S, sym, "macro");
  popTable(S);
}

static void examine_namespace(struct parser_state *S, struct symbol *sym) {
  if (sym->ident && sym->ident->reserved) return;

  switch (sym->ns) {
    case NS_MACRO: examine_macro(S, sym); break;
    case NS_TYPEDEF:
    case NS_STRUCT:
    case NS_SYMBOL: examine_symbol(S, sym, NULL); break;
    default:
      break;
  }
}

static int get_stream_id(struct dmr_C *C, const char *name) {
  int i;
  for (i = 0; i < C->T->input_stream_nr; i++) {
    if (strcmp(name, dmrC_stream_name(C, i)) == 0) return i;
  }
  return -1;
}


static void examine_symbol_list(struct parser_state *S, int stream_id,
                                struct symbol_list *list) {
  struct symbol *sym;
  if (!list) return;
  // clean_up_symbols(S->C, list);
  FOR_EACH_PTR(list, sym) {
    if (sym->pos.stream == stream_id) examine_namespace(S, sym);
  }
  END_FOR_EACH_PTR(sym);
}

/*
Parses supplied C code and returns the parsed symbols as a table.
C code may contain preprocessor macros. 

TODO: allow Lua/Ravi to specify additional command line parameters.
*/
static int dmrC_getsymbols(lua_State *L) {
  struct symbol_list *symlist;
  struct string_list *filelist = NULL;

  const char *codebuffer = luaL_checkstring(L, 1);
  char *argv[] = {NULL};
  int argc = 0;
  int retval = 0;

  char *buffer = strdup(codebuffer);
  struct dmr_C *C = new_dmr_C();
  if (!setjmp(C->jmpbuf)) {
    symlist = dmrC_sparse_initialize(C, argc, argv, &filelist);
    struct parser_state parser_state = {.L = L, .C = C, .idcount = 1, .stack = {0}, .stackpos = -1, .tabidx = 1};
    lua_newtable(L);
    int luastack = lua_gettop(L);
    push_tabindex(&parser_state);
    dmrC_sparse_buffer(C, "buffer", buffer, 1);
    int fd = get_stream_id(C, "buffer");
    examine_symbol_list(&parser_state, fd, C->file_scope->symbols);
    examine_symbol_list(&parser_state, fd, C->global_scope->symbols);
    lua_assert(luastack == lua_gettop(L));
    retval = 1;
  }
  else {
    retval = 0;
  }
  free(buffer);
  destroy_dmr_C(C);
  return retval;
}

extern int ravi_compile_C(lua_State *L);

static const luaL_Reg dmrclib[] = {{"getsymbols", dmrC_getsymbols},
                                   {"compileC", ravi_compile_C},
                                   {NULL, NULL}};

LUAMOD_API int raviopen_dmrcluaapi(lua_State *L) {
  luaL_newlib(L, dmrclib);
  return 1;
}

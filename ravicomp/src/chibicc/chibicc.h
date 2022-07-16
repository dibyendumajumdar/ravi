/*
Adapted from https://github.com/rui314/chibicc

MIT License

Copyright (c) 2019 Rui Ueyama

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef CHIBICC_H
#define CHIBICC_H

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define RAVI_EXTENSIONS

#define MAX(x, y) ((x) < (y) ? (y) : (x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#ifndef __GNUC__
# define __attribute__(x)
#endif
#ifdef _WIN32
# define noreturn
# define strncasecmp strnicmp
# define strndup _strndup
#else
#include <stdnoreturn.h>
#endif

typedef struct C_Type C_Type;
typedef struct C_Node C_Node;
typedef struct C_Member C_Member;
typedef struct C_Relocation C_Relocation;
typedef struct C_Hideset C_Hideset;
typedef struct C_Parser C_Parser;

#ifndef C_MEMORYALLOCATOR_DEFINED
#define C_MEMORYALLOCATOR_DEFINED
/*
 * Note that this struct below is also defined in allocate.h/ravi_compiler.h and all
 * definitions must be kept in sync.
 */
typedef struct C_MemoryAllocator {
  void *arena;
  void *(*realloc)(void *arena, void* mem, size_t newsize);
  void *(*calloc)(void *arena,  size_t n_elements, size_t elem_size);
  void (*free)(void *arena, void *p);
  void *(*create_arena)(size_t, int);
  size_t (*destroy_arena)(void *arena);
} C_MemoryAllocator;
#endif

//
// strings.c
//

typedef struct {
  char **data;
  int capacity;
  int len;
} StringArray;

void strarray_push(C_MemoryAllocator *allocator, StringArray *arr, char *s);
char *format(char *fmt, ...) __attribute__((format(printf, 1, 2)));

//
// C_tokenize.c
//

// C_Token
typedef enum {
  TK_IDENT,   // Identifiers
  TK_PUNCT,   // Punctuators
  TK_KEYWORD, // Keywords
  TK_STR,     // String literals
  TK_NUM,     // Numeric literals
  TK_PP_NUM,  // Preprocessing numbers
  TK_EOF,     // End-of-file markers
} C_TokenKind;

typedef struct {
  char *name;
  int file_no;
  char *contents;

  // For #line directive
  char *display_name;
  int line_delta;
} C_File;

// C_Token type
typedef struct C_Token C_Token;
struct C_Token {
  C_TokenKind kind;   // C_Token kind
  C_Token *next;      // Next token
  int64_t val;      // If kind is TK_NUM, its value
  long double fval; // If kind is TK_NUM, its value
  char *loc;        // C_Token location
  int len;          // C_Token length
  C_Type *ty;         // Used if TK_NUM or TK_STR
  char *str;        // String literal contents including terminating '\0'

  C_File *file;       // Source location
  char *filename;   // Filename
  int line_no;      // Line number
  int line_delta;   // Line number
  bool at_bol;      // True if this token is at beginning of line
  bool has_space;   // True if this token follows a space character
  C_Hideset *hideset; // For macro expansion
  C_Token *origin;    // If this is expanded from a macro, the original token
};

noreturn void C_error(C_Parser *tokenizer, char *fmt, ...) __attribute__((format(printf, 2, 3)));
noreturn void C_error_at(C_Parser *tokenizer, char *loc, char *fmt, ...) __attribute__((format(printf, 3, 4)));
noreturn void C_error_tok(C_Parser *tokenizer, C_Token *tok, char *fmt, ...) __attribute__((format(printf, 3, 4)));
void C_warn_tok(C_Parser *tokenizer, C_Token *tok, char *fmt, ...) __attribute__((format(printf, 3, 4)));
bool C_equal(C_Token *tok, char *op);
C_Token *C_skip(C_Parser *parser, C_Token *tok, char *op);
bool C_consume(C_Token **rest, C_Token *tok, char *str);
void C_convert_pp_tokens(C_Parser *tokenizer, C_Token *tok);
C_File *C_new_file(C_Parser *tokenizer, char *name, int file_no, char *contents);
C_Token *C_tokenize(C_Parser *tokenizer, C_File *file);
C_Token *C_tokenize_buffer(C_Parser *tokenizer, char *p);

#define unreachable(parser) \
  C_error(parser, "internal error at %s:%d", __FILE__, __LINE__)

//
// C_parse.c
//

// Variable or function
typedef struct C_Obj C_Obj;
struct C_Obj {
  C_Obj *next;
  char *name;    // Variable name
  C_Type *ty;      // Type
  C_Token *tok;    // representative token
  bool is_local; // local or global/function
  int align;     // alignment

  // Local variable
  int offset;

  // Global variable or function
  bool is_function;
  bool is_definition;
  bool is_static;

  // Global variable
  bool is_tentative;
  bool is_tls;
  char *init_data;
  C_Relocation *rel;

  // Function
  bool is_inline;
  C_Obj *params;
  C_Node *body;
  C_Obj *locals;
  C_Obj *va_area;
  C_Obj *alloca_bottom;
  int stack_size;

  // Static inline function
  bool is_live;
  bool is_root;
  StringArray refs;
};

// Global variable can be initialized either by a constant expression
// or a pointer to another global variable. This struct represents the
// latter.

struct C_Relocation {
  C_Relocation *next;
  int offset;
  char **label;
  long addend;
};

// AST node
typedef enum {
  ND_NULL_EXPR, // Do nothing
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_NEG,       // unary -
  ND_MOD,       // %
  ND_BITAND,    // &
  ND_BITOR,     // |
  ND_BITXOR,    // ^
  ND_SHL,       // <<
  ND_SHR,       // >>
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_ASSIGN,    // =
  ND_COND,      // ?:
  ND_COMMA,     // ,
  ND_MEMBER,    // . (struct member access)
  ND_ADDR,      // unary &
  ND_DEREF,     // unary *
  ND_NOT,       // !
  ND_BITNOT,    // ~
  ND_LOGAND,    // &&
  ND_LOGOR,     // ||
  ND_RETURN,    // "return"
  ND_IF,        // "if"
  ND_FOR,       // "for" or "while"
  ND_DO,        // "do"
  ND_SWITCH,    // "switch"
  ND_CASE,      // "case"
  ND_BLOCK,     // { ... }
  ND_GOTO,      // "goto"
  ND_GOTO_EXPR, // "goto" labels-as-values
  ND_LABEL,     // Labeled statement
  ND_LABEL_VAL, // [GNU] Labels-as-values
  ND_FUNCALL,   // Function call
  ND_EXPR_STMT, // Expression statement
  ND_STMT_EXPR, // Statement expression
  ND_VAR,       // Variable
  ND_VLA_PTR,   // VLA designator
  ND_NUM,       // Integer
  ND_CAST,      // Type cast
  ND_MEMZERO,   // Zero-clear a stack variable
  ND_ASM,       // "asm"
  ND_CAS,       // Atomic compare-and-swap
  ND_EXCH,      // Atomic exchange
} C_NodeKind;

// AST node type
struct C_Node {
  C_NodeKind kind; // C_Node kind
  C_Node *next;    // Next node
  C_Type *ty;      // Type, e.g. int or pointer to int
  C_Token *tok;    // Representative token

  C_Node *lhs;     // Left-hand side
  C_Node *rhs;     // Right-hand side

  // "if" or "for" statement
  C_Node *cond;
  C_Node *then;
  C_Node *els;
  C_Node *init;
  C_Node *inc;

  // "break" and "continue" labels
  char *brk_label;
  char *cont_label;

  // Block or statement expression
  C_Node *body;

  // Struct member access
  C_Member *member;

  // Function call
  C_Type *func_ty;
  C_Node *args;
  bool pass_by_stack;
  C_Obj *ret_buffer;

  // Goto or labeled statement, or labels-as-values
  char *label;
  char *unique_label;
  C_Node *goto_next;

  // Switch
  C_Node *case_next;
  C_Node *default_case;

  // Case
  long begin;
  long end;

  // "asm" string literal
  char *asm_str;

  // Atomic compare-and-swap
  C_Node *cas_addr;
  C_Node *cas_old;
  C_Node *cas_new;

  // Atomic op= operators
  C_Obj *atomic_addr;
  C_Node *atomic_expr;

  // Variable
  C_Obj *var;

  // Numeric literal
  int64_t val;
  long double fval;
};

typedef struct {
  char *key;
  int keylen;
  void *val;
} HashEntry;

typedef struct {
  HashEntry *buckets;
  int capacity;
  int used;
  C_MemoryAllocator *allocator;
} HashMap;

// Represents a block scope.
typedef struct C_Scope C_Scope;
struct C_Scope {
  C_Scope *next;

  // C has two block scopes; one is for variables/typedefs and
  // the other is for struct/union/enum tags.
  HashMap vars; // values are C_VarScope *
  HashMap tags; // values are C_Type *
};

// C_Scope for local variables, global variables, typedefs
// or enum constants
typedef struct {
  C_Obj *var;
  C_Type *type_def;
  C_Type *enum_ty;
  int enum_val;
} C_VarScope;

struct C_Parser {
  int file_no;
  // Input file
  C_File *current_file;

  // A list of all input files.
  C_File **input_files;

  // True if the current position is at the beginning of a line
  bool at_bol;

  // True if the current position follows a space character
  bool has_space;

  // All local variable instances created during parsing are
  // accumulated to this list.
  C_Obj *locals;

  // Likewise, global variables are accumulated to this list.
  C_Obj *globals;

  C_Scope *scope; // = &(C_Scope){0};

  // Points to the function object the parser is currently parsing.
  C_Obj *current_fn;

  // Lists of all goto statements and labels in the curent function.
  C_Node *gotos;
  C_Node *labels;

  // Current "goto" and "continue" jump targets.
  char *brk_label;
  char *cont_label;

  // Points to a node representing a switch if we are parsing
  // a switch statement. Otherwise, NULL.
  C_Node *current_switch;

  C_Obj *builtin_alloca;

  HashMap keywords; // used by tokenizer
  HashMap typewords; // used by parser
  C_MemoryAllocator *memory_allocator;

  jmp_buf env;		 /* For error handling */

  char *error_message; // Error reporting arg

#ifdef RAVI_EXTENSIONS
  bool embedded_mode;
#endif

};

void C_parser_init(C_Parser *parser, C_MemoryAllocator *memory_allocator);
C_Scope *C_global_scope(C_Parser *parser);
C_Node *C_new_cast(C_Parser *parser, C_Node *expr, C_Type *ty);
int64_t C_const_expr(C_Parser *parser, C_Token **rest, C_Token *tok);
C_Obj *C_parse(C_Scope * globalScope, C_Parser *parser, C_Token *tok);
void C_parser_destroy(C_Parser *parser);

#ifdef RAVI_EXTENSIONS
C_Node *C_parse_compound_statement(C_Scope *globalScope, C_Parser *parser, C_Token *tok);
C_Obj *C_create_function(C_Scope *globalScope, C_Parser *parser, char *name_str);
#endif

void C_destroy_parser(C_Parser *parser);

//
// type.c
//

typedef enum {
  TY_VOID,
  TY_BOOL,
  TY_CHAR,
  TY_SHORT,
  TY_INT,
  TY_LONG,
  TY_FLOAT,
  TY_DOUBLE,
  TY_LDOUBLE,
  TY_ENUM,
  TY_PTR,
  TY_FUNC,
  TY_ARRAY,
  TY_VLA, // variable-length array
  TY_STRUCT,
  TY_UNION,
} TypeKind;

struct C_Type {
  TypeKind kind;
  int size;           // sizeof() value
  int align;          // alignment
  bool is_unsigned;   // unsigned or signed
  bool is_atomic;     // true if _Atomic
  C_Type *origin;       // for type compatibility check

  // Pointer-to or array-of type. We intentionally use the same member
  // to represent pointer/array duality in C.
  //
  // In many contexts in which a pointer is expected, we examine this
  // member instead of "kind" member to determine whether a type is a
  // pointer or not. That means in many contexts "array of T" is
  // naturally handled as if it were "pointer to T", as required by
  // the C spec.
  C_Type *base;

  // Declaration
  C_Token *name;
  C_Token *name_pos;

  // Array
  int array_len;

  // Variable-length array
  C_Node *vla_len; // # of elements
  C_Obj *vla_size; // sizeof() value

  // Struct
  C_Member *members;
  bool is_flexible;
  bool is_packed;

  // Function type
  C_Type *return_ty;
  C_Type *params;
  bool is_variadic;
  C_Type *next;
};

// Struct member
struct C_Member {
  C_Member *next;
  C_Type *ty;
  C_Token *tok; // for error message
  C_Token *name;
  int idx;
  int align;
  int offset;

  // Bitfield
  bool is_bitfield;
  int bit_offset;
  int bit_width;
};

extern C_Type *C_ty_void;
extern C_Type *C_ty_bool;

extern C_Type *C_ty_char;
extern C_Type *C_ty_short;
extern C_Type *C_ty_int;
extern C_Type *C_ty_long;

extern C_Type *C_ty_uchar;
extern C_Type *C_ty_ushort;
extern C_Type *C_ty_uint;
extern C_Type *C_ty_ulong;

extern C_Type *C_ty_float;
extern C_Type *C_ty_double;
extern C_Type *C_ty_ldouble;

bool C_is_integer(C_Type *ty);
bool C_is_flonum(C_Type *ty);
bool C_is_numeric(C_Type *ty);
bool C_is_compatible(C_Type *t1, C_Type *t2);
C_Type *C_copy_type(C_Parser *parser, C_Type *ty);
C_Type *C_pointer_to(C_Parser *parser, C_Type *base);
C_Type *C_func_type(C_Parser *parser, C_Type *return_ty);
C_Type *C_array_of(C_Parser *parser, C_Type *base, int size);
C_Type *C_vla_of(C_Parser *parser, C_Type *base, C_Node *expr);
C_Type *C_enum_type(C_Parser *parser);
C_Type *C_struct_type(C_Parser *parser);
void C_add_type(C_Parser *parser, C_Node *node);

// Round up `n` to the nearest multiple of `align`. For instance,
// align_to(5, 8) returns 8 and align_to(11, 8) returns 16.
static inline int C_align_to(int n, int align) {
	return (n + align - 1) / align * align;
}


//
// unicode.c
//

int C_encode_utf8(char *buf, uint32_t c);
uint32_t C_decode_utf8(C_Parser *tokenizer, char **new_pos, char *p);
bool C_is_ident1(uint32_t c);
bool C_is_ident2(uint32_t c);
int C_display_width(C_Parser *tokenizer, char *p, int len);

//
// hashmap.c
//

void *hashmap_get(HashMap *map, char *key);
void *hashmap_get2(HashMap *map, char *key, int keylen);
void hashmap_put(HashMap *map, char *key, void *val);
void hashmap_put2(HashMap *map, char *key, int keylen, void *val);
void hashmap_delete(HashMap *map, char *key);
void hashmap_delete2(HashMap *map, char *key, int keylen);
void hashmap_test(void);
void hashmap_foreach(HashMap *map, void (*f)(void *userdata, char *key, int keylen, void *val), void *userdata);
void hashmap_destroy(HashMap *map);

#endif
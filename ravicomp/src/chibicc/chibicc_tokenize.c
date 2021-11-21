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


#include "chibicc.h"

static int error_sprintf(C_Parser *parser, const char *fmt, ...) {
  va_list args;
  int pos = parser->error_message ? (int)strlen(parser->error_message) : 0;
  int estimated_size = 128;
  int n = 0;
  for (int i = 0; i < 2; i++) {
    parser->error_message = parser->memory_allocator->realloc(parser->memory_allocator->arena, parser->error_message, pos + estimated_size); // ensure we have at least estimated_size free space
    va_start(args, fmt);
    n = vsnprintf(parser->error_message + pos, estimated_size, fmt, args);
    va_end(args);
    if (n >= estimated_size) {
      estimated_size = n + 1; // allow for 0 byte
    } else if (n < 0) {
      fprintf(stderr, "Buffer conversion error\n");
      assert(false);
      break;
    } else {
      break;
    }
  }
  return n;
}

static int error_vsprintf(C_Parser *parser, const char *fmt, va_list args) {
  int estimated_size = 128;
  int pos = parser->error_message ? (int)strlen(parser->error_message) : 0;
  int n = 0;
  for (int i = 0; i < 2; i++) {
    parser->error_message = parser->memory_allocator->realloc(parser->memory_allocator->arena, parser->error_message, pos + estimated_size); // ensure we have at least estimated_size free space
    n = vsnprintf(parser->error_message + pos, estimated_size, fmt, args);
    if (n >= estimated_size) {
      estimated_size = n + 1; // allow for 0 byte
    } else if (n < 0) {
      fprintf(stderr, "Buffer conversion error\n");
      assert(false);
      break;
    } else {
      break;
    }
  }
  return n;
}


// Reports an error and exit.
void C_error(C_Parser *parser, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  error_vsprintf(parser, fmt, ap);
  error_sprintf(parser, "\n");
  longjmp(parser->env, 1);
}

// Reports an error message in the following format.
//
// foo.c:10: x = y + 1;
//               ^ <error message here>
static void verror_at(C_Parser *tokenizer, char *filename, char *input, int line_no,
                      char *loc, char *fmt, va_list ap) {
  // Find a line containing `loc`.
  char *line = loc;
  while (input < line && line[-1] != '\n')
    line--;

  char *end = loc;
  while (*end && *end != '\n')
    end++;

  // Print out the line.
  int indent = error_sprintf(tokenizer, "%s:%d: ", filename, line_no);
  error_sprintf(tokenizer, "%.*s\n", (int)(end - line), line);

  // Show the error message.
  int pos = C_display_width(tokenizer, line, loc - line) + indent;

  error_sprintf(tokenizer, "%*s", pos, ""); // print pos spaces.
  error_sprintf(tokenizer, "^ ");
  error_vsprintf(tokenizer, fmt, ap);
  error_sprintf(tokenizer, "\n");
}

void C_error_at(C_Parser *tokenizer, char *loc, char *fmt, ...) {
  int line_no = 1;
  for (char *p = tokenizer->current_file->contents; p < loc; p++)
    if (*p == '\n')
      line_no++;

  va_list ap;
  va_start(ap, fmt);
  verror_at(tokenizer, tokenizer->current_file->name, tokenizer->current_file->contents, line_no, loc, fmt, ap);
  longjmp(tokenizer->env, 1);
}

void C_error_tok(C_Parser *tokenizer, C_Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tokenizer, tok->file->name, tok->file->contents, tok->line_no, tok->loc, fmt, ap);
  longjmp(tokenizer->env, 1);
}

void C_warn_tok(C_Parser *tokenizer, C_Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tokenizer, tok->file->name, tok->file->contents, tok->line_no, tok->loc, fmt, ap);
  va_end(ap);
}

// Consumes the current token if it matches `op`.
bool C_equal(C_Token *tok, char *op) {
  return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

// Ensure that the current token is `op`.
C_Token *C_skip(C_Parser *parser, C_Token *tok, char *op) {
  if (!C_equal(tok, op))
	  C_error_tok(parser, tok, "expected '%s'", op);
  return tok->next;
}

bool C_consume(C_Token **rest, C_Token *tok, char *str) {
  if (C_equal(tok, str)) {
    *rest = tok->next;
    return true;
  }
  *rest = tok;
  return false;
}

// Create a new token.
static C_Token *new_token(C_Parser *tokenizer, C_TokenKind kind, char *start, char *end) {
  C_Token *tok = tokenizer->memory_allocator->calloc(tokenizer->memory_allocator->arena, 1, sizeof(C_Token));
  tok->kind = kind;
  tok->loc = start;
  tok->len = end - start;
  tok->file = tokenizer->current_file;
  tok->filename = tokenizer->current_file->display_name;
  tok->at_bol = tokenizer->at_bol;
  tok->has_space = tokenizer->has_space;

  tokenizer->at_bol = tokenizer->has_space = false;
  return tok;
}

static bool startswith(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

// Read an identifier and returns the length of it.
// If p does not point to a valid identifier, 0 is returned.
static int read_ident(C_Parser *tokenizer, char *start) {
  char *p = start;
  uint32_t c = C_decode_utf8(tokenizer, &p, p);
  if (!C_is_ident1(c))
    return 0;

  for (;;) {
    char *q;
    c = C_decode_utf8(tokenizer, &q, p);
    if (!C_is_ident2(c))
      return p - start;
    p = q;
  }
}

static int from_hex(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  return c - 'A' + 10;
}

// Read a punctuator token from p and returns its length.
static int read_punct(char *p) {
  static char *kw[] = {
    "<<=", ">>=", "...", "==", "!=", "<=", ">=", "->", "+=",
    "-=", "*=", "/=", "++", "--", "%=", "&=", "|=", "^=", "&&",
    "||", "<<", ">>", "##",
  };

  for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
    if (startswith(p, kw[i]))
      return strlen(kw[i]);

  return ispunct(*p) ? 1 : 0;
}

static bool is_keyword(C_Parser *tokenizer, C_Token *tok) {
  if (tokenizer->keywords.capacity == 0) {
    static char *kw[] = {
      "return", "if", "else", "for", "while", "int", "sizeof", "char",
      "struct", "union", "short", "long", "void", "typedef", "_Bool",
      "enum", "static", "goto", "break", "continue", "switch", "case",
      "default", "extern", "_Alignof", "_Alignas", "do", "signed",
      "unsigned", "const", "volatile", "auto", "register", "restrict",
      "__restrict", "__restrict__", "_Noreturn", "float", "double",
      "typeof", "asm", "_Thread_local", "__thread", "_Atomic",
      "__attribute__",
    };

    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
      hashmap_put(&tokenizer->keywords, kw[i], (void *)1);
  }

  return hashmap_get2(&tokenizer->keywords, tok->loc, tok->len);
}

static int read_escaped_char(C_Parser *tokenizer, char **new_pos, char *p) {
  if ('0' <= *p && *p <= '7') {
    // Read an octal number.
    int c = *p++ - '0';
    if ('0' <= *p && *p <= '7') {
      c = (c << 3) + (*p++ - '0');
      if ('0' <= *p && *p <= '7')
        c = (c << 3) + (*p++ - '0');
    }
    *new_pos = p;
    return c;
  }

  if (*p == 'x') {
    // Read a hexadecimal number.
    p++;
    if (!isxdigit(*p))
	    C_error_at(tokenizer, p, "invalid hex escape sequence");

    int c = 0;
    for (; isxdigit(*p); p++)
      c = (c << 4) + from_hex(*p);
    *new_pos = p;
    return c;
  }

  *new_pos = p + 1;

  // Escape sequences are defined using themselves here. E.g.
  // '\n' is implemented using '\n'. This tautological definition
  // works because the compiler that compiles our compiler knows
  // what '\n' actually is. In other words, we "inherit" the ASCII
  // code of '\n' from the compiler that compiles our compiler,
  // so we don't have to teach the actual code here.
  //
  // This fact has huge implications not only for the correctness
  // of the compiler but also for the security of the generated code.
  // For more info, read "Reflections on Trusting Trust" by Ken Thompson.
  // https://github.com/rui314/chibicc/wiki/thompson1984.pdf
  switch (*p) {
  case 'a': return '\a';
  case 'b': return '\b';
  case 't': return '\t';
  case 'n': return '\n';
  case 'v': return '\v';
  case 'f': return '\f';
  case 'r': return '\r';
  // [GNU] \e for the ASCII escape character is a GNU C extension.
  case 'e': return 27;
  default: return *p;
  }
}

// Find a closing double-quote.
static char *string_literal_end(C_Parser *tokenizer, char *p) {
  char *start = p;
  for (; *p != '"'; p++) {
    if (*p == '\n' || *p == '\0')
	    C_error_at(tokenizer, start, "unclosed string literal");
    if (*p == '\\')
      p++;
  }
  return p;
}

static C_Token *read_string_literal(C_Parser *tokenizer, char *start, char *quote) {
  char *end = string_literal_end(tokenizer,quote + 1);
  char *buf = tokenizer->memory_allocator->calloc(tokenizer->memory_allocator->arena, 1, end - quote);
  int len = 0;

  for (char *p = quote + 1; p < end;) {
    if (*p == '\\')
      buf[len++] = read_escaped_char(tokenizer, &p, p + 1);
    else
      buf[len++] = *p++;
  }

  C_Token *tok = new_token(tokenizer, TK_STR, start, end + 1);
  tok->ty = C_array_of(tokenizer, C_ty_char, len + 1);
  tok->str = buf;
  return tok;
}

// Read a UTF-8-encoded string literal and transcode it in UTF-16.
//
// UTF-16 is yet another variable-width encoding for Unicode. Code
// points smaller than U+10000 are encoded in 2 bytes. Code points
// C_equal to or larger than that are encoded in 4 bytes. Each 2 bytes
// in the 4 byte sequence is called "surrogate", and a 4 byte sequence
// is called a "surrogate pair".
static C_Token *read_utf16_string_literal(C_Parser *tokenizer, char *start, char *quote) {
  char *end = string_literal_end(tokenizer, quote + 1);
  uint16_t *buf = tokenizer->memory_allocator->calloc(tokenizer->memory_allocator->arena, 2, end - start);
  int len = 0;

  for (char *p = quote + 1; p < end;) {
    if (*p == '\\') {
      buf[len++] = read_escaped_char(tokenizer, &p, p + 1);
      continue;
    }

    uint32_t c = C_decode_utf8(tokenizer, &p, p);
    if (c < 0x10000) {
      // Encode a code point in 2 bytes.
      buf[len++] = c;
    } else {
      // Encode a code point in 4 bytes.
      c -= 0x10000;
      buf[len++] = 0xd800 + ((c >> 10) & 0x3ff);
      buf[len++] = 0xdc00 + (c & 0x3ff);
    }
  }

  C_Token *tok = new_token(tokenizer, TK_STR, start, end + 1);
  tok->ty = C_array_of(tokenizer, C_ty_ushort, len + 1);
  tok->str = (char *)buf;
  return tok;
}

// Read a UTF-8-encoded string literal and transcode it in UTF-32.
//
// UTF-32 is a fixed-width encoding for Unicode. Each code point is
// encoded in 4 bytes.
static C_Token *read_utf32_string_literal(C_Parser *tokenizer, char *start, char *quote, C_Type *ty) {
  char *end = string_literal_end(tokenizer, quote + 1);
  uint32_t *buf = tokenizer->memory_allocator->calloc(tokenizer->memory_allocator->arena, 4, end - quote);
  int len = 0;

  for (char *p = quote + 1; p < end;) {
    if (*p == '\\')
      buf[len++] = read_escaped_char(tokenizer, &p, p + 1);
    else
      buf[len++] = C_decode_utf8(tokenizer, &p, p);
  }

  C_Token *tok = new_token(tokenizer, TK_STR, start, end + 1);
  tok->ty = C_array_of(tokenizer, ty, len + 1);
  tok->str = (char *)buf;
  return tok;
}

static C_Token *read_char_literal(C_Parser *tokenizer, char *start, char *quote, C_Type *ty) {
  char *p = quote + 1;
  if (*p == '\0')
	  C_error_at(tokenizer, start, "unclosed char literal");

  int c;
  if (*p == '\\')
    c = read_escaped_char(tokenizer, &p, p + 1);
  else
    c = C_decode_utf8(tokenizer, &p, p);

  char *end = strchr(p, '\'');
  if (!end)
	  C_error_at(tokenizer, p, "unclosed char literal");

  C_Token *tok = new_token(tokenizer, TK_NUM, start, end + 1);
  tok->val = c;
  tok->ty = ty;
  return tok;
}

static bool convert_pp_int(C_Token *tok) {
  char *p = tok->loc;

  // Read a binary, octal, decimal or hexadecimal number.
  int base = 10;
  if (!strncasecmp(p, "0x", 2) && isxdigit(p[2])) {
    p += 2;
    base = 16;
  } else if (!strncasecmp(p, "0b", 2) && (p[2] == '0' || p[2] == '1')) {
    p += 2;
    base = 2;
  } else if (*p == '0') {
    base = 8;
  }

  int64_t val = strtoul(p, &p, base);

  // Read U, L or LL suffixes.
  bool l = false;
  bool u = false;

  if (startswith(p, "LLU") || startswith(p, "LLu") ||
      startswith(p, "llU") || startswith(p, "llu") ||
      startswith(p, "ULL") || startswith(p, "Ull") ||
      startswith(p, "uLL") || startswith(p, "ull")) {
    p += 3;
    l = u = true;
  } else if (!strncasecmp(p, "lu", 2) || !strncasecmp(p, "ul", 2)) {
    p += 2;
    l = u = true;
  } else if (startswith(p, "LL") || startswith(p, "ll")) {
    p += 2;
    l = true;
  } else if (*p == 'L' || *p == 'l') {
    p++;
    l = true;
  } else if (*p == 'U' || *p == 'u') {
    p++;
    u = true;
  }

  if (p != tok->loc + tok->len)
    return false;

  // Infer a type.
  C_Type *ty;
  if (base == 10) {
    if (l && u)
      ty = C_ty_ulong;
    else if (l)
      ty = C_ty_long;
    else if (u)
      ty = (val >> 32) ? C_ty_ulong : C_ty_uint;
    else
      ty = (val >> 31) ? C_ty_long : C_ty_int;
  } else {
    if (l && u)
      ty = C_ty_ulong;
    else if (l)
      ty = (val >> 63) ? C_ty_ulong : C_ty_long;
    else if (u)
      ty = (val >> 32) ? C_ty_ulong : C_ty_uint;
    else if (val >> 63)
      ty = C_ty_ulong;
    else if (val >> 32)
      ty = C_ty_long;
    else if (val >> 31)
      ty = C_ty_uint;
    else
      ty = C_ty_int;
  }

  tok->kind = TK_NUM;
  tok->val = val;
  tok->ty = ty;
  return true;
}

// The definition of the numeric literal at the preprocessing stage
// is more relaxed than the definition of that at the later stages.
// In order to handle that, a numeric literal is tokenized as a
// "pp-number" token first and then converted to a regular number
// token after preprocessing.
//
// This function converts a pp-number token to a regular number token.
static void convert_pp_number(C_Parser *tokenizer, C_Token *tok) {
  // Try to C_parse as an integer constant.
  if (convert_pp_int(tok))
    return;

  // If it's not an integer, it must be a floating point constant.
  char *end;
  long double val = strtold(tok->loc, &end);

  C_Type *ty;
  if (*end == 'f' || *end == 'F') {
    ty = C_ty_float;
    end++;
  } else if (*end == 'l' || *end == 'L') {
    ty = C_ty_ldouble;
    end++;
  } else {
    ty = C_ty_double;
  }

  if (tok->loc + tok->len != end)
	  C_error_tok(tokenizer, tok, "invalid numeric constant");

  tok->kind = TK_NUM;
  tok->fval = val;
  tok->ty = ty;
}

void C_convert_pp_tokens(C_Parser *tokenizer, C_Token *tok) {
  for (C_Token *t = tok; t->kind != TK_EOF; t = t->next) {
    if (is_keyword(tokenizer, t))
      t->kind = TK_KEYWORD;
    else if (t->kind == TK_PP_NUM)
      convert_pp_number(tokenizer, t);
  }
}

// Initialize line info for all tokens.
static void add_line_numbers(C_Parser *tokenizer, C_Token *tok) {
  char *p = tokenizer->current_file->contents;
  int n = 1;

  do {
    if (p == tok->loc) {
      tok->line_no = n;
      tok = tok->next;
    }
    if (*p == '\n')
      n++;
  } while (*p++);
}

static C_Token *tokenize_string_literal(C_Parser *tokenizer, C_Token *tok, C_Type *basety) {
	C_Token *t;
  if (basety->size == 2)
    t = read_utf16_string_literal(tokenizer, tok->loc, tok->loc);
  else
    t = read_utf32_string_literal(tokenizer, tok->loc, tok->loc, basety);
  t->next = tok->next;
  return t;
}

// Tokenize a given string and returns new tokens.
C_Token *C_tokenize(C_Parser *tokenizer, C_File *file) {
  tokenizer->current_file = file;

  char *p = file->contents;
  C_Token head = {0};
  C_Token *cur = &head;

  tokenizer->at_bol = true;
  tokenizer->has_space = false;

  while (*p) {
    // Skip line comments.
    if (startswith(p, "//")) {
      p += 2;
      while (*p != '\n')
        p++;
      tokenizer->has_space = true;
      continue;
    }

    // Skip block comments.
    if (startswith(p, "/*")) {
      if (tokenizer->embedded_mode)
        C_error_at(tokenizer, p, "block comment not allowed in this mode");
      char *q = strstr(p + 2, "*/");
      if (!q)
	      C_error_at(tokenizer, p, "unclosed block comment");
      p = q + 2;
      tokenizer->has_space = true;
      continue;
    }

    // Skip newline.
    if (*p == '\n') {
      p++;
      tokenizer->at_bol = true;
      tokenizer->has_space = false;
      continue;
    }

    // Skip whitespace characters.
    if (isspace(*p)) {
      p++;
      tokenizer->has_space = true;
      continue;
    }

    // Numeric literal
    if (isdigit(*p) || (*p == '.' && isdigit(p[1]))) {
      char *q = p++;
      for (;;) {
        if (p[0] && p[1] && strchr("eEpP", p[0]) && strchr("+-", p[1]))
          p += 2;
        else if (isalnum(*p) || *p == '.')
          p++;
        else
          break;
      }
      cur = cur->next = new_token(tokenizer, TK_PP_NUM, q, p);
      continue;
    }

    // String literal
    if (*p == '"') {
      cur = cur->next = read_string_literal(tokenizer, p, p);
      p += cur->len;
      continue;
    }

    // UTF-8 string literal
    if (startswith(p, "u8\"")) {
      cur = cur->next = read_string_literal(tokenizer, p, p + 2);
      p += cur->len;
      continue;
    }

    // UTF-16 string literal
    if (startswith(p, "u\"")) {
      cur = cur->next = read_utf16_string_literal(tokenizer, p, p + 1);
      p += cur->len;
      continue;
    }

    // Wide string literal
    if (startswith(p, "L\"")) {
      cur = cur->next = read_utf32_string_literal(tokenizer, p, p + 1, C_ty_int);
      p += cur->len;
      continue;
    }

    // UTF-32 string literal
    if (startswith(p, "U\"")) {
      cur = cur->next = read_utf32_string_literal(tokenizer, p, p + 1, C_ty_uint);
      p += cur->len;
      continue;
    }

    // Character literal
    if (*p == '\'') {
      cur = cur->next = read_char_literal(tokenizer, p, p, C_ty_int);
      cur->val = (char)cur->val;
      p += cur->len;
      continue;
    }

    // UTF-16 character literal
    if (startswith(p, "u'")) {
      cur = cur->next = read_char_literal(tokenizer, p, p + 1, C_ty_ushort);
      cur->val &= 0xffff;
      p += cur->len;
      continue;
    }

    // Wide character literal
    if (startswith(p, "L'")) {
      cur = cur->next = read_char_literal(tokenizer, p, p + 1, C_ty_int);
      p += cur->len;
      continue;
    }

    // UTF-32 character literal
    if (startswith(p, "U'")) {
      cur = cur->next = read_char_literal(tokenizer, p, p + 1, C_ty_uint);
      p += cur->len;
      continue;
    }

    // Identifier or keyword
    int ident_len = read_ident(tokenizer, p);
    if (ident_len) {
      cur = cur->next = new_token(tokenizer, TK_IDENT, p, p + ident_len);
      p += cur->len;
      continue;
    }

    // Punctuators
    int punct_len = read_punct(p);
    if (punct_len) {
      cur = cur->next = new_token(tokenizer, TK_PUNCT, p, p + punct_len);
      p += cur->len;
      continue;
    }

    C_error_at(tokenizer, p, "invalid token");
  }

  cur = cur->next = new_token(tokenizer, TK_EOF, p, p);
  add_line_numbers(tokenizer, head.next);
  return head.next;
}

#if 0
// Returns the contents of a given file.
static char *read_file(char *path) {
  FILE *fp;

  if (strcmp(path, "-") == 0) {
    // By convention, read from stdin if a given filename is "-".
    fp = stdin;
  } else {
    fp = fopen(path, "r");
    if (!fp)
      return NULL;
  }

  char *buf;
  size_t buflen;
  FILE *out = open_memstream(&buf, &buflen);

  // Read the entire file.
  for (;;) {
    char buf2[4096];
    int n = fread(buf2, 1, sizeof(buf2), fp);
    if (n == 0)
      break;
    fwrite(buf2, 1, n, out);
  }

  if (fp != stdin)
    fclose(fp);

  // Make sure that the last line is properly terminated with '\n'.
  fflush(out);
  if (buflen == 0 || buf[buflen - 1] != '\n')
    fputc('\n', out);
  fputc('\0', out);
  fclose(out);
  return buf;
}

C_File **get_input_files(C_Parser *tokenizer) {
  return tokenizer->input_files;
}
#endif


C_File *C_new_file(C_Parser *tokenizer, char *name, int file_no, char *contents) {
  C_File *file = tokenizer->memory_allocator->calloc(tokenizer->memory_allocator->arena, 1, sizeof(C_File));
  file->name = name;
  file->display_name = name;
  file->file_no = file_no;
  file->contents = contents;
  return file;
}


// Replaces \r or \r\n with \n.
static void canonicalize_newline(char *p) {
  int i = 0, j = 0;

  while (p[i]) {
    if (p[i] == '\r' && p[i + 1] == '\n') {
      i += 2;
      p[j++] = '\n';
    } else if (p[i] == '\r') {
      i++;
      p[j++] = '\n';
    } else {
      p[j++] = p[i++];
    }
  }

  p[j] = '\0';
}

// Removes backslashes followed by a newline.
static void remove_backslash_newline(char *p) {
  int i = 0, j = 0;

  // We want to keep the number of newline characters so that
  // the logical line number matches the physical one.
  // This counter maintain the number of newlines we have removed.
  int n = 0;

  while (p[i]) {
    if (p[i] == '\\' && p[i + 1] == '\n') {
      i += 2;
      n++;
    } else if (p[i] == '\n') {
      p[j++] = p[i++];
      for (; n > 0; n--)
        p[j++] = '\n';
    } else {
      p[j++] = p[i++];
    }
  }

  for (; n > 0; n--)
    p[j++] = '\n';
  p[j] = '\0';
}

static uint32_t read_universal_char(char *p, int len) {
  uint32_t c = 0;
  for (int i = 0; i < len; i++) {
    if (!isxdigit(p[i]))
      return 0;
    c = (c << 4) | from_hex(p[i]);
  }
  return c;
}

// Replace \u or \U escape sequences with corresponding UTF-8 bytes.
static void convert_universal_chars(char *p) {
  char *q = p;

  while (*p) {
    if (startswith(p, "\\u")) {
      uint32_t c = read_universal_char(p + 2, 4);
      if (c) {
        p += 6;
        q += C_encode_utf8(q, c);
      } else {
        *q++ = *p++;
      }
    } else if (startswith(p, "\\U")) {
      uint32_t c = read_universal_char(p + 2, 8);
      if (c) {
        p += 10;
        q += C_encode_utf8(q, c);
      } else {
        *q++ = *p++;
      }
    } else if (p[0] == '\\') {
      *q++ = *p++;
      *q++ = *p++;
    } else {
      *q++ = *p++;
    }
  }

  *q = '\0';
}

#if 0
C_Token *tokenize_file(C_Parser *tokenizer, char *path) {
  char *p = read_file(path);
  if (!p)
    return NULL;
  return tokenize_buffer(p);
}
#endif

C_Token *C_tokenize_buffer(C_Parser *tokenizer, char *p) {
  if (!p)
    return NULL;

  if (setjmp(tokenizer->env) != 0)
    return NULL;

  // UTF-8 texts may start with a 3-byte "BOM" marker sequence.
  // If exists, just C_skip them because they are useless bytes.
  // (It is actually not recommended to add BOM markers to UTF-8
  // texts, but it's not uncommon particularly on Windows.)
  if (!memcmp(p, "\xef\xbb\xbf", 3))
    p += 3;

  canonicalize_newline(p);
  remove_backslash_newline(p);
  convert_universal_chars(p);

  // Save the filename for assembler .file directive.
  static int file_no;
  C_File *file = C_new_file(tokenizer, "", file_no + 1, p);

  // Save the filename for assembler .file directive.
  tokenizer->input_files = tokenizer->memory_allocator->realloc(tokenizer->memory_allocator->arena, tokenizer->input_files, sizeof(char *) * (file_no + 2));
  tokenizer->input_files[file_no] = file;
  tokenizer->input_files[file_no + 1] = NULL;
  file_no++;

  return C_tokenize(tokenizer, file);
}

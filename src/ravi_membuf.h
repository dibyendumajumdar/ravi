#ifndef RAVI_MEMBUF_H
#define RAVI_MEMBUF_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

typedef struct {
  char *buf;
  size_t allocated_size;
  size_t pos;
} membuff_t;

extern void membuff_init(membuff_t *mb, size_t initial_size);
extern void membuff_rewindpos(membuff_t *mb);
extern void membuff_resize(membuff_t *mb, size_t new_size);
extern void membuff_free(membuff_t *mb);
extern void membuff_add_string(membuff_t *mb, const char *str);
extern void membuff_add_fstring(membuff_t *mb, const char *str, ...);
extern void membuff_add_vfstring(membuff_t *mb, const char *fmt, va_list args);
extern void membuff_add_bool(membuff_t *mb, bool value);
extern void membuff_add_int(membuff_t *mb, int value);
extern void membuff_add_longlong(membuff_t *mb, int64_t value);
extern void membuff_add_char(membuff_t *mb, char c);

/* strncpy() with guaranteed 0 termination */
extern void ravi_string_copy(char *buf, const char *src, size_t buflen);


#endif

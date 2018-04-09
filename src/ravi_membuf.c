#include <ravi_membuf.h>

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ravi_string_copy(char *buf, const char *src, size_t buflen) {
  if (buflen == 0) return;
  strncpy(buf, src, buflen);
  buf[buflen - 1] = 0;
}

void membuff_init(membuff_t *mb, size_t initial_size) {
  if (initial_size > 0) {
    mb->buf = (char *)calloc(1, initial_size);
    if (mb->buf == NULL) {
      fprintf(stderr, "out of memory\n");
      exit(1);
    }
  }
  else
    mb->buf = NULL;
  mb->pos = 0;
  mb->allocated_size = initial_size;
}
void membuff_rewindpos(membuff_t *mb) { mb->pos = 0; }
void membuff_resize(membuff_t *mb, size_t new_size) {
  if (new_size <= mb->allocated_size) return;
  char *newmem = (char *)realloc(mb->buf, new_size);
  if (newmem == NULL) {
    fprintf(stderr, "out of memory\n");
    exit(1);
  }
  mb->buf = newmem;
  mb->allocated_size = new_size;
}
void membuff_free(membuff_t *mb) { free(mb->buf); }
void membuff_add_string(membuff_t *mb, const char *str) {
  size_t len = strlen(str);
  size_t required_size = mb->pos + len + 1; /* extra byte for NULL terminator */
  membuff_resize(mb, required_size);
  assert(mb->allocated_size - mb->pos > len);
  ravi_string_copy(&mb->buf[mb->pos], str, mb->allocated_size - mb->pos);
  mb->pos += len;
}
void membuff_add_fstring(membuff_t *mb, const char *fmt, ...) {
  va_list args;
  char buffer[1024];

  va_start(args, fmt);
  int n = vsnprintf(buffer, sizeof buffer, fmt, args);
  va_end(args);
  if (n < 0) { abort(); }
  membuff_add_string(mb, buffer);
}
void membuff_add_vfstring(membuff_t *mb, const char *fmt, va_list args) {
	char buffer[1024];
	int n = vsnprintf(buffer, sizeof buffer, fmt, args);
	if (n < 0) { abort(); }
	membuff_add_string(mb, buffer);
}
void membuff_add_bool(membuff_t *mb, bool value) {
  if (value)
    membuff_add_string(mb, "true");
  else
    membuff_add_string(mb, "false");
}
void membuff_add_int(membuff_t *mb, int value) {
  char temp[100];
  snprintf(temp, sizeof temp, "%d", value);
  membuff_add_string(mb, temp);
}
void membuff_add_longlong(membuff_t *mb, int64_t value) {
  char temp[100];
  snprintf(temp, sizeof temp, "%" PRId64 "", value);
  membuff_add_string(mb, temp);
}
void membuff_add_char(membuff_t *mb, char c) {
	char temp[2] = { c, '\0' };
	membuff_add_string(mb, temp);
}
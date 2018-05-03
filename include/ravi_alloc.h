/*
** Bundled memory allocator.
** Donated to the public domain.
*/

#ifndef RAVI_ALLOC_H
#define RAVI_ALLOC_H

#include "ravi_def.h"

LUA_API void *ravi_alloc_create(void);
LUA_API void ravi_alloc_destroy(void *msp);
LUA_API void *ravi_alloc_f(void *msp, void *ptr, size_t osize, size_t nsize);

#endif

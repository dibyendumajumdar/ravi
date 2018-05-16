/*
* This version is part of the dmr_c project.
* Copyright (C) 2017 Dibyendu Majumdar
*/

#ifndef DMR_C_PORT_H
#define DMR_C_PORT_H

#ifndef _WIN32
#include <unistd.h>
#include <alloca.h>
#else
#include <io.h>
#include <malloc.h>
#endif

#if defined(_WIN32) && defined(_MSC_VER)
#include <stdlib.h>
#ifndef __cplusplus
#define inline __inline
#endif
#define __alignof__ __alignof
#define __builtin_bswap16 _byteswap_ushort
#define __builtin_bswap32 _byteswap_ulong
#define __builtin_bswap64 _byteswap_uint64
#endif

#ifdef __GNUC__
#define FORMAT_ATTR(pos) __attribute__((__format__(__printf__, pos, pos + 1)))
#define NORETURN_ATTR __attribute__((__noreturn__))
#define SENTINEL_ATTR __attribute__((__sentinel__))
#else
#define FORMAT_ATTR(pos)
#define NORETURN_ATTR
#define SENTINEL_ATTR
#endif

#ifdef __cplusplus
extern "C" {
#endif

void *dmrC_blob_alloc(size_t size);
void dmrC_blob_free(void *addr, size_t size);
long double dmrC_string_to_ld(const char *nptr, char **endptr);

#include <stdint.h>

#ifdef ASMJIT_STATIC
struct backend_data { uint64_t x[2]; };
#define DMRC_BACKEND_TYPE struct backend_data
#else
#define DMRC_BACKEND_TYPE void *
#endif

#ifdef __cplusplus
}
#endif

#endif

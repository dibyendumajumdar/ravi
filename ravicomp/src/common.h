#ifndef ravicomp_COMMON_H
#define ravicomp_COMMON_H

#include <stdint.h>

typedef uint32_t nodeId_t; /* The type used to identify nodes in CFG */

/* We have two distinguished basic blocks in every proc */
enum {
	ENTRY_BLOCK = 0,
	EXIT_BLOCK = 1
};

#endif

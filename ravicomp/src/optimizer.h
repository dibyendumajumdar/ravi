#ifndef ravicomp_OPTIMIZER_H
#define ravicomp_OPTIMIZER_H

/**
 * Remove blocks that are unreachable. Blocks ae logically deleted by removing
 * all instructions, rather than being physically removed.
 */
extern int raviX_remove_unreachable_blocks(LinearizerState *linearizer);

#endif
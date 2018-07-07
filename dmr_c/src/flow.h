#ifndef DMR_C_FLOW_H
#define DMR_C_FLOW_H

/*
* Flow - walk the linearized flowgraph, simplifying it as we
* go along.
*
* Copyright (C) 2004 Linus Torvalds
*/

#include <lib.h>

#ifdef __cplusplus
extern "C" {
#endif


struct entrypoint;
struct instruction;

extern int dmrC_simplify_flow(struct dmr_C *C, struct entrypoint *ep);

extern void dmrC_simplify_symbol_usage(struct dmr_C *C, struct entrypoint *ep);
extern void dmrC_simplify_memops(struct dmr_C *C, struct entrypoint *ep);
extern void dmrC_pack_basic_blocks(struct dmr_C *C, struct entrypoint *ep);

extern void dmrC_cleanup_and_cse(struct dmr_C *C, struct entrypoint *ep);
extern int dmrC_simplify_instruction(struct dmr_C *C, struct instruction *);

extern void dmrC_kill_use(struct dmr_C *C, pseudo_t *usep);
extern void dmrC_remove_use(struct dmr_C *C, pseudo_t *);

void dmrC_check_access(struct dmr_C *C, struct instruction *insn);
void dmrC_convert_load_instruction(struct dmr_C *C, struct instruction *, pseudo_t);
void dmrC_rewrite_load_instruction(struct dmr_C *C, struct instruction *, struct pseudo_list *);
int dmrC_dominates(struct dmr_C *C, pseudo_t pseudo, struct instruction *insn, struct instruction *dom, int local);

extern void dmrC_vrfy_flow(struct entrypoint *ep);
extern int dmrC_pseudo_in_list(struct pseudo_list *list, pseudo_t pseudo);

#ifdef __cplusplus
}
#endif


#endif

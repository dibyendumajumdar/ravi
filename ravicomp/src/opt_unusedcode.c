/* A pass over linearized code to eliminate unused code.
 * Initially only tackle unreachable basic blocks
 */

#include "linearizer.h"
#include "cfg.h"
#include "graph.h"
#include "allocate.h"
#include "optimizer.h"

/**
 * Check if a basic block has 0 predecessors. If so we can remove it from the CFG.
 * We also remove all the instructions in the basic block
 */
static int process_block(LinearizerState *linearizer, Proc *proc, BasicBlock *bb)
{
	GraphNode *node = raviX_graph_node(proc->cfg, bb->index);
	GraphNodeList *predecessors = raviX_predecessors(node);
	if (raviX_node_list_size(predecessors) != 0) {
		// Has predecessors so nothing to do
		return 0;
	}
	// No predecessor blocks, so we can remove this block
	GraphNodeList *successors = raviX_successors(node);
	uint32_t count = raviX_node_list_size(successors);
	if (count == 0) {
		// Nothing to do, but odd?
		// FIXME maybe assert?
		return 0;
	}
	// Make a copy of the successor node list as we need to change the CFG
	nodeId_t *nodes = raviX_realloc_array(NULL, sizeof(nodeId_t), 0, count);
	for (uint32_t i = 0; i < count; i++) {
		nodes[i] = raviX_node_list_at(successors, i);
	}
	for (uint32_t i = 0; i < count; i++) {
		// Remove edge from bb to the successor node
		raviX_delete_edge(proc->cfg, bb->index, nodes[i]);
	}
	free(nodes);
	assert(raviX_node_list_size(successors) == 0); // All should be gone
	// Now clear out this bb
	// FIXME deallocate instructions
	raviX_ptrlist_remove_all((struct ptr_list **)&bb->insns);
	// FIXME do we deallocate bb?
	return 1; // We changed something
}

static int process_proc(LinearizerState *linearizer, Proc *proc)
{
	if (proc->cfg == NULL) {
		if (raviX_construct_cfg(proc) != 0) {
			return 1;
		}
	}
	int changed = 1;
	while (changed) {
		changed = 0;
		BasicBlock *bb;
		for (int i = 0; i < (int)proc->node_count; i++) {
			bb = proc->nodes[i];
			if (bb->index == ENTRY_BLOCK || bb->index == EXIT_BLOCK)
				continue;
			changed |= process_block(linearizer, proc, bb);
		}
	}
	return 0;
}

int raviX_remove_unreachable_blocks(LinearizerState *linearizer)
{
	Proc *proc;
	FOR_EACH_PTR(linearizer->all_procs, proc)
		{
			if (process_proc(linearizer, proc) != 0)
				return 1;
		}
	END_FOR_EACH_PTR(proc)
	return 0;
}
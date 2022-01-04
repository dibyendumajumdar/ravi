/******************************************************************************
 * Copyright (C) 2020-2022 Dibyendu Majumdar
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

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
	nodeId_t *nodes = (nodeId_t *) raviX_realloc_array(NULL, sizeof(nodeId_t), 0, count);
	for (uint32_t i = 0; i < count; i++) {
		nodes[i] = raviX_node_list_at(successors, i);
	}
	for (uint32_t i = 0; i < count; i++) {
		// Remove edge from bb to the successor node
		raviX_delete_edge(proc->cfg, bb->index, nodes[i]);
	}
	raviX_free(nodes);
	assert(raviX_node_list_size(successors) == 0); // All should be gone
	// Now clear out this bb
	// FIXME deallocate instructions
	raviX_ptrlist_remove_all((PtrList **)&bb->insns);
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
	FOR_EACH_PTR(linearizer->all_procs, Proc, proc)
		{
			if (process_proc(linearizer, proc) != 0)
				return 1;
		}
	END_FOR_EACH_PTR(proc)
	return 0;
}
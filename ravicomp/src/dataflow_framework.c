/**
 * A framework for performing data flow analysis.
 * The framework is based upon similar framework in MIR project (https://github.com/vnmakarov/mir)
 */

#include "dataflow_framework.h"
#include "allocate.h"
#include "graph.h"
#include "bitset.h"

#include <string.h>

DECLARE_ARRAY(GraphNodeArray, GraphNode *);

struct dataflow_context {
	Graph *g;
	GraphNodeArray worklist;
	GraphNodeArray pending;
	BitSet bb_to_consider;
	void *userdata;
};

static void init_data_flow(struct dataflow_context *dataflow_context, Graph *g)
{
	memset(dataflow_context, 0, sizeof *dataflow_context);
	raviX_bitset_create2(&dataflow_context->bb_to_consider, 512);
	dataflow_context->g = g;
}

static void finish_data_flow(struct dataflow_context *dataflow_context)
{
	array_clearmem(&dataflow_context->worklist);
	array_clearmem(&dataflow_context->pending);
	raviX_bitset_destroy(&dataflow_context->bb_to_consider);
}

void raviX_solve_dataflow(Graph *g, bool forward_p,
			  int (*join_function)(void *, nodeId_t, bool),
			  int (*transfer_function)(void *, nodeId_t), void *userdata)
{
	unsigned iter;
	struct dataflow_context ctx;
	GraphNodeArray *worklist;
	GraphNodeArray *pending;

	init_data_flow(&ctx, g);
	worklist = &ctx.worklist;
	pending = &ctx.pending;

	/* ensure that the graph has RPO calculated */
	raviX_classify_edges(ctx.g);

	worklist->count = 0;
	/* Initially the basic blocks are added to the worklist */
	for (uint32_t i = 0; i < raviX_graph_size(ctx.g); i++) {
		array_push(worklist, raviX_graph_node(ctx.g, i));
	}
	iter = 0;
	while (worklist->count != 0) {
		GraphNode **addr = worklist->data;
		raviX_sort_nodes_by_RPO(addr, worklist->count, forward_p);
		raviX_bitset_clear(&ctx.bb_to_consider);
		pending->count = 0;
		for (unsigned i = 0; i < worklist->count; i++) {
			int changed_p = iter == 0;
			GraphNode *bb = addr[i];
			GraphNodeList *nodes = forward_p ? raviX_predecessors(bb) : raviX_successors(bb);
			// TODO should we pass the nodes array to the join function?
			if (raviX_node_list_size(nodes) == 0)
				join_function(ctx.userdata, raviX_node_index(bb), true);
			else
				changed_p |= join_function(ctx.userdata, raviX_node_index(bb), false);
			if (changed_p && transfer_function(ctx.userdata, raviX_node_index(bb))) {
				GraphNodeList *list = forward_p ? raviX_successors(bb) : raviX_predecessors(bb);
				for (unsigned i = 0; i < raviX_node_list_size(list); i++) {
					nodeId_t index = raviX_node_list_at(list, i);
					/* If this bb is not already been added to pending then add it */
					if (raviX_bitset_set_bit_p(&ctx.bb_to_consider, index)) {
						array_push(pending, raviX_graph_node(ctx.g, index));
					}
				}
			}
		}
		iter++;
		{
			/* Swap worklist and pending */
			GraphNodeArray *t = worklist;
			worklist = pending;
			pending = t;
		}
	}

	finish_data_flow(&ctx);
}

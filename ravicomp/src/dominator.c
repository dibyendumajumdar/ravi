#include "dominator.h"

#include "ravi_compiler.h"
#include "graph.h"

#include <assert.h>

/*
 * The dominator tree construction algorithm is based on figure 9.24,
 * chapter 9, p 532, of Engineering a Compiler.
 *
 * The algorithm is also described in the paper 'A Simple, Fast
 * Dominance Algorithm' by Keith D. Cooper, Timothy J. Harvey and
 * Ken Kennedy.
 */

/*
Some terminology:

DOM(b): A node n in the CFG dominates b if n lies on every path from the entry node of the CFG to b.
	DOM9b) contains every node n that dominates b.

IDOM(b): For a node b, the set IDOM(b) contains exactly one node, the immediate dominator of b.
	 If n is b's immediate dominator then every node in {DOM(b) - b} is also in DOM(n).

The dominator tree algorithm is an optimised version of forward data flow solver. The
algorithm iterates until a fixed point is reached. The output of the algorithm is the IDOM
array that describes the dominator tree.
*/

struct dominator_tree {
	Graph *g;
	GraphNode **IDOM; /* IDOM[] - array of immediate dominators, one per node in the graph, indexed by node id */
	uint32_t N;	    /* sizeof IDOM */
};

struct dominator_tree *raviX_new_dominator_tree(Graph *g)
{
	struct dominator_tree *state = (struct dominator_tree *)calloc(1, sizeof(struct dominator_tree));
	state->N = raviX_graph_size(g);
	state->IDOM = (GraphNode **)calloc(state->N, sizeof(GraphNode *));
	state->g = g;
	return state;
}

void raviX_destroy_dominator_tree(struct dominator_tree *state)
{
	free(state->IDOM);
	free(state);
}

/* Finds nearest common ancestor */
/* The algorithm starts at the two nodes whose sets are being intersected, and walks
 * upward from each toward the root. By comparing the nodes with their RPO numbers
 * the algorithm finds the common ancestor - the immediate dominator of i and j.
 */
static GraphNode *intersect(struct dominator_tree *state, GraphNode *i, GraphNode *j)
{
	GraphNode *finger1 = i;
	GraphNode *finger2 = j;
	while (finger1 != finger2) {
		while (raviX_node_RPO(finger1) > raviX_node_RPO(finger2)) {
			finger1 = state->IDOM[raviX_node_index(finger1)];
			assert(finger1);
		}
		while (raviX_node_RPO(finger2) > raviX_node_RPO(finger1)) {
			finger2 = state->IDOM[raviX_node_index(finger2)];
			assert(finger2);
		}
	}
	return finger1;
}

/* Look for the first predecessor whose immediate dominator has been calculated.
 * Because of the order in which this search occurs, we will always find at least 1
 * such predecessor.
 */
static GraphNode *find_first_predecessor_with_idom(struct dominator_tree *state, GraphNodeList *predlist)
{
	for (uint32_t i = 0; i < raviX_node_list_size(predlist); i++) {
		nodeId_t id = raviX_node_list_at(predlist, i);
		if (state->IDOM[id])
			return raviX_graph_node(state->g, id);
	}
	return NULL;
}

/**
 * Calculates the dominator tree.
 * Before this is called the graph links should have been numbered in
 * reverse post order.
 */
void raviX_calculate_dominator_tree(struct dominator_tree *state)
{
	/*
	Some implementation details:
	The graph and links reference nodes by node ids.
	However the IDOM array references the node objects - i.e.
	pointers to 'GraphNode'. So we have some conversion from node id
	to the node, and vice versa at various points.
	*/

	uint32_t N = raviX_graph_size(state->g);
	GraphNode **nodes_in_reverse_postorder = raviX_graph_nodes_sorted_by_RPO(state->g, false);
	for (uint32_t i = 0; i < state->N; i++) {
		state->IDOM[i] = NULL; /* undefined - set to a invalid value */
	}
	// Set IDom entry for root to itself
	state->IDOM[ENTRY_BLOCK] = raviX_graph_node(state->g, ENTRY_BLOCK);
	bool changed = true;
	while (changed) {
		changed = false;
		// for all nodes, b, in reverse postorder (except root)
		for (uint32_t i = 0; i < N; i++) {
			GraphNode *b = nodes_in_reverse_postorder[i];
			nodeId_t bid = raviX_node_index(b);
			if (bid == ENTRY_BLOCK) // skip root
				continue;
			GraphNodeList *predecessors = raviX_predecessors(b); // Predecessors of b
			// NewIDom = first (processed) predecessor of b, pick one
			GraphNode *firstpred = find_first_predecessor_with_idom(state, predecessors);
			assert(firstpred != NULL);
			GraphNode *NewIDom = firstpred;
			// for all other predecessors, p, of b
			for (uint32_t k = 0; k < raviX_node_list_size(predecessors); k++) {
				nodeId_t pid = raviX_node_list_at(predecessors, k);
				GraphNode *p = raviX_graph_node(state->g, pid);
				if (p == firstpred)
					continue; // all other predecessors
				if (state->IDOM[raviX_node_index(p)] != NULL) {
					// i.e. IDoms[p] calculated
					NewIDom = intersect(state, p, NewIDom);
				}
			}
			if (state->IDOM[bid] != NewIDom) {
				state->IDOM[bid] = NewIDom;
				changed = true;
			}
		}
	}
	free(nodes_in_reverse_postorder);
}

void raviX_dominator_tree_output(struct dominator_tree *tree, FILE *fp)
{
	for (uint32_t i = 0; i < tree->N; i++) {
		fprintf(stdout, "IDOM[%d] = %d\n", i, raviX_node_index(tree->IDOM[i]));
	}
}
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

#ifndef ravicomp_GRAPH_H
#define ravicomp_GRAPH_H

#include "allocate.h"
#include "common.h"

#include <stdint.h>
#include <stdbool.h>

/*
 * Various graph manipulation routines.
 * The graph is designed to manage nodes that are just integer ids.
 * Node ids range from [0..n) - hence one can simply represent nodes as arrays.
 *
 * The graph structure does not care what the node represents and
 * knows nothing about it. The benefit of this approach is that we can make
 * the graph algorithms reusable. There may be some performance cost as we
 * need to map node ids to nodes.
 *
 * The assumption here is that each node corresponds to a basic block in
 * the program intermediate code. And each basic block is identified by a node
 * id which can be used to construct the control flow graph.
 */

/* nodeId_t is declared elsewhere */
#ifndef RAVIX_GRAPH_DEFINED
#define RAVIX_GRAPH_DEFINED
typedef struct Graph Graph;
#endif
typedef struct GraphNode GraphNode;
typedef struct GraphNodeList GraphNodeList;
enum EdgeType {
	EDGE_TYPE_UNCLASSIFIED = 0,
	EDGE_TYPE_TREE = 1,
	EDGE_TYPE_BACKWARD = 2,
	EDGE_TYPE_FORWARD = 4,
	EDGE_TYPE_CROSS = 8
};


/* Initialize the graph data structure and associate some userdata with it. */
Graph *raviX_init_graph(nodeId_t entry, nodeId_t exit, void *userdata, C_MemoryAllocator *allocator);
/* Destroy the graph data structure */
void raviX_destroy_graph(Graph *g);

/* Add an edge from one node a to b. Both nodes a and b will be implicitly added
 * to the graph if they do not already exist.
 */
void raviX_add_edge(Graph *g, nodeId_t a, nodeId_t b);
/* Check if an edge exists from one node a to b */
bool raviX_has_edge(Graph *g, nodeId_t a, nodeId_t b);
/* Delete an edge from a to b */
void raviX_delete_edge(Graph *g, nodeId_t a, nodeId_t b);
/* Get the edge classification for edge from a to b; this is only available if graph has been
 * analyzed for edges. */
enum EdgeType raviX_get_edge_type(Graph *g, nodeId_t a, nodeId_t b);

/* Get node identified by index */
GraphNode *raviX_graph_node(Graph *g, nodeId_t index);
/* Get the RPO - reverse post order index of the node */
uint32_t raviX_node_RPO(GraphNode *n);
/* Get the node's id */
nodeId_t raviX_node_index(GraphNode *n);
/* Get list of predecessors */
GraphNodeList *raviX_predecessors(GraphNode *n);
/* Get list of successors */
GraphNodeList *raviX_successors(GraphNode *n);

/* Number of entries in the node_list */
uint32_t raviX_node_list_size(GraphNodeList *list);
/* Get the nodeId at given node_link position */
nodeId_t raviX_node_list_at(GraphNodeList *list, uint32_t i);

void raviX_for_each_node(Graph *g, void (*callback)(void *arg, Graph *g, nodeId_t nodeid), void *arg);

/*
 * Classifies links in the graph and also computes the
 * reverse post order value.
 */
void raviX_classify_edges(Graph *g);
/*
 * Returns a sorted array (allocated).
 * Sorted by reverse postorder value.
 * If forward=true then
 * it will be the opposite direction, so to get reverse postorder,
 * set forward=false.
 * You must deallocate the array when done.
 * The array size will be equal to raviX_graph_size(g).
 * Before attempting to sort, you must have called
 * raviX_classify_edges(g).
 */
GraphNode **raviX_graph_nodes_sorted_by_RPO(Graph *g, bool forward);

void raviX_sort_nodes_by_RPO(GraphNode **nodes, size_t count, bool forward);

/* says how many nodes are in the graph */
uint32_t raviX_graph_size(Graph *g);
/* Generates GraphViz (dot) output */
void raviX_draw_graph(Graph *g, FILE *fp);


#endif
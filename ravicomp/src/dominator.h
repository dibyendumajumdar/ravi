#ifndef ravicomp_DOMINATOR_H
#define ravicomp_DOMINATOR_H

#include "graph.h"

#include <stdio.h>

struct dominator_tree;

struct dominator_tree *raviX_new_dominator_tree(Graph *g);
void raviX_calculate_dominator_tree(struct dominator_tree *state);
void raviX_destroy_dominator_tree(struct dominator_tree *state);
void raviX_dominator_tree_output(struct dominator_tree *tree, FILE *fp);


#endif
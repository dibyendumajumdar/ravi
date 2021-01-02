#ifndef ravicomp_DATAFLOW_FRAMEWORK_H
#define ravicomp_DATAFLOW_FRAMEWORK_H

#include "graph.h"
#include <stdbool.h>

/*
 * Data Flow Analysis framework.
 * The Join/Transfer functions should return 1 if they made any changes else 0.
 */
extern void raviX_solve_dataflow(
    Graph *g,
    bool forward_p, /* Set to true for forward data flow */
    int (*join_function)(void *userdata, nodeId_t, bool init), /* Join/Meet operator - if init is true reset the bitsets */
    int (*transfer_function)(void *userdata, nodeId_t), /* transfer function */
    void *userdata);  /* pointer to user data, will be passed to join/transfer functions */


#endif
/******************************************************************************
 * Copyright (C) 2020-2023 Dibyendu Majumdar
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
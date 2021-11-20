/******************************************************************************
 * Copyright (C) 2020-2021 Dibyendu Majumdar
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

/* Build CFG */


#include "graph.h"
#include "cfg.h"

#include <assert.h>

/* Recursively create control flow graph for each proc
 * Return 0 on success
 */
int raviX_construct_cfg(Proc *proc)
{
	Graph *g = raviX_init_graph(ENTRY_BLOCK, EXIT_BLOCK, proc, proc->linearizer->ast_container->allocator);
	for (unsigned i = 0; i < proc->node_count; i++) {
		BasicBlock *block = proc->nodes[i];
		Instruction *insn = raviX_last_instruction(block);
		if (insn == NULL)
			continue;
		if (insn->opcode == op_br || insn->opcode == op_cbr || insn->opcode == op_ret) {
			Pseudo *pseudo;
			FOR_EACH_PTR(insn->targets, Pseudo, pseudo)
			{
				assert(pseudo->type == PSEUDO_BLOCK);
				raviX_add_edge(g, block->index, pseudo->block->index);
			}
			END_FOR_EACH_PTR(pseudo)
		} else {
			return 1;
		}
	}
	proc->cfg = g;
	Proc *childproc;
	FOR_EACH_PTR(proc->procs, Proc, childproc)
	{
		if (raviX_construct_cfg(childproc) != 0)
			return 1;
	}
	END_FOR_EACH_PTR(childproc)
	return 0;
}

struct CfgArg {
	FILE *fp;
	Proc *proc;
};

static void output_node(void *arg, Graph *g, uint32_t nodeid)
{
	struct CfgArg *myargs = (struct CfgArg *)arg;
	FILE *fp = myargs->fp;
	Proc *proc = myargs->proc;
	GraphNodeList *successors = raviX_successors(raviX_graph_node(g, nodeid));
	if (!successors)
		return;
	BasicBlock *block = proc->nodes[nodeid];
	if (raviX_ptrlist_size((const PtrList *)block->insns) > 0) {
		TextBuffer buf;
		raviX_buffer_init(&buf, 1024);
		raviX_output_basic_block_as_table(proc, block, &buf);
		fprintf(fp, "L%d [shape=none, margin=0, label=<%s>];\n", nodeid, raviX_buffer_data(&buf));
		raviX_buffer_free(&buf);
	}
	for (unsigned i = 0; i < raviX_node_list_size(successors); i++) {
		fprintf(fp, "L%d -> L%d\n", nodeid, raviX_node_list_at(successors, i));
	}
	Proc *childproc;
	FOR_EACH_PTR(proc->procs, Proc, childproc) { raviX_output_cfg(childproc, fp); }
	END_FOR_EACH_PTR(childproc)
}

void raviX_output_cfg(Proc *proc, FILE *fp)
{
	Graph *g = proc->cfg;
	if (!g)
		return;
	fprintf(fp, "digraph Proc%d {\n", proc->id);
	struct CfgArg args = {fp, proc};
	raviX_for_each_node(g, output_node, &args);
	fprintf(fp, "}\n");
}
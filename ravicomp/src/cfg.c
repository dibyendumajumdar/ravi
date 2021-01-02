/* Build CFG */


#include "graph.h"
#include "cfg.h"

#include <assert.h>

/* Recursively create control flow graph for each proc
 * Return 0 on success
 */
int raviX_construct_cfg(Proc *proc)
{
	Graph *g = raviX_init_graph(ENTRY_BLOCK, EXIT_BLOCK, proc);
	for (unsigned i = 0; i < proc->node_count; i++) {
		BasicBlock *block = proc->nodes[i];
		Instruction *insn = raviX_last_instruction(block);
		if (insn == NULL)
			continue;
		if (insn->opcode == op_br || insn->opcode == op_cbr || insn->opcode == op_ret) {
			Pseudo *pseudo;
			FOR_EACH_PTR(insn->targets, pseudo)
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
	FOR_EACH_PTR(proc->procs, childproc)
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
	if (raviX_ptrlist_size((const struct ptr_list *)block->insns) > 0) {
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
	FOR_EACH_PTR(proc->procs, childproc) { raviX_output_cfg(childproc, fp); }
	END_FOR_EACH_PTR(childproc)
}

void raviX_output_cfg(Proc *proc, FILE *fp)
{
	Graph *g = proc->cfg;
	if (!g)
		return;
	fprintf(fp, "digraph Proc%d {\n", proc->id);
	struct CfgArg args = {.proc = proc, .fp = fp};
	raviX_for_each_node(g, output_node, &args);
	fprintf(fp, "}\n");
}
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

/*
 * Calculate variable liveness
 * This will use the Dataflow Framework.
 * Implementation inspired by one in MIR
 */


#include "bitset.h"
#include "dataflow_framework.h"
#include "linearizer.h"
#include "allocate.h"

struct liveness_info {
	nodeId_t node_id;
	BitSet in;
	BitSet out;
	BitSet use;
	BitSet def;
};

DECLARE_ARRAY(liveness_info_array, struct liveness_info *);

struct liveness_data {
	Proc *proc;
	struct liveness_info_array lives;
};

static void init_liveness_data(Proc *proc, struct liveness_data *liveness_data)
{
	memset(liveness_data, 0, sizeof(*liveness_data));
	for (unsigned i = 0; i < proc->node_count; i++) {
		struct liveness_info *liveness_info = (struct liveness_info *)raviX_calloc(1, sizeof(struct liveness_info));
		liveness_info->node_id = i;
		raviX_bitset_create(&liveness_info->use);
		raviX_bitset_create(&liveness_info->def);
		raviX_bitset_create(&liveness_info->in);
		raviX_bitset_create(&liveness_info->out);
		array_push(&liveness_data->lives, struct liveness_info *, liveness_info);
	}
}

static void destroy_liveness_data(struct liveness_data *liveness_data)
{
	for (unsigned i = 0; i < liveness_data->lives.count; i++) {
		raviX_bitset_create(&liveness_data->lives.data[i]->use);
		raviX_bitset_create(&liveness_data->lives.data[i]->def);
		raviX_bitset_create(&liveness_data->lives.data[i]->in);
		raviX_bitset_create(&liveness_data->lives.data[i]->out);
	}
	array_clearmem(&liveness_data->lives);
}

static inline struct liveness_info *get_liveness_info(struct liveness_data *liveness_data, nodeId_t id)
{
	return liveness_data->lives.data[id];
}

/* Life analysis */
static int live_join_func(void *userdata, nodeId_t id, bool init)
{
	struct liveness_data *liveness_data = (struct liveness_data *)userdata;
	struct liveness_info *liveness_info = get_liveness_info(liveness_data, id);
	if (init) {
		raviX_bitset_clear(&liveness_info->in);
		return 0;
	} else {
		GraphNodeList *successors = raviX_successors(raviX_graph_node(liveness_data->proc->cfg, id));
		int changed = 0;
		// out[n] = Union of in[s] where s in succ[n]
		for (unsigned i = 0; i < raviX_node_list_size(successors); i++) {
			nodeId_t succ_id = raviX_node_list_at(successors, i);
			struct liveness_info *successor_liveness_info = get_liveness_info(liveness_data, succ_id);
			changed |=
			    raviX_bitset_ior(&liveness_info->out, &liveness_info->out, &successor_liveness_info->in);
		}
		return changed;
	}
}

static int live_transfer_func(void *userdata, nodeId_t id)
{
	struct liveness_data *liveness_data = (struct liveness_data *)userdata;
	struct liveness_info *liveness_info = get_liveness_info(liveness_data, id);
	// out[n] = use[n] U (out[n] - def[n])
	// In bitset terms out[n] = use[n] | (out[n] & ~def[n])
	return raviX_bitset_ior_and_compl(&liveness_info->in, &liveness_info->use, &liveness_info->out,
					  &liveness_info->def);
}

// TODO

// Compute use/def sets of each node
// If a reg appears as the target of an instruction that's a def
// If a reg is used as operand then its a use
// Need to handle ranges / var args too
// Or should we restrict analysis to certain types of regs?

// Right now we have disjoint sets for temps / locals  - to do this efficiently we need a merged set of regs for each proc
// Liveness analysis is a backward data flow problem
// see calculate_func_cfg_live_info  in mir_genc.c

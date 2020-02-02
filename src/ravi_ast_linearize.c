/*
Copyright (C) 2018-2020 Dibyendu Majumdar
*/

#include "ravi_ast.h"
#include "ptrlist.h"
#include "ravi_fnv_hash.h"

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

static inline unsigned alloc_reg(struct pseudo_generator *generator) {
  if (generator->free_pos > 0) {
    return generator->free_regs[--generator->free_pos];
  }
  return generator->next_reg++;
}

static inline void free_reg(struct pseudo_generator *generator, unsigned reg) {
  if (generator->free_pos == (sizeof generator->free_regs / sizeof generator->free_regs[0])) {
    /* TODO proper error handling */
    fprintf(stderr, "Out of register space\n");
    abort();
  }
  generator->free_regs[generator->free_pos++] = (uint8_t)reg;
}

/* Linearizer - WIP  */
static void ravi_init_linearizer(struct linearizer *linearizer, struct ast_container *container) {
  memset(linearizer, 0, sizeof *linearizer);
  linearizer->ast_container = container;
  dmrC_allocator_init(&linearizer->edge_allocator, "edge_allocator", sizeof(struct edge), sizeof(double), CHUNK);
  dmrC_allocator_init(&linearizer->instruction_allocator, "instruction_allocator", sizeof(struct instruction),
                      sizeof(double), CHUNK);
  dmrC_allocator_init(&linearizer->ptrlist_allocator, "ptrlist_allocator", sizeof(struct ptr_list), sizeof(double),
                      CHUNK);
  dmrC_allocator_init(&linearizer->pseudo_allocator, "pseudo_allocator", sizeof(struct pseudo), sizeof(double), CHUNK);
  dmrC_allocator_init(&linearizer->basic_block_allocator, "basic_block_allocator", sizeof(struct basic_block),
                      sizeof(double), CHUNK);
  dmrC_allocator_init(&linearizer->proc_allocator, "proc_allocator", sizeof(struct proc), sizeof(double), CHUNK);
  dmrC_allocator_init(&linearizer->unsized_allocator, "unsized_allocator", 0, sizeof(double), CHUNK);
  dmrC_allocator_init(&linearizer->constant_allocator, "constant_allocator", sizeof(struct constant), sizeof(double),
                      CHUNK);
}

static void ravi_destroy_linearizer(struct linearizer *linearizer) {
  struct proc *proc;
  FOR_EACH_PTR(linearizer->all_procs, proc) {
    if (proc->constants)
      set_destroy(proc->constants, NULL);
  }
  END_FOR_EACH_PTR(proc);
  dmrC_allocator_destroy(&linearizer->edge_allocator);
  dmrC_allocator_destroy(&linearizer->instruction_allocator);
  dmrC_allocator_destroy(&linearizer->ptrlist_allocator);
  dmrC_allocator_destroy(&linearizer->pseudo_allocator);
  dmrC_allocator_destroy(&linearizer->basic_block_allocator);
  dmrC_allocator_destroy(&linearizer->proc_allocator);
  dmrC_allocator_destroy(&linearizer->unsized_allocator);
  dmrC_allocator_destroy(&linearizer->constant_allocator);
}

static int compare_constants(const void *a, const void *b) {
  const struct constant *c1 = (const struct constant *)a;
  const struct constant *c2 = (const struct constant *)b;
  if (c1->type != c2->type)
    return 1;
  if (c1->type == RAVI_TNUMINT)
    return c1->i == c2->i;
  else if (c1->type == RAVI_TNUMFLT)
    return c1->n == c2->n;
  else
    return c1->s == c2->s;
}

static uint32_t hash_constant(const void *c) {
  const struct constant *c1 = (const struct constant *)c;
  if (c1->type == RAVI_TNUMINT)
    return c1->i;
  else if (c1->type == RAVI_TNUMFLT)
    return (int)c1->n;  // FIXME maybe use Lua's hash gen
  else
    return c1->s->hash;
}

static const struct constant *allocate_constant(struct proc *proc, struct ast_node *node) {
  assert(node->type == AST_LITERAL_EXPR);
  struct constant c = {.type = node->literal_expr.type.type_code};
  if (c.type == RAVI_TNUMINT)
    c.i = node->literal_expr.u.i;
  else if (c.type == RAVI_TNUMFLT)
    c.n = node->literal_expr.u.n;
  else
    c.s = node->literal_expr.u.s;
  struct set_entry *entry = set_search(proc->constants, &c);
  if (entry == NULL) {
    int reg = proc->num_constants++;
    struct constant *c1 = dmrC_allocator_allocate(&proc->linearizer->constant_allocator, 0);
    assert(c1);
    memcpy(c1, &c, sizeof *c1);
    c1->index = reg;
    set_add(proc->constants, c1);
    printf("Created new constant and assigned reg %d\n", reg);
    return c1;
  }
  else {
    const struct constant *c1 = entry->key;
    printf("Found constant at reg %d\n", c1->index);
    return c1;
  }
}

struct pseudo *allocate_local_pseudo(struct proc *proc, struct lua_symbol *sym, unsigned reg) {
  assert(sym->symbol_type == SYM_LOCAL);
  assert(sym->var.pseudo == NULL);
  struct pseudo *pseudo = dmrC_allocator_allocate(&proc->linearizer->pseudo_allocator, 0);
  pseudo->type = PSEUDO_LOCAL;
  pseudo->symbol = sym;
  pseudo->regnum = reg;
  sym->var.pseudo = pseudo;
  return pseudo;
}

struct pseudo *allocate_constant_pseudo(struct proc *proc, const struct constant *constant) {
  struct pseudo *pseudo = dmrC_allocator_allocate(&proc->linearizer->pseudo_allocator, 0);
  pseudo->type = PSEUDO_CONSTANT;
  pseudo->constant = constant;
  pseudo->regnum = constant->index;
  return pseudo;
}

/**
 * Allocate a new proc. If there is a current proc, then the new proc gets added to the
 * current procs children.
 */
static struct proc *allocate_proc(struct linearizer *linearizer, struct ast_node *function_expr) {
  assert(function_expr->type == AST_FUNCTION_EXPR);
  struct proc *proc = dmrC_allocator_allocate(&linearizer->proc_allocator, 0);
  proc->function_expr = function_expr;
  ptrlist_add((struct ptr_list **)&linearizer->all_procs, proc, &linearizer->ptrlist_allocator);
  if (linearizer->current_proc) {
    proc->parent = linearizer->current_proc;
    ptrlist_add((struct ptr_list **)&linearizer->current_proc->procs, proc, &linearizer->ptrlist_allocator);
  }
  proc->constants = set_create(hash_constant, compare_constants);
  proc->linearizer = linearizer;
  return proc;
}

static void set_main_proc(struct linearizer *linearizer, struct proc *proc) {
  assert(linearizer->main_proc == NULL);
  assert(linearizer->current_proc == NULL);
  linearizer->main_proc = proc;
  assert(proc->function_expr->function_expr.parent_function == NULL);
}

static inline void set_current_proc(struct linearizer *linearizer, struct proc *proc) {
  linearizer->current_proc = proc;
}

static void linearize_function_args(struct linearizer *linearizer) {
  struct proc *proc = linearizer->current_proc;
  struct ast_node *func_expr = proc->function_expr;
  struct lua_symbol *sym;
  FOR_EACH_PTR(func_expr->function_expr.args, sym) {
    // printf("Assigning register %d to argument %s\n", (int)reg, getstr(sym->var.var_name));
  }
  END_FOR_EACH_PTR(sym);
}

static void linearize_statement(struct proc *proc, struct ast_node *node);
static void linearize_statement_list(struct proc *proc, struct ast_node_list *list) {
  struct ast_node *node;
  FOR_EACH_PTR(list, node) { linearize_statement(proc, node); }
  END_FOR_EACH_PTR(node);
}

static struct instruction *alloc_instruction(struct proc *proc, enum opcode op) {
  struct instruction *insn = dmrC_allocator_allocate(&proc->linearizer->instruction_allocator, 0);
  insn->opcode = op;
  return insn;
}

static struct pseudo *linearize_literal(struct proc *proc, struct ast_node *expr) {
  assert(expr->type == AST_LITERAL_EXPR);
  if (expr->literal_expr.type.type_code == RAVI_TNUMFLT || expr->literal_expr.type.type_code == RAVI_TNUMINT ||
      expr->literal_expr.type.type_code == RAVI_TSTRING) {
    return allocate_constant_pseudo(proc, allocate_constant(proc, expr));
  }
  else {
    abort();
    return NULL;
  }
}

static struct pseudo *linearize_expr(struct proc *proc, struct ast_node *expr) {
  switch (expr->type) {
    case AST_LITERAL_EXPR: {
      return linearize_literal(proc, expr);
    } break;

    default:
      abort();
      break;
  }
  assert(false);
  return NULL;
}

static void linearize_expr_list(struct proc *proc, struct ast_node_list *expr_list, struct instruction *insn,
                                struct pseudo_list **pseudo_list) {
  struct ast_node *expr;
  FOR_EACH_PTR(expr_list, expr) {
    struct pseudo *pseudo = linearize_expr(proc, expr);
    ptrlist_add((struct ptr_list **)pseudo_list, pseudo, &proc->linearizer->ptrlist_allocator);
  }
  END_FOR_EACH_PTR(expr);
}

static void linearize_return(struct proc *proc, struct ast_node *node) {
  assert(node->type == AST_RETURN_STMT);
  struct instruction *insn = alloc_instruction(proc, OP_RET);
  linearize_expr_list(proc, node->return_stmt.expr_list, insn, &insn->ret_instruction.expr_list);
}

static void linearize_statement(struct proc *proc, struct ast_node *node) {
  switch (node->type) {
    case AST_FUNCTION_EXPR: {
      /* args need type assertions but those have no ast - i.e. code gen should do it */
      // typecheck_ast_list(container, function, node->function_expr.function_statement_list);
      break;
    }
    case AST_NONE: {
      break;
    }
    case AST_RETURN_STMT: {
      linearize_return(proc, node);
      // typecheck_ast_list(container, function, node->return_stmt.expr_list);
      break;
    }
    case AST_LOCAL_STMT: {
      // typecheck_local_statement(container, function, node);
      break;
    }
    case AST_FUNCTION_STMT: {
      // typecheck_ast_node(container, function, node->function_stmt.function_expr);
      break;
    }
    case AST_LABEL_STMT: {
      break;
    }
    case AST_GOTO_STMT: {
      break;
    }
    case AST_DO_STMT: {
      break;
    }
    case AST_EXPR_STMT: {
      // typecheck_expr_statement(container, function, node);
      break;
    }
    case AST_IF_STMT: {
      // typecheck_if_statement(container, function, node);
      break;
    }
    case AST_WHILE_STMT:
    case AST_REPEAT_STMT: {
      // typecheck_while_or_repeat_statement(container, function, node);
      break;
    }
    case AST_FORIN_STMT: {
      // typecheck_for_in_statment(container, function, node);
      break;
    }
    case AST_FORNUM_STMT: {
      // typecheck_for_num_statment(container, function, node);
      break;
    }
    case AST_SUFFIXED_EXPR: {
      // typecheck_suffixedexpr(container, function, node);
      break;
    }
    case AST_FUNCTION_CALL_EXPR: {
      // if (node->function_call_expr.method_name) {
      //}
      // else {
      //}
      // typecheck_ast_list(container, function, node->function_call_expr.arg_list);
      break;
    }
    case AST_SYMBOL_EXPR: {
      /* symbol type should have been set when symbol was created */
      // copy_type(node->symbol_expr.type, node->symbol_expr.var->value_type);
      break;
    }
    case AST_BINARY_EXPR: {
      // typecheck_binaryop(container, function, node);
      break;
    }
    case AST_UNARY_EXPR: {
      // typecheck_unaryop(container, function, node);
      break;
    }
    case AST_LITERAL_EXPR: {
      /* type set during parsing */
      break;
    }
    case AST_FIELD_SELECTOR_EXPR: {
      // typecheck_ast_node(container, function, node->index_expr.expr);
      break;
    }
    case AST_Y_INDEX_EXPR: {
      // typecheck_ast_node(container, function, node->index_expr.expr);
      break;
    }
    case AST_INDEXED_ASSIGN_EXPR: {
      // if (node->indexed_assign_expr.index_expr) {
      //  typecheck_ast_node(container, function, node->indexed_assign_expr.index_expr);
      //}
      // typecheck_ast_node(container, function, node->indexed_assign_expr.value_expr);
      // copy_type(node->indexed_assign_expr.type, node->indexed_assign_expr.value_expr->common_expr.type);
      break;
    }
    case AST_TABLE_EXPR: {
      // typecheck_ast_list(container, function, node->table_expr.expr_list);
      break;
    }
    default:
      assert(0);
  }
}

/**
 * Creates and initializes a basic block to be an empty block. Returns the new basic block.
 */
static struct basic_block *create_block(struct proc *proc) {
  if (proc->node_count >= proc->allocated) {
    unsigned new_size = proc->allocated + 25;
    struct node **new_data =
        dmrC_allocator_allocate(&proc->linearizer->unsized_allocator, new_size * sizeof(struct node *));
    assert(new_data != NULL);
    if (proc->node_count > 0) {
      memcpy(new_data, proc->nodes, proc->allocated * sizeof(struct node *));
    }
    proc->allocated = new_size;
    proc->nodes = new_data;
  }
  assert(proc->node_count < proc->allocated);
  struct basic_block *new_block = dmrC_allocator_allocate(&proc->linearizer->basic_block_allocator, 0);
  proc->nodes[proc->node_count++] = bb2n(new_block);
  return new_block;
}

/**
 * Takes a basic block as an argument and makes it the current block.
 * All future instructions will be added to the end of this block
 */
static void start_block(struct proc *proc, struct basic_block *bb) { proc->current_bb = bb; }

/**
 * Create the initial blocks entry and exit for the proc.
 * sets current block to entry block.
 */
static void initialize_graph(struct proc *proc) {
  assert(proc != NULL);
  proc->entry = bb2n(create_block(proc));
  proc->exit = bb2n(create_block(proc));
  start_block(proc, n2bb(proc->entry));
}

/**
 * Makes given scope the current scope, and allocates registers for locals.
 */
static void start_scope(struct linearizer *linearizer, struct proc *proc, struct block_scope *scope) {
  proc->current_scope = scope;
  struct lua_symbol *sym;
  FOR_EACH_PTR(scope->symbol_list, sym) {
    if (sym->symbol_type == SYM_LOCAL) {
      uint8_t reg = alloc_reg(&proc->local_pseudos);
      allocate_local_pseudo(proc, sym, reg);
      printf("Assigning register %d to local %s\n", (int)reg, getstr(sym->var.var_name));
    }
  }
  END_FOR_EACH_PTR(sym);
}

/**
 * Deallocate local registers when the scope ends, in reverse order
 * so that we have a stack discipline, and then changes current scope to be the
 * parent scope.
 */
static void end_scope(struct linearizer *linearizer, struct proc *proc) {
  struct block_scope *scope = proc->current_scope;
  struct lua_symbol *sym;
  FOR_EACH_PTR_REVERSE(scope->symbol_list, sym) {
    if (sym->symbol_type == SYM_LOCAL) {
      struct pseudo *pseudo = sym->var.pseudo;
      assert(pseudo && pseudo->type == PSEUDO_LOCAL && pseudo->symbol == sym);
      printf("Free register %d for local %s\n", (int)pseudo->regnum, getstr(sym->var.var_name));
      free_reg(&proc->local_pseudos, pseudo->regnum);
    }
  }
  END_FOR_EACH_PTR_REVERSE(sym);
  proc->current_scope = scope->parent;
}

static void linearize_function(struct linearizer *linearizer) {
  struct proc *proc = linearizer->current_proc;
  assert(proc != NULL);
  struct ast_node *func_expr = proc->function_expr;
  assert(func_expr->type == AST_FUNCTION_EXPR);
  initialize_graph(proc);
  start_scope(linearizer, proc, func_expr->function_expr.main_block);
  linearize_function_args(linearizer);
  linearize_statement_list(proc, func_expr->function_expr.function_statement_list);
  end_scope(linearizer, proc);
}

void raviA_ast_linearize(struct linearizer *linearizer, struct ast_container *container) {
  ravi_init_linearizer(linearizer, container);
  struct proc *proc = allocate_proc(linearizer, container->main_function);
  set_main_proc(linearizer, proc);
  set_current_proc(linearizer, proc);
  linearize_function(linearizer);
  ravi_destroy_linearizer(linearizer);
}

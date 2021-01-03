/*
 * Generic pointer list manipulation code.
 *
 * (C) Copyright Linus Torvalds 2003-2005
 */
/*
 * This version is part of the dmr_c project.
 * Copyright (C) 2017-2021 Dibyendu Majumdar
 */

#ifndef ravicomp_PTRLIST_H
#define ravicomp_PTRLIST_H

#include <allocate.h>

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The ptrlist data structure is copied from the Linux Sparse project.
 * It is essentially a dynamic array of pointers but the array is split up
 * into nodes that are linked together. Each node contains a small number of array entries.
 *
 * The ptr list data structure is like a train - with cars linked to each other.
 * Just as in a train each car has many seats, so in ptr list each "node" has
 * several entries. Unlike a train however, the ptr list is arranged as a ring,
 * i.e. the the front and back nodes are linked to each other. Hence there is no
 * such thing as a 'head' of the list - i.e. any node can be the head!
 *
 * The disadvantage of the ptrlist structure compared to a dynamic array is
 * that it consumes more memory to maintain the linked list data structure.
 *
 * The main advantage is that it is well suited to fixed sized memory
 * allocators as there is no resizing of memory already allocated, which happens
 * with dynamic arrays. The ptrlist is made up of fixed size nodes.
 */

/* number of array entries per node */
#ifndef LIST_NODE_NR
#define LIST_NODE_NR (7)
#endif

/* structure of a node */
#define DECLARE_PTR_LIST(listname, type)                                                                               \
	typedef struct listname {                                                                                      \
		int nr_ : 8;                                                                                           \
		int rm_ : 8;                                                                                           \
		struct listname *prev_;                                                                                \
		struct listname *next_;                                                                                \
		Allocator *allocator_;                                                                                 \
		type *list_[LIST_NODE_NR];                                                                             \
	} listname

/* Each node in the list */
DECLARE_PTR_LIST(ptr_list, void);

/* The iterator strucure is used for looping */
typedef struct PtrListIterator {
	struct ptr_list *__head;
	struct ptr_list *__list;
	int __nr;
} PtrListIterator;

/* The ptr list */
extern int raviX_ptrlist_size(const struct ptr_list *self);
extern void **raviX_ptrlist_add(struct ptr_list **self, void *ptr, Allocator *ptr_list_allocator);
extern void *raviX_ptrlist_nth_entry(struct ptr_list *list, unsigned int idx);
extern void *raviX_ptrlist_first(struct ptr_list *list);
extern void *raviX_ptrlist_last(struct ptr_list *list);
extern int raviX_ptrlist_linearize(struct ptr_list *head, void **arr, int max);
extern void raviX_ptrlist_split_node(struct ptr_list *head);
extern void raviX_ptrlist_pack(struct ptr_list **self);
extern void raviX_ptrlist_remove_all(struct ptr_list **self);
extern int raviX_ptrlist_remove(struct ptr_list **self, void *entry, int count);
extern int raviX_ptrlist_replace(struct ptr_list **self, void *old_ptr, void *new_ptr, int count);
extern void *raviX_ptrlist_undo_last(struct ptr_list **self);
extern void *raviX_ptrlist_delete_last(struct ptr_list **self);
extern void raviX_ptrlist_concat(struct ptr_list *a, struct ptr_list **self);
extern void raviX_ptrlist_sort(struct ptr_list **self, void *, int (*cmp)(void *, const void *, const void *));

/* iterator functions */
extern PtrListIterator raviX_ptrlist_forward_iterator(struct ptr_list *self);
extern PtrListIterator raviX_ptrlist_reverse_iterator(struct ptr_list *self);
extern void *raviX_ptrlist_iter_next(PtrListIterator *self);
extern void *raviX_ptrlist_iter_prev(PtrListIterator *self);
extern void raviX_ptrlist_iter_split_current(PtrListIterator *self);
extern void raviX_ptrlist_iter_insert(PtrListIterator *self, void *newitem);
extern void raviX_ptrlist_iter_remove(PtrListIterator *self);
extern void raviX_ptrlist_iter_set(PtrListIterator *self, void *ptr);
extern void raviX_ptrlist_iter_mark_deleted(PtrListIterator *self);

static inline void **raviX_ptrlist_iter_this_address(PtrListIterator *self) { return &self->__list->list_[self->__nr]; }
#define ptr_list_empty(x) ((x) == NULL)
#define PTR_ENTRY_NOTAG(h, i) ((h)->list_[i])
#define PTR_ENTRY(h, i) (void *)(PTR_ENTRY_NOTAG(h, i))

#define FOR_EACH_PTR(list, var)                                                                                        \
	{                                                                                                              \
		PtrListIterator var##iter__ = raviX_ptrlist_forward_iterator((struct ptr_list *)list);                 \
		for (var = raviX_ptrlist_iter_next(&var##iter__); var != NULL;                                         \
		     var = raviX_ptrlist_iter_next(&var##iter__))
#define END_FOR_EACH_PTR(var) }

#define FOR_EACH_PTR_REVERSE(list, var)                                                                                \
	{                                                                                                              \
		PtrListIterator var##iter__ = raviX_ptrlist_reverse_iterator((struct ptr_list *)list);                 \
		for (var = raviX_ptrlist_iter_prev(&var##iter__); var != NULL;                                         \
		     var = raviX_ptrlist_iter_prev(&var##iter__))
#define END_FOR_EACH_PTR_REVERSE(var) }

#define RECURSE_PTR_REVERSE(list, var)                                                                                 \
	{                                                                                                              \
		PtrListIterator var##iter__ = list##iter__;                                                            \
		for (var = raviX_ptrlist_iter_prev(&var##iter__); var != NULL;                                         \
		     var = raviX_ptrlist_iter_prev(&var##iter__))

#define PREPARE_PTR_LIST(list, var)                                                                                    \
	PtrListIterator var##iter__ = raviX_ptrlist_forward_iterator((struct ptr_list *)list);                         \
	var = raviX_ptrlist_iter_next(&var##iter__)

#define NEXT_PTR_LIST(var) var = raviX_ptrlist_iter_next(&var##iter__)
#define FINISH_PTR_LIST(var)

#define THIS_ADDRESS(type, var) (type *)raviX_ptrlist_iter_this_address(&var##iter__)

#define DELETE_CURRENT_PTR(var) raviX_ptrlist_iter_remove(&var##iter__)

#define REPLACE_CURRENT_PTR(type, var, replacement) raviX_ptrlist_iter_set(&var##iter__, replacement)

#define INSERT_CURRENT(newval, var) raviX_ptrlist_iter_insert(&var##iter__, newval)

#define MARK_CURRENT_DELETED(PTR_TYPE, var) raviX_ptrlist_iter_mark_deleted(&var##iter__)

#ifdef __cplusplus
}
#endif

#endif

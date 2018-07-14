#ifndef DMR_C_PTRLIST_H
#define DMR_C_PTRLIST_H

/*
* Generic pointer list manipulation code.
*
* (C) Copyright Linus Torvalds 2003-2005
*/
/*
* This version is part of the dmr_c project.
* Copyright (C) 2017 Dibyendu Majumdar
*/

#include <allocate.h>

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
* The ptr list data structure is like a train - with cars linked to each other.
* Just as in a train each car has many seats, so in ptr list each "node" has
* several entries. Unlike a train however, the ptr list is arranged as a ring,
* i.e. the the front and back nodes are linked to each other. Hence there is no
* such thing as a 'head' of the list - i.e. any node can be the head!
*/

#ifndef LIST_NODE_NR
#define LIST_NODE_NR (29)
#endif

#define DECLARE_PTR_LIST(listname, type)                                       \
	struct listname {                                                      \
		int nr_ : 8;                                                   \
		int rm_ : 8;                                                   \
		struct listname *prev_;                                        \
		struct listname *next_;                                        \
		struct allocator *allocator_;                                  \
		type *list_[LIST_NODE_NR];                                     \
	}

/* Each node in the list */
DECLARE_PTR_LIST(ptr_list, void);

struct ptr_list_iter {
	struct ptr_list *__head;
	struct ptr_list *__list;
	int __nr;
};

/* The ptr list */
extern int ptrlist_size(const struct ptr_list *self);
extern void **ptrlist_add(struct ptr_list **self, void *ptr, struct allocator *alloc);
extern void *ptrlist_nth_entry(struct ptr_list *list, unsigned int idx);
extern void *ptrlist_first(struct ptr_list *list);
extern void *ptrlist_last(struct ptr_list *list);
extern int ptrlist_linearize(struct ptr_list *head, void **arr, int max);
extern void ptrlist_split_node(struct ptr_list *head);
extern void ptrlist_pack(struct ptr_list **self);
extern void ptrlist_remove_all(struct ptr_list **self);
extern int ptrlist_remove(struct ptr_list **self, void *entry, int count);
extern int ptrlist_replace(struct ptr_list **self, void *old_ptr, void *new_ptr,
	int count);
extern void *ptrlist_undo_last(struct ptr_list **self);
extern void *ptrlist_delete_last(struct ptr_list **self);
extern void ptrlist_concat(struct ptr_list *a, struct ptr_list **self);
extern void ptrlist_sort(struct ptr_list **self, void *,
			 int (*cmp)(void *, const void *, const void *));

/* iterator functions */
extern struct ptr_list_iter ptrlist_forward_iterator(struct ptr_list *self);
extern struct ptr_list_iter ptrlist_reverse_iterator(struct ptr_list *self);
extern void *ptrlist_iter_next(struct ptr_list_iter *self);
extern void *ptrlist_iter_prev(struct ptr_list_iter *self);
extern void ptrlist_iter_split_current(struct ptr_list_iter *self);
extern void ptrlist_iter_insert(struct ptr_list_iter *self, void *newitem);
extern void ptrlist_iter_remove(struct ptr_list_iter *self);
extern void ptrlist_iter_set(struct ptr_list_iter *self, void *ptr);
extern void ptrlist_iter_mark_deleted(struct ptr_list_iter *self);

static inline void **ptrlist_iter_this_address(struct ptr_list_iter *self) {
	return &self->__list->list_[self->__nr];
}
#define ptr_list_empty(x) ((x) == NULL)
#define PTR_ENTRY_NOTAG(h,i)	((h)->list_[i])
#define PTR_ENTRY(h,i)	(void *)(PTR_ENTRY_NOTAG(h,i))

#if 1

#define FOR_EACH_PTR(list, var) \
	{ struct ptr_list_iter var##iter__ = ptrlist_forward_iterator((struct ptr_list *)list); \
	for (var = ptrlist_iter_next(&var##iter__); var != NULL; var = ptrlist_iter_next(&var##iter__))
#define END_FOR_EACH_PTR(var) }

#define FOR_EACH_PTR_REVERSE(list, var) \
	{ struct ptr_list_iter var##iter__ = ptrlist_reverse_iterator((struct ptr_list *)list); \
	for (var = ptrlist_iter_prev(&var##iter__); var != NULL; var = ptrlist_iter_prev(&var##iter__))
#define END_FOR_EACH_PTR_REVERSE(var) }

#define RECURSE_PTR_REVERSE(list, var) \
	{ struct ptr_list_iter var##iter__ = list##iter__; \
	for (var = ptrlist_iter_prev(&var##iter__); var != NULL; var = ptrlist_iter_prev(&var##iter__))

#define PREPARE_PTR_LIST(list, var)	\
	struct ptr_list_iter var##iter__ = ptrlist_forward_iterator((struct ptr_list *)list); \
	var = ptrlist_iter_next(&var##iter__)

#define NEXT_PTR_LIST(var) \
	var = ptrlist_iter_next(&var##iter__)
#define FINISH_PTR_LIST(var) 

#define THIS_ADDRESS(type, var) \
	(type *)ptrlist_iter_this_address(&var##iter__)

#define DELETE_CURRENT_PTR(var) \
	ptrlist_iter_remove(&var##iter__)	

#define REPLACE_CURRENT_PTR(type, var, replacement) \
	ptrlist_iter_set(&var##iter__, replacement)

#define INSERT_CURRENT(newval, var) \
	ptrlist_iter_insert(&var##iter__, newval)	

#define MARK_CURRENT_DELETED(PTR_TYPE, var) \
	ptrlist_iter_mark_deleted(&var##iter__)

#else

#define DO_PREPARE(head, ptr, __head, __list, __nr, PTR_ENTRY)				\
	do {										\
		struct ptr_list *__head = (struct ptr_list *) (head);			\
		struct ptr_list *__list = __head;					\
		int __nr = 0;								\
		if (__head) ptr = PTR_ENTRY(__head, 0);					\
		else ptr = NULL

#define DO_NEXT(ptr, __head, __list, __nr, PTR_ENTRY)					\
		if (ptr) {								\
			if (++__nr < __list->nr_) {					\
				ptr = PTR_ENTRY(__list,__nr);				\
			} else {							\
				__list = __list->next_;					\
				ptr = NULL;						\
				while (__list->nr_ == 0 && __list != __head)		\
					__list = __list->next_;				\
				if (__list != __head) {					\
					__nr = 0;					\
					ptr = PTR_ENTRY(__list,0);			\
				}							\
			}								\
		}

#define DO_RESET(ptr, __head, __list, __nr, PTR_ENTRY)					\
	do {										\
		__nr = 0;								\
		__list = __head;							\
		if (__head) ptr = PTR_ENTRY(__head, 0);					\
	} while (0)

#define DO_FINISH(ptr, __head, __list, __nr)						\
		(void)(__nr); /* Sanity-check nesting */				\
	} while (0)

#define PREPARE_PTR_LIST(head, ptr) \
	DO_PREPARE(head, ptr, __head##ptr, __list##ptr, __nr##ptr, PTR_ENTRY)

#define NEXT_PTR_LIST(ptr) \
	DO_NEXT(ptr, __head##ptr, __list##ptr, __nr##ptr, PTR_ENTRY)

#define RESET_PTR_LIST(ptr) \
	DO_RESET(ptr, __head##ptr, __list##ptr, __nr##ptr, PTR_ENTRY)

#define FINISH_PTR_LIST(ptr) \
	DO_FINISH(ptr, __head##ptr, __list##ptr, __nr##ptr)

#define DO_FOR_EACH(head, ptr, __head, __list, __nr, PTR_ENTRY) do {			\
	struct ptr_list *__head = (struct ptr_list *) (head);				\
	struct ptr_list *__list = __head;						\
	if (__head) {									\
		do { int __nr;								\
			for (__nr = 0; __nr < __list->nr_; __nr++) {			\
				do {							\
					ptr = PTR_ENTRY(__list,__nr);			\
					if (__list->rm_ && !ptr)				\
						continue;				\
					do {

#define DO_END_FOR_EACH(ptr, __head, __list, __nr)					\
					} while (0);					\
				} while (0);						\
			}								\
		} while ((__list = __list->next_) != __head);				\
	}										\
} while (0)

#define DO_FOR_EACH_REVERSE(head, ptr, __head, __list, __nr, PTR_ENTRY) do {		\
	struct ptr_list *__head = (struct ptr_list *) (head);				\
	struct ptr_list *__list = __head;						\
	if (__head) {									\
		do { int __nr;								\
			__list = __list->prev_;						\
			__nr = __list->nr_;						\
			while (--__nr >= 0) {						\
				do {							\
					ptr = PTR_ENTRY(__list,__nr);			\
					if (__list->rm_ && !ptr)				\
						continue;				\
					do {


#define DO_END_FOR_EACH_REVERSE(ptr, __head, __list, __nr)				\
					} while (0);					\
				} while (0);						\
			}								\
		} while (__list != __head);						\
	}										\
} while (0)

#define DO_REVERSE(ptr, __head, __list, __nr, new, __newhead,				\
		   __newlist, __newnr, PTR_ENTRY) do { 					\
	struct ptr_list *__newhead = __head;						\
	struct ptr_list *__newlist = __list;						\
	int __newnr = __nr;								\
	new = ptr;									\
	goto __inside##new;								\
	if (1) {									\
		do {									\
			__newlist = __newlist->prev_;					\
			__newnr = __newlist->nr_;					\
	__inside##new:									\
			while (--__newnr >= 0) {					\
				do {							\
					new = PTR_ENTRY(__newlist,__newnr);		\
					do {

#define RECURSE_PTR_REVERSE(ptr, new)							\
	DO_REVERSE(ptr, __head##ptr, __list##ptr, __nr##ptr,				\
		   new, __head##new, __list##new, __nr##new, PTR_ENTRY)

#define DO_THIS_ADDRESS(PTR_TYPE, ptr, __head, __list, __nr)					\
	((PTR_TYPE*) (__list->list_ + __nr))


#define FOR_EACH_PTR(head, ptr) \
	DO_FOR_EACH(head, ptr, __head##ptr, __list##ptr, __nr##ptr, PTR_ENTRY)

#define END_FOR_EACH_PTR(ptr) \
	DO_END_FOR_EACH(ptr, __head##ptr, __list##ptr, __nr##ptr)

#define FOR_EACH_PTR_NOTAG(head, ptr) \
	DO_FOR_EACH(head, ptr, __head##ptr, __list##ptr, __nr##ptr, PTR_ENTRY_NOTAG)

#define END_FOR_EACH_PTR_NOTAG(ptr) END_FOR_EACH_PTR(ptr)

#define FOR_EACH_PTR_REVERSE(head, ptr) \
	DO_FOR_EACH_REVERSE(head, ptr, __head##ptr, __list##ptr, __nr##ptr, PTR_ENTRY)

#define END_FOR_EACH_PTR_REVERSE(ptr) \
	DO_END_FOR_EACH_REVERSE(ptr, __head##ptr, __list##ptr, __nr##ptr)

#define FOR_EACH_PTR_REVERSE_NOTAG(head, ptr) \
	DO_FOR_EACH_REVERSE(head, ptr, __head##ptr, __list##ptr, __nr##ptr, PTR_ENTRY_NOTAG)

#define END_FOR_EACH_PTR_REVERSE_NOTAG(ptr) END_FOR_EACH_PTR_REVERSE(ptr)

#define THIS_ADDRESS(PTR_TYPE, ptr) \
	DO_THIS_ADDRESS(PTR_TYPE, ptr, __head##ptr, __list##ptr, __nr##ptr)


#define DO_SPLIT(ptr, __head, __list, __nr) do {					\
	ptrlist_split_node(__list);							\
	if (__nr >= __list->nr_) {							\
		__nr -= __list->nr_;							\
		__list = __list->next_;							\
	};										\
} while (0)

#define DO_INSERT_CURRENT(new, ptr, __head, __list, __nr) do {				\
	void **__this, **__last;							\
	if (__list->nr_ == LIST_NODE_NR)							\
		DO_SPLIT(ptr, __head, __list, __nr);					\
	__this = __list->list_ + __nr;							\
	__last = __list->list_ + __list->nr_ - 1;						\
	while (__last >= __this) {							\
		__last[1] = __last[0];							\
		__last--;								\
	}										\
	*__this = (new);								\
	__list->nr_++;									\
} while (0)

#define INSERT_CURRENT(new, ptr) \
	DO_INSERT_CURRENT(new, ptr, __head##ptr, __list##ptr, __nr##ptr)

#define DO_DELETE_CURRENT(ptr, __head, __list, __nr) do {				\
	void **__this = __list->list_ + __nr;						\
	void **__last = __list->list_ + __list->nr_ - 1;					\
	while (__this < __last) {							\
		__this[0] = __this[1];							\
		__this++;								\
	}										\
	*__this = (void *)((uintptr_t)0xf0f0f0f0);							\
	__list->nr_--; __nr--;								\
} while (0)

#define DELETE_CURRENT_PTR(ptr) \
	DO_DELETE_CURRENT(ptr, __head##ptr, __list##ptr, __nr##ptr)

#define REPLACE_CURRENT_PTR(PTR_TYPE, ptr, new_ptr)						\
	do { *THIS_ADDRESS(PTR_TYPE, ptr) = (new_ptr); } while (0)

#define DO_MARK_CURRENT_DELETED(PTR_TYPE, ptr, __list) do {	\
		REPLACE_CURRENT_PTR(PTR_TYPE, ptr, NULL);		\
		__list->rm++;				\
	} while (0)

#define MARK_CURRENT_DELETED(PTR_TYPE, ptr) \
	DO_MARK_CURRENT_DELETED(PTR_TYPE, ptr, __list##ptr)
	
#endif

extern int test_ptrlist();

#ifdef __cplusplus
}
#endif


#endif

/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Doubly-linked list
 *
 * Copyright 2017 Phoenix Systems
 * Author: Jakub Sejdak
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_LIST_H_
#define _LIBPHOENIX_SYS_LIST_H_

#include ARCH


#define lib_listof(type, node_field, node) ({					\
	int _off = (int) &(((type *) 0)->node_field);				\
	(type *) ((node == NULL) ? NULL : ((void *) node - _off));	\
})


typedef struct _listnode_t {
	struct _listnode_t *next;
	struct _listnode_t *prev;
} listnode_t;


typedef int (*listcomp_t)(listnode_t *n1, listnode_t *n2);


typedef struct _list_t {
	listnode_t *head;
	listnode_t *tail;
	listcomp_t compare;
	size_t size;
} list_t;


extern void lib_listInit(list_t *list, listcomp_t compare);


extern void lib_listInsertAfter(list_t *list, listnode_t *prev, listnode_t *node);


extern void lib_listInsertBefore(list_t *list, listnode_t *next, listnode_t *node);


extern void lib_listAppend(list_t *list, listnode_t *node);


extern void lib_listPrepend(list_t *list, listnode_t *node);


extern void lib_listRemove(list_t *list, listnode_t *node);


extern listnode_t *lib_listFind(list_t *list, listnode_t *node);


extern listnode_t *lib_listHead(list_t *list);


extern listnode_t *lib_listTail(list_t *list);


extern size_t lib_listSize(list_t *list);


#endif

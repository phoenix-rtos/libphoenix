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

#include "list.h"


void lib_listInit(list_t *list, listcomp_t compare)
{
	list->head = NULL;
	list->tail = NULL;
	list->compare = compare;
	list->size = 0;
}


void lib_listInsertAfter(list_t *list, listnode_t *prev, listnode_t *node)
{
	node->prev = prev;

	if (prev->next == NULL) {
		node->next = NULL;
		list->tail = node;
	}
	else {
		node->next = prev->next;
		prev->next->prev = node;
	}

	prev->next = node;
	++list->size;
}


void lib_listInsertBefore(list_t *list, listnode_t *next, listnode_t *node)
{
	node->next = next;

	if (next->prev == NULL) {
		node->prev = NULL;
		list->head = node;
	}
	else {
		node->prev = next->prev;
		next->prev->next = node;
	}

	next->prev = node;
	++list->size;
}


void lib_listAppend(list_t *list, listnode_t *node)
{
	if (list->tail == NULL)
		lib_listPrepend(list, node);
	else
		lib_listInsertAfter(list, list->tail, node);
}


void lib_listPrepend(list_t *list, listnode_t *node)
{
	if (list->head == NULL) {
		list->head = node;
		list->tail = node;
		node->prev = NULL;
		node->next = NULL;
		++list->size;
		return;
	}

	lib_listInsertBefore(list, list->head, node);
}


void lib_listRemove(list_t *list, listnode_t *node)
{
	if (node->prev == NULL) {
		list->head = node->next;
		if (list->head != NULL)
			list->head->prev = NULL;
	}
	else
		node->prev->next = node->next;

	if (node->next == NULL) {
		list->tail = node->prev;
		if (list->tail != NULL)
			list->tail->next = NULL;
	}
	else
		node->next->prev = node->prev;

	--list->size;
}


listnode_t *lib_listFind(list_t *list, listnode_t *node)
{
	listnode_t *it = list->head;

	for (it = list->head; it != NULL; it = it->next) {
		if (list->compare(it, node) == 0)
			break;
	}

	return it;
}


listnode_t *lib_listHead(list_t *list)
{
	return list->head;
}


listnode_t *lib_listTail(list_t *list)
{
	return list->tail;
}


size_t lib_listSize(list_t *list)
{
	return list->size;
}

/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Doubly-linked list
 *
 * Copyright 2017 Phoenix Systems
 * Author: Jakub Sejdak, Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_LIST_H_
#define _LIBPHOENIX_SYS_LIST_H_

#include ARCH


#define LIST_ADD(list, i) { \
	if (*(list) == NULL) { \
		i->next = i; \
		i->prev = i; \
		(*(list)) = i; \
	} \
	else { \
		i->prev = (*(list))->prev; \
		(*(list))->prev->next = i; \
		i->next = (*list); \
		(*(list))->prev = i; \
	} \
}


#define  LIST_REMOVE(list, i) { \
	i->prev->next = i->next; \
	i->next->prev = i->prev; \
	if ((i->next == i) && (i->prev == i)) \
		(*(list)) = NULL; \
	else if (i == (*(list))) \
		(*(list)) = i->next; \
}


#endif

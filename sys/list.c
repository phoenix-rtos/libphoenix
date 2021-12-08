/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Doubly-linked list
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Jan Sikorski, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/list.h>


#define NODE(t, off) (*(void **)((uintptr_t)t + off))


void lib_listAdd(void **list, void *t, size_t noff, size_t poff)
{
	if (t == NULL)
		return;

	if (*list == NULL) {
		NODE(t, poff) = t;
		NODE(t, noff) = t;
		*list = t;
	}
	else {
		NODE(t, poff) = NODE(*list, poff);
		NODE(NODE(*list, poff), noff) = t;
		NODE(t, noff) = *list;
		NODE(*list, poff) = t;
	}
}


void lib_listRemove(void **list, void *t, size_t noff, size_t poff)
{
	if (t == NULL)
		return;

	if ((NODE(t, poff) == t) && (NODE(t, noff) == t)) {
		*list = NULL;
	}
	else {
		NODE(NODE(t, poff), noff) = NODE(t, noff);
		NODE(NODE(t, noff), poff) = NODE(t, poff);

		if (t == *list)
			*list = NODE(t, noff);
	}

	NODE(t, noff) = NULL;
	NODE(t, poff) = NULL;
}

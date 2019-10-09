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
#include <stdint.h>


void lib_listAdd(void **list, void *t, size_t noff, size_t poff)
{
	if (t == NULL)
		return;
	if (*list == NULL) {
		*((uintptr_t *)(t + noff)) = (uintptr_t)t;
		*((uintptr_t *)(t + poff)) = (uintptr_t)t;
		*list = t;
	}
	else {
		*((uintptr_t *)(t + poff)) = *((uintptr_t *)(*list + poff));
		*((uintptr_t *)((void *)*((uintptr_t *)(*list + poff)) + noff)) = (uintptr_t)t;
		*((uintptr_t *)(t + noff)) = *((uintptr_t *)list);
		*((uintptr_t *)(*list + poff)) = (uintptr_t)t;
	}
}


void lib_listRemove(void **list, void *t, size_t noff, size_t poff)
{
	if (t == NULL)
		return;
	if (*((uintptr_t *)(t + noff)) == (uintptr_t)t && *((uintptr_t *)(t + poff)) == (uintptr_t)t) {
		*list = NULL;
	}
	else {
		*((uintptr_t *)((void *)(*((uintptr_t *)(t + poff))) + noff)) = *((uintptr_t *)(t + noff));
		*((uintptr_t *)((void *)(*((uintptr_t *)(t + noff))) + poff)) = *((uintptr_t *)(t + poff));
		if (t == *list)
			*list = (void *)*((uintptr_t *)(t + noff));
	}
	*((uintptr_t *)(t + noff)) = (uintptr_t)NULL;
	*((uintptr_t *)(t + poff)) = (uintptr_t)NULL;
}

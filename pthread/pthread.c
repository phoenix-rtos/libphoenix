/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Doubly-linked list
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "pthread.h"


int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
	void *stack;

	if ((stack = malloc(PTHREAD_MIN_STACK)) == NULL)
		return -ENOMEM;

	beginthread(start_routine, priority, stack, arg);
}


int pthread_attr_init(pthread_attr_t *attr)
{
	return EOK;
}


int pthread_attr_destroy(pthread_attr_t *attr)
{
	return EOK;
}

/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * atexit.c
 *
 * Copyright 2019 2022 Phoenix Systems
 * Author: Kamil Amanowicz, Dawid Szpejna
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <string.h>
#include <sys/threads.h>


/* The atexit_node structure uses uint32_t for flags that
 indicate whether to call a function with arguments or not, therefore
 why ATEXIT_MAX must be 32 as type "fntype" */
#define ATEXIT_MAX 32


typedef void (*destructor_t)(void);


struct atexit_node {
	destructor_t destructors[ATEXIT_MAX];
	void *args[ATEXIT_MAX];
	void *handles[ATEXIT_MAX];
	uint32_t fntype;
	struct atexit_node *prev;
};


/* The first node is statically allocated to provide at least 32 function slots */
static struct {
	handle_t lock;
	struct atexit_node *head;
	struct atexit_node *newestNode;
	unsigned int idx;
} atexit_common = { .head = &((struct atexit_node) {}) };


/* Initialise atexit_common structure before main */
void _atexit_init(void)
{
	mutexCreate(&atexit_common.lock);
	memset(atexit_common.head, 0, sizeof(struct atexit_node));
	atexit_common.idx = 0;
	atexit_common.newestNode = atexit_common.head;
}


/* Generic function to register destructors */
static int _atexit_register(int isarg, void (*fn)(void), void *arg, void *handle)
{
	struct atexit_node *node;

	mutexLock(atexit_common.lock);
	node = atexit_common.head;

	/* Allocate new node if there are no free slots left */
	if (atexit_common.idx == ATEXIT_MAX) {
		node = (struct atexit_node *)calloc(1, sizeof(struct atexit_node));
		if (node == NULL) {
			mutexUnlock(atexit_common.lock);
			return -1;
		}
		node->prev = atexit_common.head;

		atexit_common.head = node;
		atexit_common.idx = 0;
		atexit_common.newestNode = node;
	}

	if (isarg != 0) {
		node->fntype |= (1u << atexit_common.idx);
		node->args[atexit_common.idx] = arg;
	}
	node->destructors[atexit_common.idx] = fn;
	node->handles[atexit_common.idx] = handle;

	atexit_common.idx++;

	mutexUnlock(atexit_common.lock);
	return 0;
}


/* Call destructors registered for given object, or all in case of NULL */
/* Conforming: https://itanium-cxx-abi.github.io/cxx-abi/abi.html#dso-dtor */
void __cxa_finalize(void *handle)
{
	/* No handlers registered. */
	if (atexit_common.idx == 0) {
		return;
	}

	mutexLock(atexit_common.lock);
	/* Iteration has to be over the atexit_common.head and newest node must be restored at the end
	 * as the atexit functions may register new atexit functions. */
	while (atexit_common.head != NULL) {
		atexit_common.idx--;
		destructor_t destructor = atexit_common.head->destructors[atexit_common.idx];
		/* Do not call already called destructors and destructors not from current handle. */
		if ((destructor != NULL) && ((handle == atexit_common.head->handles[atexit_common.idx]) || (handle == NULL))) {
			/* Mark destructor as called. */
			atexit_common.head->destructors[atexit_common.idx] = NULL;

			if (((atexit_common.head->fntype >> atexit_common.idx) & 1) == 1) {
				void *arg = atexit_common.head->args[atexit_common.idx];
				mutexUnlock(atexit_common.lock);
				((void (*)(void *))destructor)(arg);
			}
			else {
				mutexUnlock(atexit_common.lock);
				destructor();
			}
			mutexLock(atexit_common.lock);
		}

		if (atexit_common.idx == 0) {
			atexit_common.head = atexit_common.head->prev;
			atexit_common.idx = ATEXIT_MAX;
		}
	}

	atexit_common.head = atexit_common.newestNode;

	/* All nodes are fully emptied from destructors, free memory. */
	if (handle == NULL) {
		/* Ensure the first node that is statically allocated is not freed */
		while (atexit_common.head->prev != NULL) {
			struct atexit_node *last = atexit_common.head;
			atexit_common.head = atexit_common.head->prev;
			free(last);
		}
		atexit_common.newestNode = atexit_common.head;
		atexit_common.idx = 0;
	}
	else {
		/* Restore idx. */
		unsigned int i = ATEXIT_MAX;
		while ((i > 0) && (atexit_common.head->destructors[i - 1] == NULL)) {
			i--;
		}
		atexit_common.idx = i;
	}

	mutexUnlock(atexit_common.lock);
}


int atexit(void (*func)(void))
{
	return _atexit_register(0, func, NULL, NULL);
}


/* Register a function to run at process termination with given arguments */
int __cxa_atexit(void (*func)(void *), void *arg, void *handle)
{
	return _atexit_register(1, (void (*)(void))func, arg, handle);
}

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
	uint32_t fntype;
	struct atexit_node *prev;
};


/* The first node is statically allocated to provide at least 32 function slots */
static struct {
	handle_t lock;
	struct atexit_node *head;
	unsigned int idx;
} atexit_common = { .head = &((struct atexit_node) {}) };


/* Initialise atexit_common structure before main */
void _atexit_init(void)
{
	mutexCreate(&atexit_common.lock);
	memset(atexit_common.head, 0, sizeof(struct atexit_node));
	atexit_common.idx = 0;
}


/* Generic function to register destructors */
static int _atexit_register(int isarg, void (*fn)(void), void *arg)
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
	}

	if (isarg != 0) {
		node->fntype |= (1u << atexit_common.idx);
		node->args[atexit_common.idx] = arg;
	}
	node->destructors[atexit_common.idx] = fn;

	atexit_common.idx++;

	mutexUnlock(atexit_common.lock);
	return 0;
}


/* Call all registered destructors */
void _atexit_call(void)
{
	mutexLock(atexit_common.lock);
	while ((atexit_common.head != NULL) && (atexit_common.idx != 0)) {
		atexit_common.idx--;
		destructor_t destructor = atexit_common.head->destructors[atexit_common.idx];
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

		if (atexit_common.idx == 0) {
			struct atexit_node *last = atexit_common.head;
			atexit_common.head = atexit_common.head->prev;
			if (atexit_common.head != NULL) {
				/* Free only if it's not the first node (statically allocated) */
				free(last);
				atexit_common.idx = ATEXIT_MAX;
			}
		}
	}
	mutexUnlock(atexit_common.lock);
}


int atexit(void (*func)(void))
{
	return _atexit_register(0, func, NULL);
}


/* Register a function to run at process termination with given arguments */
int __cxa_atexit(void (*func)(void *), void *arg, void *handler)
{
	return _atexit_register(1, (void (*)(void))func, arg);
}

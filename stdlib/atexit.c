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
struct atexit_node {
	void (*destructors[ATEXIT_MAX])(void);
	void *args[ATEXIT_MAX];
	uint32_t fntype;
	struct atexit_node *prev;
};


/* The first node is statically allocated to provide at least 32 function slots */
static struct {
	handle_t lock;
	struct atexit_node *head;
} atexit_common = { .head = &((struct atexit_node) {}) };


/* Initialise atexit_common structure before main */
void _atexit_init(void)
{
	mutexCreate(&atexit_common.lock);
	memset(atexit_common.head, 0, sizeof(struct atexit_node));
}


/* Generic function to register destructors */
static int _atexit_register(int isarg, void (*fn)(void), void *arg)
{
	struct atexit_node *node;
	int i;

	mutexLock(atexit_common.lock);
	node = atexit_common.head;

	/* Looking for an empty slot */
	for (i = 0; i < ATEXIT_MAX; i++) {
		if (node->destructors[i] == NULL) {
			break;
		}
	}

	/* Allocate new node if there are no free slots left */
	if (i == ATEXIT_MAX) {
		node = (struct atexit_node *)calloc(1, sizeof(struct atexit_node));
		if (node == NULL) {
			mutexUnlock(atexit_common.lock);
			return -1;
		}
		node->prev = atexit_common.head;
		atexit_common.head = node;

		i = 0;
	}

	if (isarg) {
		node->fntype |= 1u << i;
		node->args[i] = arg;
	}
	node->destructors[i] = fn;

	mutexUnlock(atexit_common.lock);
	return 0;
}


/* Call all registered destructors */
void _atexit_call(void)
{
	struct atexit_node *last, *node = atexit_common.head;
	int i;

	while (node != NULL) {
		for (i = ATEXIT_MAX - 1; i >= 0; i--) {
			if (node->destructors[i] != NULL) {
				if (((node->fntype >> i) & 1) == 1) {
					((void (*)(void *))node->destructors[i])(node->args[i]);
				}
				else {
					node->destructors[i]();
				}
			}
		}

		last = node;
		node = node->prev;
		if (node != NULL) {
			/* Free only if it's not the first node (statically allocated) */
			free(last);
		}
	}
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

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
#include <sys/threads.h>


#define ATEXIT_MAX 32


typedef void (*destructor_t)(void);


struct atexit_node {
	destructor_t destructors[ATEXIT_MAX];
	void *args[ATEXIT_MAX];
	void *handles[ATEXIT_MAX];
	uint32_t fntype; /* i-th bit set means i-th function accepts an arg */
	struct atexit_node *prev;
};


_Static_assert(ATEXIT_MAX <= sizeof(((struct atexit_node *)0)->fntype) * 8, "ATEXIT_MAX must fit into fntype bitmask");


/*
 * List of functions to be called on termination. Nodes other than the (optional)
 * statically allocated firstNode are allocated on demand.
 */
struct atexit_list {
	struct atexit_node *head;
	struct atexit_node *newestNode;
	struct atexit_node *firstNode;
	unsigned int idx;
};


/* The first node of the exit list is statically allocated to provide at least 32 function slots */
static struct atexit_node atexit_firstNode;


static struct {
	struct atexit_list exitList;
	/* POSIX-DEVIATION: Nodes of the quick exit list are allocated only if at_quick_exit() is used, to optimize memory use. */
	struct atexit_list quickExitList;
	handle_t lock;
} atexit_common = {
	.exitList = {
		.head = &atexit_firstNode,
		.newestNode = &atexit_firstNode,
		.firstNode = &atexit_firstNode,
	},
};


/* Initialise exit lists before main */
void _atexit_init(void)
{
	mutexCreate(&atexit_common.lock);
}


/* Generic function to register destructors */
static int _atexit_register(struct atexit_list *list, int isarg, void (*fn)(void), void *arg, void *handle)
{
	struct atexit_node *node;

	mutexLock(atexit_common.lock);
	node = list->head;

	/* Allocate new node if there are no free slots left */
	if ((node == NULL) || (list->idx == ATEXIT_MAX)) {
		node = (struct atexit_node *)calloc(1, sizeof(struct atexit_node));
		if (node == NULL) {
			mutexUnlock(atexit_common.lock);
			return -1;
		}
		node->prev = list->head;

		list->head = node;
		list->idx = 0;
		list->newestNode = node;
	}

	if (isarg != 0) {
		node->fntype |= (1u << list->idx);
		node->args[list->idx] = arg;
	}
	else {
		node->fntype &= ~(1u << list->idx);
		node->args[list->idx] = NULL;
	}

	node->destructors[list->idx] = fn;
	node->handles[list->idx] = handle;

	list->idx++;

	mutexUnlock(atexit_common.lock);
	return 0;
}


/* Generic function to call destructors registered for given object, or all in case of NULL */
static void _atexit_finalize(struct atexit_list *list, void *handle)
{
	struct atexit_node *node, *prev;
	unsigned int i;

	mutexLock(atexit_common.lock);

	if ((list->head == NULL) || ((list->idx == 0) && (list->head->prev == NULL))) {
		mutexUnlock(atexit_common.lock);
		return;
	}

	/*
	 * Iteration has to be over the list->head and newest node must be restored at the end
	 * as the atexit functions may register new atexit functions.
	 */
	while (list->head != NULL) {
		if (list->idx == 0) {
			list->head = list->head->prev;
			/* Nodes other than the newest one are always full */
			list->idx = ATEXIT_MAX;
			continue;
		}

		list->idx--;
		destructor_t destructor = list->head->destructors[list->idx];
		/* Do not call already called destructors and destructors not from current handle. */
		if ((destructor != NULL) && ((handle == list->head->handles[list->idx]) || (handle == NULL))) {
			/* Mark destructor as called. */
			list->head->destructors[list->idx] = NULL;

			if (((list->head->fntype >> list->idx) & 1) == 1) {
				void *arg = list->head->args[list->idx];
				mutexUnlock(atexit_common.lock);
				((void (*)(void *))destructor)(arg);
			}
			else {
				mutexUnlock(atexit_common.lock);
				destructor();
			}
			mutexLock(atexit_common.lock);
		}
	}

	node = list->newestNode;
	while (node != list->firstNode) {
		for (i = 0; i < ATEXIT_MAX; i++) {
			if (node->destructors[i] != NULL) {
				break;
			}
		}

		if (i != ATEXIT_MAX) {
			break;
		}

		prev = node->prev;
		free(node);
		node = prev;
	}

	list->head = node;
	list->newestNode = node;

	i = ATEXIT_MAX;
	if (node == NULL) {
		i = 0;
	}
	else {
		while ((i > 0) && (node->destructors[i - 1] == NULL)) {
			i--;
		}
	}
	list->idx = i;

	mutexUnlock(atexit_common.lock);
}


/* Call destructors registered for given object, or all in case of NULL */
/* Conforming: https://itanium-cxx-abi.github.io/cxx-abi/abi.html#dso-dtor */
void __cxa_finalize(void *handle)
{
	_atexit_finalize(&atexit_common.exitList, handle);
}


/* Call functions registered with at_quick_exit() */
void _quick_exit_finalize(void)
{
	_atexit_finalize(&atexit_common.quickExitList, NULL);
}


int atexit(void (*func)(void))
{
	return _atexit_register(&atexit_common.exitList, 0, func, NULL, NULL);
}


int __cxa_atexit(void (*func)(void *), void *arg, void *handle)
{
	return _atexit_register(&atexit_common.exitList, 1, (void (*)(void))func, arg, handle);
}


int __cxa_at_quick_exit(void (*func)(void *), void *arg, void *handle)
{
	return _atexit_register(&atexit_common.quickExitList, 1, (void (*)(void))func, arg, handle);
}


int at_quick_exit(void (*func)(void))
{
	return _atexit_register(&atexit_common.quickExitList, 0, func, NULL, NULL);
}

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
#include <arch.h>


void __cxa_thread_atexit_init(void);
void __cxa_thread_atexit_run(void);
int __cxa_thread_atexit_impl(void (*)(void *), void *, void *);


#ifdef __LIBPHOENIX_ARCH_TLS_SUPPORTED


void __libc__dl_cxa_refcount(void *addr, ssize_t delta)
{
	/* Do nothing in static lib */
	(void)addr;
	(void)delta;
}


extern void __dl_cxa_refcount(void *, ssize_t) __attribute__((weak, alias("__libc__dl_cxa_refcount")));


struct __cxa_atexit_node {
	struct __cxa_atexit_node *prev;
	void (*dtor)(void *);
	void *obj;
	void *handle;
};


static __thread struct __cxa_atexit_node *__cxa_thread_atexit_head;


extern int can_use_tls;


void __cxa_thread_atexit_init(void)
{
	/* Special case for dynamic linker exit */
	if (can_use_tls != 0) {
		atexit(__cxa_thread_atexit_run);
	}
}


void __cxa_thread_atexit_run(void)
{
	while (__cxa_thread_atexit_head != NULL) {
		struct __cxa_atexit_node *last = __cxa_thread_atexit_head;

		__cxa_thread_atexit_head->dtor(__cxa_thread_atexit_head->obj);
		__cxa_thread_atexit_head = __cxa_thread_atexit_head->prev;

		if (last->handle != NULL) {
			__dl_cxa_refcount(last->handle, -1);
		}

		free(last);
	}
}


/* C++ thread-local destructor handler */
int __cxa_thread_atexit_impl(void (*dtor)(void *), void *obj, void *handle)
{
	struct __cxa_atexit_node *node = malloc(sizeof(struct __cxa_atexit_node));

	if (node == NULL) {
		return -1;
	}

	node->dtor = dtor;
	node->obj = obj;
	node->handle = handle;
	node->prev = __cxa_thread_atexit_head;

	if (node->handle != NULL) {
		__dl_cxa_refcount(node->handle, 1);
	}

	__cxa_thread_atexit_head = node;

	return 0;
}


#else


void __cxa_thread_atexit_init(void)
{
	/* Nothing to do */
}


void __cxa_thread_atexit_run(void)
{
	/* Nothing to do */
}


/* NOTE: no support for __cxa_thread_atexit_impl on platforms not supporting TLS. */
int __cxa_thread_atexit_impl(void (*dtor)(void *), void *obj, void *handle)
{
	(void)dtor;
	(void)obj;
	(void)handle;

	abort();
}


#endif

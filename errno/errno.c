/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * errno
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/rb.h>
#include <sys/threads.h>
#include <errno.h>
#include <arch.h>


#ifdef __LIBPHOENIX_ARCH_TLS_SUPPORTED

static __thread int __errno_tls;

#else

static int errno_global;

struct {
	rbtree_t tree;
	handle_t lock;
} errno_common;


static int errno_cmp(rbnode_t *n1, rbnode_t *n2)
{
	struct __errno_t *e1 = lib_treeof(struct __errno_t, linkage, n1);
	struct __errno_t *e2 = lib_treeof(struct __errno_t, linkage, n2);

	return (e1->tid < e2->tid) - (e1->tid > e2->tid);
}
#endif


int *__errno_location(void)
{
#ifndef __LIBPHOENIX_ARCH_TLS_SUPPORTED
	struct __errno_t *e, r;
	r.tid = gettid();

	mutexLock(errno_common.lock);
	e = lib_treeof(struct __errno_t, linkage, lib_rbFind(&errno_common.tree, &r.linkage));
	mutexUnlock(errno_common.lock);

	if (e != NULL)
		return &e->no;

	return &errno_global;
#else
	return &__errno_tls;
#endif
}


void _errno_new(struct __errno_t *e)
{
#ifndef __LIBPHOENIX_ARCH_TLS_SUPPORTED
	e->no = 0;
	e->tid = gettid();

	mutexLock(errno_common.lock);
	lib_rbInsert(&errno_common.tree, &e->linkage);
	mutexUnlock(errno_common.lock);
#endif
}


void _errno_remove(struct __errno_t *e)
{
#ifndef __LIBPHOENIX_ARCH_TLS_SUPPORTED
	mutexLock(errno_common.lock);
	lib_rbRemove(&errno_common.tree, &e->linkage);
	mutexUnlock(errno_common.lock);
#endif
}


void _errno_init(void)
{
#ifndef __LIBPHOENIX_ARCH_TLS_SUPPORTED
	mutexCreate(&errno_common.lock);
	lib_rbInit(&errno_common.tree, errno_cmp, NULL);
#endif
}

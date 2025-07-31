/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/threads
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/threads.h>
#include <sys/futex.h>
#include <posix/idtree.h>
#include <errno.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#undef interrupt
extern int interrupt(unsigned int n, int (*f)(unsigned int, void *), void *arg, handle_t cond, handle_t *handle);

#undef resourceDestroy
extern int resourceDestroy(handle_t);

struct _mutex_t;

static int _mutexLock(struct _mutex_t *mutex);
static void _mutexUnlock(struct _mutex_t *mutex);
static void _mutexInit(struct _mutex_t *mutex);

typedef uint32_t mutex_state_t;

enum {
	MUTEX_UNLOCKED = 0,
	MUTEX_LOCKED = 1,
	MUTEX_CONTENDED = 2,
};

typedef struct _mutex_t {
	_Atomic(mutex_state_t) futex;
	idnode_t linkage;
} mutex_t;

typedef uint32_t cond_state_t;

typedef struct _cond_t {
	_Atomic(cond_state_t) value;
	idnode_t linkage;
} cond_t;

static struct {
	idtree_t mutexes;
	idtree_t conds;
	mutex_t lock;
} uresource_common;

void _uresource_init(void)
{
	idtree_init(&uresource_common.mutexes);
	idtree_init(&uresource_common.conds);
	_mutexInit(&uresource_common.lock);
}

static mutex_t *_mutexGetFromTree(handle_t m)
{
	_mutexLock(&uresource_common.lock);
	idnode_t *id = idtree_find(&uresource_common.mutexes, (int)m);
	mutex_t *mutex = lib_treeof(mutex_t, linkage, (void *)id);
	if (mutex == NULL) {
		_mutexUnlock(&uresource_common.lock);
		return NULL;
	}
	_mutexUnlock(&uresource_common.lock);
	return mutex;
}

static cond_t *_condGetFromTree(handle_t m)
{
	_mutexLock(&uresource_common.lock);
	idnode_t *id = idtree_find(&uresource_common.conds, (int)m);
	cond_t *cond = lib_treeof(cond_t, linkage, (void *)id);
	if (cond == NULL) {
		_mutexUnlock(&uresource_common.lock);
		return NULL;
	}
	_mutexUnlock(&uresource_common.lock);
	return cond;
}

static void _mutexInit(mutex_t *mutex)
{
	memset(mutex, 0, sizeof(*mutex));
}

static int _mutexWait(mutex_t *mutex, mutex_state_t desired)
{
	return phFutexWait(&mutex->futex, desired, 0);
}

static int _mutexWakeup(mutex_t *mutex)
{
	return phFutexWakeup(&mutex->futex, 1);
}

static int _mutexLock(mutex_t *mutex)
{
	mutex_state_t state = MUTEX_UNLOCKED;
	if (!atomic_compare_exchange_weak(&mutex->futex, &state, MUTEX_LOCKED)) {
		if (state != MUTEX_CONTENDED) {
			state = atomic_exchange(&mutex->futex, MUTEX_CONTENDED);
		}

		while (state != MUTEX_UNLOCKED) {
			_mutexWait(mutex, MUTEX_CONTENDED);

			state = atomic_exchange(&mutex->futex, MUTEX_CONTENDED);
		}
	}
	return EOK;
}

static int _mutexTryLock(mutex_t *mutex)
{
	mutex_state_t state = MUTEX_UNLOCKED;
	if (atomic_compare_exchange_weak(&mutex->futex, &state, MUTEX_LOCKED)) {
		return EOK;
	}
	return -EBUSY;
}

static void _mutexUnlock(mutex_t *mutex)
{
	if (atomic_fetch_sub(&mutex->futex, 1) != MUTEX_LOCKED) {
		atomic_store(&mutex->futex, MUTEX_UNLOCKED);
		_mutexWakeup(mutex);
	}
}

int mutexCreate(handle_t *h)
{
	mutex_t *mutex = malloc(sizeof(*mutex));
	if (mutex == NULL) {
		return -ENOMEM;
	}
	_mutexInit(mutex);

	_mutexLock(&uresource_common.lock);
	*((int *)h) = idtree_alloc(&uresource_common.mutexes, &mutex->linkage);
	_mutexUnlock(&uresource_common.lock);
	return EOK;
}

int mutexLock(handle_t m)
{
	mutex_t *mutex = _mutexGetFromTree(m);
	if (mutex == NULL) {
		return -EINVAL;
	}

	return _mutexLock(mutex);
}

int __mutexUnlock(handle_t m)
{
	mutex_t *mutex = _mutexGetFromTree(m);
	if (mutex == NULL) {
		return -EINVAL;
	}

	_mutexUnlock(mutex);
	return EOK;
}

int __mutexTry(handle_t m)
{
	mutex_t *mutex = _mutexGetFromTree(m);
	if (mutex == NULL) {
		return -EINVAL;
	}

	return _mutexTryLock(mutex);
}

int mutexLock2(handle_t m1, handle_t m2)
{
	int err;
	int tmp;

	if ((err = mutexLock(m1)) < 0)
		return err;

	while (mutexTry(m2) < 0) {
		mutexUnlock(m1);
		if ((err = mutexLock(m2)) < 0)
			return err;

		tmp = m1;
		m1 = m2;
		m2 = tmp;
	}

	return EOK;
}

int __resourceDestroy(handle_t r)
{
	mutex_t *mutex = _mutexGetFromTree(r);
	if (mutex != NULL) {
		_mutexLock(&uresource_common.lock);
		idtree_remove(&uresource_common.mutexes, &mutex->linkage);
		_mutexUnlock(&uresource_common.lock);
		free(mutex);
		return EOK;
	}
	cond_t *cond = _condGetFromTree(r);
	if (cond != NULL) {
		_mutexLock(&uresource_common.lock);
		idtree_remove(&uresource_common.conds, &cond->linkage);
		_mutexUnlock(&uresource_common.lock);
		free(cond);
		return EOK;
	}
	return -EINVAL;
}

static void _condInit(cond_t *cond)
{
	memset(cond, 0, sizeof(*cond));
}


int condCreate(handle_t *h)
{
	cond_t *cond = malloc(sizeof(*cond));
	if (cond == NULL) {
		return -ENOMEM;
	}
	_condInit(cond);

	_mutexLock(&uresource_common.lock);
	*((int *)h) = idtree_alloc(&uresource_common.conds, &cond->linkage);
	_mutexUnlock(&uresource_common.lock);
	return EOK;
}


int condWait(handle_t h, handle_t m, time_t timeout)
{
	cond_t *cond = _condGetFromTree(h);
	if (cond == NULL) {
		return -EINVAL;
	}
	mutex_t *mutex = _mutexGetFromTree(m);
	if (mutex == NULL) {
		return -EINVAL;
	}

	cond_state_t value = atomic_load_explicit(&cond->value, memory_order_seq_cst);
	while (value & 1) {
		if (atomic_compare_exchange_strong_explicit(
					&cond->value,
					&value,
					value + 1,
					memory_order_relaxed,
					memory_order_relaxed)) {
			value++;
		}
	}
	_mutexUnlock(mutex);
	int ret;
	do {
		ret = phFutexWait(&cond->value, value, timeout);
	} while (ret == -EINTR);
	_mutexLock(mutex);
	return EOK;
}

static int _condWakeupN(cond_t *cond, uint32_t n)
{
	atomic_fetch_or_explicit(&cond->value, 1, memory_order_relaxed);
	return phFutexWakeup(&cond->value, n);
}

int __condSignal(handle_t h)
{
	cond_t *cond = _condGetFromTree(h);
	if (cond == NULL) {
		return -EINVAL;
	}

	return _condWakeupN(cond, 1);
}

int __condBroadcast(handle_t h)
{
	cond_t *cond = _condGetFromTree(h);
	if (cond == NULL) {
		return -EINVAL;
	}

	return _condWakeupN(cond, FUTEX_WAKEUP_ALL);
}

/* TODO: this code probably should be in sys/interrupt.c, but this is ok for now */
int __interrupt(unsigned int n, int (*f)(unsigned int, void *), void *arg, handle_t cond_handle, handle_t *handle)
{
	cond_t *cond = _condGetFromTree(cond_handle);
	if (cond == NULL) {
		return -EINVAL;
	}

	handle_t addr = (handle_t)(uintptr_t)&cond->value;
	return interrupt(n, f, arg, addr, handle);
}

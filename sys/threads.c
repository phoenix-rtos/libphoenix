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
#include <sys/time.h>
#include <posix/idtree.h>
#include <errno.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sched.h>


typedef atomic_bool spinlock_t;


static void spinlock_lock(spinlock_t *lock)
{
	bool unlocked = false;
	while (!atomic_compare_exchange_weak(lock, &unlocked, true)) {
		unlocked = false;
		sched_yield(); /* give back the cpu */
	}
}


static void spinlock_unlock(spinlock_t *lock)
{
	atomic_store(lock, false);
}


extern int phInterrupt(unsigned int n, int (*f)(unsigned int, void *), void *arg, _Atomic(uint32_t) *condFutex, handle_t *handle);


extern int phResourceDestroy(handle_t);


struct _mutex_t;


static int _mutexLock(struct _mutex_t *mutex);


static int _mutexUnlock(struct _mutex_t *mutex);


static void _mutexInit(struct _mutex_t *mutex, int type);


enum {
	MUTEX_UNLOCKED = 0,
	MUTEX_LOCKED = 1,
	MUTEX_CONTENDED = 2,
};


#define MUTEX_NO_OWNER (-1)


typedef struct _mutex_t {
	_Atomic(uint32_t) futex;
	_Atomic(int) owner;
	int type;
	idnode_t linkage;
	bool taken;
} mutex_t;


typedef struct _cond_t {
	_Atomic(uint32_t) value;
	int clock;
	idnode_t linkage;
	bool taken;
} cond_t;


#define MUTEXES_SIZE 100
#define CONDS_SIZE   100


struct {
	spinlock_t spinlock;
	mutex_t items[MUTEXES_SIZE];
} mutexes = { 0 };


struct {
	spinlock_t spinlock;
	cond_t items[CONDS_SIZE];
} conds = { 0 };


static mutex_t *reserveMutex(void)
{
	spinlock_lock(&mutexes.spinlock);
	for (size_t i = 0; i < MUTEXES_SIZE; i++) {
		if (!mutexes.items[i].taken) {
			mutexes.items[i].taken = true;
			spinlock_unlock(&mutexes.spinlock);
			return &mutexes.items[i];
		}
	}
	spinlock_unlock(&mutexes.spinlock);
	return NULL;
}


static void freeMutex(mutex_t *m)
{
	spinlock_lock(&mutexes.spinlock);
	m->taken = false;
	spinlock_unlock(&mutexes.spinlock);
}


static cond_t *reserveCond(void)
{
	spinlock_lock(&conds.spinlock);
	for (size_t i = 0; i < CONDS_SIZE; i++) {
		if (!conds.items[i].taken) {
			conds.items[i].taken = true;
			spinlock_unlock(&conds.spinlock);
			return &conds.items[i];
		}
	}
	spinlock_unlock(&conds.spinlock);
	return NULL;
}


static void freeCond(cond_t *c)
{
	spinlock_lock(&conds.spinlock);
	c->taken = false;
	spinlock_unlock(&conds.spinlock);
}


static struct {
	idtree_t mutexes;
	idtree_t conds;
	spinlock_t lock;
} uresource_common;


void _uresource_init(void)
{
	idtree_init(&uresource_common.mutexes);
	idtree_init(&uresource_common.conds);
	atomic_store(&uresource_common.lock, false);
}


static mutex_t *_mutexGetFromTree(handle_t m)
{
	spinlock_lock(&uresource_common.lock);
	idnode_t *id = idtree_find(&uresource_common.mutexes, (int)m);
	mutex_t *mutex = lib_treeof(mutex_t, linkage, (void *)id);
	if (mutex == NULL) {
		spinlock_unlock(&uresource_common.lock);
		return NULL;
	}
	spinlock_unlock(&uresource_common.lock);
	return mutex;
}


static mutex_t *_mutexGetFromTreeUnsafe(handle_t m)
{
	idnode_t *id = idtree_find(&uresource_common.mutexes, (int)m);
	mutex_t *mutex = lib_treeof(mutex_t, linkage, (void *)id);
	if (mutex == NULL) {
		return NULL;
	}
	return mutex;
}


static cond_t *_condGetFromTree(handle_t m)
{
	spinlock_lock(&uresource_common.lock);
	idnode_t *id = idtree_find(&uresource_common.conds, (int)m);
	cond_t *cond = lib_treeof(cond_t, linkage, (void *)id);
	if (cond == NULL) {
		spinlock_unlock(&uresource_common.lock);
		return NULL;
	}
	spinlock_unlock(&uresource_common.lock);
	return cond;
}


static cond_t *_condGetFromTreeUnsafe(handle_t m)
{
	idnode_t *id = idtree_find(&uresource_common.conds, (int)m);
	cond_t *cond = lib_treeof(cond_t, linkage, (void *)id);
	if (cond == NULL) {
		return NULL;
	}
	return cond;
}


static void _mutexInit(mutex_t *mutex, int type)
{
	mutex->type = type;
	atomic_init(&mutex->owner, 0);
	atomic_init(&mutex->futex, 0);

	atomic_store(&mutex->owner, MUTEX_NO_OWNER);
	atomic_store(&mutex->futex, MUTEX_UNLOCKED);
}


static int _mutexWait(mutex_t *mutex, uint32_t desired)
{
	return phFutexWait(&mutex->futex, desired, 0, 0);
}


static int _mutexWakeup(mutex_t *mutex)
{
	return phFutexWakeup(&mutex->futex, 1);
}


static int _mutexLock(mutex_t *mutex)
{
	if (mutex->type == PH_LOCK_ERRORCHECK && atomic_load(&mutex->owner) == gettid()) {
		return -EDEADLK;
	}

	uint32_t state = MUTEX_UNLOCKED;
	if (!atomic_compare_exchange_weak(&mutex->futex, &state, MUTEX_LOCKED)) {
		if (state != MUTEX_CONTENDED) {
			state = atomic_exchange(&mutex->futex, MUTEX_CONTENDED);
		}

		while (state != MUTEX_UNLOCKED) {
			_mutexWait(mutex, MUTEX_CONTENDED);

			state = atomic_exchange(&mutex->futex, MUTEX_CONTENDED);
		}
	}

	atomic_store(&mutex->owner, gettid());

	return EOK;
}


static int _mutexTryLock(mutex_t *mutex)
{
	uint32_t state = MUTEX_UNLOCKED;
	if (atomic_compare_exchange_weak(&mutex->futex, &state, MUTEX_LOCKED)) {
		atomic_store(&mutex->owner, gettid());
		return EOK;
	}
	return -EBUSY;
}


static int _mutexUnlock(mutex_t *mutex)
{
	if (mutex->type == PH_LOCK_ERRORCHECK && atomic_load(&mutex->owner) != gettid()) {
		return -EPERM;
	}

	atomic_store(&mutex->owner, MUTEX_NO_OWNER);
	if (atomic_fetch_sub(&mutex->futex, 1) != MUTEX_LOCKED) {
		atomic_store(&mutex->futex, MUTEX_UNLOCKED);
		_mutexWakeup(mutex);
	}
	return EOK;
}


int mutexCreate(handle_t *h)
{
	static const struct lockAttr attr = { .type = PH_LOCK_NORMAL };
	return mutexCreateWithAttr(h, &attr);
}


/* TODO: Recursive mutexes are not handled currently */
int mutexCreateWithAttr(handle_t *h, const struct lockAttr *attr)
{
	if ((attr->type != PH_LOCK_NORMAL) && (attr->type != PH_LOCK_RECURSIVE) && (attr->type != PH_LOCK_ERRORCHECK)) {
		return -EINVAL;
	}

	mutex_t *mutex = reserveMutex();
	if (mutex == NULL) {
		return -ENOMEM;
	}

	_mutexInit(mutex, attr->type);

	spinlock_lock(&uresource_common.lock);
	/* futex-based mutexes will have ids higher than INT_MAX/2 */
	int id = idtree_alloc(&uresource_common.mutexes, &mutex->linkage);
	if (id < 0) {
		spinlock_unlock(&uresource_common.lock);
		freeMutex(mutex);
		return -ENOMEM;
	}
	*((int *)h) = (INT_MAX / 2) + id;
	spinlock_unlock(&uresource_common.lock);
	return EOK;
}


int mutexLock(handle_t m)
{
	m -= INT_MAX / 2;

	mutex_t *mutex = _mutexGetFromTree(m);
	if (mutex == NULL) {
		return -EINVAL;
	}

	return _mutexLock(mutex);
}


int mutexUnlock(handle_t m)
{
	m -= INT_MAX / 2;

	mutex_t *mutex = _mutexGetFromTree(m);
	if (mutex == NULL) {
		return -EINVAL;
	}

	return _mutexUnlock(mutex);
}


int mutexTry(handle_t m)
{
	m -= INT_MAX / 2;

	mutex_t *mutex = _mutexGetFromTree(m);
	if (mutex == NULL) {
		return -EINVAL;
	}

	return _mutexTryLock(mutex);
}


int mutexLock2(handle_t m1, handle_t m2)
{
#if 0
	int err;
	int tmp;

	if ((err = mutexLock(m1)) < 0) {
		return err;
    }

	while (mutexTry(m2) < 0) {
		mutexUnlock(m1);
		if ((err = mutexLock(m2)) < 0) {
			return err;
        }

		tmp = m1;
		m1 = m2;
		m2 = tmp;
	}

	return EOK;
#else
	int err;

	if (m1 > m2) {
		handle_t tmp = m1;
		m1 = m2;
		m2 = tmp;
	}
	else if (m1 == m2) {
		return mutexLock(m1);
	}

	if ((err = mutexLock(m1)) != EOK) {
		return err;
	}

	if ((err = mutexLock(m2)) != EOK) {
		mutexUnlock(m1);
		return err;
	}
	return EOK;
#endif
}


static void _condInit(cond_t *cond, int clock)
{
	cond->clock = clock;
	atomic_init(&cond->value, 0);
	atomic_store(&cond->value, 0);
}


int condCreateWithAttr(handle_t *h, const struct condAttr *attr)
{
	if (attr != NULL) {
		if (attr->clock != PH_CLOCK_REALTIME && attr->clock != PH_CLOCK_RELATIVE && attr->clock != PH_CLOCK_MONOTONIC) {
			return -EINVAL;
		}
	}

	cond_t *cond = reserveCond();
	if (cond == NULL) {
		return -ENOMEM;
	}
	_condInit(cond, attr != NULL ? attr->clock : 0);

	spinlock_lock(&uresource_common.lock);
	/* futex-based conds will have ids higher than INT_MAX/2 */
	int id = idtree_alloc(&uresource_common.conds, &cond->linkage);
	if (id < 0) {
		spinlock_unlock(&uresource_common.lock);
		freeCond(cond);
		return -ENOMEM;
	}
	*((int *)h) = (INT_MAX / 2) + id;
	spinlock_unlock(&uresource_common.lock);
	return EOK;
}


int condCreate(handle_t *h)
{
	static const struct condAttr defaultAttr = { .clock = PH_CLOCK_RELATIVE };
	return condCreateWithAttr(h, &defaultAttr);
}


int condWait(handle_t h, handle_t m, time_t timeout)
{
	h -= INT_MAX / 2;
	m -= INT_MAX / 2;

	cond_t *cond = _condGetFromTree(h);
	if (cond == NULL) {
		return -EINVAL;
	}

	mutex_t *mutex = _mutexGetFromTree(m);
	if (mutex == NULL) {
		return -EINVAL;
	}

	for (;;) {
		uint32_t value = atomic_load(&cond->value);

		if (value > 0) {
			if (atomic_compare_exchange_weak(&cond->value, &value, value - 1)) {
				return EOK;
			}

			continue;
		}

		_mutexUnlock(mutex);

		int ret = phFutexWait(&cond->value, 0, timeout, cond->clock);

		_mutexLock(mutex);

		if (ret == -ETIME) {
			if (atomic_load(&cond->value) == 0) {
				return -ETIME;
			}
		}
		else if (ret == -EINTR) {
			return -EINTR;
		}
	}

	return EOK;
}


#define COND_BROADCAST (UINT32_MAX / 2)


int condSignal(handle_t h)
{
	h -= INT_MAX / 2;

	cond_t *cond = _condGetFromTree(h);
	if (cond == NULL) {
		return -EINVAL;
	}

	uint32_t value = atomic_load(&cond->value);
	while (value < COND_BROADCAST) {
		if (atomic_compare_exchange_weak(&cond->value, &value, value + 1)) {
			break;
		}
	}

	phFutexWakeup(&cond->value, 1);

	return EOK;
}


int condBroadcast(handle_t h)
{
	h -= INT_MAX / 2;

	cond_t *cond = _condGetFromTree(h);
	if (cond == NULL) {
		return -EINVAL;
	}

	atomic_store(&cond->value, COND_BROADCAST);
	phFutexWakeup(&cond->value, FUTEX_WAKEUP_ALL);

	return EOK;
}


int resourceDestroy(handle_t r)
{
	if (r >= INT_MAX / 2) {
		r -= INT_MAX / 2;

		spinlock_lock(&uresource_common.lock);

		mutex_t *mutex = _mutexGetFromTreeUnsafe(r);
		if (mutex != NULL) {
			idtree_remove(&uresource_common.mutexes, &mutex->linkage);
			spinlock_unlock(&uresource_common.lock);
			freeMutex(mutex);
			return EOK;
		}

		cond_t *cond = _condGetFromTreeUnsafe(r);
		if (cond != NULL) {
			idtree_remove(&uresource_common.conds, &cond->linkage);
			spinlock_unlock(&uresource_common.lock);
			freeCond(cond);
			return EOK;
		}

		spinlock_unlock(&uresource_common.lock);

		return -EINVAL;
	}
	else {
		return phResourceDestroy(r);
	}
}


/* TODO: this code probably should be in sys/interrupt.c, but this is ok for now */
int interrupt(unsigned int n, int (*f)(unsigned int, void *), void *arg, handle_t cond_handle, handle_t *handle)
{
	cond_t *cond = NULL;

	/* allow the caller to pass 0 into cond_handle */
	if (cond_handle != 0) {
		cond_handle -= INT_MAX / 2;

		cond = _condGetFromTree(cond_handle);
		if (cond == NULL) {
			return -EINVAL;
		}
	}


	_Atomic(uint32_t) *condFutex = cond != NULL ? &cond->value : NULL;
	return phInterrupt(n, f, arg, condFutex, handle);
}

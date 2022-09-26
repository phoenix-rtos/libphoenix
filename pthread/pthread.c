/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * pthread
 *
 * Copyright 2017, 2019 Phoenix Systems
 * Author: Pawel Pisarczyk, Marcin Baran
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/list.h>
#include <sys/mman.h>
#include <sys/minmax.h>
#include <pthread.h>
#include <unistd.h>


#define CEIL(value, size)	((((value) + (size) - 1) / (size)) * (size))


typedef struct pthread_ctx {
	handle_t id;
	void *(*start_routine)(void *);
	void *arg;
	void *retval;
	void *stack;
	size_t stacksize;
	struct pthread_ctx *next;
	struct pthread_ctx *prev;
	int detached;
	int cancelstate;
	struct __errno_t e;
	int refcount;
	struct pthread_key_data_t *key_data_list;
} pthread_ctx;


typedef struct pthread_fork_handlers_t {
	void (*prepare)(void);
	void (*parent)(void);
	void (*child)(void);
	struct pthread_fork_handlers_t *prev;
	struct pthread_fork_handlers_t *next;
} pthread_fork_handlers_t;


static struct {
	handle_t pthread_key_lock;
	handle_t pthread_list_lock;
	handle_t pthread_atfork_lock;
	handle_t mutex_cond_init_lock;
	pthread_ctx *pthread_list;
	pthread_fork_handlers_t *pthread_fork_handlers;
} pthread_common;


typedef struct __pthread_key_t {
	void (*destructor)(void *);
} __pthread_key_t;


typedef struct pthread_key_data_t {
	__pthread_key_t *key;
	void *value;
	struct pthread_key_data_t *next;
} pthread_key_data_t;


typedef struct _pthread_key_cleanup_t {
	void (*destructor)(void *);
	void *value;
	struct _pthread_key_cleanup_t *next;
} _pthread_key_cleanup_t;


static const pthread_attr_t pthread_attr_default = {
	.stackaddr = NULL,
	.policy = SCHED_RR,
	.priority = 4,
	.detached = PTHREAD_CREATE_JOINABLE,
	.stacksize = CEIL(PTHREAD_STACK_MIN, PAGE_SIZE)
};


static void _pthread_ctx_get(pthread_ctx *ctx)
{
	++ctx->refcount;
}


static void pthread_ctx_get(pthread_ctx *ctx)
{
	mutexLock(pthread_common.pthread_list_lock);
	_pthread_ctx_get(ctx);
	mutexUnlock(pthread_common.pthread_list_lock);
}


static void _pthread_ctx_put(pthread_ctx *ctx)
{
	int refcnt = --ctx->refcount;
	mutexUnlock(pthread_common.pthread_list_lock);

	if (refcnt == 0) {
		free(ctx);
	}
}


static void pthread_ctx_put(pthread_ctx *ctx)
{
	mutexLock(pthread_common.pthread_list_lock);
	_pthread_ctx_put(ctx);
}


static void start_point(void *args)
{
	pthread_ctx *ctx = (pthread_ctx *)args;

	_errno_new(&ctx->e);

	ctx->retval = (void *)(ctx->start_routine(ctx->arg));

	endthread();
}


static _pthread_key_cleanup_t *pthread_key_cleanup(pthread_ctx *ctx)
{
	_pthread_key_cleanup_t *head = NULL, *next;
	pthread_key_data_t *thread_data;

	mutexLock(pthread_common.pthread_key_lock);

	while (ctx->key_data_list != NULL) {
		thread_data = ctx->key_data_list;
		ctx->key_data_list = ctx->key_data_list->next;

		next = malloc(sizeof(_pthread_key_cleanup_t));
		if (next == NULL) {
			mutexUnlock(pthread_common.pthread_key_lock);
			return head;
		}
		next->destructor = thread_data->key->destructor;
		next->value = thread_data->value;
		next->next = head;
		head = next;

		free(thread_data);
	}
	mutexUnlock(pthread_common.pthread_key_lock);

	return head;
}


static void pthread_full_key_cleanup(_pthread_key_cleanup_t *head)
{
	_pthread_key_cleanup_t *next;

	while (head != NULL) {
		next = head->next;
		if (head->destructor != NULL) {
			head->destructor(head->value);
		}
		free(head);
		head = next;
	}
}


static pthread_ctx *find_pthread(handle_t id)
{
	mutexLock(pthread_common.pthread_list_lock);
	pthread_ctx *ctx = pthread_common.pthread_list;

	if (ctx != NULL) {
		do {
			if (ctx->id == id) {
				_pthread_ctx_get(ctx);
				mutexUnlock(pthread_common.pthread_list_lock);
				return ctx;
			}
			ctx = ctx->next;
		} while (ctx != pthread_common.pthread_list);
	}
	mutexUnlock(pthread_common.pthread_list_lock);

	return NULL;
}


int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	void *(*start_routine)(void *), void *arg)
{
	const pthread_attr_t *attrs = &pthread_attr_default;

	if (attr != NULL)
		attrs = attr;

	void *stack = mmap(attrs->stackaddr, attrs->stacksize,
		PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, NULL, 0);

	if (stack == MAP_FAILED || stack == NULL)
		return -EAGAIN;

	pthread_ctx *ctx = (pthread_ctx *)malloc(sizeof(pthread_ctx));

	if (ctx == NULL) {
		munmap(stack, attrs->stacksize);
		return -EAGAIN;
	}

	ctx->refcount = 1;
	ctx->retval = NULL;
	ctx->detached = attrs->detached;
	ctx->start_routine = start_routine;
	ctx->arg = arg;
	ctx->stack = stack;
	ctx->stacksize = attrs->stacksize;
	ctx->key_data_list = NULL;
	ctx->cancelstate = PTHREAD_CANCEL_ENABLE;
	*thread = (pthread_t)ctx;

	int err = beginthreadex(start_point, attrs->priority, stack,
		attrs->stacksize, (void *)ctx, &ctx->id);

	if (err != EOK) {
		munmap(stack, attrs->stacksize);
		pthread_ctx_put(ctx);
		thread = NULL;
	}
	else if (ctx->detached == PTHREAD_CREATE_JOINABLE) {
		mutexLock(pthread_common.pthread_list_lock);
		if (pthread_common.pthread_list != NULL && pthread_common.pthread_list->id == 0)
			pthread_common.pthread_list = NULL;

		LIST_ADD(&pthread_common.pthread_list, ctx);
		mutexUnlock(pthread_common.pthread_list_lock);
	}

	return err;
}


int pthread_join(pthread_t thread, void **value_ptr)
{
	if (thread == pthread_self())
		return -EDEADLK;

	pthread_ctx *ctx = (pthread_ctx *)thread;

	if (ctx == NULL) {
		return -EINVAL;
	}

	mutexLock(pthread_common.pthread_list_lock);

	if (ctx->detached != PTHREAD_CREATE_DETACHED) {
		int err;
		do {
			mutexUnlock(pthread_common.pthread_list_lock);
			err = threadJoin(0);
			mutexLock(pthread_common.pthread_list_lock);
			if (err < 0) {
				mutexUnlock(pthread_common.pthread_list_lock);
				return err;
			}
			else if (ctx->id != err) {
				mutexUnlock(pthread_common.pthread_list_lock);
				pthread_ctx *ghost = find_pthread(err);
				if (ghost != NULL) {
					ghost->detached = PTHREAD_CREATE_DETACHED;
					pthread_ctx_put(ghost);
				}
				mutexLock(pthread_common.pthread_list_lock);
			}
		} while (ctx->id != err);
	}

	if (value_ptr != NULL)
		*value_ptr = ctx->retval;

	_errno_remove(&ctx->e);

	LIST_REMOVE(&pthread_common.pthread_list, ctx);
	mutexUnlock(pthread_common.pthread_list_lock);

	munmap(ctx->stack, ctx->stacksize);
	pthread_ctx_put(ctx);

	return EOK;
}


int pthread_detach(pthread_t thread)
{
	pthread_ctx *ctx = (pthread_ctx *)thread;
	mutexLock(pthread_common.pthread_list_lock);

	if (ctx == NULL) {
		mutexUnlock(pthread_common.pthread_list_lock);
		return -ESRCH;
	}

	if (ctx->detached != PTHREAD_CREATE_JOINABLE) {
		mutexUnlock(pthread_common.pthread_list_lock);
		return -EINVAL;
	}

	LIST_REMOVE(&pthread_common.pthread_list, ctx);

	_pthread_ctx_put(ctx);

	return EOK;
}


int pthread_setcancelstate(int state, int *oldstate)
{
	int err = EOK;
	pthread_ctx *ctx = (pthread_ctx *)pthread_self();

	if (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE) {
		err = -EINVAL;
	}
	else {
		mutexLock(pthread_common.pthread_list_lock);
		_pthread_ctx_get(ctx);
		if (oldstate != NULL) {
			*oldstate = ctx->cancelstate;
		}
		ctx->cancelstate = state;
		_pthread_ctx_put(ctx);
	}
	return err;
}


int pthread_cancel(pthread_t thread)
{
	int err = EOK, id;
	pthread_ctx *ctx = (pthread_ctx *)thread;
	_pthread_key_cleanup_t *cleanup;

	if (ctx == NULL) {
		err = -ESRCH;
	}
	else {
		mutexLock(pthread_common.pthread_list_lock);
		_pthread_ctx_get(ctx);
		if (thread == pthread_self()) {
			if (ctx->cancelstate == PTHREAD_CANCEL_ENABLE) {
				_pthread_ctx_put(ctx);
				pthread_exit((void *)PTHREAD_CANCELED);
				/* no return */
			}
			_pthread_ctx_put(ctx);
		}
		else {
			if (ctx->cancelstate == PTHREAD_CANCEL_ENABLE) {
				cleanup = pthread_key_cleanup(ctx);
				ctx->retval = (void *)PTHREAD_CANCELED;
				id = ctx->id;
				_pthread_ctx_put(ctx);
				pthread_full_key_cleanup(cleanup);
				err = signalPost(getpid(), id, signal_cancel);
			}
			else {
				_pthread_ctx_put(ctx);
			}
		}
	}
	return err;
}


pthread_t pthread_self(void)
{
	pthread_ctx *ctx = find_pthread(gettid());
	if (ctx != NULL) {
		pthread_ctx_put(ctx);
	}
	return (pthread_t)ctx;
}


int pthread_equal(pthread_t t1, pthread_t t2)
{
	return t1 == t2;
}


void pthread_exit(void *value_ptr)
{
	pthread_ctx *ctx = (pthread_ctx *)pthread_self();
	_pthread_key_cleanup_t *cleanup;

	if (ctx != NULL) {
		mutexLock(pthread_common.pthread_list_lock);
		ctx->retval = value_ptr;
		cleanup = pthread_key_cleanup(ctx);
		_pthread_ctx_put(ctx);
		pthread_full_key_cleanup(cleanup);
	}

	endthread();
}


int pthread_attr_init(pthread_attr_t *attr)
{
	*attr = pthread_attr_default;

	return EOK;
}


int pthread_attr_destroy(pthread_attr_t *attr)
{
	return EOK;
}


int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr)
{
	if (attr == NULL)
		return -EINVAL;

	attr->stackaddr = stackaddr;

	return EOK;
}


int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr)
{
	if (attr == NULL)
		return -EINVAL;

	*stackaddr = attr->stackaddr;

	return EOK;
}


int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
	if (attr == NULL || stacksize < PTHREAD_STACK_MIN)
		return -EINVAL;

	attr->stacksize = CEIL(stacksize, PAGE_SIZE);

	return EOK;
}


int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
	if (attr == NULL)
		return -EINVAL;

	*stacksize = attr->stacksize;

	return EOK;
}


int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
	size_t stacksize)
{
	return pthread_attr_setstackaddr(attr, stackaddr) |
		pthread_attr_setstacksize(attr, stacksize);
}


int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr,
	size_t *stacksize)
{
	return pthread_attr_getstackaddr(attr, stackaddr) |
		pthread_attr_getstacksize(attr, stacksize);
}


int pthread_attr_setschedparam(pthread_attr_t *attr,
	const struct sched_param *param)
{
	if (attr == NULL)
		return -EINVAL;

	if (param->sched_priority > sched_get_priority_max(SCHED_RR) ||
			param->sched_priority < sched_get_priority_min(SCHED_RR))
		return -ENOTSUP;

	attr->priority = param->sched_priority;

	return EOK;
}


int pthread_attr_getschedparam(const pthread_attr_t *attr,
	struct sched_param *param)
{
	if (attr == NULL)
		return -EINVAL;

	param->sched_priority = attr->priority;

	return EOK;
}


int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
	if (policy < SCHED_FIFO || policy > SCHED_OTHER)
		return -EINVAL;
	else if (policy == SCHED_FIFO || policy == SCHED_OTHER)
		return -ENOTSUP;

	attr->policy = policy;

	return EOK;
}


int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy)
{
	if (attr == NULL)
		return -EINVAL;

	*policy = attr->policy;

	return EOK;
}


int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope)
{
	return EOK;
}

int pthread_attr_getscope(const pthread_attr_t *attr,
	int *contentionscope)
{
	if (attr == NULL)
		return -EINVAL;

	return EOK;
}


int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
	if (detachstate != PTHREAD_CREATE_DETACHED ||
			detachstate != PTHREAD_CREATE_JOINABLE)
		return -EINVAL;

	attr->detached = detachstate;

	return EOK;
}


int pthread_attr_getdetachstate(const pthread_attr_t *attr,
	int *detachstate)
{
	if (attr == NULL)
		return -EINVAL;

	*detachstate = attr->detached;

	return EOK;
}


int pthread_attr_getinheritsched(const pthread_attr_t *restrict attr,
	int *restrict inheritsched);


int pthread_attr_setinheritsched(pthread_attr_t *attr,
	int inheritsched);


int pthread_setschedprio(pthread_t thread, int prio);


int pthread_getschedparam(pthread_t thread, int *policy,
	struct sched_param *restrict param);


int pthread_setschedparam(pthread_t thread, int policy,
	const struct sched_param *param);


static int pthread_mutex_lazy_init(pthread_mutex_t *mutex)
{
	int err = EOK;
	if (mutex->initialized == 0) {
		mutexLock(pthread_common.mutex_cond_init_lock);
		if (mutex->initialized == 0) {
			err = mutexCreate(&mutex->mutexh);
			if (err == EOK) {
				mutex->initialized = 1;
			}
		}
		mutexUnlock(pthread_common.mutex_cond_init_lock);
	}

	return err;
}


int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
	int err;

	(void)attr;

	err = mutexCreate(&mutex->mutexh);
	if (err == EOK) {
		mutex->initialized = 1;
	}
	return err;
}


int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	int err = pthread_mutex_lazy_init(mutex);

	if (err == EOK) {
		err = mutexLock(mutex->mutexh);
	}

	return err;
}


int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	int err = pthread_mutex_lazy_init(mutex);

	if (err == EOK) {
		err = mutexTry(mutex->mutexh);
	}

	return err;
}


int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	int err = pthread_mutex_lazy_init(mutex);

	if (err == EOK) {
		err = mutexUnlock(mutex->mutexh);
	}

	return err;
}


int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	int err = EOK;

	mutexLock(pthread_common.mutex_cond_init_lock);
	if (mutex->initialized == 1) {
		mutexUnlock(pthread_common.mutex_cond_init_lock);
		err = resourceDestroy(mutex->mutexh);
	}
	else {
		mutexUnlock(pthread_common.mutex_cond_init_lock);
	}

	return err;
}


int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
	return EOK;
}


int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
	attr->type = PTHREAD_MUTEX_DEFAULT;
	return EOK;
}


int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)
{
	*type = attr->type;
	return EOK;
}


int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
	int err = -EINVAL;

	if (type >= PTHREAD_MUTEX_DEFAULT && type <= PTHREAD_MUTEX_RECURSIVE) {
		attr->type = type;
		err = EOK;
	}
	return err;
}


int sched_yield(void)
{
	return usleep(0);
}


int sched_get_priority_max(int policy)
{
	if (policy == SCHED_RR)
		return 7;

	return -EINVAL;
}


int sched_get_priority_min(int policy)
{
	if (policy == SCHED_RR)
		return 0;

	return -EINVAL;
}


int pthread_condattr_init(pthread_condattr_t *attr)
{
	attr->pshared = PTHREAD_PROCESS_PRIVATE;
	attr->clock_id = CLOCK_MONOTONIC;
	return EOK;
}


int pthread_condattr_destroy(pthread_condattr_t *attr)
{
	return EOK;
}


int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared)
{
	int err = EOK;

	/* Only 'PTHREAD_PROCESS_PRIVATE' supported */
	if (pshared != PTHREAD_PROCESS_PRIVATE) {
		err = -EINVAL;
	}
	else {
		attr->pshared = pshared;
	}

	return err;
}


int pthread_condattr_getpshared(const pthread_condattr_t *restrict attr, int *restrict pshared)
{
	*pshared = attr->pshared;
	return EOK;
}


int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id)
{
	int err = EOK;

	/* Only 'CLOCK_MONOTONIC' supported */
	if (clock_id != CLOCK_MONOTONIC) {
		err = -EINVAL;
	}
	else {
		attr->clock_id = clock_id;
	}

	return err;
}


int pthread_condattr_getclock(const pthread_condattr_t *restrict attr, clockid_t *restrict clock_id)
{
	*clock_id = attr->clock_id;
	return EOK;
}


static int pthread_cond_lazy_init(pthread_cond_t *cond)
{
	int err = EOK;

	if (cond->initialized == 0) {
		mutexLock(pthread_common.mutex_cond_init_lock);
		if (cond->initialized == 0) {
			err = condCreate(&cond->condh);
			if (err == EOK) {
				cond->initialized = 1;
			}
		}
		mutexUnlock(pthread_common.mutex_cond_init_lock);
	}
	return err;
}

int pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr)
{
	int err;

	err = condCreate(&cond->condh);
	if (err == EOK) {
		cond->initialized = 1;
	}
	return err;
}


int pthread_cond_destroy(pthread_cond_t *cond)
{
	int err = EOK;
	mutexLock(pthread_common.mutex_cond_init_lock);
	if (cond->initialized == 1) {
		mutexUnlock(pthread_common.mutex_cond_init_lock);
		err = resourceDestroy(cond->condh);
	}
	else {
		mutexUnlock(pthread_common.mutex_cond_init_lock);
	}
	return err;
}


int pthread_cond_signal(pthread_cond_t *cond)
{
	int err = pthread_cond_lazy_init(cond);

	if (err == EOK) {
		err = condSignal(cond->condh);
	}
	return err;
}


int pthread_cond_broadcast(pthread_cond_t *cond)
{
	int err = pthread_cond_lazy_init(cond);

	if (err == EOK) {
		err = condBroadcast(cond->condh);
	}
	return err;
}


int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex)
{
	int err = pthread_cond_lazy_init(cond);

	if (err == EOK) {
		err = pthread_mutex_lazy_init(mutex);
	}

	if (err == EOK) {
		err = condWait(cond->condh, mutex->mutexh, 0);
	}
	return err;
}


static time_t timespec_to_us(const struct timespec *restrict time)
{
	return (time->tv_sec * 1000000 + time->tv_nsec / 1000);
}


int pthread_cond_timedwait(pthread_cond_t *restrict cond,
	pthread_mutex_t *restrict mutex,
	const struct timespec *restrict abstime)
{
	int err = EOK;
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	if ((now.tv_sec > abstime->tv_sec) || ((now.tv_sec == abstime->tv_sec) && (now.tv_nsec >= abstime->tv_nsec))) {
		err = -ETIMEDOUT;
	}
	else {
		time_t now_us = timespec_to_us(&now);
		const time_t abstime_us = timespec_to_us(abstime);
		time_t timeout_us = abstime_us - now_us;

		err = pthread_cond_lazy_init(cond);

		if (err == EOK) {
			err = pthread_mutex_lazy_init(mutex);
		}

		if (err == EOK) {
			err = condWait(cond->condh, mutex->mutexh, timeout_us);
		}
	}
	if (err == -ETIME) {
		err = -ETIMEDOUT;
	}
	return err;
}


int pthread_sigmask(int how, const sigset_t *__restrict__ set, sigset_t *__restrict__ oset)
{
	int ret = sigprocmask(how, set, oset);
	return (ret == -1) ? errno : EOK;
}


int pthread_kill(pthread_t thread, int sig)
{
	pthread_ctx *ctx = (pthread_ctx *)thread;
	int ret = signalPostPosix(getpid(), ctx->id, sig);
	return ret;
}


int pthread_key_create(pthread_key_t *key, void (*destructor)(void *))
{
	int err = EOK;
	*key = malloc(sizeof(__pthread_key_t));
	if (key == NULL) {
		err = -ENOMEM;
	}
	else {
		(*key)->destructor = destructor;
	}
	return err;
}


int pthread_key_delete(pthread_key_t key)
{
	mutexLock(pthread_common.pthread_list_lock);

	pthread_ctx *first = pthread_common.pthread_list, *curr = pthread_common.pthread_list;

	if (first != NULL) {
		mutexLock(pthread_common.pthread_key_lock);
		do {
			pthread_key_data_t *head = curr->key_data_list;
			pthread_key_data_t *prev = NULL;
			while (head != NULL) {
				if (head->key == key) {
					if (prev == NULL) {
						curr->key_data_list = head->next;
					}
					else {
						prev->next = head->next;
					}
					free(head);
					break;
				}
				prev = head;
				head = head->next;
			}
			curr = curr->next;
		} while (curr != first);
		mutexUnlock(pthread_common.pthread_key_lock);
	}
	mutexUnlock(pthread_common.pthread_list_lock);
	free(key);
	return EOK;
}


int pthread_setspecific(pthread_key_t key, const void *value)
{
	int err = EOK;
	pthread_ctx *ctx = (pthread_ctx *)pthread_self();
	pthread_ctx_get(ctx);
	mutexLock(pthread_common.pthread_key_lock);
	pthread_key_data_t *head = ctx->key_data_list;

	while (head != NULL) {
		if (head->key == key) {
			head->value = (void *)value;
			break;
		}
		head = head->next;
	}

	if (head == NULL) {
		head = (pthread_key_data_t *)malloc(sizeof(pthread_key_data_t));
		if (head == NULL) {
			err = -ENOMEM;
		}
		else {
			head->key = key;
			head->value = (void *)value;
			head->next = ctx->key_data_list;
			ctx->key_data_list = head;
		}
	}
	mutexUnlock(pthread_common.pthread_key_lock);

	pthread_ctx_put(ctx);
	return err;
}


void *pthread_getspecific(pthread_key_t key)
{
	void *value = NULL;
	pthread_ctx *ctx = (pthread_ctx *)pthread_self();
	pthread_ctx_get(ctx);
	mutexLock(pthread_common.pthread_key_lock);
	pthread_key_data_t *head = ctx->key_data_list;
	while (head != NULL) {
		if (head->key == key) {
			value = head->value;
			break;
		}
		head = head->next;
	}
	mutexUnlock(pthread_common.pthread_key_lock);

	pthread_ctx_put(ctx);
	return value;
}


int pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
	mutexLock(pthread_common.pthread_key_lock);

	if (*once_control == PTHREAD_ONCE_INIT) {
		init_routine();
		*once_control = 0;
	}

	mutexUnlock(pthread_common.pthread_key_lock);
	return EOK;
}


int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void))
{
	int err = EOK;
	pthread_fork_handlers_t *fork_handlers = (pthread_fork_handlers_t *)malloc(sizeof(pthread_fork_handlers_t));
	if (fork_handlers == NULL) {
		err = -ENOMEM;
	}
	else {
		fork_handlers->prepare = prepare;
		fork_handlers->parent = parent;
		fork_handlers->child = child;
		mutexLock(pthread_common.pthread_atfork_lock);
		LIST_ADD(&pthread_common.pthread_fork_handlers, fork_handlers);
		mutexUnlock(pthread_common.pthread_atfork_lock);
	}
	return err;
}


void _pthread_atfork_prepare(void)
{
	/* prepare functions must be called in reverse order */
	mutexLock(pthread_common.pthread_atfork_lock);
	pthread_fork_handlers_t *last = pthread_common.pthread_fork_handlers, *curr;

	if (last != NULL) {
		last = last->prev;
		curr = last;
		do {
			if (curr->prepare != NULL) {
				curr->prepare();
			}
			curr = curr->prev;
		} while (curr != last);
	}
	mutexUnlock(pthread_common.pthread_atfork_lock);
}


/* parent and child functions must be called in order */
void _pthread_atfork_parent(void)
{
	mutexLock(pthread_common.pthread_atfork_lock);
	pthread_fork_handlers_t *first = pthread_common.pthread_fork_handlers,
							*curr = pthread_common.pthread_fork_handlers;

	if (first != NULL) {
		do {
			if (curr->parent != NULL) {
				curr->parent();
			}
			curr = curr->next;
		} while (curr != first);
	}
	mutexUnlock(pthread_common.pthread_atfork_lock);
}


void _pthread_atfork_child(void)
{
	mutexLock(pthread_common.pthread_atfork_lock);
	pthread_fork_handlers_t *first = pthread_common.pthread_fork_handlers,
		*curr = pthread_common.pthread_fork_handlers;

	if (first != NULL) {
		do {
			if (curr->child != NULL) {
				curr->child();
			}
			curr = curr->next;
		} while (curr != first);
	}
	mutexUnlock(pthread_common.pthread_atfork_lock);
}


void _pthread_init(void)
{
	mutexCreate(&pthread_common.pthread_key_lock);
	mutexCreate(&pthread_common.pthread_list_lock);
	mutexCreate(&pthread_common.pthread_atfork_lock);
	mutexCreate(&pthread_common.mutex_cond_init_lock);
	pthread_common.pthread_list = NULL;
	pthread_common.pthread_fork_handlers = NULL;
}

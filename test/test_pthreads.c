/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_pthreads
 *
 * Copyright 2019 Phoenix Systems
 * Author: Marcin Baran
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "pthread.h"

#define TEST_CREATE_JOIN_NUM 100
#define TEST_MUTEX 5
#define TEST_EQUAL 2


static void *worker(void *arg)
{
	if (arg != NULL)
		printf("Worker thread %d created\n", *(int *)arg);

	return arg;
}

static void *worker_busy(void *arg)
{
	printf("Busy worker thread %d created\n", (int)arg);

	usleep(100);

	return arg;
}


/*
 * Pthread test create/join simple thread:
 */

static int test_pthread_create_join(int num_threads)
{
	pthread_t thread;
	void *result = NULL;
	int passed = 0;
	int failed = 0;
	int err = 0;
	int arg = 0;
	int i;

	for (i = 0; i < num_threads; i++) {
		arg = i;
		printf("Creating thread %d\n", i);
		if ((err = pthread_create(&thread, NULL, worker, (void *)&arg)) != EOK) {
			printf("Failed to create thread %d. Error: %d\n", i, err);
			++failed;
			continue;
		}

		printf("Joining thread %d\n", i);
		if ((err = pthread_join(thread, &result)) != EOK) {
			printf("Failed to join thread %d. Error: %d\n", i, err);
			++failed;
		}
		else if (result != (void *)&arg) {
			printf("Wrong return value from thread %d\n", i);
			++failed;
		}
		else {
			++passed;
		}
	}

	printf("\n%s tests passed: %d, failed: %d\n\n", __func__, passed, failed);

	return passed;
};

/*
 * Pthread test threads equal
 */

static int test_pthread_equal(void)
{
	pthread_t t1, t2;
	int passed = 0;

	if (pthread_create(&t1, NULL, worker, NULL) == EOK &&
			pthread_create(&t2, NULL, worker, NULL) == EOK) {

		printf("Created 2 threads\n");

		passed += pthread_equal(t1, t1);
		passed += !pthread_equal(t1, t2);

		pthread_join(t1, NULL);
		pthread_join(t2, NULL);

	}
	else {
		printf("Failed to create 2 threads\n");
	}

	printf("\n%s tests passed: %d, failed: %d\n\n", __func__,
		passed, 2 - passed);

	return passed;
};

/*
 * Pthread test lock/trylock/unlock mutex:
 */

static int test_pthread_lock_unlock_mutex(void)
{
	pthread_mutex_t mutex;
	int passed = 0;
	int err = 0;

	printf("Creating mutex\n");

	if ((err = pthread_mutex_init(&mutex, NULL)) != EOK) {
		printf("Could not create mutex. Error: %d\n", err);
		goto DESTROY;
	}

	++passed;

	printf("Locking mutex\n");

	if ((err = pthread_mutex_lock(&mutex)) != EOK) {
		printf("Could not lock mutex. Error: %d\n", err);
		goto DESTROY;
	}

	++passed;

	printf("Trylock on mutex (should fail now)\n");

	if ((err = pthread_mutex_trylock(&mutex)) == EOK)
		printf("Trylock succeeded even though mutex locked\n");
	else
		++passed;

	printf("Unlocking mutex\n");

	if ((err = pthread_mutex_unlock(&mutex)) != EOK)
		printf("Could not unlock mutex. Error: %d\n", err);
	else
		++passed;

DESTROY:

	printf("Destroying mutex\n");

	if ((err = pthread_mutex_destroy(&mutex)) != EOK)
		printf("Could not destroy mutex. Error: %d\n", err);
	else
		++passed;

	printf("\n%s tests passed: %d, failed: %d\n\n", __func__,
			passed, 5 - passed);

	return passed;
};

/*
 * Pthread test create/join multiple threads:
 */

static int test_pthread_create_join_multiple(int num_threads)
{
	pthread_attr_t attr;
	struct sched_param param = {.sched_priority = 0};
	pthread_t *threads = malloc(num_threads * sizeof(pthread_t));

	int passed = 0;
	int failed = 0;
	void *result = NULL;
	int err = 0;
	int arg = 0;
	int i;

	pthread_attr_init(&attr);

	for (i = 0; i < num_threads; i++) {
		param.sched_priority = i % sched_get_priority_max(SCHED_RR);
		printf("Creating thread %d with priority %d\n",
			i, param.sched_priority);

		pthread_attr_setschedparam(&attr, &param);
		arg = i;

		if ((err = pthread_create(&threads[i], &attr, worker_busy,(void *)arg))
				!= EOK) {
			printf("Failed to create thread %d. Error: %d\n", i, err);
			++failed;
		}
	}

	for (i = 0; i < num_threads; ++i) {
		printf("Joining thread %d\n", i);
		arg = i;

		if ((err = pthread_join(threads[i], &result)) != EOK) {
			printf("Failed to join thread %d. Error: %d\n", i, err);
			++failed;
		}
		else if ((int)result != arg) {
			printf("Wrong return value from thread %d\n", i);
			++failed;
		}
		else {
			++passed;
		}
	}

	free(threads);

	printf("\n%s tests passed: %d, failed: %d\n\n", __func__, passed, failed);

	return passed;
}


/*
 * Pthread test create/detach threads:
 */

static int test_pthread_create_detach(int num_threads)
{
	pthread_t thread;
	int passed = 0;
	int failed = 0;
	int err = 0;
	int i;

	for (i = 0; i < num_threads; i++) {
		printf("Creating thread %d\n", i);
		if ((err = pthread_create(&thread, NULL, worker_busy, NULL)) != EOK) {
			printf("Failed to create thread %d. Error: %d\n", i, err);
			++failed;
			continue;
		}

		printf("Detaching thread %d\n", i);
		if ((err = pthread_detach(thread)) != EOK) {
			printf("Failed to detach thread %d. Error: %d\n", i, err);
			++failed;
			continue;
		}

		printf("Detaching already detached thread\n");
		if ((err = pthread_detach(thread)) != EOK) {
			printf("Detaching thread %d failed as it should. Error: %d\n", i, err);
			++passed;
		}
		else {
			printf("Detaching thread should fail %d. Error: %d\n", i, err);
			++failed;
		}
	}

	printf("\n%s tests passed: %d, failed: %d\n\n", __func__, passed, failed);

	return passed;
};


int main(void)
{
	int passed = 0;
	int num_tests = 0;

	printf("test_pthreads: Starting, main is at %p\n\n", main);

	printf("test_pthreads: Testing thread create/join:\n");
	passed += test_pthread_create_join(TEST_CREATE_JOIN_NUM);
	num_tests += TEST_CREATE_JOIN_NUM;

	printf("test_pthreads: Testing multiple thread create/join:\n");
	passed += test_pthread_create_join_multiple(TEST_CREATE_JOIN_NUM);
	num_tests += TEST_CREATE_JOIN_NUM;

	printf("test_pthreads: Testing threads equal:\n");
	passed += test_pthread_equal();
	num_tests += TEST_EQUAL;

	printf("test_pthreads: Testing lock/unlock mutex:\n");
	passed += test_pthread_lock_unlock_mutex();
	num_tests += TEST_MUTEX;

	printf("test_pthreads: Testing thread create/detach:\n");
	passed += test_pthread_create_detach(TEST_CREATE_JOIN_NUM);
	num_tests += TEST_CREATE_JOIN_NUM;

	printf("\ntest_pthreads: %d PASSED\n", passed);
	printf("test_pthreads: %d FAILED\n", num_tests - passed);

	return 0;
}

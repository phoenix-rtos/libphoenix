/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX implementation - semaphores
 *
 * Copyright 2026 Phoenix Systems
 * Author: Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <semaphore.h>
#include <stdarg.h>
#include <dirent.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/minmax.h>
#include <sys/semaphore.h>
#include <phoenix/ioctl.h>

static int sem_open_msg(sem_t *sem)
{
	int ret = EOK;
	msg_t msg = {
		.type = mtOpen,
		.oid = sem->named,
	};

	ret = msgSend(sem->named.port, &msg);
	if (ret == EOK) {
		ret = msg.o.err;
	}

	return ret;
}


static int sem_down_msg(sem_t *sem, bool try, time_t timeout)
{
	int ret = EOK;
	msg_t msg = {
		.type = mtDevCtl,
		.oid = sem->named,
	};
	ioctl_in_t *data;

	data = (ioctl_in_t *)&msg.i.raw;
	if (try) {
		data->request = SEM_DOWN_TRY;
	}
	else if (timeout > 0) {
		data->request = SEM_DOWN_TIMEOUT;
		*((time_t *)data->data) = timeout;
	}
	else {
		data->request = SEM_DOWN;
	}

	ret = msgSend(sem->named.port, &msg);
	if (ret == EOK) {
		ret = msg.o.err;
	}

	return ret;
}


static int sem_create_msg(const char *name, int value, oid_t *oid)
{
	int ret = EOK;
	oid_t semctl;
	msg_t msg;

	ret = lookup(SEMCTL_PATH, NULL, &semctl);
	if (ret != EOK) {
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	msg.type = mtCreate;
	msg.oid = semctl;
	msg.i.create.type = value;
	msg.i.create.mode = 0;
	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	ret = msgSend(semctl.port, &msg);
	if (ret == EOK) {
		*oid = msg.o.create.oid;
		ret = msg.o.err;
	}

	return ret;
}


static int sem_lookup(const char *name, oid_t *sem)
{
	int ret = EOK;
	DIR *dirp;
	char *path;
	struct dirent *dent;
	oid_t oid;

	dirp = opendir(SEMAPHORE_PATH);
	if (dirp == NULL) {
		/* posixsrv has yet not initialized the semaphore subsystem */
		return -ENODEV;
	}

	path = (char *)malloc(PATH_MAX);
	if (path == NULL) {
		closedir(dirp);
		return -ENOMEM;
	}

	for (;;) {
		errno = 0;
		dent = readdir(dirp);
		if (dent == NULL) {
			closedir(dirp);
			free(path);
			ret = -ENOENT;
			break;
		}

		if (strcmp(name, dent->d_name) == 0) {
			strlcpy(path, SEMAPHORE_PATH, PATH_MAX);
			strlcat(path, dent->d_name, PATH_MAX);
			ret = lookup(path, NULL, &oid);
			assert(ret == EOK);
			*sem = oid;
			closedir(dirp);
			free(path);
			ret = EOK;
			break;
		}
	}

	return ret;
}


sem_t *sem_open(const char *name, int oflag, ...)
{
	va_list list;
	int err = EOK;
	mode_t mode = 0;
	unsigned int value = 0;
	sem_t *sem;

	(void)mode;

	if (name == NULL || strlen(name) > SEMAPHORE_NAME_MAX) {
		SET_ERRNO(-EINVAL);
		return SEM_FAILED;
	}

	sem = (sem_t *)malloc(sizeof(*sem));
	if (sem == NULL) {
		SET_ERRNO(-ENOMEM);
		return SEM_FAILED;
	}

	sem->type = smNamed;

	if ((oflag & O_CREAT) != 0U) {
		va_start(list, oflag);
		mode = va_arg(list, mode_t);
		value = va_arg(list, unsigned int);
		va_end(list);
	}

	if ((oflag & O_CREAT) != 0U && (oflag & O_EXCL) != 0U) {
		err = sem_create_msg(name, value, &sem->named);
	}
	else {
		err = sem_lookup(name, &sem->named);
		if ((oflag & O_CREAT) != 0U && err != EOK) {
			err = sem_create_msg(name, value, &sem->named);
		}
	}

	if (err != EOK) {
		SET_ERRNO(err);
		free(sem);
		return SEM_FAILED;
	}

	err = sem_open_msg(sem);
	if (err != EOK) {
		SET_ERRNO(err);
		free(sem);
		return SEM_FAILED;
	}

	return sem;
}


int sem_wait(sem_t *sem)
{
	int ret = EOK;

	if (sem == NULL) {
		ret = -EINVAL;
	}

	if (sem->type == smNamed) {
		ret = sem_down_msg(sem, false, 0);
	}
	else if (sem->type == smUnnamed) {
		ret = semaphoreDown(&sem->unnamed, 0);
	}
	else {
		ret = -EINVAL;
	}

	return SET_ERRNO(ret);
}


int sem_trywait(sem_t *sem)
{
	int ret = EOK;

	if (sem == NULL) {
		ret = -EINVAL;
	}

	if (sem->type == smNamed) {
		ret = sem_down_msg(sem, true, 0);
	}
	else if (sem->type == smUnnamed) {
		ret = semaphoreTryDown(&sem->unnamed);
	}
	else {
		ret = -EINVAL;
	}

	return SET_ERRNO(ret);
}


int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict abs_timeout)
{
	int ret = EOK;
	time_t timeout = 0, now = 0, offs = 0;

	gettime(&now, &offs);
	now += offs;

	timeout += abs_timeout->tv_sec * 1000000 + abs_timeout->tv_nsec / 1000;
	timeout -= now;

	if (sem == NULL) {
		ret = -EINVAL;
	}

	if (sem->type == smNamed) {
		ret = sem_down_msg(sem, false, timeout);
	}
	else if (sem->type == smUnnamed) {
		ret = semaphoreDown(&sem->unnamed, timeout);
		if (ret == -ETIME) {
			ret = -ETIMEDOUT;
		}
	}
	else {
		ret = -EINVAL;
	}

	return SET_ERRNO(ret);
}


int sem_post(sem_t *sem)
{
	int ret = EOK;
	msg_t msg = { 0 };
	ioctl_in_t *data;

	if (sem == NULL) {
		return SET_ERRNO(-EINVAL);
	}

	if (sem->type == smNamed) {
		msg.type = mtDevCtl;
		msg.oid = sem->named;
		data = (ioctl_in_t *)msg.i.raw;
		data->request = SEM_UP;
		ret = msgSend(sem->named.port, &msg);
		if (ret == EOK) {
			ret = msg.o.err;
		}
	}
	else if (sem->type == smUnnamed) {
		ret = semaphoreUp(&sem->unnamed);
	}
	else {
		ret = -EINVAL;
	}

	return SET_ERRNO(ret);
}


int sem_getvalue(sem_t *restrict sem, int *restrict value)
{
	int ret = EOK;
	msg_t msg;

	if (sem == NULL) {
		return SET_ERRNO(-EINVAL);
	}

	if (sem->type == smNamed) {
		memset(&msg, 0, sizeof(msg));
		msg.type = mtGetAttr;
		msg.oid = sem->named;
		ret = msgSend(sem->named.port, &msg);
		if (ret == EOK && value != NULL) {
			*value = msg.o.attr.val;
		}
	}
	else if (sem->type == smUnnamed) {
		ret = semaphoreCount(&sem->unnamed);
		if (ret > 0) {
			if (value != NULL) {
				*value = ret;
			}
			ret = EOK;
		}
	}
	else {
		ret = -EINVAL;
	}

	return SET_ERRNO(ret);
}


int sem_close(sem_t *sem)
{
	int ret = EOK;
	msg_t msg;

	if (sem == NULL || sem->type != smNamed) {
		return SET_ERRNO(-EINVAL);
	}

	msg.type = mtClose;
	msg.oid = sem->named;
	ret = msgSend(sem->named.port, &msg);

	return SET_ERRNO(ret);
}


int sem_init(sem_t *sem, int pshared, unsigned int value)
{
	int ret = EOK;
	sem_t new = { 0 };

	if (pshared != 0) {
		ret = -ENOSYS;
	}
	else if (sem == NULL) {
		ret = -EINVAL;
	}
	else {
		new.type = smUnnamed;
		ret = semaphoreCreate(&new.unnamed, value);
	}

	*sem = new;

	return SET_ERRNO(ret);
}


int sem_destroy(sem_t *sem)
{
	int ret = -EINVAL;

	if (sem != NULL && sem->type == smUnnamed) {
		ret = semaphoreDone(&sem->unnamed);
	}

	return SET_ERRNO(ret);
}


int sem_unlink(const char *name)
{
	int ret = EOK;
	oid_t oid;
	msg_t msg;

	ret = sem_lookup(name, &oid);
	if (ret == EOK) {
		msg.type = mtDestroy;
		msg.oid = oid;
		ret = msgSend(oid.port, &msg);
	}

	return SET_ERRNO(ret);
}

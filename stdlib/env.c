/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * env.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Krystian Wasik
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

static size_t _size = 0; /* Total number of slots. */
static size_t _cnt = 0; /* Number of currently used slots. */

/* Indicates if environ was internally allocated. */
static unsigned _environ_allocated = 0;

/* Indicates if string was internally allocated. If NULL, no strings were internally allocated. */
static unsigned *_string_allocated = NULL;

#define ENV_SIZE_INCR_DIV   (2)


void _env_init(void)
{
	if (environ) {
		char **c;
		for (c = environ; *c; c++)
			_cnt++;
		_size = _cnt + 1;
	}
}

static char *_env_find(const char *name, size_t len, int *idx)
{
	unsigned i;
	char **c;

	if (!environ)
		return NULL;

	for (c = environ, i = 0; *c; c++, i++) {
		if (strncmp(*c, name, len) == 0 && (*c)[len] == '=') {
			if (idx) *idx = i;
			return *c;
		}
	}

	return NULL;
}

static int _env_insert(int idx, char *v, unsigned allocated)
{
	size_t cnt = _cnt;

	if (idx < 0) { /* If not reusing slot */

		if (cnt + 1 >= _size) { /* No free slots */

			size_t incr, new_size;
			char **new_environ;

			incr = _size / ENV_SIZE_INCR_DIV;
			if (incr < 1) incr = 1;
			new_size = _size + incr;
			if (new_size < 2) new_size = 2;

			if (_environ_allocated) {

				new_environ = realloc(environ, sizeof(char*) * new_size);
				if (!new_environ) {
					errno = ENOMEM;
					return -1;
				}

			} else {

				new_environ = malloc(sizeof(char*) * new_size);
				if (!new_environ) {
					errno = ENOMEM;
					return -1;
				}
				if (environ)
					memcpy(new_environ, environ, sizeof(char*) * _size);
				_environ_allocated = 1;
			}

			memset(new_environ + _size, 0, sizeof(char*) * (new_size - _size));

			environ = new_environ;

			if (_string_allocated) {
				unsigned * ptr = realloc(_string_allocated, sizeof(unsigned) * new_size);
				if (!ptr) {
					errno = ENOMEM;
					return -1;
				}
				_string_allocated = ptr;
			}

			_size = new_size;
		}

		idx = cnt++;
	}

	if (allocated && !_string_allocated) {

		_string_allocated = malloc(sizeof(unsigned) * _size);
		if (!_string_allocated) {
			errno = ENOMEM;
			return -1;
		}
		memset(_string_allocated, 0, sizeof(unsigned) * _size);

		_string_allocated[idx] = allocated;

	} else if (_string_allocated) {
		_string_allocated[idx] = allocated;
	}

	environ[idx] = v;
	_cnt = cnt;

	return 0;
}


int unsetenv(const char *name)
{
	char *v;
	int idx;

	if ((name == NULL) || (*name == '\0') || (strchr(name, '=') != NULL)) {
		return set_errno(-EINVAL);
	}

	v = _env_find(name, strlen(name), &idx);
	if (v == NULL) {
		return 0;
	}

	if ((_string_allocated != NULL) && (_string_allocated[idx] != 0)) {
		free(environ[idx]);
	}

	_cnt--;

	environ[idx] = environ[_cnt];
	environ[_cnt] = NULL;

	if (_string_allocated != NULL) {
		_string_allocated[idx] = _string_allocated[_cnt];
	}

	return 0;
}


int putenv(char *string)
{
	int idx = -1;
	char *x;

	if ((x = strchr(string, '=')) == NULL) {
		errno = EINVAL;
		return -1;
	}

	size_t name_len = (size_t)(x - string);

	_env_find(string, name_len, &idx);

	return _env_insert(idx, string, 0);
}


int clearenv(void)
{
	unsigned i;

	if (_string_allocated) {
		char **c;
		for (c = environ, i = 0; *c; c++, i++) {
			if (_string_allocated[i])
				free(*c);
		}

		free(_string_allocated);
		_string_allocated = NULL;
	}

	for (i = 0; i <_size; i++)
		environ[i] = NULL;

	_cnt = 0;

	return 0;
}


char *getenv(const char *name)
{
	char *v = _env_find(name, strlen(name), NULL);
	if (!v)
		return NULL;
	v = strchr(v, '=') + 1;
	return v;
}

int setenv(const char *name, const char *value, int overwrite)
{
	int idx = -1;
	unsigned len;

	if (!name || strlen(name) == 0 || strchr(name, '=')) {
		errno = EINVAL;
		return -1;
	}

	len = strlen(name) + strlen(value) + 2; /* name, value, '=', '\0' */

	char *v = _env_find(name, strlen(name), &idx);
	if (v) {
		if (!overwrite)
			return 0;

		if (strlen(v) + 1 >= len) { /* Reuse slot and string */
			v = strchr(v, '=');
			if (!v)
				return -1;
			strcpy(++v, value);
			return 0;

		} else { /* Reuse slot. Deallocate string */
			if (_string_allocated && _string_allocated[idx])
				free(v);
			v = NULL;
		}

	}

	if (!v) {
		v = malloc(sizeof(char) * len);
		if (!v) {
			errno = ENOMEM;
			return -1;
		}
	}

	if (_env_insert(idx, v, 1) < 0) {
		/* Insert can fail only if we're not reusing slot, so if it fails then
		 * we know that v was dynamically allocated and has to be freed. */
		free(v);
		return -1;
	}

	char *c = v;
	while (*name != '\0') *(c++) = *(name++);
	*(c++) = '=';
	strcpy(c, value);

	return 0;
}

/* TODO: Move? */
int system(const char *command)
{
	sigset_t mask, old_mask;
	pid_t pid;
	int ret;

	/* we assume command processor is always available */
	if (command == NULL)
		return 1;

	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &mask, &old_mask);

	pid = fork();

	if (pid == -1) {
		sigprocmask(SIG_SETMASK, &old_mask, NULL);
		return -1;
	}

	if (pid == 0) {
		execl("/bin/sh", "sh", "-c", command, NULL);
		exit(EXIT_FAILURE);
	}

	waitpid(pid, &ret, 0);
	sigprocmask(SIG_SETMASK, &old_mask, NULL);

	return ret;
}

/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fs.c
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>


FILE *stderr, *stdin, *stdout;


int fclose(FILE *file)
{
	return 0;
#if 0
	int err;
	fsclose_t close;

	if (file == NULL)
		return -EINVAL;

	close = file->oid.id;

	err = send(file->oid.port, CLOSE, &close, sizeof(close), NORMAL, NULL, 0);

	free(file);

	return err;
#endif
}


FILE *fopen(const char *filename, const char *mode)
{
	return NULL;
#if 0
	oid_t oid;
	int err;
	unsigned int m;
	size_t len, i;
	fsopen_t open;
	fscreat_t *creat;
	FILE *f;

	if (filename == NULL || mode == NULL)
		return NULL;

	if (strcmp(mode, "r") == 0)
		m = O_RDONLY;
	else if (strcmp(mode, "w") == 0)
		m = O_WRONLY | O_CREAT | O_TRUNC;
	else if (strcmp(mode, "a") == 0)
		m = O_APPEND | O_CREAT;
	else if (strcmp(mode, "r+") == 0)
		m = O_RDWR;
	else if (strcmp(mode, "w+") == 0)
		m = O_RDWR | O_CREAT | O_TRUNC;
	else if (strcmp(mode, "a+") == 0)
		m = O_RDWR | O_CREAT | O_APPEND;
	else
		return NULL;

	if (lookup(filename, &oid) == EOK) {
		open.id = oid.id;
		open.mode = m;

		if (send(oid.port, OPEN, &open, sizeof(open), NORMAL, NULL, 0) != EOK)
			return NULL;
	}
	else if (m & O_CREAT) {
		len = strlen(filename);
		for (i = len; i > 0; --i) {
			if (filename[i] == '/')
				break;
		}

		if (i == 0)
			return NULL;

		if ((creat = malloc(sizeof(fscreat_t) + len + 1)) == NULL)
			return NULL;

		strcpy(creat->path, filename);
		creat->path[i + 1] = '\0';

		if (lookup(creat->path, &oid) != EOK) {
			free(creat);
			return NULL;
		}

		creat->path[i + 1] = filename[i + 1];

		err = send(oid.port, CREAT, creat, sizeof(fscreat_t), NORMAL, &oid, sizeof(oid)) != sizeof(oid);

		free(creat);

		if (err != sizeof(oid))
			return NULL;
	}
	else {
		return NULL;
	}

	/* Now we have valid OID */
	if ((f = malloc(sizeof(FILE))) == NULL)
		return NULL;

	f->oid = oid;
	f->buff = NULL;
	f->buffsz = 0;
	f->pos = 0; /* TODO - get filesize and set pos if append */

	return f;
#endif
}


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return 0;
#if 0
	fsdata_t data;
	int err;

	if (ptr == NULL || stream == NULL)
		return 0;

	data.id = stream->oid.id;
	data.pos = stream->pos;

	err = send(stream->oid.port, READ, &data, sizeof(data), NORMAL, ptr, size * nmemb);

	if (err <= 0)
		err = 0;

	stream->pos += err;

	return (size_t)err;
#endif
}


size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return 0;
#if 0
	size_t cnt;
	int err;
	fsdata_t *data;

	if (ptr == NULL || stream == NULL)
		return 0;

	if ((data = malloc(sizeof(fsdata_t) + size * nmemb)) == NULL)
		return 0;

	data->id = stream->oid.id;
	data->pos = stream->pos;
	memcpy(data->buff, ptr, size * nmemb);

	err = send(stream->oid.port, WRITE, data, sizeof(fsdata_t) + size * nmemb, NORMAL, &cnt, sizeof(cnt));

	if (err == sizeof(cnt)) {
		stream->pos += cnt;
		return cnt;
	}

	return 0;
#endif
}


int fgetc(FILE *stream)
{
	return -ENOSYS;
#if 0
	int err;
	unsigned char c;
	fsdata_t data;

	if (stream == NULL)
		return -EINVAL;

	data.id = stream->oid.id;
	data.pos = stream->pos;

	err = send(stream->oid.port, READ, &data, sizeof(data), NORMAL, &c, sizeof(c));

	if (err > 0)
		return (int)c;

	return err;
#endif
}


int fputc(int c, FILE *stream)
{
	/* Temporary: stdout */
	write(1, &c, 1);
	return 0;
#if 0
	int err;
	union {
		fsdata_t data;
		char buff[sizeof(fsdata_t) + 1];
	} u;

	if (stream == NULL)
		return -EINVAL;

	u.data.id = stream->oid.id;
	u.data.pos = stream->pos;
	u.data.buff[0] = (unsigned char)c;

	err = send(stream->oid.port, WRITE, &u.data, sizeof(u.buff), NORMAL, NULL, 0);

	if (err < 0)
		return err;

	++(stream->pos);

	return EOK;
#endif
}


char *fgets(char *str, int n, FILE *stream)
{
	return NULL;
}


char *fgets_unlocked(char *str, int n, FILE *stream)
{
	return NULL;
}


int fileno_unlocked(FILE *stream)
{
	return -ENOSYS;
}


int getc_unlocked(FILE *stream)
{
	char c;
	/* Temporary: stdin */
	read(0, &c, 1);
	return c;
}


int putc_unlocked(int c)
{
	/* Temporary: stdout */
	write(1, &c, 1);
	return 0;
}


int puts(const char *s)
{
	int len = strlen(s);
	write(1, (void *)s, len);
	return 0;
}


int fputs_unlocked(const char *s, FILE *stream)
{
	int len = strlen(s);
	/* Temporary: stdout */
	write(1, (void *)s, len);
	return 0;
}


int ferror(FILE *stream)
{
	return 0;
}


void clearerr(FILE *stream)
{
}


int ferror_unlocked(FILE *stream)
{
	return 0;
}


int fflush(FILE *stream)
{
	return 0;
}


int fputs(const char *str, FILE *f)
{
	int len = strlen(str);
	/* Temporary: stdout */
	write(1, (void *)str, len);
	return EOK;
}

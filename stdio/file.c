/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fs.c
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski, Jan Sikorski
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
	msg_t msg = { 0 };
	int err;

	if (file == NULL)
		return -EINVAL;

	msg.type = mtClose;
	msg.i.openclose.oid = file->oid;

	if ((err = msgSend(file->oid.port, &msg)) < 0)
		return err;

	return msg.o.io.err;
}


FILE *fopen(const char *filename, const char *mode)
{
	oid_t oid;
	unsigned int m;
	msg_t msg = { 0 };
	FILE *f;
	char *canonical_name, *name, *parent;

	if (filename == NULL || mode == NULL)
		return NULL;

	canonical_name = canonicalize_file_name(filename);

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

	if (lookup(canonical_name, &oid) == EOK) {
		msg.type = mtOpen;
		memcpy(&msg.i.openclose.oid, &oid, sizeof(oid));

		if (msgSend(oid.port, &msg) != EOK)
			return NULL;

		if (msg.o.io.err < 0)
			return NULL;
	}
	else if (m & O_CREAT) {
		msg.type = mtCreate;
		msg.i.create.type = 1;

		name = strrchr(canonical_name, '/');

		if (name == canonical_name) {
			name++;
			parent = "/";
		}
		else {
			*(name++) = 0;
			parent = canonical_name;
		}

		if (lookup(parent, &oid) < EOK) {
			free(canonical_name);
			return NULL;
		}

		if (msgSend(oid.port, &msg) != EOK) {
			free(canonical_name);
			return NULL;
		}

		msg.i.ln.dir = oid;
		msg.type = mtLink;
		memcpy(&msg.i.ln.oid, &msg.o.create.oid, sizeof(oid));
		memcpy(&oid, &msg.o.create.oid, sizeof(oid));
		msg.i.data = name;
		msg.i.size = strlen(name) + 1;

		if (msgSend(oid.port, &msg) != EOK) {
			free(canonical_name);
			return NULL;
		}

		free(canonical_name);
	}
	else {
		return NULL;
	}

	/* Now we have valid OID */
	if ((f = malloc(sizeof(FILE))) == NULL)
		return NULL;

	memcpy(&f->oid, &oid, sizeof(oid));
	f->buff = NULL;
	f->buffsz = 0;
	f->pos = 0; /* TODO - get filesize and set pos if append */

	return f;
}


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int err;
	msg_t msg = { 0 };

	if (ptr == NULL || stream == NULL)
		return 0;

	msg.type = mtRead;
	msg.i.io.oid = stream->oid;
	msg.i.io.offs = stream->pos;
	msg.i.io.len = 0;

	msg.o.data = ptr;
	msg.o.size = size * nmemb;

	if ((err = msgSend(stream->oid.port, &msg)) < 0)
		return err;

	if (msg.o.io.err < 0)
		msg.o.io.err = 0;

	stream->pos += msg.o.io.err;
	return msg.o.io.err;
}


size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int err;
	msg_t msg = { 0 };

	if (ptr == NULL || stream == NULL)
		return 0;

	msg.type = mtWrite;
	msg.i.io.oid = stream->oid;
	msg.i.io.offs = stream->pos;
	msg.i.io.len = 0;

	msg.i.data = (void *)ptr;
	msg.i.size = size * nmemb;

	if ((err = msgSend(stream->oid.port, &msg)) < 0)
		return err;

	if (msg.o.io.err < 0)
		msg.o.io.err = 0;

	stream->pos += msg.o.io.err;
	return msg.o.io.err;
}


size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}


int fgetc(FILE *stream)
{
	char c;
	if (fread(&c, 1, 1, stream) != 1)
		return EOF;

	return c;
}


int fputc(int c, FILE *stream)
{
	if (fwrite(&c, 1, 1, stream) != 1)
		return EOF;

	return c;
}


char *fgets(char *str, int n, FILE *stream)
{
	int c, i = 0;
	while ((c = fgetc(stream)) != EOF) {
		str[i++] = c;
		if (c == '\n' || i == n - 1)
			break;
	}

	if (i)
		str[i] = 0;
	else
		return NULL;

	return str;
}


char *fgets_unlocked(char *str, int n, FILE *stream)
{
	return fgets(str, n, stream);
}


int fileno_unlocked(FILE *stream)
{
	return -ENOSYS;
}


int getc_unlocked(FILE *stream)
{
	char c;
	if (fread(&c, 1, 1, stream) != 1)
		return EOF;

	return c;
}


int putc_unlocked(int c)
{
	/* Temporary: stdout */
	write(1, &c, 1);
	return 0;
}


int putchar_unlocked(int c)
{
	/* Temporary: stdout */
	write(1, &c, 1);
	return 0;
}


int puts(const char *s)
{
	int len = strlen(s), l = 0, err;

	while (l < len) {
		if ((err = write(1, (void *)s + l, len - l)) < 0)
			return -1;
		l += err;
	}

	return l;
}


int fputs_unlocked(const char *s, FILE *stream)
{
	int len = strlen(s);
	return fwrite(s, 1, len, stream);
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
	return fwrite(str, 1, len, f);
}


int ioctl(int fildes, int request, ... /* arg */)
{
	return EOK;
}


int getchar_unlocked(void)
{
	int c;
	read(0, &c, 1);
	return c;
}

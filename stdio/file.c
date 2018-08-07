/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fs.c
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Aleksander Kaminski, Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>


FILE *stderr, *stdin, *stdout;


static int string2mode(const char *mode)
{
	if (strcmp(mode, "r") == 0)
		return O_RDONLY;
	else if (strcmp(mode, "w") == 0)
		return O_WRONLY | O_CREAT | O_TRUNC;
	else if (strcmp(mode, "a") == 0)
		return O_APPEND | O_CREAT;
	else if (strcmp(mode, "r+") == 0)
		return O_RDWR;
	else if (strcmp(mode, "w+") == 0)
		return O_RDWR | O_CREAT | O_TRUNC;
	else if (strcmp(mode, "a+") == 0)
		return O_RDWR | O_CREAT | O_APPEND;
	else
		return -1;
}


int fclose(FILE *file)
{
	int err;

	if (file == NULL)
		return -EINVAL;

	err = close(file->fd);
	free(file);

	return err;
}


FILE *fopen(const char *filename, const char *mode)
{
	unsigned int m;
	FILE *f;
	int fd;

	if (filename == NULL || mode == NULL)
		return NULL;

	if ((m = string2mode(mode)) < 0)
		return NULL;

	if ((fd = open(filename, m, DEFFILEMODE)) < 0)
		return NULL;

	if ((f = malloc(sizeof(FILE))) == NULL)
		return NULL;

	f->fd = fd;
	f->buff = NULL;
	f->buffsz = 0;

	return f;
}


FILE *fdopen(int fd, const char *mode)
{
//	unsigned int m;
	FILE *f;

	if (mode == NULL)
		return NULL;

#if 0 /* TODO: check if mode is compatible with fd's mode */
	if ((m = string2mode(mode)) < 0)
		return NULL;
#endif

	if ((f = malloc(sizeof(FILE))) == NULL)
		return NULL;

	f->fd = fd;
	f->buff = NULL;
	f->buffsz = 0;

	return f;
}


FILE *freopen(const char *pathname, const char *mode, FILE *stream)
{
	int m;

	if (mode == NULL || stream == NULL)
		return NULL;

	fflush(stream);

	if (pathname != NULL) {
		close(stream->fd);
		if ((m = string2mode(mode)) < 0)
			return NULL;

		stream->fd = open(pathname, m);
	}
	else {
		/* change mode */
	}

	return stream;
}


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int err;

	if (ptr == NULL || stream == NULL)
		return 0;

	err = read(stream->fd, ptr, size * nmemb);
	return err < 0 ? 0 : err;
}


size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int err;

	if (ptr == NULL || stream == NULL)
		return 0;

	err = write(stream->fd, ptr, size * nmemb);
	return err < 0 ? 0 : err;
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


long int ftell(FILE *stream)
{
	return (long)lseek(stream->fd, 0, SEEK_CUR);
}

off_t ftello(FILE *stream)
{
	return (off_t)lseek(stream->fd, 0, SEEK_CUR);
}

char *fgets_unlocked(char *str, int n, FILE *stream)
{
	return fgets(str, n, stream);
}

int fileno(FILE *stream)
{
	return stream->fd;
}

int fileno_unlocked(FILE *stream)
{
	return stream->fd;
}


int getc_unlocked(FILE *stream)
{
	char c;
	if (fread(&c, 1, 1, stream) != 1)
		return EOF;

	return c;
}


int putc(int c, FILE *stream)
{
	fwrite(&c, 1, 1, stream); /* FIXME: won't work on big endian */
	return c;
}


int putc_unlocked(int c, FILE *stream)
{
	fwrite(&c, 1, 1, stream); /* FIXME: won't work on big endian */
	return c;
}

int putchar_unlocked(int c)
{
	char cc = c;
	/* Temporary: stdout */
	if (write(1, &cc, 1) < 0)
		return EOF;
	return c;
}

int puts(const char *s)
{
	int len = strlen(s), l = 0, err;

	while (l < len) {
		if ((err = write(1, (void *)s + l, len - l)) < 0)
			return -1;
		l += err;
	}
	putchar_unlocked('\n');

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


int getchar_unlocked(void)
{
	int c;
	read(0, &c, 1);
	return c;
}


int fseek(FILE *stream, long offset, int whence)
{
	return lseek(stream->fd, offset, whence);
}


void rewind(FILE *file)
{
	fseek(file, 0, 0);
}


int fseeko(FILE *stream, off_t offset, int whence)
{
	return fseek(stream, offset, whence);
}


void __fpurge(FILE *stream)
{
}


ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
	char buff[128] = { 0 };
	size_t linesz = 0;
	offs_t offs;
	int readsz, i, nl = 0;

	offs = (offs_t)ftell(stream);

	while ((readsz = fread(buff, sizeof(buff), 1, stream)) > 0) {
		for (i = 0; i < readsz; i++) {
			if (buff[i] == '\n') {
				linesz += i + 2;
				nl = 1;
				break;
			}
		}
		if (nl)
			break;
		linesz += readsz;
	}

	if (!nl)
	   linesz++;

	if (*lineptr == NULL && linesz > 0)
		*lineptr = malloc(linesz);
	else if (*n < linesz)
		*lineptr = realloc(*lineptr, linesz);

	*n = linesz - 1;

	fseek(stream, offs, SEEK_SET);

	fread(*lineptr, linesz - 1, 1, stream);
	if (linesz > 0)
		(*lineptr)[linesz - 1] = 0;

	return linesz - 1;
}

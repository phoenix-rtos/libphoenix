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
#include <sys/mman.h>
#include <unistd.h>


#define F_EOF (1 << 0)
#define F_WRITING (1 << 1)
#define F_LINE (1 << 2)
#define F_ERROR (1 << 3)


extern FILE *stderr, *stdin, *stdout;


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

	fflush(file);
	err = close(file->fd);

	if (file->buffer != NULL)
		munmap(file->buffer, BUFSIZ);

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

	if ((f = calloc(1, sizeof(FILE))) == NULL)
		return NULL;

	if ((f->buffer = mmap(NULL, BUFSIZ, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, NULL, 0)) == MAP_FAILED) {
		close(fd);
		free(f);
		return NULL;
	}

	f->fd = fd;
	fflush(f);
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

	if ((f = calloc(1, sizeof(FILE))) == NULL)
		return NULL;

	if ((f->buffer = mmap(NULL, BUFSIZ, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, NULL, 0)) == MAP_FAILED) {
		close(fd);
		free(f);
		return NULL;
	}

	f->fd = fd;
	fflush(f);
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
	size_t readsz = nmemb * size;
	size_t bytes;

	if (!readsz || (stream->flags & F_EOF))
		return 0;

	if (stream->buffer == NULL) {
		/* unbuffered read */
		if ((err = read(stream->fd, ptr, readsz)) < 0)
			return 0;

		return err / size;
	}

	/* flush write buffer if writing */
	if (stream->flags & F_WRITING) {
		if (fflush(stream) < 0)
			return 0;

		stream->flags &= ~F_WRITING;
	}

	/* refill read buffer */
	if (stream->bufpos == stream->bufeof) {
		if ((err = read(stream->fd, stream->buffer, BUFSIZ)) == -1)
			return 0;

		stream->bufpos = 0;
		stream->bufeof = err;

		if (!err) {
			stream->flags |= F_EOF;
			return 0;
		}
	}

	if ((bytes = stream->bufeof - stream->bufpos)) {
		bytes = min(bytes, readsz);
		memcpy(ptr, stream->buffer + stream->bufpos, bytes);

		ptr += bytes;
		stream->bufpos += bytes;
		readsz -= bytes;

		if (!readsz)
			return nmemb;
	}

	/* read remainder directly into ptr */
	if ((err = read(stream->fd, ptr, readsz)) != -1) {
		bytes += err;

		if (!err)
			stream->flags |= F_EOF;
	}
	else {
		stream->flags |= F_ERROR;
		return 0;
	}

	return bytes / size;
}


size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int err;
	size_t writesz = nmemb * size;
	char *nl;

	if (!writesz)
		return 0;

	if (stream->buffer == NULL) {
		/* unbuffered write */
		if ((err = write(stream->fd, ptr, writesz)) < 0)
			return 0;

		return err / size;
	}

	/* discard reading buffer */
	if (!(stream->flags & F_WRITING)) {
		fflush(stream);
		stream->flags |= F_WRITING;
	}

	/* write to buffer if fits */
	if ((BUFSIZ - stream->bufpos) > writesz) {
		memcpy(stream->buffer + stream->bufpos, ptr, writesz);
		stream->bufpos += writesz;

		if ((stream->flags & F_LINE) && (nl = memrchr(stream->buffer, '\n', stream->bufpos)) != NULL) {
			if ((err = write(stream->fd, stream->buffer, nl - stream->buffer + 1)) < 0)
				return 0;

			stream->bufpos -= nl - stream->buffer + 1;
			memmove(stream->buffer, nl + 1, stream->bufpos);
		}

		return nmemb;
	}

	/* flush buffer and write to file */
	if ((err = write(stream->fd, stream->buffer, stream->bufpos)) == -1)
		return 0;

	stream->bufpos = 0;

	if ((err = write(stream->fd, ptr, writesz)) == -1)
		return 0;

	return err / size;
}


size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}


int fgetc(FILE *stream)
{
	unsigned char c;

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
	return (long)fseek(stream, 0, SEEK_CUR);
}


off_t ftello(FILE *stream)
{
	return (off_t)fseek(stream, 0, SEEK_CUR);
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


int getc(FILE *stream)
{
	return fgetc(stream);
}


int getc_unlocked(FILE *stream)
{
	return fgetc(stream);
}


int putc(int c, FILE *stream)
{
	char cc = c;
	fwrite(&cc, 1, 1, stream);
	return c;
}


int putc_unlocked(int c, FILE *stream)
{
	char cc = c;
	fwrite(&cc, 1, 1, stream);
	return c;
}


int putchar_unlocked(int c)
{
	char cc = c;
	fwrite(&cc, 1, 1, stdout);
	return c;
}


int putchar(int ci)
{
	return putchar_unlocked(ci);
}


int ungetc(int c, FILE *stream)
{
	/* TODO: The file-position indicator is decremented by each successful call to ungetc(); */
	if (c == EOF)
		return EOF;

	/* flush write buffer if writing */
	if (stream->flags & F_WRITING) {
		if (fflush(stream) < 0)
			return EOF;

		stream->flags &= ~F_WRITING;
	}

	if (!stream->bufpos)
		return EOF;

	stream->buffer[--stream->bufpos] = c;
	stream->flags &= ~F_EOF;
	return c;
}


int puts(const char *s)
{
	int len = strlen(s), l = 0, err;

	while (l < len) {
		if ((err = fwrite((void *)s + l, 1, len - l, stdout)) < 0)
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
	return !!(stream->flags & F_ERROR);
}


void clearerr(FILE *stream)
{
	stream->flags &= ~F_ERROR;
}


int ferror_unlocked(FILE *stream)
{
	return 0;
}


int fflush(FILE *stream)
{
	if (stream == NULL) {
		fflush(stdout);
		return 0; /* TODO: flush all FILE's */
	}

	if (stream->flags & F_WRITING) {
		if (stream->bufpos) {
			write(stream->fd, stream->buffer, stream->bufpos);
			stream->bufpos = 0;
		}
	}
	else {
		lseek(stream->fd, stream->bufpos - stream->bufeof, SEEK_CUR);
		stream->bufpos = stream->bufeof = BUFSIZ;
	}

	return 0;
}


int fputs(const char *str, FILE *f)
{
	int len = strlen(str);
	return fwrite(str, 1, len, f);
}


int getchar_unlocked(void)
{
	return getc_unlocked(stdin);
}


int fseek(FILE *stream, long offset, int whence)
{
	fflush(stream);
	stream->flags &= ~F_EOF;
	return lseek(stream->fd, offset, whence);
}


void rewind(FILE *file)
{
	fseek(file, 0, SEEK_SET);
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

	while ((readsz = fread(buff, 1, sizeof(buff), stream)) > 0) {
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

	if (linesz == 0)
		return -1; // EOF

	if (!nl)
	   linesz++;

	if (*lineptr == NULL && linesz > 0)
		*lineptr = malloc(linesz);
	else if (*n < linesz)
		*lineptr = realloc(*lineptr, linesz);

	*n = linesz - 1;

	fseek(stream, offs, SEEK_SET);
	fread(*lineptr, 1, linesz - 1, stream);

	if (linesz > 0)
		(*lineptr)[linesz - 1] = 0;

	return linesz - 1;
}


int feof(FILE *stream)
{
	return !!(stream->flags & F_EOF);
}


void setbuf(FILE *stream, char *buf)
{
	/* TODO */
}


int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
	/* TODO */
	return 0;
}


void _file_init(void)
{
	stdin->buffer = mmap(NULL, BUFSIZ, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, NULL, 0);
	stdin->bufeof = stdin->bufpos = BUFSIZ;

	stdout->buffer = mmap(NULL, BUFSIZ, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, NULL, 0);
	stdout->bufpos = 0;
	stdout->flags |= F_WRITING;

	if (isatty(stdout->fd))
		stdout->flags |= F_LINE;
}

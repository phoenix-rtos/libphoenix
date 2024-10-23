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

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/minmax.h>
#include <sys/mman.h>
#include <sys/threads.h>
#include <sys/list.h>

#include <arch.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include "../unistd/file-internal.h"


#define F_EOF     (1 << 0)
#define F_WRITING (1 << 1)
#define F_LINE    (1 << 2)
#define F_ERROR   (1 << 3)
#define F_USRBUF  (1 << 4)

typedef struct {
	FILE file; /* Must be the first member */
	pid_t pid;
} popen_FILE;


FILE *stdin, *stdout, *stderr;


static struct {
	FILE *list;
	handle_t lock;
} file_common;


static int string2mode(const char *mode)
{
	int next_char = 1;

	if (mode == NULL) {
		return -1;
	}
	else if (mode[0] == 'r') {
		if (mode[next_char] == 'b') {
			next_char += 1;
		}

		if (mode[next_char] == '\0') {
			return O_RDONLY;
		}
		else if (mode[next_char] == '+') {
			return O_RDWR;
		}
		else if (mode[next_char] == 'c') {
			/* glibc extension - ignored */
			return O_RDONLY;
		}
		else {
			return -1;
		}
	}
	else if (mode[0] == 'w') {
		if (mode[next_char] == 'b') {
			next_char += 1;
		}

		if (mode[next_char] == '\0') {
			return O_WRONLY | O_CREAT | O_TRUNC;
		}
		else if (mode[next_char] == '+') {
			return O_RDWR | O_CREAT | O_TRUNC;
		}
		else {
			return -1;
		}
	}
	else if (mode[0] == 'a') {
		if (mode[next_char] == 'b') {
			next_char += 1;
		}

		if (mode[next_char] == '\0') {
			return O_WRONLY | O_CREAT | O_APPEND;
		}
		else if (mode[next_char] == '+') {
			return O_RDWR | O_CREAT | O_APPEND;
		}
		else {
			return -1;
		}
	}

	return -1;
}


static void *buffAlloc(size_t size)
{
	void *ret;

#ifndef NOMMU
	ret = mmap(NULL, (size + (_PAGE_SIZE - 1)) & ~(_PAGE_SIZE - 1), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
	if (ret == MAP_FAILED) {
		return NULL;
	}
#else
	ret = malloc(size);
#endif

	return ret;
}


static void buffFree(void *ptr, size_t size)
{
#ifndef NOMMU
	munmap(ptr, (size + (_PAGE_SIZE - 1)) & ~(_PAGE_SIZE - 1));
#else
	free(ptr);
#endif
}


static void file_free(FILE *file)
{
	mutexLock(file_common.lock);
	LIST_REMOVE(&file_common.list, file);
	mutexUnlock(file_common.lock);

	if (file->buffer != NULL && !(file->flags & F_USRBUF)) {
		buffFree(file->buffer, file->bufsz);
	}

	resourceDestroy(file->lock);

	free(file);
}


int fclose(FILE *stream)
{
	int err;

	if (stream == NULL) {
		errno = EBADF;
		return EOF;
	}

	err = fflush(stream);
	if (err == EOF) {
		return EOF;
	}

	err = __safe_close(stream->fd);
	file_free(stream);

	return (err < 0) ? EOF : 0;
}


FILE *fopen(const char *filename, const char *mode)
{
	int m;
	FILE *f;
	int fd;

	if ((m = string2mode(mode)) < 0) {
		errno = EINVAL;
		return NULL;
	}

	if ((fd = __safe_open(filename, m, DEFFILEMODE)) < 0) {
		return NULL;
	}

	if ((f = calloc(1, sizeof(FILE))) == NULL) {
		__safe_close(fd);
		return NULL;
	}

	if ((f->buffer = buffAlloc(BUFSIZ)) == NULL) {
		__safe_close(fd);
		free(f);
		return NULL;
	}

	mutexCreate(&f->lock);
	f->bufsz = BUFSIZ;
	f->fd = fd;
	f->mode = m;

	mutexLock(file_common.lock);
	LIST_ADD(&file_common.list, f);
	mutexUnlock(file_common.lock);

	return f;
}


FILE *fdopen(int fd, const char *mode)
{
	int m, fdm;
	FILE *f;

	if ((m = string2mode(mode)) < 0) {
		errno = EINVAL;
		return NULL;
	}

	if ((fdm = fcntl(fd, F_GETFL)) < 0) {
		return NULL;
	}

	/* POSIX: check if mode argument is allowed by the file access mode of the FD (not necessarily exactly the same) */
	fdm &= 0x7;
	if ((fdm != O_RDWR) && (fdm != (m & 0x7))) {
		errno = EINVAL;
		return NULL;
	}

	if ((f = calloc(1, sizeof(FILE))) == NULL) {
		return NULL;
	}

	if ((f->buffer = buffAlloc(BUFSIZ)) == NULL) {
		free(f);
		return NULL;
	}

	mutexCreate(&f->lock);
	f->bufsz = BUFSIZ;
	f->fd = fd;
	f->mode = m;

	mutexLock(file_common.lock);
	LIST_ADD(&file_common.list, f);
	mutexUnlock(file_common.lock);

	return f;
}


FILE *freopen(const char *pathname, const char *mode, FILE *stream)
{
	int m, err;

	if ((m = string2mode(mode)) < 0) {
		errno = EINVAL;
		return NULL;
	}

	err = fflush(stream);
	if (err == EOF) {
		return NULL;
	}

	if (pathname != NULL) {
		__safe_close(stream->fd);

		if ((stream->fd = __safe_open(pathname, m, DEFFILEMODE)) < 0) {
			file_free(stream);
			return NULL;
		}

		stream->mode = m;
	}
	else {
		/* TODO: change mode */
	}

	return stream;
}


static ssize_t full_write(int fd, const void *ptr, size_t size)
{
	ssize_t err;
	ssize_t total = 0;

	while (size > 0) {
		err = __safe_write_nb(fd, ptr, size);
		if (err < 0) {
			return (errno == EAGAIN) ? total : -1;
		}
		ptr += err;
		total += err;
		size -= err;
	}

	return total;
}


static int __fflush_one(FILE *stream)
{
	int ret = 0;
	ssize_t err;
	off_t off;

	if (stream->buffer == NULL) {
		return 0;
	}

	if ((stream->flags & F_WRITING) != 0) {
		if (stream->bufpos != 0) {
			err = full_write(stream->fd, stream->buffer, stream->bufpos);
			if (err != stream->bufpos) {
				stream->flags |= F_ERROR;
				ret = -1;
			}
			else {
				stream->bufpos = 0;
			}
		}
	}
	else {
		if (stream->bufpos != stream->bufeof) {
			off = lseek(stream->fd, (off_t)stream->bufpos - stream->bufeof, SEEK_CUR);
			if (off == (off_t)-1) {
				if (errno == ESPIPE) {
					/* read buffer for non-seekable stream cannot be flushed */
				}
				else {
					stream->flags |= F_ERROR;
					ret = -1;
				}
			}
			else {
				stream->bufpos = stream->bufeof = stream->bufsz;
			}
		}
	}

	return ret;
}


static inline size_t unbuffer_data(FILE *stream, void *ptr, size_t readsz)
{
	size_t bytes = min(stream->bufeof - stream->bufpos, readsz);
	if (bytes > 0) {
		memcpy(ptr, stream->buffer + stream->bufpos, bytes);
		stream->bufpos += bytes;
	}
	return bytes;
}


static inline ssize_t read_buffer(FILE *stream, size_t readsz)
{
	ssize_t err;
	ssize_t total = 0;

	/*
	 * Note that readsz < stream->bufsz. Try to refill the buffer,
	 * but stop if at least readsz bytes have already been read.
	 */
	while (total < readsz) {
		err = __safe_read_nb(stream->fd, stream->buffer + total, stream->bufsz - total);
		if (err < 0) {
			stream->flags |= F_ERROR;
			if (errno != EAGAIN) {
				return -1;
			}
			break;
		}
		else if (err == 0) {
			stream->flags |= F_EOF;
			break;
		}
		total += err;
	}

	if (total > 0) {
		stream->bufpos = 0;
		stream->bufeof = total;
	}

	return total;
}


static inline ssize_t read_data(FILE *stream, void *ptr, size_t readsz)
{
	ssize_t err;
	ssize_t total = 0;

	while (readsz > 0) {
		err = __safe_read_nb(stream->fd, ptr, readsz);
		if (err < 0) {
			stream->flags |= F_ERROR;
			if (errno != EAGAIN) {
				return -1;
			}
			break;
		}
		else if (err == 0) {
			stream->flags |= F_EOF;
			break;
		}
		ptr += err;
		total += err;
		readsz -= err;
	}

	return total;
}


size_t fread_unlocked(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	ssize_t err;
	size_t readsz = nmemb * size;
	size_t total = 0;
	size_t bytes;

	if (readsz == 0) {
		return 0;
	}

	if (stream->buffer == NULL) {
		/* unbuffered read */
		err = read_data(stream, ptr, readsz);
		return (err > 0) ? err / size : 0;
	}

	if (stream->mode & O_WRONLY) {
		set_errno(-EBADF);
		stream->flags |= F_ERROR;
		return 0;
	}

	/* flush the write buffer if currently writing */
	if ((stream->flags & F_WRITING) != 0) {
		if (__fflush_one(stream) < 0) {
			return 0;
		}
		stream->flags &= ~F_WRITING;
		stream->bufpos = stream->bufeof = stream->bufsz;
	}

	/* read from the buffer first */
	if (stream->bufpos != stream->bufeof) {
		bytes = unbuffer_data(stream, ptr, readsz);
		ptr += bytes;
		readsz -= bytes;
		total += bytes;
	}

	/* read full blocks directly from the file */
	if (readsz >= stream->bufsz) {
		bytes = (readsz / stream->bufsz) * stream->bufsz;
		err = read_data(stream, ptr, bytes);
		if (err > 0) {
			ptr += err;
			readsz -= err;
			total += err;
		}
		if (err < bytes) {
			return total / size;
		}
	}

	if (readsz == 0) {
		return nmemb;
	}

	/*
	 * Note that at this point, the buffer is empty and readsz < stream->bufsz.
	 * Refill the buffer and read the remaining bytes from it.
	 */
	err = read_buffer(stream, readsz);
	if (err < 0) {
		return total / size;
	}
	total += unbuffer_data(stream, ptr, readsz);

	return total / size;
}


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t ret;
	mutexLock(stream->lock);
	ret = fread_unlocked(ptr, size, nmemb, stream);
	mutexUnlock(stream->lock);
	return ret;
}


static inline size_t buffer_data(FILE *stream, const void *ptr, size_t writesz)
{
	size_t bytes = min(stream->bufsz - stream->bufpos, writesz);
	if (bytes > 0) {
		memcpy(stream->buffer + stream->bufpos, ptr, bytes);
		stream->bufpos += bytes;
	}
	return bytes;
}


static inline ssize_t write_buffer(FILE *stream, size_t writesz)
{
	ssize_t err = full_write(stream->fd, stream->buffer, writesz);
	if (err >= 0) {
		stream->bufpos -= err;

		if (err < writesz) {
			/* EAGAIN */
			stream->flags |= F_ERROR;

			if (err > 0) {
				memmove(stream->buffer, stream->buffer + err, stream->bufpos);
			}
		}
	}
	else {
		stream->flags |= F_ERROR;
	}
	return err;
}


static inline ssize_t write_data(FILE *stream, const void *ptr, size_t writesz)
{
	ssize_t err = full_write(stream->fd, ptr, writesz);
	if (err >= 0) {
		if (err < writesz) {
			/* EAGAIN */
			stream->flags |= F_ERROR;
		}
	}
	else {
		stream->flags |= F_ERROR;
	}
	return err;
}


size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	ssize_t err;
	size_t writesz = nmemb * size;
	size_t total = 0;
	size_t bytes;
	char *nl;

	if (writesz == 0) {
		return 0;
	}

	if (stream->buffer == NULL) {
		/* unbuffered write */
		err = write_data(stream, ptr, writesz);
		return (err > 0) ? err / size : 0;
	}

	if ((stream->mode & O_RDONLY) != 0) {
		set_errno(-EBADF);
		stream->flags |= F_ERROR;
		return 0;
	}

	/* flush the read buffer if currently reading */
	if ((stream->flags & F_WRITING) == 0) {
		if (__fflush_one(stream) < 0) {
			return 0;
		}
		if (stream->bufpos != stream->bufeof) {
			/* abort if the read buffer cannot be flushed (non-seekable stream) */
			return 0;
		}
		stream->flags |= F_WRITING;
		stream->bufpos = 0;
	}

	/* fill the incomplete buffer first */
	if (stream->bufpos > 0 && stream->bufpos < stream->bufsz) {
		bytes = buffer_data(stream, ptr, writesz);
		ptr += bytes;
		writesz -= bytes;
		total += bytes;
	}

	/* flush the full buffer */
	if (stream->bufpos == stream->bufsz) {
		err = write_buffer(stream, stream->bufsz);
		if (err < stream->bufsz) {
			if (err > 0) {
				total += buffer_data(stream, ptr, writesz);
			}
			return total / size;
		}
	}

	/*
	 * Note that at this point, either the buffer is empty with potentially more data to be written,
	 * or the buffer still contains data but there's nothing left to write.
	 */
	if (stream->bufpos == 0) {
		/* write full blocks directly to the file */
		if (writesz >= stream->bufsz) {
			bytes = (writesz / stream->bufsz) * stream->bufsz;
			err = write_data(stream, ptr, bytes);
			if (err > 0) {
				ptr += err;
				writesz -= err;
				total += err;
			}
			if (err < bytes) {
				if (err > 0) {
					total += buffer_data(stream, ptr, writesz);
				}
				return total / size;
			}
		}

		/* write full lines in line-buffered mode directly to the file */
		if (writesz > 0 && (stream->flags & F_LINE) != 0) {
			nl = memrchr(ptr, '\n', writesz);
			if (nl != NULL) {
				bytes = nl - (const char *)ptr + 1;
				err = write_data(stream, ptr, bytes);
				if (err > 0) {
					ptr += err;
					writesz -= err;
					total += err;
				}
				if (err < bytes) {
					if (err > 0) {
						total += buffer_data(stream, ptr, writesz);
					}
					return total / size;
				}
			}
		}

		/* write the remaining bytes to the empty buffer */
		total += buffer_data(stream, ptr, writesz);
	}
	else {
		/* write full lines from the buffer in line-buffered mode */
		if (stream->bufpos > 0 && (stream->flags & F_LINE) != 0) {
			nl = memrchr(stream->buffer, '\n', stream->bufpos);
			if (nl != NULL) {
				write_buffer(stream, nl - (const char *)stream->buffer + 1);
			}
		}
	}

	return total / size;
}


size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t ret;
	mutexLock(stream->lock);
	ret = fwrite_unlocked(ptr, size, nmemb, stream);
	mutexUnlock(stream->lock);
	return ret;
}


int fgetc_unlocked(FILE *stream)
{
	unsigned char cc;

	if (stream->flags & F_EOF) {
		return EOF;
	}

	if (fread_unlocked(&cc, 1, 1, stream) != 1) {
		return EOF;
	}

	return cc;
}


int fgetc(FILE *stream)
{
	int ret;
	mutexLock(stream->lock);
	ret = fgetc_unlocked(stream);
	mutexUnlock(stream->lock);
	return ret;
}


int fputc_unlocked(int c, FILE *stream)
{
	unsigned char cc = c;
	if (fwrite_unlocked(&cc, 1, 1, stream) != 1) {
		return EOF;
	}

	return c;
}


int fputc(int c, FILE *stream)
{
	int ret;
	mutexLock(stream->lock);
	ret = fputc_unlocked(c, stream);
	mutexUnlock(stream->lock);
	return ret;
}


char *fgets_unlocked(char *str, int n, FILE *stream)
{
	int c, i = 0;
	while ((c = fgetc_unlocked(stream)) != EOF) {
		str[i++] = c;
		if (c == '\n' || i == n - 1) {
			break;
		}
	}

	if (i) {
		str[i] = 0;
	}
	else {
		return NULL;
	}

	return str;
}


char *fgets(char *str, int n, FILE *stream)
{
	char *ret;
	mutexLock(stream->lock);
	ret = fgets_unlocked(str, n, stream);
	mutexUnlock(stream->lock);
	return ret;
}


static int __fflush_unlocked(FILE *stream, int lock)
{
	int ret = 0;

	if (stream == NULL) {
		mutexLock(file_common.lock);
		if (file_common.list != NULL) {
			FILE *iter = file_common.list;
			do {
				if (lock != 0) {
					mutexLock(iter->lock);
				}
				if ((iter->flags & F_WRITING) != 0) {
					if (__fflush_one(iter) < 0) {
						ret = EOF;
					}
				}
				if (lock != 0) {
					mutexUnlock(iter->lock);
				}
				iter = iter->next;
			} while (iter != file_common.list);
		}
		mutexUnlock(file_common.lock);
	}
	else {
		if (__fflush_one(stream) < 0) {
			ret = EOF;
		}
	}

	return ret;
}


int fflush_unlocked(FILE *stream)
{
	return __fflush_unlocked(stream, 0);
}


int fflush(FILE *stream)
{
	int ret;

	if (stream != NULL) {
		mutexLock(stream->lock);
		ret = __fflush_one(stream);
		mutexUnlock(stream->lock);
	}
	else {
		ret = __fflush_unlocked(stream, 1);
	}

	return ret;
}


static off_t fseek_unlocked(FILE *stream, off_t offset, int whence)
{
	int err;

	err = __fflush_one(stream);
	if (err < 0) {
		return -1;
	}

	return lseek(stream->fd, offset, whence);
}


int fseek(FILE *stream, long offset, int whence)
{
	off_t off;

	mutexLock(stream->lock);
	off = fseek_unlocked(stream, offset, whence);
	if (off != (off_t)-1) {
		stream->flags &= ~F_EOF;
	}
	mutexUnlock(stream->lock);

	return (off != (off_t)-1) ? 0 : -1;
}


int fseeko(FILE *stream, off_t offset, int whence)
{
	off_t off;

	mutexLock(stream->lock);
	off = fseek_unlocked(stream, offset, whence);
	if (off != (off_t)-1) {
		stream->flags &= ~F_EOF;
	}
	mutexUnlock(stream->lock);

	return (off != (off_t)-1) ? 0 : -1;
}


static off_t ftell_unlocked(FILE *stream)
{
	off_t off;

	off = lseek(stream->fd, 0, SEEK_CUR);
	if (off == (off_t)-1) {
		return -1;
	}

	if (stream->buffer != NULL) {
		/* adjust the offset based on buffered data */
		if ((stream->flags & F_WRITING) != 0) {
			off += stream->bufpos;
		}
		else {
			if (stream->bufpos != stream->bufeof) {
				off -= stream->bufeof - stream->bufpos;
			}
		}
	}

	return off;
}


long int ftell(FILE *stream)
{
	long int ret;
	mutexLock(stream->lock);
	ret = (long int)ftell_unlocked(stream);
	mutexUnlock(stream->lock);
	return ret;
}


off_t ftello(FILE *stream)
{
	off_t ret;
	mutexLock(stream->lock);
	ret = ftell_unlocked(stream);
	mutexUnlock(stream->lock);
	return ret;
}


int fgetpos(FILE *stream, fpos_t *pos)
{
	off_t ret = ftello(stream);
	if (ret >= 0) {
		*pos = (fpos_t)ret;
		return 0;
	}
	return -1;
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
	return fgetc_unlocked(stream);
}


int getc(FILE *stream)
{
	int ret;
	mutexLock(stream->lock);
	ret = getc_unlocked(stream);
	mutexUnlock(stream->lock);
	return ret;
}


int putc_unlocked(int c, FILE *stream)
{
	unsigned char cc = c;
	if (fwrite_unlocked(&cc, 1, 1, stream) != 1) {
		return EOF;
	}

	return c;
}


int putc(int c, FILE *stream)
{
	int ret;
	mutexLock(stream->lock);
	ret = putc_unlocked(c, stream);
	mutexUnlock(stream->lock);
	return ret;
}


int putchar_unlocked(int c)
{
	unsigned char cc = c;
	if (fwrite_unlocked(&cc, 1, 1, stdout) != 1) {
		return EOF;
	}

	return c;
}


int putchar(int c)
{
	int ret;
	mutexLock(stdout->lock);
	ret = putchar_unlocked(c);
	mutexUnlock(stdout->lock);
	return ret;
}


static int ungetc_unlocked(int c, FILE *stream)
{
	if (c == EOF || stream->buffer == NULL) {
		return EOF;
	}

	/* flush the write buffer if currently writing */
	if ((stream->flags & F_WRITING) != 0) {
		if (__fflush_one(stream) < 0) {
			return EOF;
		}
		stream->flags &= ~F_WRITING;
		stream->bufpos = stream->bufeof = stream->bufsz;
	}

	if (stream->bufpos > 0) {
		stream->buffer[--stream->bufpos] = c;
	}
	else {
		return EOF;
	}

	stream->flags &= ~F_EOF;
	return c;
}


int ungetc(int c, FILE *stream)
{
	int ret;
	mutexLock(stream->lock);
	ret = ungetc_unlocked(c, stream);
	mutexUnlock(stream->lock);
	return ret;
}


int fputs_unlocked(const char *s, FILE *stream)
{
	size_t len = strlen(s);
	size_t wrote = 0, ret;

	while (wrote < len) {
		ret = fwrite_unlocked(s + wrote, 1, len - wrote, stream);
		if (ret == 0) {
			return EOF;
		}

		wrote += ret;
	}

	/* Truncation is ok, we could return zero instead */
	return (wrote < INT_MAX) ? (int)wrote : INT_MAX;
}


int fputs(const char *s, FILE *stream)
{
	int ret;
	mutexLock(stream->lock);
	ret = fputs_unlocked(s, stream);
	mutexUnlock(stream->lock);
	return ret;
}


int puts_unlocked(const char *s)
{
	int wrote = fputs_unlocked(s, stdout);

	if (wrote != EOF) {
		putchar_unlocked('\n');
	}

	return wrote;
}


int puts(const char *s)
{
	int ret;
	mutexLock(stdout->lock);
	ret = puts_unlocked(s);
	mutexUnlock(stdout->lock);
	return ret;
}


int ferror_unlocked(FILE *stream)
{
	return !!(stream->flags & F_ERROR);
}


int ferror(FILE *stream)
{
	return ferror_unlocked(stream);
}


void clearerr(FILE *stream)
{
	stream->flags &= ~(F_ERROR | F_EOF);
}


int getchar_unlocked(void)
{
	return getc_unlocked(stdin);
}


int getchar(void)
{
	int ret;
	mutexLock(stdin->lock);
	ret = getc_unlocked(stdin);
	mutexUnlock(stdin->lock);
	return ret;
}


void rewind(FILE *file)
{
	fseek(file, 0, SEEK_SET);
}


int fsetpos(FILE *stream, const fpos_t *pos)
{
	off_t off = (off_t)(*pos);
	return (fseeko(stream, off, SEEK_SET) >= 0) ? 0 : -1;
}


void __fpurge(FILE *stream)
{
}


ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream)
{
	int c;
	ssize_t ofs;
	size_t new_n;
	char *new_lineptr, *endptr, *ptr;

	if (lineptr == NULL || n == NULL || stream == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (*lineptr == NULL || *n == 0) {
		*n = 120;
		*lineptr = malloc(*n);
		if (*lineptr == NULL) {
			/* errno set by malloc */
			return -1;
		}
	}

	ptr = *lineptr;
	endptr = *lineptr + *n;

	for (;;) {
		if (ptr + 2 >= endptr) {
			ofs = ptr - *lineptr;

			/* Need to check for SSIZE_MAX - 1, because setting size
			 * from SSIZE_MAX - 1 to SSIZE_MAX would be not enough */
			if ((*n) >= SSIZE_MAX - 1) {
				return SET_ERRNO(-EOVERFLOW);
			}
			else if ((*n) >= SSIZE_MAX / 2) {
				new_n = SSIZE_MAX;
			}
			else {
				new_n = 2 * (*n);
			}

			/* No point in allocating less than 8 via malloc */
			if (new_n < 8u) {
				new_n = 8u;
			}

			new_lineptr = realloc(*lineptr, new_n);
			if (new_lineptr == NULL) {
				/* errno set by realloc */
				return -1;
			}

			ptr = new_lineptr + ofs;
			endptr = new_lineptr + new_n;

			*lineptr = new_lineptr;
			*n = new_n;
		}

		c = fgetc(stream);
		if (c < 0) {
			if (feof(stream) && ptr != *lineptr) {
				*ptr = '\0';
				return ptr - *lineptr;
			}

			return EOF;
		}

		*ptr++ = c;
		if (c == delim) {
			*ptr = '\0';
			return ptr - *lineptr;
		}
	}
}


ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
	return getdelim(lineptr, n, '\n', stream);
}


int feof_unlocked(FILE *stream)
{
	return !!(stream->flags & F_EOF);
}


int feof(FILE *stream)
{
	int ret;
	mutexLock(stream->lock);
	ret = feof_unlocked(stream);
	mutexUnlock(stream->lock);
	return ret;
}


int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{

	char *old_buf;
	size_t old_siz;
	int old_flags;

	mutexLock(stream->lock);

	old_buf = stream->buffer;
	old_flags = stream->flags;
	old_siz = stream->bufsz;

	stream->buffer = NULL;
	stream->bufsz = size;
	stream->flags &= ~(F_USRBUF & F_LINE);

	if (mode != _IONBF) {
		if (buffer != NULL) {
			stream->buffer = buffer;
			stream->flags |= F_USRBUF;
		}
		else if (old_siz != size) {
			stream->buffer = buffAlloc(size);
			if (stream->buffer == NULL) {
				stream->buffer = old_buf;
				stream->bufsz = old_siz;
				stream->flags = old_flags;
				mutexUnlock(stream->lock);
				return -1;
			}
		}

		if (mode == _IOLBF) {
			stream->flags |= F_LINE;
		}
	}

	if (!(old_flags & F_USRBUF) && old_siz != size) {
		buffFree(old_buf, old_siz);
	}

	mutexUnlock(stream->lock);
	return 0;
}


void setbuf(FILE *stream, char *buf)
{
	setvbuf(stream, buf, buf != NULL ? _IOFBF : _IONBF, BUFSIZ);
}


FILE *popen(const char *command, const char *mode)
{
	int fd[2], pid;
	popen_FILE *pf;

	if (pipe(fd) < 0) {
		return NULL;
	}

	if ((pf = calloc(1, sizeof(popen_FILE))) == NULL) {
		goto failed;
	}

	if ((pf->file.mode = string2mode(mode)) < 0) {
		goto failed;
	}

	if (mutexCreate(&pf->file.lock) < 0) {
		pf->file.lock = 0;
		goto failed;
	}

	pf->file.buffer = mmap(NULL, BUFSIZ, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
	if (pf->file.buffer == MAP_FAILED) {
		goto failed;
	}

	if ((pid = vfork()) < 0) {
		goto failed;
	}
	else if (!pid) {
		if (mode[0] == 'r') {
			dup2(fd[1], 1);
		}
		else {
			dup2(fd[0], 0);
		}

		close(fd[1]);
		close(fd[0]);

		execl("/bin/sh", "sh", "-c", command, NULL);
		exit(EXIT_FAILURE);
	}

	pf->pid = pid;
	pf->file.bufpos = pf->file.bufeof = 0;
	pf->file.bufsz = BUFSIZ;
	pf->file.flags = 0;

	if (mode[0] == 'r') {
		pf->file.fd = fd[0];
		close(fd[1]);
	}
	else {
		pf->file.fd = fd[1];
		close(fd[0]);
	}

	mutexLock(file_common.lock);
	LIST_ADD(&file_common.list, &pf->file);
	mutexUnlock(file_common.lock);

	return &pf->file;

failed:

	if (pf->file.lock) {
		resourceDestroy(pf->file.lock);
	}
	if (pf->file.buffer != MAP_FAILED) {
		munmap(pf->file.buffer, BUFSIZ);
	}
	free(pf);
	close(fd[0]);
	close(fd[1]);
	return NULL;
}


int pclose(FILE *file)
{
	popen_FILE *pf = (popen_FILE *)file;
	int stat;
	pid_t pid = pf->pid;

	fclose(file);

	while (waitpid(pid, &stat, 0) == -1) {
		if (errno != EINTR) {
			stat = -1;
			break;
		}
	}

	return stat;
}


void _file_init(void)
{
	mutexCreate(&file_common.lock);
	file_common.list = NULL;

	stdin = calloc(1, sizeof(FILE));
	stdout = calloc(1, sizeof(FILE));
	stderr = calloc(1, sizeof(FILE));

	stdin->fd = 0;
	stdout->fd = 1;
	stderr->fd = 2;

	stdin->buffer = buffAlloc(BUFSIZ);
	stdout->buffer = buffAlloc(BUFSIZ);
	stdin->bufsz = BUFSIZ;
	stdout->bufsz = BUFSIZ;

	stdin->bufeof = stdin->bufpos = BUFSIZ;
	mutexCreate(&stdin->lock);

	stdout->bufpos = 0;
	stdout->flags = F_WRITING;
	mutexCreate(&stdout->lock);

	stderr->buffer = NULL;
	stderr->bufsz = 0;
	stderr->flags = F_WRITING;
	mutexCreate(&stderr->lock);

	if (isatty(stdout->fd)) {
		stdout->flags |= F_LINE;
	}

	stdin->mode = O_RDONLY;
	stdout->mode = O_WRONLY;
	stderr->mode = O_WRONLY;

	LIST_ADD(&file_common.list, stdin);
	LIST_ADD(&file_common.list, stdout);
	LIST_ADD(&file_common.list, stderr);
}


void flockfile(FILE *file)
{
}


void funlockfile(FILE *file)
{
}


FILE *tmpfile(void)
{
	oid_t oid, dev;

	while (lookup("/dev/posix/tmpfile", &oid, &dev) < 0) {
		if (errno != EINTR) {
			return NULL;
		}
	}

	/* Make sure it's a device (created by posixsrv) */
	if (oid.port == dev.port) {
		return NULL;
	}

	return fopen("/dev/posix/tmpfile", "w+");
}


int remove(const char *path)
{
	return unlink(path);
}

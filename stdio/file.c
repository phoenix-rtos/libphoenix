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

#include <arch.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>


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


static int string2mode(const char *mode)
{
	int next_char = 1;

	if (mode[0] == 'r') {
		if (mode[next_char] == 'b')
			next_char += 1;

		if (mode[next_char] == '\0')
			return O_RDONLY;
		else if (mode[next_char] == '+')
			return O_RDWR;
		else if (mode[next_char] == 'c')
			/* glibc extension - ignored */
			return O_RDONLY;
		else
			return -1;
	}
	else if (mode[0] == 'w') {
		if (mode[next_char] == 'b')
			next_char += 1;

		if (mode[next_char] == '\0')
			return O_WRONLY | O_CREAT | O_TRUNC;
		else if (mode[next_char] == '+')
			return O_RDWR | O_CREAT | O_TRUNC;
		else
			return -1;
	}
	else if (mode[0] == 'a') {
		if (mode[next_char] == 'b')
			next_char += 1;

		if (mode[next_char] == '\0')
			return O_WRONLY | O_CREAT | O_APPEND;
		else if (mode[next_char] == '+')
			return O_RDWR | O_CREAT | O_APPEND;
		else
			return -1;
	}

	return -1;
}


static void *buffAlloc(size_t size)
{
	void *ret;

#ifndef NOMMU
	if ((ret = mmap(NULL, (size + (_PAGE_SIZE - 1)) & ~(_PAGE_SIZE - 1), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, NULL, 0)) == MAP_FAILED)
		return NULL;
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
	if (file->buffer != NULL && !(file->flags & F_USRBUF))
		buffFree(file->buffer, file->bufsz);

	resourceDestroy(file->lock);
	free(file);
}


static ssize_t safe_read(int fd, void *buf, size_t size)
{
	int err;
	while ((err = read(fd, buf, size)) < 0 && errno == EINTR)
		;
	return err;
}


static ssize_t safe_write(int fd, const void *buf, size_t size)
{
	int err;
	while ((err = write(fd, buf, size)) < 0 && errno == EINTR)
		;
	return err;
}


static int safe_open(const char *path, int oflag, mode_t mode)
{
	int err;
	while ((err = open(path, oflag, mode)) < 0 && errno == EINTR)
		;
	return err;
}


static int safe_close(int fd)
{
	int err;
	while ((err = close(fd)) < 0 && errno == EINTR)
		;
	return err;
}


int fclose(FILE *file)
{
	int err;

	if (file == NULL)
		return -EINVAL;

	fflush(file);
	while ((err = safe_close(file->fd)) < 0 && errno == EINTR)
		;
	file_free(file);

	return err;
}


FILE *fopen(const char *filename, const char *mode)
{
	int m;
	FILE *f;
	int fd;

	if (filename == NULL || mode == NULL)
		return NULL;

	if ((m = string2mode(mode)) < 0)
		return NULL;

	if ((fd = safe_open(filename, m, DEFFILEMODE)) < 0)
		return NULL;

	if ((f = calloc(1, sizeof(FILE))) == NULL)
		return NULL;

	if ((f->buffer = buffAlloc(BUFSIZ)) == NULL) {
		safe_close(fd);
		free(f);
		return NULL;
	}

	mutexCreate(&f->lock);
	f->bufsz = BUFSIZ;
	f->fd = fd;
	f->mode = m;
	fflush(f);
	return f;
}


FILE *fdopen(int fd, const char *mode)
{
	int m, fdm;
	FILE *f;

	if (mode == NULL) {
		errno = EINVAL;
		return NULL;
	}

	if ((m = string2mode(mode)) < 0) {
		errno = EINVAL;
		return NULL;
	}

	if ((fdm = fcntl(fd, F_GETFL)) < 0)
		return NULL;

	/* POSIX: check if mode argument is allowed by the file access mode of the FD (not necessarily exactly the same) */
	fdm &= 0x7;
	if ((fdm != O_RDWR) && (fdm != (m & 0x7))) {
		errno = EINVAL;
		return NULL;
	}

	if ((f = calloc(1, sizeof(FILE))) == NULL)
		return NULL;

	if ((f->buffer = buffAlloc(BUFSIZ)) == NULL) {
		free(f);
		return NULL;
	}

	mutexCreate(&f->lock);
	f->bufsz = BUFSIZ;
	f->fd = fd;
	f->mode = m;
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
		safe_close(stream->fd);

		if ((m = string2mode(mode)) < 0) {
			file_free(stream);
			return NULL;
		}

		if ((stream->fd = safe_open(pathname, m, DEFFILEMODE)) < 0) {
			file_free(stream);
			return NULL;
		}

		stream->mode = m;
	}
	else {
		/* change mode */
	}

	return stream;
}


static int full_read(int fd, void *ptr, size_t size)
{
	int err;
	int total = 0;

	while (size) {
		if ((err = safe_read(fd, ptr, size)) < 0)
			return -1;
		else if (!err)
			return total;
		ptr += err;
		total += err;
		size -= err;
	}

	return total;
}


static int full_write(int fd, const void *ptr, size_t size)
{
	int err;
	while (size) {
		if ((err = safe_write(fd, ptr, size)) < 0)
			return -1;
		ptr += err;
		size -= err;
	}

	return size;
}


size_t fread_unlocked(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int err;
	size_t readsz = nmemb * size;
	size_t bytes;

	if (!readsz)
		return 0;

	if (stream->buffer == NULL) {
		/* unbuffered read */
		if ((err = full_read(stream->fd, ptr, readsz)) < 0)
			return 0;

		return err / size;
	}

	/* flush write buffer if writing */
	if (stream->flags & F_WRITING) {
		if (fflush_unlocked(stream) < 0)
			return 0;

		stream->flags &= ~F_WRITING;
		stream->bufpos = stream->bufeof = stream->bufsz;
	}

	if (stream->mode & O_WRONLY) {
		set_errno(-EBADF);
		stream->flags |= F_ERROR;
		return 0;
	}

	/* refill read buffer */
	if (stream->bufpos == stream->bufeof) {
		if ((err = safe_read(stream->fd, stream->buffer, stream->bufsz)) == -1)
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
	if ((err = full_read(stream->fd, ptr, readsz)) != -1) {
		bytes += err;

		if (err < readsz)
			stream->flags |= F_EOF;
	}
	else {
		stream->flags |= F_ERROR;
		return 0;
	}

	return bytes / size;
}


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t ret;
	mutexLock(stream->lock);
	ret = fread_unlocked(ptr, size, nmemb, stream);
	mutexUnlock(stream->lock);
	return ret;
}


size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int err;
	size_t writesz = nmemb * size;
	char *nl;

	if (!writesz)
		return 0;

	if (stream->buffer == NULL) {
		/* unbuffered write */
		if ((err = safe_write(stream->fd, ptr, writesz)) < 0)
			return 0;

		return err / size;
	}

	/* discard reading buffer */
	if (!(stream->flags & F_WRITING)) {
		fflush_unlocked(stream);
		stream->flags |= F_WRITING;
		stream->bufpos = 0;
	}

	if (stream->mode & O_RDONLY) {
		set_errno(-EBADF);
		stream->flags |= F_ERROR;
		return 0;
	}

	/* write to buffer if fits */
	if ((stream->bufsz - stream->bufpos) > writesz) {
		memcpy(stream->buffer + stream->bufpos, ptr, writesz);
		stream->bufpos += writesz;

		if ((stream->flags & F_LINE) && (nl = memrchr(stream->buffer, '\n', stream->bufpos)) != NULL) {
			if ((err = full_write(stream->fd, stream->buffer, nl - stream->buffer + 1)) < 0)
				return 0;

			stream->bufpos -= nl - stream->buffer + 1;
			memmove(stream->buffer, nl + 1, stream->bufpos);
		}

		return nmemb;
	}

	/* flush buffer and write to file */
	if ((err = full_write(stream->fd, stream->buffer, stream->bufpos)) == -1)
		return 0;

	stream->bufpos = 0;

	if ((err = full_write(stream->fd, ptr, writesz)) == -1)
		return 0;

	return nmemb;
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

	if (fread_unlocked(&cc, 1, 1, stream) != 1)
		return EOF;

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
	if (fwrite_unlocked(&cc, 1, 1, stream) != 1)
		return EOF;

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
		if (c == '\n' || i == n - 1)
			break;
	}

	if (i)
		str[i] = 0;
	else
		return NULL;

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


int fflush_unlocked(FILE *stream)
{
	if (stream == NULL) {
		fflush_unlocked(stdout);
		return 0; /* TODO: flush all FILE's */
	}

	if (stream->flags & F_WRITING) {
		if (stream->bufpos) {
			full_write(stream->fd, stream->buffer, stream->bufpos);
			stream->bufpos = 0;
		}
	}
	else {
		lseek(stream->fd, (off_t)stream->bufpos - stream->bufeof, SEEK_CUR);
		stream->bufpos = stream->bufeof = stream->bufsz;
	}

	return 0;
}


int fflush(FILE *stream)
{
	int ret;

	if (stream == NULL)
		return fflush(stdout);

	mutexLock(stream->lock);
	ret = fflush_unlocked(stream);
	mutexUnlock(stream->lock);
	return ret;
}


int fseek_unlocked(FILE *stream, long offset, int whence)
{
	fflush_unlocked(stream);
	stream->flags &= ~F_EOF;
	return lseek(stream->fd, offset, whence);
}


int fseek(FILE *stream, long offset, int whence)
{
	int ret;
	mutexLock(stream->lock);
	ret = fseek_unlocked(stream, offset, whence);
	mutexUnlock(stream->lock);
	return ret >= 0 ? 0 : -1;
}


long int ftell_unlocked(FILE *stream)
{
	return (long)fseek_unlocked(stream, 0, SEEK_CUR);
}


long int ftell(FILE *stream)
{
	long int ret;
	mutexLock(stream->lock);
	ret = ftell_unlocked(stream);
	mutexUnlock(stream->lock);
	return ret;
}


off_t ftello_unlocked(FILE *stream)
{
	return (off_t)fseek_unlocked(stream, 0, SEEK_CUR);
}


off_t ftello(FILE *stream)
{
	off_t ret;
	mutexLock(stream->lock);
	ret = ftello_unlocked(stream);
	mutexUnlock(stream->lock);
	return ret;
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
	if (fwrite_unlocked(&cc, 1, 1, stream) != 1)
		return EOF;

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
	if (fwrite_unlocked(&cc, 1, 1, stdout) != 1)
		return EOF;

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


int ungetc_unlocked(int c, FILE *stream)
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


int ungetc(int c, FILE *stream)
{
	int ret;
	mutexLock(stream->lock);
	ret = ungetc_unlocked(c, stream);
	mutexUnlock(stream->lock);
	return ret;
}


int puts_unlocked(const char *s)
{
	int len = strlen(s), l = 0, err;

	while (l < len) {
		if ((err = fwrite_unlocked((void *)s + l, 1, len - l, stdout)) < 0)
			return -1;
		l += err;
	}
	putchar_unlocked('\n');

	return l;
}


int puts(const char *s)
{
	int ret;
	mutexLock(stdout->lock);
	ret = puts_unlocked(s);
	mutexUnlock(stdout->lock);
	return ret;
}


int fputs_unlocked(const char *s, FILE *stream)
{
	int len = strlen(s);
	return fwrite_unlocked(s, 1, len, stream);
}


int fputs(const char *s, FILE *stream)
{
	int ret;
	mutexLock(stream->lock);
	ret = fputs_unlocked(s, stream);
	mutexUnlock(stream->lock);
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
	stream->flags &= ~F_ERROR;
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
	char *tmp;

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
		return -1;  // EOF

	if (!nl)
		linesz++;

	if (*lineptr == NULL)
		*lineptr = malloc(linesz);
	else if (*n < linesz) {
		if ((tmp = realloc(*lineptr, linesz)) == NULL) {
			free(*lineptr);
		}
		*lineptr = tmp;
	}
	if (*lineptr == NULL) {
		return -ENOMEM;
	}

	*n = linesz - 1;

	fseek(stream, offs, SEEK_SET);
	fread(*lineptr, 1, linesz - 1, stream);

	if (linesz > 0)
		(*lineptr)[linesz - 1] = 0;

	return linesz - 1;
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

		if (mode == _IOLBF)
			stream->flags |= F_LINE;
	}

	if (!(old_flags & F_USRBUF) && old_siz != size)
		buffFree(old_buf, old_siz);

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

	if (pipe(fd) < 0)
		return NULL;

	if ((pf = calloc(1, sizeof(popen_FILE))) == NULL)
		goto failed;

	if ((pf->file.mode = string2mode(mode)) < 0)
		goto failed;

	if (mutexCreate(&pf->file.lock) < 0) {
		pf->file.lock = 0;
		goto failed;
	}

	if ((pf->file.buffer = mmap(NULL, BUFSIZ, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, NULL, 0)) == MAP_FAILED)
		goto failed;

	if ((pid = vfork()) < 0) {
		goto failed;
	}
	else if (!pid) {
		if (mode[0] == 'r')
			dup2(fd[1], 1);
		else
			dup2(fd[0], 0);

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

	return &pf->file;

failed:

	if (pf->file.lock)
		resourceDestroy(pf->file.lock);
	if (pf->file.buffer != MAP_FAILED)
		munmap(pf->file.buffer, BUFSIZ);
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

	if (isatty(stdout->fd))
		stdout->flags |= F_LINE;

	stdin->mode = O_RDONLY;
	stdout->mode = O_WRONLY;
	stderr->mode = O_WRONLY;
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
		if (errno != EINTR)
			return NULL;
	}

	/* Make sure it's a device (created by posixsrv) */
	if (oid.port == dev.port)
		return NULL;

	return fopen("/dev/posix/tmpfile", "w+");
}


int remove(const char *path)
{
	return unlink(path);
}

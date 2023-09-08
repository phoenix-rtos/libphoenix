/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdio.h
 *
 * Copyright 2017-2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Kamil Amanowicz, Michal Miroslaw, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_STDIO_H_
#define _LIBPHOENIX_STDIO_H_


#include <arch.h>
#include <stddef.h>
#include <stdarg.h>
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


#ifndef NOMMU
#define BUFSIZ _PAGE_SIZE
#else
#define BUFSIZ 80
#endif

#define EOF (-1)
#define FILENAME_MAX 128

#define _IOFBF 0x2000
#define _IOLBF 0x4000
#define _IONBF 0x8000

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

#define FOPEN_MAX 16
#define L_ctermid 11 /* Fit at least /dev/pts/XX */
#define L_tmpnam  16
#define TMP_MAX   32767

typedef offs_t fpos_t;


typedef struct _FILE {
	int fd;
	unsigned flags;

	int mode;
	size_t bufeof;
	size_t bufpos;
	size_t bufsz;
	char *buffer;
	handle_t lock;

	struct _FILE *next;
	struct _FILE *prev;
} FILE;


typedef struct _DIR {
	oid_t oid;
	size_t pos;
	struct dirent *dirent;
} DIR;


typedef struct _IO_cookie_io_functions
{
	void *read;
	void *write;
	void *seek;
	void *close;
} cookie_io_functions_t;


extern FILE *stderr, *stdin, *stdout;

#define stderr stderr
#define stdin stdin
#define stdout stdout

/* Closes the stream. All buffers are flushed. */
extern int fclose(FILE *stream);


/* Clears the end-of-file and error indicators for the given stream. */
extern void clearerr(FILE *stream);


/* Tests the end-of-file indicator for the given stream. */
extern int feof_unlocked(FILE *stream);


/* Tests the end-of-file indicator for the given stream. */
extern int feof(FILE *stream);


/* Tests the error indicator for the given stream. */
extern int ferror(FILE *stream);
extern int ferror_unlocked(FILE *stream);


/* Flushes the output buffer of a stream. */
extern int fflush(FILE *stream);
extern int fflush_unlocked(FILE *stream);


/* Gets the current file position of the stream and writes it to pos. */
extern int fgetpos(FILE *stream, fpos_t *pos);


/* Opens the filename pointed to by filename using the given mode. */
extern FILE *fopen(const char *filename, const char *mode);
extern FILE *fopen_unlocked(const char *filename, const char *mode);


/* Associates a stream with existing file descriptor fd. */
extern FILE *fdopen(int fd, const char *mode);


/* Reads data from the given stream into the array pointed to by ptr. */
extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern size_t fread_unlocked(void *ptr, size_t size, size_t nmemb, FILE *stream);


/* Associates a new filename with the given open stream and same time closing the old file in stream. */
extern FILE *freopen(const char *filename, const char *mode, FILE *stream);

/* examines the argument stream and returns its integer file descriptor */
extern int fileno(FILE *stream);
extern int fileno_unlocked(FILE *stream);

/*
 * Sets the file position of the stream to the given offset. The argument offset signifies the number of bytes
 * to seek from the given whence position.
 */
extern int fseek(FILE *stream, long int offset, int whence);
extern int fseek_unlocked(FILE *stream, long int offset, int whence);
extern int fseeko(FILE *stream, off_t offset, int whence);
extern int fseeko_unlocked(FILE *stream, off_t offset, int whence);


/*
 * Sets the file position of the given stream to the given position. The argument pos is a position given by
 * the function fgetpos.
 */
extern int fsetpos(FILE *stream, const fpos_t *pos);


/* Returns the current file position of the given stream. */
extern long int ftell(FILE *stream);
extern long int ftell_unlocked(FILE *stream);
extern off_t ftello(FILE *stream);
extern off_t ftello_unlocked(FILE *stream);


/* Writes data from the array pointed to by ptr to the given stream. */
extern size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
extern size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream);


/* Deletes the given filename so that it is no longer accessible. */
extern int remove(const char *filename);


/* Causes the filename referred to, by old_filename to be changed to new_filename. */
extern int rename(const char *old_filename, const char *new_filename);


/* Sets the file position to the beginning of the file of the given stream. */
extern void rewind(FILE *stream);


/* Defines how a stream should be buffered. */
extern void setbuf(FILE *stream, char *buffer);


/* Another function to define how a stream should be buffered. */
extern int setvbuf(FILE *stream, char *buffer, int mode, size_t size);


/* Creates a temporary file in binary update mode (wb+). */
extern FILE *tmpfile(void);


/* Generates and returns a valid temporary filename which does not exist. */
extern char *tmpnam(char *str);


/* Sends formatted output to a stream. */
extern int fprintf(FILE *stream, const char *format, ...);


/* Sends formatted output to a file descriptor. */
extern int dprintf(int fd, const char *format, ...);


/* Sends formatted output to stdout. */
extern int printf(const char *format, ...);


/* Sends formatted output to a string. */
extern int sprintf(char *str, const char *format, ...);


/* Sends formatted output to a string. */
extern int snprintf(char *str, size_t n, const char *format, ...);


/* Sends formatted output to a stream using an argument list. */
extern int vfprintf(FILE *stream, const char *format, va_list arg);


/* Sends formatted output to a file descriptor using an argument list. */
extern int vdprintf(int fd, const char *format, va_list arg);


/* Sends formatted output to stdout using an argument list. */
extern int vprintf(const char *format, va_list arg);


/* Sends formatted output to a string using an argument list. */
extern int vsprintf(char *str, const char *format, va_list arg);


/* Sends formatted output to a string using an argument list. */
extern int vsnprintf(char *str, size_t n, const char *format, va_list arg);


/* Print formatted output to allocated string.  */
extern int asprintf(char **strp, const char *fmt, ...);


/* Print formatted output to allocated string using an argument list. */
extern int vasprintf(char **strp, const char *fmt, va_list ap);


/* Reads formatted input from a stream using an argument list. */
extern int vfscanf(FILE *stream, const char *format, va_list ap);


/* Reads formatted input from a string using an argument list. */
extern int vsscanf(const char *str, const char *format, va_list ap);


/* Reads formatted input from stdin using an argument list. */
extern int vscanf(const char *format, va_list ap);


/* Reads formatted input from a stream. */
extern int fscanf(FILE *stream, const char *format, ...);


/* Reads formatted input from stdin. */
extern int scanf(const char *format, ...);


/* Reads formatted input from a string. */
extern int sscanf(const char *str, const char *format, ...);


/* Gets the next character (an unsigned char) from the specified stream and advances the position indicator for the stream. */
extern int fgetc(FILE *stream);


/*
 * Reads a line from the specified stream and stores it into the string pointed to by str. It stops when either (n-1) characters are read,
 * the newline character is read, or the end-of-file is reached, whichever comes first.
 */
extern char *fgets(char *str, int n, FILE *stream);
extern char *fgets_unlocked(char *s, int n, FILE *stream);


/*
 * Writes a character (an unsigned char) specified by the argument char to the specified stream and advances the position
 * indicator for the stream.
 */
extern int fputc(int c, FILE *stream);
extern int fputc_unlocked(int c, FILE *stream);


/* Writes a string to the specified stream up to but not including the null character. */
extern int fputs(const char *str, FILE *stream);
extern int fputs_unlocked(const char *s, FILE *stream);


/* Gets the next character (an unsigned char) from the specified stream and advances the position indicator for the stream. */
extern int getc(FILE *stream);
extern int getc_unlocked(FILE *stream);


/* Gets a character (an unsigned char) from stdin. */
extern int getchar(void);
extern int getchar_unlocked(void);


/*
 * Reads a line from stdin and stores it into the string pointed to by, str. It stops when either the newline character is read
 * or when the end-of-file is reached, whichever comes first.
 */
extern char *gets(char *str);


/*
 * Writes a character (an unsigned char) specified by the argument char to the specified stream and advances the position indicator
 * for the stream.
 */
extern int putc(int c, FILE *stream);
extern int putc_unlocked(int c, FILE *stream);


/* Writes a character (an unsigned char) specified by the argument char to stdout. */
extern int putchar(int c);
extern int putchar_unlocked(int c);


/* Writes a string to stdout up to but not including the null character. A newline character is appended to the output. */
extern int puts(const char *str);


/* Pushes the character char (an unsigned char) onto the specified stream so that the next character is read. */
extern int ungetc(int c, FILE *stream);
extern int ungetc_unlocked(int c, FILE *stream);


/* Prints a descriptive error message to stderr. First the string str is printed followed by a colon and then a space. */
extern void perror(const char *str);


extern FILE *popen(const char *command, const char *type);


extern int pclose(FILE *stream);


/* Reads a string input delimited by a user-specified character. */
extern ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream);


/* Reads a string separated by a newline character. */
extern ssize_t getline(char **lineptr, size_t *n, FILE *stream);


extern void _file_init(void);


/* stdio locking functions */
extern void flockfile(FILE *stream);
extern void funlockfile(FILE *stream);


#ifdef __cplusplus
}
#endif


#endif

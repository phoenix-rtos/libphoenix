/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdio.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk, Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_STDIO_H_
#define _LIBPHOENIX_STDIO_H_


#include ARCH
#include "stdarg.h"

#define EOF (-1)

typedef offs_t fpos_t;


typedef struct _FILE {
	oid_t oid;
	char *buff;
	size_t buffsz;
	size_t pos;
} FILE;


typedef struct _DIR {
	oid_t oid;
	size_t pos;
	struct dirent *dirent;
} DIR;


extern FILE *stderr, *stdin, *stdout;


/* Closes the stream. All buffers are flushed. */
extern int fclose(FILE *file);


/* Clears the end-of-file and error indicators for the given stream. */
extern void clearerr(FILE *stream);


/* Tests the end-of-file indicator for the given stream. */
extern int feof(FILE *stream);


/* Tests the error indicator for the given stream. */
extern int ferror(FILE *stream);


/* Flushes the output buffer of a stream. */
extern int fflush(FILE *stream);


/* Gets the current file position of the stream and writes it to pos. */
extern int fgetpos(FILE *stream, fpos_t *pos);


/* Opens the filename pointed to by filename using the given mode. */
extern FILE *fopen(const char *filename, const char *mode);


/* Reads data from the given stream into the array pointed to by ptr. */
extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);


/* Associates a new filename with the given open stream and same time closing the old file in stream. */
extern FILE *freopen(const char *filename, const char *mode, FILE *stream);


/*
 * Sets the file position of the stream to the given offset. The argument offset signifies the number of bytes
 * to seek from the given whence position.
 */
extern int fseek(FILE *stream, long int offset, int whence);


/*
 * Sets the file position of the given stream to the given position. The argument pos is a position given by
 * the function fgetpos.
 */
extern int fsetpos(FILE *stream, const fpos_t *pos);


/* Returns the current file position of the given stream. */
extern long int ftell(FILE *stream);


/* Writes data from the array pointed to by ptr to the given stream. */
extern size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);


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


/* Sends formatted output to stdout. */
extern int printf(const char *format, ...);


/* Sends formatted output to a string. */
extern int sprintf(char *str, const char *format, ...);


/* Sends formatted output to a string. */
extern int snprintf(char *str, size_t n, const char *format, ...);


/* Sends formatted output to a stream using an argument list. */
extern int vfprintf(FILE *stream, const char *format, va_list arg);


/* Sends formatted output to stdout using an argument list. */
extern int vprintf(const char *format, va_list arg);


/* Sends formatted output to a string using an argument list. */
extern int vsprintf(char *str, const char *format, va_list arg);


/* Sends formatted output to a string using an argument list. */
extern int vsnprintf(char *str, size_t n, const char *format, va_list arg);


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


/*
 * Writes a character (an unsigned char) specified by the argument char to the specified stream and advances the position
 * indicator for the stream.
 */
extern int fputc(int c, FILE *stream);


/* Writes a string to the specified stream up to but not including the null character. */
extern int fputs(const char *str, FILE *stream);


/* Gets the next character (an unsigned char) from the specified stream and advances the position indicator for the stream. */
extern int getc(FILE *stream);


/* Gets a character (an unsigned char) from stdin. */
extern int getchar(void);


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


/* Writes a character (an unsigned char) specified by the argument char to stdout. */
extern int putchar(int c);


/* Writes a string to stdout up to but not including the null character. A newline character is appended to the output. */
extern int puts(const char *str);


/* Pushes the character char (an unsigned char) onto the specified stream so that the next character is read. */
extern int ungetc(int c, FILE *stream);


/* Prints a descriptive error message to stderr. First the string str is printed followed by a colon and then a space. */
extern void perror(const char *str);


#endif

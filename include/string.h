/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * string.h
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_STRING_H_
#define _LIBPHOENIX_STRING_H_


#include <stddef.h>
#include <stdint.h>


/* Included for compability reason */
#include <strings.h>


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Searches for the first occurrence of the character c (an unsigned char) in the first n bytes of the string
 * pointed to, by the argument str.
 */
extern void *memchr(const void *str, int c, size_t n);

/* Searches backward from the end of the n bytes pointed to by s instead of forward from the beginning. */
extern void *memrchr(const void *s, int c, size_t n);


/* Compares the first n bytes of str1 and str2. */
extern int memcmp(const void *str1, const void *str2, size_t n);


/* Copies n characters from src to dest. - defined in stddef.h */
extern void *memcpy(void *dest, const void *src, size_t n);


/* Copies n characters from src to dest. Return from dest + n. */
extern void *mempcpy(void *dest, const void *src, size_t n);


/* Another function to copy n characters from str2 to str1. */
extern void *memmove(void *dest, const void *src, size_t n);


/* Copies the character c (an unsigned char) to the first n characters of the string pointed to, by the argument str. */
extern void *memset(void *str, int c, size_t n);


/* Appends the string pointed to, by src to the end of the string pointed to by dest. */
extern char *strcat(char *dest, const char *src);


/* Appends the string pointed to, by src to the end of the string pointed to, by dest up to n characters long. */
extern char *strncat(char *dest, const char *src, size_t n);


/* Searches for the first occurrence of the character c (an unsigned char) in the string pointed to, by the argument str. */
extern char *strchr(const char *str, int c);


/* Compares the string pointed to, by str1 to the string pointed to by str2. */
extern int strcmp(const char *str1, const char *str2);


/* Compares at most the first n bytes of str1 and str2. */
extern int strncmp(const char *str1, const char *str2, size_t n);


/* Compares string str1 to str2. The result is dependent on the LC_COLLATE setting of the location. */
extern int strcoll(const char *str1, const char *str2);


/* Copies the string pointed to by src to dest. Returns a pointer to the destination string dest */
extern char *strcpy(char *dest, const char *src);


/* Copies the string pointed to by src to dest. Returns a pointer to the end of the string dest */
extern char *stpcpy(char *dest, const char *src);

/* Copies up to n characters from the string pointed to, by src to dest. */
extern char *strncpy(char *dest, const char *src, size_t n);


/*  copies up to size - 1 characters from the NUL-terminated string src to dst, NUL-terminating the result */
extern size_t strlcpy(char *dst, const char *src, size_t size);


/* appends the NUL-terminated string src to the end of dst. It will append at most size - strlen(dst) - 1 bytes, NUL-terminating the result. */
extern size_t strlcat(char *dst, const char *src, size_t size);


/* Calculates the length of the initial segment of str1 which consists entirely of characters not in str2. */
extern size_t strcspn(const char *str1, const char *str2);


/* Searches an internal array for the error number errnum and returns a pointer to an error message string. */
extern char *strerror(int errnum);


/* Thread-safe strerror. */
extern int strerror_r(int errnum, char *buf, size_t buflen);


/* Computes the length of the string str up to but not including the terminating null character. */
extern size_t strlen(const char *str);


/* Computes the length of the string str up to but not including the terminating null character, but checks at most maxlen */
extern size_t strnlen(const char *str, size_t maxlen);


/* Finds the first character in the string str1 that matches any character specified in str2. */
extern char *strpbrk(const char *str1, const char *str2);


/* Searches for the last occurrence of the character c (an unsigned char) in the string pointed to by the argument str. */
extern char *strrchr(const char *str, int c);


/* Calculates the length of the initial segment of str1 which consists entirely of characters in str2. */
extern size_t strspn(const char *str1, const char *str2);


/*
 * Finds the first occurrence of the entire string needle (not including the terminating null character)
 * which appears in the string haystack.
 */
extern char *strstr(const char *haystack, const char *needle);


/* Like strstr, but ignores case of arguments */
extern char *strcasestr(const char *haystack, const char *needle);


extern char *strsep(char **string_ptr, const char *delimiter);


/* Breaks string str into a series of tokens separated by delim. */
extern char *strtok(char *str, const char *delim);


/* Transforms the first n characters of the string src into corrent locale and places them in the string dest. */
extern size_t strxfrm(char *dest, const char *src, size_t n);


extern char *strchrnul(const char *str, int z);


extern char *strchr(const char *str, int z);


extern char *strdup(const char *s1);


extern char *strndup(const char *s, size_t n);


extern int strverscmp(const char *s1, const char *s2);


extern char *strsignal(int sig);


__attribute__((__always_inline__)) static inline void bcopy(const void *src, void *dest, size_t n)
{
	memmove(dest, src, n);
}


__attribute__((__always_inline__)) static inline void bzero(void *p, size_t sz)
{
	memset(p, 0, sz);
}


extern int strcasecmp(const char *str1, const char *str2);


#ifdef __cplusplus
}
#endif


#endif

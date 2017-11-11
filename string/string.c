/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * string/string (generic implementation of string functions)
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_STRING_H_
#define _LIBPHOENIX_ARCH_STRING_H_

#include ARCH

#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>


struct {
	char *next_token;
} string_common;


#ifndef __STRCMP
#define __STRCMP
int strcmp(const char *s1, const char *s2)
{
	const char *p;
	unsigned int k;

	for (p = s1, k = 0; *p; p++, k++) {

		if (*p < *(s2 + k))
			return -1;
		else if (*p > *(s2 + k))
			return 1;
	}

	if (*p != *(s2 + k))
		return -1;

	return 0;
}
#endif


#ifndef __STRCASECMP
#define __STRCASECMP
int strcasecmp(const char *s1, const char *s2)
{
	const char *p;
	unsigned int k;

	for (p = s1, k = 0; *p; p++, k++) {

		if (tolower(*p) < tolower(*(s2 + k)))
			return -1;
		else if (tolower(*p) > tolower(*(s2 + k)))
			return 1;
	}

	if (tolower(*p) != tolower(*(s2 + k)))
		return -1;

	return 0;
}
#endif


#ifndef __STRNCASECMP
#define __STRNCASECMP
int strncasecmp(const char *s1, const char *s2, int n)
{
	const char *p;
	unsigned int k;

	for (p = s1, k = 0; *p && k < n; p++, k++) {

		if (tolower(*p) < tolower(*(s2 + k)))
			return -1;
		else if (tolower(*p) > tolower(*(s2 + k)))
			return 1;
	}

	if (tolower(*p) != tolower(*(s2 + k)))
		return -1;

	return 0;
}
#endif


/* FIXME it should return -K or +K or 0  */
#ifndef __STRNCMP
#define __STRNCMP
int strncmp(const char *s1, const char *s2, unsigned int count)
{
	const char *p;
	unsigned int k = 0;

	for (p = s1; *p; p++) {
		if ((*p != *(s2 + k)) && (k < count))
			return -1;
		k++;
	}

	if (k < count)
		return -1;

	return 0;
}
#endif


#ifndef __STRCHR
#define __STRCHR
char *strchr(const char *str, int z)
{
	do {
		if (*str == z)
			return (char *)str;
	}
	while (*(str++));

	return NULL;
}
#endif


#ifndef __MEMCHR
#define __MEMCHR
void *memchr(const void *s, int c, size_t n)
{
	int i;

	for (i = 0; i < n; ++i, ++s) {
		if (*(char *)s == c)
			return (void *)s;
	}

	return NULL;
}
#endif


#ifndef __STRCHRNUL
#define __STRCHRNUL
char *strchrnul(const char *str, int z)
{
	while (*str && *(str++) != z);
	return (char *)str;
}
#endif


#ifndef __MEMCMP
#define __MEMCMP
int memcmp(const void *s1, const void *s2, size_t count)
{
	int res;
	while (count--)
		if ((res = *((char *)s1++) - *((char *)s2++)))
			return res < 0 ? -1 : 1;

	return 0;
}
#endif


#ifndef __STRLEN
#define __STRLEN
size_t strlen(const char *s)
{
	unsigned int k;

	for (k = 0; *s; s++, k++);
	return k;
}
#endif


#ifndef __STRNLEN
#define __STRNLEN
size_t strnlen(const char *s, size_t maxlen)
{
	unsigned int k;

	for (k = 0; k < maxlen && *s != '\0'; s++, k++);
	return k;
}
#endif


#ifndef __STRCPY
#define __STRCPY
char *strcpy(char *dest, const char *src)
{
	unsigned int i = 0;

	do {
		dest[i] = src[i];
	} while (src[i++] != '\0');

	return dest;
}
#endif


#ifndef __STRNCPY
#define __STRNCPY
char *strncpy(char *dest, const char *src, size_t n)
{
	int i = 0;

	do {
		dest[i] = src[i];
		i++;
	} while (i < n && src[i - 1] != '\0');

	return dest;
}
#endif


#ifndef __STRPCPY
#define __STRPCPY
char *stpcpy(char *dest, const char *src)
{
	do
		*(dest++) = *(src++);
	while (*src);

	*dest = 0;

	return dest;
}
#endif


#ifndef __MEMMOVE
#define __MEMMOVE
void *memmove(void *dest, const void *src, size_t n)
{
	int i;

	if (dest < src)
		for (i = 0; i < n; ++i)
			((char *)dest)[i] = ((const char *)src)[i];
	else
		for (i = n; i-- > 0; )
			((char *)dest)[i] = ((const char *)src)[i];
	return dest;
}
#endif


#ifndef __STRCSPN
#define __STRCSPN
size_t strcspn(const char *s1, const char *s2)
{
	int count;
	const char *p;

	for (count = 0; *s1; ++count, ++s1) {
		for (p = s2; *p; ++p) {
			if (*s1 == *p)
				return count;
		}
	}

	return count;
}
#endif


#ifndef __STRPBRK
#define __STRPBRK
char *strpbrk(const char *s1, const char *s2)
{
	const char *p;

	for (; *s1; ++s1) {
		for (p = s2; *p; ++p) {
			if (*s1 == *p)
				return (char *)s1;
		}
	}

	return NULL;
}
#endif


#ifndef __STRRCHR
#define __STRRCHR
char *strrchr(const char *s, int c)
{
	const char *p = NULL;

	do {
		if (*s == c)
			p = s;
	}
	while (*(s++));

	return (char *)p;
}
#endif


char *strdup(const char *s1)
{
	int len;
	char *result;

	len = strlen(s1) + 1;
	result = malloc(len);
	memcpy(result, s1, len);

	return result;
}


char *strsignal(int signum)
{
	return NULL;
}


char *strerror(int errnum)
{
	return NULL;
}


char *strstr(const char *s1, const char *s2)
{
	const char *p1, *p2;

	for (; *s1; ++s1) {
		for (p1 = s1, p2 = s2; *p2 && *p1 && *p1 == *p2; ++p1, ++p2);

		if (!*p2)
			return (char *)s1;
	}

	return NULL;

}


size_t strspn(const char *s1, const char *s2)
{
	int count;
	const char *p;

	for (count = 0; *s1; ++count, ++s1) {
		for (p = s2; *p && *s1 != *p; ++p) ;
		if (!*p)
			return count;
	}

	return count;
}


char *strtok(char *s1, const char *s2)
{
	char *tokend;

	if (s1 != NULL)
		s1 += strspn(s1, s2);
	else
		s1 = string_common.next_token;

	if (!*s1)
		return NULL;

	tokend = s1 + strcspn(s1, s2);

	string_common.next_token = tokend + strspn(tokend, s2);

	*tokend = 0;

	return s1;
}


char *strcat(char *s1, const char *s2)
{
	int len = strlen(s1);
	return strcpy(s1 + len, s2) - len;
}


void *memrchr(const void *s, int c, size_t n)
{
	int i;
	for (i = 0; i < n; ++i) {
		if (((char *)s)[-i] == c)
			return (char *)s - i;
	}

	return NULL;
}


#endif

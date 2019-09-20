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

#include <ctype.h>
#include <stddef.h>
#include <string.h>


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

	return 0;
}
#endif


#ifndef __STRNCMP
#define __STRNCMP
int strncmp(const char *s1, const char *s2, size_t n)
{
	const char *p;
	size_t k = 0;

	for (p = s1, k = 0; *p && k < n; p++, k++) {
		if (*p < *(s2 + k))
			return -1;
		else if (*p > *(s2 + k))
			return 1;
	}

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
	while (*str && *str != z)
		str++;
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


#ifndef __STPCPY
#define __STPCPY
char *stpcpy(char *dest, const char *src)
{
	while (*src)
		*(dest++) = *(src++);

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


char *strsignal(int signum)
{
	switch (signum) {
	default:
		return "strsignal not implemented";
	}
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


char *strcasestr(const char *s1, const char *s2)
{
	const char *p1, *p2;

	for (; *s1; ++s1) {
		for (p1 = s1, p2 = s2; *p2 && *p1 && tolower(*p1) == tolower(*p2); ++p1, ++p2);

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

	if (s1 == NULL)
		s1 = string_common.next_token;
	s1 += strspn(s1, s2);

	if (!*s1) {
		string_common.next_token = s1;
		return NULL;
	}

	tokend = s1 + strcspn(s1, s2);

	if (*tokend) {
		*tokend = '\0';
		string_common.next_token = tokend + 1;
	} 
	else {
		string_common.next_token = tokend;
	}

	return s1;
}


char *strsep(char **string_ptr, const char *delimiter)
{
	char *ret = (*string_ptr);
	const char *p;

	if ((*string_ptr) == NULL)
		return NULL;

	for (; (**string_ptr) != '\0'; ++(*string_ptr)) {
		for (p = delimiter; (*p) != '\0' && (**string_ptr) != (*p); ++p)
			;

		if ((*p) != '\0')
			break;
	}

	if ((**string_ptr) == '\0') {
		(*string_ptr) = NULL;
	}
	else {
		(**string_ptr) = '\0';
		(*string_ptr)++;
	}

	return ret;
}


char *strcat(char *s1, const char *s2)
{
	int len = strlen(s1);
	return strcpy(s1 + len, s2) - len;
}


void *memrchr(const void *s, int c, size_t n)
{
	unsigned char *src = (unsigned char *)s + n - 1;
	while (n > 0) {
		if (*src == (unsigned char) c)
			return src;

		src -= 1;
		n -= 1;
	}

	return NULL;
}


void *mempcpy(void *dest, const void *src, size_t n)
{
	return memcpy(dest, src, n) + n;
}


int strverscmp(const char *s1, const char *s2)
{
	// TODO: implement proper algorithm
	return strcmp(s1, s2);
}

char *strncat(char *s1, const char *s2i, size_t num)
{
	return 0;
}

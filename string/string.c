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


#ifndef __STRLEN
#define __STRLEN
unsigned int strlen(const char *s)
{
	unsigned int k;
	
	for (k = 0; *s; s++, k++);	
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


#ifndef __MEMMOVE
#define __MEMMOVE
void memmove(void *dest, void *src, size_t n)
{
	int i;

	if (dest < src)
		for (i = 0; i < n; ++i)
			((char *)dest)[i] = ((char *)src)[i];
	else
		for (i = n; i > 0; --i)
			((char *)dest)[i] = ((char *)src)[i];
}
#endif


#endif

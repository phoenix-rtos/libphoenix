/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * arch/arm/multilib/string
 *
 * Copyright 2025 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <stddef.h>


void *memmove(void *dest, const void *src, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		((char *)dest)[i] = ((const char *)src)[i];
	}
	return dest;
}


int memcmp(const void *ptr1, const void *ptr2, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		int diff = ((int)(((const unsigned char *)ptr1)[i])) - ((int)(((const unsigned char *)ptr2)[i]));
		if (diff != 0) {
			return diff;
		}
	}
	return 0;
}


size_t strlen(const char *s)
{
	size_t len = 0;
	while (s[len] != '\0') {
		len++;
	}
	return len;
}


size_t strnlen(const char *s, size_t maxlen)
{
	size_t len = 0;
	while ((s[len] != '\0') && (len < maxlen)) {
		len++;
	}
	return len;
}


int strcmp(const char *s1, const char *s2)
{
	for (size_t i = 0; (s1[i] != '\0') || (s2[i] != '\0'); i++) {
		int diff = ((int)(((const unsigned char *)s1)[i])) - ((int)(((const unsigned char *)s2)[i]));
		if (diff != 0) {
			return diff;
		}
	}
	return 0;
}


int strncmp(const char *s1, const char *s2, size_t count)
{
	for (size_t i = 0; ((i < count) && ((s1[i] != '\0') || (s2[i] != '\0'))); i++) {
		int diff = ((int)(((const unsigned char *)s1)[i])) - ((int)(((const unsigned char *)s2)[i]));
		if (diff != 0) {
			return diff;
		}
	}
	return 0;
}


char *strcpy(char *dest, const char *src)
{
	size_t i = 0;
	while ((src[i] != '\0')) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return dest;
}


char *strncpy(char *dest, const char *src, size_t n)
{
	size_t i = 0;
	while (((i < n) && (src[i] != '\0'))) {
		dest[i] = src[i];
		i++;
	}
	while (i < n) {
		dest[i] = '\0';
		i++;
	}
	return dest;
}


void *memcpy(void *dest, const void *src, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		((char *)dest)[i] = ((const char *)src)[i];
	}
	return dest;
}


void *memset(void *dest, int value, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		((unsigned char *)dest)[i] = (unsigned char)value;
	}
	return dest;
}

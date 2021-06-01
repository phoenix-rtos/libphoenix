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

#include <string.h>
#include <stdlib.h>


char *strdup(const char *s1)
{
	int len;
	char *result;

	len = strlen(s1) + 1;
	result = malloc(len);
	if (result)
		memcpy(result, s1, len);

	return result;
}

char *strndup(const char *s, size_t n)
{
	int len;
	char *result;

	len = strlen(s);
	if (len > n)
		len = n;
	result = malloc(len + 1);
	if (result) {
		memcpy(result, s, len);
		result[len] = '\0';
	}

	return result;
}

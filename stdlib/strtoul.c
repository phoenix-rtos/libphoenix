/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * strtoul, strtol
 *
 * Copyright 2017-2018 Phoenix Systems
 * Author: Jakub Sejdak, Pawel Pisarczyk, Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stddef.h>
#include "ctype.h"


unsigned int strtoul(char *nptr, char **endptr, int base)
{
	unsigned int t, v = 0;

	if ((base == 16 || base == 0) && nptr[0] == '0' && (nptr[1] | 0x20) == 'x') {
		base = 16;
		nptr += 2;
	} else if (base == 0 && nptr[0] == '0')
		base = 8;
	else if (base == 0)
		base = 10;

	for (; isdigit(*nptr) || isalpha(*nptr); ++nptr) {
		t = *nptr - '0';
		if (t > 9)
			t = (*nptr | 0x20) - 'a' + 10;

		if (t >= (unsigned int)base)
			break;

		v = (v * base) + t;
	}

	if (endptr != NULL)
		*endptr = nptr;

	return v;
}


int strtol(char *nptr, char **endptr, int base)
{
	int sign = 1;

	if (*nptr == '-') {
		sign = -1;
		++nptr;
	}

	return sign * strtoul(nptr, endptr, base);
}


int atoi(char *str)
{
	return strtol(str, NULL, 10);
}


int atol(char *str)
{
	return strtol(str, NULL, 10);
}

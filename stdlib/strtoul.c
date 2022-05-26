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


unsigned long int strtoul(char *nptr, char **endptr, int base)
{
	unsigned long int t, v = 0;

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

		if (t >= (unsigned long int)base)
			break;

		v = (v * base) + t;
	}

	if (endptr != NULL)
		*endptr = nptr;

	return v;
}


long int strtol(char *nptr, char **endptr, int base)
{
	long int sign = 1;

	if (*nptr == '-') {
		sign = -1;
		++nptr;
	}

	return sign * strtoul(nptr, endptr, base);
}


int atoi(char *str)
{
	return (int)strtol(str, NULL, 10);
}


long int atol(char *str)
{
	return strtol(str, NULL, 10);
}

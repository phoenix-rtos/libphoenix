/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * strtoul, strtol
 *
 * Copyright 2017 Phoenix Systems
 * Author: Jakub Sejdak, Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH

#include "ctype.h"

unsigned int strtoul(char *nptr, char **endptr, int base)
{
	unsigned int t, v = 0;

	if (base == 16 && nptr[0] == '0' && nptr[1] == 'x')
		nptr += 2;

	if (*endptr != NULL)
		*endptr = nptr;

	while (isdigit(*nptr)) {
		t = *nptr - '0';
		if (t > 9) {
			if (base > 10) {
				t = (*nptr | 0x20) - 'a' + 10;
				if (t >= (unsigned int)base)
					break;
			}
			else {
				break;
			}
		}
		else if (t >= (unsigned int)base) {
			break;
		}

		v = (v * base) + t;

		if (*endptr != NULL)
			*endptr = nptr;

		++nptr;
	}

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

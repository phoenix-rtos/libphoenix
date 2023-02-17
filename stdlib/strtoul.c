/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * strtoul, strtol
 *
 * Copyright 2017-2018, 2023 Phoenix Systems
 * Author: Jakub Sejdak, Pawel Pisarczyk, Michal Miroslaw, Gerard Swiderski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>


unsigned long int strtoul(const char *nptr, char **endptr, int base)
{
	unsigned long int cutoff, result = 0;
	int cutlim, t, width = 0, negative = 0;
	const char *sptr = nptr;

	while (isspace(*sptr) != 0) {
		sptr++;
	}

	if (*sptr == '-') {
		negative = 1;
		sptr++;
	}
	else if (*sptr == '+') {
		sptr++;
	}

	if ((base == 16 || base == 0) && sptr[0] == '0' && (sptr[1] | 0x20) == 'x') {
		base = 16;
		sptr += 2;
	}
	else if (base == 0 && sptr[0] == '0') {
		base = 8;
	}
	else if (base == 0) {
		base = 10;
	}

	if (base < 2 || base > (11 + 'z' - 'a')) {
		errno = EINVAL;
		return 0;
	}

	cutoff = (unsigned long int)(ULONG_MAX) / (unsigned long int)base;
	cutlim = (unsigned long int)(ULONG_MAX) - (cutoff * (unsigned long int)base);

	while (isalnum(*sptr) != 0) {
		t = *sptr - '0';
		if (t > 9) {
			t = (*sptr | 0x20) - 'a' + 10;
		}

		if (t >= base) {
			break;
		}

		if (result > cutoff || (result == cutoff && t > cutlim)) {
			width = -1;
			break;
		}

		result = result * (unsigned long int)base + t;
		width++;
		sptr++;
	}

	if (width < 0) {
		errno = ERANGE;
		result = ULONG_MAX;
	}
	else if (width == 0) {
		errno = EINVAL;
	}
	else if (negative != 0) {
		result = -result;
	}

	if (endptr != NULL) {
		*endptr = (char *)(width > 0 ? sptr : nptr);
	}

	return result;
}


long int strtol(const char *nptr, char **endptr, int base)
{
	return (long int)strtoul(nptr, endptr, base);
}


int atoi(const char *str)
{
	return (int)strtol(str, NULL, 10);
}


long int atol(const char *str)
{
	return strtol(str, NULL, 10);
}

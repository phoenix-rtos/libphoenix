/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * strtoull, strtolll
 *
 * Copyright 2017 Phoenix Systems
 * Author: Adrian Kepka
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>


intmax_t strtoimax(const char *nptr, char **endptr, int base)
{
	return (intmax_t)strtoll(nptr, endptr, base);
}


uintmax_t strtoumax(const char *nptr, char **endptr, int base)
{
	return (uintmax_t)strtoull(nptr, endptr, base);
}


static unsigned long long int strtoll_common(const char *nptr, char **endptr, int base, int isUnsigned)
{
	const char *s;
	unsigned long long int acc, cutoff;
	char c;
	int neg, any, cutlim;

	s = nptr;
	do {
		c = *s++;
	} while (isspace(c));

	if (c == '-') {
		neg = 1;
		c = *s++;
	}
	else {
		neg = 0;
		if (c == '+') {
			c = *s++;
		}
	}

	any = 0;
	acc = 0uLL;

	if (((base == 0) || (base == 16)) && (c == '0') && ((s[0] == 'x') || (s[0] == 'X')) && isxdigit(s[1])) {
		c = s[1];
		s += 2;
		base = 16;
	}

	if (base == 0) {
		base = (c == '0') ? 8 : 10;
	}

	if ((base < 2) || (base > 36)) {
		errno = EINVAL;
		return 0uLL;
	}

	if (isUnsigned != 0) {
		cutoff = ULLONG_MAX;
	}
	else {
		cutoff = (neg != 0) ? -LLONG_MIN : LLONG_MAX;
	}

	cutlim = (int)(cutoff % base);
	cutoff /= base;

	for (;; c = *s++) {
		if (c >= '0' && c <= '9') {
			c -= '0';
		}
		else if (c >= 'A' && c <= 'Z') {
			c -= 'A' - 10;
		}
		else if (c >= 'a' && c <= 'z') {
			c -= 'a' - 10;
		}
		else {
			break;
		}

		if (c >= base) {
			break;
		}

		if (any < 0) {
			continue;
		}

		if ((acc > cutoff) || ((acc == cutoff) && (c > cutlim))) {
			any = -1;
		}
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}

	if (any == 0) {
		errno = EINVAL;
	}
	else if (any < 0) {
		errno = ERANGE;
		if (isUnsigned != 0) {
			acc = ULLONG_MAX;
		}
		else {
			acc = (neg != 0) ? -LLONG_MIN : LLONG_MAX;
		}
	}
	else if (neg != 0) {
		acc = -acc;
	}

	if (endptr != NULL) {
		*endptr = (char *)((any != 0) ? (s - 1) : nptr);
	}

	return acc;
}


unsigned long long int strtoull(const char *nptr, char **endptr, int base)
{
	return strtoll_common(nptr, endptr, base, 1);
}


long long int strtoll(const char *nptr, char **endptr, int base)
{
	return (long long int)strtoll_common(nptr, endptr, base, 0);
}


long long int atoll(const char *str)
{
	return strtoll(str, NULL, 10);
}

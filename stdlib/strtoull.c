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

#include "stdlib.h"
#include "ctype.h"
#include "limits.h"

unsigned long long int strtoull(const char *nptr, char **endptr, int base)
{
	const char *s;
	unsigned long long acc;
	char c;
	unsigned long long cutoff;
	int neg, any, cutlim;

	s = nptr;
	do {
		c = *s++;
	} while (isspace((unsigned char)c));

	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}

	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}

	if (base == 0)
		base = c == '0' ? 8 : 10;

	acc = 0;
	any = 0;

	if (base < 2 || base > 36)
		return acc;

	cutoff = ULONG_LONG_MAX / base;
	cutlim = ULONG_LONG_MAX % base;
	for ( ; ; c = *s++) {
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'Z')
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 10;
		else
			break;

		if (c >= base)
			break;

		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}

	if (any < 0) {
		acc = ULONG_LONG_MAX;
	} else if (neg)
		acc = -acc;

	if (endptr != NULL)
		*endptr = (char *)(any ? s - 1 : nptr);

	return (acc);
}


long long int strtoll(const char *nptr, char **endptr, int base)
{
	const char *s;
	long long int acc, cutoff;
	int c;
	int neg, any, cutlim;

	s = nptr;
	do {
		c = (unsigned char) *s++;
	} while (isspace(c));

	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}

	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}

	if (base == 0)
		base = c == '0' ? 8 : 10;

	cutoff = neg ? LONG_LONG_MIN : LONG_LONG_MAX;
	cutlim = (int)(cutoff % base);
	cutoff /= base;
	if (neg) {
		if (cutlim > 0) {
			cutlim -= base;
			cutoff += 1;
		}
		cutlim = -cutlim;
	}

	for (acc = 0, any = 0;; c = (unsigned char) *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;

		if (c >= base)
			break;

		if (any < 0)
			continue;

		if (neg) {
			if (acc < cutoff || (acc == cutoff && c > cutlim)) {
				any = -1;
				acc = LONG_LONG_MIN;
			} else {
				any = 1;
				acc *= base;
				acc -= c;
			}
		} else {
			if (acc > cutoff || (acc == cutoff && c > cutlim)) {
				any = -1;
				acc = LONG_LONG_MAX;
			} else {
				any = 1;
				acc *= base;
				acc += c;
			}
		}
	}

	if (endptr != 0)
		*endptr = (char *)(any ? s - 1 : nptr);

	return (acc);
}





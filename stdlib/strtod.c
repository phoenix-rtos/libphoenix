/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * strtod.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <stdlib.h>
#include <ctype.h>

double strtod(const char *restrict nptr, char **restrict endptr)
{
	long long integer = 0;
	double res = 0;
	int sign = 0, fracsz = 0, dotp = -1, exp = 0, exp_sign = 1;
	int len = 0, overflow = 0;

	if (nptr == NULL)
		return 0;

	/* strip leading  white spaces */
	while (isspace(*nptr))
		nptr++;

	/* check the sign */
	if (*nptr == '-') {
		sign = 1;
		nptr++;
	} else if (*nptr == '+')
		nptr++;

	/* get the fraction part and store it as integer */
	while (isdigit(*nptr) || *nptr == '.') {
		if (*nptr == '.')
			dotp = fracsz;
		fracsz++;
		nptr++;
	}

	if (fracsz != 0) {
		nptr -= fracsz;
		if (dotp >= 0)
			dotp = fracsz - dotp - 1;
		else
			dotp = 0;

		while(isdigit(*nptr) || *nptr == '.') {
			if (*nptr == '.') {
				nptr++;
				continue;
			}

			/* do not overflow long long... */
			if (len < 18)
				integer = integer * 10 + (*nptr - '0');
			else
				overflow++;
			nptr++;
			/* don't count leading zeros though */
			if (integer)
				len++;
		}
	}

	/* get the exponent part */
	if (*nptr == 'E' || *nptr == 'e') {
		nptr++;
		if (*nptr == '-') {
			exp_sign = -1;
			nptr++;
		} else if (*nptr == '+')
			nptr++;

		while(isdigit(*nptr))
			exp = exp * 10 + (*nptr++ - '0');
	}

	exp = exp - (exp_sign * dotp) + overflow;

	if (exp < 0 && exp_sign == 1) {
		exp_sign = -1;
		exp *= exp_sign;
	}

	res = (double)integer;
	while (exp--) {
		if (exp_sign == -1)
			res /= 10;
		else
			res *= 10;
	}

	if (sign)
		res = -res;

	if (endptr != NULL)
		*endptr = (char *)nptr;

	return res;
}

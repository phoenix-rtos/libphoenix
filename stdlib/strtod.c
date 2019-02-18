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
#include <math.h>


double strtod(const char *restrict nptr, char **restrict endptr)
{
	unsigned long long integer = 0, frac = 0;
	double res = 0, exp_val = 10;
	int sign = 0, intsz = 0, fracsz = 0, exp = 0, exp_sign = 1;
	int len_int = 0, len_frac = 0, overflow_int = 0, isnum = 0, i;

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

	/* get the integer part */
	while (isdigit(*nptr)) {
		if (len_int < 20)
			integer = integer * 10 + (*nptr - '0');
		else
			overflow_int++;
		if (integer)
			len_int++;
		intsz++;
		nptr++;
	}

	if (intsz)
		isnum = 1;

	/* get the fraction part */
	if (*nptr == '.') {
		nptr++;
		while (isdigit(*nptr)) {
			if (len_frac < 20)
				frac = frac * 10 + (*nptr - '0');
			if (frac)
				len_frac++;
			fracsz++;
			nptr++;
		}
	}

	if (fracsz)
		isnum = 1;

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

	if (exp < 0 && exp_sign == 1) {
		exp_sign = -1;
		exp *= exp_sign;
	}

	res = (double)integer;
	for (i = 0; i < 9; i++) {
		if ((overflow_int >> i) & 1)
			res *= exp_val;
		exp_val *= exp_val;
	}

	res += (double)frac * pow(10, -fracsz);
	exp_val = 10;
	for (i = 0; i < 9; i++) {
		if ((exp >> i) & 1) {
			if (exp_sign == -1)
				res /= exp_val;
			else
				res *= exp_val;
		}
		exp_val *= exp_val;
	}

	if (sign)
		res = -res;

	if (isnum && endptr != NULL)
		*endptr = (char *)nptr;

	return isnum ? res : 0;
}

double atof(const char *str)
{
	return strtod(str, NULL);
}

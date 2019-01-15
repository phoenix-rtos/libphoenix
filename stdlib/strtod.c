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
	double res = 0, exp_val = 10;
	int sign = 0, fracsz = 0, sep_pos = -1, exp = 0, exp_sign = 1;
	int len = 0, overflow = 0, isnum = 0, i;

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
		if (isdigit(*nptr))
			isnum = 1;
		if (*nptr == '.') {
			if (sep_pos != -1) {
				isnum = 0;
				break;
			}
			sep_pos = fracsz;
		}
		fracsz++;
		nptr++;
	}

	if (fracsz != 0) {
		nptr -= fracsz;
		if (sep_pos >= 0)
			sep_pos = fracsz - sep_pos - 1;
		else
			sep_pos = 0;

		while(isdigit(*nptr) || *nptr == '.') {
			if (*nptr == '.') {
				nptr++;
				continue;
			}

			/* do not overflow long long */
			if (len < 18)
				res = res * 10 + (*nptr - '0');
			else
				overflow++;
			nptr++;
			/* don't count leading zeros */
			if (res)
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

		if (!isdigit(*nptr))
			isnum = 0;

		while(isdigit(*nptr))
			exp = exp * 10 + (*nptr++ - '0');
	}

	exp = exp - (exp_sign * sep_pos) + overflow;

	if (exp < 0 && exp_sign == 1) {
		exp_sign = -1;
		exp *= exp_sign;
	}

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

	return res;
}

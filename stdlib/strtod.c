/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * strtod.c
 *
 * Copyright 2018, 2022 Phoenix Systems
 * Author: Kamil Amanowicz, Damian Loewnau
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <stdlib.h>
#include <ctype.h>
#include <math.h>


double strtod(const char *__restrict str, char **__restrict endptr)
{
	unsigned long long integer = 0, frac = 0;
	double res = 0, exp_val = 10;
	int sign = 0, intsz = 0, fracsz = 0, exp = 0, exp_sign = 1;
	int len_int = 0, len_frac = 0, overflow_int = 0, isnum = 0, i;

	if (str == NULL)
		return 0;

	/* strip leading  white spaces */
	while (isspace(*str))
		str++;

	/* check the sign */
	if (*str == '-') {
		sign = 1;
		str++;
	} else if (*str == '+')
		str++;

	/* get the integer part */
	while (isdigit(*str)) {
		if (len_int < 20)
			integer = integer * 10 + (*str - '0');
		else
			overflow_int++;
		if (integer)
			len_int++;
		intsz++;
		str++;
	}

	if (intsz)
		isnum = 1;

	/* get the fraction part */
	if (*str == '.') {
		str++;
		while (isdigit(*str)) {
			if (len_frac < 20)
				frac = frac * 10 + (*str - '0');
			if (frac)
				len_frac++;
			fracsz++;
			str++;
		}
	}

	if (fracsz)
		isnum = 1;

	/* get the exponent part */
	if (*str == 'E' || *str == 'e') {
		str++;
		if (*str == '-') {
			exp_sign = -1;
			str++;
		} else if (*str == '+')
			str++;

		while(isdigit(*str))
			exp = exp * 10 + (*str++ - '0');
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
		*endptr = (char *)str;

	return isnum ? res : 0;
}


float strtof(const char *__restrict str, char **__restrict endptr)
{
	return strtod(str, endptr);
}


double atof(const char *str)
{
	return strtod(str, NULL);
}

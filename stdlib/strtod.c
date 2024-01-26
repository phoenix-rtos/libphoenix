/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * strtod.c
 *
 * Copyright 2018, 2022 Phoenix Systems
 * Author: Kamil Amanowicz, Damian Loewnau, Dawid Szpejna
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>


/* Those values are related with IEEE 754 standard: binary32,
binary64 and binary binary128. Macros with SIGNIFICAND component
are responsible for precision of the specific type.*/
#define LENGTH_OF_SIGNIFICAND_FLOAT_32 8
#define RANGE_OF_EXPONENT_FLOAT_32     7


#define LENGTH_OF_SIGNIFICAND_DOUBLE_64 16
#define RANGE_OF_EXPONENT_DOUBLE_64     9


#define LENGTH_OF_SIGNIFICAND_DOUBLE_128 34
#define RANGE_OF_EXPONENT_DOUBLE_128     13


/* TODO: strto_any doesn't serve strings: hex numbers, NAN, INFINITE */
static long double strto_any(const char *__restrict str, char **__restrict endptr,
	int numdig, int powers)
{
	long double whole_number = 0, fraction = 0;
	char base = 10, is_number = 0;
	char negative = 0, exp_negative = 0;
	char len_int = 0, len_fraction = 0;
	int overflow_int = 0, exp = 0, dot_position = 0, i;
	long double tmp = base;

	if (str == NULL) {
		return 0;
	}

	/* strip leading  white spaces */
	while (isspace(*str)) {
		str++;
	}

	/*  check the sign */
	if (*str == '-' && (isdigit(*(str + 1)) || *(str + 1) == '.')) {
		negative = 1;
		str++;
	}
	else if (*str == '+' && (isdigit(*(str + 1)) || *(str + 1) == '.')) {
		str++;
	}

	/*  get the integer part */
	while (isdigit(*str)) {
		if (len_int < numdig) {
			whole_number = whole_number * base + (*str - '0');
		}
		else {
			overflow_int++;
		}

		if (whole_number) {
			len_int++;
		}

		is_number = 1;
		str++;
	}

	/*  get the fractiontion part */
	if (*str == '.') {
		str++;

		while (isdigit(*str)) {
			if (len_fraction < numdig) {
				fraction = fraction * base + (*str - '0');
				dot_position++;
			}

			if (fraction) {
				len_fraction++;
			}

			is_number = 1;
			str++;
		}
	}

	/*  get the exponent part */
	if (*str == 'E' || *str == 'e') {
		str++;

		if (*str == '-') {
			exp_negative = 1;
			str++;
		}
		else if (*str == '+') {
			str++;
		}

		if (!isdigit(*str)) {
			str--;

			if (*str == '+' || *str == '-') {
				str--;
			}
		}

		while (isdigit(*str)) {
			exp = exp * base + (*str++ - '0');
		}
	}

	for (i = 0; i < powers; i++) {
		if ((overflow_int >> i) & 1) {
			whole_number *= tmp;
		}

		tmp *= tmp;
	}

	long double result = whole_number;
	result += fraction * pow(10, -dot_position);

	tmp = base;
	for (i = 0; i < powers; i++) {
		if ((exp >> i) & 1) {
			if (exp_negative != 0) {
				result /= tmp;
			}
			else {
				result *= tmp;
			}

			is_number = 1;
		}

		tmp *= tmp;
	}

	if (negative != 0) {
		result = -result;
	}

	if (endptr != NULL) {
		*endptr = (char *)str;
	}

	if (!is_number) {
		errno = EINVAL;
	}
	else if (isinf(result)) {
		errno = ERANGE;
	}

	return is_number ? result : 0;
}


/* TODO: strto_any is not fully implemented */
float strtof(const char *__restrict str, char **__restrict endptr)
{
	return (float)strto_any(str, endptr, LENGTH_OF_SIGNIFICAND_FLOAT_32, RANGE_OF_EXPONENT_FLOAT_32);
}


/* TODO: strto_any is not fully implemented*/
double strtod(const char *__restrict str, char **__restrict endptr)
{
	return (double)strto_any(str, endptr, LENGTH_OF_SIGNIFICAND_DOUBLE_64, RANGE_OF_EXPONENT_DOUBLE_64);
}


/* TODO: strto_any is not fully implemented */
long double strtold(const char *__restrict str, char **__restrict endptr)
{
	return strto_any(str, endptr, LENGTH_OF_SIGNIFICAND_DOUBLE_128, RANGE_OF_EXPONENT_DOUBLE_128);
}


/* TODO: strto_any is not fully implemented */
double atof(const char *str)
{
	return strtod(str, NULL);
}

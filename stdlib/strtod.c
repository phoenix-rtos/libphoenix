/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * strtod.c
 *
 * Copyright 2018, 2022 Phoenix Systems
 * Author: Kamil Amanowicz, Damian Loewnau, Dawid Szpejna, Jacek Maksymowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>


#define TYPE_ID_FLT 0
#define TYPE_ID_DBL 1

#if (LDBL_MANT_DIG == DBL_MANT_DIG) && (LDBL_MAX_EXP == DBL_MAX_EXP)
#define TYPE_ID_LDBL     TYPE_ID_DBL
#define LDBL_MANT_10_DIG 15
#elif (LDBL_MANT_DIG == 64)
#define TYPE_ID_LDBL     2
#define LDBL_MANT_10_DIG 19
#elif (LDBL_MANT_DIG == 113)
#define TYPE_ID_LDBL     2
#define LDBL_MANT_10_DIG 34
#else
#error "Unknown floating point implementation"
#endif

#define UINT64_MAX_DEC_DIGITS 19


/* Parse through whitespace, sign, infinity, NaN */
static const char *floatparse_wsin(const char *str, int *sign, long double *result)
{
	*sign = 1;
	/* strip leading  white spaces */
	while (isspace(*str)) {
		str++;
	}

	/*  check the sign */
	if (*str == '-') {
		*sign = -1;
		str++;
	}
	else if (*str == '+') {
		str++;
	}

	/* check for inf{inity} */
	if (strncasecmp(str, "inf", 3) == 0) {
		str += 3;

		if (strncasecmp(str, "inity", 5) == 0) {
			str += 5;
		}

		*result = (*sign) * INFINITY;
		*sign = 0;
		return str;
	}

	/* check for NAN{(char_sequence)} */
	if (strncasecmp(str, "nan", 3) == 0) {
		str += 3;

		if (*str == '(') {
			const char *tryParen = str + 1;

			while (*tryParen) {
				if (*tryParen == ')') {
					str = tryParen + 1;
					break;
				}

				if (!isalnum(*tryParen) && (*tryParen != '_')) {
					break;
				}

				tryParen++;
			}
		}

		*result = NAN;
		*sign = 0;
		return str;
	}

	return str;
}


static inline bool valid_hex(const char *str, char decimal_point)
{
	return isxdigit(str[0]) || ((str[0] == decimal_point) && isxdigit(str[1]));
}


static inline bool valid_decimal(const char *str, char decimal_point)
{
	return isdigit(str[0]) || ((str[0] == decimal_point) && isdigit(str[1]));
}


/* Parse through 0x prefix, leading zeroes and detect validity */
static const char *floatparse_zerox(const char *str, char decimal_point, bool *is_hex)
{
	*is_hex = 0;
	if (*str == '0') {
		str++;
		if (((*str == 'x') || (*str == 'X')) && valid_hex(str + 1, decimal_point)) {
			str++;
			*is_hex = 1;
		}

		while (*str == '0') {
			str++;
		}

		return str;
	}
	else if (valid_decimal(str, decimal_point)) {
		return str;
	}

	return NULL;
}

static int get_digit_value(char byte, bool hex)
{
	int c = byte;
	c -= 'A';
	if (hex && (c >= 0)) {
		c &= ~0x20;
		c += 10;
		if (c > 0xf) {
			return -1;
		}
	}
	else {
		c = byte - '0';
		if (c > 9) {
			return -1;
		}
	}

	return c;
}


static const char *scan_number(const char *str, bool hex, char decimal_point, uint64_t limit, size_t *n_decimal, size_t *n_overflowed, uint64_t *out)
{
	const int base = hex ? 16 : 10;
	uint64_t result = 0u;
	bool allow_dec = decimal_point != 0;
	bool got_dec = !allow_dec;
	while (1) {
		char c = *str;
		if (!got_dec && (c == decimal_point)) {
			got_dec = true;
			str++;
			continue;
		}

		int dig = get_digit_value(c, hex);
		if (dig < 0) {
			break;
		}

		if (result <= limit) {
			result = result * base + dig;
			if (got_dec) {
				(*n_decimal)++;
			}
		}
		else if (allow_dec != got_dec) {
			(*n_overflowed)++;
		}

		str++;
	}

	*out = result;
	return str;
}


static const char *scan_number_long_double(const char *str, bool hex, char decimal_point, size_t digit_limit, size_t *n_decimal, size_t *n_overflowed, long double *out)
{
	const int base = hex ? 16 : 10;
	long double result = 0;
	bool got_dec = false;
	size_t n_digits = 0;
	while (1) {
		char c = *str;
		if (!got_dec && (c == decimal_point)) {
			got_dec = true;
			str++;
			continue;
		}

		int dig = get_digit_value(c, hex);
		if (dig < 0) {
			break;
		}

		if (n_digits <= digit_limit) {
			n_digits++;
			result = result * base + dig;
			if (got_dec) {
				(*n_decimal)++;
			}
		}
		else if (!got_dec) {
			(*n_overflowed)++;
		}

		str++;
	}

	*out = result;
	return str;
}

static long double mul_exp10(long double in, int64_t exp)
{
#if FLT_ROUNDS != 1
#warning "This code was only tested with FLT_ROUNDS == 1"
#endif

	if (in == 0) {
		return in;
	}

#define POW10_LOOKUPS 9
	/* Lookup is used here for increased accuracy - our implementation of pow()
	   is imprecise on some platforms and accumulates errors for large exponents. */
	static const long double lookup[POW10_LOOKUPS] = {
		1e1l,
		1e2l,
		1e4l,
		1e8l,
		1e16l,
		1e32l,
		1e64l,
		1e128l,
		1e256l,
	};

	bool negative = exp < 0;
	exp = negative ? -exp : exp;

#if LDBL_MAX_10_EXP < (1 << POW10_LOOKUPS)
	if (exp >= (1 << POW10_LOOKUPS)) {
		return negative ? 0 : INFINITY;
	}
#endif

	for (int i = 0; exp != 0 && i < POW10_LOOKUPS; exp >>= 1, i++) {
		if ((exp & 1) != 0) {
			/* TODO: some errors can accumulate here */
			if (negative) {
				in /= lookup[i];
			}
			else {
				in *= lookup[i];
			}
		}
	}

#if LDBL_MAX_10_EXP >= (1 << POW10_LOOKUPS)
	if ((exp == 0) || (in == 0) || isinf(in)) {
		return in;
	}

	long double multiplier = lookup[POW10_LOOKUPS - 1] * lookup[POW10_LOOKUPS - 1];
	for (; exp != 0; exp >>= 1) {
		if ((exp & 1) != 0) {
			if (negative) {
				in /= multiplier;
			}
			else {
				in *= multiplier;
			}
		}

		multiplier *= multiplier;
	}
#endif

	return in;
}


static long double mul_exp2(long double in, int64_t exp)
{
	if (in == 0) {
		return in;
	}

	while (exp > 1023) {
		exp -= 1023;
		in *= 0x1p1023;
	}

	while (exp < -1022) {
		/* After exiting exp should be < -53 to avoid double
		   rounding if the result is subnormal */
		exp += 1022 - 53;
		in *= (0x1p-1022 * 0x1p53);
	}


	union {
		uint64_t i;
		double d;
	} multiplier;
	multiplier.i = (uint64_t)(0x3ff + exp) << 52;
	return in * multiplier.d;
}


/* Parse through the number and exponent */
static const char *floatparse_number(const char *str, bool hex, uint8_t type, char decimal_point, long double *out)
{
	int32_t exp_max;
	int32_t exp_min;

	switch (type) {
		case TYPE_ID_FLT:
			if (hex) {
				exp_min = FLT_MIN_EXP - 64;
				exp_max = FLT_MAX_EXP + 64;
			}
			else {
				exp_min = FLT_MIN_10_EXP - UINT64_MAX_DEC_DIGITS;
				exp_max = FLT_MAX_10_EXP + UINT64_MAX_DEC_DIGITS;
			}
			break;

		case TYPE_ID_DBL:
			if (hex) {
				exp_min = DBL_MIN_EXP - 64;
				exp_max = DBL_MAX_EXP + 64;
			}
			else {
				exp_min = DBL_MIN_10_EXP - UINT64_MAX_DEC_DIGITS;
				exp_max = DBL_MAX_10_EXP + UINT64_MAX_DEC_DIGITS;
			}
			break;

		default: /* TYPE_ID_LDBL */
			if (hex) {
				exp_min = LDBL_MIN_EXP - LDBL_MANT_DIG;
				exp_max = LDBL_MAX_EXP + LDBL_MANT_DIG;
			}
			else {
				exp_min = LDBL_MIN_10_EXP - LDBL_MANT_10_DIG;
				exp_max = LDBL_MAX_10_EXP + LDBL_MANT_10_DIG;
			}
			break;
	}


	size_t overflow_int = 0, frac_position = 0;
	long double result;
	int64_t total_exp = 0; /* base 2 exponent if hex, base 10 exponent otherwise */

	if (type <= TYPE_ID_DBL) {
		/* If parsing singles/doubles it's sufficient to use uint64_t to store
		   the digits - this allows us to use integer multiplication */
		const uint64_t limit = hex ? (UINT64_MAX / 16) : (10000000000000000000ull / 10);
		uint64_t digits;
		str = scan_number(str, hex, decimal_point, limit, &frac_position, &overflow_int, &digits);
		result = (long double)digits;
	}
	else {
		str = scan_number_long_double(str, hex, decimal_point, LDBL_MANT_10_DIG, &frac_position, &overflow_int, &result);
	}

	if (frac_position > (INT32_MAX / 4) || overflow_int > (INT32_MAX / 4)) {
		/* This is an absurd situation, we read in 2^29 digits from input, so we don't care much about the result */
		total_exp = (frac_position != 0) ? INT32_MIN : INT32_MAX;
	}
	else {
		/* If there was a decimal point, we don't need to care about the overflow */
		total_exp = (frac_position != 0) ? -(int64_t)frac_position : overflow_int;
		if (hex) {
			total_exp *= 4;
		}
	}


	bool has_exponent = hex ? (*str == 'P' || *str == 'p') : (*str == 'E' || *str == 'e');
	if (has_exponent) {
		const char *str_exp = str + 1;
		bool exp_negative = false;
		if ((*str_exp == '-') || (*str_exp == '+')) {
			exp_negative = (*str_exp == '-');
			str_exp++;
		}

		if (isdigit(*str_exp)) {
			size_t exp_overflowed = 0, dummy;
			uint64_t exp_scan;
			str = scan_number(str_exp, 0, 0, ((INT32_MAX - 9) / 10), &dummy, &exp_overflowed, &exp_scan);
			if (exp_overflowed != 0) {
				total_exp = exp_negative ? INT32_MIN : INT32_MAX;
			}
			else {
				/* At this point exp_scan is guaranteed to be <= INT32_MAX */
				total_exp += exp_negative ? -(int32_t)exp_scan : exp_scan;
			}
		}
	}

	if (result != 0.0l) {
		if (total_exp < exp_min) {
			result = 0;
		}
		else if (total_exp > exp_max) {
			result = INFINITY;
		}
		else if (hex) {
			result = mul_exp2(result, total_exp);
		}
		else {
			result = mul_exp10(result, total_exp);
		}

		if (result == 0.0l) {
			/* We entered with non-zero result, but now it is zero - underflow must have occurred */
			/* TODO: detect denormalization */
			errno = ERANGE;
		}
	}

	*out = result;
	return str;
}


static long double strto_any(const char *__restrict str, char **__restrict endptr, uint8_t type)
{
	const char *decimal_point_str = localeconv()->decimal_point;
	char decimal_point;
	if (decimal_point_str == NULL) {
		decimal_point = '.';
	}
	else if ((decimal_point_str[0] == '\0') || (decimal_point_str[1] != '\0')) {
		/* TODO: multi-character decimal separators (Arabic decimal separator in UTF-8) */
		errno = EINVAL;
		return 0.0l;
	}
	else {
		decimal_point = decimal_point_str[0];
	}

	int sign;
	long double result = 0.0l;

	if (str == NULL) {
		return 0.0l;
	}

	const char *str_wsin = floatparse_wsin(str, &sign, &result);
	if (sign == 0) {
		if (endptr != NULL) {
			*endptr = (char *)str_wsin;
		}

		return result;
	}

	bool is_hex;
	const char *str_zerox = floatparse_zerox(str_wsin, decimal_point, &is_hex);
	if (str_zerox == NULL) {
		errno = EINVAL;
		if (endptr != NULL) {
			*endptr = (char *)str;
		}

		return result;
	}

	str = floatparse_number(str_zerox, is_hex, type, decimal_point, &result);

	bool range_error;
	switch (type) {
		case TYPE_ID_FLT: {
			float cast = (float)result;
			range_error = isinf(cast) || ((result != 0.0l) && (cast == 0.0f));
			break;
		}

		case TYPE_ID_DBL: {
			double cast = (double)result;
			range_error = isinf(cast) || ((result != 0.0l) && (cast == 0.0));
			break;
		}

		default:
			range_error = isinf(result);
			break;
	}

	if (range_error) {
		errno = ERANGE;
	}

	if (sign < 0) {
		result = -result;
	}

	if (endptr != NULL) {
		*endptr = (char *)str;
	}

	return result;
}


float strtof(const char *__restrict str, char **__restrict endptr)
{
	return (float)strto_any(str, endptr, TYPE_ID_FLT);
}


double strtod(const char *__restrict str, char **__restrict endptr)
{
	return (double)strto_any(str, endptr, TYPE_ID_DBL);
}


long double strtold(const char *__restrict str, char **__restrict endptr)
{
	return strto_any(str, endptr, TYPE_ID_LDBL);
}


double atof(const char *str)
{
	return strtod(str, NULL);
}

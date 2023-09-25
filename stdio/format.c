/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * format.c
 *
 * Copyright 2017, 2023 Phoenix Systems
 * Author: Adrian Kepka, Andrzej Stalke
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "format.h"
#include "bignum.h"

#include <sys/minmax.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>


#define FLAG_SIGNED                    0x1
#define FLAG_64BIT                     0x2
#define FLAG_NO_TRAILING_ZEROS         0x4
#define FLAG_SPACE                     0x10
#define FLAG_ZERO                      0x20
#define FLAG_PLUS                      0x40
#define FLAG_HEX                       0x80
#define FLAG_OCT                       0x100
#define FLAG_LARGE_DIGITS              0x200
#define FLAG_ALTERNATE                 0x400
#define FLAG_NULLMARK                  0x800
#define FLAG_MINUS                     0x1000
#define FLAG_FIELD_WIDTH_STAR          0x2000
#define DOUBLE_EXP_INVALID             (-1024)
#define HEXDOUBLE_SUFFICIENT_PRECISION 13
#define FLAG_8BIT                      0x8000
#define FLAG_16BIT                     0x10000
#define FLAG_LONG_DOUBLE               0x20000

/* clang-format off */
#define GET_UNSIGNED(number, flags, args) \
	do { \
		if (((flags) & FLAG_8BIT) != 0) { \
			(number) = (uint8_t)va_arg((args), unsigned int); \
		} \
		else if (((flags) & FLAG_16BIT) != 0) { \
			(number) = (uint16_t)va_arg((args), unsigned int); \
		} \
		else if (((flags) & FLAG_64BIT) != 0) { \
			(number) = va_arg((args), uint64_t); \
		} \
		else { \
			(number) = va_arg((args), uint32_t); \
		} \
	} while (0)


#define GET_SIGNED(number, flags, args) \
	do { \
		if (((flags) & FLAG_8BIT) != 0) { \
			(number) = (int8_t)va_arg((args), int); \
		} \
		else if (((flags) & FLAG_16BIT) != 0) { \
			(number) = (int16_t)va_arg((args), int); \
		} \
		else if (((flags) & FLAG_64BIT) != 0) { \
			(number) = va_arg((args), int64_t); \
		} \
		else { \
			(number) = va_arg((args), int32_t); \
		} \
	} while (0)


#define GET_DOUBLE(number, flags, args) \
	do { \
		if (((flags)&FLAG_LONG_DOUBLE) != 0) { \
			(number) = (double)va_arg((args), long double); \
		} \
		else { \
			(number) = (double)va_arg((args), double); \
		} \
	} while (0)

#define BIGDOUBLE_DEFAULT_SIZE 4

/* clang-format on */
union double_u64 {
	double d;
	uint64_t u;
};

struct buffer {
	char *data;
	size_t len, size;
};

struct bigdouble {
	/* Stored number */
	bignum_t num;
	/* Helper values */
	bignum_t helper1;
	uint64_t firstIntegerBit;
};

static const char smallDigits[] = "0123456789abcdef";
static const char largeDigits[] = "0123456789ABCDEF";


static inline int format_bufferInit(struct buffer *buff, size_t size)
{
	char *data = malloc(size);
	if (data != NULL) {
		*buff = (struct buffer) { .data = data, .len = 0, .size = size };
		return 0;
	}
	else {
		return -ENOMEM;
	}
}


static inline int format_bufferAddChar(struct buffer *buff, char c)
{
	char *data;
	if (buff->len >= buff->size) {
		data = realloc(buff->data, 2 * buff->size);
		if (data == NULL) {
			return -ENOMEM;
		}
		buff->data = data;
		buff->size = 2 * buff->size;
	}
	buff->data[buff->len++] = c;
	return 0;
}


static inline void format_bufferDestroy(struct buffer *buff)
{
	free(buff->data);
	buff->data = NULL;
	buff->size = 0;
	buff->len = 0;
}


static inline uint64_t format_u64FromDouble(double d)
{
	union double_u64 u;

	u.d = d;
	return u.u;
}


static inline int32_t format_doubleGetExponent(uint64_t num64)
{
	return ((int32_t)((num64 >> DOUBLE_EXP_SHIFT) & 0x7ff) - 1023);
}


static int format_bigdoubleInit(struct bigdouble *bd, const size_t size)
{
	int res = bignum_init(&bd->num, size, 0);
	if (res != 0) {
		return res;
	}
	res = bignum_init(&bd->helper1, size, 0);
	if (res != 0) {
		bignum_destroy(&bd->num);
		return res;
	}
	bd->firstIntegerBit = 0;
	return 0;
}


static void format_bigdoubleDestroy(struct bigdouble *bd)
{
	bignum_destroy(&bd->num);
	bignum_destroy(&bd->helper1);
}


static int format_bigdoubleIntegerPart(const struct bigdouble *bd, bignum_t *result)
{
	int res = bignum_copy(result, &bd->num);
	if (res == 0) {
		bignum_shiftr(result, bd->firstIntegerBit);
	}
	return res;
}


static void format_bigdoubleCutIntegerPart(struct bigdouble *result)
{
	const size_t firstIntegerIndex = result->firstIntegerBit / BIGNUM_DATA_BITS;
	const size_t fracMask = (1LLU << (result->firstIntegerBit % BIGNUM_DATA_BITS)) - 1;
	if (firstIntegerIndex < result->num.len) {
		result->num.data[firstIntegerIndex] &= fracMask;
		result->num.len = firstIntegerIndex + 1;
	}
}


static int format_bigdoubleLoad(struct bigdouble *result, double val)
{
	uint64_t num64 = format_u64FromDouble(val);
	int32_t exp = format_doubleGetExponent(num64);
	const int isSubnormalOrZero = exp == -1023;
	uint64_t mantissa = num64 & DOUBLE_MANTISSA_MASK;
	int res = format_bigdoubleInit(result, BIGDOUBLE_DEFAULT_SIZE);

	if (res != 0) {
		return res;
	}
	bignum_zero(&result->num);
	if ((isSubnormalOrZero != 0) && (mantissa == 0)) {
		/* val == 0 */
		result->firstIntegerBit = 0;
		return 0;
	}
	/* val != 0 */
	mantissa |= (isSubnormalOrZero != 0 ? 0LLU : 1LLU) << DOUBLE_EXP_SHIFT;
	result->num.data[0] = mantissa & BIGNUM_DATA_MASK;
	mantissa >>= BIGNUM_DATA_BITS;
	res = bignum_push(&result->num, (bignum_data_t)mantissa);
	if (res != 0) {
		format_bigdoubleDestroy(result);
		return res;
	}
	if (isSubnormalOrZero != 0) {
		exp = -1022;
	}
	/* Check if there is no fractional part */
	if (DOUBLE_EXP_SHIFT < exp) {
		result->firstIntegerBit = 0;
		res = bignum_shiftl(&result->num, exp - DOUBLE_EXP_SHIFT);
	}
	else {
		result->firstIntegerBit = DOUBLE_EXP_SHIFT - exp;
	}
	return res;
}


static void format_printBuffer(void *ctx, feedfunc feed, uint32_t flags, int minFieldWidth, const char *start, const char *end, char sign)
{
	const int digits_cnt = start > end ? start - end : end - start;
	int pad_len = minFieldWidth - digits_cnt - (sign ? 1 : 0);
	/* If FLAG_ZERO and FLAG_MINUS both appear, then FLAG_ZERO is ignored */
	if ((flags & FLAG_MINUS) != 0) {
		flags &= ~FLAG_ZERO;
	}

	/* pad, if needed */
	if ((pad_len > 0) && ((flags & FLAG_MINUS) == 0) && ((flags & FLAG_ZERO) == 0)) {
		while (pad_len-- > 0) {
			feed(ctx, ' ');
		}
	}

	if (sign != 0) {
		feed(ctx, sign);
	}

	/* pad, if needed */
	if ((pad_len > 0) && ((flags & FLAG_MINUS) == 0) && ((flags & FLAG_ZERO) != 0)) {
		while (pad_len-- > 0) {
			feed(ctx, '0');
		}
	}

	/* copy */
	if (start > end) {
		while ((--start) >= end) {
			feed(ctx, *start);
		}
	}
	else {
		while (start < end) {
			feed(ctx, *start++);
		}
	}

	/* pad, if needed */
	if ((pad_len > 0) && ((flags & FLAG_MINUS) != 0)) {
		while (pad_len-- > 0) {
			feed(ctx, ' ');
		}
	}
}


static void format_bufferReverse(char *buff, size_t start, size_t end)
{
	char tmp;
	end -= 1;
	for (; start < end; ++start, --end) {
		tmp = buff[start];
		buff[start] = buff[end];
		buff[end] = tmp;
	}
}


static int format_sprintfDecimalForm(struct buffer *buff, struct bigdouble *bd, uint32_t flags, int precision, unsigned int *startOffset)
{
	char *carrier;

	unsigned int removeTrailingZeros = flags & FLAG_NO_TRAILING_ZEROS && !(flags & FLAG_ALTERNATE);
	unsigned int carry = 0;
	uint32_t remainder;
	int i, length, ret;

	/* Reserve space for potential additional digit, due to carry. For example: 99.996 -> 100.00 */
	ret = format_bufferAddChar(buff, 0);
	if (ret < 0) {
		return ret;
	}
	*startOffset = 1;

	ret = format_bigdoubleIntegerPart(bd, &bd->helper1);
	if (ret < 0) {
		return ret;
	}
	format_bigdoubleCutIntegerPart(bd);

	/* Integer part */
	length = 0;
	do {
		bignum_div(&bd->helper1, &remainder, 10);
		ret = format_bufferAddChar(buff, smallDigits[remainder]);
		if (ret < 0) {
			return ret;
		}
		length += 1;
	} while (bignum_cmp(&bd->helper1, 0) != 0);
	format_bufferReverse(buff->data, *startOffset, *startOffset + length);

	/* Frac part */
	if ((precision > 0) || ((flags & FLAG_ALTERNATE) != 0)) {
		ret = format_bufferAddChar(buff, '.');
		if (ret < 0) {
			return ret;
		}
	}
	for (i = 0; i < precision; ++i) {
		ret = bignum_mul(&bd->num, 10);
		if (ret < 0) {
			return ret;
		}
		ret = format_bigdoubleIntegerPart(bd, &bd->helper1);
		if (ret < 0) {
			return ret;
		}
		format_bigdoubleCutIntegerPart(bd);
		ret = format_bufferAddChar(buff, smallDigits[bd->helper1.data[0] % 10]);
		if (ret < 0) {
			return ret;
		}
	}
	/* Rounding */
	ret = bignum_mul(&bd->num, 10);
	if (ret < 0) {
		return ret;
	}
	ret = format_bigdoubleIntegerPart(bd, &bd->helper1);
	if (ret != 0) {
		return ret;
	}

	format_bigdoubleCutIntegerPart(bd);
	if (bd->helper1.data[0] % 10 >= 5) {
		carry = 1;
	}

	carrier = buff->data + buff->len - 1;
	while (carry != 0) {
		if (*carrier == 0) {
			*carrier = '1';
			*startOffset = 0;
			carry = 0;
		}
		else if (*carrier == '9') {
			*(carrier--) = '0';
		}
		else if (('0' <= *carrier) && (*carrier <= '8')) {
			*carrier = *carrier + 1;
			carry = 0;
		}
		else {
			carrier--;
		}
	}
	if (((precision > 0) || ((flags & FLAG_ALTERNATE) != 0)) && (removeTrailingZeros != 0)) {
		--buff->len;
		while (buff->data[buff->len] == '0')
			--buff->len;
		if (buff->data[buff->len] == '.') {
			--buff->len;
		}
		++buff->len;
	}
	return 0;
}


static int format_sprintfHexadecimalForm(struct buffer *buff, double d, uint32_t flags, const int precision, unsigned int *startOffset)
{
	const char *const digits = (flags & FLAG_LARGE_DIGITS) != 0 ? largeDigits : smallDigits;
	uint32_t temp, carry = 0;
	uint64_t num64 = format_u64FromDouble(d);
	size_t currSize;
	int32_t exp = format_doubleGetExponent(num64);
	int i, ret;

	for (i = 0; i < 5; ++i) {
		ret = format_bufferAddChar(buff, 0);
		if (ret < 0) {
			return ret;
		}
	}
	*startOffset = 5;

	for (i = 0; i < precision - HEXDOUBLE_SUFFICIENT_PRECISION; ++i) {
		ret = format_bufferAddChar(buff, 0);
		if (ret < 0) {
			return ret;
		}
	}
	for (i = 0; i < HEXDOUBLE_SUFFICIENT_PRECISION; ++i) {
		ret = format_bufferAddChar(buff, num64 % 16);
		if (ret < 0) {
			return ret;
		}
		num64 /= 16;
	}
	format_bufferReverse(buff->data, *startOffset, buff->len);

	*startOffset -= 1;
	buff->data[*startOffset] = exp == -1023 ? 0 : 1;

	if (precision < HEXDOUBLE_SUFFICIENT_PRECISION) {
		i = *startOffset + precision;
		temp = buff->data[i + 1];
		if (temp >= 8) {
			carry = 1;
		}
		while (carry != 0) {
			if (buff->data[i] != 15) {
				buff->data[i] += 1;
				carry = 0;
			}
			else {
				buff->data[i] = 0;
			}
			i -= 1;
		}
		if (i < *startOffset - 1) {
			*startOffset = *startOffset - 1;
		}
	}
	for (i = *startOffset; i <= *startOffset + precision; ++i) {
		temp = buff->data[i];
		buff->data[i] = digits[temp];
	}

	buff->len = *startOffset + 1 + precision;

	if (!((precision == 0) && ((flags & FLAG_ALTERNATE) == 0))) {
		*startOffset -= 1;
		buff->data[*startOffset] = buff->data[*startOffset + 1];
		buff->data[*startOffset + 1] = '.';
	}
	*startOffset -= 1;
	buff->data[*startOffset] = (flags & FLAG_LARGE_DIGITS) != 0 ? 'X' : 'x';
	*startOffset -= 1;
	buff->data[*startOffset] = '0';

	ret = format_bufferAddChar(buff, ((flags & FLAG_LARGE_DIGITS) != 0) ? 'P' : 'p');
	if (ret < 0) {
		return ret;
	}
	if (exp < 0) {
		if (exp == -1023) {
			exp = -1022;
		}
		exp = -exp;
		ret = format_bufferAddChar(buff, '-');
		if (ret < 0) {
			return ret;
		}
	}
	else {
		ret = format_bufferAddChar(buff, '+');
		if (ret < 0) {
			return ret;
		}
	}
	if (exp > 0) {
		currSize = buff->len;
		for (; exp > 0; exp /= 10) {
			ret = format_bufferAddChar(buff, digits[exp % 10]);
			if (ret < 0) {
				return ret;
			}
		}
		format_bufferReverse(buff->data, currSize, buff->len);
	}
	else {
		ret = format_bufferAddChar(buff, digits[0]);
		if (ret < 0) {
			return ret;
		}
	}
	return 0;
}


static int format_sprintfScientificForm(struct buffer *buff, struct bigdouble *bd, uint32_t flags, const int precision, unsigned int *startOffset)
{
	char *carrier;
	unsigned int removeTrailingZeros = flags & FLAG_NO_TRAILING_ZEROS && !(flags & FLAG_ALTERNATE);
	unsigned int expIsPositive, carry = 0;
	uint32_t remainder;
	uint32_t temp;
	int i, exp = 0, resexp = DOUBLE_EXP_INVALID, ret;

	/* Reserve space for potential additional digit (carry) and for a dot. */
	for (i = 0; i < 2; ++i) {
		ret = format_bufferAddChar(buff, 0);
		if (ret < 0) {
			return DOUBLE_EXP_INVALID + ret;
		}
	}
	*startOffset = 2;

	ret = format_bigdoubleIntegerPart(bd, &bd->helper1);
	if (ret < 0) {
		return DOUBLE_EXP_INVALID + ret;
	}
	format_bigdoubleCutIntegerPart(bd);

	/* Integer part */
	if (bignum_cmp(&bd->helper1, 0) != 0) {
		expIsPositive = 1;
		do {
			bignum_div(&bd->helper1, &remainder, 10);
			ret = format_bufferAddChar(buff, smallDigits[remainder]);
			if (ret < 0) {
				return DOUBLE_EXP_INVALID + ret;
			}
			exp += 1;
		} while (bignum_cmp(&bd->helper1, 0) != 0);
		format_bufferReverse(buff->data, *startOffset, exp + *startOffset);
		exp--;
	}
	else {
		expIsPositive = 0;
		do {
			ret = bignum_mul(&bd->num, 10);
			if (ret < 0) {
				return DOUBLE_EXP_INVALID + ret;
			}
			ret = format_bigdoubleIntegerPart(bd, &bd->helper1);
			if (ret < 0) {
				return DOUBLE_EXP_INVALID + ret;
			}
			format_bigdoubleCutIntegerPart(bd);
			temp = bd->helper1.data[0] % 10;
			--exp;
		} while (temp == 0);
		ret = format_bufferAddChar(buff, smallDigits[temp]);
		if (ret < 0) {
			return DOUBLE_EXP_INVALID + ret;
		}
	}

	for (i = 0; i <= precision; ++i) {
		ret = bignum_mul(&bd->num, 10);
		if (ret < 0) {
			return DOUBLE_EXP_INVALID + ret;
		}
		ret = format_bigdoubleIntegerPart(bd, &bd->helper1);
		if (ret < 0) {
			return DOUBLE_EXP_INVALID + ret;
		}
		format_bigdoubleCutIntegerPart(bd);
		ret = format_bufferAddChar(buff, smallDigits[bd->helper1.data[0] % 10]);
		if (ret < 0) {
			return DOUBLE_EXP_INVALID + ret;
		}
	}

	/* Remove garbage digits */
	buff->len = *startOffset + precision + 2;

	/* Rounding */
	if (buff->data[*startOffset + precision + 1] - '0' >= 5) {
		carry = 1;
	}
	buff->len -= 1;
	carrier = buff->data + buff->len - 1;
	while (carry != 0) {
		if (*carrier == 0) {
			*carrier = '1';
			*startOffset -= 1;
			carry = 0;
			exp += 1;
			buff->len -= 1;
			if (exp == 0) {
				expIsPositive = 1;
			}
		}
		else if (*carrier == '9') {
			*(carrier--) = '0';
		}
		else if (('0' <= *carrier) && (*carrier <= '8')) {
			*carrier = *carrier + 1;
			carry = 0;
		}
		else {
			carrier--;
		}
	}

	if ((precision > 0) || ((flags & FLAG_ALTERNATE) != 0)) {
		/* Insert a dot */
		*startOffset -= 1;
		buff->data[*startOffset] = buff->data[*startOffset + 1];
		buff->data[*startOffset + 1] = '.';

		if (removeTrailingZeros != 0) {
			--buff->len;
			while (buff->data[buff->len] == '0') {
				--buff->len;
			}
			if (buff->data[buff->len] == '.') {
				--buff->len;
			}
			++buff->len;
		}
	}
	ret = format_bufferAddChar(buff, flags & FLAG_LARGE_DIGITS ? 'E' : 'e');
	if (ret < 0) {
		return DOUBLE_EXP_INVALID + ret;
	}
	ret = format_bufferAddChar(buff, expIsPositive ? '+' : '-');
	if (ret < 0) {
		return DOUBLE_EXP_INVALID + ret;
	}

	resexp = exp;
	if (expIsPositive == 0) {
		exp = -exp;
	}
	i = buff->len;
	ret = format_bufferAddChar(buff, smallDigits[exp % 10]);
	if (ret < 0) {
		return DOUBLE_EXP_INVALID + ret;
	}
	exp /= 10;
	if (exp == 0) {
		ret = format_bufferAddChar(buff, smallDigits[0]);
		if (ret < 0) {
			return DOUBLE_EXP_INVALID + ret;
		}
	}
	else {
		while (exp != 0) {
			ret = format_bufferAddChar(buff, smallDigits[exp % 10]);
			if (ret < 0) {
				return DOUBLE_EXP_INVALID + ret;
			}
			exp /= 10;
		}
	}
	format_bufferReverse(buff->data, i, buff->len);
	return resexp;
}


static int format_sprintfDouble(void *ctx, feedfunc feed, double d, uint32_t flags, int minFieldWidth, int precision, char format)
{
	struct buffer buff;
	char sign = 0;
	const char *chosen, *infinity = (flags & FLAG_LARGE_DIGITS) ? "INF" : "inf",
						*notNumber = (flags & FLAG_LARGE_DIGITS) ? "NAN" : "nan";
	uint64_t num64 = format_u64FromDouble(d);
	unsigned int startOffset = 0;
	int i, prec, exp, ret = 0;
	struct bigdouble bd, bd_backup;

	if (format != 'a') {
		precision = precision >= 0 ? precision : 6;
	}
	else {
		precision = precision >= 0 ? precision : HEXDOUBLE_SUFFICIENT_PRECISION;
	}

	/* check sign */
	if (((num64 >> 63) & 1) != 0) {
		sign = '-';
		d = -d;
	}
	else if ((flags & FLAG_PLUS) != 0) {
		sign = '+';
	}
	else if ((flags & FLAG_SPACE) != 0) { /* If FLAG_PLUS and FLAG_SPACE both appear, then ignore SPACE */
		sign = ' ';
	}
	else {
		sign = '\0';
	}
	num64 = format_u64FromDouble(d);

	ret = format_bufferInit(&buff, 2 * precision + 6);
	if (ret < 0) {
		return ret;
	}

	/* check special cases */
	if (((num64 >> DOUBLE_EXP_SHIFT) & 0x7ff) == 0x7ff) {
		/* for NaN and infinity, flags '#' and '0' have no effect */
		flags &= ~(FLAG_ZERO | FLAG_ALTERNATE);
		if ((num64 & 0xfffffffffffff) == 0) {
			chosen = infinity;
		}
		else {
			chosen = notNumber;
		}
		format_printBuffer(ctx, feed, flags, minFieldWidth, chosen, chosen + strlen(chosen), sign);
		format_bufferDestroy(&buff);
		return 0;
	}

	/* check zero */
	if ((num64 & 0x7fffffffffffffff) == 0) {
		ret = format_bufferAddChar(&buff, '0');
		if (ret < 0) {
			format_bufferDestroy(&buff);
			return ret;
		}
		if (format == 'a') {
			ret = format_bufferAddChar(&buff, (flags & FLAG_LARGE_DIGITS) ? 'X' : 'x');
			if (ret < 0) {
				format_bufferDestroy(&buff);
				return ret;
			}
			ret = format_bufferAddChar(&buff, '0');
			if (ret < 0) {
				format_bufferDestroy(&buff);
				return ret;
			}
		}
		if (((flags & FLAG_ALTERNATE) != 0) || ((format != 'g') && (precision > 0))) {
			ret = format_bufferAddChar(&buff, '.');
			if (ret < 0) {
				format_bufferDestroy(&buff);
				return ret;
			}
			if (format == 'g') {
				precision = precision ? precision - 1 : 0;
			}
			for (i = 0; i < precision; i++) {
				ret = format_bufferAddChar(&buff, '0');
				if (ret != 0) {
					format_bufferDestroy(&buff);
					return ret;
				}
			}
		}
		if (format == 'e') {
			ret = format_bufferAddChar(&buff, (flags & FLAG_LARGE_DIGITS) ? 'E' : 'e');
			if (ret < 0) {
				format_bufferDestroy(&buff);
				return ret;
			}
			for (i = 0; i < 3; ++i) {
				ret = format_bufferAddChar(&buff, "+00"[i]);
				if (ret < 0) {
					format_bufferDestroy(&buff);
					return ret;
				}
			}
		}
		if (format == 'a') {
			ret = format_bufferAddChar(&buff, (flags & FLAG_LARGE_DIGITS) ? 'P' : 'p');
			if (ret < 0) {
				format_bufferDestroy(&buff);
				return ret;
			}
			for (i = 0; i < 2; ++i) {
				ret = format_bufferAddChar(&buff, "+0"[i]);
				if (ret < 0) {
					format_bufferDestroy(&buff);
					return ret;
				}
			}
		}
		format_printBuffer(ctx, feed, flags, minFieldWidth, buff.data, buff.data + buff.len, sign);
		format_bufferDestroy(&buff);
		return 0;
	}

	ret = format_bigdoubleLoad(&bd, d);
	if (ret < 0) {
		format_bufferDestroy(&buff);
		return ret;
	}

	if (format == 'f') {
		ret = format_sprintfDecimalForm(&buff, &bd, flags, precision, &startOffset);
	}
	else if (format == 'e') {
		ret = format_sprintfScientificForm(&buff, &bd, flags, precision, &startOffset);
		if (ret <= DOUBLE_EXP_INVALID) {
			ret -= DOUBLE_EXP_INVALID;
		}
		else {
			ret = 0;
		}
	}
	else if (format == 'g') {
		ret = format_bigdoubleInit(&bd_backup, BIGDOUBLE_DEFAULT_SIZE);
		if (ret < 0) {
			format_bufferDestroy(&buff);
			format_bigdoubleDestroy(&bd);
			return ret;
		}
		ret = bignum_copy(&bd_backup.num, &bd.num);
		if (ret < 0) {
			format_bigdoubleDestroy(&bd_backup);
			format_bufferDestroy(&buff);
			format_bigdoubleDestroy(&bd);
			return ret;
		}
		bd_backup.firstIntegerBit = bd.firstIntegerBit;

		prec = precision ? precision : 1;
		exp = format_sprintfScientificForm(&buff, &bd_backup, flags, prec - 1, &startOffset);
		if (exp <= DOUBLE_EXP_INVALID) {
			ret = exp - DOUBLE_EXP_INVALID;
		}
		else {
			if ((prec > exp) && (exp >= -4)) {
				buff.len = 0;
				precision = prec - (exp + 1);
				ret = format_sprintfDecimalForm(&buff, &bd, flags, precision, &startOffset);
			}
		}
		format_bigdoubleDestroy(&bd_backup);
	}
	else {
		ret = format_sprintfHexadecimalForm(&buff, d, flags, precision, &startOffset);
	}
	if (ret == 0) {
		format_printBuffer(ctx, feed, flags, minFieldWidth, buff.data + startOffset, buff.data + buff.len, sign);
	}
	format_bufferDestroy(&buff);
	format_bigdoubleDestroy(&bd);
	return ret;
}


static void format_sprintf_num(void *ctx, feedfunc feed, uint64_t num64, uint32_t flags, int minFieldWidth, int precision)
{
	const char *digits = (flags & FLAG_LARGE_DIGITS) ? largeDigits : smallDigits,
			   *prefix = (flags & FLAG_LARGE_DIGITS) ? "X0" : "x0";
	char tmp_buf[32];
	char sign = 0;
	char *tmp = tmp_buf;
	int i;
	uint32_t num32 = (uint32_t)num64;
	uint32_t num_high = (uint32_t)(num64 >> 32);

	if (((flags & FLAG_NULLMARK) != 0) && (num64 == 0)) {
		flags &= ~FLAG_ZERO;
		format_printBuffer(ctx, feed, flags, minFieldWidth, FORMAT_NIL_STR, FORMAT_NIL_STR + FORMAT_NIL_STR_LEN, sign);
		return;
	}

	if ((flags & FLAG_SIGNED) != 0) {

		if ((flags & FLAG_8BIT) != 0) {
			if ((int8_t)num32 < 0) {
				num32 = -(int8_t)num32;
				sign = '-';
			}
		}
		else if ((flags & FLAG_16BIT) != 0) {
			if ((int16_t)num32 < 0) {
				num32 = -(int16_t)num32;
				sign = '-';
			}
		}
		else if ((flags & FLAG_64BIT) != 0) {
			if ((int32_t)num_high < 0) {
				num64 = -(int64_t)num64;
				num32 = (uint32_t)num64;
				num_high = (uint32_t)(num64 >> 32);
				sign = '-';
			}
		}
		else {
			if ((int32_t)num32 < 0) {
				num32 = -(int32_t)num32;
				sign = '-';
			}
		}

		if (sign == 0) {
			if ((flags & FLAG_PLUS) != 0) {
				sign = '+';
			}
			else if ((flags & FLAG_SPACE) != 0) {
				sign = ' ';
			}
		}
	}

	if (((flags & FLAG_64BIT) != 0) && (num_high == 0x0)) {
		flags &= ~FLAG_64BIT;
	}

	if (num64 == 0) {
		if (precision > 0) {
			*tmp++ = '0';
		}
	}
	else if ((flags & FLAG_HEX) != 0) {
		if ((flags & FLAG_64BIT) != 0) {
			for (i = 0; i < 8; ++i) {
				*tmp++ = digits[num32 & 0x0f];
				num32 >>= 4;
			}
			while (num_high != 0) {
				*tmp++ = digits[num_high & 0x0f];
				num_high >>= 4;
			}
		}
		else {
			while (num32 != 0) {
				*tmp++ = digits[num32 & 0x0f];
				num32 >>= 4;
			}
		}
	}
	else if ((flags & FLAG_OCT) != 0) {
		if ((flags & FLAG_64BIT) != 0) {
			// 30 bits
			for (i = 0; i < 10; ++i) {
				*tmp++ = digits[num32 & 0x07];
				num32 >>= 3;
			}
			// 31, 32 bit from num32, bit 0 from num_high
			num32 |= (num_high & 0x1) << 2;
			*tmp++ = digits[num32 & 0x07];
			num_high >>= 1;

			while (num_high != 0) {
				*tmp++ = digits[num_high & 0x07];
				num_high >>= 3;
			}
		}
		else {
			while (num32 != 0) {
				*tmp++ = digits[num32 & 0x07];
				num32 >>= 3;
			}
		}
	}
	else {
		if ((flags & FLAG_64BIT) != 0) {  // TODO: optimize
			while (num64 != 0) {
				*tmp++ = digits[num64 % 10];
				num64 /= 10;
			}
		}
		else {
			while (num32 != 0) {
				*tmp++ = digits[num32 % 10];
				num32 /= 10;
			}
		}
	}

	if (((flags & FLAG_OCT) != 0) && ((num64 != 0) || (precision == 0)) && ((flags & FLAG_ALTERNATE) != 0)) {
		*tmp++ = '0';
	}

	if (tmp - tmp_buf < precision) {
		memset(tmp, '0', precision - (tmp - tmp_buf));
		tmp = tmp_buf + precision;
	}

	if (((flags & FLAG_HEX) != 0) && (num64 != 0) && ((flags & FLAG_ALTERNATE) != 0)) {
		*tmp++ = prefix[0];
		*tmp++ = prefix[1];
	}
	format_printBuffer(ctx, feed, flags, minFieldWidth, tmp, tmp_buf, sign);
}


int format_parse(void *ctx, feedfunc feed, const char *format, va_list args)
{
	uint64_t number;
	const char *s;
	char *p;
	uint32_t flags, minFieldWidth;
	int precision, length;
	char c, fmt;
	double doubleNumber;
	int ret = 0;
	for (;;) {
		fmt = *format++;

		if (!fmt) {
			break;
		}

		if (fmt != '%') {
			feed(ctx, fmt);
			continue;
		}

		fmt = *format++;
		if (fmt == '\0') {
			feed(ctx, '%');
			break;
		}

		/* precision, padding (set default to 6 digits) */
		flags = 0, minFieldWidth = 0;
		precision = -1;

		for (;;) {
			if (fmt == ' ') {
				flags |= FLAG_SPACE;
			}
			else if (fmt == '-') {
				flags |= FLAG_MINUS;
			}
			else if (fmt == '0') {
				flags |= FLAG_ZERO;
			}
			else if (fmt == '+') {
				flags |= FLAG_PLUS;
			}
			else if (fmt == '#') {
				flags |= FLAG_ALTERNATE;
			}
			else if (fmt == '*') {
				flags |= FLAG_FIELD_WIDTH_STAR;
			}
			else {
				break;
			}

			fmt = *format++;
		}
		if (fmt == '\0') {
			break;
		}

		/* leading number digits-cnt */
		while ((fmt >= '0') && (fmt <= '9')) {
			minFieldWidth = minFieldWidth * 10 + fmt - '0';
			fmt = *format++;
		}

		if ((flags & FLAG_FIELD_WIDTH_STAR) != 0) {
			minFieldWidth = va_arg(args, int);
		}

		if (fmt == '\0') {
			break;
		}

		/* fractional number digits-cnt (only a single digit is acceptable in this impl.) */
		if (fmt == '.') {
			precision = 0;
			fmt = *format++;
			while ((fmt >= '0') && (fmt <= '9')) {
				precision = precision * 10 + fmt - '0';
				fmt = *format++;
			}
		}
		if (fmt == '\0') {
			break;
		}

		if (fmt == '*') {
			precision = va_arg(args, int);
			fmt = *format++;

			if (fmt == '\0') {
				break;
			}
		}

		if (fmt == 'L') {
			fmt = *format++;
			flags |= FLAG_LONG_DOUBLE;
		}

		if (fmt == 'h') {
			fmt = *format++;
			if (fmt == 'h') {
				fmt = *format++;
				flags |= FLAG_8BIT;
			}
			else {
				flags |= FLAG_16BIT;
			}
		}

		if (fmt == 'l') {
			fmt = *format++;
			if (sizeof(long int) == sizeof(int64_t)) {
				flags |= FLAG_64BIT;
			}

			if (fmt == 'l') {
				flags |= FLAG_64BIT;
				fmt = *format++;
			}
		}

		if (fmt == 'z') {
			fmt = *format++;
			if (sizeof(size_t) == sizeof(uint64_t)) {
				flags |= FLAG_64BIT;
			}
		}

		if (fmt == 't') {
			fmt = *format++;
			if (sizeof(ptrdiff_t) == sizeof(int64_t)) {
				flags |= FLAG_64BIT;
			}
		}

		if (fmt == 'j') {
			fmt = *format++;
			flags |= FLAG_64BIT;
		}
		if (fmt == '\0') {
			break;
		}

		number = 0;
		switch (fmt) {
			case 's':
				flags &= ~(FLAG_ZERO | FLAG_ALTERNATE | FLAG_PLUS | FLAG_SPACE);
				s = va_arg(args, char *);
				if (s == NULL) {
					s = "(null)";
				}

				if (precision >= 0) {
					p = memchr(s, 0, precision);
					if (p != NULL) {
						length = p - s;
					}
					else {
						length = precision;
					}
				}
				else {
					length = strlen(s);
				}

				format_printBuffer(ctx, feed, flags, minFieldWidth, s, s + length, 0);
				break;
			case 'c':
				c = (char)va_arg(args, int);
				format_printBuffer(ctx, feed, flags, minFieldWidth, &c, &c + 1, 0);
				break;
			case 'p':
				flags |= (FLAG_HEX | FLAG_NULLMARK | FLAG_ZERO);
				if (sizeof(void *) == sizeof(uint64_t)) {
					flags |= FLAG_64BIT;
				}
				minFieldWidth = sizeof(void *) * 2;
				GET_UNSIGNED(number, flags, args);
				format_sprintf_num(ctx, feed, number, flags, minFieldWidth, precision);
				break;
			case 'o':
				if (precision != -1) {
					flags &= ~FLAG_ZERO;
				}
				else {
					precision = 1;
				}
				flags |= FLAG_OCT;
				GET_UNSIGNED(number, flags, args);
				format_sprintf_num(ctx, feed, number, flags, minFieldWidth, precision);
				break;
			case 'X':
				flags |= FLAG_LARGE_DIGITS;
			case 'x':
				flags |= FLAG_HEX;
			case 'u':
				if (precision != -1) {
					flags &= ~FLAG_ZERO;
				}
				else {
					precision = 1;
				}
				GET_UNSIGNED(number, flags, args);
				format_sprintf_num(ctx, feed, number, flags, minFieldWidth, precision);
				break;
			case 'd':
			case 'i':
				if (precision != -1) {
					flags &= ~FLAG_ZERO;
				}
				else {
					precision = 1;
				}
				flags |= FLAG_SIGNED;
				GET_SIGNED(number, flags, args);
				format_sprintf_num(ctx, feed, number, flags, minFieldWidth, precision);
				break;
			case 'A':
				flags |= FLAG_LARGE_DIGITS;
			case 'a':
				GET_DOUBLE(doubleNumber, flags, args);
				ret = format_sprintfDouble(ctx, feed, doubleNumber, flags, minFieldWidth, precision, 'a');
				if (ret < 0) {
					return ret;
				}
				break;
			case 'E':
				flags |= FLAG_LARGE_DIGITS;
			case 'e':
				GET_DOUBLE(doubleNumber, flags, args);
				ret = format_sprintfDouble(ctx, feed, doubleNumber, flags, minFieldWidth, precision, 'e');
				if (ret < 0) {
					return ret;
				}
				break;
			case 'G':
				flags |= FLAG_LARGE_DIGITS;
			case 'g':
				flags |= FLAG_NO_TRAILING_ZEROS;
				GET_DOUBLE(doubleNumber, flags, args);
				ret = format_sprintfDouble(ctx, feed, doubleNumber, flags, minFieldWidth, precision, 'g');
				if (ret < 0) {
					return ret;
				}
				break;
			case 'F':
				flags |= FLAG_LARGE_DIGITS;
			case 'f':
				GET_DOUBLE(doubleNumber, flags, args);
				ret = format_sprintfDouble(ctx, feed, doubleNumber, flags, minFieldWidth, precision, 'f');
				if (ret < 0) {
					return ret;
				}
				break;
			case '%':
				feed(ctx, '%');
				break;
			default:
				feed(ctx, '%');
				feed(ctx, fmt);
				break;
		}
	}
	return ret;
}

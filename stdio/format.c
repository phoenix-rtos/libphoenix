/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * format.c
 *
 * Copyright 2017 Phoenix Systems
 * Author: Adrian Kepka
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "format.h"
#include "stdint.h"
#include "string.h"


#define FLAG_SIGNED        0x1
#define FLAG_64BIT         0x2
#define FLAG_FLOAT         0x4
#define FLAG_SPACE         0x10
#define FLAG_ZERO          0x20
#define FLAG_PLUS          0x40
#define FLAG_HEX           0x80
#define FLAG_LARGE_DIGITS  0x100
#define FLAG_ALTERNATE     0x200
#define FLAG_NULLMARK      0x400
#define FLAG_MINUS         0x1000
#define FLAG_STAR          0x2000


#define GET_UNSIGNED(number, flags, args) do {		\
		if ((flags) & FLAG_64BIT)		\
			(number) = va_arg((args), u64);	\
		else					\
			(number) = va_arg((args), u32);	\
	} while (0)


#define GET_SIGNED(number, flags, args) do {		\
		if ((flags) & FLAG_64BIT)		\
			(number) = va_arg((args), s64);	\
		else					\
			(number) = va_arg((args), s32);	\
	} while (0)


union float_u32
{
	float f;
	u32 u;
};


static inline float format_floatFromU32(u32 ui)
{
	union float_u32 u;

	u.u = ui;
	return u.f;
}


static inline u32 format_u32FromFloat(float f)
{
	union float_u32 u;

	u.f = f;
	return u.u;
}


static inline float format_modff(float x, float *integral_out)
{
	int h;
	float high_int, low, low_int, frac;

	if (x > 1048576.0f) {

		h = (int)(x / 1048576.0f);
		high_int = (float)h * 1048576.0f;
		low = x - high_int;

		low_int = (float)(int)low;
		frac = low - low_int;
		*integral_out = high_int * 1048576.0f + low_int;
		return frac;
	}

	low_int = (float)(int)x;
	frac = x - low_int;
	*integral_out = low_int;

	return frac;
}


static inline u32 format_fracToU32(float frac, int float_frac_len, float *overflow)
{
	const u32 s_powers10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, };
	const u32 mult = s_powers10[float_frac_len];

	frac *= (float)mult;

	/* Ensure proper rounding */
	frac += 0.5f;
	u32 ret = (u32)(s32)frac;

	if (ret >= mult) {
		*overflow += 1;
		ret -= mult;
	}
	return ret;
}

static void format_sprintf_num(void *ctx, feedfunc feed, u64 num64, u32 flags, int min_number_len, int float_frac_len)
{
	const char *digits = (flags & FLAG_LARGE_DIGITS) ? "0123456789ABCDEF" : "0123456789abcdef",
			*prefix = (flags & FLAG_LARGE_DIGITS) ? "X0" : "x0";
	char tmp_buf[32];
	char sign = 0;
	char *tmp = tmp_buf;

	if ((flags & FLAG_NULLMARK) && num64 == 0) {
		const char *s = "(nil)";
		int i;
		for(i = 0; i < strlen(s); i++)
			feed(ctx, s[i]);
	}

	if (flags & FLAG_FLOAT) {
		float f = format_floatFromU32((u32)num64);

		if (f < 0) {
			sign = '-';
			f = -f;
		}

		float integral;
		float frac = format_modff(f, &integral);

		/* max decimal digits to fit into uint32 : 9 */
		if (float_frac_len > 9)
			float_frac_len = 9;

		u32 frac32 = format_fracToU32(frac, float_frac_len, &integral);
		int frac_left = float_frac_len;

		while (frac_left-- > 0) {
			*tmp++ = digits[frac32 % 10];
			frac32 /= 10;
		}

		if (float_frac_len > 0 || (flags & FLAG_ALTERNATE))
			*tmp++ = '.';

		if (integral < 4294967296.0f)
			num64 = (u32)integral;
		else {
			float higher_part_f = (integral / 4294967296.0f);
			u32 higher_part = (u32)higher_part_f;
			u32 lower_part = (u32)(integral - (higher_part * 4294967296.0f));

			num64 = (((u64)higher_part) << 32) | lower_part;
			flags |= FLAG_64BIT;
		}
	}

	u32 num32 = (u32)num64;
	u32 num_high = (u32)(num64 >> 32);

	if (flags & FLAG_SIGNED) {

		if (flags & FLAG_64BIT) {

			if ((s32)num_high < 0) {
				num64 = -(s64)num64;
				num32 = (u32)num64;
				num_high = (u32)(num64 >> 32);
				sign = '-';
			}
		}
		else {
			if ((s32)num32 < 0) {
				num32 = -(s32)num32;
				sign = '-';
			}
		}

		if (sign == 0) {
			if (flags & FLAG_SPACE) {
				sign = ' ';
			}
			else if (flags & FLAG_PLUS) {
				sign = '+';
			}
		}
	}

	if ((flags & FLAG_64BIT) && num_high == 0x0)
		flags &= ~FLAG_64BIT;

	if (num64 == 0) {
		*tmp++ = '0';
	}
	else if (flags & FLAG_HEX) {
		if (flags & FLAG_64BIT) {
			int i;
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
		if (flags & FLAG_ALTERNATE) {
			memcpy(tmp, prefix, 2);
			tmp += 2;
		}
	}
	else {
		if (flags & FLAG_64BIT) { // TODO: optimize
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

	const int digits_cnt = tmp - tmp_buf;
	int pad_len = min_number_len - digits_cnt - (sign ? 1 : 0);

	/* pad, if needed */
	if (pad_len > 0 && !(flags & FLAG_ZERO)) {
		while (pad_len-- > 0)
			feed(ctx, ' ');
	}

	if (sign)
		feed(ctx, sign);

	/* pad, if needed */
	if (pad_len > 0 && (flags & FLAG_ZERO)) {
		while (pad_len-- > 0)
			feed(ctx, '0');
	}

	/* copy reversed */
	while ((--tmp) >= tmp_buf)
		feed(ctx, *tmp);
}


void format_parse(void *ctx, feedfunc feed, const char *format, va_list args)
{
	for (;;) {
		char fmt = *format++;

		if (!fmt)
			break;

		if (fmt != '%') {
			feed(ctx, fmt);
			continue;
		}

		fmt = *format++;
		if (fmt == 0) {
			feed(ctx, '%');
			break;
		}

		/* precission, padding (set default to 6 digits) */
		u32 flags = 0, min_number_len = 0, float_frac_len = 6;

		for (;;) {
			if (fmt == ' ')
				flags |= FLAG_SPACE;
			else if (fmt == '-')
				flags |= FLAG_MINUS;
			else if (fmt == '0')
				flags |= FLAG_ZERO;
			else if (fmt == '+')
				flags |= FLAG_PLUS;
			else if (fmt == '#')
				flags |= FLAG_ALTERNATE;
			else if (fmt == '*')
				flags |= FLAG_STAR;
			else
				break;

			fmt = *format++;
		}
		if (fmt == 0)
			break;

		/* leading number digits-cnt */
		while (fmt >= '0' && fmt <= '9') {
			min_number_len = min_number_len * 10 + fmt - '0';
			fmt = *format++;
		}
		if (fmt == 0)
			break;

		/* fractional number digits-cnt (only a single digit is acceptable in this impl.) */
		if (fmt == '.') {
			fmt = *format++;
			if (fmt >= '0' && fmt <= '9') {
				float_frac_len  = fmt - '0';
				fmt = *format++;
			}
		}
		if (fmt == 0)
			break;

		if (fmt == 'l') {
			fmt = *format++;

			if (fmt == 'l') {
				flags |= FLAG_64BIT;
				fmt = *format++;
			}
		}
		if (fmt == 0)
			break;

		if (fmt == 'z') {
			fmt = *format++;
			if (sizeof(void *) == sizeof(u64)) // FIXME "size_t" is undefined?
				flags |= FLAG_64BIT;
		}
		if (fmt == 0)
			break;

		u64 number = 0;
		switch (fmt) {
			case 's':
			{
				if (flags & FLAG_STAR)
					min_number_len = va_arg(args, int);

				const char *s = va_arg(args, char *);
				if (s == NULL)
					s = "(null)";

				int i;
				int l = strlen(s);

				if (l < min_number_len && !(flags & FLAG_MINUS)) {
					for(i = 0; i < (min_number_len - l); i++)
						feed(ctx, ' ');
				}

				for(i = 0; i < l; i++)
					feed(ctx, s[i]);

				if (l < min_number_len && (flags & FLAG_MINUS)) {
					for(i = 0; i < (min_number_len - l); i++)
						feed(ctx, ' ');
				}

				break;
			}
			case 'c':
			{
				const char c = (char) va_arg(args,int);
				feed(ctx, c);

				break;
			}
			case 'p':
				flags |= (FLAG_HEX | FLAG_NULLMARK | FLAG_ZERO);
				if (sizeof(void *) == sizeof(u64))
					flags |= FLAG_64BIT;
				min_number_len = sizeof(void *) * 2;
				GET_UNSIGNED(number, flags, args);
				format_sprintf_num(ctx, feed, number, flags, min_number_len, float_frac_len);
				break;
			case 'X':
				flags |= FLAG_LARGE_DIGITS;
			case 'x':
				flags |= FLAG_HEX;
			case 'u':
				GET_UNSIGNED(number, flags, args);
				format_sprintf_num(ctx, feed, number, flags, min_number_len, float_frac_len);
				break;
			case 'd':
			case 'i':
				flags |= FLAG_SIGNED;
				GET_SIGNED(number, flags, args);
				format_sprintf_num(ctx, feed, number, flags, min_number_len, float_frac_len);
				break;
			case 'f':
				flags |= FLAG_FLOAT;
				number = format_u32FromFloat((float)va_arg(args, double));
				format_sprintf_num(ctx, feed, number, flags, min_number_len, float_frac_len);
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

	feed(ctx, '\0');
}

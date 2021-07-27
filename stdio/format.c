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

#include <sys/minmax.h>
#include "format.h"
#include <sys/minmax.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


#define FLAG_SIGNED        0x1
#define FLAG_64BIT         0x2
#define FLAG_FLOAT         0x4
#define FLAG_SPACE         0x10
#define FLAG_ZERO          0x20
#define FLAG_PLUS          0x40
#define FLAG_HEX           0x80
#define FLAG_OCT           0x100
#define FLAG_LARGE_DIGITS  0x200
#define FLAG_ALTERNATE     0x400
#define FLAG_NULLMARK      0x800
#define FLAG_MINUS         0x1000
#define FLAG_FIELD_WIDTH_STAR 	0x2000
#define FLAG_DOUBLE		   0x4000


#define GET_UNSIGNED(number, flags, args) do {		\
		if ((flags) & FLAG_64BIT)		\
			(number) = va_arg((args), uint64_t);	\
		else					\
			(number) = va_arg((args), uint32_t);	\
	} while (0)


#define GET_SIGNED(number, flags, args) do {		\
		if ((flags) & FLAG_64BIT)		\
			(number) = va_arg((args), int64_t);	\
		else					\
			(number) = va_arg((args), int32_t);	\
	} while (0)


union float_u32
{
	float f;
	uint32_t u;
};

union double_u64
{
	double d;
	uint64_t u;
};

static inline double format_doubleFromU64(uint64_t ui)
{
	union double_u64 u;

	u.u = ui;
	return u.d;
}

static inline uint64_t format_u64FromDouble(double d)
{
	union double_u64 u;

	u.d = d;
	return u.u;
}

static inline float format_floatFromU32(uint32_t ui)
{
	union float_u32 u;

	u.u = ui;
	return u.f;
}


static inline uint32_t format_u32FromFloat(float f)
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


static inline uint32_t format_fracToU32(float frac, int float_frac_len, float *overflow)
{
	const uint32_t s_powers10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, };
	const uint32_t mult = s_powers10[float_frac_len];

	frac *= (float)mult;

	/* Ensure proper rounding */
	frac += 0.5f;
	uint32_t ret = (uint32_t)(int32_t)frac;

	if (ret >= mult) {
		*overflow += 1;
		ret -= mult;
	}
	return ret;
}


static void format_sprintf_num(void *ctx, feedfunc feed, uint64_t num64, uint32_t flags, int min_number_len, int float_frac_len)
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

	if (flags & FLAG_DOUBLE) {
		double d = format_doubleFromU64(num64), exp_val;
		/* exponent, exponent sign, trailing zeros, fraction lenght, integer lenght, temp */
		int exp = 0, exps = 0, tz = 1, frac_len, int_len, temp, i;
		/* fraction, temporary vars */
		long long frac, temp1, temp2;

		/* check sign */
		if ((num64 >> 63) & 1) {
			sign = '-';
			d = -d;
		}
		num64 = format_u64FromDouble(d);

		/* check zero */
		if (!(num64 & 0x7fffffffffffffff)) {
			if (sign)
				feed(ctx, sign);
			feed(ctx, '0');
			sign = 0;
			return;
		}

		/* check special cases */
		if (((num64 >> 52) & 0x7ff) == 0x7ff) {
			const char *s;
			int i;
			if (!(num64 & 0xfffffffffffff)) {
				/* infinity */
				s = "inf";
				if (sign)
					feed(ctx, sign);
			}
			else
				s = "NaN";

			for (i = 0; i < strlen(s); i++)
				feed(ctx, s[i]);
			sign = 0;
			return;
		}

		frac_len = float_frac_len ? float_frac_len : 1;
		int_len = 1;

		if ((num64 >> 52) < 0x3ff) {

			temp = ((int)((num64 >> 52) & 0x7ff) - 1023) / -4;

			exp_val = 10;

			for (i = 0; i < 9; i++) {
				if ((temp >> i) & 1)
					d *= exp_val;
				exp_val *= exp_val;
			}

			temp1 = d;

			num64 = format_u64FromDouble(d);
			/* shift fraction left until it's >= 1*/
			while ((num64 >> 52) < 0x3ff) {
				exp++;
				d *= 10;
				num64 = format_u64FromDouble(d);
			}
			exp = temp + exp;
		}

		if (exp == 0) {
			/* no shifting - number is larger than 1 */
			exps = 1;

			temp = ((int)((num64 >> 52) & 0x7ff) - 1023) / 4;

			exp_val = 10;

			for (i = 0; i < 9; i++) {
				if ((temp >> i) & 1)
					d /= exp_val;
				exp_val *= exp_val;
			}

			num64 = format_u64FromDouble(d);
			while((num64 >> 52) >= 0x3ff) {
				exp++;
				d /= 10;
				num64 = format_u64FromDouble(d);
			}

			d *= 10;
			exp--;
			exp = temp + exp;
			if (exp < float_frac_len) {
				int_len += exp;
				frac_len -= exp;
				while (exp--)
					d *= 10;
				exp = 0;
				frac = d;
			}
		}
		temp = frac_len;

		while (frac_len--)
			d *= 10;

		frac_len = temp - 1;
		frac = d;
		temp1 = frac / 10;

		/* rounding */
		frac += 5;
		frac /= 10;
		temp2 = frac;

		while (frac_len--) {
			temp1 /= 10;
			temp2 /= 10;
		}

		frac_len = temp - 1;
		temp = int_len - 1;

		while(temp--) {
			temp1 /= 10;
			temp2 /= 10;
		}

		if (temp2 > temp1) {
			frac /= 10;
			if (exps == 1)
				exp++;
			else
				exp--;
		}

		if (exp > frac_len || (!exps & (exp >= 5))) {
			*tmp++ = digits[exp % 10];
			exp /= 10;
			if (!exp)
				*tmp++ = '0';
			else {
				while (exp) {
					*tmp++ = digits[exp % 10];
					exp /= 10;
				}
			}

			/* check exp sign */
			if (exps)
				*tmp++ ='+';
			else
				*tmp++ ='-';
			*tmp++ ='e';
		}

		while (frac_len--) {
			if (frac % 10 || !tz) {
				tz = 0;
				*tmp++ = digits[frac % 10];
			}
			frac /= 10;
		}

		if (exp) {
			tz = 0;
			*tmp++ = digits[frac % 10];
			frac /= 10;
			while (--exp)
				*tmp++ = '0';
		}

		if (!tz)
			*tmp++ = '.';

		while (int_len--) {
			*tmp++ = digits[frac % 10];
			frac /= 10;
		}
		/* we need num32 to be 0 and num64 to not be 0 so the rest of the function won't mess our conversion */
		num64 = 0x100000000;
	}

	if (flags & FLAG_FLOAT) {
		float f = format_floatFromU32((uint32_t)num64);

		if (f < 0) {
			sign = '-';
			f = -f;
		}

		float integral;
		float frac = format_modff(f, &integral);

		/* max decimal digits to fit into uint32 : 9 */
		if (float_frac_len > 9)
			float_frac_len = 9;

		uint32_t frac32 = format_fracToU32(frac, float_frac_len, &integral);
		int frac_left = float_frac_len;

		while (frac_left-- > 0) {
			*tmp++ = digits[frac32 % 10];
			frac32 /= 10;
		}

		if (float_frac_len > 0 || (flags & FLAG_ALTERNATE))
			*tmp++ = '.';

		if (integral < 4294967296.0f)
			num64 = (uint32_t)integral;
		else {
			float higher_part_f = (integral / 4294967296.0f);
			uint32_t higher_part = (uint32_t)higher_part_f;
			uint32_t lower_part = (uint32_t)(integral - (higher_part * 4294967296.0f));

			num64 = (((uint64_t)higher_part) << 32) | lower_part;
			flags |= FLAG_64BIT;
		}
	}

	uint32_t num32 = (uint32_t)num64;
	uint32_t num_high = (uint32_t)(num64 >> 32);

	if (flags & FLAG_SIGNED) {

		if (flags & FLAG_64BIT) {

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
	else if (flags & FLAG_OCT) {
		if (flags & FLAG_64BIT) {
			int i;
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
		if (flags & FLAG_ALTERNATE) {
			*tmp++  = '0';
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
		uint32_t flags = 0, min_number_len = 0;
		int	float_frac_len = -1;

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
				flags |= FLAG_FIELD_WIDTH_STAR;
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

		if (flags & FLAG_FIELD_WIDTH_STAR)
			min_number_len = va_arg(args, int);

		if (fmt == 0)
			break;

		/* fractional number digits-cnt (only a single digit is acceptable in this impl.) */
		if (fmt == '.') {
			float_frac_len = 0;
			fmt = *format++;
			while (fmt >= '0' && fmt <= '9') {
				float_frac_len  = float_frac_len * 10 + fmt - '0';
				fmt = *format++;
			}
		}
		if (fmt == 0)
			break;

		if (fmt == '*') {
			float_frac_len = va_arg(args, int);
			fmt = *format++;

			if (fmt == 0)
				break;
		}

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
			if (sizeof(size_t) == sizeof(uint64_t))
				flags |= FLAG_64BIT;
		}
		if (fmt == 0)
			break;

		if (fmt == 'j') {
			fmt = *format++;
			flags |= FLAG_64BIT;
		}
		if (fmt == 0)
			break;

		uint64_t number = 0;
		switch (fmt) {
			case 's':
			{
				const char *s = va_arg(args, char *);
				if (s == NULL)
					s = "(null)";

				int i, l;
				if (min_number_len > 0) {
					char *p = memchr(s, 0, min_number_len);
					if (p != NULL)
						l = p - s;
					else
						l = min_number_len;
				} else {
					l = strlen(s);
				}

				if (float_frac_len >= 0)
					l = min(l, float_frac_len);

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
				if (sizeof(void *) == sizeof(uint64_t))
					flags |= FLAG_64BIT;
				min_number_len = sizeof(void *) * 2;
				GET_UNSIGNED(number, flags, args);
				format_sprintf_num(ctx, feed, number, flags, min_number_len, float_frac_len);
				break;
			case 'o':
				flags |= FLAG_OCT;
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
			case 'g':
				flags |= FLAG_DOUBLE;
				float_frac_len = float_frac_len >= 0 ? float_frac_len : 6;
				number = format_u64FromDouble((double)va_arg(args, double));
				format_sprintf_num(ctx, feed, number, flags, min_number_len, float_frac_len);
				break;
			case 'f':
				flags |= FLAG_FLOAT;
				float_frac_len = (float_frac_len >= 0) ? float_frac_len : 6;
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
}

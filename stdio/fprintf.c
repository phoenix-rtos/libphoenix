/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * printf.c
 *
 * Copyright 2017, 2022 Phoenix Systems
 * Author: Adrian Kepka, Gerard Swiderski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "format.h"
#include "sys/debug.h"


/* clang-format off */
struct feed_ctx_s {
	/* NOTE: Structure layout optimized for 32bit, and 64bit arch */
	union {
		FILE *stream;
		int fd;
	} h;
	size_t n;
	size_t total;
	int error;
	enum { feed_hStream = 0, feed_hDescriptor } hType;
	char buff[16];
};
/* clang-format on */


static size_t safe_write(int fd, const char *buff, size_t size)
{
	size_t todo = size;
	ssize_t wlen;

	while (todo != 0) {
		wlen = write(fd, buff, todo);
		if (wlen > 0) {
			todo -= wlen;
			buff += wlen;
			continue;
		}
		else if (wlen < 0) {
			if ((errno == EINTR) || (errno == EAGAIN)) {
				continue;
			}
			break;
		}
		else {
			/* Undefined behaviour (wlen==0) */
			errno = EIO;
			break;
		}
	}

	return size - todo;
}


static void format_feed(void *context, char c)
{
	size_t res;
	struct feed_ctx_s *ctx = (struct feed_ctx_s *)context;

	if (ctx->error < 0) {
		return;
	}

	ctx->buff[ctx->n++] = c;
	ctx->buff[ctx->n] = '\0';

	if (ctx->n == sizeof(ctx->buff) - 1) {
		res = (ctx->hType == feed_hStream) ?
			fwrite(ctx->buff, 1, ctx->n, ctx->h.stream) :
			safe_write(ctx->h.fd, ctx->buff, ctx->n);

		ctx->total += res;
		if (ctx->n != res) {
			ctx->error = (ctx->hType == feed_hStream ? -1 : -errno);
		}

		ctx->n = 0;
	}
}


int fprintf(FILE *stream, const char *format, ...)
{
	int err;
	va_list arg;

	va_start(arg, format);
	err = vfprintf(stream, format, arg);
	va_end(arg);

	return err;
}


int vfprintf(FILE *stream, const char *format, va_list arg)
{
	struct feed_ctx_s ctx;
	size_t res;
	int ret;

	ctx.hType = feed_hStream;
	ctx.h.stream = stream;
	ctx.n = 0;
	ctx.total = 0;
	ctx.error = 0;

	ret = format_parse(&ctx, format_feed, format, arg);

	if ((ret == 0) && (ctx.n != 0)) {
		res = fwrite(ctx.buff, 1, ctx.n, ctx.h.stream);
		ctx.total += res;

		if (ctx.n != res) {
			ctx.error = -1;
		}
	}
	if (ret != 0) {
		return SET_ERRNO(ret);
	}
	else {
		return ctx.error < 0 ? -1 : /* check error with feof() or ferror() */ ctx.total;
	}
}


int vdprintf(int fd, const char *format, va_list arg)
{
	struct feed_ctx_s ctx;
	size_t res;
	int ret;

	ctx.hType = feed_hDescriptor;
	ctx.h.fd = fd;
	ctx.n = 0;
	ctx.total = 0;
	ctx.error = 0;

	ret = format_parse(&ctx, format_feed, format, arg);

	if ((ret == 0) && (ctx.n != 0)) {
		res = safe_write(ctx.h.fd, ctx.buff, ctx.n);
		ctx.total += res;

		if (ctx.n != res) {
			ctx.error = -errno;
		}
	}

	if (ret != 0) {
		return SET_ERRNO(ret);
	}
	else {
		return ctx.error < 0 ? SET_ERRNO(ctx.error) : ctx.total;
	}
}


int dprintf(int fd, const char *format, ...)
{
	int retVal;
	va_list ap;

	va_start(ap, format);
	retVal = vdprintf(fd, format, ap);
	va_end(ap);

	return retVal;
}


#if 0

#define FLAG_SIGNED       0x1
#define FLAG_64BIT        0x2
#define FLAG_FLOAT        0x4
#define FLAG_SPACE        0x10
#define FLAG_ZERO         0x20
#define FLAG_PLUS         0x40
#define FLAG_HEX          0x80
#define FLAG_LARGE_DIGITS 0x100
#define FLAG_ALTERNATE    0x200
#define FLAG_NULLMARK     0x400


union float_u32
{
	float f;
	u32 u;
};


int fputs(const char *str, FILE *f)
{
	int len = strlen(s);
	char nl = '\n';

	write(f->fd, s, len);
	write(f->fd, &nl, 1);
	return EOK;
}


int fputc(int c, FILE *f)
{
	char b[2];
	int err;

	b[0] = c;
	b[1] = 0;

	err = fputs(b, f);
	return err;
}


int fflush(FILE *f)
{
	debug(f->buff);

	fileSend(f->h, f->buff);
	return EOK;
}


static inline float fprintf_u32tof(u32 ui)
{
	union float_u32 u;

	u.u = ui;
	return u.f;
}


static inline u32 fprintf_ftou32(float f)
{
	union float_u32 u;

	u.f = f;
	return u.u;
}


static inline float fprintf_modff(float x, float *integral_out)
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


static inline u32 fprintf_fractou32(float frac, int float_frac_len, float *overflow)
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


static void fprintf_num(FILE *f, u64 num64, u32 flags, int min_number_len, int float_frac_len)
{
	const char *digits = (flags & FLAG_LARGE_DIGITS) ? "0123456789ABCDEF" : "0123456789abcdef";
	const char *prefix = (flags & FLAG_LARGE_DIGITS) ? "X0" : "x0";
	char tmp_buf[32], sign = 0, *tmp = tmp_buf;

	if ((flags & FLAG_NULLMARK) && num64 == 0) {
		const char *s = "(nil)";
		fputs(s, f);
	}

	if (flags & FLAG_FLOAT) {
		float f = fprintf_u32tof((u32)num64);

		if (f < 0) {
			sign = '-';
			f = -f;
		}

		float integral;
		float frac = fprintf_modff(f, &integral);

		/* max decimal digits to fit into uint32 : 9 */
		if (float_frac_len > 9)
			float_frac_len = 9;

		u32 frac32 = fprintf_fractou32(frac, float_frac_len, &integral);
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
			fputc(' ', f);
	}

	if (sign)
		fputc(sign, f);

	/* pad, if needed */
	if (pad_len > 0 && (flags & FLAG_ZERO)) {
		while (pad_len-- > 0)
			fputc('0', f);
	}

	/* copy reversed */
	while ((--tmp) >= tmp_buf)
		fputc(*tmp, f);
}


int vfprintf(FILE *f, const char *format, va_list args)
{
	char fmt;
	offs_t start = f->pos;

	for (;;) {
		fmt = *format++;

		if (!fmt)
			break;

		if (fmt != '%') {
			fputc(fmt, f);
			continue;
		}

		fmt = *format++;
		if (fmt == 0) {
			fputc('%', f);
			break;
		}

		/* precission, padding (set default to 6 digits) */
		u32 flags = 0, min_number_len = 0, float_frac_len = 6;

		for (;;) {
			if (fmt == ' ')
				flags |= FLAG_SPACE;
			else if (fmt == '0')
				flags |= FLAG_ZERO;
			else if (fmt == '+')
				flags |= FLAG_PLUS;
			else if (fmt == '#')
				flags |= FLAG_ALTERNATE;
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
				const char *s = va_arg(args, char *);
				if (s == NULL)
					s = "(null)";
				fputs(s, f);
				break;
			}
			case 'c':
			{
				const char c = (char) va_arg(args,int);
				fputc(c, f);
				break;
			}
			case 'p':
				flags |= (FLAG_HEX | FLAG_NULLMARK | FLAG_ZERO);
				if (sizeof(void *) == sizeof(u64))
					flags |= FLAG_64BIT;
				min_number_len = sizeof(void *) * 2;

				if ((flags) & FLAG_64BIT)
					number = va_arg((args), u64);
				else
					number = va_arg((args), u32);
				fprintf_num(f, number, flags, min_number_len, float_frac_len);
				break;
			case 'X':
				flags |= FLAG_LARGE_DIGITS;
			case 'x':
				flags |= FLAG_HEX;
			case 'u':
				if ((flags) & FLAG_64BIT)
					number = va_arg((args), u64);
				else
					number = va_arg((args), u32);
				fprintf_num(f, number, flags, min_number_len, float_frac_len);
				break;
			case 'd':
			case 'i':
				flags |= FLAG_SIGNED;
				if ((flags) & FLAG_64BIT)
					number = va_arg((args), s64);
				else
					number = va_arg((args), s32);
				fprintf_num(f, number, flags, min_number_len, float_frac_len);
				break;
			case 'f':
				flags |= FLAG_FLOAT;
				number = fprintf_ftou32((float)va_arg(args, double));
				fprintf_num(f, number, flags, min_number_len, float_frac_len);
				break;
			case '%':
				fputc('%', f);
				break;
			default:
				fputc('%', f);
				fputc(fmt, f);
				break;
		}
	}
	fputc('\0', f);

	return (f->pos - start);
}


int vsprintf(char *str, const char *format, va_list arg)
{
	FILE f;

	f.port = 0;
	f.buff = str;
	f.buffsz = 0;
	f.pos = 0;

	vfprintf(&f, format, arg);
	fflush(&f);

	return f.pos;
}


int sprintf(char *s, const char *format, ...)
{
	int err;
	va_list arg;

	va_start(arg, format);
	err = vsprintf(s, format, arg);
	va_end(arg);

	return err;
}


int vprintf(const char *format, va_list arg)
{
	int err;

FILE *stdio;

	err = vfprintf(stdio, format, arg);
	fflush(stdio);

	return err;
}


int printf(const char *format, ...)
{
	int err;
	va_list arg;

	va_start(arg, format);
	err = vprintf(format, arg);
	va_end(arg);

	return err;
}

#endif

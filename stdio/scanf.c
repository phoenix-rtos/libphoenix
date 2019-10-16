/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * scanf.c
 *
 * Copyright 2017 Phoenix Systems
 * Author: Adrian Kepka
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "limits.h"
#include "stdarg.h"


#define	LONG		0x01	/* l: long or double */
#define	SHORT		0x04	/* h: short */
#define	SUPPRESS	0x08	/* *: suppress assignment */
#define	POINTER		0x10	/* p: void * (as hex) */
#define	NOSKIP		0x20	/* [ or c: do not skip blanks */
#define	LONGLONG	0x400	/* ll: long long (+ deprecated q: quad) */
#define	SHORTSHORT	0x4000	/* hh: char */
#define	UNSIGNED	0x8000	/* %[oupxX] conversions */


#define	SIGNOK		0x40	/* +/- is (still) legal */
#define	NDIGITS		0x80	/* no digits detected */
#define	PFXOK		0x100	/* 0x prefix is (still) legal */
#define	NZDIGITS	0x200	/* no zero digits detected */


#define	CT_CHAR		0	/* %c conversion */
#define	CT_CCL		1	/* %[...] conversion */
#define	CT_STRING	2	/* %s conversion */
#define	CT_INT		3	/* %[dioupxX] conversion */


static const unsigned char *__sccl(char *tab, const unsigned char *fmt)
{
	int c, n, v;

	c = *fmt++;
	if (c == '^') {
		v = 1;
		c = *fmt++;
	} else
		v = 0;

	memset(tab, (u8)v, 256);

	if (c == 0)
		return (fmt - 1);

	v = 1 - v;
	tab[c] = v;
	for (;;) {
		n = *fmt++;
		switch (n) {

			case 0:
				return (fmt - 1);

			case '-':
				n = *fmt;
				if (n == ']' || n < c) {
					c = '-';
					tab[c] = v;
					break;
				}
				fmt++;

				do {
					tab[++c] = v;
				} while (c < n);
				c = n;
				break;

			case ']':
				return (fmt);

			default:
				c = n;
				tab[c] = v;
				break;
		}
	}
}


static int scanf_parse(char *ccltab, const char *inp, char const *fmt0, va_list ap)
{
	const unsigned char *fmt = (const unsigned char *)fmt0;
	int c, n, inr, flags, nassigned, nconversions, nread, base;
	size_t width;
	char *p, *p0;
	char buf[32];

	static short basefix[17] = { 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	inr = strlen(inp);

	nassigned = 0;
	nconversions = 0;
	nread = 0;
	base = 0;
	for (;;) {
		c = *fmt++;
		if (c == 0)
			return (nassigned);

		if (isspace(c)) {
			while (inr > 0 && isspace(*inp)) {
				nread++;
				inr--;
				inp++;
			}
			continue;
		}

		if (c != '%') {
			if (inr <= 0)
				return (nconversions != 0 ? nassigned : -1);

			if (*inp != c)
				return nassigned;

			nread++;
			inr--;
			inp++;
			continue;
		}

		width = 0;
		flags = 0;
		for(;;) {
			c = *fmt++;
			if (c == '%') {
				if (inr <= 0)
					return (nconversions != 0 ? nassigned : -1);

				if (*inp != c)
					return nassigned;

				nread++;
				inr--;
				inp++;
				break;
			}

			switch (c) {
				case '*':
					flags |= SUPPRESS;
					continue;

				case 'l':
					if (flags & LONG) {
						flags &= ~LONG;
						flags |= LONGLONG;
					} else
						flags |= LONG;
					continue;

				case 'q':
					flags |= LONGLONG;
					continue;

				case 'h':
					if (flags & SHORT) {
						flags &= ~SHORT;
						flags |= SHORTSHORT;
					} else
						flags |= SHORT;
					continue;

				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					width = width * 10 + c - '0';
					continue;
			}

			/*
			 * Conversions.
			 */
			switch (c) {
				case 'd':
					c = CT_INT;
					base = 10;
					break;

				case 'i':
					c = CT_INT;
					base = 0;
					break;

				case 'o':
					c = CT_INT;
					flags |= UNSIGNED;
					base = 8;
					break;

				case 'u':
					c = CT_INT;
					flags |= UNSIGNED;
					base = 10;
					break;

				case 'X':
				case 'x':
					flags |= PFXOK;    /* enable 0x prefixing */
					c = CT_INT;
					flags |= UNSIGNED;
					base = 16;
					break;

				case 's':
					c = CT_STRING;
					break;

				case '[':
					fmt = __sccl(ccltab, fmt);
					flags |= NOSKIP;
					c = CT_CCL;
					break;

				case 'c':
					flags |= NOSKIP;
					c = CT_CHAR;
					break;

				case 'p':
					flags |= POINTER | PFXOK;
					c = CT_INT;
					flags |= UNSIGNED;
					base = 16;
					break;

				case 'n':
					nconversions++;
					if (flags & SUPPRESS)
						continue;
					if (flags & SHORTSHORT)
						*va_arg(ap, char *) = nread;
					else if (flags & SHORT)
						*va_arg(ap, short *) = nread;
					else if (flags & LONG)
						*va_arg(ap, long *) = nread;
					else if (flags & LONGLONG)
						*va_arg(ap, long long *) = nread;
					else
						*va_arg(ap, int *) = nread;
					continue;
			}

			break;
		}
		if(c == '%')
			continue;

		if (inr <= 0)
			return (nconversions != 0 ? nassigned : -1);

		if ((flags & NOSKIP) == 0) {
			while (isspace(*inp)) {
				nread++;
				if (--inr > 0)
					inp++;
				else
					return (nconversions != 0 ? nassigned : -1);
			}
		}

		/*
		 * Do the conversion.
		 */
		switch (c) {
			case CT_CHAR:
				if (width == 0)
					width = 1;
				if (flags & SUPPRESS) {
					size_t sum = 0;
					for (;;) {
						if ((n = inr) < (int)width) {
							sum += n;
							width -= n;
							inp += n;
							if (sum == 0)
								return (nconversions != 0 ? nassigned : -1);
							break;
						} else {
							sum += width;
							inr -= width;
							inp += width;
							break;
						}
					}
					nread += sum;
				} else {
					memcpy(va_arg(ap, char *), inp, width);
					inr -= width;
					inp += width;
					nread += width;
					nassigned++;
				}
				nconversions++;
				break;

			case CT_CCL:
				if (width == 0)
					width = (size_t)~0;	/* `infinity' */
				if (flags & SUPPRESS) {
					n = 0;
					while (ccltab[(unsigned char)*inp]) {
						n++, inr--, inp++;
						if (--width == 0)
							break;
						if (inr <= 0) {
							if (n == 0)
								return (nconversions != 0 ? nassigned : -1);
							break;
						}
					}
					if (n == 0)
						return nassigned;
				} else {
					p0 = p = va_arg(ap, char *);
					while (ccltab[(unsigned char)*inp]) {
						inr--;
						*p++ = *inp++;
						if (--width == 0)
							break;
						if (inr <= 0) {
							if (p == p0)
								return (nconversions != 0 ? nassigned : -1);
							break;
						}
					}
					n = p - p0;
					if (n == 0)
						return nassigned;
					*p = 0;
					nassigned++;
				}
				nread += n;
				nconversions++;
				break;

			case CT_STRING:
				if (width == 0)
					width = (size_t)~0;
				if (flags & SUPPRESS) {
					n = 0;
					while (!isspace(*inp)) {
						n++, inr--, inp++;
						if (--width == 0)
							break;
						if (inr <= 0)
							break;
					}
					nread += n;
				} else {
					p0 = p = va_arg(ap, char *);
					while (!isspace(*inp)) {
						inr--;
						*p++ = *inp++;
						if (--width == 0)
							break;
						if (inr <= 0)
							break;
					}
					*p = 0;
					nread += p - p0;
					nassigned++;
				}
				nconversions++;
				continue;

			case CT_INT:
				if (--width > sizeof(buf) - 2)
					width = sizeof(buf) - 2;
				width++;

				if (flags & SUPPRESS)
					width = ~0;

				flags |= SIGNOK | NDIGITS | NZDIGITS;
				for (p = buf; width; width--) {
					int ok = 0;
					c = *inp;
					switch (c) {
						case '0':
							if (base == 0) {
								base = 8;
								flags |= PFXOK;
							}
							if (flags & NZDIGITS)
								flags &= ~(SIGNOK|NZDIGITS|NDIGITS);
							else
								flags &= ~(SIGNOK|PFXOK|NDIGITS);
							ok = 1;
							break;

						case '1': case '2': case '3':
						case '4': case '5': case '6': case '7':
							base = basefix[base];
							flags &= ~(SIGNOK | PFXOK | NDIGITS);
							ok = 1;
							break;

						case '8': case '9':
							base = basefix[base];
							if (base <= 8)
								break;	/* not legal here */
							flags &= ~(SIGNOK | PFXOK | NDIGITS);
							ok = 1;
							break;

						case 'A': case 'B': case 'C':
						case 'D': case 'E': case 'F':
						case 'a': case 'b': case 'c':
						case 'd': case 'e': case 'f':
							if (base <= 10)
								break;
							flags &= ~(SIGNOK | PFXOK | NDIGITS);
							ok = 1;
							break;

						case '+': case '-':
							if (flags & SIGNOK) {
								flags &= ~SIGNOK;
								ok = 1;
							}
							break;

						case 'x': case 'X':
							if (flags & PFXOK && p == buf + 1) {
								base = 16;	/* if %i */
								flags &= ~PFXOK;
								ok = 1;
							}
							break;
					}
					if(!ok)
						break;

					if ((flags & SUPPRESS) == 0)
						*p++ = c;
					if (--inr > 0)
						inp++;
					else
						break;
				}
				if (flags & NDIGITS)
					return (nconversions != 0 ? nassigned : -1);

				c = ((unsigned char *)p)[-1];
				if (c == 'x' || c == 'X') {
					--p;
					inp--;
					inr++;
				}

				if ((flags & SUPPRESS) == 0) {
					uint64_t res;

					*p = 0;
					if ((flags & UNSIGNED) == 0)
						res = strtoll(buf, (char **)NULL, base);
					else
						res = strtoull(buf, (char **)NULL, base);
					if (flags & POINTER)
						*va_arg(ap, void **) =
							(void *)(unsigned long)res;
					else if (flags & SHORTSHORT)
						*va_arg(ap, char *) = res;
					else if (flags & SHORT)
						*va_arg(ap, short *) = res;
					else if (flags & LONG)
						*va_arg(ap, long *) = res;
					else if (flags & LONGLONG)
						*va_arg(ap, long long *) = res;
					else {
						*va_arg(ap, int *) = res;
					}
					nassigned++;
				}

				nread += p - buf;
				nconversions++;
				break;
			}
	}
}


int fscanf(FILE *stream, const char *format, ...)
{
	char *lineptr = NULL;
	size_t n = 0;
	va_list arg;
	int ret;
	char *ccltab = malloc(256);

	if ((ret = getline(&lineptr, &n, stream)) > 0) {

		va_start(arg, format);
		ret = scanf_parse(ccltab, lineptr, format, arg);
		va_end(arg);
		free(lineptr);

		if (ret < 0)
			ret = 0;
	}

	free(ccltab);
	return ret;
}

int sscanf(const char *str, const char *format, ...)
{
	int retVal;
	va_list arg;
	char *ccltab = malloc(256);

	va_start(arg, format);
	retVal = scanf_parse(ccltab, str, format, arg);
	va_end(arg);

	free(ccltab);

	return retVal;
}


int scanf(const char *format, ...)
{
	int retVal = 0;
	va_list arg;

	// TODO: Read from stdin
	va_start(arg, format);
//	retVal = scanf_parse(stdin, format, arg);
	va_end(arg);

	return retVal;
}

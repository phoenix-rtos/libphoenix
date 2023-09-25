/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * scanf.c
 *
 * Copyright 2017, 2022-2023 Phoenix Systems
 * Author: Adrian Kepka, Gerard Swiderski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "format.h"

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>


#define LONG       0x01   /* l: long or double */
#define LONGDOUBLE 0x02   /* L: long double */
#define SHORT      0x04   /* h: short */
#define SUPPRESS   0x08   /* *: suppress assignment */
#define POINTER    0x10   /* p: void * (as hex) */
#define NOSKIP     0x20   /* [ or c: do not skip blanks */
#define LONGLONG   0x400  /* ll: long long (+ deprecated q: quad) */
#define PTRDIFF    0x800  /* t: ptrdiff_t */
#define SHORTSHORT 0x4000 /* hh: char */
#define UNSIGNED   0x8000 /* %[oupxX] conversions */


#define SIGNOK   0x40  /* +/- is (still) legal */
#define NDIGITS  0x80  /* no digits detected */
#define PFXOK    0x100 /* 0x prefix is (still) legal */
#define NZDIGITS 0x200 /* no zero digits detected */


#define CT_CHAR   0 /* %c conversion */
#define CT_CCL    1 /* %[...] conversion */
#define CT_STRING 2 /* %s conversion */
#define CT_INT    3 /* %[dioupxX] conversion */
#define CT_FLOAT  4 /* %[aefgAEFG] conversion */


static const unsigned char *__sccl(char *tab, const unsigned char *fmt)
{
	int c, n, v;

	c = *fmt++;
	if (c == '^') {
		v = 1;
		c = *fmt++;
	}
	else
		v = 0;

	memset(tab, (uint8_t)v, 256);

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


static int scanf_parse(char *ccltab, const char *inp, int *inr, char const *fmt0, va_list ap)
{
	const unsigned char *fmt = (const unsigned char *)fmt0;
	int c, n, flags, nassigned, nconversions, nread, base;
	size_t width;
	char *p, *p0;
	char buf[32];

	static short basefix[17] = { 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	*inr = strlen(inp);

	nassigned = 0;
	nconversions = 0;
	nread = 0;
	base = 0;
	for (;;) {
		c = *fmt++;
		if (c == 0)
			return (nassigned);

		if (isspace(c)) {
			while (*inr > 0 && isspace(*inp)) {
				nread++;
				(*inr)--;
				inp++;
			}
			continue;
		}

		if (c != '%') {
			if (*inr <= 0)
				return (nconversions != 0 ? nassigned : -1);

			if (*inp != c)
				return nassigned;

			nread++;
			(*inr)--;
			inp++;
			continue;
		}

		width = 0;
		flags = 0;
		for (;;) {
			c = *fmt++;
			if (c == '%') {
				if (*inr <= 0)
					return (nconversions != 0 ? nassigned : -1);

				if (*inp != c)
					return nassigned;

				nread++;
				(*inr)--;
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
					}
					else
						flags |= LONG;
					continue;

				case 'L':
					flags |= LONGDOUBLE;
					continue;

				case 'q':
				case 'j':
					flags |= LONGLONG;
					continue;

				case 't':
					flags |= PTRDIFF;
					continue;

				case 'z':
					if (sizeof(size_t) == sizeof(uint64_t)) {
						flags |= LONGLONG;
					}
					continue;

				case 'h':
					if (flags & SHORT) {
						flags &= ~SHORT;
						flags |= SHORTSHORT;
					}
					else
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
					flags |= PFXOK; /* enable 0x prefixing */
					c = CT_INT;
					flags |= UNSIGNED;
					base = 16;
					break;

				case 'A':
				case 'E':
				case 'F':
				case 'G':
				case 'a':
				case 'e':
				case 'f':
				case 'g':
					c = CT_FLOAT;
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
					flags |= POINTER | PFXOK | UNSIGNED;
					c = CT_INT;
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
					else if (flags & PTRDIFF)
						*va_arg(ap, ptrdiff_t *) = nread;
					else
						*va_arg(ap, int *) = nread;
					continue;
			}

			break;
		}
		if (c == '%')
			continue;

		if (*inr <= 0)
			return (nconversions != 0 ? nassigned : -1);

		if ((flags & NOSKIP) == 0) {
			while (isspace(*inp)) {
				nread++;
				if (--(*inr) > 0)
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
						n = *inr;
						if (n < (int)width) {
							sum += n;
							width -= n;
							inp += n;
							if (sum == 0)
								return (nconversions != 0 ? nassigned : -1);
							break;
						}
						else {
							sum += width;
							*inr -= width;
							inp += width;
							break;
						}
					}
					nread += sum;
				}
				else {
					memcpy(va_arg(ap, char *), inp, width);
					*inr -= width;
					inp += width;
					nread += width;
					nassigned++;
				}
				nconversions++;
				break;

			case CT_CCL:
				if (width == 0)
					width = (size_t)~0; /* `infinity' */
				if (flags & SUPPRESS) {
					n = 0;
					while (ccltab[(unsigned char)*inp]) {
						n++;
						(*inr)--;
						inp++;
						if (--width == 0)
							break;
						if (*inr <= 0) {
							if (n == 0)
								return (nconversions != 0 ? nassigned : -1);
							break;
						}
					}
					if (n == 0)
						return nassigned;
				}
				else {
					p0 = p = va_arg(ap, char *);
					while (ccltab[(unsigned char)*inp]) {
						(*inr)--;
						*p++ = *inp++;
						if (--width == 0)
							break;
						if (*inr <= 0) {
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
					while (!isspace(*inp)) {
						nread++;
						(*inr)--;
						inp++;
						if (--width == 0)
							break;
						if (*inr <= 0)
							break;
					}
				}
				else {
					p0 = p = va_arg(ap, char *);
					while (!isspace(*inp)) {
						(*inr)--;
						*p++ = *inp++;
						if (--width == 0)
							break;
						if (*inr <= 0)
							break;
					}
					*p = 0;
					nread += p - p0;
					nassigned++;
				}
				nconversions++;
				continue;

			case CT_INT:
				if (((flags & POINTER) != 0) && ((*inr) >= FORMAT_NIL_STR_LEN) && (strncmp(FORMAT_NIL_STR, inp, FORMAT_NIL_STR_LEN) == 0)) {
					*va_arg(ap, void **) = NULL;
					nassigned++;
					nconversions++;
					nread += FORMAT_NIL_STR_LEN;
					inp += FORMAT_NIL_STR_LEN;
					(*inr) -= FORMAT_NIL_STR_LEN;
					break;
				}

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
								flags &= ~(SIGNOK | NZDIGITS | NDIGITS);
							else
								flags &= ~(SIGNOK | PFXOK | NDIGITS);
							ok = 1;
							break;

						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
							base = basefix[base];
							flags &= ~(SIGNOK | PFXOK | NDIGITS);
							ok = 1;
							break;

						case '8':
						case '9':
							base = basefix[base];
							if (base <= 8)
								break; /* not legal here */
							flags &= ~(SIGNOK | PFXOK | NDIGITS);
							ok = 1;
							break;

						case 'A':
						case 'B':
						case 'C':
						case 'D':
						case 'E':
						case 'F':
						case 'a':
						case 'b':
						case 'c':
						case 'd':
						case 'e':
						case 'f':
							if (base <= 10)
								break;
							flags &= ~(SIGNOK | PFXOK | NDIGITS);
							ok = 1;
							break;

						case '+':
						case '-':
							if (flags & SIGNOK) {
								flags &= ~SIGNOK;
								ok = 1;
							}
							break;

						case 'x':
						case 'X':
							if (flags & PFXOK && p == buf + 1) {
								base = 16; /* if %i */
								flags &= ~PFXOK;
								ok = 1;
							}
							break;
					}
					if (!ok)
						break;

					if ((flags & SUPPRESS) == 0)
						*p++ = c;
					if (--(*inr) > 0)
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
					(*inr)++;
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
					else if (flags & PTRDIFF)
						*va_arg(ap, ptrdiff_t *) = res;
					else {
						*va_arg(ap, int *) = res;
					}
					nassigned++;
				}

				nread += p - buf;
				nconversions++;
				break;

			case CT_FLOAT:
				if (width == 0 || width > sizeof(buf) - 1) {
					width = sizeof(buf) - 1;
				}

				if (strtold(inp, &p) == 0 && (inp == p || errno == ERANGE)) {
					return (nconversions != 0 ? nassigned : -1);
				}

				if ((size_t)(p - inp) >= width) {
					return (nconversions != 0 ? nassigned : -1);
				}

				width = p - inp;

				p = buf;
				while (width > 0) {
					if (*inr <= 0) {
						return (nconversions != 0 ? nassigned : -1);
					}
					*p++ = *inp++;
					width--;
					(*inr)--;
				}

				if ((flags & SUPPRESS) == 0) {
					*p = '\0';
					if ((flags & LONGDOUBLE) != 0) {
						long double res = strtold(buf, NULL);
						*va_arg(ap, long double *) = res;
					}
					else if ((flags & LONG) != 0) {
						double res = strtod(buf, NULL);
						*va_arg(ap, double *) = res;
					}
					else {
						float res = strtof(buf, NULL);
						*va_arg(ap, float *) = res;
					}
					nassigned++;
				}

				nread += p - buf;
				nconversions++;
				break;

			default:
				break;
		}
	}
	/* never reached */
}


/* Unget string from the end */
static inline void ungetstr(FILE *stream, const char *str, int nr)
{
	const char *end = strchrnul(str, '\0');

	for (; nr > 0; --nr) {
		if (ungetc((int)(unsigned char)*(--end), stream) == EOF) {
			fseeko(stream, -nr, SEEK_CUR);
			break;
		}
	}
}


int vfscanf(FILE *stream, const char *format, va_list ap)
{
	int ret, nremain;
	size_t n = 0;
	char *lineptr = NULL;
	char *ccltab;

	if (strlen(format) == 0) {
		return 0;
	}

	ccltab = malloc(256);
	if (ccltab == NULL) {
		/* errno set by malloc */
		return -1;
	}

	ret = getline(&lineptr, &n, stream);
	if (ret > 0) {
		ret = scanf_parse(ccltab, lineptr, &nremain, format, ap);
		ungetstr(stream, lineptr, nremain);
		free(lineptr);
	}

	free(ccltab);

	return ret;
}


int vsscanf(const char *str, const char *format, va_list ap)
{
	int ret, nremain;
	char *ccltab = malloc(256);

	if (ccltab == NULL) {
		/* errno set by malloc */
		return -1;
	}

	ret = scanf_parse(ccltab, str, &nremain, format, ap);
	free(ccltab);

	return ret;
}


int vscanf(const char *format, va_list ap)
{
	return vfscanf(stdin, format, ap);
}


int fscanf(FILE *stream, const char *format, ...)
{
	int ret;
	va_list ap;

	va_start(ap, format);
	ret = vfscanf(stream, format, ap);
	va_end(ap);

	return ret;
}


int sscanf(const char *str, const char *format, ...)
{
	int ret;
	va_list ap;

	va_start(ap, format);
	ret = vsscanf(str, format, ap);
	va_end(ap);

	return ret;
}


int scanf(const char *format, ...)
{
	int ret;
	va_list ap;

	va_start(ap, format);
	ret = vscanf(format, ap);
	va_end(ap);

	return ret;
}

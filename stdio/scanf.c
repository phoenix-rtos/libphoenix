/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * scanf.c
 *
 * Copyright 2017, 2022-2023, 2026 Phoenix Systems
 * Author: Adrian Kepka, Gerard Swiderski, Ziemowit Leszczynski
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
#define ALLOCATE   0x1000 /* m: allocate the converted string */
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
#define CT_NONE   5 /* No conversion (ex. %n) */


/* Size of the initial on stack stream buffer */
#define SCANF_BUFSZ 32


#define SCANF_ONSTACK 0x01 /* `buf` is on the stack, not to be grown in place or freed */
#define SCANF_EOF     0x02 /* no more data can be read from the stream */
#define SCANF_ENOMEM  0x04 /* the buffer could not be grown, the input is truncated */


/* number of `%m` arguments that fit in the initial scanf_alloc_t table */
#define SCANF_ALLOCN 16


/* states of the floating point literal scanner */
#define SCANF_FLOAT_LEAD 0  /* at the start, an optional sign may still come */
#define SCANF_FLOAT_BODY 1  /* the sign is past, the literal itself must start */
#define SCANF_FLOAT_ZERO 2  /* a lone `0`, so a `0x` prefix may still follow */
#define SCANF_FLOAT_INT  3  /* digits before the radix point */
#define SCANF_FLOAT_DOT  4  /* a radix point with no digit after it yet */
#define SCANF_FLOAT_FRAC 5  /* digits after the radix point */
#define SCANF_FLOAT_EXP  6  /* the exponent marker, a sign or a digit must follow */
#define SCANF_FLOAT_ESGN 7  /* the exponent sign, a digit must follow */
#define SCANF_FLOAT_EDIG 8  /* exponent digits */
#define SCANF_FLOAT_INF  9  /* `inf` or `infinity` */
#define SCANF_FLOAT_NAN  10 /* `nan`, which a character sequence may follow */
#define SCANF_FLOAT_NANS 11 /* the character sequence of `nan(...)` */
#define SCANF_FLOAT_STOP 12 /* nothing more can be part of the literal */


/*
 * Input source for the scanf parser. A string is scanned straight from the
 * caller's buffer, a stream is read into `buf` on demand - only as far as
 * the format requires. Whatever is buffered but not consumed is pushed back
 * to the stream once the parsing is done.
 */
typedef struct {
	FILE *stream;        /* NULL when a string is being scanned */
	const char *input;   /* input data, always terminated by `\0` at `len` */
	char *buf;           /* input data storage - used for a stream only */
	size_t bufsz;        /* size of `buf` */
	size_t len;          /* number of valid input bytes */
	size_t pos;          /* parsing position within input */
	unsigned char flags; /* state of the source */
} scanf_src_t;


/*
 * Tracks allocated `%m` arguments. If scanf() returns EOF, it must free them,
 * so they are recorded as they are assigned. Grow the table before allocating
 * the buffer to keep the failure path itself free of allocations.
 */
typedef struct {
	char ***arg;  /* each entry holds allocated buffer for one argument */
	size_t argsz; /* number of `arg` entries */
	size_t cnt;   /* number of arguments tracked */
} scanf_alloc_t;


/*
 * Scans a floating-point literal one character at a time, matching prefixes
 * accepted by strtod() to determine how much input a conversion needs to
 * buffer. Accepting more characters than strtod() does is harmless, as
 * strtod() stops at the first invalid character. However, the scan must be
 * bounded. Otherwise, input that cannot form a literal could be buffered
 * indefinitely until memory runs out.
 */
typedef struct {
	unsigned char state; /* one of SCANF_FLOAT_* */
	unsigned char hex;   /* a `0x` literal, so the digits and the marker differ */
	unsigned char len;   /* characters matched by SCANF_FLOAT_INF or SCANF_FLOAT_NAN */
} scanf_float_t;


/*
 * Reads from the stream until at least `need` bytes past the current position
 * are buffered. Returns the number of bytes actually available, which is less
 * than `need` only on end of input or when the buffer could not be grown.
 */
static size_t scanf_refill(scanf_src_t *src, size_t need)
{
	size_t avail = src->len - src->pos;

	while ((avail < need) && ((src->flags & SCANF_EOF) == 0)) {
		if ((src->len + 2) > src->bufsz) {
			/*
			 * Reclaim the consumed part of the buffer before growing it, so
			 * that a conversion which only passes over its input does not make
			 * the buffer grow at all.
			 *
			 * One consumed byte is kept, as CT_INT may push a single character
			 * back.
			 *
			 * Done once at least half of the buffer can be reclaimed, to keep
			 * the cost of the move amortized over the bytes it frees.
			 */
			if (src->pos > (src->bufsz / 2)) {
				size_t drop = src->pos - 1;

				memmove(src->buf, src->buf + drop, (src->len - drop) + 1);
				src->len -= drop;
				src->pos -= drop;
			}
		}

		if ((src->len + 2) > src->bufsz) {
			size_t bufsz = src->bufsz * 2;
			char *buf;

			if ((src->flags & SCANF_ONSTACK) != 0) {
				/* the on stack buffer cannot grow */
				buf = malloc(bufsz);
				if (buf != NULL) {
					memcpy(buf, src->buf, src->len + 1);
				}
			}
			else {
				buf = realloc(src->buf, bufsz);
			}

			if (buf == NULL) {
				/*
				 * Carry on with whatever has been buffered so far. The flag makes
				 * vfscanf() fail with ENOMEM, so that a conversion truncated here
				 * is never reported as a successful one.
				 */
				src->flags |= SCANF_ENOMEM;
				break;
			}

			src->buf = buf;
			src->input = buf;
			src->bufsz = bufsz;
			src->flags &= ~SCANF_ONSTACK;
		}

		int c = fgetc(src->stream);
		if (c == EOF) {
			src->flags |= SCANF_EOF;
			break;
		}

		/*
		 * CT_FLOAT hands the buffer straight to strtod()/strtof(), which
		 * requires the sentinel.
		 */
		src->buf[src->len++] = (char)c;
		src->buf[src->len] = '\0';
		avail++;
	}

	return (avail < need) ? avail : need;
}


/*
 * Makes sure that at least `need` bytes past the current position are buffered.
 * Returns the smaller of `need` and the number of bytes available, so a result
 * below `need` means end of input or that the buffer could not be grown.
 */
static inline size_t scanf_fill(scanf_src_t *src, size_t need)
{
	size_t avail = src->len - src->pos;

	if (avail >= need) {
		return need;
	}

	if (src->stream == NULL) {
		return avail;
	}

	return scanf_refill(src, need);
}


static size_t scanf_charsetString(const scanf_src_t *src, const char *tab, size_t width)
{
	const char *input = src->input + src->pos;
	size_t avail = src->len - src->pos;
	size_t n;

	if (width > avail) {
		width = avail;
	}

	for (n = 0; (n < width) && (tab[(unsigned char)input[n]] != 0); n++) {
	}

	return n;
}


static size_t scanf_charsetStream(scanf_src_t *src, const char *tab, size_t width)
{
	size_t n;

	for (n = 0; n < width; n++) {
		if (scanf_fill(src, n + 1) <= n) {
			break;
		}

		if (tab[(unsigned char)src->input[src->pos + n]] == 0) {
			break;
		}
	}

	return n;
}


/* number of the leading characters present in `tab`, at most `width` */
static inline size_t scanf_charset(scanf_src_t *src, const char *tab, size_t width)
{
	if (src->stream == NULL) {
		return scanf_charsetString(src, tab, width);
	}
	else {
		return scanf_charsetStream(src, tab, width);
	}
}


static size_t scanf_charsetSkipStream(scanf_src_t *src, const char *tab, size_t width)
{
	size_t n;

	for (n = 0; n < width; n++) {
		if (scanf_fill(src, 1) == 0) {
			break;
		}

		if (tab[(unsigned char)src->input[src->pos]] == 0) {
			break;
		}

		src->pos++;
	}

	return n;
}


/* As scanf_charset(), but the characters are consumed. */
static inline size_t scanf_charsetSkip(scanf_src_t *src, const char *tab, size_t width)
{
	if (src->stream == NULL) {
		size_t n = scanf_charsetString(src, tab, width);
		src->pos += n;
		return n;
	}
	else {
		return scanf_charsetSkipStream(src, tab, width);
	}
}


static size_t scanf_wordString(const scanf_src_t *src, size_t width)
{
	const char *input = src->input + src->pos;
	size_t avail = src->len - src->pos;
	size_t n;

	if (width > avail) {
		width = avail;
	}

	for (n = 0; (n < width) && (isspace((unsigned char)input[n]) == 0); n++) {
	}

	return n;
}


static size_t scanf_wordStream(scanf_src_t *src, size_t width)
{
	size_t n;

	for (n = 0; n < width; n++) {
		if (scanf_fill(src, n + 1) <= n) {
			break;
		}

		if (isspace((unsigned char)src->input[src->pos + n]) != 0) {
			break;
		}
	}

	return n;
}


/* number of the leading characters that are not separators, at most `width` */
static inline size_t scanf_word(scanf_src_t *src, size_t width)
{
	if (src->stream == NULL) {
		return scanf_wordString(src, width);
	}
	else {
		return scanf_wordStream(src, width);
	}
}


static size_t scanf_wordSkipStream(scanf_src_t *src, size_t width)
{
	size_t n;

	for (n = 0; n < width; n++) {
		if (scanf_fill(src, 1) == 0) {
			break;
		}

		if (isspace((unsigned char)src->input[src->pos]) != 0) {
			break;
		}

		src->pos++;
	}

	return n;
}


/* As scanf_word(), but the characters are consumed. */
static inline size_t scanf_wordSkip(scanf_src_t *src, size_t width)
{
	if (src->stream == NULL) {
		size_t n = scanf_wordString(src, width);
		src->pos += n;
		return n;
	}
	else {
		return scanf_wordSkipStream(src, width);
	}
}


static size_t scanf_spaceSkipString(scanf_src_t *src)
{
	const char *input = src->input + src->pos;
	size_t avail = src->len - src->pos;
	size_t n;

	for (n = 0; (n < avail) && (isspace((unsigned char)input[n]) != 0); n++) {
	}

	src->pos += n;

	return n;
}


static size_t scanf_spaceSkipStream(scanf_src_t *src)
{
	size_t n;

	for (n = 0; (scanf_fill(src, 1) != 0) && (isspace((unsigned char)src->input[src->pos]) != 0); n++) {
		src->pos++;
	}

	return n;
}


/* Number of leading white-space characters, consumed as they are matched. */
static inline size_t scanf_spaceSkip(scanf_src_t *src)
{
	if (src->stream == NULL) {
		return scanf_spaceSkipString(src);
	}
	else {
		return scanf_spaceSkipStream(src);
	}
}


/*
 * Checks whether `c` could extend the floating-point literal matched so far,
 * which is initially empty. Returns non-zero if `c` can be part of the literal.
 * Once no further characters can extend it, the state is set to
 * SCANF_FLOAT_STOP so the caller never reads a character it cannot use.
 */
static int scanf_isFloat(scanf_float_t *fl, int c)
{
	/* the only words strtod() accepts, matched one character at a time */
	static const char infWord[] = "infinity";
	static const char nanWord[] = "nan";

	int digit = (fl->hex != 0) ? isxdigit(c) : isdigit(c);
	int marker = (fl->hex != 0) ? ((c == 'p') || (c == 'P')) : ((c == 'e') || (c == 'E'));

	switch (fl->state) {
		case SCANF_FLOAT_LEAD:
			if ((c == '+') || (c == '-')) {
				fl->state = SCANF_FLOAT_BODY;
				break;
			}
			/* fall through */

		case SCANF_FLOAT_BODY:
			if (c == '0') {
				fl->state = SCANF_FLOAT_ZERO;
			}
			else if (digit != 0) {
				fl->state = SCANF_FLOAT_INT;
			}
			else if (c == '.') {
				fl->state = SCANF_FLOAT_DOT;
			}
			else if ((fl->hex == 0) && (tolower(c) == 'i')) {
				fl->state = SCANF_FLOAT_INF;
				fl->len = 1;
			}
			else if ((fl->hex == 0) && (tolower(c) == 'n')) {
				fl->state = SCANF_FLOAT_NAN;
				fl->len = 1;
			}
			else {
				fl->state = SCANF_FLOAT_STOP;
			}
			break;

		case SCANF_FLOAT_ZERO:
			if ((fl->hex == 0) && ((c == 'x') || (c == 'X'))) {
				fl->hex = 1;
				fl->state = SCANF_FLOAT_BODY;
				break;
			}
			/* fall through */

		case SCANF_FLOAT_INT:
			if (digit != 0) {
				fl->state = SCANF_FLOAT_INT;
			}
			else if (c == '.') {
				fl->state = SCANF_FLOAT_FRAC;
			}
			else if (marker != 0) {
				fl->state = SCANF_FLOAT_EXP;
			}
			else {
				fl->state = SCANF_FLOAT_STOP;
			}
			break;

		case SCANF_FLOAT_DOT:
			fl->state = (digit != 0) ? SCANF_FLOAT_FRAC : SCANF_FLOAT_STOP;
			break;

		case SCANF_FLOAT_FRAC:
			if (digit != 0) {
				fl->state = SCANF_FLOAT_FRAC;
			}
			else if (marker != 0) {
				fl->state = SCANF_FLOAT_EXP;
			}
			else {
				fl->state = SCANF_FLOAT_STOP;
			}
			break;

		case SCANF_FLOAT_EXP:
			if ((c == '+') || (c == '-')) {
				fl->state = SCANF_FLOAT_ESGN;
				break;
			}
			/* fall through */

		case SCANF_FLOAT_ESGN:
		case SCANF_FLOAT_EDIG:
			fl->state = (isdigit(c) != 0) ? SCANF_FLOAT_EDIG : SCANF_FLOAT_STOP;
			break;

		case SCANF_FLOAT_INF:
			if (tolower(c) != infWord[fl->len]) {
				fl->state = SCANF_FLOAT_STOP;
				break;
			}

			fl->len++;
			if (infWord[fl->len] == '\0') {
				/* `infinity` is complete and nothing can extend it */
				fl->state = SCANF_FLOAT_STOP;
				return 1;
			}
			break;

		case SCANF_FLOAT_NAN:
			if (nanWord[fl->len] == '\0') {
				/* `nan` is complete, only its character sequence may follow */
				fl->state = (c == '(') ? SCANF_FLOAT_NANS : SCANF_FLOAT_STOP;
				break;
			}

			if (tolower(c) != nanWord[fl->len]) {
				fl->state = SCANF_FLOAT_STOP;
				break;
			}

			fl->len++;
			break;

		case SCANF_FLOAT_NANS:
			if (c == ')') {
				/* the closing bracket is the last character of the literal */
				fl->state = SCANF_FLOAT_STOP;
				return 1;
			}

			if ((isalnum(c) == 0) && (c != '_')) {
				fl->state = SCANF_FLOAT_STOP;
			}
			break;

		default:
			fl->state = SCANF_FLOAT_STOP;
			break;
	}

	return (fl->state != SCANF_FLOAT_STOP) ? 1 : 0;
}


/*
 * Checks whether the input starts with FORMAT_NIL_STR. Compares one character
 * at a time to avoid blocking in fgetc() when the input cannot match.
 */
static int scanf_isNil(scanf_src_t *src)
{
	size_t n;

	for (n = 0; n < FORMAT_NIL_STR_LEN; n++) {
		if (scanf_fill(src, n + 1) <= n) {
			return 0;
		}

		if (src->input[src->pos + n] != FORMAT_NIL_STR[n]) {
			return 0;
		}
	}

	return 1;
}


static void scanf_unget(scanf_src_t *src)
{
	size_t remain = src->len - src->pos;

	while (remain > 0) {
		if (ungetc((int)(unsigned char)src->input[src->len - 1], src->stream) == EOF) {
			fseeko(src->stream, -(off_t)remain, SEEK_CUR);
			break;
		}

		src->len--;
		remain--;
	}
}


static char *scanf_alloc(scanf_alloc_t *alloc, char **dst, size_t size)
{
	char *p;

	if (alloc->cnt == alloc->argsz) {
		size_t argsz = (alloc->argsz == 0) ? SCANF_ALLOCN : (alloc->argsz * 2);

		char ***arg = realloc(alloc->arg, argsz * sizeof(*arg));
		if (arg == NULL) {
			/* errno set by realloc */
			return NULL;
		}

		alloc->arg = arg;
		alloc->argsz = argsz;
	}

	p = malloc(size);
	if (p == NULL) {
		/* errno set by malloc */
		return NULL;
	}

	*dst = p;
	alloc->arg[alloc->cnt++] = dst;

	return p;
}


static void scanf_allocFree(scanf_alloc_t *alloc)
{
	size_t i;

	for (i = 0; i < alloc->cnt; i++) {
		free(*alloc->arg[i]);
		*alloc->arg[i] = NULL;
	}

	alloc->cnt = 0;
}


static const unsigned char *__sccl(char *tab, const unsigned char *fmt)
{
	int c, n, v;

	c = *fmt++;
	if (c == '^') {
		v = 1;
		c = *fmt++;
	}
	else {
		v = 0;
	}

	memset(tab, (uint8_t)v, 256);

	if (c == 0) {
		return (fmt - 1);
	}

	v = 1 - v;
	tab[c] = v;
	for (;;) {
		n = *fmt++;
		switch (n) {

			case 0:
				return (fmt - 1);

			case '-':
				n = *fmt;
				if ((n == ']') || (n < c)) {
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


static int scanf_parse(scanf_src_t *src, scanf_alloc_t *alloc, char **ccltab, char const *fmt0, va_list ap)
{
	const unsigned char *fmt = (const unsigned char *)fmt0;
	int c, flags, nassigned, nconversions, nread, base;
	size_t width, len;
	char *p;
	char buf[32];

	static const short basefix[17] = { 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	nassigned = 0;
	nconversions = 0;
	nread = 0;
	base = 0;
	for (;;) {
		int convType = CT_NONE;
		c = *fmt++;
		if (c == '\0') {
			return (nassigned);
		}

		if (isspace(c) != 0) {
			nread += scanf_spaceSkip(src);
			continue;
		}

		if (c != '%') {
			if (scanf_fill(src, 1) == 0) {
				return (nconversions != 0 ? nassigned : -1);
			}

			if ((unsigned char)src->input[src->pos] != c) {
				return nassigned;
			}

			nread++;
			src->pos++;
			continue;
		}

		width = 0;
		flags = 0;
		for (;;) {
			c = *fmt++;
			if (c == '\0') {
				return nassigned;
			}

			if (c == '%') {
				if (scanf_fill(src, 1) == 0) {
					return (nconversions != 0 ? nassigned : -1);
				}

				if ((unsigned char)src->input[src->pos] != c) {
					return nassigned;
				}

				nread++;
				src->pos++;
				break;
			}

			switch (c) {
				case '*':
					flags |= SUPPRESS;
					continue;

				case 'l':
					if ((flags & LONG) != 0) {
						flags &= ~LONG;
						flags |= LONGLONG;
					}
					else {
						flags |= LONG;
					}
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

				case 'm':
					flags |= ALLOCATE;
					continue;

				case 'h':
					if ((flags & SHORT) != 0) {
						flags &= ~SHORT;
						flags |= SHORTSHORT;
					}
					else {
						flags |= SHORT;
					}
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
				default:
					break;
			}

			/*
			 * Conversions.
			 */
			switch (c) {
				case 'd':
					convType = CT_INT;
					base = 10;
					break;

				case 'i':
					convType = CT_INT;
					base = 0;
					break;

				case 'o':
					convType = CT_INT;
					flags |= UNSIGNED;
					base = 8;
					break;

				case 'u':
					convType = CT_INT;
					flags |= UNSIGNED;
					base = 10;
					break;

				case 'X':
				case 'x':
					flags |= PFXOK; /* enable 0x prefixing */
					convType = CT_INT;
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
					convType = CT_FLOAT;
					break;


				case 's':
					convType = CT_STRING;
					break;

				case '[':
					if (*ccltab == NULL) {
						*ccltab = malloc(256);
						if (*ccltab == NULL) {
							/* errno set by malloc */
							return -1;
						}
					}

					fmt = __sccl(*ccltab, fmt);
					flags |= NOSKIP;
					convType = CT_CCL;
					break;

				case 'c':
					flags |= NOSKIP;
					convType = CT_CHAR;
					break;

				case 'p':
					flags |= POINTER | PFXOK | UNSIGNED;
					convType = CT_INT;
					base = 16;
					break;

				case 'n':
					nconversions++;
					if ((flags & SUPPRESS) != 0) {
						break;
					}
					if ((flags & SHORTSHORT) != 0) {
						*va_arg(ap, char *) = nread;
					}
					else if ((flags & SHORT) != 0) {
						*va_arg(ap, short *) = nread;
					}
					else if ((flags & LONG) != 0) {
						*va_arg(ap, long *) = nread;
					}
					else if ((flags & LONGLONG) != 0) {
						*va_arg(ap, long long *) = nread;
					}
					else if ((flags & PTRDIFF) != 0) {
						*va_arg(ap, ptrdiff_t *) = nread;
					}
					else {
						*va_arg(ap, int *) = nread;
					}
					break;

				default:
					/* Character not a conversion specifier; end parsing */
					return nassigned;
			}

			break;
		}

		if (((flags & ALLOCATE) != 0) && (convType != CT_STRING) && (convType != CT_CCL) && (convType != CT_CHAR)) {
			/* `m` modifier is only valid with %s, %[ and %c conversions */
			return nassigned;
		}

		if (convType == CT_NONE) {
			continue;
		}

		if (scanf_fill(src, 1) == 0) {
			return (nconversions != 0 ? nassigned : -1);
		}

		if ((flags & NOSKIP) == 0) {
			while (isspace((unsigned char)src->input[src->pos]) != 0) {
				nread++;
				src->pos++;
				if (scanf_fill(src, 1) == 0) {
					return (nconversions != 0 ? nassigned : -1);
				}
			}
		}

		/*
		 * Do the conversion.
		 */
		switch (convType) {
			case CT_CHAR:
				if (width == 0) {
					width = 1;
				}

				width = scanf_fill(src, width);
				if (width == 0) {
					return (nconversions != 0 ? nassigned : -1);
				}

				if ((flags & SUPPRESS) == 0) {
					if ((flags & ALLOCATE) != 0) {
						p = scanf_alloc(alloc, va_arg(ap, char **), width);
						if (p == NULL) {
							/* errno set by scanf_alloc */
							return -1;
						}
					}
					else {
						p = va_arg(ap, char *);
					}

					memcpy(p, src->input + src->pos, width);
					nassigned++;
				}

				src->pos += width;
				nread += width;
				nconversions++;
				break;

			case CT_CCL:
				if (width == 0) {
					width = (size_t)~0; /* `infinity' */
				}

				/* the run of characters present in the scanset */
				if ((flags & SUPPRESS) == 0) {
					len = scanf_charset(src, *ccltab, width);
				}
				else {
					len = scanf_charsetSkip(src, *ccltab, width);
				}

				if (len == 0) {
					return nassigned;
				}

				if ((flags & SUPPRESS) == 0) {
					if ((flags & ALLOCATE) != 0) {
						p = scanf_alloc(alloc, va_arg(ap, char **), len + 1);
						if (p == NULL) {
							/* errno set by scanf_alloc */
							return -1;
						}
					}
					else {
						p = va_arg(ap, char *);
					}

					memcpy(p, src->input + src->pos, len);
					p[len] = '\0';
					nassigned++;
					src->pos += len;
				}

				nread += len;
				nconversions++;
				break;

			case CT_STRING:
				if (width == 0) {
					width = (size_t)~0;
				}

				/* the run of characters up to the next separator */
				if ((flags & SUPPRESS) == 0) {
					len = scanf_word(src, width);
				}
				else {
					len = scanf_wordSkip(src, width);
				}

				if ((flags & SUPPRESS) == 0) {
					if ((flags & ALLOCATE) != 0) {
						p = scanf_alloc(alloc, va_arg(ap, char **), len + 1);
						if (p == NULL) {
							/* errno set by scanf_alloc */
							return -1;
						}
					}
					else {
						p = va_arg(ap, char *);
					}

					memcpy(p, src->input + src->pos, len);
					p[len] = '\0';
					nassigned++;
					src->pos += len;
				}

				nread += len;
				nconversions++;
				break;

			case CT_INT:
				if (((flags & POINTER) != 0) && (scanf_isNil(src) != 0)) {
					*va_arg(ap, void **) = NULL;
					nassigned++;
					nconversions++;
					nread += FORMAT_NIL_STR_LEN;
					src->pos += FORMAT_NIL_STR_LEN;
					break;
				}

				if ((flags & SUPPRESS) == 0) {
					if ((width == 0) || (width > (sizeof(buf) - 1))) {
						width = sizeof(buf) - 1;
					}
				}
				else if (width == 0) {
					width = (size_t)~0; /* infinity */
				}

				flags |= SIGNOK | NDIGITS | NZDIGITS;
				len = 0;
				for (p = buf; width != 0; width--) {
					int ok = 0;

					if (scanf_fill(src, 1) == 0) {
						break;
					}

					c = (unsigned char)src->input[src->pos];
					switch (c) {
						case '0':
							if (base == 0) {
								base = 8;
								flags |= PFXOK;
							}
							if ((flags & NZDIGITS) != 0) {
								flags &= ~(SIGNOK | NZDIGITS | NDIGITS);
							}
							else {
								flags &= ~(SIGNOK | PFXOK | NDIGITS);
							}
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
							if (base <= 8) {
								break; /* not legal here */
							}
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
							if (base <= 10) {
								break;
							}
							flags &= ~(SIGNOK | PFXOK | NDIGITS);
							ok = 1;
							break;

						case '+':
						case '-':
							if ((flags & SIGNOK) != 0) {
								flags &= ~SIGNOK;
								ok = 1;
							}
							break;

						case 'x':
						case 'X':
							if (((flags & PFXOK) != 0) && (len == 1)) {
								base = 16; /* if %i */
								flags &= ~PFXOK;
								ok = 1;
							}
							break;
					}
					if (!ok)
						break;

					if ((flags & SUPPRESS) == 0) {
						*p++ = c;
					}

					len++;
					src->pos++;
				}
				if ((flags & NDIGITS) != 0) {
					return nassigned;
				}

				c = (unsigned char)src->input[src->pos - 1];
				if ((c == 'x') || (c == 'X')) {
					if ((flags & SUPPRESS) == 0) {
						p--;
					}

					len--;
					src->pos--;
				}

				if ((flags & SUPPRESS) == 0) {
					uint64_t res;

					*p = 0;
					if ((flags & UNSIGNED) == 0) {
						res = strtoll(buf, (char **)NULL, base);
					}
					else {
						res = strtoull(buf, (char **)NULL, base);
					}
					if ((flags & POINTER) != 0) {
						*va_arg(ap, void **) = (void *)(unsigned long)res;
					}
					else if ((flags & SHORTSHORT) != 0) {
						*va_arg(ap, char *) = res;
					}
					else if ((flags & SHORT) != 0) {
						*va_arg(ap, short *) = res;
					}
					else if ((flags & LONG) != 0) {
						*va_arg(ap, long *) = res;
					}
					else if ((flags & LONGLONG) != 0) {
						*va_arg(ap, long long *) = res;
					}
					else if ((flags & PTRDIFF) != 0) {
						*va_arg(ap, ptrdiff_t *) = res;
					}
					else {
						*va_arg(ap, int *) = res;
					}
					nassigned++;
				}

				nread += len;
				nconversions++;
				break;

			case CT_FLOAT: {
#ifndef IO_NO_FLOAT
				scanf_float_t fl = { 0 };
				union {
					float f;
					double d;
					long double ld;
				} res;

				/*
				 * Buffers the longest run of characters that may still be part
				 * of a literal, up to `width` characters. Reading beyond this
				 * point could block in fgetc() on a stream.
				 */
				width = (width != 0) ? width : (size_t)~0; /* `infinity' */
				{
					/* As above, the buffered window is kept in locals */
					const char *in = src->input + src->pos;
					size_t avail = src->len - src->pos;

					for (len = 0; len < width; len++) {
						if (len >= avail) {
							if (scanf_fill(src, len + 1) <= len) {
								break;
							}

							/* refilling may have grown or compacted the buffer */
							in = src->input + src->pos;
							avail = src->len - src->pos;
						}

						if (scanf_isFloat(&fl, (unsigned char)in[len]) == 0) {
							break;
						}

						if (fl.state == SCANF_FLOAT_STOP) {
							/* the literal is complete, reading on could block for nothing */
							len++;
							break;
						}
					}
				}

				const char *srcbuf = src->input + src->pos;

				/*
				 * strtod() stops at the first character that cannot be part of a
				 * literal, so it only has to be kept away from the input past the
				 * field width when that input could still be part of a literal.
				 */
				if ((len == width) && (len < (src->len - src->pos)) &&
						(scanf_isFloat(&fl, (unsigned char)src->input[src->pos + len]) != 0)) {
					/* TODO: handle larger widths */
					if (len > (sizeof(buf) - 1)) {
						return (nconversions != 0 ? nassigned : -1);
					}

					memcpy(buf, srcbuf, len);
					buf[len] = '\0';
					srcbuf = buf;
				}

				int is_zero;
				if ((flags & LONGDOUBLE) != 0) {
					res.ld = strtold(srcbuf, &p);
					is_zero = res.ld == 0;
				}
				else if ((flags & LONG) != 0) {
					res.d = strtod(srcbuf, &p);
					is_zero = res.d == 0;
				}
				else {
					res.f = strtof(srcbuf, &p);
					is_zero = res.f == 0;
				}

				if (is_zero && (srcbuf == p)) {
					return nassigned;
				}

				int consumed = p - srcbuf;
				src->pos += consumed;
				nread += consumed;
				nconversions++;
				if ((flags & SUPPRESS) == 0) {
					if ((flags & LONGDOUBLE) != 0) {
						*va_arg(ap, long double *) = res.ld;
					}
					else if ((flags & LONG) != 0) {
						*va_arg(ap, double *) = res.d;
					}
					else {
						*va_arg(ap, float *) = res.f;
					}

					nassigned++;
				}

				break;
#else
				return (nconversions != 0 ? nassigned : -1);
#endif
			}

			default:
				break;
		}
	}
	/* never reached */
}


int vfscanf(FILE *stream, const char *format, va_list ap)
{
	int ret;
	scanf_src_t src = { 0 };
	scanf_alloc_t alloc = { 0 };
	char *ccltab = NULL;
	char buf[SCANF_BUFSZ];

	if (strlen(format) == 0) {
		return 0;
	}

	buf[0] = '\0';

	src.stream = stream;
	src.buf = buf;
	src.bufsz = sizeof(buf);
	src.input = buf;
	src.flags = SCANF_ONSTACK;

	ret = scanf_parse(&src, &alloc, &ccltab, format, ap);

	if ((src.flags & SCANF_ENOMEM) != 0) {
		/* The input could not be buffered, so the result cannot be trusted */
		errno = ENOMEM;
		ret = -1;
	}
	if (ret < 0) {
		scanf_allocFree(&alloc);
	}

	scanf_unget(&src);

	if ((src.flags & SCANF_ONSTACK) == 0) {
		free(src.buf);
	}
	free(alloc.arg);
	free(ccltab);

	return ret;
}


int vsscanf(const char *str, const char *format, va_list ap)
{
	int ret;
	scanf_src_t src = { 0 };
	scanf_alloc_t alloc = { 0 };
	char *ccltab = NULL;

	src.input = str;
	src.len = strlen(str);

	ret = scanf_parse(&src, &alloc, &ccltab, format, ap);
	if (ret < 0) {
		scanf_allocFree(&alloc);
	}

	free(alloc.arg);
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

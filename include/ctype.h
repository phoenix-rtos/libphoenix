/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ctypes.h
 *
 * Copyright 2017, 2023 Phoenix Systems
 * Author: Pawel Pisarczyk, Adrian Kepka, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_CTYPES_H_
#define _LIBPHOENIX_CTYPES_H_


#ifdef __cplusplus
extern "C" {
#endif


/* This function checks whether the passed character is lowercase letter. */
int islower(int c);
#define __islower(c) (((c) >= 'a' && (c) <= 'z') ? 1 : 0)
#define islower(c)   __islower(c)

/* This function checks whether the passed character is an uppercase letter. */
int isupper(int c);
#define __isupper(c) (((c) >= 'A' && (c) <= 'Z') ? 1 : 0)
#define isupper(c)   __isupper(c)


/* This function checks whether the passed character is alphabetic. */
int isalpha(int c);
#define __isalpha(c) (((islower(c) != 0) || (isupper(c) != 0)) ? 1 : 0)
#define isalpha(c)   __isalpha(c)


/* This function checks whether the passed character is control character. */
int iscntrl(int c);
#define __iscntrl(c) (((((c) >= 0x00) && ((c) <= 0x1f)) || (c) == 0x7f) ? 1 : 0)
#define iscntrl(c)   __iscntrl(c)


/* This function checks whether the passed character is decimal digit. */
int isdigit(int c);
#define __isdigit(c) (((c) >= '0' && (c) <= '9') ? 1 : 0)
#define isdigit(c)   __isdigit(c)


/* This function checks whether the passed character is alphanumeric. */
int isalnum(int c);
#define __isalnum(c) (((isalpha(c) != 0) || (isdigit(c) != 0)) ? 1 : 0)
#define isalnum(c)   __isalnum(c)


/* This function checks whether the passed character is printable. */
int isprint(int c);
#define __isprint(c) ((((c) > 0x1f) && ((c) < 0x7f)) ? 1 : 0)
#define isprint(c)   __isprint(c)


/* This function checks whether the passed character has graphical representation using locale. */
int isgraph(int c);
#define __isgraph(c) ((((c) != ' ') && (isprint(c) != 0)) ? 1 : 0)
#define isgraph(c)   __isgraph(c)


/* This function checks whether the passed character is a punctuation character. */
int ispunct(int c);
#define __ispunct(c) (((isgraph(c) != 0) && (isalnum(c) == 0)) ? 1 : 0)
#define ispunct(c)   __ispunct(c)


/* This function checks whether the passed character is white-space. */
int isspace(int c);
#define __isspace(c) (((c) == ' ' || (c) == '\f' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\v') ? 1 : 0)
#define isspace(c)   __isspace(c)


/* This function checks whether the passed character is a hexadecimal digit. */
int isxdigit(int c);
#define __isxdigit(c) (((((c) >= '0') && ((c) <= '9')) || (((c) >= 'a') && ((c) <= 'f')) || (((c) >= 'A') && ((c) <= 'F'))) ? 1 : 0)
#define isxdigit(c)   __isxdigit(c)


/* This function checks whether the passed character is a blank character. */
int isblank(int c);
#define __isblank(c) ((((c) == ' ') || ((c) == '\t')) ? 1 : 0)
#define isblank(c)   __isblank(c)


/* This function converts uppercase letters to lowercase. */
int tolower(int c);
#define __tolower(c) ((isupper(c) != 0) ? ((c) - 'A' + 'a') : c)
#define tolower(c)   __tolower(c)


/* This function converts lowercase letters to uppercase. */
int toupper(int c);
#define __toupper(c) ((islower(c) != 0) ? ((c) - 'a' + 'A') : c)
#define toupper(c)   __toupper(c)


/* This function test if character is representable as ASCII value. */
int isascii(int c);
#define __isascii(c) ((((c) < 0) || ((c) > 0x7f)) ? 0 : 1)
#define isascii(c)   __isascii(c)


#ifdef __cplusplus
}
#endif


#endif

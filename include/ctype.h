/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ctype.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_CTYPE_H_
#define _LIBPHOENIX_CTYPE_H_


/* This function checks whether the passed character is alphanumeric. */
extern int isalnum(int c);


/* This function checks whether the passed character is alphabetic. */
extern int isalpha(int c);


/* This function checks whether the passed character is control character. */
extern int iscntrl(int c);


/* This function checks whether the passed character is decimal digit. */
extern int isdigit(int c);


/* This function checks whether the passed character has graphical representation using locale. */
extern int isgraph(int c);


/* This function checks whether the passed character is lowercase letter. */
extern int islower(int c);


/* This function checks whether the passed character is printable. */
extern int isprint(int c);


/* This function checks whether the passed character is a punctuation character. */
extern int ispunct(int c);


/* This function checks whether the passed character is white-space. */
extern int isspace(int c);


/* This function checks whether the passed character is an uppercase letter. */
extern int isupper(int c);


/* This function checks whether the passed character is a hexadecimal digit. */
extern int isxdigit(int c);


/* This function checks whether the passed character is a blank character. */
extern int isblank(int c);


/* This function converts uppercase letters to lowercase. */
extern int tolower(int c);


/* This function converts lowercase letters to uppercase. */
extern int toupper(int c);


static inline int isascii(int c)
{
	return (c & ~0x7f);
}


#endif

/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ctype.c
 *
 * Copyright 2017 Phoenix Systems
 * Author: Adrian Kepka
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "ctype.h"


#undef isalpha
int isalpha(int c)
{
	return __isalpha(c);
}


#undef isdigit
int isdigit(int c)
{
	return __isdigit(c);
}


#undef islower
int islower(int c)
{
	return __islower(c);
}


#undef isspace
int isspace(int c)
{
	return __isspace(c);
}


#undef isupper
int isupper(int c)
{
	return __isupper(c);
}


#undef tolower
int tolower(int c)
{
	return __tolower(c);
}


#undef toupper
int toupper(int c)
{
	return __toupper(c);
}


#undef isalnum
int isalnum(int c)
{
	return __isalnum(c);
}


#undef iscntrl
int iscntrl(int c)
{
	return __iscntrl(c);
}


#undef isprint
int isprint(int c)
{
	return __isprint(c);
}


#undef isgraph
int isgraph(int c)
{
	return __isgraph(c);
}


#undef ispunct
int ispunct(int c)
{
	return __ispunct(c);
}


#undef isxdigit
int isxdigit(int c)
{
	return __isxdigit(c);
}


#undef isblank
int isblank(int c)
{
	return __isblank(c);
}


#undef isascii
int isascii(int c)
{
	return __isascii(c);
}

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

#include <ctype.h>


int isalpha(int c)
{
	return (islower(c) || isupper(c));
}


int isdigit(int c)
{
	return (c >= '0' && c <= '9');
}


int islower(int c)
{
	return (c >= 'a' && c <= 'z');
}


int isspace(int c)
{
	return (c == ' ' || c == '\f' || c == '\t' || c == '\n' || c == '\r' || c == '\v');
}


int isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}


int tolower(int c)
{
	return isupper(c) ? c - 'A' + 'a' : c;
}


int toupper(int c)
{
	return islower(c) ? c - 'a' + 'A' : c;
}


int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}


int iscntrl(int c)
{
	return (c >= 0x00 && c <= 0x1f) || c == 0x7f;
}


int isprint(int c)
{
	return c > 0x1f && c < 0x7f;
}


int isgraph(int c)
{
	return c != ' ' && isprint(c);
}


int ispunct(int c)
{
	return isgraph(c) && !isalnum(c);
}


int isxdigit(int c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}


int isblank(int c)
{
	return c == ' ' || c == '\t';
}

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



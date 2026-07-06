/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * perror.c
 *
 * Copyright 2022 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

void perror(const char *str)
{
	const char *s = (str != NULL) ? str : "";
	const char *sep = (str != NULL && str[0] != '\0') ? ": " : "";
	fprintf(stderr, "%s%s%s\n", s, sep, strerror(errno));
}

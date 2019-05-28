/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * perror.c
 *
 * Copyright 2019 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>

void perror(const char *str)
{
	if (!str)
		str = "";
	fprintf(stderr, "%s%s%s\n", str, *str ? ": " : "", strerror(errno));
}

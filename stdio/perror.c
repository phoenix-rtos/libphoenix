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
	fprintf(stderr, "%s: %s\n", str, strerror(errno));
}

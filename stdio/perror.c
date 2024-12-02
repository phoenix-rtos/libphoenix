/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * perror.c
 *
 * Copyright 2022, 2023 Phoenix Systems
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
	char errstr[64];

	(void)strerror_r(errno, errstr, sizeof(errstr));

	if (str != NULL) {
		fprintf(stderr, "%s: ", str);
	}

	fprintf(stderr, "%s\n", errstr);
}

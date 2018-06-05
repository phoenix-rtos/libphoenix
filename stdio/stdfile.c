/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdfile.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>


static FILE stdin_file  = {0, NULL, 0};
static FILE stdout_file = {1, NULL, 0};
static FILE stderr_file = {2, NULL, 0};

FILE *stdin = &stdin_file, *stdout = &stdout_file, *stderr = &stderr_file;

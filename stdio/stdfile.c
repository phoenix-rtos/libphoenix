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


static FILE stdin_file, stdout_file;
FILE *stdin = &stdin_file, *stdout = &stdout_file, *stderr = &stdout_file;

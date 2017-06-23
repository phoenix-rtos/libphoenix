/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (armv7)
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH
#include "stdlib.h"


extern int main(int argc, char **argv);


void _start(void)
{
	exit(main(1, 0));
}

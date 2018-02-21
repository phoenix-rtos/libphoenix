/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (arm-imx)
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski
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
	_malloc_init();

	exit(main(1, 0));
}

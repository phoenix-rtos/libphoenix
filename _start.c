/*
 * Phoenix-RTOS
 *
 * Native API
 *
 * Entrypoint
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "libphoenix.h"


extern int main(int argc, char **argv);


void _start(void)
{
	ph_exit(main(1, 0));
}

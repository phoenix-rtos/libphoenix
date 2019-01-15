/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdlib/random
 *
 * Copyright 2018 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>


long random(void)
{
	return rand();
}


void srandom(unsigned int seed)
{
	return srand(seed);
}

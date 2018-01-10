/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * termios.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <termios.h>


int tcgetattr(int fildes, struct termios *termios_p)
{
	return -1;
}


int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p)
{
	return -1;
}

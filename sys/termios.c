/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/termios.c
 *
 * Copyright 2025 Phoenix Systems
 * Author: Grzegorz Mazurczak
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <sys/types.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>


int tcSetSpeed(int fd, int speed)
{
	int ret;

	if (speed <= 0) {
		errno = EINVAL;
		return -1;
	}

	do {
		ret = ioctl(fd, TCSETBR, speed);
	} while (ret < 0 && errno == EINTR);

	return ret;
}

/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * termios.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Marek Białowąs
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>


int tcgetattr(int fildes, struct termios *termios_p)
{
	int ret = ioctl(fildes, TCGETS, termios_p);
	if (ret < 0) {
		errno = -ret;
		return -1;
	}
	return ret;
}


int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p)
{
	int cmd;
	switch (optional_actions) {
		case TCSANOW:
			cmd = TCSETS;
			break;
		case TCSADRAIN:
			cmd = TCSETSW;
			break;
		case TCSAFLUSH:
			cmd = TCSETSF;
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	int ret = ioctl(fildes, cmd, termios_p);
	if (ret < 0) {
		errno = -ret;
		return -1;
	}

	return ret;
}

int tcsendbreak(int fd, int duration)
{
	int ret = ioctl(fd, TCSBRK, duration);
	if (ret < 0) {
		errno = -ret;
		return -1;
	}

	return ret;
}

int tcflush(int fd, int queue_selector)
{
	int ret = ioctl(fd, TCFLSH, queue_selector);
	if (ret < 0) {
		errno = -ret;
		return -1;
	}

	return ret;
}

int tcdrain(int fd)
{
	int ret = ioctl(fd, TCDRAIN, 0);
	if (ret < 0) {
		errno = -ret;
		return -1;
	}

	return ret;
}

int tcsetpgrp(int fd, pid_t pgrp)
{
	int ret = ioctl(fd, TIOCSPGRP, &pgrp);
	if (ret < 0) {
		errno = -ret;
		return -1;
	}

	return ret;
}

pid_t tcgetpgrp(int fd)
{
	pid_t p;
	int ret = ioctl(fd, TIOCGPGRP, &p);
	if (ret < 0) {
		errno = -ret;
		return (pid_t)-1;
	}

	return p;
}

pid_t tcgetsid(int fd)
{
	pid_t p;
	int ret = ioctl(fd, TIOCGSID, &p);
	if (ret < 0) {
		errno = -ret;
		return (pid_t)-1;
	}

	return p;
}

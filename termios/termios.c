/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * termios.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Marek Bialowas
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
	int ret;
	do {
		ret = ioctl(fildes, TCGETS, termios_p);
	} while (ret < 0 && errno == EINTR);

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

	int ret;
	do {
		ret = ioctl(fildes, cmd, termios_p);
	} while (ret < 0 && errno == EINTR);

	return ret;
}

int tcsendbreak(int fd, int duration)
{
	int ret;
	do {
		ret = ioctl(fd, TCSBRK, duration);
	} while (ret < 0 && errno == EINTR);

	return ret;
}

int tcflush(int fd, int queue_selector)
{
	int ret;
	do {
		ret = ioctl(fd, TCFLSH, queue_selector);
	} while (ret < 0 && errno == EINTR);

	return ret;
}

int tcdrain(int fd)
{
	int ret;
	do {
		ret = ioctl(fd, TCDRAIN, 0);
	} while (ret < 0 && errno == EINTR);

	return ret;
}

int tcsetpgrp(int fd, pid_t pgrp)
{
	int ret;
	do {
		ret = ioctl(fd, TIOCSPGRP, &pgrp);
	} while (ret < 0 && errno == EINTR);

	return ret;
}

pid_t tcgetpgrp(int fd)
{
	pid_t p;
	int ret;
	do {
		ret = ioctl(fd, TIOCGPGRP, &p);
	} while (ret < 0 && errno == EINTR);

	return ret < 0 ? (pid_t)-1 : p;
}

pid_t tcgetsid(int fd)
{
	pid_t p;
	int ret;
	do {
		ret = ioctl(fd, TIOCGSID, &p);
	} while (ret < 0 && errno == EINTR);

	return ret < 0 ? (pid_t)-1 : p;
}

void cfmakeraw(struct termios *termios_p)
{
	termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	termios_p->c_oflag &= ~OPOST;
	termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	termios_p->c_cflag &= ~(CSIZE | PARENB);
	termios_p->c_cflag |= CS8;

	termios_p->c_cc[VTIME] = 0;
	termios_p->c_cc[VMIN] = 1;
}

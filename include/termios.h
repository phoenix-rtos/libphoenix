/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * termios.h
 *
 * Copyright 2018, 2021 Phoenix Systems
 * Author: Jan Sikorski, Marek Białowąs, Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_TERMIOS_H_
#define _LIBPHOENIX_TERMIOS_H_

#include <sys/ioctl.h>
#include <sys/types.h>


/* Character that disables terminal special character handling */
#define _POSIX_VDISABLE '\0'


/* c_cc array subscripts */
/* Defined as macros since various programs test them in preprocessor expressions */
#define VINTR    0  /* INTR character */
#define VQUIT    1  /* QUIT character */
#define VERASE   2  /* ERASE character */
#define VKILL    3  /* KILL character */
#define VEOF     4  /* EOF character */
#define VTIME    5  /* TIME value */
#define VMIN     6  /* MIN value */
#define VSTART   7  /* START character */
#define VSTOP    8  /* STOP character */
#define VSUSP    9  /* SUSP character */
#define VEOL     10 /* EOL character */
#define VREPRINT 11 /* REPRINT character, non-POSIX */
#define VDISCARD 12 /* DISCARD character, non-POSIX */
#define VWERASE  13 /* WORD ERASE character, non-POSIX */
#define VLNEXT   14 /* LNEXT character, non-POSIX */
#define VERASE2  15 /* Another ERASE character, non-POSIX */
#define VEOL2    16 /* Another EOL character, non-POSIX */
#define NCCS     17 /* Size of c_cc array for control characters */


/* c_iflag bits */
#define BRKINT  (1 << 0)  /* Signal interrupt on break */
#define ICRNL   (1 << 1)  /* Map CR to NL on input */
#define IGNBRK  (1 << 2)  /* Ignore break condition */
#define IGNCR   (1 << 3)  /* Ignore CR */
#define IGNPAR  (1 << 4)  /* Ignore characters with parity errors */
#define INLCR   (1 << 5)  /* Map NL to CR on input */
#define INPCK   (1 << 6)  /* Enable input parity check */
#define ISTRIP  (1 << 7)  /* Strip character */
#define IUCLC   (1 << 8)  /* Map uppercase characters to lowercase on input, non-POSIX */
#define IXANY   (1 << 9)  /* Enable any character to restart output */
#define IXOFF   (1 << 10) /* Enable start/stop input control */
#define IXON    (1 << 11) /* Enable start/stop output control */
#define IMAXBEL (1 << 12) /* Ring bell on input queue full, non-POSIX */
#define PARMRK  (1 << 13) /* Mark parity errors */


/* c_oflag bits */
#define OPOST  (1 << 0)  /* Post-process output */
#define OLCUC  (1 << 1)  /* Map lowercase characters to uppercase on output, non-POSIX */
#define ONLCR  (1 << 2)  /* Map NL to CR-NL on output */
#define OCRNL  (1 << 3)  /* Map CR to NL on output */
#define ONOCR  (1 << 4)  /* No CR output at column 0 */
#define ONLRET (1 << 5)  /* NL performs CR function */
#define OFILL  (1 << 6)  /* Use fill characters for delay */
#define NLDLY  (1 << 7)  /* Select newline delays: */
#define NL0    (1 << 8)  /* Newline type 0 */
#define NL1    (1 << 9)  /* Newline type 1 */
#define CRDLY  (1 << 10) /* Select carriage return delays: */
#define CR0    (1 << 11) /* Carriage return delay type 0 */
#define CR1    (1 << 12) /* Carriage return delay type 1 */
#define CR2    (1 << 13) /* Carriage return delay type 2 */
#define CR3    (1 << 14) /* Carriage return delay type 3 */
#define TABDLY (1 << 15) /* Select horizontal tab delays: */
#define TAB0   (1 << 16) /* Horizontal tab delay type 0 */
#define TAB1   (1 << 17) /* Horizontal tab delay type 1 */
#define TAB2   (1 << 18) /* Horizontal tab delay type 2 */
#define TAB3   (1 << 19) /* Horizontal tab delay type 3 */
#define XTABS  TAB3      /* Horizonral tab expand to spaces */
#define BSDLY  (1 << 20) /* Select backspace delays: */
#define BS0    (1 << 21) /* Backspace delay type 0 */
#define BS1    (1 << 22) /* Backspace delay type 1 */
#define VTDLY  (1 << 23) /* Select vertical tab delays: */
#define VT0    (1 << 24) /* Vertical tab delay type 0 */
#define VT1    (1 << 25) /* Vertical tab delay type 1 */
#define FFDLY  (1 << 26) /* Select form feed delays: */
#define FF0    (1 << 27) /* Form feed delay type 0 */
#define FF1    (1 << 28) /* Form feed delay type 1 */


/* c_cflag bits */
#define CS5    0        /* 5-bit character */
#define CS6    1        /* 6-bit character */
#define CS7    2        /* 7-bit character */
#define CS8    3        /* 8-bit character */
#define CSIZE  CS8      /* Character size */
#define CSTOPB (1 << 3) /* Send two stop bits, else one */
#define CREAD  (1 << 4) /* Enable receiver */
#define PARENB (1 << 5) /* Parity enable */
#define PARODD (1 << 6) /* Odd parity, else even */
#define HUPCL  (1 << 7) /* Hang up on last close */
#define CLOCAL (1 << 8) /* Ignore modem status lines */


/* c_lflag bits */
#define ECHO    (1 << 0)  /* Enable echo */
#define ECHOE   (1 << 1)  /* Echo erase character as error-correcting backspace */
#define ECHOK   (1 << 2)  /* Echo KILL */
#define ECHONL  (1 << 3)  /* Echo NL */
#define ICANON  (1 << 4)  /* Canonical input (erase and kill processing) */
#define IEXTEN  (1 << 5)  /* Enable extended input character processing */
#define ISIG    (1 << 6)  /* Enable signals */
#define NOFLSH  (1 << 7)  /* Disable flush after interrupt or quit */
#define TOSTOP  (1 << 8)  /* Send SIGTTOU for background output */
#define XCASE   (1 << 9)  /* Use uppercase characters only */
#define ECHOCTL (1 << 10) /* Echo control characters as ^X */
#define ECHOKE  (1 << 11) /* Visual erase for KILL */


/* Baud rates */
/* Defined as macros since various programs test them in preprocessor expressions */
#define B0      0  /* Hang up */
#define B50     1  /* 50 baud */
#define B75     2  /* 75 baud */
#define B110    3  /* 110 baud */
#define B134    4  /* 134 baud */
#define B150    5  /* 150 baud */
#define B200    6  /* 200 baud */
#define B300    7  /* 300 baud */
#define B600    8  /* 600 baud */
#define B1200   9  /* 1200 baud */
#define B1800   10 /* 1800 baud */
#define B2400   11 /* 2400 baud */
#define B4800   12 /* 4800 baud */
#define B9600   13 /* 9600 baud */
#define B19200  14 /* 19200 baud */
#define B38400  15 /* 38400 baud */
#define B57600  16 /* 57600 baud */
#define B115200 17 /* 115200 baud */
#define B230400 18 /* 230400 baud */
#define B460800 19 /* 460800 baud */


typedef unsigned int cc_t; /* Used for terminal special characters */
typedef int speed_t;       /* Used for terminal baud rates */
typedef int tcflag_t;      /* Used for terminal modes */


struct termios {
	tcflag_t c_iflag; /* Input modes */
	tcflag_t c_oflag; /* Output modes */
	tcflag_t c_cflag; /* Control modes */
	tcflag_t c_lflag; /* Local modes */
	cc_t c_cc[NCCS];  /* Control characters */
	speed_t c_ispeed; /* Input baud rate */
	speed_t c_ospeed; /* Output baud rate */
};


struct winsize
{
	unsigned short ws_row;    /* Window rows in characters */
	unsigned short ws_col;    /* Window columns in characters */
	unsigned short ws_xpixel; /* Window horizontal size in pixels */
	unsigned short ws_ypixel; /* Window vertical size in pixels */
};


/* ioctls */
#define TCGETS     _IOR('t', 0x1, struct termios)
#define TCSETS     _IOW('t', 0x2, struct termios)
#define TCSETSW    _IOW('t', 0x3, struct termios)
#define TCSETSF    _IOW('t', 0x4, struct termios)
#define TCFLSH     _IOV('t', 0x5, int)
#define TCDRAIN    _IO('t', 0x6)
#define TCSBRK     _IOV('t', 0x9, int)
#define TIOCSCTTY  _IOV('t', 0xE, int)
#define TIOCGPGRP  _IOR('t', 0xF, pid_t)
#define TIOCSPGRP  _IOW('t', 0x10, pid_t)
#define TIOCGWINSZ _IOR('t', 0x13, struct winsize)
#define TIOCSWINSZ _IOW('t', 0x14, struct winsize)
#define TIOCNOTTY  _IO('t', 0x22)
#define TIOCGSID   _IOR('t', 0x29, pid_t)


/* tcflush */
enum { TCIFLUSH, TCOFLUSH, TCIOFLUSH };


/* tcsetattr */
enum { TCSANOW, TCSADRAIN, TCSAFLUSH };


/* tcflow */
enum { TCIOFF, TCION, TCOOFF, TCOON };


extern int tcgetattr(int fildes, struct termios *termios_p);


extern int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p);


extern int tcsendbreak(int fd, int duration);


extern int tcflush(int fd, int queue_selector);


extern int tcdrain(int fd);


extern int tcsetpgrp(int fd, pid_t pgrp);


extern pid_t tcgetpgrp(int fd);


extern pid_t tcgetsid(int fd);


extern void cfmakeraw(struct termios *termios_p);


static inline speed_t cfgetispeed(const struct termios *termios_p)
{
	return termios_p->c_ispeed;
}


static inline speed_t cfgetospeed(const struct termios *termios_p)
{
	return termios_p->c_ospeed;
}


static inline int cfsetispeed(struct termios *termios_p, speed_t speed)
{
	if (speed == 0) /* needed by POSIX */
		speed = termios_p->c_ospeed;

	termios_p->c_ispeed = speed;
	return 0;
}


static inline int cfsetospeed(struct termios *termios_p, speed_t speed)
{
	termios_p->c_ospeed = speed;
	return 0;
}


static inline int cfsetspeed(struct termios *termios_p, speed_t speed)
{
	termios_p->c_ospeed = speed;
	termios_p->c_ispeed = speed;
	return 0;
}


#endif

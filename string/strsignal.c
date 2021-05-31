/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * string/strsignal
 *
 * Copyright 2021 Phoenix Systems
 * Author: marek Bialowas
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <signal.h>

char *strsignal(int signum)
{
	switch (signum) {
		case SIGHUP:
			return "Hangup";
		case SIGINT:
			return "Interrupt";
		case SIGQUIT:
			return "Quit";
		case SIGILL:
			return "Illegal instruction";
		case SIGTRAP:
			return "Trace/breakpoint trap";
		case SIGABRT:
			return "Aborted";
		case SIGEMT:
			return "EMT trap";
		case SIGFPE:
			return "Floating point exception";
		case SIGKILL:
			return "Killed";
		case SIGBUS:
			return "Bus error";
		case SIGSEGV:
			return "Segmentation fault";
		case SIGSYS:
			return "Bad system call";
		case SIGPIPE:
			return "Broken pipe";
		case SIGALRM:
			return "Alarm clock";
		case SIGTERM:
			return "Terminated";
		case SIGURG:
			return "Urgent I/O condition";
		case SIGSTOP:
			return "Stopped (signal)";
		case SIGTSTP:
			return "Stopped";
		case SIGCONT:
			return "Continued";
		case SIGCHLD:
			return "Child exited";
		case SIGTTIN:
			return "Stopped (tty input)";
		case SIGTTOU:
			return "Stopped (tty output)";
		case SIGIO:
			return "I/O possible";
		case SIGXCPU:
			return "CPU time limit exceeded";
		case SIGXFSZ:
			return "File size limit exceeded";
		case SIGVTALRM:
			return "Virtual timer expired";
		case SIGPROF:
			return "Profiling timer expired";
		case SIGWINCH:
			return "Window changed";
		case SIGINFO:
			return "Information request";
		case SIGUSR1:
			return "User defined signal 1";
		case SIGUSR2:
			return "User defined signal 2";
		default:
			return "Unknown signal";
	}
}

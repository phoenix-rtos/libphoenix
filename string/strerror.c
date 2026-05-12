/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * string/strerror (errno names)
 *
 * Copyright 2018, 2023 Phoenix Systems
 * Author: Michal Miroslaw, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>


/* clang-format off */
static const char *__errmsgtab[] = {
	/* EOK */             "Operation succeeded",
	/* EPERM */           "Operation not permitted",
	/* ENOENT */          "No such file or directory",
	/* ESRCH */           "No such process",
	/* EINTR */           "Interrupted system call",
	/* EIO */             "I/O error",
	/* ENXIO */           "No such device or address",
	/* E2BIG */           "Argument list too long",
	/* ENOEXEC */         "Executable format error",
	/* EBADF */           "Bad file number",
	/* ECHILD */          "No child processes",
	/* EAGAIN */          "Try again",
	/* ENOMEM */          "Out of memory",
	/* EACCES */          "Permission denied",
	/* EFAULT */          "Bad address",
	/* ENOTBLK */         "Block device required",
	/* EBUSY */           "Device or resource busy",
	/* EEXIST */          "File exists",
	/* EXDEV */           "Cross-device link",
	/* ENODEV */          "No such device",
	/* ENOTDIR */         "Not a directory",
	/* EISDIR */          "Is a directory",
	/* EINVAL */          "Invalid argument",
	/* ENFILE */          "File table overflow",
	/* EMFILE */          "Too many open files",
	/* ENOTTY */          "Not a typewriter",
	/* ETXTBSY */         "Text file busy",
	/* EFBIG */           "File too large",
	/* ENOSPC */          "No space left on device",
	/* ESPIPE */          "Illegal seek",
	/* EROFS */           "Read-only file system",
	/* EMLINK */          "Too many links",
	/* EPIPE */           "Broken pipe",
	/* EDOM */            "Argument out of domain",
	/* ERANGE */          "Result not representable",
	/* ENOSYS */          "Function not implemented",
	/* ENAMETOOLONG */    "Name too long",
	/* ETIME */           "Timer expired",
	/* EDEADLK */         "Resource deadlock avoided",
	/* ENOTEMPTY */       "Directory not empty",
	/* ELOOP */           "Too many levels of symbolic links",
	NULL,
	/* ENOMSG */          "No message of the desired type",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	/* EUNATCH */         "Protocol driver not attached",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	/* ENODATA */         "No message is available on the STREAM head read queue",
	NULL,
	NULL,
	/* ENONET */          "Machine is not on the network.",
	NULL,
	/* EBADFD */          "File descriptor in bad state",
	NULL,
	NULL,
	NULL,
	NULL,
	/* EPROTO */          "Protocol error",
	NULL,
	NULL,
	/* EBADMSG */         "Bad message",
	/* EOVERFLOW */       "Value too large to be stored in data type",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	/* EILSEQ */          "Invalid or incomplete multibyte or wide character",
	NULL,
	NULL,
	NULL,
	/* ENOTSOCK */        "Not a socket",
	/* EDESTADDRREQ */    "Destination address required",
	/* EMSGSIZE */        "Message too long",
	/* EPROTOTYPE */      "Protocol wrong type for socket",
	/* ENOPROTOOPT */     "Protocol not available",
	/* EPROTONOSUPPORT */ "Protocol not supported",
	NULL,
	/* EOPNOTSUPP */      "Operation not supported on socket",
	/* EPFNOSUPPORT */    "Protocol family not supported",
	/* EAFNOSUPPORT */    "Address family not supported",
	/* EADDRINUSE */      "Address already in use",
	/* EADDRNOTAVAIL */   "Address not available",
	/* ENETDOWN */        "Network is down",
	/* ENETUNREACH */     "Network unreachable",
	/* ENETRESET */       "Connection aborted by network",
	/* ECONNABORTED */    "Connection aborted",
	/* ECONNRESET */      "Connection reset",
	/* ENOBUFS */         "No buffer space available",
	/* EISCONN */         "Socket is connected",
	/* ENOTCONN */        "The socket is not connected",
	NULL,
	NULL,
	/* ETIMEDOUT */       "Connection timed out",
	/* ECONNREFUSED */    "Connection refused",
	/* EHOSTDOWN */       "Host is down",
	/* EHOSTUNREACH */    "Host is unreachable",
	/* EALREADY */        "Connection already in progress",
	/* EINPROGRESS */     "Operation in progress",
	/* ENOLCK */          "No locks available",
	/* EUCLEAN */         "Structure needs cleaning",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	/* ENOTRECOVERABLE */ "State not recoverable"
};
/* clang-format on */


static inline const char *strerror_(int errnum, char *buff, size_t bufflen, int *err)
{
	const char *str = NULL;

	if ((errnum >= 0) && (errnum < (int)(sizeof(__errmsgtab) / sizeof(*__errmsgtab)))) {
		str = __errmsgtab[errnum];
	}

	if (str == NULL) {
		(void)snprintf(buff, bufflen, "Unknown error %d", errnum);
		*err = EINVAL;
		str = buff;
	}

	return str;
}


char *strerror(int errnum)
{
	static char unknownMsg[32];
	const char *str;
	int err = 0;

	str = strerror_(errnum, unknownMsg, sizeof(unknownMsg), &err);
	if (err != 0) {
		errno = err;
	}

	return (char *)str;
}


int strerror_r(int errnum, char *buf, size_t buflen)
{
	char unknownMsg[32];
	const char *str;
	int ret = 0;
	size_t len;

	str = strerror_(errnum, unknownMsg, sizeof(unknownMsg), &ret);
	len = strlen(str);
	if (buflen > len) {
		memcpy(buf, str, len + 1);
	}
	else {
		ret = ERANGE;
	}

	return ret;
}


const char *gai_strerror(int errnum)
{
	static char unknownMsg[32];

	switch (errnum) {
		case EAI_AGAIN:
			return "Name could not be resolved at this time";
		case EAI_BADFLAGS:
			return "Flags parameter had an invalid value";
		case EAI_FAIL:
			return "Non-recoverable failure in name resolution";
		case EAI_FAMILY:
			return "Address family was not recognized";
		case EAI_MEMORY:
			return "Memory allocation failure";
		case EAI_NODATA:
			return "No address associated with hostname";
		case EAI_NONAME:
			return "Name does not resolve";
		case EAI_OVERFLOW:
			return "Argument buffer overflow";
		case EAI_SERVICE:
			return "Service was not recognized for socket type";
		case EAI_SOCKTYPE:
			return "Intended socket type was not recognized";
		case EAI_SYSTEM:
			return "System error returned in errno";
		default:
			(void)snprintf(unknownMsg, sizeof(unknownMsg), "Unknown error %d", errnum);
			return unknownMsg;
	}
}

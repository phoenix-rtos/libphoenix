/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd (POSIX routines for user operations)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <unistd.h>
#include <sys/syslimits.h>
#include <sys/statvfs.h>
#include <limits.h>


long sysconf(int name)
{
	switch (name) {
		case _SC_OPEN_MAX:
			/* value got from MAX_FD_COUNT (kernel) */
			/* TODO: come up with a solution to access a macro defined in kernel posix module */
			return 512;
		case _SC_IOV_MAX:
			return IOV_MAX;
		case _SC_ATEXIT_MAX:
			/* we have no limit since we use lists */
			return INT_MAX;
		case _SC_PAGESIZE:
			/* _SC_PAGE_SIZE is synonym */
			return _PAGE_SIZE;
		default:
			errno = EINVAL;
			return -1;
	}
}


long pathconfStatic(int name)
{
	long res = -1;

	switch (name) {
		case _PC_FILESIZEBITS:
#ifdef FILESIZEBITS
			res = FILESIZEBITS;
#endif
			break;
		case _PC_LINK_MAX:
#ifdef LINK_MAX
			res = LINK_MAX;
#endif
			break;
		case _PC_MAX_CANON:
#ifdef MAX_CANON
			res = MAX_CANON;
#endif
			break;
		case _PC_MAX_INPUT:
#ifdef MAX_INPUT
			res = MAX_INPUT;
#endif
			break;
		case _PC_PATH_MAX:
#ifdef PATH_MAX
			res = PATH_MAX;
#endif
			break;
		case _PC_PIPE_BUF:
#ifdef PIPE_BUF
			res = PIPE_BUF;
#endif
			break;
		case _PC_2_SYMLINKS:
#ifdef POSIX2_SYMLINKS
			res = POSIX2_SYMLINKS;
#endif
			break;
		case _PC_ALLOC_SIZE_MIN:
#ifdef POSIX_ALLOC_SIZE_MIN
			res = POSIX_ALLOC_SIZE_MIN;
#endif
			break;
		case _PC_REC_INCR_XFER_SIZE:
#ifdef POSIX_REC_INCR_XFER_SIZE
			res = POSIX_REC_INCR_XFER_SIZE;
#endif
			break;
		case _PC_REC_MAX_XFER_SIZE:
#ifdef POSIX_REC_MAX_XFER_SIZE
			res = POSIX_REC_MAX_XFER_SIZE;
#endif
			break;
		case _PC_REC_MIN_XFER_SIZE:
#ifdef POSIX_REC_MIN_XFER_SIZE
			res = POSIX_REC_MIN_XFER_SIZE;
#endif
			break;
		case _PC_REC_XFER_ALIGN:
#ifdef POSIX_REC_XFER_ALIGN
			res = POSIX_REC_XFER_ALIGN;
#endif
			break;
		case _PC_SYMLINK_MAX:
#ifdef SYMLINK_MAX
			res = SYMLINK_MAX;
#endif
			break;
		case _PC_TEXTDOMAIN_MAX:
#ifdef TEXTDOMAIN_MAX
			res = TEXTDOMAIN_MAX;
#endif
			break;
		case _PC_CHOWN_RESTRICTED:
#ifdef _POSIX_CHOWN_RESTRICTED
			res = _POSIX_CHOWN_RESTRICTED;
#endif
			break;
		case _PC_NO_TRUNC:
#ifdef _POSIX_NO_TRUNC
			res = _POSIX_NO_TRUNC;
#endif
			break;
		case _PC_VDISABLE:
#ifdef _POSIX_VDISABLE
			res = _POSIX_VDISABLE;
#endif
			break;
		case _PC_ASYNC_IO:
#ifdef _POSIX_ASYNC_IO
			res = _POSIX_ASYNC_IO;
#endif
			break;
		case _PC_FALLOC:
#ifdef _POSIX_FALLOC
			res = _POSIX_FALLOC;
#endif
			break;
		case _PC_PRIO_IO:
#ifdef _POSIX_PRIO_IO
			res = _POSIX_PRIO_IO;
#endif
			break;
		case _PC_SYNC_IO:
#ifdef _POSIX_SYNC_IO
			res = _POSIX_SYNC_IO;
#endif
			break;
		case _PC_TIMESTAMP_RESOLUTION:
#ifdef _POSIX_TIMESTAMP_RESOLUTION
			res = _POSIX_TIMESTAMP_RESOLUTION;
#endif
			break;
		default:
			errno = EINVAL;
			break;
	}
	return res;
}


long pathconf(const char *path, int name)
{
	long res = -1;
	struct statvfs stat;

	switch (name) {
		case _PC_NAME_MAX:
			if (statvfs(path, &stat) == 0) {
				res = (long)stat.f_namemax;
			}
			break;
		default:
			res = pathconfStatic(name);
			break;
	}

	return res;
}


long fpathconf(int fildes, int name)
{
	long res = -1;
	struct statvfs stat;

	switch (name) {
		case _PC_NAME_MAX:
			if (fstatvfs(fildes, &stat) == 0) {
				res = (long)stat.f_namemax;
			}
			else if (errno == ENOSYS) {
				errno = EINVAL;
			}
			break;
		default:
			res = pathconfStatic(name);
			break;
	}

	return res;
}

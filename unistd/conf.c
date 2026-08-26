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

#include <phoenix/signal.h>


long sysconf(int name)
{
	switch (name) {
		case _SC_OPEN_MAX:
			/*
			 * FIXME: issue #1610
			 * TODO: Query the max fd count from the kernel and rewrite all
			 * fd POSIX functions to respect the limit.
			 *
			 * For the time being we must return -1 to stay POSIX-compliant, as we
			 * don't know the limit and lying about it is not compliant.
			 */
			return -1;
		case _SC_IOV_MAX:
			return IOV_MAX;
		case _SC_ATEXIT_MAX:
			/* we have no limit since we use lists */
			return INT_MAX;
		case _SC_PAGESIZE:
			/* _SC_PAGE_SIZE is synonym */
			return _PAGE_SIZE;
		case _SC_SPIN_LOCKS:
			return _POSIX_SPIN_LOCKS;

		case _SC_AIO_LISTIO_MAX:
		case _SC_AIO_MAX:
		case _SC_AIO_PRIO_DELTA_MAX:
			/* Asynchronous I/O is not supported. */
			return -1;
		case _SC_DELAYTIMER_MAX:
		case _SC_TIMER_MAX:
			/* POSIX timers are not supported. */
			return -1;
		case _SC_MQ_OPEN_MAX:
		case _SC_MQ_PRIO_MAX:
			/* POSIX message queues are not supported. */
			return -1;
		case _SC_RTSIG_MAX:
		case _SC_SIGQUEUE_MAX:
			/* Real-time signals are not supported. */
			return -1;
		case _SC_SEM_NSEMS_MAX:
		case _SC_SEM_VALUE_MAX:
			/* POSIX semaphores are not supported. */
			return -1;
		case _SC_THREAD_DESTRUCTOR_ITERATIONS:
		case _SC_THREAD_KEYS_MAX:
		case _SC_THREAD_STACK_MIN:
		case _SC_THREAD_THREADS_MAX:
			/* The POSIX Threads option is not fully supported. */
			return -1;
		case _SC_MEMORY_PROTECTION:
			return _POSIX_MEMORY_PROTECTION;
		case _SC_REGEXP:
			return _POSIX_REGEXP;
		case _SC_RE_DUP_MAX:
			return _POSIX_RE_DUP_MAX;
		case _SC_ARG_MAX:
			return _POSIX_ARG_MAX;
		case _SC_BC_BASE_MAX:
			return _POSIX2_BC_BASE_MAX;
		case _SC_BC_DIM_MAX:
			return _POSIX2_BC_DIM_MAX;
		case _SC_BC_SCALE_MAX:
			return _POSIX2_BC_SCALE_MAX;
		case _SC_BC_STRING_MAX:
			return _POSIX2_BC_STRING_MAX;
		case _SC_CHILD_MAX:
			return _POSIX_CHILD_MAX;
		case _SC_COLL_WEIGHTS_MAX:
			return _POSIX2_COLL_WEIGHTS_MAX;
		case _SC_EXPR_NEST_MAX:
			return _POSIX2_EXPR_NEST_MAX;
		case _SC_HOST_NAME_MAX:
			return _POSIX_HOST_NAME_MAX;
		case _SC_LINE_MAX:
			return _POSIX2_LINE_MAX;
		case _SC_LOGIN_NAME_MAX:
			return _POSIX_LOGIN_NAME_MAX;
		case _SC_NGROUPS_MAX:
			return _POSIX_NGROUPS_MAX;
		case _SC_GETGR_R_SIZE_MAX:
			return -1;
		case _SC_GETPW_R_SIZE_MAX:
			return -1;
		case _SC_NPROCESSORS_CONF:
			return -1;
		case _SC_NPROCESSORS_ONLN:
			return -1;
		case _SC_NSIG:
			return NSIG;
		case _SC_STREAM_MAX:
			return _POSIX_STREAM_MAX;
		case _SC_SYMLOOP_MAX:
			return _POSIX_SYMLOOP_MAX;
		case _SC_TTY_NAME_MAX:
			return _POSIX_TTY_NAME_MAX;
		case _SC_TZNAME_MAX:
			return _POSIX_TZNAME_MAX;
		case _SC_ADVISORY_INFO:
		case _SC_BARRIERS:
		case _SC_ASYNCHRONOUS_IO:
		case _SC_CLOCK_SELECTION:
		case _SC_CPUTIME:
		case _SC_DEVICE_CONTROL:
		case _SC_FSYNC:
		case _SC_IPV6:
		case _SC_JOB_CONTROL:
		case _SC_MAPPED_FILES:
		case _SC_MEMLOCK:
		case _SC_MEMLOCK_RANGE:
		case _SC_MESSAGE_PASSING:
		case _SC_MONOTONIC_CLOCK:
		case _SC_PRIORITIZED_IO:
		case _SC_PRIORITY_SCHEDULING:
		case _SC_RAW_SOCKETS:
		case _SC_READER_WRITER_LOCKS:
		case _SC_REALTIME_SIGNALS:
		case _SC_SAVED_IDS:
		case _SC_SEMAPHORES:
		case _SC_SHARED_MEMORY_OBJECTS:
		case _SC_SHELL:
		case _SC_SPAWN:
		case _SC_SPORADIC_SERVER:
		case _SC_SS_REPL_MAX:
		case _SC_SYNCHRONIZED_IO:
		case _SC_THREAD_ATTR_STACKADDR:
		case _SC_THREAD_ATTR_STACKSIZE:
		case _SC_THREAD_CPUTIME:
		case _SC_THREAD_PRIO_INHERIT:
		case _SC_THREAD_PRIO_PROTECT:
		case _SC_THREAD_PRIORITY_SCHEDULING:
		case _SC_THREAD_PROCESS_SHARED:
		case _SC_THREAD_ROBUST_PRIO_INHERIT:
		case _SC_THREAD_ROBUST_PRIO_PROTECT:
		case _SC_THREAD_SAFE_FUNCTIONS:
		case _SC_THREAD_SPORADIC_SERVER:
		case _SC_THREADS:
		case _SC_TIMEOUTS:
		case _SC_TIMERS:
		case _SC_TYPED_MEMORY_OBJECTS:
		case _SC_VERSION:
		case _SC_V8_ILP32_OFF32:
		case _SC_V8_ILP32_OFFBIG:
		case _SC_V8_LP64_OFF64:
		case _SC_V8_LPBIG_OFFBIG:
		case _SC_V7_ILP32_OFF32:
		case _SC_V7_ILP32_OFFBIG:
		case _SC_V7_LP64_OFF64:
		case _SC_V7_LPBIG_OFFBIG:
		case _SC_2_C_BIND:
		case _SC_2_C_DEV:
		case _SC_2_CHAR_TERM:
		case _SC_2_FORT_RUN:
		case _SC_2_LOCALEDEF:
		case _SC_2_SW_DEV:
		case _SC_2_UPE:
		case _SC_2_VERSION:
		case _SC_XOPEN_CRYPT:
		case _SC_XOPEN_ENH_I18N:
		case _SC_XOPEN_REALTIME:
		case _SC_XOPEN_REALTIME_THREADS:
		case _SC_XOPEN_SHM:
		case _SC_XOPEN_UNIX:
		case _SC_XOPEN_UUCP:
		case _SC_XOPEN_VERSION:
			/* Don't touch errno if if an option is recognized but unsupported / no limit is known. */
			return -1;

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

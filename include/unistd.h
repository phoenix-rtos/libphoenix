/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd.h
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Kamil Amanowicz, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#ifndef _LIBPHOENIX_UNISTD_H_
#define _LIBPHOENIX_UNISTD_H_


#include <sys/cdefs.h>
#include <arch.h>
#include <stddef.h>
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2


#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2


#define F_OK 0
#define R_OK (1 << 2)
#define W_OK (1 << 1)
#define X_OK (1 << 0)


#define _SC_OPEN_MAX                     0
#define _SC_IOV_MAX                      1
#define _SC_ATEXIT_MAX                   2
#define _SC_CLK_TCK                      3
#define _SC_PAGESIZE                     4
#define _SC_PAGE_SIZE                    _SC_PAGESIZE /* spec. 1170 compatibility */
#define _SC_SPIN_LOCKS                   5
#define _SC_AIO_LISTIO_MAX               6
#define _SC_AIO_MAX                      7
#define _SC_AIO_PRIO_DELTA_MAX           8
#define _SC_ARG_MAX                      9
#define _SC_BC_BASE_MAX                  10
#define _SC_BC_DIM_MAX                   11
#define _SC_BC_SCALE_MAX                 12
#define _SC_BC_STRING_MAX                13
#define _SC_CHILD_MAX                    14
#define _SC_COLL_WEIGHTS_MAX             15
#define _SC_DELAYTIMER_MAX               16
#define _SC_EXPR_NEST_MAX                17
#define _SC_HOST_NAME_MAX                18
#define _SC_LINE_MAX                     19
#define _SC_LOGIN_NAME_MAX               20
#define _SC_NGROUPS_MAX                  21
#define _SC_GETGR_R_SIZE_MAX             22
#define _SC_GETPW_R_SIZE_MAX             23
#define _SC_MQ_OPEN_MAX                  24
#define _SC_MQ_PRIO_MAX                  25
#define _SC_NPROCESSORS_CONF             26
#define _SC_NPROCESSORS_ONLN             27
#define _SC_NSIG                         28
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 29
#define _SC_THREAD_KEYS_MAX              30
#define _SC_THREAD_STACK_MIN             31
#define _SC_THREAD_THREADS_MAX           32
#define _SC_RE_DUP_MAX                   33
#define _SC_RTSIG_MAX                    34
#define _SC_SEM_NSEMS_MAX                35
#define _SC_SEM_VALUE_MAX                36
#define _SC_SIGQUEUE_MAX                 37
#define _SC_STREAM_MAX                   38
#define _SC_SYMLOOP_MAX                  39
#define _SC_TIMER_MAX                    40
#define _SC_TTY_NAME_MAX                 41
#define _SC_TZNAME_MAX                   42
#define _SC_ADVISORY_INFO                43
#define _SC_BARRIERS                     44
#define _SC_ASYNCHRONOUS_IO              45
#define _SC_CLOCK_SELECTION              46
#define _SC_CPUTIME                      47
#define _SC_DEVICE_CONTROL               48
#define _SC_FSYNC                        49
#define _SC_IPV6                         50
#define _SC_JOB_CONTROL                  51
#define _SC_MAPPED_FILES                 52
#define _SC_MEMLOCK                      53
#define _SC_MEMLOCK_RANGE                54
#define _SC_MEMORY_PROTECTION            55
#define _SC_MESSAGE_PASSING              56
#define _SC_MONOTONIC_CLOCK              57
#define _SC_PRIORITIZED_IO               58
#define _SC_PRIORITY_SCHEDULING          59
#define _SC_RAW_SOCKETS                  60
#define _SC_READER_WRITER_LOCKS          61
#define _SC_REALTIME_SIGNALS             62
#define _SC_REGEXP                       63
#define _SC_SAVED_IDS                    64
#define _SC_SEMAPHORES                   65
#define _SC_SHARED_MEMORY_OBJECTS        66
#define _SC_SHELL                        67
#define _SC_SPAWN                        68
#define _SC_SPORADIC_SERVER              69
#define _SC_SS_REPL_MAX                  70
#define _SC_SYNCHRONIZED_IO              71
#define _SC_THREAD_ATTR_STACKADDR        72
#define _SC_THREAD_ATTR_STACKSIZE        73
#define _SC_THREAD_CPUTIME               74
#define _SC_THREAD_PRIO_INHERIT          75
#define _SC_THREAD_PRIO_PROTECT          76
#define _SC_THREAD_PRIORITY_SCHEDULING   77
#define _SC_THREAD_PROCESS_SHARED        78
#define _SC_THREAD_ROBUST_PRIO_INHERIT   79
#define _SC_THREAD_ROBUST_PRIO_PROTECT   80
#define _SC_THREAD_SAFE_FUNCTIONS        81
#define _SC_THREAD_SPORADIC_SERVER       82
#define _SC_THREADS                      83
#define _SC_TIMEOUTS                     84
#define _SC_TIMERS                       85
#define _SC_TYPED_MEMORY_OBJECTS         86
#define _SC_VERSION                      87
#define _SC_V8_ILP32_OFF32               88
#define _SC_V8_ILP32_OFFBIG              89
#define _SC_V8_LP64_OFF64                90
#define _SC_V8_LPBIG_OFFBIG              91
#define _SC_V7_ILP32_OFF32               92
#define _SC_V7_ILP32_OFFBIG              93
#define _SC_V7_LP64_OFF64                94
#define _SC_V7_LPBIG_OFFBIG              95
#define _SC_2_C_BIND                     96
#define _SC_2_C_DEV                      97
#define _SC_2_CHAR_TERM                  98
#define _SC_2_FORT_RUN                   99
#define _SC_2_LOCALEDEF                  100
#define _SC_2_SW_DEV                     101
#define _SC_2_UPE                        102
#define _SC_2_VERSION                    103
#define _SC_XOPEN_CRYPT                  104
#define _SC_XOPEN_ENH_I18N               105
#define _SC_XOPEN_REALTIME               106
#define _SC_XOPEN_REALTIME_THREADS       107
#define _SC_XOPEN_SHM                    108
#define _SC_XOPEN_UNIX                   109
#define _SC_XOPEN_UUCP                   110
#define _SC_XOPEN_VERSION                111

#define _POSIX_NO_TRUNC             1
#define _POSIX_ASYNC_IO             -1 /* Async IO not implemented. */
#define _POSIX_FALLOC               -1 /* posix_fallocate not implemented. */
#define _POSIX_PRIO_IO              -1 /* Async IO not implemented. */
#define _POSIX_SYNC_IO              -1 /* Sync IO not implemented. */
#define _POSIX_TIMESTAMP_RESOLUTION -1 /* FS timestamps have a resolution up to a second. */
#define _POSIX_VDISABLE             '\0'

#define _POSIX_CLOCK_SELECTION     202405L
#define _POSIX_FSYNC               202405L
#define _POSIX_READER_WRITER_LOCKS 202405L
#define _POSIX_REGEXP              202405L
#define _POSIX_SPIN_LOCKS          202405L

#ifndef NOMMU
#define _POSIX_MEMORY_PROTECTION 202405L
#else
#define _POSIX_MEMORY_PROTECTION -1 /* mprotect() has no effect without MMU. */
#endif

#define _PC_FILESIZEBITS         0
#define _PC_LINK_MAX             1
#define _PC_MAX_CANON            2
#define _PC_MAX_INPUT            3
#define _PC_PATH_MAX             4
#define _PC_PIPE_BUF             5
#define _PC_2_SYMLINKS           6
#define _PC_ALLOC_SIZE_MIN       7
#define _PC_REC_INCR_XFER_SIZE   8
#define _PC_REC_MAX_XFER_SIZE    9
#define _PC_REC_MIN_XFER_SIZE    10
#define _PC_REC_XFER_ALIGN       11
#define _PC_SYMLINK_MAX          12
#define _PC_TEXTDOMAIN_MAX       13
#define _PC_CHOWN_RESTRICTED     14
#define _PC_NO_TRUNC             15
#define _PC_VDISABLE             16
#define _PC_ASYNC_IO             17
#define _PC_FALLOC               18
#define _PC_PRIO_IO              19
#define _PC_SYNC_IO              20
#define _PC_TIMESTAMP_RESOLUTION 21
#define _PC_NAME_MAX             22


long sysconf(int name);


long pathconf(const char *path, int name);


long fpathconf(int fildes, int name);


/* NOTE: Legacy from SUSv2, new applications should use sysconf(_SC_PAGESIZE) */
__INLINE int getpagesize(void)
{
	return (int)sysconf(_SC_PAGESIZE);
}


int vfork(void);


pid_t fork(void);


int execl(const char *path, const char *arg, ...);


int execle(const char *path, const char *arg, ...);


int execlp(const char *path, const char *arg, ...);


int execve(const char *path, char *const argv[], char *const envp[]);


int execvp(const char *file, char *const argv[]);


int execvpe(const char *file, char *const argv[], char *const envp[]);


int execv(const char *path, char *const argv[]);


pid_t getpid(void);


pid_t getppid(void);


uid_t getuid(void);


uid_t geteuid(void);


gid_t getgid(void);


gid_t getegid(void);


int setuid(uid_t uid);


int seteuid(uid_t uid);


int setgid(gid_t gid);


int setegid(gid_t gid);


int usleep(useconds_t usecs);


unsigned int sleep(unsigned int seconds);


int fsync(int fd);


void sync(void);


int chdir(const char *path);


int fchdir(int fd);


int fchown(int fd, uid_t owner, gid_t group);


int rmdir(const char *path);


ssize_t read(int fildes, void *buf, size_t nbyte);


ssize_t write(int fildes, const void *buf, size_t nbyte);


ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset);


ssize_t pwrite(int fildes, const void *buf, size_t nbyte, off_t offset);


int dup(int fildes);


int isatty(int fildes);


int dup2(int fildes, int fildes2);


void _exit(int status) __attribute__((__noreturn__));


/* Creates a symbolic link named linkpath which contains the string target. If linkpath exists, it will not be overwritten. */
int symlink(const char *target, const char *linkpath);


/* Creates a new link (also known as a hard link) to an existing file. If newpath exists, it will not be overwritten. */
int link(const char *oldpath, const char *newpath);


int unlink(const char *pathname);


ssize_t readlink(const char *path, char *buf, size_t bufsiz);


int access(const char *pathname, int mode);


char *getcwd(char *buf, size_t size);


int getopt(int argc, char *const argv[], const char *optstring);


int close(int fildes);


off_t lseek(int fildes, off_t offset, int whence);


int pipe(int fildes[2]);


int truncate(const char *path, off_t length);


int ftruncate(int fildes, off_t length);


unsigned int alarm(unsigned int seconds);


int issetugid(void);


char *getlogin(void);


int getlogin_r(char *name, size_t namesize);


char *crypt(const char *key, const char *salt);


int setpgid(pid_t pid, pid_t pgid);


pid_t getpgid(pid_t pid);


int setpgrp(void);


pid_t getpgrp(void);


pid_t setsid(void);


pid_t getsid(pid_t pid);


int gethostname(char *name, size_t namelen);


int sethostname(const char *name, size_t namelen);


long gethostid(void);


int sethostid(long hostid);


char *getpass(const char *prompt);


int getgroups(int size, gid_t list[]);


char *ttyname(int fildes);


int ttyname_r(int fildes, char *name, size_t namesize);


int pause(void);


int chroot(const char *path);


int nice(int incr);


extern char *optarg;


extern int optind, opterr, optopt;


extern char **__environ, **environ;


#ifdef __cplusplus
}
#endif


#endif

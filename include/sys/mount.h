/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/mount
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_MOUNT_H_
#define _LIBPHOENIX_SYS_MOUNT_H_


#ifdef __cplusplus
extern "C" {
#endif


#define MS_MGC_VAL     0xc0ed0000u
#define MS_RDONLY      (1 << 0)  /* Mount read-only.  */
#define MS_NOSUID      (1 << 1)  /* Ignore suid and sgid bits.  */
#define MS_NODEV       (1 << 2)  /* Disallow access to device special files.  */
#define MS_NOEXEC      (1 << 3)  /* Disallow program execution.  */
#define MS_SYNCHRONOUS (1 << 4)  /* Writes are synced at once.  */
#define MS_REMOUNT     (1 << 5)  /* Alter flags of a mounted FS.  */
#define MS_MANDLOCK    (1 << 6)  /* Allow mandatory locks on an FS.  */
#define MS_DIRSYNC     (1 << 7)  /* Directory modifications are synchronous.  */
#define MS_NOSYMFOLLOW (1 << 8)  /* Do not follow symlinks.  */
#define MS_NOATIME     (1 << 10) /* Do not update access times.  */
#define MS_NODIRATIME  (1 << 11) /* Do not update directory access times.  */
#define MS_BIND        (1 << 12) /* Bind directory at different place.  */
#define MS_MOVE        (1 << 13)
#define MS_REC         (1 << 14)
#define MS_SILENT      (1 << 15)


extern int mount(const char *source, const char *target, const char *fstype, unsigned long mode, const char *data);


extern int umount(const char *target);



#ifdef __cplusplus
}
#endif


#endif

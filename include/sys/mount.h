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

extern int mount(const char *source, const char *target, const char *fstype, long mode, char *data);


extern int umount(const char *path);

#endif

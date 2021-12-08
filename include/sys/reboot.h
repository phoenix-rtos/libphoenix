/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/reboot.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_REBOOT_H_
#define _LIBPHOENIX_SYS_REBOOT_H_


#define RB_HALT_SYSTEM 0
#define RB_AUTOBOOT    1

#define PHOENIX_REBOOT_MAGIC 0xaa55aa55UL


extern int reboot(int magic);


#endif

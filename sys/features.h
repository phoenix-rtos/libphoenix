/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/features.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_FEATURES_H_
#define _SYS_FEATURES_H_

#ifndef __GNUC_PREREQ
#if defined __GNUC__ && defined __GNUC_MINOR__
#define __GNUC_PREREQ(maj, min) ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#define __GNUC_PREREQ(maj, min) 0
#endif
#endif

#define __GNUC_PREREQ__(ma, mi) __GNUC_PREREQ(ma, mi)


#endif

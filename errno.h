/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Error definitions
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ERRNO_H_
#define _LIBPHOENIX_ERRNO_H_


#define  EOK           0  /* No error */
#define  EPERM         1  /* Operation not permitted */
#define  ENOENT        2  /* No such file or directory */
#define  ENOMEM       12  /* Out of memory */
#define  EFAULT       14  /* Bad address */
#define  EBUSY        16  /* Device or resource busy */
#define  EEXIST       17  /* File exists */
#define  EINVAL       22  /* Invalid argument */
#define  ENOTTY       25  /* Not a typewriter */
#define  ETIME        62  /* Timer expired */


#endif

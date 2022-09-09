/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ulimit.h
 *
 * Copyright 2022 Phoenix Systems
 * Author: Adam Debek
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _ULIMIT_H
#define _ULIMIT_H

/* Constants used as the first parameter for `ulimit'.  They denote limits
   which can be set or retrieved using this function.  */

#define UL_GETFSIZE 1 /* Return limit on the size of a file, in units of 512 bytes.  */
#define UL_SETFSIZE 2 /* Set limit on the size of a file to second argument.  */

/* Control process limits according to CMD.  */
extern long ulimit(int __cmd, ...);


#endif /* ulimit.h */

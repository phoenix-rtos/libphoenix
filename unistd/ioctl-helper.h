/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ioctl special case input serialization/deserialization
 *
 * Copyright 2025 Phoenix Systems
 * Author: Julian Uziembło
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_IOCTL_HELPER_H_
#define _LIBPHOENIX_IOCTL_HELPER_H_

#include <unistd.h>


/* deallocates memory if it was allocated by ioctl_serialize */
void ioctl_free(unsigned long request, void *packed);


/* serialize the request's structure into packed.
   allocates memory in special cases to be freed by ioctl_free */
int ioctl_serialize(unsigned long request, void *ioctl_val, void **packed, size_t *packed_size);


/* deserialize the request's structure from packed back into ioctl_val.
   deallocates memory if it was allocated by ioctl_serialize */
void ioctl_deserialize(unsigned long request, void *ioctl_val, void *packed);


#endif /* _LIBPHOENIX_IOCTL_HELPER_H_ */

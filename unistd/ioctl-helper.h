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


/* serialize the request's structure into out_ptr.
   allocates memory in special cases to be freed by
   ioctl_deserialize, does nothing for all other cases */
int ioctl_serialize(unsigned long request, void *ioctl_val, void **out_ptr, size_t *out_size);


/* deserialize the request's structure from in_ptr back into ioctl_val.
   deallocates memory if it was allocated by ioctl_serialize */
void ioctl_deserialize(unsigned long request, void *ioctl_val, void *in_ptr, size_t size);


#endif /* _LIBPHOENIX_IOCTL_HELPER_H_ */

/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * pthread
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_PTHREAD_H_
#define _LIBPHOENIX_PTHREAD_H_

#include <stdint.h>



extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);


extern int pthread_attr_init(pthread_attr_t *attr);


extern int pthread_attr_destroy(pthread_attr_t *attr);


#endif

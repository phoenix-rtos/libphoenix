/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Internal file operations
 *
 * Copyright 2024 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_INTERNAL_FILE_H_
#define _LIBPHOENIX_INTERNAL_FILE_H_


#include <sys/types.h>


ssize_t __safe_write(int fd, const void *buf, size_t size);


ssize_t __safe_pwrite(int fd, const void *buf, size_t size, off_t offset);


/* non-blocking variant - returns on EAGAIN */
ssize_t __safe_write_nb(int fd, const void *buf, size_t size);


/* non-blocking variant - returns on EAGAIN */
ssize_t __safe_pwrite_nb(int fd, const void *buf, size_t size, off_t offset);


ssize_t __safe_read(int fd, void *buf, size_t size);


ssize_t __safe_pread(int fd, void *buf, size_t size, off_t offset);


/* non-blocking variant - returns on EAGAIN */
ssize_t __safe_read_nb(int fd, void *buf, size_t size);


/* non-blocking variant - returns on EAGAIN */
ssize_t __safe_pread_nb(int fd, void *buf, size_t size, off_t offset);


int __safe_open(const char *path, int oflag, mode_t mode);


int __safe_close(int fd);


#endif

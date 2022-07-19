/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * alloca.h
 *
 * Copyright 2017-2018 Phoenix Systems
 * Author: Marek Bialowas
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef ALLOCA_H
#define ALLOCA_H


#ifdef __cplusplus
extern "C" {
#endif


#undef alloca

extern void *alloca(size_t size);


#ifdef  __GNUC__
#define alloca(size)   __builtin_alloca (size)
#endif


#ifdef __cplusplus
}
#endif


#endif /* ALLOCA_H */

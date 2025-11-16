/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2022 by GTD GmbH. */

#ifndef LIBMCS_CONFIG_H
#define LIBMCS_CONFIG_H

#ifdef LIBMCS_FPU_DAZ
    #undef LIBMCS_FPU_DAZ
#endif /* LIBMCS_FPU_DAZ */
#ifndef LIBMCS_LONG_DOUBLE_IS_64BITS
    #define LIBMCS_LONG_DOUBLE_IS_64BITS
#endif /* !LIBMCS_LONG_DOUBLE_IS_64BITS */
#ifndef LIBMCS_WANT_COMPLEX
    #define LIBMCS_WANT_COMPLEX
#endif /* !LIBMCS_WANT_COMPLEX */

#endif /* !LIBMCS_CONFIG_H */

/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This file contains a set of functions used by multiple procedures as
 * internal functions. These procedures should not be accessed directly by a
 * user. Note that all procedures are either macros or static inline procedures.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include "tools.h"
 *     EXTRACT_WORDS(ix0,ix1,d);                // this macro has no return value, the inputs are expected to be (int, int, double)
 *     GET_HIGH_WORD(i,d);                      // this macro has no return value, the inputs are expected to be (int, double)
 *     GET_LOW_WORD(i,d);                       // this macro has no return value, the inputs are expected to be (int, double)
 *     INSERT_WORDS(d,ix0,ix1);                 // this macro has no return value, the inputs are expected to be (double, int, int)
 *     SET_HIGH_WORD(d,v);                      // this macro has no return value, the inputs are expected to be (double, int)
 *     SET_LOW_WORD(d,v);                       // this macro has no return value, the inputs are expected to be (double, int)
 *     GET_FLOAT_WORD(i,d);                     // this macro has no return value, the inputs are expected to be (int, float)
 *     SET_FLOAT_WORD(d,i);                     // this macro has no return value, the inputs are expected to be (float, int)
 *     SAFE_RIGHT_SHIFT(op,amt);                // this macros return value has the same type as input `op`, the inputs are expected to be integer types
 *     double __forced_calculation(double x);
 *     float __forced_calculationf(float x);
 *     double __raise_invalid(void);
 *     float __raise_invalidf(void);
 *     double __raise_div_by_zero(double x);
 *     float __raise_div_by_zerof(float x);
 *     double __raise_overflow(double x);
 *     float __raise_overflowf(float x);
 *     double __raise_underflow(double x);
 *     float __raise_underflowf(float x);
 *     double __raise_inexact(double x);
 *     float __raise_inexactf(float x);
 *     int __issignaling(double x);
 *     int __issignalingf(float x);
 *     REAL_PART(z);                            // this macros return value has the non-complex type of input `z`, the input is expected to be a complex floating-point datum
 *     IMAG_PART(z);                            // this macros return value has the non-complex type of input `z`, the input is expected to be a complex floating-point datum
 *
 * Description
 * ===========
 *
 * ``EXTRACT_WORDS`` is a macro to extract two integer words from a double
 * floating-point datum.
 *
 * ``GET_HIGH_WORD`` is a macro to extract only the highword of the two integer
 * words from a double floating-point datum.
 *
 * ``GET_LOW_WORD`` is a macro to extract only the lowword of the two integer
 * words from a double floating-point datum.
 *
 * ``INSERT_WORDS`` is a macro to insert two integer words into a double
 * floating-point datum.
 *
 * ``SET_HIGH_WORD`` is a macro to insert only the highword of the two integer
 * words into a double floating-point datum.
 *
 * ``SET_LOW_WORD`` is a macro to insert only the lowword of the two integer
 * words into a double floating-point datum.
 *
 * ``GET_FLOAT_WORD`` is a macro to extract the integer representation from a
 * single floating-point datum.
 *
 * ``SET_FLOAT_WORD`` is a macro to insert the integer representation into a
 * single floating-point datum.
 *
 * ``SAFE_RIGHT_SHIFT`` is a macro to avoid undefined behaviour that can arise
 * if the amount of a right shift is exactly equal to the size of the shifted
 * operand. If the amount is equal to the size the macro returns 0.
 *
 * ``__forced_calculation`` is a function to force the execution of the input
 * to go through the :ref:`FPU <ABBR>`. The input for this function is usually
 * an arithmetic operation and not a single value. At the moment the function
 * is only used by others within this file and not expected to be called from
 * outside.
 *
 * ``__raise_invalid`` is a function to force the :ref:`FPU <ABBR>` to generate
 * an ``invalid operation`` exception.
 *
 * ``__raise_div_by_zero`` is a function to force the :ref:`FPU <ABBR>` to
 * generate an ``divide by zero`` exception.
 *
 * ``__raise_overflow`` is a function to force the :ref:`FPU <ABBR>` to
 * generate an ``overflow`` exception.
 *
 * ``__raise_underflow`` is a function to force the :ref:`FPU <ABBR>` to
 * generate an ``underflow`` exception. Even though the library usually does
 * not care about this exception for qualification purposes, the library still
 * tries to conform to the standards (mainly POSIX) in regards to where the
 * exception shall be raised intentionally. In those places this function is
 * used.
 *
 * ``__raise_inexact`` is a function to force the :ref:`FPU <ABBR>` to generate
 * an ``inexact`` exception. Even though the library usually does not care
 * about this exception for qualification purposes, the library still tries to
 * conform to the standards (mainly POSIX) in regards to where the exception
 * shall be raised intentionally. In those places this function is used.
 *
 * ``__issignaling`` is a function to check if a value is a signaling NaN.
 *
 * ``REAL_PART`` is a macro to extract only the real part of a complex
 * floating-point datum.
 *
 * ``IMAG_PART`` is a macro to extract only the imaginary part of a complex
 * floating-point datum.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    EXTRACT\_WORDS_{ix0}(d) &= \text{highword of } d  \\
 *    EXTRACT\_WORDS_{ix1}(d) &= \text{lowword of } d  \\
 *    GET\_HIGH\_WORD_{i}(d) &= \text{highword of } d  \\
 *    GET\_LOW\_WORD_{i}(d) &= \text{lowword of } d  \\
 *    SET\_HIGH\_WORD_{\text{highword of } d}(d, v) &= d \text{ with highword } v  \\
 *    SET\_LOW\_WORD_{\text{lowword of } d}(d, v) &= d \text{ with lowword } v  \\
 *    GET\_FLOAT\_WORD_{i}(d) &= \text{integer representation of } d  \\
 *    SET\_FLOAT\_WORD_{d}(i) &= \text{floating-point representation of } i  \\
 *    SAFE\_RIGHT\_SHIFT(op, amt) &= \left\{\begin{array}{ll} op \gg amt, & amt < \text{size of } op \\ 0, & otherwise \end{array}\right.  \\
 *    \_\_forced\_calculation(x) &= x  \\
 *    \_\_raise\_invalid() &= NaN  \\
 *    \_\_raise\_div\_by\_zero(x) &= \left\{\begin{array}{ll} -Inf, & x \in \mathbb{F}^{-} \\ +Inf, & otherwise \end{array}\right.  \\
 *    \_\_raise\_overflow(x) &= \left\{\begin{array}{ll} -Inf, & x \in \mathbb{F}^{-} \\ +Inf, & otherwise \end{array}\right.  \\
 *    \_\_raise\_underflow(x) &= \left\{\begin{array}{ll} -0.0, & x \in \mathbb{F}^{-} \\ +0.0, & otherwise \end{array}\right.  \\
 *    \_\_raise\_inexact(x) &= x  \\
 *    \_\_issignaling(x) &= \left\{\begin{array}{ll} 1, & x = sNaN \\ 0, & otherwise \end{array}\right.  \\
 *    REAL\_PART(z) &= \Re(z)  \\
 *    IMAG\_PART(z) &= \Im(z)
 *
 * Returns
 * =======
 *
 * ``EXTRACT_WORDS`` has no return value. It places the two extracted integer
 * words into the parameters ``ix0`` and ``ix1``.
 *
 * ``GET_HIGH_WORD`` has no return value. It places the extracted integer word
 * into the parameter ``i``.
 *
 * ``GET_LOW_WORD`` has no return value. It places the extracted integer word
 * into the parameter ``i``.
 *
 * ``INSERT_WORDS`` has no return value. It places the created double
 * floating-point datum into the parameter ``d``.
 *
 * ``SET_HIGH_WORD`` has no return value. It places the created double
 * floating-point datum into the parameter ``d``.
 *
 * ``SET_LOW_WORD`` has no return value. It places the created double
 * floating-point datum into the parameter ``d``.
 *
 * ``GET_FLOAT_WORD`` has no return value. It places the extracted integer word
 * into the parameter ``i``.
 *
 * ``SET_FLOAT_WORD`` has no return value. It places the created single
 * floating-point datum into the parameter ``d``.
 *
 * ``SAFE_RIGHT_SHIFT`` returns the value ``op`` right shifted by ``amt`` if
 * ``amt`` is smaller than the size of ``op``, otherwise it returns zero.
 *
 * ``__forced_calculation`` returns the input value, or rather the result of
 * the arithmetic calculation used as an input.
 *
 * ``__raise_invalid`` returns ``NaN``.
 *
 * ``__raise_div_by_zero`` returns ``-Inf`` for negative inputs and ``+Inf``
 * for positive inputs.
 *
 * ``__raise_overflow`` returns ``-Inf`` for negative inputs and ``+Inf`` for
 * positive inputs.
 *
 * ``__raise_underflow`` returns ``-0.0`` for negative inputs and ``+0.0`` for
 * positive inputs.
 *
 * ``__raise_inexact`` returns the input value.
 *
 * ``__issignaling`` returns ``1`` if the input value is a signaling ``NaN``,
 * else ``0``.
 *
 * ``REAL_PART`` returns the real part of the input as a floating-point datum
 * of the adequate size.
 *
 * ``IMAG_PART`` returns the imaginary part of the input as a floating-point
 * datum of the adequate size.
 *
 * Exceptions
 * ==========
 *
 * The macros do not raise exceptions.
 *
 * The functions raise the exceptions they are named after, it is their sole
 * purpose. Additionally the procedures ``__raise_overflow`` and
 * ``__raise_underflow`` may raise ``inexact``.
 *
 */

#ifndef LIBMCS_TOOLS_H
#define LIBMCS_TOOLS_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include <math.h>

#ifdef __LIBMCS_WANT_COMPLEX
    #include <complex.h>
#endif /* __LIBMCS_WANT_COMPLEX */

/* A union which permits us to convert between a double and two 32 bit
   ints.  */

#ifdef __IEEE_BIG_ENDIAN

typedef union {
    double value;
    struct {
        uint32_t msw;
        uint32_t lsw;
    } parts;
} ieee_double_shape_type;

#endif

#ifdef __IEEE_LITTLE_ENDIAN

typedef union {
    double value;
    struct {
        uint32_t lsw;
        uint32_t msw;
    } parts;
} ieee_double_shape_type;

#endif

/* Get two 32 bit ints from a double.  */

#define EXTRACT_WORDS(ix0,ix1,d)                        \
    do {                                                \
        ieee_double_shape_type ew_u;                    \
        ew_u.value = (d);                               \
        (ix0) = ew_u.parts.msw;                         \
        (ix1) = ew_u.parts.lsw;                         \
    } while (0 == 1)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)                              \
    do {                                                \
        ieee_double_shape_type gh_u;                    \
        gh_u.value = (d);                               \
        (i) = gh_u.parts.msw;                           \
    } while (0 == 1)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)                               \
    do {                                                \
        ieee_double_shape_type gl_u;                    \
        gl_u.value = (d);                               \
        (i) = gl_u.parts.lsw;                           \
    } while (0 == 1)

/* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)                         \
    do {                                                \
        ieee_double_shape_type iw_u;                    \
        iw_u.parts.msw = (ix0);                         \
        iw_u.parts.lsw = (ix1);                         \
        (d) = iw_u.value;                               \
    } while (0 == 1)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)                              \
    do {                                                \
        ieee_double_shape_type sh_u;                    \
        sh_u.value = (d);                               \
        sh_u.parts.msw = (v);                           \
        (d) = sh_u.value;                               \
    } while (0 == 1)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)                               \
    do {                                                \
        ieee_double_shape_type sl_u;                    \
        sl_u.value = (d);                               \
        sl_u.parts.lsw = (v);                           \
        (d) = sl_u.value;                               \
    } while (0 == 1)

/* A union which permits us to convert between a float and a 32 bit
   int.  */

typedef union {
    float value;
    uint32_t word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)                             \
    do {                                                \
        ieee_float_shape_type gf_u;                     \
        gf_u.value = (d);                               \
        (i) = gf_u.word;                                \
    } while (0 == 1)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)                             \
    do {                                                \
        ieee_float_shape_type sf_u;                     \
        sf_u.word = (i);                                \
        (d) = sf_u.value;                               \
    } while (0 == 1)

/* Macros to avoid undefined behaviour that can arise if the amount
   of a shift is exactly equal to the size of the shifted operand.  */

#define SAFE_RIGHT_SHIFT(op,amt)                        \
    (((amt) < 8 * sizeof(op)) ? ((op) >> (amt)) : 0)

/* Exception raising and signaling check functions */

static inline double __forced_calculation(double x);
static inline float __forced_calculationf(float x);

static inline double __raise_invalid(void);
static inline double __raise_div_by_zero(double x);
static inline double __raise_overflow(double x);
static inline double __raise_underflow(double x);
static inline double __raise_inexact(double x);

static inline float __raise_invalidf(void);
static inline float __raise_div_by_zerof(float x);
static inline float __raise_overflowf(float x);
static inline float __raise_underflowf(float x);
static inline float __raise_inexactf(float x);

static inline int __issignaling(double x);
static inline int __issignalingf(float x);


static inline double __forced_calculation(double x) {
    volatile double r = x;
    return r;
}
static inline float __forced_calculationf(float x) {
    volatile float r = x;
    return r;
}

static inline double __raise_invalid(void) {
    double r = __forced_calculation(0.0 / 0.0);
    return r;
}
static inline double __raise_div_by_zero(double x) {
    return (signbit(x) != 0) ? __forced_calculation(-1.0 / 0.0) : __forced_calculation(1.0 / 0.0);
}
static inline double __raise_overflow(double x) {
    volatile double huge = 1.0e300;
    return (signbit(x) != 0) ? -__forced_calculation(huge * huge) : __forced_calculation(huge * huge);
}
static inline double __raise_underflow(double x) {
    volatile double tiny = 1.0e-300;
    return (signbit(x) != 0) ? -__forced_calculation(tiny * tiny) : __forced_calculation(tiny * tiny);
}
static inline double __raise_inexact(double x) {
    volatile double huge = 1.0e300;
    return (__forced_calculation(huge - 1.0e-300) != 0.0) ? x : 0.0;
}
static inline float __raise_invalidf(void) {
    float r = __forced_calculationf(0.0f / 0.0f);
    return r;
}
static inline float __raise_div_by_zerof(float x) {
    return (signbit(x) != 0) ? __forced_calculationf(-1.0f / 0.0f) : __forced_calculationf(1.0f / 0.0f);
}
static inline float __raise_overflowf(float x) {
    volatile float huge = 1.0e30f;
    return (signbit(x) != 0) ? -__forced_calculationf(huge * huge) : __forced_calculationf(huge * huge);
}
static inline float __raise_underflowf(float x) {
    volatile float tiny = 1.0e-30f;
    return (signbit(x) != 0) ? -__forced_calculationf(tiny * tiny) : __forced_calculationf(tiny * tiny);
}
static inline float __raise_inexactf(float x) {
    volatile float huge = 1.0e30f;
    return (__forced_calculationf(huge - 1.0e-30f) != 0.0f) ? x : 0.0f;
}

static inline int __issignaling(double x) {
    uint32_t hx;
    GET_HIGH_WORD(hx, x);
    if (isnan(x) && (hx & 0x00080000U) == 0) {
        return 1;
    }
    else {
        return 0;
    }
}
static inline int __issignalingf(float x) {
    uint32_t ix;
    GET_FLOAT_WORD(ix, x);
    if (FLT_UWORD_IS_NAN(ix & 0x7fffffffU) && (ix & 0x00400000U) == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

#ifdef __LIBMCS_WANT_COMPLEX
    /*
     * Quoting from ISO/IEC 9899:TC2:
     *
     * 6.2.5.13 Types
     * Each complex type has the same representation and alignment requirements as
     * an array type containing exactly two elements of the corresponding real type;
     * the first element is equal to the real part, and the second element to the
     * imaginary part, of the complex number.
     */
    typedef union {
        float complex z;
        float parts[2];
    } float_complex;

    typedef union {
        double complex z;
        double parts[2];
    } double_complex;

    typedef union {
        long double complex z;
        long double parts[2];
    } long_double_complex;

    #define REAL_PART(z)    ((z).parts[0])
    #define IMAG_PART(z)    ((z).parts[1])

#endif /* __LIBMCS_WANT_COMPLEX */

#endif /* !LIBMCS_TOOLS_H */

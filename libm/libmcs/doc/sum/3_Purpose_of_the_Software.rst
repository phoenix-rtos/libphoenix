Purpose of the Software
=======================

The software in this project is intended to be included in other software projects. It is designed
and developed to be integrated in other software projects up to criticality B.

The library contains the following procedures which can be included with ``math.h`` (these lists only contain the name of the ``double`` procedure, you can expect all procedures (except for the Bessel functions) to have versions for ``float`` suffixed with ``f`` and ``long double`` suffixed with ``l`` (note however that ``long double`` only exists when it has the same size as ``double``)):

.. table:: List of all procedures in math.h
   :name: List of math procedures

   +--------------------+-----------------------------------------------------------+
   | Procedure          | Description                                               |
   +====================+===========================================================+
   | ``fpclassify``     | Procedure returning the classification of the argument    |
   +--------------------+-----------------------------------------------------------+
   | ``isfinite``       | Procedure returning whether the value of the argument is  |
   |                    | finite or not (not :math:`±Inf` and not :math:`NaN`)      |
   +--------------------+-----------------------------------------------------------+
   | ``isinf``          | Procedure returning whether the value of the argument is  |
   |                    | positive or negative Infinity or not                      |
   +--------------------+-----------------------------------------------------------+
   | ``isnan``          | Procedure returning whether the argument is a             |
   |                    | :math:`NaN` floating-point value or not                   |
   +--------------------+-----------------------------------------------------------+
   | ``isnormal``       | Procedure returning whether the argument is a normal      |
   |                    | floating-point value or not                               |
   +--------------------+-----------------------------------------------------------+
   | ``signbit``        | Procedure returning whether the argument is negative or   |
   |                    | not                                                       |
   +--------------------+-----------------------------------------------------------+
   | ``acos``           | Procedure returning the trigonometric arccosine           |
   +--------------------+-----------------------------------------------------------+
   | ``asin``           | Procedure returning the trigonometric arcsine             |
   +--------------------+-----------------------------------------------------------+
   | ``atan``           | Procedure returning the trigonometric arctangent          |
   +--------------------+-----------------------------------------------------------+
   | ``atan2``          | Procedure returning the trigonometric arctangent of       |
   |                    | :math:`\frac{y}{x}`                                       |
   +--------------------+-----------------------------------------------------------+
   | ``cos``            | Procedure returning the trigonometric cosine              |
   +--------------------+-----------------------------------------------------------+
   | ``sin``            | Procedure returning the trigonometric sine                |
   +--------------------+-----------------------------------------------------------+
   | ``tan``            | Procedure returning the trigonometric tangent             |
   +--------------------+-----------------------------------------------------------+
   | ``acosh``          | Procedure returning the hyperbolic arccosine              |
   +--------------------+-----------------------------------------------------------+
   | ``asinh``          | Procedure returning the hyperbolic arcsine                |
   +--------------------+-----------------------------------------------------------+
   | ``atanh``          | Procedure returning the hyperbolic arctangent             |
   +--------------------+-----------------------------------------------------------+
   | ``cosh``           | Procedure returning the hyperbolic cosine                 |
   +--------------------+-----------------------------------------------------------+
   | ``sinh``           | Procedure returning the hyperbolic sine                   |
   +--------------------+-----------------------------------------------------------+
   | ``tanh``           | Procedure returning the hyperbolic tangent                |
   +--------------------+-----------------------------------------------------------+
   | ``exp``            | Procedure returning the base :math:`e` exponential of     |
   |                    | :math:`x`                                                 |
   +--------------------+-----------------------------------------------------------+
   | ``exp2``           | Procedure returning the base :math:`2` exponential of     |
   |                    | :math:`x`                                                 |
   +--------------------+-----------------------------------------------------------+
   | ``expm1``          | Procedure returning the base :math:`e` exponential of     |
   |                    | :math:`x` minus :math:`1`                                 |
   +--------------------+-----------------------------------------------------------+
   | ``frexp``          | Procedure breaking :math:`x` into a normalized fraction   |
   |                    | and an integral power of :math:`2`                        |
   +--------------------+-----------------------------------------------------------+
   | ``ilogb``          | Procedure returning the binary exponent of :math:`x` as   |
   |                    | integer                                                   |
   +--------------------+-----------------------------------------------------------+
   | ``ldexp``          | Procedure returning :math:`x` multiplied by an integral   |
   |                    | power of :math:`2`                                        |
   +--------------------+-----------------------------------------------------------+
   | ``log``            | Procedure returning the natural logarithm                 |
   +--------------------+-----------------------------------------------------------+
   | ``log10``          | Procedure returning the base :math:`10` logarithm         |
   +--------------------+-----------------------------------------------------------+
   | ``log1p``          | Procedure returning the natural logarithm of              |
   |                    | :math:`x + 1`                                             |
   +--------------------+-----------------------------------------------------------+
   | ``log2``           | Procedure returning the base :math:`2` logarithm          |
   +--------------------+-----------------------------------------------------------+
   | ``logb``           | Procedure returning the binary exponent of :math:`x`      |
   +--------------------+-----------------------------------------------------------+
   | ``modf``           | Procedure breaking :math:`x` in its integral and          |
   |                    | fractional part                                           |
   +--------------------+-----------------------------------------------------------+
   | ``scalbn``         | Procedure returning :math:`x` multiplied by an integral   |
   |                    | power of :math:`2`                                        |
   +--------------------+-----------------------------------------------------------+
   | ``scalbln``        | Procedure returning :math:`x` multiplied by an integral   |
   |                    | power of :math:`2`                                        |
   +--------------------+-----------------------------------------------------------+
   | ``cbrt``           | Procedure returning the cubic root                        |
   +--------------------+-----------------------------------------------------------+
   | ``fabs``           | Procedure returning the absolute value                    |
   +--------------------+-----------------------------------------------------------+
   | ``hypot``          | Procedure returning the square root of :math:`x^2+y^2`    |
   +--------------------+-----------------------------------------------------------+
   | ``pow``            | Procedure returning :math:`x` raised to the power of      |
   |                    | :math:`y`                                                 |
   +--------------------+-----------------------------------------------------------+
   | ``sqrt``           | Procedure returning the square root                       |
   +--------------------+-----------------------------------------------------------+
   | ``erf``            | Procedure returning the error function                    |
   +--------------------+-----------------------------------------------------------+
   | ``erfc``           | Procedure returning the complementary error function      |
   |                    | (``erfc`` = :math:`1 -` ``erf``)                          |
   +--------------------+-----------------------------------------------------------+
   | ``lgamma``         | Procedure returning the natural logarithm of the absolute |
   |                    | value of gamma of :math:`x`                               |
   +--------------------+-----------------------------------------------------------+
   | ``tgamma``         | Procedure returning gamma function of :math:`x`           |
   +--------------------+-----------------------------------------------------------+
   | ``ceil``           | Procedure for rounding upwards to the nearest integer     |
   +--------------------+-----------------------------------------------------------+
   | ``floor``          | Procedure for rounding downwards to the nearest integer   |
   +--------------------+-----------------------------------------------------------+
   | ``nearbyint``      | Procedure for rounding to the nearest integer using the   |
   |                    | current rounding direction                                |
   +--------------------+-----------------------------------------------------------+
   | ``rint``           | Procedure for rounding to the nearest integer using the   |
   |                    | current rounding direction (raises inexact)               |
   +--------------------+-----------------------------------------------------------+
   | ``lrint``          | Procedure for rounding to the nearest integer using the   |
   |                    | current rounding direction                                |
   +--------------------+-----------------------------------------------------------+
   | ``llrint``         | Procedure for rounding to the nearest integer using the   |
   |                    | current rounding direction                                |
   +--------------------+-----------------------------------------------------------+
   | ``round``          | Procedure for rounding to the nearest integer (Halfway    |
   |                    | values rounded away from :math:`0`)                       |
   +--------------------+-----------------------------------------------------------+
   | ``lround``         | Procedure for rounding to the nearest integer (Halfway    |
   |                    | values rounded away from :math:`0`)                       |
   +--------------------+-----------------------------------------------------------+
   | ``llround``        | Procedure for rounding to the nearest integer (Halfway    |
   |                    | values rounded away from :math:`0`)                       |
   +--------------------+-----------------------------------------------------------+
   | ``trunc``          | Procedure for rounding towards :math:`0` to the nearest   |
   |                    | integer                                                   |
   +--------------------+-----------------------------------------------------------+
   | ``fmod``           | Procedure returning the floating-point remainder of       |
   |                    | :math:`\frac{y}{x}` (rounded towards zero)                |
   +--------------------+-----------------------------------------------------------+
   | ``remainder``      | Procedure returning the floating-point remainder of       |
   |                    | :math:`\frac{y}{x}` (rounded to nearest integral value)   |
   +--------------------+-----------------------------------------------------------+
   | ``remquo``         | Procedure returning the same value as ``remainder`` and   |
   |                    | puts the quotient in :math:`*quo`                         |
   +--------------------+-----------------------------------------------------------+
   | ``copysign``       | Procedure returning a floating-point number with the      |
   |                    | magnitude of :math:`x` and the sign of :math:`y`          |
   +--------------------+-----------------------------------------------------------+
   | ``nan``            | Procedure returning a :math:`NaN`                         |
   +--------------------+-----------------------------------------------------------+
   | ``nextafter``      | Procedure returning the next floating-point value after   |
   |                    | :math:`x` in direction of :math:`y`                       |
   +--------------------+-----------------------------------------------------------+
   | ``nexttoward``     | Procedure returning the next floating-point value after   |
   |                    | :math:`x` in direction of :math:`y`                       |
   +--------------------+-----------------------------------------------------------+
   | ``fdim``           | Procedure returning the positive difference between the   |
   |                    | arguments                                                 |
   +--------------------+-----------------------------------------------------------+
   | ``fmax``           | Procedure returning the larger of two values              |
   +--------------------+-----------------------------------------------------------+
   | ``fmin``           | Procedure returning the smaller of two values             |
   +--------------------+-----------------------------------------------------------+
   | ``fma``            | Procedure returning the result of :math:`x \cdot y + z`   |
   +--------------------+-----------------------------------------------------------+
   | ``isgreater``      | Procedure returning whether :math:`x` is greater than     |
   |                    | :math:`y`                                                 |
   +--------------------+-----------------------------------------------------------+
   | ``isgreaterequal`` | Procedure returning whether :math:`x` is greater than or  |
   |                    | equal to :math:`y`                                        |
   +--------------------+-----------------------------------------------------------+
   | ``isless``         | Procedure returning whether :math:`x` is less than        |
   |                    | :math:`y`                                                 |
   +--------------------+-----------------------------------------------------------+
   | ``islessequal``    | Procedure returning whether :math:`x` is less than or     |
   |                    | equal to :math:`y`                                        |
   +--------------------+-----------------------------------------------------------+
   | ``islessgreater``  | Procedure returning whether :math:`x` is less or greater  |
   |                    | than :math:`y`                                            |
   +--------------------+-----------------------------------------------------------+
   | ``isunordered``    | Procedure returning whether the arguments are unordered   |
   |                    | (aka at least one is :math:`NaN`)                         |
   +--------------------+-----------------------------------------------------------+
   | ``j0``             | Procedure returning the Bessel value of :math:`x` of the  |
   |                    | first kind of order :math:`0`                             |
   +--------------------+-----------------------------------------------------------+
   | ``j1``             | Procedure returning the Bessel value of :math:`x` of the  |
   |                    | first kind of order :math:`1`                             |
   +--------------------+-----------------------------------------------------------+
   | ``jn``             | Procedure returning the Bessel value of :math:`x` of the  |
   |                    | first kind of order :math:`n`                             |
   +--------------------+-----------------------------------------------------------+
   | ``y0``             | Procedure returning the Bessel value of :math:`x` of the  |
   |                    | second kind of order :math:`0`                            |
   +--------------------+-----------------------------------------------------------+
   | ``y1``             | Procedure returning the Bessel value of :math:`x` of the  |
   |                    | second kind of order :math:`1`                            |
   +--------------------+-----------------------------------------------------------+
   | ``yn``             | Procedure returning the Bessel value of :math:`x` of the  |
   |                    | second kind of order :math:`n`                            |
   +--------------------+-----------------------------------------------------------+

Additionally ``complex.h`` includes the following procedures:

.. table:: List of all procedures in complex.h
   :name: List of complex procedures

   +--------------------+-----------------------------------------------------------+
   | Procedure          | Description                                               |
   +====================+===========================================================+
   | ``cacos``          | Procedure returning the complex trigonometric arccosine   |
   +--------------------+-----------------------------------------------------------+
   | ``casin``          | Procedure returning the complex trigonometric arcsine     |
   +--------------------+-----------------------------------------------------------+
   | ``catan``          | Procedure returning the complex trigonometric arctangent  |
   +--------------------+-----------------------------------------------------------+
   | ``ccos``           | Procedure returning the complex trigonometric cosine      |
   +--------------------+-----------------------------------------------------------+
   | ``csin``           | Procedure returning the complex trigonometric sine        |
   +--------------------+-----------------------------------------------------------+
   | ``ctan``           | Procedure returning the complex trigonometric tangent     |
   +--------------------+-----------------------------------------------------------+
   | ``cacosh``         | Procedure returning the complex hyperbolic arccosine      |
   +--------------------+-----------------------------------------------------------+
   | ``casinh``         | Procedure returning the complex hyperbolic arcsine        |
   +--------------------+-----------------------------------------------------------+
   | ``catanh``         | Procedure returning the complex hyperbolic arctangent     |
   +--------------------+-----------------------------------------------------------+
   | ``ccosh``          | Procedure returning the complex hyperbolic cosine         |
   +--------------------+-----------------------------------------------------------+
   | ``csinh``          | Procedure returning the complex hyperbolic sine           |
   +--------------------+-----------------------------------------------------------+
   | ``ctanh``          | Procedure returning the complex hyperbolic tangent        |
   +--------------------+-----------------------------------------------------------+
   | ``cexp``           | Procedure returning the complex base :math:`e`            |
   |                    | exponential of :math:`z`                                  |
   +--------------------+-----------------------------------------------------------+
   | ``clog``           | Procedure returning the complex natural logarithm         |
   +--------------------+-----------------------------------------------------------+
   | ``cabs``           | Procedure returning the complex absolute value            |
   +--------------------+-----------------------------------------------------------+
   | ``cpow``           | Procedure returning the complex value :math:`x` raised to |
   |                    | the power of :math:`y`                                    |
   +--------------------+-----------------------------------------------------------+
   | ``csqrt``          | Procedure returning the complex square root               |
   +--------------------+-----------------------------------------------------------+
   | ``carg``           | Procedure returning the value of :math:`z` in the         |
   |                    | interval [:math:`-\pi`, :math:`+\pi`]                     |
   +--------------------+-----------------------------------------------------------+
   | ``cimag``          | Procedure returning the imaginary part of the value of    |
   |                    | :math:`z`                                                 |
   +--------------------+-----------------------------------------------------------+
   | ``CMPLX``          | Procedure returning the complex value with real part      |
   |                    | :math:`x` and imaginary part :math:`y`                    |
   +--------------------+-----------------------------------------------------------+
   | ``conj``           | Procedure returning the complex conjugate value of        |
   |                    | :math:`z`                                                 |
   +--------------------+-----------------------------------------------------------+
   | ``cproj``          | Procedure returning the value of the projection onto the  |
   |                    | Riemann sphere of :math:`z`                               |
   +--------------------+-----------------------------------------------------------+
   | ``creal``          | Procedure returning the real part of the value of         |
   |                    | :math:`z`                                                 |
   +--------------------+-----------------------------------------------------------+

Furthermore the library provides a number of constants, as part of ``math.h``:

.. table:: List of all constant defines in math.h
   :name: List of math constants

   ===================== ===============================================
   Name                  Description
   ===================== ===============================================
   ``M_E``               Value of :math:`e`
   ``M_LOG2E``           Value of :math:`log_{2} e`
   ``M_LOG10E``          Value of :math:`log_{10} e`
   ``M_LN2``             Value of :math:`log_e 2`
   ``M_LN10``            Value of :math:`log_e 10`
   ``M_PI``              Value of :math:`\pi`
   ``M_PI_2``            Value of :math:`\frac{\pi}{2}`
   ``M_PI_4``            Value of :math:`\frac{\pi}{4}`
   ``M_1_PI``            Value of :math:`\frac{1}{\pi}`
   ``M_2_PI``            Value of :math:`\frac{2}{\pi}`
   ``M_2_SQRTPI``        Value of :math:`\frac{2}{\sqrt{\pi}}`
   ``M_SQRT2``           Value of :math:`\sqrt{2}`
   ``M_SQRT1_2``         Value of :math:`\sqrt{\frac{1}{2}}`
   ``HUGE_VAL``          Value of :math:`+Inf` (double)
   ``HUGE_VALF``         Value of :math:`+Inf` (float)
   ``HUGE_VALL``         Value of :math:`+Inf` (long double)
   ``INFINITY``          Value of :math:`+Inf`
   ``NAN``               Value of :math:`NaN`
   ``MAXFLOAT``          Synonym of ``FLT_MAX``
   ``FP_INFINITE``       :math:`1`
   ``FP_NAN``            :math:`0`
   ``FP_NORMAL``         :math:`4`
   ``FP_SUBNORMAL``      :math:`3`
   ``FP_ZERO``           :math:`2`
   ``FP_ILOGB0``         Value to return for :ref:`ilogb` (:math:`0`)
   ``FP_ILOGBNAN``       Value to return for :ref:`ilogb` (:math:`NaN`)
   ``MATH_ERRNO``        :math:`1`
   ``MATH_ERREXCEPT``    :math:`2`
   ``math_errhandling``  ``MATH_ERREXCEPT``
   ===================== ===============================================

*Remark:* Both ``INFINITY`` and ``NAN`` expand to floats or doubles depending on the context.

``math.h`` further declares the types ``double_t`` and ``float_t`` as ``double`` and ``float``.

``complex.h`` declares the types ``complex``, ``imaginary``, ``_Complex_I``, ``_Imaginary_I`` and ``I`` as ``_Complex``, ``_Imaginary``, ``const float _Complex``, ``const float _Imaginary``, and ``_Complex_I`` respectively. (Note that your toolchain (such as GCC) may not properly implement imaginary types, as such it is likely that all definitions are complex types.)
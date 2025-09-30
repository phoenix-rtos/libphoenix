Reference Manual
================

This chapter provides the detailed documentation of all the procedures (C functions and macros) listed in chapter :ref:`Purpose of the Software`.

Each procedure is described in a separate subsection with the style similar to the documentation of the procedures in the ISO C and POSIX standards. Each subsection starts with a synopsis specifying the
procedure’s signature, followed by a short description of the evaluated mathematical function and its return value. Further the raised exceptions are noted.

Classification Macros
"""""""""""""""""""""

fpclassify
~~~~~~~~~~

.. c:autodoc:: mathd/internal/fpclassifyd.c

isfinite
~~~~~~~~

.. c:autodoc:: common/isfinite.c

isinf
~~~~~

.. c:autodoc:: common/isinf.c

isnan
~~~~~

.. c:autodoc:: common/isnan.c

isnormal
~~~~~~~~

.. c:autodoc:: common/isnormal.c

signbit
~~~~~~~

.. c:autodoc:: mathd/internal/signbitd.c

Trigonometric Functions
"""""""""""""""""""""""

acos
~~~~

.. c:autodoc:: mathd/acosd.c

asin
~~~~

.. c:autodoc:: mathd/asind.c

atan
~~~~

.. c:autodoc:: mathd/atand.c

atan2
~~~~~

.. c:autodoc:: mathd/atan2d.c

cos
~~~

.. c:autodoc:: mathd/cosd.c

sin
~~~

.. c:autodoc:: mathd/sind.c

tan
~~~

.. c:autodoc:: mathd/tand.c

Hyperbolic Functions
""""""""""""""""""""

acosh
~~~~~

.. c:autodoc:: mathd/acoshd.c

asinh
~~~~~

.. c:autodoc:: mathd/asinhd.c

atanh
~~~~~

.. c:autodoc:: mathd/atanhd.c

cosh
~~~~

.. c:autodoc:: mathd/coshd.c

sinh
~~~~

.. c:autodoc:: mathd/sinhd.c

tanh
~~~~

.. c:autodoc:: mathd/tanhd.c

Exponential and Logarithmic Functions
"""""""""""""""""""""""""""""""""""""

exp
~~~

.. c:autodoc:: mathd/expd.c

exp2
~~~~

.. c:autodoc:: mathd/exp2d.c

expm1
~~~~~

.. c:autodoc:: mathd/expm1d.c

frexp
~~~~~

.. c:autodoc:: mathd/frexpd.c

ilogb
~~~~~

.. c:autodoc:: mathd/ilogbd.c

ldexp
~~~~~

.. c:autodoc:: mathd/ldexpd.c

log
~~~

.. c:autodoc:: mathd/logd.c

log10
~~~~~

.. c:autodoc:: mathd/log10d.c

log1p
~~~~~

.. c:autodoc:: mathd/log1pd.c

log2
~~~~

.. c:autodoc:: mathd/log2d.c

logb
~~~~

.. c:autodoc:: mathd/logbd.c

modf
~~~~

.. c:autodoc:: mathd/modfd.c

scalbn
~~~~~~

.. c:autodoc:: mathd/scalbnd.c

scalbln
~~~~~~~

.. c:autodoc:: mathd/scalblnd.c

Power and Absolute-value Functions
""""""""""""""""""""""""""""""""""

cbrt
~~~~

.. c:autodoc:: mathd/cbrtd.c

fabs
~~~~

.. c:autodoc:: mathd/fabsd.c

hypot
~~~~~

.. c:autodoc:: mathd/hypotd.c

pow
~~~

.. c:autodoc:: mathd/powd.c

sqrt
~~~~

.. c:autodoc:: mathd/sqrtd.c

Error and Gamma Functions
"""""""""""""""""""""""""

erf
~~~

.. c:autodoc:: mathd/erfd.c

erfc
~~~~

.. c:autodoc:: mathd/erfcd.c

lgamma
~~~~~~

.. c:autodoc:: mathd/lgammad.c

tgamma
~~~~~~

.. c:autodoc:: mathd/tgammad.c

signgam
~~~~~~~

.. c:autodoc:: common/signgam.c

Nearest Integer Functions
"""""""""""""""""""""""""

ceil
~~~~

.. c:autodoc:: mathd/ceild.c

floor
~~~~~

.. c:autodoc:: mathd/floord.c

nearbyint
~~~~~~~~~

.. c:autodoc:: mathd/nearbyintd.c

rint
~~~~

.. c:autodoc:: mathd/rintd.c

lrint
~~~~~

.. c:autodoc:: mathd/lrintd.c

llrint
~~~~~~

.. c:autodoc:: mathd/llrintd.c

round
~~~~~

.. c:autodoc:: mathd/roundd.c

lround
~~~~~~

.. c:autodoc:: mathd/lroundd.c

llround
~~~~~~~

.. c:autodoc:: mathd/llroundd.c

trunc
~~~~~

.. c:autodoc:: mathd/truncd.c

Remainder Functions
"""""""""""""""""""

fmod
~~~~

.. c:autodoc:: mathd/fmodd.c

remainder
~~~~~~~~~

.. c:autodoc:: mathd/remainderd.c

remquo
~~~~~~

.. c:autodoc:: mathd/remquod.c

Manipulation Functions
""""""""""""""""""""""

copysign
~~~~~~~~

.. c:autodoc:: mathd/copysignd.c

nan
~~~

.. c:autodoc:: mathd/nand.c

nextafter
~~~~~~~~~

.. c:autodoc:: mathd/nextafterd.c

nexttoward
~~~~~~~~~~

.. c:autodoc:: mathd/nexttowardd.c

Maximum, Minimum and Positive Difference Functions
""""""""""""""""""""""""""""""""""""""""""""""""""

fdim
~~~~

.. c:autodoc:: mathd/fdimd.c

fmax
~~~~

.. c:autodoc:: mathd/fmaxd.c

fmin
~~~~

.. c:autodoc:: mathd/fmind.c

Floating Multiply-Add
"""""""""""""""""""""

fma
~~~

.. c:autodoc:: mathd/fmad.c

Comparison Macros
"""""""""""""""""

isgreater
~~~~~~~~~

.. c:autodoc:: common/isgreater.c

isgreaterequal
~~~~~~~~~~~~~~

.. c:autodoc:: common/isgreaterequal.c

isless
~~~~~~

.. c:autodoc:: common/isless.c

islessequal
~~~~~~~~~~~

.. c:autodoc:: common/islessequal.c

islessgreater
~~~~~~~~~~~~~

.. c:autodoc:: common/islessgreater.c

isunordered
~~~~~~~~~~~

.. c:autodoc:: common/isunordered.c

Bessel Functions (POSIX)
""""""""""""""""""""""""

j0
~~

.. c:autodoc:: mathd/j0d.c

j1
~~

.. c:autodoc:: mathd/j1d.c

jn
~~

.. c:autodoc:: mathd/jnd.c

y0
~~

.. c:autodoc:: mathd/y0d.c

y1
~~

.. c:autodoc:: mathd/y1d.c

yn
~~

.. c:autodoc:: mathd/ynd.c

Complex Trigonometric Functions
"""""""""""""""""""""""""""""""

cacos
~~~~~

.. c:autodoc:: complexd/cacosd.c

casin
~~~~~

.. c:autodoc:: complexd/casind.c

catan
~~~~~

.. c:autodoc:: complexd/catand.c

ccos
~~~~

.. c:autodoc:: complexd/ccosd.c

csin
~~~~

.. c:autodoc:: complexd/csind.c

ctan
~~~~

.. c:autodoc:: complexd/ctand.c

Complex Hyperbolic Functions
""""""""""""""""""""""""""""

cacosh
~~~~~~

.. c:autodoc:: complexd/cacoshd.c

casinh
~~~~~~

.. c:autodoc:: complexd/casinhd.c

catanh
~~~~~~

.. c:autodoc:: complexd/catanhd.c

ccosh
~~~~~

.. c:autodoc:: complexd/ccoshd.c

csinh
~~~~~

.. c:autodoc:: complexd/csinhd.c

ctanh
~~~~~

.. c:autodoc:: complexd/ctanhd.c

Complex Exponential and Logarithmic Functions
"""""""""""""""""""""""""""""""""""""""""""""

cexp
~~~~

.. c:autodoc:: complexd/cexpd.c

clog
~~~~

.. c:autodoc:: complexd/clogd.c

Complex Power and Absolute-value Functions
""""""""""""""""""""""""""""""""""""""""""

cabs
~~~~

.. c:autodoc:: complexd/cabsd.c

cpow
~~~~

.. c:autodoc:: complexd/cpowd.c

csqrt
~~~~~

.. c:autodoc:: complexd/csqrtd.c

Complex Manipulation Functions
""""""""""""""""""""""""""""""

carg
~~~~

.. c:autodoc:: complexd/cargd.c

cimag
~~~~~

.. c:autodoc:: complexd/cimagd.c

CMPLX
~~~~~

.. c:autodoc:: common/cmplx.c

conj
~~~~

.. c:autodoc:: complexd/conjd.c

cproj
~~~~~

.. c:autodoc:: complexd/cprojd.c

creal
~~~~~

.. c:autodoc:: complexd/creald.c

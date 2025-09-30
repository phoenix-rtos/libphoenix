General Behaviour
=================

All procedures can handle all input values of its respective types, including special values such as ``NaNs`` or infinities. Pointers used to output additional return values are under the user’s control, using misplaced or ``NULL`` pointers may cause unwanted behaviour or even a crash of the system. The ``NULL`` pointers are handled appropriately by the library, but no action can be taken against using misplaced pointers.

NaN Values
~~~~~~~~~~

As can be seen in :ref:`Conventions` there exist different types of ``NaNs``.

What are they? ``NaN`` is the result of a procedure that does not generate a valid and representable result. This happens for example with :math:`\frac{0.0}{0.0}`, with ``sqrt(-1.0)`` :math:`(= \sqrt{-1})`, or with ``Inf - Inf``, in the first and third cases because the behavior is undefined, in the second because complex/imaginary numbers cannot be represented in the data type of the procedure. ``NaN`` can also be used as an input argument for procedures, in most cases this results in a ``NaN`` return value.

What's the difference between ``NaN``, ``qNaN`` and ``sNaN``? Both ``qNaNs`` and ``sNaNs`` are a subgroup of all ``NaNs``, they differ in a specific bit that is either set (``qNaN``) or not set (``sNaN``). All procedures will only return ``qNaNs`` and never ``sNaNs``. The difference is on the other side: when an ``sNaN`` is put as an argument to a procedure, the :ref:`FPU <ABBR>` shall signal an ``invalid operation`` exception, while a ``qNaN`` is quietly accepted, hence the names. As ``sNaNs`` are never produced by the :ref:`FPU <ABBR>` they can be regarded as a testing feature.

In :ref:`Conventions` we also differentiate between ``-NaN`` (``NaN`` where the signbit is set) and ``+NaN`` (``NaN`` where the signbit is not set). This separation does not exist anywhere within the :ref:`IEEE-754 <ABBR>` or C18 standards, ``NaNs`` are simply ``NaNs`` regardless of their sign. We however need this differentiation for the procedures :ref:`signbit` and :ref:`copysign`, as both only check the sign of a value and ignore the rest.

The ``nan (const char *payload)``, ``nanf (const char *payload)``, and ``nanl (const char *payload)`` functions of the library are not fully ISO C compliant as they return a fix defined ``NaN`` regardless of the function parameter. The rationale for this is to keep the LibmCS standalone, without any dependency to other standard C library functions. A workaround is proposed in the section describing these functions.

.. _GeneralBehaviourSubnormalValues:

Subnormal Values
~~~~~~~~~~~~~~~~

The library regards subnormals the same as any other value, meaning when a procedure is called with a subnormal input argument, the procedures will do their work just as well as for any non subnormal value. This however only works as expected, if the :ref:`FPU <ABBR>` in use supports subnormal values.

In case the :ref:`FPU <ABBR>` does not fully support subnormals the user should enable the preprocessor define ``LIBMCS_FPU_DAZ`` while running the library's configuration when prompted to make the decision. This forces every input value to first be checked by the :ref:`FPU <ABBR>` for being a subnormal, and if it is, act according to the implementation of the :ref:`FPU <ABBR>`. This behavior will apply to most of the procedures contained in the library. Exceptions to this behavior being (i.e., the following procedures do correctly handle subnormals even if the FPU has no subnormal support):

* ``fabs`` and ``fabsf``
* ``copysign`` and ``copysignf``
* ``fpclassify``
* ``isfinite``
* ``isinf``
* ``isnan``
* ``isnormal``
* ``signbit``
* ``isunordered``
* ``isgreater`` (only on GRFPUs before GRFPU5)
* ``isgreaterequal`` (only on GRFPUs before GRFPU5)
* ``isless`` (only on GRFPUs before GRFPU5)
* ``islessequal`` (only on GRFPUs before GRFPU5)
* ``islessgreater`` (only on GRFPUs before GRFPU5)

In addition to this the following procedures implement a non-standard behavior to ensure that they don't get stuck at subnormals when using them on FPUs without subnormal support (i.e., they jump over the subnormal range):

* ``nextafter`` and ``nextafterf``
* ``nexttoward`` and ``nexttowardf``

One example of such a non-supporting :ref:`FPU <ABBR>` is the :ref:`GRFPU <ABBR>` from CAES/Gaisler. The :ref:`GRFPU <ABBR>` causes a trap if subnormal numbers occur and the :ref:`GRFPU <ABBR>` is not configured to use non-standard mode [#]_. If the non-standard mode is enabled however, the :ref:`FPU <ABBR>` behaves in a :ref:`DAZ <ABBR>` and :ref:`FTZ <ABBR>` way. This means when ``LIBMCS_FPU_DAZ`` is defined, every call to the library's procedures with a subnormal input will either cause a trap (in standard mode) or behave as if the input was zero (in non-standard mode).

If the user's :ref:`FPU <ABBR>` behaves as the :ref:`GRFPU <ABBR>` we suggest using the standard mode during production and switching to non-standard mode after sufficient testing (or if the user decides that subnormals are part of normal behaviour). This has the benefit that errors can be found more easily during production (as subnormal numbers in most cases should be an error in the source code), and not causing a trap on the final product in the odd case that a subnormal still appears.

The following table shows some FPUs, their support regarding subnormal floating-point numbers and the particularities of the supported :ref:`DAZ <ABBR>` and :ref:`FTZ <ABBR>` mode:

+--------------------------+-----------------------+-------------------------------------------------------------+
| FPU                      | Subnormal support     | Operations not affected by FTZ/DAZ mode                     |
+==========================+=======================+=============================================================+
| MEIKO FPU                |                Yes    | Not applicable                                              |
+--------------------------+-----------------------+-------------------------------------------------------------+
| GRFPU (until v4)         |                 No    | compare, move, negate, and absolute value                   |
+--------------------------+-----------------------+-------------------------------------------------------------+
| GRFPU-Lite               |                 No    | compare, move, negate, and absolute value                   |
+--------------------------+-----------------------+-------------------------------------------------------------+
| GRFPU5                   |                Yes    | Not applicable                                              |
+--------------------------+-----------------------+-------------------------------------------------------------+
| Gaisler NanoFPU          |                 No    | compare, move, negate, and absolute value                   |
+--------------------------+-----------------------+-------------------------------------------------------------+
| GRFPUnv                  |                Yes    | Not applicable                                              |
+--------------------------+-----------------------+-------------------------------------------------------------+
| ARM FPUs (NEON and VFPU) |            Selectable | V{Q}ABS, V{Q}NEG, VMOV, VMVN, VDUP, VLDR VSTR, VLDM, VSTM   |
+--------------------------+-----------------------+-------------------------------------------------------------+
| SiFive FPU               |                Yes    | Not applicable                                              |
+--------------------------+-----------------------+-------------------------------------------------------------+
| x86                      |            Selectable | x87 floating point instructions                             |
+--------------------------+-----------------------+-------------------------------------------------------------+

This table is by no means exhaustive and shall only be used for preliminary iformation purposes. The user of the library shall the concretely check the specification of the :ref:`FPU <ABBR>` as there are may additional particularities to be taken into account. For example we have the particular case of ARM FPUs:

* NEON and VFPv3 flush-to-zero preserves the sign bit. VFPv2 flush-to-zero flushes to +0.
* NEON always uses flush-to-zero mode.

.. [#] See `Handling denormalized numbers with the GRFPU <http://www.gaisler.com/doc/antn/GRLIB-AN-0007.pdf>`_ Section 4.1 for more information on how to enable non-standard mode on the :ref:`GRFPU <ABBR>`.

Fused Multiply-Add
~~~~~~~~~~~~~~~~~~

The IEEE-754 floating-point arithmetic standard requires since its 2008 version that compliant systems must support the :ref:`FMA <ABBR>` operation. This operation is also required by the ISO C standard. Older FPUs and the SPARC V8 instruction set however do not support nor require this operation. 

Because of this reason the LibmCS includes the ``fma`` and ``fmaf`` procedures but only with a non-standard conform naive implementation carrying out a multiplication and addition in sequence with two roundings instead of the single rounding step required by the :ref:`FMA <ABBR>` operation.

On systems supporting the :ref:`FMA <ABBR>` operation users can use compiler built-ins to explicitely call ``fma`` if needed.

Exceptions
~~~~~~~~~~

Exceptions raised by the procedures are listed in the :ref:`Reference Manual`. These are in accordance with the :ref:`ISO <ABBR>` C, :ref:`IEEE-754 <ABBR>` and :ref:`POSIX <ABBR>` standards. The :ref:`Reference Manual` will only list the exceptions ``invalid operation``, ``divide by zero`` and ``overflow`` while not stating ``underflow`` and ``inexact``. These are the three exceptions deemed as important in *IEEE 754 Error Handling and Programming Languages* [NM]_.

The :ref:`POSIX <ABBR>` standard does not define errors/exceptions for complex procedures. The implementations however make use of multiple non-complex procedures which do raise exceptions. As such the :ref:`Reference Manual` does not list the exceptions thrown by complex procedures, it is assumed that the exceptions of their underlying procedures apply.

All procedures raise ``invalid operation`` exception when the input argument is ``sNaN`` unless stated otherwise in the :ref:`Reference Manual`.

.. [NM] *IEEE 754 Error Handling and Programming Languages* by Nick Maclaren.

Limitations of the Libm
~~~~~~~~~~~~~~~~~~~~~~~

Qualification Status
^^^^^^^^^^^^^^^^^^^^

This software release is qualified to :ref:`ECSS <ABBR>` category B, but only for the following configurations:

#. target GR-CPCI-AT697 ("Compact PCI LEON2-FT (AT697E) Development Board") with a compiler toolchain based on EDISOFT's :ref:`RTEMS <ABBR>` 4.8 (including :ref:`GCC <ABBR>` 4.2.1), and
#. target GR-CPCI-LEON4-N2X ("Quad-Core LEON4 Next Generation Microprocessor Evaluation Board") with a compiler toolchain based on OAR's :ref:`RTEMS <ABBR>` 4.11 (including :ref:`GCC <ABBR>` 4.9.3),
#. using DMON (2.0.11.13) to connect to the targets,
#. build as per build instructions in :ref:`Operations Environment` and :ref:`Operations Manual` as well as using the configuration and Makefile included in the release,
#. ``LIBMCS_DOUBLE_IS_32BITS`` is not defined,
#. the :ref:`FPU's <ABBR>` rounding direction is set to *round to nearest with tie to even*.

The general configuration status of the library can be found in the :ref:`SCF <ABBR>` and the specific qualification evidences (for the above configuration) are in the :ref:`SUITR <ABBR>`, :ref:`SValR <ABBR>`, and :ref:`SVR <ABBR>`. All life-cycle documents apart from the specific qualification evidences are part of the qualification kit provided to the user.

If this release is intended to be used in a different configuration then the one given above, then the qualification status needs to be reassessed in a :ref:`SRF <ABBR>` and a delta-qualification carried out following the :ref:`QG <ABBR>` OP-QG.00-ML.

Compliance
^^^^^^^^^^

This software is compliant to :ref:`ISO <ABBR>` C18 (ISO/IEC 9899:2018), :ref:`IEEE-754-2019 <ABBR>`, POSIX (IEEE Std 1003.1-2017), and MISRA C:2012.

The use of some library macros will issue justifiable MISRA C non-compliances. These MISRA C non-compliances have been justified within the qualification of the LibmCS but in the case of macros, these non-compliances will be visible while compiling the software that is using the LibmCS.

Compliance to :ref:`ISO <ABBR>` TS 18661-1 is not yet met.

Rounding Mode
^^^^^^^^^^^^^

The library is only qualified for the rounding mode *round to nearest, tie to even*.

Platform Architecture
^^^^^^^^^^^^^^^^^^^^^

In case the :ref:`FPU <ABBR>` of the target platform is not implementing all :ref:`IEEE-754 <ABBR>` features, the :ref:`FPU <ABBR>` has to be configured appropriately otherwise the library may trap on those missing features. One such example is the GRFPU as seen in :ref:`GeneralBehaviourSubnormalValues`.

Errno
^^^^^

The library does not set the ``errno`` variable to report errors nor does the library read it, ``errno`` is completely ignored.

Compiler
^^^^^^^^

In general the library is prepared to be used with a :ref:`GCC <ABBR>` toolchain. It might be necessary to change parts of the library when using a different toolchain.
The compiler used on the library shall be able to understand the ``asm`` keyword. For example :ref:`GCC <ABBR>` has the flag ``-std=gnu99`` to enable the :ref:`GNU <ABBR>` C language extensions which contain ``asm``.

Data Model
^^^^^^^^^^

The library is compatibility with the following data model processor and compiler tool-chains combinations:

* ILP32 or 4/4/4 (``int`` and ``long int`` are 32 bit)
* LP64 or 4/8/8 (``int`` is 32 bit and ``long int`` is 64-bit)

.. note::
   The ``long long int`` type shall always be 64 bits.  

Bessel functions
^^^^^^^^^^^^^^^^

The procedures :ref:`jn` and :ref:`yn` have only been qualified until an ``n`` value of 15. If you for some reason need to use them with higher values for ``n``, just change the value in the unit- and validation-tests. The other Bessel procedures (:ref:`j0`, :ref:`j1`, :ref:`y0`, and :ref:`y1`) however are fully qualified.

Complex procedures
^^^^^^^^^^^^^^^^^^

The complex procedures have a lower number of requirements than the procedures defined in ``math.h`` and are less detailed. This mirrors what happens in the :ref:`ISO <ABBR>` C and :ref:`POSIX <ABBR>` standards where far less information and requirements are defined for these procedures. As such their requirements fall short of what one could usually expect for Cat. B software. However the fact that these procedures are seldom if ever used in flight and critical software justifies their subpar requirements, while their existence is justified by the need to be able to integrate the library with other :ref:`COTS <ABBR>` software without restrictions (be aware that having them for integration purposes does not necessarily mean that they are used by other software components but that they expect them to exist).

fenv.h
^^^^^^

The mathematical library contains the header file ``fenv.h``. It declares - but does not implement - the functionalities listed in this section. The ``fenv.h`` header does not contain type definitions nor macros. This header file is only provided as the starting point for the user of the library to implement their own ``fenv.h`` as it is highly hardware dependent. Inclusion of the ``fenv.h`` file in an unmodified version produces an error at compile time, which should be removed by the user after implementing their own procedures. The library also provides an implementation file ``fenv.c`` which contains stub implementations for all procedures in ``fenv.h`` which simply return :math:`-1` which is a valid return value for all procedures and denotes that an error has accrued while calling said procedure. User software will have to check for negative output values anyway as this is how the procedures are defined to give notice of errors. As such this is a reliable way to tell the user that an implementation of the procedures still needs to be provided.

A custom ``fenv.h`` file needs to contain the type definition of ``fenv_t`` and ``fexcept_t`` provided by the user:

* ``fenv_t`` represents the entire state of the floating-point environment including its status flags and control modes.
* ``fexcept_t`` represents the state of all floating-point status flags collectively, including the active floating-point exceptions along with additional information the implementation associates with their status.

A custom ``fenv.h`` file needs to contain the following defines and constants:

* Constants which need to contain the position of their respective exception flag in the given hardware environment in the form of a bitmask (and as such be easily combineable), their type is ``int`` and should be used by the functions ``feclearexcept``, ``feraiseexcept``, ``fegetexceptflag`` and ``fesetexceptflag``:

   * ``FE_DIVBYZERO``
   * ``FE_INEXACT``
   * ``FE_INVALID``
   * ``FE_OVERFLOW``
   * ``FE_UNDERFLOW``
   * ``FE_ALL_EXCEPT``

* Constants which need to represent the given rounding mode in the given hardware environment, their type is ``int`` and should be used by the functions ``fegetround`` and ``fesetround``:

   * ``FE_DOWNWARD``
   * ``FE_TONEAREST``
   * ``FE_TOWARDSZERO``
   * ``FE_UPWARD``

* Constant pointer to access the environment in the given hardware environment, the type is ``fenv_t*`` and should be used as input for the functions ``fesetenv`` and ``fegetenv``:

   * ``FE_DFL_ENV``

These are the expected procedures of a proper ``fenv.h`` implementation:

* Floating-point Exceptions

   #. Clear floating-point exceptions

      * ``int feclearexcept(int excepts)``

   #. Raise floating-point exceptions

      * ``int feraiseexcept(int excepts)``

   #. Get floating-point exception flags

      * ``int fegetexceptflag(fexcept_t* flagp, int excepts)``

   #. Set floating-point exception flags

      * ``int fesetexceptflag(const fexcept_t* flagp, int excepts)``

* Rounding Direction

   #. Get rounding direction mode

      * ``int fegetround(void)``

   #. Set rounding direction mode

      * ``int fesetround(int rdir)``

* Entire Environment

   #. Get floating-point environment

      * ``int fegetenv(fenv_t* envp)``

   #. Set floating-point environment

      * ``int fesetenv(const fenv_t* envp)``

   #. Hold floating-point exceptions

      * ``int feholdexcept(fenv_t* envp)``

   #. Update floating-point environment

      * ``int feupdateenv(const fenv_t* envp)``

* Other

   #. Test for floating-point exceptions

      * ``int fetestexcept(int excepts)``

A typical use of the floating-point environment for critical systems will anyways be a direct writing to the floating point registers once during initialization (e.g., setting the rounding mode) thus, avoiding the interface provided by ``fenv.h``.

tgmath.h
^^^^^^^^

The mathematical library contains the header file ``tgmath.h`` which except for adding an error message upon compilation has not been changed from its ``Newlib`` state. ``tgmath.h`` procedures are not qualified.

Inclusion of the ``tgmath.h`` file produces an intentional error at compile time. Contrary to the ``fenv.h`` header file we descourage any use of the ``tgmath.h`` header and suggest users to not create their own. Type generic function calls should never be used in critical software.

Thread Safety and Reentrancy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The library is thread safe and reentrant. For software that is using the library it has to be noted that usage of the ``signgam`` variable is not thread safe when executing the ``lgamma`` procedures on
multiple threads at once, as each procedure call operates on the same variable. This is a limitation forced on the library by the :ref:`POSIX <ABBR>` standard that demands the availability of the ``signgam`` global variable.

Other Header Files
^^^^^^^^^^^^^^^^^^

The library does not contain any externally available header files other than those that should be part of a ``libm`` according to the ISO C and :ref:`POSIX <ABBR>` standards. It contains ``math.h``, ``complex.h``, ``fenv.h``, and ``tgmath.h``, although the limitations of the latter two have already been stated in this chapter. This means there will be no ``float.h`` or ``limits.h`` or any other header that does not belong into a ``libm``. All those headers need to be provided by the toolchain.

Assert Usage
~~~~~~~~~~~~

The library contains an ``assert`` in its source code. More specifically assertions are used in the :ref:`frexp`, :ref:`modf` and :ref:`remquo` procedures to ensure that the library does not cause a trap when the procedures are called with a NULL-pointer.

The implementation of the library assumes that the used toolchain contains an ``assert.h`` file with the standard implementation of ``assert``. That being the case, the ``assert`` can be disabled by defining ``NDEBUG`` either in source code or during compilation. The easiest way to do this would be using the ``make release`` command.

It is of course possible to add your own definition of ``assert`` using a custom ``assert.h`` file.

We suggest enabling the ``assert`` during production and disabling it after sufficient testing. This has the benefit that errors can be found more easily during production, and not having the assertion in the object code of the final product.

Modular Arithmetic Procedures
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As the modular arithmetic functions are often a source of confusion we want to use this section to show the differences between the modular arithmetic functions and give examples for each. This information is specific to the C programming language, and the C99 and :ref:`IEEE-754 <ABBR>` standards.

Modulus Operator: ``%``
^^^^^^^^^^^^^^^^^^^^^^^

Although this operator is not part of the library but of the C language itself, we include it here to complete the overview of modular arithmetic. The ``%`` operator is specific to integers and shall only be used with integer types. The result of using the ``%`` operator is the remainder of using the ``/`` operator on the same operands. Given two variables ``a`` and ``b`` the following must be true: :math:`a \% b = a - (\frac{a}{b} \cdot b)` with integral truncation of :math:`\frac{a}{b}` towards zero.

Examples:

+-----------+------------+-------------+--------------+
| 4 % 1 = 0 | 4 % -1 = 0 | -4 % 1 =  0 | -4 % -1 =  0 |
+-----------+------------+-------------+--------------+
| 4 % 2 = 0 | 4 % -2 = 0 | -4 % 2 =  0 | -4 % -2 =  0 |
+-----------+------------+-------------+--------------+
| 4 % 3 = 1 | 4 % -3 = 1 | -4 % 3 = -1 | -4 % -3 = -1 |
+-----------+------------+-------------+--------------+
| 4 % 4 = 0 | 4 % -4 = 0 | -4 % 4 =  0 | -4 % -4 =  0 |
+-----------+------------+-------------+--------------+
| 4 % 5 = 4 | 4 % -5 = 4 | -4 % 5 = -4 | -4 % -5 = -4 |
+-----------+------------+-------------+--------------+
| 4 % 6 = 4 | 4 % -6 = 4 | -4 % 6 = -4 | -4 % -6 = -4 |
+-----------+------------+-------------+--------------+

The fmod procedure
^^^^^^^^^^^^^^^^^^

The :ref:`fmod` procedure returns the remainder of :math:`x` divided by :math:`y`. Given two variables :math:`x` and :math:`y` the following must be true: :math:`fmod(x, y) = x - n \cdot y`, for an integer :math:`n` such that the result has the same sign as :math:`x` and magnitude less than the magnitude of :math:`y`. If :math:`y` is zero the result will be ``qNaN``.

Examples:

+--------------+--------+---+---------------+--------+---+---------------+--------+---+----------------+--------+
| Input (x, y) | Return |   | Input (x, y)  | Return |   | Input (x, y)  | Return |   | Input (x, y)   | Return |
+==============+========+===+===============+========+===+===============+========+===+================+========+
| (3.14, 0.2)  | 0.14   |   | (3.14, -0.2)  | 0.14   |   | (-3.14, 0.2)  | -0.14  |   | (-3.14, -0.2)  | -0.14  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 1.0)  | 0.14   |   | (3.14, -1.0)  | 0.14   |   | (-3.14, 1.0)  | -0.14  |   | (-3.14, -1.0)  | -0.14  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 1.5)  | 0.14   |   | (3.14, -1.5)  | 0.14   |   | (-3.14, 1.5)  | -0.14  |   | (-3.14, -1.5)  | -0.14  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 2.0)  | 1.14   |   | (3.14, -2.0)  | 1.14   |   | (-3.14, 2.0)  | -1.14  |   | (-3.14, -2.0)  | -1.14  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 2.34) | 0.80   |   | (3.14, -2.34) | 0.80   |   | (-3.14, 2.34) | -0.80  |   | (-3.14, -2.34) | -0.80  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 3.0)  | 0.14   |   | (3.14, -3.0)  | 0.14   |   | (-3.14, 3.0)  | -0.14  |   | (-3.14, -3.0)  | -0.14  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 3.7)  | 3.14   |   | (3.14, -3.7)  | 3.14   |   | (-3.14, 3.7)  | -3.14  |   | (-3.14, -3.7)  | -3.14  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 4.0)  | 3.14   |   | (3.14, -4.0)  | 3.14   |   | (-3.14, 4.0)  | -3.14  |   | (-3.14, -4.0)  | -3.14  |
+--------------+--------+---+---------------+--------+---+---------------+--------+---+----------------+--------+

The remainder and remquo procedures
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :ref:`remainder` and :ref:`remquo` procedures returns the remainder of :math:`x` divided by :math:`y`. Given two variables :math:`x` and :math:`y` the following must be true: :math:`remainder(x, y) = x - n \cdot y`, where :math:`n` is the integer nearest to the exact result of :math:`\frac{x}{y}` (when the exact result is exactly in the middle of two integers, :math:`n` is even). If :math:`y` is zero the result will be ``qNaN``.

Examples:

+--------------+--------+---+---------------+--------+---+---------------+--------+---+----------------+--------+
| Input (x, y) | Return |   | Input (x, y)  | Return |   | Input (x, y)  | Return |   | Input (x, y)   | Return |
+==============+========+===+===============+========+===+===============+========+===+================+========+
| (3.14, 0.2)  | -0.06  |   | (3.14, -0.2)  | -0.06  |   | (-3.14, 0.2)  | -0.06  |   | (-3.14, -0.2)  | -0.06  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 1.0)  | 0.14   |   | (3.14, -1.0)  | 0.14   |   | (-3.14, 1.0)  | 0.14   |   | (-3.14, -1.0)  | 0.14   |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 1.5)  | 0.14   |   | (3.14, -1.5)  | 0.14   |   | (-3.14, 1.5)  | 0.14   |   | (-3.14, -1.5)  | 0.14   |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 2.0)  | -0.86  |   | (3.14, -2.0)  | -0.86  |   | (-3.14, 2.0)  | -0.86  |   | (-3.14, -2.0)  | -0.86  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 2.34) | 0.80   |   | (3.14, -2.34) | 0.80   |   | (-3.14, 2.34) | -0.80  |   | (-3.14, -2.34) | -0.80  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 3.0)  | 0.14   |   | (3.14, -3.0)  | 0.14   |   | (-3.14, 3.0)  | 0.14   |   | (-3.14, -3.0)  | 0.14   |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 3.7)  | -0.56  |   | (3.14, -3.7)  | -0.56  |   | (-3.14, 3.7)  | -0.56  |   | (-3.14, -3.7)  | -0.56  |
+--------------+--------+   +---------------+--------+   +---------------+--------+   +----------------+--------+
| (3.14, 4.0)  | -0.86  |   | (3.14, -4.0)  | -0.86  |   | (-3.14, 4.0)  | -0.86  |   | (-3.14, -4.0)  | -0.86  |
+--------------+--------+---+---------------+--------+---+---------------+--------+---+----------------+--------+

The modf procedure
^^^^^^^^^^^^^^^^^^

The :ref:`modf` procedure returns the fractional part of :math:`x` and puts the integral part of :math:`x` to the outward pointer :math:`iptr`. It's easier to think of :ref:`modf` as a procedure with one input and two outputs.

Examples:

+---------+--------+-------------+---+---------+--------+-------------+
| Input x | Return | Output iptr |   | Input x | Return | Output iptr |
+=========+========+=============+===+=========+========+=============+
| 0.14    | 0.14   | 0.0         |   | -0.14   | -0.14  | -0.0        |
+---------+--------+-------------+   +---------+--------+-------------+
| 3.0     | 0.0    | 3.0         |   | -3.0    | -0.0   | -3.0        |
+---------+--------+-------------+   +---------+--------+-------------+
| 3.14    | 0.14   | 3.0         |   | -3.14   | -0.14  | -3.0        |
+---------+--------+-------------+---+---------+--------+-------------+

Side Effects
~~~~~~~~~~~~

This library does not produce any side effects, apart from the exceptions that have been described,

* to the software that uses the library,
* nor to the hardware,

as it does not configure any hardware.

Software Components Design - General
------------------------------------

.. raw:: html

   <!--
   a. The SDD shall describe:
       1. The software components, constituting the software item.
       2. The relationship between the software components.
       3. The purpose of each software component.
       4. For each software component, the development type (e.g. new development, software to be reused).
       5. If the software is written for the reuse,
           * its provided functionality from an external point of view, and
           * its external interfaces.
       6. Handling of existing reused components.
           NOTE: See Annex N.
   b. The following shall apply to the software components specified in <5.3>a.1.:
       1. Each software component is uniquely identified.
       2. When components are expressed as models, the supplier establishes a way to assign identifiers within the model for sake of traceability.
       3. The software requirements allocation provides for each software component;
           NOTE: The documented trace can be provided automatically by tools when models are used to express components.
   c. The description of the components should be laid out hierarchically, in accordance with the following aspects for each component, further described in <5.4>:
       * <Component identifier>
       * <Type>
       * <Purpose>
       * <Function>
       * <Subordinates>
       * <Dependencies>
       * <Interfaces>
       * <Resources>
       * <References>
       * <Data>
           NOTE: Detailed description of the aspects for each component are describe in <5.4>.
   -->

fenv.h
~~~~~~

Due to requirement REQ-ML-8700: When compiling user software with ``fenv.h`` included the compiler shall throw an error and stop. To achieve this the file shall contain the preprocessing directive ``#error`` before any other statement with an additional message, that if the user removes this error he shall provide his own implementations of these procedures and be aware that they are not part of this qualification effort and need to be qualified separately.

The library provides an ``fenv.c`` file in the ``libm/common`` directory which contains stub implementations for all ``fenv.h`` procedures which return an error value (:math:`-1`) in line with the ISO C18 standard :ref:`[RD03] <RD>`.

tgmath.h
~~~~~~~~

Due to requirement REQ-ML-8800: When compiling user software with ``tgmath.h`` included the compiler shall throw an error and stop. To achieve this the file shall contain the preprocessing directive ``#error`` before any other statement with an additional message, that if the user removes this error he’s left to his own devices and he’s using a not qualified library.

Long Double
~~~~~~~~~~~

The library is limited in that it only offers ``long double`` procedures when ``long double`` has the same size as ``double``. As such there will be no independent implementation of ``long double`` procedures, instead there will only be additional wrappers to call the ``double`` procedures. These wrappers are very simple in that they only have a call to their respective ``double`` version, accompanied by an ``#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS``-structure, to only include the procedure if the preprocessor macro ``LIBMCS_LONG_DOUBLE_IS_64BITS`` is set. For example for ``acoshl``:

.. code:: c

   #include <math.h>

   #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS
   long double acoshl (long double x)
   {
     return acosh(x);
   }
   #endif

As all the ``long double`` procedures look the same there is no need to add them to :ref:`Software Components Design – Aspects of Each Component` individually.

Handling of Subnormal Numbers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Each of the procedures defined in ``math.h`` and ``complex.h`` shall enforce the behaviour of the :ref:`FPU <ABBR>` regarding subnormal numbers if the preprocessor macro ``LIBMCS_FPU_DAZ`` is set (for example during the ``make`` call). This is done by multiplying all floating point input arguments with :math:`1.0` to force the compiler to move the argument into an :ref:`FPU <ABBR>` register (care needs to be taken that the multiplication can not be thrown out by the compiler, for example by using a volatile variable to contain the :math:`1.0`). Depending on the :ref:`FPU <ABBR>`'s configuration copying a subnormal value into an :ref:`FPU <ABBR>` register will either

-  set the register to zero (this would be :ref:`DAZ <ABBR>` behaviour, and can for example be seen when using a GRFPU set to non-standard mode),
-  produce an appropriate :ref:`FPU <ABBR>` exception (for example a GRFPU without being set to the non-standard mode),
-  set the register to the input argument in case the :ref:`FPU <ABBR>` is able to handle subnormal inputs (for example a MEIKO FPU),
-  or do whatever else the :ref:`FPU <ABBR>` does in case of a subnormal input,

thus enforcing the inherent behaviour of the :ref:`FPU <ABBR>` to be represented by the library.

No procedure shall be able to generate subnormal numbers if the preprocessor macro ``LIBMCS_FPU_DAZ`` is set on a platform that can not produce subnormals. When using the preprocessor macro it is up to the user to inhibit their platform if it is inherently able to produce subnormals.

NaNs
~~~~

All procedures accept ``NaNs`` of both varieties, quiet and signaling, as valid inputs. In case the input is a signaling ``NaN``, an invalid operation exception will be raised by the :ref:`FPU <ABBR>` (except for the procedures ``fabs`` and ``copysign``), and a quiet ``NaN`` will be returned (unless there is a special case that returns a result different to ``NaN``).

The procedures will never produce a signaling ``NaN`` and only return quiet ``NaNs`` (except for the procedures ``fabs`` and ``copysign``). The procedures use the :ref:`FPU <ABBR>` to generate :ref:`FPU <ABBR>` native ``NaNs`` if ``NaNs`` need to be produced artificially. Trying to produce fixed ``NaNs`` would slow down the whole code since there would be a need to check everywhere and produce the *artificial* ``NaNs``. As the overall numerical code will anyways produce :ref:`FPU <ABBR>` native ``NaNs``, it would also not help the reproducibility objective.

In case any of the inputs of a procedure is ``NaN`` and the output also is a ``NaN``, the procedure does not necessarily return the (or one of the) input ``NaN`` but may produce a different one.

The tables within this document will not differentiate between quiet and signaling ``NaNs`` as inputs as the behaviour is always the same.

In some places of this document there will be ``NaNs`` accompanied by a sign, this is to symbolize that the signbit of the ``NaN`` is set or not to be used by the ``fabs`` and ``copysign`` procedures.

Exceptions
~~~~~~~~~~

The library causes the :ref:`FPU <ABBR>` to produce exceptions. Those exceptions are ``invalid operation``, ``divide-by-zero``, ``overflow``, ``underflow``, and ``inexact``. This :ref:`SDD <ABBR>` details where the exceptions are intentionally produced but not when they appear during a mathematical operation within the procedure. A comprehensive list of all exceptions thrown by each procedure can be found in :ref:`Software Components Design – Aspects of Each Component`.

Errno
~~~~~

The library does not set the ``errno`` variable to report errors. It is explicitly ignored and will not be defined or changed as to not interfere with the user using ``errno`` elsewhere in their project.

Implementation Tricks
~~~~~~~~~~~~~~~~~~~~~

The implementation of the library employs a number of tricks to achieve the intended results. For example the following tricks are used:

-  ``if (huge+x>0) return x;`` is used to produce an ``inexact`` exception for ``x`` close to zero (``huge`` is a very large floating point number). Producing the exception is ``huge+x``, as ``huge`` is a lot larger than ``x`` and in turn the result cannot be an exact value, unless ``x`` is zero. This calculation is placed into an if-clause to ensure that the compiler may not remove the calculation during optimization. As this if-clause is always true, it creates an unreachable branch. This downside however is offset by producing the intended results.
-  ``if (x is not finite) return x+x;`` is used when ``x`` is either infinite or ``NaN`` to return a same signed infinity or the ``NaN``. The addition is needed to create the ``invalid operation`` exception in the case a signaling ``NaN`` was used as input.
-  ``if (x is not finite) return x-x;`` is used when ``x`` is either infinite or ``NaN`` to return ``NaN``. Subtracting two same signed infinity values produces a ``NaN`` as result as well as an ``invalid operation`` exception.
-  ``return (x-x)/zero;`` is used to produce a ``NaN`` regardless of the input value. This statement also produces an ``invalid operation`` exception by dividing zero by zero (or in the case of an infinite ``x``, even earlier).
-  ``return x/zero;`` is used to produce an infinity with the sign of ``x``. This statement also produces a ``divide-by-zero`` exception. This trick shall not be used when ``x`` is zero.

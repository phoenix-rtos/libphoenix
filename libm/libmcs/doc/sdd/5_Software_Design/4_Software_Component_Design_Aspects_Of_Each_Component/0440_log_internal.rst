.. _internal_log:

Internal Logarithm Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:autodoc:: mathd/internal/log1pmfd.h

.. raw:: html

   <!--
    TODO: Subsections belonging into the implementation file shall be added when the file exists.

    TODO: Recheck/Redo the procedures that use this function, :ref:`log2` and :ref:`log10`. See https://github.com/freebsd/freebsd-src/search?q=k_log1p
    -->

Special cases
^^^^^^^^^^^^^

The special cases are in the respective external functions :ref:`log2` and :ref:`log10`.

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

``__log1pmf``: The approach is to use a polynomial approximation for the calculation.

The procedure shall be always inlined.

This calculation is a simplification of the theory employed and described in :ref:`log10` and omits the considerations regarding extra precision, as this is done by the upper level procedures.

Use the following constants as calculation/return values:

* :math:`L1 =` ``0x3FE5555555555593 (0x3F2AAAAA)``
* :math:`L2 =` ``0x3FD999999997FA04 (0x3ECCCE13)``
* :math:`L3 =` ``0x3FD2492494229359 (0x3E91E9EE)``
* :math:`L4 =` ``0x3FCC71C51D8E78AF (0x3E789E26)``
* :math:`L5 =` ``0x3FC7466496CB03DE``
* :math:`L6 =` ``0x3FC39A09D078C69F``
* :math:`L7 =` ``0x3FC2F112DF3E5244``

The logarithm is computed by evaluating the following polynomial and return :math:`r`:

.. math::
   :label: formula_log_internal_1

      s &= \frac{f}{2+f}  \\
      R &= L1 \cdot s^2 + L2 \cdot s^4 + L3 \cdot s^6 + L4 \cdot s^8 + L5 \cdot s^{10} +L6 \cdot s^{12} + L7 \cdot s^{14}  \\
      r &= s \cdot \Big(\frac{f^2}{2} + R\Big)

For the float version the polynomial can be shortened to:

.. math::
   :label: formula_log_internal_2

      s &= \frac{f}{2+f}  \\
      R &= L1 \cdot s^2 + L2 \cdot s^4 + L3 \cdot s^6 + L4 \cdot s^8  \\
      r &= s \cdot \Big(\frac{f^2}{2} + R\Big)

Requirements
^^^^^^^^^^^^

Internal functions do not directly implement requirements.

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/mathd/internal/log1pmfd.h
* libm/mathf/internal/log1pmff.h
* libm/mathd/internal/log1pmfd.c
* libm/mathf/internal/log1pmff.c

References
^^^^^^^^^^

* :ref:`log10`
* :ref:`log1p`
* :ref:`log2`

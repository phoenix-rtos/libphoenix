round
~~~~~

.. c:autodoc:: mathd/roundd.c

Special cases
^^^^^^^^^^^^^

+--------------------------+--------------------------+
| x                        | Result                   |
+==========================+==========================+
| :math:`±0`               | :math:`x`                |
+--------------------------+--------------------------+
| :math:`±Inf`             | :math:`x`                |
+--------------------------+--------------------------+
| :math:`NaN`              | :math:`NaN`              |
+--------------------------+--------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

The approach is to first check whether the input is fractional, then remove the fraction and round in the correct direction.

#. All the following return values shall have the same sign as the input value.
#. If the exponent is :math:`< 20`, the integral part of :math:`x` is in the double's highword:

   #. If the exponent is :math:`< 0`, there is no integral part:

      #. If the exponent is :math:`-1`, return a zero.
      #. Return a one.

   #. If the input is integral, return :math:`x`.
   #. Return :math:`x` rounded to the nearest integer, with halfway cases rounded away from zero, regardless of the current rounding direction.

#. If the exponent is :math:`> 51`, there is no fractional part:

   #. If the exponent is :math:`= 1024`, :math:`x` is :math:`NaN` or infinite, return :math:`x+x`.
   #. Return :math:`x`.

#. If the input is integral, return :math:`x`.
#. Return :math:`x` rounded to the nearest integer, with halfway cases rounded away from zero, regardless of the current rounding direction.

For the float version the approach can be shortened:

#. All the following return values shall have the same sign as the input value.
#. If the exponent is :math:`< 23`, there may be a fractional part:

   #. If the exponent is :math:`< 0`, there is no integral part:

      #. If the exponent is :math:`-1`, return a zero.
      #. Return a one.

   #. If the input is integral, return :math:`x`.
   #. Return :math:`x` rounded to the nearest integer, with halfway cases rounded away from zero, regardless of the current rounding direction.

#. If the exponent is :math:`= 128`, :math:`x` is :math:`NaN` or infinite, return :math:`x+x`.
#. Return :math:`x`.

Requirements
^^^^^^^^^^^^

* REQ-ML-1020
* REQ-ML-1031
* REQ-ML-1032

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/roundd.c
* libm/mathf/roundf.c

References
^^^^^^^^^^

* :ref:`nearbyint`
* :ref:`rint`

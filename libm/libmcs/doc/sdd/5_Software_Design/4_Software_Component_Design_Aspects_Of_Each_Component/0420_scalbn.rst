scalbn
~~~~~~

.. c:autodoc:: mathd/scalbnd.c

Special cases
^^^^^^^^^^^^^

+------------------------+------------------------+------------------------+
| x                      | n                      | Result                 |
+========================+========================+========================+
| :math:`±0`             | :math:`\in \mathbb{I}` | :math:`x`              |
+------------------------+------------------------+------------------------+
| :math:`\in \mathbb{F}` | :math:`0`              | :math:`x`              |
+------------------------+------------------------+------------------------+
| :math:`±Inf`           | :math:`\in \mathbb{I}` | :math:`x`              |
+------------------------+------------------------+------------------------+
| :math:`NaN`            | :math:`\in \mathbb{I}` | :math:`qNaN`           |
+------------------------+------------------------+------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

The approach is to first work on special values and then use bit-fiddling tricks.

All the following return values shall have the same sign as the input value, use :ref:`copysign`.

#. For :math:`x` is zero, infinite or ``NaN``, return :math:`x`.

#. If the input is subnormal or zero:

   #. If the input is zero, return a zero.
   #. Scale up the input by multiplying with :math:`2^{54}`.
   #. If :math:`n` is :math:`< -50000` return a zero.

#. If the exponent of :math:`x` is :math:`1024`, :math:`x` is :math:`NaN` or infinite, return :math:`x+x`.
#. If the exponent of :math:`x` :math:`+ n` is :math:`> 1023`, return an infinity, with overflow.
#. If the exponent of :math:`x` :math:`+ n` is :math:`> 0`, return :math:`x` with the exponent increased by :math:`n`.
#. If the exponent of :math:`x` :math:`+ n` is :math:`> -54`, return :math:`x` with the exponent increased by :math:`n` and then multiplied with :math:`2^{-54}`.
#. If :math:`n` is :math:`> 50000` return an infinity, with overflow.
#. Return a zero.

For the float version:

#. If the input is zero, return a zero.
#. If the input is not finite, return :math:`x+x`.
#. If the input is subnormal:

   #. Scale up the input by multiplying with :math:`2^{25}`.
   #. If :math:`n` is :math:`< -50000` return a zero.

#. If the exponent :math:`x` :math:`+ n` is :math:`> 254`, return an infinity, with overflow.
#. If the exponent :math:`x` :math:`+ n` is :math:`> 0`, return :math:`x` with the exponent increased by :math:`n`.
#. If the exponent :math:`x` :math:`+ n` is :math:`>= -22`, return :math:`x` with the exponent increased by :math:`n` and then multiplied with :math:`2^{-25}`.
#. If :math:`n` is :math:`> 50000` return an infinity, with overflow.
#. Return a zero.

Requirements
^^^^^^^^^^^^

* REQ-ML-4200
* REQ-ML-4210
* REQ-ML-4220
* REQ-ML-4230
* REQ-ML-4240

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/scalbnd.c
* libm/mathf/scalbnf.c

References
^^^^^^^^^^

* :ref:`__rem_pio2_internal <internal_trig>`
* :ref:`copysign`
* :ref:`ldexp`
* :ref:`pow`
* :ref:`scalbln`

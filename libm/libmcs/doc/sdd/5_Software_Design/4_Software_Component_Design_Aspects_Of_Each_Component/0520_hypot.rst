hypot
~~~~~

.. c:autodoc:: mathd/hypotd.c

Special cases
^^^^^^^^^^^^^

+-----------------------------+-----------------------------+------------------------------+
| x                           | y                           | Result                       |
+=============================+=============================+==============================+
| :math:`\in \mathbb{F}`      | :math:`±Inf`                | :math:`+Inf`                 |
+-----------------------------+-----------------------------+------------------------------+
| :math:`±Inf`                | :math:`\in \mathbb{F}`      | :math:`+Inf`                 |
+-----------------------------+-----------------------------+------------------------------+
| :math:`NaN`                 | :math:`\neq ±Inf`           | :math:`qNaN`                 |
+-----------------------------+-----------------------------+------------------------------+
| :math:`\neq ±Inf`           | :math:`NaN`                 | :math:`qNaN`                 |
+-----------------------------+-----------------------------+------------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

The approach is to first work on special values and overflows, then use :ref:`sqrt` for the calculation.

#. Assume :math:`x > y`, otherwise replace :math:`x` with :math:`y` and :math:`y` with :math:`x`.
#. Set both :math:`x` and :math:`y` to their absolute value.
#. For :math:`\frac{x}{y} > 2^{60}` (float: :math:`2^{30}`), :math:`y` is too small to matter when the result is represented with double (float) precision, return :math:`x`.
#. For :math:`x` or :math:`y` is infinite, return ``+Inf``.
#. For :math:`x` or :math:`y` is ``NaN``, return :math:`x + y`.
#. For :math:`x > 2^{500}` (float: :math:`2^{50}`), scale down :math:`x` and :math:`y` by multiplying them with :math:`2^{-600}` (float: :math:`2^{-68}`), otherwise their combined squares will overflow the double (float) range.
#. For :math:`y < 2^{-500}` (float: :math:`2^{-50}`), scale up :math:`x` and :math:`y` by multiplying them with :math:`2^{600}` (float: :math:`2^{68}`), otherwise their combined squares will underflow the double (float) range.

#. If :math:`z = x \cdot x + y \cdot y` has an error of less than :math:`\frac{\sqrt{2}}{2}` :ref:`ULPs <ABBR>`, then :math:`\sqrt{z}` has an error of less than 1 :ref:`ULP <ABBR>`. Using this relation calculate :math:`\sqrt{x \cdot x + y \cdot y}` as follows:

    #. If :math:`x > 2y` use the following to replace :math:`x \cdot x + y \cdot y`:

       .. math::
          :label: formula_hypot_1

          x_1 \cdot x_1 + (y \cdot y + (x_2 \cdot (x+x_1)))

       with :math:`x_1 = x` with the lower 32bits (float: 12bits) cleared and :math:`x_2 = x - x_1`.
    #. Otherwise use:

       .. math::
          :label: formula_hypot_2

          t_1 \cdot y_1 + ((x-y) \cdot (x-y)+(t_1 \cdot y_2 + t_2 \cdot y))

       with :math:`t_1 = 2x` with the lower 32bits (float: 12bits) cleared and :math:`t_2 = 2x - t_1`, and :math:`y_1 = y` with the lower 32bits (float: 12bits) cleared and :math:`y_2 = y - y_1`.
    #. Use :ref:`sqrt` to calculate the square root of the replacement :math:numref:`formula_hypot_1` or :math:numref:`formula_hypot_2`.

#. Revert the scaling on the resulting square root if any was done in an earlier step.
#. Return the calculated square root.

Requirements
^^^^^^^^^^^^

* REQ-ML-1260
* REQ-ML-1270
* REQ-ML-1271

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/hypotd.c
* libm/mathf/hypotf.c

References
^^^^^^^^^^

* :ref:`sqrt`

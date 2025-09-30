ldexp
~~~~~

.. c:autodoc:: mathd/ldexpd.c

Special cases
^^^^^^^^^^^^^

+------------------------+------------------------+------------------------+
| x                      | exp                    | Result                 |
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

The approach is to first work on special values and then use :ref:`scalbn`.

#. For :math:`x` is zero, infinite or ``NaN``, return :math:`x`.
#. Else use :ref:`scalbn` and return its result.

Requirements
^^^^^^^^^^^^

* REQ-ML-4100
* REQ-ML-4110
* REQ-ML-4120
* REQ-ML-4130
* REQ-ML-4140

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/ldexpd.c
* libm/mathf/ldexpf.c

References
^^^^^^^^^^

* :ref:`isfinite`
* :ref:`scalbn`

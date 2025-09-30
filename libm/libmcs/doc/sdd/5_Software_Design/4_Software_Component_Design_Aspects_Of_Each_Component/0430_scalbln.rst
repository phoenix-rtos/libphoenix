scalbln
~~~~~~~

.. c:autodoc:: mathd/scalblnd.c

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

Implement similarly to :ref:`scalbn`.

.. Here there be dragons. (TODO)

Requirements
^^^^^^^^^^^^

* REQ-ML-4250

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/scalblnd.c
* libm/mathf/scalblnf.c

References
^^^^^^^^^^

* :ref:`copysign`
* :ref:`scalbn`

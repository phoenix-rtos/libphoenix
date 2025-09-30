nearbyint
~~~~~~~~~

.. c:autodoc:: mathd/nearbyintd.c

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

Call :ref:`rint`.

Requirements
^^^^^^^^^^^^

* REQ-ML-4500
* REQ-ML-4510
* REQ-ML-4520
* REQ-ML-4540

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/nearbyintd.c
* libm/mathf/nearbyintf.c

References
^^^^^^^^^^

* :ref:`rint`
* :ref:`round`

isgreater
~~~~~~~~~

.. c:autodoc:: common/isgreater.c

Special cases
^^^^^^^^^^^^^

+-----------------------------+-----------------------------+-----------------------------+
| x                           | y                           | Result                      |
+=============================+=============================+=============================+
| :math:`NaN`                 | :math:`\in \mathbb{F}`      | :math:`0`                   |
+-----------------------------+-----------------------------+-----------------------------+
| :math:`\in \mathbb{F}`      | :math:`NaN`                 | :math:`0`                   |
+-----------------------------+-----------------------------+-----------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

#. Use :ref:`isunordered` to return :math:`0` if either input is :math:`NaN`.
#. If :math:`x > y`, return :math:`1`, otherwise return :math:`0`.

Requirements
^^^^^^^^^^^^

* REQ-ML-5000
* REQ-ML-5010

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/common/isgreater.c

References
^^^^^^^^^^

* :ref:`isunordered`

isless
~~~~~~

.. c:autodoc:: common/isless.c

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
#. If :math:`x < y`, return :math:`1`, otherwise return :math:`0`.

Requirements
^^^^^^^^^^^^

* REQ-ML-5200
* REQ-ML-5210

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/common/isless.c

References
^^^^^^^^^^

* :ref:`isunordered`

islessequal
~~~~~~~~~~~

.. c:autodoc:: common/islessequal.c

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
#. If :math:`x <= y`, return :math:`1`, otherwise return :math:`0`.

Requirements
^^^^^^^^^^^^

* REQ-ML-5300
* REQ-ML-5310

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/common/islessequal.c

References
^^^^^^^^^^

* :ref:`isunordered`

tgamma
~~~~~~

.. c:autodoc:: mathd/tgammad.c

Special cases
^^^^^^^^^^^^^

+--------------------------------------+--------------------------------------+
| x                                    | Result                               |
+======================================+======================================+
| :math:`-Inf`                         | :math:`qNaN`                         |
+--------------------------------------+--------------------------------------+
| :math:`\in \mathbb{Z}_{<0}`          | :math:`qNaN`                         |
+--------------------------------------+--------------------------------------+
| :math:`±0`                           | :math:`±Inf`                         |
+--------------------------------------+--------------------------------------+
| :math:`+Inf`                         | :math:`+Inf`                         |
+--------------------------------------+--------------------------------------+
| :math:`NaN`                          | :math:`qNaN`                         |
+--------------------------------------+--------------------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

Call :ref:`__lgamma <internal_gamma>` with :math:`x` and a local pointer to receive the sign. Use the result of :ref:`__lgamma <internal_gamma>` as input for :ref:`exp` and use the local pointer with the sign to decide the sign of the result of the :ref:`exp` call. Return the now correctly signed result.

Requirements
^^^^^^^^^^^^

* REQ-ML-5800
* REQ-ML-5810
* REQ-ML-5820
* REQ-ML-5830
* REQ-ML-5840
* REQ-ML-5841

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/tgammad.c
* libm/mathf/tgammaf.c

References
^^^^^^^^^^

* :ref:`__lgamma <internal_gamma>`
* :ref:`exp`
* :ref:`lgamma`

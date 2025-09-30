acosh
~~~~~

.. c:autodoc:: mathd/acoshd.c

Special cases
^^^^^^^^^^^^^

+--------------------------+--------------------------+
| x                        | Result                   |
+==========================+==========================+
| :math:`+1`               | :math:`+0`               |
+--------------------------+--------------------------+
| :math:`<+1`              | :math:`qNaN`             |
+--------------------------+--------------------------+
| :math:`-Inf`             | :math:`qNaN`             |
+--------------------------+--------------------------+
| :math:`+Inf`             | :math:`+Inf`             |
+--------------------------+--------------------------+
| :math:`NaN`              | :math:`qNaN`             |
+--------------------------+--------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

Implement based on :math:`cosh^{-1}(x) = ln(x + \sqrt{x^2-1})` using :ref:`log`, :ref:`log1p`, and :ref:`sqrt`:

#. For large :math:`x` (:math:`x > 2^{28}`): :math:`acosh(x) = log(x) + log(2)`
#. For :math:`x > 2`: :math:`acosh(x) = log\Big(\frac{2x - 1}{x + \sqrt{x^2 - 1}}\Big)`
#. For smaller :math:`x`: :math:`acosh(x) = log1p(t + \sqrt{2t+t^2}) \wedge t = x - 1`

Requirements
^^^^^^^^^^^^

* REQ-ML-2710
* REQ-ML-3010
* REQ-ML-3020
* REQ-ML-3030
* REQ-ML-3040

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/acoshd.c
* libm/mathf/acoshf.c

References
^^^^^^^^^^

* :ref:`log`
* :ref:`log1p`
* :ref:`sqrt`

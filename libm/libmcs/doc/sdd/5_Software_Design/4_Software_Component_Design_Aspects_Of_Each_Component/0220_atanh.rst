atanh
~~~~~

.. c:autodoc:: mathd/atanhd.c

Special cases
^^^^^^^^^^^^^

+--------------------------+--------------------------+
| x                        | Result                   |
+==========================+==========================+
| :math:`±0`               | :math:`x`                |
+--------------------------+--------------------------+
| :math:`\notin [-1, 1]`   | :math:`qNaN`             |
+--------------------------+--------------------------+
| :math:`±1`               | :math:`±Inf`             |
+--------------------------+--------------------------+
| :math:`±Inf`             | :math:`qNaN`             |
+--------------------------+--------------------------+
| :math:`NaN`              | :math:`qNaN`             |
+--------------------------+--------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

Implement based on :math:`tanh^{-1}(x) = \frac{1}{2} ln \left( \frac{1+x}{1-x} \right)` using :ref:`log1p`:

#. Reduce to positive as :math:`tanh^{-1}(-x) = -tanh^{-1}(x)`
#. For :math:`x >= \frac{1}{2}`: :math:`atanh(x) = \frac{1}{2} log\Big(1 + \frac{2x}{1 - x}\Big) = \frac{1}{2} log1p\Big(\frac{2x}{1 - x}\Big)`
#. Otherwise: :math:`atanh(x) = \frac{1}{2} log1p\Big(2x + \frac{2x^2}{1-x}\Big)`

Requirements
^^^^^^^^^^^^

* REQ-ML-3100
* REQ-ML-3110
* REQ-ML-3120
* REQ-ML-3121
* REQ-ML-3140
* REQ-ML-3141

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/atanhd.c
* libm/mathf/atanhf.c

References
^^^^^^^^^^

* :ref:`log1p`

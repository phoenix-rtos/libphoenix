asinh
~~~~~

.. c:autodoc:: mathd/asinhd.c

Special cases
^^^^^^^^^^^^^

+--------------------------+--------------------------+
| x                        | Result                   |
+==========================+==========================+
| :math:`±0`               | :math:`x`                |
+--------------------------+--------------------------+
| :math:`±Inf`             | :math:`x`                |
+--------------------------+--------------------------+
| :math:`NaN`              | :math:`qNaN`             |
+--------------------------+--------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

Implement based on :math:`sinh^{-1}(x) = sign(x) \cdot ln(|x| + \sqrt{x^2+1})` using :ref:`log`, :ref:`log1p`, and :ref:`sqrt`:

#. For large :math:`x` (:math:`|x| > 2^{28}`): :math:`asinh(x) = sign(x) \cdot log(|x|) + log(2)`
#. For :math:`x > 2`: :math:`asinh(x) = sign(x) \cdot log\Big(\frac{|2x| + 1}{|x| + \sqrt{x^2 + 1}}\Big)`
#. For smaller :math:`x`: :math:`asinh(x) = sign(x) \cdot log1p(|x| + \frac{x^2}{1 + \sqrt{1+x^2}})`
#. For very small :math:`x` (:math:`|x| < 2^{-28}`): :math:`asinh(x) = x`

.. Here there be dragons. (TODO)

Requirements
^^^^^^^^^^^^

* REQ-ML-2620
* REQ-ML-2630
* REQ-ML-2640
* REQ-ML-2700

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/asinhd.c
* libm/mathf/asinhf.c

References
^^^^^^^^^^

* :ref:`fabs`
* :ref:`log`
* :ref:`log1p`
* :ref:`sqrt`

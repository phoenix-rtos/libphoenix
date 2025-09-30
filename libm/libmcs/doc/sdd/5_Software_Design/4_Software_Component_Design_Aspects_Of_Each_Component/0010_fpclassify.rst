fpclassify
~~~~~~~~~~

.. c:autodoc:: mathd/internal/fpclassifyd.c

Special cases
^^^^^^^^^^^^^

+------------------------------+--------------------------+
| x                            | Result                   |
+==============================+==========================+
| :math:`±0`                   | ``FP_ZERO``              |
+------------------------------+--------------------------+
| :math:`\in \mathbb{S}`       | ``FP_SUBNORMAL``         |
+------------------------------+--------------------------+
| :math:`±Inf`                 | ``FP_INFINITE``          |
+------------------------------+--------------------------+
| :math:`NaN`                  | ``FP_NAN``               |
+------------------------------+--------------------------+
| all others                   | ``FP_NORMAL``            |
+------------------------------+--------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

The approach is to call one of two internal procedures named ``__fpclassifyd`` or ``__fpclassifyf`` depending on the type of the input. These procedures follow the following approach:

#. For :math:`x` is :math:`-0` or :math:`+0`, return ``FP_ZERO``.
#. For :math:`x` is finite and not subnormal, return ``FP_NORMAL``.
#. For :math:`x` is subnormal, return ``FP_SUBNORMAL``.
#. For :math:`x` is infinite (:math:`±Inf`), return ``FP_INFINITE``.
#. For :math:`x` is :math:`NaN`, return ``FP_NAN``.

Requirements
^^^^^^^^^^^^

* REQ-ML-5700

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/internal/fpclassifyd.c
* libm/mathf/internal/fpclassifyf.c

References
^^^^^^^^^^

* :numref:`Tbl. %s <List of math constants>`
* :ref:`isfinite`
* :ref:`isinf`
* :ref:`isnan`
* :ref:`isnormal`
* :ref:`isunordered`

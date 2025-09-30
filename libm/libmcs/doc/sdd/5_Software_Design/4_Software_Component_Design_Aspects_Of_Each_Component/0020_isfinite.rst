isfinite
~~~~~~~~

.. c:autodoc:: common/isfinite.c

Special cases
^^^^^^^^^^^^^

This macro does not have special cases.

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

The approach is to call ``fpclassify`` and decide on the return value depending on the result:

#. Call ``fpclassify`` with input :math:`x`, return :math:`0` if the call returned ``FP_INFINITE`` or ``FP_NAN``.
#. Else return :math:`1`.

Requirements
^^^^^^^^^^^^

* REQ-ML-1300

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/common/isfinite.c

References
^^^^^^^^^^

* :ref:`fpclassify`
* :ref:`ldexp`

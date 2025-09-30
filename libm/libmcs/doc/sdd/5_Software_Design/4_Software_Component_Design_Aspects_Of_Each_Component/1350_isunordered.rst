isunordered
~~~~~~~~~~~

.. c:autodoc:: common/isunordered.c

Special cases
^^^^^^^^^^^^^

This macro does not have special cases.

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

If at least one input is :math:`NaN` return :math:`1`, else return :math:`0`.

Requirements
^^^^^^^^^^^^

* REQ-ML-4900
* REQ-ML-4910

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/common/isunordered.c

References
^^^^^^^^^^

* :ref:`isgreater`
* :ref:`isgreaterequal`
* :ref:`isless`
* :ref:`islessequal`
* :ref:`islessgreater`

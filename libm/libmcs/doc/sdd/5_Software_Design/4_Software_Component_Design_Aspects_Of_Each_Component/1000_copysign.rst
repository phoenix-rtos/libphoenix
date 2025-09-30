copysign
~~~~~~~~

.. c:autodoc:: mathd/copysignd.c

Special cases
^^^^^^^^^^^^^

+-----------------------------+-----------------------------+-----------------------------+
| x                           | y                           | Result                      |
+=============================+=============================+=============================+
| :math:`NaN`                 | :math:`\in \mathbb{F}`      | :math:`qNaN`                |
+-----------------------------+-----------------------------+-----------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

Return the value with the magnitude of :math:`x` and sign of :math:`y`.

Requirements
^^^^^^^^^^^^

* REQ-ML-1380
* REQ-ML-1381

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/copysignd.c
* libm/mathf/copysignf.c

References
^^^^^^^^^^

* :ref:`cproj`

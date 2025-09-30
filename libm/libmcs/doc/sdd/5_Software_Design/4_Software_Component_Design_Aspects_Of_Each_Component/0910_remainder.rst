remainder
~~~~~~~~~

.. c:autodoc:: mathd/remainderd.c

Special cases
^^^^^^^^^^^^^

+-----------------------------+-----------------------------+-----------------------------+
| x                           | y                           | Result                      |
+=============================+=============================+=============================+
| :math:`\in \mathbb{F}`      | :math:`±0`                  | :math:`qNaN`                |
+-----------------------------+-----------------------------+-----------------------------+
| :math:`±Inf`                | :math:`\in \mathbb{F}`      | :math:`qNaN`                |
+-----------------------------+-----------------------------+-----------------------------+
| :math:`NaN`                 | :math:`\in \mathbb{F}`      | :math:`qNaN`                |
+-----------------------------+-----------------------------+-----------------------------+
| :math:`\in \mathbb{F}`      | :math:`NaN`                 | :math:`qNaN`                |
+-----------------------------+-----------------------------+-----------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

Implement based on :ref:`fmod`.

.. Here there be dragons. (TODO)

Requirements
^^^^^^^^^^^^

* REQ-ML-3900
* REQ-ML-3910
* REQ-ML-3920
* REQ-ML-3940

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/remainderd.c
* libm/mathf/remainderf.c

References
^^^^^^^^^^

* :ref:`fmod`

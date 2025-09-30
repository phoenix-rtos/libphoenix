lrint
~~~~~

.. c:autodoc:: mathd/lrintd.c

Special cases
^^^^^^^^^^^^^

+------------------------------------+------------------------------------+
| x                                  | Result                             |
+====================================+====================================+
| :math:`±0`                         | :math:`0`                          |
+------------------------------------+------------------------------------+
| :math:`-Inf`                       | min :math:`\mathbb{I}_l`           |
+------------------------------------+------------------------------------+
| :math:`<` min :math:`\mathbb{I}_l` | min :math:`\mathbb{I}_l`           |
+------------------------------------+------------------------------------+
| :math:`>` max :math:`\mathbb{I}_l` | max :math:`\mathbb{I}_l`           |
+------------------------------------+------------------------------------+
| :math:`+Inf`                       | max :math:`\mathbb{I}_l`           |
+------------------------------------+------------------------------------+
| :math:`NaN`                        | :math:`lrint(±Inf)`                |
+------------------------------------+------------------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

Implement based on bit-fiddling.

.. Here there be dragons. (TODO)

Requirements
^^^^^^^^^^^^

* REQ-ML-4650
* REQ-ML-4653
* REQ-ML-4656
* REQ-ML-4659
* REQ-ML-4662

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/lrintd.c
* libm/mathf/lrintff.c

References
^^^^^^^^^^

* :ref:`rint`

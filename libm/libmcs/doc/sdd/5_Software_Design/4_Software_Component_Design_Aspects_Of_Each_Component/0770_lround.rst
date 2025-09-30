lround
~~~~~~

.. c:autodoc:: mathd/lroundd.c

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
| :math:`NaN`                        | :math:`lround(±Inf)`               |
+------------------------------------+------------------------------------+

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

Implement based on bit-fiddling.

.. Here there be dragons. (TODO)

Requirements
^^^^^^^^^^^^

* REQ-ML-8300
* REQ-ML-8310
* REQ-ML-8320
* REQ-ML-8330
* REQ-ML-8340

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/include/math.h
* libm/mathd/lroundd.c
* libm/mathf/lroundf.c

References
^^^^^^^^^^

* :ref:`round`

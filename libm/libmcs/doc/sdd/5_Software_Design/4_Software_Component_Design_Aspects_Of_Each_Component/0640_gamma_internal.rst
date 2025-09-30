.. _internal_gamma:

Internal Gamma Functions
~~~~~~~~~~~~~~~~~~~~~~~~

.. c:autodoc:: mathd/internal/gammad.c

Special cases
^^^^^^^^^^^^^

The special cases are in the external functions :ref:`lgamma` and :ref:`tgamma`.

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

``__lgamma``: Implement based on a rational approximation.

.. Here there be dragons. (TODO)

``__sin_pi``: Implement based on :ref:`__cos <internal_trig>`, and :ref:`__sin <internal_trig>`.

.. Here there be dragons. (TODO)

Requirements
^^^^^^^^^^^^

Internal functions do not directly implement requirements.

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/mathd/internal/gammad.c
* libm/mathd/internal/gammad.h
* libm/mathf/internal/gammaf.c
* libm/mathf/internal/gammaf.h

References
^^^^^^^^^^

* :ref:`__cos, __sin <internal_trig>`
* :ref:`lgamma`
* :ref:`tgamma`

.. _internal_misc:

Exception Raising And Floating-Point/Integer Conversion Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:autodoc:: common/tools.h

Special cases
^^^^^^^^^^^^^

The exception raising procedures are themselves special cases but do not have any.

The floating-point to integer conversion (and vice-versa) procedures do not have any.

Mathematical Approach
^^^^^^^^^^^^^^^^^^^^^

Implement based on bit-fiddling and tricks for forcing the FPU to do as it should.

.. Here there be dragons. (TODO)

Requirements
^^^^^^^^^^^^

Internal functions do not directly implement requirements.

Source Code Files
^^^^^^^^^^^^^^^^^

* libm/common/tools.h
* libm/common/tools.c

References
^^^^^^^^^^

Exception raising procedures are used in a lot of other procedures. They are used whenever the `Mathematical Approach` of the procedure explicitly states raising an exception.

Floating-point to integer conversion (and vice-versa) procedures are used in a lot of other procedures. They are used whenever the `Mathematical Approach` of the procedure makes use of, or manipulates, the hexadecimal representation of floating-point datums, for example for checking/extracting/manipulating the sign/exponent/mantissa. They are used quite liberally.

``SAFE_RIGHT_SHIFT`` is used by :ref:`lrint`, :ref:`llrint`, :ref:`lround` and :ref:`llround`.

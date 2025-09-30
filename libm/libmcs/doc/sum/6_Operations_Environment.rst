Operations Environment
======================

General
~~~~~~~

The software in this project is designed to be included in other software.

Other than a compiler no additional software is necessary to build, or access the library's procedures.

The hardware parts used by the library are the :ref:`CPU <ABBR>`, :ref:`FPU <ABBR>` and :ref:`RAM <ABBR>`.

The following sections describe additional constraints which have to be considered when using the library on specific hardware platforms.

.. _OperationsEnviromentHardwareConfiguration:

Hardware Configuration
~~~~~~~~~~~~~~~~~~~~~~

The software described here contains several mathematical procedures which execute floating point operations. For this to be possible the target platform's configuration needs to be modified, in particular the :ref:`FPU <ABBR>`:

#. Enable the :ref:`FPU <ABBR>` before executing any floating-point procedure.
#. Configure the :ref:`FPU <ABBR>` regarding subnormal numbers and other trap causing situations. See also :ref:`GeneralBehaviourSubnormalValues`.

As these are platform dependent actions, their procedures cannot reasonably be described here for every platform and have to be researched in the corresponding hardware manuals.

.. _OperationsEnviromentSoftwareConfiguration:

Software Configuration
~~~~~~~~~~~~~~~~~~~~~~

The software configuration is confined to the configuration script and the compile switches. At runtime, there is no option to configure the library itself.

To include the library in other software projects while guaranteeing :ref:`IEEE-754 <ABBR>` standard compliance, the following compilation flags should be enabled when building the project when using :ref:`GCC <ABBR>`:

* ``-frounding-math``
* ``-fsignaling-nans``
* ``-fno-builtin``

We advise using the following flag combination to include exclusively those procedures into the final executable that are actually used within your software:

* ``-ffunction-sections -fdata-sections``

For this to work you also have to add the linker option ``-gc-sections`` while building your project.

On the other hand the following compilation flags shall not be used, since they can produce code which is not compliant to the :ref:`IEEE-754 <ABBR>` standard:

* ``-O3``
* ``-ffast-math``, it activates ``-funsafe-math-optimizations``, ``-fno-rounding-math``, ``-ffinite-math-only``, ``-fno-signaling-nans``, and ``-fcx-limited-range``

Depending on the processor and :ref:`FPU <ABBR>` you are using consider adding architecture specific flags, for example those listed in `SPARC Options <https://gcc.gnu.org/onlinedocs/gcc/SPARC-Options.html>`_ (especially those starting with ``-mfix``).

Additionally we have tested the following compilation flags which caused no problems to the behaviour of the library (tested with the :ref:`RTEMS <ABBR>` 4.8 toolchain provided by EDISOFT):

* ``-O2``
* ``-mhard-float`` [#]_
* ``-mcpu=v8``
* ``-fno-zero-initialized-in-bss``
* ``-fno-inline-functions-called-once``

In case the used architecture and toolchain support :ref:`FMA <ABBR>` instructions we advise using the flag ``-mno-fused-madd`` as the library was not tested for compatibility with said instruction.

Remember to also add flags that alter the sizes of different types such as ``long double`` or ``long int`` to those appropriate for your hardware, for example ``-mlong-double-64`` can be set for x86 targets that typically use 80-bit ``long double``. These flags are different depending on the toolchain and should be in line with the rest of your software.

.. _OperationsEnviromentUsingHardwareInstructions:

Using Hardware Instructions
~~~~~~~~~~~~~~~~~~~~~~~~~~~

To replace LibmCS procedures with custom procedures, such as hardware instruction calling procedures, several steps need to be followed:

* Add the file containing the procedure into an architecture specific sub-directory within the ``libm/machine`` directory. If the directory does not exist, make one for your architecture. An example for this step has already been added to the library for the SPARC V8 architecture.
* Replace the file you want to replace in the ``Makefile.in``. As an example for this step we have added a patch for the library where this is done for the SPARC V8 ``sqrt`` procedure.

Note that replacing procedures may cause different results and in turn may cause unit tests to fail. You will have to justify those fails, or change the respective tests and justify that change.

.. [#] The flag ``-mhard-float`` is the default behaviour for SPARC compilations with :ref:`GCC <ABBR>`.

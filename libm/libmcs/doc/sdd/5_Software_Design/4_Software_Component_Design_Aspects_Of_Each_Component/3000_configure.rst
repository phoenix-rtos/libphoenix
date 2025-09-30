configure
~~~~~~~~~

Description and Use
^^^^^^^^^^^^^^^^^^^

The ``configure`` file shall be a script to prepare for running the :ref:`Makefile`. It's intention is to smoothen the process of building and rebuilding the library for multiple platforms, toolchains and configurations.

For this the user has to be asked multiple questions to configure the library for the project's use-case.

Alternatively the script should be callable with command line options to enable/disable the flags and input paths. The use should be very similar to ``configure`` scripts created using an ``autoconf/automake`` setup.

Approach
^^^^^^^^

#. Use ``getopt`` to define usable flags.
#. Parse user arguments into variables.
#. Create a user help (flag ``-h`` or on unexpected options).
#. Create a an empty ``user_make`` file which will later be read by the :ref:`Makefile`.
#. Create a minimal C program to check preset defines of the toolchain (e.g., type sizes).
#. Before asking the following questions first check if the user has already provided the answer via flag.
#. If any of the below questions was answered with a faulty value, ask again.
#. Ask for path to toolchain.
#. Ask for additional compilation flags.
#. Ask for DAZ/FTZ mode.
#. Use the previously created C program to check the size of ``long int``, if it is 32bit add ``-DLIBMCS_LONG_IS_32BITS`` to the additional compilation flags.
#. Use the previously created C program to check the size of ``double``, if it is 32bit add ``-DLIBMCS_DOUBLE_IS_32BITS`` to the additional compilation flags.
#. Use the previously created C program to check the size of ``long double``, if it is 64bit ask if ``long double`` procedures shall be compiled into the library, and if so add ``-DLIBMCS_LONG_DOUBLE_IS_64BITS`` to the additional compilation flags.
#. Use the previously created C program to check the endianess of the toolchain/platform/flag-combination, if no endianess is found ask the user whether it is little or big endian, and add the specific flag to the additional compilation flags (either ``-D__BYTE_ORDER__=__ORDER_BIG_ENDIAN__`` or ``-D__BYTE_ORDER__=__ORDER_LITTLE_ENDIAN__``).
#. Add the path to the toolchain and the accumulated additional compilation flags to the ``user_make``.
#. Ask if ``complex`` procedures shall be compiled into the library, and if so add a variable to the ``user_make``.
#. Finish of the ``user_make`` with a variable that tells the :ref:`Makefile` that the configuration was successful.

Requirements
^^^^^^^^^^^^

* REQ-ML-0024
* REQ-ML-0025
* REQ-ML-0026
* REQ-ML-0027
* REQ-ML-0028
* REQ-ML-0090
* REQ-ML-0100
* REQ-ML-0115
* REQ-ML-0180
* REQ-ML-1901

Source Code Files
^^^^^^^^^^^^^^^^^

* configure

References
^^^^^^^^^^

* :ref:`Makefile`

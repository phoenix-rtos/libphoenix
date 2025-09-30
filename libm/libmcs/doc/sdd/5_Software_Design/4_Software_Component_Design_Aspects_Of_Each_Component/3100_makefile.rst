Makefile
~~~~~~~~

Description and Use
^^^^^^^^^^^^^^^^^^^

The ``Makefile`` is a commonly used script to build software, in this case a library.

This ``Makefile`` has to be run after :ref:`configure` was run successfully, otherwise it shall throw an error. This may be avoidable by adding the configuration successful variable to the ``make`` call, but shall not be encouraged.

This ``Makefile`` shall provide the following targets: ``all`` (builds the library), ``debug`` (same as ``all``), ``release`` (same as ``debug`` but with extra flag ``-DNDBUG``), ``clean`` (removes the build corresponding to the current configuration), ``cleanall`` (removes all builds), and ``distclean`` (removes ``user_make`` and the C program built for testing purposes within :ref:`configure` (if :ref:`configure` was run successfully the testing files should already be removed)).

Approach
^^^^^^^^

#. Include ``user_make``.
#. Check for the configuration successful variable, if it is not set (either via ``user_make`` or manually) stop and throw an error.
#. Check if the user provided a build name, if not create one based on toolchain and target.
#. Define include paths.
#. Define source paths. Check if the user wanted ``long double`` and/or ``complex`` procedures and add them if so.
#. Add compilation/linking flags (library specific flags, user flags, coverage, etc.).
#. Build it all.
#. Add mechanism for verbose output throughout all steps.
#. Add mechanism to create a ``build_log`` in YAML format.

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

* Makefile

References
^^^^^^^^^^

* :ref:`configure`

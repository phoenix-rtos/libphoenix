Bindings for LibmCS
===================

.. raw:: html

   <!--
   TODO: Is this chapter accurate to the current state at all?
    -->

To enable the usage of the library from Matlab and using the Ada programming language we wish to provide some best practices and examples. This will cover the usage of library functions, but not the macros and constants as those are not directly accessible, they can however be easily wrapped with C-functions that return the result/value of the macros and constants. These functions can then be placed into Matlab/Ada the same as all other functions.

Matlab
~~~~~~

This section describes the best practice to use the library from within Matlab using the sine function as an example.

.. _BindingsMatlabGCC:

Wrapper for GCC
^^^^^^^^^^^^^^^

Verify that your system has a working :ref:`GCC <ABBR>` toolchain. Officially supported by Matlab is :ref:`GCC <ABBR>` 4.9, but newer versions seem to work as well (at least for this example). Next, create a file named ``gcc`` with the following content:

.. code-block:: bash

   #!/bin/bash

   /usr/bin/gcc $(echo $@ | sed 's/-lm //g')

Place this script in your path before the real :ref:`GCC <ABBR>` and check that ``which gcc`` in your shell returns the path to this script instead of the real :ref:`GCC <ABBR>`:

.. code-block:: bash

   % which gcc

   /home/<user_name>/bin/gcc

This strips ``-lm``, the default math library from the :ref:`GCC <ABBR>` command line which allows to use the library's ``libm.a`` instead. Now start Matlab.

Wrapper for a C Function
^^^^^^^^^^^^^^^^^^^^^^^^

Create a wrapper for the :ref:`sin` function available in the library's ``libm.a``. Name the file ``libmcs_sin.m`` and fill it with this content:

.. code-block:: matlab

   function y = libmcs_sin(u)

      y = 0.0;

      y = coder.ceval('sin', u);

Using the C Function Wrapper from Matlab
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The wrapper cannot be called directly because ``ceval`` cannot be called from Matlab. Thus it is necessary to generate a MEX function from this wrapper with ``codegen``. Generate and compile the code with the following command:

.. code-block:: matlab

   codegen -config:mex -args 0.0 -o libmcs_sin ../libmcs/build-<used_ARCH>/bin/libm.a libmcs_sin

Then calling the C function is possible from Matlab as one would expect. Matlab automatically uses the generated MEX function instead of the ``.m`` function:

.. code-block:: matlab

   >> libmcs_sin(3.14)

   ans =

      0.0016

After all MEX functions are generated, remove the :ref:`GCC <ABBR>` wrapper created in :ref:`BindingsMatlabGCC`. Otherwise you may not be able to build other software correctly.

Using the Wrapper from Simulink
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Create a new Model. Go to ``Simulation -> Model Configuration Parameters -> Simulation Target``. Under ``Additional Build Information -> Libraries`` enter the path to the library's ``libm.a``. In the reserved name field enter the function which shall not be used from the default libm, in this example ``sin``. The options are shown in the figure:

.. raw:: html

   <!--
   TODO: Add figure. ./Figure/config1.PNG Caption{Simulink: Replacement Configuration Part 1}
    -->

Go to ``Simulation -> Model Configuration Parameters -> Code Generation -> Custom Code`` and check the checkbox reading "Use the same custom code settings as simulation target". The options are shown in the figure:

.. raw:: html

   <!--
   TODO: Add figure. ./Figure/config2.PNG Caption{Simulink: Replacement Configuration Part 2}
    -->

Now add a Constant Block, a Matlab Function Block and a Scope Block to your model, so it looks like shown in the figure:

.. raw:: html

   <!--
   TODO: Add figure. ./Figure/simulink.PNG Caption{Simulink:  Minimal Simulink Model}
    -->

Double-click the Matlab Function Block and type in your Matlab Code calling the previously defined Wrapper:

.. code-block:: matlab

   function y = fcn(u)

      y = libmcs_sin(u);

You can now run your model as usual or use code generation.

Verify that the Correct Function is Called
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you want to check whether the correct function is called, go to the library source code and edit the file ``libm/mathd/sind.c``. Change the sin function to return an arbitrary, but fixed value by inserting a ``return 99;`` statement right at the beginning of the function. After recompiling the library, as well as the MEX and S-functions, using the libmcs_sin function in Matlab and Simulink should then return ``99`` for all input values. This proves that the library's :ref:`sin` function is called and not the one from standard libm.

Ada
~~~

This section describes the best practice to use the library as part of the Ada programming language using the sine function as an example.

Create Bindings for Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For function bindings first create a thin binding that uses C-type input and output objects, then create a thick binding to convert the C-types to Ada-types. The following two example files contain some more description in the form of comments.

.. code-block:: ada

   with Interfaces.C;

   -- This package contains thin & thick bindings for the mathematical functions
   -- as defined in Ada LRM Annex A.5.1.
   package LIBMCS.Elementary_Functions is

   -- Declaration of the thick binding function that should
   -- be used to call 'sinf'. This function converts the Math_Float to the
   -- appropriate C types and calls 'c_sinf' that is declared in
   -- the private part of this package.
   function Sinf (X : Math_Float) return Math_Float;

   -- ... More functions see Ada LRM Annex A.5.1

   private
   use Interfaces.C;

   -- The thin binding declaration for 'sinf'.
   function c_sinf (X : c_float) return c_float;

   -- This import pragma tells the compiler to substitute calls to 'c_sinf'
   -- with appropriate calls to the C function 'sinf'
   -- This pragma could be an aspect in Ada2012.
   pragma Import (Convention => C,
   Entity => c_sinf,
   External_Name => "sinf");

   end LIBMCS.Elementary_Functions;

.. code-block:: ada

   -- Implementation file for the LIBMCS.Elementary_Functions package.
   package body LIBMCS.Elementary_Functions is
   use Interfaces.C;

   -- Thick binding for 'sinf'. This function converts the argument given as
   -- the native Ada type 'Math_Float' to the appropriate C type.
   -- This function can be responsible for additional range/boundary checking
   -- and raise exceptions if invalid values are provided.
   -- The function also converts the return value of 'sinf' from the C type to
   -- the native Ada type.
   function Sinf (X : Math_Float) return Math_Float is
   c_X  : c_float := c_float (X);
   Sine : c_float;
   begin
   Sine := c_sinf (c_X);
   return Math_Float (Sine);
   end Sinf;

   -- ... More functions see Ada LRM Annex A.5.1

   end LIBMCS.Elementary_Functions;

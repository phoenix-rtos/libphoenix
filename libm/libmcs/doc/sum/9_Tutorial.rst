Tutorial
========

This tutorial presents how to implement the library into user software, how to use the provided procedures, how to build it, and the execution's output.

The examples use an :ref:`RTEMS <ABBR>` :ref:`OS <ABBR>` (version 4.10, it includes :ref:`GCC <ABBR>` 4.4.6) provided by Cobham Gaisler including their :ref:`BSP <ABBR>`.

The user's experience will vary (possibly heavily) when using different hardware and/or :ref:`OS <ABBR>`.

It is assumed, that the library was built as shown in the :ref:`Operations Manual`.

Example 1: LEON2 with Meiko FPU
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first example is done for a LEON2 platform with a Meiko :ref:`FPU <ABBR>`. This :ref:`FPU <ABBR>` is perfectly able to work with subnormal numbers and does not throw any traps when executed with those.

Getting Started
^^^^^^^^^^^^^^^

The first important step is to include the library part we want to use:

.. code-block:: c

   #include "math.h"

The second step is to start the :ref:`FPU <ABBR>`. This step is :ref:`FPU <ABBR>` and :ref:`BSP <ABBR>` specific and simply enables the :ref:`FPU <ABBR>` in the :ref:`PSR <ABBR>`:

.. code-block:: c

   uint32_t psr;
   sparc_get_psr(psr);
   psr |= SPARC_PSR_EF_MASK;
   sparc_set_psr(psr);

The Meiko :ref:`FPU <ABBR>` has an :ref:`ISO <ABBR>` C18 standard conform handling of subnormal numbers, and the non-standard bit in the :ref:`FSR <ABBR>` cannot be set.

The final step is an actual call to some library procedures:

.. code-block:: c

   double input = M_PI_4;
   double result = sin(input);

   float inputX = 3.5f;
   float inputY = 1.2f;
   float resultf = fmodf(inputX, inputY);

Using all those snipets we can create a small test program. Sadly :ref:`RTEMS <ABBR>` and the :ref:`BSP <ABBR>` need some more configuring, but as the program would not run without them, we're going to show that as well. We've also added some ``printf`` to actually get a visible output (which one can only see if some type of console output is possible...):

.. code-block:: c

   #include <rtems.h>

   #include <bsp.h>

   #define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
   #define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

   #define CONFIGURE_RTEMS_INIT_TASKS_TABLE
   #define CONFIGURE_MAXIMUM_TASKS 1

   #define CONFIGURE_INIT
   #include <rtems/confdefs.h>

   #include <stdio.h>
   #include <stdlib.h>

   #include "math.h"

   void test()
   {
      double dbuf, ret;

      double d1 = 0.5, d2 = -2.0;
      float f1 = 0.5f, f2 = -2.0f;

      printf("sin(%lf)            = %lf\n", d1, sin(d1));
      printf("atan2(%lf, %lf)     = %lf\n", d1, d2, atan2(d1, d2));
      ret = modf(d1, &dbuf);
      printf("modf(%lf, %lf)      = %lf\n", d1, dbuf, ret);
      printf("isfinite(%lf)       = %d\n", d1, isfinite(d1));

      printf("cosf(%f)            = %f\n", f1, cosf(f1));
      printf("powf(%f, %f)        = %f\n", f1, f2, powf(f1, f2));
      printf("fmodf(%f, %f)       = %f\n", f1, f2, fmodf(f1, f2));
      printf("signbit(%f)         = %d\n", f1, signbit(f1));
   }

   void ConfigurationMultiprocessingEnableFpu()
   {
      uint32_t psr;
      sparc_get_psr(psr);
      psr |= SPARC_PSR_EF_MASK;
      sparc_set_psr(psr);
   }

   rtems_task Init(rtems_task_argument arg)
   {
      ConfigurationMultiprocessingEnableFpu();

      test();
      exit(0);
   }

Using the Software on a Typical Task
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Let's build the software using a shell command::

   $ sparc-rtems-gcc test test_rtems.c libmcs/build-sparc_v8/bin/libm.a -Ilibmcs/libm/include -qleon2 -frounding-math -fsignaling-nans -fno-builtin

As one can see the command builds with the library, includes the header files, uses the LEON2 specific flag ``-qleon2``, and uses the flags provided in :ref:`OperationsEnviromentSoftwareConfiguration`, before finally producing the ``test`` executable.

After running this program on the LEON2 platform it produces the following output on the console:

.. code-block:: none

   sin(0.500000)                 = 0.479426
   atan2(0.500000, -2.000000)    = 2.896614
   modf(0.500000, 0.000000)      = 0.500000
   isfinite(0.500000)            = 1

   cosf(0.500000)                = 0.877583
   powf(0.500000, -2.000000)     = 4.000000
   fmodf(0.500000, -2.000000)    = 0.500000
   signbit(0.500000)             = 0

Example 2: LEON4 with GRFPU
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The second example is done for a LEON4 platform with a GRFPU. This :ref:`FPU <ABBR>` has to be configured to prevent traps on subnormal numbers as described in :ref:`OperationsManualErrorConditions`.

Getting Started
^^^^^^^^^^^^^^^

The first important step is to include the library:

.. code-block:: c

   #include "math.h"

The second step is to start the :ref:`FPU <ABBR>`. This step is :ref:`FPU <ABBR>` and :ref:`BSP <ABBR>` specific and simply enables the :ref:`FPU <ABBR>` in the :ref:`PSR <ABBR>`:

.. code-block:: c

   uint32_t psr;
   sparc_get_psr(psr);
   psr |= SPARC_PSR_EF_MASK;
   sparc_set_psr(psr);

The third step is to set the :ref:`FPU <ABBR>` to non-standard mode (if wanted). This step is :ref:`FPU <ABBR>` and :ref:`BSP <ABBR>` specific and sets the appropriate bit in the :ref:`FSR <ABBR>`. The :ref:`BSP <ABBR>` did not provide direct access to the :ref:`FSR <ABBR>` therefore some asm-macros were needed:

.. code-block:: c

   #define GET_FSR(fsr) \
      asm volatile("st %%fsr, %[reg]" : [reg] "=g" (fsr) : "0" (fsr));
   #define SET_FSR(fsr) \
      asm volatile("ld %[reg], %%fsr" : : [reg] "g" (fsr));

   uint32_t fsr;
   GET_FSR(fsr);
   fsr |= (1 << 22);
   SET_FSR(fsr);

The final step is an actual call to some library procedures:

.. code-block:: c

   double input = M_PI_4;
   double result = sin(input);

   float inputX = 3.5f;
   float inputY = 1.2f;
   float resultf = fmodf(inputX, inputY);

Using all those snipets we can create a small test program. Sadly :ref:`RTEMS <ABBR>` and the :ref:`BSP <ABBR>` need some more configuring, but as the program would not run without them, we're going to show that as well. We've also added some ``printf`` to actually get a visible output (which one can only see if some type of console output is possible...):

.. code-block:: c

   #include <rtems.h>

   #include <bsp.h>

   #define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
   #define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

   #define CONFIGURE_RTEMS_INIT_TASKS_TABLE
   #define CONFIGURE_MAXIMUM_TASKS 1

   #define CONFIGURE_INIT
   #include <rtems/confdefs.h>

   #include <stdio.h>
   #include <stdlib.h>

   #include "math.h"

   void test()
   {
      double dbuf, ret;

      double d1 = 0.5, d2 = -2.0;
      float f1 = 0.5f, f2 = -2.0f;

      printf("sin(%lf)            = %lf\n", d1, sin(d1));
      printf("atan2(%lf, %lf)     = %lf\n", d1, d2, atan2(d1, d2));
      ret = modf(d1, &dbuf);
      printf("modf(%lf, %lf)      = %lf\n", d1, dbuf, ret);
      printf("isfinite(%lf)       = %d\n", d1, isfinite(d1));

      printf("cosf(%f)            = %f\n", f1, cosf(f1));
      printf("powf(%f, %f)        = %f\n", f1, f2, powf(f1, f2));
      printf("fmodf(%f, %f)       = %f\n", f1, f2, fmodf(f1, f2));
      printf("signbit(%f)         = %d\n", f1, signbit(f1));
   }

   #define GET_FSR(fsr) \
      asm volatile("st %%fsr, %[reg]" : [reg] "=g" (fsr) : "0" (fsr));

   #define SET_FSR(fsr) \
      asm volatile("ld %[reg], %%fsr" : : [reg] "g" (fsr));

   void ConfigurationMultiprocessingEnableFpu()
   {
      uint32_t psr;
      sparc_get_psr(psr);
      psr |= SPARC_PSR_EF_MASK;
      sparc_set_psr(psr);

      uint32_t fsr;
      GET_FSR(fsr);
      fsr |= (1 << 22);
      SET_FSR(fsr);
   }

   rtems_task Init(rtems_task_argument arg)
   {
      ConfigurationMultiprocessingEnableFpu();

      test();
      exit(0);
   }

Using the Software on a Typical Task
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Let's build the software using a shell command::

   $ sparc-rtems-gcc test test_rtems.c libmcs/build-sparc_v8/bin/libm.a -Ilibmcs/libm/include -qleon3mp -frounding-math -fsignaling-nans -fno-builtin

As one can see the command builds with the library, includes the header files, uses the LEON4 specific flag ``-qleon3mp`` (LEON3 and LEON4 use the same in this toolchain), and uses the flags provided in :ref:`OperationsEnviromentSoftwareConfiguration`, before finally producing the ``test`` executable.

After running this program on the LEON4 platform it produces the following output on the console:

.. code-block:: none

   sin(0.500000)                 = 0.479426
   atan2(0.500000, -2.000000)    = 2.896614
   modf(0.500000, 0.000000)      = 0.500000
   isfinite(0.500000)            = 1

   cosf(0.500000)                = 0.877583
   powf(0.500000, -2.000000)     = 4.000000
   fmodf(0.500000, -2.000000)    = 0.500000
   signbit(0.500000)             = 0

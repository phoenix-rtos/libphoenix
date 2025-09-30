Software Components Design – Aspects of Each Component
------------------------------------------------------

.. raw:: html

   <!--
   ### General
   a. This part of the DRD, as well as <5.5>, may be produced as the detailed design model of a tool, if agreed with the customer.

   ### <Component Identifier>
   a. Each component should have a unique identifier.
   b. The component should be named according to the rules of the programming language or operating system to be used.
   c. A hierarchical naming scheme should be used that identifies the parent of the component (e.g. ParentName_ChildName).

   ### <Type>
   a. Component type should be described by stating its logical and physical characteristics.
   b. The logical characteristics should be described by stating the package, library or class that the component belongs to.
   c. The physical characteristics should be described by stating the type of component, using the implementation terminology (e.g. task, subroutine, subprogram, package and file).
       NOTE: The contents of some components description clauses depend on the component type. For the purpose of this guide, the following categories are used: executable (i.e. contains computer instructions) or non–executable (i.e. contains only data).

   ### <Purpose>
   a. The purpose of a component should describe its trace to the software requirements that it implements.
       NOTE: Backward traceability depends upon each component description explicitly referencing the requirements that justify its existence.

   ### <Function>
   a. The function of a component shall be described in the software architectural design.
   b. The description specified in <5.4.5>a. should be done by stating what the component does.
       NOTE 1: The function description depends upon the component type. Therefore, it can be a description of the process.
       NOTE 2: Process descriptions can use such techniques as structured English, precondition–postcondition specifications and state–transition diagrams.

   ### <Subordinates>
   a. The subordinates of a component should be described by listing the immediate children.
       NOTE 1: The subordinates of a unit are the units that are ’called by’ it. The subordinates of a database can be the files that ’compose’ it.
       NOTE 2: The subordinates of an object are the objects that are ’used by’ it.

   ### <Dependencies>
   a. The dependencies of a component should be described by listing the constraints upon its use by other components.
       NOTE: Examples are:
           * Operations to take place before this component is called,
           * Operations that are excluded when this operation takes place.

   ### <Interfaces>
   a. Both control flow and data flow aspects of an interface shall be described for each “executable” component.
   b. Data aspects of ’non executable’ components should be described.
   c. The control flow to and from a component should be described in terms of how to start (e.g. subroutine call) and terminate (e.g. return) the execution of the component.
   d. If the information in <5.4.8>c. is implicit in the definition of the type of component, a description need not be done.
   e. If control flows take place during execution (e.g. interrupt), they should be described.
   f. The data flow input to and output from each component shall be described.
   g. It should be ensured that data structures:
       1. are associated with the control flow (e.g. call argument list);
       2. interface components through common data areas and files.

   ### <Resources>
   a. The resources’ needs of a component should be described by itemising what the component needs from its environment to perform its function.
       NOTE 1: Items that are part of the component interface are excluded.
       NOTE 2: Examples of resources’ needs of a component are displays, printers and buffers.

   ### <References>
   a. Explicit references should be inserted where a component description uses or implies material from another document.

   ### <Data>
   a. The data internal to a component should be described.
       NOTE: The amount of details to be provided depends strongly on the type of the component.
   b. The data structures internal to a program or subroutine should also be described.
   c. Data structure definitions shall include the:
       1. description of each element (e.g. name, type, dimension);
       2. relationships between the elements (i.e. the structure);
       3. range of possible values of each element;
       4. initial values of each element.
   -->

Classification Macros
"""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   0010_fpclassify.rst
   0020_isfinite.rst
   0030_isinf.rst
   0040_isnan.rst
   0050_isnormal.rst
   0060_signbit.rst

Trigonometric Functions
"""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   0100_acos.rst
   0110_asin.rst
   0120_atan.rst
   0130_atan2.rst
   0140_cos.rst
   0150_sin.rst
   0160_tan.rst
   0170_trig.rst

Hyperbolic Functions
""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   0200_acosh.rst
   0210_asinh.rst
   0220_atanh.rst
   0230_cosh.rst
   0240_sinh.rst
   0250_tanh.rst

Exponential and Logarithmic Functions
"""""""""""""""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   0300_exp.rst
   0310_exp2.rst
   0320_expm1.rst
   0330_frexp.rst
   0340_ilogb.rst
   0350_ldexp.rst
   0360_log.rst
   0370_log10.rst
   0380_log1p.rst
   0390_log2.rst
   0400_logb.rst
   0410_modf.rst
   0420_scalbn.rst
   0430_scalbln.rst
   0440_log_internal.rst

Power and Absolute-value Functions
""""""""""""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   0500_cbrt.rst
   0510_fabs.rst
   0520_hypot.rst
   0530_pow.rst
   0540_sqrt.rst

Error and Gamma Functions
"""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   0600_erf.rst
   0610_erfc.rst
   0620_lgamma.rst
   0630_tgamma.rst
   0640_gamma_internal.rst
   0650_signgam.rst

Nearest Integer Functions
"""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   0700_ceil.rst
   0710_floor.rst
   0720_nearbyint.rst
   0730_rint.rst
   0740_lrint.rst
   0750_llrint.rst
   0760_round.rst
   0770_lround.rst
   0780_llround.rst
   0790_trunc.rst

Remainder Functions
"""""""""""""""""""

.. toctree::
   :maxdepth: 1

   0900_fmod.rst
   0910_remainder.rst
   0920_remquo.rst

Manipulation Functions
""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   1000_copysign.rst
   1010_nan.rst
   1020_nextafter.rst
   1030_nexttoward.rst

Maximum, Minimum and Positive Difference Functions
""""""""""""""""""""""""""""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   1100_fdim.rst
   1110_fmax.rst
   1120_fmin.rst

Floating Multiply-Add
"""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   1200_fma.rst

Comparison Macros
"""""""""""""""""

.. toctree::
   :maxdepth: 1

   1300_isgreater.rst
   1310_isgreaterequal.rst
   1320_isless.rst
   1330_islessequal.rst
   1340_islessgreater.rst
   1350_isunordered.rst

Bessel Functions (POSIX)
""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   1400_j0.rst
   1410_j1.rst
   1420_jn.rst
   1430_y0.rst
   1440_y1.rst
   1450_yn.rst

Complex Trigonometric Functions
"""""""""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   1500_cacos.rst
   1510_casin.rst
   1520_catan.rst
   1530_ccos.rst
   1540_csin.rst
   1550_ctan.rst

Complex Hyperbolic Functions
""""""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   1600_cacosh.rst
   1610_casinh.rst
   1620_catanh.rst
   1630_ccosh.rst
   1640_csinh.rst
   1650_ctanh.rst

Complex Exponential and Logarithmic Functions
"""""""""""""""""""""""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   1700_cexp.rst
   1710_clog.rst

Complex Power and Absolute-value Functions
""""""""""""""""""""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   1800_cabs.rst
   1810_cpow.rst
   1820_csqrt.rst

Complex Manipulation Functions
""""""""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   1900_carg.rst
   1910_cimag.rst
   1920_cmplx.rst
   1930_conj.rst
   1940_cproj.rst
   1950_creal.rst

Miscellaneous Internal Functions
""""""""""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   2000_misc_internal.rst

Build and Configuration Files
"""""""""""""""""""""""""""""

.. toctree::
   :maxdepth: 1

   3000_configure.rst
   3100_makefile.rst

General
-------

.. raw:: html

   <!--
   a. The SDD shall describe the software architectural design.
   b. The architecture structure of the software item shall be described, identifying the software components, their hierarchical relationships, any dependency and interfaces between them.
   c. For flight software, the design shall reflect in flight modification requirements.
   d. The structure in <5.2> to <5.5> should be used.
   -->

The herein described software is a mathematical library, not designed for independent execution, but for embedding into other software projects. This chapter describes the software design in a top-bottom approach: First in :ref:`Overall Architecture` a common overview over the structure of the software is given. The section afterwards :ref:`Software Components Design - General` identifies each component in the software. All components are described in :ref:`Software Components Design – Aspects of Each Component`. The last section :ref:`Internal Interface Design` gives an overview about the internal usage between the components.

The library is a reuse of the mathematical library contained in ``newlib`` called ``libm``, specifically we use the ``libm`` contained in ``newlib`` v4.0.0.

In the process of improving the reused software several design and implementation changes have to be made to the library.

This document follows in structure and content the :ref:`DRD <ABBR>` in ECSS-E-ST-40 :ref:`[AD01] <AD>`, but because of the type of the reused software some required items have to be redefined for an appropriate design description or are not relevant. The reinterpreting/redefining of items is done at the start of each section. All items not relevant in this context are mentioned at the end of each section to be compliant with :ref:`ECSS <ABBR>`.

The source code is designed to be compliant to the mandatory and required rules depicted in MISRA C:2012 :ref:`[AD04] <AD>`.

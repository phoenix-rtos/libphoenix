Software Design Overview
========================

Software Static Architecture
----------------------------

.. raw:: html

   <!--
   a. The SDD shall describe the architecture of the software item, as well as the main relationship with the major components identified.
   b. The SDD shall also describe any system state or mode in which the software operates.
   c. The SDD shall describe the separated mission and configuration data.
       NOTE: Data can be classified in the following categories:
           * data resulting from the mission analysis and which thus vary from one mission to another;
           * reference data which are specific to a family of software product;
           * reference data which never change;
           * data depending only on the specific mission requirements (e.g. calibration of sensors);
           * data required for the software operation which only vary the higher level system design (in which is embedded the software) is changed;
   -->

The mathematical library is a software library. The library is a fully passive library and provides solutions for mathematical procedures to the calling software. As such it can be seen as a replacement of the
``libm`` standard library. The procedures the library implements are listed in :ref:`Overall Architecture`.

The library has no separate operational modes or states, it simply provides the above mentioned procedures.

Further the library does not store data of any kind.

Software Dynamic Architecture
-----------------------------

.. raw:: html

   <!--
   a. The SDD shall describe the design choices to cope with the real time constraints (e.g. selection and description of the computational model).
   -->

The library has no real-time constraints, all procedures provided calculate a response right away. Each procedure however has a bounded execution time (refer to :ref:`SRS <ABBR>` :ref:`[RD01] <RD>` §5.3).

Software Behaviour
------------------

As previously noted, the library’s behaviour is that of a passive library. It never executes code of any kind without being explicitly called by another program.

Interfaces Context
------------------

.. raw:: html

   <!--
   a. The SDD shall identify all the external interfaces or refer to the ICD.
   b. The description in <4.4>a. should be based on system block diagram or context diagram to illustrate the relationship between this system and other systems.
   -->

No :ref:`ICD <ABBR>` will be provided for this project and its contents are part of the :ref:`SUM <ABBR>` and this :ref:`SDD <ABBR>`. All of the :ref:`ICDs <ABBR>` contents required by :ref:`ECSS <ABBR>` are part of these two documents without any loss.

Section :ref:`Software Components Design – Aspects of Each Component` provides information on the function signatures of all library procedures. The library is meant to be included in other software projects.

The interface provided is that of a C library that can be compiled to an object file to be statically or dynamically linked to the calling software.

The library and all its components are compliant to the IEEE 754-2019 (Information technology – Microprocessor Systems – Floating-Point arithmetic), IEEE 1003.1-2017 (POSIX) and ISO/IEC 9899 (Programming languages – C) standards.

Long Lifetime Software
----------------------

.. raw:: html

   <!--
   a. The SDD shall describe the design choices to cope with the long planned lifetime of the software, in particular minimum dependency on the operating system and the hardware to improve portability.
   -->

The library does not cause restrictions regarding the longevity of the operating software. It does not depend on a particular operating system or hardware setup.

Memory and CPU Budget
---------------------

.. raw:: html

   <!--
   a. The SDD shall document and summarize the allocation of memory and processing time to the software components.
   -->

The library uses 100% of available processing time while executing a procedure and 0% while it doesn’t. This is due to the library not having any tasks that run continuously or periodically. It simply executes one function, returns its result and stops.

The static memory needed for the library differs depending on the used toolchain and on which procedures are used by the user (due to ``gc-sections`` flags to the compiler). Still the library targets to use
as little static memory as possible while providing its functionality. The library’s allocation of dynamic memory is marginal - each procedure only requires several bytes to save intermediate variables. This however
is only stack memory, no ``malloc`` is used, therefore no memory will be allocated from heap.

Design Standards, Conventions and Procedures
--------------------------------------------

.. raw:: html

   <!--
   a. The SDD shall summarize (or reference in the SDP) the software methods adopted for the architectural and the detailed design.
       NOTE: A design method offers often the following characteristics:
           * decomposition of the software architecture in design objects having integral parts that communicate with each other and with the outside environment
           * explicit recognition of typical activities of real‐time systems (i.e. cyclic and sporadic threads, protected resources)
           * integration of appropriate scheduling paradigms with the design process
           * explicit definition of the application timing requirements for each activity
           * static verification of processor allocation, schedulability and timing analysis
           * consistent code generation
   b. The following information shall be summarized:
       1. software architectural design method;
       2. software detailed design method;
       3. code documentation standards;
       4. naming conventions;
       5. programming standards;
       6. intended list of reuse components
       7. main design trade‐off.
   -->

The main design method of this project is ‘reuse’, therefore the design of the functions highly depends on the already existing implementation, which in turn causes this :ref:`SDD <ABBR>` to be more of a description of an existing design rather than a design produced before implementation. However the procedures and their designs may be improved during this project, turning the design method into ‘reuse with improvements’.

Source code documentation is done using the restructured text format for file, procedure and variable documentation, it will then be interpreted by Sphinx to generate HTML/PDF documents. Additionally the software engineer may use further comments within the code to describe parts of their implementation.

All aspects regarding the design that are not explicitly stated shall remain as they were in newlib’s libm.

The library will guarantee thread safety and reentrancy for all procedures unless explicitly stated otherwise. The only exception will be the ``lgamma`` procedures due to the requirement to have the global variable ``signgam``, as such the content of the variable is not thread safe when ``lgamma`` procedures are executed in multiple threads at the same time.

We define the set of all floating-point numbers as :math:`\mathbb{F}` and the set of all subnormal numbers as :math:`\mathbb{S}` to be used in mathematical formulae. The set of floating-point numbers :math:`\mathbb{F}` contains all floating-point numbers, that is normals, subnormals (therefore :math:`\mathbb{S} \subset \mathbb{F}`), zeroes, infinities and ``NaN``, of the type given by the context. If a reduction to a specific type is needed, this will be indicated as follows: :math:`\mathbb{F}_s` for single precision, :math:`\mathbb{F}_d` for double precision, and :math:`\mathbb{F}_{ld}` for long double precision. The same will be done for :math:`\mathbb{S}` with :math:`\mathbb{S}_s`, :math:`\mathbb{S}_d`, and :math:`\mathbb{S}_{ld}` respectively. Furthermore :math:`\mathbb{F}^{+}` symbolizes all floating-point numbers where the sign bit is not set, while :math:`\mathbb{F}^{-}` contains all those where the sign bit is set (this includes :math:`-0.0`, ``-Inf``, and ``NaNs`` where the signbit is set). Using these sets we are able to express floating-point numbers in mathematical formulae, for example the formula :math:`x \in \mathbb{F} \setminus \left \{ \pm \text{Inf}, \text{NaN} \right \}` denotes that `x` is a floating-point number that is neither infinity nor ``NaN``, which leaves `x` to be either zero, a normal or a subnormal number. :math:`\mathbb{F}` cannot be fully represented by conventional number classifications due to the introduction of :math:`\pm 0.0`, explicit infinities and ``NaNs``, however the following is valid: :math:`(\mathbb{F} \setminus \left \{ -0.0, \pm \text{Inf}, \text{NaN} \right \}) \subset \mathbb{Q}`.

Similarly we define the set of all 32bit integer numbers as :math:`\mathbb{I}`. :math:`\mathbb{I}` is a subset of :math:`\mathbb{Z}` limited by the size of the integer, therefore :math:`\mathbb{I} \subset \mathbb{Z}`. The same concept applies for :math:`\mathbb{I}_{l}` which represents either 32bit or 64bit integer numbers depending on the size of ``long int``, and :math:`\mathbb{I}_{ll}` which represents 64bit integer numbers.

To denote the similarity, but not always equality, between the mathematical function and its representing procedure we use :math:`\approx` where appropriate. This is not used if a procedure is able to exactly mimic the functionality of the mathematical function.

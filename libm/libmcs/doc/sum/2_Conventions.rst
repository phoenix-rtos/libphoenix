Conventions
===========

This chapter lists the symbols, stylistic conventions, and command syntax used in this document. Each convention is identified by a name and described by an example and the additional
explanation.

.. table:: Notation conventions
  :width: 100%

  +---------------------------+---------------------------+----------------------------------------------------------+
  | Convention                | Example                   | Explanation                                              |
  +===========================+===========================+==========================================================+
  | Pi                        | :math:`\pi`               | The mathematical constant :math:`\pi`                    |
  +---------------------------+---------------------------+----------------------------------------------------------+
  | Not a Number              | NaN or ``NaN``            | Representation for "not a number"-value                  |
  +---------------------------+---------------------------+----------------------------------------------------------+
  | Quiet NaN                 | qNaN, ``qNaN``            | Representation for "quiet not a number"-value            |
  +---------------------------+---------------------------+----------------------------------------------------------+
  | Signalling NaN            | sNaN, ``sNaN``            | Representation for "signalling not a number"-value       |
  +---------------------------+---------------------------+----------------------------------------------------------+
  | Signed NaN                | +NaN or -NaN              | Representation for "not a number"-value, that explicitly |
  |                           |                           | has the signbit unset/set. This definition does not      |
  |                           |                           | exist within the IEEE-754 standard and is added for      |
  |                           |                           | better readability of the table                          |
  +---------------------------+---------------------------+----------------------------------------------------------+
  | Infinity                  | +/-Inf or ±Inf            | Representation for an Infinity value                     |
  +---------------------------+---------------------------+----------------------------------------------------------+
  | File                      | ``k_sin.c``               | Indication of a code file                                |
  +---------------------------+---------------------------+----------------------------------------------------------+
  | Source Code               | ``sin(double x)``         | Indication of code                                       |
  +---------------------------+---------------------------+----------------------------------------------------------+
  | Constants                 | ``M_PI``, ``FLT_MAX``     | Indication of code                                       |
  +---------------------------+---------------------------+----------------------------------------------------------+

We define the set of all floating-point numbers as :math:`\mathbb{F}` and the set of all subnormal numbers as :math:`\mathbb{S}` to be used in mathematical formulae. The set of floating-point numbers :math:`\mathbb{F}` contains all floating-point numbers, that is normals, subnormals (therefore :math:`\mathbb{S} \subset \mathbb{F}`), zeroes, infinities and ``NaN``, of the type given by the context. If a reduction to a specific type is needed, this will be indicated as follows: :math:`\mathbb{F}_s` for single precision, :math:`\mathbb{F}_d` for double precision, and :math:`\mathbb{F}_{ld}` for long double precision. The same will be done for :math:`\mathbb{S}` with :math:`\mathbb{S}_s`, :math:`\mathbb{S}_d`, and :math:`\mathbb{S}_{ld}` respectively. Furthermore :math:`\mathbb{F}^{+}` symbolizes all floating-point numbers where the sign bit is not set, while :math:`\mathbb{F}^{-}` contains all those where the sign bit is set (this includes :math:`-0.0`, ``-Inf``, and ``NaNs`` where the signbit is set). Using these sets we are able to express floating-point numbers in mathematical formulae, for example the formula :math:`x \in \mathbb{F} \setminus \left \{ \pm \text{Inf}, \text{NaN} \right \}` denotes that `x` is a floating-point number that is neither infinity nor ``NaN``, which leaves `x` to be either zero, a normal or a subnormal number. :math:`\mathbb{F}` cannot be fully represented by conventional number classifications due to the introduction of :math:`\pm 0.0`, explicit infinities and ``NaNs``, however the following is valid: :math:`(\mathbb{F} \setminus \left \{ -0.0, \pm \text{Inf}, \text{NaN} \right \}) \subset \mathbb{Q}`.

Similarly we define the set of all 32bit integer numbers as :math:`\mathbb{I}`. :math:`\mathbb{I}` is a subset of :math:`\mathbb{Z}` limited by the size of the integer, therefore :math:`\mathbb{I} \subset \mathbb{Z}`. The same concept applies for :math:`\mathbb{I}_{l}` which represents either 32bit or 64bit integer numbers depending on the size of ``long int``, and :math:`\mathbb{I}_{ll}` which represents 64bit integer numbers.

To denote the similarity, but not always equality, between the mathematical function and its representing procedure we use :math:`\approx` where appropriate. This is not used if a procedure is able to exactly mimic the functionality of the mathematical function.

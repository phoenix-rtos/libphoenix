# Phoenix math library implementation
This directory contains implementation of math library. It currently links to libphoenix with intention to be compiled to separate libm.a library. There are two implementation for two following reasons:
- libphoenix implementation is not complete.
- version for critical systems is nice to have :)

Usage: For each project it is suggested to set following
- ```LIBM_USE_LIBMCS := y/n``` - select implementation of math library for critical systems (if not set will use phoenix implementation)
- ```LIBM_WANT_COMPLEX := y/n``` - select if project wants implementation of complex routines
- ```LIBM_LIBMCS_DAZ := y/n``` - (applicable only for libmcs) select whether denormals are zero. This is especially useful if underlying FPU has limited functionalities implemented.

## LIBMCS
This is a copy of libmcs math library implementation from https://gitlab.com/gtd-gmbh/libmcs (commit: 79a3235511f01644139c95249487729621667249)

For documentation please refer to /docs directory present on the main repo linked above. 

## Phoenix 
This is implementation of math library developed alongside Pheonix RTOS operating system. Keep in mind that this implementation is not complete, not all functions in C99 standard are implemented. Since headers are shared for both implementations, and libmcs implementation contains complete set of C99 function, there might be some undefined symbols when linking projects that use this implementation in libphoenix. Not present functions will be added if such need arises in the future.

# Notes
1. This is a temporary state, moving math library to be separate to libphoenix
2. libmcs - has no ARM optimization for ```sqrt``` function
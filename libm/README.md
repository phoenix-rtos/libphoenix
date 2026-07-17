# Libm
This directory contains implementation of math library. It currently links to libphoenix with intention to be compiled to separate libm.a library. There are two implementations for the following cases:
- libphoenix implementation is not complete.
- version for critical systems is nice to have :)

Usage: For each project it is suggested to set following
- ```LIBM_USE_LIBMCS ?= y/n``` - select implementation of math library for critical systems (if not set will use phoenix implementation)
- ```LIBM_WANT_COMPLEX ?= y/n``` - select if project wants implementation of complex routines
- ```LIBM_LIBMCS_DAZ ?= y/n``` - (applicable only for libmcs) select whether denormals are zero. This is especially useful if underlying FPU has limited functionalities implemented.
- ```LIBM_USE_HW ?= y``` - enable hardware intrinsics for math functions

# Architecture specific code
Some targets might support instructions that implement parts of `math.h` required functions. Such implementation may be placed `arch/<target_name>`, and they will take precedence before software implementation, unless stated otherwise via appropriate flags. Please refer to already existing code if new architecture specific implementation is to be added. 

# LIBMCS
This is a copy of libmcs math library implementation from https://gitlab.com/gtd-gmbh/libmcs (v1.3.1)

For documentation please refer to /docs directory present on the main repo linked above. 

# Phoenix 
This is implementation of math library developed alongside Pheonix RTOS operating system. Keep in mind that this implementation is not complete, not all functions in C99 standard are implemented.

# Notes
1. This is a temporary state, moving math library to be separate to libphoenix
2. Some hardware instrinsics for math functions are still not implemented 














# LibmCS

This is the mathematical library for critical systems (LibmCS).

It has been developed in compliance with the European Cooperation for Space Standardization (ECSS) standards E-ST-40 and Q-ST-80 to criticality Category B and it is also compliant to the standards:

- IEEE-754,
- C18,
- POSIX, and
- MISRA C

The library has been developed by GTD GmbH under the ESA Contract No. 4000130278/20/NL/AS.

The copyright and licensing condition of the library can be found in the COPYING.md file.


## Getting Started

### Documentation Entry Point

If you are a software architect interested in assessing the possibility to include the LibmCS in your software project you may start reading the:

- Software Release Document (OR-SRelD.00-ML)

If you are a software product assurance engineer interested in the aspects for the delta-qualification of the LibmCS for your specific project you may start reading the:

- Qualification Guideline (OP-QG.00-ML)

If you are a software engineer interested in using the library in your software project you can continue reading the following section and taking a look at the Software User Manual available in this repository.

*Note*: The OR-SRelD.00-ML and OP-QG.00-ML documents are accessible through the [ESA ESSR (European Space Software Repository)](https://essr.esa.int/) site, which is accessible to all ESA member states (search this site for LibmCS). The documents and the qualification-kit can also be obtained through GTD GmbH (contact libmcs@gtd-gmbh.de).

### Programmer's Entry Point

To start using the LibmCS in your software project you can clone this repository and start configuring and compiling the library:

```
> ./configure
> make
```

#### Using Configure With Flags

For CI and similar activities it can be useful to not use an interactive script for the configuration as such it is also possible to call `configure` with flags which answer the questions otherwise asked interactively (if only some flags are set, the remaining questions will still be asked). The following flags can be set:

| Configure Flag                     | Description                                                                                                                                                                 |
|------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `--cross-compile <CROSS_COMPILE>`  | Tells `make` which toolchain to use.                                                                                                                                        |
| `--compilation-flags <CFLAGS>`     | Used do define additional flags to the compiler. Takes both user and library specific flags/defines, e.g. `-mcpu=...`.                                                          |
| `--enable-denormal-handling`       | The library will assume the platform does not provide denormals and use the platform to convert denormal inputs/outputs to 0.                                               |
| `--disable-denormal-handling`      | The library will assume the platform does provide denormals properly.                                                                                                       |
| `--enable-long-double-procedures`  | Should be set if the size of `long double` is 64 bit and the procedures shall be provided.                                                                                  |
| `--disable-long-double-procedures` | Should be set if the size of `long double` is not 64 bit or the procedures shall not be provided. If this is set, the library will not have `long double` procedures built. |
| `--enable-complex-procedures`      | If this is set complex procedures are available.                                                                                                                            |
| `--disable-complex-procedures`     | If this is set the library will not have complex procedures built.                                                                                                          |
| `--big-endian`                     | Tells the compilation that variables are in big endian.                                                                                                                     |
| `--little-endian`                  | Tells the compilation that variables are in little endian.                                                                                                                  |

Of special note here is also that the library needs to know which endianness the platform uses (big endian for SPARC processors and little usually for the rest). Most compilers use the define `__BYTE_ORDER__` with either `__ORDER_BIG_ENDIAN__` or `__ORDER_LITTLE_ENDIAN__` set, but if your toolchain does not provide this define you have to do it yourself (e.g. the EDISOFT RTEMS does not).

##### Some Examples

1. For a LEON2 processor and the EDISOFT RTEMS 4.8 tool-chain, without `long double` functions nor complex functions, the configuration would look like this:

```
> ./configure --cross-compile /opt/rtems-4.8/bin/sparc-rtems4.8- --compilation-flags "-mcpu=leon" --enable-denormal-handling --disable-long-double-procedures --disable-complex-procedures --big-endian
```

2. For a LEON3/LEON4 processor and an OAR RTEMS 4.11 tool-chain, without `long double` functions nor complex functions, the configuration would look like this:

```
> ./configure --cross-compile /opt/rtems-4.11-2016.04.01.FPU/bin/sparc-rtems4.11- --compilation-flags "-mcpu=leon3" --disable-denormal-handling --disable-long-double-procedures --disable-complex-procedures --big-endian
```

3. For an ARM R52 processor and a certain GCC cross compiler,  without `long double` functions nor complex functions, the configuration would look like this:

```
> ./configure --cross-compile <CROSS_COMPILER_PREFIX> --compilation-flags "-mcpu=cortex-r52" --enable-denormal-handling --disable-long-double-procedures --disable-complex-procedures --little-endian
```

4. For a LEON3/LEON4 processor and an RTEMS 6 UNI/SMP tool-chain, without `long double` functions nor complex functions, the configuration would look like this:

```
> ./configure --cross-compile /opt/rtems/rtems-6-sparc-gr712rc-smp-6/bin/sparc-rtems6- --compilation-flags "-mcpu=leon3" --disable-denormal-handling --disable-long-double-procedures --disable-complex-procedures --big-endian
```


#### Test Coverage

Test coverage is not set using `configure`, it can be enabled by adding `COVERAGE=1` to the `make` call. Note that `configure` still has to be run first before `make` also for this case.

#### Make Targets

| Make Target | Description                                                                                                                        |
|-------------|------------------------------------------------------------------------------------------------------------------------------------|
| `all`       | Builds the library. This is the default target.                                                                                    |
| `debug`     | Builds the library. Currently equivalent to `all`.                                                                                 |
| `release`   | Builds the library. Equivalent to `all` with the additional flag `-DNDEBUG` set.                                                   |
| `clean`     | Removes the build directory of the library of the current ARCH. (ARCH can be set manually or is extracted from using the compiler) |
| `cleanall`  | Removes all build directories of the library.                                                                                      |
| `distclean` | Removes all files created by 'configure' and all builds.                                                                           |

Note: LibmCS does not have an `install` target.

#### Using Hardware Instructions

To replace LibmCS procedures with custom procedures, such as hardware instruction calling procedures (e.g., using the CPU hardware implementation of procedures such as square root), several steps need to be followed:

+ Add the file containing the procedure into an architecture specific sub-directory within the `libm/machine` directory. If the directory does not exist, make one for your architecture. An example for this step has already been added to the library for the SPARC V8 architecture.
+ Replace the file you want to replace in the `Makefile.in`. As an example for this step check the branch `makefile-sparc-v8-hardware-sqrt` where this is done for the SPARC V8 `sqrt` procedure.

Note that replacing procedures may cause different results and in turn may cause unit tests to fail. You will have to justify those fails, or change the respective tests and justify that change.


## How to Contribute

To learn how to contribute to the LibmCS please read the CONTRIBUTING.md file.

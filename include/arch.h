#if defined(__i386__) || defined(__x86_64__)
#include <arch/ia32/arch.h>
#elif defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
#include <arch/armv7m/arch.h>
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_8A__) || defined(__ARM_ARCH_7__)
#include <arch/armv7a/arch.h>
#elif defined(__ARM_ARCH_4T__) || defined(__ARM_ARCH_5TE__) /* not currently supported, map to 7M for libgcc to compile */
#include <arch/armv7m/arch.h>
#elif defined(__ARM_ARCH_8R__)
#include <arch/armv8r/arch.h>
#elif defined(__ARM_ARCH_8M_BASE__) || defined(__ARM_ARCH_8M_MAIN__)
#include <arch/armv8m/arch.h>
#elif defined(__aarch64__)
#include <arch/aarch64/arch.h>
#elif defined(__riscv) && (__riscv_xlen == 64)
#include <arch/riscv64/arch.h>
#elif defined(__sparc__)
#include <arch/sparcv8leon/arch.h>
#else
#error "unsupported architecture"
#endif

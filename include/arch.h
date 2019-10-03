#if defined(__i386__) || defined(__x86_64__)
#include <arch/ia32/arch.h>
#elif defined(__ARM_ARCH_7A__)
#include <arch/arm-imx/arch.h>
#elif defined(__ARM_ARCH_7M__)
#include <arch/armv7/arch.h>
#elif defined(__riscv) && (__riscv_xlen == 64)
#include <arch/riscv64/arch.h>
#else
#error "unsupported architecture"
#endif

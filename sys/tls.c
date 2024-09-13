#include <stdlib.h>
#include <sys/threads.h>
#include <sys/auxv.h>
#include <stdint.h>
#include <sys/tls.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>
#include <assert.h>
#include <stdlib.h>


static size_t tlsSize;
static size_t tdataSz;
static size_t tlsAlign;
/* NOTE: Provided by linker. Can't use info from header as data can be detached from code. */
extern char __tdata_start;


int __tls_alloc(struct tls_tcb **tcbp)
{
	*tcbp = NULL;

	if (tlsSize == 0) {
		return 0;
	}

	void *ptr = calloc(1, sizeof(struct tls_tcb) + tlsSize + tlsAlign);
	if (ptr == NULL) {
		return -1;
	}

	struct tls_tcb *tcb;

#ifdef __LIBPHOENIX_ARCH_TLS_VARIANT_I
	tcb = (struct tls_tcb *)ptr;
	ptr += sizeof(struct tls_tcb);
#elif defined(__LIBPHOENIX_ARCH_TLS_VARIANT_II)
	tcb = (struct tls_tcb *)(ptr + tlsSize + tlsAlign);
	tcb->tcb_self = tcb;
	ptr = (uint8_t *)tcb - tlsSize;
#endif

	memcpy(ptr, &__tdata_start, tdataSz);

	*tcbp = tcb;

	return 0;
}


void __tls_release(void)
{
	if (tlsSize == 0) {
		return;
	}

	void *tcb = (void *)__tls_getTcb();
	assert(tcb != NULL); /* If size != 0 tcb cannot be NULL */

#ifdef __LIBPHOENIX_ARCH_TLS_VARIANT_I
	free(tcb);
#elif defined(__LIBPHOENIX_ARCH_TLS_VARIANT_II)
	free(tcb - tlsSize - tlsAlign);
#endif
}


void _tls_init(void)
{
	uint64_t phdrV = getauxval(AT_PHDR);
	uint64_t phnum = getauxval(AT_PHNUM);

	if ((phdrV == 0) || (phnum == 0)) {
		/* Assume no TLS. */
		return;
	}

	const Elf_Phdr *phdr = (const Elf_Phdr *)(unsigned long)phdrV;

	for (uint64_t i = 0; i < phnum; i++) {
		if (phdr[i].p_type == PT_INTERP) {
			/* Dynamic linker takes care of TLS init. */
			return;
		}
		if (phdr[i].p_type == PT_TLS) {
			Elf_Off misalign = phdr[i].p_offset & (phdr[i].p_align - 1);

			tdataSz = phdr[i].p_filesz;
			tlsSize = phdr[i].p_memsz + misalign;
#ifdef __LIBPHOENIX_ARCH_TLS_VARIANT_II
			/* Members of tls_tcb have to be aligned. */
			tlsAlign = (sizeof(void *) - (tlsSize & (sizeof(void *) - 1))) & (sizeof(void *) - 1);
#endif
			break;
		}
	}

	if (tlsSize == 0) {
		return;
	}

	__tls_archInit();

	struct tls_tcb *tcb;

	if (__tls_alloc(&tcb) != 0) {
		_exit(126);
	}

	tlsSetPtr(__tls_tcbPtrToTlsPtr(tcb));
}

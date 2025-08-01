#ifndef _LIBPHOENIX_MYMUTEX_H_
#define _LIBPHOENIX_MYMUTEX_H_

#include <stdint.h>

typedef struct {
	_Atomic(uint32_t) futex;
} mymutex_t;

void mm_lock(mymutex_t *mm);
void mm_unlock(mymutex_t *mm);


#define MM_INIT(mm) \
	do { \
		memset((mm), 0, sizeof(*(mm))); \
	} while (0)

#endif

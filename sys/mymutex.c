#include <stdatomic.h>
#include <sys/mymutex.h>
#include <sys/futex.h>

void mm_lock(mymutex_t *mm)
{
	uint32_t unlocked = 0;
	while (!atomic_compare_exchange_weak(&mm->futex, &unlocked, 1)) {
		unlocked = 0;
		phFutexWait(&mm->futex, 1, 0);
	}
}

void mm_unlock(mymutex_t *mm)
{
	uint32_t locked = 1;
	if (atomic_compare_exchange_strong(&mm->futex, &locked, 0)) {
		phFutexWakeup(&mm->futex, 1);
	}
}

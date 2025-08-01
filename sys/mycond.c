#include <stdatomic.h>
#include <stdint.h>
#include <errno.h>
#include <sys/mymutex.h>
#include <sys/futex.h>
#include <sys/mycond.h>

void mc_wait(mycond_t *mc, mymutex_t *mm)
{
	uint32_t value = atomic_load_explicit(&mc->value, memory_order_seq_cst);
	while (value & 1) {
		if (atomic_compare_exchange_strong_explicit(
					&mc->value,
					&value,
					value + 1,
					memory_order_seq_cst,
					memory_order_seq_cst)) {
			value++;
		}
	}
	mm_unlock(mm);
	int ret;
	do {
		ret = phFutexWait(&mc->value, value, 0);
	} while (ret == -EINTR);
	mm_lock(mm);
}

void _mc_signal_n(mycond_t *mc, uint32_t n)
{
	atomic_fetch_or_explicit(&mc->value, 1, memory_order_seq_cst);
	phFutexWakeup(&mc->value, n);
}

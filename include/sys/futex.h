#ifndef _SYS_FUTEX_H_
#define _SYS_FUTEX_H_

int futex_wait(int *address, int value, time_t timeout);
int futex_wakeup(int *address, int value, int n_threads);

#endif

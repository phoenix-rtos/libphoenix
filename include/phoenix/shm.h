#ifndef PHOENIX_SHM
#define PHOENIX_SHM

#include "sys/types.h"


#define SHM_PATH    "/dev/posix/shm/"
#define SHMCTL_PATH "/dev/posix/shmctl"
#define SHM_NAME_MAX    256


#ifdef __cplusplus
extern "C" {
#endif


int shm_open(const char *name, int oflag, ...);


int shm_unlink(const char *name);


#ifdef __cplusplus
}
#endif


#endif

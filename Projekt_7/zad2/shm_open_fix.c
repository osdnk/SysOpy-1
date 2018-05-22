#include <stdlib.h>
#include <string.h>

/* This avoids a segfault when code using shm_open()
   is compiled statically. (For some reason, compiling
   the code statically causes the __shm_directory()
   function calls in librt.a to not reach the implementation
   in libpthread.a. Implementing the function ourselves
   fixes this issue.)
*/

#ifndef  SHM_MOUNT
#define  SHM_MOUNT "/dev/shm/"
#endif
static const char  shm_mount[] = SHM_MOUNT;

const char *__shm_directory(size_t *len)
{
    if (len)
        *len = strlen(shm_mount);
    return shm_mount;
}
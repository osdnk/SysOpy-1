
#define _POSIX_C_SOURCE 199309L
#include "props.h"
#include <time.h>

const int base_sem_count = 4;
const char shm_name[32] = "/kremowka";

long get_time() {
  struct timespec time_point;
  if (clock_gettime(CLOCK_MONOTONIC, &time_point) == -1) exit(3);
  return time_point.tv_nsec / 1000;
}
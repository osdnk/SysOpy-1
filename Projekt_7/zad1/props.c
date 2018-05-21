#define _POSIX_C_SOURCE 199309L
#include "props.h"
#include <time.h>

const int program_id = 7312;
const int shmem_id = 2137;
const int base_sem_count = 4;

void block_critical_frame(struct sembuf *operation, int sem_set_id) {
  operation->sem_num = 3;
  operation->sem_op = -1;
  semop(sem_set_id, operation, 1);
}

void unblock_critical_frame(struct sembuf *operation, int sem_set_id) {
  operation->sem_op = 1;
  operation->sem_num = 3;
  semop(sem_set_id, operation, 1);
}

long get_time() {
  struct timespec time_point;
  if (clock_gettime(CLOCK_MONOTONIC, &time_point) == -1) exit(3);
  return time_point.tv_nsec / 1000;
}
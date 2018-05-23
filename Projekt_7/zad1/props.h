#ifndef PROPS_H
#define PROPS_H

#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>

extern const int program_id;
extern const int shmem_id;
extern const int base_sem_count;
union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

typedef enum state {
  BARBER_SLEEPING = 0,
  BARBER_AWAKEN = 1,
} state;

typedef struct client_data {
  pid_t pid;
  long enter_time;
} client_data;

typedef struct shop_state {
  int queued_clients;
  int lenght;
  int begin;
  int end;
  int sem_set_id;
  state barber_state;
  int is_barber_open;
  int total_sem_count;
  client_data chair;
  client_data shop_queue[2137];
} shop_state;
void block_critical_frame(struct sembuf *operation, int sem_set_id);
void unblock_critical_frame(struct sembuf *operation, int sem_set_id);
long get_time();

#endif
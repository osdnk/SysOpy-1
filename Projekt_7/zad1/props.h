#ifndef PROPS_H
#define PROPS_H

#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

const int program_id = 7312;
const int shmem_id = 2137;
const int base_sem_count = 4;

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

typedef enum who { BARBER, CLINET } who;

typedef enum state {
  BARBER_SLEEPING = 0,
  BARBER_AWAKEN = 1,
  CLIENT_BEING_BARBED = 2
} state;

typedef struct client_data {
  pid_t pid;
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

void block_critical_frame(struct sembuf *operation, int sem_set_id) {
  // fprintf(stderr, "blocking state, %d,%d\n", getpid(),
  //         semctl(sem_set_id, 3, GETVAL));
  operation->sem_op = -1;
  operation->sem_num = 3;
  semop(sem_set_id, operation, 1);
  // fprintf(stderr, "blocking state, %d,%d\n", getpid(),
  //         semctl(sem_set_id, 3, GETVAL));
}

void unblock_critical_frame(struct sembuf *operation,int sem_set_id) {
  operation->sem_op = 1;
  operation->sem_num = 3;
  semop(sem_set_id, operation, 1);
}

#endif

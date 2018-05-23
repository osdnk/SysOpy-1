#ifndef PROPS_H
#define PROPS_H

#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern const char shm_name[32];
extern const int base_sem_count;

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
  char sem_set_names[1028][32];
  state barber_state;
  int is_barber_open;
  int total_sem_count;
  client_data chair;
  client_data shop_queue[1024];
} shop_state;

long get_time();

#endif
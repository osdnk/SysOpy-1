#include "props.h"

void init_semaphores();
void init_memory();

int shmemory_id;
int sem_set_id;
key_t barb_sem_set_key;
key_t shm_key;
void *queue_addres;
shop_state *shp_state;
struct sembuf client_things;

int main(int argc, char **argv) {
  if (argc < 1) exit(1);

  int barb_need = strtol(argv[1], NULL, 10);

  init_semaphores();
  init_memory();

  for (int i = 0; i < barb_need; i++) {
    if (semctl(sem_set_id, 1, GETVAL) == 0) {
      fprintf(stderr, "clinet: %d: queue is full exiting\n", getpid());
    }

    client_things.sem_num = 1;
    client_things.sem_op = -1;
    semop(sem_set_id, &client_things, 1);
    fprintf(stderr, "clinet: %d: entered the shop\n", getpid());

    block_critical_frame(&client_things, sem_set_id);

    if (shp_state->barber_state == BARBER_SLEEPING) {
      fprintf(stderr, "clinet: %d: meeting sleeping barber\n", getpid());

      fprintf(stderr, "clinet: %d sitting on the chair\n", getpid());

      client_data client;
      client.pid = getpid();
      client.enter_time = get_time();
      shp_state->chair = client;

      client_things.sem_num = 0;  // barber semaphore
      client_things.sem_op = 1;

      fprintf(stderr, "clinet: %d: aweking barber\n", getpid());
      semop(sem_set_id, &client_things, 1);
      shp_state->barber_state = BARBER_AWAKEN;
      unblock_critical_frame(&client_things, sem_set_id);

      client_things.sem_num = 2;
      client_things.sem_op = -1;

      if (semop(sem_set_id, &client_things, 1) == -1) {
        fprintf(stderr, "semop error\n");
        fprintf(stderr, "%s\n", strerror(errno));
        exit(2137);
      }
      fprintf(stderr, "clinet: %d: leaving shop as barbed person\n", getpid());

    } else {
      fprintf(stderr, "clinet: %d: sitting in the queue pos: %d\n", getpid(),
              shp_state->end);
      // move end of the queue by one
      int end = shp_state->end;
      client_data client;
      client.pid = getpid();
      client.enter_time = get_time();
      shp_state->shop_queue[end] = client;
      // add queued client
      shp_state->queued_clients += 1;
      shp_state->end = (end + 1) % shp_state->lenght;

      // unblock frame
      unblock_critical_frame(&client_things, sem_set_id);

      // block client in the queue
      client_things.sem_op = -1;
      client_things.sem_num = end + base_sem_count;
      semop(sem_set_id, &client_things, 1);

      fprintf(stderr, "clinet: %d sitting on the chair\n", getpid());

      shp_state->chair = client;

      client_things.sem_op = -1;
      client_things.sem_num = 2;
      semop(sem_set_id, &client_things, 1);
      fprintf(stderr, "clinet: %d: leaving shop as barbed person\n", getpid());
    }
  }
  fprintf(stderr, "clinet: %d: fully barbed\n", getpid());
}

void init_semaphores() {
  barb_sem_set_key = ftok(getenv("HOME"), program_id);
  sem_set_id = semget(barb_sem_set_key, 0, 0);

  if (sem_set_id == -1) exit(2);

  client_things.sem_num = 1;
  client_things.sem_op = -1;
  client_things.sem_flg = 0;
}

void init_memory() {
  shm_key = ftok(getenv("HOME"), shmem_id);

  shmemory_id = shmget(shm_key, 0, 0666);

  queue_addres = shmat(shmemory_id, NULL, 0);
  if (queue_addres == (void *)-1) {
    fprintf(stderr, "attaching failed");
    exit(40);
  }
  shp_state = (shop_state *)queue_addres;
}

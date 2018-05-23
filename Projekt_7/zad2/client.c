#include "props.h"

void init_memory();
void init_semaphores();

shop_state *shp_state = NULL;
sem_t *sem_set_addreses[1028];

int main(int argc, char const *argv[]) {
  if (argc < 1) exit(1);

  int barb_need = strtol(argv[1], NULL, 10);

  init_memory();
  init_semaphores();
  int sem_value = -1;

  for (int i = 0; i < barb_need; i++) {
    if (sem_getvalue(sem_set_addreses[1], &sem_value) == -1) {
      perror("sem_getvalue error: ");
    }
    if (sem_value == 0) {
      fprintf(stderr, "clinet: %d: queue is full exiting\n", getpid());
    }
    sem_wait(sem_set_addreses[1]);

    sem_wait(sem_set_addreses[3]);
    if (shp_state->barber_state == BARBER_SLEEPING) {
      fprintf(stderr, "clinet: %d: meeting sleeping barber\n", getpid());

      fprintf(stderr, "clinet: %d sitting on the chair\n", getpid());

      client_data client;
      client.pid = getpid();
      client.enter_time = get_time();
      shp_state->chair = client;

      fprintf(stderr, "clinet: %d: aweking barber\n", getpid());
      shp_state->barber_state = BARBER_AWAKEN;
      sem_post(sem_set_addreses[0]);
      sem_post(sem_set_addreses[3]);
      sem_wait(sem_set_addreses[2]);
      fprintf(stderr, "clinet: %d: leaving shop as barbed person\n", getpid());
    } else {
      fprintf(stderr, "clinet: %d: sitting in the queue pos: %d\n", getpid(),
              shp_state->end);

      sem_post(sem_set_addreses[0]);// inform barber "im in queue fucker"

      int end = shp_state->end;
      client_data client;
      client.pid = getpid();
      client.enter_time = get_time();
      shp_state->shop_queue[end] = client;
      // add queued client
      shp_state->queued_clients += 1;
      shp_state->end = (end + 1) % shp_state->lenght;
      sem_post(sem_set_addreses[3]);
      sem_wait(sem_set_addreses[end + base_sem_count]);
      sem_wait(sem_set_addreses[2]);
      fprintf(stderr, "clinet: %d sitting on the chair\n", getpid());
      shp_state->chair = client;

      fprintf(stderr, "clinet: %d: leaving shop as barbed person\n", getpid());
    }
    fprintf(stderr, "clinet: %d: fully barbed\n", getpid());
  }

  return 0;
}

void init_memory() {
  int shm_id = shm_open(shm_name, O_RDWR, 0666);
  if (shm_id == -1) {
    perror("shm_open error: ");
    exit(3);
  }
  void *queue_addres =
      mmap(NULL, sizeof(shop_state), PROT_READ | PROT_WRITE | PROT_EXEC,
           MAP_SHARED, shm_id, 0);
  if (queue_addres == MAP_FAILED) {
    perror("mmap error: ");
    exit(40);
  }
  shp_state = (shop_state *)queue_addres;
}

void init_semaphores() {
  for (int i = 0; i < shp_state->lenght + base_sem_count; i++) {
    sem_set_addreses[i] = sem_open(shp_state->sem_set_names[i], O_RDWR);
    if (sem_set_addreses[i] == SEM_FAILED) {
      perror("sem_open error: ");
      exit(10);
    }
  }
}

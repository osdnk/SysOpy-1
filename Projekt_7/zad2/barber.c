#define _DEFAULT_SOURCE
#include "props.h"

void sig_term_handler(int);
void clean_up(void);
void init_semaphores();
void init_memory();
void barb_client();
client_data get_client_from_the_queue();

int total_sem_count = -1;
int queue_length = -1;
int shm_id = -1;
shop_state *shp_state = NULL;
sem_t *sem_set_addreses[1028];

int main(int argc, char const *argv[]) {
  atexit(clean_up);
  signal(SIGINT, sig_term_handler);
  signal(SIGTERM, sig_term_handler);

  if (argc < 2) exit(1);

  queue_length = strtol(argv[1], NULL, 10);
  total_sem_count = queue_length + base_sem_count;
  init_memory(); 
  init_semaphores();

  while (shp_state->is_barber_open) {
    sem_wait(sem_set_addreses[3]);
    int value = -1;
    sem_getvalue(sem_set_addreses[0], &value);
    if (shp_state->barber_state == BARBER_AWAKEN && value == 0) {
      shp_state->barber_state = BARBER_SLEEPING;
      fprintf(stderr, "barber falls asslep\n");
    }
    sem_post(sem_set_addreses[3]);

    sem_wait(sem_set_addreses[0]);

    if (shp_state->barber_state == BARBER_SLEEPING) {
      fprintf(stderr, "barber awakens!!!\n");
    }

    barb_client();
  }

  return 0;
}

client_data get_client_from_the_queue() {
  fprintf(stderr, "get client form: %d seat in queue\n", shp_state->begin);

  sem_wait(sem_set_addreses[3]);
  client_data client = shp_state->shop_queue[shp_state->begin];
  shp_state->begin = (shp_state->begin + 1) % shp_state->lenght;
  shp_state->queued_clients -= 1;
  sem_post(sem_set_addreses[3]);

  sem_post(sem_set_addreses[shp_state->begin + base_sem_count]);
  return client;
}

void barb_client() {
  client_data client = shp_state->chair;

  if (client.pid == -1) {
    client = get_client_from_the_queue();
    fprintf(stderr, "picked client %d form the queue\n", client.pid);
  }
  sem_post(sem_set_addreses[2]);
  fprintf(stderr, "barbed client: %d, queue enter time: %ld\n", client.pid,
          client.enter_time);

  shp_state->chair.pid = -1;
  sem_post(sem_set_addreses[1]);
}

void sig_term_handler(int signum) { exit(signum); }

void init_memory() {
  shm_id = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
  if (shm_id == -1) {
    perror("shm_open error: ");
    exit(3);
  }
  if (ftruncate(shm_id, sizeof(shop_state)) == -1) {
    perror("ftruncate error: ");
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

  client_data client = {-1, -1};
  shp_state->is_barber_open = 1;
  shp_state->barber_state = BARBER_AWAKEN;
  shp_state->begin = shp_state->end = 0;
  shp_state->lenght = queue_length;
  shp_state->queued_clients = 0;
  shp_state->total_sem_count = total_sem_count;
  shp_state->chair = client;

  strcpy(shp_state->sem_set_names[0], "/barber");
  strcpy(shp_state->sem_set_names[1], "/queue_door");
  strcpy(shp_state->sem_set_names[2], "/chair");
  strcpy(shp_state->sem_set_names[3], "/crit_sect");

  for (int i = base_sem_count; i < total_sem_count; i++)
    sprintf(shp_state->sem_set_names[i], "/%d", i);
  for (int i = 0; i < queue_length; i++) shp_state->shop_queue[i] = client;

}

void init_semaphores() {
  sem_set_addreses[0] =
      sem_open(shp_state->sem_set_names[0], O_CREAT | O_EXCL | O_RDWR, 0666, 0);
  sem_set_addreses[1] = sem_open(shp_state->sem_set_names[1],
                                 O_CREAT | O_EXCL | O_RDWR, 0666, queue_length);
  sem_set_addreses[2] =
      sem_open(shp_state->sem_set_names[2], O_CREAT | O_EXCL | O_RDWR, 0666, 0);
  sem_set_addreses[3] =
      sem_open(shp_state->sem_set_names[3], O_CREAT | O_EXCL | O_RDWR, 0666, 1);
  for (int i = base_sem_count; i < total_sem_count; i++) {
    sem_set_addreses[i] = sem_open(shp_state->sem_set_names[i],
                                   O_CREAT | O_EXCL | O_RDWR, 0666, 0);
  }
}

void clean_up(void) {
  for (int i = 0; i < total_sem_count; i++) {
    sem_close(sem_set_addreses[i]);
    sem_unlink(shp_state->sem_set_names[i]);
  }
  munmap(shp_state, sizeof(shop_state));
  shm_unlink(shm_name);
}

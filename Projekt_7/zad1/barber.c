#include "props.h"

void sig_term_handler(int);
int is_queue_empty();
client_data get_client_from_the_queue();
void barb_client();
void update_state();
void init_semaphores();
void init_memory();
void clean_up(void);

int shmemory_id;
int sem_set_id;
shop_state *shp_state = NULL;
int total_sem_count = -1;
int queue_length = -1;
struct sembuf barber_actions;

int main(int argc, char **argv) {
  atexit(clean_up);
  signal(SIGINT, sig_term_handler);
  signal(SIGTERM, sig_term_handler);

  if (argc < 2) exit(1);

  queue_length = strtol(argv[1], NULL, 10);
  total_sem_count = queue_length + base_sem_count;

  init_semaphores();
  init_memory();

  while (shp_state->is_barber_open) {
    block_critical_frame(&barber_actions, sem_set_id);
    if (shp_state->queued_clients > 0) {
      fprintf(stderr, "currently in queue: %d\n", shp_state->queued_clients);
      semctl(sem_set_id, 0, SETVAL, shp_state->queued_clients);
    } else if (shp_state->barber_state != BARBER_SLEEPING &&
               semctl(sem_set_id, 0, GETVAL) == 0) {
      shp_state->barber_state = BARBER_SLEEPING;
      fprintf(stderr, "barber falls asslep\n");
    }
    unblock_critical_frame(&barber_actions, sem_set_id);

    barber_actions.sem_num = 0;
    barber_actions.sem_op = -1;
    if (semop(sem_set_id, &barber_actions, 1) == -1) {
      fprintf(stderr, "semop barber failure, %s\n", strerror(errno));
      exit(30);
    }

    if (shp_state->barber_state == BARBER_SLEEPING) {
      fprintf(stderr, "barber awakens!!!\n");
    }

    barb_client();
  }
}

client_data get_client_from_the_queue() {
  struct sembuf cls;
  cls.sem_num = shp_state->begin + base_sem_count;
  cls.sem_op = 1;
  cls.sem_flg = 0;

  fprintf(stderr, "get client form: %d\n", shp_state->begin);

  client_data client = shp_state->shop_queue[shp_state->begin];
  shp_state->begin = (shp_state->begin + 1) % shp_state->lenght;
  shp_state->queued_clients -= 1;

  semop(sem_set_id, &cls, 1);
  return client;
}

void barb_client() {
  struct sembuf cls;

  cls.sem_flg = 0;

  client_data client = shp_state->chair;

  if (client.pid == -1) {
    client = get_client_from_the_queue();
    fprintf(stderr, "picked client %d form the queue\n", client.pid);
  }

  // fprintf(stderr, "barbing clinet %d\n", client.pid);
  shp_state->chair.pid = -1;
  cls.sem_num = 2;
  cls.sem_op = 1;
  if (semop(sem_set_id, &cls, 1) == -1) exit(1);
  fprintf(stderr, "barbed client: %d, queue enter time: %ld\n", client.pid,
          client.enter_time);

  cls.sem_num = 1;
  semop(sem_set_id, &cls, 1);
}

void sig_term_handler(int signum) { exit(signum); }

void clean_up(void) {
  fprintf(stderr, "exit clean up %d, %d\n", shmemory_id, sem_set_id);
  shmdt(shp_state);
  shmctl(shmemory_id, IPC_RMID, NULL);
  semctl(sem_set_id, 0, IPC_RMID);
}

void init_memory() {
  key_t shm_key = ftok(getenv("HOME"), shmem_id);
  if (sem_set_id == -1) exit(2);

  int shm_size = sizeof(shop_state);
  shmemory_id = shmget(shm_key, shm_size, IPC_CREAT | IPC_EXCL | 0666);
  void *queue_addres = shmat(shmemory_id, NULL, 0);

  if (queue_addres == (void *)-1) {
    fprintf(stderr, "attaching failed\n");
    exit(40);
  }
  shp_state = (shop_state *)queue_addres;

  client_data client = {-1, -1};

  shp_state->is_barber_open = 1;
  shp_state->barber_state = BARBER_AWAKEN;
  shp_state->begin = shp_state->end = 0;
  shp_state->lenght = queue_length;
  shp_state->queued_clients = 0;
  shp_state->sem_set_id = sem_set_id;
  shp_state->total_sem_count = total_sem_count;
  shp_state->chair = client;

  for (int i = 0; i < queue_length; i++) shp_state->shop_queue[i] = client;
}

void init_semaphores() {
  key_t sem_set_key = ftok(getenv("HOME"), program_id);
  sem_set_id =
      semget(sem_set_key, total_sem_count, IPC_CREAT | IPC_EXCL | 0666);

  if (sem_set_id == -1) exit(2);

  semctl(sem_set_id, 0, SETVAL, 0);             // barber sleeper
  semctl(sem_set_id, 1, SETVAL, queue_length);  // can acces to the queue
  semctl(sem_set_id, 2, SETVAL, 0);             // can sit on the chair
  semctl(sem_set_id, 3, SETVAL, 1);             // can edit state

  fprintf(stderr, "%d\n", semctl(sem_set_id, 3, GETVAL));

  for (int i = base_sem_count; i < total_sem_count; i++) {
    semctl(sem_set_id, i, SETVAL, 0);  // sit in the queue
  }

  barber_actions.sem_num = 0;
  barber_actions.sem_op = -1;
  barber_actions.sem_flg = 0;
}

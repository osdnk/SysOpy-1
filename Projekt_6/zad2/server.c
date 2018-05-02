#define _GNU_SOURCE

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "params.h"

typedef struct tuple {
  char q_name[MAX_NAME_LEN];
  int q_id;
} tuple;

void remove_queue();
void int_sig_handler(int signum);
void server_response(char *buff, tuple clients[MAX_CLIENT_COUNT],
                     int *clients_count);
void login_handler(char *buff, tuple clients[MAX_CLIENT_COUNT],
                   int *clients_count);
void mirror_handler(char *buff, tuple clients[MAX_CLIENT_COUNT],
                    int *clients_count);
void time_handler(char *buff, tuple clients[MAX_CLIENT_COUNT],
                  int *clients_count);
void calc_handler(char *buff, tuple clients[MAX_CLIENT_COUNT],
                  int *clients_count);
void end_handler();

mqd_t server_queue_id;
int server_active = 1;

tuple clients[MAX_CLIENT_COUNT];
int clients_count = 0;

int main(int agrv, char **args) {
  atexit(remove_queue);
  signal(SIGINT, int_sig_handler);

  // [0] - pid and client id [1] - client queue id
  struct mq_attr queue_atr;
  queue_atr.mq_maxmsg = MAX_Q_SIZE;
  queue_atr.mq_msgsize = MAX_MSG_SIZE;

  server_queue_id =
      mq_open(PROGRAM_ID, O_RDWR | O_CREAT | O_EXCL, 0666, &queue_atr);
      
  char *buff = malloc(sizeof(char) * MAX_MSG_SIZE);

  while (server_active) {
    mq_receive(server_queue_id, buff, MAX_MSG_SIZE, NULL);
    server_response(buff, clients, &clients_count);
  }

  free(buff);
}

mqd_t get_queue_id(char *buff, tuple clients[], int *clients_count) {
  if (buff[1] != -1) return clients[(int)buff[1]].q_id;
  return -1;
}

char *get_queue_name(char *buff, tuple clients[], int *clients_count) {
  if (buff[1] != -1) return clients[(int)buff[1]].q_name;
  return NULL;
}

void remove_queue() {
  for (int i = 0; i < clients_count; i++) {
    mq_close(clients[i].q_id);
  }
  mq_close(server_queue_id);
  mq_unlink(PROGRAM_ID);
}

void int_sig_handler(int _) { exit(2); }

void end_handler(char *buff, tuple clients[], int *clients_count) {
  server_active = 0;
  mq_close(server_queue_id);
  for (int i = 0; i < (*clients_count); i++) mq_unlink(clients[i].q_name);
  for (int i = 0; i < (*clients_count); i++)
    kill(strtol(clients[i].q_name + 1, NULL, 10), SIGINT);
}

void mirror_handler(char *buff, tuple clients[MAX_CLIENT_COUNT],
                    int *clients_count) {
  int queue_id = get_queue_id(buff, clients, clients_count);
  mq_send(queue_id, buff + 2, MAX_MSG_SIZE, 3);
}

void login_handler(char *buff, tuple clients[MAX_CLIENT_COUNT],
                   int *clients_count) {
  mqd_t queue_id = buff[1];
  if (queue_id != -1) return;
  if ((*clients_count) < MAX_CLIENT_COUNT) {
    sprintf(clients[(*clients_count)].q_name, "%s", buff + 2);
    clients[(*clients_count)].q_id = mq_open(buff + 2, O_WRONLY);

    buff[0] = (*clients_count);
    queue_id = clients[(*clients_count)].q_id;
    fprintf(stderr, "login: %d\n", queue_id);
    mq_send(queue_id, buff, MAX_MSG_SIZE, 3);
    (*clients_count) += 1;
  }
}

void time_handler(char *buff, tuple clients[MAX_CLIENT_COUNT],
                  int *clients_count) {
  mqd_t queue_id = get_queue_id(buff, clients, clients_count);
  char time_buff[32];
  time_t time_raw;
  time(&time_raw);
  struct tm *timeinfo = localtime(&time_raw);
  strftime(time_buff, 32, "%b %d %H:%M", timeinfo);
  sprintf(buff, "%s", time_buff);
  fprintf(stderr, "time: %d\n", queue_id);
  mq_send(queue_id, buff, MAX_MSG_SIZE, 3);
}

void calc_handler(char *buff, tuple clients[MAX_CLIENT_COUNT],
                  int *clients_count) {
  mqd_t queue_id = get_queue_id(buff, clients, clients_count);
  char great_command[4128];
  sprintf(great_command, "echo '%s' | bc", buff + 2);
  FILE *done_math = popen(great_command, "r");
  fgets(buff, MAX_MSG_SIZE, done_math);
  pclose(done_math);
  mq_send(queue_id, buff, MAX_MSG_SIZE, 3);
}

void server_response(char *buff, tuple clients[MAX_CLIENT_COUNT],
                     int *clients_count) {
  fprintf(stderr, "%s\n", buff + 2);
  switch (buff[0]) {
    case MIRROR:
      fprintf(stderr, "server_resonse mirror\n");
      mirror_handler(buff, clients, clients_count);
      break;
    case END:
      fprintf(stderr, "server_resonse end\n");
      end_handler(buff, clients, clients_count);
      break;
    case CALC:
      fprintf(stderr, "server_resonse calc\n");
      calc_handler(buff, clients, clients_count);
      break;
    case TIME:
      fprintf(stderr, "server_resonse time\n");
      time_handler(buff, clients, clients_count);
      break;
    case LOGIN:
      fprintf(stderr, "server_resonse login\n");
      login_handler(buff, clients, clients_count);
      break;
  }
}

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

void remove_queue();
void int_sig_handler(int signum);
void ask_login(int* client_id, mqd_t* server_queue_id);
void ask_time(int* client_id, mqd_t* server_queue_id);
void ask_mirror(int* client_id, mqd_t* server_queue_id);
void ask_calc(int* client_id, mqd_t* server_queue_id);
void ask_end(mqd_t* server_queue_id);

mqd_t client_queue_id;
char qname[MAX_NAME_LEN];

int main(int agrv, char** argc) {
  atexit(remove_queue);
  signal(SIGINT, int_sig_handler);
  int const buff_size = MAX_NAME_LEN;
  int client_id = -1;

  sprintf(qname, "/%d", getpid());
  fprintf(stderr, "%s\n", qname);

  mqd_t server_queue_id = mq_open(PROGRAM_ID, O_WRONLY);
  fprintf(stderr, "%d\n", server_queue_id);

  struct mq_attr queue_atr;
  queue_atr.mq_maxmsg = MAX_Q_SIZE;
  queue_atr.mq_msgsize = MAX_MSG_SIZE;

  client_queue_id = mq_open(qname, O_RDWR | O_CREAT | O_EXCL, 0666, &queue_atr);
  fprintf(stderr, "%d\n", client_queue_id);

  char buff[buff_size];

  ask_login(&client_id, &server_queue_id);
  int keep_asking = 1;

  while (keep_asking) {
    fgets(buff, buff_size, stdin);
    if (buff[strlen(buff) - 1] == '\n') buff[strlen(buff) - 1] = '\0';
    if (strcmp("end", buff) == 0) {
      ask_end(&server_queue_id);
      keep_asking = 0;
    } else if (strcmp("mirror", buff) == 0) {
      ask_mirror(&client_id, &server_queue_id);
    } else if (strcmp("calc", buff) == 0) {
      ask_calc(&client_id, &server_queue_id);
    } else if (strcmp("time", buff) == 0) {
      ask_time(&client_id, &server_queue_id);
    } else if (strcmp("q", buff) == 0) {
      break;
    } else
      fprintf(stderr, "WRONG COMMAND\n");
  }
}

void remove_queue() {
  mq_close(client_queue_id);
  mq_unlink(qname);
}

void int_sig_handler(int signum) { exit(2); }

void ask_login(int* client_id, mqd_t* server_queue_id) {
  char* msg = malloc(sizeof(char) * MAX_MSG_SIZE);
  msg[0] = LOGIN;
  msg[1] = (*client_id);
  sprintf((msg + 2), "%s", qname);
  mq_send((*server_queue_id), msg, MAX_MSG_SIZE, 3);
  mq_receive(client_queue_id, msg, MAX_MSG_SIZE, NULL);
  (*client_id) = msg[0];
  free(msg);
}

void ask_time(int* client_id, mqd_t* server_queue_id) {
  char* msg = malloc(sizeof(char) * MAX_MSG_SIZE);
  msg[0] = TIME;
  msg[1] = (*client_id);
  msg[2] = '\0';
  mq_send((*server_queue_id), msg, MAX_MSG_SIZE, 3);
  mq_receive(client_queue_id, msg, MAX_MSG_SIZE, NULL);
  fprintf(stderr, "%s\n", msg);
  free(msg);
}

void ask_mirror(int* client_id, mqd_t* server_queue_id) {
  char* msg = malloc(sizeof(char) * MAX_MSG_SIZE);
  msg[0] = MIRROR;
  msg[1] = (*client_id);
  char buff[255];
  fgets(buff, MAX_MSG_SIZE, stdin);
  sprintf((msg + 2), "%s", buff);
  mq_send((*server_queue_id), msg, MAX_MSG_SIZE, 3);
  mq_receive(client_queue_id, msg, MAX_MSG_SIZE, NULL);
  fprintf(stderr, "%s\n", msg);
  free(msg);
}

void ask_calc(int* client_id, mqd_t* server_queue_id) {
  char* msg = malloc(sizeof(char) * MAX_MSG_SIZE);
  msg[0] = CALC;
  msg[1] = (*client_id);
  char buff[255];
  fgets(buff, MAX_MSG_SIZE, stdin);
  sprintf((msg + 2), "%s", buff);
  mq_send((*server_queue_id), msg, MAX_MSG_SIZE, 3);
  mq_receive(client_queue_id, msg, MAX_MSG_SIZE, NULL);
  fprintf(stderr, "%s\n", msg);
  free(msg);
}

void ask_end(mqd_t* server_queue_id) {
  char* msg = malloc(sizeof(char) * MAX_MSG_SIZE);
  msg[0] = END;
  mq_send((*server_queue_id), msg, MAX_MSG_SIZE, 3);
  free(msg);
}

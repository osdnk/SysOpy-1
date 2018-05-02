#define _GNU_SOURCE

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>

#include "params.h"

#define MAX_CLIENT_COUNT 255

void remove_queue();
void int_sig_handler(int signum);
void server_response(message *buff, int clients[MAX_CLIENT_COUNT][2],
                     int *clients_count);
void login_handler(message *buff, int clients[MAX_CLIENT_COUNT][2],
                   int *clients_count);
void mirror_handler(message *buff, int clients[MAX_CLIENT_COUNT][2],
                    int *clients_count);
void time_handler(message *buff, int clients[MAX_CLIENT_COUNT][2],
                  int *clients_count);
void calc_handler(message *buff, int clients[MAX_CLIENT_COUNT][2],
                  int *clients_count);
void end_handler();

int server_id;
int server_active = 1;

int main(int agrv, char **args) {
  atexit(remove_queue);
  signal(SIGINT, int_sig_handler);

  // [0] - pid and client id [1] - client queue id
  int clients[MAX_CLIENT_COUNT][2];
  int clients_count = 0;
  struct msqid_ds server_queue_state;

  key_t key = ftok(getenv("HOME"), PROGRAM_ID);

  server_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
  fprintf(stderr, "%d\n", server_id);
  message buff;

  while (1) {
    if (server_active == 0) {
      if (msgctl(server_id, IPC_STAT, &server_queue_state) == -1)
        exit(9);
      if (server_queue_state.msg_qnum == 0)
        break;
    }
    sleep(1);
    msgrcv(server_id, &buff, message_size, 0, 0);
    server_response(&buff, clients, &clients_count);
  }
}

void remove_queue() { msgctl(server_id, IPC_RMID, NULL); }

void int_sig_handler(int _) { exit(2); }

void end_handler() { server_active = 0; }

int get_queue_id(message *msg_buff, int clients[][2], int *clients_count) {

  if (msg_buff->id != -1)
    return clients[msg_buff->id][1];
  int queue_id = -1;
  for (int i = 0; i < (*clients_count); i++) {
    if (clients[i][0] == msg_buff->pid) {
      queue_id = clients[i][1];
      break;
    }
  }
  return queue_id;
}

void mirror_handler(message *buff, int clients[MAX_CLIENT_COUNT][2],
                    int *clients_count) {
  int queue_id = get_queue_id(buff, clients, clients_count);
  buff->pid = getpid();
  msgsnd(queue_id, buff, message_size, 0);
}

void login_handler(message *buff, int clients[MAX_CLIENT_COUNT][2],
                   int *clients_count) {
  fprintf(stderr, "buff->pid: %d\n", buff->pid);
  int queue_id;
  if ((*clients_count) < MAX_CLIENT_COUNT) {
    clients[(*clients_count)][0] = buff->pid;
    clients[(*clients_count)][1] = strtol(buff->msg, NULL, 10);

    buff->id = (*clients_count);
    sprintf(buff->msg, "ok");
    queue_id = clients[(*clients_count)][1];
    msgsnd(queue_id, buff, message_size, 0);

    (*clients_count) += 1;
  }
}

void time_handler(message *buff, int clients[MAX_CLIENT_COUNT][2],
                  int *clients_count) {
  char time_buff[32];
  time_t time_raw;
  time(&time_raw);
  struct tm *timeinfo = localtime(&time_raw);
  strftime(time_buff, 32, "%b %d %H:%M", timeinfo);
  sprintf(buff->msg, "%s", time_buff);

  int queue_id = get_queue_id(buff, clients, clients_count);
  msgsnd(queue_id, buff, message_size, 0);
}

void calc_handler(message *buff, int clients[MAX_CLIENT_COUNT][2],
                  int *clients_count) {

  int queue_id = get_queue_id(buff, clients, clients_count);

  char great_command[4128];
  sprintf(great_command, "echo '%s' | bc", buff->msg);
  FILE *done_math = popen(great_command, "r");
  fgets(buff->msg, MAX_MSG_SIZE, done_math);
  pclose(done_math);
  msgsnd(queue_id, buff, message_size, 0);
}

void server_response(message *buff, int clients[MAX_CLIENT_COUNT][2],
                     int *clients_count) {
  if (buff == NULL)
    return;
  fprintf(stderr, "server response\t");
  switch (buff->mtype) {
  case MIRROR:
    fprintf(stderr, "mirror\n");
    mirror_handler(buff, clients, clients_count);
    break;
  case END:
    fprintf(stderr, "end\n");
    end_handler();
    break;
  case CALC:
    fprintf(stderr, "calc\n");
    calc_handler(buff, clients, clients_count);
    break;
  case TIME:
    fprintf(stderr, "time\n");
    time_handler(buff, clients, clients_count);
    break;
  case LOGIN:
    fprintf(stderr, "login\n");
    login_handler(buff, clients, clients_count);
    break;
  }
}

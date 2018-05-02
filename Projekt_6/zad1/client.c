
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

void remove_queue();
void int_sig_handler(int signum);
void ask_login(int *client_id, int server_queue_id);
void ask_time(int *client_id, int server_queue_id);
void ask_mirror(int *client_id, int server_queue_id);
void ask_calc(int *client_id, int server_queue_id);
void ask_end(int server_queue_id);

int client_queue_id;

int main(int agrv, char **argc) {
  atexit(remove_queue);
  signal(SIGINT, int_sig_handler);

  int const buff_size = 255;
  int client_id = -1;
  key_t key = ftok(getenv("HOME"), getpid());

  client_queue_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
  int server_queue_id = msgget(ftok(getenv("HOME"), PROGRAM_ID), 0);
  fprintf(stderr, "%d\n", server_queue_id);
  char buff[buff_size];

  ask_login(&client_id, server_queue_id);

  int keep_asking = 1;

  while (keep_asking) {
    fgets(buff, buff_size, stdin);
    if (buff[strlen(buff) - 1] == '\n')
      buff[strlen(buff) - 1] = '\0';
    if (strcmp("end", buff) == 0) {
      ask_end(server_queue_id);
      keep_asking = 0;
    } else if (strcmp("mirror", buff) == 0) {
      ask_mirror(&client_id, server_queue_id);
    } else if (strcmp("calc", buff) == 0) {
      ask_calc(&client_id, server_queue_id);
    } else if (strcmp("time", buff) == 0) {
      ask_time(&client_id, server_queue_id);
    } else
      fprintf(stderr, "WRONG COMMAND\n");
  }
}

void remove_queue() { msgctl(client_queue_id, IPC_RMID, NULL); }

void int_sig_handler(int signum) { exit(2); }

void ask_login(int *client_id, int server_queue_id) {
  message buff;
  buff.mtype = LOGIN;
  buff.pid = getpid();
  buff.id = (*client_id);
  sprintf(buff.msg, "%d\n", client_queue_id);
  msgsnd(server_queue_id, &buff, message_size, 0);
  msgrcv(client_queue_id, &buff, message_size, 0, 0);
  (*client_id) = buff.id;
}
void ask_time(int *client_id, int server_queue_id) {
  message buff;
  buff.mtype = TIME;
  buff.id = (*client_id);
  msgsnd(server_queue_id, &buff, message_size, 0);
  msgrcv(client_queue_id, &buff, message_size, 0, 0);
  fprintf(stdout, "%s\n", buff.msg);
}
void ask_mirror(int *client_id, int server_queue_id) {
  message buff;
  buff.mtype = MIRROR;
  buff.id = (*client_id);
  fgets(buff.msg, MAX_MSG_SIZE, stdin);
  msgsnd(server_queue_id, &buff, message_size, 0);
  msgrcv(client_queue_id, &buff, message_size, 0, 0);
  fprintf(stdout, "%s\n", buff.msg);
}
void ask_calc(int *client_id, int server_queue_id) {
  message buff;
  buff.mtype = CALC;
  fgets(buff.msg, MAX_MSG_SIZE, stdin);
  msgsnd(server_queue_id, &buff, message_size, 0);
  msgrcv(client_queue_id, &buff, message_size, 0, 0);
  fprintf(stdout, "%s\n", buff.msg);
}
void ask_end(int server_queue_id) {
  message buff;
  buff.mtype = END;
  msgsnd(server_queue_id, &buff, message_size, 0);
}

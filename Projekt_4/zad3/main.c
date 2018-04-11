#define _GNU_SOURCE

#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define FAILURE_EXIT(code, format, ...)     \
  {                                         \
    fprintf(stderr, format, ##__VA_ARGS__); \
    exit(code);                             \
  }

void reciever_handler(int, siginfo_t *, void *);
void reciever();

volatile int signals_to_send;
volatile int op_type;
volatile int send_to_child = 0;
volatile int recieved_by_child = 0;
volatile int recieved_from_child = 0;
volatile pid_t child_pid;

void printStats() {
  printf("SENT: %d\n", send_to_child);
  printf("RECIEVED: %d\n", recieved_from_child);
  printf("RECIEVED CHILD: %d\n", recieved_by_child);
}

int main(int argc, char *argv[]) {
  signals_to_send = (int)strtol(argv[1], NULL, 10);
  op_type = (int)strtol(argv[2], NULL, 10);

  child_pid = fork();
  if (child_pid == 0)
    execl("./reflector", "./reflector", NULL);
  else if (child_pid > 0)
    reciever();
  else
    FAILURE_EXIT(2, "FORK ERROR\n");

  printStats();

  return 0;
}

void reciever_handler(int signum, siginfo_t *info, void *context) {
  if (signum == SIGINT) {
    // fprintf(stdout, "Received SIGINT\n");
    kill(child_pid, SIGUSR2);
    printStats();
    exit(9);
  }
  if (info->si_pid != child_pid) return;

  if ((op_type == 1 || op_type == 2) && signum == SIGUSR1) {
    recieved_from_child += 1;
    // fprintf(stdout, "Received SIGUSR1\n");
  } else if (op_type == 3 && signum == SIGRTMIN) {
    recieved_from_child += 1;
    // fprintf(stdout, "Received SIGRTMIN\n");
  }
}

void reciever() {
  sleep(1);

  struct sigaction act;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = reciever_handler;

  if (sigaction(SIGINT, &act, NULL) == -1)
    FAILURE_EXIT(1, "CATCHING ERROR: SIGINT\n");
  if (op_type == 1 || op_type == 2)
    if (sigaction(SIGUSR1, &act, NULL) == -1)
      FAILURE_EXIT(1, "CATCHING ERROR: SIGUSR1\n");
  if (op_type == 3)
    if (sigaction(SIGRTMIN, &act, NULL) == -1)
      FAILURE_EXIT(1, "CATCHING ERROR: SIGRTMIN\n");

  if (op_type == 1 || op_type == 2) {
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT);
    for (; send_to_child < signals_to_send; send_to_child += 1) {
      // fprintf(stdout, "SENDING SIGUSR1\n");
      kill(child_pid, SIGUSR1);
      if (op_type == 2) sigsuspend(&mask);
    }
    // fprintf(stdout, "SENDING SIGUSR2\n");
    kill(child_pid, SIGUSR2);
  } else if (op_type == 3) {
    for (; send_to_child < signals_to_send; send_to_child += 1) {
      // fprintf(stdout, "SENDING SIGRTMIN\n");
      kill(child_pid, SIGRTMIN);
    }
    send_to_child += 1;
    // fprintf(stdout, "SENDING SIGRTMAX\n");
    kill(child_pid, SIGRTMAX);
  }

  int status = 0;
  waitpid(child_pid, &status, 0);
  if (WIFEXITED(status))
    recieved_by_child = WEXITSTATUS(status);
  else
    FAILURE_EXIT(1, "TERMINATION ERROR\n");
}
#define _GNU_SOURCE

#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

volatile int n = 0;
volatile int k = 0;

void int_sig_handler(int, siginfo_t *, void *);
void usr_sig_handler(int, siginfo_t *, void *);
void chl_sig_handler(int, siginfo_t *, void *);
void ret_sig_handler(int, siginfo_t *, void *);

volatile pid_t *pid_queue;
volatile pid_t *children_pid;
volatile int children_quantity;
volatile int signals_required;

int check_if_children(pid_t pid) {
  for (int i = 0; i < children_quantity; i++)
    if (children_pid[i] == pid) return i;
  return -1;
}

int main(int argc, char **argv) {
  children_quantity = strtol(argv[1], NULL, 10);
  signals_required = strtol(argv[2], NULL, 10);

  children_pid = calloc(children_quantity, sizeof(pid_t));
  pid_queue = calloc(children_quantity, sizeof(pid_t));

  struct sigaction act;
  sigemptyset(&act.sa_mask);

  act.sa_flags = SA_SIGINFO;

  act.sa_sigaction = int_sig_handler;
  if(sigaction(SIGINT, &act, NULL) == -1) exit(EXIT_FAILURE);
  act.sa_sigaction = usr_sig_handler;
  if(sigaction(SIGUSR1, &act, NULL) == -1) exit(EXIT_FAILURE);
  act.sa_sigaction = chl_sig_handler;
  if(sigaction(SIGCHLD, &act, NULL) == -1) exit(EXIT_FAILURE);

  act.sa_sigaction = ret_sig_handler;
  for (int i = SIGRTMIN; i <= SIGRTMAX; i++) if(sigaction(SIGRTMIN + i, &act, NULL) == -1) exit(EXIT_FAILURE);


  for (int i = 0; i < children_quantity; i++) {
    pid_t pid = fork();
    if (!pid) {
      execl("./child", "./child", NULL);
      exit(EXIT_SUCCESS);
    } else {
      children_pid[n++] = pid;
    }
  }

  while (wait(NULL))
    if (errno == ECHILD) break;

  return 0;
}

void int_sig_handler(int signum, siginfo_t *info, void *context) {
  fprintf(stdout, "\tParent Process: Received SIGINT\n");
  for (int i = 0; i < n; i++)
    if (children_pid[i] != -1) {
      kill(children_pid[i], SIGKILL);
      waitpid(children_pid[i], NULL, 0);
    }
  exit(0);
}

void usr_sig_handler(int signum, siginfo_t *info, void *context) {
  if (check_if_children(info->si_pid) == -1) return;

  fprintf(stdout, "Parent Process: Received SIGUSR1 form PID: %d\n", info->si_pid);
  if (k >= signals_required) {
    fprintf(stdout, "Parent Process: Sending SIGUSR1 to Child PID: %d\n",
              info->si_pid);
    kill(info->si_pid, SIGUSR1);
    waitpid(info->si_pid, NULL, 0);
  } else {
    pid_queue[k++] = info->si_pid;
    if (k >= signals_required) {
      for (int i = 0; i < signals_required; i++) {
        if (pid_queue[i] > 0) {
          fprintf(stdout, "Parent Process: Sending SIGUSR1 to Child PID: %d\n",
                    pid_queue[i]);
          kill(pid_queue[i], SIGUSR1);
          waitpid(pid_queue[i], NULL, 0);
        }
      }
    }
  }
}

void chl_sig_handler(int signum, siginfo_t *info, void *context) {
  if (info->si_code == CLD_EXITED) {
    fprintf(stdout, "Parent Process: Child %d has terminated, with exit status: %d\n",
              info->si_pid, info->si_status);
  } else {
    fprintf(stdout, "Parent Process: Child %d has terminated by signal: %d\n",
              info->si_pid, info->si_status);
  }
  n -= 1;
  if (n == 0) {
    fprintf(stdout, "Parent Process: No more children, Terminating\n");
    exit(0);
  }
  for (int i = 0; i < n; i++) {
    if (info->si_pid == children_pid[i]) {
      children_pid[i] = -1;
      break;
    }
  }
}

void ret_sig_handler(int signum, siginfo_t *info, void *context) {
  fprintf(stdout, "Received SIGRT: SIGMIN+%i, for PID: %d\n", signum - SIGRTMIN,
            info->si_pid);
}

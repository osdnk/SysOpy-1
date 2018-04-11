#define _GNU_SOURCE

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usr1_sig_handler_r(int signum);
void usr2_sig_handler_r(int signum);
void rmax_sig_handler_r(int signum);
void rmin_sig_handler_r(int signum);

volatile int counter_ = 0;

int main(int argc, char** argv) {
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGUSR1);
  sigdelset(&mask, SIGUSR2);
  sigdelset(&mask, SIGRTMIN);
  sigdelset(&mask, SIGRTMAX);

  struct sigaction act;

  sigemptyset(&act.sa_mask);
  act.sa_handler = usr2_sig_handler_r;
  if (sigaction(SIGUSR2, &act, NULL) == -1) exit(EXIT_FAILURE);

  act.sa_handler = rmax_sig_handler_r;
  if (sigaction(SIGRTMAX, &act, NULL) == -1) exit(EXIT_FAILURE);

  sigdelset(&act.sa_mask, SIGUSR2);
  act.sa_handler = usr1_sig_handler_r;
  if (sigaction(SIGUSR1, &act, NULL) == -1) exit(EXIT_FAILURE);

  sigfillset(&act.sa_mask);
  sigdelset(&act.sa_mask, SIGRTMAX);
  act.sa_handler = rmin_sig_handler_r;
  if (sigaction(SIGRTMIN, &act, NULL) == -1) exit(EXIT_FAILURE);

  sigprocmask(SIG_SETMASK, &mask, NULL);

  while (1) sigsuspend(&mask);

  return counter_;
}

void usr1_sig_handler_r(int signum) {
  counter_ += 1;
  kill(getppid(), SIGUSR1);
}

void usr2_sig_handler_r(int signum) {
  counter_ += 1;
  exit(counter_);
}

void rmin_sig_handler_r(int signum) {
  counter_ += 1;
  kill(getppid(), SIGRTMIN);
}

void rmax_sig_handler_r(int signum) {
  counter_ += 1;
  exit(counter_);
}

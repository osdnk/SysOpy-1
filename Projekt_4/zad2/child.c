#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void usr_sig_handler(int signum) {
    kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
}

int main() {

    const union sigval value;
    signal(SIGUSR1, usr_sig_handler);
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);

    srand(getpid());
    int sleepTime = (rand() % 11);

    printf("Pid: %d SleepTime: %d\n", getpid(), sleepTime);

    fflush(stdout);
    sleep(sleepTime);

    while(sigqueue(getppid(), SIGUSR1, value) != 0);

    sigsuspend(&mask);

    return sleepTime;
}

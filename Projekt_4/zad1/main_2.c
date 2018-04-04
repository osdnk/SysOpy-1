
#define _XOPEN_SOURCE
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int yay = 0;
int yay_2 = 0;
pid_t pid = 0;

void signal_handler() {
    struct sigaction act;
    act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(yay == 0)
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    yay = yay == 1 ? 0 : 1;
}

void init_signal() {
    printf("\nOdebrano sygnał SIGINt\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {

    struct sigaction act;
    act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    pid = fork();
    if (pid == 0){
        execl("./script.sh", "./script.sh", (char*) NULL);
        exit(EXIT_SUCCESS);
    }

    while(1) {
        sigaction(SIGTSTP, &act, NULL);
        signal(SIGINT, init_signal);

        if(yay == 0) {
            if(yay_2){
                yay_2 = 0;
                yay = 0;

                pid = fork();
                if (pid == 0){
                    execl("./script.sh", "./script.sh", (char*) NULL);
                    exit(EXIT_SUCCESS);
                }

            }
        }
        else {
            if(yay_2 == 0) {
                kill(pid, SIGKILL);
                yay_2 = 1;
            }
        }
    }
    return 0;
}

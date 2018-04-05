#define _XOPEN_SOURCE
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int yay = 0;

void signal_handler(int num) {
    if(yay == 0)
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    yay = yay == 1 ? 0 : 1;
}

void init_signal(int num) {
    printf("\nOdebrano sygnał SIGINT\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {

    time_t just_time;
    char buff[128];
    struct sigaction act;
    act.sa_handler = signal_handler;
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGINT);
    act.sa_flags = 0;

    while(1){
        sigaction(SIGTSTP, &act,NULL);
        signal(SIGINT,init_signal);


        if(yay)
            continue;

        just_time = time(NULL);
        strftime(buff, sizeof(buff),"%H:%M:%S",localtime(&just_time));
        printf("%s\n",buff);
        sleep(1);
    }
    return 0;
}

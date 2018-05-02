#ifndef PARAMS_H
#define PARAMS_H

#define MAX_MSG_SIZE 4096
#define PROGRAM_ID 7312

#include <sys/types.h>

typedef enum mtype {
    MIRROR = 1,
    CALC = 2,
    TIME = 3,
    END = 4,
    LOGIN = 5,
} mtype;

typedef struct message {
    long mtype;
    pid_t pid;
    int id;
    char msg[MAX_MSG_SIZE];
} message;

const int message_size = sizeof(message) - sizeof(long);

#endif

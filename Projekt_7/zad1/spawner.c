#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv) {
  if (argc < 3) {
    fprintf(stderr, "need more arguments");
    exit(EXIT_FAILURE);
  }

  int client_count = strtol(argv[1], NULL, 10);

  for (int i = 0; i < client_count; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      execlp("./client", "./client", argv[2], NULL);
      exit(0);
    }
  }

  return 0;
}

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void parse_args(char *buff, char **args) {
  int i = 0;
  char *p = strtok(buff, " \n\t");
  while (p != NULL) {
    args[i++] = p;
    p = strtok(NULL, " \n\t");
  }
  args[i++] = NULL;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("no enough arguments");
    exit(EXIT_FAILURE);
  }
  
  char *file = argv[1];
  FILE *fp = fopen(file, "r");
  char buff[255];
  char *args[255];
  int status = 0;

  while (fgets(buff, 255, fp) != NULL) {
    parse_args(buff, args);

    pid_t pid = fork();
    if (pid == -1) {
      perror("fork failed");
      return EXIT_FAILURE;
    } else if (pid == 0) {
      printf("starting new process %s on pid %d parent pid: %d\n", args[0],
             getpid(), getppid());
      execvp(args[0], args);
      exit(EXIT_SUCCESS);
    }

    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid failed");
      return EXIT_FAILURE;
    }
    const int exit_status = WEXITSTATUS(status);

    printf("exit status was %d\n", exit_status);

    if (exit_status) {
      printf("failed: %s\n", args[0]);
      break;
    } else {
      printf("succeed: %s\n", args[0]);
    }
  }

  fclose(fp);
  return 0;
}
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
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

double get_time_val(struct timeval time) {
  return (double)(time.tv_sec * 1000000.0 + time.tv_usec) / 1000000.0;
}

void set_limits(char *time_val, char *memory_val) {
  long time_val_s = strtol(time_val, NULL, 10);
  long memory_val_s = strtol(memory_val, NULL, 10) * 1048576;

  struct rlimit cpu_time_limit;
  struct rlimit memory_limit;

  cpu_time_limit.rlim_max = (rlim_t) time_val_s;
  cpu_time_limit.rlim_cur = (rlim_t) time_val_s;

  memory_limit.rlim_max = (rlim_t) memory_val_s;
  memory_limit.rlim_cur = (rlim_t) memory_val_s;

  setrlimit(RLIMIT_CPU, &cpu_time_limit);
  setrlimit(RLIMIT_CPU, &memory_limit);
}

int main(int argc, char **argv) {
  if (argc < 4) {
    printf("no enough arguments");
    exit(EXIT_FAILURE);
  }

  char *file = argv[1];
  char *time_val = argv[2];
  char *memory_val = argv[3];

  FILE *fp = fopen(file, "r");
  char buff[255];
  char *args[255];
  int status = 0;
  struct rusage time_start;
  struct rusage time_end;

  while (fgets(buff, 255, fp) != NULL) {
    getrusage(RUSAGE_CHILDREN, &time_start);
    parse_args(buff, args);

    pid_t pid = fork();
    if (pid == -1) {
      perror("fork failed");
      return EXIT_FAILURE;
    } else if (pid == 0) {
      printf("starting new process %s on pid %d parent pid: %d\n", args[0],
             getpid(), getppid());
      set_limits(time_val, memory_val);
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

    getrusage(RUSAGE_CHILDREN, &time_end);
    double sys = get_time_val(time_end.ru_utime) - get_time_val(time_start.ru_utime);
    double usr = get_time_val(time_end.ru_stime) - get_time_val(time_start.ru_stime);

    printf("USER: %lf   ", usr);
    printf("SYSTEM: %lf     ", sys);
    printf("\n\n");
  }

  fclose(fp);
  return 0;
}
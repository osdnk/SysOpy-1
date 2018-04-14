
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define WRITE_MSG(format, ...)              \
  {                                         \
    char buffer[255];                       \
    sprintf(buffer, format, ##__VA_ARGS__); \
    write(2, buffer, strlen(buffer));       \
  }

void parse_args(char *buff, char **args) {
  int i = 0;
  char *p = strtok(buff, " \n\t");
  while (p != NULL) {
    args[i++] = p;
    p = strtok(NULL, " \n\t");
  }
  args[i] = NULL;
}

int parse_programs(char *buff, char **programs) {
  int i = 0;
  char *p = strtok(buff, "|");
  while (p != NULL) {
    programs[i++] = p;
    p = strtok(NULL, "|");
  }
  programs[i++] = NULL;
  return i - 1;
}

int forker(char **programs, int i, int prog_num) {
  int status = 0;
  int fd[2] = {0, 0};
  char *args[255];

  parse_args(programs[i], args);

  pipe(fd);
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork failed");
    exit(EXIT_FAILURE);

  } else if (pid == 0) {  // children

    int child_exit_val = 0;
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);

    if (i + 2 < prog_num)
      child_exit_val = forker(programs, i + 1, prog_num);
    else {
      i += 1;
      parse_args(programs[i], args);
      WRITE_MSG("starting new process %s on pid %d parent pid: %d\n", args[0],
                getpid(), getppid());
      child_exit_val = execvp(args[0], args);
      close(fd[1]);
    }

    WRITE_MSG("child out: %s %d\n", args[0], pid);
    exit(child_exit_val);

  } else {  // parent
    WRITE_MSG("starting new process %s on pid %d parent pid: %d\n", args[0],
              getpid(), getppid());

    close(fd[1]);
    dup2(fd[0], STDIN_FILENO);

    WRITE_MSG("waiting for %d\n", pid);
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid failed\n");
      exit(EXIT_FAILURE);
    }
    const int exit_status = WEXITSTATUS(status);

    WRITE_MSG("exit status was %d, pid %d\n", exit_status, pid);

    if (exit_status) {
      WRITE_MSG("failed: %d\n", pid);
      exit(EXIT_FAILURE);
    } else {
      WRITE_MSG("succeed: %d\n", pid);
      int is_exec_succ = 0;
      is_exec_succ = execvp(args[0], args);
      close(fd[0]);
      if (is_exec_succ != 0) {
        WRITE_MSG("failed: %d\n", getpid());
        exit(1);
      }
      exit(0);
    }
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    WRITE_MSG("no enough arguments");
    exit(EXIT_FAILURE);
  }

  char *file = argv[1];
  FILE *fp = fopen(file, "r");
  char buff[255];
  char *programs[255];
  int status = 0;
  int proc_num = 0;

  while (fgets(buff, 255, fp) != NULL) {
    proc_num = parse_programs(buff, programs);
    pid_t pid = fork();
    if (pid == 0)
      forker(programs, 0, proc_num);
    else {
      WRITE_MSG("waiting for %d\n", pid);
      if (waitpid(pid, &status, 0) == -1) {
        WRITE_MSG("waitpid failed\n");
        exit(EXIT_FAILURE);
      }

      const int exit_status = WEXITSTATUS(status);
      WRITE_MSG("succeed: %d\n",pid);

      if (exit_status) {
        WRITE_MSG("line failed\n");
        exit(EXIT_FAILURE);
      } else {
        WRITE_MSG("line succeed\n");
      }
    }
  }

  fclose(fp);
  return 0;
}
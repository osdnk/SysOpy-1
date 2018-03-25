#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void parse_args(char *buff, char **args)
{
    int i = 0;
    char *p = strtok(buff, " ");
    while (p != NULL)
    {
        args[i++] = p;
        printf("%s", args[i]);
        p = strtok(NULL, " ");
    }
    args[i++] = NULL;
}

int main(int argc, char **argv)
{

    FILE *fp = fopen("ex.txt", "r");
    char *buff = malloc(sizeof(char) * 255);
    char **args = malloc(sizeof(char *) * 255);
    int result = 0;
    int pid = getpid();

    while (fread(buff, sizeof(char), 255, fp) > 0)
    {
        parse_args(buff, args);

        pid_t new_process = fork();
        if (new_process != 0)
        {
            pid = getpid();
            printf("new process pid: %d\n", pid);
            execvp(args[0], args);
        }
        wait(&result);
        if (result == 0)
        {
            printf("succeded: %s\nexit code: %d\n", args[0], result);
            exit(0);
        }
        else
        {
            printf("failed: %s\nexit code: %d\n", args[0], result);
        }
    }

    fclose(fp);
    free(buff);
    free(args);
    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>

void parse_args(char *buff, char **args)
{
    int i = 0;
    char *p = strtok(buff, " \n\t");
    while (p != NULL)
    {
        args[i++] = p;
        p = strtok(NULL, " \n\t");
    }
    args[i++] = NULL;
}

int main(int argc, char **argv)
{

    FILE *fp = fopen("programs.txt", "r");
    char buff[255];
    char *args[255];
    int status = 0;

    while (fgets(buff, 255, fp) != NULL)
    {
        parse_args(buff, args);

        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork failed");
            return EXIT_FAILURE;
        }
        else if (pid > 0)
        {
            printf("starting new process %s on pid %d\n", args[0], pid);
            execvp(args[0], args);
            return EXIT_SUCCESS;
        }
        printf("looking for process on pid %d\n", pid);
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid failed");
            return EXIT_FAILURE;
        }
        // wait(&status);
        const int es = WEXITSTATUS(status);

        printf("exit status was %d %d\n", status, es);

        if (es)
        {
            printf("succeded: %s\n", args[0]);
        }
        else
        {
            printf("failed: %s\n", args[0]);
            break;
        }
    }

    fclose(fp);
    return 0;
}
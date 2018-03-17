#define _GNU_SOURCE

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <time.h>
#include <limits.h>
// #include <grp.h>

static const char default_format[] = " %b %d %H:%M";
int const buff_size = 100;

int main(int argc, char **argv)
{

    if (argc<4){
        printf("need more arguments");
        exit(EXIT_FAILURE);
    }

    char* path = argv[1];
    char* op = argv[2];
    char* param_date = argv[3];

    const char *format = default_format;
    char buffer[buff_size];

    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        printf("couldnt open the directory\n");
        exit(EXIT_FAILURE);
    }

    struct dirent *rdir = readdir(dir);

    struct stat fileStat;

    while (rdir != NULL)
    {
        if (strcmp(rdir->d_name, ".") == 0 || strcmp(rdir->d_name, "..") == 0)
        {
            rdir = readdir(dir);
            continue;
        }
        
        stat(rdir->d_name, &fileStat);
        if (!S_ISLNK(fileStat.st_mode))
        {
            printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
            printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
            printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
            printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
            printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
            printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
            printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
            printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
            printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
            printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");

            printf(" %ld\t", fileStat.st_nlink);

            printf(" %s\t", getpwuid(fileStat.st_uid)->pw_name);
            printf(" %s\t", getpwuid(fileStat.st_gid)->pw_name);

            printf(" %ld\t", fileStat.st_size);

            strftime(buffer, buff_size, format, localtime(&fileStat.st_mtime));
            printf(" %s\t", buffer);

            realpath(rdir->d_name, buffer);
            printf(" %s\t", buffer);

            printf("\n");
        }
        rdir = readdir(dir);
    }

    closedir(dir);
    return 0;
}
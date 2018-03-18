#define _GNU_SOURCE 500

#include <time.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>
#include <ftw.h>
#include <getopt.h>
#include <limits.h>

int const buff_size = PATH_MAX;

time_t date;
char *op;

int date_compare(time_t *date_1, const time_t *date_2)
{
    struct tm *tm1 = malloc(sizeof(struct tm));
    struct tm *tm2 = malloc(sizeof(struct tm));
    tm1 = localtime_r(date_1, tm1);
    tm2 = localtime_r(date_2, tm2);

    int res =
        tm1->tm_mon - tm2->tm_mon == 0
            ? (tm1->tm_mday - tm2->tm_mday == 0
                   ? (tm1->tm_hour - tm2->tm_hour == 0
                          ? (tm1->tm_min - tm2->tm_min == 0
                                 ? 0
                                 : tm1->tm_min - tm2->tm_min)
                          : tm1->tm_hour - tm2->tm_hour)
                   : tm1->tm_mday - tm2->tm_mday)
            : tm1->tm_mon - tm2->tm_mon;
            
    free(tm1);
    free(tm2);
    return res;
}

void print_info(const char *path, const struct stat *file_stat)
{
    char buffer[buff_size];
    printf((S_ISDIR(file_stat->st_mode)) ? "d" : "-");
    printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat->st_mode & S_IXOTH) ? "x" : "-");

    printf(" %ld\t", file_stat->st_nlink);

    printf(" %s\t", getpwuid(file_stat->st_uid)->pw_name);
    printf(" %s\t", getpwuid(file_stat->st_gid)->pw_name);

    printf(" %ld\t", file_stat->st_size);

    strftime(buffer, buff_size, "%b %d %H:%M", localtime(&file_stat->st_mtime));
    printf(" %s\t", buffer);

    printf(" %s\t", path);

    printf("\n");
}

int process(const char *path, const struct stat *file_stat, int type_flag, struct FTW *ftw)
{
    if (type_flag == FTW_F)
    {
        if (strcmp(op, "=") == 0)
        {
            date_compare(&date, &file_stat->st_mtime) == 0
                ? print_info(path, file_stat)
                : "";
        }
        else if (strcmp(op, "<") == 0)
        {
            date_compare(&date, &file_stat->st_mtime) > 0
                ? print_info(path, file_stat)
                : "";
        }
        else if (strcmp(op, ">") == 0)
        {
            date_compare(&date, &file_stat->st_mtime) < 0
                ? print_info(path, file_stat)
                : "";
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    char *start_path = argv[1];
    op = argv[2];
    char *usr_date = argv[3];

    struct tm *tm = malloc(sizeof(struct tm));

    strptime(strcat(usr_date, ":00"), "%b %d %H:%M:%S", tm);
    date = mktime(tm);

    nftw(start_path, process, 100, FTW_PHYS);
    return (0);
}
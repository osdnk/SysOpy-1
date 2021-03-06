#define _GNU_SOURCE 500

#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static const char default_format[] = "%b %d %H:%M";
int const buff_size = PATH_MAX;

int date_compare(time_t *date_1, time_t *date_2) {
  struct tm *tm1 = malloc(sizeof(struct tm));
  struct tm *tm2 = malloc(sizeof(struct tm));
  tm1 = localtime_r(date_1, tm1);
  tm2 = localtime_r(date_2, tm2);

  int res = tm1->tm_mon - tm2->tm_mon == 0
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

void print_info(char *path, struct dirent *rdir, struct stat *file_stat,
                char *buffer, const char *format) {
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

  strftime(buffer, buff_size, format, localtime(&file_stat->st_mtime));
  printf(" %s\t", buffer);

  printf(" %s\t", realpath(path, buffer));

  printf("\n");
}

void tree_rusher(char *path, char *op, time_t *date, const char *format,
                 char *buffer) {
  if (path == NULL) return;
  DIR *dir = opendir(path);

  if (dir == NULL) return;

  struct dirent *rdir = readdir(dir);
  struct stat file_stat;

  char new_path[buff_size];

  while (rdir != NULL) {
    strcpy(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, rdir->d_name);

    lstat(new_path, &file_stat);

    if (S_ISLNK(file_stat.st_mode)) {
      rdir = readdir(dir);
      continue;
    } else if (strcmp(rdir->d_name, ".") == 0 ||
               strcmp(rdir->d_name, "..") == 0) {
      rdir = readdir(dir);
      continue;
    } else {
      if (S_ISREG(file_stat.st_mode)) {
        if (strcmp(op, "=") == 0)
          date_compare(date, &file_stat.st_mtime) == 0
              ? print_info(new_path, rdir, &file_stat, buffer, format)
              : "";
        else if (strcmp(op, "<") == 0)
          date_compare(date, &file_stat.st_mtime) > 0
              ? print_info(new_path, rdir, &file_stat, buffer, format)
              : "";
        else if (strcmp(op, ">") == 0)
          date_compare(date, &file_stat.st_mtime) < 0
              ? print_info(new_path, rdir, &file_stat, buffer, format)
              : "";
      }

      if (S_ISDIR(file_stat.st_mode)) {
        pid_t pid = fork();
        if (pid == 0) {
          tree_rusher(realpath(rdir->d_name, new_path), op, date, format,
                      buffer);
          exit(EXIT_SUCCESS);
        }
      }
      rdir = readdir(dir);
    }
  }
  closedir(dir);
}

int main(int argc, char **argv) {
  if (argc < 4) {
    printf("need more arguments");
    exit(EXIT_FAILURE);
  }

  char *start_path = argv[1];
  char *op = argv[2];
  char *usr_date = argv[3];

  char buff[buff_size];

  const char *format = default_format;
  struct tm *tm = malloc(sizeof(struct tm));

  strptime(strcat(usr_date, ":00"), "%b %d %H:%M:%S", tm);
  time_t date = mktime(tm);
  tm = localtime(&date);

  char buffer[buff_size];

  DIR *dir = opendir(start_path);
  if (dir == NULL) {
    printf("couldnt open the directory\n");
    exit(EXIT_FAILURE);
  }

  tree_rusher(realpath(start_path, buff), op, &date, format, buffer);

  free(tm);
  closedir(dir);

  sleep(1);
  return 0;
}
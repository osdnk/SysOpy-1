#ifndef SYS
#define SYS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void sys_generate_file(char *path, int records_number, int block_size);
void sys_sort_file(char *path, int records_number, int block_size);
void sys_copy_file(char *path_source, char *path_target, int records_number, int block_size);

#endif
#ifndef SYS
#define SYS

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void sys_generate_file(char *path, int records_number, int block_size);
void sys_sort_file(char *path, int records_number, int block_size);
void sys_copy_file(char *path_source, char *path_target, int records_number,
                   int block_size);

#endif
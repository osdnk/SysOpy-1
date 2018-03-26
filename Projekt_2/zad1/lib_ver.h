#ifndef LIB
#define LIB

#include <stdio.h>
#include <stdlib.h>

void lib_generate_file(char *path, int records_number, int block_size);
void lib_sort_file(char *path, int records_number, int block_size);
void lib_copy_file(char *path_source, char *path_target, int records_number,
                   int block_size);

#endif
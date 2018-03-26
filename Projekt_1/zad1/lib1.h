#ifndef LIB1_H
#define LIB1_H

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEIGHT 1000000
#define WIDTH 1000

typedef struct Table {
  int height;
  int width;
  int current_size;
  char **blocks;
  bool is_static;
  int *sums;
} Table;

extern char staticArray[HEIGHT][WIDTH];

Table *create_table(unsigned int height, unsigned int width, bool is_static);
void delete_table(Table *table);
void create_block(unsigned int, Table *table, char *value);
void delete_block(unsigned int index, Table *table);
char *find_block(unsigned int index, Table *table);
unsigned int char_to_sum(char *chars, unsigned int width);

#endif
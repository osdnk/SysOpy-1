#ifndef LIB1_H
#define LIB1_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

typedef struct Table
{
    int height;
    int width;
    int current_last;
    char **blocks;
    bool is_static;
} Table;

extern char staticArray[600000][1000];

Table* create_table(unsigned int height, unsigned int width, bool is_static);
void delete_table(Table **table);
void create_block(Table *table, char* value);
void delete_block(unsigned int index, Table *table);
char *find_block(unsigned int index, Table *table);
unsigned int char_to_sum(char *chars, unsigned int width);

#endif
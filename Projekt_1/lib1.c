#include "lib1.h"

char staticArray[1000][1000];

Table *create_table(unsigned int height, unsigned int width, bool is_static)
{
    Table *table = calloc(1, sizeof(Table));

    table->height = height;
    table->is_static = is_static;
    table->current_last = 0;
    table->width = width;

    if (is_static)
    {
        table->blocks = (char **)staticArray;
    }
    else
    {
        char **arr = calloc(height, sizeof(char *));
        table->blocks = (char **)arr;
    }

    return table;
}

void delete_table(Table **table)
{
    if (!(*table)->is_static)
    {
        for (int i = 0; i < (*table)->current_last; i++)
            free((*table)->blocks[i]);

        free(*(*table)->blocks);
    }

    (*table) = NULL;
}

void create_block(Table *table, char *value)
{
    if (table->current_last < table->height)
    {
        if (table->is_static)
        {
            table->blocks[table->current_last] = value;
        }
        else
        {
            table->blocks[table->current_last] = calloc((size_t)table->width, sizeof(char));
            strcpy(table->blocks[table->current_last], value);
        }
        table->current_last += 1;
    }
    else
    {
        printf("Array is full");
        exit(1);
    }
}

void delete_block(unsigned int index, Table *table)
{
    if (table->height <= index || table->current_last <= index)
    {
        printf("invalid index");
        exit(1);
    }
    if (table->current_last == 0)
    {
        printf("array is empty");
        exit(1);
    }
    else
    {
        if (table->is_static)
        {
            table->blocks[index] = "";
        }
        else
        {
            free(table->blocks[index]);
        }
        while (index < table->current_last && index < table->height)
        {
            table->blocks[index] = table->blocks[index + 1];
            index += 1;
        }
        if (table->is_static)
        {
            table->blocks[index] = "";
        }
        else
        {
            table->blocks[index] = NULL;
        }
        table->current_last -= 1;
    }
}

unsigned int char_to_sum(char *chars, unsigned int width)
{
    int sum = 0;
    for (int i = 0; i < width; i++)
        if(chars[i] != '\n')
            sum += chars[i];

    return sum;
}

char *find_block(unsigned int index, Table *table)
{
    if (table->height <= index || table->current_last <= index)
    {
        printf("invalid index");
        exit(1);
    }
    int looking_sum = char_to_sum(table->blocks[index], (unsigned int)table->width);
    int current_closest = -1;
    int closest_diff = 1000;
    for (int i = 0; i < table->current_last; i++)
    {
        int tmp_diff = abs(char_to_sum(table->blocks[i], (unsigned int)table->width) - looking_sum);
        if (tmp_diff < closest_diff && i != index)
        {
            current_closest = i;
            closest_diff = tmp_diff;
        }
    }

    if (current_closest == -1)
        return NULL;
    else
        return table->blocks[current_closest];
}
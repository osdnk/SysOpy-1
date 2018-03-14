#include "lib1.h"

char staticArray[HEIGHT][WIDTH];

Table *create_table(unsigned int height, unsigned int width, bool is_static)
{
    Table *table = calloc(1, sizeof(Table));

    table->height = height;
    table->is_static = is_static;
    table->current_size = 0;
    table->width = width;
    table->sums = calloc(height, sizeof(int));

    if (is_static)
    {
        table->blocks = (char **)staticArray;
    }
    else
    {
        char **arr = calloc(height, sizeof(char *));
        table->blocks = (char **)arr;
        // for (int i = 0; i < table->height; i++)
        //     table->blocks[i] = NULL;
    }

    return table;
}

void delete_table(Table *table)
{
    if (!table->is_static)
    {
        for (int i = 0; i < table->height; i++)
            free(table->blocks[i]);

        free(table->blocks);
    }

    free(table);
}

void create_block(unsigned int index, Table *table, char *value)
{
    if (table->height <= index)
    {
        printf("invalid index(create): %d", index);
        exit(1);
    }
    else if (table->current_size == table->height - 1)
    {
        printf("Array is full");
        exit(1);
    }
    else
    {
        if (table->is_static)
        {
            table->blocks[index] = value;
        }
        else
        {
            if (table->blocks[index] != NULL)
            {
                delete_block(index, table);
            }
            table->blocks[index] = calloc(table->width, sizeof(char));
            strcpy(table->blocks[index], value);
        }

        table->sums[index] = char_to_sum(value, table->width);
        table->current_size += 1;
    }
}

void delete_block(unsigned int index, Table *table)
{
    if (table->height <= index)
    {
        printf("invalid index(delete): %d", index);
        exit(1);
    }
    if (table->current_size == 0)
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
            table->blocks[index] = NULL;
        }
        table->current_size -= 1;
    }
}

unsigned int char_to_sum(char *chars, unsigned int width)
{
    int sum = 0;
    for (int i = 0; i < width; i++)
    {
        if (chars[i] == '\0')
            break;
        if (chars[i] != '\n')
            sum += chars[i];
    }

    return sum;
}

char *find_block(unsigned int index, Table *table)
{
    if (table->height <= index)
    {
        printf("invalid index(find): %d", index);
        exit(1);
    }
    int looking_sum = table->sums[index];
    int current_closest = -1;
    int closest_diff = 100000;
    for (int i = 0; i < table->width; i++)
    {
        int tmp_diff = abs(table->sums[i] - looking_sum);
        if (tmp_diff < closest_diff && i != index)
        {
            current_closest = i;
            closest_diff = tmp_diff;
        }
    }

    if (current_closest == -1)
        return NULL;
    else
    {
        return table->blocks[current_closest];
    }
}
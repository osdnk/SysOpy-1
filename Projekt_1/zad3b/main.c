
#define MAX_OPS 6

#include "lib1.h"

#include <dlfcn.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

double time_diff(clock_t t1, clock_t t2);
char *create_value(int max_size);
void print_times(clock_t real_start, struct tms *time_start, clock_t real_end, struct tms *time_end, char *message, FILE *fp);

#ifdef DYNAMIC

void add_remove_blocks(int count, Table *tab, void (*delete_block)(unsigned int, Table *), void (*create_block)(Table *, char *))
{
    for (int i = 0; i < count; i++)
        (*create_block)(tab, create_value(tab->width));

    for (int i = 0; i < count; i++)
        (*delete_block)(rand() % tab->current_last, tab);
}

void add_remove_blocks_cross(int count, Table *tab, void (*delete_block)(unsigned int, Table *), void (*create_block)(Table *, char *))
{
    for (int i = 0; i < count; i++)
    {
        (*create_block)(tab, create_value(tab->width));
        (*delete_block)(rand() % tab->height, tab);
    }
}

#endif

#ifndef DYNAMIC

void add_remove_blocks(int count, Table *tab);
void add_remove_blocks_cross(int count, Table *tab);
#endif

int main(int argc, char **argv)
{

#ifdef DYNAMIC

    void *handle;

    handle = dlopen("./liblib1.so", RTLD_LAZY);
    Table *(*create_table)(unsigned int, unsigned int, bool) = dlsym(handle, "create_table");
    void (*delete_block)(unsigned int, Table *) = dlsym(handle, "delete_block");
    void (*create_block)(Table *, char *) = dlsym(handle, "create_block");
    char *(*find_block)(unsigned int, Table *) = dlsym(handle, "find_block");
    void (*delete_table)(Table **) = dlsym(handle, "delete_table");

#endif

    clock_t time_arr[MAX_OPS] = {0, 0, 0, 0, 0, 0};
    struct tms *tms_arr[MAX_OPS];

    for (int i = 0; i < MAX_OPS; i++)
        tms_arr[i] = calloc(1, sizeof(struct tms *));

    char **messages = calloc(MAX_OPS, sizeof(char *));
    for (int i = 0; i < MAX_OPS; i++)
        messages[i] = calloc(15, sizeof(char));

    int current_time = 0;
    int curretn_message = 0;

    if (argc < 4)
    {
        printf("program require at least 4 arguments\narr_size, block_size, static|dynamic , [operations]");
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    fp = fopen("./raport3b.txt", "a");

    int arr_size, block_size;
    bool is_static;

    arr_size = atoi(argv[1]);
    block_size = atoi(argv[2]);

    if (strcmp("static", argv[3]) == 0)
    {
        is_static = true;
    }
    else if (strcmp("dynamic", argv[3]) == 0)
    {
        is_static = false;
    }
    else
    {
        printf("static or dynamic ONLY");
        exit(EXIT_FAILURE);
    }

    printf("ARRAY_SIZE: %d BLOCK_SIZE %d ALLOCATION %s", arr_size, block_size, argv[3]);
    fprintf(fp, "ARRAY_SIZE: %d BLOCK_SIZE %d ALLOCATION %s", arr_size, block_size, argv[3]);

    Table *table;

    if (strcmp(argv[4], "c") == 0)
    {
        time_arr[current_time] = times(tms_arr[current_time]);
        current_time += 1;

        strcpy(messages[curretn_message], "CREATING ARRAY");
        curretn_message += 2;

        table = create_table(arr_size, block_size, is_static);

        for (int i = 0; i < table->height / 2; i++)
            create_block(table, create_value(table->width));

        time_arr[current_time] = times(tms_arr[current_time]);
        current_time += 1;
    }
    else
    {
        printf("You must create a table ");
        exit(EXIT_FAILURE);
    }

    for (int i = 5; i < argc; i++)
    {

        time_arr[current_time] = times(tms_arr[current_time]);
        current_time += 1;

        if (strcmp(argv[i], "f") == 0)
        {
            strcpy(messages[curretn_message], "LOOKING FOR BLOCK");
            curretn_message += 2;
            if (table->current_last > 0)
            {
                int count;

                if (i + 1 < argc)
                {
                    i += 1;
                    count = atoi(argv[i]);
                }
                for (int i = 0; i < count; i++)
                    find_block(rand() % table->current_last, table);
            }
        }
        else if (strcmp(argv[i], "ar") == 0)
        {
            int count;

            if (i + 1 < argc)
            {
                i += 1;
                count = atoi(argv[i]);
            }
            else
            {
                printf("unvalid operations");
                exit(EXIT_FAILURE);
            }
            if (count > table->height)
            {
                printf("invalid index");
                exit(EXIT_FAILURE);
            }
            if (table->height - table->current_last < count)
            {
                printf("invalid index");
                exit(EXIT_FAILURE);
            }

            strcpy(messages[curretn_message], "REMOVING AND ADDING BLOCKS");
            curretn_message += 2;

#ifdef DYNAMIC

            add_remove_blocks(count, table, delete_block, create_block);
#endif

#ifndef DYNAMIC

            add_remove_blocks(count, table);
#endif
        }
        else if (strcmp(argv[i], "arc") == 0)
        {
            int count;

            if (i + 1 < argc)
            {
                i += 1;
                count = atoi(argv[i]);
            }
            else
            {
                printf("unvalid operations");
                exit(EXIT_FAILURE);
            }
            if (count > table->height)
            {
                printf("invalid index");
                exit(EXIT_FAILURE);
            }
            if (table->height - table->current_last < count)
            {
                printf("invalid index");
                exit(EXIT_FAILURE);
            }
            strcpy(messages[curretn_message], "CROSS REMOVING AND ADDING BLOCKS");

#ifdef DYNAMIC

            add_remove_blocks_cross(count, table, delete_block, create_block);
#endif

#ifndef DYNAMIC

            add_remove_blocks(count, table);
#endif
        }
        else
        {
            printf("invalid parameter %s", argv[i]);
            exit(EXIT_FAILURE);
        }

        time_arr[current_time] = times(tms_arr[current_time]);
        current_time += 1;
    }

    delete_table(&table);

    for (int i = 0; i < MAX_OPS; i += 2)
        print_times(time_arr[i], tms_arr[i], time_arr[i + 1], tms_arr[i + 1], messages[i], fp);

    return 0;
}

void print_times(clock_t real_start, struct tms *time_start, clock_t real_end, struct tms *time_end, char *message, FILE *fp)
{
    printf("%s\n", message);
    printf("REAL: %fl\n", time_diff(real_start, real_end));
    printf("USER: %fl\n", time_diff(time_start->tms_utime, time_end->tms_utime));
    printf("SYSTEM: %fl\n", time_diff(time_start->tms_stime, time_end->tms_stime));

    fprintf(fp, "%s\n", message);
    fprintf(fp, "REAL: %fl\n", time_diff(real_start, real_end));
    fprintf(fp, "USER: %fl\n", time_diff(time_start->tms_utime, time_end->tms_utime));
    fprintf(fp, "SYSTEM: %fl\n", time_diff(time_start->tms_stime, time_end->tms_stime));
}

double time_diff(clock_t t1, clock_t t2)
{
    return (double)(t2 - t1) / sysconf(_SC_CLK_TCK);
}

char *create_value(int max_size)
{
    int size = rand() % max_size;
    char *valuee = calloc(max_size, sizeof(char));
    char sub_vals[49] = "abcdefghijklmnopqrstuwyzABCDEFGHIJKLMNOPQRSTUWYZ";
    valuee[size - 1] = '\0';
    while (size > 1)
    {
        int index = rand() % 49;
        valuee[size - 2] = sub_vals[index];
        size -= 1;
    }
    return valuee;
}

#ifdef DYNAMIC

#endif

#ifndef DYNAMIC

void add_remove_blocks(int quantity, Table *tab)
{
    for (int i = 0; i < quantity; i++)
        create_block(tab, create_value(tab->width));

    for (int i = 0; i < quantity; i++)
        delete_block(rand() % tab->current_last, tab);
}

void add_remove_blocks_cross(int quantity, Table *tab)
{
    for (int i = 0; i < quantity; i++)
    {
        create_block(tab, create_value(tab->width));
        delete_block(rand() % tab->height, tab);
    }
}

#endif
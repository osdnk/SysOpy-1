#include "lib_ver.h"
#include "sys_ver.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>

#define ops 2

double time_val(struct timeval *time)
{
    return (double)(time->tv_sec * 1000000.0 + time->tv_usec) / 1000000.0;
}

void print_times(struct timeval *real_start, struct rusage *time_start, struct timeval *real_end,
                 struct rusage *time_end, char *message, FILE *fp)
{
    double real = time_val(real_end) - time_val(real_start);
    double sys = time_val(&time_end->ru_utime) - time_val(&time_start->ru_utime);
    double usr = time_val(&time_end->ru_stime) - time_val(&time_start->ru_stime);

    printf("%s\n", message);
    printf("REAL: %lf   ", real);
    printf("USER: %lf   ", usr);
    printf("SYSTEM: %lf     ", sys);
    printf("\n\n");

    fprintf(fp, "%s\n", message);
    fprintf(fp, "REAL: %lf  ", real);
    fprintf(fp, "USER: %lf  ", usr);
    fprintf(fp, "SYSTEM: %lf    ", sys);
    fprintf(fp, "\n");
}

int main(int argc, char **argv)
{

    char *file_path;
    char *cp_to_path;
    char *op;
    char *type;
    int counter, block_size = 0, records_number = 0;
    while (1)
    {
        static struct option long_options[] =
            {
                {"copy", required_argument, 0, 'C'},
                {"path", required_argument, 0, 'P'},
                {"block_size", required_argument, 0, 's'},
                {"records_number", required_argument, 0, 'n'},
                {"operation", required_argument, 0, 'o'},
                {"type", required_argument, 0, 't'},
                {0, 0, 0, 0}};

        int option_index = 0;

        counter = getopt_long(argc, argv, ":F:n:s:C:o:t:",
                              long_options, &option_index);

        if (counter == -1)
            break;

        switch (counter)
        {
        case 0:
            break;

        case 'F':
            file_path = optarg;
            break;

        case 's':
            block_size = atoi(optarg);
            break;

        case 'n':
            records_number = atoi(optarg);
            break;

        case 'C':
            cp_to_path = optarg;
            break;

        case 'o':
            op = optarg;
            break;

        case 't':
            type = optarg;
            break;

        case '?':
            printf("very helpfull info");
            break;

        default:
            abort();
        }
    }

    FILE *fp = fopen("wyniki.out", "a");

    struct rusage usage_time[ops];
    struct timeval real_time[ops];

    char **messages = calloc(ops, sizeof(char *));
    for (int i = 0; i < ops; i++)
        messages[i] = calloc(ops * 2 + 1, sizeof(char));

    int current_time = 0;
    int curretn_message = 0;

    int int_type;

    if (strcmp(type, "sys") == 0)
    {
        int_type = 0;
    }
    else if (strcmp(type, "lib") == 0)
    {
        int_type = 1;
    }
    else
    {
        printf("invalid operation type\n");
        exit(EXIT_FAILURE);
    }

    getrusage(RUSAGE_SELF, &usage_time[current_time]);
    gettimeofday(&real_time[current_time], 0);
    current_time += 1;

    int generate__ = 0;

    if (strcmp(op, "generate") == 0 && file_path && records_number && block_size)
    {
        strcpy(messages[curretn_message], "GENERATE");
        curretn_message += 2;
        generate__ = 1;

        if (int_type)
            lib_generate_file(file_path, records_number, block_size);
        else
            sys_generate_file(file_path, records_number, block_size);
    }
    if (strcmp(op, "copy") == 0 && file_path && cp_to_path && records_number && block_size)
    {
        strcpy(messages[curretn_message], "COPY");
        curretn_message += 2;

        if (int_type)
            lib_copy_file(file_path, cp_to_path, records_number, block_size);
        else
            sys_copy_file(file_path, cp_to_path, records_number, block_size);
    }
    if (strcmp(op, "sort") == 0 && file_path && cp_to_path && records_number && block_size)
    {
        strcpy(messages[curretn_message], "SORT");
        curretn_message += 2;

        if (int_type)
            lib_sort_file(file_path, records_number, block_size);
        else
            sys_sort_file(file_path, records_number, block_size);
    }

    getrusage(RUSAGE_SELF, &usage_time[current_time]);
    gettimeofday(&real_time[current_time], 0);
    current_time += 1;
    if (!generate__)
    {
        fprintf(fp, "+++++++++++++++++++++++++++\n");
        if (int_type)
            fprintf(fp, "LIB_VER:\n");
        else
            fprintf(fp, "SYS_VER:\n");
        fprintf(fp, "\tBLOCK_SIZE: %d RECORDS_NUMBER: %d\n", block_size, records_number);
        print_times(&real_time[0], &usage_time[0], &real_time[1], &usage_time[1], messages[0], fp);
        fclose(fp);
    }

    for (int i = 0; i < ops; i++)
        free(messages[i]);

    free(messages);
    return 0;
}

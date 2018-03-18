#include "lib_ver.h"
#include "sys_ver.h"

#include <getopt.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <sys/resource.h>

void print_times(clock_t real_start, struct tms *time_start, clock_t real_end, struct tms *time_end, char *message, FILE *fp);

int main(int argc, char **argv)
{

    char *file_path;
    char *cp_to_path;
    int counter, block_size = 0, records_number = 0;
    while (1)
    {
        static struct option long_options[] =
            {
                {"copy", required_argument, 0, 'C'},
                {"path", required_argument, 0, 'P'},
                {"block_size", required_argument, 0, 's'},
                {"records_number", required_argument, 0, 'n'},
                {0, 0, 0, 0}};

        int option_index = 0;

        counter = getopt_long(argc, argv, ":F:n:s:C:",
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

        case '?':
            printf("very helpfull info");
            break;

        default:
            abort();
        }
    }

    if (file_path && records_number && block_size)
    {
        lib_generate_file(file_path, records_number, block_size);
    }
    if (file_path && cp_to_path && records_number && block_size)
    {
        lib_copy_file(file_path, cp_to_path, records_number, block_size);
    }
    if (file_path && records_number && block_size)
    {
        lib_sort_file(file_path, records_number, block_size);
    }

    return 0;
}

void print_times(clock_t real_start, struct tms *time_start, clock_t real_end, struct tms *time_end, char *message, FILE *fp)
{
    printf("%s\n", message);
    printf("REAL: %fl   ", time_diff(real_start, real_end));
    printf("USER: %fl   ", time_diff(time_start->tms_utime, time_end->tms_utime));
    printf("SYSTEM: %fl\n", time_diff(time_start->tms_stime, time_end->tms_stime));

    fprintf(fp, "%s\n", message);
    fprintf(fp, "REAL: %fl  ", time_diff(real_start, real_end));
    fprintf(fp, "USER: %fl  ", time_diff(time_start->tms_utime, time_end->tms_utime));
    fprintf(fp, "SYSTEM: %fl\n", time_diff(time_start->tms_stime, time_end->tms_stime));
}
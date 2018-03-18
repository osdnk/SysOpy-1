#include "lib_ver.h"

void lib_generate_file(char *path, int records_number, int block_size)
{
    FILE *fp = fopen(path, "w+");
    FILE *urnd = fopen("/dev/urandom", "r");
    char *buff = malloc(sizeof(char) * block_size);

    for (int i = 0; i < records_number; i++)
    {
        if (fread(buff, sizeof(char), block_size, urnd) != block_size)
        {
            printf("failed reading [generate]\n");
            return;
        }

        // decoding bits to letters
        for (int j = 0; j < block_size; j++)
            buff[j] = (char)(abs(buff[j]) % 25 + 'A');
        buff[block_size - 1] = '\n';

        if (fwrite(buff, sizeof(char), block_size, fp) != block_size)
        {
            printf("failed writing [generate]\n");
            return;
        }
    }
    fclose(fp);
    fclose(urnd);
    free(buff);
}

void lib_sort_file(char *path, int records_number, int block_size)
{
    FILE *file = fopen(path, "r+");
    char *record_0 = malloc(block_size * sizeof(char));
    char *record_1 = malloc(block_size * sizeof(char));
    long int offset = (long int)(block_size * sizeof(char));
    int j;

    for (int i = 0; i < records_number; i++)
    {
        fseek(file, i * offset, 0);

        if (fread(record_0, sizeof(char), (size_t)block_size, file) != block_size)
            return;

        for (j = i - 1; j >= 0; j--)
        {
            fseek(file, j * offset, 0);
            if (fread(record_1, sizeof(char), (size_t)block_size, file) != block_size)
                return;
            if (record_0[0] >= record_1[0])
                break;
            if (fwrite(record_1, sizeof(char), (size_t)block_size, file) != block_size)
                return;
        }
        fseek(file, (j + 1) * offset, 0);
        if (fwrite(record_0, sizeof(char), (size_t)block_size, file) != block_size)
            return;
    }

    fclose(file);
    free(record_0);
    free(record_1);
}
void lib_copy_file(char *path_source, char *path_target, int records_number, int block_size)
{
    FILE *src = fopen(path_source, "r");
    FILE *trg = fopen(path_target, "w+");

    char *buff = malloc(sizeof(char) * block_size);
    for (int i = 0; i < records_number; i++)
    {
        if (fread(buff, sizeof(char), block_size, src) != block_size)
        {
            printf("failed reading [cp]\n");
            return;
        }

        if (fwrite(buff, sizeof(char), block_size, trg) != block_size)
        {
            printf("failed writing [cp]\n");
            return;
        }
    }
    fclose(src);
    fclose(trg);
    free(buff);
}
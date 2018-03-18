#include "sys_ver.h"

void sys_generate_file(char *path, int records_number, int block_size)
{
    int fp = open(path, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    int urnd = open("/dev/urandom", O_RDONLY);
    char *buff = malloc(sizeof(char) * block_size);

    for (int i = 0; i < records_number; i++)
    {
        if (read(urnd, buff, sizeof(char) * block_size) == 0)
        {
            printf("failed reading [generate]\n");
            return;
        }

        // decoding bits to letters
        for (int j = 0; j < block_size; j++)
            buff[j] = (char)(abs(buff[j]) % 25 + 'A');
        buff[block_size - 1] = '\n';

        if (write(fp, buff, sizeof(char) * block_size) == 0)
        {
            printf("failed writing [generate]\n");
            return;
        }
    }
    close(fp);
    close(urnd);
    free(buff);
}
void sys_sort_file(char *path, int records_number, int block_size)
{
    int fp = open(path, O_RDWR);
    char *record_0 = malloc(block_size * sizeof(char));
    char *record_1 = malloc(block_size * sizeof(char));
    long int offset = (long int)(block_size * sizeof(char));
    int j;

    for (int i = 0; i < records_number; i++)
    {
        lseek(fp, i * offset, 0);

        if (read(fp, record_0, sizeof(char) * (size_t)block_size) == 0)
            return;

        for (j = i - 1; j >= 0; j--)
        {
            lseek(fp, j * offset, 0);
            if (read(fp, record_1, sizeof(char) * (size_t)block_size) == 0)
                return;
            if (record_0[0] >= record_1[0])
                break;
            if (write(fp, record_1, sizeof(char) * (size_t)block_size) == 0)
                return;
        }
        lseek(fp, (j + 1) * offset, 0);
        if (write(fp, record_0, sizeof(char) * (size_t)block_size) == 0)
            return;
    }

    close(fp);
    free(record_0);
    free(record_1);
}
void sys_copy_file(char *path_source, char *path_target, int records_number, int block_size)
{
    int src = open(path_source, O_RDONLY);
    int trg = open(path_target, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

    char *buff = malloc(sizeof(char) * block_size);
    for (int i = 0; i < records_number; i++)
    {
        if (read(src, buff, sizeof(char) * block_size) == 0)
        {
            printf("failed reading [cp]\n");
            return;
        }

        if (write(trg, buff, sizeof(char) * block_size) == 0)
        {
            printf("failed writing [cp]\n");
            return;
        }
    }
    close(src);
    close(trg);
    free(buff);
}
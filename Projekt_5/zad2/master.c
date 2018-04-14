#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

const int size = 64;

int main(int argc, char** argv) {
    char *stream_name;
    stream_name = argv[1];
    mkfifo(stream_name, 0777);
    char buff[size];
    FILE * fp = fopen(stream_name, "r");

    while(fgets(buff, size, fp) != NULL){
        fwrite(buff, sizeof(char), strlen(buff), stdout);
    }
    fclose(fp);
    remove(stream_name);
}

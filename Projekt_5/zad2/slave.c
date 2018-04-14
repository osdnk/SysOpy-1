#define _DEFAULT_SOURCE

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

int main(int argc, char** argv) {
   char* stream_name = argv[1];
   int op_num = strtol(argv[2], NULL, 10);

   char buff[64];
   char date_val[64];

   int pipe = open(stream_name, O_WRONLY);

   sprintf(buff, "%d\n", getpid());
   fwrite(buff, sizeof(char), strlen(buff), stdout);
   srand(getpid());
   for(int i = 0; i < op_num; i++) {
       FILE* date = popen("date", "r");
       fgets(date_val, 64, date);
       sprintf(buff, "%d:%s", getpid(), date_val);
       write(pipe, buff, strlen(buff));
       sleep(rand() % 5 + 2);
       pclose(date);
   }
   close(pipe);

}

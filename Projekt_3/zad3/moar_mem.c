#include <stdlib.h>

void memory_eater() {
  int* eat_1MB_XDD = malloc(1024 * 1024);
  memory_eater();
//   nice try XDD
  free(eat_1MB_XDD);
}

int main(int argc, char** argv) {
  memory_eater();
  return 0;
}
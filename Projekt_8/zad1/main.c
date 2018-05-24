#define _DEFAULT_SOURCE
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int buff_size = 1024;

void load_matrix(char *buff, int **out_matrix, FILE *fp, int height, int width);
void free_matrix(int width, int heigth, int **matrix);
int **allocate_matrix(int width, int heigth);
void load_matrix_(char *buff, double **out_matrix, FILE *fp, int height,
                  int width);
void free_matrix_(int width, int heigth, double **matrix);
double **allocate_matrix_(int width, int heigth);
void save_picture(int width, int heigth, int **out_pict_matrix, FILE *fp);
void apply_filter(int begin, int end, int width, int heigth, int c,
                  int **pict_matrix, double **filter_matrix,
                  int **out_pict_matrix);
int calc_pixel(int x, int y, int width, int heigth, int c, int **I, double **K);

int main(int argc, char **argv) {
  if (argc < 5) {
    printf("threads count | in file path | filter path | out file name\n");
    exit(3);
  };
  char buff[buff_size];
  char *token, *string;
  int **pict_matrix;
  double **filter_matrix;
  int **out_pict_matrix;
  FILE *fp_in_pict = fopen(argv[2], "r+");
  FILE *fp_filter = fopen(argv[3], "r+");
  FILE *fp_out_pict = fopen(argv[4], "w+");

  if (fp_in_pict == NULL) {
    perror("failed to open the picture");
    exit(5);
  }

  if (fp_filter == NULL) {
    perror("failed to open the filter");
    exit(5);
  }

  if (fp_out_pict == NULL) {
    perror("failed to create the output pciture");
    exit(5);
  }

  fgets(buff, buff_size, fp_in_pict); // get rid of "P2"
  fgets(buff, buff_size, fp_in_pict);

  string = strdup(buff);
  token = strsep(&string, " \t");
  int width = strtol(token, NULL, 10);
  token = strsep(&string, " \t");
  int heigth = strtol(token, NULL, 10);

  pict_matrix = allocate_matrix(width, heigth);
  out_pict_matrix = allocate_matrix(width, heigth);

  fgets(buff, buff_size, fp_in_pict); // get rid of max pixel val

  load_matrix(buff, pict_matrix, fp_in_pict, heigth, width);

  fgets(buff, buff_size, fp_filter);
  int filter_size = strtol(buff, NULL, 10);
  filter_matrix = allocate_matrix_(filter_size, filter_size);

  load_matrix_(buff, filter_matrix, fp_filter, filter_size, filter_size);

  apply_filter(0, width, width, heigth, filter_size, pict_matrix, filter_matrix,
               out_pict_matrix);

  /*important stuff*/
  save_picture(width, heigth, out_pict_matrix, fp_out_pict);

  free_matrix(width, heigth, pict_matrix);
  free_matrix(width, heigth, out_pict_matrix);
  free_matrix_(filter_size, filter_size, filter_matrix);

  fclose(fp_in_pict);
  fclose(fp_out_pict);
  fclose(fp_filter);
}

void apply_filter(int begin, int end, int width, int heigth, int c,
                  int **pict_matrix, double **filter_matrix,
                  int **out_pict_matrix) {
  for (int y = 0; y < heigth; y++)
    for (int x = begin; x < end; x++) {
      out_pict_matrix[y][x] =
          calc_pixel(x, y, width, heigth, c, pict_matrix, filter_matrix);
    }
}

int calc_pixel(int x, int y, int width, int heigth, int c, int **I,
               double **K) {
  double pixel_val = 0;
  for (int j = 0; j < c; j++) {
    int b = round(fmax(0, y - ceil(c / 2) + j));
    b = b < heigth ? b : heigth - 1;
    for (int i = 0; i < c; i++) {
      int a = round(fmax(0, x - ceil(c / 2) + i));
      a = a < width ? a : width - 1;
      double v = I[b][a] * K[j][i];
      pixel_val += v;
    }
  }
  pixel_val = pixel_val < 0 ? 0 : pixel_val;
  printf("pixel val: %lf\n", pixel_val);
  return round(pixel_val);
}

void save_picture(int width, int heigth, int **out_pict_matrix, FILE *fp) {
  char buff[1024];
  fprintf(fp, "P2\n");
  fprintf(fp, "%d %d\n", width, heigth);
  fprintf(fp, "%d\n", 255);
  for (int i = 0; i < heigth; i++) {
    for (int j = 0; j < width; j++) {

      if (j < width - 1) {
        sprintf(buff, "%d ", out_pict_matrix[i][j]);
      } else {
        sprintf(buff, "%d\n", out_pict_matrix[i][j]);
      }
      fputs(buff, fp);
    }
  }
}

int **allocate_matrix(int width, int heigth) {
  int **matrix = calloc(heigth, sizeof(int *));
  for (int i = 0; i < heigth; i++) {
    matrix[i] = calloc(width, sizeof(int));
  }
  return matrix;
}

void free_matrix(int width, int heigth, int **matrix) {
  for (int i = 0; i < heigth; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

void load_matrix(char *buff, int **out_matrix, FILE *fp, int heigth,
                 int width) {
  int j = 0;
  int i = 0;
  while (fgets(buff, buff_size, fp) != NULL) {
    for (char *word = strtok(buff, " \n\t\r"); word != NULL;
         word = strtok(NULL, " \t\n\r")) {
      out_matrix[i][j] = strtol(word, NULL, 10);
      j++;
      i = (j < width) ? i : i + 1;
      j = (j < width) ? j : 0;
    }
  }
}
double **allocate_matrix_(int width, int heigth) {
  double **matrix = calloc(heigth, sizeof(double *));
  for (int i = 0; i < heigth; i++) {
    matrix[i] = calloc(width, sizeof(double));
  }
  return matrix;
}

void free_matrix_(int width, int heigth, double **matrix) {
  for (int i = 0; i < heigth; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

void load_matrix_(char *buff, double **out_matrix, FILE *fp, int heigth,
                  int width) {
  int j = 0;
  int i = 0;
  while (fgets(buff, buff_size, fp) != NULL) {
    for (char *word = strtok(buff, " \n\t\r"); word != NULL;
         word = strtok(NULL, " \t\n\r")) {
      out_matrix[i][j] = strtod(word, NULL);
      j++;
      i = (j < width) ? i : i + 1;
      j = (j < width) ? j : 0;
    }
  }
}

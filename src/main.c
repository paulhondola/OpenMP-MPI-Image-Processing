#include "bmp/bmp_io.h"
#include "convolution/convolution.h"
#include <limits.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslimits.h>

void get_args(int argc, char **argv, int *threads, char *input_file,
              char *output_file) {
  if (argc < 4) {
    printf(
        "Usage: mpirun -n <clusters> %s <threads> <input_file> <output_file>\n",
        argv[0]);
    exit(1);
  }

  *threads = atoi(argv[1]);
  strcpy(input_file, argv[2]);
  strcpy(output_file, argv[3]);
}

int main(int argc, char **argv) {
  char input_file[PATH_MAX];
  char output_file[PATH_MAX];
  int total_threads = 0;

  get_args(argc, argv, &total_threads, input_file, output_file);

  Image *img = read_BMP(input_file);

  if (!img) {
    printf("Error: Could not open file %s\n", input_file);
    return 2;
  }

  int original_width = img->width;
  int original_height = img->height;

  int result = convolve(img, BOXBLUR_KERNEL);

  if (!result) {
    printf("Error: Could not convolve image\n");
    return 2;
  }

  int new_width = img->width;
  int new_height = img->height;

  printf("Pre convolution: width: %d , height: %d\n", original_width,
         original_height);
  printf("Post convolution: width: %d , height: %d\n", new_width, new_height);

  save_BMP(output_file, img);

  free_BMP(img);

  return 0;
}

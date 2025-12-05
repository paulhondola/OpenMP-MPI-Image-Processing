#include "benchmark.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

const char *BASE_FOLDER = "images/base";
const char *SERIAL_FOLDER = "serial";
const char *PARALLEL_FOLDER = "parallel";

// Define a struct to map kernel names to their folder and kernel data
typedef struct {
  const char *name;
  const char *folder;
  const Kernel *kernel_ptr;
} BenchmarkKernel;

// Kernels and their output folders
// Note: Pointers to likely extern Kernels defined in kernel.h
BenchmarkKernel kernels[] = {
    {"Ridge", "images/ridge", &RIDGE_KERNEL},
    {"Edge", "images/edge", &EDGE_KERNEL},
    {"Sharpen", "images/sharpen", &SHARPEN_KERNEL},
    {"BoxBlur", "images/boxblur", &BOXBLUR_KERNEL},
    {"Gaussian3", "images/gaussblur3", &GAUSSIAN3_KERNEL},
    {"Gaussian5", "images/gaussblur5", &GAUSSIAN5_KERNEL},
    {"Unsharp5", "images/unsharp5", &UNSHARP5_KERNEL}};
int num_kernels = 7;

// Input files (located in images/base)
const char *files[] = {"Large.bmp", "XL.bmp", "XXL.bmp"};
int num_files = 3;

int create_directory(const char *path) {
  if (mkdir(path, 0777) != 0) {
    // if the directory already exists, ignore the error
    if (errno == EEXIST) {
      return 1;
    }
    fprintf(stderr, "Error: Could not create directory %s\n", path);
    return 0;
  }
  return 1;
}

int create_directories(void) {
  for (int k = 0; k < num_kernels; k++) {
    BenchmarkKernel bk = kernels[k];
    if (!create_directory(bk.folder)) {
      return 0;
    }
  }
  return 1;
}

void run_benchmark_kernel(const char *input_path, const char *img_name,
                          BenchmarkKernel bk) {
  // 1. Read the clean image every time because convolve modifies it in
  // place
  printf("\tApplying kernel: %s\n", bk.name);

  Image *img = read_BMP(input_path);

  if (!img) {
    fprintf(stderr, "\t\tError: Could not read file %s\n", input_path);
    return;
  }

  // 2. Convolve
  if (!convolve(img, *bk.kernel_ptr)) {
    fprintf(stderr, "\t\tError: Convolution failed for %s on %s\n", bk.name,
            img_name);
    free_BMP(img);
    return;
  }

  // 3. Save
  char output_path[PATH_MAX];
  sprintf(output_path, "%s/%s/%s", bk.folder, SERIAL_FOLDER, img_name);
  if (!save_BMP(output_path, img)) {
    fprintf(stderr, "\t\tError: Could not save to %s\n", output_path);
  } else {
    printf("\t\tSaved to: %s\n", output_path);
  }

  // 4. Free
  free_BMP(img);
}

void run_benchmark_serial(void) {

  printf("Starting benchmark...\n");

  if (!create_directories()) {
    fprintf(stderr, "Error: Could not create directories\n");
    return;
  }

  for (int f = 0; f < num_files; f++) {
    const char *img_name = files[f];
    char input_path[PATH_MAX];
    sprintf(input_path, "%s/%s", BASE_FOLDER, img_name);

    printf("Processing file: %s\n", input_path);
    for (int k = 0; k < num_kernels; k++) {
      run_benchmark_kernel(input_path, img_name, kernels[k]);
    }
  }

  printf("Benchmark completed.\n");
}

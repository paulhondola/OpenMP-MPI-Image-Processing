#include "benchmark.h"
#include "../bmp/bmp_io.h"
#include "../convolution/convolution.h"
#include "../file_utils/file_utils.h"
#include "../kernel/kernel.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

const char *IMAGES_FOLDER = "images";
const char *BASE_FOLDER = "base";
const char *SERIAL_FOLDER = "serial";
const char *PARALLEL_FOLDER = "parallel";

// Define a struct to map kernel names to their directories and kernel data
typedef struct {
  const char *name;
  const char *directory;
  const Kernel *kernel_ptr;
} benchmark_kernel;

// Kernels and their output folders
// Pointers to extern Kernels defined in kernel.h
const benchmark_kernel kernels[] = {
    {"Ridge", "ridge", &RIDGE_KERNEL},
    {"Edge", "edge", &EDGE_KERNEL},
    {"Sharpen", "sharpen", &SHARPEN_KERNEL},
    {"BoxBlur", "boxblur", &BOXBLUR_KERNEL},
    {"Gaussian3", "gaussblur3", &GAUSSIAN3_KERNEL},
    {"Gaussian5", "gaussblur5", &GAUSSIAN5_KERNEL},
    {"Unsharp5", "unsharp5", &UNSHARP5_KERNEL}};
const int num_kernels = sizeof(kernels) / sizeof(kernels[0]);

// Input files (located in images/base)
const char *files[] = {"Large.bmp", "XL.bmp", "XXL.bmp"};
const int num_files = sizeof(files) / sizeof(files[0]);

app_error create_directories(void) {
  char path[PATH_MAX];
  for (int k = 0; k < num_kernels; k++) {
    const char *dir = kernels[k].directory;

    // Create kernel folder
    snprintf(path, PATH_MAX, "%s/%s", IMAGES_FOLDER, dir);
    if (create_directory(path) != SUCCESS) {
      return ERR_DIR_CREATE;
    }

    // Create serial folder
    snprintf(path, PATH_MAX, "%s/%s/%s", IMAGES_FOLDER, dir, SERIAL_FOLDER);
    if (create_directory(path) != SUCCESS) {
      return ERR_DIR_CREATE;
    }

    // Create parallel folder
    snprintf(path, PATH_MAX, "%s/%s/%s", IMAGES_FOLDER, dir, PARALLEL_FOLDER);
    if (create_directory(path) != SUCCESS) {
      return ERR_DIR_CREATE;
    }
  }
  return SUCCESS;
}

app_error run_benchmark_kernel(const char *input_path, const char *img_name,
                               benchmark_kernel bk) {
  // 1. Read the clean image every time because convolve modifies it in
  // place
  printf("\tApplying kernel: %s\n", bk.name);

  Image *img = NULL;
  app_error err = SUCCESS;

  err = read_BMP(input_path, &img);

  if (err != SUCCESS) {
    fprintf(stderr, "\t\tError: Could not read file %s: %s\n", input_path,
            get_error_string(err));
    return err;
  }

  // 2. Convolve
  err = convolve(img, *bk.kernel_ptr);
  if (err != SUCCESS) {
    fprintf(stderr, "\t\tError: Convolution failed for %s on %s: %s\n", bk.name,
            img_name, get_error_string(err));
    free_BMP(img);
    return err;
  }

  // 3. Save
  char output_path[PATH_MAX];
  snprintf(output_path, PATH_MAX, "%s/%s/%s/%s", IMAGES_FOLDER, bk.directory,
           SERIAL_FOLDER, img_name);
  err = save_BMP(output_path, img);
  if (err != SUCCESS) {
    fprintf(stderr, "\t\tError: Could not save to %s: %s\n", output_path,
            get_error_string(err));
    free_BMP(img);
    return err;
  }

  printf("\t\tSaved to: %s\n", output_path);

  // 4. Free
  free_BMP(img);

  return SUCCESS;
}

app_error run_benchmark_serial(void) {

  printf("\nStarting serial benchmark\n");

  app_error err = SUCCESS;

  err = create_directories();
  if (err != SUCCESS) {
    return err;
  }

  for (int f = 0; f < num_files; f++) {
    const char *img_name = files[f];
    char input_path[PATH_MAX];
    snprintf(input_path, PATH_MAX, "%s/%s/%s", IMAGES_FOLDER, BASE_FOLDER,
             img_name);

    printf("\nProcessing file: %s\n", input_path);
    for (int k = 0; k < num_kernels; k++) {
      err = run_benchmark_kernel(input_path, img_name, kernels[k]);
      if (err != SUCCESS) {
        return err;
      }
    }
  }

  printf("\nSerial benchmark completed\n");

  return SUCCESS;
}

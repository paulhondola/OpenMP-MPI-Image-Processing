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

const char *BASE_FOLDER = "images/base";
const char *SERIAL_FOLDER = "serial";
const char *PARALLEL_FOLDER = "parallel";

// Define a struct to map kernel names to their folder and kernel data
typedef struct {
  const char *name;
  const char *folder;
  const Kernel *kernel_ptr;
} benchmark_kernel;

// Kernels and their output folders
// Pointers to extern Kernels defined in kernel.h
const benchmark_kernel kernels[] = {
    {"Ridge", "images/ridge", &RIDGE_KERNEL},
    {"Edge", "images/edge", &EDGE_KERNEL},
    {"Sharpen", "images/sharpen", &SHARPEN_KERNEL},
    {"BoxBlur", "images/boxblur", &BOXBLUR_KERNEL},
    {"Gaussian3", "images/gaussblur3", &GAUSSIAN3_KERNEL},
    {"Gaussian5", "images/gaussblur5", &GAUSSIAN5_KERNEL},
    {"Unsharp5", "images/unsharp5", &UNSHARP5_KERNEL}};
const int num_kernels = sizeof(kernels) / sizeof(kernels[0]);

// Input files (located in images/base)
const char *files[] = {"Large.bmp", "XL.bmp", "XXL.bmp"};
const int num_files = sizeof(files) / sizeof(files[0]);

app_error create_directories(void) {
  char path[PATH_MAX];
  for (int k = 0; k < num_kernels; k++) {
    const char *folder = kernels[k].folder;

    // Create kernel folder
    snprintf(path, sizeof(path), "%s", folder);
    if (create_directory_recursive(path) != SUCCESS) {
      return ERR_DIR_CREATE;
    }

    // Create serial folder
    snprintf(path, sizeof(path), "%s/%s", folder, SERIAL_FOLDER);
    if (create_directory_recursive(path) != SUCCESS) {
      return ERR_DIR_CREATE;
    }

    // Create parallel folder
    snprintf(path, sizeof(path), "%s/%s", folder, PARALLEL_FOLDER);
    if (create_directory_recursive(path) != SUCCESS) {
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
  app_error err = read_BMP(input_path, &img);

  if (err != SUCCESS) {
    fprintf(stderr, "\t\tError: Could not read file %s: %s\n", input_path,
            get_error_string(err));
    return err;
  }

  // 2. Convolve
  if (convolve(img, *bk.kernel_ptr) != SUCCESS) {
    fprintf(stderr, "\t\tError: Convolution failed for %s on %s\n", bk.name,
            img_name);
    free_BMP(img);
    return ERR_CONVOLUTION;
  }

  // 3. Save
  char output_path[PATH_MAX];
  snprintf(output_path, PATH_MAX, "%s/%s/%s", bk.folder, SERIAL_FOLDER,
           img_name);
  if (save_BMP(output_path, img) != SUCCESS) {
    fprintf(stderr, "\t\tError: Could not save to %s\n", output_path);
    return ERR_FILE_WRITE;
  }

  printf("\t\tSaved to: %s\n", output_path);

  // 4. Free
  free_BMP(img);

  return SUCCESS;
}

app_error run_benchmark_serial(void) {

  printf("\nStarting serial benchmark\n");

  if (create_directories() != SUCCESS) {
    fprintf(stderr, "Error: Could not create directories\n");
    return ERR_DIR_CREATE;
  }

  for (int f = 0; f < num_files; f++) {
    const char *img_name = files[f];
    char input_path[PATH_MAX];
    snprintf(input_path, PATH_MAX, "%s/%s", BASE_FOLDER, img_name);

    printf("\nProcessing file: %s\n", input_path);
    for (int k = 0; k < num_kernels; k++) {
      app_error err = run_benchmark_kernel(input_path, img_name, kernels[k]);
      if (err != SUCCESS) {
        return err;
      }
    }
  }

  printf("\nSerial benchmark completed\n");

  return SUCCESS;
}

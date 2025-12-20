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

typedef app_error (*convolve_function)(Image *, Kernel);

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

// Level 1: Run specific kernel on an image
app_error run_single_kernel(Image *img, const char *img_name,
                            benchmark_kernel bk, const char *folder,
                            convolve_function cv_fn) {
  printf("\tApplying kernel: %s\n", bk.name);

  app_error err = cv_fn(img, *bk.kernel_ptr);
  if (err != SUCCESS) {
    fprintf(stderr, "\t\tError: Convolution failed for %s on %s: %s\n", bk.name,
            img_name, get_error_string(err));
    return err;
  }

  char output_path[PATH_MAX];
  snprintf(output_path, PATH_MAX, "%s/%s/%s/%s", IMAGES_FOLDER, bk.directory,
           folder, img_name);

  err = save_BMP(img, output_path);
  if (err != SUCCESS) {
    fprintf(stderr, "\t\tError: Could not save to %s: %s\n", output_path,
            get_error_string(err));
    return err;
  }

  printf("\t\tSaved to: %s\n", output_path);
  return SUCCESS;
}

// Level 2: Run all kernels on an image
app_error run_all_kernels(Image *base_img, const char *img_name,
                          const char *folder, convolve_function cv_fn) {
  app_error err = SUCCESS;

  for (int k = 0; k < num_kernels; k++) {
    Image *working_img = NULL;
    // Create a fresh copy for each kernel since convolution is in-place
    err = copy_image(base_img, &working_img);
    if (err != SUCCESS) {
      fprintf(stderr, "\tError: Could not copy image: %s\n",
              get_error_string(err));
      return err;
    }

    err = run_single_kernel(working_img, img_name, kernels[k], folder, cv_fn);

    // Always free the working copy
    free_BMP(working_img);

    if (err != SUCCESS) {
      return err;
    }
  }
  return SUCCESS;
}

// Level 3: Run on all existing files
app_error run_all_files(const char *folder, convolve_function cv_fn) {
  app_error err = create_directories();
  if (err != SUCCESS) {
    return err;
  }

  for (int f = 0; f < num_files; f++) {
    const char *img_name = files[f];
    char input_path[PATH_MAX];
    snprintf(input_path, PATH_MAX, "%s/%s/%s", IMAGES_FOLDER, BASE_FOLDER,
             img_name);

    printf("\nProcessing file: %s\n", input_path);

    // Load base image once per file
    Image *base_img = NULL;
    // Note: read_BMP takes (Image **, const char *) based on previous edits
    err = read_BMP(&base_img, input_path);
    if (err != SUCCESS) {
      fprintf(stderr, "\tError: Could not read base file %s: %s\n", input_path,
              get_error_string(err));
      return err;
    }

    // Run all kernels on this file
    err = run_all_kernels(base_img, img_name, folder, cv_fn);

    // Free base image
    free_BMP(base_img);

    if (err != SUCCESS) {
      return err;
    }
  }
  return SUCCESS;
}

// Wrapper for Serial Benchmark
app_error run_benchmark_serial(void) {
  printf("\n--- Starting Serial Benchmark ---\n");
  return run_all_files(SERIAL_FOLDER, convolve_serial);
}

// Wrapper for Parallel Benchmark
app_error run_benchmark_parallel(void) {
  printf("\n--- Starting Parallel Benchmark ---\n");
  return run_all_files(PARALLEL_FOLDER, convolve_parallel);
}

app_error run_verification(void) {
  printf("\n--- Starting Verification ---\n");
  app_error err = SUCCESS;
  int mismatches = 0;

  for (int f = 0; f < num_files; f++) {
    const char *img_name = files[f];
    printf("\nVerifying file: %s\n", img_name);

    for (int k = 0; k < num_kernels; k++) {
      const char *dir = kernels[k].directory;
      char serial_path[PATH_MAX];
      char parallel_path[PATH_MAX];

      snprintf(serial_path, PATH_MAX, "%s/%s/%s/%s", IMAGES_FOLDER, dir,
               SERIAL_FOLDER, img_name);
      snprintf(parallel_path, PATH_MAX, "%s/%s/%s/%s", IMAGES_FOLDER, dir,
               PARALLEL_FOLDER, img_name);

      Image *img_serial = NULL;
      Image *img_parallel = NULL;

      if (read_BMP(&img_serial, serial_path) != SUCCESS) {
        fprintf(stderr, "\tError reading serial output: %s\n", serial_path);
        mismatches++;
        continue;
      }

      if (read_BMP(&img_parallel, parallel_path) != SUCCESS) {
        fprintf(stderr, "\tError reading parallel output: %s\n", parallel_path);
        free_BMP(img_serial);
        mismatches++;
        continue;
      }

      if (check_images_match(img_serial, img_parallel) != SUCCESS) {
        fprintf(stderr, "\tMismatch found in kernel %s\n", kernels[k].name);
        mismatches++;
      } else {
        printf("\t%s: Match\n", kernels[k].name);
      }

      free_BMP(img_serial);
      free_BMP(img_parallel);
    }
  }

  if (mismatches > 0) {
    fprintf(stderr, "\nVerification completed with %d mismatches\n",
            mismatches);
    return ERR_IMAGE_DIFFERENCE;
  }

  printf("\nVerification completed successfully. All images match.\n");
  return SUCCESS;
}

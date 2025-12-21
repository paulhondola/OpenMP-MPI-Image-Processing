#include "kernel_run.h"
#include "../file_utils/file_utils.h"
#include <limits.h>
#include <stdio.h>

// Level 0: Create directories
app_error create_directories(void) {
  char path[PATH_MAX];
  for (int k = 0; k < NUM_KERNELS; k++) {
    const char *dir = CONV_KERNELS[k].directory;

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
    snprintf(path, PATH_MAX, "%s/%s/%s", IMAGES_FOLDER, dir,
             PARALLEL_DISTRIBUTED_FS_FOLDER);
    if (create_directory(path) != SUCCESS) {
      return ERR_DIR_CREATE;
    }

    snprintf(path, PATH_MAX, "%s/%s/%s", IMAGES_FOLDER, dir,
             PARALLEL_SHARED_FS_FOLDER);
    if (create_directory(path) != SUCCESS) {
      return ERR_DIR_CREATE;
    }
  }
  return SUCCESS;
}

// Level 1: Run a single kernel on a single file
app_error run_single_kernel(Image *img, const char *img_name, Kernel kernel,
                            const char *benchmark_type_folder,
                            convolve_function cv_fn) {
  printf("\tApplying kernel: %s\n", kernel.name);

  app_error err = cv_fn(img, kernel);
  if (err != SUCCESS) {
    fprintf(stderr, "\t\tError: Convolution failed for %s on %s: %s\n",
            kernel.name, img_name, get_error_string(err));
    return err;
  }

  char output_path[PATH_MAX];
  snprintf(output_path, PATH_MAX, "%s/%s/%s/%s", IMAGES_FOLDER,
           kernel.directory, benchmark_type_folder, img_name);

  err = save_BMP(img, output_path);
  if (err != SUCCESS) {
    fprintf(stderr, "\t\tError: Could not save to %s: %s\n", output_path,
            get_error_string(err));
    return err;
  }

  printf("\t\tSaved to: %s\n", output_path);
  return SUCCESS;
}

// Level 2: Run all kernels on a single file
app_error run_all_kernels(Image *base_img, const char *img_name,
                          const char *benchmark_type_folder,
                          convolve_function cv_fn) {
  app_error err = SUCCESS;

  for (int k = 0; k < NUM_KERNELS; k++) {
    Image *working_img = NULL;
    // Create a fresh copy for each kernel since convolution is in-place
    err = copy_image(base_img, &working_img);
    if (err != SUCCESS) {
      fprintf(stderr, "\tError: Could not copy image: %s\n",
              get_error_string(err));
      return err;
    }

    err = run_single_kernel(working_img, img_name, CONV_KERNELS[k],
                            benchmark_type_folder, cv_fn);

    // Always free the working copy
    free_BMP(working_img);

    if (err != SUCCESS) {
      return err;
    }
  }
  return SUCCESS;
}

// Level 3: Run on all existing files
app_error run_all_files(const char *benchmark_type_folder,
                        convolve_function cv_fn) {
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
    err = run_all_kernels(base_img, img_name, benchmark_type_folder, cv_fn);

    // Free base image
    free_BMP(base_img);

    if (err != SUCCESS) {
      return err;
    }
  }
  return SUCCESS;
}
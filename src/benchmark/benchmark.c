#include "benchmark.h"
#include "../bmp/bmp_io.h"
#include "../convolution/convolution.h"
#include "../file_utils/file_utils.h"
#include "../kernel/kernel.h"
#include "kernel_run.h"
#include <limits.h>

app_error run_benchmark_serial(void) {
  printf("\n--- Starting Serial Benchmark ---\n");
  return run_all_files(SERIAL_FOLDER, convolve_parallel_multithreaded);
}

app_error run_benchmark_parallel_shared_fs(void) {
  printf("\n--- Starting Parallel Benchmark (Shared Filesystem) ---\n");
  return run_all_files(PARALLEL_SHARED_FS_FOLDER,
                       convolve_parallel_multithreaded);
}

app_error run_benchmark_parallel_distributed_fs(void) {
  printf("\n--- Starting Parallel Benchmark (Distributed Filesystem) ---\n");
  return run_all_files(PARALLEL_DISTRIBUTED_FS_FOLDER,
                       convolve_parallel_multithreaded);
}

app_error run_verification(void) {
  printf("\n--- Starting Verification ---\n");

  int mismatches = 0;

  for (int f = 0; f < num_files; f++) {
    const char *img_name = files[f];
    printf("\nVerifying file: %s\n", img_name);

    for (int k = 0; k < NUM_KERNELS; k++) {
      const char *dir = CONV_KERNELS[k].directory;
      char serial_path[PATH_MAX];
      char parallel_shared_path[PATH_MAX];
      char parallel_distributed_path[PATH_MAX];

      snprintf(serial_path, PATH_MAX, "%s/%s/%s/%s", IMAGES_FOLDER, dir,
               SERIAL_FOLDER, img_name);
      snprintf(parallel_shared_path, PATH_MAX, "%s/%s/%s/%s", IMAGES_FOLDER,
               dir, PARALLEL_SHARED_FS_FOLDER, img_name);
      snprintf(parallel_distributed_path, PATH_MAX, "%s/%s/%s/%s",
               IMAGES_FOLDER, dir, PARALLEL_DISTRIBUTED_FS_FOLDER, img_name);

      Image *img_serial = NULL;
      Image *img_parallel_shared = NULL;
      Image *img_parallel_distributed = NULL;

      if (read_BMP(&img_serial, serial_path) != SUCCESS) {
        fprintf(stderr, "\tError reading serial output: %s\n", serial_path);
        mismatches++;
        continue;
      }

      if (read_BMP(&img_parallel_shared, parallel_shared_path) != SUCCESS) {
        fprintf(stderr, "\tError reading parallel shared output: %s\n",
                parallel_shared_path);
        free_BMP(img_serial);
        mismatches++;
        continue;
      }

      if (read_BMP(&img_parallel_distributed, parallel_distributed_path) !=
          SUCCESS) {
        fprintf(stderr, "\tError reading parallel distributed output: %s\n",
                parallel_distributed_path);
        free_BMP(img_serial);
        free_BMP(img_parallel_shared);
        mismatches++;
        continue;
      }

      if (check_images_match(img_serial, img_parallel_shared) != SUCCESS) {
        fprintf(stderr, "\tMismatch found in kernel %s\n",
                CONV_KERNELS[k].name);
        mismatches++;
      } else {
        printf("\t%s: Match\n", CONV_KERNELS[k].name);
      }

      if (check_images_match(img_serial, img_parallel_distributed) != SUCCESS) {
        fprintf(stderr, "\tMismatch found in kernel %s\n",
                CONV_KERNELS[k].name);
        mismatches++;
      } else {
        printf("\t%s: Match\n", CONV_KERNELS[k].name);
      }

      free_BMP(img_serial);
      free_BMP(img_parallel_shared);
      free_BMP(img_parallel_distributed);
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

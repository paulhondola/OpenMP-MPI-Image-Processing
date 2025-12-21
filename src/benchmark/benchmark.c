#include "benchmark.h"
#include "../bmp/bmp_io.h"
#include "../constants/files.h"
#include "../constants/kernel.h"
#include "../convolution/convolution.h"
#include "kernel_run.h"
#include <limits.h>
#include <mpi.h>

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

app_error run_benchmark_serial(void) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    printf("\n--- Starting Serial Benchmark ---\n");
    app_error err =
        run_all_files(SERIAL_FOLDER, convolve_serial, benchmark_data[0]);
    return err;
  }
  return SUCCESS;
}

app_error run_benchmark_parallel_multithreaded(void) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    printf("\n--- Starting Parallel Benchmark (Multithreaded) ---\n");
    return run_all_files(PARALLEL_MULTITHREADED_FOLDER,
                         convolve_parallel_multithreaded, benchmark_data[1]);
  }
  return SUCCESS;
}

app_error run_benchmark_parallel_distributed_fs(void) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    printf("\n--- Starting Parallel Benchmark (Distributed Filesystem) ---\n");
  }
  // All ranks participate in Distributed FS benchmark
  return run_all_files(PARALLEL_DISTRIBUTED_FS_FOLDER,
                       convolve_parallel_distributed_filesystem,
                       benchmark_data[2]);
}

app_error run_benchmark_parallel_shared_fs(void) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    printf("\n--- Starting Parallel Benchmark (Shared Filesystem) ---\n");
    return run_all_files(PARALLEL_SHARED_FS_FOLDER,
                         convolve_parallel_shared_filesystem,
                         benchmark_data[3]);
  }
  return SUCCESS;
}

app_error verify_implementation(const char *kernel_dir, const char *impl_folder,
                                const char *img_name, Image *img_serial,
                                int *mismatches) {
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s/%s/%s", IMAGES_FOLDER, kernel_dir,
           impl_folder, img_name);

  Image *img_parallel = NULL;
  app_error err = read_BMP(&img_parallel, path);
  if (err) {
    fprintf(stderr, "\tError reading %s output: %s\n", impl_folder, path);
    (*mismatches)++;
    return err;
  }

  err = check_images_match(img_serial, img_parallel);
  if (err) {
    fprintf(stderr, RED "\tMismatch found in kernel %s (%s)\n" RESET,
            kernel_dir, impl_folder);
    (*mismatches)++;
  } else {
    printf(GREEN "\t%s (%s): Match\n" RESET, kernel_dir, impl_folder);
  }

  free_BMP(img_parallel);
  return err;
}

app_error run_verification(void) {
  printf("\n--- Starting Verification ---\n");

  int mismatches = 0;
  app_error err = SUCCESS;
  for (int f = 0; f < BENCHMARK_FILES; f++) {
    const char *img_name = files[f];
    printf("\nVerifying file: %s\n", img_name);

    for (int k = 0; k < KERNEL_TYPES; k++) {
      const char *kernel_name = CONV_KERNELS[k].name;
      char serial_path[PATH_MAX];

      snprintf(serial_path, PATH_MAX, "%s/%s/%s/%s", IMAGES_FOLDER, kernel_name,
               SERIAL_FOLDER, img_name);

      Image *img_serial = NULL;

      err = read_BMP(&img_serial, serial_path);
      if (err) {
        fprintf(stderr, "\tError reading serial output: %s | ERROR CODE: %s\n",
                serial_path, get_error_string(err));
        mismatches++;
        continue;
      }

      err = verify_implementation(kernel_name, PARALLEL_MULTITHREADED_FOLDER,
                                  img_name, img_serial, &mismatches);

      if (err)
        fprintf(stderr, "%s\n", get_error_string(err));

      (void)verify_implementation(kernel_name, PARALLEL_DISTRIBUTED_FS_FOLDER,
                                  img_name, img_serial, &mismatches);
      (void)verify_implementation(kernel_name, PARALLEL_SHARED_FS_FOLDER,
                                  img_name, img_serial, &mismatches);

      free_BMP(img_serial);
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

#include "benchmark/benchmark.h"
#include "bmp/bmp_io.h"
#include "constants/files.h"
#include "constants/kernel.h"
#include "errors/errors.h"
#include "file_utils/file_utils.h"
#include <limits.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_THREAD_COUNT 10

void init_mpi(int argc, char **argv, int *comm_rank, int *comm_size,
              int *omp_threads) {
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, comm_rank);
  MPI_Comm_size(MPI_COMM_WORLD, comm_size);

  if (argc < 2) {
    *omp_threads = DEFAULT_THREAD_COUNT;
  } else {
    *omp_threads = atoi(argv[1]);
  }

  omp_set_num_threads(*omp_threads);
}

int main(int argc, char **argv) {
  int comm_rank, comm_size, omp_threads;
  init_mpi(argc, argv, &comm_rank, &comm_size, &omp_threads);

  if (comm_rank == 0) {
    printf("Running with %d MPI processes and %d OpenMP threads per process\n",
           comm_size, omp_threads);
    printf("Filesystem Mode: Distributed\n");
  }

  app_error err = SUCCESS;

  // RUN SERIAL
  err = run_benchmark_serial();
  if (err != SUCCESS) {
    if (comm_rank == 0)
      fprintf(stderr, "Serial benchmark failed with error: %s\n",
              get_error_string(err));
    MPI_Finalize();
    return 1;
  }

#ifdef TEST_MULTITHREADED
  err = run_benchmark_parallel_multithreaded();
  if (err != SUCCESS) {
    if (comm_rank == 0)
      fprintf(stderr,
              "Parallel benchmark (Multithreaded) failed with error: %s\n",
              get_error_string(err));
    MPI_Finalize();
    return 2;
  }
#endif

#ifdef TEST_DISTRIBUTED_FS
  err = run_benchmark_parallel_distributed_fs();
  if (err != SUCCESS) {
    if (comm_rank == 0)
      fprintf(
          stderr,
          "Parallel benchmark (Distributed Filesystem) failed with error: %s\n",
          get_error_string(err));
    MPI_Finalize();
    return 3;
  }
#endif

#ifdef TEST_SHARED_FS
  err = run_benchmark_parallel_shared_fs();
  if (err != SUCCESS) {
    if (comm_rank == 0)
      fprintf(stderr,
              "Parallel benchmark (Shared Filesystem) failed with error: %s\n",
              get_error_string(err));
    MPI_Finalize();
    return 4;
  }
#endif

  if (comm_rank == 0) {
    err = run_verification();
    if (err != SUCCESS) {
      fprintf(stderr, "Verification failed with error: %s\n",
              get_error_string(err));
      MPI_Finalize();
      return -1;
    }

    // Write results to CSV
    err = init_benchmark_csv(CSV_FILE);
    if (err != SUCCESS) {
      fprintf(stderr, "Failed to initialize CSV file: %s\n",
              get_error_string(err));
    } else {
      for (int f = 0; f < BENCHMARK_FILES; f++) {
        // Read file to get dimensions
        char input_path[PATH_MAX];
        snprintf(input_path, PATH_MAX, "%s/%s/%s", IMAGES_FOLDER, BASE_FOLDER,
                 files[f]);
        Image *img = NULL;
        if (read_BMP(&img, input_path) == SUCCESS) {
          int width = img->width;
          int height = img->height;

          for (int k = 0; k < KERNEL_TYPES; k++) {
            double serial_time = benchmark_data[0][f][k];
            double multithreaded_time = benchmark_data[1][f][k];
            double distributed_time = benchmark_data[2][f][k];
            double shared_time = benchmark_data[3][f][k];

            append_benchmark_result(
                CSV_FILE, width, height, CONV_KERNELS[k].name, comm_size,
                omp_threads, serial_time, multithreaded_time, distributed_time,
                shared_time);
          }
          free_BMP(img);
        } else {
          fprintf(stderr, "Failed to read image %s for info\n", input_path);
        }
      }
      printf("\nBenchmark results written to %s\n", CSV_FILE);
    }
  }

  MPI_Finalize();
  return 0;
}

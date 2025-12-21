#include "benchmark/benchmark.h"
#include "errors/errors.h"
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RUN_TESTS

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

#ifdef RUN_TESTS

  // RUN SERIAL
  err = run_benchmark_serial();
  if (err != SUCCESS) {
    if (comm_rank == 0)
      fprintf(stderr, "Serial benchmark failed with error: %s\n",
              get_error_string(err));
    MPI_Finalize();
    return 1;
  }

  // RUN PARALLEL MULTITHREADED
  err = run_benchmark_parallel_multithreaded();
  if (err != SUCCESS) {
    if (comm_rank == 0)
      fprintf(stderr,
              "Parallel benchmark (Multithreaded) failed with error: %s\n",
              get_error_string(err));
    MPI_Finalize();
    return 2;
  }

  // RUN PARALLEL SHARED FS
  err = run_benchmark_parallel_shared_fs();
  if (err != SUCCESS) {
    if (comm_rank == 0)
      fprintf(stderr,
              "Parallel benchmark (Shared Filesystem) failed with error: %s\n",
              get_error_string(err));
    MPI_Finalize();
    return 3;
  }

  // RUN PARALLEL DISTRIBUTED FS
  err = run_benchmark_parallel_distributed_fs();
  if (err != SUCCESS) {
    if (comm_rank == 0)
      fprintf(
          stderr,
          "Parallel benchmark (Distributed Filesystem) failed with error: %s\n",
          get_error_string(err));
    MPI_Finalize();
    return 4;
  }
#endif

  // CHECK THE IMAGES MATCH AGAINST THE SERIAL OUTPUT
  if (comm_rank == 0) {
    err = run_verification();
    if (err != SUCCESS) {
      fprintf(stderr, "Verification failed with error: %s\n",
              get_error_string(err));
      MPI_Finalize();
      return -1;
    }
  }

  MPI_Finalize();
  return 0;
}

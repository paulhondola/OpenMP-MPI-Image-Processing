#include "benchmark/benchmark.h"
#include "errors/errors.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RUN_TESTS

int main(int argc, char **argv) {
  int threads = 1;
  int shared_fs = 0;

  if (argc > 1) {
    // Check if first arg is threads or flag
    if (argv[1][0] == '-') {
      // flag
      if (strcmp(argv[1], "--shared-fs") == 0)
        shared_fs = 1;
    } else {
      threads = atoi(argv[1]);
    }
  }

  if (argc > 2) {
    if (strcmp(argv[2], "--shared-fs") == 0)
      shared_fs = 1;
  }

  omp_set_num_threads(threads);
  printf("Running with %d threads\n", threads);
  if (shared_fs) {
    printf("Filesystem Mode: Shared\n");
  } else {
    printf("Filesystem Mode: Distributed (Non-Shared)\n");
  }

  app_error err = SUCCESS;

#ifdef RUN_TESTS
  err = run_benchmark_serial();
  if (err != SUCCESS) {
    fprintf(stderr, "Serial benchmark failed with error: %s\n",
            get_error_string(err));
    return 1;
  }

  err = run_benchmark_parallel_shared_fs();
  if (err != SUCCESS) {
    fprintf(stderr,
            "Parallel benchmark (Shared Filesystem) failed with error: %s\n",
            get_error_string(err));
    return 1;
  }

  err = run_benchmark_parallel_distributed_fs();
  if (err != SUCCESS) {
    fprintf(
        stderr,
        "Parallel benchmark (Distributed Filesystem) failed with error: %s\n",
        get_error_string(err));
    return 1;
  }
#endif

  err = run_verification();
  if (err != SUCCESS) {
    fprintf(stderr, "Verification failed with error: %s\n",
            get_error_string(err));
    return 3;
  }

  return 0;
}

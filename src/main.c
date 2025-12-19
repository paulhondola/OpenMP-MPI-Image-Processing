#include "benchmark/benchmark.h"
#include "errors/errors.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  int threads = 1;
  if (argc > 1) {
    threads = atoi(argv[1]);
  }

  omp_set_num_threads(threads);
  printf("Running with %d threads\n", threads);

  app_error err = run_benchmark_serial();
  if (err != SUCCESS) {
    fprintf(stderr, "Serial benchmark failed with error: %s\n",
            get_error_string(err));
    return 1;
  }

  err = run_benchmark_parallel();
  if (err != SUCCESS) {
    fprintf(stderr, "Parallel benchmark failed with error: %s\n",
            get_error_string(err));
    return 1;
  }

  return 0;
}

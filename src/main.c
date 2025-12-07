#include "benchmark/benchmark.h"
#include "errors/errors.h"
#include <stdio.h>

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  app_error err = run_benchmark_serial();
  if (err != SUCCESS) {
    fprintf(stderr, "Benchmark failed with error: %s\n", get_error_string(err));
    return 1;
  }
  return 0;
}

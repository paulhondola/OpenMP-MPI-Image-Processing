#include "benchmark/benchmark.h"
#include <limits.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  run_benchmark_serial();
  return 0;
}

#include "benchmark/benchmark.h"
#include "bmp/bmp_io.h"
#include "constants/files.h"
#include "constants/kernel.h"
#include "errors/errors.h"
#include "file_utils/file_utils.h"
#include <limits.h>
#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_THREAD_COUNT 10

void print_usage(const char *prog_name) {
  printf("Usage: %s [options]\n", prog_name);
  printf("Options:\n");
  printf("  -t <n>  Set number of OpenMP threads (default: %d)\n",
         DEFAULT_THREAD_COUNT);
  printf("  -s      Run Serial benchmark\n");
  printf("  -m      Run Parallel Multithreaded benchmark\n");
  printf("  -d      Run Parallel Distributed Filesystem benchmark\n");
  printf("  -h      Run Parallel Shared Filesystem benchmark\n");
  printf("  -p      Run Parallel Task Pool benchmark\n");
  printf("  -a      Run All benchmarks\n");
  printf("  --help  Show this help message\n");
  printf("\nIf no mode flags are provided, Distributed mode (-d) is run by "
         "default.\n");
}

void parse_args(int argc, char **argv, BenchmarkConfig *config) {
  config->omp_threads = DEFAULT_THREAD_COUNT;
  config->run_serial = 0;
  config->run_multithreaded = 0;
  config->run_distributed = 0;
  config->run_shared = 0;
  config->run_task_pool = 0;

  bool flags_set = false;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0) {
      print_usage(argv[0]);
      exit(0);
    } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
      config->omp_threads = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-s") == 0) {
      config->run_serial = 1;
      flags_set = true;
    } else if (strcmp(argv[i], "-m") == 0) {
      config->run_multithreaded = 1;
      flags_set = true;
    } else if (strcmp(argv[i], "-d") == 0) {
      config->run_distributed = 1;
      flags_set = true;
    } else if (strcmp(argv[i], "-h") == 0) { // -h for shared (parallel_shared)
      config->run_shared = 1;
      flags_set = true;
    } else if (strcmp(argv[i], "-p") == 0) { // -p for task pool
      config->run_task_pool = 1;
      flags_set = true;
    } else if (strcmp(argv[i], "-a") == 0) {
      config->run_serial = 1;
      config->run_multithreaded = 1;
      config->run_distributed = 1;
      config->run_shared = 1;
      config->run_task_pool = 1;
      flags_set = true;
    } else {
      fprintf(stderr, "Unknown argument: %s\n", argv[i]);
      print_usage(argv[0]);
      exit(1);
    }
  }

  // If no benchmark flags set, default to distributed (original behavior seemed
  // to default to something or was manual) Or maybe error? For now, let's say
  // if none set, run nothing or print guidance? Previous behavior was
  // controlled by compile flags. If no flags are provided, maybe run all? Or
  // just Distributed as that was the active one in the file.
  if (!flags_set) {
    // Default to distributed as per previous default behavior in main
    config->run_distributed = 1;
  }
}

void init_mpi(int argc, char **argv, int *comm_rank, int *comm_size,
              BenchmarkConfig *config) {
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, comm_rank);
  MPI_Comm_size(MPI_COMM_WORLD, comm_size);

  parse_args(argc, argv, config);

  omp_set_num_threads(config->omp_threads);
}

app_error run_benchmarks(int comm_rank, BenchmarkConfig config) {
  app_error err = SUCCESS;

  if (config.run_serial) {
    err = run_benchmark_serial();
    if (err != SUCCESS) {
      if (comm_rank == 0)
        fprintf(stderr, "Serial benchmark failed with error: %s\n",
                get_error_string(err));
      return err;
    }
  }

  if (config.run_multithreaded) {
    err = run_benchmark_parallel_multithreaded();
    if (err != SUCCESS) {
      if (comm_rank == 0)
        fprintf(stderr,
                "Parallel benchmark (Multithreaded) failed with error: %s\n",
                get_error_string(err));
      return err;
    }
  }

  if (config.run_distributed) {
    err = run_benchmark_parallel_distributed_fs();
    if (err != SUCCESS) {
      if (comm_rank == 0)
        fprintf(stderr,
                "Parallel benchmark (Distributed Filesystem) failed with "
                "error: %s\n",
                get_error_string(err));
      return err;
    }
  }

  if (config.run_shared) {
    err = run_benchmark_parallel_shared_fs();
    if (err != SUCCESS) {
      if (comm_rank == 0)
        fprintf(
            stderr,
            "Parallel benchmark (Shared Filesystem) failed with error: %s\n",
            get_error_string(err));
      return err;
    }
  }

  if (config.run_task_pool) {
    err = run_benchmark_task_pool();
    if (err != SUCCESS) {
      if (comm_rank == 0)
        fprintf(stderr,
                "Parallel benchmark (Task Pool) failed with error: %s\n",
                get_error_string(err));
      return err;
    }
  }

  return err;
}

app_error write_benchmark_results(int comm_size, BenchmarkConfig config) {
  // Determine if we are running ALL benchmarks
  bool run_all = config.run_serial && config.run_multithreaded &&
                 config.run_distributed && config.run_shared &&
                 config.run_task_pool;

  // Initialize necessary CSV files
  app_error err = SUCCESS;
  if (run_all) {
    err = init_benchmark_csv(CSV_FILE);
  } else {
    if (config.run_serial)
      err = init_benchmark_csv(SERIAL_CSV_FILE);
    if (!err && config.run_multithreaded)
      err = init_benchmark_csv(MULTITHREADED_CSV_FILE);
    if (!err && config.run_distributed)
      err = init_benchmark_csv(DISTRIBUTED_CSV_FILE);
    if (!err && config.run_shared)
      err = init_benchmark_csv(SHARED_CSV_FILE);
    if (!err && config.run_task_pool)
      err = init_benchmark_csv(TASK_POOL_CSV_FILE);
  }

  if (err != SUCCESS) {
    fprintf(stderr, "Failed to initialize CSV file(s): %s\n",
            get_error_string(err));
    return err;
  }

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
        double serial_time = -1;
        if (config.run_serial)
          serial_time = benchmark_data[0][f][k];

        double multithreaded_time = -1;
        if (config.run_multithreaded)
          multithreaded_time = benchmark_data[1][f][k];

        double distributed_time = -1;
        if (config.run_distributed)
          distributed_time = benchmark_data[2][f][k];

        double shared_time = -1;
        if (config.run_shared)
          shared_time = benchmark_data[3][f][k];

        double task_pool_time = -1;
        if (config.run_task_pool)
          task_pool_time = benchmark_data[4][f][k];

// Prepare arguments for append
// Helper block to avoid repetition
#define APPEND_RESULT(file)                                                    \
  err = append_benchmark_result(file, width * height, CONV_KERNELS[k].size,    \
                                comm_size, config.omp_threads, serial_time,    \
                                multithreaded_time, distributed_time,          \
                                shared_time, task_pool_time);                  \
  if (err != SUCCESS) {                                                        \
    fprintf(stderr, "Failed to append result to %s: %s\n", file,               \
            get_error_string(err));                                            \
    return err;                                                                \
  }

        if (run_all) {
          APPEND_RESULT(CSV_FILE);
        } else {
          if (config.run_serial) {
            APPEND_RESULT(SERIAL_CSV_FILE);
          }
          if (config.run_multithreaded) {
            APPEND_RESULT(MULTITHREADED_CSV_FILE);
          }
          if (config.run_distributed) {
            APPEND_RESULT(DISTRIBUTED_CSV_FILE);
          }
          if (config.run_shared) {
            APPEND_RESULT(SHARED_CSV_FILE);
          }
          if (config.run_task_pool) {
            APPEND_RESULT(TASK_POOL_CSV_FILE);
          }
        }

#undef APPEND_RESULT
      }
      free_BMP(img);
    } else {
      fprintf(stderr, "Failed to read image %s for info\n", input_path);
    }
  }

  if (run_all) {
    printf("\nBenchmark results written to %s\n", CSV_FILE);
  } else {
    printf("\nBenchmark results written to separate files based on executed "
           "modes.\n");
  }

  return err;
}

int main(int argc, char **argv) {
  int comm_rank, comm_size;
  BenchmarkConfig config;
  init_mpi(argc, argv, &comm_rank, &comm_size, &config);

  if (comm_rank == 0) {
    printf("Running with %d MPI processes and %d OpenMP threads per process\n",
           comm_size, config.omp_threads);
    if (config.run_serial)
      printf("Mode: Serial\n");
    if (config.run_multithreaded)
      printf("Mode: Parallel Multithreaded\n");
    if (config.run_distributed)
      printf("Mode: Parallel Distributed Filesystem\n");
    if (config.run_shared)
      printf("Mode: Parallel Shared Filesystem\n");
    if (config.run_task_pool)
      printf("Mode: Parallel Task Pool\n");
  }

  // Run all benchmarks
  app_error err = run_benchmarks(comm_rank, config);
  if (err != SUCCESS) {
    MPI_Finalize();
    return err;
  }
  if (comm_rank == 0) {
    // Run verification
    err = run_verification(config);
    if (err != SUCCESS) {
      fprintf(stderr, "Verification failed with error: %s\n",
              get_error_string(err));
      MPI_Finalize();
      return err;
    }

    // Write results to CSV
    err = write_benchmark_results(comm_size, config);
    if (err != SUCCESS) {
      fprintf(stderr, "Failed to write benchmark results: %s\n",
              get_error_string(err));
      MPI_Finalize();
      return err;
    }
  }

  MPI_Finalize();
  return 0;
}

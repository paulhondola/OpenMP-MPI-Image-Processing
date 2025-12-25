#include "benchmark_io.h"
#include "../bmp/bmp_io.h"
#include "../config/files.h"
#include "../config/kernel.h"
#include "../file_utils/file_utils.h"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

app_error write_benchmark_results(int comm_size, BenchmarkConfig config) {
  // Determine if we are running ALL benchmarks
  bool run_all = config.run_serial && config.run_multithreaded &&
                 config.run_distributed && config.run_shared &&
                 config.run_task_pool;

  // Initialize necessary CSV files
  app_error err = SUCCESS;
  if (run_all) {
    err = init_benchmark_csv(MULTI_RUN_CSV_FILE, MULTI_RUN_CSV_HEADER);
  } else {
    if (config.run_serial)
      err = init_benchmark_csv(SERIAL_CSV_FILE, SINGLE_RUN_CSV_HEADER);
    if (!err && config.run_multithreaded)
      err = init_benchmark_csv(MULTITHREADED_CSV_FILE, SINGLE_RUN_CSV_HEADER);
    if (!err && config.run_distributed)
      err = init_benchmark_csv(DISTRIBUTED_CSV_FILE, SINGLE_RUN_CSV_HEADER);
    if (!err && config.run_shared)
      err = init_benchmark_csv(SHARED_CSV_FILE, SINGLE_RUN_CSV_HEADER);
    if (!err && config.run_task_pool)
      err = init_benchmark_csv(TASK_POOL_CSV_FILE, SINGLE_RUN_CSV_HEADER);
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
          APPEND_RESULT(MULTI_RUN_CSV_FILE);
        } else {
          if (config.run_serial) {
            err = append_single_benchmark_result(
                SERIAL_CSV_FILE, width * height, CONV_KERNELS[k].size,
                comm_size, config.omp_threads, serial_time);
            if (err != SUCCESS)
              return err;
          }
          if (config.run_multithreaded) {
            err = append_single_benchmark_result(
                MULTITHREADED_CSV_FILE, width * height, CONV_KERNELS[k].size,
                comm_size, config.omp_threads, multithreaded_time);
            if (err != SUCCESS)
              return err;
          }
          if (config.run_distributed) {
            err = append_single_benchmark_result(
                DISTRIBUTED_CSV_FILE, width * height, CONV_KERNELS[k].size,
                comm_size, config.omp_threads, distributed_time);
            if (err != SUCCESS)
              return err;
          }
          if (config.run_shared) {
            err = append_single_benchmark_result(
                SHARED_CSV_FILE, width * height, CONV_KERNELS[k].size,
                comm_size, config.omp_threads, shared_time);
            if (err != SUCCESS)
              return err;
          }
          if (config.run_task_pool) {
            err = append_single_benchmark_result(
                TASK_POOL_CSV_FILE, width * height, CONV_KERNELS[k].size,
                comm_size, config.omp_threads, task_pool_time);
            if (err != SUCCESS)
              return err;
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
    printf("\nBenchmark results written to %s\n", MULTI_RUN_CSV_FILE);
  } else {
    printf("\nBenchmark results written to separate files based on executed "
           "modes.\n");
  }

  return err;
}

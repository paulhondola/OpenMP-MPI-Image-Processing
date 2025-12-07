#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

#include "../errors/errors.h"

/**
 * Initializes the benchmark CSV file with headers if it doesn't exist.
 * @param filename Name of the CSV file
 * @return app_error code (SUCCESS on success)
 */
app_error init_benchmark_csv(const char *filename);

/**
 * Appends a benchmark result row to the CSV file.
 * Automatically calculates speedup and efficiency.
 * @param filename Name of the CSV file
 * @param operation Name of the operation/benchmark
 * @param clusters Number of clusters (or processes)
 * @param threads Number of threads
 * @param serial_time Time taken for serial execution
 * @param parallel_time Time taken for parallel execution
 * @return app_error code (SUCCESS on success)
 */
app_error append_benchmark_result(const char *filename, const char *operation,
                                  int clusters, int threads, double serial_time,
                                  double parallel_time);

/**
 * Recursively creates directories for the given path.
 * Splits input path into tokens by "/" and creates directories for every token.
 * @param path Path to the directory
 * @return app_error code (SUCCESS on success)
 */
app_error create_directory_recursive(const char *path);

#endif
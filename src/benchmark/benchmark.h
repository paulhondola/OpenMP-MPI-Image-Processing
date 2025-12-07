#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#include "../errors/errors.h"

/**
 * Runs the serial version of the image processing benchmark.
 * Executes convolution with various kernels on a test image.
 * @return app_error code:
 *         - SUCCESS: Benchmark completed successfully
 *         - ERR_DIR_CREATE: Failed to create output directories
 *         - ERR_FILE_OPEN: Failed to open input file
 *         - ERR_BMP_HEADER: Invalid BMP header in input
 *         - ERR_MEM_ALLOC: Memory allocation failed
 *         - ERR_CONVOLUTION: Convolution operation failed
 *         - ERR_FILE_WRITE: Failed to write output file
 */
app_error run_benchmark_serial(void);

/**
 * Runs the parallel version of the image processing benchmark.
 * Uses MPI and OpenMP for distributed and shared memory parallelism.
 * @return app_error code:
 *         - SUCCESS: Benchmark completed successfully
 *         - ERR_DIR_CREATE: Failed to create output directories
 *         - ERR_FILE_OPEN: Failed to open input file
 *         - ERR_BMP_HEADER: Invalid BMP header in input
 *         - ERR_MEM_ALLOC: Memory allocation failed
 *         - ERR_CONVOLUTION: Convolution operation failed
 *         - ERR_FILE_WRITE: Failed to write output file
 */
app_error run_benchmark_parallel(void);

#endif
#ifndef __FILES_H__
#define __FILES_H__

#define CONVOLUTION_MODES 5
#define BENCHMARK_FILES 3
#define KERNEL_TYPES 7

// Constants for file paths and names
extern const char *files[];
extern const char *IMAGES_FOLDER;
extern const char *BASE_FOLDER;
extern const char *SERIAL_FOLDER;
extern const char *PARALLEL_MULTITHREADED_FOLDER;
extern const char *PARALLEL_DISTRIBUTED_FS_FOLDER;
extern const char *PARALLEL_SHARED_FS_FOLDER;
extern const char *TASK_POOL_FOLDER;

extern const char *CSV_FILE;
extern const char *SERIAL_CSV_FILE;
extern const char *MULTITHREADED_CSV_FILE;
extern const char *DISTRIBUTED_CSV_FILE;
extern const char *SHARED_CSV_FILE;
extern const char *TASK_POOL_CSV_FILE;
extern const char *CSV_HEADER;

extern double benchmark_data[CONVOLUTION_MODES][BENCHMARK_FILES][KERNEL_TYPES];

#endif
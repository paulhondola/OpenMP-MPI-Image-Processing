#include "files.h"

// CSV Header for the single-run data file
const char *SINGLE_RUN_CSV_HEADER =
    "Pixel Count,Kernel Size,Clusters,Threads,Time";
const char *SERIAL_CSV_FILE = "data/chronos/serial_data.csv";
const char *MULTITHREADED_CSV_FILE = "data/chronos/multithreaded_data.csv";
const char *DISTRIBUTED_CSV_FILE = "data/chronos/distributed_data.csv";
const char *SHARED_CSV_FILE = "data/chronos/shared_data.csv";
const char *TASK_POOL_CSV_FILE = "data/chronos/task_pool_data.csv";

// CSV Header for the multi-run data file
const char *MULTI_RUN_CSV_FILE = "data/chronos/time_data.csv";
const char *MULTI_RUN_CSV_HEADER =
    "Pixel Count,Kernel Size,Clusters,Threads,Serial "
    "Time,Multithreaded "
    "Time,Distributed Time,Shared Time,Task Pool Time";

// CSV Header for the speedup data file
const char *SPEEDUP_CSV_FILE = "data/chronos/speedups_data.csv";
const char *SPEEDUP_CSV_HEADER =
    "Pixel Count,Kernel Size,Clusters,Threads,Serial "
    "Speedup,Multithreaded "
    "Speedup,Distributed Speedup,Shared Speedup,Task Pool Speedup";

// Image directories
const char *IMAGES_FOLDER = "images";
const char *BASE_FOLDER = "base";
const char *SERIAL_FOLDER = "serial";
const char *MULTITHREADED_FOLDER = "multithreaded";
const char *DISTRIBUTED_FOLDER = "distributed";
const char *SHARED_FOLDER = "shared";
const char *TASK_POOL_FOLDER = "task_pool";

// Input files (located in images/base)
const char *files[] = {"Large.bmp", "XL.bmp", "XXL.bmp"};

// Elapsed time data for each convolution mode, file, and kernel type
double benchmark_data[CONVOLUTION_MODES][BENCHMARK_FILES][KERNEL_TYPES];

#include "files.h"

// CSV Header for the benchmark data file
const char *CSV_FILE = "data/benchmark_data.csv";
const char *CSV_HEADER =
    "Image Width,Image Height,Operation,Clusters,Threads,Serial "
    "Time,Multithreaded "
    "Speedup,Distributed Speedup,Shared Speedup";

// Image directories
const char *IMAGES_FOLDER = "images";
const char *BASE_FOLDER = "base";
const char *SERIAL_FOLDER = "serial";
const char *PARALLEL_MULTITHREADED_FOLDER = "parallel_multithreaded";
const char *PARALLEL_DISTRIBUTED_FS_FOLDER = "parallel_distributed";
const char *PARALLEL_SHARED_FS_FOLDER = "parallel_shared";

// Input files (located in images/base)
const char *files[] = {"Large.bmp", "XL.bmp", "XXL.bmp"};

double benchmark_data[CONVOLUTION_MODES][BENCHMARK_FILES][KERNEL_TYPES];

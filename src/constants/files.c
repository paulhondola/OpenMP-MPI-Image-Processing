// CSV Header for the benchmark data file
const char *CSV_HEADER =
    "Image Size,Operation,Clusters,Threads,Serial Time,Multithreaded "
    "Speedup,Distributed Speedup,Shared Speedup";

// Input files (located in images/base)
const char *files[] = {"Large.bmp", "XL.bmp", "XXL.bmp"};
const int num_files = sizeof(files) / sizeof(files[0]);

// Image directories
const char *IMAGES_FOLDER = "images";
const char *BASE_FOLDER = "base";
const char *SERIAL_FOLDER = "serial";
const char *PARALLEL_MULTITHREADED_FOLDER = "parallel_multithreaded";
const char *PARALLEL_DISTRIBUTED_FS_FOLDER = "parallel_distributed";
const char *PARALLEL_SHARED_FS_FOLDER = "parallel_shared";

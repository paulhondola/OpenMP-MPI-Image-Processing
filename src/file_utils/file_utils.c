#include "file_utils.h"
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

// CSV Header for the benchmark data file
const char *CSV_HEADER = "Operation,Clusters,Threads,Serial Time,Parallel "
                         "Time,Speedup,Efficiency";

// Input files (located in images/base)
const char *files[] = {"Large.bmp", "XL.bmp", "XXL.bmp"};
const int num_files = sizeof(files) / sizeof(files[0]);

// Image directories
const char *IMAGES_FOLDER = "images";
const char *BASE_FOLDER = "base";
const char *SERIAL_FOLDER = "serial";
const char *PARALLEL_DISTRIBUTED_FS_FOLDER = "parallel_distributed";
const char *PARALLEL_SHARED_FS_FOLDER = "parallel_shared";

app_error create_directory(const char *path) {
  if (mkdir(path, 0777) == -1) {
    if (errno != EEXIST) {
      perror("mkdir");
      return ERR_DIR_CREATE;
    }
  }
  return SUCCESS;
}

app_error init_benchmark_csv(const char *filename) {
  FILE *fp = fopen(filename, "a+");
  if (fp == NULL) {
    perror("Error opening CSV file");
    return ERR_FILE_OPEN;
  }

  // Check if file is empty
  if (fseek(fp, 0, SEEK_END) != 0) {
    perror("Error seeking to end of CSV file");
    fclose(fp);
    return ERR_FILE_IO;
  }
  long size = ftell(fp);
  if (size == -1L) {
    perror("Error telling position in CSV file");
    fclose(fp);
    return ERR_FILE_IO;
  }

  // If file is empty, write csv header
  if (size == 0) {
    fprintf(fp, "%s\n", CSV_HEADER);
  }

  fclose(fp);
  return SUCCESS;
}

app_error append_benchmark_result(const char *filename, const char *operation,
                                  int clusters, int threads, double serial_time,
                                  double parallel_time) {
  FILE *fp = fopen(filename, "a");
  if (fp == NULL) {
    perror("Error opening CSV file for appending");
    return ERR_FILE_OPEN;
  }

  double speedup = 0.0;
  double efficiency = 0.0;

  if (parallel_time > 0) {
    speedup = serial_time / parallel_time;
    if (clusters > 0 && threads > 0) {
      efficiency = speedup / (clusters * threads);
    }
  }

  fprintf(fp, "%s,%d,%d,%.6f,%.6f,%.6f,%.6f\n", operation, clusters, threads,
          serial_time, parallel_time, speedup, efficiency);
  fclose(fp);
  return SUCCESS;
}

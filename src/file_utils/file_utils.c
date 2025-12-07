#include "file_utils.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

const char *CSV_HEADER = "Operation,Clusters,Threads,Serial Time,Parallel "
                         "Time,Speedup,Efficiency";

app_error create_directory_recursive(const char *path) {
  if (path == NULL || strlen(path) == 0) {
    return ERR_INVALID_ARGS;
  }

  struct stat st;
  if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
    return SUCCESS;
  }

  char temp_path[PATH_MAX];
  char current_path[PATH_MAX] = "";

  strncpy(temp_path, path, PATH_MAX - 1);
  temp_path[PATH_MAX - 1] = '\0';

  char *token = strtok(temp_path, "/");
  while (token != NULL) {
    // Append token to current path
    if (strlen(current_path) + strlen(token) + 2 >= PATH_MAX) {
      fprintf(stderr, "Error: Path too long\n");
      return ERR_PATH_TOO_LONG;
    }

    strcat(current_path, token);
    strcat(current_path, "/");

    // Create directory
    if (mkdir(current_path, 0755) != 0) {
      if (errno != EEXIST) {
        perror("Error creating directory");
        return ERR_DIR_CREATE;
      } else {
        struct stat s;
        // Remove trailing slash for stat
        current_path[strlen(current_path) - 1] = '\0';
        if (stat(current_path, &s) == 0) {
          if (!S_ISDIR(s.st_mode)) {
            fprintf(stderr, "Error: '%s' exists but is not a directory\n",
                    current_path);
            return ERR_NOT_A_DIR;
          }
        }
        // Restore slash
        strcat(current_path, "/");
      }
    }

    token = strtok(NULL, "/");
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
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

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
    if (clusters * threads > 0) {
      efficiency = speedup / (clusters * threads);
    }
  }

  fprintf(fp, "%s,%d,%d,%.6f,%.6f,%.6f,%.6f\n", operation, clusters, threads,
          serial_time, parallel_time, speedup, efficiency);
  fclose(fp);
  return SUCCESS;
}

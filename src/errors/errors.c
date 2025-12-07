#include "errors.h"

const char *get_error_string(app_error err) {
  switch (err) {
  case SUCCESS:
    return "Success";
  case ERR_FILE_OPEN:
    return "Error opening file";
  case ERR_FILE_READ:
    return "Error reading file";
  case ERR_FILE_WRITE:
    return "Error writing file";
  case ERR_MEM_ALLOC:
    return "Memory allocation failed";
  case ERR_PATH_TOO_LONG:
    return "Path too long";
  case ERR_DIR_CREATE:
    return "Error creating directory";
  case ERR_NOT_A_DIR:
    return "Path exists but is not a directory";
  case ERR_INVALID_ARGS:
    return "Invalid arguments";
  case ERR_BMP_HEADER:
    return "Invalid BMP header";
  case ERR_BMP_DATA:
    return "Invalid BMP data";
  case ERR_CONVOLUTION:
    return "Convolution error";
  case ERR_UNKNOWN:
    return "Unknown error";
  default:
    return "Unknown error code";
  }
}

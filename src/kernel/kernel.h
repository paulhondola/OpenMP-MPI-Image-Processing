#ifndef __KERNEL_H__
#define __KERNEL_H__

/**
 * Represents a square convolution kernel.
 * Contains the size (dimension) and the flattened data array.
 */
typedef struct {
  int size;
  const double *data;
} Kernel;

/** Pre-defined edge detection kernel used for ridge detection. */
extern const Kernel RIDGE_KERNEL;
/** Pre-defined edge detection kernel. */
extern const Kernel EDGE_KERNEL;
/** Pre-defined sharpen kernel. */
extern const Kernel SHARPEN_KERNEL;
/** Pre-defined box blur kernel. */
extern const Kernel BOXBLUR_KERNEL;
/** Pre-defined 3x3 Gaussian blur kernel. */
extern const Kernel GAUSSIAN3_KERNEL;
/** Pre-defined 5x5 Gaussian blur kernel. */
extern const Kernel GAUSSIAN5_KERNEL;
/** Pre-defined 5x5 Unsharp masking kernel. */
extern const Kernel UNSHARP5_KERNEL;

#endif
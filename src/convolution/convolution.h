#ifndef __CONVOLUTION_H__
#define __CONVOLUTION_H__

#include "../bmp/bmp_io.h"
#include "../errors/errors.h"
#include "../kernel/kernel.h"

/**
 * Applies a convolution kernel to an image.
 * @param img Pointer to the Image structure to modify
 * @param kernel The convolution kernel to apply
 * @return app_error code:
 *         - SUCCESS: Convolution completed successfully
 *         - ERR_MEM_ALLOC: Memory allocation failed for output image
 */
app_error convolve_serial(Image *img, Kernel kernel);
app_error convolve_parallel(Image *img, Kernel kernel);

app_error check_images_match(Image *img1, Image *img2);

#endif
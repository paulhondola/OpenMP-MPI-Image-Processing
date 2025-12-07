#ifndef __CONVOLUTION_H__
#define __CONVOLUTION_H__

#include "../bmp/bmp_io.h"
#include "../errors/errors.h"
#include "../kernel/kernel.h"

app_error convolve(Image *img, Kernel kernel);

#endif
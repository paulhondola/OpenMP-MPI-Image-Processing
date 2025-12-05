#ifndef __CONVOLUTION_H__
#define __CONVOLUTION_H__

#include "../bmp/bmp_io.h"
#include "../kernel/kernel.h"

int convolve(Image *img, Kernel kernel);

#endif
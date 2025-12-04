#ifndef __BMP_IO_H__
#define __BMP_IO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Data structures for representing BMP images in memory */

typedef struct Image Image;
/* Read BMP file, build and return Image struct */
Image *readBMP(const char *filename);

/* Save Image in file in BMP format */
int saveBMP(const char *filename, const Image *img);

#endif
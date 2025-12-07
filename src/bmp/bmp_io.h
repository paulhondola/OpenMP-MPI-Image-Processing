#ifndef __BMP_IO_H__
#define __BMP_IO_H__

#include "../errors/errors.h"
#include <stdio.h>

/* Data structures for representing BMP images in memory */

typedef struct {
  unsigned char r, g, b;
} Pixel; // one RGB point

typedef struct {
  int width;
  int height;
  Pixel *data;
} Image; // a BMP image as an array of RGB points

/* Read BMP file, build and return Image struct via pointer */
app_error read_BMP(const char *filename, Image **img);

/* Save Image in file in BMP format */
app_error save_BMP(const char *filename, const Image *img);

/* Allocators */
Pixel *alloc_pixel(int width, int height);
Image *alloc_image(int width, int height, Pixel *data);

/* Free memory allocated for Image */
void free_BMP(Image *img);

/* Debug */
void print_BMP_header(Image *img, FILE *fp);
void print_BMP_pixels(Image *img, FILE *fp);

#endif
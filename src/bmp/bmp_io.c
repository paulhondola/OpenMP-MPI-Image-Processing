#include "bmp_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Data structures for representing BMP images in memory */

/* Read BMP file, build and return Image struct */
struct Image *readBMP(const char *filename) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    fprintf(stderr, "Error: Could not open file %s\n", filename);
    return NULL;
  }

  unsigned char header[54];
  if (fread(header, sizeof(unsigned char), 54, f) != 54) {
    fprintf(stderr, "Error: Invalid BMP header\n");
    fclose(f);
    return NULL;
  }

  if (header[0] != 'B' || header[1] != 'M') {
    fprintf(stderr, "Error: Not a valid BMP file\n");
    fclose(f);
    return NULL;
  }

  int width = *(int *)&header[18];
  int height = *(int *)&header[22];
  int bitsPerPixel = *(short *)&header[28];

  if (bitsPerPixel != 24) {
    fprintf(stderr, "Error: Only 24-bit BMPs are supported\n");
    fclose(f);
    return NULL;
  }

  int row_padded = (width * 3 + 3) & (~3);
  unsigned char *row = (unsigned char *)malloc(row_padded);
  RGB *data = (RGB *)malloc(width * height * sizeof(RGB));
  if (!data || !row) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    free(data);
    free(row);
    fclose(f);
    return NULL;
  }

  for (int y = 0; y < height; y++) {
    fread(row, sizeof(unsigned char), row_padded, f);
    for (int x = 0; x < width; x++) {
      data[(height - 1 - y) * width + x].b = row[x * 3];
      data[(height - 1 - y) * width + x].g = row[x * 3 + 1];
      data[(height - 1 - y) * width + x].r = row[x * 3 + 2];
    }
  }

  free(row);
  fclose(f);

  struct Image *img = (struct Image *)malloc(sizeof(struct Image));
  img->width = width;
  img->height = height;
  img->data = data;
  return img;
}

/* Save Image in file in BMP format */
int saveBMP(const char *filename, const struct Image *img) {
  FILE *f = fopen(filename, "wb");
  if (!f) {
    fprintf(stderr, "Error: Could not create file %s\n", filename);
    return 0;
  }

  int width = img->width;
  int height = img->height;
  int row_padded = (width * 3 + 3) & (~3);
  int fileSize = 54 + row_padded * height;

  unsigned char header[54] = {
      'B', 'M',       // Signature
      0,   0,   0, 0, // File size
      0,   0,   0, 0, // Reserved
      54,  0,   0, 0, // Data offset
      40,  0,   0, 0, // Header size
      0,   0,   0, 0, // Width
      0,   0,   0, 0, // Height
      1,   0,         // Planes
      24,  0,         // Bits per pixel
      0,   0,   0, 0, // Compression (none)
      0,   0,   0, 0, // Image size (can be 0 for no compression)
      0,   0,   0, 0, // X pixels per meter
      0,   0,   0, 0, // Y pixels per meter
      0,   0,   0, 0, // Total colors
      0,   0,   0, 0  // Important colors
  };

  // Fill in width, height, and file size
  *(int *)&header[2] = fileSize;
  *(int *)&header[18] = width;
  *(int *)&header[22] = height;

  fwrite(header, sizeof(unsigned char), 54, f);

  unsigned char *row = (unsigned char *)calloc(1, row_padded);
  if (!row) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    fclose(f);
    return 0;
  }

  // Write pixel data bottom-to-top
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      RGB pixel = img->data[(height - 1 - y) * width + x];
      row[x * 3] = pixel.b;
      row[x * 3 + 1] = pixel.g;
      row[x * 3 + 2] = pixel.r;
    }
    fwrite(row, sizeof(unsigned char), row_padded, f);
  }

  free(row);
  fclose(f);
  return 1;
}

/* Test program */
int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s input.bmp output.bmp\n", argv[0]);
    return 1;
  }

  const char *in_filename = argv[1];
  const char *out_filename = argv[2];

  printf("Loading image from file %s \n", in_filename);

  // Read input
  struct Image *img = readBMP(in_filename);
  if (!img) {
    fprintf(stderr, "Error reading %s\n", in_filename);
    return 1;
  }

  int width = img->width;
  int height = img->height;

  printf("Image size is width=%d  and height=%d \n", width, height);

  // Transform image - draw a horizontal black line in the middle of the image
  int thickness = height / 10;
  for (int i = height / 2 - thickness; i <= height / 2 + thickness; i++)
    for (int j = 0; j < width; j++) {
      img->data[i * width + j].r = 0;
      img->data[i * width + j].g = 0;
      img->data[i * width + j].b = 0;
    }

  // Save output
  saveBMP(out_filename, img);
  printf("Modified image saved in file %s \n", out_filename);

  free(img->data);
  free(img);

  return 0;
}

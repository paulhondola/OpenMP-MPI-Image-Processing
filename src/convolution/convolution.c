#include "convolution.h"
#include <stdlib.h>

unsigned char cast_to_pixel_value(double val) {
  return (unsigned char)(val < 0 ? 0 : (val > 255 ? 255 : val));
}

void clamp_pixel(Pixel *p, double r, double g, double b) {
  p->r = cast_to_pixel_value(r);
  p->g = cast_to_pixel_value(g);
  p->b = cast_to_pixel_value(b);
}

void clamp_to_boundary(int *px, int *py, int width, int height) {
  if (*px < 0)
    *px = 0;
  if (*px >= width)
    *px = width - 1;
  if (*py < 0)
    *py = 0;
  if (*py >= height)
    *py = height - 1;
}

app_error convolve_serial(Image *img, Kernel kernel) {
  int width = img->width;
  int height = img->height;
  int k_size = kernel.size;
  int half_k = k_size / 2;

  Pixel *output = alloc_pixel(width, height);
  if (!output) {
    return ERR_MEM_ALLOC;
  }

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double r_acc = 0, g_acc = 0, b_acc = 0;

      for (int ky = 0; ky < k_size; ky++) {
        for (int kx = 0; kx < k_size; kx++) {
          int py = y + ky - half_k;
          int px = x + kx - half_k;

          clamp_to_boundary(&px, &py, width, height);

          Pixel p = img->data[py * width + px];
          double k_val = kernel.data[ky * k_size + kx];

          r_acc += p.r * k_val;
          g_acc += p.g * k_val;
          b_acc += p.b * k_val;
        }
      }

      Pixel out_p;
      clamp_pixel(&out_p, r_acc, g_acc, b_acc);
      output[y * width + x] = out_p;
    }
  }

  free(img->data);
  img->data = output;

  return SUCCESS;
}

app_error convolve_parallel_multithreaded(Image *img, Kernel kernel) {
  int width = img->width;
  int height = img->height;
  int k_size = kernel.size;
  int half_k = k_size / 2;

  Pixel *output = alloc_pixel(width, height);
  if (!output) {
    return ERR_MEM_ALLOC;
  }

  Pixel *restrict output_data = output;
  const Pixel *restrict input_data = img->data;
  const double *restrict kernel_data = kernel.data;

#pragma omp parallel for collapse(2) schedule(dynamic)
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double r_acc = 0, g_acc = 0, b_acc = 0;

      for (int ky = 0; ky < k_size; ky++) {
        for (int kx = 0; kx < k_size; kx++) {
          int py = y + ky - half_k;
          int px = x + kx - half_k;

          clamp_to_boundary(&px, &py, width, height);

          Pixel p = input_data[py * width + px];
          double k_val = kernel_data[ky * k_size + kx];

          r_acc += p.r * k_val;
          g_acc += p.g * k_val;
          b_acc += p.b * k_val;
        }
      }

      Pixel out_p;
      clamp_pixel(&out_p, r_acc, g_acc, b_acc);
      output_data[y * width + x] = out_p;
    }
  }

  free(img->data);
  img->data = output;

  return SUCCESS;
}

app_error convolve_parallel_distributed_filesystem(Image *img, Kernel kernel) {
  int width = img->width;
  int height = img->height;
  int k_size = kernel.size;
  int half_k = k_size / 2;

  Pixel *output = alloc_pixel(width, height);
  if (!output) {
    return ERR_MEM_ALLOC;
  }

  Pixel *restrict output_data = output;
  const Pixel *restrict input_data = img->data;
  const double *restrict kernel_data = kernel.data;

#pragma omp parallel for collapse(2) schedule(dynamic)
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double r_acc = 0, g_acc = 0, b_acc = 0;

      for (int ky = 0; ky < k_size; ky++) {
        for (int kx = 0; kx < k_size; kx++) {
          int py = y + ky - half_k;
          int px = x + kx - half_k;

          clamp_to_boundary(&px, &py, width, height);

          Pixel p = input_data[py * width + px];
          double k_val = kernel_data[ky * k_size + kx];

          r_acc += p.r * k_val;
          g_acc += p.g * k_val;
          b_acc += p.b * k_val;
        }
      }

      Pixel out_p;
      clamp_pixel(&out_p, r_acc, g_acc, b_acc);
      output_data[y * width + x] = out_p;
    }
  }

  free(img->data);
  img->data = output;

  return SUCCESS;
}

app_error convolve_parallel_shared_filesystem(Image *img, Kernel kernel) {
  int width = img->width;
  int height = img->height;
  int k_size = kernel.size;
  int half_k = k_size / 2;

  Pixel *output = alloc_pixel(width, height);
  if (!output) {
    return ERR_MEM_ALLOC;
  }

  Pixel *restrict output_data = output;
  const Pixel *restrict input_data = img->data;
  const double *restrict kernel_data = kernel.data;

#pragma omp parallel for collapse(2) schedule(dynamic)
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double r_acc = 0, g_acc = 0, b_acc = 0;

      for (int ky = 0; ky < k_size; ky++) {
        for (int kx = 0; kx < k_size; kx++) {
          int py = y + ky - half_k;
          int px = x + kx - half_k;

          clamp_to_boundary(&px, &py, width, height);

          Pixel p = input_data[py * width + px];
          double k_val = kernel_data[ky * k_size + kx];

          r_acc += p.r * k_val;
          g_acc += p.g * k_val;
          b_acc += p.b * k_val;
        }
      }

      Pixel out_p;
      clamp_pixel(&out_p, r_acc, g_acc, b_acc);
      output_data[y * width + x] = out_p;
    }
  }

  free(img->data);
  img->data = output;

  return SUCCESS;
}

app_error check_images_match(Image *img1, Image *img2) {
  if (img1->width != img2->width || img1->height != img2->height) {
    return ERR_IMAGE_DIFFERENCE;
  }

  int match = 1;

#pragma omp parallel for shared(match)
  for (int i = 0; i < img1->width * img1->height; i++) {
    if (!match)
      continue; // Early exit hint for threads
    if (img1->data[i].r != img2->data[i].r ||
        img1->data[i].g != img2->data[i].g ||
        img1->data[i].b != img2->data[i].b) {
#pragma omp atomic write
      match = 0;
    }
  }

  return match ? SUCCESS : ERR_IMAGE_DIFFERENCE;
}

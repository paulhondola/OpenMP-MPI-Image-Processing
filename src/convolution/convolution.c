#include "convolution.h"

void clamp_pixel(Pixel *p, double r, double g, double b) {
  p->r = (unsigned char)(r < 0 ? 0 : (r > 255 ? 255 : r));
  p->g = (unsigned char)(g < 0 ? 0 : (g > 255 ? 255 : g));
  p->b = (unsigned char)(b < 0 ? 0 : (b > 255 ? 255 : b));
}

int convolve(Image *img, Kernel kernel) {
  int width = img->width;
  int height = img->height;
  int k_size = kernel.size;
  int half_k = k_size / 2;

  Pixel *output = alloc_pixel(width, height);
  if (!output) {
    return 0;
  }

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double r_acc = 0, g_acc = 0, b_acc = 0;

      for (int ky = 0; ky < k_size; ky++) {
        for (int kx = 0; kx < k_size; kx++) {
          int py = y + ky - half_k;
          int px = x + kx - half_k;

          // Clamp to boundary
          if (px < 0)
            px = 0;
          if (px >= width)
            px = width - 1;
          if (py < 0)
            py = 0;
          if (py >= height)
            py = height - 1;

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

  // Replace old data with new data
  free(img->data);
  img->data = output;

  return 1;
}
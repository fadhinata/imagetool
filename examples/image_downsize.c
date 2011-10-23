#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#include <interface/screen.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <bmp.h>

int image_downsize(int argc, char *argv[])
{
  int x, y, w, h, bpp;
  int sum;
  bytemap_t *a, *b, *c;
  bytemap_t *gray;
  bitmap_t *bin, *roi;
  char path[256], *fn, *ptr;
  unsigned char *pixels;
  BITMAPINFO *bmpinfo;

  w = 1024;//bmpinfo->bmiHeader.biWidth / 2;
  h = 640; //bmpinfo->bmiHeader.biHeight / 2;
  bpp = 32;
  printf("w: %d, h: %d\n", w, h);

  initialize_screen(w, h, bpp);

  strcpy(path, argv[1]);
  fn = strrchr(path, '/') + 1;
  printf("Filename: %s\n", fn);

  pixels = LoadDIBitmap(argv[1], &bmpinfo);
  assert(bmpinfo->bmiHeader.biBitCount == 8);

  a = bytemap_new(w, h);
  b = bytemap_new(w, h);
  c = bytemap_new(w, h);
  gray = bytemap_new(w, h);

  bin = bitmap_new(w, h);
  roi = bitmap_new(w, h);

  // Display Image
  printf("Display Image\n");
  for (y = (bmpinfo->bmiHeader.biHeight - h) / 2; y < (bmpinfo->bmiHeader.biHeight - h) / 2 + h; y++) {
    for (x = (bmpinfo->bmiHeader.biWidth - w) / 2; x < (bmpinfo->bmiHeader.biWidth - w) / 2 + w; x++) {
      sum = pixels[y * bmpinfo->bmiHeader.biWidth + x];
      bytemap_put_value(sum, a, x - (bmpinfo->bmiHeader.biWidth - w) / 2, y - (bmpinfo->bmiHeader.biHeight - h) / 2);
    }
  }

  free(bmpinfo);
  free(pixels);

  write_bytemap_to_screen(a, a, a);
  wait_keyhit();

  bitmap_destroy(roi);
  bitmap_destroy(bin);

  bytemap_destroy(gray);
  bytemap_destroy(c);
  bytemap_destroy(b);
  bytemap_destroy(a);

  return 0;
}

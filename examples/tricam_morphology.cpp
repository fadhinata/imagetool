#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <SDL/SDL.h>

#include <common.h>
#include <bmp.h>
#include <smoothing/mean_smooth.h>
#include <interface/screen.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <thresholding/threshold.h>
#include <morphology/binary_morph.h>
#include <labeling/labeling.h>
#include <statistics/pixmap_statistic.h>
#include <convert/convert.h>

#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

int tricam_morphology(int argc, char *argv[])
{
  bitmap_t *spot, *bruise, *bin;
  bytemap_t *gray;
  bytemap_t *se;
  int x, y, i, j, w, h, bpp, mode = 0, save = 0;
  real_t value;
  char path[256], *fn, *ptr;
  FILE *fd = NULL;

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);
  
  gray = bytemap_new(w, h);
  bruise = bitmap_new(w, h);
  spot = bitmap_new(w, h);
  bin = bitmap_new(w, h);

  se = bytemap_new(3, 3);
  bytemap_fill(se, 0, 0, 3, 3, 1);

  // Display UP image
  printf("Load and display: ");
  strcpy(path, argv[1]);
  strcat(path, "_xxx_spot.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(gray, NULL, NULL);
  bytemap2bitmap(spot, gray);
  //wait_keyhit();

  binary_opening(bin, spot, se);
  dynamic_bitmap2bytemap(gray, bin);
  write_bytemap_to_screen(gray, NULL, NULL);
  strcpy(path, argv[1]);
  strcat(path, "_xxx_spot_opening.bmp");
  save_bytemap_as_gray_BMP(gray, path);
  //wait_keyhit();

  printf("Load and display: ");
  strcpy(path, argv[1]);
  strcat(path, "_xxx_bruise.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(gray, NULL, NULL);
  bytemap2bitmap(bruise, gray);
  //wait_keyhit();

  binary_opening(bin, bruise, se);
  dynamic_bitmap2bytemap(gray, bin);
  write_bytemap_to_screen(NULL, NULL, gray);
  strcpy(path, argv[1]);
  strcat(path, "_xxx_bruise_opening.bmp");
  save_bytemap_as_gray_BMP(gray, path);
  //wait_keyhit();

  bytemap_destroy(se);
  bitmap_destroy(bin);
  bitmap_destroy(spot);
  bitmap_destroy(bruise);
  bytemap_destroy(gray);

  deinitialize_screen();

  return 0;
}

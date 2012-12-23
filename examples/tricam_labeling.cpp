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

int tricam_labeling(int argc, char *argv[])
{
  bitmap_t *spot, *bruise, *bin;
  dwordmap_t *labelmap;
  bytemap_t *gray;
  label_info_t *label_info;
  int x, y, i, j, w, h, bpp, mode = 0, save = 0;
  int label, area;
  real_t value;
  char path[256], *fn, *ptr;
  FILE *fd = NULL;

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);
  
  gray = bytemap_new(w, h);
  bruise = bitmap_new(w, h);
  spot = bitmap_new(w, h);
  bin = bitmap_new(w, h);
  labelmap = dwordmap_new(w, h);

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

  printf("Big blobs greping\n");
  bitmap_clear(bin);
  label_info = label_info_new();
  labeling(labelmap, label_info, spot, NEIGHBOR_8);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 25) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(bin, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);

  dynamic_bitmap2bytemap(gray, bin);
  write_bytemap_to_screen(gray, NULL, NULL);
  strcpy(path, argv[1]);
  strcat(path, "_xxx_spot_labeling.bmp");
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

  printf("Big blobs greping\n");
  bitmap_clear(bin);
  label_info = label_info_new();
  labeling(labelmap, label_info, bruise, NEIGHBOR_8);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 25) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(bin, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);

  dynamic_bitmap2bytemap(gray, bin);
  write_bytemap_to_screen(NULL, NULL, gray);
  strcpy(path, argv[1]);
  strcat(path, "_xxx_bruise_labeling.bmp");
  save_bytemap_as_gray_BMP(gray, path);
  //wait_keyhit();

  dwordmap_destroy(labelmap);
  bitmap_destroy(bin);
  bitmap_destroy(spot);
  bitmap_destroy(bruise);
  bytemap_destroy(gray);

  deinitialize_screen();

  return 0;
}

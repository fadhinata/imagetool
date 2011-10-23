#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

#include <common.h>
#include <bmp.h>
#include <interface/screen.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <thresholding/threshold.h>
#include <morphology/binary_morph.h>
#include <labeling/labeling.h>

#define DN_MIN (11.0)
#define DN_MEAN (24.41464478)
#define DN_MAX (44.0)

#define MID_MIN (10.0)
#define MID_MEAN (21.76591283)
#define MID_MAX (44.0)

#define UP_MIN (11.0)
#define UP_MEAN (22.29564339)
#define UP_MAX (42.0)

#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

int tricam_process(int argc, char *argv[])
{
  int i, j, w, h, bpp;
  bytemap_t *a, *b, *c;
  bytemap_t *gray;
  bitmap_t *bin, *roi;
  char path[256], *fn, *ptr;
  real_t value;

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);
  
  a = bytemap_new(w, h);
  b = bytemap_new(w, h);
  c = bytemap_new(w, h);
  gray = bytemap_new(w, h);

  bin = bitmap_new(w, h);
  roi = bitmap_new(w, h);
  
  strcpy(path, argv[1]);
  strcat(path, "_DN.bmp");

  fn = strrchr(path, '/') + 1;
  printf("filename: %s\n", path);

  // Display Image
  load_and_display_BMP(path);
  read_bytemap_in_screen(a, b, c);
  write_bytemap_to_screen(a, a, a);
  wait_keyhit();

  printf("Image thresholding\n");
  bytemap_threshold(bin, a, 15, 255);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  bytemap_t *se;
  printf("Image Morphology\n");
  se = bytemap_new(3, 3);
  bytemap_fill(se, 0, 0, 3, 3, 1);
  binary_closing(roi, bin, se);
  binary_opening(bin, roi, se);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();
  bytemap_destroy(se);

  label_info_t *label_info;
  dwordmap_t *labelmap;
  point_t *centroid;
  int label, area;

  printf("Big blobs greping\n");
  label_info = label_info_new();
  labelmap = dwordmap_new(w, h);

  bitmap_clear(roi);
  labeling(labelmap, label_info, bin, NEIGHBOR_8);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 1000) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(roi, j, i);
	  }
	}
      }
    }
  }

  dwordmap_destroy(labelmap);
  label_info_destroy(label_info);
  write_bitmap_to_screen(roi, roi, roi);
  wait_keyhit();

  printf("MID image loading\n");
  strcpy(path, argv[1]);
  strcat(path, "_MID.bmp");
  load_and_display_BMP(path);
  read_bytemap_in_screen(b, NULL, NULL);
  wait_keyhit();

  printf("UP image loading\n");
  strcpy(path, argv[1]);
  strcat(path, "_UP.bmp");
  load_and_display_BMP(path);
  read_bytemap_in_screen(c, NULL, NULL);
  wait_keyhit();

  uint8_t vmin, vmax;
  real_t vmean;

  // Statistic
  bytemap_get_min_max_on_roi(&vmin, &vmax, a, roi);
  bytemap_get_mean_on_roi(&vmean, a, roi);
  printf("DN-Statistics are min: %d, mean: %lf, max: %d\n", vmin, vmean, vmax);

  printf("DN-Image normalization\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = bytemap_get_value(a, j, i);
	if (value < DN_MEAN) {
	  value = round(128.0 / (DN_MEAN - DN_MIN) * (value - DN_MIN));
	} else {
	  value = round(128.0 / (DN_MAX - DN_MEAN) * (value - DN_MEAN) + 128.0);
	}
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, a, j, i);
      } else {
	bytemap_put_value(0, a, j, i);
      }
    }
  }
  write_bytemap_to_screen(a, a, a);
  wait_keyhit();

  bytemap_get_min_max_on_roi(&vmin, &vmax, b, roi);
  bytemap_get_mean_on_roi(&vmean, b, roi);
  printf("DN-Statistics are min: %d, mean: %lf, max: %d\n", vmin, vmean, vmax);

  printf("MID-Image normalization\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = bytemap_get_value(b, j, i);
	if (value < MID_MEAN) {
	  value = round(128.0 / (MID_MEAN - MID_MIN) * (value - MID_MIN));
	} else {
	  value = round(128.0 / (MID_MAX - MID_MEAN) * (value - MID_MEAN) + 128.0);
	}
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, b, j, i);
      } else {
	bytemap_put_value(0, b, j, i);
      }
    }
  }
  write_bytemap_to_screen(b, b, b);
  wait_keyhit();

  bytemap_get_min_max_on_roi(&vmin, &vmax, c, roi);
  bytemap_get_mean_on_roi(&vmean, c, roi);
  printf("DN-Statistics are min: %d, mean: %lf, max: %d\n", vmin, vmean, vmax);

  printf("UP-Image normalization\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = bytemap_get_value(c, j, i);
	if (value < UP_MEAN) {
	  value = round(128.0 / (UP_MEAN - UP_MIN) * (value - UP_MIN));
	} else {
	  value = round(128.0 / (UP_MAX - UP_MEAN) * (value - UP_MEAN) + 128.0);
	}
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, c, j, i);
      } else {
	bytemap_put_value(0, c, j, i);
      }
    }
  }
  write_bytemap_to_screen(c, c, c);
  wait_keyhit();
  
  /*
  uint8_t vmin, vmax;
  real_t vmean;

  // Statistic
  bytemap_get_min_max_on_roi(&vmin, &vmax, a, roi);
  bytemap_get_mean_on_roi(&vmean, a, roi);
  printf("Statistics are min: %d, mean: %lf, max: %d\n", vmin, vmean, vmax);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = bytemap_get_value(a, j, i);
	if (value < vmean) {
	  value = (int)round(128.0 / ((real_t)vmean - (real_t)vmin) * ((real_t)value - (real_t)vmin));
	} else {
	  value = (int)round(128.0 / ((real_t)vmax - (real_t)vmean) * ((real_t)value - (real_t)vmean) + 128.0);
	}
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, a, j, i);
      } else {
	bytemap_put_value(0, a, j, i);
      }
    }
  }
  write_bytemap_to_screen(a, a, a);
  wait_keyhit();
  */

  bitmap_destroy(roi);
  bitmap_destroy(bin);

  bytemap_destroy(gray);
  bytemap_destroy(c);
  bytemap_destroy(b);
  bytemap_destroy(a);

  return 0;
}

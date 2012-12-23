#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <getopt.h>

#include <common.h>
#include <bmp.h>
#include <smoothing/mean_smooth.h>
#include <smoothing/median_smooth.h>
#include <interface/screen.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <thresholding/threshold.h>
#include <morphology/binary_morph.h>
#include <labeling/labeling.h>
#include <statistics/pixmap_statistic.h>
#include <convert/convert.h>
#include <color/gray.h>

// This is for .../Data/try10/*.bmp

/////////////////////////////////////////
#define UP_DX 0
#define UP_DY 0

#define UP_FROM 60
#define UP_TO 255

#define UP_MIN 35.544
#define UP_MEAN 113.647
#define UP_MAX 198.289

/////////////////////////////////////////
#define MID_DX -2
#define MID_DY 1

#define MID_FROM 60
#define MID_TO 255

#define MID_MIN 37.198
#define MID_MEAN 128.222
#define MID_MAX 249.738

/////////////////////////////////////////
#define DN_DX 3
#define DN_DY 0

#define DN_FROM 30
#define DN_TO 255

#define DN_MIN 20.544
#define DN_MEAN 65.617
#define DN_MAX 113.671

/////////////////////////////////////////
#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

//#undef wait_keyhit()
//#define wait_keyhit()

static const char *program_name = NULL;
static char *path;
static int verbose = 0;

static const struct option long_options[] = {
  {"help", 0, NULL, 'h'},
  {"op", 1, NULL, 'o'},
  {"sub", 1, NULL, 's'},
  {"div", 1, NULL, 'd'},
  {"from", 1, NULL, 'f'},
  {"to", 1, NULL, 't'},
  {"verbose", 0, NULL, 'v'},
  {"name", 1, NULL, 'n'},
  {NULL, 0, NULL, 0}
};

static const char *const short_options = "ho:s:d:f:t:vn:";

static void print_usage(FILE *stream, int exit_code)
{
  fprintf(stream, "Usage : %s [ ... ]\n", program_name);
  fprintf(stream, "-h,           --help    Display this usage information.\n");
  fprintf(stream, "-o,           --op      Set operand for subtracting.\n");
  fprintf(stream, "-s,           --sub     Set subtract for subtracting.\n");
  fprintf(stream, "-d,           --div     Set subtract for subtracting.\n");
  fprintf(stream, "-f [Cutup],   --from    Set Cutup value to threshold.\n");
  fprintf(stream, "-t [Cutdown], --to      Set Cutdown value to threhold.\n");
  fprintf(stream, "-v,           --verbose Print verbose message.\n");
  fprintf(stream, "-n [image],   --name    Set input file.\n");
  exit(exit_code);
}

int tricam_apple_sub_div(int argc, char *argv[])
{
  int i, j, x, y, w, h, bpp;
  bytemap_t *up_image, *mid_image, *dn_image;
  bytemap_t *gray, *se, *red, *green, *blue;
  bitmap_t *up_roi, *mid_roi, *dn_roi;
  bitmap_t *bin, *roi, *tmp;
  char filename[256], *fn, *ptr;
  real_t value;
  FILE *fd;
  label_info_t *label_info;
  dwordmap_t *labelmap;
  point_t *centroid;
  int n, label, area;
  uint8_t vmin, vmax;
  real_t vmean;

  bytemap_t *op1, *op2, *op3;
  char *path;
  int next_option;
  char *token1, *token2, *token3;
  real_t cutup = -2550000, cutdown = -4748;

  program_name = argv[0];
  do {
    next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option) {
    case 'h': print_usage(stdout, 0); break;
    case 'o': token1 = strdup(optarg); break;
    case 's': token2 = strdup(optarg); break;
    case 'd': token3 = strdup(optarg); break;
    case 'f': cutup = strtod(optarg, NULL); break;
    case 't': cutdown = strtod(optarg, NULL); break;
    case 'v': verbose = 1; break;
    case 'n': path = strdup(optarg); break;
    case '?': print_usage(stderr, 1); break;
    case -1: break;
    default: abort();
    }
  } while (next_option != -1);

  printf("(%s - %s) / %s, %lf, %lf, %s\n",
	 token1, token2, token3, cutup, cutdown, path);

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);
  
  up_image = bytemap_new(w, h);
  mid_image = bytemap_new(w, h);
  dn_image = bytemap_new(w, h);

  gray = bytemap_new(w, h);
  red = bytemap_new(w, h);
  green = bytemap_new(w, h);
  blue = bytemap_new(w, h);

  up_roi = bitmap_new(w, h);
  mid_roi = bitmap_new(w, h);
  dn_roi = bitmap_new(w, h);

  bin = bitmap_new(w, h);
  tmp = bitmap_new(w, h);
  roi = bitmap_new(w, h);

  se = bytemap_new(3, 3);
  bytemap_fill(se, 0, 0, 3, 3, 1);

  labelmap = dwordmap_new(w, h);

  // Up display image ////////////////////
  sprintf(filename, "%s_up.bmp", path);
  fn = strrchr(filename, '/') + 1;
  printf("Filename: %s\n", fn);
  printf("Display image\n");
  load_and_display_BMP(filename);
  read_bytemap_in_screen(up_image, NULL, NULL);
  bytemap_clear(gray);
  bytemap_copy(gray, UP_DX, UP_DY, up_image, 0, 0, w, h);
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  printf("Median filtering \n");
  mean_smooth(up_image, gray, 3);
  write_bytemap_to_screen(up_image, up_image, up_image);
  wait_keyhit();
  ////////////////////////////////////////

  // MID display image ///////////////////
  sprintf(filename, "%s_mid.bmp", path);
  fn = strrchr(filename, '/') + 1;
  printf("Filename: %s\n", fn);
  printf("Display image\n");
  load_and_display_BMP(filename);
  read_bytemap_in_screen(mid_image, NULL, NULL);
  bytemap_clear(gray);
  bytemap_copy(gray, MID_DX, MID_DY, mid_image, 0, 0, w, h);
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  printf("Median filtering \n");
  mean_smooth(mid_image, gray, 3);
  write_bytemap_to_screen(mid_image, mid_image, mid_image);
  wait_keyhit();
  /////////////////////////////////////////

  // DN display image /////////////////////
  sprintf(filename, "%s_dn.bmp", path);
  fn = strrchr(filename, '/') + 1;
  printf("filename: %s\n", fn);
  printf("Display image\n");
  load_and_display_BMP(filename);
  read_bytemap_in_screen(dn_image, NULL, NULL);
  bytemap_clear(gray);
  bytemap_copy(gray, DN_DX, DN_DY, dn_image, 0, 0, w, h);
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  printf("Median filtering \n");
  mean_smooth(dn_image, gray, 3);
  write_bytemap_to_screen(dn_image, dn_image, dn_image);
  wait_keyhit();
  ///////////////////////////////////////

  printf("Image thresholding\n");
  bytemap_threshold(bin, up_image, UP_FROM, UP_TO);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Big blobs greping\n");
  bitmap_clear(roi);
  label_info = label_info_new();
  labeling(labelmap, label_info, bin, NEIGHBOR_8);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 3000) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(roi, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);
  write_bitmap_to_screen(roi, roi, roi);
  wait_keyhit();

  printf("Fill holes\n");
  bitmap_complement(bin, roi);
  labeling_grep_big_blob(roi, bin);
  bitmap_complement(bin, roi);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Image Morphology\n");
  binary_closing(tmp, bin, se);
  binary_opening(up_roi, tmp, se);
  write_bitmap_to_screen(up_roi, up_roi, up_roi);
  wait_keyhit();

  ///////////////////////////////////////
  printf("Image thresholding\n");
  bytemap_threshold(bin, mid_image, MID_FROM, MID_TO);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Big blobs greping\n");
  bitmap_clear(roi);
  label_info = label_info_new();
  labeling(labelmap, label_info, bin, NEIGHBOR_8);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 3000) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(roi, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);
  write_bitmap_to_screen(roi, roi, roi);
  wait_keyhit();

  printf("Fill holes\n");
  bitmap_complement(bin, roi);
  labeling_grep_big_blob(roi, bin);
  bitmap_complement(bin, roi);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Image Morphology\n");
  binary_closing(tmp, bin, se);
  binary_opening(mid_roi, tmp, se);
  write_bitmap_to_screen(mid_roi, mid_roi, mid_roi);
  wait_keyhit();

  ///////////////////////////////////////////////////
  printf("Image thresholding\n");
  bytemap_threshold(bin, dn_image, DN_FROM, DN_TO);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Big blobs greping\n");
  bitmap_clear(roi);

  label_info = label_info_new();
  labeling(labelmap, label_info, bin, NEIGHBOR_8);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 3000) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(roi, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);
  write_bitmap_to_screen(roi, roi, roi);
  wait_keyhit();

  printf("Fill holes\n");
  bitmap_complement(bin, roi);
  labeling_grep_big_blob(roi, bin);
  bitmap_complement(bin, roi);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Image Morphology\n");
  binary_closing(tmp, bin, se);
  binary_opening(dn_roi, tmp, se);
  write_bitmap_to_screen(dn_roi, dn_roi, dn_roi);
  wait_keyhit();
  ///////////////////////////////////////////////

  bitmap_clear(roi);
  bitmap_or(roi, up_roi);
  bitmap_or(roi, mid_roi);
  bitmap_or(roi, dn_roi);
  write_bitmap_to_screen(up_roi, mid_roi, dn_roi);
  wait_keyhit();
  write_bitmap_to_screen(roi, roi, roi);
  wait_keyhit();

  // Up image normalization
  printf("Up-Image global normalization\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = bytemap_get_value(up_image, j, i);
	if (value < UP_MEAN) {
	  value = round(128.0 / (UP_MEAN - UP_MIN) * (value - UP_MIN));
	} else {
	  value = round(128.0 / (UP_MAX - UP_MEAN) * (value - UP_MEAN) + 128.0);
	}
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, up_image, j, i);
      } else {
	bytemap_put_value(0, up_image, j, i);
      }
    }
  }
  write_bytemap_to_screen(up_image, up_image, up_image);
  //strcpy(path, argv[1]);
  //strcat(path, "_up_normal.bmp");
  //save_bytemap_as_gray_BMP(up_image, path);
  wait_keyhit();
  //////////////////////////////////////////

  // Mid image normalization
  printf("MID-Image global normalization\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = bytemap_get_value(mid_image, j, i);
	if (value < MID_MEAN) {
	  value = round(128.0 / (MID_MEAN - MID_MIN) * (value - MID_MIN));
	} else {
	  value = round(128.0 / (MID_MAX - MID_MEAN) * (value - MID_MEAN) + 128.0);
	}
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, mid_image, j, i);
      } else {
	bytemap_put_value(0, mid_image, j, i);
      }
    }
  }
  write_bytemap_to_screen(mid_image, mid_image, mid_image);
  //strcpy(path, argv[1]);
  //strcat(path, "_mid_normal.bmp");
  //save_bytemap_as_gray_BMP(mid_image, path);
  wait_keyhit();
  ///////////////////////////////////////////////

  // Dn image normalization
  printf("Dn image global normalization\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = bytemap_get_value(dn_image, j, i);
	if (value < DN_MEAN) {
	  value = round(128.0 / (DN_MEAN - DN_MIN) * (value - DN_MIN));
	} else {
	  value = round(128.0 / (DN_MAX - DN_MEAN) * (value - DN_MEAN) + 128.0);
	}
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, dn_image, j, i);
      } else {
	bytemap_put_value(0, dn_image, j, i);
      }
    }
  }
  write_bytemap_to_screen(dn_image, dn_image, dn_image);
  //strcpy(path, argv[1]);
  //strcat(path, "_dn_normal.bmp");
  //save_bytemap_as_gray_BMP(dn_image, path);
  wait_keyhit();
  ////////////////////////////////////////////

  if (strcmp(token1, "up") == 0) op1 = up_image;
  else if (strcmp(token1, "mid") == 0) op1 = mid_image;
  else if (strcmp(token1, "dn") == 0) op1 = dn_image;
  else exit(1);

  if (strcmp(token2, "up") == 0) op2 = up_image;
  else if (strcmp(token2, "mid") == 0) op2 = mid_image;
  else if (strcmp(token2, "dn") == 0) op2 = dn_image;
  else exit(1);

  if (strcmp(token3, "up") == 0) op3 = up_image;
  else if (strcmp(token3, "mid") == 0) op3 = mid_image;
  else if (strcmp(token3, "dn") == 0) op3 = dn_image;
  else exit(1);

  real_t value1, value2, value3;

  // 10000.0 * (op1 - op2) / (op3 + 1.0)
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value1 = bytemap_get_value(op1, j, i);
      value2 = bytemap_get_value(op2, j, i);
      value3 = bytemap_get_value(op3, j, i);
      value = 10000.0 * (value1 - value2) / (value3 + 1.0);
      if (value >= cutup && value <= cutdown)
	bitmap_set_value(bin, j, i);
      else
	bitmap_reset_value(bin, j, i);
    }
  }

#if 1
  // labeling for post processing; removing outliner
  label_info = label_info_new();
  n = labeling(labelmap, label_info, bin, NEIGHBOR_8);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      label = dwordmap_get_value(labelmap, x, y);
      if (label != UNLABEL) {
	if (bitmap_isreset(roi, x-1, y-1) ||
	    bitmap_isreset(roi, x, y-1) ||
	    bitmap_isreset(roi, x+1, y-1) ||
	    bitmap_isreset(roi, x-1, y) ||
	    bitmap_isreset(roi, x+1, y) ||
	    bitmap_isreset(roi, x-1, y+1) ||
	    bitmap_isreset(roi, x, y+1) ||
	    bitmap_isreset(roi, x+1, y+1)) {
	  for (i = 0; i < h; i++) {
	    for (j = 0; j < w; j++) {
	      if (label == dwordmap_get_value(labelmap, j, i)) {
		bitmap_reset_value(bin, j, i);
	      }
	    }
	  }
	}
      } else {
	dwordmap_put_value(n, labelmap, x, y);
      }
    }
  }
  label_info_destroy(label_info);
#endif

  //dynamic_dwordmap2rgb(red, green, blue, labelmap);
  //write_bytemap_to_screen(red, green, blue);
  //wait_keyhit();

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = (bytemap_get_value(up_image, j, i) + bytemap_get_value(mid_image, j, i) + bytemap_get_value(dn_image, j, i)) / 3.0;
      if (bitmap_isset(bin, j, i))
	bytemap_put_value(255, red, j, i);
      else
	bytemap_put_value(value, red, j, i);
      bytemap_put_value(value, green, j, i);
      bytemap_put_value(value, blue, j, i);
    }
  }
  write_bytemap_to_screen(red, green, blue);
  sprintf(filename, "%s_%s_sub_%s_div_%s_at_%07.0lf_%07.0lf.bmp",
	  path, token1, token2, token3, cutup, cutdown);
  save_screen_as_BMP(filename);
  wait_keyhit();

  dynamic_bitmap2bytemap(gray, bin);
  sprintf(filename, "%s_%s_sub_%s_div_%s_at_%07.0lf_%07.0lf_result.bmp",
	  path, token1, token2, token3, cutup, cutdown);
  save_bytemap_as_gray_BMP(gray, filename);
  //save_bytemap_as_BMP(gray, path);

  bytemap_destroy(blue);
  bytemap_destroy(green);
  bytemap_destroy(red);

  dwordmap_destroy(labelmap);

  bytemap_destroy(se);
  bitmap_destroy(roi);
  bitmap_destroy(tmp);
  bitmap_destroy(bin);

  bitmap_destroy(dn_roi);
  bitmap_destroy(mid_roi);
  bitmap_destroy(up_roi);

  bytemap_destroy(gray);

  bytemap_destroy(dn_image);
  bytemap_destroy(mid_image);
  bytemap_destroy(up_image);

  return 0;
}

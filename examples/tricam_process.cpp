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

// This is for .../Data/try11/*.bmp

/////////////////////////////////////////
#define UP_DX 0
#define UP_DY 0

#define UP_FROM 15
#define UP_TO 255

#define UP_MIN 9
#define UP_MEAN 63.62
#define UP_MAX 215

/////////////////////////////////////////
#define MID_DX -37
#define MID_DY 0

#define MID_FROM 20
#define MID_TO 255

#define MID_MIN 10
#define MID_MEAN 48.20
#define MID_MAX 198


/////////////////////////////////////////
#define DN_DX -83
#define DN_DY 3

#define DN_FROM 13
#define DN_TO 255

#define DN_MIN 3
#define DN_MEAN 18.05
#define DN_MAX 49

/////////////////////////////////////////
#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

#undef wait_keyhit()
#define wait_keyhit()

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

int tricam_process(int argc, char *argv[])
{
  int i, j, w, h, bpp;
  bytemap_t *up_image, *mid_image, *dn_image;
  bytemap_t *gray, *se;
  bitmap_t *up_roi, *mid_roi, *dn_roi;
  bitmap_t *bin, *roi, *tmp;
  char *fn, *ptr;
  real_t value;
  FILE *fd;
  label_info_t *label_info;
  dwordmap_t *labelmap;
  point_t *centroid;
  int label, area;
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

  //  printf("(%s - %s) / %s, %lf, %lf, %s\n", token1, token2, token3, cutup, cutdown, path);

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);

  up_image = bytemap_new(w, h);
  mid_image = bytemap_new(w, h);
  dn_image = bytemap_new(w, h);

  gray = bytemap_new(w, h);

  up_roi = bitmap_new(w, h);
  mid_roi = bitmap_new(w, h);
  dn_roi = bitmap_new(w, h);

  bin = bitmap_new(w, h);
  tmp = bitmap_new(w, h);
  roi = bitmap_new(w, h);

  se = bytemap_new(3, 3);
  bytemap_fill(se, 0, 0, 3, 3, 1);

  labelmap = dwordmap_new(w, h);

  path = (char *)malloc(256 * sizeof(char));

  // Up display image ////////////////////
  strcpy(path, argv[1]);
  strcat(path, "_up.bmp");
  fn = strrchr(path, '/') + 1;
  printf("Filename: %s\n", fn);
  printf("Display image\n");
  load_and_display_BMP(path);
  read_bytemap_in_screen(up_image, NULL, NULL);
  bytemap_clear(gray);
  bytemap_copy(gray, UP_DX, UP_DY, up_image, 0, 0, w, h);
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  printf("Mean filtering \n");
  mean_smooth(up_image, gray, 3);
  write_bytemap_to_screen(up_image, up_image, up_image);
  wait_keyhit();
  ////////////////////////////////////////

  // MID display image ///////////////////
  strcpy(path, argv[1]);
  strcat(path, "_mid.bmp");
  fn = strrchr(path, '/') + 1;
  printf("Filename: %s\n", fn);
  printf("Display image\n");
  load_and_display_BMP(path);
  read_bytemap_in_screen(mid_image, NULL, NULL);
  bytemap_clear(gray);
  bytemap_copy(gray, MID_DX, MID_DY, mid_image, 0, 0, w, h);
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  printf("Mean filtering \n");
  mean_smooth(mid_image, gray, 3);
  write_bytemap_to_screen(mid_image, mid_image, mid_image);
  wait_keyhit();
  /////////////////////////////////////////

  // DN display image /////////////////////
  strcpy(path, argv[1]);
  strcat(path, "_dn.bmp");
  fn = strrchr(path, '/') + 1;
  printf("filename: %s\n", fn);
  printf("Display image\n");
  load_and_display_BMP(path);
  read_bytemap_in_screen(dn_image, NULL, NULL);
  bytemap_clear(gray);
  bytemap_copy(gray, DN_DX, DN_DY, dn_image, 0, 0, w, h);
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  printf("Mean filtering \n");
  mean_smooth(dn_image, gray, 3);
  write_bytemap_to_screen(dn_image, dn_image, dn_image);
  wait_keyhit();
  ///////////////////////////////////////

  printf("Up Image thresholding\n");
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
  printf("Mid Image thresholding\n");
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
  printf("Dn Image thresholding\n");
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

  /*
  //write_bitmap_to_screen(up_roi, mid_roi, dn_roi);
  //read_bytemap_in_screen(up_image, mid_image, dn_image);

  strcpy(path, argv[1]);
  strcat(path, "_up_roi.bmp");
  save_bytemap_as_BMP(up_image, path);

  strcpy(path, argv[1]);
  strcat(path, "_mid_roi.bmp");
  save_bytemap_as_BMP(mid_image, path);

  strcpy(path, argv[1]);
  strcat(path, "_dn_roi.bmp");
  save_bytemap_as_BMP(dn_image, path);

  read_bytemap_in_screen(gray, NULL, NULL);
  strcpy(path, argv[1]);
  strcat(path, "_roi.bmp");
  save_bytemap_as_BMP(gray, path);
  */

#if 0
  // up image statistic /////////////////////////
  bytemap_get_min_max_on_roi(&vmin, &vmax, up_image, roi);
  bytemap_get_mean_on_roi(&vmean, up_image, roi);
  printf("Up image statistics are min: %d, mean: %lf, max: %d\n", vmin, vmean, vmax);

  fd = fopen("up_stat.txt", "a+");
  fprintf(fd, "%d, %lf, %d\n", vmin, vmean, vmax);
  fclose(fd);
  ////////////////////////////////////////////////

  // mid image statistic /////////////////////////
  bytemap_get_min_max_on_roi(&vmin, &vmax, mid_image, roi);
  bytemap_get_mean_on_roi(&vmean, mid_image, roi);
  printf("Mid image statistics are min: %d, mean: %lf, max: %d\n", vmin, vmean, vmax);

  fd = fopen("mid_stat.txt", "a+");
  fprintf(fd, "%d, %lf, %d\n", vmin, vmean, vmax);
  fclose(fd);
  /////////////////////////////////////////////////

  // dn image statistic ///////////////////////////
  bytemap_get_min_max_on_roi(&vmin, &vmax, dn_image, roi);
  bytemap_get_mean_on_roi(&vmean, dn_image, roi);
  printf("Dn image statistics are min: %d, mean: %lf, max: %d\n", vmin, vmean, vmax);

  fd = fopen("dn_stat.txt", "a+");
  fprintf(fd, "%d, %lf, %d\n", vmin, vmean, vmax);
  fclose(fd);
  /////////////////////////////////////////////////

  return 0;
#endif

#if 1
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
  strcpy(path, argv[1]);
  strcat(path, "_up_normal.bmp");
  save_bytemap_as_gray_BMP(up_image, path);
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
  strcpy(path, argv[1]);
  strcat(path, "_mid_normal.bmp");
  save_bytemap_as_gray_BMP(mid_image, path);
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
  strcpy(path, argv[1]);
  strcat(path, "_dn_normal.bmp");
  save_bytemap_as_gray_BMP(dn_image, path);
  wait_keyhit();
  ////////////////////////////////////////////

  return 0;
#endif

#if 0
  printf("Image subtraction between DN and MID for bruise-detection\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = ((real_t)bytemap_get_value(dn_image, j, i) - (real_t)bytemap_get_value(mid_image, j, i) + 255.0) / 2.0;
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, gray, j, i);
      } else {
	bytemap_put_value(0, gray, j, i);
      }
    }
  }
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  bytemap_threshold(bin, gray, 142, 255);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Image subtraction between UP and MID for spot-detection\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = ((real_t)bytemap_get_value(up_image, j, i) - (real_t)bytemap_get_value(mid_imageb, j, i) + 255.0) / 2.0;
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, gray, j, i);
      } else {
	bytemap_put_value(0, gray, j, i);
      }
    }
  }
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();
  
  bytemap_threshold(bin, gray, 1, 110);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();
#endif

  dwordmap_destroy(labelmap);

  bytemap_destroy(se);
  return 0;

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
